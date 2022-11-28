/**
* File Name: ak_ao_sample.c
* Description: This is a simple example to show how the AO module working.
* Notes:
*/
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <getopt.h>

#include "ak_ao.h"
#include "ak_common.h"
#include "ak_log.h"
#include "ak_mem.h"
#include "ak_thread.h"


#ifdef AK_RTOS
#include <kernel.h>
#endif

#define PCM_READ_LEN            4096
#define LEN_HINT                512
#define LEN_OPTION_SHORT        512
#define RFILE_CACHE_LEN         (10240)

static const char pcm_def_parth[] = "/mnt/ak_ao_test.pcm";
static int sample_rate = 8000;
static int volume = 3;
static char *pcm_file = (char *)pcm_def_parth;
static int channel_num = AUDIO_CHANNEL_MONO;
static int dev_sel = DEV_DAC;
static int ao_handle_id = -1;
static FILE *fp = NULL;


static const char ac_option_hint[][LEN_HINT] = {
    "	打印帮助信息" ,
    "	采样率[ 8000 12000 11025 16000 22050 24000 32000 44100 48000]" ,
    "	通道数[1 2]" ,
    "	音量[-90~20db]" ,
    "	播放的文件名称" ,
    "	设备选择，0-dac，1-i2s0" ,
    ""
};

static struct option option_long[] = {
    { "help"        , no_argument       , NULL , 'h' } , //"       打印帮助信息" ,
    { "sample-rate" , required_argument , NULL , 's' } , //"[NUM]  采样率[8000 16000 32000 44100 48000]" ,
    { "channel-number" , required_argument , NULL , 'c' } , //"[NUM]  通道数[1 2]" ,
    { "volume"      , required_argument , NULL , 'v' } , //"[NUM]  音量[-90~20db]" ,
    { "pcm-file"    , required_argument , NULL , 'p' } , //"[FILE] 播放的文件名称" ,
    { "dev-sel"     , required_argument , NULL , 'd' } , //"[DEV_SEL] 输入设备选择" ,
    {0, 0, 0, 0}
};


/**
 * help_hint - help note 
 * @pc_prog_name[IN]: help parameter
 * notes: 
 */
