#!/bin/sh
sample=ak_tde_sample
pic_src=./anyka.logo.160.577.rgb                                                #源图文件名
pic_bg=./desert.800.1280.rgb                                                    #背景图文件名
format="--format-s 3 --format-bg 3 --format-out 1"                              #源图 背景图 屏幕格式
file="--file-s $pic_src --file-bg $pic_bg"                                      #文件名称
rect_bg="--rect-bg 800,1280,0,0,800,1280"                                       #背景图坐标

rect_s="--rect-s 160,577,0,0,160,577"                                           #源图坐标
rect_t="--rect-t 320,352,160,577"                                               #贴图到屏幕坐标,屏幕宽高由sample获取
rect_t_rotate="--rect-t 112,560,577,160"                                        #旋转贴图到屏幕坐标
rect_t_scale="--rect-t 250,250,300,800"                                         #拉伸贴图到屏幕坐标

#rect_s="--rect-s 160,577,0,0,80,300"                                           #源图坐标
#rect_t="--rect-t 320,352,160,577"                                              #贴图到屏幕坐标,屏幕宽高由sample获取
#rect_t_rotate="--rect-t 112,560,300,80"                                        #旋转贴图到屏幕坐标
#rect_t_scale="--rect-t 250,250,300,800"                                        #拉伸贴图到屏幕坐标

$sample --opt-blit $file $rect_s $rect_bg $rect_t $format --reset-screen --opt-colorkey FFFF00,FFFFFF,1
$sample --opt-blit $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,0
while :
do
	#echo "拷贝";sleep 1
	$sample --opt-blit $file $rect_s $rect_bg $rect_t $format
	#echo "拷贝|透明度";sleep 1
	$sample --opt-blit $file $rect_s $rect_bg $rect_t $format --opt-transparent 7
	#echo "拷贝|颜色过滤";sleep 1
	$sample --opt-blit $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,0
	#echo "拷贝|颜色过滤";sleep 1
	$sample --opt-blit $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,1
	#echo "拷贝|颜色过滤|透明度";sleep 1
	$sample --opt-blit $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,0 --opt-transparent 7
	#echo "拷贝|颜色过滤|透明度";sleep 1
	$sample --opt-blit $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,1 --opt-transparent 7

	#echo "旋转90"
	$sample --opt-rotate 1 $file $rect_s $rect_bg $rect_t_rotate $format
	#echo "旋转90|透明度"
	$sample --opt-rotate 1 $file $rect_s $rect_bg $rect_t_rotate $format --opt-transparent 7
	#echo "旋转90|颜色过滤"
	$sample --opt-rotate 1 $file $rect_s $rect_bg $rect_t_rotate $format --opt-colorkey FFFF00,FFFFFF,1
	$sample --opt-rotate 1 $file $rect_s $rect_bg $rect_t_rotate $format --opt-colorkey FFFF00,FFFFFF,0
	#echo "旋转90|颜色过滤|透明度"
	$sample --opt-rotate 1 $file $rect_s $rect_bg $rect_t_rotate $format --opt-colorkey FFFF00,FFFFFF,1 --opt-transparent 7
	$sample --opt-rotate 1 $file $rect_s $rect_bg $rect_t_rotate $format --opt-colorkey FFFF00,FFFFFF,0 --opt-transparent 7

	#echo "旋转180"
	$sample --opt-rotate 2 $file $rect_s $rect_bg $rect_t $format
	#echo "旋转180|透明度"
	$sample --opt-rotate 2 $file $rect_s $rect_bg $rect_t $format --opt-transparent 7
	#echo "旋转180|颜色过滤"
	$sample --opt-rotate 2 $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,1
	$sample --opt-rotate 2 $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,0
	#echo "旋转180|颜色过滤|透明度"
	$sample --opt-rotate 2 $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,1 --opt-transparent 7
	$sample --opt-rotate 2 $file $rect_s $rect_bg $rect_t $format --opt-colorkey FFFF00,FFFFFF,0 --opt-transparent 7

	#echo "旋转270"
	$sample --opt-rotate 3 $file $rect_s $rect_bg $rect_t_rotate $format
	#echo "旋转270|透明度"
	$sample --opt-rotate 3 $file $rect_s $rect_bg $rect_t_rotate $format --opt-transparent 7
	#echo "旋转270|颜色过滤"
	$sample --opt-rotate 3 $file $rect_s $rect_bg $rect_t_rotate $format --opt-colorkey FFFF00,FFFFFF,1
	$sample --opt-rotate 3 $file $rect_s $rect_bg $rect_t_rotate $format --opt-colorkey FFFF00,FFFFFF,0
	#echo "旋转270|颜色过滤|透明度"
	$sample --opt-rotate 3 $file $rect_s $rect_bg $rect_t_rotate $format --opt-colorkey FFFF00,FFFFFF,1 --opt-transparent 7
	$sample --opt-rotate 3 $file $rect_s $rect_bg $rect_t_rotate $format --opt-colorkey FFFF00,FFFFFF,0 --opt-transparent 7

	#echo "拉伸"
	$sample --opt-scale $file $rect_s $rect_bg $rect_t_scale $format
	#echo "拉伸|透明度"
	$sample --opt-scale $file $rect_s $rect_bg $rect_t_scale $format --opt-transparent 7
	#echo "拉伸|颜色过滤"
	$sample --opt-scale $file $rect_s $rect_bg $rect_t_scale $format --opt-colorkey F00000,FFFFFF,1
	#echo "拉伸|颜色过滤|透明度"
	$sample --opt-scale $file $rect_s $rect_bg $rect_t_scale $format --opt-colorkey F00000,FFFFFF,1 --opt-transparent 7
	#echo "拉伸|颜色过滤"
	$sample --opt-scale $file $rect_s $rect_bg $rect_t_scale $format --opt-colorkey F00000,FFFFFF,0
	#echo "拉伸|颜色过滤|透明度"
	$sample --opt-scale $file $rect_s $rect_bg $rect_t_scale $format --opt-colorkey F00000,FFFFFF,0 --opt-transparent 7
done