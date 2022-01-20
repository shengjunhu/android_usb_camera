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

    //C++ object id
    private final long nativeObj;

    public UsbMessenger() {
        this.nativeObj = nativeInit();
        Logger.d(TAG, "UsbMessenger: " + this.nativeObj);
    }

    public final synchronized boolean open(int productId, int vendorId) {
        boolean ret = false;
        if (this.nativeObj == 0) {
            Logger.e(TAG, "open: UsbMessenger native already destroyed");
        } else {
            int status = nativeOpen(this.nativeObj, productId, vendorId);
            Logger.d(TAG, "open: " + status);
            if (STATUS_SUCCESS < status) Logger.w(TAG, "open: had device opened");
            ret = (status == STATUS_SUCCESS);
        }
        return ret;
    }

    public final synchronized boolean syncRequest(byte[] request, byte[] response) {
        boolean ret = false;
        if (this.nativeObj == 0) {
            Logger.e(TAG, "syncRequest: already destroyed");
        } else if (request == null || request.length == 0) {
            Logger.w(TAG, "syncRequest: request null");
        } else if (response == null || response.length == 0) {
            Logger.w(TAG, "syncRequest: response null");
        } else {
            int status = nativeSyncRequest(this.nativeObj, request, response);
            Logger.d(TAG, "syncRequest: " + status);
            ret = (STATUS_SUCCESS == status);
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
            nativeDestroy(this.nativeObj);
            Logger.d(TAG, "destroy");
        } else {
            Logger.w(TAG, "destroy: already destroy");
        }
    }

//=======================================Native API=================================================

    private native long nativeInit();

    private native int nativeOpen(long nativeObj, int productId, int vendorId);

    private native int nativeSyncRequest(long nativeObj, byte[] request, byte[] response);

    private native int nativeClose(long nativeObj);

    private native void nativeDestroy(long nativeObj);

}
