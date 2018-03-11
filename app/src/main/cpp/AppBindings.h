#ifndef TAINTDROIDHACK_APPBINDINGS_H
#define TAINTDROIDHACK_APPBINDINGS_H

#include <android/log.h>

#define TAG "HACK"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, TAG, __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN, TAG, __VA_ARGS__)

#endif //TAINTDROIDHACK_APPBINDINGS_H
