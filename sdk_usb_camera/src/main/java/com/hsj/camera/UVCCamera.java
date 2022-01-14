package com.hsj.camera;

import android.util.Log;
import android.view.Surface;

/**
 * @Author:Hsj
 * @Date:2020-08-31
 * @Class:UVCCamera2
 * @Desc:UVCCamera2
 */
public final class UVCCamera {

    private static final String TAG = "UVCCamera";
    //Frame Format
    public static final int FRAME_FORMAT_MJPEG = 0;
    public static final int FRAME_FORMAT_YUYV  = 1;
    public static final int FRAME_FORMAT_DEPTH = 2;
    //Camera Status
    private static final int STATUS_ERROR_DESTROYED = 50;
    private static final int STATUS_ERROR_OPEN = 40;
    private static final int STATUS_ERROR_SIZE = 30;
    private static final int STATUS_ERROR_START = 20;
    private static final int STATUS_ERROR_STOP = 10;
    private static final int STATUS_SUCCESS = 0;

    static {
        System.loadLibrary("camera");
    }

//=====================================Size class===================================================


//======================================Java API====================================================

    private final long nativeObj;

    public UVCCamera() {
        this.nativeObj = nativeInit();
    }

    public final synchronized boolean create(int productId, int vendorId) {
        if (this.nativeObj == 0) {
            Log.e(TAG, "Can't be call after call destroy");
            return false;
        } else {
            int status = nativeCreate(this.nativeObj, productId, vendorId);
            Logger.d(TAG, "create: " + status);
            return STATUS_SUCCESS == status;
        }
    }

    public final String[] getSupportFrameSize() {
        return null;
    }

    public final boolean setFrameSize(int width, int height, int frameFormat) {
        if (this.nativeObj == 0) {
            Log.w(TAG, "Can't be call after call destroy");
            return false;
        } else {
            int status = nativeFrameSize(this.nativeObj, width, height, frameFormat);
            Logger.d(TAG, "setFrameSize: " + status);
            return STATUS_SUCCESS == status;
        }
    }

    public final boolean setFrameCallback(IFrameCallback frameCallback) {
        if (this.nativeObj == 0) {
            Log.w(TAG, "Can't be call after call destroy");
            return false;
        } else {
            int status = nativeFrameCallback(this.nativeObj,frameCallback);
            Logger.d(TAG, "setFrameCallback: " + status);
            return STATUS_SUCCESS == status;
        }
    }

    public final boolean setPreview(Surface surface){
        if (this.nativeObj == 0) {
            Log.w(TAG, "Can't be call after call setPreview");
            return false;
        } else {
            int status = nativePreview(this.nativeObj, surface);
            Logger.d(TAG, "setPreview: " + status);
            return STATUS_SUCCESS == status;
        }
    }

    public final synchronized boolean start() {
        if (this.nativeObj == 0) {
            Log.w(TAG, "Can't be call after call destroy");
            return false;
        } else {
            int status = nativeStart(this.nativeObj);
            Logger.d(TAG, "start: " + status);
            return STATUS_SUCCESS == status;
        }
    }

    public final synchronized boolean stop() {
        if (this.nativeObj == 0) {
            Log.w(TAG, "Can't be call after call destroy");
            return false;
        } else {
            int status = nativeStop(this.nativeObj);
            Logger.d(TAG, "stop: " + status);
            return STATUS_SUCCESS == status;
        }
    }

    public final synchronized void destroy() {
        if (this.nativeObj == 0) {
            Logger.w(TAG, "destroy: already destroyed");
        }else {
            int status = nativeDestroy(this.nativeObj);
            Logger.w(TAG, "destroy: " + status);
        }
    }

//=======================================Native API=================================================

    private native long nativeInit();

    private native int nativeCreate(long nativeObj, int productId, int vendorId);

    private native int nativeFrameSize(long nativeObj, int width, int height, int pixelFormat);

    private native int nativeFrameCallback(long nativeObj, IFrameCallback frameCallback);

    private native int nativePreview(long nativeObj, Surface surface);

    private native int nativeStart(long nativeObj);

    private native int nativeStop(long nativeObj);

    private native int nativeDestroy(long nativeObj);

}
