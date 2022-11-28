#ifndef _AK_VQE_H_
#define _AK_VQE_H_

#include "ak_common.h"
#include "ak_common_audio.h"

enum ak_vqe_error_type 
{
    ERROR_VQE_OPEN_LIB_ERROR = (MODULE_ID_VQE << 24) + 0,
    ERROR_VQE_LIB_ERROR,
    ERROR_VQE_CLOSE_LIB_ERROR,
    ERROR_VQE_USER_TOO_MANY,
    ERROR_VQE_USER_NULL,
    ERROR_VQE_NO_USER,
    ERROR_VQE_OUTBUF_LEN_ERROR,
    ERROR_VQE_ECHO_ERROR,
    ERROR_VQE_THREAD_CREATE_FAILED,
    ERROR_VQE_LOOPBACK_ERROR,
    ERROR_VQE_SET_AI_ERROR,
    ERROR_VQE_SET_AO_ERROR,
    ERROR_VQE_FILL_ERROR,
    ERROR_VQE_EQ_NO_OPEN
};

enum ak_vqe_type 
{
    AK_VQE_RESAMPLE,
    AK_VQE_ASLC,
    AK_VQE_EQ,

    AK_VQE_TYPE_NUM
};


/**
 * ak_vqe_get_version - voice quality enhancement module get version
 * return: version string
 * notes:
 */
const char* ak_vqe_get_version(void);

/**
 * ak_vqe_open - open voice quality enhancement module
 * @vqe_type[IN]: voice quality enhancement type, resample/aslc/eq
 * @vqe_handle_id[OUT]: vqe module opened handle id
 * return: 0 success, other failed
 * notes: 
 */
int ak_vqe_open(enum ak_vqe_type vqe_type, int *vqe_handle_id);

/**
 * ak_vqe_set_res_attr - voice quality enhancement module set resample attribute
 * @vqe_handle_id[IN]: vqe module opened handle id
 * @pcm_attr_in[IN]: resample input, audio data attribute
 * @pcm_attr_out[IN]: resample output, audio data attribute
 * return: 0 success, other failed
 * notes: 
 */
int ak_vqe_set_res_attr(int vqe_handle_id, 
                           struct ak_audio_data_attr *pcm_attr_in, 
                           struct ak_audio_data_attr *pcm_attr_out);

/**
 * ak_vqe_set_aslc_attr - voice quality enhancement module set aslc attribute
 * @vqe_handle_id[IN]: vqe module opened handle id
 * @pcm_attr_in[IN]: aslc input, audio data attribute 
 * @db[IN]: aslc volume value
 * return: 0 success, other failed
 * notes: 
 */
int ak_vqe_set_aslc_attr(int vqe_handle_id, 
                             struct ak_audio_data_attr *pcm_attr_in,
                             int db);

/**
 * ak_vqe_set_eq_attr - voice quality enhancement module set eq attribute
 * @vqe_handle_id[IN]: vqe module opened handle id
 * @pcm_attr_in[IN]: audio data attribute
 * @eq_attr[IN]: eq attribute
 * return: 0 success, other failed
 * notes: 
 */
int ak_vqe_set_eq_attr(int vqe_handle_id, 
                           struct ak_audio_data_attr *pcm_attr_in, 
                           struct ak_audio_eq_attr *eq_attr);

/**
 * ak_vqe_process - voice quality enhancement module deal with data
 * @vqe_handle_id[IN]: vqe opened handle id
 * @inbuf[IN]: audio origin input data
 * @inbuf_len[IN]: audio input buffer length, unit byte
 * @outbuf[OUT]: audio output data buffer, buffer will malloc in ak_vqe_process function
 * @ret_data_len[OUT]: output data length
 * return: 0 success, other failed
 * notes:
 */
int ak_vqe_process(int vqe_handle_id, 
                       unsigned char *inbuf, unsigned int inbuf_len, 
                       unsigned char **outbuf,
                       unsigned int *ret_data_len);

/**
 * ak_vqe_close - voice quality enhancement module close 
 * @vqe_handle_id[IN]: vqe module hanle id
 * return: 0 success, other failed
 * notes:
 */
int ak_vqe_close(int vqe_handle_id);

/**
 * ak_vqe_get_filter_info - get audio filter lib version
 * return: audio filter lib version
 * notes:
 */
char *ak_vqe_get_filter_info(void);

#endif
