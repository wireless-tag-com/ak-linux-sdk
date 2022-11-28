# AnyCloud37E SDK V1.08 正式版 更改说明

支持AK37E芯片SDK V1.08正式版本，发布时间2022年7月20日。
主要修改点如下：

## 内核

- 【新增功能】支持AK3761E芯片配置。
- dts

​       【代码优化】LCD复位管脚DTS配置报错，单nand flash方案的dts去掉spi_nand_flash_cs_pins。

## 驱动

- 【bug修复】ak_ai_set_source接口设置不支持line-in的芯片平台时，增加返回错误提示。
- 【bug修复】红外(MLX90640)调试出现I2c通信异常。
- 【新增功能】增加支持WiFi芯片：ATBM6012B和ATBM6031B。
- 【新增功能】支持uboot中8bit、16bit、18bit RGB LCD的sharepin配置。
- 【代码优化】添加led的默认状态配置。
- 【新增功能】支持GC0328、GC032A驱动。
- 【新增功能】支持GM25Q64A和GM25Q128A两款flash。
- 【代码优化】PWM支持占空比 0% 、100%。

## Bootloader

- 【bug修复】处理配置lcd的power管脚为GPIO0时，返回结果一直报错的问题。

## 中间件

- 【新增功能】AI和AO模块支持96000采样率。
- 【代码优化】更新adec模块，节省内存拷贝次数。
- 【bug修复】第一次使用ak_vo_check_lcdc_busy获取lcd状态，概率性获取状态为 1。
- 【bug修复】ao设置数字音量时，初始化限幅值默值为32768。
- 【代码优化】ak_aenc_print_codec_info不建议使用，后面使用ak_aenc_get_codec_info替代。
- 【代码优化】aenc在编码时候默认添加ADTS头信息，整理aenc处理aac的ADTS头的时候的逻辑，添加库返回值判断。
- 【代码优化】声音检测sample增加模拟信号与数字信号对应检测方法。
- 【代码优化】音频调试工具界面的设置属性名称与VQE debug属性输出的命名统一。
- 【代码优化】VDEC模块解决重置当前激活的SPS和PPS等参数会导致视频解码库报段错误的问题，解决随机篡改H264码流进行煲机会卡死的问题。

## 工具

- 【新增功能】burntool中nand配置文件中删除WINBOND W25N01GV的配置。
- 【代码优化】Anyka_Audio_Tuning_Tool音频调试工具界面的设置属性名称更新，导出头文件格式更新。
- 【代码优化】Anyka_USB_Burn_Tool更新帮助信息。
- 【代码优化】Anyka_USB_Burn_Tool与Anyka_Uart_Burn_Tool更新producer：修复使用FM25Q64烧录失败的问题，修复FLASH读写时spi mode切换可能会导致烧写失败的问题。



# AnyCloud37E SDK V1.07 正式版 更改说明

发布时间2022年6月8日。SDK无修改。



# AnyCloud37E SDK V1.06 正式版 更改说明

发布时间2022年4月2日。SDK无修改。




# AnyCloud37E SDK V1.05 正式版 更改说明

支持AK37E芯片SDK V1.05正式版本，发布时间2022年3月10日。
主要修改点如下：

## 内核

* 【bug修复】解决tftpboot重复下载文件，出现内存分配问题，下载速度下降的问题。
* 【新增功能】支持双网卡网桥数据转发功能。
* dts

​       【代码优化】RGB屏默认驱动能力改成0档。

## 驱动
* 【bug修复】解决以太网iperf2打流时进入standby，然后再唤醒，iperf2退出运行的问题。
* 【bug修复】解决进入standby使用rtc定时唤醒，实际唤醒时间与设置时间不一致的问题。
* 【bug修复】解决进入standby后唤醒，对读卡器进行读写操作失败的问题。
* 【bug修复】解决在后台运行音频相关demo，进入standby，提示error! Err wait available_data的问题。
* 【bug修复】解决调用ak_vdec_sample之后发现电流值变大，到视频解码结束后，电流值未出现下降。
* 【新增功能】支持USB的 RX/TX的DMA/CPU工作方式可配。
* 【bug修复】解决w25q128,常温watch_dog,写出现No such file or directory的问题。
* 【bug修复】解决Nandflash QE bit配置缺省问题。
* 【代码优化】增加TS和KEY的sample。
* 【bug修复】修复gpiokey按下为高电平的情况下, 释放按键时事件丢失问题。
* 【bug修复】修复udc作为usb读卡器时连接到37E上，当主控37E休眠唤醒后，读卡器读写异常的问题。
* 【bug修复】修复加载SD异常卡后操作mmcqd线程，CPU耗尽的问题。
* 【bug修复】修复板上连接USB转串口，但不接任何主机，系统启动会概率性失败，卡在uboot的问题。
* 【bug修复】修复问题：使用nandflash GD5F1GQ5UEYIG，测试env分区擦除后，默认uboot通过envbk分区进行修复。实测过程中，再uboot中主动擦除env分区（本来是好的block），重启后发现程序出现异常。

