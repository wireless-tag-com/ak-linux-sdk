#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <unistd.h>

#include "ak_common.h"
#include "ak_log.h"
#include "ak_common_graphics.h"
#include "ak_vdec.h"
#include "ak_vo.h"
#include "ak_thread.h"
#include "ak_mem.h"

#ifdef AK_RTOS
#include <kernel.h>
#define THREAD_PRIO 90
#define __STATIC__  static
#else
#define THREAD_PRIO -1
#define __STATIC__
#endif

#define RECORD_READ_LEN      1024*100 /* read video file buffer length */
#define DE_VIDEO_SIZE_MAX    5
/* this is length for parse */
#define LEN_HINT                512
#define LEN_OPTION_SHORT        512

/* mipi screen res */
#define MIPI_SC_WIDTH 1280
#define MIPI_SC_HEIGHT 800

/* RGB screen res */
#define RGB_SC_WIDTH  1024
#define RGB_SC_HEIGHT  600

#define MAX_DE_NUM   4

/* resolation define */
struct resolution_t {
	unsigned int width;
	unsigned int height;
	unsigned char str[20];
};

static char *pc_prog_name = NULL;                      //demo名称
static char *file  = NULL;                              //file name
static char *type         = NULL;                      //get the type input
static int refresh_flag   = 0;                         //flag to refresh
static int res            = 3;
static int refresh_record_flag = 0;                    //flag to refresh
static int decoder_num = 1;
static ak_mutex_t refresh_flag_lock;
struct ak_layer_pos  obj_pos[MAX_DE_NUM];              //store the pos
static int outtype = 0;
static int dst_width = 0;
static int dst_height = 0;
static int rotate = 0;
static int vo_format = GP_FORMAT_RGB888;
static int gi_frame_buff_yum = 0;
static int gi_stream_buff_size = 0;
/* decoder resolution */
static struct resolution_t resolutions[DE_VIDEO_SIZE_MAX] = {
    {640,   360,   "DE_VIDEO_SIZE_360P"},
    {640,   480,   "DE_VIDEO_SIZE_VGA"},
    {1280,  720,   "DE_VIDEO_SIZE_720P"},
    {1920,	1080,  "DE_VIDEO_SIZE_1080P"},
    {2560,  1920,  "DE_VIDEO_SIZE_1920P"}
};

/* this is the message to print */
static char ac_option_hint[  ][ LEN_HINT ] = {                                  //操作提示数组
    "打印帮助信息" ,
    "屏幕宽度" ,
    "屏幕高度" ,
    "屏幕旋转角度 可选值:0,90,180,270" ,
    "解码的数量:[1-4]" ,
    "解码视频文件路径或JPEG文件目录路径" ,
    "解码数据格式 [h264, h265, jpeg]" ,
    "解码输出格式, [0 - RGB565, 1 - RGB888, default 1]",
    "解码文件分辨率, [0 - 640*360, 1 - 640*480, 2 - 1280*720, 3 - 1920*1080, 4 - 2560*1920]" ,
    "帧缓冲区数量,取值范围,37E:[0,2-15](0为3个),37D:[0,3-15](0为4个)" ,
    "流缓冲区大小,取值范围[300k-96M],必须4k对齐" ,
    "" ,
};

/* opt for print the message */
static struct option option_long[ ] = {
    { "help"              , no_argument       , NULL , 'h' } ,                  //"打印帮助信息" ,
    { "screen-w"          , required_argument , NULL , 'w' } ,                  //"屏幕宽度" ,
    { "screen-h"          , required_argument , NULL , 'e' } ,                  //"屏幕高度" ,
    { "screen-r"          , required_argument , NULL , 'x' } ,                  //"屏幕旋转角度 可选值:0,90,180,270" ,
    { "decode-num"        , required_argument , NULL , 'n' } ,                  //"解码的数量:[1-4]" ,
    { "file-dir"          , required_argument , NULL , 'f' } ,                  //"解码视频文件路径或JPEG文件目录路径" ,
    { "format-in"         , required_argument , NULL , 'i' } ,                  //"解码数据格式 [h264, h265, jpeg]" ,
    { "format-out"        , required_argument , NULL , 'o' } ,                  //"解码输出格式, [0 - RGB565, 1 - RGB888, default 1]",
    { "resolution"        , required_argument , NULL , 'r' } ,                  //"解码文件分辨率, [0 - 640*360, 1 - 640*480, 2 - 1280*720, 3 - 1920*1080, 4 - 2560*1920]" ,
    { "frame-buff-num"    , required_argument , NULL , 'y' } ,                  //"帧缓冲区数量,取值范围,37E:[0,2-15](0为3个),37D:[0,3-15](0为4个)" ,
    { "stream-size"       , required_argument , NULL , 's' } ,                  //"流缓冲区大小,取值范围[300k-96M],必须4k对齐" ,
    { 0                   , 0                 , 0    , 0   } ,                  //"" ,
};

