/**
* Copyright (C) 2018 Anyka(Guangzhou) Microelectronics Technology CO.,LTD.
* File Name: ak_vi_demo.c
* Description: This is a simple example to show how the VI module working.
* Notes:
* History: V1.0.0
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <getopt.h>
#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include "ak_common.h"
#include "ak_common_graphics.h"
#include "ak_vi.h"
#include "ak_mem.h"
#include "ak_log.h"

#ifdef AK_RTOS
#include <kernel.h>
#define __STATIC__  static
#else
#define __STATIC__
#endif

#define LEN_HINT         512
#define LEN_OPTION_SHORT 512

#if !defined(__CHIP_AK37E_SERIES) && !defined(__CHIP_AK39EV200_SERIES)

#ifdef FAST_LAUNCH
#define DEF_FRAME_DEPTH     2
#else
#define DEF_FRAME_DEPTH     3
#endif

#define RES_GROUP	8
#elif defined(__CHIP_AK39EV200_SERIES)
#define DEF_FRAME_DEPTH		2
#define RES_GROUP	4
#else //__CHIP_AK37E_SERIES
#define DEF_FRAME_DEPTH		2
#define RES_GROUP	6
#endif


#if !defined(__CHIP_AK37E_SERIES) && !defined(__CHIP_AK39EV200_SERIES)
#define THIRD_CHN_SUPPORT
#endif

static int frame_num = 10;
static int channel_num = 0;
static char *isp_path = "";
static char *save_path = "/mnt/";
static int main_res_id = 4;
static int sub_res_id = 0;
static VI_CHN_ATTR chn_attr = {0};
static VI_CHN_ATTR chn_attr_sub = {0};
static int dev_cnt = 1;
static int save_file_flag = 1;
static int dma_pool_enable=0;

#if defined(__CHIP_AK37E_SERIES)
static RECTANGLE_S  res_group[RES_GROUP]=
{
    {320, 240},         /* QVGA */
    {352, 288},			/* QCIV */
    {640, 360},			/* 640*360 */
    {640, 480},			/*   VGA   */
    {1280, 720},
    {960, 1080},
};
#elif defined(__CHIP_AK39EV200_SERIES)
static RECTANGLE_S  res_group[RES_GROUP]=
{
    {640, 360},			/* 640*360 */
    {640, 480},			/*   VGA   */
    {1280, 720},        /* 720P */
    {1280, 960},
};
#else
static RECTANGLE_S  res_group[RES_GROUP]=
{
    {640, 360},			/* 640*360 */
    {640, 480},			/*   VGA   */
    {1280,720},			/*   720P  */
    {960, 1080},			/*   1080i */
    {1920,1080},			/*   1080P */
    {2304,1296},
    {2560,1440},
    {2560,1920}
};
#endif

/*     ***********************************
    ***********************************
    *
    use this demo
    must follow this:
    1. make sure the driver is insmode;
    2. mount the T card;
    3. the file path is exit;
    *
    ***********************************
    ***********************************
*/

static char ac_option_hint[  ][ LEN_HINT ] = {
    "			打印帮助信息" ,
    "[NUM]		采样帧数[0 1000]" ,
    "[NUM]		采集通道[0 1 16], 主通道0，次通道1,Third CHN 16" ,
    "[PATH]		ISP config file 保存路径, 默认为空,需要填写" ,
    "[PATH]		采集视频帧保存目录路径, 默认为/mnt/" ,
    "[NUM]		主通道分辨率index[0 - 7], 37e [0 - 5], V200 [0 - 3]" ,
    "[NUM]		次通道分辨率index[0 - 3]" ,
    "[NUM]		device count [1 - 2], default 1" ,
    "[NUM]		dma pool enable, [0-disable, 1-enable], default 0",
    "[NUM]		save file flag, 0 not save, otherwise save, default 1" ,
};

