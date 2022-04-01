//
// Created by Hsj on 2022/1/13.
//

#include "Common.h"
#include "UsbCamera.h"
#include "UsbMessenger.h"

#define OBJECT_ID "nativeObj"
#define USB_CAMERA  "com/hsj/camera/UsbCamera"
#define USB_MESSENGER "com/hsj/camera/UsbMessenger"

typedef jlong CAMERA_ID;
typedef jlong MESSENGER_ID;

static void setFieldLong(JNIEnv *env, jobject obj, const char *fieldName, jlong value) {
    jclass clazz = env->GetObjectClass(obj);
    jfieldID field = env->GetFieldID(clazz, fieldName, "J");
    if (LIKELY(field)) {
        env->SetLongField(obj, field, value);
    } else {
        LOGE("Failed to found %s", fieldName);
    }
    env->DeleteLocalRef(clazz);
}

//======================================UsbMessenger================================================

static MESSENGER_ID nativeInit2(JNIEnv *env, jobject thiz) {
    auto *messenger = new UsbMessenger();
    auto messengerId = reinterpret_cast<MESSENGER_ID>(messenger);
    setFieldLong(env, thiz, OBJECT_ID, messengerId);
    return messengerId;
}

static jint nativeConnect2(JNIEnv *env, jobject thiz, MESSENGER_ID mId, jint fd) {
    auto *messenger = reinterpret_cast<UsbMessenger *>(mId);
    jint ret = STATUS_NONE_INIT;
    if (messenger) {
        ret = messenger->connect(fd);
    } else {
        LOGE("connect: UsbMessenger had been release.");
    }
    return ret;
}

static jint nativeOpen2(JNIEnv *env, jobject thiz, MESSENGER_ID mId, jint vid, jint pid, jint bus, jint dev) {
    auto *messenger = reinterpret_cast<UsbMessenger *>(mId);
    jint ret = STATUS_NONE_INIT;
    if (messenger) {
        ret = messenger->open(vid, pid, bus, dev);
    } else {
        LOGE("open: UsbMessenger had been release.");
    }
    return ret;
}

static jint nativeSyncRequest2(JNIEnv *env, jobject thiz, MESSENGER_ID mId, jbyteArray in, jbyteArray out) {
    auto *messenger = reinterpret_cast<UsbMessenger *>(mId);
    jint ret = STATUS_NONE_INIT;
    if (messenger) {
        char *request = (char *) env->GetByteArrayElements(in, JNI_FALSE);
        char *response = (char *) env->GetByteArrayElements(out, JNI_FALSE);
        ret = messenger->syncRequest(request, response);
    } else {
        LOGE("syncRequest: UsbMessenger had been release.");
    }
    return ret;
}

static jint nativeClose2(JNIEnv *env, jobject thiz, MESSENGER_ID mId) {
    auto *messenger = reinterpret_cast<UsbMessenger *>(mId);
    jint ret = STATUS_SUCCESS;
    if (messenger) {
        ret = messenger->close();
    } else {
        LOGE("close: UsbMessenger had been release.");
    }
    return ret;
}

static void nativeDestroy2(JNIEnv *env, jobject thiz, MESSENGER_ID mId) {
    auto *messenger = reinterpret_cast<UsbMessenger *>(mId);
    if (messenger) {
        messenger->destroy();
        setFieldLong(env, thiz, OBJECT_ID, 0);
        SAFE_DELETE(messenger);
    } else {
        LOGW("destroy: UsbMessenger had been release.");
    }
}

static const JNINativeMethod USB_MESSENGER_METHODS[] = {
    {"nativeInit",         "()J",       (void *) nativeInit2},
    {"nativeConnect",      "(JI)I",     (void *) nativeConnect2},
    {"nativeOpen",         "(JIIII)I",  (void *) nativeOpen2},
    {"nativeSyncRequest",  "(J[B[B)I",  (void *) nativeSyncRequest2},
    {"nativeClose",        "(J)I",      (void *) nativeClose2},
    {"nativeDestroy",      "(J)V",      (void *) nativeDestroy2}
};

