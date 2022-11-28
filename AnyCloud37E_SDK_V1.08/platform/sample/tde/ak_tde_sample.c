/**
* Copyright (C) 2019 Anyka(Guangzhou) Microelectronics Technology CO.,LTD.
* File Name: ak_tde_demo.c
* Description: This is a simple example to show how the tde module working.
* Notes:
* History: V1.0.0
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#include "ak_tde_sample.h"
#include "ak_common.h"
#include "ak_mem.h"
#include "ak_common_graphics.h"
#include "ak_tde.h"
#include "ak_mem.h"
#include "ak_log.h"

#ifdef AK_RTOS
#include <kernel.h>
#define __STATIC__  static
#else
#define __STATIC__
#endif

static int fd_gui = 0;

static char *pc_prog_name = NULL;                                                      //demo名称
static char *pc_file_src = DEFAULT_FILE_SRC;                                           //tde_opt使用的测试图片
static char *pc_file_bg = DEFAULT_FILE_BG;                                             //tde_opt使用的背景图片

extern void *osal_fb_mmap_viraddr(int fb_len, int fb_fd);
extern int osal_fb_munmap_viraddr(void *addr, int fb_len);

static struct ak_tde_cmd tde_cmd_param;                                                //进行tde操作的结构体
static struct fb_fix_screeninfo fb_fix_screeninfo_param;                               //屏幕固定参数
static struct fb_var_screeninfo fb_var_screeninfo_param;                               //屏幕可配置参数
static char c_reset_screen = AK_FALSE;                                                 //是否重置屏幕参数
static void *p_vaddr_fb = NULL;                                                        //fb0的虚拟内存地址
static char ac_option_hint[  ][ LEN_HINT ] = {                                         //操作提示数组
    "打印帮助信息" ,
    "拷贝图层" ,
    "拉伸图层" ,
    "[空值|0-255] 重置屏幕参数如果设置了颜色值则在初始化的时候用该颜色进行初始化" ,
    "[HEX] 使用指定颜色进行矩形填充" ,
    "[0-3] 选择指定角度 (0:DEGREE0 1:DEGREE90 2:DEGREE180 3:DEGREE270)" ,
    "[0-15] 透明度设置" ,
    "\"[[HEX-COLOR-MIN] [HEX-COLOR-MAX] [0:DEL 1:KEEP]\" 颜色过滤设置" ,
    "[FILE] 源图文件   (DEFAULT: ak_tde_s_test.rgb)" ,
    "[FILE] 背景图文件 (DEFAULT: ak_tde_bg_test.rgb)" ,
    "\"[PIC-WIDTH] [PIC-HEIGHT] [RECT-TOP] [RECT-LEFT] [RECT-WIDTH] [RECT-HEIGHT]\" 源图坐标参数" ,
    "\"[PIC-WIDTH] [PIC-HEIGHT] [RECT-TOP] [RECT-LEFT] [RECT-WIDTH] [RECT-HEIGHT]\" 背景图坐标参数" ,
    "[NUM] 背景图层格式 \n\t\t\t\t\t0:RGB565 1:RGB888 2:BGR565 3:BGR888 4:YUV420P 5:YUV420SP\n\t\t\t\t\t6:ARGB8888 7:RGBA8888 8:ABGR8888 9:BGRA8888 10:TILED32X4" ,
    "[NUM] 目标图层格式 0:RGB565 1:RGB888 2:BGR565 3:BGR888 4:YUV420P 5:YUV420SP" ,
    "[NUM] 源图图层格式\n\t\t\t\t\t0:RGB565 1:RGB888 2:BGR565 3:BGR888 4:YUV420P 5:YUV420SP\n\t\t\t\t\t6:ARGB8888 7:RGBA8888 8:ABGR8888 9:BGRA8888 10:TILED32X4" ,
    "\"[RECT-TOP] [RECT-LEFT] [RECT-WIDTH] [RECT-HEIGHT]\" 源图贴图的坐标参数" ,
    "" ,
};

static double af_format_byte[  ] = { 2 , 3 , 2 , 3 , 1.5 , 1.5 , 4 , 4 , 4 , 4 , 0 };  //图形格式占用的字节数
static struct ak_tde_layer tde_layer_src = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , GP_FORMAT_RGB888 } ;    //源坐标
static struct ak_tde_layer tde_layer_tgt = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , GP_FORMAT_RGB888 } ;    //目标坐标
static struct ak_tde_layer tde_layer_bg = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , GP_FORMAT_RGB888 } ;     //背景图坐标
static struct ak_tde_layer tde_layer_screen = { 0 , 0 , 0 , 0 , 0 , 0 , 0 , GP_FORMAT_RGB888 } ; //屏幕坐标
static unsigned char c_color_reset = 0xff ;                                     //重置屏幕的颜色

static struct option option_long[ ] = {
    { "help"              , no_argument       , NULL , 'h' } ,                  //"打印帮助信息" ,
    { "opt-blit"          , no_argument       , NULL , 'A' } ,                  //"拷贝图层" ,
    { "opt-scale"         , no_argument       , NULL , 'B' } ,                  //"拉伸图层" ,
    { "reset-screen"      , optional_argument , NULL , 'C' } ,                  //"[NULL]|[0-255] 重置屏幕参数如果设置了颜色值则在初始化的时候用该颜色进行初始化" ,
    { "opt-fillrect"      , required_argument , NULL , 'a' } ,                  //"[HEX] 使用指定颜色进行矩形填充" ,
    { "opt-rotate"        , required_argument , NULL , 'b' } ,                  //"[0-3] 选择指定角度 (0:DEGREE0 1:DEGREE90 2:DEGREE180 3:DEGREE270)" ,
    { "opt-transparent"   , required_argument , NULL , 'c' } ,                  //"[0-15] 透明度设置" ,
    { "opt-colorkey"      , required_argument , NULL , 'd' } ,                  //"\"[[HEX-COLOR-MIN] [HEX-COLOR-MAX] [0:DEL 1:KEEP]\" 颜色过滤设置" ,
    { "file-s"            , required_argument , NULL , 'e' } ,                  //"[FILE] 源图文件   (DEFAULT: ak_tde_s_test.rgb)" ,
    { "file-bg"           , required_argument , NULL , 'f' } ,                  //"[FILE] 背景图文件 (DEFAULT: ak_tde_bg_test.rgb)" ,
    { "rect-s"            , required_argument , NULL , 'g' } ,                  //"\"[PIC-WIDTH] [PIC-HEIGHT] [RECT-TOP] [RECT-LEFT] [RECT-WIDTH] [RECT-HEIGHT]\" 源图坐标参数" ,
    { "rect-bg"           , required_argument , NULL , 'i' } ,                  //"\"[PIC-WIDTH] [PIC-HEIGHT] [RECT-TOP] [RECT-LEFT] [RECT-WIDTH] [RECT-HEIGHT]\" 背景图坐标参数" ,
    { "format-bg"         , required_argument , NULL , 'j' } ,                  //"[NUM] 背景图层格式 0:RGB565 1:RGB888 2:BGR565 3:BGR888 4:YUV420P 5:YUV420SP 6:ARGB8888 7:RGBA8888 8:ABGR8888 9:BGRA8888 10:TILED32X4" ,
    { "format-out"        , required_argument , NULL , 'k' } ,                  //"[NUM] 目标图层格式 0:RGB565 1:RGB888 2:BGR565 3:BGR888 4:YUV420P 5:YUV420SP" ,
    { "format-s"          , required_argument , NULL , 'l' } ,                  //"[NUM] 源图图层格式 0:RGB565 1:RGB888 2:BGR565 3:BGR888 4:YUV420P 5:YUV420SP 6:ARGB8888 7:RGBA8888 8:ABGR8888 9:BGRA8888 10:TILED32X4" ,
    { "rect-t"            , required_argument , NULL , 'm' } ,                  //"\"[RECT-TOP] [RECT-LEFT] [RECT-WIDTH] [RECT-HEIGHT]\" 源图贴图的坐标参数" ,
    { 0                   , 0                 , 0    ,  0  } ,                  //"" ,
 };

/*
    help_hint: 根据option_long和ac_option_hint数组打印帮助信息
    return: 0
*/
static int help_hint(void)
{
    int i;

    printf( "%s\n" , pc_prog_name);
    printf( "Version: \r\n\t%s \n\n", ak_tde_get_version());
    printf( "Usage: \r\n\t%s  [options] <value>\n\n", pc_prog_name);
    printf( "options: \r\n");
    for(i = 0; i < sizeof(option_long) / sizeof(struct option); i ++) {
        if( option_long[ i ].val != 0 ) {
            printf("\t-%c, --%-16s\t%s\n" , option_long[ i ].val , option_long[ i ].name , ac_option_hint[ i ]);
        }
    }
    printf("\nexample: \r\n\tak_tde_sample --opt-rotate 2 --file-s /mnt/anyka.logo.160.577.rgb --file-bg /mnt/desert.1024.600.rgb --rect-s 160,577,0,0,160,577 --rect-bg 1024,600,0,0,1024,600 --rect-t 0,0,0,0 --format-bg 1 \n" );
    printf("\n\n");
    return 0;
}