static struct option option_long[ ] = {
    { "help"            , no_argument       , NULL , 'h' } , //"       打印帮助信息" ,
    { "NUM"             , required_argument , NULL , 'n' } , //"		[NUM]  采集帧数" ,
    { "channel_number"  , required_argument , NULL , 'c' } , //"[NUM]  主通道 0， 次通道 1" ,
    { "isp_cfg"         , required_argument , NULL , 'f' } , //"[CONFIG] ISP config file 路径" ,
    { "path"            , required_argument , NULL , 'p' } , //"[PATH] 数据帧保存路径" ,
    { "main_chn_res"    , required_argument , NULL , 'm' } , //"[PATH] 数据帧保存路径" ,
    { "sub_chn_res"     , required_argument , NULL , 's' } , //"[PATH] 数据帧保存路径" ,
    { "dev_cnt"         , required_argument , NULL , 'd' } , //"[NUM]		device count [1 - 2], default 1",
    { "dma_pool_enable" , required_argument , NULL , 'o' } , //"DMA_POOL" ,
    { "save_file_flag"  , required_argument , NULL , 'a' } , //"[NUM]		save file flag, 0 not save, otherwise save, default 1",

    {NULL    , 0, 0, 0}
};

static void usage(const char * name)
{
    printf(" %s -n [frame_num] -c [channel_num] -f [isp_config_path] -p [frame_save_path] -m [main chn res index] -s [sub chn res index]\n", name);
    printf("\t[isp_config_path]: 	 default is null \n");
    printf("\t[frame_save_path]: 	 default is /mnt/ \n");
#ifndef __CHIP_AK37E_SERIES
    printf("\t[main/sub chn res index]:       0 - 640*360 \n \t\t\t\t\t1 - 640*480 \n "
                                  "\t\t\t\t\t2 - 1280*720\n \t\t\t\t\t3 - 960*1080\n \t\t\t\t\t4 - 1920*1080\n"
                                  "\t\t\t\t\t5 - 2304*1296\n \t\t\t\t\t6 - 2560*1440\n \t\t\t\t\t7 - 2560*1920\n\n");
    printf("eg: %s -n 1000 -c 1 -f isp_pr2000_dvp_pal_25fps_27M.conf -p vi_yuv/ -m 3 -s 0\n", name);
#else
    printf("\t[main/sub chn res index]:       0 - 320*240 \n \t\t\t\t\t1 - 352*288 \n \t\t\t\t\t2 - 640*360\n \t\t\t\t\t3 - 640*480\n "
                                  "\t\t\t\t\t4- 1280*720\n \t\t\t\t\t5- 960*1080\n\n");
    printf("eg: %s -n 10 -c 0 -m 3\n", name);
#endif
}

/*
 * help_hint: use the -h --help option.Print option of help information
 * return: 0
 */
static int help_hint( char *pc_prog_name )
{
    int i;


	printf("%s\n" , pc_prog_name);
    for(i = 0; i < sizeof(option_long) / sizeof(struct option); i ++)
    {
        if(option_long[i].name != NULL)
            printf("\t--%-16s -%c %s\n" , option_long[ i ].name , option_long[ i ].val , ac_option_hint[ i ]);
    }

    usage(pc_prog_name);

    printf("\n\n");
    return 0;
}

/*get option short function */
static char *get_option_short( struct option *p_option, int i_num_option, char *pc_option_short,
                        int i_len_option )
{
    int i;
    int i_offset = 0;
    char c_option;

    for( i = 0 ; i < i_num_option ; i ++ )
    {
        c_option = p_option[ i ].val;
        switch( p_option[ i ].has_arg )
        {
        case no_argument:
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c" , c_option );
            break;
        case required_argument:
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c:" , c_option );
            break;
        case optional_argument:
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c::" , c_option );
            break;
        }
    }
    return pc_option_short;
}

