APP_ABI :=armeabi-v7a
#APP_ABI :=all
APP_PLATFORM := android-19
NDK_TOOLCHAIN_VERSION:=4.9

#APP_STL:= gnustl_shared
APP_STL:= gnustl_static
#APP_STL:= c++_static
#APP_STL := stlport_static

NDK_TOOLCHAIN_VERSION := clang
APP_CPPFLAGS := -fexceptions
APP_CPPFLAGS := -std=c++11
