//
// Created by shengjunhu on 2022/9/1.
//

#pragma once

#ifndef ANDROID_UVC_CAMERA_FRAME_PREVIEW_H
#define ANDROID_UVC_CAMERA_FRAME_PREVIEW_H

#include <android/native_window_jni.h>

class frame_preview {
public:
    virtual ~frame_preview() = default;

    virtual int initCondition() = 0;

    virtual int setNativeWindow(ANativeWindow *window) = 0;

    virtual int setTranspose(const uint16_t rotate, const uint8_t flip) = 0;

    virtual int setViewSize(const uint16_t width, const uint16_t height) = 0;

    virtual int setFrameSize(const uint16_t width, const uint16_t height) = 0;

    virtual int renderPause() = 0;

    virtual int voidFun(const uint8_t *data) = 0;

    virtual int renderRGB(const uint8_t *data) = 0;

    virtual int renderNV21(const uint8_t *data) = 0;

    virtual int renderYUV420P(const uint8_t *data) = 0;

    virtual int renderYUV422P(const uint8_t *data) = 0;

    virtual int renderDEPTH16(const uint8_t *data) = 0;

    virtual int renderYUY2(const uint8_t *data) = 0;
};

#endif //ANDROID_UVC_CAMERA_FRAME_PREVIEW_H