/* param parse function*/
static int parse_option( int argc, char **argv )
{
    int i_option;
    char ac_option_short[ LEN_OPTION_SHORT ];
    int i_array_num = sizeof( option_long ) / sizeof( struct option ) ;
    char c_flag = 1;
    optind = 0;

    get_option_short( option_long, i_array_num , ac_option_short , LEN_OPTION_SHORT );
    while((i_option = getopt_long(argc , argv , ac_option_short , option_long , NULL)) > 0)
    {
        switch(i_option)
        {
        case 'h' :                                                          //help
            help_hint( argv[ 0 ] );
            c_flag = 0;
            goto parse_option_end;
        case 'n' :                                                          //frame count
            frame_num = atoi( optarg );
            break;
        case 'c' :                                                          //sample-rate
            channel_num = atoi( optarg );
            break;
        case 'f' :                                                          //path
            isp_path = optarg;
            break;
        case 'p' :                                                          //path
            save_path = optarg;
            break;
        case 'm' :
            main_res_id = atoi(optarg);
            break;
        case 's' :
            sub_res_id = atoi(optarg);
            break;
        case 'd' :
            dev_cnt = atoi(optarg);
            break;
        case 'o' :                                                          //dma pool
            dma_pool_enable = atoi( optarg );
            break;
        case 'a' :
            save_file_flag = atoi(optarg);
            break;
        default:
            help_hint( argv[ 0 ] );
            c_flag = 0;
            goto parse_option_end;
        }
    }
parse_option_end:
    return c_flag;
}

/*
 * check_dir: check whether the 'path' was exist.
 * path[IN]: pointer to the path which will be checking.
 * return: 1 on exist, 0 is not.
 */
static int check_dir(const char *path)
{
    struct stat buf = {0};

    if (NULL == path)
        return 0;

    stat(path, &buf);
    if (S_ISDIR(buf.st_mode)) {
        return 1;
    } else {
        return 0;
    }
}

/*
 * save_yuv_data - use to save yuv data to file
 * path[IN]: pointer to saving directory
 * index[IN]: frame number index
 * frame[IN]: pointer to yuv data, include main and sub channel data
 * attr[IN]:  vi channel attribute.
 */
static void save_yuv_data(int chn_id, const char *path, int index,
        struct video_input_frame *frame, VI_CHN_ATTR *attr)
{
    FILE *fd = NULL;
    unsigned int len = 0;
    unsigned char *buf = NULL;
    struct ak_date date;
    char time_str[32] = {0};
    char file_path[255] = {0};
    int dev_id;

    if ((VIDEO_CHN0 == chn_id) || (VIDEO_CHN1 == chn_id) || (VIDEO_CHN16 == chn_id))
    {
        dev_id = 0;
    }
    else
    {
        dev_id = 1;
    }

    ak_print_normal(MODULE_ID_APP, "saving frame, index=%d\n", index);

    /* construct file name */
    ak_get_localdate(&date);
    ak_date_to_string(&date, time_str);
    if((chn_id == VIDEO_CHN0) || (chn_id == VIDEO_CHN2))
        sprintf(file_path, "%sdev%d_main_%s_%d_%dx%d.yuv", path, dev_id, time_str, index,
            attr->res.width, attr->res.height);
    else if ((chn_id == VIDEO_CHN1) || (chn_id == VIDEO_CHN3))
        sprintf(file_path, "%sdev%d_sub_%s_%d_%dx%d.yuv", path, dev_id, time_str, index,
            attr->res.width, attr->res.height);
    else if ((chn_id == VIDEO_CHN16) || (chn_id == VIDEO_CHN17))
        sprintf(file_path, "%sdev%d_td_chn_%s_%d_%dx%d.yuv", path, dev_id, time_str, index,
            attr->res.width/2, attr->res.height/2);

    /*
     * open appointed file to save YUV data
     * save main channel yuv here
     */
    fd = fopen(file_path, "w+b");
    if (fd) {
        buf = frame->vi_frame.data;
        len = frame->vi_frame.len;
        do {
            len -= fwrite(buf+frame->vi_frame.len-len, 1, len, fd);
        } while (len != 0);

        fclose(fd);
    } else {
        ak_print_normal(MODULE_ID_APP, "open YUV file[%s] failed!!\n", file_path);
    }
}

