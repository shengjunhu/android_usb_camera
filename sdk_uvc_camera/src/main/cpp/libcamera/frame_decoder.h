//
// Created by shengjunhu on 2022/9/1.
//

#pragma once

#ifndef ANDROID_UVC_CAMERA_FRAME_DECODER_H
#define ANDROID_UVC_CAMERA_FRAME_DECODER_H

#include <unistd.h>
#include <turbojpeg.h>

#ifdef __cplusplus
extern "C" {
#endif

//FrameFormat: These match the enums in java/com/hsj/camera/UVCCamera.FrameFormat
typedef enum {
    FRAME_FORMAT_YUY2     = 0x04,
    FRAME_FORMAT_MJPG     = 0x06,
    FRAME_FORMAT_H264     = 0x10,
    FRAME_FORMAT_UNKNOWN  = 0x00,
} FrameFormat;

//PixelFormat: These match the enums in java/com/hsj/camera/UVCCamera.PixelFormat
typedef enum {
    PIXEL_FORMAT_RGB      = 0x25, //RGB :    R8B8G8
    PIXEL_FORMAT_NV21     = 0x24, //NV21:    YYYYYYYY VUVU
    PIXEL_FORMAT_YUV420P  = 0x23, //YUV420P: YYYY U V
    PIXEL_FORMAT_YUV422P  = 0x22, //YUV422P: YYYY UU VV
    PIXEL_FORMAT_DEPTH16  = 0x21, //DEPTH16: unsigned short
    PIXEL_FORMAT_YUY2     = 0x20, //YUYV:    YUYV YUYV
    PIXEL_FORMAT_NONE     = 0x00, //NONE:    No Pixel
} PixelFormat;

//PixelFlip: These match the enums in java/com/hsj/camera/UVCCamera.PixelFlip
typedef enum {
    PIXEL_FLIP_H          = 0x01,
    PIXEL_FLIP_V          = 0x02,
    PIXEL_FLIP_NONE       = 0x00,
} PixelFlip;

//PixelRotate: These match the enums in java/com/hsj/camera/UVCCamera.PixelRotate
typedef enum {
    PIXEL_ROTATE_90       = 0x5A,
    PIXEL_ROTATE_180      = 0xB4,
    PIXEL_ROTATE_270      = 0x10E,
    PIXEL_ROTATE_NONE     = 0x00,
} PixelRotate;

//VideoFormat: These match the enums in java/com/hsj/camera/UVCCamera.VideoFormat
typedef enum {
    VIDEO_FORMAT_MP4      = 0x11,
    VIDEO_FORMAT_H264     = 0x10,
    VIDEO_FORMAT_NONE     = 0x00,
} VideoFormat;

class frame_decoder {
public:
    frame_decoder(uint16_t width, uint16_t height, FrameFormat format, PixelFormat pixel);
    ~frame_decoder();

    int voidFun(uint8_t  *src, size_t size, uint8_t *dst);
    int FrameToCopy(uint8_t *src, size_t size, uint8_t *dst);

    int YUY2ToRGB(uint8_t  *src, size_t size, uint8_t *dst);
    int YUY2ToNV21(uint8_t *src, size_t size, uint8_t *dst);
    int YUY2ToYUV420P(uint8_t *src, size_t size, uint8_t *dst);
    int YUY2ToYUV422P(uint8_t *src, size_t size, uint8_t *dst);

    int MJPGToRGB(uint8_t  *src, size_t size, uint8_t *dst);
    int MJPGToNV21(uint8_t *src, size_t size, uint8_t *dst);
    int MJPGToYUV420P(uint8_t *src, size_t size, uint8_t *dst);
    int MJPGToYUV422P(uint8_t *src, size_t size, uint8_t *dst);

    int H264ToRGB(uint8_t  *src, size_t size, uint8_t *dst);
    int H264ToNV21(uint8_t *src, size_t size, uint8_t *dst);
    int H264ToYUV420P(uint8_t *src, size_t size, uint8_t *dst);
    int H264ToYUV422P(uint8_t *src, size_t size, uint8_t *dst);

private:
    // Frame size
    uint16_t width;
    uint16_t height;

    // YUV: length
    uint32_t length_y;
    uint32_t length_u;
    // YUV: stride
    uint32_t stride_rgb;
    uint32_t stride_argb;
    uint32_t stride_yuy2;
    uint16_t stride_uv;
    uint16_t stride_y;
    uint16_t stride_u;
    uint16_t stride_v;

    // YUV: buffer
    uint8_t *buffer;
    uint8_t *buffer_u;
    uint8_t *buffer_v;
    uint8_t *buffer_uv;

    // jpeg-turbo
    int _width;
    int _height;
    int sub_sample;
    int color_space;
    tjhandle handle;
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_UVC_CAMERA_FRAME_DECODER_H