/*
* get_option_short: fill the stort option string.
* return: option short string addr.
*/
static char *get_option_short( struct option *p_option, int i_num_option, char *pc_option_short, int i_len_option )
{
    int i;
    int i_offset = 0;
    char c_option;

    /* get the option */
    for( i = 0 ; i < i_num_option ; i ++ )
    {
        /* get the opt */
        if( ( c_option = p_option[ i ].val ) == 0 )
        {
            continue;
        }

        switch( p_option[ i ].has_arg )
        {
        case no_argument:
            /* if no argument, set the offset for default */
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c" , c_option );
            break;
        case required_argument:
            /* required argument offset calculate */
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c:" , c_option );
            break;
        case optional_argument:
            /* calculate the option offset */
            i_offset += snprintf( pc_option_short + i_offset , i_len_option - i_offset , "%c::" , c_option );
            break;
        }
    }
    return pc_option_short;
}

static void usage(const char * name)
{
    /* a example for this sample in usage */

    printf("\nexample: \r\n"
            "\t%s -w 1920 -e 1200 -x 90 -n 1 -f /mnt/test.str -i h264 -r 3\n"
            "\t%s -w 1280 -e 800 -x 90 -n 1 -f /mnt/test.str -i h264 -r 3\n"
            "\t%s -w 1024 -e 600 -x 0 -n 1 -f /mnt/test.str -i h264 -r 3\n", name, name, name );
    printf( "\tNOTE: jpeg file_path should be a directory\n"
            "\teg: if you have several jpeg pictures in /mnt/picture,you should set the \"-f,--file_dir\" value as /mnt/picture\n");
}

/* if opt is not supported, print the help message */
static int help_hint(void)
{
    int i;

    printf( "%s\n" , pc_prog_name);
    printf( "Version: \r\n\t%s \n\n", ak_vdec_get_version());
    printf( "Usage: \r\n\t%s  [options] <value>\n\n", pc_prog_name);

    /* parse the all supported option */
    printf( "options: \r\n");
    for(i = 0; i < sizeof(option_long) / sizeof(struct option); i ++)
    {
        /* get the opt */
        if( option_long[ i ].val != 0 )
            printf("\t-%c, --%-16s\t%s\n" , option_long[ i ].val , option_long[ i ].name , ac_option_hint[ i ]);
    }

    /* an example */
    usage(pc_prog_name);
    printf( "\n\n");
    return 0;
}

/* parse the option from cmd line */
static int parse_option( int argc, char **argv )
{
    int i_option;

    char ac_option_short[ LEN_OPTION_SHORT ];                           /* opt string */
    int i_array_num = sizeof( option_long ) / sizeof( struct option ) ; /* get the option num*/
    char c_flag = AK_TRUE;
    pc_prog_name = argv[ 0 ];   /* get the option */
    optind = 0;

    get_option_short( option_long, i_array_num , ac_option_short , LEN_OPTION_SHORT );  /* parse the cmd line input */
    while((i_option = getopt_long(argc , argv , ac_option_short , option_long , NULL)) > 0)
    {
        switch(i_option)
        {
        case 'h' :                                                          //help
            help_hint();
            c_flag = AK_FALSE;
            goto parse_option_end;
        case 'w' :                                                          //screen type
            dst_width = atoi( optarg );
            break;
        case 'e' :                                                          //screen type
            dst_height = atoi( optarg );
            break;
        case 'x' :                                                          //screen type
            switch ( atoi( optarg ) ) {
                case 0 :
                    rotate = AK_GP_ROTATE_NONE;
                    break;
                case 90 :
                    rotate = AK_GP_ROTATE_90;
                    break;
                case 180 :
                    rotate = AK_GP_ROTATE_180;
                    break;
                case 270 :
                    rotate = AK_GP_ROTATE_270;
                    break;
                default :
                    rotate = AK_GP_ROTATE_NONE;
                    break;
            }
            break;
        case 'n' :                                                          //decode num
            decoder_num = atoi( optarg );
            break;
        case 'f' :                                                          //data file path
            file = optarg;
            break;
        case 'i' :                                                          //data_file format
            type = optarg;
            break;
        case 'o' :
            vo_format = atoi( optarg );
            break;
        case 'r' :                                                          //decode num
            res = atoi( optarg );
            break;
        case 'y' :
            gi_frame_buff_yum = atoi( optarg );
            break;
        case 's' :
            gi_stream_buff_size = atoi( optarg );
            break;
        default :
            help_hint();                                                    //if out of range, print message
            c_flag = AK_FALSE;
            goto parse_option_end;
        }
    }

parse_option_end:
    return c_flag;
}