static int start_vi(int dev_id)
{
    /*
     * step 0: global value initialize
     */
    int ret = -1;                                //return value
    int width = res_group[main_res_id].width;
    int height = res_group[main_res_id].height;
    int subwidth = res_group[sub_res_id].width;
    int subheight = res_group[sub_res_id].height;
    int chn_main_id = VIDEO_CHN0;
    int chn_sub_id = VIDEO_CHN1;
    int chn_trd_id = VIDEO_CHN16;

    if (dev_id)
    {
        chn_main_id = VIDEO_CHN2;
        chn_sub_id = VIDEO_CHN3;
        chn_trd_id = VIDEO_CHN17;
    }

    /* open vi flow */

    /*
     * step 1: open video input device
     */
    ret = ak_vi_open(dev_id);
    if (AK_SUCCESS != ret) {
        ak_print_error_ex(MODULE_ID_APP, "vi device %d open failed\n", dev_id);
        return ret;
    }

    /* default vi ae para */
#ifdef AK_RTOS
#ifndef __CHIP_AK37E_SERIES
#include "ak_vpss.h"
    struct vpss_isp_ae_init_info ae_init_info;
    ae_init_info.a_gain = 256;
    ae_init_info.d_gain = 256;
    ae_init_info.exp_time = 2154;
    ae_init_info.isp_d_gain = 256;
    ak_vpss_set_ae_init_info(dev_id, &ae_init_info);
#endif
#endif


#ifndef __CHIP_AK37E_SERIES
    /*
     * step 2: load isp config
     */
    ret = ak_vi_load_sensor_cfg(dev_id, isp_path);
    if (AK_SUCCESS != ret) {
        ak_print_error_ex(MODULE_ID_APP, "vi device %d load isp cfg [%s] failed!\n", dev_id, isp_path);
        return ret;
    }
#endif
    /*
     * step 3: get sensor support max resolution
     */
    RECTANGLE_S res;                //max sensor resolution
    VI_DEV_ATTR    dev_attr;
    memset(&dev_attr, 0, sizeof(VI_DEV_ATTR));
    dev_attr.dev_id = dev_id;
    dev_attr.crop.left = 0;
    dev_attr.crop.top = 0;
    dev_attr.crop.width = width;
    dev_attr.crop.height = height;
    dev_attr.max_width = width;
    dev_attr.max_height = height;
    dev_attr.sub_max_width = subwidth;
    dev_attr.sub_max_height = subheight;

    /* get sensor resolution */
    ret = ak_vi_get_sensor_resolution(dev_id, &res);
    if (ret) {
        ak_print_error_ex(MODULE_ID_APP, "Can't get dev[%d]resolution\n", dev_id);
        ak_vi_close(dev_id);
        return ret;
    } else {
        ak_print_normal_ex(MODULE_ID_APP, "get dev res w:[%d]h:[%d]\n",res.width, res.height);
        dev_attr.crop.width = res.width;
        dev_attr.crop.height = res.height;
    }

    if(dma_pool_enable)
    {
        unsigned long vi_dma_pool = 0;
    #ifndef __CHIP_AK37E_SERIES
        vi_dma_pool += dev_attr.crop.width*dev_attr.crop.height*3/2*5/4+64;
    #endif
        vi_dma_pool += (width*height*3/2+2048)*DEF_FRAME_DEPTH;
        vi_dma_pool += (subwidth*subheight*3/2+2048)*DEF_FRAME_DEPTH;

        unsigned long total = 0;
        ak_mem_dma_pool_get_free_mem(MODULE_ID_VI, &total);
        if (vi_dma_pool > total)
        {
            ak_print_error_ex(MODULE_ID_APP, "VI Dma pool free [%lu] bytes! VI need SIZE [%lu] bytes!\n", total, vi_dma_pool);
            return -1;
        }
    }

    /*
     * step 4: set vi device working parameters
     * default parameters: 25fps, day mode
     */
    ret = ak_vi_set_dev_attr(dev_id, &dev_attr);
    if (ret) {
        ak_print_error_ex(MODULE_ID_APP, "vi device %d set device attribute failed!\n", dev_id);
        ak_vi_close(dev_id);
        return ret;
    }

    /*
     * step 5: set main channel attribute
     */

    memset(&chn_attr, 0, sizeof(VI_CHN_ATTR));
    chn_attr.chn_id = chn_main_id;
    chn_attr.res.width = width;
    chn_attr.res.height = height;
    chn_attr.frame_depth = DEF_FRAME_DEPTH;
    /*disable frame control*/
    chn_attr.frame_rate = 0;
    ret = ak_vi_set_chn_attr(chn_main_id, &chn_attr);
    if (ret) {
        ak_print_error_ex(MODULE_ID_APP, "vi device %d set channel [%d] attribute failed!\n", dev_id, chn_main_id);
        ak_vi_close(dev_id);
        return ret;
    }
    ak_print_normal_ex(MODULE_ID_APP, "vi device %d main sub channel attribute\n", dev_id);

#ifndef __CHIP_AK37E_SERIES
    /*
     * step 6: set sub channel attribute
     */

    memset(&chn_attr_sub, 0, sizeof(VI_CHN_ATTR));
    chn_attr_sub.chn_id = chn_sub_id;
    chn_attr_sub.res.width = subwidth;
    chn_attr_sub.res.height = subheight;
    chn_attr_sub.frame_depth = DEF_FRAME_DEPTH;
    /*disable frame control*/
    chn_attr_sub.frame_rate = 0;
    ret = ak_vi_set_chn_attr(chn_sub_id, &chn_attr_sub);
    if (ret) {
        ak_print_error_ex(MODULE_ID_APP, "vi device %d set channel [%d] attribute failed!\n", dev_id, chn_sub_id);
        ak_vi_close(dev_id);
        return ret;
    }
    ak_print_normal_ex(MODULE_ID_APP, "vi device %d set sub channel attribute\n", dev_id);

#ifdef THIRD_CHN_SUPPORT
    /*
     * step 7: set third channel attribute
     */
    VI_CHN_ATTR chn_attr_td;
    memset(&chn_attr_td, 0, sizeof(VI_CHN_ATTR));
    chn_attr_td.chn_id = chn_trd_id;
	/* resolution of third channel must be half of the resolution of sub channel */
	chn_attr_td.res.width = subwidth/2;
    chn_attr_td.res.height = subheight/2;
    chn_attr_td.frame_depth = DEF_FRAME_DEPTH;
    /*disable frame control*/
    chn_attr_td.frame_rate = 0;
    ret = ak_vi_set_chn_attr(chn_trd_id, &chn_attr_td);
    if (ret) {
        ak_print_error_ex(MODULE_ID_APP, "vi device %d set channel [%d] attribute failed!\n", dev_id, chn_trd_id);
        ak_vi_close(dev_id);
        return ret;
    }
    ak_print_normal_ex(MODULE_ID_APP, "vi device %d set td channel attribute\n", dev_id);
#endif
#endif
	/*
     * step 8: enable vi device
     */
    ret = ak_vi_enable_dev(dev_id);
    if (ret) {
        ak_print_error_ex(MODULE_ID_APP, "vi device %d enable device  failed!\n", dev_id);
        ak_vi_close(dev_id);
        return ret;
    }

    /*
     * step 9: enable vi main channel
     */
    ret = ak_vi_enable_chn(chn_main_id);
    if(ret)
    {
        ak_print_error_ex(MODULE_ID_APP, "vi channel[%d] enable failed!\n", chn_main_id);
        ak_vi_close(dev_id);
        return ret;
    }

#ifndef __CHIP_AK37E_SERIES
    /*
     * step 10: enable vi sub channel
     */
    ret = ak_vi_enable_chn(chn_sub_id);
    if(ret)
    {
        ak_print_error_ex(MODULE_ID_APP, "vi channel[%d] enable failed!\n",chn_sub_id);
        ak_vi_close(dev_id);
        return ret;
    }
#ifdef THIRD_CHN_SUPPORT
    /*
     * step 11: enable vi third channel
     */
    ret = ak_vi_enable_chn(chn_trd_id);
    if(ret)
    {
        ak_print_error_ex(MODULE_ID_APP, "vi channel[%d] enable failed!\n",chn_trd_id);
        ak_vi_close(dev_id);
        return ret;
    }
#endif
#endif
    return 0;
}

