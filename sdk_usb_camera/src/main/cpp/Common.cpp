//
// Created by Hsj on 2022/1/13.
//

#include <ctime>
#include <iostream>
#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

static JavaVM *jvm;

JNIEnv *getEnv() {
    JNIEnv *env = nullptr;
    if (jvm) {
        jint ret = jvm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
        if (JNI_OK == ret) {
            LOGD("getEnv succeed");
        } else {
            LOGE("getEnv failed: %d", ret);
        }
    } else {
        LOGE("getEnv failed: jvm is null");
    }
    return env;
}

JavaVM *getJVM() {
    return jvm;
}

void setJVM(JavaVM *vm) {
    if (jvm) SAFE_DELETE(jvm);
    jvm = vm;
}

uint64_t timeMs() {
    struct timeval time;
    gettimeofday(&time, NULL);
    return (uint64_t) time.tv_sec * 1000 + time.tv_usec / 1000;
}

void saveFile(const char *path, void *data, size_t size) {
    FILE *fp = fopen(path, "wb");
    if (fp) {
        fwrite(data, 1, size, fp);
        fclose(fp);
    }
}

#ifdef __cplusplus
}  // extern "C"
#endif