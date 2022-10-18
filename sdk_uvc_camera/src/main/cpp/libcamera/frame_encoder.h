//
// Created by shengjunhu on 2022/9/1.
//

#pragma once

#ifndef ANDROID_UVC_CAMERA_FRAME_ENCODER_H
#define ANDROID_UVC_CAMERA_FRAME_ENCODER_H

#include "frame_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

class frame_encoder {
public:
    frame_encoder(uint16_t width, uint16_t height, PixelFormat pixel, VideoFormat format);
    ~frame_encoder();

    int RGBToMP4(uint8_t  *src, size_t size, uint8_t *dst);
    int NV21ToMP4(uint8_t *src, size_t size, uint8_t *dst);
    int YUY2ToMp4(uint8_t *src, size_t size, uint8_t *dst);

    int RGBToH264(uint8_t  *src, size_t size, uint8_t *dst);
    int NV21ToH264(uint8_t *src, size_t size, uint8_t *dst);
    int YUY2ToH264(uint8_t *src, size_t size, uint8_t *dst);

    int H264ToCopy(uint8_t *src, size_t size, uint8_t *dst);
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_UVC_CAMERA_FRAME_ENCODER_H
