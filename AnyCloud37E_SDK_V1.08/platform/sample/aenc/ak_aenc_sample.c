/**
* File Name: ak_aenc_sample.c
* Description: This is a simple example to show how the AENC module working.
* Notes:
*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>


#include "ak_ai.h"
#include "ak_aenc.h"
#include "ak_thread.h"
#include "ak_log.h"

#ifdef AK_RTOS
#include <kernel.h>
#endif

#define LEN_HINT         512
#define LEN_OPTION_SHORT 512
#define DEV_STR_LEN      10


static const char save_def_parth[] = "/mnt/";

static FILE *fp;  /* save file handle */
int flag = -1;    /*get aenc stream flag */
static int ai_handle_id = 0;
static int aenc_handle_id = -1;

#ifdef AK_RTOS
static int send_frame_end = 0;
#endif

static int sample_rate = 8000;
static int channel_num = 1;
static int enc_type = AK_AUDIO_TYPE_MP3;
static int save_time = 20000;        // set save time(ms)
static char *save_path = (char *)save_def_parth;    // set save path
static int save_file = 1;
static int volume = 0;
static int dev_sel = DEV_ADC;
static int frame_interval = 32;


/*
    ***********************************
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

static const char ac_option_hint[  ][ LEN_HINT ] = {
    "	打印帮助信息" ,
    "	采集时间，单位：秒" ,
    "	采样率[8000 12000 11025 16000 22050 24000 32000 44100 48000]" ,
    "	通道数[1,2]" ,
    "	编码类型[mp3 aac amr g711a g711u pcm]" ,
    "	文件保存路径" ,
    "	是否保存文件[0:不保存文件，1:保存文件]" ,
    "	音量[-90~20db]" ,
    "	设备选择，0-adc，1-pdm, 2-I2S0" ,
    ""
};

static struct option option_long[ ] = {
    { "help"        , no_argument       , NULL , 'h' } , //"       打印帮助信息" ,
    { "second"      , required_argument , NULL , 't' } , //"[SEC]  采集时间" ,
    { "sample-rate" , required_argument , NULL , 's' } , //"[NUM]  采样率[8000 16000 32000 44100 48000]" ,
    { "channel"     , required_argument , NULL , 'c' } , //"[NUM]  音量[1 2]" ,
    { "eccode-type" , required_argument , NULL , 'e' } , //"[TYPE]    编码类型[mp3 aac amr g711a g711u pcm]" ,
    { "path"        , required_argument , NULL , 'p' } , //"[PATH] 文件保存路径" ,
    { "save file"   , required_argument , NULL , 'f' } , //"[NUM] save file or not
    { "volume"      , required_argument , NULL , 'v' } , //"[NUM]  数字音量[-90~20db]" ,
    { "dev-sel"     , required_argument , NULL , 'd' } , //"[DEV_SEL] 输入设备选择" ,
    {0, 0, 0, 0}
};


/**
 * parse_aenc_type - get audio file type 
 * @aenc_type[IN]: encode type [mp3/amr/aac/g711a/g711u/pcm]
 * notes: return audio type number
 */
