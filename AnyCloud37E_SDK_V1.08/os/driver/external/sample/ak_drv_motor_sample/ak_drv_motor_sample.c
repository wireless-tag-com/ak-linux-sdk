#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <getopt.h>
#include "ak_drv_common.h"
#include "../../include/ak_motor.h"

#define FILE_PATH_MAX 64

#define MAX_HINT_LEN 512
#define DEFULAT_CFG_VALUE (-1)

#define SAMPLE_NAME "ak_drv_motor_sample"

#define OPTION_STRING   "n:c::s::x::t::b::m:a::h"

#define MOTOR0 "/dev/motor0"
#define MOTOR1 "/dev/motor1"

#define CMD_MOTOR_MOVE_LIMIT 0
#define CMD_MOTOR_MOVE_NOLIMIT 1
#define CMD_MOTOR_RESET 2
#define CMD_MOTOR_MIDDLE 3
#define CMD_MOTOR_CRUISE 4
#define CMD_MOTOR_BOUNDARY 5
#define CMD_MOTOR_STATUS 6
#define CMD_MOTOR_CFG 7

static struct option long_options[] =
{
    {"devno", required_argument, NULL, 'n'},
    {"current", optional_argument, NULL, 'c'},
    {"speed", optional_argument, NULL, 's'},
    {"step", optional_argument, NULL, 'x'},
    {"total", optional_argument, NULL, 't'},
    {"boundary", optional_argument, NULL, 'b'},
    {"cmd", required_argument, NULL, 'm'},
    {"args", optional_argument, NULL, 'a'},
    {"help", no_argument, NULL, 'h'},
    {0, 0, 0, 0},
};

static char option_hint[][MAX_HINT_LEN] =
{
    "马达的设备号 从0开始 依具体设备而定",
    "当前的位置",
    "频率 单位时间/1秒内的步进总数",
    "一圈的步数总和",
    "总运动步数",
    "边界步数",
    "命令",
    "命令参数",
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
        if (long_options[i].val == 'm') {
            ak_print_normal("       0: MOTOR_MOVE_LIMIT 有限制旋转\n");
            ak_print_normal("       1: MOTOR_MOVE_NOLIMIT 无限制旋转\n");
            ak_print_normal("       2: MOTOR_RESET 转动到初始位置\n");
            ak_print_normal("       3: MOTOR_MIDDLE 转动到中间位置\n");
            ak_print_normal("       4: MOTOR_CRUISE 反复转动\n");
            ak_print_normal("       5: MOTOR_BOUNDARY 转动到边界\n");
            ak_print_normal("       6: MOTOR_STATUS 获取当前状态\n");
            ak_print_normal("       7: CMD_MOTOR_CFG 更新配置\n");
        }
    }
    ak_print_normal("\n");
    ak_print_normal("example:\n");
    ak_print_normal("  限定边界转动步数(顺时针步数大于0 逆时针步数小于0):\n");
    ak_print_normal("    %s -n 0 -m 0 -a300\n", app);
    ak_print_normal("  不限定边界转动步数(顺时针步数大于0 逆时针步数小于0):\n");
    ak_print_normal("    %s -n 0 -m 1 -a300\n", app);
    ak_print_normal("  转动到初始位置:\n");
    ak_print_normal("    %s -n 0 -m 2\n", app);
    ak_print_normal("  转动到中间位置:\n");
    ak_print_normal("    %s -n 0 -m 3\n", app);
    ak_print_normal("  反复转动:\n");
    ak_print_normal("    %s -n 0 -m 4 -a60\n", app);
    ak_print_normal("  转动到边界:\n");
    ak_print_normal("    %s -n 0 -m 5 -a10\n", app);
    ak_print_normal("  获取当前的状态:\n");
    ak_print_normal("    %s -n 0 -m 6\n", app);
    ak_print_normal("  更新配置:\n");
    ak_print_normal("    %s -n 0 -c0 -s400 -x4096 -t3600 -b50 -m 7\n", app);
    ak_print_normal("  对应的长指令:\n");
    ak_print_normal("    %s --devno 0 --current=0 --speed=400 --step=4096 --total=3600 --boundary=50 --cmd 7\n", app);
    ak_print_normal("\n");
}

static int devno = DEFULAT_CFG_VALUE;
static int current = DEFULAT_CFG_VALUE;
static int speed = DEFULAT_CFG_VALUE;
static int step = DEFULAT_CFG_VALUE;
static int total = DEFULAT_CFG_VALUE;
static int boundary = DEFULAT_CFG_VALUE;
static int cmd = DEFULAT_CFG_VALUE;
static int args = DEFULAT_CFG_VALUE;

