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

//------------------------------------------------------------------------------
//
//    Includes
//
//------------------------------------------------------------------------------
#include <stdarg.h>     // for va_start, va_list
#include <math.h>
#include <assert.h>
#include <nx_gpu_surf.h>
#include <linux/videodev2.h>
#include "nx_gl_tools.h"

#define APP_PLATFORM_Y_ALIGN_SIZE  16
#define APP_PLATFORM_X_ALIGN_SIZE  32

#ifndef ALIGN
#define  ALIGN(X,N) ( ((X) + (N - 1)) & (~((N) - 1)) )
#endif

//------------------------------------------------------------------------------
//
//    Defines
//
//------------------------------------------------------------------------------
/* Source is memory */
// #define APP_PLATFORM_SRC_FORMAT 	NX_GSURF_VMEM_IMAGE_FORMAT_RGBA //NX_GSURF_VMEM_IMAGE_FORMAT_YUV420 NX_GSURF_VMEM_IMAGE_FORMAT_YUV420M NX_GSURF_VMEM_IMAGE_FORMAT_RGBA
// #define APP_PLATFORM_DST_FORMAT 	NX_GSURF_VMEM_IMAGE_FORMAT_YUV420 //NX_GSURF_VMEM_IMAGE_FORMAT_NV21 //NX_GSURF_VMEM_IMAGE_FORMAT_NV12 //NX_GSURF_VMEM_IMAGE_FORMAT_YUV420 //NX_GSURF_VMEM_IMAGE_FORMAT_YUV422

//------------------------------------------------------------------------------
//
//    Ratate Functions
//    memory to memory
//------------------------------------------------------------------------------
#define MAX_BUFFER_NUM		31
typedef struct
{
	HGSURFCTRL 		ghAppGSurfCtrl;
	HGSURFSOURCE 	ghAppGSurfSource[MAX_BUFFER_NUM];
	HGSURFTARGET 	ghAppGSurfTarget[MAX_BUFFER_NUM];
	int32_t			srcImageFormat;
	int32_t			dstImageFormat;
	int32_t 		srcDmaFd[MAX_BUFFER_NUM];
	int32_t 		srcDmaBufNum;
	int32_t 		dstDmaFd[MAX_BUFFER_NUM];
	int32_t 		dstDmaBufNum;
	uint32_t 		srcWidth;
	uint32_t 		srcHeight;
	uint32_t 		dstWidth;
	uint32_t 		dstHeight;
	int32_t			dstOutBufNum;
} NX_GL_Rotate_HANDLE;

static int32_t FindSrcDmaFdIndex(NX_GL_Rotate_HANDLE *pRotateHANDLE, int32_t srcDmaFd)
{
	int32_t i=0;

	for( i = 0 ; i < pRotateHANDLE->srcDmaBufNum ; i ++ )
	{
		if( pRotateHANDLE->srcDmaFd[i] == srcDmaFd )
		{
			//	already added memory
			return i;
		}
	}
	return -1;
}

