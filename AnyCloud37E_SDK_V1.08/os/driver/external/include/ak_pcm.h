#ifndef __AKPCM_H__
#define __AKPCM_H__

/********************** IOCTL *********************************************/
#define PCM_IOC_MAGIC			'P'
#define AKPCM_IO(nr)			_IOC(_IOC_NONE, PCM_IOC_MAGIC, nr, 0)
#define AKPCM_IOR(nr)			_IOR(PCM_IOC_MAGIC, nr, int)
#define AKPCM_IORn(nr, size)	_IOR(PCM_IOC_MAGIC, nr, size)
#define AKPCM_IOW(nr)			_IOW(PCM_IOC_MAGIC, nr, int)
#define AKPCM_IOWn(nr, size)	_IOW(PCM_IOC_MAGIC, nr, size)
/* ------------- command ------------------------------------------------ */
#define IOC_NR_PREPARE		(0xE0)
#define IOC_NR_RESUME		(0xE1)
#define IOC_NR_RSTBUF		(0xE3)
#define IOC_NR_GETTIMER		(0xE6)
#define IOC_NR_GET_BUF_STATUS	(0xE7)
#define IOC_NR_NOTICE_END	(0xE9)
#define IOC_NR_GET_ACT_RATE	(0xEA)
#define IOC_PREPARE			AKPCM_IOWn(IOC_NR_PREPARE, struct akpcm_pars)
#define IOC_RESUME			AKPCM_IO(IOC_NR_RESUME)
#define IOC_RSTBUF			AKPCM_IO(IOC_NR_RSTBUF)
#define IOC_GETTIMER		AKPCM_IORn(IOC_NR_GETTIMER, unsigned long long)
#define IOC_GET_BUF_STATUS	AKPCM_IORn(IOC_NR_GET_BUF_STATUS, unsigned long)
#define IOC_GET_ACT_RATE	AKPCM_IORn(IOC_NR_GET_ACT_RATE, unsigned long)

/* ------------- HW parameters ------------------------------------------ */
/* ------------- HW configures ------------------------------------------ */
/* ------------- SW configures ------------------------------------------ */
#define IOC_NR_PARS			(0xF2) /* refer to akpcm_pars */
#define IOC_GET_PARS		AKPCM_IORn(IOC_NR_PARS, struct akpcm_pars)
/* ------------- SOURCEs -------------------------------------------------- */
#define IOC_NR_SOURCES		(0x10)
#define IOC_GET_SOURCES		AKPCM_IOR(IOC_NR_SOURCES)
/* If set to SIGNAL_SRC_MUTE, devices will be power down */
#define IOC_SET_SOURCES		AKPCM_IOW(IOC_NR_SOURCES)
/* ------------- GAINs ------------------------------------------------- */
#define IOC_NR_GAIN			(0x30) /* HP */
#define IOC_GET_GAIN		AKPCM_IOR(IOC_NR_GAIN)
#define IOC_SET_GAIN		AKPCM_IOW(IOC_NR_GAIN)

/* ------------- SPEAKER ---------------------------------------- */

#define IOC_SPEAKER			(0x81)
#define IOC_SET_SPEAKER		AKPCM_IOW(IOC_SPEAKER)

/* play_dev */
#define AKPCM_PLAYDEV_HP	(1UL<<0)

/* cptr_dev */
#define AKPCM_CPTRDEV_MIC	(1UL<<0)
#define AKPCM_CPTRDEV_LI	(1UL<<1)
#define AKPCM_CPTRDEV_AUTO	(0UL<<0)

/* sample_bits */
#define AKPCM_SMPL_BIT_U8	(1UL<<0)
#define AKPCM_SMPL_BIT_U16	(1UL<<2)

/* rates */
#define AKPCM_RATE_8000			(1<<1) /* 8000Hz */
#define AKPCM_RATE_11025		(1<<2) /* 11025Hz */
#define AKPCM_RATE_16000		(1<<3) /* 16000Hz */
#define AKPCM_RATE_22050		(1<<4) /* 22050Hz */
#define AKPCM_RATE_32000		(1<<5) /* 32000Hz */
#define AKPCM_RATE_44100		(1<<6) /* 44100Hz */
#define AKPCM_RATE_48000		(1<<7) /* 48000Hz */
#define AKPCM_RATE_CONTINUOUS	(1<<30) /* continuous range */
#define AKPCM_RATE_KNOT			(1<<31) /* more non-continuos rates */
#define AKPCM_RATE_ALL	(AKPCM_RATE_8000 | AKPCM_RATE_11025 | \
            AKPCM_RATE_16000 | AKPCM_RATE_22050 | AKPCM_RATE_32000 | \
            AKPCM_RATE_44100 | AKPCM_RATE_48000)

/* select for MixerOUT */
#define MIXER_OUT_SEL_DAC			(0b100) //SOURCE_DAC
#define MIXER_OUT_SEL_LINEIN		(0b010) //SOURCE_LINEIN
#define MIXER_OUT_SEL_MIC			(0b001) //SOURCE_MIC
#define MIXER_OUT_SEL_MUTE			0 //SIGNAL_SRC_MUTE
#define MIXER_OUT_SEL_MAX			(MIXER_OUT_SEL_DAC|MIXER_OUT_SEL_LINEIN|MIXER_OUT_SEL_MIC)

#define HP_IN_MUTE					0
#define HP_IN_MIC_OFFSET			0
#define HP_IN_LINEIN_OFFSET			1
#define HP_IN_DAC_OFFSET			2

/* select for MixerIN */
#define MIXER_IN_SEL_DAC			(0b001) //SOURCE_ADC_DAC
#define MIXER_IN_SEL_LINEIN			(0b100) //SOURCE_ADC_LINEIN
#define MIXER_IN_SEL_MIC			(0b010) //SOURCE_ADC_MIC
#define MIXER_IN_SEL_MUTE			0
#define MIXER_IN_SEL_MAX			(MIXER_IN_SEL_DAC|MIXER_IN_SEL_LINEIN|MIXER_IN_SEL_MIC)

#define HEADPHONE_GAIN_MIN		0
#define HEADPHONE_GAIN_MAX		5
#define LINEIN_GAIN_MIN			0
#define LINEIN_GAIN_MAX			15
#define MIC_GAIN_MIN			0
#define MIC_GAIN_MAX			7

struct akpcm_pars {
	/* -- HW parameters -- */
	unsigned int id;
	unsigned int rate; /* rate in Hz */
	unsigned int channels;
	unsigned int sample_bits;
	unsigned int period_bytes; /* DMA size  */
	unsigned int periods; /* buffer size = (periods * period_bytes) */
	unsigned int reserved; //stop_threshold;
};

enum akpcm_snd_track {
	AK_PCM_TRACK_MONO_LEFT,
	AK_PCM_TRACK_MONO_RIGHT,
	AK_PCM_TRACK_MONO = AK_PCM_TRACK_MONO_RIGHT,
	AK_PCM_TRACK_STEREO,
	AK_PCM_TRACK_MAX,
};

#endif
