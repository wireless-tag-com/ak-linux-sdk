#ifndef _AK_AI_H_
#define _AK_AI_H_

#include "ak_common.h"
#include "ak_common_audio.h"

enum ak_ai_error_type 
{
    ERROR_AI_OPEN_LIB_ERROR = (MODULE_ID_AI << 24) + 0,
    ERROR_AI_NR_NOT_OPEN,
    ERROR_AI_RESET_RB_ERROR,    // reset ring buffer error
    ERROR_AI_DEV_ID_NOT_SUPPORT,
    ERROR_AI_DEV_HANDLE_NULL,
    ERROR_AI_GAIN_ERROR,
    ERROR_AI_CAPTURE_START_ALREADY,
    ERROR_AI_CAPTURE_NO_START,
    ERROR_AI_NOT_RUNNING,
    ERROR_AI_RING_BUF_RETURN_ERROR,
    ERROR_AI_DEV_READ_ERROR,
    ERROR_AI_FRAME_SIZE_NOT_SET,
    ERROR_AI_DEV_ALREADY_OPEN,
    ERROR_AI_DRIVER_FAILED,
    ERROR_AI_MUTE_IS_ON,
    ERROR_AI_ASLC_ERROR,
    ERROR_AI_EQ_ERROR,
    ERROR_AI_NO_DATA,
    ERROR_AI_RB_NOT_ENOUGH_DATA,
    ERROR_AI_RB_READ_NOT_ENOUGH,
    ERROR_AI_DEV_INVALID_FD,
    ERROR_AI_CLOSE_FILTER_LIB_ERROR,
    ERROR_AI_THREAD_CREATE_FAILED,
    ERROR_AI_VQE_ERROR
};

enum ak_ai_source 
{
    AI_SOURCE_MIC,        //set adc mic source manually
    AI_SOURCE_LINEIN    //set adc linein source manually
};

/* audio output param */
struct ak_audio_in_param 
{
    struct ak_audio_data_attr pcm_data_attr;
    int dev_id;// see  enum ak_audio_ai_dev_id
};


/** 
 * ak_ai_print_filter_info - print audio filter version & support functions
 * notes: 
 */
void ak_ai_print_filter_info(void);

/**
 * ak_ai_get_version - get audio in version
 * return: version string
 * notes:
 */
const char* ak_ai_get_version(void);

/**
 * ak_ai_open - open audio in device
 * @param[IN]: open audio input param, dev_id  see enum ak_audio_ai_dev_id 
 * @ai_handle_id[OUT]: audio in opened handle id
 * return: 0 success, otherwise failed
 * notes: sample_bits set 16 bit
 */
int ak_ai_open(const struct ak_audio_in_param *param, int *ai_handle_id);

/**
 * ak_ai_get_handle_id - get ai handle id
 * @dev_id[IN]: audio in device id
 * @ai_handle_id[OUT]: audio in opened handle id
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_get_handle_id(int dev_id, int *ai_handle_id);

/**
 * ak_ai_start_capture - start ai capture
 * @ai_handle_id[IN]: audio in opened handle id
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_start_capture(int ai_handle_id);

/**
 * ak_ai_stop_capture - stop ai capture
 * @ai_handle_id[IN]: audio in opened handle id
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_stop_capture(int ai_handle_id);

/**
 * ak_ai_get_frame - get audio frame
 * @ai_handle_id[IN]: audio in opened handle id
 * @pcm_frame[OUT]: audio frame info
 * @block[IN]: =1 block mode; =0 non-block mode
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_get_frame(int ai_handle_id, struct frame *pcm_frame, unsigned char block);

/**
 * ak_ai_release_frame - release one frame data
 * @ai_handle_id[IN]: audio in opened handle id
 * @pcm_frame[IN]: audio frame info
 * return: 0 success, otherwise failed
 * notes: use "ak_ai_get_frame" get frame data,after used,release frame
 */
int ak_ai_release_frame(int ai_handle_id, struct frame *pcm_frame);

/**
 * ak_ai_set_dev_buf_size - set audio device buffer size
 * @ai_handle_id[IN]: audio in opened handle id
 * @dev_buf_size[IN]: device buffer size, unit: byte
 * return: 0 success, otherwise failed
 * notes: this function is deprecated and no longer in use
 */
int ak_ai_set_dev_buf_size(int ai_handle_id, enum ak_audio_dev_buf_size dev_buf_size);

