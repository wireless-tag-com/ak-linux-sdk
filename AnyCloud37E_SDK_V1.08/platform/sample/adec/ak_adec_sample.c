/**
* File Name: ak_adec_sample.c
* Description: This is a simple example to show how the ADEC module working.
* Notes:
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>

#include "ak_common.h"

#include "ak_ao.h"
#include "ak_adec.h"
#include "ak_thread.h"
#include "ak_log.h"

#ifdef AK_RTOS
#include <kernel.h>
#endif

#define RECORD_READ_LEN         4096 // read audio file buffer length
#define LEN_HINT                512
#define LEN_OPTION_SHORT        512
#define PCM_READ_LEN		 	512


static const char pcm_def_parth[] = "/mnt/ak_ao_test.pcm";
static FILE *fp = NULL;

static int ao_handle_id = -1;
static int send_frame_end = 0;

static int sample_rate = 8000;
static int channel_num = 1;
static char *play_file = (char *)pcm_def_parth;
static int dec_type = AK_AUDIO_TYPE_PCM;

static int flag  = 0;
static int adec_handle_id = -1;
static int volume = 0;
static int dev_sel = DEV_DAC;


/* ***********************************
    ***********************************
    *
    use this sample
    must follow this:
    1. make sure the driver is insmode;
    2. mount the T card;
    3. the file path is exit;
    *
    ***********************************
    ***********************************
*/

static const char ac_option_hint[][LEN_HINT] = {
    "	打印帮助信息" ,
    "	采样率[8000 12000 11025 16000 22050 24000 32000 44100 48000]" ,
    "	通道数[1 2]" ,
    "	编码类型[mp3 aac amr g711a g711u pcm]" ,
    "	播放的文件名称" ,
    "	音量[-90~20db]" ,
    "	设备选择，0-adc，1-pdm, 2-I2S0" ,
    ""
};

static struct option option_long[] = {
    { "help"        , no_argument       , NULL , 'h' } , //"       打印帮助信息" ,
    { "sample-rate" , required_argument , NULL , 's' } , //"[NUM]  采样率[8000 16000 32000 44100 48000]" ,
    { "channel-num" , required_argument , NULL , 'c' } , //"[NUM]  通道数[1 2]" ,
    { "decode-type" , required_argument , NULL , 'd' } , //"[TYPE] 编码类型[mp3 aac amr g711a g711u pcm]" ,
    { "play-file"   , required_argument , NULL , 'p' } , //"[FILE] 播放的文件名称" ,
    { "volume"      , required_argument , NULL , 'v' } , //"[NUM]  数字音量[-90~20db]" ,
    { "dev-sel"     , required_argument , NULL , 'D' } , //"[DEV_SEL] 输入设备选择" ,
    {0, 0, 0, 0}
};


/**
 * parse_adec_type - get audio file type 
 * @adec_type[IN]: encode type [mp3/amr/aac/g711a/g711u/pcm]
 * notes: return audio type number
 */
static int parse_adec_type(const char *adec_type)
{
    int ret = -1;
    char type[10] = {0};
    int type_len = strlen(adec_type);

    /* the type length <= 5,such as g711u */
    if (type_len <= 5) {
        sprintf(type, "%s", adec_type);

        int i = 0;
        for (i=0; i<type_len; ++i) {
            type[i] = tolower(type[i]);
        }

        ak_print_notice_ex(MODULE_ID_APP, "audio decode type: %s\n", type);

        /* check the audio file type */
        if (0 == strcmp(type, "mp3"))
            ret = AK_AUDIO_TYPE_MP3;
        else if (0 == strcmp(type, "amr"))
            ret = AK_AUDIO_TYPE_AMR;
        else if (0 == strcmp(type, "aac"))
            ret = AK_AUDIO_TYPE_AAC;
        else if (0 == strcmp(type, "g711a"))
            ret = AK_AUDIO_TYPE_PCM_ALAW;
        else if (0 == strcmp(type, "g711u"))
            ret = AK_AUDIO_TYPE_PCM_ULAW;
        else if (0 == strcmp(type, "pcm"))
            ret = AK_AUDIO_TYPE_PCM;
        else if (0 == strcmp(type, "dra")) /* ret = AK_AUDIO_TYPE_DRA; */
            ret = AK_AUDIO_TYPE_UNKNOWN;
        else /* unknow type */
            ret = AK_AUDIO_TYPE_UNKNOWN;

    }
    return ret;
}