/**
 * decode_stream - send the stream data to decode
 * @adec_handle[IN]: adec handle
 * @data[IN]: audio file data
 * @read_len[IN]: audio file data length
 * notes:
 */

extern unsigned long get_tick_count(void);

static void decode_stream(int handle_id, unsigned char *data, int read_len)
{
    int send_len = 0;
    int dec_len = 0;
    int ret = 0;

    /* cycle to send the data to decode */
    while (read_len > 0)
    {
        /* send stream to decode */
        //printf("[%d] vdec_%d send frame \n",get_tick_count(),handle_id);
        ret = ak_vdec_send_stream(handle_id, &data[send_len], read_len, 1, &dec_len);
        if (ret !=  0)
        {
            ak_print_error_ex(MODULE_ID_APP, "write video data failed!\n");
            break;
        }

        /* record the length to send to decoder */
        read_len -= dec_len;
        send_len += dec_len;
    }
}

/* show the stream to screen */
int demo_play_func(struct ak_vdec_frame *frame)
{
    /* set the pos in the screen */
    int pos_top = obj_pos[frame->id].top;
    int pos_left = obj_pos[frame->id].left;
    int width = obj_pos[frame->id].width;
    int height = obj_pos[frame->id].height;

    /* obj to add to layer */
    struct ak_vo_obj    obj;
    memset(&obj, 0x00, sizeof(struct ak_vo_obj));
    if (frame->data_type == AK_TILE_YUV)
    {
        /* if the data type is tileyuv */
        obj.format = GP_FORMAT_TILED32X4;
        obj.vo_tileyuv.data = frame->tileyuv_data;

        //if want to clip the tileyuv data,set the following flag true.NOTE: not clip here
        obj.vo_tileyuv.flag = 0;
        obj.vo_tileyuv.clip_pos.left   = 0;
        obj.vo_tileyuv.clip_pos.top    = 0;
        obj.vo_tileyuv.clip_pos.width  = resolutions[res].width;
        obj.vo_tileyuv.clip_pos.height = resolutions[res].height;
    }
    else
    {
        /* if the data type is yuv420sp */
        if(outtype == AK_YUV420P)
            obj.format = GP_FORMAT_YUV420P;
        else if(outtype == AK_YUV420SP)
            obj.format = GP_FORMAT_YUV420SP;
        obj.cmd = GP_OPT_SCALE;         /* scale to screen */
        obj.vo_layer.width = frame->yuv_data.pitch_width;   /* the real width */
        obj.vo_layer.height = frame->yuv_data.pitch_height; /* the real height */

        /* pos and range from the src */
        obj.vo_layer.clip_pos.top = 0;
        obj.vo_layer.clip_pos.left = 0;
        obj.vo_layer.clip_pos.width = frame->width;
        obj.vo_layer.clip_pos.height = frame->height;
        ak_mem_dma_vaddr2paddr(frame->yuv_data.data, &(obj.vo_layer.dma_addr));
    }

    /* pos and range for the dst layer to contain the src */
    obj.dst_layer.top = pos_top;
    obj.dst_layer.left = pos_left;
    obj.dst_layer.width = width;
    obj.dst_layer.height = height;

    /* add this picture to layer */
    ak_vo_add_obj(&obj, AK_VO_LAYER_VIDEO_1);

    ak_thread_mutex_lock(&refresh_flag_lock);
    /* set the refresh flag */
    refresh_flag |= (0x01 << frame->id);

    /* get the value */
    if(refresh_flag == refresh_record_flag)
    {
        refresh_flag = 0;
        /* video_1 is created,only flush the layer */
        int cmd = AK_VO_REFRESH_VIDEO_GROUP & 0x100;
        ak_vo_refresh_screen(cmd);
        //ak_vo_refresh_screen_ex(cmd , AK_VO_REFRESH_MODE_WAIT_LCDC | AK_VO_REFRESH_MODE_LAYER_TMP );
    }
    ak_thread_mutex_unlock(&refresh_flag_lock);

    /* wait for all decoder ready */
    while ( (refresh_flag != refresh_record_flag) && (refresh_flag & (0x01 << frame->id)) )
    {
        ak_sleep_ms(5);
    }

    return 0;
}

