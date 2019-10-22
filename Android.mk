LOCAL_PATH		:= $(call my-dir)

NX_INC_TOP		:= $(LOCAL_PATH)/../include
NX_LIB_TOP		:= $(LOCAL_PATH)/../lib/android

include $(CLEAR_VARS)
LOCAL_MODULE    := libnxgpusurf
LOCAL_MODULE_CLASS := STATIC_LIBRARIES
LOCAL_MODULE_SUFFIX := .a
LOCAL_C_INCLUDE := $(NX_INC_TOP)
LOCAL_SRC_FILES := /../lib/android/libnxgpusurf_dbg.a
include $(BUILD_PREBUILT)


include $(CLEAR_VARS)

#########################################################################
#																		#
#								Includes								#
#																		#
#########################################################################
LOCAL_C_INCLUDES +=	\
	external/libdrm \
	hardware/nexell/s5pxx18/gralloc	\
	$(NX_INC_TOP) \
	$(NX_INC_TOP)/drm \
	$(LOCAL_PATH)

#########################################################################
#																		#
#								Sources									#
#																		#
#########################################################################
LOCAL_SRC_FILES		+=		\
	common.cpp \
	main.cpp \
	main_cam_cvt.cpp \
	main_cam_cvt_sequence.cpp \
	main_mem_cvt.cpp \
	main_mem_cvt_format.cpp

#########################################################################
#																		#
#								Library									#
#																		#
#########################################################################

#LOCAL_FORCE_STATIC_EXECUTABLE := true

LOCAL_STATIC_LIBRARIES += liblog
LOCAL_STATIC_LIBRARIES += libnxgpusurf
LOCAL_STATIC_LIBRARIES += libdrm libnx_renderer libnx_v4l2

LOCAL_LDLIBS += -lGLES_mali
#LOCAL_SHARED_LIBRARIES += libGLES_mali


#########################################################################
#																		#
#								Target									#
#																		#
#########################################################################
LOCAL_MODULE		:= NxGpuSurf
LOCAL_MODULE_TAGS	:= optional
LOCAL_CFLAGS 		:= -DANDROID -pthread -DGL_GLEXT_PROTOTYPES -DNX_PLATFORM_DRM_USER_ALLOC_USE -DNX_DEBUG
LOCAL_MODULE_PATH := $(TARGET_ROOT_OUT_SBIN)
LOCAL_UNSTRIPPED_PATH := $(TARGET_ROOT_OUT_SBIN_UNSTRIPPED)
LOCAL_POST_INSTALL_CMD := $(hide) mkdir -p $(TARGET_ROOT_OUT)/sbin

include $(BUILD_EXECUTABLE)
