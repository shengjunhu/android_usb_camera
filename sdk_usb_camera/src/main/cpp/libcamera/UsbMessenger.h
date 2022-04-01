//
// Created by Hsj on 2022/1/14.
//

#ifndef ANDROID_USB_CAMERA_USBMESSENGER_H
#define ANDROID_USB_CAMERA_USBMESSENGER_H

#ifdef __cplusplus
extern "C" {
#endif

class UsbMessenger {
private:

public:
    UsbMessenger();
    ~UsbMessenger();
    int connect(int fd);
    int open(int vendorId, int productId, int bus_num, int dev_num);
    int syncRequest(const char *request, char *response);
    int close();
    void destroy();
};

#ifdef __cplusplus
}  // extern "C"
#endif

#endif //ANDROID_USB_CAMERA_USBMESSENGER_H