#if 0
/*
    init_regloop: 初始化一个正则表达式的循环读取结构体
    @p_regloop[IN]:循环结构体
    @pc_pattern[IN]:正则表达式
    @i_flags[IN]:标志
    return: 成功:AK_SUCCESS 失败:AK_FAILED
*/
int init_regloop( struct regloop *p_regloop , char *pc_pattern , int i_flags )
{
    int i_status = 0;

    memset( p_regloop , 0 , sizeof( struct regloop ) );

    if ( ( pc_pattern != NULL ) &&
         ( ( i_status = regcomp( &p_regloop->regex_t_use , pc_pattern , i_flags ) ) == 0 ) ) {
        p_regloop->pc_pattern = pc_pattern;
    }
    else {
        return AK_FALSE;
    }
    return AK_TRUE;
}

/*
    free_regloop: 释放一个正则表达式的循环读取结构体的正则指针
    @p_regloop[IN]:循环结构体
    return: 0
*/
int free_regloop( struct regloop *p_regloop )
{
    if( p_regloop->pc_pattern != NULL ) {
        regfree( &p_regloop->regex_t_use );
    }
    return 0;
}

/*
    match_regloop: 重置一个正则循环结构体
    @p_regloop[IN]:循环结构体
    @pc_buff[IN]:输入的字符串
    @pc_res[OUT]:每次获取的正则匹配结果
    @i_len_res[IN]:匹配结果指针的缓冲区长度
    return: 正则匹配结果长度
*/
int match_regloop( struct regloop *p_regloop , char *pc_buff , char *pc_res , int i_len_res )
{
    int i_len_match = 0, i_len_cpy = 0;
    regmatch_t regmatch_t_res ;
    char *pc_now = pc_buff + p_regloop->i_offset ;

    pc_res[ 0 ] = 0;
    if ( regexec( &p_regloop->regex_t_use , pc_now , 1 , &regmatch_t_res , 0 ) == 0 ) {
        if ( ( i_len_match = regmatch_t_res.rm_eo - regmatch_t_res.rm_so ) <= 0 ) {                 //匹配到的字符串长度为0
            return 0;
        }

        if ( i_len_res > i_len_match ) {
            i_len_cpy = i_len_match;
        }
        else {
            i_len_cpy = i_len_res - 1;
        }
        memcpy( pc_res , pc_now + regmatch_t_res.rm_so , i_len_cpy ) ;
        pc_res[ i_len_cpy ] = 0 ;
        p_regloop->i_offset += regmatch_t_res.rm_eo;
    }
    return i_len_cpy ;
}
#else

