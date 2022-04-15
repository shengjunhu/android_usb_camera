//
// Created by Hsj on 2022/4/13.
//

#ifndef ANDROID_USB_CAMERA_FRAMEPREVIEW_H
#define ANDROID_USB_CAMERA_FRAMEPREVIEW_H

#include <android/native_window_jni.h>
#include "FrameDecoder.h"

typedef uint16_t DepthPixel;

#ifdef __cplusplus
extern "C" {
#endif

class IPreview {
public:
    virtual ~IPreview() = default;
    virtual int pause() = 0;
    virtual int renderI420(const uint8_t *data) = 0;
    virtual int renderDepth(const uint8_t *data) = 0;
};

class GLESPreview : public IPreview {
private:
    size_t frame_size;
    size_t u_length;
    size_t v_length;
    int stride_u;
    int stride_v;
    int stride_depth;
    uint32_t *histogram;
    ANativeWindow *window;
    void calculateDepthHist(const DepthPixel *depth, const size_t size);
public:
    GLESPreview(uint16_t width, uint16_t height, PixelFormat format, ANativeWindow *window);
    ~GLESPreview();
    int pause();
    int renderI420(const uint8_t *data);
    int renderDepth(const uint8_t *data);
};

class NativePreview : public IPreview{
private:
    size_t frame_size;
    size_t u_length;
    size_t v_length;
    int stride_u;
    int stride_v;
    int stride_depth;
    uint32_t *histogram;
    ANativeWindow *window;
    void calculateDepthHist(const DepthPixel *depth, const size_t size);
public:
    NativePreview(uint16_t width, uint16_t height, PixelFormat format, ANativeWindow *window);
    ~NativePreview();
    int pause();
    int renderI420(const uint8_t *data);
    int renderDepth(const uint8_t *data);
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_FRAMEPREVIEW_H