void *NX_GlRotateInit(uint32_t srcWidth, uint32_t srcHeight, uint32_t dstWidth, uint32_t dstHeight, int32_t (*pDstDmaFd)[3], int32_t srcImageFormat, int32_t dstOutBufNum)
{
	NX_GSURF_VMEM_IMAGE_FORMAT_MODE glSrcFormat = NX_GSURF_VMEM_IMAGE_FORMAT_YUV420;
	NX_GSURF_VMEM_IMAGE_FORMAT_MODE glDstFormat = NX_GSURF_VMEM_IMAGE_FORMAT_YUV420;

	NX_GL_Rotate_HANDLE *pRotate_HANDLE = (NX_GL_Rotate_HANDLE *)malloc(sizeof(NX_GL_Rotate_HANDLE));

	memset(pRotate_HANDLE, 0x00, sizeof(NX_GL_Rotate_HANDLE) );

	for(int i = 0; i < MAX_BUFFER_NUM; i++)
	{
		pRotate_HANDLE->srcDmaFd[i] = -1;
		pRotate_HANDLE->dstDmaFd[i] = -1;
	}

	if( (srcImageFormat == V4L2_PIX_FMT_YUV420) || srcImageFormat == V4L2_PIX_FMT_YUV420M)
	{
		pRotate_HANDLE->srcImageFormat = srcImageFormat;
		pRotate_HANDLE->dstImageFormat = srcImageFormat;

		glSrcFormat = NX_GSURF_VMEM_IMAGE_FORMAT_YUV420;
		glDstFormat = glSrcFormat;

	}
	else
	{
		NX_ERR("Not Support imageFormat !!!\n");
		return NULL;
	}

	pRotate_HANDLE->dstOutBufNum = dstOutBufNum;

	pRotate_HANDLE->srcWidth  = srcWidth;
	pRotate_HANDLE->srcHeight = srcHeight;
	pRotate_HANDLE->dstWidth  = dstWidth;
	pRotate_HANDLE->dstHeight = dstHeight;


	//create GSurf handle
	pRotate_HANDLE->ghAppGSurfCtrl = nxGSurfaceCreate(dstOutBufNum, NX_FALSE,
						0, 0, 0, 0, 0, NX_FALSE);

	nxGSurfaceSetDoneCallback(pRotate_HANDLE->ghAppGSurfCtrl, NULL); /* not used  at pixmap */

	//init GSurf EGL
	nxGSurfaceInitEGL(pRotate_HANDLE->ghAppGSurfCtrl, NULL,
	      glSrcFormat, APP_PLATFORM_X_ALIGN_SIZE, APP_PLATFORM_Y_ALIGN_SIZE,
	      glDstFormat, APP_PLATFORM_X_ALIGN_SIZE, APP_PLATFORM_Y_ALIGN_SIZE);

	//create target
	unsigned int gRenderWidth = pRotate_HANDLE->dstWidth;
	unsigned int gRenderHeight = pRotate_HANDLE->dstHeight;

	for (int i = 0 ; i < dstOutBufNum ; i++)
	{
		//create GSurf target handle
		if(pRotate_HANDLE->srcImageFormat == V4L2_PIX_FMT_YUV420)
		{
			pRotate_HANDLE->ghAppGSurfTarget[i] = nxGSurfaceCreateTarget(pRotate_HANDLE->ghAppGSurfCtrl, gRenderWidth, gRenderHeight, pDstDmaFd[i][0]);
		}
		else if(pRotate_HANDLE->srcImageFormat == V4L2_PIX_FMT_YUV420M)
		{
			pRotate_HANDLE->ghAppGSurfTarget[i] = nxGSurfaceCreateTargetWithFDs(pRotate_HANDLE->ghAppGSurfCtrl, gRenderWidth, gRenderHeight, pDstDmaFd[i]);
		}
		else
		{
			return NULL;
		}

		pRotate_HANDLE->dstDmaFd[i] = pDstDmaFd[i][0];
		pRotate_HANDLE->dstDmaBufNum++;
	}

	//init GSurf GL
	nxGSurfaceInitGL(pRotate_HANDLE->ghAppGSurfCtrl, pRotate_HANDLE->ghAppGSurfTarget[0]);

	return (void *)pRotate_HANDLE;
}

