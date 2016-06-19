
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := zlib

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../src/zlib
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
#LOCAL_LDLIBS    := -llog

LOCAL_SRC_FILES := \
../../../src/zlib/adler32.c \
../../../src/zlib/compress.c \
../../../src/zlib/crc32.c \
../../../src/zlib/deflate.c \
../../../src/zlib/gzclose.c \
../../../src/zlib/gzlib.c \
../../../src/zlib/gzread.c \
../../../src/zlib/infback.c \
../../../src/zlib/inffast.c \
../../../src/zlib/inflate.c \
../../../src/zlib/inftrees.c \
../../../src/zlib/trees.c \
../../../src/zlib/uncompr.c \
../../../src/zlib/zutil.c

include $(BUILD_STATIC_LIBRARY)

