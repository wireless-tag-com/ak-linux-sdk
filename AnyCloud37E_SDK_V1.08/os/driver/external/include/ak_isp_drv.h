#ifndef _AK_ISP_DRV_H_
#define _AK_ISP_DRV_H_

#ifndef AK_RTOS
//#include "anyka_types.h"
#endif

#define ISP_DRV_LIB_VER     "isp_drv_lib-V0.2.3"

#if 1
enum ak_yuv420_type{
	YUV420_PLANAR = 0,
	YUV420_SEMI_PLANAR,
};
#endif

enum isp_working_mode {
    ISP_JPEG_MODE,			//JPEG compression frame mode
    ISP_RAW_OUT,
    ISP_YUV_OUT,			//YUV single frame mode. is not support minor channel
    ISP_YUV_VIDEO_OUT,      //YUV video frame mode
    ISP_RGB_OUT,			//RGB single frame mode
    ISP_RGB_VIDEO_OUT,		//RGB video frame mode
};

enum buffer_id {
    BUFFER_ONE,
    BUFFER_TWO,
    BUFFER_THREE,
    BUFFER_FOUR,
};

enum data_format {
	BAYER_RAW_DATA = 0,
	YUV422_DATA,
	JPEG_COMPRESS_DATA,
};

struct input_data_format {
	enum data_format df;
	int data_width;
};

typedef struct ak_isp_blc {
    unsigned short     black_level_enable;	//使能位
    unsigned short     bl_r_a;				//[ 0,1023]
    unsigned short     bl_gr_a;				//[ 0,1023]
    unsigned short     bl_gb_a;				//[ 0,1023]
    unsigned short     bl_b_a;				//[ 0,1023]
    signed short     bl_r_offset;			//[-2048,2047]
    signed short     bl_gr_offset;			//[-2048,2047]
    signed short     bl_gb_offset;			//[-2048,2047]
    signed short     bl_b_offset;			//[-2048,2047]
}AK_ISP_BLC;

typedef struct ak_isp_blc_attr {
    unsigned short       blc_mode;             //0联动模式，1手动模式
    AK_ISP_BLC  m_blc;
    AK_ISP_BLC  linkage_blc[9];
}AK_ISP_BLC_ATTR;

typedef struct {
    unsigned short coef_b[10];    //[0,255]
    unsigned short coef_c[10];    //[0,1023]
}lens_coef;

typedef struct ak_isp_lsc_attr {
    unsigned short       enable;
    //the reference point of lens correction
    unsigned short       xref;        //[0,4096]
    unsigned short       yref;        //[0,4096]
    unsigned short       lsc_shift;   //[0，15]
    lens_coef   lsc_r_coef;
    lens_coef   lsc_gr_coef;
    lens_coef   lsc_gb_coef;
    lens_coef   lsc_b_coef;
    //the range of ten segment
    unsigned short       range[10];   //[0，1023]
    unsigned short       lsc_mode;
    unsigned char      linkage_strength[10];   //[0，1023]
}AK_ISP_LSC_ATTR;

typedef struct ak_isp_raw_lut_attr {
    unsigned short   raw_r[129];      //10bit
    unsigned short   raw_g[129];      //10bit
    unsigned short   raw_b[129];      //10bit
    unsigned short   r_key[16];
    unsigned short   g_key[16];
    unsigned short   b_key[16];
    unsigned short   raw_gamma_enable;
}AK_ISP_RAW_LUT_ATTR;

typedef struct ak_isp_rgb_gamma_attr {
    unsigned short   r_gamma[129];   //10bit
    unsigned short   g_gamma[129];   //10bit
    unsigned short   b_gamma [129];  //10bit
    unsigned short   r_key[16];
    unsigned short   g_key[16];
    unsigned short   b_key[16];
    unsigned short   rgb_gamma_enable;  //如果不使能，就是一条直线
} AK_ISP_RGB_GAMMA_ATTR;

typedef struct ak_isp_y_gamma_attr {
	unsigned short    ygamma[129];    //10bit
	unsigned short    ygamma_key[16]; //曲线的关键点
	unsigned short    ygamma_uv_adjust_enable;
	unsigned short    ygamma_uv_adjust_level;
	unsigned short    ygamma_cnoise_yth1;   //Ygamma色差抑制门限值
	unsigned short    ygamma_cnoise_yth2;   //Ygamma色差抑制门限值
	unsigned short    ygamma_cnoise_slop;
	unsigned short    ygamma_cnoise_gain ;  //UV调整系数计算参数
}AK_ISP_Y_GAMMA_ATTR;

typedef struct ak_isp_nr1 {
    unsigned short  nr1_enable;            //使能位
    unsigned short  nr1_weight_rtbl[17];   //10bit
    unsigned short  nr1_weight_gtbl[17];   //10bit
    unsigned short  nr1_weight_btbl[17];   //10bit
    unsigned short  nr1_k;                 //[0,15]
    unsigned short  nr1_lc_lut[17];        //10bit
    unsigned short  nr1_lc_lut_key[16];
    unsigned short  nr1_calc_g_k;
    unsigned short  nr1_calc_r_k;
    unsigned short  nr1_calc_b_k;
}AK_ISP_NR1;

typedef struct ak_isp_s_nr1_attr {
    unsigned short       nr1_mode;             //nr1 模式，自动或者联动模式
    AK_ISP_NR1  manual_nr1;
    AK_ISP_NR1  linkage_nr1[9];       //联动参数
}AK_ISP_NR1_ATTR;

typedef struct ak_isp_nr2 {
    unsigned short    nr2_enable;
	unsigned short    nr2_pass2_enable;
	unsigned short ynr_mf_th1;	//[0, 8191],default:300
	unsigned short ynr_mf_th2;	//[0, 8191],default:500
	unsigned short ynr_intensity1;//[0, 15],default:15
	unsigned short ynr_intensity2;//[0, 15],default:15
	//unsigned short ynr_intensity_slop;／／ynr_intensity_slop = (ynr_intensity1-ynr_intensity2 )*1024/(ynr_mf_th2-ynr_mf_th1);

    unsigned short    nr2_weight_tbl[17];    //10bit
    unsigned short    nr2_k;                 //[0,15]
    unsigned short    nr2_calc_y_k;
    unsigned short    y_dpc_enable;
	unsigned short	 y_dpc_th;
	unsigned short	 y_black_dpc_enable;
	unsigned short	 y_white_dpc_enable;
}AK_ISP_NR2;

typedef struct ak_isp_uvnr {
    unsigned short  uvnr_enable;            //使能位
    unsigned short  uvnr_pass2_enable;
    unsigned short  uvnr_weight_rtbl[17];   //10bit
    unsigned short  uvnr_k;                 //[0,15]
    unsigned short  uvnr_calc_k;
}AK_ISP_UVNR;

typedef struct ak_isp_nr2_attr {
    unsigned short       nr2_mode;          //手动或者联动模式
    AK_ISP_NR2  manual_nr2;
    AK_ISP_NR2  linkage_nr2[9];
}AK_ISP_NR2_ATTR;

typedef struct ak_isp_uvnr_attr {
    unsigned short       uvnr_mode;          //手动或者联动模式
    AK_ISP_UVNR  manual_uvnr;
    AK_ISP_UVNR  linkage_uvnr[9];
}AK_ISP_UVNR_ATTR;

typedef struct ak_isp_3d_nr_ref_attr {
	unsigned int	yaddr_3d;
	unsigned int	ysize_3d;
	unsigned int	uaddr_3d;
	unsigned int	usize_3d;
	unsigned int	vaddr_3d;
	unsigned int	vsize_3d;
}AK_ISP_3D_NR_REF_ATTR;

