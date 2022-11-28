/*
* @FILENAME: ak_drv_common.c
* @BRIEF
* Copyright (C) 2019 Anyka(Guangzhou) Microelcctronics Technology CO., LTD.
* @AUTHOR Bill gates
* @DATE 2019-12-16
* @VERSION 1.0
* @REF Please refer to ...
*/

/*******************************************************************************
    The following is an example to use module drivers APIs

*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <stdarg.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <dlfcn.h>
#include <ucontext.h>

#include "ak_drv_common.h"

/* print color define */
#define NONE          "\033[m"          //close
#define NONE_N        "\033[m\n"        //close and new_line
#define RED           "\033[0;32;31m"   //red
#define LIGHT_RED     "\033[1;31m"      //light red
#define GREEN         "\033[0;32;32m"   //green
#define LIGHT_GREEN   "\033[1;32m"      //light green
#define BLUE          "\033[0;32;34m"   //blue
#define LIGHT_BLUE    "\033[1;34m"      //light blue
#define DARK_GREY     "\033[1;30m"      //dark grey
#define LIGHT_GREY    "\033[0;37m"      //light grey
#define CYAN          "\033[0;36m"      //syan
#define LIGHT_CYAN    "\033[1;36m"      //light cyan
#define PURPLE        "\033[0;35m"      //purple
#define LIGHT_PURPLE  "\033[1;35m"      //light purple
#define BROWN         "\033[0;33m"      //brown
#define YELLOW        "\033[1;33m"      //yellow
#define WHITE         "\033[1;37m"      //white

#define MAX_TRACE_SIZE          16
#define DATE_YEAR_DIFF          1900
#define LOG_BUF_LEN             (1024)
#define AK_PRINT_LEVEL_IDENT    "/var/log/ak_print_level"

/* default level LOG_LEVEL_NORMAL */
static int log_level = LOG_LEVEL_NORMAL;
static unsigned char set_default_level = AK_TRUE;

/**
 * @BRIEF       get_cur_log_level: get current print log level
 * @AUTHOR      cao_donghua
 * @DATE date   2019-12-17
 * @PARAM[in] void
 * return we return log level
 * retval 0~6
 */
static inline int get_cur_log_level(void)
{
    char buf[10] = {0};
    FILE *filp = NULL;

    /*
     * If log ident file not exist, we will create this file
     * and set to defalt level 3.
     */
    if (set_default_level) {
        set_default_level = AK_FALSE;

        filp = fopen(AK_PRINT_LEVEL_IDENT, "w");
        if (filp) {
            sprintf(buf, "%d", log_level);
            fwrite(buf, 1, strlen(buf), filp);
            fclose(filp);
        } else {
            fprintf(stderr, "\n\tfopen() %s failed, %s\n\n",
                AK_PRINT_LEVEL_IDENT, strerror(errno));
        }

        //printf("default log level=%d\n", log_level);
    } else {
        filp = fopen(AK_PRINT_LEVEL_IDENT, "r");
        if (filp) {
            fread(buf, 1, 9, filp);
            fclose(filp);
        }

        int level = atoi(buf);
        if (level > LOG_LEVEL_DEBUG)
            level = LOG_LEVEL_DEBUG;
        else if (level < LOG_LEVEL_RESERVED)
            level = LOG_LEVEL_RESERVED;

        log_level = level;
    }

    return log_level;
}


/**
 * @BRIEF       ak_print: print function we defined for debugging
 * @AUTHOR      cao_donghua
 * @DATE date   2019-12-17
 * @PARAM[in] level:print level [0,5]
 * @PARAM[in] fmt:char string format like printf()
 * @PARAM[in] ...:variable arguments list
 * return we return 0 always.
 * retval 0
 */
int ak_print(int level, const char *fmt, ...)
{
    /* do not print and wouldn't write to sys-log */
    if (level > get_cur_log_level())
        return 0;

    char buf[LOG_BUF_LEN] = {0};
    va_list args;

    va_start(args, fmt);
    vsnprintf(buf, LOG_BUF_LEN, fmt, args);
    va_end(args);

    /* color select and syslog write judge */
    switch (level) {
    case LOG_LEVEL_ERROR:
        /* error print, red color */
        printf(LIGHT_RED"%s"NONE, buf);
        syslog(LOG_ERR, "%s", buf);
        break;
    case LOG_LEVEL_WARNING:
        /* fatal print, yellow color */
        printf(PURPLE"%s"NONE, buf);
        syslog(LOG_EMERG, "%s", buf);
        break;
    case LOG_LEVEL_NOTICE:
        /* fatal print, yellow color */
        printf(YELLOW"%s"NONE, buf);
        syslog(LOG_NOTICE, "%s", buf);
        break;
    case LOG_LEVEL_NORMAL:
        /* normal print, we add no color to it */
        printf("%s", buf);
        syslog(LOG_NOTICE, "%s", buf);
        break;
    case LOG_LEVEL_INFO:
        /* info print, blue color */
        printf(BLUE"%s"NONE, buf);
        syslog(LOG_INFO, "%s", buf);
        break;
    case LOG_LEVEL_DEBUG:
        /* debug print, green color */
        printf(GREEN"%s"NONE, buf);
        syslog(LOG_DEBUG, "%s", buf);
        break;
    default:
        break;
    }

    fflush(stdout);
    fflush(stderr);

    return 0;
}