/*
    reset_regloop: 重置一个正则循环结构体
    @p_regloop[IN]:循环结构体
    return: 0
*/
int reset_regloop( struct regloop *p_regloop )
{
    if( p_regloop != NULL ) {
        memset( p_regloop , 0 , sizeof( struct regloop ) );
        p_regloop->i_times = -1;
    }

    return 0;
}


int match_regloop( struct regloop *p_regloop , char *pc_buff , char *pc_res , int i_len_res )
{
    int i_len_match = 0, i_len_cpy = 0;
    int i = 0,blunk_cnt = -1;
    int str_len = -1;
    char *pc_now = pc_buff + p_regloop->i_offset ;

    pc_res[ 0 ] = 0;
    str_len = strlen(pc_now);
    //rt_kprintf(" %s: pc_now = [%s], i_offset = %d \n ", __FUNCTION__ ,pc_now, p_regloop->i_offset);

    for(i = 0; i <= str_len;i++)
    {

        if(pc_now[i] != ' ' && blunk_cnt == -1)
        {
            blunk_cnt = i;
        }
        else if(pc_now[i] == ',' || pc_now[i] == 0 || pc_now[i] == ' ')
        {
            if(pc_now[i] == 0)
                i_len_match = i - blunk_cnt;
            else
                i_len_match = i - blunk_cnt;

            if ( i_len_res > i_len_match ) {
            i_len_cpy = i_len_match;
            }
            else {
                i_len_cpy = i_len_res - 1;
            }

            memcpy( pc_res , pc_now + blunk_cnt, i_len_cpy ) ;
            pc_res[ i_len_cpy ] = 0 ;

            p_regloop->i_times++;
            p_regloop->i_offset++;

            //rt_kprintf(" %s: pc_res[%d] = [%s] [%d %d]\n ", __FUNCTION__, p_regloop->i_times, pc_res ,blunk_cnt,i_len_cpy);
            break;
        }

        p_regloop->i_offset++;
    }

    return i_len_cpy ;
}

