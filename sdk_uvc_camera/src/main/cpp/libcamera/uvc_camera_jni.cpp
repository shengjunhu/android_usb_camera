//
// Created by shengjunhu on 2022/9/1.
//

#include "common.hpp"
#include "uvc_camera.h"

#define OBJECT_ID   "nativeObj"
#define UVC_CAMERA  "com/hsj/camera/UVCCamera"
#define CAMERA_VIEW "com/hsj/camera/CameraView"

typedef jlong VIEW_ID;
typedef jlong CAMERA_ID;
static JavaVM *jvm = nullptr;

static void setFieldLong(JNIEnv *env, jobject obj, const char *fieldName, jlong value) {
    jclass clazz = env->GetObjectClass(obj);
    jfieldID field = env->GetFieldID(clazz, fieldName, "J");
    if (LIKELY(field)) {
        env->SetLongField(obj, field, value);
    } else {
        LOGE("setFieldLong: failed to found %s.", fieldName);
    }
    env->DeleteLocalRef(clazz);
}

static CAMERA_ID nativeInit(JNIEnv *env, jobject thiz) {
    auto *camera = new uvc_camera();
    auto cameraId = reinterpret_cast<CAMERA_ID>(camera);
    setFieldLong(env, thiz, OBJECT_ID, cameraId);
    return cameraId;
}

static jint nativeConnect(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jint fd) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        ret = camera->connectDevice(fd);
    } else {
        LOGE("nativeConnect: already destroyed.");
    }
    return ret;
}

static jint nativeOpen(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jint vid, jint pid, jstring usbfs) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        int bus = 0, dev = 0;
        if (usbfs != nullptr){
            auto *_usbfs = (const char *) env->GetStringChars(usbfs, JNI_FALSE);
            if(2 == sscanf(_usbfs, "/dev/bus/usb/%d/%d", &bus, &dev)){
                ret = camera->openDevice(vid, pid, bus, dev);
            } else {
                ret = STATUS_FAILED;
                LOGE("nativeOpen: parse usbfs(%s) failed.", _usbfs);
            }
            env->ReleaseStringUTFChars(usbfs, _usbfs);
        }
    } else {
        LOGE("nativeOpen: already destroyed.");
    }
    return ret;
}

static jint nativeGetSupportInfo(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject supportInfos) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        if(supportInfos != nullptr){
            std::vector<SupportInfo> infos;
            ret = camera->getSupportInfo(infos);
            if (ret == STATUS_SUCCEED) {
                if (!infos.empty()) {
                    jclass cls = env->GetObjectClass(supportInfos);
                    jmethodID mid = env->GetMethodID(cls,"add","(Ljava/lang/Object;)Z");
                    jclass cls2 = env->FindClass("com/hsj/camera/UVCCamera$SupportInfo");
                    jmethodID mid2 = env->GetMethodID(cls2, "<init>","(ILjava/lang/String;III)V");
                    for(auto &info : infos) {
                        jstring format = env->NewStringUTF((char *)info.format);
                        jobject obj = env->NewObject(cls2, mid2, (jint)info.type, format, info.width, info.height, info.fps);
                        env->CallBooleanMethod(supportInfos, mid, obj);
                        env->DeleteLocalRef(obj);
                    }
                } else{
                    ret = STATUS_EMPTY_DATA;
                    LOGE("nativeGetSupportInfo: supportInfo empty.");
                }
            } else {
                ret = STATUS_FAILED;
                LOGE("nativeGetSupportInfo: failed(%d).", ret);
            }
        } else {
            ret = STATUS_EMPTY_PARAM;
            LOGE("nativeGetSupportInfo: supportInfos is null.");
        }
    } else {
        LOGE("nativeGetSupportInfo: already destroyed.");
    }
    return ret;
}

