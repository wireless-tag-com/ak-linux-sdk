#ifndef _AK_VIDEO_INPUT_H_
#define _AK_VIDEO_INPUT_H_

typedef int VI_DEV;
typedef int VI_CHN;

#define VIDEO_DEV0	    0
#define VIDEO_DEV1	    1
#define VIDEO_DEV_MUX	2


enum ak_vi_chn_enum{
	VIDEO_CHN0	=	0,
	VIDEO_CHN1,
	VIDEO_CHN2,
	VIDEO_CHN3,
	VIDEO_CHN_TOTAL,
	VIDEO_CHN16 = 16,
	VIDEO_CHN17
};

enum video_channel {
	VIDEO_CHN_MAIN,
	VIDEO_CHN_SUB,
	VIDEO_CHN_NUM
};

/* vi module errno list */
enum ak_vi_errno {                                                            //错误代码
	ERROR_VI_INVALID_DEVID					= ( MODULE_ID_VI << 24 ) + 0,
	ERROR_VI_DEVID_NOT_OPEN					= ( MODULE_ID_VI << 24 ) + 1,
	ERROR_VI_DEVID_NOT_MATCH				= ( MODULE_ID_VI << 24 ) + 2,
	ERROR_VI_ATTR_PARM_WRONG				= ( MODULE_ID_VI << 24 ) + 3,
	ERROR_VI_INVALID_CHNID					= ( MODULE_ID_VI << 24 ) + 4,
	ERROR_VI_DEV_NOT_ENABLE					= ( MODULE_ID_VI << 24 ) + 5,
	ERROR_VI_DEV_NOT_DISABLE				= ( MODULE_ID_VI << 24 ) + 6,
	ERROR_VI_DEV_REGISTER_FAIL				= ( MODULE_ID_VI << 24 ) + 7,
	ERROR_VI_CHANNEL_REGISTER_FAIL			= ( MODULE_ID_VI << 24 ) + 8,
	ERROR_VI_CHANNEL_DEVICE_NOT_OPEN		= ( MODULE_ID_VI << 24 ) + 9,
	ERROR_VI_CHANNEL_NOT_EXIST				= ( MODULE_ID_VI << 24 ) + 10,
	ERROR_VI_CHANNEL_NOT_ENABLE				= ( MODULE_ID_VI << 24 ) + 11,
	ERROR_VI_CHANNEL_NOT_DISABLE			= ( MODULE_ID_VI << 24 ) + 12,
	ERROR_VI_CHANNEL_ID_NOT_MATCH			= ( MODULE_ID_VI << 24 ) + 13,
};

typedef enum AK_VI_DATA_PATH {
	VI_PATH_BYPASS = 0,								//ISP is bypass
	VI_PATH_ISP,									//ISP enable
	VI_PATH_RAW,									//capture raw data, for debug
	VI_PATH_TOTAL
}VI_DATA_PATH_E;

typedef enum AK_VI_DATA_TYPE {
	VI_DATA_TYPE_YUV420SP = 0,						//NV12 (YUV420SP)
	VI_DATA_TYPE_YUV420P,							//I420 (YUV420P)  
	VI_DATA_TYPE_RGB ,								//RGB Data
	VI_DATA_TYPR_RAW,								//Raw Data
	VI_DATA_TYPE_TOTAL
}VI_DATA_TYPE_E;

typedef enum AK_VI_INTF_MODE {
	VI_INTF_DVP = 0,								//DVP 并行接口
	VI_INTF_MIPI_1,									//MIPI串行1线接口
	VI_INTF_MIPI_2,									//MIPI串行2线接口
	VI_INTF_TOTAL
}VI_INTF_MODE_E;

enum ak_vi_daynight_mode {
	VI_MODE_DAY_OUTDOOR,
	VI_MODE_NIGHTTIME,
	VI_MODE_USR_DEFINE_1,
	VI_MODE_USR_DEFINE_2,
	VI_MODE_USR_DEFINE_3,
	VI_MODE_NUM
};

struct crop_info {
	int		left;	// x position of crop
	int		top;	// y position of crop
	int		width;	// width of crop
	int		height;	// height of crop
};

typedef struct AK_VI_DEV_ATTR{
	VI_DEV 				dev_id;						//vi设备号
	VI_INTF_MODE_E 		interf_mode;				//interface mode, dvp/mipi1/mipi2
	int					fd;							//vi设备对应的设备文件描述符
	VI_DATA_PATH_E		data_path;					//数据路径，1为启用ISP
	VI_DATA_TYPE_E		data_type;					//数据类型，RGB/ YUV
	struct crop_info	crop;						//crop info of device	
	int 				max_width;					//max value of width of main chn resolution
	int					max_height;					//max value of height of main chn resolution
	int					frame_rate;					//source frame_rate of the ISP 
	int 				sub_max_width;				//max value of width of sub chn resolution
	int					sub_max_height;				//max value of height of sub chn resolution
}VI_DEV_ATTR;

typedef struct AK_VI_RECTANGLE{
	int		width;									//width lenght of resolution
	int		height;									//height lenght of resolution
}RECTANGLE_S;

typedef struct AK_VI_CHN_ATTR {
	VI_CHN			chn_id;							//VI 通道号。
	int				frame_rate;						//当前通道的目标帧率
	RECTANGLE_S		res;							//目标分辨率
	int				frame_depth;					//frame buffer number of the channel
}VI_CHN_ATTR;

