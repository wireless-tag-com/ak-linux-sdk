#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/time.h>
#include <regex.h>
#include <getopt.h>
#include <errno.h>

#include "list.h"

#include "ak_ai.h"
#include "ak_aenc.h"
#include "ak_thread.h"
#include "ak_log.h"
#include "ak_mem.h"

#include "ak_vi.h"
#include "ak_venc.h"
#include "ak_mux.h"
#include "ak_mux_sample.h"

#ifdef AK_RTOS
#include <kernel.h>
#endif

static char ac_option_hint[  ][ LEN_HINT ] = {
    "       打印帮助信息" ,
    "[PATH] 录像保存路径" ,
    "[SEC]  录像时长" ,
    "[NUM]  录像录制文件个数,0为不限制录像数量" ,
    "[NUM]  采样率 可选值: 8000,11025,12000,16000,22050,24000,32000,44100,48000" ,
    "[NUM]  采集音量设置0-8" ,
    "[FILE] sensor的配置文件" ,
    "[NUM]  分辨率宽" ,
    "[NUM]  分辨率高" ,
    "[NUM]  帧率" ,
    "[NUM]  I帧出现频率" ,
    "[NUM]  音频编码格式 AVI格式可选值:     - 2,MP3 4,AAC 17,(g711a)PCM_ALAW 18,(g711u)PCM_ULAW 6,PCM\n"
    "                                                  3gp/mp4格式可选值: - 3,AMR 4,AAC 17,(g711a)PCM_ALAW 18,(g711u)PCM_ULAW",
    "[NUM]  venc编码类型 可选值: 0,H264 1,JPEG 2,H265" ,
    "[NUM]  合成文件类型 可选值: 0,MUX_TYPE_AVI 1,MUX_TYPE_MP4. JPEG编码格式仅仅支持AVI" ,
    "[STR]  目录模板,默认:\%Y\%M\%D           可选值:\%Y-年,\%M-月,,\%D-日,\%h-时,\%m-分,\%s-秒,\%S-录制长度ms" ,
    "[STR]  文件模板,默认:\%Y\%M\%D.\%h\%m\%s.\%S 可选值:\%Y-年,\%M-月,,\%D-日,\%h-时,\%m-分,\%s-秒,\%S-录制长度ms" ,
    "[PATH] 录像合成的暂存目录" ,
    "" ,
};

static struct option option_long[ ] = {
    { "help"           , no_argument       , NULL , 'h' } ,                     //"       打印帮助信息" ,
    { "path_save"      , required_argument , NULL , 'a' } ,                     //"[PATH] 录像保存路径" ,
    { "record_sec"     , required_argument , NULL , 'b' } ,                     //"[SEC]  录像时长" ,
    { "record_num"     , required_argument , NULL , 'c' } ,                     //"[NUM]  录像录制文件个数,0为不限制录像数量" ,
    { "ai_sample_rate" , required_argument , NULL , 'd' } ,                     //"[NUM]  采样率 可选值: 8000,11025,12000,16000,22050,24000,32000,44100,48000" ,
    { "ai_volume"      , required_argument , NULL , 'e' } ,                     //"[NUM]  采集音量设置0-8" ,
    { "vi_isp_cfg"     , required_argument , NULL , 'f' } ,                     //"[FILE] sensor的配置文件" ,
    { "vi_width"       , required_argument , NULL , 'g' } ,                     //"[NUM]  分辨率宽" ,
    { "vi_height"      , required_argument , NULL , 'i' } ,                     //"[NUM]  分辨率高" ,
    { "vi_fps"         , required_argument , NULL , 'j' } ,                     //"[NUM]  帧率" ,
    { "venc_goplen"    , required_argument , NULL , 'k' } ,                     //"[NUM]  I帧出现频率" ,
    { "aenc_type"      , required_argument , NULL , 'l' } ,                     //"[NUM]  音频编码格式 AVI格式可选值:     - 2,MP3 4,AAC 17,(g711a)PCM_ALAW 18,(g711u)PCM_ULAW 6,PCM\n"
                                                                                //"                                                  3gp/mp4格式可选值: - 3,AMR 4,AAC 17,(g711a)PCM_ALAW 18,(g711u)PCM_ULAW",
    { "venc_type"      , required_argument , NULL , 'm' } ,                     //"[NUM]  venc编码类型 可选值: 0,H264 1,JPEG 2,H265" ,
    { "mux_type"       , required_argument , NULL , 'n' } ,                     //"[NUM]  合成文件类型 可选值: 0,MUX_TYPE_AVI 1,MUX_TYPE_MP4. JPEG编码格式仅仅支持AVI" ,
    { "path_template"  , required_argument , NULL , 'o' } ,                     //"[STR]  目录模板,默认:\%Y\%M\%D           可选值:\%Y-年,\%M-月,,\%D-日,\%h-时,\%m-分,\%s-秒,\%S-录制长度ms" ,
    { "file_template"  , required_argument , NULL , 'p' } ,                     //"[STR]  文件模板,默认:\%Y\%M\%D.\%h\%m\%s.\%S 可选值:\%Y-年,\%M-月,,\%D-日,\%h-时,\%m-分,\%s-秒,\%S-录制长度ms" ,
    { "path_tmp"       , required_argument , NULL , 'q' } ,                     //"[PATH] 录像合成的暂存目录" ,
    { 0                ,                 0 ,    0 ,  0  } ,                     //"" ,
};

static struct dvr_conf dvr_conf_param = {
    .i_vi_dev                                 = VIDEO_DEV0 ,
    .i_vi_handle                              = HANDLE_NOT_INIT ,
    .i_ai_handle                              = HANDLE_NOT_INIT ,
    .i_venc_handle                            = HANDLE_NOT_INIT ,
    .i_aenc_handle                            = HANDLE_NOT_INIT ,
    .i_mux_handle                             = HANDLE_NOT_INIT ,

    .ac_vi_isp_cfg                            = ISP_CONF_FILE ,                 //sensor的配置文件
    .i_vi_sub_width                           = VI_SUB_WIDTH ,                  //主通道分辨率
    .i_vi_sub_height                          = VI_SUB_HEIGHT ,
    .i_vi_width                               = VI_WIDTH ,                      //次通道分辨率
    .i_vi_height                              = VI_HEIGHT ,
    .i_vi_fps                                 = VI_FPS ,                        //采集帧率

    .audio_in_param.pcm_data_attr.sample_bits = AK_AUDIO_SMPLE_BIT_16 ,         //目前只支持16bit的值
    .audio_in_param.pcm_data_attr.channel_num = AUDIO_CHANNEL_MONO ,            //单声道
    .audio_in_param.pcm_data_attr.sample_rate = AK_AUDIO_SAMPLE_RATE_8000 ,     //采样率
    .audio_in_param.dev_id                    = 0 ,
    .i_aenc_type                              = AK_AUDIO_TYPE_AAC ,             //音频编码格式
    .i_ai_volume                              = AI_VOLUME ,                     //音量设置