typedef struct ak_isp_3d_nr {
	unsigned short		tnr_enable;			//default:1(GUI edit)
	unsigned short		updata_ref;			//更新y/uv,default:1
	unsigned short		tnr_refFrame_format;	//参考帧的格式,default:2

	unsigned short		t_mf_th1; 	//[0, 8191],default:300(GUI edit)
	unsigned short		t_mf_th2; 	//[0, 8191],default:500(GUI edit)
	unsigned short 		statScale[129];	//[0, 15],default:8(GUI edit)
	unsigned short		statScale_key[16];
	unsigned short	 	t_k_scale;	//[0, 63],default:34
	unsigned short		minstep;	//[0-15],default:2
	unsigned short		update_th;	//[0-15],default:8

	unsigned short		uvnr_k; 		//[0, 15],default:8(GUI edit)
	unsigned short		uvlp_k1; 		//[0, 15],default:7(GUI edit)
	unsigned short		uvlp_k2; 		//[0, 15],default:0(GUI edit)
	//unsigned short		uvlp_kslop;		//[0, 127]	 uvlp_kslop=(uvlp_k1-uvlp_k2 )*1024/(t_mf_th2-t_mf_th1);
	unsigned short		t_uv_th1;		//[0, 255],default:48(GUI edit)
	unsigned short		t_uv_th2;		//[0, 255],default:16(GUI edit)
	//unsigned short		t_uv_th_slop;		//[0, 255]	t_uv_th_slop =  (t_uv_th1-t_uv_th2 )*1024/(t_mf_th2-t_mf_th1);
	unsigned short		t_uv_k; 		//[0, 127],default:120(GUI edit)
	unsigned short		t_uv_kslop; 		//[0, 127],default:32(GUI edit)

	unsigned short		ynr_weight_tbl[17];//ynr_strength(GUI edit)
	unsigned short		ynr_calc_k;
	unsigned short		ylp_k1;			//[0, 15],default:7(GUI edit)
	unsigned short		ylp_k2;			//[0, 15],default:0(GUI edit)
	//unsigned short		ylp_kslop;		//[0, 255]	ylp_kslop = (ylp_k1-ylp_k2 )*1024/(t_mf_th2-t_mf_th1);
	unsigned short		t_y_th1;		//[0, 255],default:48(GUI edit)
	unsigned short		t_y_th2;		//[0, 255],default:16(GUI edit)
	//unsigned short		t_y_th_slop;		//[0, 255]	t_y_th_slop = (t_y_th1-t_y_th2 )*1024/(t_mf_th2-t_mf_th1);
	unsigned short		t_y_k; 		//[0, 127],default:120(GUI edit)
	unsigned short		t_y_kslop; 		//[0, 127],default:32(GUI edit)

	unsigned int		md_th;			//[0, 65535]  运动检测阈值 [0-127],default:0
}AK_ISP_3D_NR;

typedef struct  ak_isp_3d_nr_attr {
    unsigned short         _3d_nr_mode;
    AK_ISP_3D_NR  manual_3d_nr;
    AK_ISP_3D_NR  linkage_3d_nr[9];
}AK_ISP_3D_NR_ATTR;

typedef struct  ak_isp_3d_nr_stat_info {
	unsigned short		MD_stat_max;
	unsigned short		MD_stat[24][32];		//运动检测分块输出lz0499 9_12
	unsigned int		MD_level;			    //运动检测输出
}AK_ISP_3D_NR_STAT_INFO;

typedef struct ak_isp_gb {

    unsigned short   gb_enable;        //使能位
    unsigned short   gb_en_th;         //[0,255]
    unsigned short   gb_kstep;         //[0,15]
    unsigned short   gb_threshold;     //[0,1023
} AK_ISP_GB;

typedef struct ak_isp_gb_attr {
    unsigned short      gb_mode;       //模式选择，手动或者联动
    AK_ISP_GB  manual_gb;
    AK_ISP_GB  linkage_gb[9];
} AK_ISP_GB_ATTR;

typedef struct ak_isp_demo_attr {
    unsigned short  dm_HV_th;       //方向判别系数
    unsigned short  dm_rg_thre;     //[0 1023]
    unsigned short  dm_bg_thre;     //[0 1023]
    unsigned short  dm_hf_th1;      //[0, 1023]
    unsigned short  dm_hf_th2;      //[0, 1023]

    unsigned short  dm_rg_gain;     //[0 255]
    unsigned short  dm_bg_gain;     //[0 255]
    unsigned short  dm_gr_gain;     //[0 255]
    unsigned short  dm_gb_gain;     //[0 255]
    //unsigned short  cfa_mode;
}AK_ISP_DEMO_ATTR;

typedef struct ak_isp_ccm {
	unsigned short  cc_enable;		   //颜色校正使能
	unsigned short  cc_cnoise_yth;	   //亮度控制增益
	unsigned short  cc_cnoise_gain;	   //亮度控制增益
	unsigned short  cc_cnoise_slop;	   //亮度控制增益
	signed short  ccm[3][3];		   //[-2048, 2047]
}AK_ISP_CCM;

typedef struct ak_isp_ccm_attr {
    unsigned short       cc_mode;  		//颜色校正矩阵联动或者手动
    AK_ISP_CCM  manual_ccm;
    AK_ISP_CCM  ccm[4]; 		//四个联动矩阵
}AK_ISP_CCM_ATTR;

typedef struct ak_isp_wdr {
    unsigned short   hdr_uv_adjust_level;        //uv调整程度, [0,31]
    unsigned short   hdr_cnoise_suppress_slop;   //抑制斜率
    unsigned short   wdr_enable;

	unsigned short	wdr_th1;	  //0-1023
	unsigned short	wdr_th2;	  //0-1023
	unsigned short	wdr_th3;	  //0-1023
	unsigned short	wdr_th4;	  //0-1023
	unsigned short	wdr_th5;	  //0-1023

    //unsigned short wdr_light_weight;

	unsigned short	area_tb1[65];	  //曲线 10bit
	unsigned short	area_tb2[65];	  //曲线 10bit
	unsigned short	area_tb3[65];	  //曲线 10bit
	unsigned short	area_tb4[65];	  //曲线 10bit
	unsigned short	area_tb5[65];	  //曲线 10bit
	unsigned short	area_tb6[65];	  //曲线 10bit

	unsigned short	area1_key[16];
	unsigned short	area2_key[16];
	unsigned short	area3_key[16];
	unsigned short	area4_key[16];
	unsigned short	area5_key[16];
	unsigned short	area6_key[16];

    unsigned short   hdr_uv_adjust_enable;       //uv调整使能
    unsigned short   hdr_cnoise_suppress_yth1;   //色彩噪声亮度阈值1
    unsigned short   hdr_cnoise_suppress_yth2;   //色彩噪声亮度阈值2
    unsigned short   hdr_cnoise_suppress_gain;   //色差抑制
}AK_ISP_WDR;

typedef struct ak_isp_wdr_attr {
	unsigned short wdr_mode;              //模式选择，手动或者联动
	AK_ISP_WDR manual_wdr;
	AK_ISP_WDR linkage_wdr[9];
}AK_ISP_WDR_ATTR;

typedef struct ak_isp_wdr_ex_attr {
	 unsigned short     hdr_blkW;
	 unsigned short     hdr_blkH;
	 unsigned short     hdr_reverseW_g;   	 //[0,511]
	 unsigned short     hdr_reverseW_shift;   //[0,15];
	 unsigned short     hdr_reverseH_g;       //[0,511]
	 unsigned short     hdr_reverseH_shift;   //[0,15]
	 unsigned short     hdr_weight_g;       	 //[0,511]
	 unsigned short     hdr_weight_shift;     //[0,15]
	 unsigned short     hdr_weight_k;    	 //[0,15]
}AK_ISP_WDR_EX_ATTR;

typedef struct ak_isp_sharp {

    unsigned short   mf_hpf_k;                  	//[0,127]
    unsigned short   mf_hpf_shift;             	//[0,15]

    unsigned short   hf_hpf_k;                 	//[0,127]
    unsigned short   hf_hpf_shift;            	//[0,15]

    unsigned short   sharp_method;            	//[0,3]
    unsigned short   sharp_skin_gain_weaken;  	//[0，3]

    unsigned short   sharp_skin_gain_th;      	//[0, 255]
    unsigned short   sharp_skin_detect_enable;
    unsigned short   ysharp_enable;          	//[0,1]
	unsigned short sharp_mf_th1;				//[0, 8191],default:300
	unsigned short sharp_mf_th2;				//[0, 8191],default:500
	unsigned short sharp_intensity1;		//[0, 15],default:15
	unsigned short sharp_intensity2;		//[0, 15],default:15
	//unsigned short sharp_intensity_slop;	sharp_intensity_slop = (sharp_intensity1-sharp_intensity2 )*1024/(sharp_mf_th2-sharp_mf_th1);
	unsigned short sharp_factor_th;	//[0, 255],default:32(GUI edit)
	unsigned short sharp_gain_slop;	//[0, 255],default:0(GUI edit)
    signed short   MF_HPF_LUT[256];        	//[-256,255]
    signed short   HF_HPF_LUT[256]; 			//[-256,255]
    unsigned short	MF_LUT_KEY[16];
	unsigned short	HF_LUT_KEY[16];
}AK_ISP_SHARP;

