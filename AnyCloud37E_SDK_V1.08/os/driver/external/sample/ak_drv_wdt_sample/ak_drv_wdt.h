#ifndef _AK_DRV_WDT_H_
#define _AK_DRV_WDT_H_

/**
 * ak_drv_wdt_open - open watch dog and watch dog start work.
 * @feed_timeout: [in] second, [1, 357]
 * return: 0 - success; otherwise error code;
 * note:
 */
int ak_drv_wdt_open(unsigned int feed_timeout);

/**
 * ak_drv_wdt_feed - feed watch dog.
 * return:0 - success; otherwise error code;
 */
int ak_drv_wdt_feed(void);

/**
 * ak_drv_wdt_close - close watch dog.
 * return:0 - success; otherwise error code;
 */
int ak_drv_wdt_close(void);


#endif