    .venc_param.width                         = VI_WIDTH ,                      //分辨率宽度
    .venc_param.height                        = VI_HEIGHT ,                     //分辨率高度
    .venc_param.fps                           = VI_FPS ,                        //FPS
    .venc_param.goplen                        = VENC_GOP_LEN ,                  //I帧出现频率
    .venc_param.target_kbps                   = VENC_TARGET_KBPS ,              //目标码率
    .venc_param.max_kbps                      = VENC_MAX_KBPS ,                 //最大码率
    .venc_param.br_mode                       = BR_MODE_CBR ,                   //码率控制模式
    .venc_param.minqp                         = VENC_MINQP ,                    //QP最小值
    .venc_param.maxqp                         = VENC_MAXQP ,                    //QP最大值
    .venc_param.initqp                        = VENC_INITQP  ,                  //初始化qp值
    .venc_param.jpeg_qlevel                   = JPEG_QLEVEL_DEFAULT ,           //Jpeg编码质量,0 默认,1 最高,2 高,3 低
    .venc_param.chroma_mode                   = CHROMA_4_2_0 ,                  //色彩模式, 0 灰度图像, 1 彩色图像
    .venc_param.max_picture_size              = 0 ,                             //最大图像大小,用于限制I帧大小,0 不限制
    .venc_param.enc_level                     = VENC_LEVEL ,                    //编码级别,[10, 50],一般分辨率越大,配置越大
    .venc_param.smart_mode                    = VENC_SMART_MODE ,               //Smart模式(0不使能smart,1 LTR模式, 2 变goplen模式, 3 跳帧模式)
    .venc_param.smart_goplen                  = VENC_SMART_GOPLEN ,             //Smart模式下的goplen
    .venc_param.smart_quality                 = VENC_SMART_QUALITY ,            //Smart模式图像质量
    .venc_param.smart_static_value            = 0 ,                             //Smart模式时,判断当前是静止场景的区块数
    .venc_param.enc_out_type                  = H264_ENC_TYPE ,                 //编码类型 264|265

    .mux_input_param.audio_type               = AK_AUDIO_TYPE_AAC ,             //音频编码格式 (AVI格式可选值:     - MP3:2, AAC:4, PCM:6,(g711a)PCM_ALAW:17, (g711u)PCM_ULAW:18)
                                                                                //             (3gp/mp4格式可选值: - AMR:3, AAC:4,       (g711a)PCM_ALAW:17, (g711u)PCM_ULAW:18)
    .mux_input_param.video_type               = H264_ENC_TYPE ,                 //venc编码类型 (可选值: 0,H264 1,JPEG 2,H265 )
    .mux_input_param.media_rec_type           = MUX_TYPE_MP4 ,                  //合成文件类型(可选值: 0:MUX_TYPE_AVI, 1:MUX_TYPE_MP4 )
    .mux_input_param.width                    = VI_WIDTH ,
    .mux_input_param.height                   = VI_HEIGHT ,
    .mux_input_param.file_fps                 = VI_FPS ,
    .mux_input_param.record_second            = RECORD_SEC ,                    //录像时长
    .mux_input_param.capture_audio            = AK_TRUE ,
    .mux_input_param.audio_attr.sample_rate   = AK_AUDIO_SAMPLE_RATE_8000 ,     //采样率 (可选值: 8000,11025,12000,16000,22050,24000,32000,44100,48000)
    .mux_input_param.audio_attr.channel_num   = AUDIO_CHANNEL_MONO ,
    .mux_input_param.audio_attr.sample_bits   = AK_AUDIO_SMPLE_BIT_16 ,
    .mux_input_param.data_dest                = MUX_DATA_TO_LOCAL_FILE ,

    .i_sec                                    = RECORD_SEC ,                    //录像时长
    .ac_path_save                             = RECORD_PATH_SAVE,               //保存录像路径
    .ac_path_tmp                              = RECORD_PATH_TMP,                //录制过程的录像保存路径
    .ac_template_path                         = PATH_TEMPLATE,                  //文件保存的路径模板
    .ac_template_file                         = FILE_TEMPLATE,                  //文件保存的文件名模板

    .p_dbdesc_param                           = NULL,                           //数据库记录

    .i_buff_record                            = LEN_BUFF_RECORD,                //录像缓存长度
    .i_buff_index                             = LEN_BUFF_INDEX,                 //索引缓存长度
};

static char gc_run = AK_TRUE;
static char *gpc_prog_name = NULL;                                              //sample|demo名称
static int gi_record_num = RECORD_NUM;

static void create_mux_file( struct dvr_conf *p_dvr_conf )                      //创建和开始一个mux文件
{
    struct ak_mux_start_param mux_start_param;

    mux_start_param.file_path      = p_dvr_conf->ac_file_video;
    mux_start_param.i_buff_record  = p_dvr_conf->i_buff_record;
    mux_start_param.pc_buff_record = p_dvr_conf->pc_buff_record;
    mux_start_param.i_buff_index   = p_dvr_conf->i_buff_index;
    mux_start_param.pc_buff_index  = p_dvr_conf->pc_buff_index;


    if ( ak_mux_start( p_dvr_conf->i_mux_handle, &mux_start_param ) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP,
                     "p_dvr_conf->i_mux_handle= %d p_dvr_conf->ac_file_video= '%s'\n",
                     p_dvr_conf->i_mux_handle, p_dvr_conf->ac_file_video );
    }
}

static int dvr_get_localdate(struct dvr_date *p_dvr_date)                       //获取当前时间
{
    if ( p_dvr_date == NULL ) {
        return AK_FAILED;
    }

    gettimeofday( &p_dvr_date->timeval_val, &p_dvr_date->timezone_val );
    localtime_r( &( p_dvr_date->timeval_val.tv_sec ), &p_dvr_date->tm_val );
    p_dvr_date->year = p_dvr_date->tm_val.tm_year + 1900 ;
    p_dvr_date->mon  = p_dvr_date->tm_val.tm_mon + 1 ;
    p_dvr_date->day  = p_dvr_date->tm_val.tm_mday ;
    p_dvr_date->hour = p_dvr_date->tm_val.tm_hour;
    p_dvr_date->min  = p_dvr_date->tm_val.tm_min;
    p_dvr_date->sec  = p_dvr_date->tm_val.tm_sec;
    p_dvr_date->timezone = p_dvr_date->timezone_val.tz_dsttime;

    return AK_SUCCESS;
}

/*
替换字符串
对pc_src使用pc_rep替换pc_key后保存到pc_res
*/
static int strrep( char *pc_src, char *pc_key, char *pc_rep, char *pc_tmp )
{
    int i_len = strlen( pc_key );                                               //获取需要进行替换的字符串长度
    int i_times = 0;
    char *pc_pos = NULL, *pc_now = pc_src ;

    pc_tmp[ 0 ] = 0x0;
    while( ( pc_pos = strstr( pc_now , pc_key ) ) != NULL ) {                   //定位需要替换的字符串位置
        strncat( pc_tmp , pc_now , pc_pos - pc_now );                           //将定位之前的字符串拷贝到pc_res的结尾去,指定长度拷贝
        strcat( pc_tmp, pc_rep );                                               //将替换字符串拷贝到pc_res结尾
        pc_now = pc_pos + i_len ;                                               //位置跳转
        i_times ++ ;
    }
    if( i_times > 0 ) {
        strcat( pc_tmp , pc_now );                                              //将余下字符串拷贝过去
        i_len = strlen( pc_tmp );
        memcpy( pc_src, pc_tmp, strlen( pc_tmp ) ) ;
        pc_src[ i_len ] = 0x0;
    }
    return i_times;
}

