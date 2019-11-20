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

#ifndef __NX_GPU_SURF_H__
#define __NX_GPU_SURF_H__

/*
[Main feature]
	Format converter using GPU.
	Mem_to_Mem, Mem_to_Display operation.

[Usage]
	1. From Memory
		1) Mem to Mem
		    - src format : NX_GSURF_VMEM_IMAGE_FORMAT_YUV420, NX_GSURF_VMEM_IMAGE_FORMAT_NX_GSURF_VMEM_IMAGE_FORMAT_YUV420M, NX_GSURF_VMEM_IMAGE_FORMAT_RGBA
			- dst format : NX_GSURF_VMEM_IMAGE_FORMAT_NV21, NX_GSURF_VMEM_IMAGE_FORMAT_NV12, NX_GSURF_VMEM_IMAGE_FORMAT_YUV422, NX_GSURF_VMEM_IMAGE_FORMAT_YUV420
		2) Mem to Display
		    - src format : NX_GSURF_VMEM_IMAGE_FORMAT_YUV420, NX_GSURF_VMEM_IMAGE_FORMAT_NX_GSURF_VMEM_IMAGE_FORMAT_YUV420M, NX_GSURF_VMEM_IMAGE_FORMAT_RGBA
			- dst format : NX_GSURF_VMEM_IMAGE_FORMAT_YUV422

	2. From 4Camera
		1) 4Cam to Mem
			- src format : NX_GSURF_VMEM_IMAGE_FORMAT_YUV420
			- dst format : NX_GSURF_VMEM_IMAGE_FORMAT_NV21, NX_GSURF_VMEM_IMAGE_FORMAT_NV12, NX_GSURF_VMEM_IMAGE_FORMAT_YUV422, NX_GSURF_VMEM_IMAGE_FORMAT_YUV420
		2) 4Cam to Display
			- src format : NX_GSURF_VMEM_IMAGE_FORMAT_YUV420
			- dst format : NX_GSURF_VMEM_IMAGE_FORMAT_YUV422

[Sequeunce]
	1. Create and Init. (nxGSurfaceCreate, nxGSurfaceInitEGL, nxGSurfaceInitGL, nxGSurfaceCreateSource4ch, nxGSurfaceCreateTarget)
	2. Render. 			(nxGSurfaceRender4ch, nxGSurfaceUpdate)
	3. called Callback 	(It's optional. nxGSurfaceSetDoneCallback).
	   User process can start with callback argument. (NXRenderInfo)

[Format performance]
	1. src YUV420, dst YUV422       (One rendering, The fastest)
	2. src RGBA,   dst RGBA         (One rendering)
	3. src YUV420, dst YUV420       (Three rendering)
	   src YUV420, dst NV21 or NV12 (Two rendering, Time is alomot the same with the YUV420)
	4. src RGBA,   dst YUV420       (Three rendering, and too much arithmetic operation, The slowest)

[Source 4Channel camera memory shape]
	|--------------stride-----------|
	|-width-|                       |
	|_______|_______________________|
	|       |       |       |       |
	|       |       |       |       |
	|  0ch  |  1ch  |  2ch  |  3ch  |
	|_______|_______|_______|_______|
*/



//------------------------------------------------------------------------------
//
//    Includes
//
//------------------------------------------------------------------------------
#include <base_interface.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>



//------------------------------------------------------------------------------
//
//    Defines
//
//------------------------------------------------------------------------------
typedef void* HGSURFSOURCE;
typedef void* HGSURFTARGET;
typedef void* HGSURFCTRL;

//---------------------------------------------------------
//    1Channel Rotate  (only for memory source, not camera)
//---------------------------------------------------------
typedef enum{
	NX_GSURF_ROTATE_O,
	NX_GSURF_ROTATE_90,
	NX_GSURF_ROTATE_180,
	NX_GSURF_ROTATE_270,
	NX_GSURF_ROTATE_MIRROR,
	NX_GSURF_ROTATE_VFLIP,
	NX_GSURF_ROTATE_MAX
}NX_GSURF_ROTATE_MODE;

//---------------------------------------------------------
//    4Channel Direction (only for 4 camera source)
//---------------------------------------------------------
typedef enum
{
	NX_GSURF_DIRECTION_CH0,    	/* show CH0 camera image */
	NX_GSURF_DIRECTION_CH1,   	/* show CH1 camera image */
	NX_GSURF_DIRECTION_CH2,   	/* show CH2 camera image */
	NX_GSURF_DIRECTION_CH3,    	/* show CH3 camera image */
	NX_GSURF_DIRECTION_MAX  	/* show 4Channel camera images at once */
}NX_GSURF_DIRECTION_MODE;

