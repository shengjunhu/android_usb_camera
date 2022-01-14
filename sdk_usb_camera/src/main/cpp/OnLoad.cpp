//
// Created by Hsj on 2022/1/13.
//

#include "Common.h"

#define OBJECT_ID   "nativeObj"
#define CLASS_NAME  "com/hsj/camera/CameraView"
#define CLASS_NAME2 "com/hsj/camera/UsbCamera"
#define CLASS_NAME3 "com/hsj/camera/UsbMessenger"

//==================================================================================================

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

//==================================================================================================

#include "Preview.h"

typedef jlong PREVIEW_ID;

static PREVIEW_ID nativeCreate2(JNIEnv *env, jobject thiz, jobject surface, jint frameW, jint frameH, jint format) {
    return 0;
}

static void nativeUpdate2(JNIEnv *env, jobject thiz, PREVIEW_ID previewId, jbyteArray frame) {

}

static void nativeUpdate2_(JNIEnv *env, jobject thiz, PREVIEW_ID previewId, jobject frame) {
}

static void nativePause2(JNIEnv *env, jobject thiz, PREVIEW_ID previewId) {

}

static void nativeDestroy2(JNIEnv *env, jobject thiz, PREVIEW_ID previewId) {

}

static const JNINativeMethod METHODS[] = {
        {"nativeCreate",        "(Landroid/view/Surface;III)J",        (void *) nativeCreate2},
        {"nativeUpdate",        "(J[B)V",                              (void *) nativeUpdate2},
        {"nativeUpdate2",       "(JLjava/nio/ByteBuffer;)V",           (void *) nativeUpdate2_},
        {"nativePause",         "(J)V",                                (void *) nativePause2},
        {"nativeDestroy",       "(J)V",                                (void *) nativeDestroy2}
};

//==================================================================================================

#include "UsbCamera.h"

typedef jlong CAMERA_ID;

static CAMERA_ID nativeInit(JNIEnv *env, jobject thiz) {
    return 0;
}

static jint nativeCreate(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, int productId, jint vendorId) {
    return 0;
}

static jint nativeAutoExposure(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jboolean isAuto) {
    return 0;
}

static jint nativeSetExposure(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, int level) {
    return 0;
}

static jint nativeFrameCallback(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jobject frame_callback) {
    return 0;
}

static jint nativeFrameSize(JNIEnv *env, jobject thiz, CAMERA_ID cameraId, jint width, jint height, jint frameFormat) {
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

static jint nativeDestroy(JNIEnv *env, jobject thiz, CAMERA_ID cameraId) {
    return 0;
}

static const JNINativeMethod METHODS2[] = {
        {"nativeInit",          "()J",                                 (void *) nativeInit},
        {"nativeCreate",        "(JII)I",                              (void *) nativeCreate},
        {"nativeAutoExposure",  "(JZ)I",                               (void *) nativeAutoExposure},
        {"nativeSetExposure",   "(JI)I",                               (void *) nativeSetExposure},
        {"nativeFrameCallback", "(JLcom/hsj/camera/IFrameCallback;)I", (void *) nativeFrameCallback},
        {"nativeFrameSize",     "(JIII)I",                             (void *) nativeFrameSize},
        {"nativePreview",       "(JLandroid/view/Surface;)I",          (void *) nativePreview},
        {"nativeStart",         "(J)I",                                (void *) nativeStart},
        {"nativeStop",          "(J)I",                                (void *) nativeStop},
        {"nativeDestroy",       "(J)I",                                (void *) nativeDestroy}
};

//==================================================================================================

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM *vm, void *reserved) {
    JNIEnv *env;
    jint ret = vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6);
    if (JNI_OK == ret) {
        //Preview
        if (JNI_OK == ret){
            jclass cls = env->FindClass(CLASS_NAME);
            if (cls != nullptr) {
                jint size = sizeof(METHODS) / sizeof(JNINativeMethod);
                ret = env->RegisterNatives(cls, METHODS, size);
            }
        }

        //UsbCamera
        if (JNI_OK == ret){
            jclass cls = env->FindClass(CLASS_NAME2);
            if (cls != nullptr) {
                jint size = sizeof(METHODS2) / sizeof(JNINativeMethod);
                ret = env->RegisterNatives(cls, METHODS2, size);
            }
        }

    }
    return ret == JNI_OK ? JNI_VERSION_1_6 : ret;
}
