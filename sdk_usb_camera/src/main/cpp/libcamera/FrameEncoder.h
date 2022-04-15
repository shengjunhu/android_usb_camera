//
// Created by Hsj on 2022/4/12.
//

#ifndef ANDROID_USB_CAMERA_FRAMEENCODER_H
#define ANDROID_USB_CAMERA_FRAMEENCODER_H

#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    VIDEO_FORMAT_UNKNOWN    = 0x00, //UNKNOWN
    VIDEO_FORMAT_MP4        = 0x06, //MP4.
    VIDEO_FORMAT_H264       = 0x07, //H264.
} VideoFormat;

class FrameEncoder {
public:
    FrameEncoder(uint16_t width, uint16_t height);
    ~FrameEncoder();
    int YUVToMP4(uint8_t *src, size_t size, uint8_t *dst);
    int YUVToH264(uint8_t *src, size_t size, uint8_t *dst);
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_FRAMEENCODER_H
