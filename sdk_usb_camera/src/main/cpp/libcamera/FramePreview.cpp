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

// Render by OpenGL ES

GLESPreview::GLESPreview(uint16_t width, uint16_t height,
       PixelFormat format, ANativeWindow *window) : window(window) {
    if (format == PIXEL_FORMAT_I420) {
        stride_u = width / 2;
        stride_v = width / 2;
        u_length = width * height;
        v_length = u_length + u_length / 4;
        histogram = nullptr;
    } else if (format == PIXEL_FORMAT_DEPTH) {
        stride_depth = width * 2;
        frame_size = width * height * 2;
        histogram = (uint32_t *) malloc(HIST_SIZE * sizeof(uint32_t));
    } else {
        histogram = nullptr;
        LOGE("PixelFormat error: %d", format);
    }
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
}

GLESPreview::~GLESPreview() {
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    SAFE_FREE(histogram)
}

int GLESPreview::pause() {
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

int GLESPreview::renderI420(const uint8_t *data) {
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        libyuv::I420ToRGBA(data, buffer.width,
                           data + u_length, stride_u,
                           data + v_length, stride_v,
                           (uint8_t *) buffer.bits,
                           buffer.stride * 4,
                           buffer.width, buffer.height);
        ANativeWindow_unlockAndPost(window);
        return STATUS_SUCCESS;
    } else {
        return STATUS_EXE_FAILED;
    }
}

int GLESPreview::renderDepth(const uint8_t *data) {
    // 1-Calculate Depth
    calculateDepthHist((const DepthPixel *) data, frame_size);
    // 2-Update texture
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        auto *dest = (uint8_t *) buffer.bits;
        for (uint32_t h = 0; h < buffer.height; ++h) {
            uint8_t *texture = dest + h * buffer.stride * 4;
            const auto *depth = (const DepthPixel *) (data + h * stride_depth);
            for (int w = 0; w < buffer.width; ++w, ++depth, texture += 4) {
                uint32_t value = histogram[*depth];
                //Yellow: RGBA (R=G, B=0)
                texture[0] = value;
                texture[1] = value;
                texture[2] = 0x00;
                texture[3] = 0XFF;
            }
        }
        ANativeWindow_unlockAndPost(window);
        return STATUS_SUCCESS;
    } else {
        return STATUS_EXE_FAILED;
    }
}

void GLESPreview::calculateDepthHist(const DepthPixel *depth, const size_t size) {
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

// Render by ANativeWindow

NativePreview::NativePreview(uint16_t width, uint16_t height,
        PixelFormat format, ANativeWindow *window) : window(window) {
    if (format == PIXEL_FORMAT_I420) {
        stride_u = width / 2;
        stride_v = width / 2;
        u_length = width * height;
        v_length = u_length + u_length / 4;
        histogram = nullptr;
    } else if (format == PIXEL_FORMAT_DEPTH) {
        stride_depth = width * 2;
        frame_size = width * height * 2;
        histogram = (uint32_t *) malloc(HIST_SIZE * sizeof(uint32_t));
    } else {
        histogram = nullptr;
        LOGE("PixelFormat error: %d", format);
    }
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);
}

NativePreview::~NativePreview() {
    if (window) {
        ANativeWindow_release(window);
        window = nullptr;
    }
    SAFE_FREE(histogram)
}

int NativePreview::pause() {
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

int NativePreview::renderI420(const uint8_t *data) {
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        libyuv::I420ToRGBA(data, buffer.width,
                           data + u_length, stride_u,
                           data + v_length, stride_v,
                           (uint8_t *) buffer.bits,
                           buffer.stride * 4,
                           buffer.width, buffer.height);
        ANativeWindow_unlockAndPost(window);
        return STATUS_SUCCESS;
    } else {
        return STATUS_EXE_FAILED;
    }
}

int NativePreview::renderDepth(const uint8_t *data) {
    // 1-Calculate Depth
    calculateDepthHist((const DepthPixel *) data, frame_size);
    // 2-Update texture
    ANativeWindow_Buffer buffer;
    if (LIKELY(0 == ANativeWindow_lock(window, &buffer, nullptr))) {
        auto *dest = (uint8_t *) buffer.bits;
        for (uint32_t h = 0; h < buffer.height; ++h) {
            uint8_t *texture = dest + h * buffer.stride * 4;
            const auto *depth = (const DepthPixel *) (data + h * stride_depth);
            for (int w = 0; w < buffer.width; ++w, ++depth, texture += 4) {
                uint32_t value = histogram[*depth];
                //Yellow: RGBA (R=G, B=0)
                texture[0] = value;
                texture[1] = value;
                texture[2] = 0x00;
                texture[3] = 0XFF;
            }
        }
        ANativeWindow_unlockAndPost(window);
        return STATUS_SUCCESS;
    } else {
        return STATUS_EXE_FAILED;
    }
}

void NativePreview::calculateDepthHist(const DepthPixel *depth, const size_t size) {
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

#ifdef __cplusplus
}  // extern "C"
#endif
