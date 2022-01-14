//
// Created by Hsj on 2022/1/13.
//

#ifndef ANDROID_UVCCAMERA2_COMMON_H
#define ANDROID_UVCCAMERA2_COMMON_H

#include <jni.h>
#include <unistd.h>
#include <libgen.h>
#include <android/log.h>

#ifdef __cplusplus
extern "C" {
#endif

void setVM(JavaVM *);
JavaVM *getVM();
JNIEnv *getEnv();
uint64_t timeMs();

#ifdef __GNUC__
    #define LIKELY(X) __builtin_expect(!!(X), 1)
    #define UNLIKELY(X) __builtin_expect(!!(X), 0)
    #define CONDITION(cond) __builtin_expect(cond!=0, 0)
#else
    #define LIKELY(X) (X)
    #define UNLIKELY(X) (X)
    #define	CONDITION(cond)	(cond)
#endif

#define SAFE_FREE(x) {if(x != NULL){free(x); x = NULL;}}
#define SAFE_DELETE(p) {if(p) { delete (p); (p) = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p) { delete [](p); (p) = NULL;}}

#ifndef LOG_TAG
    #define LOG_TAG "UVCCamera"
#endif

#ifdef LOG_SWITCH
    #define LOGI(FMT,...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[%d*%s:%d:%s]:" FMT,	\
							gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
    #define LOGD(FMT,...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%d*%s:%d:%s]:" FMT,\
							gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
    #define LOGW(FMT,...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "[%d*%s:%d:%s]:" FMT,	\
							gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
    #define LOGE(FMT,...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "[%d*%s:%d:%s]:" FMT,\
							gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
    #define LOGI(...)
    #define LOGD(...)
    #define LOGW(...)
    #define LOGE(...)
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_UVCCAMERA2_COMMON_H
