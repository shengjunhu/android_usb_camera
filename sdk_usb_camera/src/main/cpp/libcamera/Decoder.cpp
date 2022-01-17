//
// Created by Hsj on 2022/1/13.
//

#include "Decoder.h"

#ifdef __cplusplus
extern "C" {
#endif

Decoder::Decoder(int width, int height) : width(width), height(height),
    flags(0), subSample(0), colorSpace(0) {
    //1. create decompress
    handle = tjInitDecompress();
    //2. alloc yuv422 out buffer memory: subSample = TJSAMP_422
    size_t out_size = tjBufSizeYUV2(width, 4, height, TJSAMP_422);
    dst = tjAlloc(out_size);
}

uint8_t* Decoder::convert2YUV(unsigned char *raw, size_t size) {
    //3. get raw_buffer info: subSample = TJSAMP_422
    tjDecompressHeader3(handle, raw, size, &width, &height, &subSample, &colorSpace);
    //4. decompress to YUV422 22ms (flag = 0、TJFLAG_FASTDCT)
    tjDecompressToYUV2(handle, raw, size, dst, width, 4, height, flags);
    return dst;
}

Decoder::~Decoder()  {
    //5. destroy handle
    if (dst) {
        tjFree(dst);
        dst = nullptr;
    }
    if (handle) {
        tjDestroy(handle);
        handle = nullptr;
    }
}

#ifdef __cplusplus
}  // extern "C"
#endif