#endif

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

#if 0
static int scale_para_check(struct ak_tde_layer *src, struct ak_tde_layer *dst)
{
    if(src->pos_width/dst->pos_width >= 10 || dst->pos_width/src->pos_width >= 10)
    {
        ak_print_error(MODULE_ID_APP, " para err, scale width too large: %d -> %d \n", src->pos_width,dst->pos_width);
        return AK_FALSE;
    }

    if(src->pos_height/dst->pos_height >= 10 || dst->pos_height/src->pos_height >= 10)
    {
        ak_print_error(MODULE_ID_APP, " para err, scale height too large: %d -> %d \n", src->pos_height,dst->pos_height);
        return AK_FALSE;
    }

    return AK_TRUE;
}
#endif

/*
    parse_option: 对传入的选项进行解释
    @argc[IN]: 从程序入口传入的选项数量
    @argv[IN]: 选项内容的字符串指针数组
    return: AK_TRUE:继续运行  AK_FALSE:退出应用,在打印帮助或者选项解释错误的时候使用
*/
static int parse_option( int argc, char **argv )
{
    int i_option;
    struct regloop regloop_num = {NULL, 0, 0,0};
    char ac_data[ LEN_DATA ];
    char ac_option_short[ LEN_OPTION_SHORT ];
    int i_array_num = sizeof( option_long ) / sizeof( struct option ) ;
    char c_flag = AK_TRUE;
    pc_prog_name = argv[ 0 ];

#ifdef AK_RTOS
    optind = 0;
#endif
    memset( &tde_cmd_param , 0 , sizeof( struct ak_tde_cmd ) );

    //init_regloop( &regloop_num, "[0-9A-Fa-f]+", REG_EXTENDED | REG_NEWLINE );   //初始化一个匹配16进制数字正则表达式
    get_option_short( option_long, i_array_num , ac_option_short , LEN_OPTION_SHORT );
    while((i_option = getopt_long(argc , argv , ac_option_short , option_long , NULL)) > 0) {
        switch(i_option) {
            case 'h' :                                                          //help
                help_hint();
                c_flag = AK_FALSE;
                goto parse_option_end;
            case 'A' :                                                          //opt-blit 图层拷贝操作
                tde_cmd_param.opt = tde_cmd_param.opt | GP_OPT_BLIT ;
                break;
            case 'B' :                                                          //opt-scale 拉伸操作
                tde_cmd_param.opt = tde_cmd_param.opt | GP_OPT_SCALE ;
                break;
            case 'C' :                                                          //reset-screen 重置屏幕参数
                c_reset_screen = AK_TRUE;
                if( optarg != NULL ) {                                          //设置memset屏幕初始化的颜色
                    c_color_reset = atoi( optarg ) % 0x100 ;
                }
                break;
            case 'a' :                                                          //opt-fillrect 画矩形操作
                tde_cmd_param.opt = tde_cmd_param.opt | GP_OPT_FILLRECT;
                sscanf( optarg , "%x" , &tde_cmd_param.color_rect );
                break;
            case 'b' :                                                          //opt-rotate 旋转操作
                tde_cmd_param.opt = tde_cmd_param.opt | GP_OPT_ROTATE ;
                tde_cmd_param.rotate_param = atoi(optarg);
                break;
            case 'c' :                                                          //opt-transparent 透明度操作
                tde_cmd_param.opt = tde_cmd_param.opt | GP_OPT_TRANSPARENT ;
                tde_cmd_param.alpha = atoi(optarg);
                break;
            case 'd' :                                                          //opt-colorkey 颜色过滤(colorkey)操作
                tde_cmd_param.opt = tde_cmd_param.opt | GP_OPT_COLORKEY ;
                reset_regloop( &regloop_num );
                //rt_kprintf(" <%s>, %d \n", optarg, __LINE__);
                while( match_regloop( &regloop_num, optarg, ac_data, LEN_DATA ) > 0 )
                {         //寻找匹配数字
                    switch( regloop_num.i_times ) {
                        case 0:
                            sscanf( ac_data , "%x" , &tde_cmd_param.colorkey_param.color_min );     //颜色值min
                            break;
                        case 1:
                            sscanf( ac_data , "%x" , &tde_cmd_param.colorkey_param.color_max );     //颜色值max
                            break;
                        case 2:
                            tde_cmd_param.colorkey_param.coloract = atoi( ac_data ) ;     //对范围内颜色是保留还是删除
                            break;
                    }
                }
                break;
            case 'e' :                                                          //file-s 源图文件
                pc_file_src = optarg;
                break;
            case 'f' :                                                          //file-bg 源图文件
                pc_file_bg = optarg;
                break;
            case 'g' :                                                          //rect-s 源图层宽高坐标信息
                reset_regloop( &regloop_num );

                //rt_kprintf(" <%s> , %d \n", optarg, __LINE__);
                while( match_regloop( &regloop_num, optarg, ac_data, LEN_DATA ) > 0 )
                {         //寻找匹配数字
                    switch( regloop_num.i_times ) {
                        case 0:
                            tde_layer_src.width = atoi( ac_data ) ;             //图层宽度
                            break;
                        case 1:
                            tde_layer_src.height = atoi( ac_data ) ;            //图层高度
                            break;
                        case 2:
                            tde_layer_src.pos_left = atoi( ac_data ) ;          //选中区域坐标X值
                            break;
                        case 3:
                            tde_layer_src.pos_top = atoi( ac_data ) ;           //选中区域坐标Y值
                            break;
                        case 4:
                            tde_layer_src.pos_width = atoi( ac_data ) ;         //选中区域宽度
                            break;
                        case 5:
                            tde_layer_src.pos_height = atoi( ac_data ) ;        //选中区域高度
                            break;
                    }
                }
                break;
            case 'i' :                                                          //rect-bg 背景图层参数设置
                reset_regloop( &regloop_num );

                //rt_kprintf(" <%s>, %d \n", optarg, __LINE__);
                while( match_regloop( &regloop_num, optarg, ac_data, LEN_DATA ) > 0 )
                {    //寻找匹配数字
                    switch( regloop_num.i_times ) {
                        case 0:
                            tde_layer_bg.width = atoi( ac_data ) ;              //图层宽度
                            break;
                        case 1:
                            tde_layer_bg.height = atoi( ac_data ) ;             //图层高度
                            break;
                        case 2:
                            tde_layer_bg.pos_left = atoi( ac_data ) ;          //选中区域坐标X值
                            break;
                        case 3:
                            tde_layer_bg.pos_top = atoi( ac_data ) ;           //选中区域坐标Y值
                            break;
                        case 4:
                            tde_layer_bg.pos_width = atoi( ac_data ) ;         //选中区域宽度
                            break;
                        case 5:
                            tde_layer_bg.pos_height = atoi( ac_data ) ;        //选中区域高度
                            break;
                    }
                }
                break;
            case 'j' :
                tde_layer_bg.format_param = atoi(optarg);
                break;
            case 'k' :
                tde_layer_screen.format_param = atoi(optarg);
                break;
            case 'l' :
                tde_layer_src.format_param = atoi(optarg);
                break;
            case 'm' :                                                          //rect-t 源图贴图的坐标信息
                reset_regloop( &regloop_num );

                //rt_kprintf(" ***   %s , %d \n", optarg, __LINE__);
                while( match_regloop( &regloop_num, optarg, ac_data, LEN_DATA ) > 0 )
                {    //寻找匹配数字
                    switch( regloop_num.i_times ) {
                        case 0:
                            tde_layer_tgt.pos_left = atoi( ac_data ) ;          //选中区域坐标X值
                            break;
                        case 1:
                            tde_layer_tgt.pos_top = atoi( ac_data ) ;           //选中区域坐标Y值
                            break;
                        case 2:
                            tde_layer_tgt.pos_width = atoi( ac_data ) ;         //选中区域宽度
                            break;
                        case 3:
                            tde_layer_tgt.pos_height = atoi( ac_data ) ;        //选中区域高度
                            break;
                    }
                }
                break;
            default :
                help_hint();
                c_flag = AK_FALSE;
                goto parse_option_end;
        }
    }

#if 0                                                                           //此处有bug,因为检测图层的时候tde_layer_screen的宽高还未赋值,会出现float异常
    if(tde_cmd_param.opt & GP_OPT_SCALE) {
        c_flag = scale_para_check(&tde_layer_src,&tde_layer_tgt) && scale_para_check(&tde_layer_bg,&tde_layer_screen);
    }
#endif

parse_option_end:
    //free_regloop( &regloop_num );                                               //释放正则表达式结构的分配资源
    return c_flag;
}

