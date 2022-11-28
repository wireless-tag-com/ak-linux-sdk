#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>

#include "ak_drv_common.h"
#include "ak_drv_wdt.h"

#define MAX_HINT_LEN 512
#define DEFULAT_CFG_VALUE (-1)

#define SAMPLE_NAME "ak_drv_wdt_sample"

#define DEFAULT_TIMEOUT 8

static int feed_time = DEFULAT_CFG_VALUE;//second
static int feed_circle = DEFULAT_CFG_VALUE;//second
static int timeout = DEFULAT_CFG_VALUE;//second

#define OPTION_STRING   "t:f:c:h"

static struct option long_options[] =
{
    {"timeout", required_argument, NULL, 't'},
    {"feedtime", required_argument, NULL, 'f'},
    {"feedcircle", required_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

static char option_hint[][MAX_HINT_LEN] =
{
    "看门狗的超时时间设置 单位为秒",
    "看门狗的喂狗时间设置 单位为秒",
    "看门狗的喂狗次数",
    "显示帮助信息",
};

static void usage(const char *app)
{
    int opt_num = sizeof(long_options)/sizeof(struct option) - 1;
    int i;

    ak_print_normal("version:\n");
    ak_print_normal("  %s V1.0.0\n", app);
    ak_print_normal("\n");
    ak_print_normal("usage:\n");
    ak_print_normal("  %s [options] <>\n", app);
    ak_print_normal("\n");
    ak_print_normal("options:\n");
    for (i = 0; i < opt_num; i++) {
        ak_print_normal("  -%c, --%s  %s\n", long_options[i].val, long_options[i].name, option_hint[i]);
    }
    ak_print_normal("\n");
    ak_print_normal("example:\n");
    ak_print_normal("  %s -t 3 -f 2 -c 5\n", app);
    ak_print_normal("  %s --timeout 3 --feedtime 2 --feedcircle 5\n", app);
}

int main (int argc, char **argv)
{
    int opt_val = 0, option_idx = 0;

    if (argc == 1) {
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

    while ((opt_val = getopt_long(argc, argv, OPTION_STRING,
                long_options, &option_idx)) != -1) {
        switch (opt_val) {
            case 't':
                timeout = atoi(optarg);
                break;
            case 'f':
                feed_time = atoi(optarg);
                break;
            case 'c':
                feed_circle = atoi(optarg);
                break;
            case 'h':
            default:
                usage(SAMPLE_NAME);
                return -EINVAL;
        }
    }
				ak_print_normal("timeout %d.\n",timeout);

    if ((timeout < 1) || (timeout > 357)) {
        ak_print_normal("timeout error,effective range:1~357.\n");
        return -EINVAL;
    }

    if (feed_time <= 0) {
        ak_print_normal("feed_time error,use default value.\n");
        if (timeout > 1)
            feed_time = timeout - 1;
        else
            feed_time = 1;
    }

    if (feed_circle <= 0) {
        ak_print_normal("feed_circle error,use default value.\n");
        feed_circle = 5;
    }

    /*
    *watchdog dev open
    */
    if (ak_drv_wdt_open(timeout)){
        return 0;
    }

    /*
    *feed watchdog
    */
    for(int i = 0; i < feed_circle; i++){
        ak_print_normal("feed dog....%d\n", i);
        ak_drv_wdt_feed();
        sleep(feed_time);
    }

    /*
    *close watch dog
    */
    ak_drv_wdt_close();

    ak_print_normal("wdt sample end.\n");

    return 0;
}