/**
 * help_hint - help note 
 * @pc_prog_name[IN]: help parameter
 * notes: 
 */
static int help_hint(char *pc_prog_name)
{
    int i;

    printf("version: \r\n	%s \n\n", ak_adec_get_version());
    printf("usage: \r\n  %s  [options] <value>\n\n", pc_prog_name);
    printf("options: \r\n");

    for(i = 0; i < sizeof(option_long) / sizeof(struct option)-1; i ++) {
        printf("  -%c,--%-16s\t%s \n" , option_long[i].val , option_long[i].name , ac_option_hint[i]);
    }
    printf("\nexample: \r\n"
    "  %s -s 8000 -c 1 -d mp3 -p /mnt/xxx.mp3\n", pc_prog_name);
    printf("\n\n");

    return 0;
}

static char *get_option_short(struct option *p_option, int i_num_option, char *pc_option_short, int i_len_option)
{
    int i;
    int i_offset = 0;
    char c_option;

    for (i = 0; i < i_num_option; i++) {
        c_option = p_option[i].val;
        switch (p_option[i].has_arg) {
        case no_argument:
            i_offset += snprintf(pc_option_short + i_offset , i_len_option - i_offset , "%c" , c_option);
            break;
        case required_argument:
            i_offset += snprintf(pc_option_short + i_offset , i_len_option - i_offset , "%c:" , c_option);
            break;
        case optional_argument:
            i_offset += snprintf(pc_option_short + i_offset , i_len_option - i_offset , "%c::" , c_option);
            break;
        }
    }
    return pc_option_short;
}

/**
 * parse_option - parse option
 * notes: 
 */
static int parse_option(int argc, char **argv)
{
    int i_option;
    char ac_option_short[LEN_OPTION_SHORT];
    int i_array_num = sizeof(option_long) / sizeof(struct option) ;
    char c_flag = 1;

    char *type = "pcm";

    if (argc < 6 && argc != 1) {
        help_hint(argv[0]);
        c_flag = 0;
        goto parse_option_end;
    }

#ifdef AK_RTOS
    optind = 0;
#endif
    get_option_short(option_long, i_array_num, ac_option_short, LEN_OPTION_SHORT);
    while ((i_option = getopt_long(argc, argv, ac_option_short, option_long, NULL)) > 0) {
        switch(i_option) {
        case 'h' :                                                          //help
            help_hint(argv[0]);
            c_flag = 0;
            goto parse_option_end;
        case 's' :                                                          //sample-rate
            sample_rate = atoi(optarg);
            break;
        case 'c' :                                                          //volume
            channel_num = atoi(optarg);
            printf("----channel_num- %d-----\n", channel_num);
            break;
        case 'd' :                                                          //volume
            type = optarg;
            dec_type = parse_adec_type(type);
            printf("----dec_type- %d-----\n", dec_type);
            break;
        case 'p' :                                                          //pcm-file
            play_file = optarg;
            break;
        case 'v' :                                                          //volume
            volume = atoi(optarg);
            break;
        case 'D':
            dev_sel = atoi(optarg);
            break;
        default:
            help_hint(argv[0]);
            c_flag = 0;
            goto parse_option_end;
        }
    }

parse_option_end:
    return c_flag;
}

#ifdef AK_RTOS
static void param_init(void)
{
    fp = NULL;
    ao_handle_id = -1;
    send_frame_end = 0;
    sample_rate = 8000;
    channel_num = 1;
    play_file = (char *)pcm_def_parth;
    dec_type = AK_AUDIO_TYPE_PCM;
}
#endif

/**
 * print_recording_dot - print dot when playing
 * notes: print one dot for 10 seconds
 */
static void print_playing_dot(void)
{
    static unsigned char first_flag = 1;
    static struct ak_timeval cur_time;
    static struct ak_timeval print_time;

    if (first_flag) {
        first_flag = 0;
        ak_get_ostime(&cur_time);
        ak_get_ostime(&print_time);
        ak_print_normal_ex(MODULE_ID_APP,"\n.");
    }

    /* get time */
    ak_get_ostime(&cur_time);
    if (ak_diff_ms_time(&cur_time, &print_time) >= 10000) {
        ak_get_ostime(&print_time);
        ak_print_normal_ex(MODULE_ID_APP, ".");
    }
}

