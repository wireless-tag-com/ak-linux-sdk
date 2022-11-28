#ifndef _AK_COMMON_AUDIO_H_
#define _AK_COMMON_AUDIO_H_


#define _H3D_CHIP_ID             0x20170200
#define _H3B_CHIP_ID             0x20160101


#define    EQ_MAX_BANDS             (10)
#define    MAX_AUDIO_FILE_PATH_LEN  (256)
#define    MAX_AUDIO_FILE_NAME_LEN  (256)
#define    DEF_DEV_BUF_LEN	        (512)// unit:byte

/* default vqe param */

/* aec lib param */
#define SYNC_COMPEN_STATUS      (170)  //samples to compensate for far leading near

/* ai agc */
#define DEF_AI_AGC_LEVEL        (24576) //  auto gain control , default (0.75) * (1 << 15)
#define DEF_AI_AGC_MIN_GAIN     (0)
#define DEF_AI_AGC_MAX_GAIN     (4)
#define DEF_AI_NEAR_SENSITIVE   (40)

/* ai nr */
#define DEF_AI_NOISE_SUP_DB     (-40) // noise suppress db

/* ai aec */
#define DEF_AEC_OUT_DIGI_GAIN   (1024)
#define DEF_AEC_IN_DIGI_GAIN    (1024)
#define DEF_AEC_TAIL            (512)

/* ai aslc */
#define DEF_AI_LIMIT            (32768)
#define DEF_AI_DB               (0)


//////////////////////////////////////
/* ao agc */
#define DEF_AO_AGC_LEVEL        (24576) // auto gain control, default (0.75) * (1 << 15)
#define DEF_AO_AGC_MIN_GAIN     (0)
#define DEF_AO_AGC_MAX_GAIN     (4)
#define DEF_AO_NEAR_SENSITIVE   (20)

/* ao nr */
#define DEF_AO_NOISE_SUP_DB     (-40) // noise suppress db

/* ao aslc */
#define DEF_AO_LIMIT            (32768)
#define DEF_AO_DB               (0)


/******** default vqe param end **********/

/* vad type */
enum ak_vad_type
{
    AK_VAD_RESERVED      = 0x00,      //   0: VAD reserved type, VAD not active
    AK_VAD_DET_AFTER_NR  = 0x01,      //   1: sound level detection, after nr
    AK_VAD_DET_AFTER_AGC = 0x02,      //   2: sound level detection, after agc
    AK_VAD_DET_AFTER_ADC = 0x03,      //   3: sound level detection, on adc_stream
    AK_VAD_DET_PROBE     = 0x8000     //  bit15: probe mode
};

/* audio function status */
enum audio_func_status 
{
	AUDIO_FUNC_DISABLE = 0x00,
	AUDIO_FUNC_ENABLE,
};

/* audio channel type */
enum ak_audio_channel_type 
{
	AUDIO_CHANNEL_RESERVED = 0x00,
	AUDIO_CHANNEL_MONO,
	AUDIO_CHANNEL_STEREO,
};

enum ak_audio_type 
{
	AK_AUDIO_TYPE_UNKNOWN,
	AK_AUDIO_TYPE_MIDI,
	AK_AUDIO_TYPE_MP3,
	AK_AUDIO_TYPE_AMR,
	AK_AUDIO_TYPE_AAC,
	AK_AUDIO_TYPE_WMA,
	AK_AUDIO_TYPE_PCM,
	AK_AUDIO_TYPE_ADPCM_IMA,
	AK_AUDIO_TYPE_ADPCM_MS,
	AK_AUDIO_TYPE_ADPCM_FLASH,
	AK_AUDIO_TYPE_APE,
	AK_AUDIO_TYPE_FLAC,
	AK_AUDIO_TYPE_OGG_FLAC,
	AK_AUDIO_TYPE_RA8LBR,
	AK_AUDIO_TYPE_DRA,
	AK_AUDIO_TYPE_OGG_VORBIS,
	AK_AUDIO_TYPE_AC3,
	AK_AUDIO_TYPE_PCM_ALAW,
	AK_AUDIO_TYPE_PCM_ULAW,
	AK_AUDIO_TYPE_SBC,
	AK_AUDIO_TYPE_MSBC,
	AK_AUDIO_TYPE_SPEEX,
	AK_AUDIO_TYPE_SPEEX_WB
};

