//
// Created by wanglizheng on 2019/3/20.
//

#ifndef ANDROID_PROJECT_LOG_H
#define ANDROID_PROJECT_LOG_H

#include <android/log.h>
#define LOG_TAG "longxiang"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

#if DEBUG
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)
#else
#define LOGD(...)
#endif

#if VERBOSE
#define LOGV(...) __android_log_print(ANDROID_LOG_VERBOSE, LOG_TAG, __VA_ARGS__)
#else
#define LOGV(...)
#endif



#endif //ANDROID_PROJECT_LOG_H