struct video_input_frame {
	struct frame vi_frame;
	unsigned long phyaddr;
	int	type;
	void *mdinfo;
};

/* ak_vi_get_version
 * return the vi lib version
 */
const char* ak_vi_get_version(void);

/**
 * ak_vi_open: open video input device
 * @dev_id[IN]: video input device ID
 * return: AK_SUCCESS if successful, error code if failed
 * notes:
 */
int  ak_vi_open(VI_DEV dev_id);

/**
 * ak_vi_close: close video input device
 * @dev_id[IN]: video input device ID
 * return: AK_SUCCESS if successful, error code if failed
 * notes:
 */
int  ak_vi_close(VI_DEV dev_id);

/* ak_vi_load_sensor_cfg 	-- load isp config file 
 * dev_id[IN]		: 	video input device ID
 * config_file[IN]	:	config file path 
 * return AK_SUCCESS if success, Error Code if failed
 */
int ak_vi_load_sensor_cfg(VI_DEV dev_id, const char *config_file);

/* ak_vi_get_sensor_resolution	---	 get sensor resolution by device id
 * dev_id[IN]		: 	video input device ID
 * res[OUT]			:	pointer to record the resolution of vi device 	
 * return AK_SUCCESS if success, AK_FAILED if failed
 */
int ak_vi_get_sensor_resolution(VI_DEV dev_id, RECTANGLE_S *res);

/* ak_vi_set_dev_attr -- set dev basic attr
 * dev_id[IN] : 	video input device ID
 * dev_attr[IN]   :		video ipnout device attribution
 * return AK_SUCCESS if successful, error code if failed
 */
int ak_vi_set_dev_attr(VI_DEV dev_id,  VI_DEV_ATTR *dev_attr);

/* ak_vi_get_dev_attr -- set dev basic attr
 * dev_id[IN] : 	video input device ID
 * dev_attr[OUT]   :		video ipnout device attribution
 * return AK_SUCCESS if successful, error code if failed
 */
int ak_vi_get_dev_attr(VI_DEV dev_id, VI_DEV_ATTR *dev_attr);

/* ak_vi_set_chn_attr	--	set channel attribute
 * chn_id[IN]			: 		channal id
 * chn_attr[IN]			:		channel attribute
 */
int ak_vi_set_chn_attr(VI_CHN chn_id, VI_CHN_ATTR *chn_attr);

/* ak_vi_get_chn_attr	--	get channel attribute
 * chn_id[IN]			: 		channal id
 * chn_attr[OUT]			:		channel attribute
 * return AK_SUCCESS if successful, error code if failed
 */
int ak_vi_get_chn_attr(VI_CHN chn_id, VI_CHN_ATTR *chn_attr);

/* ak_vi_enable_dev  --		enable vi device to start capture
 * dev_id[IN]			: 		vi device id
 * return AK_SUCCESS if success, error code if failed
 * */
int ak_vi_enable_dev(VI_DEV dev_id);

/* ak_vi_disable_dev  --		disable vi device to start capture
 * dev_id[IN]			: 		vi device id
 * return AK_SUCCESS if success, error code if failed
 * */
int ak_vi_disable_dev(VI_DEV dev_id);

/* ak_vi_enable_chn  --		enable vi channel to start capture
 * chn_id[IN]			: 		vi channel id
 * return AK_SUCCESS if success, error code if failed
 * */
int ak_vi_enable_chn(VI_CHN chn_id);

/* ak_vi_disable_chn	  --		enable vi channel to start capture
 * chn_id[IN]			: 		vi channel id
 * return AK_SUCCESS if success, error code if failed
 * */
int ak_vi_disable_chn(VI_CHN chn_id);

/**
 * ak_vi_get_frame: get frame
 * chn_id[IN]	:	channel id 
 * @frame[OUT]: store frames
 * return: 0 success, otherwise failed
 */
int ak_vi_get_frame(VI_CHN chn_id, struct video_input_frame *frame);

/**
 * ak_vi_release_frame:  release  frame
 * chn_id	[IN]:	channel id 
 * @frame[IN]:  pointer to vi frames
 * return: AK_SUCCESS if  success, error code if failed
 */
int ak_vi_release_frame(VI_CHN chn_id, struct video_input_frame *frame);

/* ak_vi_change_chn_fps	--	change channel fps
 * chn_id[IN]			: 		channal id
 * frame_rate[IN]		:		frame_rate
 */
int ak_vi_change_chn_fps(VI_CHN chn_id, int frame_rate);

/**
 * brief: switch day night mode
 * @dev[IN]: device id
 * @mode[IN]:day or night
 * return: 0 success, otherwise error code;
 * notes:
 */
int ak_vi_switch_mode(VI_DEV dev_id, enum ak_vi_daynight_mode mode);

/*
 *  brief:  get sensor id
 * @dev[IN]: device id
 * ak_vi_get_sensor_id 
 * id[OUT]: sensor id
 * return: 0 success, -1 failed
 */
int ak_vi_get_sensor_id(int dev, int *id);

/* ak_vi_set_cfg_file_path 	-- load isp config file 
 * dev_id[IN]		: 	video input device ID
 * config_file[IN]	:	config file path 
 * return AK_SUCCESS if success, Error Code if failed
 */
int ak_vi_set_cfg_file_path(VI_DEV dev_id, const char *config_file);

#endif
