#ifndef _AK_AUDIO_ENCODE_H_
#define _AK_AUDIO_ENCODE_H_

#include "ak_common_audio.h"

enum aenc_aac_attr
{
    AENC_AAC_RESERVED = 0,
    AENC_AAC_SAVE_FRAME_HEAD,
    AENC_AAC_CUT_FRAME_HEAD
};

enum ak_aenc_error_type 
{
    ERROR_AENC_OPEN_LIB_ERROR = (MODULE_ID_ADEC << 24) + 0,
    ERROR_AENC_CLOSE_LIB_ERROR,
    ERROR_AENC_OPEN_TOO_MANY,
    ERROR_AENC_NOT_SUPPORT_TYPE,
    ERROR_AENC_INVALID_PARAM,
    ERROR_AENC_USER_NULL,
    ERROR_AENC_CALC_FRAME_LEN_ERROR,
    ERROR_AENC_LIST_CNT_TOO_MANY,
    ERROR_AENC_BUF_EXIST,
    ERROR_AENC_WRONG_OFFSET,
    ERROR_AENC_RB_ERROR,
    ERROR_AENC_NO_DATA,
    ERROR_AENC_INVALID_FRAME_LEN,
    ERROR_AENC_AUDIO_CODEC_ERROR
};

/* audio encode attr */
struct aenc_attr 
{
    enum aenc_aac_attr aac_head;    //AAC head attr
};

struct aenc_param 
{    
    struct ak_audio_data_attr aenc_data_attr;
    enum ak_audio_type type;    //encode/decode type
};

/** 
 * ak_aenc_print_codec_info - print audio codec version
 * notes: no suggest to use
 */
void ak_aenc_print_codec_info(void);

/**
 * ak_aenc_get_version - get audio encode version
 * return: version string
 * notes: 
 */
const char* ak_aenc_get_version(void);

/**
 * ak_aenc_open - open anyka audio encode
 * @param[IN]: audio input data encode param
 * @aenc_handle_id[OUT]: aenc handle id number
 * return: 0 success, other failed
 * notes: 
 */
int ak_aenc_open(const struct aenc_param *param, int *aenc_handle_id);

/**
 * ak_aenc_set_attr - set aenc attribution after open
 * @aenc_handle_id[IN]: opened encode handle id
 * @attr[IN]: audio encode attribution
 * return: 0 success, other failed
 */
int ak_aenc_set_attr(int aenc_handle_id, const struct aenc_attr *attr);

/**
 * ak_aenc_send_frame - send pcm data(frame) to encode 
 * @aenc_handle_id[IN]: opened encode handle id
 * @pcm_frame[IN]: the audio pcm raw data info 
 * @block[IN]: block = 1,block mode;  block = 0, non block mode
 * return: 0 success, other failed 
 * notes: 
 */
int ak_aenc_send_frame(int aenc_handle_id, const struct frame *pcm_frame, unsigned char block);

/**
 * ak_aenc_get_stream - get audio encoded data, stream
 * @aenc_handle_id[IN]: opened encode handle id
 * @stream[OUT]: audio stream after encode
 * @block[IN]: block = 1,block mode;  block = 0, non block mode
 * return: 0 success, other failed
 * notes: 
 */
int ak_aenc_get_stream(int aenc_handle_id, struct audio_stream *stream, unsigned char block);

/**
 * ak_aenc_release_stream -  release audio data stream 
 * @aenc_handle_id[IN]: opened encode handle id
 * @stream[IN]: audio stream entry from ak_aenc_get_stream
 * return: 0 success, other failed
 * notes: 
 */
int ak_aenc_release_stream(int aenc_handle_id, struct audio_stream *stream);

/**
 * ak_aenc_clear_encode_buf -  clear  audio encode bufffer
 * @aenc_handle_id[IN]: opened encode handle id
 * return: 0 success, other failed
 * notes: 
 */
int ak_aenc_clear_encode_buf(int aenc_handle_id);

/**
 * ak_aenc_print_runtime_status - print run time status
 * @aenc_handle_id[IN]: opened encode handle id
 * return: 0 success, other failed
 */
int ak_aenc_print_runtime_status(int aenc_handle_id);

/**
 * ak_aenc_close - close audio encode
 * @aenc_handle_id[IN]: opened encode handle id 
 * return: 0 success, other failed
 */
int ak_aenc_close(int aenc_handle_id);

/**
 * ak_aenc_save_file - save two file, one file is stream input to aenc, two file is stream output from aenc
 * @aenc_handle_id[IN]: opened audio encode handle id
 * @save_file_info[IN]: save file infomation
 * return: 0 success, other failed
 * notes: 
 */
int ak_aenc_save_file(int aenc_handle_id, struct ak_audio_save_file_info *save_file_info);

/**
 * ak_aenc_get_codec_info - get audio code lib version
 * return: audio code lib version
 * notes:
 */
char *ak_aenc_get_codec_info(void);

#endif
