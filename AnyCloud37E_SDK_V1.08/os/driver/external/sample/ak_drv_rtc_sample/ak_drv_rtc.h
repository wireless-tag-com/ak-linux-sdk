#ifndef _AK_DRV_RTC_H_
#define _AK_DRV_RTC_H_

/**
 * @brief read the rtc time
 * 
 * @param void
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_rtc_read(void);

/**
 * @brief synchronize the system time to the rtc
 * 
 * @param void
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_sync_to_rtc(void);

/**
 * @brief synchronize the system time from the rtc
 * 
 * @param void
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_sync_from_rtc(void);

/**
 * @brief set up a relative-time wakeup
 * 
 * @param s: how many seconds will it wakeup
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_alarm_wakeup_s(int s);

/**
 * @brief set up a fixed-time wakeup
 * 
 * @param time: set a fixed time
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_alarm_wakeup_t(const char *time);

#endif
