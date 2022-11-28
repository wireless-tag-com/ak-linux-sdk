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
#include <unistd.h>

#ifdef AK_RTOS
#include <kernel.h>
#define __STATIC__  static
#else
#define __STATIC__
#endif

#include "ak_common_graphics.h"
#include "ak_mem.h"
#include "ak_log.h"
#include "ak_vo.h"
#include "ak_tde.h"

#define LEN_HINT                512
#define LEN_OPTION_SHORT        512
#define MAX_DIS_NUM             4
#define VO_RES_GROUP            5
#define LOGO_WIDTH              600
#define LOGO_HEIGHT             165

static char *pc_prog_name = NULL;                                                      //demo名称
static char *data_file = "/mnt/yuv";
static char *logo_file = "/mnt/aklogo.600.165.rgb";
static int display_num = 4;
static AK_GP_FORMAT data_format = GP_FORMAT_YUV420P;
static AK_GP_FORMAT out_format  = GP_FORMAT_RGB888;
static int gi_dst_w = 0;
static int gi_dst_h = 0;
static int rotate = 0;
static int gi_src_w = 0;
static int gi_src_h = 0;

static char ac_option_hint[  ][ LEN_HINT ] = {                                         //操作提示数组
    "打印帮助信息" ,
    "屏幕宽度" ,
    "屏幕高度" ,
    "屏幕旋转角度 可选值:0,90,180,270" ,
    "文件原始宽度" ,
    "文件原始高度" ,
    "yuv文件路径" ,
    "分屏数目1-4" ,
    "[NUM] 数据输入格式 0:RGB565 1:RGB888 2:BGR565 3:BGR888 4:YUV420P 5:YUV420SP 6:ARGB8888 7:RGBA8888 8:ABGR8888 9:BGRA8888 10:TILED32X4" ,
    "[NUM] 数据输出格式 0:RGB565 1:RGB888 2:BGR565 3:BGR888" ,
    "logo 文件的路径，只支持提供的anyka.logo.rgb文件显示",
    "",
};

static struct option option_long[ ] = {
    { "help"              , no_argument       , NULL , 'h' } ,      //"打印帮助信息" ,
    { "screen-w"          , required_argument , NULL , 'w' } ,      //"屏幕宽度" ,
    { "screen-h"          , required_argument , NULL , 'e' } ,      //"屏幕高度" ,
    { "screen-r"          , required_argument , NULL , 'x' } ,      //"屏幕旋转角度 可选值:0,90,180,270" ,
    { "file-w"            , required_argument , NULL , 'r' } ,      //"文件原始宽度" ,
    { "file-h"            , required_argument , NULL , 's' } ,      //"文件原始高度" ,
    { "file-dir"          , required_argument , NULL , 'f' } ,      //"yuv文件路径" ,
    { "display-num"       , required_argument , NULL , 'n' } ,      //"分屏数目1-4" ,
    { "format-in"         , required_argument , NULL , 'i' } ,      //"[NUM] 数据输入格式 0:RGB565 1:RGB888 2:BGR565 3:BGR888 4:YUV420P 5:YUV420SP 6:ARGB8888 7:RGBA8888 8:ABGR8888 9:BGRA8888 10:TILED32X4" ,
    { "format-out"        , required_argument , NULL , 'o' } ,      //"[NUM] 数据输出格式 0:RGB565 1:RGB888 2:BGR565 3:BGR888" ,
    { "logo-file"         , required_argument , NULL , 'l' } ,      //"logo 文件的路径，只支持提供的anyka.logo.rgb文件显示",
    { 0                   , 0                 , 0    , 0   } ,      //"",
};

/*
 * help_hint: use the -h --help option.Print option of help information
 * return: 0
 */