//=======================================UsbCamera==================================================

static CAMERA_ID nativeInit(JNIEnv *env, jobject thiz) {
    auto *camera = new UsbCamera();
    auto cameraId = reinterpret_cast<CAMERA_ID>(camera);
    setFieldLong(env, thiz, OBJECT_ID, cameraId);
    return cameraId;
}

static jint nativeConnect(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jint fd) {
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    jint ret = STATUS_NONE_INIT;
    if (camera) {
        ret = camera->connect(fd);
    } else {
        LOGE("open: UsbCamera had been release.");
    }
    return ret;
}

static jint nativeOpen(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jint vid, jint pid, jint bus, jint dev) {
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    jint ret = STATUS_NONE_INIT;
    if (camera) {
        ret = camera->open(vid, pid, bus, dev);
    } else {
        LOGE("open: UsbCamera had been release.");
    }
    return ret;
}

static jint nativeGetSupportInfo(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject supportInfos) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    if (camera) {
        if(supportInfos){
            std::vector<SupportInfo> infos;
            ret = camera->getSupportInfo(infos);
            if (ret == STATUS_SUCCESS) {
                if (!infos.empty()) {
                    jclass cls = env->GetObjectClass(supportInfos);
                    jmethodID mid = env->GetMethodID(cls,"add","(Ljava/lang/Object;)Z");
                    jclass cls2 = env->FindClass("com/hsj/camera/UsbCamera$SupportInfo");
                    jmethodID mid2 = env->GetMethodID(cls2, "<init>","(IIII)V");
                    for(auto & info : infos) {
                        jobject obj = env->NewObject(cls2, mid2, (int)info.format, info.width, info.height, info.fps);
                        env->CallBooleanMethod(supportInfos, mid, obj);
                        env->DeleteLocalRef(obj);
                    }
                } else{
                    ret = STATUS_EMPTY_DATA;
                    LOGE("getSupportInfo: supportInfo empty");
                }
            } else {
                ret = STATUS_EXE_FAILED;
                LOGE("getSupportInfo: failed: %d.", ret);
            }
        } else {
            ret = STATUS_EMPTY_PARAM;
            LOGE("getSupportInfo: supportInfos is null.");
        }
    } else {
        LOGE("getSupportSize: UsbCamera had been release.");
    }
    return ret;
}

static jint nativeSetSupportInfo(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject supportInfo) {
    jint ret = STATUS_NONE_INIT;
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    if (camera) {
        if (supportInfo){
            jclass cls = env->GetObjectClass(supportInfo);
            jfieldID fId  = env->GetFieldID(cls, "format","I");
            jfieldID fId2 = env->GetFieldID(cls, "width", "I");
            jfieldID fId3 = env->GetFieldID(cls, "height","I");
            jfieldID fId4 = env->GetFieldID(cls, "fps",   "I");
            if(fId && fId2 && fId3 && fId4) {
                SupportInfo info(env->GetIntField(supportInfo, fId),
                                 env->GetIntField(supportInfo, fId2),
                                 env->GetIntField(supportInfo, fId3),
                                 env->GetIntField(supportInfo, fId4));
                ret = camera->setSupportInfo(info);
            } else {
                ret = STATUS_ERROR_NONE;
                LOGE("setSupportInfo: com.hsj.camera.UsbCamera$SupportInfo class had been changed.");
            }
        } else {
            ret = STATUS_EMPTY_PARAM;
            LOGE("setSupportInfo: supportInfo is null.");
        }
    } else {
        LOGE("setSupportInfo: UsbCamera had been release.");
    }
    return ret;
}

static jint nativeFrameCallback(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jint format,jobject callback) {
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    jint ret = STATUS_NONE_INIT;
    if (camera) {
        jobject obj = nullptr;
        if (callback != nullptr) {
            obj = env->NewGlobalRef(callback);
        } else {
            LOGW("setFrameCallback: callback is null.");
        }
        ret = camera->setFrameCallback(format, obj);
    } else {
        LOGE("setFrameCallback: UsbCamera had been release.");
    }
    return ret;
}

