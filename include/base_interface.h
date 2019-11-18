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
#ifndef __NX_BASE_INTERFACE_H__
#define __NX_BASE_INTERFACE_H__

//------------------------------------------------------------------------------
//
//    Includes
//    
//------------------------------------------------------------------------------
#if 1
#if defined(linux)
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>		// for open/close
#include <fcntl.h>		// for O_RDWR
#include <sys/ioctl.h>	// for ioctl
#include <sys/types.h>	// for mmap options
#include <sys/mman.h>	// for mmap options 
#elif defined(WIN32)
#include <windows.h>
#endif
#endif
#include <math.h>
#include <assert.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <inttypes.h>
#if defined(WIN32)
#pragma warning(disable: 4201)  
#pragma warning(disable: 4996)
#pragma warning(disable: 4127)  
#endif



//------------------------------------------------------------------------------
//
//    Defines
//    
//------------------------------------------------------------------------------
#if 1
  #ifdef __cplusplus
#define BASE_API		extern "C"
  #else
#define BASE_API		extern
  #endif //__cplusplus
#else
#define BASE_API  
#endif //BASE_API


#if defined( WIN32 )
	#define NX_INLINE			__inline
	#define NX_STATIC_INLINE	static __inline
	#define NX_LOCAL
#else
	#define NX_INLINE			inline
	#define NX_STATIC_INLINE	static inline
	#define NX_LOCAL 			__attribute__ ((visibility ("internal")))
#endif

#if defined(_WIN32)
    #define NX_CALLBACK     __stdcall
#else
    #define NX_CALLBACK
#endif

#if defined(NX_API_USE_DLL)
#if defined(NX_API_DLL_EXPORTS)
#   define NX_APICALL __declspec(dllexport)
#else
#   define NX_APICALL __declspec(dllimport)
#endif
#else
#   define NX_APICALL
#endif

#if defined(NX_DEVDRV_USE_DLL)
#if defined(NX_DEVDRV_DLL_EXPORTS)
#   define NX_DEVCALL __declspec(dllexport)
#else
#   define NX_DEVCALL __declspec(dllimport)
#endif
#else
#   define NX_DEVCALL
#endif

#if defined( WIN32 )
typedef __int64 NX_LONG;
typedef unsigned __int64 NX_ULONG;
#else
typedef long long NX_LONG;
typedef unsigned long long NX_ULONG;
#endif

typedef unsigned int 		NX_BOOL;
typedef int	                NX_F24;
typedef unsigned short	    NX_F16;
typedef signed char	    	NX_BYTE;
typedef unsigned char		NX_UBYTE;
typedef short	        	NX_SHORT;
typedef unsigned short		NX_USHORT;
typedef int	            	NX_INT;
typedef unsigned int		NX_UINT;
typedef void	        	NX_VOID;
typedef float	        	NX_FLOAT;
typedef int	            	NX_FIXED;
#if defined( WIN32 )
typedef __int64 			NX_LONG;
typedef unsigned __int64 	NX_ULONG;
#else
typedef long long 			NX_LONG;
typedef unsigned long long 	NX_ULONG;
#endif

typedef char               NX_S8 ;
typedef unsigned char      NX_U8 ;
typedef short              NX_S16;
typedef unsigned short     NX_U16;
typedef int                NX_S32;
typedef unsigned int       NX_U32;

#if defined( WIN32 )
typedef __int64            NX_S64;
typedef unsigned __int64   NX_U64;
#else
typedef long long          NX_S64;
typedef unsigned long long NX_U64;
#endif

#ifndef INT64_MAX
#define INT64_MAX 0x7fffffffffffffffLL
#endif

#define NX_TRUE			1
#define NX_FALSE		0

#define NX_STRING_MAX	256
#define KB 1024
#define MB (KB*KB)

#define NX_ADDR_BITS	32

//#define NX_BASE_USE_VMEM_LINK_FILESYSTEM
//#define _T(p_const_str)	p_const_str
#define NX_CASSERT_STATIC(expr)    typedef char __NX_C_ASSERT_STATIC__[(expr)?1:-1]
NX_CASSERT_STATIC( sizeof(NX_S8 )  == 1 );
NX_CASSERT_STATIC( sizeof(NX_U8 )  == 1 );
NX_CASSERT_STATIC( sizeof(NX_S16)  == 2 );
NX_CASSERT_STATIC( sizeof(NX_U16)  == 2 );
NX_CASSERT_STATIC( sizeof(NX_S32)  == 4 );
NX_CASSERT_STATIC( sizeof(NX_U32)  == 4 );
NX_CASSERT_STATIC( sizeof(NX_S64)  == 8 );
NX_CASSERT_STATIC( sizeof(NX_U64)  == 8 );
NX_CASSERT_STATIC( sizeof(NX_BOOL) == 4 );