/*
    ak_gui_open: 打开lcd屏幕设备
    @p_fb_fix_screeninfo[OUT]:返回屏幕的只读信息
    @p_fb_var_screeninfo[OUT]:返回屏幕的可配置信息
    @pv_addr[OUT]:显存的地址
    return: 成功:AK_SUCCESS 失败:AK_FAILED
*/
static int ak_gui_open( struct fb_fix_screeninfo *p_fb_fix_screeninfo, struct fb_var_screeninfo *p_fb_var_screeninfo, void **pv_addr )
{
    int i_screen_size = 0 ;

    if ( fd_gui > 0 ) {
        return AK_SUCCESS;
    }
    else if ( ( fd_gui = open( DEV_GUI, O_RDWR ) ) > 0 ) {

        if (ioctl(fd_gui, FBIOGET_FSCREENINFO, p_fb_fix_screeninfo) < 0) {
            close( fd_gui );
            return AK_FAILED;
        }
        if (ioctl(fd_gui, FBIOGET_VSCREENINFO, p_fb_var_screeninfo) < 0) {
            close( fd_gui );
            return AK_FAILED;
        }

        if ( DUAL_FB_FIX == AK_TRUE )  {                                        //如果使用双buffer的话则调用ioctl
            i_screen_size = p_fb_fix_screeninfo->smem_len / 2 ;
        }
        else {
            i_screen_size = p_fb_fix_screeninfo->smem_len ;
        }
        if( p_fb_var_screeninfo->xres_virtual * p_fb_var_screeninfo->yres_virtual *
            af_format_byte[ tde_layer_screen.format_param ] > i_screen_size  ) {
            ak_print_error_ex( MODULE_ID_APP, "SCREEN FORMAT ERROR. Assign pixels sizes(%0.0f) is more than screen pixels sizes(%d)\n",
                               p_fb_var_screeninfo->xres_virtual * p_fb_var_screeninfo->yres_virtual *
                               af_format_byte[ tde_layer_screen.format_param ], i_screen_size );
            close( fd_gui );
            return AK_FAILED;
        }
        *pv_addr = osal_fb_mmap_viraddr(p_fb_fix_screeninfo->smem_len, fd_gui);//( void * )mmap( 0 , p_fb_fix_screeninfo->smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fd_gui, 0 );

        if( c_reset_screen == AK_TRUE ) {
            p_fb_var_screeninfo->activate |= FB_ACTIVATE_FORCE;
            p_fb_var_screeninfo->activate |= FB_ACTIVATE_NOW;
            p_fb_var_screeninfo->xres = p_fb_var_screeninfo->xres_virtual;
            p_fb_var_screeninfo->yres = p_fb_var_screeninfo->yres_virtual;
            switch ( tde_layer_screen.format_param ) {
                case GP_FORMAT_RGB565 :
                case GP_FORMAT_BGR565 :
                    p_fb_var_screeninfo->bits_per_pixel = BITS_PER_PIXEL565;
                    p_fb_var_screeninfo->red.offset = OFFSET565_RED;
                    p_fb_var_screeninfo->red.length = 5 ;
                    p_fb_var_screeninfo->green.offset = OFFSET565_GREEN;
                    p_fb_var_screeninfo->green.length = 6 ;
                    p_fb_var_screeninfo->blue.offset = OFFSET565_BLUE;
                    p_fb_var_screeninfo->blue.length = 5 ;
                    break;
                case GP_FORMAT_RGB888 :
                case GP_FORMAT_BGR888 :
                    p_fb_var_screeninfo->bits_per_pixel = BITS_PER_PIXEL;
                    p_fb_var_screeninfo->red.offset = OFFSET_RED;
                    p_fb_var_screeninfo->red.length = LEN_COLOR ;
                    p_fb_var_screeninfo->green.offset = OFFSET_GREEN;
                    p_fb_var_screeninfo->green.length = LEN_COLOR ;
                    p_fb_var_screeninfo->blue.offset = OFFSET_BLUE;
                    p_fb_var_screeninfo->blue.length = LEN_COLOR ;
                    break;
                default :
                    close( fd_gui );
                    return AK_FAILED;
            }
            if (ioctl(fd_gui, FBIOPUT_VSCREENINFO, p_fb_var_screeninfo) < 0) {
                close( fd_gui );
                return AK_FAILED;
            }
            memset( *pv_addr, 0 , p_fb_fix_screeninfo->smem_len );
        }

        tde_layer_screen.width      = fb_var_screeninfo_param.xres;
        tde_layer_screen.height     = fb_var_screeninfo_param.yres;
        tde_layer_screen.pos_left   = 0;
        tde_layer_screen.pos_top    = 0;
        tde_layer_screen.pos_width  = fb_var_screeninfo_param.xres;
        tde_layer_screen.pos_height = fb_var_screeninfo_param.yres;
        tde_layer_screen.phyaddr    = fb_fix_screeninfo_param.smem_start;
        return AK_SUCCESS;
    }
    else {
        return AK_FAILED;
    }
}

