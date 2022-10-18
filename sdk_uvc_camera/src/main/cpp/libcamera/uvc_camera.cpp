//
// Created by shengjunhu on 2022/9/1.
//

#include "common.hpp"
#include "uvc_camera.h"
#include "libuvc/libuvc_internal.h"

#define MIN_BANDWIDTH 0.10f
#define MAX_BANDWIDTH 1.00f

#ifdef __cplusplus
extern "C" {
#endif

uvc_camera::uvc_camera() :
        fd(0),
        bandwidth(1.0f),
        converter(nullptr),
        uvc_device(nullptr),
        uvc_context(nullptr),
        uvc_device_handle(nullptr) {
    // 1. uvc_init()
    uvc_error_t uvc_ret = uvc_init(&uvc_context, nullptr);
    if (uvc_ret == UVC_SUCCESS) {
        status = STATUS_INIT_OK;
    } else {
        status = STATUS_INIT_NONE;
        LOGE("uvc_init(): %d.", uvc_ret);
    }
}

uvc_camera::~uvc_camera() {

}

//==================================================================================================

inline const StatusInfo uvc_camera::getStatus() const { return status; }

static void frame_callback(uvc_frame_t *frame, void *args) {
    auto *converter = reinterpret_cast<frame_converter *>(args);
    //1. Decoder: function pointer
    converter->decodeFrame((uint8_t *) frame->data, frame->data_bytes);
    //2. Render frame
    converter->renderFrame();
    //3. Send frame to Java
    converter->callbackFrame();
}

//==================================================================================================

int uvc_camera::connectDevice(const int _fd) {
    int ret = STATUS_SUCCEED;
    if (getStatus() == STATUS_INIT_OK) {
        fd = dup(_fd);
        // 2 uvc_wrap() == uvc_open()
        uvc_error_t uvc_ret = uvc_wrap(fd, uvc_context, &uvc_device_handle);
        if (uvc_ret == UVC_SUCCESS) {
            status = STATUS_OPEN;
        } else {
            close(fd);
            fd = 0;
            LOGE("connectDevice: uvc_wrap(): %d.", uvc_ret);
        }
    } else {
        ret = STATUS_ERROR_STEP;
        LOGE("connectDevice: status(%d).", getStatus());
    }
    return ret;
}

int uvc_camera::openDevice(const int vid, const int pid, const int bus, const int dev) {
    int ret = STATUS_SUCCEED;
    if (getStatus() == STATUS_INIT_OK) {
        // 2 Filter devices by vid and pid
        uvc_device_t **uvc_devices;
        uvc_error_t uvc_ret = uvc_find_devices(uvc_context, &uvc_devices, vid, pid, nullptr);
        if (uvc_ret == UVC_SUCCESS) {
            // 2.2 Filter target device by bus and dev
            int dev_index = 0;
            uvc_device_t *temp;
            while ((temp = uvc_devices[dev_index++]) != nullptr) {
                uvc_device_descriptor_t *desc;
                if (uvc_get_device_descriptor(temp, &desc) == UVC_SUCCESS) {
                    if (uvc_device == nullptr && ((bus == 0 && dev == 0) ||
                       (bus == libusb_get_bus_number(temp->usb_dev) &&
                        dev == libusb_get_device_address(temp->usb_dev)))) {
                        uvc_free_device_descriptor(desc);
                        uvc_device = temp;
                        continue;
                    }
                    //free device
                    uvc_free_device_descriptor(desc);
                    uvc_unref_device(temp);
                    SAFE_FREE(temp);
                }
            }
            SAFE_FREE(uvc_devices);
            // 2.3 Open target device
            if (uvc_device != nullptr) {
                uvc_ret = uvc_open(uvc_device, &uvc_device_handle);
                if (uvc_ret == UVC_SUCCESS) {
                    status = STATUS_OPEN;
                } else {
                    LOGE("openDevice: uvc_open: %d", uvc_ret);
                    ret = STATUS_FAILED;
                }
            } else {
                LOGE("openDevice: uvc_find_device(): no target device.");
            }
        } else {
            LOGE("openDevice: uvc_find_devices(): no target devices: %d.", uvc_ret);
            ret = STATUS_FAILED;
        }
    } else {
        ret = STATUS_ERROR_STEP;
        LOGE("openDevice: status(%d).", getStatus());
    }
    return ret;
}

int uvc_camera::getSupportInfo(std::vector<SupportInfo> &supportInfo) {
    int ret = STATUS_SUCCEED;
    if (getStatus() == STATUS_OPEN) {
        // 3. Filter all support format and size
        if (uvc_device_handle) {
            if (uvc_device_handle->info->stream_ifs) {
                uvc_streaming_interface_t *stream_if;
                DL_FOREACH(uvc_device_handle->info->stream_ifs, stream_if) {
                    uvc_format_desc_t *format_desc;
                    DL_FOREACH(stream_if->format_descs, format_desc) {
                        //frame type:    format_desc->bDescriptorSubtype
                        //frame format:  format_desc->fourccFormat
                        //format default: format_desc->bDefaultFrameIndex
                        uvc_frame_desc_t *frame_desc;
                        DL_FOREACH(format_desc->frame_descs, frame_desc) {
                            //width:  frame_desc->wWidth
                            //height: frame_desc->wHeight
                            //fps:    10000000 / frame_desc->dwDefaultFrameInterval
                            uint32_t fps = 10000000 / frame_desc->dwDefaultFrameInterval;
                            SupportInfo info(format_desc->bDescriptorSubtype, format_desc->fourccFormat,
                                    frame_desc->wWidth, frame_desc->wHeight, fps);
                            supportInfo.emplace_back(info);
                        }
                    }
                }
            } else {
                ret = STATUS_EMPTY_DATA;
                LOGE("getSupportInfo: uvc_streaming_interface_t is null.");
            }
        } else {
            ret = STATUS_EMPTY_OBJ;
            LOGE("getSupportInfo: uvc_device_handle is null.");
        }
    } else {
        ret = STATUS_ERROR_STEP;
        LOGE("getSupportInfo: status(%d).", getStatus());
    }
    return ret;
}

int uvc_camera::setSupportInfo(SupportInfo &supportInfo) {
    int ret = STATUS_SUCCEED;
    int _status = getStatus();
    if (_status == STATUS_OPEN || _status == STATUS_CONFIGURE) {
        // 4. Configure format、size and fps
        if (uvc_device_handle) {
            enum uvc_frame_format format;
            if (supportInfo.type == UVC_VS_FORMAT_UNCOMPRESSED){
                format = UVC_FRAME_FORMAT_YUYV;
            } else if (supportInfo.type == UVC_VS_FORMAT_MJPEG) {
                format = UVC_FRAME_FORMAT_MJPEG;
            } else if (supportInfo.type == UVC_VS_FORMAT_FRAME_BASED) {
                format = UVC_FRAME_FORMAT_H264;
            } else {
                format = UVC_FRAME_FORMAT_ANY;
            }
            uvc_error_t uvc_ret = uvc_get_stream_ctrl_format_size(uvc_device_handle, &ctrl,
                            format, supportInfo.width, supportInfo.height, supportInfo.fps);
            if (uvc_ret == UVC_SUCCESS) {
                if (supportInfo.bandwidth < MIN_BANDWIDTH || supportInfo.bandwidth > MAX_BANDWIDTH){
                    this->bandwidth = MAX_BANDWIDTH;
                } else {
                    this->bandwidth = supportInfo.bandwidth;
                }
                status = STATUS_CONFIGURE;
            } else {
                ret = STATUS_FAILED;
                LOGE("setSupportInfo: uvc_get_stream_ctrl_format_size() failed: %d.", uvc_ret);
            }
        } else {
            ret = STATUS_EMPTY_OBJ;
            LOGE("setSupportInfo: uvc_device_handle is null.");
        }
    } else {
        ret = STATUS_ERROR_STEP;
        LOGE("setSupportInfo: status(%d).", getStatus());
    }
    return ret;
}

int uvc_camera::setFrameConverter(frame_converter *_converter) {
    // 5. Configure frame process
    int ret = STATUS_SUCCEED;
    if (getStatus() == STATUS_CONFIGURE) {
        SAFE_DELETE(this->converter);
        this->converter = _converter;
    } else {
        ret = STATUS_ERROR_STEP;
        LOGW("setFrameConverter: status(%d).", getStatus());
    }
    return ret;
}

int uvc_camera::setFramePreview(jlong previewId) {
    int ret = STATUS_SUCCEED;
    // 6. Configure frame preview
    if (getStatus() == STATUS_CONFIGURE) {
        if (this->converter != nullptr) {
            this->converter->setPreview(previewId);
        } else {
            ret = STATUS_ERROR_STEP;
            LOGW("setFramePreview: FrameConverter is null.");
        }
    } else {
        ret = STATUS_ERROR_STEP;
        LOGE("setFramePreview: status(%d).", getStatus());
    }
    return ret;
}

int uvc_camera::startStream() {
    int ret = STATUS_SUCCEED;
    // 7. Start streaming
    if (getStatus() == STATUS_CONFIGURE) {
        uvc_error_t uvc_ret = uvc_start_streaming(uvc_device_handle,
                    &ctrl, frame_callback, converter, bandwidth);
        if (uvc_ret == UVC_SUCCESS) {
            status = STATUS_RUN;
        } else {
            ret = STATUS_FAILED;
            LOGE("startStream: uvc_start_streaming(): %d.", uvc_ret);
        }
    } else {
        ret = STATUS_ERROR_STEP;
        LOGW("startStream: status(%d).", getStatus());
    }
    return ret;
}

int uvc_camera::takePhoto(){
    return STATUS_FAILED;
}

int uvc_camera::startVideo(){
    return STATUS_FAILED;
}

int uvc_camera::stopVideo(){
    return STATUS_FAILED;
}

int uvc_camera::stopStream() {
    // 8. Stop streaming
    if (getStatus() == STATUS_RUN) {
        uvc_stop_streaming(uvc_device_handle);
        status = STATUS_CONFIGURE;
        converter->renderPause();
    } else {
        LOGW("stopStream: status(%d).", getStatus());
    }
    return STATUS_SUCCEED;
}

int uvc_camera::closeDevice() {
    int ret = STATUS_SUCCEED;
    // 9. Close device
    if (getStatus() == STATUS_OPEN || getStatus() == STATUS_CONFIGURE) {
        if (uvc_device_handle != nullptr) {
            uvc_close(uvc_device_handle);
            uvc_device_handle = nullptr;
        }
        if (fd > 0) {
            ::close(fd);
            fd = 0;
        } else if (uvc_device != nullptr) {
            uvc_unref_device(uvc_device);
            SAFE_FREE(uvc_device);
        }
        SAFE_DELETE(this->converter);
        status = STATUS_INIT_OK;
    } else {
        LOGW("closeDevice: status(%d).", getStatus());
    }
    return ret;
}

void uvc_camera::destroy() {
    // 10. Exit uvc and release resource
    if (getStatus() == STATUS_INIT_OK) {
        if (uvc_context != nullptr) {
            uvc_exit(uvc_context);
            uvc_context = nullptr;
        }
        status = STATUS_INIT_NONE;
    } else {
        LOGW("destroy: status(%d).", getStatus());
    }
}

#ifdef __cplusplus
}  // extern "C"
#endif