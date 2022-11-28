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

#define SAMPLE_NAME "ak_drv_saradc_sample"

#define FILE_PATH_MAX 64

#define OPTION_STRING   "c:h"

#define AK_SAR_ADC_PATCH "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw"

static struct option long_options[] =
{
    {"channel", required_argument, NULL, 'c'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

static char option_hint[][MAX_HINT_LEN] =
{
    "指定的通道数 下标从0开始",
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
    ak_print_normal("  %s -c 0\n", app);
    ak_print_normal("  %s --channel 0\n", app);
}

int ak_sardac_read_channel_data(int channel, char *result, int count)
{
    int fd = -1;
    int ret = 0;
    char path[FILE_PATH_MAX] = {0};

    memset(path, 0, FILE_PATH_MAX);
    sprintf(path, AK_SAR_ADC_PATCH, channel);
    //ak_print_normal("trying to open %s\n", path);

    if (!access(path, F_OK)) {
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            ak_print_error("open %s: %s\n", path, strerror(errno));
            return -EPERM;
        }
        ret = read(fd, result, count);
        if (ret < 0) {
            close(fd);
            ak_print_error("read %s: %s\n", path, strerror(errno));
            return ret;
        }
        close(fd);
    } else {
        return -ENOENT;
    }

    //ak_print_normal("close %s successfully\n", path);
    return 0;
}

int main (int argc, char **argv)
{
    int opt_val = 0, option_idx = 0;
    char value[FILE_PATH_MAX] = {0};
    int channel = 0;

    if (argc == 1) {
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

    while ((opt_val = getopt_long(argc, argv, OPTION_STRING,
                long_options, &option_idx)) != -1) {
        switch (opt_val) {
            case 'c':
                channel = atoi(optarg);
                break;
            case 'h':
            default:
                usage(SAMPLE_NAME);
                return -EINVAL;
        }
    }

    memset(value, 0, FILE_PATH_MAX);
    opt_val = ak_sardac_read_channel_data(channel, value, FILE_PATH_MAX);

    if (opt_val) {
        if (opt_val == (-ENOENT))
            ak_print_notice("CHANNEL_%d: no such channel\n", channel);
        else if (opt_val == (-EPERM))
            ak_print_notice("CHANNEL_%d: no permission\n", channel);
        else
            ak_print_notice("CHANNEL_%d fail to get value\n", channel);
        usage(SAMPLE_NAME);
    } else {
        ak_print_notice("\nCHANNEL_%d: value %s\n\n", channel, value);
    }

    return 0;
}
