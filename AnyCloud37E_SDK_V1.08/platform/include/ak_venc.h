#ifndef _AK_VENC_H_
#define _AK_VENC_H_

#include "ak_common_video.h"

enum ak_venc_error_type
{
    ERROR_VENC_INIT_LIB_ERROR = (MODULE_ID_VENC << 24) + 0,
	ERROR_VENC_OPEN_LIB_ERROR,
    ERROR_VENC_OPEN_TOO_MANY,
    ERROR_VENC_INVALID_PARAM,
    ERROR_VENC_USER_NULL,
    ERROR_VENC_PARAM_CANNOT_DYNAMIC_SET,
    ERROR_VENC_ENCODE_FAILED
};

/* h.264 / h.265 encode control define */
enum bitrate_ctrl_mode {
    BR_MODE_CBR,
    BR_MODE_VBR,
    BR_MODE_CONST_QP,
    BR_MODE_LOW_LATENCY,
	BR_MODE_TYPE_TOTAL
};

enum profile_mode {
    PROFILE_MAIN,
    PROFILE_HIGH,
    PROFILE_BASE,
    PROFILE_C_BASE,
    PROFILE_HEVC_MAIN,
    PROFILE_HEVC_MAIN_STILL,
    PROFILE_HEVC_MAIN_INTRA,
    PROFILE_JPEG
};

enum chroma_mode {
    CHROMA_MONO,
    CHROMA_4_2_0,
    CHROMA_4_2_2
};

enum jpeg_quant_table_level {
    JPEG_QLEVEL_DEFAULT,
    JPEG_QLEVEL_HIGHEST,
    JPEG_QLEVEL_HIGH,
    JPEG_QLEVEL_LOW
};

enum smart_mode {
    SMART_DISABLE,
    SMART_LTR,
    SMART_CHANGING_GOPLEN,
    SMART_SKIP_FRAME
};


struct venc_roi_param {
    int  enable;    //1 enable, 0 disable
    long top;
    long bottom;
    long left;
    long right;
    long delta_qp;
};

struct venc_param {
    unsigned short                width;
    unsigned short                height;
    unsigned short                fps;
    unsigned short                goplen;
    unsigned short                target_kbps;
    unsigned short                max_kbps;
    enum profile_mode             profile;
    enum bitrate_ctrl_mode        br_mode;
    unsigned short                initqp;       //recommend Dynamic bit rate parameter[20,25]
    unsigned short                minqp;        //recommend Dynamic bit rate parameter[20,25]
    unsigned short                maxqp;        //recommend Dynamic bit rate parameter[45,50]
    enum jpeg_quant_table_level   jpeg_qlevel;
    enum chroma_mode              chroma_mode;
    enum encode_output_type       enc_out_type; //encode output type, h264 or jpeg or h265
    unsigned int                  max_picture_size;
    unsigned short                enc_level;
    enum smart_mode               smart_mode;         //0:disable smart, 1:mode of LTR, 2:mode of changing GOP length
    unsigned short                smart_goplen;       //smart goplen
    unsigned short                smart_quality;      //smart quality
    unsigned short                smart_static_value; //smart static value

};



struct venc_stat {
    unsigned short fps;
    unsigned short goplen;
    unsigned short kbps;
    unsigned short max_picture_size;
};

#define AK_VENC_MAX_DMA_BLOCK_NUM	20
// 用来做dma内存池管理的
typedef struct ak_venc_req_dma_block{
	unsigned int block_num;
	unsigned int block_size[AK_VENC_MAX_DMA_BLOCK_NUM];
}AK_VENC_REQ_DMA_BLOCK;

/**
 * ak_venc_get_version - get venc version
 * return: version string
 */
const char* ak_venc_get_version(void);

/**
 * ak_venc_get_enc_lib_version - get venc encode lib version
 * return: version string
 */
const char* ak_venc_get_enc_lib_version(void);

/**
 * ak_venc_open - open encoder and set encode param
 * @param[IN]: encode param
 * @handle_id[OUT]: handle id
 * return: 0 success , others error code.
 */
int ak_venc_open(const struct venc_param *param, int *handle_id);


/**
 * ak_venc_encode_frame - encode single frame
 * @handle_id[IN]: handle id return by ak_venc_open
 * @frame[IN]: frame which you want to encode
 * @frame_len[IN]: lenght of frame
 * @mdinfo[IN]: md info array
 * @stream[OUT]: encode output buffer address
 * return: 0 success , others error code.
 */
int ak_venc_encode_frame(int handle_id, const unsigned char *frame,
        unsigned int frame_len, void *mdinfo, struct video_stream *stream);

/**
 * ak_venc_release_stream - release stream resource
 * @handle_id[IN]: handle id return by ak_venc_open
 * @stream[IN]: stream return by ak_venc_encode_frame()
 * return: 0 success , others error code.
 * notes:
 */
int ak_venc_release_stream(int handle_id, struct video_stream *stream);


/**
 * ak_venc_close - close video encode
 * @handle_id[IN]: handle id return by ak_venc_open()
 * return: 0 success , others error code.
 */
int ak_venc_close(int handle_id);

/**
 * ak_venc_set_attr - set venc params
 * @handle_id[IN]: handle id return by ak_venc_open
 * @param[IN]: param to set
 * return: 0 success , others error code.
 * notes:
 */
int ak_venc_set_attr(int handle_id, const struct venc_param *param);

/**
 * ak_venc_get_attr - get venc params
 * @handle_id[IN]: handle id return by ak_venc_open
 * @param[OUT]: params
 * return: 0 success , others error code.
 * notes:
 */
int ak_venc_get_attr(int handle_id, struct venc_param *param);

