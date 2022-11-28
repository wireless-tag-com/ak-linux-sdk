#ifndef _AK_VO_H
#define _AK_VO_H

#include "ak_common.h"
#include "ak_common_graphics.h"

#define DEV_NUM     0

/* vo module errno list */
enum ak_vo_errno {                                                            //错误代码
    ERROR_VO_INVALID_DEV_ID            = ( MODULE_ID_VO << 24 ) + 0,          //0 无效句柄ID
    ERROR_VO_OPENED_ALREADY            = ( MODULE_ID_VO << 24 ) + 1,          //1 vo已经打开
    ERROR_VO_OPENED_FB_DEV_FAILED      = ( MODULE_ID_VO << 24 ) + 2,          //2 fb设备打开失败
    ERROR_VO_FBGET_FSCREENINFO_FAILED  = ( MODULE_ID_VO << 24 ) + 3,          //3 fb获取固定参数失败
    ERROR_VO_FBGET_VSCREENINFO_FAILED  = ( MODULE_ID_VO << 24 ) + 4,          //4 fb获取可设参数失败
    ERROR_VO_FBPUT_VSCREENINFO_FAILED  = ( MODULE_ID_VO << 24 ) + 5,          //5 fb设置参数失败
    ERROR_VO_MEMORY_INIT_FAILED        = ( MODULE_ID_VO << 24 ) + 6,          //6 初始化内存失败
    ERROR_VO_RESOLUTION_INVALID        = ( MODULE_ID_VO << 24 ) + 7,          //7 vo的分辨率无效
    ERROR_VO_WRONG_OBJ_FORMAT          = ( MODULE_ID_VO << 24 ) + 8,          //8 图像格式错误
    ERROR_VO_ADD_OBJECT_FAILED         = ( MODULE_ID_VO << 24 ) + 9,          //9 图像添加失败
    ERROR_VO_WRONG_OBJECT_PARAM        = ( MODULE_ID_VO << 24 ) + 10,         //10 图像添加参数错误
};

enum ak_vo_layer {
    AK_VO_LAYER_BG_1 = 0X00,
    AK_VO_LAYER_BG_2,
    AK_VO_LAYER_BG_3,
    AK_VO_LAYER_BG_4,
    AK_VO_LAYER_BG_5,
    AK_VO_LAYER_BG_6,
    AK_VO_LAYER_BG_7,
    AK_VO_LAYER_BG_8,
    AK_VO_LAYER_VIDEO_1,
    AK_VO_LAYER_VIDEO_2,
    AK_VO_LAYER_VIDEO_3,
    AK_VO_LAYER_VIDEO_4,
    AK_VO_LAYER_VIDEO_5,
    AK_VO_LAYER_VIDEO_6,
    AK_VO_LAYER_VIDEO_7,
    AK_VO_LAYER_VIDEO_8,
    AK_VO_LAYER_GUI_1,
    AK_VO_LAYER_GUI_2,
    AK_VO_LAYER_GUI_3,
    AK_VO_LAYER_GUI_4,
    AK_VO_LAYER_GUI_5,
    AK_VO_LAYER_GUI_6,
    AK_VO_LAYER_GUI_7,
    AK_VO_LAYER_GUI_8,
    AK_VO_LAYER_NUM
};

enum ak_vo_init_mode {
    AK_VO_INIT_BLACK = 0,
    AK_VO_INIT_WHIHE = 1,
    AK_VO_INIT_NONE  = 2,
};

struct ak_vo_param{
    int width;                      /* width of visable screen  */
    int height;                     /* height of visable screen */
    AK_GP_FORMAT    format;         /* format of pixel */
    AK_GP_ROTATE    rotate;         /* rotation mode */
    enum ak_vo_init_mode init_mode; /* 0 - init screen to 0x00( BLACK ) ; 1 - init screen to 0xFF( WHITE ) 2 - not init */
};

struct ak_layer_pos{
    int top;            /* top position of the layer */
    int left;           /* left position of the layer */
    int width;          /* width of the  layer */
    int height;         /* height of the layer */
};

struct ak_layer_obj{
    int width;                      /* width of layer */
    int height;                     /* height of layer */
    struct ak_layer_pos	clip_pos;   /* postion info the clip picture */
    unsigned long dma_addr;         /* phyaddr of dma memory */
};

struct ak_tileyuv_obj{
    void *data;         /* pointer to the data */
    int flag;             /* clip flag*/
    struct ak_layer_pos clip_pos;   /* postion info the clip picture */
    void *priv;            /*reserved */
};

struct ak_vo_obj{
    AK_GP_FORMAT    format;                 /* src object format */
    AK_GP_OPT cmd;                          /* command for the obj*/
    union{
        struct ak_layer_obj	layer;          /* src info of the layer */
        struct ak_tileyuv_obj	tileyuv;    /* src info of the fbd */
    }src_obj;
    struct ak_layer_pos	dst_layer;          /* dst position  info of the layer */

