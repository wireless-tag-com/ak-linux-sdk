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

#define PWM_DEV_MAX_NUM 5
#define PERIOD_S_MIN 167L /* 1s /6Mhz */
#define PERIOD_S_MAX 10869565L /* 1s / 92hz */

#define SAMPLE_NAME "ak_drv_pwm_sample"

int ak_drv_led_set(char *type, int status);

static int dev_no = DEFULAT_CFG_VALUE;
static long long duty_ns = DEFULAT_CFG_VALUE;
static long long period_ns = DEFULAT_CFG_VALUE;
static int time = DEFULAT_CFG_VALUE;

#define OPTION_STRING   "n:d:p:t:h"

static struct option long_options[] =
{
    {"pwm_no", required_argument, NULL, 'n'},
    {"duty_ns", required_argument, NULL, 'd'},
    {"period_ns", required_argument, NULL, 'p'},
    {"time", required_argument, NULL, 't'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

static char option_hint[][MAX_HINT_LEN] =
{
    "PWM设备标识号 可选从0-5",
    "脉冲宽度 单位是纳秒/ns 脉冲宽度必须小于信号周期",
    "信号周期 单位是纳秒/ns duty_ns/period_ns 即为占空比 范围为167(6Mhz)-10869565(92Hz)",
    "脉冲持续时间 单位为秒",
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
    ak_print_normal("  %s -n 0 -d 5000 -p 10000 -t 5\n", app);
    ak_print_normal("  %s --pwm_no 0 --duty_ns 5000 --period_ns 10000 --time 5\n", app);
}

/*
 * ak_drv_pwm_open - open pwm device
 * device_no[IN]: pwm device minor-number,[0-4]
 * return: 0 - success; otherwise error code;
 */
int ak_drv_pwm_open(int device_no)
{
    int fd = -1;
    int ret = 0;
    char dev_name[64] = {0};
    char path[64] = {0};

    if (device_no < 0 || device_no > PWM_DEV_MAX_NUM) {
        ak_print_error("device_no error, %d, must in [0-%d]\n", device_no, PWM_DEV_MAX_NUM);
        return -1;
    }

    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0", device_no);

    if (!access(path, F_OK)) {
        ak_print_notice( "pwm dev %d already opened.\n", device_no);
        return 0;
    }

    /*
    *create pwm dev use export;
    *close pwm dev use unexport.
    */
    sprintf(dev_name, "/sys/class/pwm/pwmchip%d/export", device_no);

    fd = open(dev_name, O_WRONLY);
    if (fd < 0) {
        ak_print_error("Open pwm dev %d fail\n", device_no);
        return -1;
    }

    /*
    *write 0 to export, it will create pwm0 folder.
    */
    ret = write(fd, "0", strlen("0"));
    if(ret < 0) {
        close(fd);
        ak_print_error("create pwm dev %d fail\n", device_no);
        return -1;
    }
    close(fd);

    return 0;
}

/*
 * ak_drv_pwm_set - set pwd working param
 * device_no[IN]: pwm device minor-number,[0-4]
 * duty_ns[IN]: pwm duty time in ns.
 * period_ns[IN]: pwm period time in ns.
 * return: 0 on success, otherwise error code.
 */
int ak_drv_pwm_set(int device_no, long long duty_ns, long long period_ns)
{
    int fd = -1;
    int ret;
    char path[64] = {0};
    char str[32] = {0};

    /* arguments check */
    if (device_no < 0 || device_no > PWM_DEV_MAX_NUM) {
        ak_print_error("device_no error, %d, must in [0-%d]\n", device_no, PWM_DEV_MAX_NUM);
        return -1;
    }

    if (period_ns < PERIOD_S_MIN || period_ns > PERIOD_S_MAX) {
        ak_print_error("period_ns error, must in [%ld-%ld]\n", PERIOD_S_MIN, PERIOD_S_MAX);
        return -1;
    }

    if (duty_ns < 0 || duty_ns > period_ns) {
        ak_print_error("duty_ns error, must less than period_ns\n");
        return -1;
    }

    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0", device_no);

    if (access(path, F_OK)) {
        ak_print_error("pwm dev %d not open.\n", device_no);
        return -1;
    }

    /*
    *disable pwm before set pwm period and duty_cycle
    */
    memset(path, 0, 64);
    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0/enable", device_no);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        ak_print_error("pwm dev %d enable file open failed.\n", device_no);
        return -1;
    }

    ret = write(fd, "0", strlen("0"));
    if (ret < 0) {
        ak_print_error("pwm dev %d set disable failed.\n", device_no);
        close(fd);
        return -1;
    }
    close(fd);

    /*
    *set pwn period
    */
    memset(path, 0, 64);
    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0/period", device_no);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        ak_print_error("pwm dev %d period file open failed.\n", device_no);
        return -1;
    }

    sprintf(str, "%lld", period_ns);
    ret = write(fd, str, strlen(str));
    if (ret < 0) {
        ak_print_error("pwm dev %d set period_ns failed.\n", device_no);
        close(fd);
        return -1;
    }
    close(fd);