/**
 * open_play_file - open play file 
 * @play_file[IN]: file name
 * notes: return file handle
 */
static FILE* open_play_file(const char *play_file)
{
    FILE *fp = fopen(play_file, "r");
    if (NULL == fp) {
        ak_print_error_ex(MODULE_ID_APP, "open %s failed\n", play_file);
        return NULL;
    }
    
    /* amr file,have to skip the head 6 byte */
    unsigned char data[10] = {0};
    if (AK_AUDIO_TYPE_AMR == dec_type) {
        fread(data, 1, 6, fp);
    }

    ak_print_normal(MODULE_ID_APP, "open record file: %s OK\n", play_file);
    return fp;
}

/*
 * close_play_file: close play file.
 * fp[IN]: pointer of opened play file.
 * return: void.
 */
static void close_play_file(FILE *fp)
{
    if(NULL != fp) {
        fclose(fp);
        fp = NULL;
    }
}

/*
    设置各种音频处理参数结构体为默认值，默认值是从ak_audio_config.h抄的。
    参数仅作参考，实际参数要根据不同产品独立设定。
    1：NEAR(ai), ak_audio_nr_attr
    2：NEAR(ai), ak_audio_agc_attr
    3：NEAR(ai), ak_audio_aec_attr
    4：NEAR(ai), ak_audio_aslc_attr
    5：FAR(ao),  ak_audio_nr_attr
    6：FAR(ao),  ak_audio_aslc_attr
    7：NEAR(ai), ak_ai_set_eq_attr
    8：FAR(ao),  ak_ai_set_eq_attr
*/
static void setup_default_audio_argument(void *audio_args, char args_type)
{
    struct ak_audio_nr_attr     default_ai_nr_attr      = {-40, 0, 1};
    struct ak_audio_agc_attr    default_ai_agc_attr     = {24576, 4, 0, 80, 0, 1};
    struct ak_audio_aec_attr    default_ai_aec_attr     = {0, 1024, 1024, 0, 512, 1, 0};
    struct ak_audio_aslc_attr   default_ai_aslc_attr    = {32768, 0, 0};
    struct ak_audio_eq_attr     default_ai_eq_attr      = {
    0,
    10,
    {50, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {717, 717, 717, 717, 717, 717, 717, 717, 717, 717},
    {TYPE_HPF, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1},
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    struct ak_audio_nr_attr     default_ao_nr_attr      = {0, 0, 0};
    struct ak_audio_aslc_attr   default_ao_aslc_attr    = {32768, volume, 0};
    struct ak_audio_eq_attr     default_ao_eq_attr      = {
    0,
    10,
    {50, 63, 125, 250, 500, 1000, 2000, 4000, 8000, 16000},
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    {717, 717, 717, 717, 717, 717, 717, 717, 717, 717},
    {TYPE_HPF, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1},
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
    };

    switch (args_type) {
    case 1:
        *(struct ak_audio_nr_attr*)audio_args = default_ai_nr_attr;
        break;
    case 2:
        *(struct ak_audio_agc_attr*)audio_args = default_ai_agc_attr;
        break;
    case 3:
        *(struct ak_audio_aec_attr*)audio_args = default_ai_aec_attr;
        break;
    case 4:
        *(struct ak_audio_aslc_attr*)audio_args = default_ai_aslc_attr;
        break;
    case 5:
        *(struct ak_audio_nr_attr*)audio_args = default_ao_nr_attr;
        break;
    case 6:
        *(struct ak_audio_aslc_attr*)audio_args = default_ao_aslc_attr;
        break;
    case 7:
        *(struct ak_audio_eq_attr*)audio_args = default_ai_eq_attr;
        break;
    case 8:
        *(struct ak_audio_eq_attr*)audio_args = default_ao_eq_attr;
        break;
    default:
        break;
    }

    return;
}

/**
 * init_ao - open ao
 * notes:
 */
static int init_ao(void)
{
    struct ak_audio_out_param ao_param;
    ao_param.pcm_data_attr.sample_bits = 16;
    ao_param.pcm_data_attr.channel_num = channel_num;
    ao_param.pcm_data_attr.sample_rate = sample_rate;
    ao_param.dev_id = dev_sel;

	if (DEV_DAC == dev_sel) {
		if ((channel_num != 1) && (channel_num != 2)) {
			ak_print_error(MODULE_ID_APP, "the channel num of dac must use 1 or 2\n");
			return AK_FAILED;
		}
	} else if (DEV_I2S0_AO == dev_sel) {
		if (channel_num != 2) {
			ak_print_error(MODULE_ID_APP, "the channel num of i2s0 must use 2\n");
			return AK_FAILED;
		}
	} else {
		ak_print_error(MODULE_ID_APP, "device id must use 0-dac or 1-i2s0\n");
		return AK_FAILED;
	}

    /* open ao */
    if (ak_ao_open(&ao_param, &ao_handle_id)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_ao_open failed\n");
        return AK_FAILED;
    }
    
    if (ak_ao_set_gain(ao_handle_id, 6)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_ao_set_gain failed\n");
        return AK_FAILED;
    }
    
    if ((DEV_DAC == ao_param.dev_id) && (AUDIO_CHANNEL_MONO == channel_num)) {
        struct ak_audio_nr_attr nr_attr;
        setup_default_audio_argument(&nr_attr, 5);
        
        /* enable_nr, nr only support 8000 or 16000 sample rate */
        if (sample_rate != AK_AUDIO_SAMPLE_RATE_8000 && sample_rate != AK_AUDIO_SAMPLE_RATE_16000) {
            ak_print_warning_ex(MODULE_ID_APP, "ak_ai_enable_agc only support sample rate 8000 or 16000\n");
        } else {
            if (ak_ao_set_nr_attr(ao_handle_id, &nr_attr)) {
                ak_print_error_ex(MODULE_ID_APP, "ak_ao_set_nr_attr failed\n");
                return AK_FAILED;
            }
        }
    }

    struct ak_audio_aslc_attr ao_aslc_attr;
    setup_default_audio_argument(&ao_aslc_attr, 6);
    if (ak_ao_set_aslc_attr(ao_handle_id, &ao_aslc_attr)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_ao_set_aslc_attr failed\n");
        return AK_FAILED;
    }

    struct ak_audio_eq_attr ao_eq_attr;
    setup_default_audio_argument(&ao_eq_attr, 8);
    if (ak_ao_set_eq_attr(ao_handle_id, &ao_eq_attr)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_ao_set_eq_attr failed\n");
        return AK_FAILED;
    }

    return AK_SUCCESS;
}

/**
 * destroy_ao - close ao
 * notes: 
 */
static void destroy_ao(void)
{
    /* ao close */
    if (-1 != ao_handle_id) {
        ak_ao_close(ao_handle_id);
        ao_handle_id = -1;
    }
}

/**
 * init_adec - open adec
 * notes:
 */
static int init_adec(void)
{
    /* open audio encode */
    struct adec_param adec_param;
    adec_param.type = dec_type;
    adec_param.adec_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16;
    adec_param.adec_data_attr.channel_num = channel_num;
    adec_param.adec_data_attr.sample_rate = sample_rate;

    /* open adec */
    if (ak_adec_open(&adec_param, &adec_handle_id)) { // open
        ak_print_error_ex(MODULE_ID_APP, "ak_adec_open failed\n");
        return AK_FAILED;
    }

    return AK_SUCCESS;
}

/**
 * destroy_adec - close adec
 * notes: 
 */
static void destroy_adec(void)
{
    /* adec close */
    if (-1 != adec_handle_id) {
        ak_adec_close(adec_handle_id);
        adec_handle_id = -1;
    }
}

/**
 * read_play_file - read play file and then send to adec 
 * @arg[IN]: arg is NULL
 * notes: 
 */
static void* read_play_file(void *arg)
{    
    ak_thread_set_name("[adec read]attachment");

    unsigned int read_len = 0;
    unsigned int total_len = 0;
    unsigned char data[RECORD_READ_LEN] = {0};

    ak_print_normal_ex(MODULE_ID_APP, "\n\t thread create success \n");

    while (flag) {
        /* read the record file stream */
        memset(data, 0x00, RECORD_READ_LEN);
        read_len = fread(data, sizeof(char), RECORD_READ_LEN, fp);
        if (read_len > 0) {
            total_len += read_len;
            ak_adec_send_stream(adec_handle_id, data, read_len, 1);
            print_playing_dot();
        } else if (0 == read_len) {
            /* read to the end of file */
            send_frame_end = 1;
            ak_print_normal_ex(MODULE_ID_APP, "\n\tread to the end of file\n");
            break;
        } else {
            ak_print_error_ex(MODULE_ID_APP, "\nread, %s\n", strerror(errno));
            break;
        }
    }
    /* read over the file, must call ak_adec_read_end */
    ak_adec_send_stream_end(adec_handle_id);
    ak_print_notice_ex(MODULE_ID_APP, "%s exit\n", __func__);

    ak_thread_exit();
    return NULL;
}

/**
 * get_frame_then_play - get frame from adec and then send data to ao 
 * @arg[IN]: arg is NULL
 * notes: 
 */
static void* get_frame_then_play(void *arg)
{
    ak_thread_set_name("[adec play]attachment");

    struct frame pcm_frame = {0};
    int play_len = 0;

    while (1) {
        if (ak_adec_get_frame(adec_handle_id, &pcm_frame, 1)) {
            ak_print_normal_ex(MODULE_ID_APP, "get frame end\n");
            break;
        }

        if (pcm_frame.len == 0 || pcm_frame.data == NULL) {
            continue;
        }

        if (ak_ao_send_frame(ao_handle_id, pcm_frame.data, pcm_frame.len, &play_len)) {
            ak_adec_release_frame(adec_handle_id, &pcm_frame);
            break;
        }
        ak_adec_release_frame(adec_handle_id, &pcm_frame);
    }

    ak_print_notice_ex(MODULE_ID_APP, "%s exit\n", __func__);
    ak_thread_exit();
    return NULL;
}

/**
 * Preconditions:
 * 1. T card is already mounted
 * 2. speaker must ready
 */
#ifdef AK_RTOS
static int adec_sample(int argc, char *argv[])
#else
int main(int argc, char **argv)
#endif
{
    /* start the application */
    sdk_run_config config = {0};
    config.mem_trace_flag = SDK_RUN_NORMAL;
    ak_sdk_init( &config );

#ifdef AK_RTOS
    param_init();
#endif

    if (parse_option( argc, argv ) == 0) {
        goto main_end;
    }

    ak_print_normal(MODULE_ID_APP, "*****************************************\n");
    ak_print_normal(MODULE_ID_APP, "** adec version: %s **\n", ak_adec_get_version());
    ak_print_normal(MODULE_ID_APP, "** %s **\n", ak_adec_get_codec_info());
    ak_print_normal(MODULE_ID_APP, "** ao version: %s **\n", ak_ao_get_version());
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");

    if (init_ao()) {
        ak_print_error_ex(MODULE_ID_APP, "init ao failed\n");
        goto ao_end;
    }

    if (init_adec()) {
        ak_print_error_ex(MODULE_ID_APP, "init adec failed\n");
        goto adec_end;
    }

    /* open audio file */
    fp = open_play_file(play_file);
    if (NULL == fp) {
        goto adec_end;
    }

    ak_pthread_t send_stream_tid;
    ak_pthread_t play_tid;
    flag = 1;

    /* create record file thread */ 
    int ret = ak_thread_create(&(send_stream_tid), read_play_file,
                            NULL, ANYKA_THREAD_MIN_STACK_SIZE, 11);

    if (ret) {
        goto adec_end;
    }

    /* create get frame and play thread */
    ret = ak_thread_create(&(play_tid), get_frame_then_play,
                            NULL, ANYKA_THREAD_MIN_STACK_SIZE, 11);

    if (ret) {
        goto adec_end;
    }

    /* wait thread end */
    ak_thread_join(send_stream_tid);
    ak_thread_join(play_tid);

adec_end:
    destroy_adec();
    
ao_end:
    close_play_file(fp);
    destroy_ao();

main_end:
    ak_print_normal_ex(MODULE_ID_APP,"----- %s exit -----\n", argv[0]);
    ak_sdk_exit();
    return 0;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(adec_sample, ak_adec_sample, Audio Decode Sample Program);
#endif

/* end of file */
