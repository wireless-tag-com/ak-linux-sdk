#ifndef _AK_VIDEO_DECODE_H
#define _AK_VIDEO_DECODE_H

#include "ak_common_video.h"

/* vdec errno */
enum ak_vdec_errno {                                                            //错误代码
    ERROR_VDEC_INVALID_HANDLE_ID          = ( MODULE_ID_VDEC << 24 ) + 0,       //0 无效VDEC句柄ID
    ERROR_VDEC_DECODER_INIT_FAILED        = ( MODULE_ID_VDEC << 24 ) + 1,       //1 解码器初始化失败
    ERROR_VDEC_OUT_OF_DECODER_LIMITATION  = ( MODULE_ID_VDEC << 24 ) + 2,       //2 解码器打开的数量超出限制
    ERROR_VDEC_FREE_SIZE_NO_ENOUGH        = ( MODULE_ID_VDEC << 24 ) + 3,       //3 解码空余缓存不足
    ERROR_VDEC_GET_DECODE_FARME_FAILED    = ( MODULE_ID_VDEC << 24 ) + 4,       //4 解码数据获取失败
    ERROR_VDEC_HANDLE_ID_NOT_MATCH        = ( MODULE_ID_VDEC << 24 ) + 5,       //5 解码句柄ID不匹配
    ERROR_VDEC_FRAME_ON_USE               = ( MODULE_ID_VDEC << 24 ) + 6,       //6 解码数据帧正在使用
    ERROR_VDEC_WRONG_FRAME_FORMAT         = ( MODULE_ID_VDEC << 24 ) + 7,       //7 解码数据帧格式错误
    ERROR_VDEC_START_DECODER_FAILED       = ( MODULE_ID_VDEC << 24 ) + 8,       //8 解码启动失败
    ERROR_VDEC_DECODE_FAILURE             = ( MODULE_ID_VDEC << 24 ) + 9,       //9 解码码流过程中出现了错误但仍可以继续解码
    ERROR_VDEC_DECODE_ERROR_EXIT          = ( MODULE_ID_VDEC << 24 ) + 10,      //10 解码码流过程出现了非常严重的错误,无法继续解码下去,需要停止送码流并退出解码；
};

/* decoder output source type */
enum ak_vdec_output_type{
    AK_YUV420P,
    AK_YUV420SP,
    AK_TILE_YUV,
    AK_RGB565,
    AK_ARGB8888,
    AK_VDEC_OUTPUT_NUM,
};

/* send stream block mode */
enum block_mode
{
    BLOCK,
    NONBLOCK
};

/* open parameter */
struct ak_vdec_param{
    enum encode_output_type vdec_type;
    int sc_width;
    int sc_height;
    enum ak_vdec_output_type output_type;
    int stream_buf_size;    /*stream buffer size.between[300k,96M].0 for default 2M. Size must be 4096 alignment. */
    int frame_buf_num;      /*total yuv frame buff number, 37E:[0,2-15](0 for 3) A37D:[0,3-15](0 for 4)*/
};

/* store the untiled data */
struct ak_vdec_data
{
    int     pitch_width;    /* the real width  */
    int     pitch_height;   /* the real height */
    int     data_size;      /* the decoded  data size */
    unsigned char *data;    /* the decoded  data pointer */
    void *FBD;              /* pointer to store the untile data */
};

/* vdec frame data struct */
struct ak_vdec_frame
{
    int     id;             /* recored handle id for video decoder */
    int     width;          /* width of decoded YUV */
    int     height;         /* height of decoded YUV */
    unsigned long long ts;  /* timestamp(ms) */
    enum ak_vdec_output_type data_type; /* frame type */

    union {
        struct ak_vdec_data data;   /* untiled data */
        void    *tiled_data;        /* tiled data */
    }frame_obj;
};

#define  yuv_data   frame_obj.data
#define  tileyuv_data   frame_obj.tiled_data

/**
 * ak_vdec_get_version - get video decode version
 * return: version string
 * notes:
 */
const char* ak_vdec_get_version(void);

/*
 * ak_vdec_open - open anyka video decode
 * @param[IN]: 		video stream decode param
 * @handle_id[OUT]:	handle_id pointer to record the id of alloc decoder
 * return: AK_SUCCESS if successful; Error code if failed.
 */
int ak_vdec_open(const struct ak_vdec_param *param, int *handle_id);

/*
 * ak_vdec_close - open anyka video decode
 * @handle_id[IN]:  handle_id pointer to record the id of alloc decoder struct
 * return: AK_SUCCESS if successful; Error code if failed.
 */
int ak_vdec_close(int handle_id);

/*
 * ak_vdec_send_stream --   decode video stream
 * @handle_id[IN]      :    handle id of video decoder
 * @data[IN]           :    data add to decode buffer
 * @len[IN]            :    length of data
 * @mod[IN]            :    0 block mode; 1 non-block mode, only support block( 0 ) mode now.
 * count[OUT]          :    point to store the length of sent data.
 * return: AK_SUCCESS, Error code if failed
 * */
int ak_vdec_send_stream(int handle_id, const unsigned char *data, unsigned int len, enum block_mode mod, int *count);


/*
 * ak_vdec_get_frame    --  get frame from video decoder
 * @handle_id[IN]   :       handle id of video decoder
 * @frame[OUT]      :       point to store the vdec frame data
 * return AK_SUCCESS if successful, Error code if falied
 */
int ak_vdec_get_frame(int handle_id, struct ak_vdec_frame *frame);

/*
 * ak_vdec_release_frame    --  release frame from video decoder
 * @handle_id[IN]   :       handle id of video decoder
 * @frame           :       point to store the vdec frame data
 * return AK_SUCCESS if successful, Error code if falied
 */
int ak_vdec_release_frame(int handle_id, struct ak_vdec_frame *frame);

/*
 * ak_vdec_clear_buff   --  clear the decoder buffer
 * @handle_id[IN]       :       handle id of video decoder
 * return AK_SUCCESS if successful, Error code if failed
 **/
int ak_vdec_clear_buff(int handle_id);

/*
 * ak_vdec_end_stream   --  notice send stream end
 * @handle_id[IN]       :       handle id of video decoder
 * return AK_SUCCESS if successful, Error code if failed
 * */
int ak_vdec_end_stream(int handle_id);

/*
 * ak_vdec_get_decode_finish        --  check video decode finish status
 * @handle_id[IN]       :       handle id of video decoder
 * @status[OUT]         :       pointer to store the decode finish status
 * return AK_SUCCESS if successful, Error code if failed
 * */
int ak_vdec_get_decode_finish(int handle_id, int *status);

/*
 * ak_vdec_frame_decode	--	force the decoder to output the one frame data
 * @handle_id[IN]		:		handle id of video decoder
 * return AK_SUCCESS if successful, Error code if failed
 * NOTE : CALL the function if you certainly had send a complete frame to the decoder.
 *        And it will force the decoder to decode the frame and output as quickly.
 * */
int ak_vdec_frame_decode(int handle_id);

/**
 * ak_vdec_get_dec_lib_version - get vdec decode lib version
 * return: version string
 */
const char* ak_vdec_get_dec_lib_version(void);

#endif
