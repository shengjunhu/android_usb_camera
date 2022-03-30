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

int UsbMessenger::open(int vendorId, int productId, int fd) {
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