    int alpha;                              /* alpha value */
    struct ak_gp_colorkey   colorkey;       /* colorkey struct */
    unsigned int color_rect;                /* color Rectangular filled*/
};
#define vo_layer        src_obj.layer
#define vo_tileyuv      src_obj.tileyuv


/* layer input infor */
struct ak_vo_layer_in
{
    struct ak_layer_pos create_layer;   /* layer size and pos in screen */

    AK_GP_FORMAT    format;             /* layer format , only support the rgb except the yuv format */
    AK_GP_OPT   layer_opt;              /* command for the layer support the alpha or colorkey */

    int alpha;                          /* alpha value */
    struct ak_gp_colorkey   colorkey;   /* colorkey struct */
};

/* layer output inform */
struct ak_vo_layer_out
{
    unsigned long dma_addr; /* phyaddr of dma memory */
    int layer_size;         /* layer size  in byte */
    void *vir_addr;         /* layer virtual address */
};

/*
 * ak_vo_get_version	--	get vo version
 * return string of version
 */
const char* ak_vo_get_version(void);

/*
 * ak_vo_open   --  open vo device
 *@param[IN]    :   param of vo
 *@dev_no[IN]   :   dev num of device
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_open(struct ak_vo_param *param, int dev_no);

/*
 * ak_vo_get_resolution     --      get the resolution of vo
 * @width[OUT]      :       width of visable screen
 * @height[OUT]     :       height of visable screen
 * @fb_width[OUT]   :       width of framebuffer
 * @fb_height[OUT]  :       height of framebuffer
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_get_resolution(int *width, int* height, int *fb_width, int *fb_height);

/*
 * ak_vo_close  --  close vo device
 *@dev_no[IN]   :   dev num of device
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_close(int dev_no);

/*
 * ak_vo_create_bg_layer    --  ·  create the bg layer
 * @bg_layer[IN]            :       pointer of struct ak_layer_pos, contains the size of layer and the pos
 *                                  in fb buffer;
 * @type[IN]                :       type of the layer;
 * @bg_info[OUT]            :       output the bg layer info
 * return AK_SUCCESS if successful, other value if failed
 */
int ak_vo_create_bg_layer(struct ak_vo_layer_in *bg_layer, enum ak_vo_layer type, struct ak_vo_layer_out *bg_info);

/*
 * ak_vo_create_video_layer     --  create the video layer
 * @video_layer[IN]         :       pointer of struct ak_layer_pos, contains the size of layer and the pos
 *                                  in fb buffer;
 * @type[IN]                :       type of the layer;
 * return AK_SUCCESS if successful, other value if failed
 */
int ak_vo_create_video_layer(struct ak_vo_layer_in *video_layer, enum ak_vo_layer type);

/*
 * ak_vo_create_gui_layer   --      create the gui layer
 * @gui_layer[IN]           :       pointer of struct ak_layer_pos, contains the size of layer and the pos
 *                                  in fb buffer;
 * @type[IN]                :       type of the layer;
 * @gui_info[OUT]           :       output the gui layer info
 * return AK_SUCCESS if successful, other value if failed
 */
int ak_vo_create_gui_layer(struct ak_vo_layer_in *gui_layer, enum ak_vo_layer type, struct ak_vo_layer_out *gui_info);

/*
 * ak_vo_set_layer_pos      --  set the layer position
 * @type[IN]  :                 the dst layer users want to change pos
 * @left[IN]  :                 left value;
 * @top[IN]   :                 top value
 * return AK_SUCCESS if successful, other value if failed
 */
int ak_vo_set_layer_pos(enum ak_vo_layer type, int left, int top);

/*
 * ak_vo_destroy_layer      --  destroy  layer
 * @type[IN]                :       type of the layer;
 * return AK_SUCCESS if successful, other value if failed
 */
int ak_vo_destroy_layer(enum ak_vo_layer type);

/*
 * ak_vo_add_obj    --  add object to vo
 * @obj[IN]     :       pointer of struct ak_vo_obj
 * @layer_no[IN]:       dst layer to add obj
 * return AK_SUCCESS if successful, Error code if failed
 * */
int ak_vo_add_obj(struct ak_vo_obj	*obj, enum ak_vo_layer layer_no);

/* THIS cmd is provided to ak_vo_refresh_screen function
 * NOTE : you can combine several value of them with | operation
          A type group contains 8 layers listed above.
          So a bit of the group value stands for a layer in sequence
 **/


enum ak_vo_refresh_cmd
{
    AK_VO_REFRESH_BG_GROUP    = 0xff,
    AK_VO_REFRESH_VIDEO_GROUP = 0xff00,
    AK_VO_REFRESH_GUI_GROUP   = 0xff0000
};

