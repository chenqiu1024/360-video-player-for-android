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