typedef struct ak_isp_sharp_attr{
    unsigned short          ysharp_mode;
    AK_ISP_SHARP   manual_sharp_attr;
    AK_ISP_SHARP   linkage_sharp_attr[9];
}AK_ISP_SHARP_ATTR;

typedef struct ak_isp_sharp_ex_attr {
    signed short  mf_HPF[6];            //
                                 //M13,M14,M15,
                                 //M11,M12,M14,
                                 //M10,M11,M13,
    signed short  hf_HPF[3];            //
                                 //M22 M21,M22,
                                 //M21,M20, M21,
}AK_ISP_SHARP_EX_ATTR;

typedef struct ak_isp_saturation {
    unsigned short    SE_enable;       // 使能位
    unsigned short    SE_th1;          //[0, 1023]
    unsigned short    SE_th2;          //[0, 1023]
	unsigned short	 SE_th3;	      //[0, 1023]
	unsigned short	 SE_th4;	      //[0, 1023]
	unsigned short	 SE_scale_slop1;  //[0, 255]
	unsigned short	 SE_scale_slop2;  //[0, 255]
	unsigned short	 SE_scale1; 	  //[0,255]
	unsigned short	 SE_scale2;	      //[0,255]
	unsigned short	 SE_scale3; 	  //[0,255]
}AK_ISP_SATURATION;

typedef struct  ak_isp_saturation_attr {
    unsigned short               SE_mode;        //饱和度模式
    AK_ISP_SATURATION   manual_sat;
    AK_ISP_SATURATION   linkage_sat[9];
}AK_ISP_SATURATION_ATTR;

typedef struct  ak_isp_contrast {
     unsigned short  y_contrast;  	//[0,511]
     signed short  y_shift;    	//[0, 511]
}AK_ISP_CONTRAST;

typedef struct ak_isp_auto_contrast
{
	unsigned short  dark_pixel_area; 	//[0, 511]
	unsigned short  dark_pixel_rate;  	//[1, 256]
	unsigned short  shift_max;    		//[0, 127]
}AK_ISP_AUTO_CONTRAST;

typedef struct  ak_isp_contrast_ATTR {
	unsigned short	cc_mode;	//模式选择，手动或者联动
	AK_ISP_CONTRAST	manual_contrast;
	AK_ISP_AUTO_CONTRAST	linkage_contrast[9];
}AK_ISP_CONTRAST_ATTR;

typedef  struct ak_isp_fcs {
    unsigned short  fcs_th;     			//[0, 255]
    unsigned short  fcs_gain_slop;  		//[0,63]
    unsigned short  fcs_enable;   		//使能位
    unsigned short  fcs_uv_nr_enable;  	//使能位
    unsigned short  fcs_uv_nr_th;  		//[0, 1023]
}AK_ISP_FCS;

typedef  struct ak_isp_fcs_attr {
    unsigned short       fcs_mode;   	//模式选择，手动或者联动
    AK_ISP_FCS  manual_fcs;
    AK_ISP_FCS  linkage_fcs[9];
}AK_ISP_FCS_ATTR;

typedef struct ak_isp_hue {
	unsigned short  hue_sat_en;			//hue使能
	signed char   hue_lut_a[65];		//[-128, 127]
	signed char   hue_lut_b[65];		//[-128, 127]
	unsigned char   hue_lut_s[65];		//[0, 255]
}AK_ISP_HUE;

typedef struct ak_isp_hue_attr {
	unsigned short        hue_mode;		//联动或者手动
	AK_ISP_HUE   manual_hue;
	AK_ISP_HUE   hue[4];		//四个联动参数
}AK_ISP_HUE_ATTR;

typedef  struct ak_isp_rgb2yuv_attr {
    unsigned short  mode; 				//bt601 或者bt709
}AK_ISP_RGB2YUV_ATTR;

typedef struct ak_isp_effect_attr {
    unsigned short  y_a;     // [0, 255]
    signed short  y_b;     //[-128, 127]
    signed short  uv_a;    //[-256, 255]
    signed short  uv_b;    //[-256, 255]
    unsigned short  dark_margin_en;   //黑边使能
}AK_ISP_EFFECT_ATTR;

typedef struct ak_isp_ddpc {
	unsigned short ddpc_enable;         //动态坏点使能位
	unsigned short ddpc_th;             //10bit
	unsigned short white_dpc_enable;    //白点消除使能位
	unsigned short black_dpc_enable;    //黑点消除使能位
}AK_ISP_DDPC;

typedef struct ak_isp_ddpc_attr {
	unsigned short ddpc_mode;            //模式选择，手动或者联动
	AK_ISP_DDPC manual_ddpc;
	AK_ISP_DDPC linkage_ddpc[9];
}AK_ISP_DDPC_ATTR;

typedef struct ak_isp_sdpc_attr {
    unsigned int   sdpc_enable; 		//静态坏点使能位

    /* 静态坏点坐标值，最大为1024个
    数据格式{6h0 ,y_position[9:0],5'h0,x_position[10:0]} */
    unsigned int   sdpc_table[1024];
}AK_ISP_SDPC_ATTR;

typedef struct ak_isp_af_attr {
	unsigned short  af_win0_left; 	//[0, 4095]
	unsigned short  af_win0_right;	//[0, 4095]
	unsigned short  af_win0_top;	    //[0, 4095]
	unsigned short  af_win0_bottom;   //[0, 4095]

	unsigned short  af_win1_left; 	//[0, 4095]
	unsigned short  af_win1_right;	//[0, 4095]
	unsigned short  af_win1_top;	    //[[0, 4095]
	unsigned short  af_win1_bottom;   //[0, 4095]

	unsigned short  af_win2_left; 	//[0, 4095]
	unsigned short  af_win2_right;	//[0, 4095]
	unsigned short  af_win2_top;	    //[0, 4095]
	unsigned short  af_win2_bottom;   //[0, 4095]

	unsigned short  af_win3_left; 	//[0, 4095]
	unsigned short  af_win3_right;	//[0, 4095]
	unsigned short  af_win3_top;	    //[0, 4095]
	unsigned short  af_win3_bottom;   //[0, 4095]

	unsigned short  af_win4_left; 	//[0, 4095]
	unsigned short  af_win4_right;	//[0, 4095]
	unsigned short  af_win4_top;	    //[0, 4095]
	unsigned short  af_win4_bottom;   //[0, 4095]

	unsigned short   af_th;       //[0, 128]
}AK_ISP_AF_ATTR;

typedef struct ak_isp_af_stat_info {
   unsigned int  af_statics[5];            //统计结果
}AK_ISP_AF_STAT_INFO;

typedef struct ak_isp_weight_attr {
   unsigned short   zone_weight[8][16];      //权重系数
}AK_ISP_WEIGHT_ATTR;

typedef  struct  ak_isp_wb_type_attr {
    unsigned short   wb_type;
}AK_ISP_WB_TYPE_ATTR;

typedef struct  ak_isp_awb_algo {
    short current_r_gain;
    short current_b_gain;
    short current_g_gain;
    short target_r_gain;
    short target_b_gain;
    short target_g_gain;
    short calc_r_gain;
    short calc_b_gain;
    short calc_g_gain;
    char awb_locked;
}AK_ISP_AWB_ALGO;

typedef  struct  ak_isp_mwb_attr {
    unsigned short  r_gain;
    unsigned short  g_gain;
    unsigned short  b_gain;
    signed short  r_offset;
    signed short  g_offset;
    signed short  b_offset;
}AK_ISP_MWB_ATTR;

typedef  struct ak_isp_awb_attr {
    unsigned short   g_weight[16];
    unsigned short   y_low;               	//y_low<=y_high
    unsigned short   y_high;
		unsigned short   err_est;
    unsigned short   gr_low[10];            //gr_low[i]<=gr_high[i]
    unsigned short   gb_low[10];            //gb_low[i]<=gb_high[i]
    unsigned short   gr_high[10];
    unsigned short   gb_high[10];
    unsigned short   rb_low[10];           //rb_low[i]<=rb_high[i]
    unsigned short   rb_high[10];

    //awb软件部分需要设置的参数
    unsigned short   auto_wb_step;                 //白平衡步长计算
    unsigned short   total_cnt_thresh;            //像素个数阈值
    unsigned short   colortemp_stable_cnt_thresh; //稳定帧数，多少帧一样认为环境色温改变
    unsigned short	colortemp_envi[10];
}AK_ISP_AWB_ATTR;

