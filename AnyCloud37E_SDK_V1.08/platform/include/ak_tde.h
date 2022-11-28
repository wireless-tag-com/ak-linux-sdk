#ifndef __AK_TDE_H__
#define __AK_TDE_H__

enum ak_tde_errno {                                                             //错误代码
    ERROR_TDE_DEV_NOT_OPEN              = ( MODULE_ID_TDE << 24 ) + 0,          //0 未打开TDE设备
    ERROR_TDE_NOT_SUPPORT_OPT           = ( MODULE_ID_TDE << 24 ) + 1,          //1 在ak_tde_opt操作中,opt不能为enum ak_gp_opt之外的类型
    ERROR_TDE_NO_MAIN_OPT               = ( MODULE_ID_TDE << 24 ) + 2,          //2 没有设置主操作
    ERROR_TDE_ROTATE_NOT_SUPPORT_YUV    = ( MODULE_ID_TDE << 24 ) + 3,          //3 旋转操作不支持YUV输入/输出格式
    ERROR_TDE_FILLRECT_NOT_SUPPORT_YUV  = ( MODULE_ID_TDE << 24 ) + 4,          //4 FILLRECT操作不支持YUV输出格式
    ERROR_TDE_NOT_SUPPORT_INPUT_FORMAT  = ( MODULE_ID_TDE << 24 ) + 5,          //5 不支持输入格式
    ERROR_TDE_NOT_SUPPORT_OUTPUT_FORMAT = ( MODULE_ID_TDE << 24 ) + 6,          //6 不支持输出格式
    ERROR_TDE_INPUT_RECT_OVERFLOW       = ( MODULE_ID_TDE << 24 ) + 7,          //7 输入坐标溢出
    ERROR_TDE_OUTPUT_RECT_OVERFLOW      = ( MODULE_ID_TDE << 24 ) + 8,          //8 输出坐标溢出
    ERROR_TDE_PHYADDR_NULL              = ( MODULE_ID_TDE << 24 ) + 9,          //9 物理地址不能为空
    ERROR_TDE_IOCTL_ERROR               = ( MODULE_ID_TDE << 24 ) + 10,         //10 IOCTL出错
    ERROR_TDE_ROTATE_NOT_SUPPORT_VAL    = ( MODULE_ID_TDE << 24 ) + 11,         //11 旋转角度出错
    ERROR_TDE_ALPHA_NOT_SUPPORT_VAL     = ( MODULE_ID_TDE << 24 ) + 12,         //12 透明度数值设置出错
    ERROR_TDE_SET_FLOW_FAILED           = ( MODULE_ID_TDE << 24 ) + 13,         //13 flow设置错误
    ERROR_TDE_NOT_SUPPORT_SIZE          = ( MODULE_ID_TDE << 24 ) + 14,         //14 拉伸缩放的源宽高和目标宽高比较大于等于SIZE_MIN_SCALER,或者其他操作中图层的宽高和选中宽高等于0|大于最大分辨率
    ERROR_TDE_CMD_CANNOT_BE_NULL        = ( MODULE_ID_TDE << 24 ) + 15,         //15 在ak_tde_opt操作中,p_tde_tde不能为空
    ERROR_TDE_COLORKEY_SET_TYPE         = ( MODULE_ID_TDE << 24 ) + 16,         //16 colorkey的选择范围设置出错
    ERROR_TDE_LAYER_CANNOT_BE_NULL      = ( MODULE_ID_TDE << 24 ) + 17,         //17 layer不能为空
    ERROR_TDE_SCALER_OVERFLOW           = ( MODULE_ID_TDE << 24 ) + 18,         //18 缩放倍数超过限定的10倍
    ERROR_TDE_YUV_SIZE_CANNOT_BE_ODD    = ( MODULE_ID_TDE << 24 ) + 19,         //19 YUV格式的像素值不能为奇数
};

struct ak_tde_layer                                                             //图层描述结构
{
    unsigned short width;                                                       //图层宽度
    unsigned short height;                                                      //图层高度
    unsigned short pos_left;                                                    //截取的位置坐标X轴
    unsigned short pos_top;                                                     //截取的位置坐标Y轴
    unsigned short pos_width;                                                   //截取的宽度
    unsigned short pos_height;                                                  //截取的高度
    unsigned long phyaddr;                                                      //图层物理地址
    enum ak_gp_format format_param;                                             //图层的颜色属性
};

struct ak_tde_cmd {                                                             //tde参数用以配置操作模式结构
    unsigned int opt;                                                           //图层操作指令,可以使用位操作进行定义
    struct ak_tde_layer tde_layer_src;                                          //源图层属性
    struct ak_tde_layer tde_layer_dst;                                          //目标图层属性
    enum ak_gp_rotate rotate_param;                                             //翻转角度
    int alpha;                                                                  //透明度设置参数
    struct ak_gp_colorkey colorkey_param;                                       //颜色过滤设置参数
    unsigned int color_rect;                                                    //fillrect接口颜色输入
    void *pv_reserve;                                                           //保留接口参数传入指针,在当前配置参数满足不了应用参数传入需求情况下,使用该指针
};

