//
// Created by shengjunhu on 2022/9/1.
//

#include <cassert>
#include <cstring>
#include "common.hpp"
#include "libyuv.h"
#include "frame_decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

frame_decoder::frame_decoder(uint16_t width, uint16_t height, FrameFormat format, PixelFormat pixel):
        width(width), height(height) {

    if (format == FRAME_FORMAT_YUY2) {
        if (pixel == PIXEL_FORMAT_RGB) {
            stride_rgb  = width * 3;
            stride_argb = width * 4;
            stride_yuy2 = width * 2;
            buffer = (uint8_t *) malloc(stride_argb * height);
            assert(buffer != nullptr);
        } else if (pixel == PIXEL_FORMAT_NV21) {
            stride_y    = width;
            stride_uv   = width / 2;
            stride_yuy2 = width * 2;
            length_y    = width * height;
            buffer = (uint8_t *) malloc( (stride_y + stride_uv) * height);
            assert(buffer != nullptr);
            buffer_uv   = buffer + length_y;
        } else if (pixel == PIXEL_FORMAT_YUV420P) {
            stride_y    = width;
            stride_u    = width / 2;
            stride_v    = width / 2;
            stride_yuy2 = width * 2;
            length_y    = stride_y * height;
            length_u    = length_y / 4;
        } else if (pixel == PIXEL_FORMAT_YUV422P) {
            stride_y    = width;
            stride_u    = width;
            stride_v    = width;
            stride_yuy2 = width * 2;
            length_y    = stride_y * height;
            length_u    = length_y / 2;
        }
    } else if (format == FRAME_FORMAT_MJPG) {
        handle = tjInitDecompress();
        if (pixel == PIXEL_FORMAT_NV21) {
            stride_y    = width;
            stride_u    = width;
            stride_v    = width;
            stride_uv   = width / 2;
            length_y    = stride_y * height;
            length_u    = length_y / 4;
            buffer = (uint8_t *) malloc(width * height * 2);
            assert(buffer != nullptr);
        } else if (pixel == PIXEL_FORMAT_YUV420P) {
            stride_y    = width;
            stride_u    = width / 2;
            stride_v    = width / 2;
            length_y    = stride_y * height;
            length_u    = length_y / 4;
            buffer = (uint8_t *) malloc(width * height * 2);
            assert(buffer != nullptr);
        }
    } else if (format == FRAME_FORMAT_H264) {
        LOGE("FrameDecoder: format(%d) not implemented to convert pixel(%d).", format, pixel);
    } else {
        LOGW("FrameDecoder: format(%d) unknown.", format);
    }
}

frame_decoder::~frame_decoder() {
    SAFE_FREE(buffer);
    if (handle) {
        tjDestroy(handle);
        handle = nullptr;
    }
}

//==================================================================================================

int frame_decoder::voidFun(uint8_t  *src, size_t size, uint8_t *dst){
    return STATUS_SUCCEED;
}

int frame_decoder::FrameToCopy(uint8_t *src, size_t size, uint8_t *dst){
    memcpy(dst, src, size);
    return STATUS_SUCCEED;
}

//==========================================YUY2====================================================

int frame_decoder::YUY2ToRGB(uint8_t *src, size_t size, uint8_t *dst){
    int ret = libyuv::YUY2ToARGB(src, stride_yuy2, buffer, stride_argb, width, height);
    if (STATUS_SUCCEED == ret){
        ret = libyuv::ARGBToRAW(buffer, stride_argb, dst, stride_rgb, width, height);
    }
    return ret;
}

int frame_decoder::YUY2ToNV21(uint8_t *src, size_t size, uint8_t *dst){
    int ret = libyuv::YUY2ToNV12(src, stride_yuy2,
            buffer, stride_y, buffer_uv, stride_uv,
            width, height);
    if (STATUS_SUCCEED == ret){
        uint8_t *dst_uv = dst + length_y;
        ret = libyuv::NV21ToNV12(buffer, stride_y, buffer_uv, stride_uv,
                dst, stride_y, dst_uv, stride_uv,
                width, height);
    }
    return ret;
}

