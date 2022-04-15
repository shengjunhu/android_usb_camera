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
    stride_yuy2 = width * 2;
    stride_u    = width / 2;
    stride_v    = width / 2;
    y_length    = width * height;
    u2_length   = y_length / 2;
    u4_length    = y_length / 4;
    //MJPG
    handle = tjInitDecompress();
    buffer = (uint8_t *) malloc(y_length * 2);
    //H264
    //...
}

FrameDecoder::~FrameDecoder() {
    SAFE_FREE(buffer);
    if (handle) {
        tjDestroy(handle);
        handle = nullptr;
    }
}

int FrameDecoder::YUY2ToCopy(uint8_t *src, size_t size, uint8_t *dst){
    uint16_t stride = width * 2;
    libyuv::CopyPlane(src, stride, dst, stride, stride, height);
    return STATUS_SUCCESS;
}

int FrameDecoder::YUY2ToI420(uint8_t *src, size_t size, uint8_t *dst){
    uint8_t *dst_u = dst   + y_length;
    uint8_t *dst_v = dst_u + u4_length;
    return libyuv::YUY2ToI420(src, stride_yuy2, dst, width,
            dst_u, stride_u, dst_v, stride_v, width, height);
}

int FrameDecoder::MJPGToI420(uint8_t *src, size_t size, uint8_t *dst){
    int ret = tjDecompressHeader3(handle, src, size, &_width, &_height, &sub_sample, &color_space);
    if (0 == ret) {
        ret = tjDecompressToYUV2(handle, src, size, buffer, _width, 4, _height, TJFLAG_FASTDCT);
        if (0 == ret) {
            uint8_t *src_u = buffer + y_length;
            uint8_t *src_v = src_u  + u2_length;
            uint8_t *dst_u = dst    + y_length;
            uint8_t *dst_v = dst_u  + u4_length;
            ret = libyuv::I422ToI420(buffer, width, src_u,  stride_u, src_v,  stride_v,
                    dst, width, dst_u, stride_u, dst_v, stride_v, width, height);
        }
    }
    return ret;
}

int FrameDecoder::H264ToI420(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_SUCCESS;
}

#ifdef __cplusplus
}  // extern "C"
#endif