static void save_mux_file( struct dvr_conf *p_dvr_conf )                        //保存录像文件
{
    char ac_file[ LEN_FILE ] , ac_fullpath_file[ LEN_FILE ] , ac_cmd[ LEN_CMD ] ;
    char ac_template_path[ LEN_TEMPLATE ] = {0}, ac_template_file[ LEN_TEMPLATE ] = {0}, ac_tmp[ LEN_TEMPLATE ] = {0} ;
    char ac_num[ FILE_TEMPLATE_NUM ][ LEN_NUM ] ;
    int i_len_full ;

    snprintf( ac_num[ FILE_TEMPLATE_YEAR ] , LEN_NUM, "%04d",   p_dvr_conf->dvr_date_param.year );  //将ak_date结构体的日期时间数据转为字符串
    snprintf( ac_num[ FILE_TEMPLATE_MONTH ], LEN_NUM, "%02d",   p_dvr_conf->dvr_date_param.mon );
    snprintf( ac_num[ FILE_TEMPLATE_DAY ]  , LEN_NUM, "%02d",   p_dvr_conf->dvr_date_param.day );
    snprintf( ac_num[ FILE_TEMPLATE_HOUR ] , LEN_NUM, "%02d",   p_dvr_conf->dvr_date_param.hour );
    snprintf( ac_num[ FILE_TEMPLATE_MIN ]  , LEN_NUM, "%02d",   p_dvr_conf->dvr_date_param.min );
    snprintf( ac_num[ FILE_TEMPLATE_SEC ]  , LEN_NUM, "%02d",   p_dvr_conf->dvr_date_param.sec );
    snprintf( ac_num[ FILE_TEMPLATE_MS ]   , LEN_NUM, "%08llu", p_dvr_conf->i_record_ms );

    memcpy( ac_template_path, p_dvr_conf->ac_template_path, strlen( p_dvr_conf->ac_template_path ) ) ;        //根据模板格式生成路径名称
    strrep( ac_template_path, "%Y",  ac_num[ FILE_TEMPLATE_YEAR ], ac_tmp ) ;
    strrep( ac_template_path, "%M",  ac_num[ FILE_TEMPLATE_MONTH ], ac_tmp ) ;
    strrep( ac_template_path, "%D",  ac_num[ FILE_TEMPLATE_DAY ], ac_tmp ) ;
    strrep( ac_template_path, "%h",  ac_num[ FILE_TEMPLATE_HOUR ], ac_tmp ) ;
    strrep( ac_template_path, "%m",  ac_num[ FILE_TEMPLATE_MIN ], ac_tmp ) ;
    strrep( ac_template_path, "%s",  ac_num[ FILE_TEMPLATE_SEC ], ac_tmp ) ;
    strrep( ac_template_path, "%S",  ac_num[ FILE_TEMPLATE_MS ], ac_tmp ) ;

    memcpy( ac_template_file, p_dvr_conf->ac_template_file, strlen( p_dvr_conf->ac_template_file ) ) ;        //根据模板格式生成文件名称
    strrep( ac_template_file, "%Y",  ac_num[ FILE_TEMPLATE_YEAR ], ac_tmp ) ;
    strrep( ac_template_file, "%M",  ac_num[ FILE_TEMPLATE_MONTH ], ac_tmp ) ;
    strrep( ac_template_file, "%D",  ac_num[ FILE_TEMPLATE_DAY ], ac_tmp ) ;
    strrep( ac_template_file, "%h",  ac_num[ FILE_TEMPLATE_HOUR ], ac_tmp ) ;
    strrep( ac_template_file, "%m",  ac_num[ FILE_TEMPLATE_MIN ], ac_tmp ) ;
    strrep( ac_template_file, "%s",  ac_num[ FILE_TEMPLATE_SEC ], ac_tmp ) ;
    strrep( ac_template_file, "%S",  ac_num[ FILE_TEMPLATE_MS ], ac_tmp ) ;

    switch( p_dvr_conf->mux_input_param.media_rec_type ) {                      //拼接文件名称+扩展名
        case MUX_TYPE_MP4 :
            snprintf( ac_file , sizeof(ac_file), "%s.%s", ac_template_file , FILE_EXT_MP4 );
            break;
        case MUX_TYPE_AVI :
            snprintf( ac_file , sizeof(ac_file), "%s.%s", ac_template_file , FILE_EXT_AVI );
            break;
        default :
            break;
    }
    if( strlen( ac_template_path ) > 0 ) {
        i_len_full = snprintf( ac_fullpath_file , LEN_FILE , "%s/%s" , p_dvr_conf->ac_path_save , ac_template_path );   //根据设定的录像路径和模板路径进行拼接
    }
    else {
        i_len_full = snprintf( ac_fullpath_file , LEN_FILE , "%s" , p_dvr_conf->ac_path_save );
    }
#ifdef AK_RTOS
    snprintf( ac_cmd, sizeof ac_cmd, "mkdir %s", ac_fullpath_file );
#else
    snprintf( ac_cmd, sizeof ac_cmd, "mkdir -p %s", ac_fullpath_file );
#endif
    system( ac_cmd );
    snprintf( ac_fullpath_file + i_len_full , sizeof(ac_fullpath_file) - i_len_full, "/%s", ac_file );        //拼接文件名

    ak_print_normal(MODULE_ID_APP, "ac_fullpath_file= '%s' ac_file= '%s' \n", ac_fullpath_file , ac_file );
    rename( p_dvr_conf->ac_file_video, ac_fullpath_file );                      //移动文件到相应位置
    remove( p_dvr_conf->ac_file_idx );                                          //删除索引暂存文件
}

static int dvr_init_mux( struct dvr_conf *p_dvr_conf )
{
    p_dvr_conf->mux_input_param.frame_interval = p_dvr_conf->i_ai_frame_interval;
    if (ak_mux_open( &p_dvr_conf->mux_input_param, &p_dvr_conf->i_mux_handle)) {
        ak_print_error_ex(MODULE_ID_APP, "mux open error\n" );
        return AK_FAILED;
    }
    else {
        ak_print_normal(MODULE_ID_APP, "mux open success\n" );
    }

    switch( p_dvr_conf->mux_input_param.media_rec_type ) {                      //拼接录像暂存文件名称
        case MUX_TYPE_MP4 :
            snprintf( p_dvr_conf->ac_file_video, sizeof(p_dvr_conf->ac_file_video), "%s/%s.%s", p_dvr_conf->ac_path_tmp, FILE_TMP_SAVE, FILE_EXT_MP4 );
            break;
        case MUX_TYPE_AVI :
            snprintf( p_dvr_conf->ac_file_video, sizeof(p_dvr_conf->ac_file_video), "%s/%s.%s", p_dvr_conf->ac_path_tmp, FILE_TMP_SAVE, FILE_EXT_AVI );
            break;
        default :
            p_dvr_conf->mux_input_param.media_rec_type = MUX_TYPE_MP4;
            snprintf( p_dvr_conf->ac_file_video, sizeof(p_dvr_conf->ac_file_video), "%s/%s.%s", p_dvr_conf->ac_path_tmp, FILE_TMP_SAVE, FILE_EXT_MP4 );
            break;
    }
    snprintf( p_dvr_conf->ac_file_idx, sizeof(p_dvr_conf->ac_file_idx), "%s/%s", p_dvr_conf->ac_path_tmp, FILE_TMP_IDX );         //拼接暂存的视频索引文件
    ak_print_normal(MODULE_ID_APP,
                 "p_dvr_conf->ac_file_video= '%s' p_dvr_conf->ac_file_idx= '%s'\n",
                 p_dvr_conf->ac_file_video, p_dvr_conf->ac_file_idx );

    INIT_LIST_HEAD( &p_dvr_conf->list_head_aenc );                              //初始化aenc venc mux的锁和链表结构体
    INIT_LIST_HEAD( &p_dvr_conf->list_head_venc );
    INIT_LIST_HEAD( &p_dvr_conf->list_head_ai );
    pthread_mutex_init( &p_dvr_conf->pthread_mutex_t_aenc, NULL );
    pthread_mutex_init( &p_dvr_conf->pthread_mutex_t_venc, NULL );
    pthread_mutex_init( &p_dvr_conf->pthread_mutex_t_ai, NULL );

    if( p_dvr_conf->i_buff_record > 0 ) {                                       //初始化录像缓存
        if ( ( p_dvr_conf->pc_buff_record = calloc( 1, p_dvr_conf->i_buff_record ) ) == NULL ) {
            p_dvr_conf->i_buff_record = 0 ;
        }
    }

    if( p_dvr_conf->i_buff_index > 0 ) {                                        //初始化索引缓存
        if ( ( p_dvr_conf->pc_buff_index = calloc( 1, p_dvr_conf->i_buff_index ) ) == NULL ) {
            p_dvr_conf->i_buff_index = 0 ;
        }
    }
    return AK_SUCCESS;
}

