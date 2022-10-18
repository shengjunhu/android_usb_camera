package com.hsj.camera;

import android.content.Context;
import android.content.res.TypedArray;
import android.opengl.GLSurfaceView;
import android.util.AttributeSet;
import android.util.Log;
import android.view.Gravity;
import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.FrameLayout;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

/**
 * @Author:Hsj
 * @Date:2022/4/26
 * @Class:CameraView
 * @Desc:
 */
public final class CameraView extends FrameLayout {

    private static final String TAG = "CameraView";
    private static final int UNKNOWN = -1;
    public static final int SURFACE_VIEW = 0;
    public static final int GL_SURFACE_VIEW = 1;

    public CameraView(Context context) {
        this(context, null);
    }

    public CameraView(Context context, AttributeSet attrs) {
        this(context, attrs, 0);
    }

    public CameraView(Context context, AttributeSet attrs, int defStyleAttr) {
        super(context, attrs, defStyleAttr);
        TypedArray arr = context.obtainStyledAttributes(attrs, R.styleable.CameraView);
        int type = arr.getInt(R.styleable.CameraView_content_view, UNKNOWN);
        arr.recycle();

        this.nativeObj = nativeInit();
        setViewType(type, new FrameLayout.LayoutParams(
                FrameLayout.LayoutParams.MATCH_PARENT,
                FrameLayout.LayoutParams.MATCH_PARENT,
                Gravity.CENTER));
    }

    public boolean setViewType(int type, LayoutParams params) {
        boolean ret = true;
        if (type == SURFACE_VIEW) {
            if (this.surfaceView != null){
                removeViewAt(0);
                this.surfaceView = null;
            }
            if (this.glSurfaceView != null){
                removeViewAt(0);
                this.glSurfaceView = null;
            }
            createSurfaceView(params);
        } else if (type == GL_SURFACE_VIEW) {
            if (this.surfaceView != null){
                removeViewAt(0);
                this.surfaceView = null;
            }
            if (this.glSurfaceView != null){
                removeViewAt(0);
                this.glSurfaceView = null;
            }
            createGLSurfaceView(params);
        } else {
            Log.w(TAG, "setCameraView(): unknown type: " + type);
            ret = false;
        }
        return ret;
    }

    public void destroy(){
        if (this.surfaceView != null){
            removeViewAt(0);
            this.surfaceView = null;
        }
        if (this.glSurfaceView != null){
            removeViewAt(0);
            this.glSurfaceView = null;
        }
        nativeRelease(nativeObj);
    }

//======================================OnStatusListener============================================

    private OnStatusListener listener;

    public interface OnStatusListener{
        void onCreated();
        void onChanged();
        void onDestroyed();
    }

    public void setOnStatusListener(OnStatusListener listener){
        this.listener = listener;
    }

//========================================SurfaceView===============================================

    private SurfaceView surfaceView;

    private void createSurfaceView(LayoutParams params) {
        Log.e(TAG, "createSurfaceView");
        this.surfaceView = new SurfaceView(getContext());
        addView(this.surfaceView, 0);
        this.surfaceView.setLayoutParams(params);
        this.surfaceView.setZOrderOnTop(true);
        this.surfaceView.setZOrderMediaOverlay(true);
        this.surfaceView.getHolder().addCallback(holderCallback);
    }

    private final SurfaceHolder.Callback holderCallback = new SurfaceHolder.Callback() {
        @Override
        public void surfaceCreated(SurfaceHolder holder) {
            Log.e(TAG, "surfaceCreated");
            int ret = nativeWindow(nativeObj, holder.getSurface());
            Log.e(TAG, "surfaceCreated: " + ret);
            if (listener != null) listener.onCreated();
        }

        @Override
        public void surfaceChanged(SurfaceHolder holder, int format, int width, int height) {
            if (listener != null) listener.onChanged();
        }

        @Override
        public void surfaceDestroyed(SurfaceHolder holder) {
            if (listener != null) listener.onDestroyed();
        }
    };

//=======================================GLSurfaceView==============================================

    private GLSurfaceView glSurfaceView;

    private void createGLSurfaceView(LayoutParams params) {
        this.glSurfaceView = new GLSurfaceView(getContext());
        addView(this.glSurfaceView, 0);
        this.glSurfaceView.setLayoutParams(params);
        this.glSurfaceView.setZOrderOnTop(true);
        this.glSurfaceView.setZOrderMediaOverlay(true);
        this.glSurfaceView.setEGLConfigChooser(8, 8, 8, 8, 16, 0);
        this.glSurfaceView.setEGLContextClientVersion(3);
        this.glSurfaceView.setRenderer(glRender);
    }

    private final GLSurfaceView.Renderer glRender = new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
            nativeCreate(nativeObj);
            if (listener != null) listener.onCreated();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int width, int height) {
            nativeResize(nativeObj, width, height);
            if (listener != null) listener.onChanged();
        }

        @Override
        public void onDrawFrame(GL10 gl) {
            nativeDraw(nativeObj);
        }
    };

    public void onResume() {
        if (this.glSurfaceView != null) this.glSurfaceView.onResume();
    }

    public void onPause() {
        if (this.glSurfaceView != null) this.glSurfaceView.onPause();
        if (this.listener != null) this.listener.onDestroyed();
        nativePause(nativeObj);
    }

//=========================================Native===================================================

    private final long nativeObj;

    private native long nativeInit();

    private native int nativeWindow(long nativeObj, Surface surface);

    private native int nativeCreate(long nativeObj);

    private native int nativeResize(long nativeObj, int width, int height);

    private native int nativeDraw(long nativeObj);

    private native int nativePause(long nativeObj);

    private native void nativeRelease(long nativeObj);

}
