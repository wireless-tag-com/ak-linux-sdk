#ifndef _AK_AUDIO_DECODE_H_
#define _AK_AUDIO_DECODE_H_

#include "ak_common_audio.h"

struct adec_aac_dec_param
{
    unsigned short aac_stream_format;// 0: auto detect; 1: adts; 2: adif; 3: raw
};

enum ak_adec_error_type 
{
    ERROR_ADEC_OPEN_LIB_ERROR = (MODULE_ID_ADEC << 24) + 0,
    ERROR_ADEC_CLOSE_LIB_ERROR,
    ERROR_ADEC_OPEN_TOO_MANY,
    ERROR_ADEC_INVALID_PARAM,
    ERROR_ADEC_USER_NULL,
    ERROR_ADEC_RB_ERROR,
    ERROR_ADEC_ATC_LIB_ERROR,
    ERROR_ADEC_SEND_STREAM_ERROR,
    ERROR_ADEC_GET_FRAME_NO_DATA,
    ERROR_ADEC_SEND_STREAM_NO_SPACE,
    ERROR_ADEC_NO_DECODE_DATA,
    ERROR_ADEC_STREAM_END,
    ERROR_ADEC_SEND_STREAM_NO_START
};

struct adec_param 
{
    struct ak_audio_data_attr adec_data_attr;
    enum ak_audio_type type;                    //encode/decode type
};

struct ak_dec_buf_status
{
    int buf_total_len;           // decode total buf 
    int buf_remain_len;         //decode remain buf

};

/**
 * ak_adec_get_version - get audio decode version
 * return: version string
 * notes:
 */
const char* ak_adec_get_version(void);

/**
 * ak_adec_open - open audio decode
 * @param[IN]: adec open params 
 * @adec_handle_id[OUT]: adec handle id number
 * return: 0 success; other failed
 * notes: 
 */
int ak_adec_open(const struct adec_param *param, int *adec_handle_id);

/**
 * ak_adec_set_output_pcm_len - set output pcm data length
 * @adec_handle_id[IN]: adec handle id number
 * @out_pcm_len[IN]: set output pcm data length
 * return: 0 success; other failed
 */
int ak_adec_set_output_pcm_len(int adec_handle_id, unsigned int out_pcm_len);

/**
 * ak_adec_send_stream - send decode audio stream
 * @adec_handle_id[IN]: adec handle id number
 * @data[IN]: data add to decode buffer
 * @len[IN]: add to decode buffer data len
 * @block[IN]: =1 block mode; =0 non-block mode
 * return: 0 success; other failed
 */
int ak_adec_send_stream(int adec_handle_id, const unsigned char *data,
                                unsigned int len, unsigned char block);

/**
 * ak_adec_get_frame -get pcm data after decode
 * @adec_handle_id[IN]: adec handle id number
 * @pcm_frame[OUT]: audio decode frame out buffer 
 * @block[IN]: =1 block mode; =0 non-block mode
 * return: 0 success; other failed
 */
int ak_adec_get_frame(int adec_handle_id, struct frame *pcm_frame, unsigned char block);

/**
 * ak_adec_release_frame -release frame data
 * @adec_handle_id[IN]: adec handle id number
 * @pcm_frame[IN]: audio decode frame out buffer 
 * return: 0 success; other failed
 */
int ak_adec_release_frame(int adec_handle_id, struct frame *pcm_frame);

/**
 * ak_adec_send_stream_end - notice send stream end
 * @adec_handle_id[IN]: adec handle id number
 * return: 0 success; other failed
 * note: when play data send end,use this function
 */
int ak_adec_send_stream_end(int adec_handle_id);

/**
 * ak_adec_clear_decode_buf - clear decode buffer
 * @adec_handle_id[IN]: adec handle id number
 * return: 0 success; other failed
 * note: 
 */
int ak_adec_clear_decode_buf(int adec_handle_id);

/**
 * ak_adec_print_runtime_status - close anyka audio decode
 * @adec_handle_id[IN]: adec handle id number
 * return: 0 success; other failed
 */
int ak_adec_print_runtime_status(int adec_handle_id);

/**
 * ak_adec_close - close anyka audio decode
 * @adec_handle_id[IN]: adec handle id number
 * return: 0 success; other failed
 */
int ak_adec_close(int adec_handle_id);

/**
 * ak_adec_save_file - save 4 files, one file is stream input to adec, two file is pcm output from adec, three is stream before decode, four is pcm after decode
 * @adec_handle_id[IN]: adec handle id number
 * @save_file_info[IN]: save file infomation
 * return: 0 success, other failed
 * notes: 
 */
int ak_adec_save_file(int adec_handle_id, struct ak_audio_save_file_info *save_file_info);

/**
 * ak_adec_get_codec_info - print audio code lib version
 * return: audio code lib version
 * notes: 
 */
char *ak_adec_get_codec_info(void);

/**
 * ak_adec_get_buf_status - audio output get device buffer status
 * @ao_handle_id[IN]: adec opened handle id
 * @buf_status[OUT]: get decode buffer total length and decode buffer remain length
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_adec_get_buf_status(int adec_handle_id, struct ak_dec_buf_status *buf_status);

/**
 * ak_adec_set_aac_dec_param - audio decode set aac decode parameter
 * @ao_handle_id[IN]: adec opened handle id
 * @dec_param[IN]: get decode buffer total length and decode buffer remain length
 * return: 0 success, otherwise failed
 * notes: this function will reopen audio decode lib, must use before ak_adec_send_stream
 */
int ak_adec_set_aac_dec_param(int adec_handle_id, struct adec_aac_dec_param *dec_param);


#endif
