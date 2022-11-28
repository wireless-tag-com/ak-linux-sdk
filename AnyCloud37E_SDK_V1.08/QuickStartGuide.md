# AnyCloud37E SDK #

----------

AnyCloud37E SDK是基于嵌入式linux操作系统的软件开发包。

## 说明 ##

以下仅简单描述SDK的快速编译、开发入门、镜像烧录等步骤，更详细的说明请参考PDK包中的《AnyCloud37E平台用户开发手册》。

## 依赖 ##

1. 安装gcc交叉工具链，参考PDK包中的《AnyCloud平台SDK用户开发手册》；

2. SDK的编译依赖以下第三方工具包，如果缺失，可按照以下步骤安装。以Ubuntu系统为例：

```
   sudo apt-get install build-essential
   sudo apt-get install libncurses5
   sudo apt-get install libncurses5-dev
   sudo apt-get install python
   sudo apt-get install u-boot-tools
```

## 快速编译 ##

1. 解压安装SDK包：tar -zxf AnyCloud37E_SDK_Vx.xx.tar.gz

2. 进入解压后的SDK根目录：cd ~/AnyCloud37E_SDK_Vx.xx

3. 自动编译：./auto_build.sh

   注：默认编译包含内核源码、U-boot源码、中间件示例代码、驱动示例代码、文件系统镜像、应用示例源码。
   可通过编辑配置编译脚本auto_build.sh对编译项进行裁剪。

## 开发入门 ##

1. 编写应用代码

   参考AnyCloud37E_SDK_Vx.xx/platform/sample目录下的中间件示例代码进行代码及Makefile编写。
   



## 烧录镜像 ##

SDK完成编译后将在AnyCloud37E_SDK_Vx.xx/image目录下生成以下镜像文件：
   * uImage：内核镜像文件；
   * u-boot.bin：全功能U-Boot目标镜像文件；
   * *.dtb：与目标开发板匹配的DTB二进制文件；
   * root.sqsh4：Linux标准的根文件系统；
   * usr.sqsh4：挂载到/usr/目录；
   * usr.jffs2：挂载到/etc/config/目录。