void *vdec_send_decode_th(void *arg)
{
    ak_thread_set_name("vdec_demo_test");

    int *handle_id = (int *)arg;
    char *file_name = file;

    /* open video file */
    FILE *fp = fopen(file_name, "r");
    if(NULL == fp) {
        ak_print_error_ex(MODULE_ID_APP, "open %s failed\n", file_name);
        return NULL;
    }
    ak_print_normal(MODULE_ID_APP, "open record file: %s OK\n", file_name);

    /* read video file stream and send to decode, */
    int read_len = 0;
    int total_len = 0;
    unsigned char *data = (unsigned char *)ak_mem_alloc(MODULE_ID_APP, RECORD_READ_LEN);

    /* set file from the beginning */
    fseek(fp, 0, SEEK_SET);

    /* loop for sending data to decode */
    do
    {
        /* read the record file stream */
        memset(data, 0x00, RECORD_READ_LEN);
        read_len = fread(data, sizeof(char), RECORD_READ_LEN, fp);

        /* get the data and send to decoder */
        if(read_len > 0)
        {
            total_len += read_len;
            /* play loop */
            decode_stream(*handle_id, data, read_len);
            ak_sleep_ms(10);
        }
        else if(0 == read_len)
        {
            /* read to the end of file */
            ak_print_normal_ex(MODULE_ID_APP, "\n\tread to the end of file\n");
            break;
        }
        else
        {
            /* if the data is wrong */
            ak_print_error_ex(MODULE_ID_APP, "\nread file error\n");
            break;
        }
    }while(1);

    /* if finish, notice the decoder for data sending finish */
    ak_vdec_end_stream(*handle_id);
    if (data != NULL)
        ak_mem_free(data);

    fclose(fp);

    ak_print_normal_ex(MODULE_ID_APP, "send stream th exit\n");
    return NULL;
}

