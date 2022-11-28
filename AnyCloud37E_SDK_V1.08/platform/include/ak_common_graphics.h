#ifndef __AK_COMMON_GRAPHICS_H__
#define __AK_COMMON_GRAPHICS_H__

typedef enum ak_gp_format {                                                     //格式类型
    GP_FORMAT_RGB565 = 0,                                                       //支持的输入和输出格式
    GP_FORMAT_RGB888,
    GP_FORMAT_BGR565,
    GP_FORMAT_BGR888,
    GP_FORMAT_YUV420P,
    GP_FORMAT_YUV420SP,
    GP_FORMAT_ARGB8888,                                                         //下列仅定义为输入格式
    GP_FORMAT_RGBA8888,
    GP_FORMAT_ABGR8888,
    GP_FORMAT_BGRA8888,
    GP_FORMAT_TILED32X4,
}AK_GP_FORMAT;

typedef enum ak_gp_rotate{
    AK_GP_ROTATE_NONE = 0x0,
    AK_GP_ROTATE_90,
    AK_GP_ROTATE_180,
    AK_GP_ROTATE_270,
}AK_GP_ROTATE;

typedef enum ak_gp_opt {
    GP_OPT_NONE        = 0,                                                    //空操作
    GP_OPT_FILLRECT    = 1,                                                    //颜色填充操作
    GP_OPT_BLIT        = 2,                                                    //仅仅对图层进行拷贝操作
    GP_OPT_SCALE       = 4,                                                    //拉伸|缩放操作
    GP_OPT_ROTATE      = 8,                                                    //旋转操作
    GP_OPT_TRANSPARENT = 16,                                                   //设置图层的透明度
    GP_OPT_COLORKEY    = 32,                                                   //将图层中指定的颜色进行过滤
    GP_OPT_BLIT_TRANSPARENT = GP_OPT_BLIT | GP_OPT_TRANSPARENT ,
    GP_OPT_BLIT_COLORKEY = GP_OPT_BLIT | GP_OPT_COLORKEY ,
    GP_OPT_BLIT_TRANSPARENT_COLORKEY = GP_OPT_BLIT | GP_OPT_TRANSPARENT | GP_OPT_COLORKEY ,
    GP_OPT_SCALE_TRANSPARENT = GP_OPT_SCALE | GP_OPT_TRANSPARENT ,
    GP_OPT_SCALE_COLORKEY = GP_OPT_SCALE | GP_OPT_COLORKEY ,
    GP_OPT_SCALE_TRANSPARENT_COLORKEY = GP_OPT_SCALE | GP_OPT_TRANSPARENT | GP_OPT_COLORKEY ,
    GP_OPT_ROTATE_TRANSPARENT = GP_OPT_ROTATE | GP_OPT_TRANSPARENT ,
    GP_OPT_ROTATE_COLORKEY = GP_OPT_ROTATE | GP_OPT_COLORKEY ,
    GP_OPT_ROTATE_TRANSPARENT_COLORKEY = GP_OPT_ROTATE | GP_OPT_TRANSPARENT | GP_OPT_COLORKEY ,
}AK_GP_OPT;

typedef enum ak_gp_coloract {                                                   //colorkey操作中对颜色范围的操作方式
    COLOR_DELETE = 0,                                                           //color_min和color_max区间的颜色进行删除,区间外的颜色保留
    COLOR_KEEP = 1,                                                             //color_min和color_max区间的颜色保留,范围外的进行删除
}AK_GP_COLORACT;

struct ak_gp_colorkey {                                                         //颜色过滤结构
    unsigned int color_min;                                                     //颜色下限值
    unsigned int color_max;                                                     //颜色上限值
    enum ak_gp_coloract coloract;                                               //colorkey操作中对颜色范围的操作方式
};

#endif
