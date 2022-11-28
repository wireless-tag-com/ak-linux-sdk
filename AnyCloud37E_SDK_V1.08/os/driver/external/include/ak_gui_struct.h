#ifndef __AK_GUI_STRUCT__
#define __AK_GUI_STRUCT__

#define FBD_ADDR_ALIGN		(32 - 1)
#define FBD_PITCH_ALIGN		(32 - 1)
#define FBD_WIDTH_ALIGN		(8 - 1)

/*layer**********************************************************/
struct transparent_attr {
	int is_src_image_select;/*选择对源图片或目标图片进行透明色功能*/
	int is_inside_pority;/*对比图片单个像素值在或不在预定参考值范围内则透明*/
	int enable;/*透明色总开关*/

	int color_th_min;/*设置透明色功能时的参考色的最小值*/
	int color_th_max;/*设置透明色功能时的参考色的最大值*/

#if defined(CONFIG_MACH_AK37E)
	int transparent_with_alpha;/*当源图ARGB8888/RGBA8888/ABGR8888/BGRA8888格式时，
								 透明色功能是否需要比较Alpha通道的值。*/
#endif
};

enum logic_type {
	OP_NONE = 0,/*不操作*/
	OP_AND,/*逻辑与*/
	OP_OR,/*逻辑或*/
	OP_XOR,/*逻辑异或*/
	OP_COPY,/*复制操作，输出源图片*/
};

struct layer_operation_attr {
	int is_logic_operation;/*源图和目标图做逻辑操作，还是alpha blending操作*/
	enum logic_type logic_type;
	int alpha_blending_value;
#if defined(CONFIG_MACH_AK37E)
	int layer_operation_select;/*alpha值的选择,1:使用源图的Alpha通道,0:使用软件配置的值*/
#endif
};

struct clip_attr {
	int clip_enable;
	unsigned short clip_top;
	unsigned short clip_left;
	unsigned short clip_bottom;
	unsigned short clip_right;
};

struct akgui_layer_attr {
	struct transparent_attr transparent;
	struct clip_attr clip;
	struct layer_operation_attr layer_operation;
};

/*public**********************************************************/
enum OFORMAT_TYPE {
	OF_RGB565 = 0,
	OF_RGB888,
	OF_BGR565,
	OF_BGR888,
	OF_YUV420P,
	OF_YUV420SP
};

enum IFORMAT_TYPE {
	IF_RGB565 = 0,
	IF_RGB888,
	IF_BGR565, IF_BGR888,
	IF_YUV420P,
	IF_YUV420SP,
	IF_ARGB8888,
	IF_RGBA8888,
	IF_ABGR8888,
	IF_BGRA8888,
	IF_TILED32X4,
};

/*rectangular**********************************************************/
enum rect_rotation_degree {
	/*顺时针旋转角度*/
	RECT_ROTAION_CLOCKWISE_DEGREE_0 = 0,
	RECT_ROTAION_CLOCKWISE_DEGREE_90,
	RECT_ROTAION_CLOCKWISE_DEGREE_180,
	RECT_ROTAION_CLOCKWISE_DEGREE_270
};

struct rect_accelerate_attr {
	int sw_block_partition_enable;/*是否使能通过软件配置
									划分Rectangular flow单次处理的块大小，
									以优化DMA性能和减少不必要的内存占用*/
	unsigned short block_height_size_cfg;/*Rectangular Flow时
										   单个处理单元（Block）的高度*/
	unsigned short block_width_size_cfg;/*Rectangular Flow时
										   单个处理单元（Block）的宽度*/
	unsigned short block_height_number_cfg;/*Rectangular Flow时纵向Block数目*/
	unsigned short block_width_number_cfg;/*Rectangular Flow时横向Block数目*/
};

struct monochrome_fill_attr {
	int monochrome_fill_enable;/*单色填充*/
	unsigned int monochrome_fill_color;/*矩形绘制时的画笔颜色*/
};

struct akgui_fbd_attr {
	unsigned int LumaAddr;
	unsigned int ChromaAddr;
	unsigned int MapLumaAddr;
	unsigned int MapChromaAddr;
	unsigned int TilePitch;

	unsigned int PictureHeight;
	unsigned int PictureWidth;
};

struct akgui_rect_attr {
	enum rect_rotation_degree rotation_degree;/*旋转*/
	struct monochrome_fill_attr monochrome_fill;/*单色矩形填充*/

	unsigned int src_image_baddr;
	unsigned short src_image_width;/*使用Rectangular Flow时源图像的宽度*/
	unsigned short src_image_y_offset;/*使用Rectangular Flow时，
										从源图片中截取矩形区域的起始点Y坐标*/
	unsigned short src_image_x_offset;/*使用Rectangular Flow时，
										从源图片中截取矩形区域的起始点X坐标*/
	unsigned int dst_image_baddr;
	unsigned short dst_image_width;/*Rectangular Flow时，目标图像的宽度*/
	unsigned short dst_image_y_offset;/*使用Rectangular Flow时，
										贴图图像在目标图像中的起始点Y坐标*/
	unsigned short dst_image_x_offset;/*使用Rectangular Flow时，
										贴图图像在目标图像中的起始点X坐标*/
	unsigned short dst_image_rect_height;/*使用Rectangular Flow时，
										   目标图片中贴图区域的垂直方向尺寸。*/
	unsigned short dst_image_rect_width;/*使用Rectangular Flow时，
										  目标图片中贴图区域的水平方向尺寸*/

