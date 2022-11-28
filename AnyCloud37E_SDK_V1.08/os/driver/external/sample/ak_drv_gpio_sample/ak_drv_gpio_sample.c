#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include "ak_drv_common.h"

#define FILE_PATH_MAX 64

#define MAX_HINT_LEN 512
#define DEFULAT_CFG_VALUE (-1)

#define SAMPLE_NAME "ak_drv_gpio_sample"

#define OPTION_STRING   "rwn:d::v::c::p::e::i::s::h"

static struct option long_options[] =
{
    {"read", no_argument, NULL, 'r'},
    {"write", no_argument, NULL, 'w'},
    {"gpio_no", required_argument, NULL, 'n'},
    {"direction", optional_argument, NULL, 'd'},
    {"value", optional_argument, NULL, 'v'},
    {"drive", optional_argument, NULL, 'c'},
    {"pull_polarity", optional_argument, NULL, 'p'},
    {"pull_enable", optional_argument, NULL, 'e'},
    {"input_enable", optional_argument, NULL, 'i'},
    {"slew_rate", optional_argument, NULL, 's'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

static char option_hint[][MAX_HINT_LEN] =
{
    "读操作",
    "写操作",
    "GPIO pin脚的标识号 从0开始",
    "GPIO pin脚的方向 out表示输入 in表示输出",
    "GPIO pin脚的输出值 0标识高电平 1表示低电平",
    "GPIO pin脚的驱动能力 可选0-3共4档",
    "GPIO pin脚的上下拉配置 pullup标识上拉 pulldown表示下拉",
    "GPIO pin脚的上下拉使能 1表示",
    "GPIO pin脚的输入使能 1表示使能输入 0表示关闭输入",
    "GPIO pin脚的slew rate能力 fast表示使能 slow表示关闭",
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
    ak_print_normal("  %s -r -n 79 -d -v -c -p -e -i -s\n", app);
    ak_print_normal("  %s --read --gpio_no 79 --direction --value --drive --pull_polarity --pull_enable --input_enable --slew_rate\n", app);
    ak_print_normal("  %s -w -n 79 -dout -v1 -c3 -ppullup -e1 -i0 -sslow\n", app);
    ak_print_normal("  %s --write --gpio_no 79 --direction=out --value=1 --drive=3 --pull_polarity=pullup --pull_enable=1 --input_enable=0 --slew_rate=slow\n", app);
}

int ak_drv_gpio_open(int gpio)
{
    int fd = -1;
    int ret = 0;
    char value[FILE_PATH_MAX] = {0};
    char path[FILE_PATH_MAX] = {0};

    memset(path, 0, FILE_PATH_MAX);
    sprintf(path, "/sys/class/gpio/gpio%d", gpio);

    if (access(path, F_OK)) {
        memset(path, 0, FILE_PATH_MAX);
        sprintf(path, "/sys/class/gpio/export");
        fd = open(path, O_WRONLY);
        if (fd < 0) {
            ak_print_error("Open gpio export fail\n");
            return -1;
        }
        memset(value, 0, FILE_PATH_MAX);
        sprintf(value, "%d\n", gpio);
        ret = write(fd, value, strlen(value));
        if (ret < 0) {
            close(fd);
            ak_print_error("create gpio%d fail %d\n", gpio, ret);
            return -1;
        }
        close(fd);
        //ak_print_normal("open gpio%d successfully\n", gpio);
    } else {
        ak_print_normal("gpio%d already open\n", gpio);
    }

    return 0;
}

int ak_drv_gpio_close(int gpio)
{
    int fd = -1;
    int ret = 0;
    char value[FILE_PATH_MAX] = {0};
    char path[FILE_PATH_MAX] = {0};

    memset(path, 0, FILE_PATH_MAX);
    sprintf(path, "/sys/class/gpio/gpio%d", gpio);

    if (!access(path, F_OK)) {
        memset(path, 0, FILE_PATH_MAX);
        sprintf(path, "/sys/class/gpio/unexport");
        fd = open(path, O_WRONLY);
        if (fd < 0) {
            ak_print_error("Open gpio unexport fail\n");
            return -1;
        }
        memset(value, 0, FILE_PATH_MAX);
        sprintf(value, "%d\n", gpio);
        ret = write(fd, value, strlen(value));
        if (ret < 0) {
            close(fd);
            ak_print_error("remove gpio%d fail %d\n", gpio, ret);
            return -1;
        }
        close(fd);
    }

    //ak_print_normal("close gpio%d successfully\n", gpio);
    return 0;
}

int ak_drv_gpio_read(int gpio, char *node, char *result, int count)
{
    int fd = -1;
    int ret = 0;
    char path[FILE_PATH_MAX] = {0};

    memset(path, 0, FILE_PATH_MAX);
    sprintf(path, "/sys/class/gpio/gpio%d/%s", gpio, node);
    //ak_print_normal("trying to open %s\n", path);

    if (!access(path, F_OK)) {
        fd = open(path, O_RDONLY);
        if (fd < 0) {
            ak_print_error("open %s: %s\n", path, strerror(errno));
            return -1;
        }
        ret = read(fd, result, count);
        if (ret < 0) {
            close(fd);
            ak_print_error("read %s: %s\n", path, strerror(errno));
            return -1;
        }
        close(fd);
    }

    //ak_print_normal("close %s successfully\n", path);
    return 0;
}

int ak_drv_gpio_write(int gpio, char *node, char *value, int count)
{
    int fd = -1;
    int ret = 0;
    char path[FILE_PATH_MAX] = {0};

    memset(path, 0, FILE_PATH_MAX);
    sprintf(path, "/sys/class/gpio/gpio%d/%s", gpio, node);
    //ak_print_normal("trying to open %s\n", path);

    if (!access(path, F_OK)) {
        fd = open(path, O_WRONLY);
        if (fd < 0) {
            ak_print_error("open %s:%s\n", path, strerror(errno));
            return -1;
        }
        ret = write(fd, value, strlen(value));
        if (ret < 0) {
            close(fd);
            ak_print_error("write %s:%s %s\n", path, value, strerror(errno));
            return -1;
        }
        close(fd);
    }

    //ak_print_normal("close %s successfully\n", path);
    return 0;
}

#define OPT_FIND 1
#define OP_MODE_READ 0
#define OP_MODE_WRITE 1
static int op_mode = OP_MODE_READ;
static int gpio_no = DEFULAT_CFG_VALUE;
#define DIR_IN  0
#define DIR_OUT 1
static int gpio_direction = DEFULAT_CFG_VALUE;
static int gpio_value = DEFULAT_CFG_VALUE;
static int gpio_drive = DEFULAT_CFG_VALUE;
#define GPIO_PULL_UP   1
#define GPIO_PULL_DOWN 0
static int gpio_pull_polarity = DEFULAT_CFG_VALUE;
static int gpio_pull_enable = DEFULAT_CFG_VALUE;
static int gpio_input_enable = DEFULAT_CFG_VALUE;
#define GPIO_SLEW_RATE_ENABLE  1
#define GPIO_SLEW_RATE_DISABLE 0
static int gpio_slew_rate = DEFULAT_CFG_VALUE;

int main (int argc, char **argv)
{
    int opt_val = 0, option_idx = 0;
    int i, tmp = 0;
    char value[FILE_PATH_MAX] = {0};

    tmp = 0;
    for (i = 0; i < argc; i++) {
        if ((!strcmp(argv[i], "-r")) || !strcmp(argv[i], "--read") || !strcmp(argv[i], "-read")) {
            tmp++;
            op_mode = OP_MODE_READ;
        }
        if ((!strcmp(argv[i], "-w")) || !strcmp(argv[i], "--write") || !strcmp(argv[i], "-write")) {
            tmp++;
            op_mode = OP_MODE_WRITE;
        }
    }
    if (tmp != 1) {
        ak_print_error("please check the operation mode, read or write\n");
        usage(SAMPLE_NAME);
        return -1;
    }

    while ((opt_val = getopt_long(argc, argv, OPTION_STRING,
                long_options, &option_idx)) != -1) {
        switch (opt_val) {
            case 'r':
                op_mode = OP_MODE_READ;
                break;
            case 'w':
                op_mode = OP_MODE_WRITE;
                break;
            case 'n':
                gpio_no = atoi(optarg);
                break;
            case 'd':
                if (op_mode == OP_MODE_WRITE) {
                    if (optarg && !strcmp(optarg, "in")) {
                        gpio_direction = DIR_IN;
                    } else {
                        gpio_direction = DIR_OUT;
                    }
                } else {
                    gpio_direction = OPT_FIND;
                }
                break;
            case 'v':
                if (optarg && op_mode == OP_MODE_WRITE) {
                    gpio_value = atoi(optarg);
                    if (gpio_value)
                        gpio_value = 1;
                    else
                        gpio_value = 0;
                } else {
                    gpio_value = OPT_FIND;
                }
                break;
            case 'c':
                if (optarg && op_mode == OP_MODE_WRITE) {
                    gpio_drive = atoi(optarg);
                    if (gpio_drive > 3)
                        gpio_drive = 3;
                    if (gpio_drive < 0)
                        gpio_drive = 0;
                } else {
                    gpio_drive = OPT_FIND;
                }
                break;
            case 'p':
                if (optarg && op_mode == OP_MODE_WRITE) {
                    if (!strcmp(optarg, "pullup")) {
                        gpio_pull_polarity = GPIO_PULL_UP;
                    } else {
                        gpio_pull_polarity = GPIO_PULL_DOWN;
                    }
                } else {
                    gpio_pull_polarity = OPT_FIND;
                }
                break;
            case 'e':
                if (optarg && op_mode == OP_MODE_WRITE) {
                    gpio_pull_enable = atoi(optarg);
                } else {
                    gpio_pull_enable = OPT_FIND;
                }
                break;
            case 'i':
                if (optarg && op_mode == OP_MODE_WRITE) {
                    gpio_input_enable = atoi(optarg);
                } else {
                    gpio_input_enable = OPT_FIND;
                }
                break;
            case 's':
                if (optarg && op_mode == OP_MODE_WRITE) {
                    if (!strcmp(optarg, "fast")) {
                        gpio_slew_rate = GPIO_SLEW_RATE_ENABLE;
                    } else {
                        gpio_slew_rate = GPIO_SLEW_RATE_DISABLE;
                    }
                } else {
                    gpio_slew_rate = OPT_FIND;
                }
                break;
            case 'h':
            default:
                usage(SAMPLE_NAME);
                return -EINVAL;
        }
    }

    ak_drv_gpio_open(gpio_no);

    if (op_mode == OP_MODE_READ) {
        if (gpio_direction == OPT_FIND) {
            memset(value, 0, FILE_PATH_MAX);
            if (!ak_drv_gpio_read(gpio_no, "direction", value, FILE_PATH_MAX))
                ak_print_notice("direction: %s", value);
        }
        if (gpio_value == OPT_FIND) {
            memset(value, 0, FILE_PATH_MAX);
            if (!ak_drv_gpio_read(gpio_no, "value", value, FILE_PATH_MAX))
                ak_print_notice("value: %s", value);
        }
        if (gpio_drive == OPT_FIND) {
            memset(value, 0, FILE_PATH_MAX);
            if (!ak_drv_gpio_read(gpio_no, "drive", value, FILE_PATH_MAX))
                ak_print_notice("drive: %s", value);
        }
        if (gpio_pull_polarity == OPT_FIND) {
            memset(value, 0, FILE_PATH_MAX);
            if (!ak_drv_gpio_read(gpio_no, "pull_polarity", value, FILE_PATH_MAX))
                ak_print_notice("pull_polarity: %s", value);
        }
        if (gpio_pull_enable == OPT_FIND) {
            memset(value, 0, FILE_PATH_MAX);
            if (!ak_drv_gpio_read(gpio_no, "pull_enable", value, FILE_PATH_MAX))
                ak_print_notice("pull_enable: %s", value);
        }
        if (gpio_input_enable == OPT_FIND) {
            memset(value, 0, FILE_PATH_MAX);
            if (!ak_drv_gpio_read(gpio_no, "input_enable", value, FILE_PATH_MAX))
                ak_print_notice("input_enable: %s", value);
        }
        if (gpio_slew_rate == OPT_FIND) {
            memset(value, 0, FILE_PATH_MAX);
            if (!ak_drv_gpio_read(gpio_no, "slew_rate", value, FILE_PATH_MAX))
                ak_print_notice("slew_rate: %s", value);
        }
    } else {
        if (gpio_direction != DEFULAT_CFG_VALUE) {
            if (gpio_direction == DIR_IN) {
                ak_drv_gpio_write(gpio_no, "direction", "in", strlen("in"));
            } else {
                ak_drv_gpio_write(gpio_no, "direction", "out", strlen("out"));
            }
        }
        if (gpio_value != DEFULAT_CFG_VALUE) {
            if (gpio_value == 1) {
                ak_drv_gpio_write(gpio_no, "value", "1", strlen("1"));
            } else {
                ak_drv_gpio_write(gpio_no, "value", "0", strlen("0"));
            }
        }
        if (gpio_drive != DEFULAT_CFG_VALUE) {
            memset(value, 0, FILE_PATH_MAX);
            sprintf(value, "%d", gpio_drive);
            ak_drv_gpio_write(gpio_no, "drive", value, strlen(value));
        }
        if (gpio_pull_polarity != DEFULAT_CFG_VALUE) {
            if (gpio_pull_polarity == GPIO_PULL_UP) {
                ak_drv_gpio_write(gpio_no, "pull_polarity", "pullup", strlen("pullup"));
            } else {
                ak_drv_gpio_write(gpio_no, "pull_polarity", "pulldown", strlen("pulldown"));
            }
        }
        if (gpio_pull_enable != DEFULAT_CFG_VALUE) {
            if (gpio_pull_enable) {
                ak_drv_gpio_write(gpio_no, "pull_enable", "1", strlen("1"));
            } else {
                ak_drv_gpio_write(gpio_no, "pull_enable", "0", strlen("0"));
            }
        }
        if (gpio_input_enable != DEFULAT_CFG_VALUE) {
            if (gpio_input_enable) {
                ak_drv_gpio_write(gpio_no, "input_enable", "1", strlen("1"));
            } else {
                ak_drv_gpio_write(gpio_no, "input_enable", "0", strlen("0"));
            }
        }
        if (gpio_slew_rate != DEFULAT_CFG_VALUE) {
            if (gpio_slew_rate == GPIO_SLEW_RATE_ENABLE) {
                ak_drv_gpio_write(gpio_no, "slew_rate", "fast", strlen("fast"));
            } else {
                ak_drv_gpio_write(gpio_no, "slew_rate", "slow", strlen("slow"));
            }
        }
    }

    ak_drv_gpio_close(gpio_no);

    ak_print_notice("\n!!!!%s PIN_%d CFG:dir %d val %d drv %d %s pull_en %d input_en %d slew_rate %d successfully!!!!\n\n",
        (op_mode == OP_MODE_READ) ? "read" : "write", gpio_no, gpio_direction,
        gpio_value, gpio_drive, (gpio_pull_polarity == GPIO_PULL_UP) ? "pullup" : "pulldown",
        gpio_pull_enable, gpio_input_enable, gpio_slew_rate);

    return 0;
}
