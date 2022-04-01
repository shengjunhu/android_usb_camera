//
// Created by Hsj on 2022/1/14.
//

#include "Common.h"
#include "UsbMessenger.h"

#ifdef __cplusplus
extern "C" {
#endif

UsbMessenger::UsbMessenger() {

}

UsbMessenger::~UsbMessenger() {

}

int UsbMessenger::connect(int fd) {
    return STATUS_SUCCESS;
}

int UsbMessenger::open(int vendorId, int productId, int bus_num, int dev_num) {
    return STATUS_SUCCESS;
}

int UsbMessenger::syncRequest(const char *request, char *response) {
    return STATUS_SUCCESS;
}

int UsbMessenger::close() {
    return STATUS_SUCCESS;
}

void UsbMessenger::destroy() {

}

#ifdef __cplusplus
}  // extern "C"
#endif