/*
    timeval_mark: 对一个timeval结构体时间赋予当前时间戳
    @p_timeval[IN]: 传入的struct timeval结构体指针
    return: 传入的结构体指针地址
*/
static struct timeval *timeval_mark( struct timeval *p_timeval )
{
    gettimeofday( p_timeval , 0 ) ;
    return p_timeval ;
}

/*
    timeval_count: 对两个timeval之间的时间差进行统计
    @p_timeval_begin[IN]: 传入的struct timeval结构体指针,记录开始值
    @p_timeval_end[IN]: 传入的struct timeval,记录结束值
    return: 两个struct timeval的usec差值
*/
static long long timeval_count( struct timeval *p_timeval_begin , struct timeval *p_timeval_end )
{
    long long i_time = ( p_timeval_end->tv_sec - p_timeval_begin->tv_sec ) * S2US + ( p_timeval_end->tv_usec - p_timeval_begin->tv_usec ) ;
    if ( i_time > 0 ) {
        return i_time ;
    }
    else {
        return 0 ;
    }
}

static int get_audio_frame_interval(int sample_rate,int sample_bits, int input_channel, int data_len)
{
    int sample_bytes = (sample_bits >> 3);
    int channel_num = input_channel;
    int sec_bytes = (sample_rate * sample_bytes * channel_num);

    if (0 == sec_bytes) {
        sec_bytes = 8000 * (16 >> 3) * 1;
    }

    return ((1000 * data_len) / sec_bytes);
}

static void* thread_ai( void *pv_conf )                                         //录像ai采集线程
{
    struct dvr_conf *p_dvr_conf = ( struct dvr_conf * )pv_conf ;
    int ret ;
    struct frame_entry *p_frame_entry;

    ak_thread_set_name( "thread_ai" );
    while( p_dvr_conf->i_run == AK_TRUE ) {
        if ( ( p_frame_entry = ( struct frame_entry * )calloc( 1, sizeof( struct frame_entry ) ) ) == NULL ) {
            ak_print_error_ex(MODULE_ID_APP, "calloc failed\n" );
            continue ;
        }
        if ( ( ret = ak_ai_get_frame( p_dvr_conf->i_ai_handle, &p_frame_entry->frame_stream, 0 ) ) == AK_SUCCESS ) {
            MUTEX_LOCK_AI;
            list_add_tail( &p_frame_entry->list , &p_dvr_conf->list_head_ai );
            MUTEX_UNLOCK_AI;
        }
        else {
            FREE_POINT( p_frame_entry );
            switch( ret ) {
                case ERROR_AI_NO_DATA :
                    ak_sleep_ms( 10 );
                    continue;
                default :
                    ak_print_error_ex(MODULE_ID_APP, "ak_ai_get_frame error ret=%d\n", ret );
                    break;
            }
        }
    }

    ak_print_normal(MODULE_ID_APP, "%s exit\n", __func__);
    ak_thread_exit();
    return NULL;
}

static void* thread_aenc( void *pv_conf )                                       //录像aenc采集线程
{
    struct dvr_conf *p_dvr_conf = ( struct dvr_conf * )pv_conf ;
    struct frame_entry *p_frame_now, *p_frame_next;
    struct aenc_stream_entry *p_aenc_stream_entry ;
    int ret;

    ak_thread_set_name( "thread_aenc" );
    while( p_dvr_conf->i_run == AK_TRUE ) {
        list_for_each_entry_safe( p_frame_now, p_frame_next, &p_dvr_conf->list_head_ai, list ) {//遍历AI链表
            MUTEX_LOCK_AI;
            list_del_init( &p_frame_now->list );                                //从链表脱钩
            MUTEX_UNLOCK_AI;

            if ( ak_aenc_send_frame( p_dvr_conf->i_aenc_handle, &p_frame_now->frame_stream, 0 ) != AK_SUCCESS ) {
                ak_print_error_ex( MODULE_ID_APP, "send frame error\n" );
            }
            ak_ai_release_frame( p_dvr_conf->i_ai_handle, &p_frame_now->frame_stream );
            FREE_POINT( p_frame_now );
        }

        if ( ( p_aenc_stream_entry = ( struct aenc_stream_entry * )calloc( 1, sizeof( struct aenc_stream_entry ) ) ) == NULL ) {
            ak_print_error_ex(MODULE_ID_APP, "calloc failed\n" );
            continue ;
        }
        if ( ( ret = ak_aenc_get_stream( p_dvr_conf->i_aenc_handle , &p_aenc_stream_entry->stream, 0 ) ) == AK_SUCCESS ) {
            MUTEX_LOCK_AENC;
            list_add_tail( &p_aenc_stream_entry->list , &p_dvr_conf->list_head_aenc );
            MUTEX_UNLOCK_AENC;
        }
        else {
            switch( ret ) {
                case ERROR_AENC_NO_DATA :
                    ak_sleep_ms( 10 );
                    break ;
                default :
                    ak_print_error_ex(MODULE_ID_APP, "ak_aenc_get_stream error ret=%d\n", ret );
                    break;
            }
            FREE_POINT( p_aenc_stream_entry );
        }
    }

    ak_print_normal(MODULE_ID_APP, "%s exit\n", __func__);
    ak_thread_exit();
    return NULL;
}