static int stop_vi(int dev_id)
{
    int ret = -1;
    int chn_main_id = VIDEO_CHN0;
    int chn_sub_id = VIDEO_CHN1;
    int chn_trd_id = VIDEO_CHN16;

    if (dev_id)
    {
        chn_main_id = VIDEO_CHN2;
        chn_sub_id = VIDEO_CHN3;
        chn_trd_id = VIDEO_CHN17;
    }

    ak_vi_disable_chn(chn_main_id);

#ifndef __CHIP_AK37E_SERIES
    ak_vi_disable_chn(chn_sub_id);
#ifdef THIRD_CHN_SUPPORT
    ak_vi_disable_chn(chn_trd_id);
#endif
#endif
	ak_vi_disable_dev(dev_id);
    ret = ak_vi_close(dev_id);

    return ret;
}

/*
 * vi_capture_loop: loop to get and release yuv, between get and release,
 *                  here we just save the frame to file, on your platform,
 *                  you can rewrite the save_function with your code.
 * vi_handle[IN]: pointer to vi handle, return by ak_vi_open()
 * number[IN]: save numbers
 * path[IN]:   save directory path, if NULL, will not save anymore.
 * attr[IN]:   vi channel attribute.
 */
static void vi_capture_loop(int dev_cnt, int number, const char *path,
        VI_CHN_ATTR *attr, VI_CHN_ATTR *attr_sub)
{
    int count = 0;
    int chn_id = 0;
    int flag_0 = 0;
    int flag_1 = 0;
    struct video_input_frame frame;

    ak_print_normal(MODULE_ID_APP, "capture start\n");

    /*
     * To get frame by loop
     */
    while (count <  number * dev_cnt)
    {
        chn_id = channel_num;
        flag_0 = 0;
        flag_1 = 0;

        while (flag_0 + flag_1 < dev_cnt)
        {
            memset(&frame, 0x00, sizeof(frame));

            /* to get frame according to the channel number */
            int ret = ak_vi_get_frame(chn_id, &frame);

            if (!ret) {
                /*
                 * Here, you can implement your code to use this frame.
                 * Notice, do not occupy it too long.
                 */
                ak_print_normal_ex(MODULE_ID_APP, "[%d] yuv len: %u\n", count,
                        frame.vi_frame.len);
                ak_print_normal_ex(MODULE_ID_APP, "[%d] phyaddr: 0x%08x\n", count,
                        frame.phyaddr);

                if (save_file_flag)
                {
                    if ((chn_id == VIDEO_CHN0) || (chn_id == VIDEO_CHN2))
                        save_yuv_data(chn_id, save_path, count, &frame,attr);
                    else if ((chn_id == VIDEO_CHN1) || (chn_id == VIDEO_CHN3))
                        save_yuv_data(chn_id, save_path, count, &frame,attr_sub);
        			else
                        save_yuv_data(chn_id, save_path, count, &frame,attr_sub);
                }
                /*
                 * in this context, this frame was useless,
                 * release frame data
                 */
                ak_vi_release_frame(chn_id, &frame);
                count++;

                if (chn_id == channel_num)
                {
                    flag_0 = 1;

                    if (2 == dev_cnt)
                    {
                        if (VIDEO_CHN0 == channel_num)
                            chn_id = VIDEO_CHN2;
                        else if (VIDEO_CHN1 == channel_num)
                            chn_id = VIDEO_CHN3;
                        else
                            chn_id = VIDEO_CHN17;
                    }
                }
                else
                {
                    flag_1 = 1;
                }
            } else {

                /*
                 *    If getting too fast, it will have no data,
                 *    just take breath.
                 */
                ak_print_normal_ex(MODULE_ID_APP, "get frame failed!\n");
                ak_sleep_ms(10);
            }

        }
    }

    ak_print_normal(MODULE_ID_APP,"capture finish\n\n");
}

