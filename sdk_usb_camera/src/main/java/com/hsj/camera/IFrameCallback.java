package com.hsj.camera;

import java.nio.ByteBuffer;

/**
 * @Author:Hsj
 * @Date:2022/1/13
 * @Class:IFrameCallback
 * @Desc:
 */
public interface IFrameCallback {
    void onFrame(ByteBuffer frame);
}