/*
 * ak_vo_refresh_screen		--	refresh the screen
 * cmd[IN] : cmd to refresh the layer created before.
 *           the value can be seen above in enum ak_vo_refresh_cmd
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_refresh_screen(int cmd);

enum AK_VO_REFRESH_MODE
{
    AK_VO_REFRESH_MODE_NONE         = 0x0 ,
    AK_VO_REFRESH_MODE_WAIT_LCDC    = 0x1 ,
    AK_VO_REFRESH_MODE_LAYER_BOTTOM = 0x2 ,
    AK_VO_REFRESH_MODE_LAYER_TMP    = 0x4 ,
};

/*
 * ak_vo_refresh_screen_ex --    刷新屏幕的扩展模式.(refresh the screen according to the extended mode)
 * @cmd[IN]  : 设定刷新到屏幕的图层标志位.(Refresh the layer flag of the screen)
 * @mode[IN]  :  0 - 与ak_vo_refresh_screen()保持一致.( As ak_vo_refresh_screen() function )
                 非0则按下列位设置进行操作(NOT 0 ,operater according to the following bit setting)
                 BIT0 是否等待LCD控制器可用(Whether to wait for the lcd controller to be available)
                 BIT1 使用最底同屏幕尺寸图层进行TDE预操作(Use the bottom layer with the same screen size for tde pre-opeartion)
                 BIT2 使用临时分配图层进行TDE预操作( Use temporary allocation layer for tde pre-operation)
                 可选值:
                 0x0 ( AK_VO_REFRESH_MODE_NONE ):
                       同ak_vo_refresh_screen
                 0x1 ( AK_VO_REFRESH_MODE_WAIT_LCDC ):
                       不进行tde预处理,单纯等待lcdc可用
                 0x2 ( AK_VO_REFRESH_MODE_LAYER_BOTTOM ):
                       判断lcd控制器是否可用，如果处于非可用状态，则对图层进行遍历，
                       找到一个和屏幕同等尺寸的图层，将其以上的图层blit到该图层，
                       然后，再将处理后的图层拷贝到FB内存上
                 0x3 ( AK_VO_REFRESH_MODE_LAYER_BOTTOM | AK_VO_REFRESH_MODE_WAIT_LCDC ):
                       判断lcd控制器是否可用，如果处于非可用状态，则对图层进行遍历，
                       找到一个和屏幕同等尺寸的图层，将其以上的图层blit到该图层，
                       等lcd控制器可用后，再将再将处理后的图层拷贝到FB内存上
                 0x4 ( AK_VO_REFRESH_MODE_LAYER_TMP ):
                       判断lcd控制器是否可用，如果处于非可用状态，则创建一个同fb缓冲区大小的临时图层
                       将全部图层全部BLIT上去后，再将临时图层拷贝到FB内存上
                 0x5 ( AK_VO_REFRESH_MODE_LAYER_TMP | AK_VO_REFRESH_MODE_WAIT_LCDC ):
                       判断lcd控制器是否可用，如果处于非可用状态，则创建一个同fb缓冲区大小的临时图层
                       将全部图层全部BLIT上去后，等lcd控制器可用后，再将临时图层拷贝到FB内存上，
  * return 成功 - AK_SUCCESS, 失败 - 大于0的错误码 ( successful - AK_SUCCESS , fail > 0 )
*/
int ak_vo_refresh_screen_ex(int cmd, int mode);

/*
 * ak_vo_set_layer_colorkey     --  set the  dst layer colorkey opt
 * @type[IN]  :                dst layer to set colorkey
 * @colorkey[IN]  :            colorkey value to be set
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_set_layer_colorkey(enum ak_vo_layer type, struct ak_gp_colorkey colorkey);

/*
 * ak_vo_set_layer_alpha        --  set the  dst layer transparent opt
 * @type[IN]  :                dst layer to set colorkey
 * @alpha[IN]  :            alpha value to be set
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_set_layer_alpha(enum ak_vo_layer type, int alpha);

/*
 * THIS buff num is used for seting the vo module using the buff to show screen
 */
enum ak_vo_buff_num
{
    AK_VO_BUFF_SINGLE = 0x00,
    AK_VO_BUFF_DOUBLE
};

/*
 * ak_vo_set_fbuffer_mode     --  set the  buffer mode
 * @mode[IN]  :                mode of the buff usage,value is from enum ak_vo_buff_num
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_set_fbuffer_mode(int mode);

/*
 * ak_vo_get_fbuffer_status  --  get the  current buffer use status
 * @cur_mode[OUT]        :            cur using on the module
 * @drv_sup[OUT]         :            cur mode that driver support
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_get_fbuffer_status(int *cur_mode, int *drv_sup);

/*
 * ak_vo_get_layer_res  --  get the  layer res
 * @type[IN]          :            layer num
 * @width[OUT]        :            cur layer width
 * @height[OUT]       :            cur layer height
 * return AK_SUCCESS if successful, Error code if failed
 */
int ak_vo_get_layer_res(enum ak_vo_layer type,int *width, int *height);

/*
 * ak_vo_check_lcdc_busy		--		check whether lcd controler is idle to accept the new cmd
 * @status[OUT]			:		busy status, valid value range: AK_TRUE or AK_FALSE
 * return AK_SUCCESS is successful, Error code if failed
 */
int ak_vo_check_lcdc_busy(unsigned long *status);

#endif