static jint nativeSetSupportInfo(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject supportInfo) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        if (supportInfo){
            jclass cls = env->GetObjectClass(supportInfo);
            jfieldID fId1 = env->GetFieldID(cls, "type",  "I");
            jfieldID fId2 = env->GetFieldID(cls, "format","Ljava/lang/String;");
            jfieldID fId3 = env->GetFieldID(cls, "width", "I");
            jfieldID fId4 = env->GetFieldID(cls, "height","I");
            jfieldID fId5 = env->GetFieldID(cls, "fps",   "I");
            jfieldID fId6 = env->GetFieldID(cls, "pixel", "I");

            jint type     = env->GetIntField(supportInfo, fId1);
            auto format   = (jstring)env->GetObjectField(supportInfo, fId2);
            auto _format  = (uint8_t *)env->GetStringChars(format, JNI_FALSE);
            jint width    = env->GetIntField(supportInfo, fId3);
            jint height   = env->GetIntField(supportInfo, fId4);
            jint fps      = env->GetIntField(supportInfo, fId5);
            jint pixel    = env->GetIntField(supportInfo, fId6);

            SupportInfo info(type, _format, width, height, fps);
            ret = camera->setSupportInfo(info);
            if(STATUS_SUCCEED == ret){
                auto *converter = new frame_converter(info.width, info.height, (FrameFormat)info.type, (PixelFormat)pixel);
                if(pixel != PIXEL_FORMAT_NONE){
                    if (converter->buffer_size != 0){
                        jclass cls2 = env->GetObjectClass(thiz);

                        //UVCCamera.java: ByteBuffer frame
                        jfieldID fId = env->GetFieldID(cls2, "frame", "Ljava/nio/ByteBuffer;");
                        jobject buffer = env->NewDirectByteBuffer(converter->buffer, converter->buffer_size);
                        env->SetObjectField(thiz, fId, buffer);

                        //UVCCamera.java: updateFrame()
                        jmethodID mid = env->GetMethodID(cls2, "updateFrame", "()V");
                        jobject obj = env->NewGlobalRef(thiz);

                        //Jni: setCallback
                        converter->setCallback(jvm, obj, mid);
                    } else {
                        ret = STATUS_FAILED;
                        LOGE("nativeSetSupportInfo: pixel(%d) is unsupported.", pixel);
                    }
                } else {
                    LOGI("nativeSetSupportInfo: pixel(%d), frame well not be send to UVCCamera.java.", pixel);
                }
                camera->setFrameConverter(converter);
            } else {
                LOGE("nativeSetSupportInfo: failed(%d).", ret);
            }
        } else {
            ret = STATUS_EMPTY_PARAM;
            LOGE("nativeSetSupportInfo: supportInfo is null.");
        }
    } else {
        LOGE("nativeSetSupportInfo: already destroyed.");
    }
    return ret;
}

static jint nativeSetFramePreview(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject obj) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr && obj != nullptr) {
        jclass cls = env->GetObjectClass(obj);
        jfieldID fId = env->GetFieldID(cls, "nativeObj", "J");
        jlong previewId = env->GetLongField(obj, fId);
        ret = camera->setFramePreview(previewId);
    } else {
        LOGE("nativeSetPreview: already destroyed.");
    }
    return ret;
}

static jint nativeStart(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        //ret = camera->startStream();
    } else {
        LOGE("nativeStart: already destroyed.");
    }
    return ret;
}

static jint nativeTakePhoto(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        ret = camera->takePhoto();
    } else {
        LOGE("nativeTakePhoto: already destroyed.");
    }
    return ret;
}

static jint nativeStartVideo(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        ret = camera->startVideo();
    } else {
        LOGE("nativeStartVideo: already destroyed.");
    }
    return ret;
}

static jint nativeStopVideo(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        ret = camera->stopVideo();
    } else {
        LOGE("nativeStopVideo: already destroyed.");
    }
    return ret;
}

static jint nativeStop(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        ret = camera->stopStream();
    } else {
        LOGE("nativeStop: already destroyed.");
    }
    return ret;
}

static jint nativeClose(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        ret = camera->closeDevice();
    } else {
        LOGW("nativeClose: already destroyed.");
    }
    return ret;
}

static void nativeDestroy(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    auto *camera = reinterpret_cast<uvc_camera *>(cameraId);
    if (camera != nullptr) {
        camera->destroy();
        setFieldLong(env, thiz, OBJECT_ID, 0);
        SAFE_DELETE(camera);
    } else {
        LOGW("nativeDestroy: already destroyed.");
    }
}

static const JNINativeMethod USB_CAMERA_METHODS[] = {
    {"nativeInit",           "()J",                                        (void *) nativeInit},
    {"nativeOpen",           "(JIILjava/lang/String;)I",                   (void *) nativeOpen},
    {"nativeConnect",        "(JI)I",                                      (void *) nativeConnect},
    {"nativeGetSupportInfo", "(JLjava/util/List;)I",                       (void *) nativeGetSupportInfo},
    {"nativeSetSupportInfo", "(JLcom/hsj/camera/UVCCamera$SupportInfo;)I", (void *) nativeSetSupportInfo},
    {"nativeSetFramePreview","(JLcom/hsj/camera/CameraView;)I",            (void *) nativeSetFramePreview},
    {"nativeStart",          "(J)I",                                       (void *) nativeStart},
    {"nativeTakePhoto",      "(J)I",                                       (void *) nativeTakePhoto},
    {"nativeStartVideo",     "(J)I",                                       (void *) nativeStartVideo},
    {"nativeStopVideo",      "(J)I",                                       (void *) nativeStopVideo},
    {"nativeStop",           "(J)I",                                       (void *) nativeStop},
    {"nativeClose",          "(J)I",                                       (void *) nativeClose},
    {"nativeDestroy",        "(J)V",                                       (void *) nativeDestroy}
};

//==================================================================================================

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jint ret = vm->GetEnv((void**)&env, JNI_VERSION_1_6);
    if (ret == JNI_OK) {
        jclass cls = env->FindClass(UVC_CAMERA);
        if (cls != nullptr) {
            jint size = sizeof(USB_CAMERA_METHODS) / sizeof(JNINativeMethod);
            ret = env->RegisterNatives(cls, USB_CAMERA_METHODS, size);
            if (ret == JNI_OK) jvm = vm;
        } else {
            LOGE("JNI_OnLoad failed: not find %s.", UVC_CAMERA);
            ret = JNI_ERR;
        }
    }
    return ret == JNI_OK ? JNI_VERSION_1_6 : ret;
}