package com.hsj.camera;

/**
 * @Author:Hsj
 * @Date:2022/1/13
 * @Class:UsbMessenger
 * @Desc:
 */
public final class UsbMessenger {

    private static final String TAG = "UsbMessenger";
    private static final int STATUS_SUCCESS = 0;

    static {
        System.loadLibrary("camera");
    }

//======================================Java API====================================================

    private final long nativeObj;

    public UsbMessenger() {
        this.nativeObj = nativeInit();
        Logger.d(TAG, "UsbMessenger: " + this.nativeObj);
    }

    public final synchronized boolean open(int productId, int vendorId) {
        if (this.nativeObj == 0) {
            Logger.e(TAG, "open: can't be call after close");
            return false;
        } else {
            int status = nativeOpen(this.nativeObj, productId, vendorId);
            Logger.d(TAG, "open: " + status);
            return STATUS_SUCCESS == status;
        }
    }

    public final synchronized void syncRequest(byte[] request, byte[] response) {
        if (this.nativeObj == 0) {
            Logger.e(TAG, "syncRequest: can't be call after close");
        } else if (request == null || request.length == 0){
            Logger.w(TAG, "syncRequest: request null");
        } else if (response == null || response.length == 0){
            Logger.w(TAG, "syncRequest: response null");
        } else {
            nativeSyncRequest(this.nativeObj, request, response);
            Logger.d(TAG, "syncRequest: " + response[0]);
        }
    }

    public final synchronized void asyncRequest(byte[] request, byte[] response) {
        if (this.nativeObj == 0) {
            Logger.e(TAG, "asyncRequest: can't be call after close");
        } else if (request == null || request.length == 0) {
            Logger.w(TAG, "asyncRequest: request null");
        } else if (response == null || response.length == 0) {
            Logger.w(TAG, "asyncRequest: response null");
        } else {
            nativeAsyncRequest(this.nativeObj, request, response);
            Logger.d(TAG, "asyncRequest: " + response[0]);
        }
    }

    public final synchronized void close() {
        if (this.nativeObj == 0) {
            Logger.e(TAG, "close: already close");
        }else {
            nativeClose(this.nativeObj);
            Logger.d(TAG, "close");
        }
    }

//=======================================Native API=================================================

    private native long nativeInit();

    private native int nativeOpen(long nativeObj, int productId, int vendorId);

    private native void nativeSyncRequest(long nativeObj, byte[] request, byte[] response);

    private native void nativeAsyncRequest(long nativeObj, byte[] request, byte[] response);

    private native void nativeClose(long nativeObj);

}
