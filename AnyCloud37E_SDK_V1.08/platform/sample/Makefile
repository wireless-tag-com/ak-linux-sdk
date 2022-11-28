include config.mk

dir_y:=

ifeq ($(AI), y)
	dir_y += ai
	dir_y += ai_resample
endif

ifeq ($(AO), y)
	dir_y += ao
endif

ifeq ($(AENC), y)
	dir_y += aenc
endif

ifeq ($(ADEC), y)
	dir_y += adec
endif

ifeq ($(VENC), y)
	dir_y += venc
endif

ifeq ($(VDEC), y)
	dir_y += vdec
endif

ifeq ($(OSD), y)
	dir_y += osd
endif

ifeq ($(VO), y)
	dir_y += vo
endif

ifeq ($(TDE), y)
	dir_y += tde
endif

ifeq ($(MUX), y)
	dir_y += mux
endif

ifeq ($(DEMUX), y)
	dir_y += demux
endif

ifeq ($(VPSS), y)
	dir_y += vpss
endif

ifeq ($(VI), y)
	dir_y += vi
endif

ifeq ($(TIMER), y)
	dir_y += timer
endif


ifeq ($(VAD), y)
	dir_y += vad
endif

ifeq ($(UUID), y)
	dir_y += uuid
endif

ifeq ($(UUID_EXT), y)
	dir_y += uuid_ext
endif

all:
	@for i in $(dir_y); \
	do \
		$(MAKE) -C $$i; \
		if [ $$? -ne 0 ];then exit 1; fi \
	done

install:
	@for i in $(dir_y); \
	do \
		$(MAKE) -C $$i install; \
	done

clean:
	@for i in $(dir_y); \
	do \
		$(MAKE) -C $$i clean; \
	done

demo:
	$(MAKE) -C demo


.PHONY: all install clean image demo