static void* thread_vi_venc( void *pv_conf )                                    //录像venc采集线程
{
    struct video_input_frame vi_frame_get, vi_frame_mark;
    struct venc_stream_entry *p_venc_stream_entry;
    struct dvr_conf *p_dvr_conf = ( struct dvr_conf * )pv_conf ;
    unsigned long long i_ts_prev = 0, i_us_split;                               //上一帧的时间戳,用以进行调试
    struct timeval timeval_begin, timeval_end ;
    int i_times_venc = 0  ;
    char c_gop;

    ak_thread_set_name( "thread_vi_venc" );
    while( p_dvr_conf->i_run == AK_TRUE ) {
        memset( &vi_frame_get, 0x00, sizeof( vi_frame_get ) );
        if ( ak_vi_get_frame( VIDEO_CHN0, &vi_frame_get) == AK_SUCCESS ) {          //获取一帧yuv视频
            p_venc_stream_entry = ( struct venc_stream_entry *)calloc( 1, sizeof( struct venc_stream_entry ) );         //创建一个视频编码帧的链表结构体
            if ( ( p_venc_stream_entry != NULL ) &&
                 ( ak_venc_encode_frame( p_dvr_conf->i_venc_handle,
                                         vi_frame_get.vi_frame.data, vi_frame_get.vi_frame.len, vi_frame_get.mdinfo,
                                         &p_venc_stream_entry->stream ) == AK_SUCCESS ) ) {         //成功编码一帧视频
                vi_frame_mark = vi_frame_get;
                ak_vi_release_frame( VIDEO_CHN0, &vi_frame_get );
                i_times_venc ++ ;
                p_venc_stream_entry->stream.ts = vi_frame_mark.vi_frame.ts;          //对时间戳进行赋值
                p_venc_stream_entry->stream.seq_no = vi_frame_mark.vi_frame.seq_no ; //对帧序列进行赋值

                if  ( ( p_venc_stream_entry->stream.frame_type == FRAME_TYPE_I ) ||
                      ( ( p_dvr_conf->venc_param.enc_out_type == MJPEG_ENC_TYPE ) &&
                        ( ( p_venc_stream_entry->stream.seq_no - 1 ) % p_dvr_conf->venc_param.goplen == 0 ) ) ) {      //如果是视频流判断是否当前帧是I帧，如果是JPEG编码则判断是否达到GOP数
                    c_gop = AK_TRUE;
                }
                else {
                    c_gop = AK_FALSE;
                }

                if ( c_gop == AK_TRUE ) {                                       //如果是视频流判断是否当前帧是I帧，如果是JPEG编码则判断是否达到GOP数
                    dvr_get_localdate( &p_venc_stream_entry->dvr_date_param );  //获取当前时间,用以在mux线程判断进行截断操作
                    if( i_ts_prev != 0 ) {                                      //测试代码,判断sensor的采集周期
                        i_us_split = timeval_count( &timeval_begin, timeval_mark( &timeval_end ) ) ;
                        timeval_begin = timeval_end ;
                        ak_print_normal_ex( MODULE_ID_APP,
                                            "i_times_venc= %d i_ts_prev= %llu vi_frame_mark.vi_frame.ts= %llu diff= %llu i_us_split= %llu fps= %f\n" ,
                                            i_times_venc, i_ts_prev, vi_frame_mark.vi_frame.ts, vi_frame_mark.vi_frame.ts - i_ts_prev, i_us_split ,
                                           ( double )i_times_venc / i_us_split * 1000000 );
                        i_times_venc = 0 ;
                    }
                    else {
                        timeval_mark( &timeval_begin );
                    }
                    i_ts_prev = vi_frame_mark.vi_frame.ts;
                }
                MUTEX_LOCK_VENC;
                list_add_tail( &p_venc_stream_entry->list , &p_dvr_conf->list_head_venc );//加到链表中
                MUTEX_UNLOCK_VENC;
            }
            else {
                free( p_venc_stream_entry );
            }
        }
    }

    ak_print_normal(MODULE_ID_APP, "%s exit\n", __func__);
    ak_thread_exit();
    return NULL;
}

static void* thread_mux( void *pv_conf )                                        //录像合成线程
{
    int i_mux_init = AK_FALSE;
    unsigned long long i_ts_start = 0;
    struct dvr_conf *p_dvr_conf = ( struct dvr_conf * )pv_conf ;
    struct venc_stream_entry *p_venc_stream_now = NULL , *p_venc_stream_next = NULL;
    struct aenc_stream_entry *p_aenc_stream_now = NULL , *p_aenc_stream_next = NULL;
    int i_num_file = 0, i_aenc_times ;
    char c_aenc_loop, c_gop ;

    ak_thread_set_name( "thread_mux" );
    while( p_dvr_conf->i_run == AK_TRUE ) {
        list_for_each_entry_safe( p_venc_stream_now, p_venc_stream_next, &p_dvr_conf->list_head_venc, list ) {//遍历venc链表
            if( p_venc_stream_now != NULL ) {                                   //获取一帧编码数据
                MUTEX_LOCK_VENC;                                                //加锁,开始mux venc数据
                list_del_init( &p_venc_stream_now->list );                      //从链表脱钩
                MUTEX_UNLOCK_VENC;
                if  ( ( p_venc_stream_now->stream.frame_type == FRAME_TYPE_I ) ||
                      ( ( p_dvr_conf->mux_input_param.video_type == MJPEG_ENC_TYPE ) &&
                        ( ( p_venc_stream_now->stream.seq_no - 1 ) % p_dvr_conf->venc_param.goplen == 0 ) ) ) {         //如果是视频流判断是否当前帧是I帧，如果是JPEG编码则判断是否达到GOP数
                    c_gop = AK_TRUE;
                }
                else {
                    c_gop = AK_FALSE;
                }

                if ( ( c_gop == AK_TRUE ) &&
                     ( ( i_mux_init == AK_FALSE ) ||
                       ( ( p_venc_stream_now->stream.ts - i_ts_start + MS_CUT_ADD ) >= p_dvr_conf->i_sec * S2MS ) ) ) { //判断是否已经开始录像,判断录像时长是否需要截断并开始新的录像
                    if ( i_mux_init == AK_TRUE ) {                              //已经开始录像则保存录像
                        p_dvr_conf->i_record_ms = p_venc_stream_now->stream.ts - i_ts_start ;       //获取录像时长(毫秒)
#if 1
                        i_aenc_times = 0;
                        for( ;; ) {                                             //在该录像结束前,合成下一个视频首个I帧时间戳之前的全部音频帧
                            c_aenc_loop = AK_FALSE;
                            list_for_each_entry_safe( p_aenc_stream_now, p_aenc_stream_next, &p_dvr_conf->list_head_aenc, list ) {
                                if ( p_aenc_stream_now->stream.ts < p_venc_stream_now->stream.ts ) {
                                    MUTEX_LOCK_AENC;
                                    list_del_init( &p_aenc_stream_now->list );  //从链表脱钩
                                    MUTEX_UNLOCK_AENC;
                                    if ( ak_mux_add_audio( p_dvr_conf->i_mux_handle, &( p_aenc_stream_now->stream ) ) != AK_SUCCESS ) {
                                        ak_print_error_ex(MODULE_ID_APP,
                                                     "p_dvr_conf->i_mux_handle= %d p_aenc_stream_now->stream => data= %p len= %d ts= %llu seq_no= %lu \n",
                                                     p_dvr_conf->i_mux_handle, p_aenc_stream_now->stream.data, p_aenc_stream_now->stream.len, p_aenc_stream_now->stream.ts, p_aenc_stream_now->stream.seq_no );

                                    }
                                    ak_aenc_release_stream( p_dvr_conf->i_aenc_handle, &p_aenc_stream_now->stream );
                                    FREE_POINT( p_aenc_stream_now );
                                }
                                else {                                          //获取到的音频帧的时间戳大于或等于下一个视频开始的I帧时间戳则表示音频帧已经全部合成了
                                    goto thread_mux_savefile ;                  //跳转到合成文件保存代码
                                }
                                c_aenc_loop = AK_TRUE ;
                            }
                            if ( c_aenc_loop == AK_FALSE ) {                    //轮询TIMES_CUT_LOOP次均为空则退出
                                i_aenc_times ++ ;
                                if( i_aenc_times > TIMES_CUT_LOOP ) {
                                    goto thread_mux_savefile ;
                                }
                                ak_sleep_ms( 10 );
                                continue ;
                            }
                        }
#endif
thread_mux_savefile :
                        ak_mux_stop( p_dvr_conf->i_mux_handle );                //停止当前的录像
                        save_mux_file( p_dvr_conf );                            //保存当前的录像文件
                        i_num_file ++ ;
                    }
                    if( ( gi_record_num > 0 ) && ( i_num_file >= gi_record_num ) ) {      //合成文件个数达到指定数量则退出
                        i_mux_init = AK_FALSE;
                        gc_run = AK_FALSE;
                    }
                    else {
                        create_mux_file( p_dvr_conf );                          //重新创建录像文件
                        i_ts_start = p_venc_stream_now->stream.ts ;             //获取开始录像的时间戳
                        p_dvr_conf->dvr_date_param = p_venc_stream_now->dvr_date_param ;  //赋值dvr_date_param
                        i_mux_init = AK_TRUE;
                    }
                }
                if( i_mux_init == AK_TRUE ) {
                    ak_mux_add_video( p_dvr_conf->i_mux_handle, &p_venc_stream_now->stream );
                }
                ak_venc_release_stream( p_dvr_conf->i_venc_handle, &p_venc_stream_now->stream );    //释放这一帧数据
                FREE_POINT( p_venc_stream_now );                                //释放
            }
        }

        if ( i_mux_init != AK_TRUE ) {
            goto thread_mux_continue ;
        }


        list_for_each_entry_safe( p_aenc_stream_now, p_aenc_stream_next, &p_dvr_conf->list_head_aenc, list ) {
            if( p_aenc_stream_now != NULL ) {
                MUTEX_LOCK_AENC;                                                        //加锁,开始mux aenc数据
                list_del_init( &p_aenc_stream_now->list );                      //从链表脱钩
                MUTEX_UNLOCK_AENC;
                if ( ak_mux_add_audio( p_dvr_conf->i_mux_handle, &( p_aenc_stream_now->stream ) ) != AK_SUCCESS ) {
                    ak_print_error_ex(MODULE_ID_APP,
                                 "p_dvr_conf->i_mux_handle= %d p_aenc_stream_now->stream => data= %p len= %d ts= %llu seq_no= %lu \n",
                                 p_dvr_conf->i_mux_handle, p_aenc_stream_now->stream.data, p_aenc_stream_now->stream.len, p_aenc_stream_now->stream.ts, p_aenc_stream_now->stream.seq_no );

                }
                ak_aenc_release_stream( p_dvr_conf->i_aenc_handle, &p_aenc_stream_now->stream );
                FREE_POINT( p_aenc_stream_now );
            }
        }

thread_mux_continue :
        ak_sleep_ms(10);
    }
    ak_mux_stop( p_dvr_conf->i_mux_handle );

    ak_print_normal(MODULE_ID_APP, "%s exit\n", __func__);
    ak_thread_exit();
    return NULL;
}

