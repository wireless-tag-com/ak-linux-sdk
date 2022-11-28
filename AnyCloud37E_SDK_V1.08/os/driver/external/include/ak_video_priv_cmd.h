#ifndef __AK_VIDEO_PRIV_CMD_H__
#define __AK_VIDEO_PRIV_CMD_H__

#define MD_HEADER_SIZE	128
#define MD_MAGIC		0xdba8f04e
#define MD_VERSION		0x0000

/*rawdata*/
#define RAWDATA_HEADER_SIZE	128
#define RAWDATA_HEADER_MAGIC	0x52415744;/*RAWD*/

enum ak_video_priv_cmd {
	/*SET commad start*/
	SET_SUB_CHN_RESOLUTION = 0x100,
	SET_CHNS_AT_SAME_TIME,
	SET_CAPTURE_RAWDATA,

	/*GET commad start*/
	GET_SENSOR_ID = 0x300,
	GET_SENSOR_CB,
	GET_PHYADDR,
	GET_MAX_EXP_FOR_FPS,
};

enum rawdata_format {
	BAYER_RAWDATA = 0,
	YUV422_16B_DATA,
};

/*
 * struct vb2_buffer->field
 * struct vb2_v4l2_buffer->field
 */
enum field_define {
	NORMAL_FRAME = 0,
	RAWDATA_FRAME,/*indicate is rawdata*/
};

struct priv_chn_resolution {
	int type;
	int width;
	int height;
};

struct priv_sensor_id {
	int type;
	int sensor_id;
};

struct priv_phyaddr {
	int type;
	unsigned int phyaddr;/*set index, then return phyaddr for index*/
};

struct priv_max_exp_for_fps {
	int type;
	int fps;
	int max_exp;
};

struct priv_chns_at_same_time{
	int type;
	int chns;/*bits map*/
};

/*
 * md header:
 * magic,
 * version,
 * width blocks num,
 * height blocks num,
 * md level,
 * .......(128BYTES header),
 * md data
 * */
struct md_header {
	/*total 128 BYTES*/
	unsigned int magic;//4bytes
	unsigned short version;//2bytes
	unsigned short width_block_num;//2bytes
	unsigned short height_block_num;//2bytes
	unsigned short block_size;//2bytes
	unsigned short md_level;//2bytes
	unsigned char reserved[114];//114bytes
};

/*
 * struct rawdata_header
 * @magic:		magic value
 * @header_size:	only the size of the header
 * @format:		input data format
 * @rawdata_size:	only the rawdata size exclude header
 * @bits_width:		bits width of a pixsel
 * @width:			width pixsel of the rawdata picture
 * @height:			height pixsel of the rawdata picture
 */
struct rawdata_header {
	unsigned int magic;
	int header_size;
	enum rawdata_format format;
	int rawdata_size;
	int bits_width;
	int width;
	int height;
};
#endif
