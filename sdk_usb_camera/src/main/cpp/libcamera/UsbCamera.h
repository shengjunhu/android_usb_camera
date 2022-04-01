//
// Created by Hsj on 2022/1/14.
//

#include <jni.h>
#include <vector>
#include <android/native_window_jni.h>
#include "libuvc/libuvc.h"

#ifndef ANDROID_USB_CAMERA_USBCAMERA_H
#define ANDROID_USB_CAMERA_USBCAMERA_H

typedef enum {
    FRAME_FORMAT_YUYV       = 0x04, //YUYV.
    FRAME_FORMAT_MJPEG      = 0x06, //MJPEG.
    FRAME_FORMAT_H264       = 0x10, //H264.
} FrameFormat;

typedef enum {
    PIXEL_FORMAT_RAW        = 0x04, //YUYV.
    PIXEL_FORMAT_YUV420_888 = 0x0A, //NV12.
} PixelFormat;

typedef enum {
    STATUS_INIT_NONE        = 0x00, //No Init.
    STATUS_INIT_OK          = 0x0B, //Init Succeed.
    STATUS_OPEN             = 0x0C, //Device Opened.
    STATUS_CONFIGURE        = 0x0D, //Device Configured.
    STATUS_RUN              = 0x0E, //Stream Output ing.
} StatusInfo;

struct SupportInfo {
    uvc_vs_desc_subtype format;
    int width;
    int height;
    int fps;

    SupportInfo(int format, int width, int height, int fps){
        this->format = (uvc_vs_desc_subtype)format;
        this->width = width;
        this->height = height;
        this->fps = fps;
    }
};

#ifdef __cplusplus
extern "C" {
#endif

class UsbCamera {
private:
    int _fd;
    uvc_stream_ctrl_t ctrl;
    uvc_device_t *uvc_device;
    uvc_context_t *uvc_context;
    uvc_device_handle_t *uvc_device_handle;

    volatile StatusInfo status;
    inline const StatusInfo getStatus() const;
public:
    UsbCamera();
    ~UsbCamera();
    int connect(int fd);
    int open(int vendorId, int productId, int bus_num, int dev_num);
    int getSupportInfo(std::vector<SupportInfo> &supportInfos);
    int setSupportInfo(SupportInfo &supportInfo);
    int setFrameCallback(int pixelFormat,jobject callback);
    int setPreview(ANativeWindow* window);
    int start();
    int stop();
    int close();
    void destroy();
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_USBCAMERA_H