typedef  struct  ak_isp_awb_ctrl {
	int rgain_max;
	int rgain_min;
	int ggain_max;
	int ggain_min;
	int bgain_max;
	int bgain_min;
	int rgain_ex;
	int bgain_ex;
}AK_ISP_AWB_CTRL;

typedef  struct  ak_isp_awb_ex_attr {
	int awb_ex_ctrl_enable;
	AK_ISP_AWB_CTRL awb_ctrl[10];
}AK_ISP_AWB_EX_ATTR;

typedef  struct ak_isp_awb_stat_info {
    //白平衡统计结果
    unsigned int   total_R[10];
    unsigned int   total_G[10];
    unsigned int   total_B[10];
    unsigned int   total_cnt[10];
    //经由自动白平衡算法算出的白平衡增益值
    unsigned short   r_gain;
    unsigned short   g_gain;
    unsigned short   b_gain;
    signed short   r_offset;
    signed short   g_offset;
    signed short   b_offset;
    unsigned short   current_colortemp_index;     //环境色温标记，是参数随环境变化的色温指标。
    unsigned short   colortemp_stable_cnt[10];         //每一种色温稳定的帧数计数
}AK_ISP_AWB_STAT_INFO;

typedef struct ak_isp_wb_gain {
    unsigned short  r_gain;
    unsigned short  g_gain;
    unsigned short  b_gain;
    signed short  r_offset;
    signed short  g_offset;
    signed short  b_offset;
}AK_ISP_WB_GAIN;

typedef struct ak_isp_raw_hist_attr {
    unsigned short  enable;
}AK_ISP_RAW_HIST_ATTR;

typedef struct ak_isp_raw_hist_stat_info {
    unsigned int   raw_g_hist[256];
    unsigned int   raw_g_total;
}AK_ISP_RAW_HIST_STAT_INFO;

typedef struct ak_isp_rgb_hist_attr {
    unsigned short  enable;
}AK_ISP_RGB_HIST_ATTR;

typedef struct ak_isp_rgb_hist_stat_info {
    unsigned int   rgb_hist[256];
    unsigned int   rgb_total;
}AK_ISP_RGB_HIST_STAT_INFO;

typedef struct  ak_isp_yuv_hist_attr {
    unsigned short  enable;
}AK_ISP_YUV_HIST_ATTR;

typedef struct  ak_isp_yuv_hist_stat_info {
    unsigned int    y_hist[256];
    unsigned int    y_total;
}AK_ISP_YUV_HIST_STAT_INFO;

typedef  struct  ak_isp_exp_type {
    unsigned short  exp_type;
}AK_ISP_EXP_TYPE;

typedef struct ak_isp_manual_ae {
	unsigned int   exp_time;
	unsigned int   a_gain;
	unsigned int   d_gain;
	unsigned int   isp_d_gain;
}AK_ISP_ME_ATTR;

typedef struct ak_isp_ae_attr {
    unsigned int   exp_time_max;			//曝光时间的最大值
    unsigned int   exp_time_min;			//曝光时间的最小值
    unsigned int   d_gain_max;      		//数字增益的最大值
    unsigned int   d_gain_min;     		//数字增益的最小值
    unsigned int   isp_d_gain_min;  		//isp数字增益的最小
    unsigned int   isp_d_gain_max;  		//isp数字增益的最大值
    unsigned int   a_gain_max;     		//模拟增益的最大值
    unsigned int   a_gain_min;      		//模拟增益的最小值
    unsigned int   exp_step;            	//用户曝光调整步长
    unsigned int   exp_stable_range;     	//稳定范围
    unsigned int   target_lumiance[10];     	//目标亮度
    unsigned int   envi_gain_range[10][2];
    unsigned int   hist_weight[16];
    unsigned int		OE_suppress_en;
    unsigned int   OE_detect_scope;		//[0,255]
    unsigned int   OE_rate_max;			//[0, 255]
    unsigned int   OE_rate_min;			//[0, 255]
}AK_ISP_AE_ATTR;

typedef  struct ak_isp_ae_run_info {
    unsigned char   current_calc_avg_lumi;         		//现在的计算出的亮度值
    unsigned char   current_calc_avg_compensation_lumi; 	//经过曝光补偿后的亮度值

    unsigned char   current_target_lumi;                 //白天黑夜的标记

    int  current_a_gain;                     	//模拟增益的值
    int  current_d_gain;                    	//数字增益的值
    int  current_isp_d_gain;                	//isp数字增益的值
    int  current_exp_time;                	//曝光时间的值

    unsigned int  current_a_gain_step;             	//现在的模拟增益的步长
    unsigned int  current_d_gain_step;             	//数字增益的步长
    unsigned int  current_isp_d_gain_step;        		//isp数字增益的步长
    unsigned int  current_exp_time_step;          		//曝光时间的步长
}AK_ISP_AE_RUN_INFO;

typedef struct ak_isp_aec_algo {
    unsigned int  aec_status;       		//自动曝光控制的状态，稳定或者不稳定
    unsigned int  aec_locked;      		//自动曝光锁定与否

    unsigned char  exp_time_need_updata;   	//需要被更新，减少iic读写次数
    unsigned char  a_gain_need_updata;    	//需要被更新，减少iic读写次数
    unsigned char  d_gain_need_updata;       //需要被更新，只有需要更新时才写
    unsigned char  isp_d_gain_need_updata; 	//需要被更新
    unsigned char  lock_range;
}AK_ISP_AEC_ALGO;

//帧率控制结构体
typedef  struct ak_isp_frame_rate_attr {
	unsigned int  	hight_light_frame_rate ;
	unsigned int  	hight_light_max_exp_time ;
	unsigned int  	hight_light_to_low_light_gain;
	unsigned int  	low_light_frame_rate;
	unsigned int  	low_light_max_exp_time;
	unsigned int  	low_light_to_hight_light_gain;
}AK_ISP_FRAME_RATE_ATTR;

typedef struct ak_isp_misc_attr {
	unsigned short  hsyn_pol;
	unsigned short  vsync_pol;
	unsigned short  pclk_pol;
	unsigned short  test_pattern_en;
	unsigned short  test_pattern_cfg;
	unsigned short  cfa_mode;
	unsigned short  inputdataw;
	unsigned short  one_line_cycle;
	unsigned short  hblank_cycle;
	unsigned short  frame_start_delay_en;
	unsigned short  frame_start_delay_num;
	unsigned short  flip_en;
	unsigned short  mirror_en;
	unsigned short  twoframe_merge_en;//lz0499 9_12
	unsigned short  mipi_line_end_sel; //lz0499 9_21
	unsigned short  mipi_line_end_cnt_en_cfg; //lz0499 9_21
	unsigned short  mipi_count_time; //lz0499 9_21
} AK_ISP_MISC_ATTR;

typedef struct ak_isp_mask_area {
    unsigned short  start_xpos;
    unsigned short  end_xpos;
    unsigned short  start_ypos;
    unsigned short  end_ypos;
    unsigned char enable;
}AK_ISP_MASK_AREA;

typedef struct ak_isp_main_chan_mask_area_attr {
    AK_ISP_MASK_AREA  mask_area[4];
}AK_ISP_MAIN_CHAN_MASK_AREA_ATTR;

typedef struct ak_isp_sub_chan_mask_area_attr {
    AK_ISP_MASK_AREA  mask_area[4];
}AK_ISP_SUB_CHAN_MASK_AREA_ATTR;

typedef struct ak_isp_mask_color_attr {
    unsigned char  color_type;   //0:指定遮挡色；1：
    unsigned char  mk_alpha;
    unsigned char  y_mk_color;
    unsigned char  u_mk_color;
    unsigned char  v_mk_color;
}AK_ISP_MASK_COLOR_ATTR;

//enum for sub sample
typedef enum {
    SUBSMP_1X,  /*no sub sample*/
    SUBSMP_2X,  /*sub sample 1/2 * 1/2 */
    SUBSMP_4X,  /*sub sample 1/4 * 1/4 */
    SUBSMP_8X   /*sub sample 1/8 * 1/8 */
} T_SUBSMP_RTO;

typedef enum mask_num{
    MAIN_CHAN_ONE=0,
    MAIN_CHAN_TWO,
    MAIN_CHAN_THREE,
    MAIN_CHAN_FOURE,
    SUB_CHAN_ONE,
    SUB_CHAN_TWO,
    SUB_CHAN_THREE,
    SUB_CHAN_FOURE,
}MASK_NUM;

typedef enum osd_channel {
	OSD_CHN0 = 0,
	OSD_CHN1,
	OSD_CHN2,
	OSD_CHN_NUM
} OSD_CHANNEL;