## Bootloader

* 【代码优化】更新producer至5.1.18，配置flag、exflag默认值。
* 【bug修复】解决使用 WINBOND 25Q128JVSQ NOR FLASH 频繁的FLASH只读的问题。

## 中间件

* 【bug修复】解决执行ak_mux_sample打印error! Unknow mixer souce 0的问题。
* 【bug修复】sample vo下需去掉readme。
* 【bug修复】解决开eq后，dac loopback一直打印的问题。
* 【bug修复】解决打开声音侦测动态设置，影响音频流的问题。
* 【bug修复】设置声音侦测灵敏度（ak_ai_set_vad_attr）失败 。
* 【bug修复】解决h264解码出现断言的问题。
* 【bug修复】处理ak_ao_save_file调用时高采样率数据会溢出的问题。

## 工具

* 【新增功能】Audio tool上增加dump时长的界面。
* 【新增功能】Audio tool Import 导入失败无提示。
* 【新增功能】Audio tool未连接显示调试功能。
* 【新增功能】Audio tool AI Nsupport db设置数值范围与需求不符。
* 【新增功能】Audio tool 连接断开两次后，第三次连接失败。

## 工具链

* 【bug修复】更新工具链版本至1.05版本，主要优化工具链大小，并修复bison执行文件导致的编译问题。



# AnyCloud37E SDK V1.04 正式版 更改说明

支持AK37E芯片SDK V1.04正式版本，发布时间2021年10月20日。
主要修改点如下：

## 内核

* 【代码优化】内核修复通过ION反复分配释放预留内存导致分配失败的问题 。

* 【代码优化】内核驱动添加版本号。

* dts

  【新增功能】DTS新增LCD reset/power/backlight的激活电平配置功能。

  【bug修复】解决外接I2S设备，运行TF_Card.sh挂载命令后再加载I2S控制器的驱动会出现错误的问题。

## 驱动

* 【新增功能】standby GPIO唤醒支持同时配置多个gpio。 
* 【新增功能】standby支持RTC、AIN唤醒源。 
* 【新增功能】增加UART流控配置。 
* 【代码优化】uart cpu占用率高问题的处理。
* 【新增功能】添加rtc standby sample 。
* 【bug修复】解决SPI1设置时钟频率无效的问题。
* 【bug修复】解决在10寸屏配置下运行AK_VDEC_Sample煲机，概率性出现解码H.264文件自动退出的问题。
* 【bug修复】解决使用TP9950外接NTSC（960*240）模拟信号，图像会出现抖动的问题。

## 中间件

* 【新增功能】增加声音侦测sample测试。 
* 【代码优化】视频编码库优化了通道管理的逻辑。 
* 【新增功能】增加mux模块以及mux sample。 
* 【新增功能】mjpeg增加解码直接解出RGB功能。 
* 【bug修复】解决B帧解析错误之后不能恢复的问题。 
* 【新增功能】增加efuse功能中间件封装模块uuid，以及uuid sample。
* 【新增功能】atc库支持打印带模块字符串和打印等级设置。
* 【新增功能】demux sample添加解析mjpeg成单张图片。
* 【bug修复】更新音频库和音效库，修正了eq参数使用默认参数的情况下会出错的问题。
* 【bug修复】更新视频解码库，处理解码异常H264码流会报断言错的问题。

##  工具

* 【代码优化】烧录工具更新producer，固定烧录以1线和20M的CLK的方式。
* 【bug修复】烧录工具解决勾选“升级”烧录uboot分区失败的问题。 

## 工具链

* 【新增功能】更新编译工具链至1.0.04版本，添加gdb以及支持backtrace功能。 



# AnyCloud37E SDK V1.03 正式版 更改说明

支持AK37E芯片SDK V1.03正式版本，发布时间2021年8月5日。
主要修改点如下：

## 内核
* 【新增功能】linux支持standby模式，支持唤醒源为GPIO。

* 【新增功能】新增支持两路SPI0_CS拖2两路Slave。

* SPI Nand Flash
  【新增器件】新增支持四款SPI Nand Flash:芯天下：XT26G01C、XT26G02E，旺宏MX：MX35LF2GE4AD、MX35LF1G4AB。

* dts

  【新增功能】SDK编译配置支持MPU。

  【新增功能】支持配置I2S。


