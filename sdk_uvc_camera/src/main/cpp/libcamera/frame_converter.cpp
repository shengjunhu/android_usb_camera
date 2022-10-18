//
// Created by shengjunhu on 2022/9/1.
//

#include <cassert>
#include "common.hpp"
#include "frame_converter.h"

#ifdef __cplusplus
extern "C" {
#endif

//==================================================================================================

frame_converter::frame_converter(uint16_t width, uint16_t height, FrameFormat format, PixelFormat pixel):
        width(width), height(height), decode(&frame_decoder::voidFun),
        preview(nullptr), render(&frame_preview::voidFun),
        jvm(nullptr), obj(nullptr), mid(nullptr),
        callback(&frame_converter::voidFun) {

    if (format == FRAME_FORMAT_YUY2) {
        if (pixel == PIXEL_FORMAT_RGB) {
            buffer_size = width * height * 3 ;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderRGB;
            decode = &frame_decoder::YUY2ToRGB;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_NV21) {
            buffer_size = width * height * 3 / 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderNV21;
            decode = &frame_decoder::YUY2ToNV21;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_YUV420P) {
            buffer_size = width * height * 3 / 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderYUV420P;
            decode = &frame_decoder::YUY2ToYUV420P;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_YUV422P) {
            buffer_size = width * height * 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderYUV422P;
            decode = &frame_decoder::YUY2ToYUV422P;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_DEPTH16) {
            buffer_size = width * height * 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderDEPTH16;
            decode = &frame_decoder::FrameToCopy;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_YUY2) {
            buffer_size = width * height * 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderYUY2;
            decode = &frame_decoder::FrameToCopy;
            decoder = new frame_decoder(width, height, format, pixel);
        } else {
            buffer_size = 0;
            LOGE("FrameConverter: frame format(%d) is unsupported to convert pixel(%d).", format, pixel);
        }
    } else if (format == FRAME_FORMAT_MJPG) {
        if (pixel == PIXEL_FORMAT_RGB) {
            buffer_size = width * height * 3 ;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderRGB;
            decode = &frame_decoder::MJPGToRGB;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_NV21) {
            buffer_size = width * height * 3 / 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderNV21;
            decode = &frame_decoder::MJPGToNV21;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_YUV420P) {
            buffer_size = width * height * 3 / 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderYUV420P;
            decode = &frame_decoder::MJPGToYUV420P;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_YUV422P){
            buffer_size = width * height * 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderYUV422P;
            decode = &frame_decoder::MJPGToYUV422P;
            decoder = new frame_decoder(width, height, format, pixel);
        } else {
            buffer_size = 0;
            LOGE("FrameConverter: frame format(%d) is unsupported to convert pixel(%d).", format, pixel);
        }
    } else if (format == FRAME_FORMAT_H264) {
        if (pixel == PIXEL_FORMAT_RGB) {
            buffer_size = width * height * 3 ;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderRGB;
            decode = &frame_decoder::H264ToRGB;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_NV21) {
            buffer_size = width * height * 3 / 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderNV21;
            decode = &frame_decoder::H264ToNV21;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_YUV420P) {
            buffer_size = width * height * 3 / 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderYUV420P;
            decode = &frame_decoder::H264ToYUV420P;
            decoder = new frame_decoder(width, height, format, pixel);
        } else if (pixel == PIXEL_FORMAT_YUV422P) {
            buffer_size = width * height * 2;
            buffer = (uint8_t *) malloc(buffer_size);
            assert(buffer != nullptr);
            render = &frame_preview::renderYUV422P;
            decode = &frame_decoder::H264ToYUV422P;
            decoder = new frame_decoder(width, height, format, pixel);
        } else {
            buffer_size = 0;
            LOGE("FrameConverter: frame format(%d) is unsupported to convert pixel(%d).", format, pixel);
        }
    } else {
        // Frame format: FRAME_FORMAT_UNKNOWN
        buffer_size = width * height * 2;
        buffer = (uint8_t *) malloc(buffer_size);
        assert(buffer != nullptr);
        render = &frame_preview::renderYUY2;
        decode = &frame_decoder::FrameToCopy;
        decoder = new frame_decoder(width, height, format, pixel);
        LOGW("FrameConverter: frame format(%d) is unknown.", format);
    }

}

frame_converter::~frame_converter() {
    SAFE_FREE(buffer);
    SAFE_DELETE(decoder);
    SAFE_DELETE(preview);
    if(jvm != nullptr && obj != nullptr){
        if (JNI_OK == jvm->GetEnv((void**)&env, JNI_VERSION_1_6)){
            env->DeleteGlobalRef(obj);
        }
        env = nullptr;
        obj = nullptr;
        mid = nullptr;
    }
    decode = nullptr;
    render = nullptr;
    callback = nullptr;
}

//==================================================================================================

int frame_converter::voidFun(){
    return STATUS_SUCCEED;
}

int frame_converter::updateFrame(){
    jvm->AttachCurrentThread(&env, nullptr);
    env->CallVoidMethod(obj, mid);
    jvm->DetachCurrentThread();
    return STATUS_SUCCEED;
}

//==================================================================================================

int frame_converter::renderPause(){
    return this->preview->renderPause();
}

int frame_converter::renderFrame(){
    return (this->preview->*render)(buffer);
}

int frame_converter::callbackFrame(){
    return (this->*callback)();
}

int frame_converter::decodeFrame(uint8_t *src, size_t size){
    return (this->decoder->*decode)(src, size, buffer);
}

//==================================================================================================

int frame_converter::setPreview(jlong previewId){
    SAFE_DELETE(this->preview);
    this->preview = reinterpret_cast<frame_preview *>(previewId);
    return STATUS_SUCCEED;
}

int frame_converter::setCallback(JavaVM *_jvm, jobject _obj, jmethodID _mid){
    if (this->jvm != nullptr) {
        if (JNI_OK == this->jvm->GetEnv((void**)&env, JNI_VERSION_1_6)){
            env->DeleteGlobalRef(this->obj);
        }
        this->obj = nullptr;
        this->mid = nullptr;
    }
    if (_jvm != nullptr && _obj != nullptr && _mid != nullptr){
        this->callback = &frame_converter::updateFrame;
        this->jvm = _jvm;
        this->obj = _obj;
        this->mid = _mid;
        return STATUS_SUCCEED;
    } else {
        return STATUS_FAILED;
    }
}

//==================================================================================================

#ifdef __cplusplus
}  // extern "C"
#endif