typedef struct ak_isp_osd_color_table_attr {
	unsigned int color_table[16];
}AK_ISP_OSD_COLOR_TABLE_ATTR;

// lz0499 9_26 have some problem need to fix
typedef struct ak_isp_osd_context_attr {
	OSD_CHANNEL chn;
	unsigned int *osd_context_addr;
	unsigned int osd_width;
	unsigned int osd_height;
	unsigned short start_xpos;
	unsigned short start_ypos;
	unsigned short alpha;
	unsigned char enable;
}AK_ISP_OSD_CONTEXT_ATTR;

typedef struct ak_isp_osd_mem_attr {
	OSD_CHANNEL chn;
	unsigned char *dma_paddr;
	unsigned char *dma_vaddr;
	unsigned int size;
}AK_ISP_OSD_MEM_ATTR;

typedef enum ak_isp_pclk_polar {
	POLAR_ERR	= 0,
	POLAR_RISING,
	POLAR_FALLING,
}AK_ISP_PCLK_POLAR;

typedef struct ak_isp_ae_info
{
    int  a_gain;                     	//模拟增益的值
    int  d_gain;                    	//数字增益的值
    int  isp_d_gain;                	//isp数字增益的值
    int  exp_time;                	//曝光时间的值

}AK_ISP_AE_INFO;

typedef struct ak_isp_wb_gain_info {
	AK_ISP_WB_GAIN isp_wb_gain;
	char flg;

}ISP_WB_GAIN_INFO;

typedef struct ak_isp_ae_info_update
{
	AK_ISP_AE_INFO ae_info;
	char flg;

}AK_ISP_AE_INFO_UPDATE;

struct ae_fast_struct {
	int sensor_exp_time;
	int sensor_a_gain;
	int sensor_d_gain;
	int isp_d_gain;
	AK_ISP_WB_GAIN wb;
};

void  ak_isp_ae_info(void *isp_struct,AK_ISP_AE_INFO *isp_ae_info);
int ak_isp_vi_start_capturing(void *isp_struct,int yuv420_type);
//int ak_isp_vi_start_capturing(void *isp_struct);

int ak_isp_vi_capturing_one(void *isp_struct);
int ak_isp_vi_stop_capturing(void *isp_struct);
int ak_isp_vi_set_input_size(void *isp_struct,int width, int height);
int ak_isp_vi_set_crop(void *isp_struct,int sx, int sy, int width, int height);
int ak_isp_vi_get_crop(void *isp_struct,int *sx, int *sy, int *width, int *height);
int ak_isp_vi_apply_mode(void *isp_struct,enum isp_working_mode);
int ak_isp_vi_get_input_data_format(void *isp_struct, struct input_data_format *idf);
int ak_isp_set_misc_attr_ex(void *isp_struct, int oneline, int fsden, int hblank, int fsdnum);
int ak_isp_vo_set_misc_attr(void *isp_struct,AK_ISP_MISC_ATTR *p_misc);
int ak_isp_vo_get_misc_attr(void *isp_struct,AK_ISP_MISC_ATTR *p_misc);

int ak_isp_set_flip_mirror(void *isp_struct,int flip_en, int mirror_en);

int ak_isp_vo_set_main_channel_scale(void *isp_struct,int width, int height);
int ak_isp_vo_get_main_channel_scale(void *isp_struct,int *width, int *height);

int ak_isp_vo_set_sub_channel_scale(void *isp_struct,int width, int height);
int ak_isp_vo_get_sub_channel_scale(void *isp_struct,int *width, int *height);

int ak_isp_vo_enable_target_lines_done(void *isp_struct, int lines);
int ak_isp_vo_disable_target_lines_done(void *isp_struct);
int ak_isp_vo_check_update_status(void *isp_struct);
int ak_isp_vo_clear_update_status(void *isp_struct, int bit);
int ak_isp_vo_check_irq_status (void *isp_struct);
int ak_isp_vo_clear_irq_status(void *isp_struct,int bit);
int ak_isp_vo_enable_irq_status(void *isp_struct,int bit);
unsigned int ak_isp_vo_get_reg(void *isp_struct, int reg);
int ak_isp_vo_set_cfg_reg(void *isp_struct,int regaddr, int value, int bitmask);

int ak_isp_enable_buffer_main(void *isp_struct);
int ak_isp_enable_buffer_sub(void *isp_struct);
int ak_isp_enable_buffer_ch3(void *isp_struct);
int ak_isp_vo_enable_buffer(void *isp_struct,enum buffer_id id);
int ak_isp_vo_enable_buffer_main(void *isp_struct,enum buffer_id id);
int ak_isp_vo_enable_buffer_sub(void *isp_struct,enum buffer_id id);
int  ak_isp_vo_enable_buffer_ch3(void *isp_struct,enum buffer_id  id);

int ak_isp_vo_disable_buffer(void *isp_struct,enum buffer_id id);
int ak_isp_vo_disable_buffer_main(void *isp_struct,enum buffer_id id);
int ak_isp_vo_disable_buffer_sub(void *isp_struct,enum buffer_id id);
int ak_isp_vo_disable_buffer_ch3(void *isp_struct,enum buffer_id id);
int ak_isp_vo_set_buffer_addr(void *isp_struct,enum buffer_id id,
		unsigned long yaddr_main_chan_addr, unsigned long yaddr_sub_chan_addr,unsigned long yaddr_sub_chan3_addr);
int ak_isp_vo_set_main_buffer_addr \
	(void *isp_struct,
	 enum buffer_id  id,\
	 unsigned long yaddr_main_chan_addr);
int ak_isp_vo_set_sub_buffer_addr \
	(void *isp_struct,
	 enum buffer_id  id,\
	 unsigned long yaddr_sub_chan_addr);
int ak_isp_vo_set_ch3_buffer_addr \
	(void *isp_struct,
	 enum buffer_id  id,\
	 unsigned long yaddr_chan3_addr);
int ak_isp_vo_get_using_frame_buf_id(void *isp_struct);
int ak_isp_vo_get_using_frame_main_buf_id(void *isp_struct);
int ak_isp_vo_get_using_frame_sub_buf_id(void *isp_struct);
int ak_isp_vo_get_using_frame_ch3_buf_id(void *isp_struct);
int ak_isp_vo_get_using_frame_buf_hwid(void *isp_struct);
int isp_set_pp_frame_ctrl(void *isp_struct, int ch1_frame_ctrl, int ch2_frame_ctrl, int ch3_frame_ctrl);
int isp_set_pp_frame_ctrl_single(void *isp_struct, int ch_frame_ctrl, int index);
int ak_isp_vo_update_setting(void *isp_struct);
int ak_isp_is_continuous(void *isp_struct);

//int ak_isp_vpp_set_osd(AK_ISP_OSD_ATTR *p_osd);

//int ak_isp_vpp_set_occlusion_attr(AK_ISP_OCCLUSION_ATTR *p_occ);
//int ak_isp_vpp_occlusion_color_attr(AK_ISP_OCCLUSION_COLOR *p_occ_color);

/**
 * @brief: set blc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_blc:  blc param
 */
int ak_isp_vp_set_blc_attr(void *isp_struct,AK_ISP_BLC_ATTR *p_blc);

/**
 * @brief: get blc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_blc:  blc param
 */
int ak_isp_vp_get_blc_attr(void *isp_struct,AK_ISP_BLC_ATTR *p_blc);

/**
 * @brief: set lsc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_lsc:  lsc param
 */
int ak_isp_vp_set_lsc_attr(void *isp_struct,AK_ISP_LSC_ATTR *p_lsc);

/**
 * @brief: get lsc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_lsc:  lsc param
 */
int ak_isp_vp_get_lsc_attr(void *isp_struct,AK_ISP_LSC_ATTR *p_lsc);

/**
 * @brief: set rgb gamma param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_rgb_gamma:  rgb gamma param
 */
int ak_isp_vp_set_rgb_gamma_attr(void *isp_struct,AK_ISP_RGB_GAMMA_ATTR *p_rgb_gamma);

/**
 * @brief: get rgb gamma param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_rgb_gamma:  rgb gamma param
 */
int ak_isp_vp_get_rgb_gamma_attr(void *isp_struct,AK_ISP_RGB_GAMMA_ATTR *p_rgb_gamma);

/**
 * @brief: set y gamma param
 * @author: lz
 * @date: 2016-8-26
 * @param [in] *p_y_gamma:  y gamma param
 */
