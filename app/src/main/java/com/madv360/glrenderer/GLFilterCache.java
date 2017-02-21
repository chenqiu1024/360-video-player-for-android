package com.madv360.glrenderer;

import android.content.Context;

import com.oculus.sample.R;

import java.io.File;
import java.io.FileOutputStream;
import java.io.InputStream;

import bootstrap.appContainer.ElephantApp;

/**
 * Created by qiudong on 16/7/16.
 */
public class GLFilterCache {
    public static final int GLFilterNone = 0;
    public static final int GLFilterTestID = -1;
    public static final int GLFilterSimpleBeautyID = 1;
    public static final int GLFilterInverseColorID = 2;
    public static final int GLFilterBilateralID = 3;
    public static final int GLFilterKuwaharaID = 4;
    public static final int GLFilterSepiaToneID = 5;
    public static final int GLFilterAmatorkaID = 6;
    public static final int GLFilterMissEtikateID = 7;

    public static final int OrientationNormal = 0;
    public static final int OrientationMirror = 1;
    public static final int OrientationRotateLeft = 2;
    public static final int OrientationRotateLeftMirror = 3;
    public static final int OrientationRotateRight = 4;
    public static final int OrientationRotateRightMirror = 5;
    public static final int OrientationRotate180Degree = 6;
    public static final int OrientationRotate180DegreeMirror = 7;

    public GLFilterCache() {
//        String resourceDirectory = getResourceDirectory(ElephantApp.getInstance().getApplicationContext());
//        init(resourceDirectory);
        init(null);
    }

    private static String getResourceDirectory(Context context) {
        String[] resFileNames = new String[]{"lookup_miss_etikate.png"
                , "lookup_amatorka.png"
                , "lookup_soft_elegance_1.png"
                , "lookup_soft_elegance_2.png"};
        String resPath = bootstrap.appContainer.AppStorageManager.getResDirEndWithSeparator();
        File directory = new File(resPath);
        if (!directory.exists()) {
            directory.mkdirs();
        }
        File file = new File(resPath, resFileNames[0]);
        if (!file.exists()) {
            int[] resResIDs = new int[]{R.raw.lookup_miss_etikate, R.raw.lookup_amatorka, R.raw.lookup_soft_elegance_1, R.raw.lookup_soft_elegance_2};
            try {
                for (int i = 0; i < resResIDs.length; ++i) {
                    InputStream ins = context.getResources().openRawResource(resResIDs[i]);
                    int size = ins.available();
                    byte[] data = new byte[size];
                    ins.read(data);
                    ins.close();

                    File tmpFile = new File(resPath, resFileNames[i]);
                    FileOutputStream fos = new FileOutputStream(tmpFile);
                    fos.write(data);
                    fos.close();
                }
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
        return resPath;
    }

    @Override
    public void finalize() {
        dealloc();
    }

    public final native void render(int filterID, int x, int y, int width, int height, int sourceTexture, int sourceTextureTarget);

    public final native void render(int filterID, int x, int y, int width, int height, int sourceTexture, int sourceTextureTarget, int sourceOrientation, Vec2f texcoordOrigin, Vec2f texcoordSize);

    public final native void releaseGLObjects();

    private final native void init(String resourceDirectory);

    private final native void dealloc();

    private long nativeGLFilterCachePointer = 0;

    static {
        System.loadLibrary("madvframework");
    }
}
