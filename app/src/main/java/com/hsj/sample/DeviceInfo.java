package com.hsj.sample;

import android.content.res.Configuration;
import com.hsj.camera.UsbCamera;

/**
 * @Author:Hsj
 * @Date:2022/4/20
 * @Class:DeviceInfo
 * @Desc:
 */
public final class DeviceInfo {

    //Usb Device Info
    private String deviceName;
    private int vid;
    private int pid;
    private int fd;
    private int format;
    private UsbCamera.SupportInfo info;
    private int decoder;
    private int render;
    private int rotate;
    private int mirror;
    //Android Device Screen
    private int maxContentWidth;
    private int maxContentHeight;
    private int screenOrientation = Configuration.ORIENTATION_UNDEFINED;

    public DeviceInfo() {
    }

//==================================================================================================

    public void setFd(int fd) {
        this.fd = fd;
    }

    public void setDevice(int vid, int pid, String deviceName) {
        this.vid = vid;
        this.pid = pid;
        this.deviceName = deviceName;
    }

    public void setFormat(int format) {
        this.format = format;
    }

    public void setSupportInfo(UsbCamera.SupportInfo info) {
        this.info = info;
    }

    public void setDecoder(int decoder) {
        this.decoder = decoder;
    }

    public void setRender(int render) {
        this.render = render;
    }

    public void setRotate(int index) {
        if (index == 1) {
            this.rotate = 90;
        } else if (index == 2) {
            this.rotate = 180;
        } else if (index == 3) {
            this.rotate = 270;
        } else {
            this.rotate = 0;
        }
    }

    public void setMirror(int index) {
        if (index == 1) {
            this.mirror = 1;
        } else if (index == 2) {
            this.mirror = -1;
        }  else {
            this.mirror = 0;
        }
    }

    public void setScreenOrientation(int screenOrientation) {
        this.screenOrientation = screenOrientation;
    }

    public void setMaxContentWidthHeight(int maxContentWidth,int maxContentHeight) {
        this.maxContentWidth = maxContentWidth;
        this.maxContentHeight = maxContentHeight;
    }

//==================================================================================================

    public int getFd() {
        return fd;
    }

    public int getVid() {
        return vid;
    }

    public int getPid() {
        return pid;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public int getFormat() {
        return format;
    }

    public UsbCamera.SupportInfo getSupportInfo(){
        return this.info;
    }

    public int getDecoder() {
        return decoder;
    }

    public int getRender() {
        return render;
    }

    public int getRotate() {
        return rotate;
    }

    public int getMirror() {
        return mirror;
    }

    public int getScreenOrientation() {
        return screenOrientation;
    }

    public int getMaxContentWidth() {
        return maxContentWidth;
    }

    public int getMaxContentHeight() {
        return maxContentHeight;
    }
}
