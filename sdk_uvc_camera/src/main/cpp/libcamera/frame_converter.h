//
// Created by shengjunhu on 2022/9/1.
//

#pragma once

#ifndef ANDROID_UVC_CAMERA_FRAME_CONVERTER_H
#define ANDROID_UVC_CAMERA_FRAME_CONVERTER_H

#include "frame_decoder.h"
#include "frame_encoder.h"
#include "frame_preview.h"

#ifdef __cplusplus
extern "C" {
#endif

class frame_converter {
public:
    frame_converter(uint16_t width, uint16_t height, FrameFormat format, PixelFormat pixel);
    ~frame_converter();

    int renderPause();
    int renderFrame();
    int callbackFrame();

    int setPreview(jlong previewId);
    int decodeFrame(uint8_t *src, size_t size);
    int setCallback(JavaVM *jvm, jobject obj, jmethodID mid);

    // Buffer
    uint8_t *buffer;
    size_t buffer_size;

private:
    typedef int (frame_converter::*Callback)();
    typedef int (frame_preview::*Render)(const uint8_t *data);
    typedef int (frame_decoder::*Decoder)(uint8_t *src, size_t size, uint8_t *dst);

    int voidFun();
    int updateFrame();

    // Frame size
    uint16_t width;
    uint16_t height;

    // Decoder
    Decoder decode;
    frame_decoder *decoder;

    // Preview
    Render render;
    frame_preview *preview;

    // Callback
    JNIEnv *env;
    JavaVM *jvm;
    jobject obj;
    jmethodID mid;
    Callback callback;
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_UVC_CAMERA_FRAME_CONVERTER_H
