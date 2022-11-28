dir_y :=
dir_y += ak_drv_gpio_sample
dir_y += ak_drv_led_sample
dir_y += ak_drv_motor_sample
dir_y += ak_drv_pwm_sample
dir_y += ak_drv_saradc_sample
dir_y += ak_drv_wdt_sample
dir_y += ak_drv_efuse_sample
dir_y += ak_drv_rtc_sample
dir_y += ak_drv_ts_sample
dir_y += ak_drv_key_sample

INSTALL :=
#KSRC := $(shell pwd)/../bd

INSTALL := 
#KSRC := $(shell pwd)/../bd
export KSRC
export INSTALL
export CROSS_COMPILE

all:
	@for i in $(dir_y); \
	do \
		$(MAKE) -C $$i; \
		if [ $$? -ne 0 ];then exit 1; fi \
	done

clean:
	@for i in $(dir_y); \
	do \
		$(MAKE) -C $$i clean; \
	done

install:
	@for i in $(dir_y); \
	do \
		$(MAKE) -C $$i  install; \
	done

help:
	@echo "Compile :"
	@echo "    --KSRC : Kernel Build Directory(Only Support Absolute Path)"
	@echo "    --examples : "
	@echo "           make KSRC=/home/test/linux/bd"
	@echo "Install :"
	@echo "    --INSTALL : Modules Intall Directory(Only Support Absolute Path)"
	@echo "    --examples : "
	@echo "           make install INSTALL=/home/test/linux/modules"

.PHONY: all clean help
