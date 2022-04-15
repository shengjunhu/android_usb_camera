//
// Created by Hsj on 2022/4/12.
//

#include "Common.h"
#include "libyuv.h"
#include "FrameDecoder.h"

#ifdef __cplusplus
extern "C" {
#endif

FrameDecoder::FrameDecoder(uint16_t width, uint16_t height): width(width), height(height) {
    //YUY2
    stride_u = width / 2;
    stride_v = width / 2;
    stride_yuy2 = width * 2;
    y_length = height * width;
    u_length = height * stride_u;
    //MJPG
    handle = tjInitDecompress();
    //H264
}

FrameDecoder::~FrameDecoder() {
    SAFE_FREE(buffer);
    if (handle) {
        tjDestroy(handle);
        handle = nullptr;
    }
}

int FrameDecoder::YUY2ToCopy(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_SUCCESS;
}

int FrameDecoder::YUY2ToYUV420(uint8_t *src, size_t size, uint8_t *dst){
    uint8_t *dst_u = src + y_length;
    uint8_t *dst_v = dst_u + u_length;
    return libyuv::YUY2ToI420(
            src, stride_yuy2,
            dst, width,
            dst_u, stride_u,
            dst_v, stride_v,
            width, height);
}

int FrameDecoder::MJPGToYUY2(uint8_t *src, size_t size, uint8_t *dst){
    uint64_t start = timeMs();
    int ret = tjDecompressHeader3(handle, src, size, &_width, &_height, &sub_sample, &color_space);
    LOGE("w=%d, h=%d, s=%d, c= %d", _width, _height, sub_sample, color_space);
    if (0 == ret) {
        ret = tjDecompressToYUV2(handle, src, size, dst, _width, 4, _height, TJFLAG_FASTDCT);
        LOGW("tjDecompressToYUV2: %lld", timeMs()-start);
    }
    return ret;
}

int FrameDecoder::MJPGToYUV420(uint8_t *src, size_t size, uint8_t *dst){
    int ret = tjDecompressHeader3(handle, src, size, &_width, &_height, &sub_sample, &color_space);
    if (0 == ret) {
        ret = tjDecompressToYUV2(handle, src, size, dst, _width, 4, _height, TJFLAG_FASTDCT);
    }
    return ret;
}

int FrameDecoder::H264ToYUY2(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_SUCCESS;
}

int FrameDecoder::H264ToYUV420(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_SUCCESS;
}

#ifdef __cplusplus
}  // extern "C"
#endif