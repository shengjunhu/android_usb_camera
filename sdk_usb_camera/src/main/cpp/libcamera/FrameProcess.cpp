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

FrameProcess::FrameProcess(jint width, jint height, jint format, jint pixel):
    jvm(nullptr), obj(nullptr), mid(nullptr), pixel_size(0),
    decode(nullptr), render(nullptr), preview(nullptr) {
    buffer_size = width * height * 2;
    buffer = (uint8_t *) malloc(buffer_size);
    if (format == FRAME_FORMAT_YUY2){
        if (pixel == PIXEL_FORMAT_YUV420){
            pixel_size = buffer_size * 3 / 4 ;
            decode = &FrameDecoder::YUY2ToYUV420;
            decoder = new FrameDecoder(width, height);
        } else {
            pixel_size = buffer_size;
            decode = &FrameDecoder::YUY2ToCopy;
            decoder = new FrameDecoder(width, height);
        }
    } else if (format == FRAME_FORMAT_MJPG){
        if (pixel == PIXEL_FORMAT_YUV420){
            pixel_size = buffer_size * 3 / 4 ;
            decode = &FrameDecoder::MJPGToYUV420;
            decoder = new FrameDecoder(width, height);
        } else if (pixel == PIXEL_FORMAT_YUY2){
            pixel_size = buffer_size;
            decode = &FrameDecoder::MJPGToYUY2;
            decoder = new FrameDecoder(width, height);
        }
    } else if (format == FRAME_FORMAT_H264){
        if (pixel == PIXEL_FORMAT_YUV420){
            pixel_size = buffer_size * 3 / 4 ;
            decode = &FrameDecoder::H264ToYUV420;
            decoder = new FrameDecoder(width, height);
        } else if (pixel == PIXEL_FORMAT_YUY2){
            pixel_size = buffer_size;
            decode = &FrameDecoder::H264ToYUY2;
            decoder = new FrameDecoder(width, height);
        }
    } else {
        LOGE("Frame format error: %d", format);
    }
}

FrameProcess::~FrameProcess() {

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

int FrameProcess::setPreview(IPreview *_preview){
    SAFE_DELETE(preview);
    int ret = STATUS_SUCCESS;
    this->preview = _preview;
    if (pixel == PIXEL_FORMAT_YUV420){
        render = &IPreview::renderYUV420;
    } else if (pixel == PIXEL_FORMAT_YUY2){
        render = &IPreview::renderYUYV;
    } else if (pixel == PIXEL_FORMAT_DEPTH){
        render = &IPreview::renderDEPTH;
    } else {
        LOGE("Pixel format error: %d", pixel);
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
            LOGI("JavaCallback: DeleteGlobalRef");
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
