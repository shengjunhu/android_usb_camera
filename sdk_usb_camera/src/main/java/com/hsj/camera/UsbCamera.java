package com.hsj.camera;

import android.view.Surface;

/**
 * @Author:Hsj
 * @Date:2022/1/13
 * @Class:UsbCamera
 * @Desc:UsbCamera
 */
public final class UsbCamera {

    private static final String TAG = "UsbCamera";
    //Frame Format
    public static final int FRAME_FORMAT_YUYV  = 0;
    public static final int FRAME_FORMAT_MJPEG = 1;
    public static final int FRAME_FORMAT_H264  = 2;
    public static final int FRAME_FORMAT_H265  = 3;
    //Pixel Format
    public static final int PIXEL_FORMAT_RAW   = 0;
    public static final int PIXEL_FORMAT_NV12  = 1;
    public static final int PIXEL_FORMAT_GRAY  = 2;
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

//======================================Java API====================================================

    //C++ object id
    private final long nativeObj;

    public UsbCamera() {
        this.nativeObj = nativeInit();
    }

    public final synchronized boolean open(int productId, int vendorId) {
        boolean ret = false;
        if (this.nativeObj != 0) {
            int status = nativeOpen(this.nativeObj, productId, vendorId);
            Logger.d(TAG, "open: " + status);
            ret = (STATUS_SUCCESS == status);
        } else {
            Logger.e(TAG, "open: already destroyed");
        }
        return ret;
    }

    public final String[] getSupportFrameSize() {
        return null;
    }

    public final boolean setFrameSize(int width, int height, int frameFormat) {
        boolean ret = false;
        if (this.nativeObj != 0) {
            int status = nativeFrameSize(this.nativeObj, width, height, frameFormat);
            Logger.d(TAG, "setFrameSize: " + status);
            ret = (STATUS_SUCCESS == status);
        } else {
            Logger.e(TAG, "setFrameSize: already destroyed");
        }
        return ret;
    }

    public final boolean setFrameCallback(IFrameCallback frameCallback) {
        boolean ret = false;
        if (this.nativeObj != 0) {
            int status = nativeFrameCallback(this.nativeObj,frameCallback);
            Logger.d(TAG, "setFrameCallback: " + status);
            ret = (STATUS_SUCCESS == status);
        } else {
            Logger.e(TAG, "setFrameCallback: already destroyed");
        }
        return ret;
    }

    public final boolean setPreview(Surface surface){
        boolean ret = false;
        if (this.nativeObj != 0) {
            int status = nativePreview(this.nativeObj, surface);
            Logger.d(TAG, "setPreview: " + status);
            ret = (STATUS_SUCCESS == status);
        } else {
            Logger.e(TAG, "setPreview: already destroyed");
        }
        return ret;
    }

    public final synchronized boolean start() {
        boolean ret = false;
        if (this.nativeObj != 0) {
            int status = nativeStart(this.nativeObj);
            Logger.d(TAG, "start: " + status);
            ret = (STATUS_SUCCESS == status);
        } else {
            Logger.e(TAG, "start: already destroyed");
        }
        return ret;
    }

    public final synchronized boolean stop() {
        boolean ret = false;
        if (this.nativeObj != 0) {
            int status = nativeStop(this.nativeObj);
            Logger.d(TAG, "stop: " + status);
            ret = (STATUS_SUCCESS == status);
        } else {
            Logger.e(TAG, "stop: already destroyed");
        }
        return ret;
    }

    public final synchronized boolean close() {
        boolean ret = false;
        if (this.nativeObj != 0) {
            int status = nativeClose(this.nativeObj);
            Logger.d(TAG, "close: " + status);
            ret = (STATUS_SUCCESS == status);
        } else {
            Logger.e(TAG, "close: already destroyed");
        }
        return ret;
    }

    public final synchronized void destroy() {
        if (this.nativeObj != 0) {
            int status = nativeDestroy(this.nativeObj);
            Logger.d(TAG, "destroy: " + status);
        } else {
            Logger.e(TAG, "destroy: already destroyed");
        }
    }

//=======================================Native API=================================================

    private native long nativeInit();

    private native int nativeOpen(long nativeObj, int productId, int vendorId);

    private native int nativeFrameSize(long nativeObj, int width, int height, int pixelFormat);

    private native int nativeFrameCallback(long nativeObj, IFrameCallback frameCallback);

    private native int nativePreview(long nativeObj, Surface surface);

    private native int nativeStart(long nativeObj);

    private native int nativeStop(long nativeObj);

    private native int nativeClose(long nativeObj);

    private native int nativeDestroy(long nativeObj);

}
