//
// Created by Hsj on 2022/1/13.
//

#ifndef ANDROID_USB_CAMERA_COMMON_H
#define ANDROID_USB_CAMERA_COMMON_H

#include <jni.h>
#include <unistd.h>
#include <libgen.h>
#include <android/log.h>

#define STATUS_NONE_INIT     -9
#define STATUS_EMPTY_PARAM   -8
#define STATUS_EMPTY_DATA    -7
#define STATUS_ERROR_NONE    -6
#define STATUS_ERROR_STEP    -5
#define STATUS_EMPTY_OBJ     -4
#define STATUS_EXE_FAILED    -3
#define STATUS_SUCCESS        0

#ifdef __cplusplus
extern "C" {
#endif

JNIEnv *getEnv();
JavaVM *getJVM();
void setJVM(JavaVM *vm);
uint64_t timeMs();
void saveFile(const char *path, void *data, size_t size);

#ifdef __GNUC__
    #define LIKELY(X) __builtin_expect(!!(X), 1)
    #define UNLIKELY(X) __builtin_expect(!!(X), 0)
    #define CONDITION(cond) __builtin_expect(cond!=0, 0)
#else
    #define LIKELY(X) (X)
    #define UNLIKELY(X) (X)
    #define	CONDITION(cond)	(cond)
#endif

#define DL_FOREACH(head,el) for(el=head;el;el=el->next)
#define SAFE_FREE(x) {if(x != NULL){free(x); x = NULL;}}
#define SAFE_DELETE(p) {if(p) { delete (p); (p) = NULL;}}
#define SAFE_DELETE_ARRAY(p) {if(p) { delete [](p); (p) = NULL;}}

#ifndef LOG_TAG
    #define LOG_TAG "UsbCamera"
#endif

#ifdef LOG_E
    #define LOGE(FMT,...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, "[%d*%s:%d:%s]:" FMT,	\
							gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
    #define LOGE(...)
#endif

#ifdef LOG_W
    #define LOGW(FMT,...) __android_log_print(ANDROID_LOG_WARN, LOG_TAG, "[%d*%s:%d:%s]:" FMT,	\
							gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
    #define LOGW(...)
#endif

#ifdef LOG_I
    #define LOGI(FMT,...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, "[%d*%s:%d:%s]:" FMT,	\
							gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
    #define LOGI(...)
#endif

#ifdef LOG_D
    #define LOGD(FMT,...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, "[%d*%s:%d:%s]:" FMT,	\
							gettid(), basename(__FILE__), __LINE__, __FUNCTION__, ## __VA_ARGS__)
#else
    #define LOGD(...)
#endif

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_COMMON_H
