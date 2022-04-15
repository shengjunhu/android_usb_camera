//
// Created by Hsj on 2022/4/13.
//

#include <cstring>
#include <malloc.h>
#include "libyuv.h"
#include "Common.h"
#include "FramePreview.h"

#define HIST_SIZE 0xFFFF

#ifdef __cplusplus
extern "C" {
#endif

FramePreview::FramePreview(ANativeWindow *window,uint16_t width, uint16_t height, PixelFormat format) :
        window(window), width(width), height(height), stride_width(width * 2), histogram(nullptr) {
    if (format == PIXEL_FORMAT_YUV420) {
        stride_u = width / 2;
        stride_v = width / 2;
        index_u = width * height;
        index_v = index_u + index_u / 2;
    } else if (format == PIXEL_FORMAT_DEPTH) {
        frame_size = width * height * 2;
        histogram = (uint32_t *) malloc(HIST_SIZE * sizeof(uint32_t));
    }
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
}

FramePreview::~FramePreview() {
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    SAFE_FREE(histogram)
}

void FramePreview::calculateDepthHist(const DepthPixel *depth, const size_t size) {
    uint32_t hist = 0;
    uint32_t count = 0;
    // Calculate the accumulative histogram
    memset(histogram, 0, HIST_SIZE * sizeof(uint32_t));
    size_t length = size / sizeof(DepthPixel);
    for (int i = 0; i < length; ++i, ++depth) {
        hist = *depth;
        if (hist != 0) {
            histogram[hist]++;
            count++;
        }
    }
    uint32_t index = 0;
    for (index = 1; index < HIST_SIZE; index++) {
        histogram[index] += histogram[index - 1];
    }
    if (count != 0) {
        for (index = 1; index < HIST_SIZE; index++) {
            histogram[index] = (uint32_t) (256.0f - 256.0f * histogram[index] / count);
        }
    }
}

//==================================================================================================

int FramePreview::pause() {
    ANativeWindow_Buffer buffer;
    if (LIKELY(ANativeWindow_lock(window, &buffer, nullptr) == 0)) {
        auto *dst = (uint8_t *) buffer.bits;
        const int32_t size_line = buffer.width * 4;
        const int32_t size_stride = buffer.stride * 4;
        for (int32_t i = 0; i < buffer.height; i++) {
            memset(dst, 0, size_line);
            dst += size_stride;
        }
        ANativeWindow_unlockAndPost(window);
        return STATUS_SUCCESS;
    } else {
        return STATUS_EXE_FAILED;
    }
}

int FramePreview::renderYUY2(const uint8_t *src) {
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        libyuv::YUY2ToARGB(src, buffer.width * 2,
                           (uint8_t *) buffer.bits,
                           buffer.stride * 4,
                           buffer.width, buffer.height);
        ANativeWindow_unlockAndPost(window);
        return STATUS_SUCCESS;
    } else {
        return STATUS_EXE_FAILED;
    }
}

int FramePreview::renderYUV420(const uint8_t *src) {
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        libyuv::I420ToABGR(src, buffer.width,
                           src + index_u, stride_u,
                           src + index_v, stride_v,
                           (uint8_t *) buffer.bits,
                           buffer.stride * 4,
                           buffer.width, buffer.height);
        ANativeWindow_unlockAndPost(window);
        return STATUS_SUCCESS;
    } else {
        return STATUS_EXE_FAILED;
    }
}

int FramePreview::renderDEPTH(const uint8_t *data) {
    // 1-Calculate Depth
    calculateDepthHist((const DepthPixel *) data, frame_size);
    // 2-Update texture
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        auto *dest = (uint8_t *) buffer.bits;
        for (uint32_t h = 0; h < buffer.height; ++h) {
            uint8_t *texture = dest + h * buffer.stride * 4;
            const auto *depth = (const DepthPixel *) (data + h * stride_width);
            for (int w = 0; w < buffer.width; ++w, ++depth, texture += 4) {
                uint32_t value = histogram[*depth];
                texture[0] = value;
                texture[1] = value;
                texture[2] = value;
                texture[3] = 0XFF;
            }
        }
        ANativeWindow_unlockAndPost(window);
        return STATUS_SUCCESS;
    } else {
        return STATUS_EXE_FAILED;
    }
}

#ifdef __cplusplus
}  // extern "C"
#endif
