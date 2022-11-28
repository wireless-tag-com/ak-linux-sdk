#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <linux/input.h>

#include "ak_drv_common.h"

#define TS_POINT_MAX_NUM	10

struct ak_timeval {
	unsigned long sec;     /* seconds */
	unsigned long usec;    /* microseconds */
};

struct ak_ts_info{
	int x;
	int y;
};

struct ak_ts_event{
	int map;
	struct ak_ts_info info[TS_POINT_MAX_NUM];
};

static int ts_handle = -1;
static unsigned int ts_index = 0;
static struct ak_ts_event ts_old = {0};

#define LEN_HINT         512
#define LEN_OPTION_SHORT 512

static struct ak_ts_event ts;

/**
 * @brief find the number for the touch screen
 * 
 * @return int -1: not find
 * @return int other: number
 */
int ak_find_ts_number(void)
{
	int fd = -1;
	int i;
	char path[128] = {0};
	char name[32] = {0};
	
	for (i=0; i<3; i++)
	{
		/*
		 * 通过查找设备名字中是否带有'ts'字符来确定是否是触摸屏设备
		 */
		sprintf(path, "/sys/class/input/input%d/name", i);
		fd = open(path, O_RDONLY);

		if (fd < 0)
		{
			printf( "Open input%d name fail\n", i);
			return -1;
		}

		read(fd, name, 32);

		if (strstr(name, "ts") != NULL)
		{
			close(fd);
			printf("# Find device: %s", name);
			return i;
		}
		close(fd);
	}

	return -1;
}

/**
 * ak_drv_ts_open: open touch screen driver
 * return: 0 - success; otherwise error code;
 */
int ak_drv_ts_open(void)
{
	int num;
	char path[128] = {0};

	if (ts_handle < 0) 
	{
		num = ak_find_ts_number();
		if(num < 0)
		{
			ak_print_error("Not find touch device\n");
			return -1;
		}

		sprintf(path, "/dev/input/event%d", num);
		printf("# Device path: %s\n", path);

		ts_handle = open(path, O_RDONLY);
		
		if (ts_handle < 0) 
		{
			ak_print_error("Open ts dev fail\n");
			return -1;
		}
	}

	ts_old.map = 0;
	
	for (int i=0; i<TS_POINT_MAX_NUM; i++)
	{
		ts_old.info[i].x = -1;
		ts_old.info[i].y = -1;
	}

	return AK_SUCCESS;
}

/**
 * ak_drv_ts_get_event: get touch screen event .
 * @ts[OUT]:  ts event
 * @ms[IN]:  time out .
 * return: 0 - success; otherwise error code;
 */
