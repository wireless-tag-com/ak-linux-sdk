
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include "ak_drv_common.h"

#define MAX_HINT_LEN 512
#define DEFULAT_CFG_VALUE (-1)

#define SAMPLE_NAME "ak_drv_led_sample"

int ak_drv_led_set(char *type, int status);

static char *type = NULL;
static int dev_status = DEFULAT_CFG_VALUE;

#define OPTION_STRING   "t:s:h"

static struct option long_options[] =
{
    {"type", required_argument, NULL, 't'},
    {"status", required_argument, NULL, 's'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

static char option_hint[][MAX_HINT_LEN] =
{
    "LED灯的类型 可选择的类型有 state_led/状态灯 irled/红外灯",
    "LED的状态 0表示关闭LED灯 1表示打开LED灯",
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
    ak_print_normal("  %s -t state_led -s 1\n", app);
    ak_print_normal("  %s --type state_led --status 1\n", app);
}

int main (int argc, char **argv)
{
    int opt_val = 0, option_idx = 0;

    while ((opt_val = getopt_long(argc, argv, OPTION_STRING,
                long_options, &option_idx)) != -1) {
        switch (opt_val) {
            case 't':
                type = optarg;
                break;
            case 's':
                dev_status = atoi(optarg);
                break;
            case 'h':
            default:
                usage(SAMPLE_NAME);
                return -EINVAL;
        }
    }

    if ((type == NULL)
        || (dev_status == DEFULAT_CFG_VALUE)) {
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

    ak_print_normal("LED:%s @%d\n", type, dev_status);
    ak_drv_led_set(type, dev_status);

    return 0;
}
