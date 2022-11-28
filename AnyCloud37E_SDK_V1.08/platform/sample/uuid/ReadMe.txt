ak_uuid_sample测试

Efuse 模块可以用来读取芯片的唯一ID。
本测试用例用来测试读取global分区。
测试步骤:
Step1:
insmod /usr/modules/ak_efuse.ko，成功产生设备节点/dev/efuse。

Step2:
   通过cat /sys/kernel/efuse/efuse_OR命令查看当前芯片的id。

Step3：
   ./ak_uuid_sample

对比step2和step3的数据，查看step3获取到的数据是否和step2查看到的数据一致。