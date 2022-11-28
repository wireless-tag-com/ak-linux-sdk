/*
* @FILENAME: ak_modules_drv_common.h
* @BRIEF
* Copyright (C) 2019 Anyka(Guangzhou) Microelcctronics Technology CO., LTD.
* @AUTHOR Bill gates
* @DATE 2019-12-16
* @VERSION 1.0
* @REF Please refer to ...
*/

/*******************************************************************************
    The following is an example to use lcd drivers APIs

*******************************************************************************/

#ifndef _AK_DRV_COMMON_H_
#define _AK_DRV_COMMON_H_

#define AK_SUCCESS          0
#define AK_FAILED           -1
#define AK_FALSE            0
#define AK_TRUE             1


/**
 * @BRIEF print functions
 * 1. error print, call this when sys-func fail or self-define error occur
 * 2. warning print, warning conditions
 * 3. notice print, normal but significant
 * 4. normal print, normal message or tips
 * 5. info print, use for cycle print like watch-dog feed print
 * 6. debug print, use for debug
 */
enum LOG_LEVEL {
    LOG_LEVEL_RESERVED = 0,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_WARNING,
    LOG_LEVEL_NOTICE,
    LOG_LEVEL_NORMAL,
    LOG_LEVEL_INFO,
    LOG_LEVEL_DEBUG,
};

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
int ak_print(int level, const char *fmt, ...)__attribute__((format(printf,2,3)));

#define ak_print_error(fmt, arg...) \
    ak_print(LOG_LEVEL_ERROR, "\n"fmt, ##arg)
#define ak_print_warning(fmt, arg...) \
    ak_print(LOG_LEVEL_WARNING, "\n"fmt, ##arg)
#define ak_print_notice(fmt, arg...) \
    ak_print(LOG_LEVEL_NOTICE, fmt, ##arg)
#define ak_print_normal(fmt, arg...) \
    ak_print(LOG_LEVEL_NORMAL, fmt, ##arg)
#define ak_print_info(fmt, arg...) \
    ak_print(LOG_LEVEL_INFO, fmt, ##arg)
#define ak_print_debug(fmt, arg...) \
    ak_print(LOG_LEVEL_DEBUG, fmt, ##arg)

#define ak_print_error_ex(fmt, arg...) \
    ak_print(LOG_LEVEL_ERROR, "\n[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_warning_ex(fmt, arg...) \
    ak_print(LOG_LEVEL_WARNING, "\n[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_notice_ex(fmt, arg...) \
    ak_print(LOG_LEVEL_NOTICE, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_normal_ex(fmt, arg...) \
    ak_print(LOG_LEVEL_NORMAL, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_info_ex(fmt, arg...) \
    ak_print(LOG_LEVEL_INFO, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_debug_ex(fmt, arg...) \
    ak_print(LOG_LEVEL_DEBUG,  "[%s:%d] "fmt, __func__, __LINE__, ##arg)

#endif
