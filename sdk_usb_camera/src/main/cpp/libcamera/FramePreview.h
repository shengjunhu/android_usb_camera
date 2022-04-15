//
// Created by Hsj on 2022/4/13.
//

#ifndef ANDROID_USB_CAMERA_FRAMEPREVIEW_H
#define ANDROID_USB_CAMERA_FRAMEPREVIEW_H

#include <android/native_window_jni.h>
#include "FrameDecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t DepthPixel;

class IPreview {
protected:
    int width, height;
public:
    virtual ~IPreview() = default;
    virtual int pause() = 0;
    virtual int renderYUYV(uint8_t *data) = 0;
    virtual int renderDEPTH(uint8_t *data) = 0;
    virtual int renderYUV420(uint8_t *data) = 0;
};

class FramePreview {
private:
    uint16_t width;
    uint16_t height;
    size_t frame_size;

    size_t index_u;
    size_t index_v;

    int stride_u;
    int stride_v;
    int stride_width;

    uint32_t *histogram;
    ANativeWindow *window;
    void calculateDepthHist(const DepthPixel *depth, const size_t size);

public:
    FramePreview(ANativeWindow *window, uint16_t width, uint16_t height, PixelFormat format);
    ~FramePreview();
    int pause();
    int renderYUY2(const uint8_t *src);
    int renderDEPTH(const uint8_t *src);
    int renderYUV420(const uint8_t *src);
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_FRAMEPREVIEW_H
