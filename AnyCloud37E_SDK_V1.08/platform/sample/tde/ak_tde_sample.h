#define LEN_HINT                    512
#define LEN_PATH                    1024
#define LEN_DATA                    256
#define LEN_OPTION_SHORT            512

#define DEFAULT_WIDTH               1024
#define DEFAULT_HEIGHT              600
#define DEFAULT_POS_TOP             0
#define DEFAULT_POS_LEFT            0
#define DEFAULT_POS_WIDTH           1024
#define DEFAULT_POS_HEIGHT          600
#define DEFAULT_FORMAT              TDE_FORMAT_RGB888
#define DEFAULT_FILE_SRC            "./ak_tde_s_test.rgb"
#define DEFAULT_FILE_BG             "./ak_tde_bg_test.rgb"
#define DEFAULT_DIR_YUV             "./yuv"
#define DEFAULT_NUM_BUFFYUV         10

#define LEN_OFFSET_V                4                                           //yuv的u值偏移量 y=0 u=h*w v=h*w+h*w/4
#define YUV_TEST_PRINT              100
#define DEV_GUI                     "/dev/fb0"                                  //gui lcd使用的设备名称
#define SIZE_ERROR                  0xffffffff
#define USEC2SEC                    1000000

#define BITS_PER_PIXEL              24
#define BITS_PER_PIXEL565           16
#define LEN_COLOR                   8
#define DUAL_FB_FIX                 fb_fix_screeninfo_param.reserved[ 0 ]
#define DUAL_FB_VAR                 fb_var_screeninfo_param.reserved[ 0 ]

#ifndef AK_SUCCESS
#define AK_SUCCESS 0
#endif

#ifndef AK_FAILED
#define AK_FAILED -1
#endif

enum color_offset {
	OFFSET_RED = 16,
	OFFSET_GREEN = 8,
	OFFSET_BLUE = 0,
	OFFSET565_RED = 11,
	OFFSET565_GREEN = 5,
	OFFSET565_BLUE = 0,
};

struct regloop {
	//regex_t regex_t_use ;
	char *pc_pattern;
	int i_flags;
	int i_offset ;
	int i_times;
} ;

int test_yuv( char *pc_pach );
static int test_tde( void );

/*
	help_hint: 根据option_long和ac_option_hint数组打印帮助信息
	return: 0
*/
static int help_hint(void);

/*
	get_option_short: 根据option_long填充短选项字符串
	@p_option[IN]: struct option数组地址
	@i_num_option[IN]: 数组元素个数
	@pc_option_short[IN]: 填充的数组地址
	@i_len_option[IN]: 数组长度
	return: pc_option_short
*/
static char *get_option_short( struct option *p_option, int i_num_option, char *pc_option_short, int i_len_option );

/*
	parse_option: 对传入的选项进行解释
	@argc[IN]: 从程序入口传入的选项数量
	@argv[IN]: 选项内容的字符串指针数组
	return: AK_TRUE:继续运行  AK_FALSE:退出应用,在打印帮助或者选项解释错误的时候使用
*/
static int parse_option( int argc, char **argv );

/*
	get_file_size: 获取文件长度
	return: 成功:AK_SUCCESS 失败:AK_FAILED
*/
static long long int get_file_size( char *pc_filename );

/*
	ak_gui_open: 打开lcd屏幕设备
	@p_fb_fix_screeninfo[OUT]:返回屏幕的只读信息
	@p_fb_var_screeninfo[OUT]:返回屏幕的可配置信息
	@pv_addr[OUT]:显存的地址
	return: 成功:AK_SUCCESS 失败:AK_FAILED
*/
static int ak_gui_open( struct fb_fix_screeninfo *p_fb_fix_screeninfo, struct fb_var_screeninfo *p_fb_var_screeninfo, void **pv_addr );

/*
	ak_gui_close: 关闭lcd屏幕设备
	return: 成功:AK_SUCCESS 失败:AK_FAILED
*/
static int ak_gui_close( void );

/*
	lcd_show_fix: 打印lcd屏幕的只读参数信息
	@fix[IN]:struct fb_fix_screeninfo指针
	return: NULL
*/
void lcd_show_fix(struct fb_fix_screeninfo *fix);

/*
	lcd_show_var: 打印lcd屏幕的可修改的参数信息
	@var[IN]:struct fb_var_screeninfo指针
	return: NULL
*/
void lcd_show_var(struct fb_var_screeninfo *var);

/*
	dump_file: 将显存镜像内容保存成文件
	@pc_file[IN]:文件名称
	@pv_addr[IN]:显存地址
	@i_size[IN]:长度
	return: 写入文件长度
*/
int dump_file( char *pc_file, void *pv_addr, int i_size );

/*
	init_regloop: 初始化一个正则表达式的循环读取结构体
	@p_regloop[IN]:循环结构体
	@pc_pattern[IN]:正则表达式
	@i_flags[IN]:标志
	return: 成功:AK_SUCCESS 失败:AK_FAILED
*/
int init_regloop( struct regloop *p_regloop , char *pc_pattern , int i_flags );

/*
	free_regloop: 释放一个正则表达式的循环读取结构体的正则指针
	@p_regloop[IN]:循环结构体
	return: 0
*/
int free_regloop( struct regloop *p_regloop );

/*
	reset_regloop: 重置一个正则循环结构体
	@p_regloop[IN]:循环结构体
	return: 0
*/
int reset_regloop( struct regloop *p_regloop );

/*
	match_regloop: 重置一个正则循环结构体
	@p_regloop[IN]:循环结构体
	@pc_buff[IN]:输入的字符串
	@pc_res[OUT]:每次获取的正则匹配结果
	@i_len_res[IN]:匹配结果指针的缓冲区长度
	return: 正则匹配结果长度
*/
int match_regloop( struct regloop *p_regloop , char *pc_buff , char *pc_res , int i_len_res );
