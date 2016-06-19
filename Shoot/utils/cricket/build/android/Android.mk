
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := ck
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/../../inc

ifeq ($(NDK_DEBUG),1)
LOCAL_SRC_FILES := ../../lib/android/ndk/$(TARGET_ARCH_ABI)/debug/libck.a
else
LOCAL_SRC_FILES := ../../lib/android/ndk/$(TARGET_ARCH_ABI)/release/libck.a
endif

include $(PREBUILT_STATIC_LIBRARY)

