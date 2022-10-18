//
// Created by shengjunhu on 2022/9/1.
//

#pragma once

#ifndef ANDROID_UVC_CAMERA_UVC_CAMERA_H
#define ANDROID_UVC_CAMERA_UVC_CAMERA_H

#include <jni.h>
#include <vector>
#include <string>
#include "libuvc/libuvc.h"
#include "frame_converter.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    STATUS_INIT_NONE = 0x00, //No Init.
    STATUS_INIT_OK   = 0x0B, //Init Succeed.
    STATUS_OPEN      = 0x0C, //Device Opened.
    STATUS_CONFIGURE = 0x0D, //Device Configured.
    STATUS_RUN       = 0x0E, //Device Streaming.
} StatusInfo;

struct SupportInfo {
    // Frame type
    uvc_vs_desc_subtype type;
    // Frame format
    uint8_t format[4];
    // Frame width
    uint16_t width;
    // Frame height
    uint16_t height;
    // Frame fps
    uint8_t fps;
    // Bandwidth∈[0.10f, 1.00f]
    float bandwidth;

    // Pixel info
    uint8_t pixel;
    uint8_t video;
    uint16_t rotate;
    uint8_t flip;
    std::string photo_dir;
    std::string video_dir;
};

class uvc_camera {
public:
    uvc_camera();
    ~uvc_camera();
    int connectDevice(const int fd);
    int openDevice(const int vendorId, const int productId, const int bus, const int dev);
    int getSupportInfo(std::vector<SupportInfo> &supportInfo);
    int setSupportInfo(SupportInfo &supportInfo);
    int setFrameConverter(frame_converter *converter);
    int setFramePreview(jlong previewId);
    int startStream();
    int takePhoto();
    int startVideo();
    int stopVideo();
    int stopStream();
    int closeDevice();
    void destroy();

private:
    //uvc
    uvc_stream_ctrl_t ctrl;
    uvc_device_t *uvc_device;
    uvc_context_t *uvc_context;
    uvc_device_handle_t *uvc_device_handle;

    //local
    int fd;
    float bandwidth;
    frame_converter *converter;
    volatile StatusInfo status;
    inline const StatusInfo getStatus() const;
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_UVC_CAMERA_UVC_CAMERA_H
