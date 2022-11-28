#ifndef _AK_LOG_H_
#define _AK_LOG_H_

/**
 * print functions
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
 * ak_print: print function we defined for debugging
 * @module[IN]: module id
 * @level[IN]: print level [0,5]
 * @fmt[IN]: format like printf()
 * @...[IN]: variable arguments list
 * return: we return 0 always.
 */
int ak_print(int module_id, int level, const char *fmt, ...)__attribute__((format(printf,3,4)));

#define ak_print_error(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_ERROR, fmt, ##arg)
#define ak_print_warning(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_WARNING, fmt, ##arg)
#define ak_print_notice(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_NOTICE, fmt, ##arg)
#define ak_print_normal(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_NORMAL, fmt, ##arg)
#define ak_print_info(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_INFO, fmt, ##arg)
#define ak_print_debug(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_DEBUG, fmt, ##arg)

#define ak_print_error_ex(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_ERROR, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_warning_ex(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_WARNING, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_notice_ex(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_NOTICE, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_normal_ex(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_NORMAL, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_info_ex(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_INFO, "[%s:%d] "fmt, __func__, __LINE__, ##arg)
#define ak_print_debug_ex(module_id, fmt, arg...) \
	ak_print(module_id, LOG_LEVEL_DEBUG,  "[%s:%d] "fmt, __func__, __LINE__, ##arg)

/**
 * ak_print_get_version - get log version
 * return: version string
 * notes:
 */
const char* ak_print_get_version(void);

/**
 * ak_print_set_level - set current print level
 * @module_id[IN]: module id
 * @level[IN]: current level
 * return: old level
 */
int ak_print_set_level(int module_id, int level);

/**
 * ak_print_set_syslog_level - set print log level
 * @module[IN] : module id
 * @level[IN]: level to set, value in enum LOG_LEVEL
 * return: always return 0
 */
int ak_print_set_syslog_level(int module_id, int level);

#endif

/* end of file */
