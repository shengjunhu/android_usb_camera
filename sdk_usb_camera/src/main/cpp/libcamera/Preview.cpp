//
// Created by Hsj on 2022/1/13.
//

#include <cstring>
#include <malloc.h>
#include "libyuv.h"
#include "Common.h"
#include "Decoder.h"
#include "Preview.h"

#ifdef __cplusplus
extern "C" {
#endif

#define HIST_SIZE 0xFFFF

//==================================================================================================

Preview::Preview(int width, int height, DataFormat format, ANativeWindow *window) :
        width(width), height(height), format(format), stride_width(width * 2),
        yuv422(NULL), histogram(NULL), window(window) {
    if (format == DATA_FORMAT_NV12) {
        start_uv = width * height;
    } else if (format == DATA_FORMAT_YUV422) {
        stride_uv = width / 2;
        start_u = width * height;
        start_v = start_u * 3 / 2;
    } else if (format == DATA_FORMAT_YUYV) {
        stride_uv = width / 2;
        start_u = width * height;
        start_v = start_u * 3 / 2;
        yuv422 = (uint8_t *) malloc(width * height * 2);
    } else if (format == DATA_FORMAT_DEPTH) {
        frameSize = width * height * 2;
        histogram = (uint32_t *) malloc(HIST_SIZE * sizeof(uint32_t));
    } else {
        LOGE("Error DataFormat: %d", format);
    }
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
}

void Preview::render(uint8_t *data) {
    switch (format) {
        case DATA_FORMAT_NV12:
            renderNV12(data);
            break;
        case DATA_FORMAT_YUV422:
            renderYUV422(data);
            break;
        case DATA_FORMAT_YUYV:
            renderYUYV(data);
            break;
        case DATA_FORMAT_DEPTH:
            renderDepth(data);
            break;
        case DATA_FORMAT_ERROR:
        default:
            LOGE("Error DataFormat %d to render", format);
            break;
    }
}

void Preview::pause() {
    ANativeWindow_Buffer buffer;
    if (LIKELY(ANativeWindow_lock(window, &buffer, nullptr) == 0)) {
        auto *dest = (uint8_t *) buffer.bits;
        const size_t size_line = buffer.width * 4;
        const int size_stride = buffer.stride * 4;
        for (int i = 0; i < buffer.height; i++) {
            memset(dest, 0, size_line);
            dest += size_stride;
        }
        ANativeWindow_unlockAndPost(window);
    }
}

Preview::~Preview() {
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    SAFE_FREE(yuv422)
    SAFE_FREE(histogram)
}

//==================================================================================================

//NV12:10ms
void Preview::renderNV12(const uint8_t *data) {
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        auto *dest = (uint8_t *) buffer.bits;
        libyuv::NV12ToABGR(data, buffer.width,
                           data + start_uv, buffer.width,
                           dest, buffer.stride * 4,
                           buffer.width, buffer.height);
        ANativeWindow_unlockAndPost(window);
    }
}

//YUV422:10ms (YUV)
void Preview::renderYUV422(const uint8_t *data) {
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        auto *dest = (uint8_t *) buffer.bits;
        libyuv::I422ToABGR(data, buffer.width,
                           data + start_u, stride_uv,
                           data + start_v, stride_uv,
                           dest, buffer.stride * 4,
                           buffer.width, buffer.height);
        ANativeWindow_unlockAndPost(window);
    }
}

//YUYV: 18ms (YUV422)
void Preview::renderYUYV(const uint8_t *data) {
    libyuv::YUY2ToI422(data, stride_width,
                       yuv422, width,
                       yuv422 + start_u, stride_uv,
                       yuv422 + start_v, stride_uv,
                       width, height);
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        auto *dest = (uint8_t *) buffer.bits;
        libyuv::I422ToABGR(yuv422, buffer.width,
                           yuv422 + start_u, stride_uv,
                           yuv422 + start_v, stride_uv,
                           dest, buffer.stride * 4,
                           buffer.width, buffer.height);
        ANativeWindow_unlockAndPost(window);
    }
}

//DEPTH16: 20ms
void Preview::renderDepth(const uint8_t *data) {
    // 1-Calculate Depth
    calculateDepthHist((const DepthPixel *) data, frameSize);
    // 2-Update texture
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        auto *dest = (uint8_t *) buffer.bits;
        for (int h = 0; h < buffer.height; ++h) {
            uint8_t *texture = dest + h * buffer.stride * 4;
            const auto *depth = (const DepthPixel *) (data + h * stride_width);
            for (int w = 0; w < buffer.width; ++w, ++depth, texture += 4) {
                unsigned int val = histogram[*depth];
                texture[0] = val;
                texture[1] = val;
                texture[2] = val;
                texture[3] = 0xff;
            }
        }
        ANativeWindow_unlockAndPost(window);
    }
}

void Preview::calculateDepthHist(const DepthPixel *depth, const size_t size) {
    unsigned int value = 0;
    unsigned int index = 0;
    unsigned int numberOfPoints = 0;
    // Calculate the accumulative histogram
    memset(histogram, 0, HIST_SIZE * sizeof(uint32_t));
    for (int i = 0; i < size / sizeof(DepthPixel); ++i, ++depth) {
        value = *depth;
        if (value != 0) {
            histogram[value]++;
            numberOfPoints++;
        }
    }
    for (index = 1; index < HIST_SIZE; index++) {
        histogram[index] += histogram[index - 1];
    }
    if (numberOfPoints != 0) {
        for (index = 1; index < HIST_SIZE; index++) {
            histogram[index] = (uint32_t) (256 * (1.0f -((float) histogram[index] / numberOfPoints)));
        }
    }
}

#ifdef __cplusplus
}  // extern "C"
#endif