enum ak_audio_sample_rate 
{
	AK_AUDIO_SAMPLE_RATE_8000 = 8000,
	AK_AUDIO_SAMPLE_RATE_11025 = 11025,
	AK_AUDIO_SAMPLE_RATE_12000 = 12000,
	AK_AUDIO_SAMPLE_RATE_16000 = 16000,
	AK_AUDIO_SAMPLE_RATE_22050 = 22050,
	AK_AUDIO_SAMPLE_RATE_24000 = 24000,
	AK_AUDIO_SAMPLE_RATE_32000 = 32000,
	AK_AUDIO_SAMPLE_RATE_44100 = 44100,
	AK_AUDIO_SAMPLE_RATE_48000 = 48000,
	AK_AUDIO_SAMPLE_RATE_64000 = 64000,// now not support
	AK_AUDIO_SAMPLE_RATE_96000 = 96000// 37E support
};

enum ak_audio_dev_buf_size 
{
	AK_AUDIO_DEV_BUF_SIZE_256 = 256,// now not support
	AK_AUDIO_DEV_BUF_SIZE_512 = 512,
	AK_AUDIO_DEV_BUF_SIZE_1024 = 1024,
	AK_AUDIO_DEV_BUF_SIZE_2048 = 2048,
	AK_AUDIO_DEV_BUF_SIZE_3072 = 3072,
	AK_AUDIO_DEV_BUF_SIZE_4096 = 4096
};

enum ak_audio_sample_bit 
{
	AK_AUDIO_SMPLE_BIT_16 = 16
};

/* audio data attribute */
struct ak_audio_data_attr 
{
	enum ak_audio_sample_rate sample_rate;
	enum ak_audio_sample_bit sample_bits;
	enum ak_audio_channel_type channel_num;
};

struct audio_stream 
{
	unsigned char *data; 	//stream data
	unsigned int len; 		//stream len in bytes
	unsigned long long ts;	//timestamp(ms)
	unsigned long seq_no;	//current stream sequence no according to frame
};

struct ak_audio_eq_attr 
{
    signed short  pre_gain;                 // [-12288, 12288]
    unsigned long bands;                    //1~10
    unsigned long bandfreqs[EQ_MAX_BANDS];
    signed short  bandgains[EQ_MAX_BANDS];  // [-32768, 0) U (0, 32768]
    unsigned short bandQ[EQ_MAX_BANDS];     // [0, 65536)
    unsigned short band_types[EQ_MAX_BANDS];// HPF(hight pass filter);HSF(high shelf filter;LPF(low pass filter);LSF(low shelf filter);PF(peaking filter)
    unsigned char  smoothEna;               // deprecated and no longer in use
    unsigned short smoothTime;              // deprecated and no longer in use
    unsigned char   dcRmEna;                // deprecated and no longer in use
    unsigned long   dcfb;                   // deprecated and no longer in use
    unsigned char   aslc_ena;               // deprecated and no longer in use
    unsigned short  aslc_level_max;         // deprecated and no longer in use
    unsigned char enable;                   // 0: disable, 1: enable 
    unsigned char band_enable[EQ_MAX_BANDS];
};

enum ak_audio_eq_type 
{
    TYPE_NO ,
    TYPE_HPF ,
    TYPE_LPF ,
    TYPE_HSF ,
    TYPE_LSF ,
    TYPE_PF1    //PeaKing filter
};

enum ak_atclib_print_level 
{
    // debug zones
    AUDIO_PRINT_LEVEL_ERROR    =  0x0001,
    AUDIO_PRINT_LEVEL_WARNING  =  0x0002,
    AUDIO_PRINT_LEVEL_INFO     =  0x0004,// print user information
    AUDIO_PRINT_LEVEL_FUNCTION =  0x0008,// print function in or out information
    AUDIO_PRINT_LEVEL_PARAM    =  0x0010,// print function param
    AUDIO_PRINT_LEVEL_TRACE    =  0x0020,// print trace information
    AUDIO_PRINT_LEVEL_OPEN     =  0x0040,// Open information
    AUDIO_PRINT_LEVEL_CLOSE    =  0x0080,// Close information
    AUDIO_PRINT_LEVEL_EVENT    =  0x0100,// print user receive event
    AUDIO_PRINT_LEVEL_VERBOSE  =  0x8000,
    AUDIO_PRINT_LEVEL_DEFAULT  =  0x10000
};