/**
 * ak_venc_request_idr - request I frame
 * @handle_id[IN]: handle id return by ak_venc_open
 * return: 0 success , others error code.
 * notes:
 */
int ak_venc_request_idr(int handle_id);


/**
 * ak_venc_get_rate_stat - on stream-encode, get encode rate stat info
 * @handle_id[IN]: handle id return by ak_venc_open
 * @stat[OUT]: stream rate stat info
 * return: 0 success, others error code.
 * notes:
 */
int ak_venc_get_stat(int handle_id, struct venc_stat *stat);

/**
 * ak_venc_set_qp - set the venc qp value
 * @handle_id[IN]: handle id return by ak_venc_open
 * @minQP[IN]: stream rate stat info
 * @maxQP[IN]: stream rate stat info
 * return: 0 success, others error code.
 * notes: if venc opened in const qp mode, that func couldn't work if called.
 */
int ak_venc_set_qp(int handle_id, int minQP, int maxQP);

/**
 * ak_venc_reset_qp - reset the venc qp value
 * @handle_id[IN]: handle id return by ak_venc_open
 * return: 0 success, others error code.
 * notes: if venc opened in const qp mode, that func couldn't work if called.
 *        And that func will make the qp back to the value when opening.
 */
int ak_venc_reset_qp(int handle_id);

/**
 * ak_venc_set_stream_buff - set the venc stream buff size
 * @handle_id[IN]: handle id return by ak_venc_open
 * @size[IN]     : size to set，unit is byte.
 * return: 0 success, others error code.
 * notes: if venc opened in const qp mode, that func couldn't work if called.
 *        And that func will make the qp back to the value when opening.
 */
int ak_venc_set_stream_buff(int handle_id, int size);

/**
 * ak_venc_set_iframe_param - set the venc iframe size
 * @handle_id[IN]: handle id return by ak_venc_open
 * @minQP[IN]: Iframe stat info
 * @maxQP[IN]: Iframe stat info
 * @minframeSize[IN]: min frame KByte size
 * @maxframeSize[IN]: max frame KByte size
 * return: 0 success, others error code.
 */
int ak_venc_set_iframe_param(int handle_id, int minQP, int maxQP,  int minframeSize, int maxframeSize);

/**
 * ak_venc_get_iframe_param - get the venc iframe size
 * @handle_id[IN]: handle id return by ak_venc_open
 * @minQP[OUT]: Iframe stat info
 * @maxQP[OUT]: Iframe stat info
 * @minframeSize[OUT]: min frame KByte size
 * @maxframeSize[OUT]: max frame KByte size
 * return: 0 success, others error code.
 */
int ak_venc_get_iframe_param(int handle_id, int *minQP, int *maxQP,  int *minframeSize, int *maxframeSize);

/**
 * ak_venc_set_ROI_Mdinfo - set the ROI param to night mode NR(ROI的功能用于减轻夜视图像中的噪声)
 * @handle_id[IN]: handle id return by ak_venc_open
 * @Threshold[IN]: threshold of mdinfo value
 * @Quality[IN]:  max qp delta (Q0 format)
 * @RecoverStep[IN]: recover step of qp delta (Q2)
 * @WaitIntra[IN]: wait num of frames to enable Intra, 0-disable, 1-enable
 * @WaitSkip[IN]: wait num of frames to enable skip, 0-disable, 1-enable
 * return: 0 success, others error code.
 */
int ak_venc_set_ROI_Mdinfo(int handle_id, int Threshold, int Quality,  int RecoverStep, int WaitIntra, int WaitSkip);


/**
 * ak_venc_set_jpeg_slice - set the jpeg slice encode config
 * @handle_id[IN]: handle id return by ak_venc_open
 * @picHeight[IN]:total frame height
 * @firstSliceHeight[IN]: first Slice Height
 * return: 0 success, others error code.
 */
int ak_venc_set_jpeg_slice(int handle_id, int picHeight,int firstSliceHeight );

/**
 * ak_venc_jpeg_slice_encode - jpeg slice encode
 * @handle_id[IN]: handle id return by ak_venc_open
 * @frame[IN]: slice frame which you want to encode
 * @frame_len[IN]: lenght of slice frame
 * @slice_idx[IN]: index of slice number
 * @slice_height[IN]: height of slice frame
 * @stream[OUT]: encode output buffer address
 * return: 0 success , others error code.
 */
int ak_venc_jpeg_slice_encode(int handle_id, const unsigned char *frame,
               unsigned int frame_len, int slice_idx, int slice_height,struct video_stream *stream);

/**
 * ak_venc_change_CUblocksize - brief Set the information of CUsize [0:2] only for HEVC
 * @handle_id[IN]: handle id return by ak_venc_open
 * @size[IN]: CU size  defaut=0, 0:32x32 ; 1:16x16; 2:8x8 just for 265
 * return: 0 success, others error code.
 */
int ak_venc_change_CUblocksize(int handle_id, int size);

/**
 * ak_venc_strictlylimit_frameSize - Limit the size of frame strictly
 * @handle_id[IN]: handle id return by ak_venc_open
 * @maxsize[IN]: max frame size(unit is kbytes), 0 <= maxsize(KB) * 1024 < Stream_Buffer Size, when maxsize==0, will disable frame size limit
 * return: 0 success, others error code.
 */
int ak_venc_strictlylimit_frameSize(int handle_id, int maxsize);

/**
 * 用来获取给定配置下，编码库会使用的dma内存大小的信息
 */
int ak_venc_get_req_dma_block(const struct venc_param *param, struct venc_roi_param *roi, AK_VENC_REQ_DMA_BLOCK *dma_block);


#endif