int frame_decoder::YUY2ToYUV420P(uint8_t *src, size_t size, uint8_t *dst){
    uint8_t *dst_u = dst + length_y;
    uint8_t *dst_v = dst_u + length_u;
    return libyuv::YUY2ToI420(src, stride_yuy2,
            dst, stride_y, dst_u, stride_u, dst_v, stride_v,
            width, height);
}

int frame_decoder::YUY2ToYUV422P(uint8_t *src, size_t size, uint8_t *dst){
    uint8_t *dst_u = dst + length_y;
    uint8_t *dst_v = dst_u + length_u;
    return libyuv::YUY2ToI422(src, stride_yuy2,
            dst, stride_y, dst_u, stride_u, dst_v, stride_v,
            width, height);
}

//=========================================MJPG=====================================================

int frame_decoder::MJPGToRGB(uint8_t *src, size_t size, uint8_t *dst){
    int ret = tjDecompressHeader3(handle, src, size, &_width, &_height, &sub_sample, &color_space);
    if (STATUS_SUCCEED == ret) {
        ret = tjDecompress2(handle, src, size, dst, _width,
                _width * 3, _height, TJPF_RGB, TJFLAG_FASTDCT);
    }
    return ret;
}

int frame_decoder::MJPGToNV21(uint8_t *src, size_t size, uint8_t *dst){
    int ret = tjDecompressHeader3(handle, src, size, &_width, &_height, &sub_sample, &color_space);
    if (STATUS_SUCCEED == ret) {
        ret = tjDecompressToYUV2(handle, src, size, buffer, _width, 4, _height, TJFLAG_FASTDCT);
        if (STATUS_SUCCEED == ret) {
            uint8_t *dst_uv = dst + length_y;
            ret = libyuv::I422ToNV21(buffer, stride_y, buffer_u, stride_u, buffer_v, stride_v,
                    dst, stride_y, dst_uv, stride_uv,
                    width, height);
        }
    }
    return ret;
}

int frame_decoder::MJPGToYUV420P(uint8_t *src, size_t size, uint8_t *dst){
    int ret = tjDecompressHeader3(handle, src, size, &_width, &_height, &sub_sample, &color_space);
    if (STATUS_SUCCEED == ret) {
        ret = tjDecompressToYUV2(handle, src, size, buffer, _width, 4, _height, TJFLAG_FASTDCT);
        if (STATUS_SUCCEED == ret) {
            uint8_t *dst_u = dst + length_y;
            uint8_t *dst_v = dst_u + length_u;
            ret = libyuv::I422ToI420(buffer, width, buffer_u, width, buffer_v, width,
                    dst, stride_y, dst_u, stride_u, dst_v, stride_v,
                    width, height);
        }
    }
    return ret;
}

int frame_decoder::MJPGToYUV422P(uint8_t *src, size_t size, uint8_t *dst){
    int ret = tjDecompressHeader3(handle, src, size, &_width, &_height, &sub_sample, &color_space);
    if (STATUS_SUCCEED == ret) {
        ret = tjDecompressToYUV2(handle, src, size, buffer, _width, 4, _height, TJFLAG_FASTDCT);
    }
    return ret;
}

//===========================================H264===================================================

int frame_decoder::H264ToRGB(uint8_t *src, size_t size, uint8_t *dst){
    LOGE("H264ToRGB(): not implemented.");
    return STATUS_FAILED;
}

int frame_decoder::H264ToNV21(uint8_t *src, size_t size, uint8_t *dst){
    LOGE("H264ToNV21(): not implemented.");
    return STATUS_FAILED;
}

int frame_decoder::H264ToYUV420P(uint8_t *src, size_t size, uint8_t *dst){
    LOGE("H264ToYUV420P(): not implemented.");
    return STATUS_FAILED;
}

int frame_decoder::H264ToYUV422P(uint8_t *src, size_t size, uint8_t *dst){
    LOGE("H264ToYUV422P(): not implemented.");
    return STATUS_FAILED;
}

//==================================================================================================

#ifdef __cplusplus
}  // extern "C"
#endif