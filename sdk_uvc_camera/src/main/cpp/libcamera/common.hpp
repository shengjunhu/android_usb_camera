//
// Created by shengjunhu on 2022/9/1.
//

#ifndef ANDROID_UVC_CAMERA_COMMON_HPP
#define ANDROID_UVC_CAMERA_COMMON_HPP

#include <jni.h>
#include <unistd.h>
#include <libgen.h>
#include <stdlib.h>
#include <android/log.h>

#define STATUS_SUCCEED        0
#define STATUS_FAILED        -1
#define STATUS_ERROR_STEP    -2
#define STATUS_EMPTY_OBJ     -3
#define STATUS_EMPTY_DATA    -4
#define STATUS_EMPTY_PARAM   -5
#define STATUS_ERROR_PARAM   -6
#define STATUS_NONE_INIT     -7

#ifdef __cplusplus
extern "C" {
#endif

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
#define SAFE_DELETE_ARRAY(p) {if(p) { delete[](p); (p) = NULL;}}

#ifndef LOG_TAG
    #define LOG_TAG "UVCCamera"
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

#define TIME_MS()                                               \
({  struct timeval time;                                        \
    gettimeofday(&time, NULL);                                  \
    return (uint64_t) time.tv_sec * 1000 + time.tv_usec / 1000; \
})

#define SAVE_FILE(path, data, size)         \
({                                          \
    const char *_path = (const char *)path; \
    void *_data = (void *)data;             \
    size_t *_size = (size_t *)size;         \
    FILE *fp = fopen(_path, "wb");          \
    if (fp) {                               \
        fwrite(_data, 1, _size, fp);        \
        fclose(fp);                         \
    }                                       \
})

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_UVC_CAMERA_COMMON_HPP
