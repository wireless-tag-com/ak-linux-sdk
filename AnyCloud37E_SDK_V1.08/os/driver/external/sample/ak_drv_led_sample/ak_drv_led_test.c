#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "ak_drv_common.h"

#define LED_DEV_MAX_NUM     5

/*
 * ak_drv_led_set
 * type[IN]: led device type
 * status[IN]: 1 means light on the led,0 means light off led.
 * return: 0 on success, otherwise error code.
 */
int ak_drv_led_set(char *type, int status)
{
    int fd = -1;
    int ret;
    char path[64] = {0};
    char str[32] = {0};

    if (status < 0 || status > 1) {
        ak_print_error("switch value error.\n");
        return -EINVAL;
    }

    sprintf(path, "/sys/class/leds/%s/brightness", type);

    if (access(path, F_OK)) {
        ak_print_error("led dev %s not open.\n", type);
        return -EPERM;
    }

    /*
    * open and set leds.
    */
    memset(path, 0, 64);
    sprintf(path, "/sys/class/leds/%s/brightness", type);
    fd = open(path, O_RDWR);

    if (fd < 0) {
        ak_print_error("led dev %s file open failed.\n", type);
        return -EPERM;
    }

    memset(str, 0, 32);
    sprintf(str, "%d", status);
    ret = write(fd, str, strlen(str));
    if (ret < 0) {
        ak_print_error("led dev %s set failed.\n", type);
        close(fd);
        return -EPERM;
    }

    close(fd);

    return 0;
}