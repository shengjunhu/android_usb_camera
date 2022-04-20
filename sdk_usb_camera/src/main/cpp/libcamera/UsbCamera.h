//
// Created by Hsj on 2022/1/14.
//

#ifndef ANDROID_USB_CAMERA_USBCAMERA_H
#define ANDROID_USB_CAMERA_USBCAMERA_H

#include <jni.h>
#include <vector>
#include "libuvc/libuvc.h"
#include "FrameProcess.h"

#ifdef __cplusplus
extern "C" {
#endif

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
    int pixel;
    int rotate;
    bool mirror;

    SupportInfo(int format, int width, int height, int fps) {
        this->format = (uvc_vs_desc_subtype)format;
        this->width = width;
        this->height = height;
        this->fps = fps;
    }

};

class UsbCamera {
private:
    //usb+uvc
    uvc_stream_ctrl_t ctrl;
    uvc_device_t *uvc_device;
    uvc_context_t *uvc_context;
    uvc_device_handle_t *uvc_device_handle;
    //local
    int fd;
    FrameProcess *process;
    volatile StatusInfo status;
    inline const StatusInfo getStatus() const;
public:
    UsbCamera();
    ~UsbCamera();
    int connectDevice(int fd);
    int openDevice(int vendorId, int productId, int bus_num, int dev_num);
    int getSupportInfo(std::vector<SupportInfo> &supportInfo);
    int setSupportInfo(SupportInfo &configInfo);
    int setFrameProcess(FrameProcess *process);
    int setPreview(ANativeWindow* window);
    int startStream();
    int stopStream();
    int closeDevice();
    void destroy();
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_USBCAMERA_H