/**
 * ak_ai_get_dev_buf_size - get audio device buffer size
 *        that read from AD driver each time
 * @ai_handle_id[IN]: audio in opened handle id
 * @dev_buf_size[OUT]: device buffer size, unit: byte
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_get_dev_buf_size(int ai_handle_id, int *dev_buf_size);

/**
 * ak_ai_set_frame_length - set audio frame length, unit: byte
 * @ai_handle_id[IN]: audio in opened handle id
 * @frame_length[IN]: audio frame length, 8K mono:[512,4096], 16K mono:[1024,4096]
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_set_frame_length(int ai_handle_id, int frame_length);

/**
 * ak_ai_get_frame_length - get audio frame length, unit: byte
 * @ai_handle_id[IN]: audio in opened handle id
 * @frame_length[OUT]: audio frame length, [80, 25600], unit: byte
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_get_frame_length(int ai_handle_id, int *frame_length);

/**
 * ak_ai_clear_frame_buffer - frame buffer clean
 * @ai_handle_id[IN]: audio in opened handle id
 * return: 0 success, otherwise failed
 * notes: clean buffer after "ak_ai_open"
 */
int ak_ai_clear_frame_buffer(int ai_handle_id);

/**
 * ak_ai_set_gain - set volume
 * @ai_handle_id[IN]: audio in opened handle id
 * @gain[IN]: audio input gain
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_set_gain(int ai_handle_id, int gain);

/**
 * ak_ai_get_gain - get adc volume
 * @ai_handle_id[IN]: audio in opened handle id
 * @gain[OUT]: audio input gain
 * return:0 success, otherwise failed
 * notes:
 */
int ak_ai_get_gain(int ai_handle_id, int *gain);

/**
 * ak_ai_set_volume - set volume
 * @ai_handle_id[IN]: audio in opened handle id
 * @db[IN]: audio input volume, -90~20db
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_set_volume(int ai_handle_id, int db);

/**
 * ak_ai_get_volume - get volume
 * @ai_handle_id[IN]: audio in opened handle id
 * @db[OUT]: audio input volume
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_get_volume(int ai_handle_id, int *db);

/**
 * ak_ai_enable_nr - adc nr switch
 * @ai_handle_id[IN]: opened audio input handle id
 * @nr_enable[IN]: 0 disable nr, 1 enable nr.
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_enable_nr(int ai_handle_id, int nr_enable);

/**
 * ak_ai_enable_agc - adc agc switch
 * @ai_handle_id[IN]: opened audio input handle id
 * @agc_enable[IN]: 0 disable agc, 1 enable agc.
 * return: 0 success, otherwise failed
 * notes: if want to set agc,must set nr first
 */
int ak_ai_enable_agc(int ai_handle_id, int agc_enable);

/**
 * ak_ai_enable_aec - AEC switch
 * @ai_handle_id[IN]: opened audio input handle id
 * @aec_enable[IN]: 0 disable AEC, 1 enable AEC
 * return: 0 success, otherwise failed
 * notes: when AEC enable, must set ai and ao
 */
int ak_ai_enable_aec(int ai_handle_id, int aec_enable);

/**
 * ak_ai_enable_eq - enable eq
 * @ai_handle_id[IN]: audio out opened handle id
 * @enable[IN]: 1 enable,0 disable
 * return: 0 success otherwise failed
 * notes:
 */
int ak_ai_enable_eq(int ai_handle_id, int enable);

/**
 * ak_ai_set_agc_attr -  set attribute
 * @ai_handle_id[IN]: opened audio input handle id
 * @agc_attr[IN]: agc attribute
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_set_agc_attr(int ai_handle_id, struct ak_audio_agc_attr *agc_attr);

/**
 * ak_ai_get_agc_attr -  get attribute
 * @ai_handle_id[IN]: opened audio input handle
 * @agc_attr[OUT]: agc attribute
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_get_agc_attr(int ai_handle_id, struct ak_audio_agc_attr *agc_attr);

/**
 * ak_ai_set_aec_attr -  set attribute
 * @ai_handle_id[IN]: opened audio input handle id
 * @aec_attr[IN]: aec attribute
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_set_aec_attr(int ai_handle_id, struct ak_audio_aec_attr *aec_attr);

/**
 * ak_ai_get_aec_attr -  get attribute
 * @ai_handle_id[IN]: opened audio input handle
 * @aec_attr[OUT]: aec attribute 
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_get_aec_attr(int ai_handle_id, struct ak_audio_aec_attr *aec_attr);

/**
 * ak_ai_set_eq_attr - set eq attribute
 * @ai_handle_id[IN]: audio out opened handle id
 * @eq_attr[IN]: eq attribute
 * return: 0 success  otherwise failed
 * notes:
 */
