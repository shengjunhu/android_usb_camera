//
// Created by Hsj on 2022/1/13.
//

#ifndef ANDROID_USB_CAMERA_DECODER_H
#define ANDROID_USB_CAMERA_DECODER_H

#include <unistd.h>
#include "turbojpeg.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    DATA_FORMAT_NV12   = 1, //yvu
    DATA_FORMAT_YUV422 = 2, //yuv
    DATA_FORMAT_YUYV   = 3, //yuyv
    DATA_FORMAT_DEPTH  = 4, //uint16_t
    DATA_FORMAT_ERROR  = 0,
} DataFormat;

class Decoder {
private:
    int flags;
    int width;
    int height;
    int subSample;
    int colorSpace;
    tjhandle handle;
    uint8_t *dst;
public:
    Decoder(int width, int height);
    ~Decoder();
    uint8_t* convert2YUV(unsigned char *raw, size_t size);
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_DECODER_H
