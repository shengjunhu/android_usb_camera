package com.hsj.camera;

import java.nio.ByteBuffer;

/**
 * @Author:Hsj
 * @Date:2022/1/13
 * @Class:UsbCallback
 * @Desc:
 */
public interface FrameCallback {
    void onFrame(ByteBuffer frame);
}