static int parse_aenc_type(const char *aenc_type)
{
    int ret = -1;
    char type[10] = {0};
    int type_len = strlen(aenc_type);

    /* the type length <= 5, such as g711u */
    if (type_len <= 5) {
        sprintf(type, "%s", aenc_type);

        int i = 0;
        for (i=0; i<type_len; ++i) {
            type[i] = tolower(type[i]);
        }
        ak_print_notice_ex(MODULE_ID_APP, "audio encode type: %s\n", type);

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
        else
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

    printf("version: \r\n  %s \n\n", ak_aenc_get_version());
    printf("usage: \r\n  %s  [options] <value>\n\n", pc_prog_name);
    printf("options: \r\n");

    for(i = 0; i < sizeof(option_long) / sizeof(struct option)-1; i ++) {
        printf("  -%c,--%-16s\t%s \n" , option_long[i].val , option_long[i].name , ac_option_hint[i]);
    }
    printf("\nexample: \r\n"
    "  %s -t 60 -s 8000 -c 1 -e mp3 -p /mnt/\n", pc_prog_name);
    printf("\n\n");

    return 0;
}

static char *get_option_short(struct option *p_option, int i_num_option, char *pc_option_short, int i_len_option)
{
    int i;
    int i_offset = 0;
    char c_option;

    for (i = 0; i < i_num_option; i ++) {
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
    char *type= "mp3";    // set save path

    if (argc < 6 && argc != 1) {
        help_hint(argv[0]);
        c_flag = 0;
        goto parse_option_end;
    }

#ifdef AK_RTOS
    optind = 0;
    int i;

    for (i=1; i<argc; i=i+2) {
        if(argv[i][0] == '-') {
            continue;
        } else {
            help_hint(argv[0]);
            return 0;
        }
    }

#endif
    get_option_short(option_long, i_array_num , ac_option_short , LEN_OPTION_SHORT);
    while ((i_option = getopt_long(argc , argv , ac_option_short , option_long , NULL)) > 0) {
        switch(i_option) {
        case 'h' :                                                          //help
            help_hint(argv[0]);
            c_flag = 0;
            goto parse_option_end;
        case 't' :                                                          //second
            save_time = atoi(optarg) * 1000 ;
            break;
        case 's' :                                                          //sample-rate
            sample_rate = atoi(optarg);
            break;
        case 'c' :                                                          //volume
            channel_num = atoi(optarg);
            printf("----channel_num- %d-----\n", channel_num);
            break;
        case 'e' :                                                          //volume
            type = optarg;
            printf("----enc_type- %s-----\n", type);
            enc_type = parse_aenc_type(type);
            printf("---nnn  -enc_type- %d-----\n", enc_type);
            break;
        case 'p' :                                                          //path
            save_path = optarg;
            break;
        case 'f' :                                                          //path
            save_file = atoi(optarg);
            break;
        case 'v' :                                                          //volume
            volume = atoi(optarg);
            break;
        case 'd':
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
    ai_handle_id = -1;
    aenc_handle_id = -1;
    send_frame_end = 0;

    sample_rate = 8000;
    channel_num = 1;
    enc_type = AK_AUDIO_TYPE_MP3;
    save_time = 20000;		 // set save time(ms)
    save_path= (char *)save_def_parth;    // set save path
}
#endif

/**
 * print_recording_dot - print dot when recording
 * notes: print one dot for 10 seconds
 */
static void print_recording_dot(void)
{
    static struct ak_timeval cur_time = {0};
    static struct ak_timeval print_time = {0};

    ak_get_ostime(&cur_time);
    if (ak_diff_ms_time(&cur_time, &print_time) >= 10000) {
        print_time = cur_time;
        ak_print_normal(MODULE_ID_APP, ".");
    }
}

/**
 * open_record_file - create the audio record file
 * @enc_type[IN]: encode type [mp3/amr/aac/g711a/g711u/pcm]
 * @sample_rate[IN]: audio sample rate
 * @channel_num[IN]: audio channel number
 * @save_time[IN]:  audio file save time, unit: sencond
 * notes:
 */
static int open_record_file(int enc_type, int sample_rate, int channel_num, int save_time, int dev_sel)
{
    char time_str[20] = {0};
    char file_path[255] = {0};
    struct ak_date date;

    char dev[DEV_STR_LEN] = {0};

    if (DEV_ADC == dev_sel) {
        strncpy(dev, "ADC", DEV_STR_LEN);
    } else if (DEV_PDM == dev_sel) {
        strncpy(dev, "PDM", DEV_STR_LEN);
    } else if (DEV_I2S0_AI== dev_sel) {
        strncpy(dev, "I2S0", DEV_STR_LEN);
    } else if (DEV_I2S1_AI == dev_sel) {
        strncpy(dev, "I2S1", DEV_STR_LEN);
    }
    
    /* get the current time */
    ak_get_localdate(&date);
    ak_date_to_string(&date, time_str);

    /* set the file full path */
    switch (enc_type) {
    case AK_AUDIO_TYPE_AAC:
        sprintf(file_path, "%s%s_%d_%d_%d_%s.aac", save_path, time_str, sample_rate, channel_num, save_time, dev);
        break;
    case AK_AUDIO_TYPE_AMR:
        sprintf(file_path, "%s%s_%d_%d_%d_%s.amr", save_path, time_str, sample_rate, channel_num, save_time, dev);
        break;
    case AK_AUDIO_TYPE_MP3:
        sprintf(file_path, "%s%s_%d_%d_%d_%s.mp3", save_path, time_str, sample_rate, channel_num, save_time, dev);
        break;
    case AK_AUDIO_TYPE_PCM_ALAW:
        sprintf(file_path, "%s%s_%d_%d_%d_%s.g711a", save_path, time_str, sample_rate, channel_num, save_time, dev);
        break;
    case AK_AUDIO_TYPE_PCM_ULAW:
        sprintf(file_path, "%s%s_%d_%d_%d_%s.g711u", save_path, time_str, sample_rate, channel_num, save_time, dev);
        break;
    case AK_AUDIO_TYPE_PCM:
        sprintf(file_path, "%s%s_%d_%d_%d_%s.pcm", save_path, time_str, sample_rate, channel_num, save_time, dev);
        break;
    default:
        sprintf(file_path, "%s%s_%d_%d_%d_%s", save_path, time_str, sample_rate, channel_num, save_time, dev);
        break;
    }

    fp = fopen(file_path, "wb");
    if (NULL == fp) {
        ak_print_error_ex(MODULE_ID_APP, "create file err\n");
        return AK_FAILED;
    }

    /* write AMR file head */
    if (AK_AUDIO_TYPE_AMR == enc_type) {
        const unsigned char amrheader[]= "#!AMR\n";
        fwrite(amrheader, sizeof(amrheader) - 1, 1, fp);
    }
    ak_print_normal_ex(MODULE_ID_APP, "create amr file: %s OK\n", file_path);
    return AK_SUCCESS;
}

/*
 * close_record_file: close record file.
 * fp[IN]: pointer of opened record file.
 * return: void.
 */
static void close_record_file(FILE *fp)
{
    if(fp && save_file) {
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
    struct ak_audio_aslc_attr   default_ai_aslc_attr    = {32768, volume, 0};
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
    struct ak_audio_aslc_attr   default_ao_aslc_attr    = {32768, 0, 0};
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
 * init_ai - open ai, set nr, set agc, set gain, set volume
 * notes: 
 */
static int init_ai(void)
{
    struct ak_audio_in_param param;
    memset(&param, 0, sizeof(struct ak_audio_in_param));
    ak_print_notice_ex(MODULE_ID_APP, "sample_rate=%d\n", sample_rate);
    param.pcm_data_attr.sample_rate = sample_rate;              // set sample rate
    param.pcm_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16;    // sample bits only support 16 bit
    param.pcm_data_attr.channel_num = channel_num;              // channel number
    param.dev_id = dev_sel;                                     //DEV_ADC;

    if (ak_ai_open(&param, &ai_handle_id)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_ai_open failed\n");
        return AK_FAILED;
    }

    /* set source, source include mic and linein */
    if (DEV_ADC == dev_sel) {
        if (ak_ai_set_source(ai_handle_id, AI_SOURCE_MIC)) {
            ak_print_error_ex(MODULE_ID_APP, "ak_ai_set_source failed\n");
            return AK_FAILED;
        }

        /* enable_nr, nr only support 8000 or 16000 sample rate */
        struct ak_audio_nr_attr nr_attr;
        setup_default_audio_argument(&nr_attr, 1);
        if (sample_rate != AK_AUDIO_SAMPLE_RATE_8000 && sample_rate != AK_AUDIO_SAMPLE_RATE_16000) {
            ak_print_warning_ex(MODULE_ID_APP, "ak_ai_set_nr_attr only support sample rate 8000 or 16000\n");
        } else {
            if (ak_ai_set_nr_attr(ai_handle_id, &nr_attr)) {
                ak_print_error_ex(MODULE_ID_APP, "ak_ai_set_nr_attr failed\n");
                return AK_FAILED;
            }
        }

        /*enable_agc, agc only support 8000 or 16000 sample rate */ 
        struct ak_audio_agc_attr agc_attr;
        setup_default_audio_argument(&agc_attr, 2);
        if (sample_rate != AK_AUDIO_SAMPLE_RATE_8000 && sample_rate != AK_AUDIO_SAMPLE_RATE_16000) {
            ak_print_warning_ex(MODULE_ID_APP, "ak_ai_set_agc_attr only support sample rate 8000 or 16000\n");
        } else {
            if (ak_ai_set_agc_attr(ai_handle_id, &agc_attr)) {
                ak_print_error_ex(MODULE_ID_APP, "ak_ai_set_agc_attr failed\n");
                return AK_FAILED;
            }
        }

        /*enable_aec, aec only support 8000 or 16000 sample rate, aec will real open when ai and ao all open */ 
        struct ak_audio_aec_attr aec_attr;
        setup_default_audio_argument(&aec_attr, 3);
        if (sample_rate != AK_AUDIO_SAMPLE_RATE_8000 && sample_rate != AK_AUDIO_SAMPLE_RATE_16000) {
            ak_print_warning_ex(MODULE_ID_APP, "ak_ai_set_aec_attr only support sample rate 8000 or 16000\n");
        } else {
            if (ak_ai_set_aec_attr(ai_handle_id, &aec_attr)) {
                ak_print_error_ex(MODULE_ID_APP, "ak_ai_set_aec_attr failed\n");
                return AK_FAILED;
            }
        }
    }

    if (DEV_PDM == dev_sel) {
        ak_print_warning_ex(MODULE_ID_APP, "ak_ai_set_gain no support PDM\n");
    } else {
        if (ak_ai_set_gain(ai_handle_id, 5)) {
            ak_print_error_ex(MODULE_ID_APP, "ak_ai_set_gain failed\n");
            return AK_FAILED;
        }
    }

    struct ak_audio_aslc_attr ai_aslc_attr;
    setup_default_audio_argument(&ai_aslc_attr, 4);
    if (ak_ai_set_aslc_attr(ai_handle_id, &ai_aslc_attr)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_ai_set_aslc_attr failed\n");
        return AK_FAILED;
    }

    struct ak_audio_eq_attr ai_eq_attr;
    setup_default_audio_argument(&ai_eq_attr, 7);
    if (ak_ai_set_eq_attr(ai_handle_id, &ai_eq_attr)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_ai_set_eq_attr failed\n");
        return AK_FAILED;
    }

    int frame_len = 0;
    if (ak_ai_get_frame_length(ai_handle_id, &frame_len)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_ai_set_frame_length failed\n");
        return AK_FAILED;
    }
    frame_interval = ak_audio_len_to_interval(&param.pcm_data_attr, frame_len);

    return AK_SUCCESS;
}

/**
 * destroy_ai - close ai
 * notes: 
 */
static void destroy_ai(void)
{
    if (-1 != ai_handle_id) {
        /* ai close */
        ak_ai_close(ai_handle_id);
        ai_handle_id = -1;
    }
}

/**
 * init_aenc - open aenc
 * notes: 
 */
static int init_aenc(void)
{
    /* open audio encode */
    struct aenc_param aenc_param;
    aenc_param.type = enc_type;
    aenc_param.aenc_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16;
    aenc_param.aenc_data_attr.channel_num = channel_num;
    aenc_param.aenc_data_attr.sample_rate = sample_rate;

    if (ak_aenc_open(&aenc_param, &aenc_handle_id)) {
        ak_print_error_ex(MODULE_ID_APP, "ak_aenc_open failed\n");
        return AK_FAILED;
    }

    if (aenc_param.type == AK_AUDIO_TYPE_AAC) {
        struct aenc_attr attr;
        attr.aac_head = AENC_AAC_SAVE_FRAME_HEAD;
        if (ak_aenc_set_attr(aenc_handle_id, &attr)) {
            ak_print_error_ex(MODULE_ID_APP, "ak_aenc_set_attr failed\n");
            return AK_FAILED;
        }
    }
    return AK_SUCCESS;
}

/**
 * destroy_aenc - close aenc
 * notes: 
 */
static void destroy_aenc(void)
{
    if (-1 != aenc_handle_id) {
        /* aenc close */
        ak_aenc_close(aenc_handle_id);
        aenc_handle_id = -1;
    }
}

/**
 * get_stream - aenc get stream thread 
 * @arg[IN]: arg is NULL
 * notes: 
 */
static void* get_stream(void *arg)
{
    ak_thread_set_name("[aenc stream]attachment");

    struct audio_stream stream = {0};
    int ret = -1;
    int sleep_time = frame_interval == 0 ? 16: (frame_interval / 2);
    ak_print_normal_ex(MODULE_ID_APP, "sleep_time=%d\n", sleep_time);

    while(1 == flag) {
        ret = ak_aenc_get_stream(aenc_handle_id, &stream, 0);
        if (ret) {
            if (ERROR_AENC_NO_DATA == ret) {
                ak_sleep_ms(sleep_time);
                continue;
            } else {
                ak_print_error_ex(MODULE_ID_APP, "ak_aenc_get_stream error ret=%d\n", ret);
                break;
            }
        } else {
            if (stream.data && stream.len) {
                if (save_file) {
                    fwrite(stream.data, stream.len, 1, fp);
                }
                print_recording_dot();
            } else {
                continue;
            }
        }
        ak_aenc_release_stream(aenc_handle_id, &stream);
    }

    ak_print_notice_ex(MODULE_ID_APP, "%s exit\n", __func__);
    ak_thread_exit();
    return NULL;
}

/**
 * get_frame_encode - ai get frame and then send to aenc encode 
 * @arg[IN]: arg is NULL
 * notes: 
 */
static void* get_frame_encode(void *arg)
{
    ak_thread_set_name("[aenc frame]attachment");

    struct frame ai_frame = {0};
    unsigned long long start_ts = 0;// use to save capture start time
    unsigned long long end_ts = 0;  // the last frame time

    int ret = AK_FAILED;
    int sleep_time = frame_interval == 0 ? 16: (frame_interval / 2);
    ak_print_normal_ex(MODULE_ID_APP, "sleep_time=%d\n", sleep_time);

    ret = ak_ai_start_capture(ai_handle_id);
    if (ret) {
        ak_print_error(MODULE_ID_APP, "*** ak_ai_start_capture failed. ***\n");
        return NULL;
    }

    while (1) {
        ret = ak_ai_get_frame(ai_handle_id, &ai_frame, 0);
        if (ret) {
            if (ERROR_AI_NO_DATA == ret) {
                ak_sleep_ms(sleep_time);
                continue;
            } else {
                ak_print_error_ex(MODULE_ID_APP, "ak_ai_get_frame error ret=%d\n", ret);
                flag = 0;
                break;
            }
        }
        ret = ak_aenc_send_frame(aenc_handle_id, &ai_frame, 0);
        if (ret) {
            ak_ai_release_frame(ai_handle_id, &ai_frame);
            ak_print_error_ex(MODULE_ID_APP, "send frame error\n");
            flag = 0;
            break;
        }

        if (0 == start_ts) {
            start_ts = ai_frame.ts;
        }

        end_ts = ai_frame.ts;
        ak_ai_release_frame(ai_handle_id, &ai_frame);
        /* time is up */
        if ((end_ts - start_ts) >= save_time) {
            ak_print_notice_ex(MODULE_ID_APP, "time is up\n");
            flag = 0;
            break;
        }
    }
    ak_ai_stop_capture(ai_handle_id);
    ak_print_notice_ex(MODULE_ID_APP, "get_frame_encode exit\n");
    ak_thread_exit();
    return NULL;
}

/**
 * Preconditions:
 * 1. T card is already mounted
 * 2. mic or linein must ready
 */
#ifdef AK_RTOS
static int aenc_sample(int argc, char *argv[])
#else
int main(int argc, char **argv)
#endif
{
    /* start the application */
    sdk_run_config config = {0};
    config.mem_trace_flag = SDK_RUN_NORMAL;
    ak_sdk_init(&config);

#ifdef AK_RTOS
    param_init();
#endif

    if (parse_option(argc, argv) == 0) {
        goto main_end;
    }

    ak_print_normal(MODULE_ID_APP, "*****************************************\n");
    ak_print_normal(MODULE_ID_APP, "** aenc version: %s **\n", ak_aenc_get_version());
    ak_print_normal(MODULE_ID_APP, "** %s **\n", ak_aenc_get_codec_info());
    ak_print_normal(MODULE_ID_APP, "** ai version: %s **\n", ak_ai_get_version());
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");

    if (init_ai()) {
        ak_print_error_ex(MODULE_ID_APP, "init ai failed\n");
        goto ai_end;
    }

    if (init_aenc()) {
        ak_print_error_ex(MODULE_ID_APP, "init aenc failed\n");
        goto aenc_end;
    }

    if (save_file) {
        if (0 > open_record_file(enc_type, sample_rate, channel_num, save_time, dev_sel)) {
            ak_print_error_ex(MODULE_ID_APP,"create_record_file create err!\n");
            goto aenc_end;
        }
    }

    ak_pthread_t encode_tid;
    ak_pthread_t get_tid;

    int ret = -1;
    flag = 1;
    ret = ak_thread_create(&(get_tid), get_stream, NULL, ANYKA_THREAD_MIN_STACK_SIZE, 19);
    if (ret) {
        goto aenc_end;
    }
    ret = ak_thread_create(&(encode_tid), get_frame_encode, NULL, ANYKA_THREAD_MIN_STACK_SIZE, 19);
    if (ret) {
        goto aenc_end;
    }

    ak_thread_join(get_tid);
    ak_thread_join(encode_tid);

aenc_end:
    destroy_aenc();

ai_end:
    close_record_file(fp);
    destroy_ai();
    
main_end:
    ak_print_notice_ex(MODULE_ID_APP, "----- %s exit -----\n", argv[0]);
    ak_sdk_exit();
    return 0;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(aenc_sample, ak_aenc_sample, Audio Encode Sample Program);
#endif

/* end of file */


