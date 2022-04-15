//
// Created by Hsj on 2022/4/13.
//

#ifndef ANDROID_USB_CAMERA_FRAMEPROCESS_H
#define ANDROID_USB_CAMERA_FRAMEPROCESS_H

#include "FrameDecoder.h"
#include "FrameEncoder.h"
#include "FramePreview.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    FRAME_FORMAT_UNKNOWN    = 0x0,  //UNKNOWN
    FRAME_FORMAT_YUY2       = 0x04, //YUY2.
    FRAME_FORMAT_MJPG       = 0x06, //MJPG.
    FRAME_FORMAT_H264       = 0x10, //H264.
} FrameFormat;

typedef int (IPreview::*Render)(const uint8_t *data);

typedef int (FrameDecoder::*Decode)(uint8_t *src, size_t size, uint8_t *dst);

class FrameProcess {
private:
    uint16_t width;
    uint16_t height;
    PixelFormat pixel;
    // decoder
    Decode decode;
    FrameDecoder *decoder;
    // Preview
    Render render;
    IPreview *preview;
public:
    // jni
    JavaVM *jvm;
    jobject obj;
    jmethodID mid;
    // decode buffer
    uint8_t *buffer;
    size_t buffer_size;
    size_t pixel_size;
public:
    FrameProcess(jint width, jint height, jint format, jint pixel);
    ~FrameProcess();
    int renderFrame(uint8_t *src);
    int setPreview(ANativeWindow *window);
    int decodeFrame(uint8_t *src, size_t size, uint8_t *dst);
    int setJavaObj(JavaVM* jvm, jobject obj, jmethodID mid);
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_FRAMEPROCESS_H
