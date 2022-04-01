package com.hsj.camera;

import android.text.TextUtils;
import android.view.Surface;
import java.nio.ByteBuffer;
import java.util.List;

/**
 * @Author:Hsj
 * @Date:2022/1/13
 * @Class:UsbCamera
 * @Desc:UsbCamera
 */
public final class UsbCamera {

    private static final String TAG = "UsbCamera";
    //Frame Format: These match the enums in cpp/libcamera/Camera.h
    public static final int FRAME_FORMAT_YUYV        = 0x04;
    public static final int FRAME_FORMAT_MJPEG       = 0x06;
    public static final int FRAME_FORMAT_H264        = 0x08;
    //Pixel Format: These match the enums in cpp/libcamera/Camera.h
    public static final int PIXEL_FORMAT_RAW         = 0x04;
    public static final int PIXEL_FORMAT_YUV420_888  = 0x0A;
    //Camera Status
    public static final int STATUS_OK        =  0;
    public static final int STATUS_ERROR     = -10;
    public static final int STATUS_DESTROYED = -9;

    static {
        System.loadLibrary("camera");
    }

    public enum FLIP {
        FLIP_H(1), FLIP_V(-1), FLIP_NONE(0);

        private int flip;

        FLIP(int flip){
            this.flip = flip;
        }

        public int getValue(){
            return flip;
        }
    }

    public enum ROTATE {
        ROTATE_0(0), ROTATE_90(90), ROTATE_180(180), ROTATE_270(270);

        private int rotate;

        ROTATE(int rotate){
            this.rotate = rotate;
        }

        public int getValue(){
            return rotate;
        }
    }

    public static class SupportInfo {
        public int format;
        public int width;
        public int height;
        public int fps;
        public ROTATE rotate;
        public FLIP flip;

        public SupportInfo(int format, int width, int height, int fps) {
            this.format = format;
            this.width  = width;
            this.height = height;
            this.fps    = fps;
        }

        public void setTransfer(ROTATE rotate, FLIP flip){
            this.rotate = rotate;
            this.flip = flip;
        }

        public String getInfo(){
            String info = "Unknown";
            if (format == FRAME_FORMAT_MJPEG){
                info = "MJPEG ";
            } else if (format == FRAME_FORMAT_YUYV){
                info = "YUYV ";
            } else if (format == FRAME_FORMAT_H264){
                info = "H264 ";
            }
            info += width + " x " + height + " FPS: " + fps;
            return info;
        }
    }

//======================================Java API====================================================

    //C++ object id
    private final long nativeObj;
    private ByteBuffer frame;

    public UsbCamera() {
        this.nativeObj = nativeInit();
    }

    public UsbCamera(boolean debug) {
        this.nativeObj = nativeInit();
        Logger.OPEN = debug;
    }

    public final synchronized int open(int fileDescriptor) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeConnect(this.nativeObj, fileDescriptor);
            Logger.d(TAG, "open: " + status);
        } else {
            Logger.e(TAG, "open: already destroyed");
        }
        return status;
    }

    public final synchronized int open(int vendorId, int productId, String deviceName) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            int busNum = 0, devNum = 0;
            if (!TextUtils.isEmpty(deviceName)){
                final String[] arr = deviceName.split("/");
                if (arr.length > 2) {
                    busNum = Integer.parseInt(arr[arr.length - 2]);
                    devNum = Integer.parseInt(arr[arr.length - 1]);
                }
            }
            status = nativeOpen(this.nativeObj, vendorId, productId, busNum, devNum);
            Logger.d(TAG, "open: " + status);
        } else {
            Logger.e(TAG, "open: already destroyed");
        }
        return status;
    }

    public final synchronized int getSupportInfo(List<SupportInfo> supportInfos) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeGetSupportInfo(this.nativeObj, supportInfos);
            Logger.d(TAG, "getSupportInfo: " + status);
        } else {
            Logger.e(TAG, "getSupportInfo: already destroyed");
        }
        return status;
    }

    public final synchronized int setSupportInfo(SupportInfo supportInfo) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeSetSupportInfo(this.nativeObj, supportInfo);
            Logger.d(TAG, "setSupportInfo: " + status);
        } else {
            Logger.e(TAG, "setSupportInfo: already destroyed");
        }
        return status;
    }

    public final synchronized int setFrameCallback(int pixelFormat, FrameCallback callback) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeFrameCallback(this.nativeObj, pixelFormat, callback);
            Logger.d(TAG, "setFrameCallback: " + status);
        } else {
            Logger.e(TAG, "setFrameCallback: already destroyed");
        }
        return status;
    }

    public final synchronized int setPreview(Surface surface){
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativePreview(this.nativeObj, surface);
            Logger.d(TAG, "setPreview: " + status);
        } else {
            Logger.e(TAG, "setPreview: already destroyed");
        }
        return status;
    }

    public final synchronized int start() {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeStart(this.nativeObj);
            Logger.d(TAG, "start: " + status);
        } else {
            Logger.e(TAG, "start: already destroyed");
        }
        return status;
    }

    public final synchronized int stop() {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeStop(this.nativeObj);
            Logger.d(TAG, "stop: " + status);
        } else {
            Logger.e(TAG, "stop: already destroyed");
        }
        return status;
    }

    public final synchronized void close() {
        if (this.nativeObj != 0) {
            int status = nativeClose(this.nativeObj);
            Logger.d(TAG, "close: " + status);
        } else {
            Logger.e(TAG, "close: already destroyed");
        }
    }

    public final synchronized void destroy() {
        if (this.nativeObj != 0) {
            nativeDestroy(this.nativeObj);
        } else {
            Logger.w(TAG, "destroy: already destroyed");
        }
    }

//=======================================Native API=================================================

    private native long nativeInit();

    private native int nativeConnect(long nativeObj, int fd);

    private native int nativeOpen(long nativeObj, int vendorId, int productId, int bus, int dev);

    private native int nativeGetSupportInfo(long nativeObj, List<SupportInfo> supportInfo);

    private native int nativeSetSupportInfo(long nativeObj, SupportInfo supportInfo);

    private native int nativeFrameCallback(long nativeObj, int pixelFormat, FrameCallback callback);

    private native int nativePreview(long nativeObj, Surface surface);

    private native int nativeStart(long nativeObj);

    private native int nativeStop(long nativeObj);

    private native int nativeClose(long nativeObj);

    private native void nativeDestroy(long nativeObj);

}
