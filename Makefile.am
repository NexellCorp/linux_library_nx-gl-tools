AM_CPPFLAGS = \
	$(WARN_CFLAGS) \
	-fPIC \
	-I./include

LDFLAGS += \
	-ldrm -lnx_v4l2 -lnx_renderer \
	-L./lib/linux -lnxgpusurf

DEFS += \
	-DGL_GLEXT_PROTOTYPES \
	-DNX_PLATFORM_DRM_USER_ALLOC_USE

libnx_gl_tools_la_LDFLAGS = \
	-ldrm -lnx_v4l2 -lnx_renderer \
	-L./lib/linux -lnxgpusurf

libnx_gl_tools_la_LTLIBRARIES = libnx_gl_tools.la
libnx_gl_tools_ladir = ${SDKTARGETSYSROOT}/usr/lib

libnx_gl_tools_la_SOURCES = \
	nx_gl_tools.cpp

libnx_gl_tools_includedir = ${SDKTARGETSYSROOT}/usr/include
libnx_gl_tools_include_HEADERS = \
	nx_gl_tools.h