int ak_drv_ts_get_event(struct ak_ts_event *ts, long ms)
{
    int fd = -1;
    int ret = 0, rdbyte;
	fd_set readfds;
	struct input_event ts_event[64], *event;
	struct timeval timeout;
	int i = 0;

	/* arguments check */
	if (NULL == ts) 
	{
		printf("para null\n");
		return -1;
	}

	if (ts_handle < 0) 
	{
		printf("ts is not init\n");
		return -1;
	}

	/*
	 * check wait time, deal with it differently.
	 * 0, no wait; > 0, wait
	 */
	fd = ts_handle;

	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);

	ts->map = 0;
	
	for (i=0; i<TS_POINT_MAX_NUM; i++)
	{
		ts->info[i].x = -1;
		ts->info[i].y = -1;
	}

	/* block mode */
	if (ms == 0) 
	{
		ret = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
		if (ret < 1) {
			printf("select error\n");
		}
	} 
	else 
	{ /* unblock mode */
        timeout.tv_sec = ms / 1000;
		timeout.tv_usec = (ms % 1000) * 1000;
        ret = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
	}

	/* To check whether the fd's status has changed */
	if (FD_ISSET(fd, &readfds)) 
	{
		/** read the event to the buf **/
		rdbyte = read(fd, ts_event, 64 * sizeof(struct input_event));
		/* parse the event */
		
		for (i=0; (i<rdbyte/sizeof(struct input_event)); i++) 
		{
			event = (struct input_event *)&ts_event[i];
			ak_print_info("i:%d type = %d, code = %d, value = %d!\n",
					i, event->type, event->code, event->value);
			/* filter others event */
			if (EV_ABS != event->type) 
				continue;
			
			/* get ts event */
			if ((ABS_MT_TRACKING_ID == event->code) || (ABS_MT_SLOT == event->code))
			{
				if ((event->value >= 0) && (event->value < TS_POINT_MAX_NUM)) 
				{
					ts_index = event->value;
				}
			}
			else if (ABS_MT_POSITION_X == event->code) 
			{
				ts->info[ts_index].x = event->value;
			}
			else if (ABS_MT_POSITION_Y == event->code) 
			{
				ts->info[ts_index].y = event->value;
			}
		}

		/*
		* input架构会过滤同样的消息，导致上层收到的消息可能不全。
		* 平台处理方法是保存了上一次的坐标信息，
		* 当本次消息不全时把上次保留的值赋过来。
		*/
		for (i=0; i<TS_POINT_MAX_NUM; i++)
		{
			if ((ts->info[i].x >= 0) && (ts->info[i].y >= 0))
			{
				ts->map |= 1 << i;
			}
			else if ((ts->info[i].x >= 0) && (ts->info[i].y < 0))
			{
				if (ts_old.info[i].y >= 0)
				{
					ts->info[i].y = ts_old.info[i].y;
					ts->map |= 1 << i;
				}
			}
			else if ((ts->info[i].y >= 0) && (ts->info[i].x < 0))
			{
				if (ts_old.info[i].x >= 0)
				{
					ts->info[i].x = ts_old.info[i].x;
					ts->map |= 1 << i;
				}
			}
		}

		memcpy(&ts_old, ts, sizeof(struct ak_ts_event));

		ret = AK_SUCCESS;
	} 
	else 
	{
		ak_print_info("fd status no change.\n");
		ret = -1;
	}
	return ret;
}

/**
 * ak_drv_ts_close: close touch screen .
 * return: 0 - success; otherwise error code;
 */
int ak_drv_ts_close(void)
{
	/* argument check */
	if (ts_handle < 0) 
	{
		printf("ts is not init\n");
		return -1;
	}
	
	/* close device */
	close(ts_handle);
	ts_handle = -1;

	return AK_SUCCESS;
}

static inline int ak_ts_drawline_test(void)
{
		int ret = 0;
		unsigned int timeout = 0;
		/* 
	*open ts driver
	*/	
    ret = ak_drv_ts_open();
    if(ret)
	{
		printf( "ak_drv_ts_open fail.\n");
		return -1;
  	}
    
	do 
	{
		printf( "ak_drv_ts_get_event timeout 1000 ms.\n"); 
		ret = ak_drv_ts_get_event(&ts, 1000);
		
		/* 
		*get ts, output 
		*/
		if(ret == 0) 
		{
	   		for (int i=0; i<TS_POINT_MAX_NUM; i++) 
			{
				/* 
				*check map, print ts info.
				*/
				if (ts.map & (1 << i))
           			ak_print_normal( "id %d, get ts:[%d , %d].\n", i, ts.info[i].x, ts.info[i].y); 
	   		}
	   	}

		/* 
		*run time is over and exit 
		*/
		if (timeout++ > 50000000)
			break;
	} while(1);
	
	/* 
	*close ts handle  
	*/	
    ak_drv_ts_close();
    
    return 0;
}
	
static inline void usage(const char *app)
{
	ak_print_normal("Usage: %s [options]\n\n"
		"options:\n"
		" 1   touch test\n"
		"-h   help\n",app);	
}

int main (int argc, char **argv)
{
	if (argc < 2){
		usage(argv[0]);  	
		return -1;
	}	
			
	ak_print_normal( "ts test start.\n");

	switch(*argv[1]){
		case '1': ak_ts_drawline_test(); 
				  break;

		case 'h':
		default	:
				  usage(argv[0]);
				  break;
	}	

  ak_print_normal( "ts test end.\n");	
    
    return 0;
}