static void usage(const char * name)
{
    printf("\nexample: \r\n"
            "\t%s -w 1024 -e 600  -x 0  -r 1920 -s 1080 -f /mnt/yuv -n 1 -i 4 -o 1 -l /mnt/aklogo.600.165.rgb\n"
            "\t%s -w 1280 -e 800  -x 90 -r 1920 -s 1080 -f /mnt/yuv -n 1 -i 4 -o 1 -l /mnt/aklogo.600.165.rgb\n"
            "\t%s -w 1920 -e 1200 -x 90 -r 1920 -s 1080 -f /mnt/yuv -n 1 -i 4 -o 1 -l /mnt/aklogo.600.165.rgb\n" ,
            name, name, name );
    printf("\tNOTE:the logo-dir should name aklogo.600.165.rgb, this file format is rgb888\n");
}

static int help_hint(void)
{
    int i;

    printf( "%s\n" , pc_prog_name);
    printf( "Version: \r\n\t%s \n\n", ak_vo_get_version());
    printf( "Usage: \r\n\t%s  [options] <value>\n\n", pc_prog_name);

    /* parse the all supported option */
    printf( "options: \r\n");
    for(i = 0; i < sizeof(option_long) / sizeof(struct option); i ++) {
        if( option_long[ i ].val != 0 )
            printf("\t-%c, --%-16s\t%s\n" , option_long[ i ].val , option_long[ i ].name , ac_option_hint[ i ]);
    }

    usage(pc_prog_name);
    printf("\n\n");
    return AK_SUCCESS;
}

/*
 * get_option_short: fill the stort option string.
 * return: option short string addr.
 */