//---------------------------------------------------------
//    VMEM Format
//---------------------------------------------------------
typedef enum{
	NX_GSURF_VMEM_IMAGE_FORMAT_RGBA,   	 /* target: display, mem, 			source: mem */
	/* NX_GSURF_VMEM_IMAGE_FORMAT_YUV420 support 'seperated fd' and 'non-seperated fd' both. */
	NX_GSURF_VMEM_IMAGE_FORMAT_YUV420, 	 /* target: mem, 				source: mem, 4camera */
	NX_GSURF_VMEM_IMAGE_FORMAT_YUV422, 	 /* target: display, mem, 		source not supported */
	NX_GSURF_VMEM_IMAGE_FORMAT_NV12,   	 /* target: mem, 				source not supported */
	NX_GSURF_VMEM_IMAGE_FORMAT_NV21,   	 /* target: mem, 				source not supported */
	NX_GSURF_VMEM_IMAGE_FORMAT_UV,	   	 /* target not supported, 		source not supported */
	NX_GSURF_VMEM_IMAGE_FORMAT_Y,	     /* target not supported, 		source not supported */
	NX_GSURF_VMEM_IMAGE_FORMAT_U,	     /* target not supported, 		source not supported */
	NX_GSURF_VMEM_IMAGE_FORMAT_V,	     /* target not supported, 		source not supported */
	NX_GSURF_VMEM_IMAGE_FORMAT_MAX
}NX_GSURF_VMEM_IMAGE_FORMAT_MODE;

//---------------------------------------------------------
//    Structure defines
//---------------------------------------------------------
typedef struct tagNXDisplayRect
{
    unsigned int    x;
    unsigned int    y;
    unsigned int    width;
    unsigned int    height;
} NXDisplayRect;

typedef struct tag_DISPLAY_INFO
{
	unsigned int    connectorID;
    unsigned int    crtcID;
    unsigned int    planeID;
	unsigned int	width;		//GL Render target width
	unsigned int	height;		//GL Render target height
    NXDisplayRect 	dspDstRect; //LCD H/W size
    NXDisplayRect 	dspSrcRect; //GL Render target
} NXDisplayInfo;

typedef struct tagNXRenderInfo
{
    int cam_idx; //used source camera buffer index
    int do_display;
    HGSURFSOURCE hsource; //used user hsource (source camera mode => NULL)
    HGSURFTARGET htarget; //used user htarget (destination display mode => NULL)
} NXRenderInfo;

typedef void (* NX_GSURF_DONE_CALLBACK)(NXRenderInfo* prend_info);



//------------------------------------------------------------------------------
//
//    Functions
//    Caution: This API is not thread-safe!
//
//------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif

//
// target_queue_cnt : Semaphore for target is created as target_queue_cnt. But it effects only when target is display.
// display_en : Target can be display.
// camera_en : Source can be 4 channel camera.
//
NX_APICALL HGSURFCTRL nxGSurfaceCreate(unsigned int target_queue_cnt, NX_BOOL display_en,
	unsigned int cam_stride, unsigned int cam_width, unsigned int cam_height, unsigned int cam_cnt, unsigned int cam_module, NX_BOOL camera_en);
//
//
//
NX_APICALL void  nxGSurfaceDestroy(HGSURFCTRL hgsurf_ctrl);
//
// source_width_align : Source width might needs extra align offset(32X).
// source_height_align : Source height might needs extra align offset(16X).
// target_width_align : Target width might needs extra align offset(32X).
// target_height_align : Target height might needs extra align offset(16X).
//
// 1280 x 800 YUV420 => height_align can be 0. (height_align 16 is ok also)
//  _______
// |       |
// |   Y   |
// |_______|
// |   U   |
// |_______|
// |   V   |
// |_______|
//
// 1280 x 720 YUV420 => height_align chould be 16X.
//  _______
// |       |
// |   Y   |
// |_______|
// |   U   |
// |_______|
// |_______| <- height_algin dummy data
// |   V   |
// |_______|
//
//  720 * 1280 YUV420 => width_align chould be 32X.
//
// |       | |
// |   Y   | |
// |_______|_|
// |   U   | |
// |_______|_|
// |   V   | |
// |_______|_|
//

NX_APICALL NX_BOOL nxGSurfaceInitEGL(HGSURFCTRL hgsurf_ctrl, NXDisplayInfo* pdisp_info,
		NX_GSURF_VMEM_IMAGE_FORMAT_MODE source_format, unsigned int source_width_align, unsigned int source_height_align,
		NX_GSURF_VMEM_IMAGE_FORMAT_MODE target_format, unsigned int target_width_align, unsigned int target_height_align);