int ak_isp_vp_set_y_gamma_attr(void *isp_struct,AK_ISP_Y_GAMMA_ATTR *p_y_gamma);

/**
 * @brief: get y gamma param
 * @author: lz
 * @date: 2016-8-26
 * @param [out] *p_y_gamma:  y gamma param
 */
int ak_isp_vp_get_y_gamma_attr(void *isp_struct,AK_ISP_Y_GAMMA_ATTR *p_y_gamma);

/**
 * @brief: set raw gamma param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_raw_lut:  raw gamma param
 */
int ak_isp_vp_set_raw_lut_attr(void *isp_struct,AK_ISP_RAW_LUT_ATTR *p_raw_lut);

/**
 * @brief: get raw gamma param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_raw_lut:  raw gamma param
 */
int ak_isp_vp_get_raw_lut_attr(void *isp_struct,AK_ISP_RAW_LUT_ATTR *p_raw_lut);

/**
 * @brief: set dpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_dpc:  dpc param
 */
int ak_isp_vp_set_dpc_attr(void *isp_struct,AK_ISP_DDPC_ATTR *p_dpc);

/**
 * @brief: get dpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_dpc:  dpc param
 */
int ak_isp_vp_get_dpc_attr(void *isp_struct,AK_ISP_DDPC_ATTR *p_dpc);

/**
 * @brief: set sdpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_sdpc:  sdpc param
 */
int ak_isp_vp_set_sdpc_attr(void *isp_struct,AK_ISP_SDPC_ATTR *p_sdpc);

/**
 * @brief: get sdpc param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_sdpc:  sdpc param
 */
int ak_isp_vp_get_sdpc_attr(void *isp_struct,AK_ISP_SDPC_ATTR *p_sdpc);

/**
 * @brief: set raw noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *nr1:  raw noise remove  param
 */
int ak_isp_vp_set_nr1_attr(void *isp_struct,AK_ISP_NR1_ATTR *p_nr1);

/**
 * @brief: get raw noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *nr1:  raw noise remove  param
 */
int ak_isp_vp_get_nr1_attr(void *isp_struct,AK_ISP_NR1_ATTR *p_nr1);

/**
 * @brief: set green balance param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_gb:  green balance param
 */
int ak_isp_vp_set_gb_attr(void *isp_struct,AK_ISP_GB_ATTR *p_gb);

/**
 * @brief: get green balance param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_gb:  green balance param
 */
int ak_isp_vp_get_gb_attr(void *isp_struct,AK_ISP_GB_ATTR *p_gb);

/**
 * @brief: set demosaic param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_demo:  demosaic param
 */
int ak_isp_vp_set_demo_attr(void *isp_struct,AK_ISP_DEMO_ATTR *p_demo);

/**
 * @brief: get demosaic param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_demo:  demosaic param
 */
int ak_isp_vp_get_demo_attr(void *isp_struct,AK_ISP_DEMO_ATTR *p_demo);

/**
 * @brief: set color correct param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_ccm:  color correct param
 */
int ak_isp_vp_set_ccm_attr(void *isp_struct,AK_ISP_CCM_ATTR *p_ccm);

/**
 * @brief: set color correct param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_ccm:  color correct param
 */
int ak_isp_vp_get_ccm_attr(void *isp_struct,AK_ISP_CCM_ATTR *p_ccm);

/**
 * @brief: set wdr  param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_wdr: wdr param
 */
int ak_isp_vp_set_wdr_attr(void *isp_struct,AK_ISP_WDR_ATTR *p_wdr);

/**
 * @brief: get wdr  param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_wdr: wdr param
 */
int ak_isp_vp_get_wdr_attr(void *isp_struct,AK_ISP_WDR_ATTR *p_wdr);

/**
 * @brief: set yuv noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_nr2: noise remove param
 */
int ak_isp_vp_set_nr2_attr(void *isp_struct,AK_ISP_NR2_ATTR *p_nr2);

/**
 * @brief: get yuv noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_nr2: noise remove param
 */
int ak_isp_vp_get_nr2_attr(void *isp_struct,AK_ISP_NR2_ATTR *p_nr2);

/**
 * @brief: set yuv noise remove param
 * @author: wyf
 * @date: 2020-1-06
 * @param [in] *p_uvnr: noise remove param
 */
int ak_isp_vp_set_uvnr_attr(void *isp_struct,AK_ISP_UVNR_ATTR *p_uvnr);

/**
 * @brief: get yuv noise remove param
 * @author: wyf
 * @date: 2020-1-06
 * @param [out] *p_uvnr: noise remove param
 */
int ak_isp_vp_get_uvnr_attr(void *isp_struct,AK_ISP_UVNR_ATTR *p_uvnr);

/**
 * @brief: set 3d noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_3d_nr: 3d noise remove param
 */
int ak_isp_vp_set_3d_nr_attr(void *isp_struct,AK_ISP_3D_NR_ATTR *p_3d_nr);

/**
 * @brief: get 3d noise remove param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_3d_nr: 3d noise remove param
 */
int ak_isp_vp_get_3d_nr_attr(void *isp_struct,AK_ISP_3D_NR_ATTR *p_3d_nr);

/**
 * @brief: set 3d noise remove reference param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_ref: 3d noise remove referenc param
 */
int ak_isp_vp_set_3d_nr_ref_addr(void *isp_struct,AK_ISP_3D_NR_REF_ATTR *p_ref);

/**
 * @brief: get 3d noise remove reference param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_ref: 3d noise remove referenc param
 */
int ak_isp_vp_get_3d_nr_ref_addr(void *isp_struct,AK_ISP_3D_NR_REF_ATTR *p_ref);

/**
 * @brief: get 3d noise remove statics param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_3d_nr_stat_info: 3d noise remove statics param
 */
int ak_isp_vp_get_3d_nr_stat_info(void *isp_struct,AK_ISP_3D_NR_STAT_INFO * p_3d_nr_stat_info);

/**
 * @brief: set sharp param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_sharp: sharp param
 */
int ak_isp_vp_set_sharp_attr(void *isp_struct,AK_ISP_SHARP_ATTR *p_sharp);

/**
 * @brief: get sharp param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_sharp: sharp param
 */
int ak_isp_vp_get_sharp_attr(void *isp_struct,AK_ISP_SHARP_ATTR* p_sharp);

/**
 * @brief: set sharp other param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_sharp_ex: sharp other param
 */
int ak_isp_vp_set_sharp_ex_attr(void *isp_struct,AK_ISP_SHARP_EX_ATTR *p_sharp_ex);

/**
 * @brief: get sharp other param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_sharp_ex: sharp other param
 */
int ak_isp_vp_get_sharp_ex_attr(void *isp_struct,AK_ISP_SHARP_EX_ATTR* p_sharp_ex);

/**
 * @brief: set false color param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *fcs: false color param
 */
int ak_isp_vp_set_fcs_attr(void *isp_struct,AK_ISP_FCS_ATTR *p_fcs);

/**
 * @brief: get false color param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *fcs: false color param
 */
int ak_isp_vp_get_fcs_attr(void *isp_struct,AK_ISP_FCS_ATTR *p_fcs);

/**
 * @brief: set hue param
 * @author: lz
 * @date: 2016-8-26
 * @param [in] *p_hue:hue param
 */
int ak_isp_vp_set_hue_attr(void *isp_struct,AK_ISP_HUE_ATTR *p_hue);

/**
 * @brief: gethue param
 * @author: lz
 * @date: 2016-8-26
 * @param [in] *p_hue:hue param
 */
int ak_isp_vp_get_hue_attr(void *isp_struct,AK_ISP_HUE_ATTR *p_hue);

/**
 * @brief: set satruration param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_sat: satruration param
 */
int ak_isp_vp_set_saturation_attr(void *isp_struct,AK_ISP_SATURATION_ATTR *p_sat);

/**
 * @brief: get satruration param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_sat: satruration param
 */
int ak_isp_vp_get_saturation_attr(void *isp_struct,AK_ISP_SATURATION_ATTR *p_sat);

int ak_isp_vp_set_contrast_attr(void *isp_struct,AK_ISP_CONTRAST_ATTR  *p_contrast);
int ak_isp_vp_get_contrast_attr(void *isp_struct,AK_ISP_CONTRAST_ATTR  *p_contrast);