static int help_hint(char *pc_prog_name)
{
    int i;

    printf("version: \r\n  %s \n\n", ak_ao_get_version());
    printf("usage: \r\n  %s  [options] <value>\n\n", pc_prog_name);
    printf("options: \r\n");
    for (i = 0; i < sizeof(option_long) / sizeof(struct option) - 1; i ++) {
        printf("  -%c,--%-16s\t%s \n" , option_long[i].val , option_long[i].name , ac_option_hint[i]);
    }
    printf("\nexample: \r\n"
    "  %s -d 0 -s 8000 -c 1 -v 5 -p /mnt/xxx.pcm\n", pc_prog_name);
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
        switch(p_option[i].has_arg) {
        case no_argument:
            i_offset += snprintf(pc_option_short + i_offset, i_len_option - i_offset, "%c", c_option);
            break;
        case required_argument:
            i_offset += snprintf(pc_option_short + i_offset, i_len_option - i_offset, "%c:", c_option);
            break;
        case optional_argument:
            i_offset += snprintf(pc_option_short + i_offset, i_len_option - i_offset, "%c::", c_option);
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

    if (argc < 5 && argc != 1) {
        help_hint(argv[0]);
        c_flag = 0;
        goto parse_option_end;
    }

#ifdef AK_RTOS
    optind = 0;
#endif
    get_option_short(option_long, i_array_num, ac_option_short, LEN_OPTION_SHORT);
    while ((i_option = getopt_long(argc, argv, ac_option_short, option_long, NULL)) > 0) {
        switch (i_option) {
        case 'h' :                                                          //help
            help_hint(argv[0]);
            c_flag = 0;
            goto parse_option_end;
        case 's' :                                                          //sample-rate
            sample_rate = atoi(optarg);
            break;
        case 'c' :                                                          //channel-num
            channel_num = atoi(optarg);
            break;
        case 'v' :                                                          //volume
            volume = atoi(optarg);
            break;
        case 'p' :                                                          //pcm-file
            pcm_file = optarg;
            break;
        case 'd':                                                           // device-select
            dev_sel = atoi(optarg);
            break;
        }
    }
parse_option_end:
    return c_flag;
}

#ifdef AK_RTOS
static void param_init(void)
{
    sample_rate = 8000;
    volume = 3;
    pcm_file = (char *)pcm_def_parth;
    channel_num = AUDIO_CHANNEL_MONO;
    dev_sel = DEV_DAC;
}
#endif

/**
 * open_pcm_file - open pcm file 
 * @pcm_file[IN]: file name
 * notes: return file handle
 */
static FILE* open_pcm_file(const char *pcm_file)
{
    FILE *fp = fopen(pcm_file, "r");
    if (NULL == fp) {
        printf("open pcm file err\n");
        return NULL;
    }
    printf("open pcm file: %s OK\n", pcm_file);
    return fp;
}

/*
 * close_pcm_file: close pcm file.
 * fp[IN]: pointer of opened pcm file.
 * return: void.
 */
static void close_pcm_file(FILE *fp)
{
    if(fp) {
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
    {TYPE_HPF, TYPE_PF1, TYPE_PF1, TYPE_PF1,TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1, TYPE_PF1},
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
    /* close ao */
    if (-1 != ao_handle_id) {
        ak_ao_close(ao_handle_id);
        ao_handle_id = -1;
    }
}

/**
 * write_da_pcm - read pcm file and then send to ao play 
 * @arg[IN]: arg is NULL
 * notes: 
 */
static void* write_da_pcm(void *arg)
{
    ak_thread_set_name("[ao sample]attachment");
    int read_len = 0;
    int send_len = 0;
    int total_len = 0;
    int data_len = PCM_READ_LEN;
    if (AK_AUDIO_SAMPLE_RATE_44100 <= sample_rate &&  AUDIO_CHANNEL_STEREO == channel_num) {
        data_len = PCM_READ_LEN << 1; // when sample rate more than 44100, the frame length which send to ao must more than 8192 byte
    }

    if (AK_AUDIO_SAMPLE_RATE_48000 < sample_rate) {
        data_len = PCM_READ_LEN << 2; // when sample rate more than 48000, the frame length which send to ao must more than 16384 byte
    }

    unsigned char *data = (unsigned char *)ak_mem_alloc(MODULE_ID_APP, data_len);
    if (!data) {
        ak_print_error_ex(MODULE_ID_APP, "calloc audio data failed\n");
        return NULL;
    }

    while (1) {
        memset(data, 0x00, data_len);
        read_len = fread(data, sizeof(char), data_len, fp);

        if (read_len > 0) {
            /* send frame and play */
            if (ak_ao_send_frame(ao_handle_id, data, read_len, &send_len)) {
                ak_print_error_ex(MODULE_ID_APP, "write pcm to DA error!\n");
                break;
            }
            total_len += send_len;
            ak_sleep_ms(10);
        } else if(0 == read_len) {
            /* read to the end of file */
            ak_print_normal(MODULE_ID_APP, "\n\t read to the end of file\n");
            break;
        } else {
            ak_print_error(MODULE_ID_APP, "read, %s\n", strerror(errno));
            break;
        }
    }

    ak_print_normal(MODULE_ID_APP, "%s exit\n", __func__);
    if (data) {
        ak_mem_free(data);
    }
    ak_thread_exit();
    return NULL;
}

/**
 * Preconditions:
 * 1. T card is already mounted
 * 2. speaker must ready
 */
#ifdef AK_RTOS
static int ao_sample(int argc, char *argv[])
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
    ak_print_normal(MODULE_ID_APP, "** ao version: %s **\n", ak_ao_get_version());
    ak_print_normal(MODULE_ID_APP, "** %s **\n", ak_ao_get_filter_info());
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");

    if (-90 > volume || 20 < volume) {  // aslc volume
        ak_print_error_ex(MODULE_ID_APP, "volume must in -90~20\n");
        goto main_end;
    }

    if (init_ao()) {
        ak_print_error_ex(MODULE_ID_APP, "--- ao open false ---\n");
        goto main_end;
    }

    /* open pcm file */
    fp = open_pcm_file(pcm_file);
    if (NULL == fp) {
        ak_print_error_ex(MODULE_ID_APP, "open file error\n");
        goto main_end;
    }
#ifdef AK_RTOS
    {
        int io_size = RFILE_CACHE_LEN;
        setvbuf(fp, NULL, _IOFBF, io_size);
    }
#endif

    ak_pthread_t write_dac_tid;
    /* create record file thread */ 
    int ret = ak_thread_create(&(write_dac_tid), write_da_pcm,
                            NULL, ANYKA_THREAD_MIN_STACK_SIZE, 11);

    if (ret) {
        goto close_file_end;
    }

    /* wait thread end */
    ak_thread_join(write_dac_tid);

close_file_end:
    close_pcm_file(fp);

main_end:
    destroy_ao();

    ak_print_notice_ex(MODULE_ID_APP, "----- %s exit -----\n", argv[0]);
    ak_sdk_exit();
    return ret;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(ao_sample, ak_ao_sample, ao sample)
#endif
