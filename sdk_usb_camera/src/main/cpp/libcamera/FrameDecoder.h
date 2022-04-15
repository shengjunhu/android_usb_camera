//
// Created by Hsj on 2022/4/12.
//

#ifndef ANDROID_USB_CAMERA_FRAMEDECODER_H
#define ANDROID_USB_CAMERA_FRAMEDECODER_H

#include <unistd.h>
#include <turbojpeg.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    PIXEL_FORMAT_UNKNOWN    = 0x00, //UNKNOWN
    PIXEL_FORMAT_YUY2       = 0x04, //YUY2: Y U Y V.
    PIXEL_FORMAT_DEPTH      = 0x05, //DEPTH: width * height * sizeof(uint_16)
    PIXEL_FORMAT_YUV420     = 0x0A, //YUV420: Y U V
} PixelFormat;

class FrameDecoder {
private:
    uint16_t width;
    uint16_t height;
    //YUY2
    size_t y_length;
    size_t u_length;
    uint16_t stride_u;
    uint16_t stride_v;
    uint16_t stride_yuy2;
    //MJPG
    int _width;
    int _height;
    int sub_sample;
    int color_space;
    tjhandle handle;
    uint8_t *buffer;
    //H264
public:
    FrameDecoder(uint16_t width, uint16_t height);
    ~FrameDecoder();

    int YUY2ToCopy(uint8_t *src, size_t size, uint8_t *dst);
    int YUY2ToYUV420(uint8_t *src, size_t size, uint8_t *dst);

    int MJPGToYUY2(uint8_t *src, size_t size, uint8_t *dst);
    int MJPGToYUV420(uint8_t *src, size_t size, uint8_t *dst);

    int H264ToYUY2(uint8_t *src, size_t size, uint8_t *dst);
    int H264ToYUV420(uint8_t *src, size_t size, uint8_t *dst);
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_FRAMEDECODER_H
