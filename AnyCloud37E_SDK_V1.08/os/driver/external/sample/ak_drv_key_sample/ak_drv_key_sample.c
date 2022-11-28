/*
* @FILENAME: ak_drv_key_sample.c
* @BRIEF
* Copyright (C) 2019 Anyka(Guangzhou) Microelcctronics Technology CO., LTD.
* @DATE 2019-12-22
* @VERSION 1.0
*/

/*******************************************************************************
	The following is an example to use module drivers APIs
	
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <getopt.h>
#include <linux/input.h>
#include "ak_drv_common.h"

#define KEY_INPUT_DEV_PATH "/dev/input/event0"
#define SAMPLE_NAME "ak_drv_key_sample"

/**
 * @brief key_event, 按键事件传递结构体
 * @member code:
 * 		按键的键值, 用于区分不同的按键, 可在DTS中adkeys/gpiokeys节点的linux,code属性值指定
 * @member stat:
 * 		按键当前的状态, 记录按键按压或者释放时的状态变化
 * 		(1) 当被测试驱动为ak_gpio_keys时, 按下时固定为1, 松开时固定为0
 * 		(2) 当被测试驱动为ak_adc_key时, 按下时的值根据DTS上adkeys节点中的pressed-input属性的值来确定
 */
struct key_event{
   int code;
   int stat;
};

static int key_handle = -1;

int ak_drv_key_open(void)
{	
	int fd = -1;
	int i = 0;
	int dev_id = -1;
	char path[128] = {0};
	char name[32] = {0};
	
	if (key_handle < 0) 
	{
		/*
		*  由于触屏和按键都是input设备，根据加载顺序，
		*  先加载的是/dev/input/event0，后加载的是/dev/input/event1，
		*  这里通过检验name，自动识别哪个是按键设备。
		*/
		for (i=0; i<3; i++)
		{
			sprintf(path, "/sys/class/input/input%d/name", i);
			fd = open(path, O_RDONLY);

			if (fd < 0)
			{
				printf( "Open input%d name fail\n", i);
				return -1;
			}

			read(fd, name, 32);

			if ((0 == strncmp(name, "gpiokeys", 8)) || (0 == strncmp(name, "adkeys", 6)))
			{
				dev_id = i;
				close(fd);
				break;
			}
			close(fd);
			
		}

		if (-1 == dev_id)
		{
			printf( "find key dev fail\n");
			return -1;
		}

		sprintf(path, "/dev/input/event%d", dev_id);
		key_handle = open(path, O_RDONLY);
		
		if (key_handle < 0) 
		{
			printf( "Open key dev fail\n");
			return -1;
		}
	}
	else
		printf( "key dev already opened!\n");

	return AK_SUCCESS;
}

/**
 * ak_drv_key_get_event: get key event .
 * @key[OUT]:  key event
 * @ms[IN]:  time out , <0  block; = 0  unblock; > 0 wait time.
 * return: 0 - success; otherwise error code;
 */
int ak_drv_key_get_event(struct key_event *key, long ms)
{
    int fd = -1;
    int ret = 0, rdbyte;
	fd_set readfds;
	struct input_event keyevent[64], *event;
	struct timeval timeout;

	/* arguments check */
	if (NULL == key) {
		printf( "para err\n");
		return -1;
	}

	if (key_handle < 0) {
		printf( "not init\n");
		return -1;
	}

	/*
	 * check wait time, deal with it differently.
	 * 0, no wait; > 0, wait
	 */
	fd = key_handle;
	FD_ZERO(&readfds);
	FD_SET(fd, &readfds);

	/* block mode */
	if (ms < 0) {
		ret = select(FD_SETSIZE, &readfds, NULL, NULL, NULL);
		if (ret < 1) {
			printf( "select error\n");
		}
	} else { /* unblock mode */
        timeout.tv_sec = ms / 1000;
		timeout.tv_usec = (ms % 1000) * 1000;
        ret = select(FD_SETSIZE, &readfds, NULL, NULL, &timeout);
	}

	/* To check whether the fd's status has changed */
	if (FD_ISSET(fd, &readfds)) {
		/** read the event to the buf **/
		rdbyte = read(fd, keyevent, 64 * sizeof(struct input_event));
		/* parse the event */
		int i;
		for (i = 0; (i < rdbyte / sizeof(struct input_event)); i++) {
			event = (struct input_event *)&keyevent[i];
			ak_print_info( "i:%d type = %d, code = %d, value = %d!\n",
					i, event->type, event->code, event->value);
			/* filter others event */
			if (EV_KEY != event->type) {
				continue;
			}
			/* get key event */
			key->code = event->code;
			key->stat = event->value;
		}
		ret = AK_SUCCESS;
	} else
		ret = -1;

	return ret;
}

/**
 * ak_drv_key_close: close key .
 * @handle[IN]:  key handle
 * return: 0 - success; otherwise error;
 */
int ak_drv_key_close(void)
{
	/* argument check */
	if (key_handle < 0) {
		printf( "not init\n");
		return -1;
	}
	/* close device */
	close(key_handle);
	key_handle = -1;

	return AK_SUCCESS;
}

static inline int ak_key_press_test(void)
{
		int ret = -1;
	  struct key_event key;
	  unsigned int timeout = 0;
	/*
	  *open key driver 
	  */	
    ret = ak_drv_key_open();
    if(ret){
  	  printf( "ak_drv_key_open fail.\n");
	  return -1;
  	}
		
	memset(&key,0,sizeof(key));

	/*
	* demo will run wait_time
	*/
	do {
       ak_print_normal("ak_drv_key_get_event timeout 1000 ms.\n"); 
	   memset(&key,0,sizeof(key));
       ret = ak_drv_key_get_event(&key,1000);
	   /* 
	   	* get key, output 
	   	*/
	   if(ret == 0)
           ak_print_normal("get key:%d stat:%d.\n",key.code, key.stat); 
		
		/* 
		* run time is over and exit 
		*/
		if (timeout++ > 50000000)
			break;
	} while (1);
	/* 
	* close key handle  
	*/	
    ak_drv_key_close();
    
    return 0;
	}


static inline void usage(const char *app)
{
	ak_print_normal("Usage: %s [options]\n\n"
		"options:\n"
		"   1 key press test\n"
		"  -h help\n",app);	
}

int main (int argc, char **argv)
{
    if (argc < 2) {
        usage(argv[0]);
        return -1;
    }

    ak_print_normal("key test start.\n");
    switch (*argv[1]) {
        case '1': ak_key_press_test();
            break;
        case 'h':
        default:
            usage(SAMPLE_NAME);
            break;
    }

    ak_print_normal( "key test end.\n");
    
    return 0;
}
