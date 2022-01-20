package com.hsj.camera;

import android.content.Context;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import java.nio.ByteBuffer;

import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * @Author:Hsj
 * @Date:2022/1/13
 * @Class:CameraView
 * @Desc:
 */
public final class CameraView extends SurfaceView implements SurfaceHolder.Callback {

    private static final String TAG = "CameraView";
    public static final int FRAME_RGB = 200;
    public static final int FRAME_GRAY = 202;
    public static final int FRAME_DEPTH = 100;

    //C++ object id
    private long nativeObj;
    //Frame Info
    private int frameType;
    private int frameWidth;
    private int frameHeight;
    private Surface surface;

    static {
        System.loadLibrary("camera");
    }

    public CameraView(Context context) {
        this(context, null);
    }

    public CameraView(Context context, AttributeSet attrs) {
        super(context, attrs);
        setZOrderOnTop(true);
        setZOrderMediaOverlay(true);
        getHolder().addCallback(this);
    }

//==================================================================================================

    @Override
    public void surfaceCreated(SurfaceHolder holder) {
        this.surface = holder.getSurface();
        create(frameWidth, frameHeight, frameType);
    }

    @Override
    public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
        pause();
    }

    @Override
    public void surfaceDestroyed(SurfaceHolder holder) {
        destroy();
    }

//==================================================================================================

    public synchronized void create(int frameW, int frameH, int frameType) {
        if (nativeObj == 0 && frameW != 0 && frameH != 0 && surface != null) {
            nativeCreate(surface, frameW, frameH, frameType);
        } else {
            this.frameType = frameType;
            this.frameWidth = frameW;
            this.frameHeight = frameH;
        }
    }

    public synchronized void update(byte[] data) {
        if (nativeObj != 0) {
            nativeUpdate(nativeObj, data);
        } else {
            Logger.w(TAG, "destroy: already destroyed");
        }
    }

    public synchronized void update(ByteBuffer data) {
        if (nativeObj != 0) {
            nativeRender(nativeObj, data);
        }else {
            Logger.w(TAG, "destroy: already destroyed");
        }
    }

    public synchronized void pause() {
        if (nativeObj != 0) {
            nativePause(nativeObj);
        }else {
            Logger.w(TAG, "destroy: already destroyed");
        }
    }

    private synchronized void destroy() {
        if (nativeObj != 0) {
            nativeDestroy(nativeObj);
        } else {
            Logger.w(TAG, "destroy: already destroyed");
        }
    }

//==================================================================================================

    private native long nativeCreate(Surface surface, int frameW, int frameH, int frameFormat);

    private native void nativeUpdate(long nativePtr, byte[] data);

    private native void nativeRender(long nativePtr, ByteBuffer data);

    private native void nativePause(long nativePtr);

    private native void nativeDestroy(long nativePtr);

//==================================================================================================

}
