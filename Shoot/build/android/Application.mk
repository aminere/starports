NDK_TOOLCHAIN_VERSION := 4.8
APP_PLATFORM := android-11
APP_STL := gnustl_static
APP_CPPFLAGS += -std=c++11 -fexceptions
LOCAL_C_INCLUDES += ${ANDROID_NDK}/sources/cxx-stl/gnu-libstdc++/4.8/include
APP_ABI := armeabi-v7a #armeabi
APP_OPTIM := debug