//
//
//
NX_APICALL void nxGSurfaceDeinitEGL(HGSURFCTRL hgsurf_ctrl);
//
//
//
NX_APICALL NX_BOOL nxGSurfaceInitGL(HGSURFCTRL hgsurf_ctrl, HGSURFTARGET htarget);
//
//
//
NX_APICALL void nxGSurfaceDeinitGL(HGSURFCTRL hgsurf_ctrl);
//
// source for memory with single dma_fd.
//
NX_APICALL HGSURFSOURCE nxGSurfaceCreateSource(HGSURFCTRL hgsurf_ctrl, unsigned int src_width, unsigned int src_height, int src_dma_fd);
//
// source for memory with Y,U,V sperated dam_fd.
//
NX_APICALL HGSURFSOURCE nxGSurfaceCreateSourceWithFDs(HGSURFCTRL hgsurf_ctrl, unsigned int tex_width, unsigned int tex_height, int* psrc_dma_fds);
//
// source for 4 camera
//
NX_APICALL HGSURFSOURCE nxGSurfaceCreateSource4ch(HGSURFCTRL hgsurf_ctrl, unsigned int src_stride, unsigned int src_width, unsigned int src_height, int src_dma_fd);
//
//
//
NX_APICALL void nxGSurfaceDestroySource(HGSURFCTRL hgsurf_ctrl, HGSURFSOURCE hsource);
//
//
//
NX_APICALL HGSURFTARGET nxGSurfaceCreateTarget(HGSURFCTRL hgsurf_ctrl, unsigned int dst_width, unsigned int dst_height, int dst_dma_fd);
//
// This is used with nxGSurfaceRenderAll4ChToEachImages().
//
NX_APICALL HGSURFTARGET nxGSurfaceCreateTargetEglImages(HGSURFCTRL hgsurf_ctrl, unsigned int dst_width, unsigned int dst_height, int* dst_dma_fds);
//
//
//
NX_APICALL HGSURFTARGET nxGSurfaceCreateTargetWithFDs(HGSURFCTRL hgsurf_ctrl, unsigned int dst_width, unsigned int dst_height, int* pdst_dma_fd);
//
//
//
NX_APICALL void nxGSurfaceDestroyTarget(HGSURFCTRL hgsurf_ctrl, HGSURFTARGET htarget);
//
// This is used with nxGSurfaceRenderAll4ChToEachImages().
//
NX_APICALL void nxGSurfaceDestroyTargetEglImages(HGSURFCTRL hgsurf_ctrl, HGSURFTARGET htarget);
//
// texture is memory
// all target format : 30fps
//
NX_APICALL NX_BOOL nxGSurfaceRender(HGSURFCTRL hgsurf_ctrl, HGSURFSOURCE hsource, HGSURFTARGET htarget,
			int x, int y, int width, int height, NX_GSURF_ROTATE_MODE rotate_mode);
//
// texture is one of 4ch cameras, ch_mode < NX_GSURF_DIRECTION_MAX (ch0 or 1 or 2 or 3 => one image)
// all target format         : 30fps
// textures are all 4ch cameras, ch_mode = NX_GSURF_DIRECTION_MAX (all ch0,1,2,3 => one quad image)
// target YUV422, RGBA       : 30fps
// target YUV420 			 : 26.27fps
// target NV21, NV12         : 26.03fps
//
NX_APICALL NX_BOOL nxGSurfaceRender4ch(HGSURFCTRL hgsurf_ctrl, HGSURFTARGET htarget,
			int x, int y, int width, int height, NX_GSURF_DIRECTION_MODE ch_mode, NX_BOOL hflip, NX_BOOL vflip);
//
// This is used with nxGSurfaceCreateTargetEglImages() and nxGSurfaceDestroyTargetEglImages().
// textures are all 4ch cameras.
// target YUV420 only : 30fps
//
NX_APICALL NX_BOOL nxGSurfaceRenderAll4ChToEachImages(HGSURFCTRL hgsurf_ctrl, HGSURFTARGET htarget,
			int x, int y, int width, int height, NX_BOOL hflip, NX_BOOL vflip);
//
//
//
NX_APICALL void nxGSurfaceUpdate(HGSURFCTRL hgsurf_ctrl);
//
// This callback is called when current H/W rendering is completed.
// This callback is valid in case of display only.
//
NX_APICALL void nxGSurfaceSetDoneCallback(HGSURFCTRL hgsurf_ctrl, NX_GSURF_DONE_CALLBACK pfunc);
//
// Call glClear and wait current done.
//
NX_APICALL void nxGSurfaceStop(HGSURFCTRL hgsurf_ctrl);
//
//
//
NX_APICALL void nxGSurfaceMakeCurrent(HGSURFCTRL hgsurf_ctrl, EGLSurface surface, unsigned int plane);
//
//
//
NX_APICALL EGLSurface nxGSurfaceGetSurface(HGSURFTARGET htarget, unsigned int chidx, unsigned int plane);
//
//
//
NX_APICALL void nxGSurfaceGetSize(HGSURFTARGET htarget, unsigned int* pwidth, unsigned int* pheight);

#ifdef __cplusplus
}
#endif

#endif