    /*
    *set pwn duty_cycle
    */
    memset(path, 0, 64);
    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0/duty_cycle", device_no);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        ak_print_error("pwm dev %d duty file open failed.\n", device_no);
        return -1;
    }

    memset(str, 0, 32);
    sprintf(str, "%lld", duty_ns);
    ret = write(fd, str, strlen(str));
    if (ret < 0) {
        ak_print_error("pwm dev %d set duty_ns failed.\n", device_no);
        close(fd);
        return -1;
    }

    close(fd);

    /*
    *enable pwm after set pwm period and duty_cycle
    */
    memset(path, 0, 64);
    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0/enable", device_no);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        ak_print_error("pwm dev %d enable file open failed.\n", device_no);
        return -1;
    }

    ret = write(fd, "1", strlen("1"));
    if (ret < 0) {
        ak_print_error("pwm dev %d set enable failed.\n", device_no);
        close(fd);
        return -1;
    }
    close(fd);

    return 0;
}

/*
 * ak_drv_pwm_close - close pwm
 * device_no[IN]: pwm device minor-number,[0-4]
 * return: 0 on success, otherwise error code.
 */
int ak_drv_pwm_close(int device_no)
{
    int fd = -1;
    int ret = 0;
    char path[64] = {0};
    char dev_name[64] = {0};
    char str[32] = {0};

    if (device_no < 0 || device_no > PWM_DEV_MAX_NUM) {
        ak_print_error("device_no error, %d, must in [0-%d]\n", device_no, PWM_DEV_MAX_NUM);
        return -1;
    }

    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0", device_no);

    if (access(path, F_OK)) {
        ak_print_error("pwm dev %d not open.\n", device_no);
        return -1;
    }

    /*
    *disable pwm before close
    */
    memset(path, 0, 64);
    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0/enable", device_no);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        ak_print_error("pwm dev %d enable file open failed.\n", device_no);
        return -1;
    }

    ret = write(fd, "0", strlen("0"));
    if (ret < 0) {
        ak_print_error("pwm dev %d set disable failed.\n", device_no);
        close(fd);
        return -1;
    }
    close(fd);

    /*
    *set pwn period
    */
    memset(path, 0, 64);
    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0/period", device_no);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        ak_print_error("pwm dev %d period file open failed.\n", device_no);
        return -1;
    }

    sprintf(str, "%ld", PERIOD_S_MAX);
    ret = write(fd, str, strlen(str));
    if (ret < 0) {
        ak_print_error("pwm dev %d set period_ns failed.\n", device_no);
        close(fd);
        return -1;
    }
    close(fd);

    /*
    *set pwn duty_cycle
    */
    memset(path, 0, 64);
    sprintf(path, "/sys/class/pwm/pwmchip%d/pwm0/duty_cycle", device_no);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        ak_print_error("pwm dev %d duty file open failed.\n", device_no);
        return -1;
    }

    memset(str, 0, 32);
    sprintf(str, "%d", 0);
    ret = write(fd, str, strlen(str));
    if (ret < 0) {
        ak_print_error("pwm dev %d set duty_ns failed.\n", device_no);
        close(fd);
        return -1;
    }

    /*
    *close pwm dev
    */
    sprintf(dev_name, "/sys/class/pwm/pwmchip%d/unexport", device_no);

    fd = open(dev_name, O_WRONLY);

    if (fd < 0) {
        ak_print_error("Open pwm dev unexport %d fail\n", device_no);
        return -1;
    }

    /*
    *write 0 to unexport, it will close pwm dev.
    */
    ret = write(fd, "0", strlen("0"));
    if (ret < 0) {
        close(fd);
        ak_print_error("close pwm dev %d fail\n", device_no);
        return -1;
    }
    close(fd);

    return 0;
}

int main (int argc, char **argv)
{
    int opt_val = 0, option_idx = 0;
    int ret;

    while ((opt_val = getopt_long(argc, argv, OPTION_STRING,
                long_options, &option_idx)) != -1) {
        switch (opt_val) {
            case 'n':
                dev_no = atoi(optarg);
                break;
            case 'd':
                duty_ns = atoi(optarg);
                break;
            case 'p':
                period_ns = atoi(optarg);
                break;
            case 't':
                time = atoi(optarg);
                break;
            case 'h':
            default:
                usage(SAMPLE_NAME);
                return -EINVAL;
        }
    }

    if ((dev_no == DEFULAT_CFG_VALUE)
        || (duty_ns == DEFULAT_CFG_VALUE)
        || (period_ns == DEFULAT_CFG_VALUE)
        || (time == DEFULAT_CFG_VALUE)) {
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

    if (period_ns < PERIOD_S_MIN || period_ns > PERIOD_S_MAX) {
        ak_print_error("please check the period_ns.\n");
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

    if (duty_ns < 0 || duty_ns > period_ns) {
        ak_print_error("please check the duty_ns.\n");
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

    ak_print_normal("PWM_%d:@%lld on %lld(%ds)\n", dev_no, duty_ns, period_ns, time);
    ak_print_normal("try to open pwm device...\n");
    ret = ak_drv_pwm_open(dev_no);
    if (ret) {
        ak_print_error("open pwm device fail.\n");
        return -1;
    }
    ak_print_normal("try to config pwm device...\n");
    ret = ak_drv_pwm_set(dev_no, duty_ns, period_ns);
    if (ret) {
        ak_print_error("set pwm device fail.\n");
        return -1;
    }
    ak_print_normal("keep %d seconds\n", time);
    sleep(time);
    ak_print_normal("try to close pwm device...\n");
    ak_drv_pwm_close(dev_no);

    return 0;
}