struct ak_audio_save_file_info
{
    int  enable;
    char file_path[MAX_AUDIO_FILE_PATH_LEN];
    char file_name[MAX_AUDIO_FILE_NAME_LEN];
    int  save_second;// unit: second, value in [0-180]second
};

struct ak_audio_aec_attr 
{
    unsigned long   audio_out_threshold;// now no use this param! max amplitude of far signal, 1~32767, 0: use default. default is 9830
    signed short   	audio_out_digi_gain;// digtal gain for far signal,1~1024, 0: use default. default is 1024
    signed short   	audio_in_digi_gain; // digtal gain for near signal, 1~1024, 0: use default. default is 1024
    int crc;
    unsigned short  tail;             // tail size in samples
    int enable;
    unsigned long audio_farBreakdownThresh;
};

struct ak_audio_agc_attr 
{
    unsigned long   agc_level;          // 1~32767, 0: use default.default is 24576
    signed short    agc_max_gain;       // agc's max_gain, 2~8, default is 4
    signed short    agc_min_gain;       // agc's min_gain, default is 102
    signed short    near_sensitivity;   // sensitivity of near-end speech [1:100], 0: use default, default is 20
    int crc;
    int enable;
};

struct ak_audio_vad_attr
{
    signed long     vad_thresh;        // threshold of signal amplitude. use the form of AK32Q15(x.xx) to set it
    signed long     vad_level;         // sensitivity level. [0, 4], smaller is more sensible
    int enable;
    enum ak_vad_type vad_type;         // sound level detection type
};

struct ak_audio_vad_status
{
    int trigger;              // trigger of status change, 1: voice is active, 0: voice is not active
    unsigned long long ts;    // timestamp of the event
};

struct ak_audio_vad_status_ex
{
    int trigger;              // trigger of status change, 1: voice is active, 0: voice is not active
    unsigned long long ts;    // timestamp of the event    
    signed long vadThresh;    // when vadType has a probe flag, this is a calibrated value. in other cases, it is the same as echo_param_vad.vadTresh
    unsigned long vadType;    // reserve setting, not be used
    signed long vadLevel;     // reserve setting, not be used
    unsigned long long pre_ts;// reserve setting, previous timestamp of the event, , not be used
    signed long reserve;      // reserve setting, not be used
};

struct ak_audio_nr_attr 
{
    signed short  noise_suppress_db;  // attenuation of noise in dB (negative number), 0: use default, default is -40
    int crc;
    int enable;
};

struct ak_audio_aslc_attr 
{
    signed long limit;   // max amplitude of samples, 1~32767(Q15), 0: use default. use AK32Q15
    signed long  aslc_db;  //automatic sound volume control, aslc_db is for the voluume, unit db,  -90~20db
    int crc;
};

struct ak_ai_vqe_config
{
    struct ak_audio_nr_attr   nr_attr;
    struct ak_audio_agc_attr  agc_attr;
    struct ak_audio_aec_attr  aec_attr;
    struct ak_audio_eq_attr   eq_attr;
    struct ak_audio_aslc_attr aslc_attr;
    struct ak_audio_vad_attr  vad_attr;
};

struct ak_audio_threshold_attr 
{
    unsigned long   audio_out_threshold; // max amplitude of far signal, 1~32767, 0: use default. default is 9830
};

struct ak_ao_vqe_config
{
    struct ak_audio_nr_attr        nr_attr;
    struct ak_audio_eq_attr        eq_attr;
    struct ak_audio_aslc_attr      aslc_attr;
};

enum ak_audio_ai_dev_id
{
    DEV_ADC = 0,
    DEV_PDM = 1,
    DEV_I2S0_AI = 2,
    DEV_I2S1_AI = 3,
};

enum ak_audio_ao_dev_id
{
    DEV_DAC = 0,
    DEV_I2S0_AO = 1
    //DEV_I2S1_AO = 2,
};

struct ak_dev_buf_status 
{
    int buf_total_len;  // ai or ao device buffer total length, unit: byte
    int buf_remain_len; // ai or ao device buffer remain length, unit: byte
};

struct ak_audio_hs_attr // howling suppress attribute
{
    int crc;
    int enable;
};


static inline int ak_audio_check_sample(int sample_rate)
{
	if (sample_rate != AK_AUDIO_SAMPLE_RATE_8000 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_12000 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_11025 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_16000 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_22050 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_24000 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_32000 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_44100 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_48000 &&
		sample_rate != AK_AUDIO_SAMPLE_RATE_96000) {
		/* sample rate not support  */
		return -1;
	}
	return 0;
}

