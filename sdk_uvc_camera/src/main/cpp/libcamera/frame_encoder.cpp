//
// Created by shengjunhu on 2022/9/1.
//

#include "common.hpp"
#include "frame_encoder.h"

#ifdef __cplusplus
extern "C" {
#endif

frame_encoder::frame_encoder(uint16_t width, uint16_t height, PixelFormat pixel, VideoFormat format) {

}

frame_encoder::~frame_encoder() {

}

//==========================================To MP4==================================================

int RGBToMP4(uint8_t  *src, size_t size, uint8_t *dst){
    return STATUS_FAILED;
}

int NV21ToMP4(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_FAILED;
}

int YUY2ToMp4(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_FAILED;
}

//==========================================To H264=================================================

int RGBToH264(uint8_t  *src, size_t size, uint8_t *dst){
    return STATUS_FAILED;
}

int NV21ToH264(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_FAILED;
}

int YUY2ToH264(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_FAILED;
}

int H264ToCopy(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_FAILED;
}

//==================================================================================================

#ifdef __cplusplus
}  // extern "C"
#endif