	struct akgui_layer_attr layer;

	enum OFORMAT_TYPE oformat;
	enum IFORMAT_TYPE iformat;

	int need_dest_image;		/*是否需要下载目标图*/
	int rect_enable;
};


/*scaler**********************************************************/
struct akgui_scaler_attr {
//	unsigned short inv_src_image_scale_height;/*缩放系数*/
//	unsigned short para_vilx;/*协助芯片内部逻辑完成V方向上的scale操作*/
//	unsigned short para_hilx;/*协助芯片内部逻辑完成H方向上的scale操作*/
	unsigned int src_image_baddr1;/*当输入格式为YUV420或YUV420 
									semi-planar时，代表Y分量的地址；
									当输入格式为RGB时，代表RGB数据的基地址*/
	unsigned int src_image_baddr2;/*输入格式为YUV420时，U分量的地址；
									输入格式为YUV420 semi-planar时，
									UV分量的地址。当进行scale操作且
									输入格式为RGB时，此值没有意义*/
	unsigned int src_image_baddr3;/*输入格式为YUV420时，V分量的地址。
									当进行scale操作且输入格式为
									RGB或YUV420 semi-planar时，此值没有意义*/
	unsigned short src_image_height;/*Scaler Flow时整个源图片的
									  垂直方向尺寸.*/
	unsigned short src_image_width;/*Scaler Flow时整个源图片的
									 水平方向尺寸.*/
	unsigned short src_image_y_offset;/*Scaler Flow缩放时，
										从整个源图片中截取的参与
										缩放的矩形区域的起始点Y坐标*/
	unsigned short src_image_x_offset;/*Scaler Flow缩放时，
										从整个源图片中截取的参与
										缩放的矩形区域的起始点X坐标*/
	/*上面几个和下面几个指虚页*/
	unsigned short src_image_scale_height;/*源图片虚页后需要
											缩放的区域的垂直方向尺寸*/
	unsigned short src_image_scale_width;/*源图片虚页后需要
										   缩放的区域的水平方向尺寸*/
	unsigned int dst_image_baddr1;/*如果目标图像是RGB/BGR格式，
									代表全部目标图像数据的基地址；
									如果目标图像是yuv格式，
									代表目标图像中Y分量的基地址*/
	unsigned int dst_image_baddr2;/*Scaler Flow时目标图像的U分量地址
									（YUV420）或UV分量地址
									（YUV420 semi-planar）。
									此寄存器仅在目标图像为YUV420
									或YUV420 semi-planar时有效*/
	unsigned int dst_image_baddr3;/*Scaler Flow时目标图像的V分量地址。
									此寄存器仅在目标图像为YUV420时有效*/
	unsigned short dst_image_height;/*Scaler Flow时整个目标图像垂直方向尺寸*/
	unsigned short dst_image_width;/*Scaler Flow时整个目标图像水平方向尺寸*/
	unsigned short dst_image_y_offset;/*Scaler Flow缩放后的图像贴图
										在目标图像中的起始点Y坐标*/
	unsigned short dst_image_x_offset;/*Scaler Flow缩放后的图像贴图
										在目标图像中的起始点X坐标*/
	unsigned short dst_image_rect_height;/*使用Scaler Flow时，
										   目标图片中贴图区域的
										   垂直方向尺寸*/
	unsigned short dst_image_rect_width;/*使用Scaler Flow时，
										   目标图片中贴图区域的
										   水平方向尺寸*/

	struct akgui_fbd_attr fbd_attr;

	struct akgui_layer_attr layer;

	enum OFORMAT_TYPE oformat;
	enum IFORMAT_TYPE iformat;

	int need_dest_image;		/*是否需要下载目标图*/
	int scaler_enable;
};

/*rotation**********************************************************/
struct akgui_rotation_attr {
};

struct akgui_attr {
	struct akgui_rect_attr rect;
	struct akgui_scaler_attr scaler;
};


struct fb_dma_attr {
	unsigned int dma_size;
	unsigned int dma_paddr;
	unsigned int *dma_vaddr;
};

#if 0
#define AKISP_MAGIC 'L'
#define AK_FB_SET_GUI	_IOR(AKISP_MAGIC,  1, int)
#define AK_FB_ALLOC_DMA	_IOR(AKISP_MAGIC,  2, int)
#define AK_FB_FREE_DMA	_IOR(AKISP_MAGIC,  3, int)
#endif

static inline int fill_rgb2internal(unsigned char fill_r, unsigned char fill_g,
		unsigned char fill_b, enum OFORMAT_TYPE oformat, unsigned int *output)
{
	int ret = 0;

	switch (oformat) {
		case OF_RGB565:
		case OF_BGR565:
			*output = ((fill_r << 16) | (fill_g << 8) | fill_b)
				& 0xf8fcf8;
			break;

		case OF_RGB888:
		case OF_BGR888:
			*output = (fill_r << 16) | (fill_g << 8) | fill_b;
			break;

		default:
			ret = -1;
			break;
	}

	return ret;
}

#endif
