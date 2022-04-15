//
// Created by Hsj on 2022/4/12.
//

#include "Common.h"
#include "FrameEncoder.h"

#ifdef __cplusplus
extern "C" {
#endif

FrameEncoder::FrameEncoder(uint16_t width, uint16_t height) {

}

FrameEncoder::~FrameEncoder() {

}

int FrameEncoder::YUVToMP4(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_SUCCESS;
}

int FrameEncoder::YUVToH264(uint8_t *src, size_t size, uint8_t *dst){
    return STATUS_SUCCESS;
}

#ifdef __cplusplus
}  // extern "C"
#endif