static jint nativePreview(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject surface) {
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    jint ret = STATUS_NONE_INIT;
    if (camera) {
        ANativeWindow* window = nullptr;
        if (surface!= nullptr){
            window = ANativeWindow_fromSurface(env, surface);
        } else {
            LOGW("setPreview: surface is null.");
        }
        ret = camera->setPreview(window);
    } else {
        LOGE("open: UsbCamera had been release.");
    }
    return ret;
}

static jint nativeStart(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    jint ret = STATUS_NONE_INIT;
    if (camera) {
        ret = camera->start();
    } else {
        LOGE("start: UsbCamera had been release.");
    }
    return ret;
}

static jint nativeStop(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    jint ret = STATUS_NONE_INIT;
    if (camera) {
        ret = camera->stop();
    } else {
        LOGE("stop: UsbCamera had been release.");
    }
    return ret;
}

static jint nativeClose(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    jint ret = STATUS_SUCCESS;
    if (camera) {
        ret = camera->close();
    } else {
        LOGE("close: UsbCamera had been release.");
    }
    return ret;
}

static void nativeDestroy(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    auto *camera = reinterpret_cast<UsbCamera *>(cameraId);
    if (camera) {
        camera->destroy();
        setFieldLong(env, thiz, OBJECT_ID, 0);
        SAFE_DELETE(camera);
    } else {
        LOGW("destroy: UsbCamera had been release.");
    }
}

static const JNINativeMethod USB_CAMERA_METHODS[] = {
    {"nativeInit",          "()J",                                         (void *) nativeInit},
    {"nativeConnect",       "(JI)I",                                       (void *) nativeConnect},
    {"nativeOpen",          "(JIIII)I",                                    (void *) nativeOpen},
    {"nativeGetSupportInfo","(JLjava/util/List;)I",                        (void *) nativeGetSupportInfo},
    {"nativeSetSupportInfo","(JLcom/hsj/camera/UsbCamera$SupportInfo;)I",  (void *) nativeSetSupportInfo},
    {"nativeFrameCallback", "(JILcom/hsj/camera/FrameCallback;)I",         (void *) nativeFrameCallback},
    {"nativePreview",       "(JLandroid/view/Surface;)I",                  (void *) nativePreview},
    {"nativeStart",         "(J)I",                                        (void *) nativeStart},
    {"nativeStop",          "(J)I",                                        (void *) nativeStop},
    {"nativeClose",         "(J)I",                                        (void *) nativeClose},
    {"nativeDestroy",       "(J)V",                                        (void *) nativeDestroy}
};

//==================================================================================================

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jint ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (JNI_OK == ret) {
        //UsbMessenger
        if (JNI_OK == ret) {
            jclass cls = env->FindClass(USB_MESSENGER);
            if (cls != nullptr) {
                jint size = sizeof(USB_MESSENGER_METHODS) / sizeof(JNINativeMethod);
                ret = env->RegisterNatives(cls, USB_MESSENGER_METHODS, size);
            } else {
                LOGE("RegisterNatives failed: not find %s class", USB_MESSENGER);
                ret = JNI_ERR;
            }
        }
        //UsbCamera
        if (JNI_OK == ret) {
            jclass cls = env->FindClass(USB_CAMERA);
            if (cls != nullptr) {
                jint size = sizeof(USB_CAMERA_METHODS) / sizeof(JNINativeMethod);
                ret = env->RegisterNatives(cls, USB_CAMERA_METHODS, size);
            } else {
                LOGE("RegisterNatives failed: not find %s class", USB_CAMERA);
                ret = JNI_ERR;
            }
        }
        //JVM
        if (JNI_OK == ret) setJVM(vm);
    }
    return ret == JNI_OK ? JNI_VERSION_1_6 : ret;
}
