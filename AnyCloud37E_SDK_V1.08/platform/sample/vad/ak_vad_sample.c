/**
* File Name: ak_ai_sample.c
* Description: This is a simple example to show how the AI module working.
* Notes:
*/

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <getopt.h>
#include <errno.h>

#include "ak_ai.h"
#include "ak_common.h"
#include "ak_log.h"
#include "ak_mem.h"
#include "ak_thread.h"

#ifdef AK_RTOS
#include <kernel.h>
#endif

#define LEN_HINT         512
#define LEN_OPTION_SHORT 512
#define DEV_STR_LEN      10
#define AK32Q15(x)    ((signed long)((x)*(1<<15)))

static void setup_default_audio_argument(void *audio_args, char args_type);

static const char save_def_parth[] = "/mnt/";
static FILE *fp = NULL;
static int sample_rate = 8000;
static int volume = 5;
static int save_time = 20000;        // set save time(ms)
static char *save_path= (char *)save_def_parth;    // set save path
static int channel_num = AUDIO_CHANNEL_MONO;
static int dev_sel = DEV_ADC;
static int interval = 1000; // detect interval
static int frame_interval = 32;
static int ai_handle_id = -1;
static int vad_thresh = AK32Q15(0.05);
static int probe_mode = 0;// probe_mode=1 is vad probe mode, probe_mode=0 is vad normal mode
static float vad_thresh_fs_sum = 0;// vad thresh in FS, vad thresh in FS is vad_thresh/AK32Q15(1), 10 times vad thresh in FS add together


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
    "	采集时间" ,
    "	采样率[8000 12000 11025 16000 22050 24000 32000 44100 48000]" ,
    "	通道数[1 2]" ,
    "	音量[-90~20db]" ,
    "	文件保存路径" ,
    "	设备选择，0-adc，1-pdm, 2-I2S0" ,
    "	声音侦测间隔,默认(1000)" ,
    "	侦测声音阈值,默认(16384)" ,    
    "	声音侦测调试模式，probe_mode=1 使用声音侦测调试模式，probe_mode=0 使用声音侦测普通模式" ,
    ""
};

static struct option option_long[] = {
    { "help"        , no_argument       , NULL , 'h' } , //"       打印帮助信息" ,
    { "second"      , required_argument , NULL , 't' } , //"[SEC]  采集时间" ,
    { "sample-rate" , required_argument , NULL , 's' } , //"[NUM]  采样率[8000 16000 32000 44100 48000]" ,
    { "channel-number" , required_argument , NULL , 'c' } , //"[NUM]  通道数[1 2]" ,
    { "volume"      , required_argument , NULL , 'v' } , //"[NUM]  数字音量[-90~20db]" ,
    { "path"        , required_argument , NULL , 'p' } , //"[PATH] 文件保存路径" ,
    { "dev-sel"     , required_argument , NULL , 'd' } , //"[DEV_SEL] 输入设备选择" ,
    { "interval"    , required_argument , NULL , 'm' } , //"侦测的时间间隔单位ms 默认(1000)" ,
    { "vad_thresh"  , required_argument , NULL , 'n' } , //"侦测声音阈值，单位sample" ,    
    { "probe_mode"  , required_argument , NULL , 'r' } , //"声音侦测调试模式" ,
    {0, 0, 0, 0}
};


/*
 * help_hint: use the -h --help option.Print option of help information
 * return: 0
 */
static int help_hint( char *pc_prog_name )
{
    int i;
    printf( "version: \r\n  %s \n\n", ak_ai_get_version());
    printf( "usage: \r\n  %s  [options] <value>\n\n", pc_prog_name);
    printf( "options: \r\n");
    for(i = 0; i < sizeof(option_long) / sizeof(struct option) - 1; i++) {
        printf("  -%c,--%-16s\t%s \n" , option_long[i].val , option_long[i].name , ac_option_hint[i]);
    }
    printf( "\nexample: \r\n"
    "  %s -t 60 -s 8000 -c 1 -v 5 -d 0 -m 100 -p /mnt/ -n 16384\n", pc_prog_name);
    printf("\n\n");
    return 0;
}

static char *get_option_short( struct option *p_option, int i_num_option, char *pc_option_short,
                        int i_len_option )
{
    int i;
    int i_offset = 0;
    char c_option;

    for (i = 0; i < i_num_option; i++) {
        c_option = p_option[i].val;
        switch (p_option[i].has_arg) {
        case no_argument:
            i_offset += snprintf(pc_option_short + i_offset, i_len_option - i_offset, "%c", c_option);
            break;
        case required_argument:
            i_offset += snprintf( pc_option_short + i_offset, i_len_option - i_offset, "%c:", c_option);
            break;
        case optional_argument:
            i_offset += snprintf( pc_option_short + i_offset, i_len_option - i_offset, "%c::", c_option);
            break;
        }
    }
    return pc_option_short;
}