int ak_ai_set_eq_attr(int ai_handle_id, struct ak_audio_eq_attr *eq_attr);

/**
 * ak_ai_get_eq_attr - get eq attribute
 * @ai_handle_id[IN]: audio out opened handle 
 * @eq_attr[OUT]: eq attribute
 * return: 0 success  otherwise failed
 * notes:
 */
int ak_ai_get_eq_attr(int ai_handle_id, struct ak_audio_eq_attr *eq_attr);

/**
 * ak_ai_set_vad_attr - set vad attribute
 * @ai_handle_id[IN]: audio out opened handle 
 * @vad_attr[IN]: vad attribute
 * return: 0 success  otherwise failed
 * notes:
 */
int ak_ai_set_vad_attr(int ai_handle_id, struct ak_audio_vad_attr *vad_attr);

/**
 * ak_ai_get_vad_attr - get vad attribute
 * @ai_handle_id[IN]: audio out opened handle 
 * @vad_attr[OUT]: vad attribute
 * return: 0 success  otherwise failed
 * notes:
 */
int ak_ai_get_vad_attr(int ai_handle_id, struct ak_audio_vad_attr * vad_attr);

/**
 * ak_ai_get_vad_status - get vad current status
 * @ai_handle_id[IN]: audio out opened handle 
 * @vad_attr[OUT]: vad status
 * return: 0 success  otherwise failed
 * notes:
 */
int ak_ai_get_vad_status(int ai_handle_id, struct ak_audio_vad_status *vad_status);

/**
 * ak_ai_set_source - set audio input source, linein or mic
 * @ai_handle_id[IN]: opened audio input handle id
 * @src[IN]: appointed source
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_set_source(int ai_handle_id, enum ak_ai_source src);

/**
 * ak_ai_get_source - get audio input source, linein or mic
 * @ai_handle_id[IN]: opened audio input handle id
 * @src[OUT]: appointed source
 * return: 0 success, otherwise failed
 */
int ak_ai_get_source(int ai_handle_id, int *src);

/**
 * ak_ai_print_runtime_status - print ai run time status
 * @ai_handle_id[IN]: audio in opened handle id
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_print_runtime_status(int ai_handle_id);

/**
 * ak_ai_save_aec_dump_file - whether save aec dump file
 * @ai_handle_id[IN]: audio in opened handle id
 * @enable[IN]: 0 disable , 1 enable.
 * return: 0 success, otherwise failed
 * notes: for debug use
 */
int ak_ai_save_aec_dump_file(int ai_handle_id, int enable);

/**
 * ak_ai_close - close audio input
 * @ai_handle_id[IN]: opened audio input handle id
 * return: 0 success, otherwise failed
 * notes: for debug use
 */
int ak_ai_close(int ai_handle_id);

/**
 * ak_ai_set_nr_attr -  set nr attribute
 * @ai_handle_id[IN]: opened audio input handle id
 * @nr_attr[IN]: nr attribute
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_set_nr_attr(int ai_handle_id, struct ak_audio_nr_attr *nr_attr);

/**
 * ak_ai_get_nr_attr -  get nr attribute
 * @ai_handle_id[IN]: opened audio input handle id
 * @nr_attr[OUT]: nr attribute
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_get_nr_attr(int ai_handle_id, struct ak_audio_nr_attr *nr_attr);

/**
 * ak_ai_save_file - save two file, one file is pcm input to ai, two file is pcm output from ai
 * @ai_handle_id[IN]: audio in opened handle id
 * @save_file_info[IN]: save file infomation
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_save_file(int ai_handle_id, struct ak_audio_save_file_info *save_file_info);

/**
 * ak_ai_set_aslc_attr - set aslc attribute
 * @ai_handle_id[IN]: opened audio input handle id
 * @aslc_attr[IN]: aslc attribute
 * return: 0 success  otherwise failed
 * notes:
 */