void *vdec_solo_frame(void *arg)
{
    char file_name[128];
    int ret = -1;
    int *handle_id = (int *)arg;

    /* read video file stream and send to decode, */
    int read_len = 0;
    int total_len = 0;
    unsigned char *data = NULL;
    char *dir_name = file;
    int retry = 100;

    /* a dir opt */
    DIR *dir = NULL;
    struct dirent *entry = NULL;

    /* this dir only contain the jpeg file */
    dir = opendir(dir_name);//
    if (dir == NULL)
    {
        ak_print_error_ex(MODULE_ID_APP, "can't open file dir[%s]\n", dir_name);
        goto jpeg_exit;
    }

    /* a loop for read all the jpeg file */
    while ((entry = readdir(dir)))
    {
        /* if get the file */
        if (entry->d_type == DT_REG)
        {
            /* open video file */
            sprintf(file_name, "%s/%s",  dir_name, entry->d_name);
            FILE *fp = fopen(file_name, "r");
            if(NULL == fp)
            {
                ak_print_error_ex(MODULE_ID_APP, "open %s failed\n", file_name);
                continue;
            }
            ak_print_normal(MODULE_ID_APP, "open record file: %s OK\n", file_name);

            /* get the file size */
            fseek(fp, 0, SEEK_END);
            int file_size = ftell(fp);

            data = (unsigned char *)ak_mem_alloc(MODULE_ID_APP, file_size);

            /* set file from the beginning */
            fseek(fp, 0, SEEK_SET);

            /* read the record file stream */
            memset(data, 0x00, file_size);
            read_len = fread(data, sizeof(char), file_size, fp);

            /* send the data to decode */
            ak_print_normal(MODULE_ID_APP, "size is [%d] \n", read_len);
            if(read_len > 0)
            {
                total_len += read_len;
                /* play loop */
                decode_stream(*handle_id, data, read_len);
            }
            else if(0 == read_len)
            {
                /* read to the end of file */
                ak_print_normal_ex(MODULE_ID_APP, "\n\tread to the end of file\n");
                goto file_failed;
            }
            else
            {
                /* error is not support */
                ak_print_error_ex(MODULE_ID_APP, "read file error\n");
                goto file_failed;
            }

            /* inform that sending data is over */
            ak_vdec_end_stream(*handle_id);

            struct ak_vdec_frame frame;
            /* need to get the frame for a while */
            while (1)
            {
                /* get the frame */
                ret = ak_vdec_get_frame(*handle_id, &frame);
                if(ret == 0)
                {
                    //printf("[%d] vdec_%d get frame %d x %d \n",get_tick_count(),*handle_id,frame.width,frame.height);
                    /* set frame status */
                    demo_play_func(&frame);

                    /* relase the frame and push back to decoder */
                    ak_vdec_release_frame(*handle_id, &frame);
                    ak_sleep_ms(40);
                    break;
                }
                else
                {
                    ak_sleep_ms(20);
                    if(retry-- < 0)
                    {
                        ak_print_error_ex(MODULE_ID_APP, "id [%d] get frame failed! waiting 2s!\n", *handle_id);
                        break;
                    }
                }
            }

file_failed:
            fclose(fp);
            if (NULL != data)
                ak_mem_free(data);
        }
    }

    /* finished */
    closedir(dir);

jpeg_exit:
    ak_print_normal_ex(MODULE_ID_APP, "JPEG_sending is done, decoder [%d] send jpeg th exit\n", *handle_id);
    decoder_num--;
    ak_thread_mutex_lock(&refresh_flag_lock);
    //clear_bit
    refresh_record_flag &= ~(1 << (*handle_id));
    refresh_flag &= ~(1 << (*handle_id));
    ak_thread_mutex_unlock(&refresh_flag_lock);
    return NULL;
}

void *vdec_get_frame_th(void *arg)
{
    ak_thread_set_name("vdec_get_frame");
    int *id =(int *)arg;
    int ret = -1;
    int status = 0;
    int retry = 500;//5s

    /* a loop for getting the frame and display */
    do{
        /* get frame */
        struct ak_vdec_frame frame;
        ret = ak_vdec_get_frame(*id, &frame);

        if(ret == 0)
        {
            retry = 500;
            /* invoke the callback function to process the frame*/
            //printf("[%d] vdec_%d get frame %d x %d\n",get_tick_count(),*id,frame.width, frame.height);
            demo_play_func(&frame);

            /* relase the frame and push back to decoder */
            ak_vdec_release_frame(*id, &frame);
        }
        else
        {
            /* get frame failed , sleep 10ms before next cycle*/
            if(retry-- <= 0)
            {
                ak_print_error_ex(MODULE_ID_APP, "id [%d] get frame failed! waiting for 5 s\n", *id);
                goto exit;
            }
            ak_sleep_ms(10);
        }

        /* check the status finished */
        ak_vdec_get_decode_finish(*id, &status);
        /* true means finished */
        if (status)
        {
            decoder_num--;  //that means current decoder is finished
            ak_print_normal(MODULE_ID_APP, "id is [%d] status [%d]\n", *id, status);
            goto exit;
        }
    }while(1);

exit:
    ak_thread_mutex_lock(&refresh_flag_lock);
    //clear_bit
    refresh_record_flag &= ~(1 << *id);
    refresh_flag &= ~(1 << *id);
    ak_thread_mutex_unlock(&refresh_flag_lock);
    ak_print_normal_ex(MODULE_ID_APP, "exit \n");
    return NULL;
}