/*
    ak_gui_close: 关闭lcd屏幕设备
    return: 成功:AK_SUCCESS 失败:AK_FAILED
*/
static int ak_gui_close( void )
{
    if ( fd_gui > 0 ){
        if( p_vaddr_fb != NULL ) {
            osal_fb_munmap_viraddr( p_vaddr_fb, fb_fix_screeninfo_param.smem_len );//munmap( p_vaddr_fb, fb_fix_screeninfo_param.smem_len );
        }
        if( close( fd_gui ) == 0 ) {
            fd_gui = 0 ;
            return AK_SUCCESS;
        }
        else {

            return AK_FAILED;
        }
    }
    else {
        return AK_SUCCESS;
    }
}

/*
    get_file_size: 获取文件长度
    return: 成功:AK_SUCCESS 失败:AK_FAILED
*/
static long long int get_file_size( char *pc_filename )
{
    struct stat stat_buf;
    if( stat( pc_filename , &stat_buf ) < 0 ){
        return AK_FAILED ;
    }
    return ( unsigned int )stat_buf.st_size ;
}

__STATIC__ int main( int argc, char **argv )
{
    sdk_run_config config= {0};

    config.mem_trace_flag = SDK_RUN_NORMAL;
    ak_sdk_init( &config );

    if( parse_option( argc, argv ) == AK_FALSE ) {                              //解释和配置选项
        return 0;                                                               //打印帮助后退出
    }

    ak_print_normal(MODULE_ID_APP, "*****************************************\n");
	ak_print_normal(MODULE_ID_APP, "** tde demo version: %s **\n", ak_tde_get_version());
    ak_print_normal(MODULE_ID_APP, "*****************************************\n");

    if( ak_tde_open( ) != ERROR_TYPE_NO_ERROR ) {
        return 0;
    }
    if( ak_gui_open( &fb_fix_screeninfo_param, &fb_var_screeninfo_param, &p_vaddr_fb) != AK_SUCCESS ) {
        ak_print_error_ex(MODULE_ID_APP, "fb open error.\n" );
        return 0;
    }

    test_tde( );                                                                //tde图形测试
#ifdef AK_RTOS
    ak_sleep_ms(3000);
#endif
    ak_gui_close( );
    ak_tde_close( );
    ak_sdk_exit( );
    return 0;
}

