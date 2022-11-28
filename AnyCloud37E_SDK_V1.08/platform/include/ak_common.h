#ifndef _AK_COMMON_H_
#define _AK_COMMON_H_

#include <stdio.h>
#include <time.h>

#define AK_SUCCESS          0
#define AK_FAILED           -1
#define AK_FALSE			0
#define AK_TRUE          	1

/* os_adapter */
#define MODULE_NAME_MEMORY             "MEMORY"
#define	MODULE_NAME_THREAD             "THREAD"
#define	MODULE_NAME_LOG                "LOG"
#define	MODULE_NAME_COMMON             "COMMON"
#define	MODULE_NAME_IPCSRV             "IPCSRV"
#define	MODULE_NAME_AI                 "AI"
#define	MODULE_NAME_AO                 "AO"
#define	MODULE_NAME_VI                 "VI"
#define	MODULE_NAME_VO                 "VO"
#define	MODULE_NAME_TIMER              "TIMER"
#define	MODULE_NAME_VPSS               "VPSS"
#define	MODULE_NAME_DRV                "DRV"
#define MODULE_NAME_TDE                "TDE"
#define MODULE_NAME_UUID			   "UUID"
#define MODULE_NAME_OSAL               "OSAL"

/* mpi */
#define	MODULE_NAME_VENC               "VENC"
#define	MODULE_NAME_VDEC               "VDEC"
#define	MODULE_NAME_AENC               "AENC"
#define	MODULE_NAME_ADEC               "ADEC"
#define	MODULE_NAME_OSD                "OSD"
#define MODULE_NAME_MUX                "MUX"
#define MODULE_NAME_DEMUX              "DEMUX"
#define MODULE_NAME_SVP                "SVP"

/* app */
#define	MODULE_NAME_DVR                "DVR"
#define	MODULE_NAME_MD                 "MD"
#define	MODULE_NAME_GUI                "GUI"
#define	MODULE_NAME_AED                "AED"
#define MODULE_NAME_AUDIO_TOOL         "AUDIO_TOOL"
#define MODULE_NAME_VQE                "VQE"

#define	MODULE_NAME_APP_VIDEO          "APP_VIDEO"
#define	MODULE_NAME_APP_AUDIO          "APP_AUDIO"

#define MODULE_NAME_DVR_IDX            "DVR_IDX"
#define MODULE_NAME_NET                "NET"
#define MODULE_NAME_APP_MEM            "APP_MEM"
#define MODULE_NAME_APP_AUDIO          "APP_AUDIO"

/*atc*/
#define MODULE_NAME_ATC_ECHO		   "ECHO"
#define MODULE_NAME_ATC_ADEC		   "SD_ADEC"
#define MODULE_NAME_ATC_AENC		   "SD_AENC"
#define	MODULE_NAME_AKV_VENC			   "AKV_VENC"
#define	MODULE_NAME_AKV_VDEC			   "AKV_VDEC"
#define	MODULE_NAME_AKV_MUX			   "AKV_MUX"
#define	MODULE_NAME_AKV_DEMUX			   "AKV_DEMUX"

/* end */
#define MODULE_NAME_APP                "Application"
#define	MODULE_NAME_ALL                "ALL"

/**********************Module ID*******************/
/* 注意：新增模块ID只能在尾部添加，不可在中间插入 */
/**************************************************/
enum module_id {
    MODULE_ID_MEMORY                 = 0x0,
	MODULE_ID_THREAD,
	MODULE_ID_LOG,
	MODULE_ID_COMMON,
	MODULE_ID_IPCSRV,
	MODULE_ID_AI,
	MODULE_ID_AO,
	MODULE_ID_VI,
	MODULE_ID_VO,
	MODULE_ID_TIMER,
	MODULE_ID_VPSS,
	MODULE_ID_DRV,
	MODULE_ID_TDE,
	MODULE_ID_VENC,
	MODULE_ID_VDEC,
	MODULE_ID_AENC,// 15
	MODULE_ID_ADEC,
	MODULE_ID_OSD,
	MODULE_ID_MUX,
	MODULE_ID_DEMUX,
	MODULE_ID_DVR,
	MODULE_ID_MD,
	MODULE_ID_GUI,
	MODULE_ID_AED,
	MODULE_ID_AUDIO_TOOL,

