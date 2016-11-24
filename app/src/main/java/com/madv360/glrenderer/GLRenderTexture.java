package com.madv360.glrenderer;

/**
 * Created by qiudong on 16/7/16.
 */
public class GLRenderTexture {

    public GLRenderTexture(int width, int height) {
        init(width, height);
    }

    @Override
    public void finalize() {
        dealloc();
    }

    private final native void init(int width, int height);

    private final native void dealloc();

    public native void releaseGLObjects();

    public final native void blit();

    public final native void unblit();

    public final native int getFramebuffer();

    public final native int getTexture();

    public final native int getWidth();

    public final native int getHeight();

    public final native int copyPixelData(byte[] data);

    public final native byte[] copyPixelDataFromPBO();

    public final native int bytesLength();

    public final native void resizeIfNecessary(int width, int height);

    private long nativeGLRenderTexturePointer = 0;

    static {
        System.loadLibrary("madvframework");
    }
}