static int dvr_init_aenc( struct dvr_conf *p_dvr_conf )
{
    struct aenc_param aenc_param_in;

    memcpy( &aenc_param_in.aenc_data_attr , &p_dvr_conf->mux_input_param.audio_attr , sizeof(struct ak_audio_data_attr ) );
    aenc_param_in.type = p_dvr_conf->mux_input_param.audio_type ;
    ak_print_normal(MODULE_ID_APP,
                 "aenc_param_in=> sample_rate=%d channel_num=%d sample_bits=%d type=%d \n" ,
                 aenc_param_in.aenc_data_attr.sample_rate , aenc_param_in.aenc_data_attr.channel_num , aenc_param_in.aenc_data_attr.sample_bits, aenc_param_in.type );
    if ( ak_aenc_open( &aenc_param_in, &p_dvr_conf->i_aenc_handle ) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP, "ak_aenc_open failed\n" );
        return AK_FAILED;
    }
    return AK_SUCCESS;
}

static int ak_dvr_init( struct dvr_conf *p_dvr_conf )
{
    if ( dvr_init_ai( p_dvr_conf ) ) {
        ak_print_error_ex(MODULE_ID_APP, "dvr_init_ai failed\n" );
        return AK_FAILED;
    }

    if ( dvr_init_aenc( p_dvr_conf ) ) {
        ak_print_error_ex(MODULE_ID_APP, "dvr_init_aenc failed\n" );
        return AK_FAILED;
    }

    if ( dvr_init_vi( p_dvr_conf ) ) {
        ak_print_error_ex(MODULE_ID_APP, "dvr_init_vi failed\n" );
        return AK_FAILED;
    }

    if ( dvr_init_venc( p_dvr_conf ) ) {
        ak_print_error_ex(MODULE_ID_APP, "dvr_init_venc failed\n" );
        return AK_FAILED;
    }

    if ( dvr_init_mux( p_dvr_conf ) ) {
        ak_print_error_ex(MODULE_ID_APP, "dvr_init_mux failed\n" );
        return AK_FAILED;
    }

    return AK_SUCCESS;
}

static int ak_dvr_start( struct dvr_conf *p_dvr_conf )
{
    p_dvr_conf->i_run = AK_TRUE;

    ak_ai_start_capture( p_dvr_conf->i_ai_handle );
    ak_thread_create( &p_dvr_conf->a_pthread_t[ THREAD_MUX ], thread_mux, ( void * )p_dvr_conf, ANYKA_THREAD_MIN_STACK_SIZE, -1);
    ak_thread_create( &p_dvr_conf->a_pthread_t[ THREAD_AI ], thread_ai, ( void * )p_dvr_conf, ANYKA_THREAD_MIN_STACK_SIZE, -1);
    ak_thread_create( &p_dvr_conf->a_pthread_t[ THREAD_AENC ], thread_aenc, ( void * )p_dvr_conf, ANYKA_THREAD_MIN_STACK_SIZE, -1);
    ak_thread_create( &p_dvr_conf->a_pthread_t[ THREAD_VI_VENC ], thread_vi_venc, ( void * )p_dvr_conf, ANYKA_THREAD_MIN_STACK_SIZE, 90);

    return AK_SUCCESS;                                                          //此处暂时先不判断线程是否启动成功
}

static int ak_dvr_stop( struct dvr_conf *p_dvr_conf )
{
    p_dvr_conf->i_run = AK_FALSE;
    ak_ai_stop_capture( p_dvr_conf->i_ai_handle );

    ak_thread_join( p_dvr_conf->a_pthread_t[ THREAD_MUX ] ) ;
    ak_thread_join( p_dvr_conf->a_pthread_t[ THREAD_AI ] ) ;
    ak_thread_join( p_dvr_conf->a_pthread_t[ THREAD_AENC ] ) ;
    ak_thread_join( p_dvr_conf->a_pthread_t[ THREAD_VI_VENC ] ) ;

    ak_mux_stop( p_dvr_conf->i_mux_handle );
    ak_print_normal(MODULE_ID_APP, "p_dvr_conf->i_run= %d\n" , p_dvr_conf->i_run );

    return AK_SUCCESS;
}

static int ak_dvr_exit( struct dvr_conf *p_dvr_conf )
{
    if( p_dvr_conf->i_ai_handle != HANDLE_NOT_INIT ) {
        ak_ai_close( p_dvr_conf->i_ai_handle );
        p_dvr_conf->i_ai_handle = HANDLE_NOT_INIT;
    }
    if( p_dvr_conf->i_aenc_handle != HANDLE_NOT_INIT ) {
        ak_aenc_close( p_dvr_conf->i_aenc_handle );
        p_dvr_conf->i_aenc_handle = HANDLE_NOT_INIT;
    }
    if( p_dvr_conf->i_vi_handle != HANDLE_NOT_INIT ) {
        ak_vi_disable_chn( VIDEO_CHN0 );
        ak_vi_disable_dev(p_dvr_conf->i_vi_dev);
        ak_vi_close( p_dvr_conf->i_vi_dev );
        p_dvr_conf->i_vi_handle = HANDLE_NOT_INIT;
    }
    if( p_dvr_conf->i_venc_handle != HANDLE_NOT_INIT ) {
        ak_venc_close( p_dvr_conf->i_venc_handle );
        p_dvr_conf->i_venc_handle = HANDLE_NOT_INIT;
    }
    if( p_dvr_conf->i_mux_handle != HANDLE_NOT_INIT ) {
        ak_mux_close( p_dvr_conf->i_mux_handle );
        p_dvr_conf->i_mux_handle = HANDLE_NOT_INIT;
    }
    FREE_POINT( p_dvr_conf->pc_buff_record );
    FREE_POINT( p_dvr_conf->pc_buff_index );
    return AK_SUCCESS;
}

