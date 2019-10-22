//------------------------------------------------------------------------------
//
//    Copyright (C) 2019 Nexell Co., All Rights Reserved
//    Nexell Co. Proprietary & Confidential
//
//    NEXELL INFORMS THAT THIS CODE AND INFORMATION IS PROVIDED "AS IS" BASE
//    AND WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING
//    BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS
//    FOR A PARTICULAR PURPOSE.
//
//    Description    :
//    Author         : hyunjh(hyunjh@nexell.co.kr)
//    History        : 2019-07-24 1st release
//------------------------------------------------------------------------------

#ifndef __NX_GlTOOL_H__
#define __NX_GlTOOL_H__

//------------------------------------------------------------------------------
//
//    Defines
//    
//------------------------------------------------------------------------------
#ifdef ANDROID
#define NX_DBG			ALOGD
#define NX_ERR			ALOGE
#else
#define NX_DBG			printf
#define NX_ERR			printf
#endif

//------------------------------------------------------------------------------
//
//    Ratate Functions
//    memory to memory
//------------------------------------------------------------------------------
enum
{
	NX_ROTATE_O,
	NX_ROTATE_90R,
	NX_ROTATE_180,
	NX_ROTATE_270R,
	NX_ROTATE_MIRROR,
	NX_ROTATE_VFLIP,
	NX_ROTATE_MAX
};

#ifdef __cplusplus
extern "C" {
#endif

void *NX_GlRotateInit(uint32_t srcWidth, uint32_t srcHeight, int32_t (*pDstDmaFd)[3], int32_t srcImageFormat, int32_t dstOutBufNum);
int32_t NX_GlRotateRun(void *pHandle, int32_t* pSrcDmaFd, int32_t *pDstDmaFd, int32_t rotateMode);
void NX_GlRotateDeInit(void *pHandle);

#ifdef __cplusplus
}
#endif

#endif