static int parse_option(int argc, char **argv)
{
    int i_option;
    char ac_option_short[LEN_OPTION_SHORT];
    int i_array_num = sizeof(option_long) / sizeof(struct option) ;
    char c_flag = 1;

    if (argc < 5 && argc != 1) {
        help_hint(argv[0]);
        c_flag = 0;
        goto parse_option_end;
    }

#ifdef AK_RTOS
    optind = 0;
#endif
    get_option_short(option_long, i_array_num , ac_option_short , LEN_OPTION_SHORT);
    while ((i_option = getopt_long(argc , argv , ac_option_short , option_long , NULL)) > 0) {
        switch (i_option) {
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
            case 'c' :                                                          //channel number
                channel_num = atoi(optarg);
                break;
            case 'v' :                                                          //volume
                volume = atoi(optarg);
                break;
            case 'd':                                                           // choose device
                dev_sel = atoi(optarg);
                break;
            case 'p' :                                                          //path
                save_path = optarg;
                break;
            case 'm' :                                                          // voice detect interval
                interval = atoi(optarg);
                break;
            case 'n' :                                                          // voice detect thresh
                vad_thresh = atoi(optarg);
                break;
            case 'r' :                                                          // voice detect probe mode
                probe_mode = atoi(optarg);
                break;

        }
    }
parse_option_end:
    return c_flag;
}

/*
 * open_pcm_file: open pcm file.
 * sample_rate[IN]: audio input sample rate.
 * channel_num[IN]: audio input channle number.
 * path[IN]: pointer to the path which will be checking.
 * fp[OUT]: pointer of opened pcm file.
 * save_time[IN]: audio input save time.
 * dev_sel[IN]: audio input device id, see enum ak_audio_ai_dev_id.
 * return: 0 is success, -1 is error.
 */
static int open_pcm_file(int sample_rate, int channel_num, const char *path, FILE **fp, int save_time, int dev_sel)
{
    struct stat s = {0};
    if (stat(path, &s) != 0) {
        printf("stat error. path:%s, errno:%d\n", path, errno);
        return -1;
    }

    if (S_ISDIR(s.st_mode) == 0) {
        printf("path is not dir. path:%s\n", path);
        return -1;
    }

    struct ak_date date;
    ak_get_localdate(&date);
    char time_str[20] = {0};
    ak_date_to_string(&date, time_str);
    char file_path[255] = {0};
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

    sprintf(file_path, "%s/%s_%d_%d_%d_%s.pcm", path, time_str, sample_rate, channel_num, save_time, dev);

    /* open file */
    *fp = fopen(file_path, "wb");
    if (NULL == *fp) {
        ak_print_normal(MODULE_ID_APP, "open pcm file: %s error\n", file_path);
        return -1;
    } else {
        ak_print_normal(MODULE_ID_APP, "open pcm file: %s OK\n", file_path);
        return 0;
    }
}

/*
 * close_pcm_file: close pcm file.
 * fp[IN]: pointer of opened pcm file.
 * return: void.
 */
static void close_pcm_file(FILE *fp)
{
    /* close file handle */
    if (NULL != fp) {
        fclose(fp);
        fp = NULL;
    }
}

/*
 * probe_mode_calc_avg: when in probe_mode, calculate the average of vadThresh
 * probe_count[IN]: probe mode counter
 * return: void.
 */
static void probe_mode_calc_avg(int probe_count, struct ak_audio_vad_status_ex *vad_status)
{
    float vad_thresh_avg = 0;
    float vad_thresh_fs = (float)vad_status->vadThresh / AK32Q15(1);

    if (10 <= probe_count && 20 > probe_count) { // 10 times
        vad_thresh_fs_sum += vad_thresh_fs;
        ak_print_normal_ex(MODULE_ID_APP, "vad probe: vadThresh = %.4f FS @ %lldms\n", vad_thresh_fs, (vad_status->ts / 1000));
    }
    if (20 == probe_count) {
        vad_thresh_avg = vad_thresh_fs_sum / 10;
        ak_print_notice_ex(MODULE_ID_APP, "vad_thresh_avg_fs = %.4f FS, vad_thresh = %ld\n", vad_thresh_avg, AK32Q15(vad_thresh_avg));
    }
}

/*
 * ai_capture_loop: loop to get and release pcm data, between get and release,
 *                  here we just save the frame to file, on your platform,
 *                  you can rewrite the save_function with your code.
 */
