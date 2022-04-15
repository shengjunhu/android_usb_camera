//
// Created by Hsj on 2022/4/13.
//

#include <malloc.h>
#include "Common.h"
#include "FrameProcess.h"

#ifdef __cplusplus
extern "C" {
#endif

//==================================================================================================

FrameProcess::FrameProcess(jint width, jint height, jint format, jint _pixel):
    width(width), height(height), jvm(nullptr), obj(nullptr), mid(nullptr),
    pixel_size(0), decode(nullptr), render(nullptr), preview(nullptr) {
    pixel = PixelFormat(_pixel);
    buffer_size = width * height * 2;
    buffer = (uint8_t *) malloc(buffer_size);
    if (format == FRAME_FORMAT_YUY2){
        if (pixel == PIXEL_FORMAT_I420){
            pixel_size = buffer_size * 3 / 4 ;
            decode = &FrameDecoder::YUY2ToI420;
        } else {
            pixel_size = buffer_size;
            decode = &FrameDecoder::YUY2ToCopy;
        }
        decoder = new FrameDecoder(width, height);
    } else if (format == FRAME_FORMAT_MJPG){
        pixel_size = buffer_size * 3 / 4 ;
        decode = &FrameDecoder::MJPGToI420;
        decoder = new FrameDecoder(width, height);
    } else if (format == FRAME_FORMAT_H264){
        pixel_size = buffer_size * 3 / 4 ;
        decode = &FrameDecoder::H264ToI420;
        decoder = new FrameDecoder(width, height);
    } else {
        LOGE("Frame format error: %d", format);
    }
}

FrameProcess::~FrameProcess() {
    decode = nullptr;
    render = nullptr;
    SAFE_FREE(buffer);
    SAFE_DELETE(decoder);
    SAFE_DELETE(preview);
    if(jvm != nullptr && obj != nullptr){
        JNIEnv *env;
        if (JNI_OK == jvm->GetEnv((void**)&env, JNI_VERSION_1_6)){
            env->DeleteGlobalRef(obj);
        }
        obj = nullptr;
        mid = nullptr;
    }
}

//==================================================================================================

int FrameProcess::renderFrame(uint8_t *src){
    if (preview != nullptr){
        return (preview->*render)(src);
    } else {
        LOGE("decodeFrame failed.");
        return STATUS_NONE_INIT;
    }
}

int FrameProcess::setPreview(ANativeWindow *window){
    int ret = STATUS_SUCCESS;
    SAFE_DELETE(preview);
    if(window != nullptr){
        LOGI("setPreview: window is nullptr");
    } else if (pixel == PIXEL_FORMAT_I420){
        render = &IPreview::renderI420;
#ifdef ON_GL_PREVIEW
        this->preview = new GLESPreview(width, height, pixel, window);
#else
        this->preview = new NativePreview(width, height, pixel, window);
#endif
    } else if (pixel == PIXEL_FORMAT_DEPTH){
        render = &IPreview::renderDepth;
#ifdef ON_GL_PREVIEW
        this->preview = new GLESPreview(width, height, pixel, window);
#else
        this->preview = new NativePreview(width, height, pixel, window);
#endif
    } else {
        LOGE("PixelFormat error: %d", pixel);
        ret = STATUS_EXE_FAILED;
    }
    return ret;
}

int FrameProcess::decodeFrame(uint8_t *src, size_t size, uint8_t *dst){
    if (pixel_size != 0){
        return (decoder->*decode)(src, size, dst);
    } else {
        LOGE("decodeFrame failed.");
        return STATUS_NONE_INIT;
    }
}

int FrameProcess::setJavaObj(JavaVM* _jvm, jobject _obj, jmethodID _mid){
    if (this->jvm != nullptr) {
        JNIEnv *env;
        if (JNI_OK == this->jvm->GetEnv((void**)&env, JNI_VERSION_1_6)){
            env->DeleteGlobalRef(this->obj);
            this->obj = nullptr;
            this->mid = nullptr;
        }
    }
    this->jvm = _jvm;
    this->obj = _obj;
    this->mid = _mid;
    return STATUS_SUCCESS;
}

#ifdef __cplusplus
}  // extern "C"
#endif
