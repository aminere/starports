LOCAL_PATH := $(call my-dir)

#-------------------------------------------------------------
# specify libck as a "prebuilt" static library

include $(CLEAR_VARS)
LOCAL_MODULE := ck

ifeq ($(NDK_DEBUG),1)
LOCAL_SRC_FILES := ../../../../../lib/android/ndk/$(TARGET_ARCH_ABI)/debug/libck.a
else
LOCAL_SRC_FILES := ../../../../../lib/android/ndk/$(TARGET_ARCH_ABI)/release/libck.a
endif

include $(PREBUILT_STATIC_LIBRARY)

#-------------------------------------------------------------

include $(CLEAR_VARS)

LOCAL_MODULE := game

LOCAL_SRC_FILES := \
    bindings.cpp \
    ../../cube.cpp \
    ../../game.cpp \
    ../../gl.cpp \
    ../../groundplane.cpp \
    ../../ship.cpp \
    ../../timer.cpp \

LOCAL_LDLIBS := -lGLESv1_CM -llog
LOCAL_STATIC_LIBRARIES := ck cpufeatures

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../../../../../inc 

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/cpufeatures)