static int dvr_init_ai( struct dvr_conf *p_dvr_conf )
{
    if ( ak_ai_open( &p_dvr_conf->audio_in_param, &p_dvr_conf->i_ai_handle ) ) {
        return AK_FAILED;
    }

    p_dvr_conf->i_ai_frame_len =  ak_get_dev_buf_len( p_dvr_conf->audio_in_param.pcm_data_attr.sample_rate ) ;
    p_dvr_conf->i_ai_frame_interval = get_audio_frame_interval( p_dvr_conf->audio_in_param.pcm_data_attr.sample_rate,
                                                                p_dvr_conf->audio_in_param.pcm_data_attr.sample_bits,
                                                                p_dvr_conf->audio_in_param.pcm_data_attr.channel_num,
                                                                p_dvr_conf->i_ai_frame_len );

    ak_print_normal(MODULE_ID_APP,
                 "p_dvr_conf->i_ai_frame_len= %d p_dvr_conf->i_ai_frame_interval=%d p_dvr_conf->i_ai_volume= %d\n" ,
                 p_dvr_conf->i_ai_frame_len , p_dvr_conf->i_ai_frame_interval, p_dvr_conf->i_ai_volume );
    ak_ai_enable_nr( p_dvr_conf->i_ai_handle, AUDIO_FUNC_ENABLE);
    ak_ai_enable_agc( p_dvr_conf->i_ai_handle, AUDIO_FUNC_ENABLE);
    ak_ai_enable_aec( p_dvr_conf->i_ai_handle, AUDIO_FUNC_ENABLE);
    ak_ai_set_source( p_dvr_conf->i_ai_handle, AI_SOURCE_MIC );
    ak_ai_set_gain( p_dvr_conf->i_ai_handle, p_dvr_conf->i_ai_volume );         //必须先调用ak_ai_set_source再调用ak_ai_set_gain设置音量
    return AK_SUCCESS;
}

static int dvr_init_vi( struct dvr_conf *p_dvr_conf )
{
    RECTANGLE_S venc_res = {0};
    VI_DEV_ATTR dev_attr= {0};
    VI_CHN_ATTR chn_attr_set = {0};

    if ( ak_vi_open( p_dvr_conf->i_vi_dev ) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP, "vi device open failed\n" );
        return AK_FAILED;
    }

#ifndef __CHIP_AK37E_SERIES                                                     //AK37E不支持sensor配置文件选项
    if ( ak_vi_load_sensor_cfg( p_dvr_conf->i_vi_dev, p_dvr_conf->ac_vi_isp_cfg ) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP, "vi device load cfg [%s] failed!\n", p_dvr_conf->ac_vi_isp_cfg );
        goto dvr_init_vi_failure;
    }
#endif

    dev_attr.dev_id = p_dvr_conf->i_vi_dev;
    dev_attr.crop.left = 0;
    dev_attr.crop.top = 0;
    dev_attr.crop.width = p_dvr_conf->i_vi_width;
    dev_attr.crop.height = p_dvr_conf->i_vi_height;
    dev_attr.max_width = p_dvr_conf->i_vi_width;
    dev_attr.max_height = p_dvr_conf->i_vi_height;
    dev_attr.sub_max_width = p_dvr_conf->i_vi_sub_width;
    dev_attr.sub_max_height = p_dvr_conf->i_vi_sub_height;

    if ( ak_vi_get_sensor_resolution(p_dvr_conf->i_vi_dev, &venc_res) != AK_SUCCESS ) {
        goto dvr_init_vi_failure;
    }
    else {
        ak_print_normal(MODULE_ID_APP, "get dev res w:[%d]h:[%d]\n",venc_res.width, venc_res.height );
        dev_attr.crop.width = venc_res.width;
        dev_attr.crop.height = venc_res.height;
    }

    if ( ak_vi_set_dev_attr(p_dvr_conf->i_vi_dev, &dev_attr) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP, "vi device set device attribute failed!\n" );
        goto dvr_init_vi_failure;
    }

    chn_attr_set.chn_id = VIDEO_CHN0;
    chn_attr_set.res.width = p_dvr_conf->i_vi_width;
    chn_attr_set.res.height = p_dvr_conf->i_vi_height;
    chn_attr_set.frame_rate = p_dvr_conf->i_vi_fps;
    chn_attr_set.frame_depth = VI_FRAME_DEPTH ;
    if ( ak_vi_set_chn_attr( VIDEO_CHN0, &chn_attr_set) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP, "vi device set channel attribute failed!\n" );
        goto dvr_init_vi_failure;
    }

#ifndef __CHIP_AK37E_SERIES
    chn_attr_set.chn_id = VIDEO_CHN1;                                           //AK37E仅仅支持主通道,次通道屏蔽
    chn_attr_set.res.width = p_dvr_conf->i_vi_sub_width;;
    chn_attr_set.res.height = p_dvr_conf->i_vi_sub_height;
    chn_attr_set.frame_rate = p_dvr_conf->i_vi_fps;
    chn_attr_set.frame_depth = VI_FRAME_DEPTH;
    if ( ak_vi_set_chn_attr( VIDEO_CHN1, &chn_attr_set) != AK_SUCCESS ) {
        ak_print_normal(MODULE_ID_APP, "vi device set sub channel attribute failed!\n" );
        goto dvr_init_vi_failure;
    }
#endif

    if ( ak_vi_enable_dev( p_dvr_conf->i_vi_dev ) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP, "vi device enable device failed!\n" );
        goto dvr_init_vi_failure;
    }

    if ( ak_vi_enable_chn( VIDEO_CHN0 ) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP, "vi device enable device failed!\n" );
        goto dvr_init_vi_failure;
    }

    p_dvr_conf->i_vi_handle = HANDLE_INIT;
    return AK_SUCCESS;

dvr_init_vi_failure:
    ak_vi_close( p_dvr_conf->i_vi_dev );
    p_dvr_conf->i_vi_handle = HANDLE_NOT_INIT;
    return AK_FAILED;
}

static int dvr_init_venc( struct dvr_conf *p_dvr_conf )
{
    switch( p_dvr_conf->venc_param.enc_out_type ) {
        case H264_ENC_TYPE :
            p_dvr_conf->venc_param.profile = PROFILE_MAIN;
            break;
        case HEVC_ENC_TYPE :
            p_dvr_conf->venc_param.profile = PROFILE_HEVC_MAIN;
            break;
        case MJPEG_ENC_TYPE :
            p_dvr_conf->venc_param.profile = PROFILE_JPEG;
            break;
        default :
            ak_print_error_ex(MODULE_ID_APP, "enc_out_type= %d error.\n" , p_dvr_conf->venc_param.enc_out_type );
            return AK_FAILED;
    }
    return ak_venc_open( &p_dvr_conf->venc_param, &p_dvr_conf->i_venc_handle );
}

static int help_hint(void)                                                      //打印帮助信息
{
    int i ;

    printf( "%s\n" , gpc_prog_name ) ;
    for( i = 0 ; i < sizeof( option_long ) / sizeof( struct option ) ; i ++ ) {
        if( option_long[ i ].val != 0 ) {
            printf( "\t--%-16s -%c %s\n" , option_long[ i ].name , option_long[ i ].val , ac_option_hint[ i ] ) ;
        }
    }
    printf( HING_USAGE, gpc_prog_name, gpc_prog_name, gpc_prog_name, gpc_prog_name );
    printf( "\n\n" ) ;
    return 0 ;
}

