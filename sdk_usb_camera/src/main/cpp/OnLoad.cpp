//
// Created by Hsj on 2022/1/13.
//

#include "Common.h"
#define OBJECT_ID "nativeObj"

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

//======================================Preview=====================================================

#include "Preview.h"

#define CAMERA_VIEW "com/hsj/camera/CameraView"

typedef jlong PREVIEW_ID;

static PREVIEW_ID nativeCreate(JNIEnv *env, jobject thiz, jobject surface, jint frameW, jint frameH, jint format) {
    if (surface) {
        ANativeWindow *window = ANativeWindow_fromSurface(env, surface);
        ANativeWindow_setBuffersGeometry(window, frameW, frameH, WINDOW_FORMAT_RGBA_8888);
        auto *preview = new Preview(frameW, frameH, (DataFormat)format, ANativeWindow_fromSurface(env, surface));
        auto previewId = reinterpret_cast<PREVIEW_ID>(preview);
        setFieldLong(env, thiz, OBJECT_ID, previewId);
        return previewId;
    } else {
        LOGE("Preview: nativeCreate surface null");
        return STATUS_ERROR_NONE;
    }
}

static void nativeUpdate(JNIEnv *env, jobject thiz, PREVIEW_ID previewId, jbyteArray frame) {
    auto *preview = reinterpret_cast<Preview *>(previewId);
    if (LIKELY(preview)) {
        preview->render((uint8_t *) env->GetByteArrayElements(frame, JNI_FALSE));
    }
}

static void nativeRender(JNIEnv *env, jobject thiz, PREVIEW_ID previewId, jobject frame) {
    auto *preview = reinterpret_cast<Preview *>(previewId);
    if (LIKELY(preview)) {
        preview->render((uint8_t *) env->GetDirectBufferAddress(frame));
    }
}

static void nativePause(JNIEnv *env, jobject thiz, PREVIEW_ID previewId) {
    auto *preview = reinterpret_cast<Preview *>(previewId);
    if (LIKELY(preview)) {
        preview->pause();
    }
}

static void nativeDestroy3(JNIEnv *env, jobject thiz, PREVIEW_ID previewId) {
    auto *preview = reinterpret_cast<Preview *>(previewId);
    setFieldLong(env, thiz, OBJECT_ID, 0);
    SAFE_DELETE(preview);
}

static const JNINativeMethod CAMERA_VIEW_METHODS[] = {
    {"nativeCreate",   "(Landroid/view/Surface;III)J", (void *) nativeCreate},
    {"nativeUpdate",   "(J[B)V",                       (void *) nativeUpdate},
    {"nativeRender",  "(JLjava/nio/ByteBuffer;)V",     (void *) nativeRender},
    {"nativePause",    "(J)V",                         (void *) nativePause},
    {"nativeDestroy",  "(J)V",                         (void *) nativeDestroy3}
};

//======================================UsbMessenger================================================

#include "UsbMessenger.h"

#define USB_MESSENGER "com/hsj/camera/UsbMessenger"

typedef jlong MESSENGER_ID;

static MESSENGER_ID nativeInit2(JNIEnv *env, jobject thiz) {
    return 0;
}

static jint nativeOpen2(JNIEnv *env, jobject thiz, MESSENGER_ID msgerId, jint productId, jint vendorId) {
    return 0;
}

static jint nativeSyncRequest(JNIEnv *env, jobject thiz, MESSENGER_ID msgerId, jbyteArray request, jbyteArray response) {
    return 0;
}

static jint nativeClose2(JNIEnv *env, jobject thiz, MESSENGER_ID msgerId) {
    return 0;
}

static void nativeDestroy2(JNIEnv *env, jobject thiz, MESSENGER_ID msgerId) {

}

static const JNINativeMethod USB_MESSENGER_METHODS[] = {
    {"nativeInit",         "()J",             (void *) nativeInit2},
    {"nativeOpen",         "(JII)I",          (void *) nativeOpen2},
    {"nativeSyncRequest",  "(J[B[B)I",        (void *) nativeSyncRequest},
    {"nativeClose",        "(J)I",            (void *) nativeClose2 },
    {"nativeDestroy",      "(J)V",            (void *) nativeDestroy2}
};

//=======================================UsbCamera==================================================

#include "UsbCamera.h"

#define USB_CAMERA  "com/hsj/camera/UsbCamera"

typedef jlong CAMERA_ID;

static CAMERA_ID nativeInit(JNIEnv *env, jobject thiz) {
    return 0;
}

static jint nativeOpen(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jint productId, jint vendorId) {
    return 0;
}

static jint nativeFrameCallback(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject frame_callback) {
    return 0;
}

static jint nativeFrameSize(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jint width, jint height, jint format) {
    return 0;
}

static jint nativePreview(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject surface) {
    return 0;
}

static jint nativeStart(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    return 0;
}

static jint nativeStop(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    return 0;
}

static jint nativeClose(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    return 0;
}

static jint nativeDestroy(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    return 0;
}

static const JNINativeMethod USB_CAMERA_METHODS[] = {
    {"nativeInit",          "()J",                                 (void *) nativeInit},
    {"nativeOpen",          "(JII)I",                              (void *) nativeOpen},
    {"nativeFrameSize",     "(JIII)I",                             (void *) nativeFrameSize},
    {"nativeFrameCallback", "(JLcom/hsj/camera/IFrameCallback;)I", (void *) nativeFrameCallback},
    {"nativePreview",       "(JLandroid/view/Surface;)I",          (void *) nativePreview},
    {"nativeStart",         "(J)I",                                (void *) nativeStart},
    {"nativeStop",          "(J)I",                                (void *) nativeStop},
    {"nativeClose",         "(J)I",                                (void *) nativeClose},
    {"nativeDestroy",       "(J)I",                                (void *) nativeDestroy}
};

//==================================================================================================

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jint ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (JNI_OK == ret) {
        //Preview
        if (JNI_OK == ret){
            jclass cls = env->FindClass(CAMERA_VIEW);
            if (cls != nullptr) {
                jint size = sizeof(CAMERA_VIEW_METHODS) / sizeof(JNINativeMethod);
                ret = env->RegisterNatives(cls, CAMERA_VIEW_METHODS, size);
            }
        }
        //UsbCamera
        if (JNI_OK == ret){
            jclass cls = env->FindClass(USB_CAMERA);
            if (cls != nullptr) {
                jint size = sizeof(USB_CAMERA_METHODS) / sizeof(JNINativeMethod);
                ret = env->RegisterNatives(cls, USB_CAMERA_METHODS, size);
            }
        }
        //UsbMessenger
        if (JNI_OK == ret){
            jclass cls = env->FindClass(USB_MESSENGER);
            if (cls != nullptr) {
                jint size = sizeof(USB_MESSENGER_METHODS) / sizeof(JNINativeMethod);
                ret = env->RegisterNatives(cls, USB_MESSENGER_METHODS, size);
            }
        }
    }
    return ret == JNI_OK ? JNI_VERSION_1_6 : ret;
}
