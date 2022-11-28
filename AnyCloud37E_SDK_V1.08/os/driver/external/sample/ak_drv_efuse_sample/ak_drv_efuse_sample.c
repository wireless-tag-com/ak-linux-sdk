
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

#define READ_GLOBAL_ID (1)

#define SAMPLE_NAME "ak_drv_efuse_sample"

int ak_drv_led_set(char *type, int status);

#define OPTION_STRING   "c:h"

static struct option long_options[] =
{
    {"cmd", required_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

static char option_hint[][MAX_HINT_LEN] =
{
    "命令 可选择的类型有 1：显示global ID",
    "显示帮助信息",
};

int ak_drv_read_efuse(void);
int ak_drv_read_globalID(void);

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
    ak_print_normal("  %s -c 1\n", app);
    ak_print_normal("  %s --cmd 1\n", app);
}

int main (int argc, char **argv)
{
    int cmd = 0;
    int opt_val = 0, option_idx = 0;

    if (argc <= 1) {
        usage(SAMPLE_NAME);
        return 0;
    }

    while ((opt_val = getopt_long(argc, argv, OPTION_STRING,
                long_options, &option_idx)) != -1) {
        switch (opt_val) {
            case 'c':
                cmd = atoi(optarg);
                break;
            case 'h':
            default:
                usage(SAMPLE_NAME);
                return -EINVAL;
        }
    }

    ak_print_normal("CMD:@%d\n", cmd);
    switch (cmd) {
        case READ_GLOBAL_ID:
            ak_drv_read_globalID();
            break;
        default:
            ak_print_normal("unknow CMD@%d\n", cmd);
            break;
    }

    return 0;
}