    MODULE_ID_VQE,

	MODULE_ID_APP_VIDEO,
	MODULE_ID_DVR_IDX,
	MODULE_ID_NET,
	MODULE_ID_OSAL,
	MODULE_ID_APP_MEM,
	MODULE_ID_APP_AUDIO,
	MODULE_ID_UUID,
	MODULE_ID_ATC_ECHO,
	MODULE_ID_ATC_ADEC,
	MODULE_ID_ATC_AENC,
	MODULE_ID_AKV_VENC,
	MODULE_ID_AKV_VDEC,
	MODULE_ID_AKV_MUX,
	MODULE_ID_AKV_DEMUX,
	MODULE_ID_SVP,
    /* 新增模块ID，请在该行之上添加 */
	MODULE_ID_APP                     = 0xFE,
	MODULE_ID_ALL                     = 0xFF,
};

enum ak_error_type {
	ERROR_TYPE_NO_ERROR            = 0,
	ERROR_TYPE_POINTER_NULL        = 0x100,
	ERROR_TYPE_INVALID_ARG,
	ERROR_TYPE_MALLOC_FAILED,
	ERROR_TYPE_PMEM_MALLOC_FAILED,
	ERROR_TYPE_FUNC_NOT_SUPPORT,		//5
	ERROR_TYPE_NOT_INIT,
	ERROR_TYPE_CALL_ORDER_WRONG,
	ERROR_TYPE_DEV_OPEN_FAILED,
	ERROR_TYPE_DEV_INIT_FAILED,
	ERROR_TYPE_DEV_CTRL_FAILED,			//10
	ERROR_TYPE_DEV_READ_FAILED,
	ERROR_TYPE_DEV_WRITE_FAILED,
	ERROR_TYPE_DEV_CLOSE_FAILED,
	ERROR_TYPE_FILE_OPEN_FAILED,
	ERROR_TYPE_FCNTL_FAILED,
	ERROR_TYPE_FILE_READ_FAILED,		//15
	ERROR_TYPE_FILE_WRITE_FAILED,
	ERROR_TYPE_IOCTL_FAILED,
	ERROR_TYPE_CLOSE_FAILED,
	ERROR_TYPE_GET_V4L2_PTR_FAILED,
	ERROR_TYPE_INVALID_USER,
	ERROR_TYPE_THREAD_CREATE_FAILED,	//20
	ERROR_TYPE_POPEN_FAILED,
	ERROR_TYPE_MEDIA_LIB_FAILED,
	ERROR_TYPE_NO_DATA,
	ERROR_TYPE_EBUSY,
	ERROR_TYPE_NO_ENOUGH_RESOURCE,
	ERROR_TYPE_NO_LEFT_SPACE,

	/* for other module*/
	ERROR_TYPE_NUM
};

enum sdk_run_mode {
	SDK_RUN_NORMAL,
	SDK_RUN_DEBUG
};

/* audio/video data frame */
struct frame {
	unsigned char *data;	//frame data
	unsigned int len;		//frame len in bytes
	unsigned long long ts;	//timestamp(ms)
	unsigned long seq_no;	//current frame sequence no.
};

struct ak_timeval {
	unsigned long sec;     /* seconds */
	unsigned long usec;    /* microseconds */
};

struct ak_date {
	int year; 		//4 number
	int month;		//0-11
	int day; 		//0-30
	int hour; 		//0-23
	int minute; 	//0-59
	int second; 	//0-59
	int timezone; 	//local time zone 0-23
};