/*
    get_option_short: 根据option_long填充短选项字符串
    @p_option[IN]: struct option数组地址
    @i_num_option[IN]: 数组元素个数
    @pc_option_short[IN]: 填充的数组地址
    @i_len_option[IN]: 数组长度
    return: pc_option_short
*/
static char *get_option_short( struct option *p_option, int i_num_option, char *pc_option_short, int i_len_option )
{
    int i;
    int i_offset = 0;
    char c_option;

    for( i = 0 ; i < i_num_option ; i ++ ) {
        if( ( c_option = p_option[ i ].val ) == 0 ) {
            continue;
        }
        switch( p_option[ i ].has_arg ){
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

/*
    parse_option: 对传入的选项进行解释
    @argc[IN]: 从程序入口传入的选项数量
    @argv[IN]: 选项内容的字符串指针数组
    return: AK_TRUE:继续运行  AK_FALSE:退出应用,在打印帮助或者选项解释错误的时候使用
*/
static int parse_option( int argc, char **argv )
{
    int i_option;

    char ac_option_short[ LEN_OPTION_SHORT ];
    int i_array_num = sizeof( option_long ) / sizeof( struct option ) ;
    char c_flag = AK_TRUE;
    gpc_prog_name = argv[ 0 ];

    optind = 0;
    if ( argc <= 1 ) {
        goto print_help ;
    }
    get_option_short( option_long, i_array_num , ac_option_short , LEN_OPTION_SHORT );
    while((i_option = getopt_long(argc , argv , ac_option_short , option_long , NULL)) > 0) {
        switch(i_option) {
            case 'h' :                                                          //help
                goto print_help ;
            case 'a' :                                                          //path_save - 录像保存路径
                snprintf( dvr_conf_param.ac_path_save, LEN_FILE, "%s", optarg ) ;
                snprintf( dvr_conf_param.ac_path_tmp, LEN_FILE, "%s/tmp", optarg ) ;
                break;
            case 'b' :                                                          //record_sec - 录像时长
                dvr_conf_param.i_sec = atoi( optarg );
                dvr_conf_param.mux_input_param.record_second = atoi( optarg );
                if(dvr_conf_param.i_sec < 0){
                    ak_print_error_ex(MODULE_ID_APP, "record_sec %d is illegal. Must greater than 0\n" ,
                                       dvr_conf_param.mux_input_param.record_second );
                    goto print_help ;
                }
                break;
            case 'c' :                                                          //record_num - 录像录制文件个数
                gi_record_num = atoi( optarg );
                break;
            case 'd' :                                                          //ai_sample_rate - 采样率
                dvr_conf_param.audio_in_param.pcm_data_attr.sample_rate = atoi( optarg );
                dvr_conf_param.mux_input_param.audio_attr.sample_rate   = atoi( optarg );
                break;
            case 'e' :                                                          //ai_volume - 采集音量设置
                dvr_conf_param.i_ai_volume = atoi( optarg );
                if ( ( dvr_conf_param.i_ai_volume < 0 ) || ( dvr_conf_param.i_ai_volume > 8 ) ) {
                    ak_print_error_ex(MODULE_ID_APP, "volume %d is illegal. Must in [0-8]\n" , dvr_conf_param.i_ai_volume );
                    goto print_help ;
                }
                break;
            case 'f' :                                                          //vi_isp_cfg - sensor的配置文件
                snprintf( dvr_conf_param.ac_vi_isp_cfg, LEN_FILE, "%s", optarg );
                break;
            case 'g' :                                                          //vi_width - 分辨率宽
                dvr_conf_param.i_vi_width = atoi( optarg );
                dvr_conf_param.venc_param.width = atoi( optarg );
                dvr_conf_param.mux_input_param.width = atoi( optarg );
                break;
            case 'i' :                                                          //vi_height - 分辨率高
                dvr_conf_param.i_vi_height = atoi( optarg );
                dvr_conf_param.venc_param.height = atoi( optarg );
                dvr_conf_param.mux_input_param.height = atoi( optarg );
                break;
            case 'j' :                                                          //vi_fps - 帧率
                dvr_conf_param.i_vi_fps = atoi( optarg );
                dvr_conf_param.venc_param.fps = atoi( optarg );
                dvr_conf_param.mux_input_param.file_fps = atoi( optarg );
                break;
            case 'k' :                                                          //venc_goplen - I帧出现频率
                dvr_conf_param.venc_param.goplen = atoi( optarg );
                break;
            case 'l' :                                                          //aenc_type - 音频编码格式
                dvr_conf_param.i_aenc_type = atoi( optarg );
                dvr_conf_param.mux_input_param.audio_type = atoi( optarg );
                break;
            case 'm' :                                                          //venc_type - venc编码类型
                dvr_conf_param.venc_param.enc_out_type = atoi( optarg );
                dvr_conf_param.mux_input_param.video_type = atoi( optarg );
                break;
            case 'n' :                                                          //mux_type - 合成文件类型
                dvr_conf_param.mux_input_param.media_rec_type = atoi( optarg );
                break;
            case 'o' :                                                          //保存路径模板
                snprintf( dvr_conf_param.ac_template_path, LEN_FILE, "%s", optarg );
                break;
            case 'p' :                                                          //文件名称模板
                if( strlen( optarg ) <= 0 ) {
                    ak_print_error_ex(MODULE_ID_APP, "File template can't be null.\n" );
                    goto print_help ;
                }
                snprintf( dvr_conf_param.ac_template_file, LEN_FILE, "%s", optarg );
                break;
            case 'q' :                                                          //录像合成暂存目录位置
                if( strlen( optarg ) <= 0 ) {
                    ak_print_error_ex(MODULE_ID_APP, "Path temp can't be null.\n" );
                    goto print_help ;
                }
                snprintf( dvr_conf_param.ac_path_tmp, LEN_FILE, "%s", optarg );
                break;
            default :
print_help:
                help_hint();
                c_flag = AK_FALSE;
                goto parse_option_end;
        }
    }
parse_option_end:
    return c_flag;
}

#ifdef AK_RTOS
static int mux_sample(int argc, char *argv[])
#else
int main(int argc, char **argv)
#endif
{
    sdk_run_config config = {0};
    config.mem_trace_flag = SDK_RUN_NORMAL;
    ak_sdk_init(&config);

    char ac_cmd[ LEN_CMD ] ;
    if( parse_option( argc, argv ) == AK_FALSE ) {                              //解释和配置选项
        return 0;                                                               //打印帮助后退出
    }

#ifdef AK_RTOS
    snprintf( ac_cmd, sizeof(ac_cmd), "mkdir %s", dvr_conf_param.ac_path_save );
    system( ac_cmd );                                                           //创建录像保存目录
    snprintf( ac_cmd, sizeof(ac_cmd), "mkdir %s", dvr_conf_param.ac_path_tmp );
    system( ac_cmd );                                                           //创建录像暂存目录
#else
    snprintf( ac_cmd, sizeof(ac_cmd), "mkdir -p %s", dvr_conf_param.ac_path_save );
    system( ac_cmd );                                                           //创建录像保存目录
    snprintf( ac_cmd, sizeof(ac_cmd), "mkdir -p %s", dvr_conf_param.ac_path_tmp );
    system( ac_cmd );
#endif

    int ret = ak_dvr_init( &dvr_conf_param );
    if (ret) {
        ak_print_error_ex(MODULE_ID_APP, "ak_dvr_init failed\n");
        goto exit;
    }
    ak_dvr_start( &dvr_conf_param );
    for( ;; ) {
        sleep( 1 );
        if( gc_run != AK_TRUE ) {
            break;
        }
    }
    ak_dvr_stop( &dvr_conf_param );

exit:
    ak_dvr_exit( &dvr_conf_param );

    gc_run = AK_TRUE;

    /* exit */
    printf("----- %s exit -----\n", argv[0]);
    ak_sdk_exit();
    return 0;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(mux_sample, ak_mux_sample, mux sample);
#endif
