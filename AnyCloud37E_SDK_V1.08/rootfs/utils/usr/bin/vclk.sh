#!/bin/sh
																				#该脚本用来获取vclk的值
DEVMEM_RES=`devmem 0x08000008`                                                  #读取寄存器地址
VCLK_SEL=`hexmov -a $DEVMEM_RES,17,3 -B`                                        #位操作向右移动17位,取最后3位的值,10进制输出
if [ $VCLK_SEL -eq 0 ]
then
    let VCLK_SEL=1                                                              #如果VCLK_SEL等于0则置为1
fi
echo VCLK_SEL=$VCLK_SEL
                                                                                #PLL_M、PLL_N、PLL_OD为CORE PLL输出频率的配置参数
PLL_M=`hexmov -a $DEVMEM_RES,0,8 -B`                                            #位操作向右移动0位,取最后8位的值,10进制输出
PLL_N=`hexmov -a $DEVMEM_RES,8,4 -B`                                            #位操作向右移动8位,取最后4位的值,10进制输出
PLL_OD=`hexmov -a $DEVMEM_RES,12,2 -B`                                          #位操作向右移动12位,取最后2位的值,10进制输出
PLL_OD_LSHIFT=`awk 'BEGIN{print lshift(1,"'$PLL_OD'")}'`                        #用awk函数lshift向左移动1位
let CORE_PLL_CLK=12*$PLL_M/$PLL_N/$PLL_OD_LSHIFT                                #根据公式core_pll_clk = 12*M/(N*2^OD)算出CORE_PLL_CLK值
echo DEVMEM_RES=$DEVMEM_RES
echo PLL_M=$PLL_M
echo PLL_N=$PLL_N
echo PLL_OD=$PLL_OD
echo CORE_PLL_CLK=$CORE_PLL_CLK
VCLK=`awk 'BEGIN{print rshift("'$CORE_PLL_CLK'","'$VCLK_SEL'")}'`               #vclk是video相关模块的工作时钟,如isp, video encoder,video decoder,lcd controller, gui等
#let VCLK=$VCLK*$MHZ
echo VCLK=$VCLK                                                                 #打印VCLK的值