int32_t NX_GlRotateRun(void *pHandle, int32_t* pSrcDmaFd, int32_t *pDstDmaFd, int32_t rotateMode)
{
	if(!pHandle)
	{
		NX_ERR("pHandle is NULL !\n");
		return -1; 
	}
	NX_GL_Rotate_HANDLE *pRotateHANDLE = (NX_GL_Rotate_HANDLE *)pHandle;

	int32_t srcDmaFd = pSrcDmaFd[0];

	HGSURFSOURCE hsource = NULL;
	HGSURFTARGET htarget = NULL;

	// find target
	for(int i = 0; i < pRotateHANDLE->dstDmaBufNum; i++)
	{
		if(pRotateHANDLE->dstDmaFd[i] == pDstDmaFd[0])
		{
			htarget = pRotateHANDLE->ghAppGSurfTarget[i];
			break;
		}
	}

	int32_t index = FindSrcDmaFdIndex(pRotateHANDLE, srcDmaFd );
	if( 0 > index )
	{
		//create GSurf source handle
		int32_t srcWidth = pRotateHANDLE->srcWidth;
		int32_t srcHeight = pRotateHANDLE->srcHeight;
		if(pRotateHANDLE->dstImageFormat == V4L2_PIX_FMT_YUV420)
		{
			pRotateHANDLE->ghAppGSurfSource[pRotateHANDLE->srcDmaBufNum] = nxGSurfaceCreateSource(pRotateHANDLE->ghAppGSurfCtrl, srcWidth, srcHeight, pSrcDmaFd[0]);
		}
		else if(pRotateHANDLE->dstImageFormat == V4L2_PIX_FMT_YUV420M)
		{
			pRotateHANDLE->ghAppGSurfSource[pRotateHANDLE->srcDmaBufNum] = nxGSurfaceCreateSourceWithFDs(pRotateHANDLE->ghAppGSurfCtrl, srcWidth, srcHeight, pSrcDmaFd);
		}
		else
		{
			return -1;
		}

		pRotateHANDLE->srcDmaFd[pRotateHANDLE->srcDmaBufNum] = srcDmaFd;
		index = pRotateHANDLE->srcDmaBufNum;
		pRotateHANDLE->srcDmaBufNum++;
	}

	hsource = pRotateHANDLE->ghAppGSurfSource[index];

	uint32_t gRenderWidth = pRotateHANDLE->dstWidth;
	uint32_t gRenderHeight = pRotateHANDLE->dstHeight;
	nxGSurfaceRender(pRotateHANDLE->ghAppGSurfCtrl, hsource, htarget,
					0, 0, gRenderWidth, gRenderHeight, (NX_GSURF_ROTATE_MODE)rotateMode);

	nxGSurfaceUpdate(pRotateHANDLE->ghAppGSurfCtrl);

	return 0;
}

void NX_GlRotateDeInit(void *pHandle)
{
	if(!pHandle)
	{
		NX_ERR("pHandle is NULL !\n");
		return; 
	}

	NX_GL_Rotate_HANDLE *pRotateHANDLE = (NX_GL_Rotate_HANDLE *)pHandle;

	//destroy GSurf source handle
	for (int i = 0 ; i < pRotateHANDLE->srcDmaBufNum ; i++)
	{

		nxGSurfaceDestroySource(pRotateHANDLE->ghAppGSurfCtrl, pRotateHANDLE->ghAppGSurfSource[i]);
		pRotateHANDLE->ghAppGSurfSource[i] = NULL;
	}

	//deinit GSurf GL
	nxGSurfaceDeinitGL(pRotateHANDLE->ghAppGSurfCtrl);

	//destroy target	
	for (int i = 0 ; i < pRotateHANDLE->dstOutBufNum ; i++)
	{
		//destroy GSurf target handle
		nxGSurfaceDestroyTarget(pRotateHANDLE->ghAppGSurfCtrl, pRotateHANDLE->ghAppGSurfTarget[i]);
		pRotateHANDLE->ghAppGSurfTarget[i] = NULL;
	}

	//deinit GSurf EGL
	nxGSurfaceDeinitEGL(pRotateHANDLE->ghAppGSurfCtrl);

	//destroy GSurf handle
	nxGSurfaceDestroy(pRotateHANDLE->ghAppGSurfCtrl);
	pRotateHANDLE->ghAppGSurfCtrl = NULL;

	if(pRotateHANDLE)
	{
		NX_GL_Rotate_HANDLE *pRotate_HANDLE = (NX_GL_Rotate_HANDLE *)pRotateHANDLE;
		if(pRotate_HANDLE)
		{
			free(pRotate_HANDLE);
		}
	}
}