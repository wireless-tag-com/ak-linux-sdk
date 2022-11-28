#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include "ak_drv_rtc.h"
#include "ak_drv_common.h"

#define SYS_POWER_PATH		"/sys/power/state"	/* Power Management Path */
#define PROC_DEV_NODE		"/dev/rtc0"			/* RTC Device Node */

/**
 * @brief read the rtc time
 * 
 * @param void
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_rtc_read(void)
{
	return system("hwclock -r");
}

/**
 * @brief synchronize the system time to the rtc
 * 
 * @param void
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_sync_to_rtc(void)
{
	return system("hwclock -w");
}

/**
 * @brief synchronize the system time from the rtc
 * 
 * @param void
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_sync_from_rtc(void)
{
	return system("hwclock -s");
}

/**
 * @brief check and sync the system time to the rtc
 * 
 * @param void
 * @return int: 0->complete   !0->error
 */
static int ak_drv_sync_check(void)
{
	int retval = AK_SUCCESS;
	/* sync to hardware */
	if(system("hwclock -w")) {
		/* set system time */
		ak_print_notice("set the system time to 2021-1-1 00:00:00\n");
		retval |= system("date -s \"2021-1-1 00:00:00\"");
		/* sync to hardware */
		retval |= system("hwclock -w");
	}
	return retval;
}

/**
 * @brief set up a relative-time wakeup
 * 
 * @param s: how many seconds will it wakeup
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_alarm_wakeup_s(int s)
{
	char buffer[128];
	int retval = AK_SUCCESS;

	if(s <= 1) {
		ak_print_error("time must be greater than 1 second!\n");
		return AK_FAILED;
	}

	/* add the seconds 1 in the command rtcwake, so drop 1 here */
	s--;

	snprintf(buffer, sizeof(buffer), "rtcwake -m standby -s %d", s);

	/* check */
	retval = ak_drv_sync_check();
	if(!retval) {
		/* entry standby */
		retval = system(buffer);
	} else {
		retval = AK_FAILED;
	}
	
	return retval;
}

/**
 * @brief set up a fixed-time wakeup
 * 
 * @param time: set a fixed time
 * @return int: AK_SUCCESS or AK_FAILED
 */
int ak_drv_alarm_wakeup_t(const char *time)
{
	char buffer[128];
	int retval = AK_SUCCESS;

	snprintf(buffer, sizeof(buffer), "rtcwake -m standby -t `date -d \"%s\" +%%s`", time);

	/* check */
	retval = ak_drv_sync_check();
	if(!retval) {
		/* entry standby */
		retval = system(buffer);
	} else {
		retval = AK_FAILED;
	}

	return retval;
}
