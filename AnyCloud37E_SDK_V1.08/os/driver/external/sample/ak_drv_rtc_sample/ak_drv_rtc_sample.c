#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include "ak_drv_common.h"
#include "ak_drv_rtc.h"

#define SAMPLE_NAME      "ak_drv_rtc_sample"

static struct option long_options[] = {
    {"show",   no_argument,       NULL, 's'},
    {"write",  no_argument,       NULL, 'w'},
    {"read",   no_argument,       NULL, 'r'},
	{"wakeup", required_argument, NULL, 'u'},
    {"help",   no_argument,       NULL, 'h'},
    {NULL,     no_argument,       NULL, 0},
};

static char option_describe[][128] = {
    "显示RTC时间",
    "将系统时间同步到RTC",
    "将RTC时间同步到系统",
    "使用RTC定时唤醒系统",
	"显示帮助信息",
};

static void make_short_option(char buffer[])
{
	int opt_num = sizeof(long_options)/sizeof(struct option) - 1;
	int i, j, p = 0;
	for (i = 0; i < opt_num; i++) {
		buffer[p++] = (char)long_options[i].val;
		j = long_options[i].has_arg;
		while(j--) buffer[p++] = ':';
	}
	buffer[p] = '\0';
}

static void usage(const char *app)
{
    int opt_num = sizeof(long_options)/sizeof(struct option) - 1;
    int i;

    ak_print_normal("version:\n");
    ak_print_normal("  %s V1.0.0\n", app);
    ak_print_normal("\n");
    ak_print_normal("usage:\n");
    ak_print_normal("  %s [options]\n", app);
    ak_print_normal("\n");
    ak_print_normal("options:\n");
    for (i = 0; i < opt_num; i++) {
        ak_print_normal("  -%c, --%-8s  %s\n", long_options[i].val, long_options[i].name, option_describe[i]);
    }
    ak_print_normal("\n");
    ak_print_normal("example:\n");
    ak_print_normal("  %s -s\n", app);
	ak_print_normal("  %s -w\n", app);
	ak_print_normal("  %s -r\n", app);
	ak_print_normal("  %s -u 5\n", app);
	ak_print_normal("  %s -u \"00:01:00\"\n", app);
	ak_print_normal("  %s -u \"2021-1-1 00:01:00\"\n", app);
	ak_print_normal("\n");
}

int main (int argc, char **argv)
{
    int opt_val = 0, option_idx = 0;
	char short_option[32];
	int retval = 0;
	int s;

    if (argc == 1) {
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

	make_short_option(short_option);

    while ((opt_val = getopt_long(argc, argv, short_option,
                long_options, &option_idx)) != -1) {
        switch (opt_val) {
            case 's':
                retval = ak_drv_rtc_read();
				if(retval) ak_print_error("读取RTC出错! \n");
                break;
            case 'w':
                retval = ak_drv_sync_to_rtc();
				if(retval) ak_print_error("将系统时间同步到RTC出错, 请先尝试使用date命令设置系统时间!\n");
                break;
            case 'r':
                retval = ak_drv_sync_from_rtc();
				if(retval) ak_print_error("将RTC时间同步到系统出错, 请先尝试将系统时间同步到RTC!\n");
                break;
			case 'u':
				if(strrchr(optarg, ':')) {
					retval = ak_drv_alarm_wakeup_t(optarg);
				} else if((s = atoi(optarg)) != -1) {
					retval = ak_drv_alarm_wakeup_s(s);
				} else {
					ak_print_error("配置RTC定时唤醒系统时出错!\n");
				}
				break;
			case 'h':
				usage(SAMPLE_NAME);
				break;
            default:
                usage(SAMPLE_NAME);
                return -EINVAL;
        }
    }

	if(!retval) ak_print_normal("complete!\n\n");

    return retval;
}