static char *get_option_short( struct option *p_option, int i_num_option, char *pc_option_short, int i_len_option )
{
    int i;
    int i_offset = 0;
    char c_option;

    for( i = 0 ; i < i_num_option ; i ++ ) {
        if( ( c_option = p_option[ i ].val ) == 0 )
            continue;

        switch( p_option[ i ].has_arg ) {
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

static int parse_option( int argc, char **argv )
{
    int i_option;

    char ac_option_short[ LEN_OPTION_SHORT ];
    int i_array_num = sizeof( option_long ) / sizeof( struct option ) ;
    char c_flag = AK_TRUE;
    pc_prog_name = argv[ 0 ];

    optind = 0;
    get_option_short( option_long, i_array_num , ac_option_short , LEN_OPTION_SHORT );
    while((i_option = getopt_long(argc , argv , ac_option_short , option_long , NULL)) > 0) {
        switch(i_option) {
            case 'h' :                                                          //help
                help_hint();
                c_flag = AK_FALSE;
                goto parse_option_end;
            case 'w' :                                                          //screen type
                gi_dst_w = atoi( optarg );
                break;
            case 'e' :                                                          //screen type
                gi_dst_h = atoi( optarg );
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
            case 'f' :                                                          //data file path
                data_file = optarg;
                break;
            case 'n' :                                                          //display num
                display_num = atoi( optarg );
                break;
            case 'i' :                                                          //data_file format
                data_format = atoi(optarg);
                break;
            case 'o' :                                                          //out put format
                out_format = atoi(optarg);
                break;
            case 'l' :                                                          //logo file
                logo_file = optarg;
                break;
            case 'r' :                                                          //file-w
                gi_src_w = atoi(optarg);
                break;
            case 's' :                                                          //file-h
                gi_src_h = atoi(optarg);
                break;
            default :
                help_hint();
                c_flag = AK_FALSE;
                goto parse_option_end;

        }
    }
parse_option_end:

    return c_flag;
}

/* add the  obj to GUI layer */
static int add_logo_to_gui(void)
{
    /* get the logo file */
    FILE *fp_logo = fopen(logo_file, "rb");
    if (NULL == fp_logo) {
        ak_print_error_ex(MODULE_ID_APP, "logo file open failed !!! This file is %s, check it!\n", logo_file);
        return AK_FAILED;
    }
    /* get the file size */
    fseek(fp_logo, 0, SEEK_END);
    int file_size = ftell(fp_logo);
    fseek(fp_logo, 0, SEEK_SET);

    /* read data from teh file */
    void *logo_data = ak_mem_dma_alloc(MODULE_ID_APP, file_size);
    if(logo_data == NULL) {
        ak_print_error_ex(MODULE_ID_APP, "Can't malloc DMA memory!\n");
        return AK_FAILED;
    }
    fread(logo_data, 1, file_size, fp_logo);

    struct ak_vo_obj gui_obj;

    /* set obj src info*/
    gui_obj.format = GP_FORMAT_RGB888;
    gui_obj.cmd = GP_OPT_BLIT;
    gui_obj.vo_layer.width = LOGO_WIDTH;
    gui_obj.vo_layer.height = LOGO_HEIGHT;
    gui_obj.vo_layer.clip_pos.top = 0;
    gui_obj.vo_layer.clip_pos.left = 0;
    gui_obj.vo_layer.clip_pos.width = LOGO_WIDTH;
    gui_obj.vo_layer.clip_pos.height = LOGO_HEIGHT;

    ak_mem_dma_vaddr2paddr(logo_data, &(gui_obj.vo_layer.dma_addr));

    /* set dst_layer 1 info*/
    gui_obj.dst_layer.top = 0;
    gui_obj.dst_layer.left = 0;
    gui_obj.dst_layer.width = LOGO_WIDTH;
    gui_obj.dst_layer.height = LOGO_HEIGHT;

    /* display obj 1*/
    ak_vo_add_obj(&gui_obj, AK_VO_LAYER_GUI_1);

    /* free source */
    ak_mem_dma_free(logo_data);
    //posix_fadvise(fileno(fp_logo), 0, file_size, POSIX_FADV_DONTNEED);
    fclose(fp_logo);

    return AK_SUCCESS;
}

__STATIC__ int main(int argc, char **argv)
{
    /* start the application */
    sdk_run_config config;
    memset(&config, 0, sizeof(sdk_run_config));
    config.mem_trace_flag = SDK_RUN_NORMAL;
    ak_sdk_init( &config );

    /* parse the option */
    if( parse_option( argc, argv ) == AK_FALSE ) {            //解释和配置选项
        return AK_FAILED;                                           //打印帮助后退出
    }

    /* print the version */
    ak_print_normal(MODULE_ID_VI, "*****************************************\n");
    ak_print_normal(MODULE_ID_VI, "** vo version: %s **\n", ak_vo_get_version());
    ak_print_normal(MODULE_ID_VI, "*****************************************\n");

    /* check the num */
    if (display_num < 1 || display_num > MAX_DIS_NUM) {
        ak_print_error_ex(MODULE_ID_APP, "display num is only support 1 to %d\n", MAX_DIS_NUM);
        return AK_FAILED;
    }

    /* store the file path */
    char file_path[64]="";
    int ret = 0;
    /* get the res and rotation */
    /* fill the struct to open the vo */
    struct ak_vo_param    param;

    /* fill the struct ready to open */
    param.width = gi_dst_w;//1280;
    param.height = gi_dst_h;// 800;
    param.format = out_format;  //format to output
    param.rotate = rotate;    //rotate value
    //param.init_mode = AK_VO_INIT_WHIHE ;
    param.init_mode = AK_VO_INIT_BLACK ;
    //param.init_mode = AK_VO_INIT_NONE ;
    /* open vo */
    ret = ak_vo_open(&param, DEV_NUM);      //open vo
    //ak_sleep_ms( 1000 ) ;
    if(ret != 0) {
        /* open failed return -1 */
        ak_print_error_ex(MODULE_ID_APP, "ak_vo_open failed![%d]\n",ret);
        return AK_FAILED;
    }

    /* create the video layer */
    struct ak_vo_layer_in video_layer;
    video_layer.create_layer.height = gi_dst_h;   //800; //get the res
    video_layer.create_layer.width  = gi_dst_w;    //1280; //get the res
    /* just from (0,0) */
    video_layer.create_layer.left  = 0;         //layer left pos
    video_layer.create_layer.top   = 0;         // layer top pos
    video_layer.layer_opt          = 0;
    video_layer.format             = out_format;

    /* vo layer create api */
    ret = ak_vo_create_video_layer(&video_layer, AK_VO_LAYER_VIDEO_1);
    if(ret != 0) {
        /* if failed, close the vo */
        ak_print_error_ex(MODULE_ID_APP, "ak_vo_create_video_layer failed![%d]\n",ret);
        ak_vo_close(DEV_NUM);       // if failed, close the vo
        return AK_FAILED;
    }

    if ((gi_dst_w >= LOGO_WIDTH) && (gi_dst_h >= LOGO_HEIGHT)) {
        /* create the gui layer */
        struct ak_vo_layer_in gui_layer;
        /* only support the given log file */
        gui_layer.create_layer.height = LOGO_HEIGHT;    /* logo res */
        gui_layer.create_layer.width  = LOGO_WIDTH;    /* logo res */
        gui_layer.create_layer.left  = 0;       /* logo pos */
        gui_layer.create_layer.top   = 0;       /* logo pos */
        gui_layer.format             = out_format;  /* gui layer format */

        gui_layer.layer_opt          = GP_OPT_COLORKEY; /* support the colorkey opt */

        gui_layer.colorkey.coloract = COLOR_DELETE;       /* delete the color */
        if(out_format == GP_FORMAT_RGB565 || out_format == GP_FORMAT_BGR565) {
            gui_layer.colorkey.color_min = 0xF8FCF8;        /* min value */
            gui_layer.colorkey.color_max = 0xF8FCF8;        /* max value */
        }
        else {
            gui_layer.colorkey.color_min = 0xFFFF00;        /* min value */
            gui_layer.colorkey.color_max = 0xFFFFFF;        /* max value */
        }

        struct ak_vo_layer_out gui_info;                /* output the gui layer info */
        ret = ak_vo_create_gui_layer(&gui_layer, AK_VO_LAYER_GUI_1, &gui_info);
        if(ret != 0) {
            ak_print_error_ex(MODULE_ID_APP, "ak_vo_create_gui_layer failed![%d]\n",ret);
            ak_vo_destroy_layer(AK_VO_LAYER_VIDEO_1);
            ak_vo_close(DEV_NUM);
            return AK_FAILED;
        }
    }

    /* use the double buff mode */
    ak_vo_set_fbuffer_mode(AK_VO_BUFF_DOUBLE);

    /* support the yuv 420p */
    int len = 0;

    /* get the size of the pixel */
    switch (data_format) {
    /* 565 */
    case GP_FORMAT_RGB565:
    case GP_FORMAT_BGR565:
        len = gi_src_w*gi_src_h*2;
        break;
    /* 888 */
    case GP_FORMAT_BGR888:
    case GP_FORMAT_RGB888:
        len = gi_src_w*gi_src_h*3;
        break;
    /* 8888 */
    case GP_FORMAT_ABGR8888:
    case GP_FORMAT_ARGB8888:
    case GP_FORMAT_RGBA8888:
    case GP_FORMAT_BGRA8888:
        len = gi_src_w*gi_src_h*4;
        break;
    /* YUV */
    case GP_FORMAT_YUV420P:
    case GP_FORMAT_YUV420SP:
        len = (gi_src_w*gi_src_h*3)/2;
        break;
    default:
        ak_print_error_ex(MODULE_ID_APP, "here not support the tileyuv data\n");

    }

    /* need dma mem */
    void *data = ak_mem_dma_alloc(MODULE_ID_APP, len);
    if(data == NULL) {
        ak_print_error_ex(MODULE_ID_APP, "Can't malloc DMA memory!\n");
        ret = AK_FAILED;
        goto err;
    }

    /* 1-4 partion screen to show the picture */
    int dou = 1;
    if (display_num == 1)
        dou = 1;
    else
        dou = 2;

    if ((gi_dst_w >= LOGO_WIDTH) && (gi_dst_h >= LOGO_HEIGHT)) {
        /* add logo to gui layer */
        ret = add_logo_to_gui();
        if(ret != 0) {
            ak_print_error_ex(MODULE_ID_APP, "add_logo_to_gui failed![%d]\n",ret);
            goto err;
        }
    }

    int i_alpha = 0;

#ifdef AK_RTOS
    struct dirent *entry = NULL;
    DIR *dir = NULL;

    dir = opendir(data_file);//dir open
    if (dir == NULL)
    {
        ak_print_error_ex(MODULE_ID_APP, "can't open file dir[%s]\n", data_file);
        ret = AK_FAILED;
        goto err;
    }
    while ((entry = readdir(dir))) {
        if( ( strcmp( entry->d_name, "." ) == 0 ) ||
            ( strcmp( entry->d_name, ".." ) == 0 ) ||
            ( entry->d_type == DT_DIR ) )  {
            continue ;
        }
        snprintf(file_path, sizeof( file_path ) , "%s/%s",  data_file, entry->d_name);
#else
    int i_num_scandir, i ;
    struct dirent **pp_dirent_list = NULL;
    i_num_scandir = scandir( data_file, &pp_dirent_list, 0 , alphasort );       //使用排序读取方式
    for( i = 0; i < i_num_scandir; i ++ ) {
        if( ( strcmp( pp_dirent_list[ i ]->d_name, "." ) == 0 ) ||
            ( strcmp( pp_dirent_list[ i ]->d_name, ".." ) == 0 ) ||
            ( pp_dirent_list[ i ]->d_type == DT_DIR ) )  {
            continue ;
        }
        snprintf(file_path, sizeof( file_path ) , "%s/%s",  data_file, pp_dirent_list[ i ]->d_name);
#endif
        /* get data from file */
        FILE *fp = fopen(file_path, "rb");
        if(fp!= NULL) {
            int ret = 0;
            memset(data, 0, len);
            if ( ( ret = fread(data, 1, len, fp) ) <= 0 ) {     // read the file
                ak_print_error_ex(MODULE_ID_APP, "read [%d] byte to dma buffer\n",ret);
            }

            /* obj add */
            struct ak_vo_obj obj;

            /* set obj src info*/
            obj.format = data_format;
            obj.cmd = GP_OPT_SCALE;
            obj.vo_layer.width = gi_src_w;
            obj.vo_layer.height = gi_src_h;
            obj.vo_layer.clip_pos.top = 0;
            obj.vo_layer.clip_pos.left = 0;
            obj.vo_layer.clip_pos.width = gi_src_w;
            obj.vo_layer.clip_pos.height = gi_src_h;

            ak_mem_dma_vaddr2paddr(data, &(obj.vo_layer.dma_addr));

            /* show as the screen partion set */
            int counter = display_num;
            if (display_num == 1) {
                /* set dst_layer 1 info*/
                obj.dst_layer.top = 0;
                obj.dst_layer.left = 0;
                obj.dst_layer.width = gi_dst_w;
                obj.dst_layer.height = gi_dst_h;
                /* display obj 1*/
                ak_vo_add_obj(&obj, AK_VO_LAYER_VIDEO_1);
            }
            else {
                /* more than one partion */
                if (counter) {
                    /* set dst_layer 1 info*/
                    obj.dst_layer.top = 0;
                    obj.dst_layer.left = 0;
                    obj.dst_layer.width = gi_dst_w/dou;
                    obj.dst_layer.height = gi_dst_h/dou;
                    /* display obj 1*/
                    ak_vo_add_obj(&obj, AK_VO_LAYER_VIDEO_1);
                    counter--;
                }

                if (counter != 0) {
                    /* set dst_layer 2 info*/
                    obj.dst_layer.top = 0;
                    obj.dst_layer.left = gi_dst_w/dou;
                    obj.dst_layer.width = gi_dst_w/dou;
                    obj.dst_layer.height = gi_dst_h/dou;//400;
                    /* display obj 1*/
                    ak_vo_add_obj(&obj, AK_VO_LAYER_VIDEO_1);
                    counter--;
                }

                if (counter) {
                    /* set dst_layer 3 info*/
                    obj.dst_layer.top = gi_dst_h/dou;//400;
                    obj.dst_layer.left = 0;
                    obj.dst_layer.width = gi_dst_w/dou;
                    obj.dst_layer.height = gi_dst_h/dou;//400;
                    /* display obj 1*/
                    ak_vo_add_obj(&obj, AK_VO_LAYER_VIDEO_1);
                    counter--;
                }

                if (counter) {
                    /* set dst_layer 4 info*/
                    obj.dst_layer.top = gi_dst_h/dou;//400;
                    obj.dst_layer.left = gi_dst_w/dou;
                    obj.dst_layer.width = gi_dst_w/dou;
                    obj.dst_layer.height = gi_dst_h/dou;//400;
                    /* display obj 1*/
                    ak_vo_add_obj(&obj, AK_VO_LAYER_VIDEO_1);
                    counter--;
                }

            }
#if 0                                                                           //ak_vo_refresh_screen_ex中判断lcdc控制器,此处不需要判断
            /* check the lcdc status  */
            unsigned long status = AK_FALSE;
            ret = ak_vo_check_lcdc_busy(&status);
            if(ret == AK_SUCCESS) {
                if(status == AK_TRUE) {
                    ak_print_warning_ex(MODULE_ID_APP, "LCDC is busy now, should wait sometime before do the refresh screen\n");
                    ak_sleep_ms(5);
                }
            }
            else {
                ak_print_error_ex(MODULE_ID_APP, "check lcdc status failed!\n");
            }
#endif
            /* flush to screen */
#if 1
            int cmd = AK_VO_REFRESH_VIDEO_GROUP & 0x100;
            cmd |= AK_VO_REFRESH_GUI_GROUP & 0x10000;
            i_alpha = ( i_alpha + 1 ) % 17 ;
            if ((gi_dst_w >= LOGO_WIDTH) && (gi_dst_h >= LOGO_HEIGHT)) {
                ak_vo_set_layer_alpha( AK_VO_LAYER_GUI_1, i_alpha - 1 );
            }
            //ak_vo_refresh_screen_ex(cmd , AK_VO_REFRESH_MODE_WAIT_LCDC | AK_VO_REFRESH_MODE_LAYER_TMP );
            ak_vo_refresh_screen(cmd);

#else
            int cmd = AK_VO_REFRESH_GUI_GROUP & 0x10000;
            for( ;; ) {
                i_alpha = ( i_alpha + 1 ) % 17 ;
                ak_vo_set_layer_alpha( AK_VO_LAYER_GUI_1, i_alpha - 1 );
                for( int i = 0 ; i < 10; i ++ ) {
                    ak_vo_refresh_screen_ex(cmd , AK_VO_REFRESH_MODE_WAIT_LCDC | AK_VO_REFRESH_MODE_LAYER_TMP );
                }
            }
#endif
            //ak_vo_refresh_screen( cmd );


            /* finish one file show */
            //posix_fadvise(fileno(fp), 0, len, POSIX_FADV_DONTNEED);
            fclose(fp);
        }
        else {
            ak_print_error_ex(MODULE_ID_APP, "open file [%s] filed!\n", file_path);
        }
        ret = AK_SUCCESS;
        //ak_sleep_ms(200);
    }
#ifdef AK_RTOS
    if(dir != NULL) {
        closedir(dir);
    }
#endif

err:
    /* if err or finish, release the src */
    if(data)
        ak_mem_dma_free(data);

    /* destroy the layer, release the src */
    ak_vo_destroy_layer(AK_VO_LAYER_VIDEO_1);
    if ((gi_dst_w >= LOGO_WIDTH) && (gi_dst_h >= LOGO_HEIGHT)) {
        ak_vo_destroy_layer(AK_VO_LAYER_GUI_1);
    }

    /* close the vo */
    ak_vo_close(DEV_NUM);
    ak_sdk_exit();

    return ret;
}

#ifdef AK_RTOS
SHELL_CMD_EXPORT_ALIAS(main, ak_vo_sample, vo sample)
#endif