typedef struct _sdk_run_config {
	int mem_trace_flag;
	int dma_mem_trace_flag;
	int audio_tool_server_flag;       // audio tool server flag, 0 : disable, 1 : enable
	int isp_tool_server_flag;         // isp tool server flag, 0 : disable, 1 : enable
} sdk_run_config;

/**
 * ak_common_get_version - get common module version
 * return: version string
 */
const char* ak_common_get_version(void);

/**
 * ak_sleep_ms - sleep certain time
 * @ms[IN]: milli-seconds
 * return: none
 */
void ak_sleep_ms(const int ms);

/**
 * ak_get_ostime - get OS time since cpu boot
 * @tv[OUT]: time value since startup
 * return: void
 */
void ak_get_ostime(struct ak_timeval *tv);

/**
 * ak_get_os_timestamp - get OS time stamp
 * return: time stamp
 */
long ak_get_os_timestamp(void);

/**
 * ak_get_localdate - get local date time
 * @date[OUT]: get date time
 * return: AK_SUCCESS or error_code
 * notes:
 */
int ak_get_localdate(struct ak_date *date);

/**
 * ak_set_localdate - set local date time
 * @date[IN]: set date time
 * return: AK_SUCCESS or error_code
 */
int ak_set_localdate(const struct ak_date *date);

/**
 * ak_diff_ms_time - diff value of ms time between cur_time and pre_time
 * @cur_time[IN]: current time
 * @pre_time[IN]: previous time
 * return: diff time, uint: ms
 */
long ak_diff_ms_time(const struct ak_timeval *cur_time,
					const struct ak_timeval *pre_time);

/**
 * ak_seconds_to_string - transfer total seconds to readable time string
 * @secs: passed total seconds from 1970-01-01 00:00:00
 * return: readable time string after transferred
 * notes: string format: yyyy-MM-dd HH:mm:ss, ex: 2016-04-06 10:06:06
 *      time_str min len 20 bytes.
 * IMPORTANT: use ONLY ONCE after call this function.
 */
char* ak_seconds_to_string(time_t secs);

/**
 * ak_seconds_to_data - transfer seconds to date time value
 * @seconds[IN]: seconds from 1970-01-01 00:00:00
 * @date[OUT]: date time value
 * return: 0 success; error code when failed
 * notes: seconds from 1970-01-01 00:00:00 +0000(UTC)
 */
int ak_seconds_to_date(long seconds, struct ak_date *date);

/**
 * ak_date_to_seconds - transfer date time value to seconds
 * @date[IN]: date time value
 * return: seconds after transferred; failed -1
 * notes: seconds from 1970-01-01 00:00:00 +0000(UTC)
 */
long ak_date_to_seconds(const struct ak_date *date);

/**
 * ak_date_to_string - transfer date time value to time string
 * @date[IN]: date time value
 * @str[OUT]: date time string after transfer OK
 * return: return AK_SUCCESS or error_code
 * notes: 1. string format: yyyyMMdd-HHmmss, ex: 20160406-100606
 *		2. MAKE SURE str has enough space outside
 */
int ak_date_to_string(const struct ak_date *date, char *str);

/**
 * ak_string_to_date - transfer date time string to date time value
 * @time_str[IN]: time string
 * @date[OUT]: date time value after transfer OK
 * return: return AK_SUCCESS or error_code
 * notes: string format: yyyyMMdd-HHmmss, ex: 20160406-100606
 */
int ak_string_to_date(const char *time_str, struct ak_date *date);

/**
 * ak_open - open file
 * @pathname[IN]: path
 * @flags[IN]: flag
 * return: return AK_SUCCESS or error_code
 * notes: 
 */
int ak_open(const char *pathname, int flags);

/**
 * ak_sdk_init - SDK init
 * @config[IN]: sdk run config
 * return: return AK_SUCCESS or error_code
 */
int ak_sdk_init(sdk_run_config *config);

/**
 * ak_sdk_exit - SDK exit
 * @void: void
 * return: return AK_SUCCESS or error_code
 */
int ak_sdk_exit(void);

#endif

/* end of file */