/**
 * Preconditions:
 * 1??TF card is already mounted
 * 2??yuv_data is already created in /mnt
 * 3??ircut is already opened at day mode
 * 4??your main video progress must stop
 */
#ifdef AK_RTOS
static int vi_sample(int argc, char *argv[])
#else
int main(int argc, char **argv)
#endif
{
    /* start the application */
    sdk_run_config config;
    memset(&config, 0, sizeof(config));
    config.mem_trace_flag = SDK_RUN_NORMAL;
    config.isp_tool_server_flag = 0;
    ak_sdk_init( &config );

    ak_print_normal(MODULE_ID_APP, "*****************************************\n");
    ak_print_normal(MODULE_ID_APP, "** vi demo version: %s **\n", ak_vi_get_version());
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");

    channel_num = VIDEO_CHN0;

    if( parse_option( argc, argv ) == 0 )
    {
        return 0;
    }
    /*check param validate*/
    if(frame_num < 0 || frame_num > 1000 || channel_num < 0 || (channel_num > 1&&channel_num != VIDEO_CHN16) || strlen(save_path) == 0 )
    {
        ak_print_error_ex(MODULE_ID_APP,"INPUT param error!\n");
        help_hint( argv[ 0 ] );
        return 0;
    }

#ifndef __CHIP_AK37E_SERIES
    if (strlen(isp_path) == 0)
    {
        ak_print_error_ex(MODULE_ID_APP,"INPUT param error!\n");
        help_hint( argv[ 0 ] );
        return 0;
    }
#endif

    if(dev_cnt < 1 || dev_cnt > 2)
    {
        ak_print_error_ex(MODULE_ID_APP,"dev_cnt error! use default value 1.\n");
        dev_cnt = 1;
    }

    /*check the data save path */
    if(check_dir(save_path) == 0 && save_file_flag)
    {
        ak_print_error_ex(MODULE_ID_APP, "save path is not existed!\n");
        return 0;
    }

    if(dma_pool_enable)
	{
		int vi_dma_size = 0;	//M bytes;

		/* below is a rough calc method, and based on the crop res of sensor is equal or less than the main channel res*/
		/* 3MP res, 20 for vi */
#if defined(__CHIP_AK37E_SERIES)
        vi_dma_size = 6;
#elif defined(__CHIP_AK39EV200_SERIES)
        vi_dma_size = 8;
#else
		if(main_res_id <= 5)
		{
			vi_dma_size = 20;
		}
		/*above 3MP res, 32 for vi */
		else
		{
			vi_dma_size = 32;
		}
#endif
        if (2 == dev_cnt)
        {
            vi_dma_size *= 2;
        }

		ak_mem_dma_pool_create(MODULE_ID_VI, vi_dma_size*1024*1024 - 300*1024); /* 300KB left for OSD and audio*/
		ak_mem_dma_pool_activate();
	}

    int ret = -1;

    /*
     * step 1: start vi
     */
    ret = start_vi(VIDEO_DEV0);
    if (ret)
    {
        goto exit;
    }

    if (dev_cnt > 1)
    {
        start_vi(VIDEO_DEV1);
    }

    if(dma_pool_enable)
	{
		unsigned long total = 0;
		ak_mem_dma_pool_get_used_mem(MODULE_ID_VI, &total);
		ak_print_notice_ex(MODULE_ID_APP, "VI Dma pool used total [%lu] bytes!", total);
		total = 0;
		ak_mem_dma_pool_get_free_mem(MODULE_ID_VI, &total);
		ak_print_notice_ex(MODULE_ID_APP, "VI Dma pool free total [%lu] bytes!", total);
	}

    /*
     * step 2: start to capture and save yuv frames
     */
    vi_capture_loop(dev_cnt, frame_num, save_path, &chn_attr, &chn_attr_sub);

    /*
     * step 3: release resource
     */
    stop_vi(VIDEO_DEV0);
    if (dev_cnt > 1)
    {
        stop_vi(VIDEO_DEV1);
    }


exit:
    /* exit */
    if(dma_pool_enable)
	{
		ak_mem_dma_pool_exit();
	}

    ak_sdk_exit();
    ak_print_normal(MODULE_ID_APP, "exit vi demo\n");
    return ret;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(vi_sample, ak_vi_sample, vi sample)
#endif
