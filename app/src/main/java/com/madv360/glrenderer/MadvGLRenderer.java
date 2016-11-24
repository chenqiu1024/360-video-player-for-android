package com.madv360.glrenderer;

import android.opengl.GLUtils;
import android.util.Log;

import javax.microedition.khronos.egl.EGL10;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.egl.EGLContext;
import javax.microedition.khronos.egl.EGLDisplay;
import javax.microedition.khronos.egl.EGLSurface;

/**
 * Created by qiudong on 16/4/20.
 */
public class MadvGLRenderer {
    public static final int PanoramaDisplayModePlain = 0;
    public static final int PanoramaDisplayModeSphere = 0x01;
    public static final int PanoramaDisplayModeLittlePlanet = 0x02;
    public static final int PanoramaDisplayModeStereoGraphic = 0x03;

    public static final int PanoramaDisplayModeLUT = 0x10;
    public static final int PanoramaDisplayModePlainStitch = 0x20;

    public static final int PanoramaDisplayModeExclusiveMask = 0x0f;

    public static final int CLIP_Z_NEAR = 3;
    public static final int CLIP_Z_FAR = 65536;

    public MadvGLRenderer(String lutPath, Vec2f leftSrcTextureSize, Vec2f rightSrcTextureSize) {
        Log.d("QD:MadvGLRenderer", "lutPath = " + lutPath + ", srcSize = (" + leftSrcTextureSize.x + ", " + leftSrcTextureSize.y + ")");
        nativeGLRendererPointer = createNativeGLRenderer(lutPath, leftSrcTextureSize, rightSrcTextureSize);
    }

//    public void initGL(int width, int height) {
//        egl = (EGL10) EGLContext.getEGL();
//        eglDisplay = egl.eglGetDisplay(EGL10.EGL_DEFAULT_DISPLAY);
//
//        int[] version = new int[2];
//        egl.eglInitialize(eglDisplay, version);
//
//        EGLConfig eglConfig = chooseEglConfig();
//        eglContext = createContext(egl, eglDisplay, eglConfig);
//
//        int[] attrList = new int[] {
//                EGL10.EGL_WIDTH, width,
//                EGL10.EGL_HEIGHT, height,
//                EGL10.EGL_NONE
//        };
//        eglSurface = egl.eglCreatePbufferSurface(eglDisplay, eglConfig, attrList);
////        eglSurface = egl.eglCreateWindowSurface(eglDisplay, eglConfig, texture, null);
//
//        if (eglSurface == null || eglSurface == EGL10.EGL_NO_SURFACE) {
//            throw new RuntimeException("GL Error: " + GLUtils.getEGLErrorString(egl.eglGetError()));
//        }
//
//        if (!egl.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
//            throw new RuntimeException("GL Make current error: " + GLUtils.getEGLErrorString(egl.eglGetError()));
//        }
//    }
//
//    public void releaseGL() {
//        egl.eglMakeCurrent(eglDisplay, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_SURFACE, EGL10.EGL_NO_CONTEXT);
//        egl.eglDestroySurface(eglDisplay, eglSurface);
//        egl.eglDestroyContext(eglDisplay, eglContext);
//        egl.eglTerminate(eglDisplay);
//    }
//
//    private EGLContext createContext(EGL10 egl, EGLDisplay eglDisplay, EGLConfig eglConfig) {
//        int[] attribList = {EGL_CONTEXT_CLIENT_VERSION, 2, EGL10.EGL_NONE};
//        return egl.eglCreateContext(eglDisplay, eglConfig, EGL10.EGL_NO_CONTEXT, attribList);
//    }
//
//    private EGLConfig chooseEglConfig() {
//        int[] configsCount = new int[1];
//        EGLConfig[] configs = new EGLConfig[1];
//        int[] configSpec = getConfig();
//
//        if (!egl.eglChooseConfig(eglDisplay, configSpec, configs, 1, configsCount)) {
//            throw new IllegalArgumentException("Failed to choose config: " + GLUtils.getEGLErrorString(egl.eglGetError()));
//        } else if (configsCount[0] > 0) {
//            return configs[0];
//        }
//
//        return null;
//    }
//
//    private int[] getConfig() {
//        return new int[]{
//                EGL10.EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
//                EGL10.EGL_RED_SIZE, 8,
//                EGL10.EGL_GREEN_SIZE, 8,
//                EGL10.EGL_BLUE_SIZE, 8,
//                EGL10.EGL_ALPHA_SIZE, 8,
//                EGL10.EGL_DEPTH_SIZE, 0,
//                EGL10.EGL_STENCIL_SIZE, 0,
//                EGL10.EGL_NONE
//        };
//    }

//    private static final int EGL_OPENGL_ES2_BIT = 4;
//    private static final int EGL_OPENGL_ES3_BIT = 0x0040;
//    private static final int EGL_CONTEXT_CLIENT_VERSION = 0x3098;
//
//    EGL10 egl;
//    EGLDisplay eglDisplay;
//    EGLContext eglContext;
//    EGLSurface eglSurface;

