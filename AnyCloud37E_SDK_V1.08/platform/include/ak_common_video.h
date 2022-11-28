#ifndef _AK_COMMON_VIDEO_H_
#define _AK_COMMON_VIDEO_H_

/* video encode frame define */
enum video_frame_type {
    FRAME_TYPE_P,
    FRAME_TYPE_I,
    FRAME_TYPE_PI,
};

struct video_stream {
	unsigned char *data;	//stream data
	unsigned int len; 		//stream len in bytes
	unsigned long long ts;	//timestamp(ms)
	unsigned long seq_no;	//current stream sequence no according to frame
	enum video_frame_type frame_type;	//I or P frame
};


/* video encode output type define */
enum encode_output_type {
    H264_ENC_TYPE,
    MJPEG_ENC_TYPE,
    HEVC_ENC_TYPE
};

#endif

/* end of file */
