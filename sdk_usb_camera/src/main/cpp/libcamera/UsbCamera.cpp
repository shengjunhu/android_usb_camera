//
// Created by Hsj on 2022/1/14.
//

#include "UsbCamera.h"
#include "Common.h"
#include "libuvc/libuvc_internal.h"

#ifdef __cplusplus
extern "C" {
#endif

UsbCamera::UsbCamera() : _fd(0), status(STATUS_INIT_NONE) {

}

UsbCamera::~UsbCamera() {

}

//==================================================================================================

inline const StatusInfo UsbCamera::getStatus() const { return status; }

void frameBack(uvc_frame_t *frame, void *ptr) {
    LOGE("frameBack...");
}

//==================================================================================================

int UsbCamera::connect(int fd) {
    int ret = STATUS_SUCCESS;
    uvc_error_t uvc_ret;
    //1. init uvc
    if (getStatus() == STATUS_INIT_NONE) {
        uvc_ret = uvc_init_fd(&uvc_context, nullptr);
        LOGD("uvc_init(): %d", uvc_ret);
        if (uvc_ret == UVC_SUCCESS) {
            status = STATUS_INIT_OK;
        } else {
            LOGE("uvc_init() failed: %d", uvc_ret);
        }
    }
    //2. connect device
    if (getStatus() == STATUS_INIT_OK) {
        fd = dup(fd);
        uvc_ret = uvc_find_device_opened(uvc_context, uvc_device, &uvc_device_handle, fd);
        if (uvc_ret == UVC_SUCCESS) {
            _fd = fd;
            status = STATUS_OPEN;
        } else {
            ::close(fd);
            LOGE("connect: %d", uvc_ret);
        }
    } else {
        ret = STATUS_ERROR_STEP;
        LOGE("connect: status=%d", getStatus());
    }
    return ret;
}

int UsbCamera::open(int vendorId, int productId, int bus_num, int dev_num) {
    int ret = STATUS_SUCCESS;
    uvc_error_t uvc_ret;
    //1. init uvc
    if (getStatus() == STATUS_INIT_NONE) {
        uvc_ret = uvc_init(&uvc_context, nullptr);
        if (uvc_ret == UVC_SUCCESS) {
            status = STATUS_INIT_OK;
        } else {
            LOGE("uvc_init() failed: %d", uvc_ret);
        }
    }
    //2. find device
    if (getStatus() == STATUS_INIT_OK) {
        uvc_ret = uvc_find_device2(uvc_context, &uvc_device, vendorId, productId, bus_num, dev_num);
        if (uvc_ret == UVC_SUCCESS) {
            //3. open device
            uvc_ret = uvc_open(uvc_device, &uvc_device_handle);
            if (uvc_ret == UVC_SUCCESS) {
                status = STATUS_OPEN;
            } else {
                LOGE("uvc_open: %d", uvc_ret);
                ret = STATUS_EXE_FAILED;
            }
        } else {
            LOGE("uvc_find_device() failed: %d", uvc_ret);
            ret = STATUS_EXE_FAILED;
        }
    } else {
        ret = STATUS_ERROR_STEP;
        LOGE("open: status=%d", getStatus());
    }
    return ret;
}

int UsbCamera::getSupportInfo(std::vector<SupportInfo> &supportInfos) {
    int ret = STATUS_SUCCESS;
    if (getStatus() == STATUS_OPEN) {
        if (uvc_device_handle) {
            uvc_device_info *device_info = uvc_device_handle->info;
            if (device_info) {
                uvc_streaming_interface_t *stream_info = device_info->stream_ifs;
                if (stream_info){
                    int stream_id = 0;
                    uvc_streaming_interface_t *info;
                    DL_FOREACH(stream_info, info) {
                        ++stream_id;
                        uvc_format_desc_t *fmt_desc;
                        uvc_frame_desc_t *frame_desc;
                        DL_FOREACH(info->format_descs, fmt_desc){
                            DL_FOREACH(fmt_desc->frame_descs, frame_desc){
                                uint32_t fps = 10000000 / frame_desc->dwDefaultFrameInterval;
                                SupportInfo supportInfo(fmt_desc->bDescriptorSubtype,frame_desc->wWidth,frame_desc->wHeight,fps);
                                supportInfos.emplace_back(supportInfo);
                            }
                        }
                    }
                } else {
                    ret = STATUS_EMPTY_DATA;
                    LOGE("getSupportSize: uvc_streaming_interface_t is null");
                }
            } else {
                ret = STATUS_EMPTY_DATA;
                LOGE("getSupportSize: uvc_device_info is null");
            }
        } else {
            ret = STATUS_EMPTY_OBJ;
            LOGE("getSupportSize: uvc_device_handle is null");
        }
    }else {
        ret = STATUS_ERROR_STEP;
        LOGE("getSupportFormat: status=%d", getStatus());
    }
    return ret;
}

int UsbCamera::setSupportInfo(SupportInfo &supportInfo) {
    int ret = STATUS_SUCCESS;
    if (getStatus() == STATUS_OPEN) {
        if (uvc_device_handle) {
            enum uvc_frame_format frame_format = UVC_FRAME_FORMAT_YUYV;
            if (supportInfo.format == UVC_VS_FORMAT_MJPEG){
                frame_format = UVC_FRAME_FORMAT_MJPEG;
            } else if (supportInfo.format == UVC_VS_FORMAT_FRAME_BASED){
                frame_format = UVC_FRAME_FORMAT_H264;
            }
            uvc_error_t uvc_ret = uvc_get_stream_ctrl_format_size(uvc_device_handle, &ctrl,
                         frame_format, supportInfo.width, supportInfo.height, supportInfo.fps);
            if(uvc_ret == UVC_SUCCESS){
                status = STATUS_CONFIGURE;
            } else {
                ret = STATUS_EXE_FAILED;
                LOGE("uvc_get_stream_ctrl_format_size() failed: %d", uvc_ret);
            }
        } else {
            ret = STATUS_EMPTY_OBJ;
            LOGE("setFrameSize: uvc_device_handle is null");
        }
    }else {
        ret = STATUS_ERROR_STEP;
        LOGE("setFrameSize: status=%d", getStatus());
    }
    return ret;
}

int UsbCamera::setFrameCallback(int pixelFormat, jobject callback) {
    return STATUS_SUCCESS;
}

int UsbCamera::setPreview(ANativeWindow *window) {
    return STATUS_SUCCESS;
}

int UsbCamera::start() {
    if(getStatus() == STATUS_CONFIGURE){
        uvc_error_t uvc_ret = uvc_start_streaming(uvc_device_handle,
                &ctrl, frameBack, (void *) 0x8,0x2);
        LOGD("start: %d", uvc_ret);
        status = STATUS_RUN;
    } else {
        LOGW("start: status=%d", getStatus());
    }
    return STATUS_SUCCESS;
}

int UsbCamera::stop() {
    if(getStatus() == STATUS_RUN){
        uvc_stop_streaming(uvc_device_handle);
        status = STATUS_CONFIGURE;
    } else {
        LOGW("stop: status=%d", getStatus());
    }
    return STATUS_SUCCESS;
}

int UsbCamera::close() {
    int ret = STATUS_SUCCESS;
    int st = getStatus();
    if (st == STATUS_OPEN || st == STATUS_CONFIGURE){
        uvc_close(uvc_device_handle);
        if (_fd > 0) {
            ::close(_fd);
            _fd = 0;
        } else {
            uvc_unref_device(uvc_device);
        }
        status = STATUS_INIT_OK;
    } else {
        LOGW("close: status=%d", getStatus());
    }
    return ret;
}

void UsbCamera::destroy() {
    if (getStatus() == STATUS_INIT_OK){
        uvc_exit(uvc_context);
        status = STATUS_INIT_NONE;
    } else {
        LOGW("destroy: status=%d", getStatus());
    }
}

#ifdef __cplusplus
}  // extern "C"
#endif