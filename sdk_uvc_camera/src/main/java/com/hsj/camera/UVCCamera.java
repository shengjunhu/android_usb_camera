package com.hsj.camera;

import java.nio.ByteBuffer;
import java.util.List;

/**
 * @Author:Hsj
 * @Date:2022/9/1
 * @Class:UVCCamera
 * @Desc:UVCCamera
 */
public final class UVCCamera {

    private static final String TAG = "UVCCamera";
    // Camera Status
    public static final int STATUS_SUCCEED     =  0;
    public static final int STATUS_FAILED      = -1;
    public static final int STATUS_ERROR_STEP  = -2;
    public static final int STATUS_EMPTY_OBJ   = -3;
    public static final int STATUS_EMPTY_DATA  = -4;
    public static final int STATUS_EMPTY_PARAM = -5;
    public static final int STATUS_ERROR_PARAM = -6;
    public static final int STATUS_NONE_INIT   = -7;
    public static final int STATUS_DESTROYED   = -8;

    static {
        System.loadLibrary("camera");
    }

    // FrameFormat: These match the enums in cpp/libcamera/FrameDecoder.h
    public enum FrameFormat {
        FRAME_FORMAT_YUY2(0x04),
        FRAME_FORMAT_MJPG(0x06),
        FRAME_FORMAT_H264(0x10),
        FRAME_FORMAT_UNKNOWN(0x00);

        private int format;

        FrameFormat(int format){
            this.format = format;
        }

        public int getValue(){
            return format;
        }

        public String getName(){
            String name;
            switch (format){
                case 0x04: name = "FRAME_FORMAT_YUY2"; break;
                case 0x06: name = "FRAME_FORMAT_MJPG"; break;
                case 0x10: name = "FRAME_FORMAT_H264"; break;
                default: name ="FRAME_FORMAT_UNKNOWN"; break;
            }
            return name;
        }
    }

    // PixelFormat: These match the enums in cpp/libcamera/FrameDecoder.h
    public enum PixelFormat {
        /**{@see android.graphics.ImageFormat#FLEX_RGB_888}.*/
        PIXEL_FORMAT_RGB(0x25),     //RGB: R8B8G8
        /**{@see android.graphics.ImageFormat#NV21}.*/
        PIXEL_FORMAT_NV21(0x24),    //NV21: YYYYYYYY VUVU
        /**{@see android.graphics.ImageFormat#YUV_420_888}.*/
        PIXEL_FORMAT_YUV420P(0x23), //YUV420P: YYYY U  V
        /**{@see android.graphics.ImageFormat#YUV_422_888}.*/
        PIXEL_FORMAT_YUV422P(0x22), //YUV422P: YYYY UU VV
        /**{@see android.graphics.ImageFormat#DEPTH16}.*/
        PIXEL_FORMAT_DEPTH16(0x21), //DEPTH16: unsigned short
        /**{@see android.graphics.ImageFormat#YUY2}.*/
        PIXEL_FORMAT_YUY2(0x20),    //YUYV: YUYV YUYV
        // No image to be callback
        PIXEL_FORMAT_NONE(0x00);    //NONE: No Pixel

        private int pixel;

        PixelFormat(int pixel){
            this.pixel = pixel;
        }

        public int getValue(){
            return pixel;
        }

        public String getName(){
            String name;
            switch (pixel){
                case 0x25: name = "PIXEL_FORMAT_RGB";     break;
                case 0x24: name = "PIXEL_FORMAT_NV21";    break;
                case 0x23: name = "PIXEL_FORMAT_YUV420P"; break;
                case 0x22: name = "PIXEL_FORMAT_YUV422P"; break;
                case 0x21: name = "PIXEL_FORMAT_DEPTH16"; break;
                case 0x20: name = "PIXEL_FORMAT_YUY2";    break;
                default: name ="PIXEL_FORMAT_UNKNOWN";    break;
            }
            return name;
        }
    }

    // VideoFormat: These match the enums in cpp/libcamera/FrameDecoder.h
    public enum VideoFormat {
        VIDEO_FORMAT_MP4(0x11),
        VIDEO_FORMAT_H264(0x10),
        VIDEO_FORMAT_NONE(0x00);

        private int format;

        VideoFormat(int format){
            this.format = format;
        }

        public int getValue(){
            return format;
        }

        public String getName(){
            String name;
            switch (format){
                case 0x11: name = "VIDEO_FORMAT_MP4";  break;
                case 0x10: name = "VIDEO_FORMAT_H264"; break;
                default: name ="VIDEO_FORMAT_UNKNOWN"; break;
            }
            return name;
        }
    }

    // PixelRotate: These match the enums in cpp/libcamera/FrameDecoder.h
    public enum PixelRotate {
        // Rotate 90 clockwise of every image
        PIXEL_ROTATE_90(0x5A),
        // Rotate 18 clockwise of every image
        PIXEL_ROTATE_180(0xB4),
        // Rotate 270 clockwise of every image
        PIXEL_ROTATE_270(0x10E),
        // No pixel to be rotate of every image
        PIXEL_ROTATE_NONE(0x00);

