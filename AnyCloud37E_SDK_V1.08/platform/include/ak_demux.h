#ifndef _AK_DEMUX_H_
#define _AK_DEMUX_H_

#include "ak_common_video.h"
#include "ak_common_audio.h"
//#include "ak_venc.h"

#define LEN_PATH_FILE           256
#define LEN_DATETIME            16

#define OFFSET_MONTH            4
#define OFFSET_DAY              6
#define LEN_YEAR                4
#define LEN_MONTH               2
#define LEN_DAY                 2

#define OFFSET_MIN              2
#define OFFSET_SEC              4
#define LEN_HOUR                2
#define LEN_MIN                 2
#define LEN_SEC                 2

#define TIMEZONE                28800 //8*3600

enum ak_demux_error_type
{
    ERROR_DEMUX_OPEN_LIB_ERROR = (MODULE_ID_DEMUX << 24) + 0,
    ERROR_DEMUX_CLOSE_LIB_ERROR,
    ERROR_DEMUX_GET_LIB_INFO_ERROR,
    ERROR_DEMUX_START_LIB_ERROR,
    ERROR_DEMUX_STOP_LIB_ERROR,
    ERROR_DEMUX_OPEN_TOO_MANY,
    ERROR_DEMUX_USER_NULL,
    ERROR_DEMUX_LIB_HANDLE_NULL,
    ERROR_DEMUX_AUDIO_TYPE_ERROR,
    ERROR_DEMUX_VIDEO_TYPE_ERROR,
    ERROR_DEMUX_AUDIO_END,
    ERROR_DEMUX_VIDEO_END,
    ERROR_DEMUX_LIB_ERROR,
    ERROR_DEMUX_NO_AUDIO,
    ERROR_DEMUX_NO_VIDEO,
    ERROR_DEMUX_SRC_EOF,
};

enum ak_demux_data_type
{
	DEMUX_DATA_UNKNOW = 0x00,
	DEMUX_DATA_VIDEO,
	DEMUX_DATA_AUDIO
};

enum ak_demux_direction
{
	DEMUX_DIRECTION_NEAREST = 0,	//seek to nearest key frame
	DEMUX_DIRECTION_LAST,			//seek to last key frame
	DEMUX_DIRECTION_NEXT			//seek to next key frame
};

/* demux media data source */
enum ak_demux_data_source
{
	DEMUX_DATA_FROM_UNKNOWN = 0x00,
	DEMUX_DATA_FROM_LOCAL_FILE,
	DEMUX_DATA_FROM_INTERNET
};

struct ak_demux_input
{
    void *from_dir;// file_fd or socket
    enum ak_demux_data_source data_source;
    unsigned long start_ms;
};

struct ak_demux_stream
{
	unsigned char *data;	// demux stream data
	unsigned int len; 		// demux stream len in bytes
	unsigned long long ts;	// demux timestamp(ms)
	enum video_frame_type frame_type;	// demux I or P frame
};

struct ak_demux_video_info
{
	unsigned int fps;
	int video_type;// enum encode_output_type
	unsigned short width;
	unsigned short height;
};

struct ak_demux_audio_info
{
    struct ak_audio_data_attr audio_data_attr;
    enum ak_audio_type audio_type; //encode/decode type
};

/**
 * ak_demux_get_version - get demux version
 * return: version string
 * notes:
 */
const char* ak_demux_get_version(void);

/**
 * ak_demux_open - open demux lib
 * @demux_param[IN]: demux input param
 * @demux_handle_id[OUT]: demux handle id
 * return: 0 success, other failed
 */
int ak_demux_open(const struct ak_demux_input *demux_param, int *demux_handle_id);

/**
 * ak_demux_get_total_time: get media  total time
 * @demux_handle_id[IN]: opened demux handle id
 * @total_time_ms[OUT]: media total time(ms)
 * return: 0 on succeess, other failed
 */
int ak_demux_get_total_time(int demux_handle_id, unsigned long *total_time_ms);

/**
 * ak_demux_get_video_info: get media  video information
 * @demux_handle_id[IN]: opened demux handle id
 * @demux_video_info[OUT]: store video info
 * return: 0 on succeess, other failed
 */
int ak_demux_get_video_info(int demux_handle_id, struct ak_demux_video_info *demux_video_info);

/**
 * ak_demux_get_audio_info: get media  audio information
 * @demux_handle_id[IN]: opened demux handle id
 * @demux_audio_info[OUT]: store audio info
 * return: 0 on succeess, other failed
 */
int ak_demux_get_audio_info(int demux_handle_id, struct ak_demux_audio_info *demux_audio_info);

/**
 * ak_demux_get_data - mux start
 * @demux_handle_id[IN]: opened mux handle id
 * @type[IN]: the type of data to get
 * @stream[OUT]: the stream of the type data
 * return: 0 success, other failed
 */
int ak_demux_get_data(int demux_handle_id, enum ak_demux_data_type *type, struct ak_demux_stream *stream);

/**
 * ak_demux_get_video - get video data after demux
 * @demux_handle_id[IN]: opened demux handle
 * @stream[OUT]:  video data stream
 * return: the stream packet after demux, NULL failed
 */
int ak_demux_get_video(int demux_handle_id, struct ak_demux_stream *stream);

/**
 * ak_demux_get_audio - get audio data after demux
 * @demux_handle_id[IN]: opened demux handle
 * @stream[OUT] : audio data stream
 * return: the stream packet after demux, NULL failed
 */
int ak_demux_get_audio(int demux_handle_id, struct ak_demux_stream *stream);

/**
 * ak_demux_free_data - demux free data
 * @demux_handle_id[IN]: opened mux handle id
 * @stream[IN]         : stream pointer to be free
 * return: 0 success, other failed
 */
int ak_demux_free_data(int demux_handle_id, struct ak_demux_stream *stream);

/**
 * ak_demux_close - close demux lib
 * @demux_handle_id[IN]: opened mux handle
 * return: 0 success, other failed
 */
int ak_demux_close(int demux_handle_id);

int ak_demux_seek_to_keyframe(int demux_handle_id, unsigned long ms,
                                    enum ak_demux_direction direct);
#endif

