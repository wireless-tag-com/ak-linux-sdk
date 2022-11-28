平台环境变量镜像制作步骤说明如下：

1、首先制作一个平台自己的env环境变量文件，命名为env.cfg，每个环境变量编写格式为：空格 + name + 空格 + value， 参考envtool下的env_v330.cfg或者env_v331.cfg文件；

2、envtool下的env_v330.cfg和env_v331.cfg文件，分别对应ak391xev330和ak391xev331芯片；

3、然后在shell下执行tr命令，制作一个内容全为0xff的env.img环境变量镜像，大小为4K的环境变量镜像env.img；
tr '\000' '\377' < /dev/zero | dd of=./env.img bs=1024 count=4

4、在linux环境下，执行mkenv_tools下自带的./fw_setenv -s env.cfg生成最终的平台环境变量镜像env.img；

5、在linux环境下，执行mkenv_tools下自带的./fw_printenv 查看生成的变量镜像env.img结果内容是否正确。