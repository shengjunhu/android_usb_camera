package com.hsj.camera.sample;

import android.app.Activity;
import android.content.Context;
import android.hardware.usb.UsbDevice;
import android.hardware.usb.UsbDeviceConnection;
import android.hardware.usb.UsbManager;
import android.os.Bundle;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;
import androidx.fragment.app.Fragment;

import com.hsj.camera.IFrameCallback;
import com.hsj.camera.UsbCamera;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;

/**
 * @Author:Hsj
 * @Date:2022/4/19
 * @Class:PreviewFragment
 * @Desc:
 */
public final class PreviewFragment extends Fragment {

    private static final String TAG = "PreviewFragment";

    @Override
    public void onAttach(@NonNull Context context) {
        super.onAttach(context);
        Log.d(TAG,"onAttach");
    }

    @Override
    public void onCreate(@Nullable Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        Log.d(TAG,"onCreate");
    }

    @Nullable
    @Override
    public View onCreateView(@NonNull LayoutInflater inflater, @Nullable ViewGroup container, @Nullable Bundle savedInstanceState) {
        //return super.onCreateView(inflater, container, savedInstanceState);
        Log.d(TAG,"onCreateView");
        if (rootView == null){
            Activity activity = getActivity();
            if (activity != null){
                rootView = new FrameLayout(activity);
                this.camera = new UsbCamera();
            }
        }
        return rootView;
    }

    @Override
    public void onActivityCreated(@Nullable Bundle savedInstanceState) {
        super.onActivityCreated(savedInstanceState);
        Log.d(TAG,"onActivityCreated");
    }

    @Override
    public void onStart() {
        super.onStart();
        Log.d(TAG,"onStart");
    }

    @Override
    public void onResume() {
        super.onResume();
        Log.d(TAG,"onResume");
    }

    @Override
    public void onPause() {
        super.onPause();
        Log.d(TAG,"onPause");
    }

    @Override
    public void onStop() {
        super.onStop();
        Log.d(TAG,"onStop");
        stop();
    }

    @Override
    public void onDestroyView() {
        super.onDestroyView();
        Log.d(TAG,"onDestroyView");
        close();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG,"onDestroy");
        destroy();
    }

    @Override
    public void onDetach() {
        super.onDetach();
        Log.d(TAG,"onDetach");
    }

//==================================================================================================

    private List<UsbCamera.SupportInfo> supportInfos = new ArrayList<>();
    private FrameLayout rootView;
    private UsbCamera camera;

    private void showToast(String msg) {
        Toast.makeText(getContext(), msg, Toast.LENGTH_SHORT).show();
    }

    public void open(DeviceInfo info){
        int status;
        if (info.getFd() != 0){
            status = camera.open(info.getFd());
        } else {
            status = camera.open(info.getVid(),info.getPid(),info.getDeviceName());
        }
        if (UsbCamera.STATUS_OK == status){
            supportInfos.clear();
            status = camera.getSupportInfo(supportInfos);
            if (UsbCamera.STATUS_OK != status){
                showToast("Get device support info failed: " + status);
            }
        } else {
            showToast("Device open failed: " + status);
        }
    }

    public void start(DeviceInfo info){
        UsbCamera.SupportInfo config = info.getSupportInfo();
        if (config != null){
            //config.setFormatCallback(UsbCamera.PIXEL_FORMAT.PIXEL_FORMAT_I420);
            int status = camera.setSupportInfo(config);
            if (UsbCamera.STATUS_OK == status) {
                //camera.setFrameCallback(frameCallback);
                status = camera.start();
                if (UsbCamera.STATUS_OK != status) {
                    showToast("start failed: " + status);
                }
            }
        }
    }

    public List<UsbCamera.SupportInfo> getSupport(){
        return this.supportInfos;
    }

    public void stop(){
        if (this.camera != null) {
            this.camera.stop();
        }
    }

    public void close(){
        if (this.camera != null) {
            this.supportInfos.clear();
            this.camera.close();
        }
    }

    private void destroy() {
        if (this.camera != null) {
            this.camera.destroy();
            this.camera = null;
        }
    }

    private final IFrameCallback frameCallback = new IFrameCallback() {
        @Override
        public void onFrame(ByteBuffer frame) {
            Log.i(TAG, "onFrame");
        }
    };

//==================================================================================================

}