//Align a positive integer to the nearest equal or higher multiple of some base.
#define NX_ALIGN( value, base ) (((value) + ((base) - 1)) & ~((base) - 1))

#define MAX_INT32  				(0x7fffffff)
#define MIN_INT32  				(-0x7fffffff-1)

#define NX_ABS(x)				((x) < 0) ? (-x) : (x)
#define	NX_MAX(x, y)			(((x) > (y)) ? (x) : (y))
#define	NX_MIN(x, y)			(((x) < (y)) ? (x) : (y))
#define	NX_CLAMP(x, min, max)	(((x) < (min)) ? (min) : ((x) > (max)) ? (max) : (x))
#define	NX_LOW_32_OF_64(x)		(*((unsigned int*)(&x)))
#define	NX_LOW_32_FROM_PTR(x)	((unsigned int)((NX_ULONG)(x) & 0xFFFFFFFFUL))

/* Branch hint */
#if defined(_MSC_VER)
#define __builtin_expect(expr,b) expr
#endif
#define LIKELY(x)       __builtin_expect((x),1)
#define UNLIKELY(x)     __builtin_expect((x),0)

/*! Stringify macros */
#define JOIN(X, Y) _DO_JOIN(X, Y)
#define _DO_JOIN(X, Y) _DO_JOIN2(X, Y)
#define _DO_JOIN2(X, Y) X##Y

#if defined( WIN32 )
#define NX_ATTR_ALIGN
#else
#define NX_ATTR_ALIGN 	__attribute__ ((packed))
#endif

/* Number of DWORDS */
#define NX_SIZEOF32(X) (sizeof(X) / sizeof(uint32_t))

//	return code
typedef enum 
{
	NX_ERR_NO_ERROR,
	NX_ERR_FAILED				= -1,
	NX_ERR_OUT_OF_MEM			= -2,
	NX_ERR_OUT_OF_VMEM 			= -3,
	NX_ERR_INVALID_ARG			= -4,
	NX_ERR_TIMEOUT				= -5,
	NX_ERR_OS_SYSCALL_FAILED	= -6,
	NX_ERR_NOT_READY_YET		= -7,
	NX_ERR_ASSERT				= -8, // very critical error
}NX_BASE_DBG_RET_TYPE;



//------------------------------------------------------------------------------
//    VMEM Platrom Defines
//------------------------------------------------------------------------------
#ifdef WIN32
#define NX_FEATURE_PLATFORM_SW_ALLOC_USE
#else
/* source */
#if !defined( NX_PLATFORM_ION_ALLOC_USE ) && !defined( NX_PLATFORM_DRM_ALLOC_USE ) && !defined( NX_PLATFORM_DRM_USER_ALLOC_USE )
#error "unvalid define. set define at your Makefile"
#endif
#endif

#if defined( linux ) && !defined( ANDROID )
/* destination drm display device */
#define NX_PLATFORM_DST_DRM_DISP_USE
#endif

#if defined( NX_PLATFORM_DRM_ALLOC_USE )
#include <fcntl.h>
#include <gbm.h>
#include <drm/drm_fourcc.h>
#include <nx_video_alloc.h>
#elif defined( NX_PLATFORM_DRM_USER_ALLOC_USE )
typedef struct tagNxDrmVmem
{
	int drm_fd;
	int gem_fd;
	int flinks;
	int dma_fd;
	void *pBuffer;
	unsigned int size;
}NxDrmVmem;
#include <fcntl.h>
#include <gbm.h>
#include <drm/drm_fourcc.h>
#define DRM_DEVICE_NAME "/dev/dri/card0"
typedef NxDrmVmem* NX_MEMORY_HANDLE_INTERNAL;

#elif defined( NX_PLATFORM_ION_ALLOC_USE )

#include <nx_alloc_mem.h>
typedef fbdev_window NX_PLATFORM_WINDOW_STRUCT;

#else
#error "choose valid vmem allocator"
#endif


#endif