## 驱动 
*  【新增功能】音频支持PDM采集。
*  【新增功能】音频支持I2S采集与播放。
*  【新增功能】支持gpio_key功能。
*  【代码优化】dac的设备节点名称改为pcmC0D0p，I2S0播放设备的设备节点名称改为pcmC1D0p。
*  【代码优化】dac loopback设备节点名称改为pcmC0D0l，I2S loopback设备节点名称改为pcmC1D0l。
*  【代码优化】adc的设备节点名称改为pcmC0D0c，PDM采集的设备节点名称改为pcmC2D0c，I2S0采集的设备节点名称改为pcmC1D0c，I2S1的设备名称节点改成pcmC1D1c。
*  【bug修复】解决ATBM6032i tcp协议rx、tx数据差异较大的问题。
*  【bug修复】解决MPU屏驱动异常的问题。


## Bootloader

【新增功能】Uboot支持MPU屏显示。

## 中间件

* 【新增功能】音频支持PDM采集。

* 【新增功能】音频支持I2S采集与播放。

* 【新增功能】增加ak_adec_get_buf_status，可以获取解码缓存区的状态。

* 【新增功能】ai模块增加声音侦测功能的接口。

* 【新增功能】ai模块可以通过ak_ai_set_max_frame_num接口配置最大缓存的帧数量。

* 【新增功能】ai模块可以通过ak_ai_get_max_frame_num接口获取到最大缓存的帧数量。

* 【bug修复】解决用ai_ao.sh脚本煲机一段时间后，出现“pcm frame is too many”打印的问题。

* 【bug修复】解决aenc 运行ak_aenc_sample，概率性出现pcm frame is too many，进程不退出的问题。

* 【bug修复】解决使用venc sample写多帧jpeg格式文件，出现Segmentation fault的问题。



# AnyCloud37E SDK V1.02 正式版 更改说明

支持AK37E芯片SDK V1.02正式版本，发布时间2021年5月14日。
包括以下修改：

* 1、新增支持320*240分辨率MPU屏（驱动IC ILI9342C）；
* 2、新增支持SZ18201；
* 3、新增支持AD芯片TP9950，新增支持BT.656、BT.1120；
* 4、EVB_CBDM_AK376xE_V1.0.2核心板新增支持UART1/UART2/UART3（mipi的dts里面默认打开了uart0/1/2， uart3需要手动参考uart2的配置修改dts）；
* 5、EVB_CBDM_AK376xE_V1.0.2核心板新增支持Camera采集功能；
* 6、EVB_CBDM_AK376xE_V1.0.2核心板新增支持nor+nand的配置；
* 7、解决atbm6031，运行wifi_driver.sh ap大概率性出现wifi加载失败，导致ap开启失败的问题；
* 8、解决网卡1概率出现动态获取不到ip的问题；
* 9、音频优化，支持播放采样率与采集采样率不相同的场景；
* 10、提供normal工作模式下，RTC计时的软件校准方案；
* 11、I2C速率由最高312kbps提升至400kbps。 



# AnyCloud37E SDK V1.01 正式版 更改说明


支持AK37E芯片SDK V1.01正式版本，发布时间2021年3月17日。
包括以下修改：
* 1、支持vi采集，支持MJPEG编码，最大编码分辨率到4096*4096;
* 2、支持 MIPI DSI V1.3接口LCD屏；
* 3、音频输入模块增加在打开ai的情况下，重新配置输入参数的功能接口：int ak_ai_reset_params(int ai_handle_id, struct ak_audio_in_param *param, int frame_length)；
* 4、音频输出模块增加在打开ao的情况下，重新配置输入参数的功能接口：int ak_ao_reset_params(int ao_handle_id, struct ak_audio_out_param *param)；
* 5、音频输出模块增加获取播放buffer状态的接口：int ak_ao_get_buf_status(int ao_handle_id, struct ak_dev_buf_status *buf_status)；
* 6、ao模块支持防啸叫功能接口:int ak_ao_enable_hs(int ao_handle_id, int enable)；
* 7、新增支持JL11X1 PHY；
* 8、新增支持GC0308 sensor。


# AnyCloud37E SDK V1.00 正式版 更改说明

支持AK37E芯片的第一个SDK正式版本，发布时间2020年12月23日。
包括以下功能：

* 1、视频解码支持MJPEG解码以及H264解码;
* 2、视频播放最高支持最高支持1920*1080@30fps解码性能；
* 3、音频支持8000、11025、12000、16000、22050、24000、32000、44100、48000采集以及音频播放功能；
* 4、音频编码及音频解码支持amr、g711a/u、MP3、AAC编解码，如果需要其他格式的编解码可以后续修改编解码库进行增加；
* 5、触摸屏功能支持型号ADT07016BR50-22T；
* 6、双以太网功能；
* 7、wifi支持atbm6031, atbm6032, rtl8188ftv, rtl8189ftv；
* 8、TF卡功能；
* 9、支持USB接口接U盘，读写U盘数据。
