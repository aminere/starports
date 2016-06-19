LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += \
    $(LOCAL_PATH)/../../../../extern/freetype/builds \
    $(LOCAL_PATH)/../../../../extern/freetype/include
	
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)

LOCAL_SRC_FILES:= \
    ../../../../extern/freetype/src/base/ftbbox.c \
    ../../../../extern/freetype/src/base/ftbitmap.c \
    ../../../../extern/freetype/src/base/ftglyph.c \
    ../../../../extern/freetype/src/base/ftstroke.c \
    ../../../../extern/freetype/src/base/ftxf86.c \
    ../../../../extern/freetype/src/base/ftbase.c \
    ../../../../extern/freetype/src/base/ftsystem.c \
    ../../../../extern/freetype/src/base/ftinit.c \
    ../../../../extern/freetype/src/base/ftgasp.c \
    ../../../../extern/freetype/src/raster/raster.c \
    ../../../../extern/freetype/src/sfnt/sfnt.c \
    ../../../../extern/freetype/src/smooth/smooth.c \
    ../../../../extern/freetype/src/autofit/autofit.c \
    ../../../../extern/freetype/src/truetype/truetype.c \
    ../../../../extern/freetype/src/cff/cff.c \
    ../../../../extern/freetype/src/psnames/psnames.c \
    ../../../../extern/freetype/src/pshinter/pshinter.c \
    ../../../../extern/freetype/src/type1/type1.c \
    ../../../../extern/freetype/src/cid/type1cid.c \
    ../../../../extern/freetype/src/pfr/pfr.c \
    ../../../../extern/freetype/src/type42/type42.c \
    ../../../../extern/freetype/src/winfonts/winfnt.c \
    ../../../../extern/freetype/src/pcf/pcf.c \
    ../../../../extern/freetype/src/psaux/psaux.c \
    ../../../../extern/freetype/src/bdf/bdf.c \
    ../../../../extern/freetype/src/gzip/ftgzip.c \
    ../../../../extern/freetype/src/lzw/ftlzw.c

LOCAL_CFLAGS += -DFT2_BUILD_LIBRARY
LOCAL_MODULE := freetype

include $(BUILD_STATIC_LIBRARY)