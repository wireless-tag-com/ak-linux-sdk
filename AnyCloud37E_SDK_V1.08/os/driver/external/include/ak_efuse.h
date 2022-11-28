/*
 * @file  ak_efuse.h
 * @brief efuse operation interface
 * Copyright (C) 2021 Anyka (Guangzhou) Software Technology Co., LTD
 * @author yangruibin
 * @date  2021-02-25
 * @version 1.0
 */

#ifndef __AK_EFUSE_H__
#define __AK_EFUSE_H__

/********************** IOCTL *********************************************/
#define AK_EFUSE_IOC_MAGIC			'F'

#define DO_GLOBE_ID_GET_LEN		_IOR(AK_EFUSE_IOC_MAGIC, 0x101, int)
#define DO_GLOBE_ID_GET			_IOR(AK_EFUSE_IOC_MAGIC, 0x102, int)
#define DO_CUSTOMER_ID_GET_LEN	_IOR(AK_EFUSE_IOC_MAGIC, 0x103, int)
#define DO_CUSTOMER_ID_GET		_IOR(AK_EFUSE_IOC_MAGIC, 0x104, int)
#define DO_CUSTOMER_ID_SET		_IOW(AK_EFUSE_IOC_MAGIC, 0x105, int)

#define ERR_PARAM         1  // ioctl参数检查错误
#define ERR_NOLOCK        2  // GLOBE ID读取，没有锁定错误码
#define ERR_LOCKED        3  // Customer ID写入，已经锁定错误码
#define ERR_OTHER         4  // 其他错误，数据比对错误等

#endif //__AK_EFUSE_H__