        private int rotate;

        PixelRotate(int rotate){
            this.rotate = rotate;
        }

        public int getValue(){
            return rotate;
        }

        public String getName(){
            String name;
            switch (rotate){
                case 0x5A : name = "PIXEL_ROTATE_90"; break;
                case 0xB4 : name = "PIXEL_ROTATE_180"; break;
                case 0x10E: name = "PIXEL_ROTATE_270"; break;
                default: name = "PIXEL_ROTATE_NONE"; break;
            }
            return name;
        }
    }

    // PixelFlip: These match the enums in cpp/libcamera/FrameDecoder.h
    public enum PixelFlip {
        // Left and right mirror of every image
        PIXEL_FLIP_H(0x01),
        // Top and bottom mirror of every image
        PIXEL_FLIP_V(0x02),
        // No pixel to be flip
        PIXEL_FLIP_NONE(0x00);

        private int flip;

        PixelFlip(int flip){
            this.flip = flip;
        }

        public int getValue(){
            return flip;
        }

        public String getName(){
            String name;
            switch (flip){
                case 0x01: name = "PIXEL_FLIP_H"; break;
                case 0x02: name = "PIXEL_FLIP_V"; break;
                default: name = "PIXEL_FLIP_NONE"; break;
            }
            return name;
        }
    }

    //Support Info
    public static class SupportInfo {
        /** Frame type(int), {@link UVCCamera.FrameFormat}. */
        public final int type;
        /** Frame format(String), {@link UVCCamera.FrameFormat}. */
        public final String format;
        // Frame width
        public final int width;
        // Frame height
        public final int height;
        // Frame fps
        public final int fps;
        // Bandwidth∈[0.10f, 1.00f]
        public float bandwidth;

        /** {@link UVCCamera.PixelFormat}, for image callback. */
        public int pixel;
        /** {@link UVCCamera.VideoFormat}, for video format. */
        public int video;
        /** {@link UVCCamera.PixelRotate}, for pixel or video. */
        public int rotate;
        /** {@link UVCCamera.PixelFlip}, for pixel or video. */
        public int flip;
        /** If {@link UVCCamera#takePhoto()}, save photo in directory. */
        public String photoDir;
        /** If {@link UVCCamera#startVideo()}, save video in directory. */
        public String videoDir;

        /**
         * UVC Camera support configure
         * @param width  frame width
         * @param height frame height
         * @param format frame format
         * @param fps    frame fps
         */
        public SupportInfo(int type, String format, int width, int height, int fps) {
            this.type      = type;
            this.format    = format;
            this.width     = width;
            this.height    = height;
            this.fps       = fps;
            this.bandwidth = 1.0f;

            this.pixel     = PixelFormat.PIXEL_FORMAT_NONE.getValue();
            this.video     = VideoFormat.VIDEO_FORMAT_NONE.getValue();
            this.rotate    = PixelRotate.PIXEL_ROTATE_NONE.getValue();
            this.flip      = PixelFlip.PIXEL_FLIP_NONE.getValue();
            this.photoDir  = null;
            this.videoDir  = null;
        }

        @Override
        public String toString() {
            return "SupportInfo{" +
                    "type=" + type +
                    ", format='" + format + '\'' +
                    ", width=" + width +
                    ", height=" + height +
                    ", fps=" + fps +
                    ", bandwidth=" + bandwidth +
                    ", pixel=" + pixel +
                    ", video=" + video +
                    ", rotate=" + rotate +
                    ", flip=" + flip +
                    ", photoDir='" + photoDir + '\'' +
                    ", videoDir='" + videoDir + '\'' +
                    '}';
        }
    }

//======================================Java API====================================================

    //C++ object id
    private final long nativeObj;

    public UVCCamera() {
        this(false);
    }

    public UVCCamera(boolean debug) {
        this.nativeObj = nativeInit();
        Logger.OPEN = debug;
    }

    /**
     * Set fd to cpp
     *
     * @param fd {@link android.hardware.usb.UsbDeviceConnection#getFileDescriptor()}
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int open(int fd) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeConnect(this.nativeObj, fd);
            Logger.d(TAG, "open: " + status);
        } else {
            Logger.e(TAG, "open: already destroyed.");
        }
        return status;
    }

    /**
     * Select target usb device by param and open it.
     * There is need access /dev/bus/usb permission
     *
     * @param vid {@link android.hardware.usb.UsbDevice#getVendorId()}
     * @param pid {@link android.hardware.usb.UsbDevice#getProductId()}
     * @param usbfs {@link android.hardware.usb.UsbDevice#getDeviceName()}
     * example: /dev/bus/usb/001/004
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int open(int vid, int pid, String usbfs) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeOpen(this.nativeObj, vid, pid, usbfs);
            Logger.d(TAG, "open: " + status);
        } else {
            Logger.e(TAG, "open: already destroyed.");
        }
        return status;
    }

    /**
     * Get uvc camera support information.
     *
     * @param supportInfo is empty and not null.
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int getSupportInfo(List<SupportInfo> supportInfo) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeGetSupportInfo(this.nativeObj, supportInfo);
            Logger.d(TAG, "getSupportInfo: " + status);
        } else {
            Logger.e(TAG, "getSupportInfo: already destroyed.");
        }
        return status;
    }

    /**
     * Set uvc camera support information and configure image convert param
     *
     * @param supportInfo {@link UVCCamera.SupportInfo}
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int setSupportInfo(SupportInfo supportInfo) {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            Logger.d(TAG, "setSupportInfo: " + supportInfo.toString());
            status = nativeSetSupportInfo(this.nativeObj, supportInfo);
            Logger.d(TAG, "setSupportInfo: " + status);
        } else {
            Logger.e(TAG, "setSupportInfo: already destroyed.");
        }
        return status;
    }

    /**
     * Set frame callback
     *
     * @param callback {@link UVCCamera.IFrameCallback}
     */
    public final synchronized void setFrameCallback(IFrameCallback callback) {
        this.callback = callback;
    }