int main (int argc, char **argv)
{
    int opt_val = 0, option_idx = 0, ret = 0, looper = 0;
    struct motor_parm parm;
    int fd, operrno;
    struct motor_reset_data reset;
    struct motor_message msg;

    while ((opt_val = getopt_long(argc, argv, OPTION_STRING,
                long_options, &option_idx)) != -1) {
        switch (opt_val) {
            case 'n':
                devno = atoi(optarg);
                break;
            case 'c':
                if (optarg != NULL) {
                    current = atoi(optarg);
                }
                break;
            case 's':
                if (optarg != NULL) {
                    speed = atoi(optarg);
                }
                break;
            case 'x':
                if (optarg != NULL) {
                    step = atoi(optarg);
                }
                break;
            case 't':
                if (optarg != NULL) {
                    total = atoi(optarg);
                }
                break;
            case 'b':
                if (optarg != NULL) {
                    boundary = atoi(optarg);
                }
                break;
            case 'm':
                cmd = atoi(optarg);
                break;
            case 'a':
                if (optarg != NULL) {
                    //ak_print_normal("args: %s\n", optarg);
                    args = atoi(optarg);
                }
                break;
            case 'h':
            default:
                usage(SAMPLE_NAME);
                return -EINVAL;
        }
    }

    ak_print_normal("INPUT:devno:%d cmd:%d args %d\n", devno, cmd, args);
    ak_print_normal("INPUT:current:%d speed:%d step:%d total %d boundary %d\n",
        current, speed, step, total, boundary);

    if ((devno == DEFULAT_CFG_VALUE)
        || (cmd == DEFULAT_CFG_VALUE)) {
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

    if ((cmd == CMD_MOTOR_MOVE_LIMIT) || (cmd == CMD_MOTOR_MOVE_NOLIMIT) || (cmd == CMD_MOTOR_BOUNDARY)) {
        if (args == DEFULAT_CFG_VALUE) {
            ak_print_notice("Need the args!!!\n");
            usage(SAMPLE_NAME);
            return -EINVAL;
        }
    }

    if ((cmd == CMD_MOTOR_CRUISE) && (args < 0)) {
        ak_print_notice("Need the args > 0(s)!!!\n");
        usage(SAMPLE_NAME);
        return -EINVAL;
    }

    if ((cmd == CMD_MOTOR_CFG)
        && ((current == DEFULAT_CFG_VALUE)
            || (speed == DEFULAT_CFG_VALUE)
            || (step == DEFULAT_CFG_VALUE)
            || (total == DEFULAT_CFG_VALUE)
            || (boundary == DEFULAT_CFG_VALUE))) {
        ak_print_notice("Need");
        if (current == DEFULAT_CFG_VALUE)
            ak_print_notice(" current");
        if (speed == DEFULAT_CFG_VALUE)
            ak_print_notice(" speed");
        if (step == DEFULAT_CFG_VALUE)
            ak_print_notice(" step");
        if (total == DEFULAT_CFG_VALUE)
            ak_print_notice(" total");
        if (boundary == DEFULAT_CFG_VALUE)
            ak_print_notice(" boundary");
        ak_print_notice("\n\n");
        return -EINVAL;
    } else {
        parm.pos = current;
        parm.speed_step = speed;
        parm.steps_one_circle = step;
        parm.total_steps = total;
        parm.boundary_steps = boundary;
    }

    switch (devno) {
        case 0:
            fd = open(MOTOR0, 0);
            operrno = errno;
            break;
        case 1:
            fd = open(MOTOR1, 0);
            operrno = errno;
            break;
        default:
            usage(SAMPLE_NAME);
            return -1;
    }

    if (fd < 0) {
        ak_print_notice("fail to open motor device %s\n", strerror(operrno));
        return -1;
    }

    switch (cmd) {
        case CMD_MOTOR_MOVE_LIMIT:
            if (ioctl(fd, MOTOR_MOVE_LIMIT, &args)) {
                ak_print_error("MOTOR_MOVE_LIMIT fail\n");
            }
            break;
        case CMD_MOTOR_MOVE_NOLIMIT:
            if (ioctl(fd, MOTOR_MOVE_NOLIMIT, &args)) {
                ak_print_error("MOTOR_MOVE_NOLIMIT fail\n");
            }
            break;
        case CMD_MOTOR_RESET:
            if (ioctl(fd, MOTOR_RESET, &reset)) {
                ak_print_error("MOTOR_STOP fail\n");
            } else {
                ak_print_normal("total_steps:%d cur_step:%d\n", reset.total_steps, reset.cur_step);
            }
            break;
        case CMD_MOTOR_MIDDLE:
            if (ioctl(fd, MOTOR_MIDDLE, NULL)) {
                ak_print_error("CMD_MOTOR_MIDDLE fail\n");
            }
            break;
        case CMD_MOTOR_CRUISE:
            if (ioctl(fd, MOTOR_CRUISE, NULL)) {
                ak_print_error("MOTOR_CRUISE fail\n");
            }
            break;
        case CMD_MOTOR_BOUNDARY:
            if (ioctl(fd, MOTOR_BOUNDARY, &args)) {
                ak_print_error("MOTOR_BOUNDARY fail\n");
            }
            break;
        case CMD_MOTOR_STATUS:
            if (!ioctl(fd, MOTOR_GET_STATUS, &msg)) {
                ak_print_normal("status:%d\n", msg.status);
                ak_print_normal("pos:%d\n", msg.pos);
                ak_print_normal("speed_step:%d\n", msg.speed_step);
                ak_print_normal("speed_angle:%d\n", msg.speed_angle);
                ak_print_normal("steps_one_circle:%d\n", msg.steps_one_circle);
                ak_print_normal("total_steps:%d\n", msg.total_steps);
                ak_print_normal("boundary_steps:%d\n", msg.boundary_steps);
                ak_print_normal("attach_timer:%d\n", msg.attach_timer);
            }
            break;
        case CMD_MOTOR_CFG:
            if (ioctl(fd, MOTOR_PARM, &parm)) {
                ak_print_error("MOTOR_CFG fail\n");
            }
            break;
        default:
            usage(SAMPLE_NAME);
    }

    if ((cmd == CMD_MOTOR_STATUS) || (cmd == CMD_MOTOR_CFG))
        goto exit;

    if (cmd == CMD_MOTOR_CRUISE) {
        do {
            sleep(1);
            looper++;
        } while (looper < args);
    } else {
        do {
            sleep(1);
            ret = ioctl(fd, MOTOR_GET_STATUS, &msg);
            if (ret) {
                break;
            }
        } while(msg.status == MOTOR_IS_RUNNING);
    }

    ak_print_normal("!!!%s ending!!!\n", SAMPLE_NAME);

exit:
    if (fd)
        close(fd);

    return 0;
}