int ak_ai_set_aslc_attr(int ai_handle_id, struct ak_audio_aslc_attr *aslc_attr);

/**
 * ak_ai_get_aslc_attr - get aslc attribute
 * @ai_handle_id[IN]: opened audio input handle id
 * @aslc_attr[OUT]: aslc attribute
 * return: 0 success  otherwise failed
 * notes:
 */
int ak_ai_get_aslc_attr(int ai_handle_id, struct ak_audio_aslc_attr *aslc_attr);

/**
 * ak_ai_set_lib_print_level - set filter lib print level
 * @ai_handle_id[IN]: opened audio input handle id
 * @print_level[IN]: print level, enum ak_atclib_print_level,  bit-or of AUDIO_PRINT_LEVEL_XXX. Default is AUDIO_PRINT_LEVEL_ERROR | AUDIO_PRINT_LEVEL_WARNING | AUDIO_PRINT_LEVEL_INFO 
 * return: 0 success, otherwise failed
 * notes: 
 */
int ak_ai_set_lib_print_level(int ai_handle_id, int print_level);

/**
 * ak_ai_get_params - get ai parameter, include sample rate, sample bit, channel number
 * @ai_handle_id[IN]: opened audio input handle id
 * @pcm_attr[OUT]: store ai params delivery by ak_ai_open
 * return: 0 success, otherwise failed
 * notes: call after ak_ai_open
 */
int ak_ai_get_params(int ai_handle_id, struct ak_audio_data_attr *pcm_attr);

/**
 * ak_ai_reset_params - use to reset sample rate or channel numbe or frame_length
 * @ai_handle_id[IN]: opened audio input handle id
 * @param[IN]: ai params, include sample rate, sample bit, if param = NULL, use previous setting
 * @frame_length[IN]: audio frame length, unit: byte, mono frame_length >= 320, frame_length <= 4096;
                                stereo frame_length >= 640, unit: byte, frame_length <= 8192;
                                when frame_length is set 0, use default setting
 * return: 0 success, otherwise failed
 * notes: if capture start has begin, must stop capture first, then use ak_ai_reset_params, and then ak_ai_start_capture
 */
int ak_ai_reset_params(int ai_handle_id, struct ak_audio_in_param *param, int frame_length);

/**
 * ak_ai_set_aec_dump_file_info - set save aec dump file attribute, such as save file path and file time
 * @ai_handle_id[IN]: audio in opened handle
 * @save_file_info[IN]: save file infomation
 * return: 0 success, otherwise failed
 * notes: call after set all kind of ADC attr
 */
int ak_ai_set_aec_dump_file_info(int ai_handle_id, struct ak_audio_save_file_info *save_file_info);

/**
* ak_ai_set_max_frame_num - set ai module inner frame buffer nodes number hold
* @ai_handle_id[IN]: audio in opened handle
* @ai_frames_num[IN]: inner frame buffer nodes limit size
* return: 0 success, otherwise failed
* notes: this function is deprecated and no longer in use
*/
int ak_ai_set_max_frame_num(int ai_handle_id,int frame_num);

/**
 * ak_ai_get_max_frame_num - get the max frame to keep when user didn't to get.
 * @ai_handle_id[IN]: opened audio input handle id
 * @pframe_num[OUT]: audio frame max num
 * return:  >0 success, otherwise failed
 * notes: this function is deprecated and no longer in use
 */
int ak_ai_get_max_frame_num(int ai_handle_id, int *pframe_num);

/**
 * ak_ai_enable_vad - adc vad switch
 * @ai_handle_id[IN]: opened audio input handle id
 * @vad_enable[IN]: 0 disable vad, 1 enable vad.
 * return: 0 success, otherwise failed
 * notes: This function is use to enable vad, if not use ak_ai_set_vad_attr, default vad_thresh = 3276, default vad_type = AK_VAD_DET_AFTER_AGC
 */
int ak_ai_enable_vad(int ai_handle_id, int vad_enable);

/**
 * ak_ai_get_vad_status_ex -  get vad status, can use probe mode and normal mode
 * @ai_handle_id[IN]: opened audio input handle id
 * @vad_status[OUT]: vad status
 * return: 0 success, otherwise failed
 * notes:
 */
int ak_ai_get_vad_status_ex(int ai_handle_id, struct ak_audio_vad_status_ex *vad_status);

#endif
