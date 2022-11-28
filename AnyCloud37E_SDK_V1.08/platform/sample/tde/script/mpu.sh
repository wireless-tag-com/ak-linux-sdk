#!/bin/sh
sample=ak_tde_sample
pic_src=./aklogo.145.40.rgb                                                     #源图文件名
pic_bg=./desert.1024.600.rgb                                                    #背景图文件名
format="--format-s 3 --format-bg 1 --format-out 0"                              #源图 背景图 屏幕格式
file="--file-s $pic_src --file-bg $pic_bg"                                      #文件名称
rect_bg="--rect-bg 1024,600,0,0,1024,600"                                       #背景图坐标

rect_s="--rect-s 145,40,0,0,145,40"                                             #源图坐标
rect_t="--rect-t 88,100,145,40"                                                 #贴图到屏幕坐标,屏幕宽高由sample获取
rect_t_rotate="--rect-t 140,48,40,145"                                          #旋转贴图到屏幕坐标
rect_t_scale="--rect-t 70,95,180,50"                                            #拉伸贴图到屏幕坐标

echo $sample --opt-blit $file $rect_s $rect_bg $rect_t $format --reset-screen --opt-colorkey FFFF00,FFFFFF,1
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