#if 0
static int dump_layer_para(const char * name, struct ak_tde_layer* layer)
{
    printf( "%d %d %d %d %d %d %d 0x%x\n",
            layer->width, layer->height,layer->pos_left, layer->pos_top,layer->pos_width, layer->pos_height,
            layer->format_param, layer->phyaddr);
    return 0;
}
#endif

int test_tde( void )                                                            //tde图形操作测试
{
    long long i_filesize_src = 0, i_filesize_bg = 0;
    FILE *pFILE;
    void *p_vaddr_src= NULL, *p_vaddr_bg= NULL;
    int i_dmasize_src = tde_layer_src.width * tde_layer_src.height * af_format_byte[ tde_layer_src.format_param ];      //源图片的设定大小
    int i_dmasize_bg  = tde_layer_bg.width * tde_layer_bg.height * af_format_byte[ tde_layer_bg.format_param ];         //背景图片的设定大小
    int i_total_offset = tde_layer_screen.width * tde_layer_screen.height * af_format_byte[ tde_layer_screen.format_param ] ;

    if ( ( ( tde_cmd_param.opt & GP_OPT_FILLRECT ) == 0 ) &&
         ( ( i_dmasize_src == 0 ) || ( i_dmasize_bg == 0 ) ) ) {
        ak_print_error_ex(MODULE_ID_APP, "FORMAT ERROR. i_dmasize_src= %d i_dmasize_bg= %d\n", i_dmasize_src, i_dmasize_bg );
        goto test_tde_end;
    }

    if ( DUAL_FB_FIX == AK_TRUE )  {                                            //如果使用双buffer的话，将buffer设置为使用另外一个buffer的偏移值
        DUAL_FB_VAR ^= 1;
        tde_layer_screen.phyaddr = fb_fix_screeninfo_param.smem_start + DUAL_FB_VAR * i_total_offset ;
    }
    else {
        tde_layer_screen.phyaddr = fb_fix_screeninfo_param.smem_start ;
    }

    i_filesize_src = get_file_size( pc_file_src );                              //获取文件长度
    i_filesize_bg = get_file_size( pc_file_bg );
    if ( ( ( tde_cmd_param.opt & GP_OPT_FILLRECT ) == 0 ) &&
         ( ( i_filesize_src <= 0 ) || ( i_filesize_src != i_dmasize_src ) ||
           ( i_filesize_bg <= 0 ) || ( i_filesize_bg != i_dmasize_bg ) ) ) {
        ak_print_error( MODULE_ID_APP, "FILE SIZE NOT FIT FORMAT. i_filesize_src= %lld i_dmasize_src= %d i_filesize_bg= %lld i_dmasize_bg= %d\n",
                        i_filesize_src, i_dmasize_src, i_filesize_bg, i_dmasize_bg );
        goto test_tde_end;
    }

    if( i_filesize_src > 0 ) {
        p_vaddr_src = ak_mem_dma_alloc( 1, i_filesize_src );                         //分配pmem内存
        ak_mem_dma_vaddr2paddr( p_vaddr_src , ( unsigned long * )&tde_layer_src.phyaddr );    //获取源图片dma物理地址
    }
    if( i_filesize_bg > 0 ) {
        p_vaddr_bg = ak_mem_dma_alloc( 1, i_filesize_bg );
        ak_mem_dma_vaddr2paddr( p_vaddr_bg , ( unsigned long * )&tde_layer_bg.phyaddr );      //获取背景图片dma物理地址
    }

    if( i_filesize_src > 0 ) {                                                  //源图片
        pFILE = fopen( pc_file_src , "rb" );                                    //将图片内容读入pmem
        fseek(pFILE, 0, SEEK_SET);
        fread( ( char * )p_vaddr_src, 1, i_filesize_src, pFILE);
        fclose( pFILE );
    }
    if( i_filesize_bg > 0 ) {                                                   //背景图片
        pFILE = fopen( pc_file_bg , "rb" );                                     //将图片内容读入pmem
        fseek( pFILE, 0, SEEK_SET ) ;
        fread( ( char * )p_vaddr_bg, 1, i_filesize_bg, pFILE);
        fclose( pFILE );
        ak_tde_opt_scale( &tde_layer_bg, &tde_layer_screen );                   //贴背景图片
    }
    else {                                                                      //没有背景图片则使用白色背景
        memset( p_vaddr_fb , c_color_reset , fb_fix_screeninfo_param.smem_len );
    }

    tde_layer_tgt.width = fb_var_screeninfo_param.xres;                         //屏幕宽
    tde_layer_tgt.height = fb_var_screeninfo_param.yres;                        //屏幕高
    tde_layer_tgt.phyaddr = tde_layer_screen.phyaddr;                           //屏幕的fb物理地址直接赋值
    tde_layer_tgt.format_param = tde_layer_screen.format_param;
    tde_cmd_param.tde_layer_src = tde_layer_src;
    tde_cmd_param.tde_layer_dst = tde_layer_tgt;

    //dump_layer_para("src", &tde_layer_src);
    //dump_layer_para("tgt", &tde_layer_tgt);
    ak_tde_opt( &tde_cmd_param );                                               //贴源图片
    if ( DUAL_FB_FIX == AK_TRUE )  {                                            //如果使用双buffer的话则调用ioctl
        ioctl( fd_gui, FBIOPUT_VSCREENINFO, &fb_var_screeninfo_param ) ;
    }
test_tde_end:
    if( p_vaddr_src != NULL ) {
        ak_mem_dma_free( p_vaddr_src );
    }
    if( p_vaddr_bg != NULL ) {
        ak_mem_dma_free( p_vaddr_bg );
    }
    return 0;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(main, ak_tde_sample, TDE Sample Program);
#endif