struct ak_tde_decode_info
{
    void *p_fbd_info;
    unsigned short clip_left;                                                   //截取的位置坐标x轴 left position of the layer
    unsigned short clip_top;                                                    //截取的位置坐标y轴 top position of the layer
    unsigned short clip_width;                                                  //截取的宽度 width of the  layer
    unsigned short clip_height;                                                 //截取的高度 height of the layer
};

/*
   ak_tde_get_version: 获取tde版本号
   return: 版本号
*/
const char* ak_tde_get_version(void);

/*
   ak_tde_open: 打开tde设备
   return: 成功-ERROR_TYPE_NO_ERROR 失败-ERROR_TYPE_DEV_OPEN_FAILED
*/
int ak_tde_open( void );

/*
   ak_tde_close: 关闭tde设备
   return: 成功-ERROR_TYPE_NO_ERROR 失败-ERROR_TYPE_DEV_CLOSE_FAILED
*/
int ak_tde_close( void );

/*
   ak_tde_opt: 根据设置的参数进行操作
   @p_tde_cmd[IN]:传入的设置参数
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_opt( struct ak_tde_cmd *p_tde_cmd );

/*
   ak_tde_opt_blit: 图层拷贝工作
   @p_tde_layer_src[IN]: 源图层描述结构指针
   @p_tde_layer_dst[IN]: 目标图层描述结构指针
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_opt_blit( struct ak_tde_layer *p_tde_layer_src, struct ak_tde_layer *p_tde_layer_dst );

/*
   ak_tde_opt_format: 图层转换工作
   @p_tde_layer_src[IN]: 源图层描述结构指针
   @p_tde_layer_dst[IN]: 目标图层描述结构指针
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_opt_format( struct ak_tde_layer *p_tde_layer_src, struct ak_tde_layer *p_tde_layer_dst );

/*
   ak_tde_opt_scale: 图层拉伸
   @p_tde_layer_src[IN]: 源图层描述结构指针
   @p_tde_layer_dst[IN]: 目标图层描述结构指针
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_opt_scale( struct ak_tde_layer *p_tde_layer_src, struct ak_tde_layer *p_tde_layer_dst );

/*
   ak_tde_opt_rotate: 图层旋转
   @p_tde_layer_src[IN]: 源图层描述结构指针
   @p_tde_layer_dst[IN]: 目标图层描述结构指针
   @rotate_param[IN]: 旋转角度
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_opt_rotate( struct ak_tde_layer *p_tde_layer_src, struct ak_tde_layer *p_tde_layer_dst, enum ak_gp_rotate rotate_param);

/*
   ak_tde_opt_transparent: 图层透明度
   @p_tde_layer_src[IN]: 源图层描述结构指针
   @p_tde_layer_dst[IN]: 目标图层描述结构指针
   @alpha[IN]: 透明度0-15 0为透明度最高,15为透明度最低
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_opt_transparent( struct ak_tde_layer *p_tde_layer_src, struct ak_tde_layer *p_tde_layer_dst, int alpha);

/*
   ak_tde_opt_colorkey: colorkey颜色过滤
   @p_tde_layer_src[IN]: 源图层描述结构指针
   @p_tde_layer_dst[IN]: 目标图层描述结构指针
   @colorkey_param[IN]: colorkey参数
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_opt_colorkey( struct ak_tde_layer *p_tde_layer_src, struct ak_tde_layer *p_tde_layer_dst, struct ak_gp_colorkey colorkey_param);

/*
   ak_tde_opt_fillrect: 指定的坐标画矩形
   @p_tde_layer[IN]: 图层描述结构指针
   @color[IN]: 颜色
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_opt_fillrect( struct ak_tde_layer *p_tde_layer, unsigned int color );

/*
   ak_tde_decode: 将一帧视频进行解码后写入对应的物理内存中去
   @p_fbd_info[IN]: 解码数据描述结构
   @p_tde_layer[IN]: 解码后的目标图层结构
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_decode( void *p_fbd_info, struct ak_tde_layer *p_tde_layer );

/*
   ak_tde_decode_ex: 将一帧视频进行解码后写入对应的物理内存中去
   @decode_info[IN]: 解码数据描述结构
   @p_tde_layer[IN]: 解码后的目标图层结构
   return: 成功-0 失败:>0的错误码
*/
int ak_tde_decode_ex( struct ak_tde_decode_info *decode_info, struct ak_tde_layer *p_tde_layer );
#endif