int ak_isp_vp_set_rgb2yuv_attr(void *isp_struct,AK_ISP_RGB2YUV_ATTR*p_rgb2yuv_attr);
int ak_isp_vp_get_rgb2yuv_attr(void *isp_struct,AK_ISP_RGB2YUV_ATTR*p_rgb2yuv_attr);
int ak_isp_vp_set_effect_attr(void *isp_struct,AK_ISP_EFFECT_ATTR *p_effect_attr);
int ak_isp_vp_get_effect_attr(void *isp_struct,AK_ISP_EFFECT_ATTR *p_effect_attr);

int ak_isp_vpp_set_mask_color(void *isp_struct,AK_ISP_MASK_COLOR_ATTR *p_mask);
int ak_isp_vpp_get_mask_color(void *isp_struct,AK_ISP_MASK_COLOR_ATTR *p_mask);
int ak_isp_vpp_set_main_chan_mask_area_attr(void *isp_struct,AK_ISP_MAIN_CHAN_MASK_AREA_ATTR *p_mask);
int ak_isp_vpp_get_main_chan_mask_area_attr(void *isp_struct,AK_ISP_MAIN_CHAN_MASK_AREA_ATTR *p_mask);
int ak_isp_vpp_set_sub_chan_mask_area_attr(void *isp_struct,AK_ISP_SUB_CHAN_MASK_AREA_ATTR *p_mask);
int ak_isp_vpp_get_sub_chan_mask_area_attr(void *isp_struct,AK_ISP_SUB_CHAN_MASK_AREA_ATTR *p_mask);
int ak_isp_vpp_set_osd_color_table_attr(void *isp_struct,AK_ISP_OSD_COLOR_TABLE_ATTR *p_isp_color_table);
int ak_isp_vpp_set_main_channel_osd_context_attr(void *isp_struct,AK_ISP_OSD_CONTEXT_ATTR *p_context);
int ak_isp_vpp_set_sub_channel_osd_context_attr(void *isp_struct,AK_ISP_OSD_CONTEXT_ATTR *p_context);
int ak_isp_vpp_set_main_channel_osd_mem_attr(void *isp_struct,AK_ISP_OSD_MEM_ATTR *p_mem);
int ak_isp_vpp_set_sub_channel_osd_mem_attr(void *isp_struct,AK_ISP_OSD_MEM_ATTR *p_mem);

/**
 * @brief: set auto focus param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_af:  af  param
 */
int ak_isp_vp_set_af_attr(void *isp_struct,AK_ISP_AF_ATTR *p_af);

/**
 * @brief: get auto focus param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_af:  af  param
 */
int ak_isp_vp_get_af_attr(void *isp_struct,AK_ISP_AF_ATTR *p_af);

/**
 * @brief: get auto focus statics info param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_af_stat_info:  af_stat  param
 */
int ak_isp_vp_get_af_stat_info(void *isp_struct,AK_ISP_AF_STAT_INFO *p_af_stat_info);

/**
 * @brief: set white balance  type
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_type:  white balance  type  param
 */
int ak_isp_vp_set_wb_type(void *isp_struct,AK_ISP_WB_TYPE_ATTR *p_type);
/**
 * @brief: get white balance  type
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_type:  white balance  type  param
 */
int ak_isp_vp_get_wb_type(void *isp_struct,AK_ISP_WB_TYPE_ATTR *p_type);

/**
 * @brief: set manual white balance
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_mwb:  manual white balance  type  param
 */
int ak_isp_vp_set_mwb_attr(void *isp_struct,AK_ISP_MWB_ATTR *p_mwb);

/**
 * @brief: get manual white balance
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_mwb:  manual white balance  type  param
 */
int ak_isp_vp_get_mwb_attr(void *isp_struct,AK_ISP_MWB_ATTR *p_mwb);

/**
 * @brief: set auto white balance
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_awb:  auto white balance  param
 */
int ak_isp_vp_set_awb_attr(void *isp_struct,AK_ISP_AWB_ATTR *p_awb);

/**
 * @brief: get auto white balance
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_awb:  auto white balance  param
 */
int ak_isp_vp_get_awb_attr(void *isp_struct,AK_ISP_AWB_ATTR *p_awb);

/**
 * @brief: set auto white balance ex attr
 * @author: wyf
 * @date: 2016-5-16
 * @param [in] *p_awb:  auto white balance  param
 */
int ak_isp_vp_set_awb_ex_attr(void *isp_struct,AK_ISP_AWB_EX_ATTR *p_awb);

/**
 * @brief: get auto white balance ex attr
 * @author: wyf
 * @date: 2016-5-16
 * @param [in] *p_awb:  auto white balance  param
 */
int ak_isp_vp_get_awb_ex_attr(void *isp_struct,AK_ISP_AWB_EX_ATTR *p_awb);

/**
 * @brief: get awb statics info
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_awb_stat_info:  awb statics info  param
 */
int ak_isp_vp_get_awb_stat_info(void *isp_struct,AK_ISP_AWB_STAT_INFO *p_awb_stat_info);

/**
 * @brief: set exp  type
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_exp_type:  exp type  param
 */
int ak_isp_vp_set_exp_type(void *isp_struct,AK_ISP_EXP_TYPE* p_exp_type);

/**
 * @brief: get exp  type
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_exp_type:  exp type  param
 */
int ak_isp_vp_get_exp_type( void *isp_struct,AK_ISP_EXP_TYPE* p_exp_type);

/**
 * @brief: set auto exposure  param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_ae:  auto exposure  param
 */
int ak_isp_vp_set_ae_attr( void *isp_struct,AK_ISP_AE_ATTR *p_ae);

/**
 * @brief: get auto exposure  param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_ae:  auto exposure  param
 */
int ak_isp_vp_get_ae_attr(void *isp_struct,AK_ISP_AE_ATTR *p_ae);

/**
 * @brief: set manual exposure  param
 * @author: lizhi
 * @date: 2020-07-08
 * @param [in] *p_mae:  manual exposure  param
 */
int ak_isp_vp_set_me_attr(void *isp_struct,AK_ISP_ME_ATTR *p_me);

/**
 * @brief: get manual exposure  param
 * @author: lizhi
 * @date: 2020-06-01
 * @param [in] *p_mae:  manual exposure  param
 */
int  ak_isp_vp_get_me_attr(void *isp_struct,AK_ISP_ME_ATTR *p_me);

/**
 * @brief: set frame rate param
 * @author: lz
 * @date: 2016-8-29
 * @param [in] *p_frame_rate:  frame rate param
 */
int ak_isp_vp_set_frame_rate(void *isp_struct, AK_ISP_FRAME_RATE_ATTR*p_frame_rate);

/**
 * @brief: set frame rate param
 * @author: lz
 * @date: 2016-8-29
 * @param [in] *p_frame_rate:  frame rate param
 */
int ak_isp_vp_get_frame_rate(void *isp_struct,AK_ISP_FRAME_RATE_ATTR*p_frame_rate);

/**
 * @brief: get auto  exposure  running info
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_ae_stat:  auto  exposure  running info  param
 */
int ak_isp_vp_get_ae_run_info(void *isp_struct,AK_ISP_AE_RUN_INFO*p_ae_stat);

/**
 * @brief: get raw hist   running info
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_raw_hist_stat:  raw hist info  param
 */
int ak_isp_vp_get_raw_hist_stat_info(void *isp_struct,AK_ISP_RAW_HIST_STAT_INFO *p_raw_hist_stat);

/**
 * @brief: get rgb hist   running info
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_rgb_hist_stat:  rgb hist info  param
 */
int ak_isp_vp_get_rgb_hist_stat_info(void *isp_struct,AK_ISP_RGB_HIST_STAT_INFO *p_rgb_hist_stat);

/**
 * @brief: get rgb hist   running info
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [out] *p_yuv_hist_stat:  yuv hist info  param
 */
int ak_isp_vp_get_yuv_hist_stat_info(void *isp_struct,AK_ISP_YUV_HIST_STAT_INFO *p_yuv_hist_stat);

int ak_isp_vp_set_rgb_hist_attr(void *isp_struct,AK_ISP_RGB_HIST_ATTR *p_rgb_hist);
int ak_isp_vp_get_rgb_hist_attr(void *isp_struct,AK_ISP_RGB_HIST_ATTR *p_rgb_hist);

//int ak_isp_vp_set_yuv_hist_attr(AK_ISP_YUV_HIST_ATTR *p_yuv_hist);
//int ak_isp_vp_get_yuv_hist_attr(AK_ISP_YUV_HIST_ATTR *p_yuv_hist);
int ak_isp_vp_set_yuv_hist_attr(void *isp_struct,AK_ISP_YUV_HIST_ATTR *p_yuv_hist);
int ak_isp_vp_get_yuv_hist_attr(void *isp_struct,AK_ISP_YUV_HIST_ATTR *p_yuv_hist);