    static {
        System.loadLibrary("madvframework");
    }

    protected void finalize() {
        releaseNativeGLRenderer();
    }

    public native long createNativeGLRenderer(String lutPath, Vec2f srcTextureSizeL, Vec2f srcTextureSizeR);

    public native void releaseNativeGLRenderer();

    public native void setSourceTextures(boolean separateSourceTexture, int srcTextureL, int srcTextureR, Vec2f srcTextureSizeL, Vec2f srcTextureSizeR, int srcTextureTarget, boolean isYUVColorSpace);

    public native void setSourceYUVTextures(boolean separateSourceTexture, int[] srcTextureL, int[] srcTextureR, Vec2f srcTextureSizeL, Vec2f srcTextureSizeR, int srcTextureTarget, boolean isYUVColorSpace);

    public native void setCapsTexture(int texture, int textureTarget);

    public native void setNeedDrawCaps(boolean drawCaps);

    public native void draw(int x, int y, int width, int height);

    public native void draw(int currentDisplayMode, int x, int y, int width, int height, boolean separateSourceTextures, Vec2f leftSrcSize, Vec2f rightSrcSize, int srcTextureType, int leftSrcTexture, int rightSrcTexture);

//    public native void setEulerAngles(float yawRadius, float pitchRadius, float bankRadius);
    public native float getDiffYawRadius();
    public native void setDiffYawRadius(float diffYawRadius);

    public native float getDiffPitchRadius();
    public native void setDiffPitchRadius(float diffPitchRadius);

    public native void setGyroQuaternion(float x, float y, float z, float w);
    public native void resetGyroQuaternion(float x, float y, float z, float w);

    public native void commitGyroQuaternion();

    public native void normalizeRotationMatrix();

    public native void setPanPoint(Vec2f point);
    public native void resetPanPoint(Vec2f point);

    public native Vec2f getPanPoint();

    public native void commitPanPoint();

    public native void commitMatrix();

    public native void resetRotation();

    public native void adustPosition();

    public native int getDisplayMode();

    public native void setDisplayMode(int displayMode);

    public native boolean getIsYUVColorSpace();

    public native void setIsYUVColorSpace(boolean isYUVColorSpace);

    public native float getFocalLength();

    public native void setFocalLength(float focalLength);

    public native void setFOVDegree(int fovDegree);

    public static native byte[] renderThumbnail(int sourceTexture, Vec2f srcSize, Vec2f destSize, String lutPath);

//    public native float getYawRadius();
//
//    public native void setYawRadius(float yawRadius);
//
//    public native float getPitchRadius();
//
//    public native void setPitchRadius(float pitchRadius);
//
//    public native float getBankRadius();
//
//    public native void setBankRadius(float bankRadius);

    public native void setGyroMatrix(float[] matrix, int rank);

    public native void rotationByProjectedPoints(Vec2f fromPoint, Vec2f toPoint);

    public native boolean getFlipX();

    public native void setFlipX(boolean flipX);

    public native boolean getFlipY();

    public native void setFlipY(boolean flipY);

    private long nativeGLRendererPointer = 0;
}