/* this func is used to set the obj pos in layer */
int set_obj_pos(int decoder, int max_width, int max_height)
{
    switch (decoder)
    {
    case 1:
        obj_pos[0].top = 0;
        obj_pos[0].left = 0;
        obj_pos[0].width = max_width;
        obj_pos[0].height = max_height;
        break;
    case 2:
    case 3:
    case 4:
        obj_pos[0].top = 0;
        obj_pos[0].left = 0;
        obj_pos[0].width = max_width/2;
        obj_pos[0].height = max_height/2;

        obj_pos[1].top = 0;
        obj_pos[1].left = max_width/2;
        obj_pos[1].width = max_width/2;
        obj_pos[1].height = max_height/2;

        obj_pos[2].top = max_height/2;
        obj_pos[2].left = 0;
        obj_pos[2].width = max_width/2;
        obj_pos[2].height = max_height/2;

        obj_pos[3].top = max_height/2;
        obj_pos[3].left = max_width/2;
        obj_pos[3].width = max_width/2;
        obj_pos[3].height = max_height/2;
        break;
    default :
        return AK_FAILED;
    }

    return AK_SUCCESS;
}


/**
 * note: read the appointed video file, decode, and then output to vo  to playing
 */
__STATIC__ int main(int argc, char **argv)
{
    /* init sdk running */
    sdk_run_config config;
    memset(&config, 0, sizeof(sdk_run_config));
    config.mem_trace_flag = SDK_RUN_NORMAL;
    ak_sdk_init( &config );

    /* start to parse the opt */
    if( parse_option( argc, argv ) == AK_FALSE )
    {                                                                           //解释和配置选项
        return AK_FAILED;
    }

    ak_print_normal(MODULE_ID_APP, "*****************************************\n");
    ak_print_normal(MODULE_ID_APP, "** vdec version: %s **\n", ak_vdec_get_version());
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");

    ak_pthread_t jpeg_stream_th[MAX_DE_NUM];
    ak_pthread_t get_stream_th[MAX_DE_NUM];
    ak_pthread_t decode_stream_th[MAX_DE_NUM];
    int handle_id[MAX_DE_NUM] = { -1, -1, -1, -1};  //vdec handle id

    /* SUPPORT the 4 decoder open */
    if (decoder_num > MAX_DE_NUM || decoder_num < 1)
    {
        ak_print_error_ex(MODULE_ID_APP, "Only support the max [%d] decoder\n", MAX_DE_NUM);
        return AK_FAILED;
    }

    /* SUPPORT the 1 decoder open 2560*1920 */
    if (decoder_num > 1 && (res == DE_VIDEO_SIZE_MAX-1))
    {
        ak_print_error_ex(MODULE_ID_APP, "Only support 1 decoder for 2560*1920\n");
        return AK_FAILED;
    }

    if (vo_format > 1 || vo_format < 0)
    {
        ak_print_error_ex(MODULE_ID_APP, "Only support the format-out 0 or 1\n");
        return AK_FAILED;
    }

    memset(&jpeg_stream_th, -1, sizeof(jpeg_stream_th));
    memset(&get_stream_th, -1, sizeof(get_stream_th));
    memset(&decode_stream_th, -1, sizeof(decode_stream_th));

    int ret = 0;
    /* open vo device*/
    struct ak_vo_param vo_param = {0};
    vo_param.width = dst_width;
    vo_param.height = dst_height;
    vo_param.format = vo_format;     //set to RGB888 output
    vo_param.rotate = rotate;

    /* start to open the vo */
    ret = ak_vo_open(&vo_param, DEV_NUM);
    if(ret != 0)
    {
        ak_print_error_ex(MODULE_ID_APP, "ak_vo_open failed![%d]\n", ret);
        return ret;
    }

    /* create the video layer */
    struct ak_vo_layer_in video_layer;
    video_layer.create_layer.height = dst_height;   //layer size
    video_layer.create_layer.width  = dst_width;    //layer size
    video_layer.create_layer.left  = 0;             //layer pos
    video_layer.create_layer.top   = 0;             //layer pos
    video_layer.layer_opt          = 0;             //opt
    ret = ak_vo_create_video_layer(&video_layer, AK_VO_LAYER_VIDEO_1);
    if(ret != 0)
    {
        ak_print_error_ex(MODULE_ID_APP, "ak_vo_create_video_layer failed![%d]\n", ret);
        ak_vo_close(DEV_NUM);
        return ret;
    }

    /* set the obj pos in layer */
    if (set_obj_pos(decoder_num, dst_width, dst_height))
        goto err;

    ak_thread_mutex_init(&refresh_flag_lock, NULL);

    int intype = 0;
    int mode = 0;
    int i = 0;
    int ready_open = decoder_num;

    while (i < ready_open)
    {
        /* get the file */
        if (file == NULL)
        {
            ak_print_error_ex(MODULE_ID_APP, "please input the file\n");

            /* print the hint and notice */
            help_hint();
            goto err;
        }
        else
            ak_print_normal_ex(MODULE_ID_APP, "file %s\n", file);

        /* get the type for output */
        if (type == NULL)
        {
            ak_print_error_ex(MODULE_ID_APP, "please input the type\n");
            /* print the hint and notice */
            help_hint();
            goto err;
        }

        /* get the type for input */
        if(!strcmp(type,"h264"))
        {
            /* h264 */
            ak_print_normal(MODULE_ID_APP, "h264 success\n");
            intype = H264_ENC_TYPE;
        }
        else if(!strcmp(type,"h265"))
        {
            /* h265 */
            ak_print_normal(MODULE_ID_APP, "h265 success\n");
            intype = HEVC_ENC_TYPE;
        }
        else if(!strcmp(type,"jpeg"))
        {
            /* jpeg */
            intype = MJPEG_ENC_TYPE;
            //outtype = AK_YUV420P;
            mode = 1;
        }
        else
        {
            ak_print_error_ex(MODULE_ID_APP, "unsupport video decode input type [%s] \n", type);
            goto err;
        }


        /* output vdec data set */
        //if(intype != MJPEG_ENC_TYPE)
        outtype = AK_YUV420SP;

        /* open vdec */
        struct ak_vdec_param param = {0};
        param.vdec_type = intype;
        param.sc_height = resolutions[res].height;         //vdec height res set
        param.sc_width = resolutions[res].width;          //vdec width res set
        param.output_type = outtype;
        param.frame_buf_num = gi_frame_buff_yum ;
        param.stream_buf_size = gi_stream_buff_size ;

        /* open the vdec */
        ret = ak_vdec_open(&param, &handle_id[i]);
        if(ret != 0)
        {
            ak_print_error_ex(MODULE_ID_APP, "ak_vdec_open failed!\n");

            /* destroy the layer */
            goto err;
        }

        ak_thread_mutex_lock(&refresh_flag_lock);
        /* refresh flag to record */
        refresh_record_flag |= (1 << handle_id[i]);
        ak_thread_mutex_unlock(&refresh_flag_lock);

        /* mode == 1 means the solo frame decode */
        if (mode)
        {
            /* if decode the jpeg data */
            ak_thread_create(&jpeg_stream_th[i], vdec_solo_frame, &handle_id[i], ANYKA_THREAD_MIN_STACK_SIZE, THREAD_PRIO);
        }
        else
        {
            /* h264 or h265 decode, there is two thread */
            /* get frame thread */
            ak_thread_create(&get_stream_th[i], vdec_get_frame_th, &handle_id[i], ANYKA_THREAD_MIN_STACK_SIZE, THREAD_PRIO);

            /* send stream thread */
            ak_thread_create(&decode_stream_th[i], vdec_send_decode_th, &handle_id[i], ANYKA_THREAD_MIN_STACK_SIZE, THREAD_PRIO);
        }
        i++;
    }

    for(i= 0; i < ready_open; i++)
    {
        if (mode)
        {
            ak_thread_join(jpeg_stream_th[i]);
        }
        else
        {
            ak_thread_join(decode_stream_th[i]);
            ak_thread_join(get_stream_th[i]);
        }
    }

err:
    /* close the vdec and vo, release the src */
    i = 0;
    while (i < MAX_DE_NUM && handle_id[i] != -1)
    {
        ak_vdec_close(handle_id[i]);
        i++;
    }

    /* destroy the layer and close vo */
    ak_vo_destroy_layer(AK_VO_LAYER_VIDEO_1);
    ak_vo_close(DEV_NUM);
    ak_thread_mutex_destroy(&refresh_flag_lock);
    ak_sdk_exit();

    return ret;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(main, ak_vdec_sample, Video Decode Sample Program);
#endif

/* end of file */