    /**
     * Set frame preview
     *
     * @param view {@link CameraView}
     */
    public final synchronized int setFramePreview(CameraView view){
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeSetFramePreview(this.nativeObj, view);
            Logger.d(TAG, "setFramePreview: " + status);
        } else {
            Logger.e(TAG, "setPreview: already destroyed.");
        }
        return status;
    }

    /**
     * Start stream
     *
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int start() {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeStart(this.nativeObj);
            Logger.d(TAG, "start: " + status);
        } else {
            Logger.e(TAG, "start: already destroyed.");
        }
        return status;
    }

    /**
     * Take photo
     * {@link UVCCamera.IFrameCallback#onPicture(byte[])}
     *
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int takePhoto(){
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeTakePhoto(this.nativeObj);
            Logger.d(TAG, "takePhoto: " + status);
        } else {
            Logger.e(TAG, "takePhoto: already destroyed.");
        }
        return status;
    }

    /**
     * Start record video
     *
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int startVideo(){
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeStartVideo(this.nativeObj);
            Logger.d(TAG, "startVideo: " + status);
        } else {
            Logger.e(TAG, "startVideo: already destroyed.");
        }
        return status;
    }

    /**
     * Stop record video
     *
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int stopVideo(){
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeStopVideo(this.nativeObj);
            Logger.d(TAG, "stopVideo: " + status);
        } else {
            Logger.e(TAG, "stopVideo: already destroyed.");
        }
        return status;
    }

    /**
     * Stop stream
     *
     * @return STATUS_SUCCESS is succeed, otherwise are failed.
     */
    public final synchronized int stop() {
        int status = STATUS_DESTROYED;
        if (this.nativeObj != 0) {
            status = nativeStop(this.nativeObj);
            Logger.d(TAG, "stop: " + status);
        } else {
            Logger.e(TAG, "stop: already destroyed.");
        }
        return status;
    }

    /**
     * Close uvc device
     */
    public final synchronized void close() {
        if (this.nativeObj != 0) {
            int status = nativeClose(this.nativeObj);
            Logger.d(TAG, "close: " + status);
        } else {
            Logger.e(TAG, "close: already destroyed.");
        }
    }

    /**
     * Release source
     */
    public final synchronized void destroy() {
        if (this.nativeObj != 0) {
            nativeDestroy(this.nativeObj);
            Logger.d(TAG, "destroy: 0");
        } else {
            Logger.w(TAG, "destroy: already destroyed.");
        }
    }

//=======================================Native Call================================================

    //To be call by C++
    private byte[] picture;
    private ByteBuffer frame;
    private IFrameCallback callback;

    public interface IFrameCallback {
        /**
         * Frame Callback, frame format is {@link UVCCamera.SupportInfo#pixel}
         *
         * get data:
         * frame.clear()
         * byte[] data = new byte[frame.capacity()];
         * frame.get(data);
         *
         * @param frame {@link java.nio.ByteBuffer}
         */
        void onFrame(ByteBuffer frame);

        /**
         * Take Photo
         * {@link UVCCamera#takePhoto()}
         *
         * @param picture jpeg {@link android.graphics.ImageFormat#JPEG}
         */
        void onPicture(byte[] picture);
    }

    private void updateFrame() {
        if (callback != null) callback.onFrame(frame);
    }

    private void updatePhoto() {
        if (callback != null) callback.onPicture(picture);
    }

//=======================================Native API=================================================

    private native long nativeInit();

    private native int nativeConnect(long nativeObj, int fd);

    private native int nativeOpen(long nativeObj, int vid, int pid, String usbfs);

    private native int nativeGetSupportInfo(long nativeObj, List<SupportInfo> supportInfo);

    private native int nativeSetSupportInfo(long nativeObj, SupportInfo supportInfo);

    private native int nativeSetFramePreview(long nativeObj, CameraView view);

    private native int nativeStart(long nativeObj);

    private native int nativeTakePhoto(long nativeObj);

    private native int nativeStartVideo(long nativeObj);

    private native int nativeStopVideo(long nativeObj);

    private native int nativeStop(long nativeObj);

    private native int nativeClose(long nativeObj);

    private native void nativeDestroy(long nativeObj);

//==================================================================================================

}