static void* ai_capture_loop(void *arg)
{ 
    ak_thread_set_name("[vad sample]attachment");
    unsigned long long start_ts = 0;// use to save capture start time
    unsigned long long end_ts = 0;    // the last frame time
    unsigned long long trigger_ts = 0;
    unsigned long long temp_ts = 0;
    struct frame frame = {0};
    int ret = AK_FAILED;
    int probe_count = 0;
    
    struct ak_audio_vad_status_ex vad_status = {0};
    int sleep_time = frame_interval == 0 ? 16: (frame_interval / 2);
    ak_print_normal_ex(MODULE_ID_APP, "sleep_time=%d\n", sleep_time);

    ak_print_normal(MODULE_ID_APP, "*** capture start ***\n");
    ret = ak_ai_start_capture(ai_handle_id);
    if (ret) {
        ak_print_error(MODULE_ID_APP, "*** ak_ai_start_capture failed. ***\n");
        return NULL;
    }

    while (1) {
        /* get the pcm data frame */
        ret = ak_ai_get_frame(ai_handle_id, &frame, 0);
        if (ret) {
            if (ERROR_AI_NO_DATA == ret) {
                ak_sleep_ms(10);
                continue;
            } else {
                break;
            }
        }

        if (!frame.data || frame.len <= 0) {
            ak_sleep_ms(10);
            continue;
        }

        if (NULL != fp) {
            if (fwrite(frame.data, frame.len, 1, fp) < 0) {
                ak_ai_release_frame(ai_handle_id, &frame);
                ak_print_normal(MODULE_ID_APP, "write file error.\n");
                break;
            }
        }

        /* save the begin time */
        if (0 == start_ts) {
            start_ts = frame.ts;
            end_ts = frame.ts;
            temp_ts = frame.ts;
        }
        if (frame.ts - temp_ts > interval) {
            ak_ai_get_vad_status_ex(ai_handle_id, &vad_status);
            if (vad_status.ts != trigger_ts) {
                trigger_ts = vad_status.ts;
                if (probe_mode) {
                    probe_count++;                 
                    probe_mode_calc_avg(probe_count, &vad_status);
                } else {
                    ak_print_normal_ex(MODULE_ID_APP, "vad: trigger = %d, vadThresh =%ld @ %lldms\n", 
                                    vad_status.trigger, vad_status.vadThresh, vad_status.ts / 1000);
                }
            }
            temp_ts = frame.ts;
        }
        end_ts = frame.ts;

        ak_ai_release_frame(ai_handle_id, &frame);

        if (probe_mode) {
            if (20 <= probe_count) {
                break;
            }
        } else {
            /* time is up */
            if ((end_ts - start_ts) >= save_time) {
                ak_print_normal(MODULE_ID_APP, "*** timp is up, start_ts = %lld, end_ts = %lld, save_time = %d ***\n\n",
                            start_ts, end_ts, save_time);
                break;
            }
        }
    }
    ak_ai_stop_capture(ai_handle_id);
    ak_print_normal_ex(MODULE_ID_APP, "*** capture finish ***\n\n");
    ak_thread_exit();
    return NULL;
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
    ak_print_notice_ex(MODULE_ID_APP, "capture sample_rate=%d\n", AK_AUDIO_SAMPLE_RATE_8000);
    param.pcm_data_attr.sample_rate = AK_AUDIO_SAMPLE_RATE_8000;              // set sample rate
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
 * set_vad_param - set vad parameter
 * notes: 
 */
static int set_vad_param(void)
{
    struct ak_audio_vad_attr vad_attr = {0};
    vad_attr.enable = 1;                 // enable voice detect attribute
    vad_attr.vad_thresh = vad_thresh; // threshold of signal amplitude. use the form of AK32Q15(x.xx) to set it
    vad_attr.vad_level = 0;              // sensitivity level. [0, 4], smaller is more sensible
    if (probe_mode)
        vad_attr.vad_type = AK_VAD_DET_AFTER_ADC | AK_VAD_DET_PROBE;
    else
        vad_attr.vad_type = AK_VAD_DET_AFTER_ADC;
    return ak_ai_set_vad_attr(ai_handle_id, &vad_attr);
}


#ifdef AK_RTOS
static void param_init(void)
{
    fp = NULL;
    sample_rate = 8000;
    volume = 5;
    save_time = 20000;		 // set save time(ms)
    save_path= (char *)save_def_parth;    // set save path
    channel_num = AUDIO_CHANNEL_MONO;
    dev_sel = DEV_ADC;
}
#endif

/**
 * Preconditions:
 * 1. T card is already mounted
 * 2. mic or linein must ready
 */
#ifdef AK_RTOS
static int vad_sample(int argc, char *argv[])
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

    if (parse_option(argc, argv) == 0){
        goto exit;
    }
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");
    ak_print_normal(MODULE_ID_APP, "** ai version: %s **\n", ak_ai_get_version());
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");

    if (init_ai()) {
        ak_print_error_ex(MODULE_ID_APP, "init ai failed\n");
        goto ai_end;
    }

    if (set_vad_param()) {
        ak_print_error_ex(MODULE_ID_APP, "init vqe failed\n");
        goto ai_end;
    }

    if (0 > open_pcm_file(sample_rate, channel_num, save_path, &fp, save_time,dev_sel)) {
        goto ai_end;
    }
    
    int ret = -1;
    ak_pthread_t capture_loop_tid;
    ret = ak_thread_create(&(capture_loop_tid), ai_capture_loop, NULL, ANYKA_THREAD_MIN_STACK_SIZE, 19);
    if (ret) {
        goto ai_end;
    }

    /* wait thread end */
    ak_thread_join(capture_loop_tid);

ai_end:
    destroy_ai();
    close_pcm_file(fp);

exit:
    ak_print_normal(MODULE_ID_APP, "******** exit ai demo ********\n");
    ak_sdk_exit();
    return ret;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(vad_sample, ak_vad_sample, voice active detection sample)
#endif