/**
 * @brief: set zone weight param
 * @author: xiepenghe
 * @date: 2016-5-06
 * @param [in] *p_weight:  weight  param
 */
int ak_isp_vp_set_zone_weight(void *isp_struct,AK_ISP_WEIGHT_ATTR *p_weight);
int ak_isp_vp_get_zone_weight(void *isp_struct, AK_ISP_WEIGHT_ATTR *p_weight);
int ak_isp_vp_set_raw_hist_attr(void *isp_struct,AK_ISP_RAW_HIST_ATTR *p_raw_hist);
int ak_isp_vp_get_raw_hist_attr(void *isp_struct,AK_ISP_RAW_HIST_ATTR *p_raw_hist);

/* Pclk_Polar not isp function, but described in PG */
AK_ISP_PCLK_POLAR ak_isp_get_pclk_polar(void *isp_struct);
int ak_isp_get_bits_width(void *isp_struct);

int ak_isp_set_isp_capturing(void *isp_struct,int resume);

int ak_isp_irq_work(void *isp_struct);
int ak_isp_ae_work(void *isp_struct);
int ak_isp_awb_work(void *isp_struct);

int ak_isp_fast_ae_work(void *isp_struct,const int (*p)[2]);
int ak_isp_fast_awb_work(void *isp_struct);
int ak_isp_set_awb_work_suspend(void *isp_struct, int ae_suspend_flag);

typedef void (*ISPDRV_CB_PRINTK)(char * format, ...);
typedef void (*ISPDRV_CB_MEMCPY)(void *dst, void *src, unsigned long sz);
typedef void (*ISPDRV_CB_MEMSET)(void *ptr, unsigned char value, unsigned long sz);
typedef void* (*ISPDRV_CB_MALLOC)(unsigned long sz);
typedef void (*ISPDRV_CB_FREE)(void *ptr);
typedef void* (*ISPDRV_CB_DMAMALLOC)(unsigned long sz, void *handle);
typedef void (*ISPDRV_CB_DMAFREE)(void *ptr, unsigned long sz, unsigned long handle);
typedef void (*ISPDRV_CB_MSLEEP)(int ms);
typedef void (*ISPDRV_CB_CACHE_INVALID)(void);
typedef unsigned long (*ISPDRV_CB_WORD_READ)(void *addr);
typedef void (*ISPDRV_CB_WORD_WRITE)(unsigned long value, void *addr);

typedef struct {
    ISPDRV_CB_PRINTK cb_printk;
    ISPDRV_CB_MEMCPY cb_memcpy;
    ISPDRV_CB_MEMSET cb_memset;
    ISPDRV_CB_MALLOC cb_malloc;
    ISPDRV_CB_FREE   cb_free;
    ISPDRV_CB_DMAMALLOC cb_dmamalloc;
    ISPDRV_CB_DMAFREE cb_dmafree;
    ISPDRV_CB_MSLEEP cb_msleep;
	ISPDRV_CB_CACHE_INVALID cb_cache_invalid;
	ISPDRV_CB_WORD_READ cb_word_read;
	ISPDRV_CB_WORD_WRITE cb_word_write;
}AK_ISP_FUNC_CB;

typedef struct sensor_reg_info {
    unsigned short reg_addr;
    unsigned short value;
}AK_ISP_SENSOR_REG_INFO;

typedef struct sensor_init_para {
    unsigned short num;
    AK_ISP_SENSOR_REG_INFO *reg_info;
}AK_ISP_SENSOR_INIT_PARA;

enum sensor_bus_type {
	BUS_TYPE_RAW,
	BUS_TYPE_YUV,
	BUS_TYPE_NUM
};

enum scene {
	SCENE_INDOOR = 0,
	SCENE_OUTDOOR
};

typedef struct sensor_callback {
	int (*sensor_init_func)(void *arg, const AK_ISP_SENSOR_INIT_PARA *para);
	int (*sensor_read_reg_func)(void *arg, const int reg_addr);
	int (*sensor_write_reg_func)(void *arg, const int reg_addr, int value);
	int (*sensor_read_id_func)(void *arg);	//no use IIC bus
	int (*sensor_update_a_gain_func)(void *arg, const unsigned int a_gain);
	int (*sensor_update_d_gain_func)(void *arg, const unsigned int d_gain);
	int (*sensor_updata_exp_time_func)(void *arg, unsigned int exp_time);
	int (*sensor_timer_func)(void *arg);
	int (*sensor_set_standby_in_func)(void *arg);
	int (*sensor_set_standby_out_func)(void *arg);
#if 0
	int (*sensor_init_func)(const int id, const AK_ISP_SENSOR_INIT_PARA *para);
	int (*sensor_read_reg_func)(const int id, const int reg_addr);
	int (*sensor_write_reg_func)(const int id, const int reg_addr, int value);
	int (*sensor_read_id_func)(const int id);	//no use IIC bus
	int (*sensor_update_a_gain_func)(const int id, const unsigned int a_gain);
	int (*sensor_update_d_gain_func)(const int id, const unsigned int d_gain);
	int (*sensor_updata_exp_time_func)(const int id, unsigned int exp_time);
	int (*sensor_timer_func)(const int id);
	int (*sensor_set_standby_in_func)(void);
	int (*sensor_set_standby_out_func)(void);
#endif

	int (*sensor_probe_id_func)(void *arg);	//use IIC bus
	int (*sensor_get_resolution_func)(void *arg, int *width, int *height);
	int (*sensor_get_mclk_func)(void *arg);
	int (*sensor_get_fps_func)(void *arg);
	int (*sensor_get_valid_coordinate_func)(void *arg, int *x, int *y);
	enum sensor_bus_type (*sensor_get_bus_type_func)(void *arg);
	int (*sensor_get_parameter_func)(void *arg, int param, void *value);

	int (*sensor_set_power_on_func)(void *arg);
	int (*sensor_set_power_off_func)(void *arg);
	int (*sensor_set_fps_func)(void *arg, const int fps);
	//int (*sensor_set_standby_in_func)(const int pwdn_pin, const int reset_pin);
	//int (*sensor_set_standby_out_func)(const int pwdn_pin, const int reset_pin);
    int (*sensor_get_fast_para_func)(void *arg, AK_ISP_AE_INFO *para);
}AK_ISP_SENSOR_CB;

struct sensor_cb_info {
	AK_ISP_SENSOR_CB *cb;
	void *arg;
};

void *isp_struct2_module_switch_sensor(void *isp);
int isp2_module_curr_sensor(void);

int isp2_module_init(AK_ISP_FUNC_CB *cb, struct sensor_cb_info *sensor_cb,
		void *reg_base, void **isp_struct, int isp_id);
void isp2_module_fini(void *isp_struct);

void  isp2_print_reg_table(void *isp_struct);

int ak_isp_register_sensor(void *sensor_info);
void *ak_isp_get_sensor(int *index);
void ak_isp_remove_all_sensors(void);
int ak_isp_set_td(void *isp_struct);
int ak_isp_reload_td(void *isp_struct);

int ak_isp_sensor_cb_init(void *isp_struct, AK_ISP_SENSOR_INIT_PARA *sensor_para);

enum scene ak_isp_get_scene(void *isp_struct);

int ak_isp_vpp_mainchn_osdmem_useok(void *isp_struct);

int ak_isp_vpp_subchn_osdmem_useok(void *isp_struct);

int ak_isp_get_mdinfo(void *isp_struct, AK_ISP_3D_NR_STAT_INFO **_3d_nr_stat_para,
				int *width_block_num, int *height_block_num, int *block_size);

int ak_isp_get_md_array_max_size(void *isp_struct,
				int *width_block_num, int *height_block_num, int *block_size);

int _dual_3dnr_work(void *isp_struct);

int ak_isp_set_ae_fast_struct_default(void *isp_struct, struct ae_fast_struct *ae_fast);
int ak_isp_set_ae_fast_struct(void *isp_struct, struct ae_fast_struct *ae_fast);
int ak_isp_ae_fast_set_ae(void *isp_struct);
int ak_isp_ae_fast_set_isp_d_gain(void *isp_struct);
int ak_isp_ae_fast_set_wb(void *isp_struct);
int ak_isp_set_ae_work_suspend(void *isp_struct, int ae_suspend_flag);
int ak_isp_vi_start_capturing_one(void *isp_struct);
const char *ak_isp_get_version(void *isp_struct);
#endif
