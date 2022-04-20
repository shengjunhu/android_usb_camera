package com.hsj.camera.sample;

import com.hsj.camera.UsbCamera;

/**
 * @Author:Hsj
 * @Date:2022/4/20
 * @Class:DeviceInfo
 * @Desc:
 */
public final class DeviceInfo {

    private int fd;
    private int vid;
    private int pid;
    private String deviceName;
    private int format;
    private UsbCamera.SupportInfo info;
    private int decoder;
    private int render;
    private int rotate;
    private int mirror;

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

    public void setRotate(int type) {
        if (type == 1) {
            this.rotate = 90;
        } else if (type == 2) {
            this.rotate = 180;
        } else if (type == 3) {
            this.rotate = 270;
        } else {
            this.rotate = 0;
        }
    }

    public void setMirror(int type) {
        if (type == 1) {
            this.mirror = 1;
        } else if (type == 2) {
            this.mirror = -1;
        }  else {
            this.mirror = 0;
        }
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
}