static inline int ak_audio_check_sample_bit(int sample_bit)
{
	if (sample_bit != 16) {
		return -1;
	}
    return 0;
}

static inline int ak_audio_check_channle(int channel_num)
{
	if (channel_num != AUDIO_CHANNEL_MONO && 
		channel_num != AUDIO_CHANNEL_STEREO) {
		return -1;
	}
    return 0;
}

/**
 * ak_audio_len_to_interval - calculate audio data ts interval, unit ms
 * @pcm_attr[IN]: audio pcm attribute, include sample rate, sample bit, channel number
 * @data_len[IN]: audio data length, unit byte
 * return: audio data ts interval
 */
static inline unsigned int ak_audio_len_to_interval(struct ak_audio_data_attr *pcm_attr, unsigned int data_len)
{
	/*   time(ms) = data_len  / (sample rete* channel num *sample bit /8 / 1000)  */
	/* 8 is bit to byte */
	/* 1000 is second to ms */
	/* dev->frame_interval is us */

	unsigned int sample_bytes = (pcm_attr->sample_bits >> 3);
	unsigned int sec_bytes = (pcm_attr->sample_rate * sample_bytes * pcm_attr->channel_num);

	if (0 == sec_bytes)
		sec_bytes = 8000 * (16 >> 3) * 1;
	
	return ((1000 * data_len) / sec_bytes);
}

/**
 * ak_audio_interval_to_len - calculate the size of pcm
 * @pcm_attr[IN]: audio pcm attribute, include sample rate, sample bit, channel number
 * @interval[IN]: audio data ts interval, unit ms
 * return: pcm size, unit byte
 */
static inline unsigned int ak_audio_interval_to_len(struct ak_audio_data_attr *pcm_attr, unsigned int interval)
{
	unsigned int sample_bytes = (pcm_attr->sample_bits >> 3);
	/* size = sample rete* channel num * time(ms) *sample bit /8 / 1000 */
	/* 8 is bit to byte */
	/* 1000000 is second to ms */
	/* dev->frame_interval is ms */
	unsigned int size = (pcm_attr->sample_rate * pcm_attr->channel_num * interval * sample_bytes ) / 1000;
	if (size & 1) {
		size++;
	}

	return size;
}

/**
 * ak_stereo_to_mono: copy frome double channel to single channel
 * @dest[IN]: single channel data
 * @src[IN]: double channel data
 * @src_len[IN]: double channel data length
 * return: void
 */
static inline void ak_stereo_to_mono(void *dest, void *src, int src_len)
{
	int i;

	for (i=0; i<src_len/4; i++) {
		((signed short*)dest)[i] = ((signed short*)src)[2*i];
	}
}

/**
 * ak_mono_to_stereo: copy frome single channel to double channel
 * @src[IN]: single channel data
 * @src_len[IN]: single channel data length
 * @dest[IN]: double channel data
 * return: void
 */
static inline void ak_mono_to_stereo(const unsigned char *src, int src_len, unsigned char *dest)
{
	int i = 0;
    int buf_size = src_len << 1;

	for (i=buf_size/4; i>0; i--) {
		((signed short*)dest)[2*i-1] = ((signed short*)src)[i-1];
		((signed short*)dest)[2*i-2] = ((signed short*)src)[i-1];
	}
}

/**
 * ak_get_dev_buf_len: get ai or ao device buffer length
 * @sample_rate[IN]: audio sample rate
 * return: void
 */
static inline unsigned int ak_get_dev_buf_len(int sample_rate)
{
    unsigned int dev_buf_len = 0;
    
    if (AK_AUDIO_SAMPLE_RATE_16000 > sample_rate)// 8000~ 24000--512
        dev_buf_len = DEF_DEV_BUF_LEN;
    else if (AK_AUDIO_SAMPLE_RATE_32000 > sample_rate)// 16000~22050 -1024
        dev_buf_len = DEF_DEV_BUF_LEN << 1;
    else if (AK_AUDIO_SAMPLE_RATE_48000 >= sample_rate)
        dev_buf_len = DEF_DEV_BUF_LEN << 2;// 3200~48000-2048
    else
        dev_buf_len = DEF_DEV_BUF_LEN << 3;// 96000-4096
    return dev_buf_len;
}


#endif

/* end of file */
