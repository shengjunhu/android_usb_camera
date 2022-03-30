//
// Created by Hsj on 2022/1/13.
//

#ifndef ANDROID_USB_CAMERA_PREVIEW_H
#define ANDROID_USB_CAMERA_PREVIEW_H

#include <android/native_window_jni.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t DepthPixel;

class Preview {
private:
    int width;
    int height;
    int format;
    size_t frameSize;

    int stride_width;
    int stride_uv;
    int start_uv;
    int start_u;
    int start_v;

    uint8_t *yuv422;
    uint32_t *histogram;
    ANativeWindow *window;

    void renderNV12(const uint8_t *data);
    void renderYUV422(const uint8_t *data);
    void renderYUYV(const uint8_t *data);
    void renderDepth(const uint8_t *data);
    void calculateDepthHist(const DepthPixel *depth, const size_t size);

public:
    Preview(int width, int height, DataFormat format, ANativeWindow *window);
    ~Preview();
    void render(uint8_t *data);
    void pause();
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_PREVIEW_H
