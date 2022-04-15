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
    //Camera Status
    public static final int STATUS_OK        =  0;
    public static final int STATUS_ERROR     = -10;
    public static final int STATUS_DESTROYED = -9;

    static {
        System.loadLibrary("camera");
    }

    public enum FRAME_FORMAT {
        //Frame Format: These match the enums in cpp/libcamera/Camera.h
        FRAME_FORMAT_YUY2(0x04),
        FRAME_FORMAT_MJPG(0x06),
        FRAME_FORMAT_H264(0x08);

        private int format;

        FRAME_FORMAT(int format){
            this.format = format;
        }

        public int getValue(){
            return format;
        }
    }

    public enum PIXEL_FORMAT {
        //Pixel Format: These match the enums in cpp/libcamera/Camera.h
        PIXEL_FORMAT_DEPTH(0x05),
        PIXEL_FORMAT_I420(0x0A);

        private int format;

        PIXEL_FORMAT(int format){
            this.format = format;
        }

        public int getValue(){
            return format;
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
        private int format;
        private int width;
        private int height;
        private int fps;
        private int pixel;
        private int rotate;
        private int mirror;

        public SupportInfo(int format, int width, int height, int fps) {
            this.format = format;
            this.width  = width;
            this.height = height;
            this.fps    = fps;
        }

        public void setFormatCallback(PIXEL_FORMAT pixelFormat){
            this.pixel = pixelFormat.getValue();
        }

        public void setRotate(ROTATE rotate){
            this.rotate = rotate.getValue();
        }

        public void setMirror(int mirror){
            this.mirror = mirror;
        }

        public String getInfo(){
            String info = "Unknown";
            if (format == FRAME_FORMAT.FRAME_FORMAT_MJPG.getValue()){
                info = "MJPEG ";
            } else if (format == FRAME_FORMAT.FRAME_FORMAT_YUY2.getValue()){
                info = "YUYV ";
            } else if (format == FRAME_FORMAT.FRAME_FORMAT_H264.getValue()){
                info = "H264 ";
            }
            info += width + " x " + height + " FPS: " + fps;
            return info;
        }
    }

//======================================Java API====================================================

    //C++ object id
    private final long nativeObj;

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

    public final synchronized int getSupportInfo(List<SupportInfo> supportInfo) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeGetSupportInfo(this.nativeObj, supportInfo);
            Logger.d(TAG, "getSupportInfo: " + status);
        } else {
            Logger.e(TAG, "getSupportInfo: already destroyed");
        }
        return status;
    }

    public final synchronized void setFrameCallback(IFrameCallback callback) {
        if (this.nativeObj != 0) {
            this.callback = callback;
        } else {
            Logger.e(TAG, "setFrameCallback: already destroyed");
        }
    }

    public final synchronized int setConfigInfo(SupportInfo configInfo) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeSetConfigInfo(this.nativeObj, configInfo);
            Logger.d(TAG, "setSupportInfo: " + status);
        } else {
            Logger.e(TAG, "setSupportInfo: already destroyed");
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

//=======================================Native Call================================================

    private ByteBuffer frame;
    private IFrameCallback callback;

    private void updateFrame() {
        if (callback != null) {
            callback.onFrame(frame);
        } else {
            Logger.w(TAG,"IFrameCallback is null");
        }
    }

//=======================================Native API=================================================

    private native long nativeInit();

    private native int nativeConnect(long nativeObj, int fd);

    private native int nativeOpen(long nativeObj, int vendorId, int productId, int bus, int dev);

    private native int nativeGetSupportInfo(long nativeObj, List<SupportInfo> supportInfo);

    private native int nativeSetConfigInfo(long nativeObj, SupportInfo configInfo);

    private native int nativePreview(long nativeObj, Surface surface);

    private native int nativeStart(long nativeObj);

    private native int nativeStop(long nativeObj);

    private native int nativeClose(long nativeObj);

    private native void nativeDestroy(long nativeObj);

}
