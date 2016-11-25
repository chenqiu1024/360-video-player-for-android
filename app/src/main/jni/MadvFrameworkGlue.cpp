//
// Created by QiuDong on 16/5/30.
//
#include "MadvFrameworkGlue.h"
#include "MadvGLRenderer_Android.h"
#include "GLFilters/GLFilterCache.h"
#include "GLRenderTexture.h"
#include <fstream>
#include <stdio.h>

using namespace std;

static jclass clazz_Vec2f = NULL;
static jfieldID field_Vec2f_x = NULL;
static jfieldID field_Vec2f_y = NULL;

static jclass clazz_Renderer = NULL;
static jfieldID field_Renderer_nativeGLRendererPointer = NULL;

static jclass clazz_FilterCache = NULL;
static jfieldID field_FilterCache_nativeGLFilterCachePointer = NULL;

static jclass clazz_GLRenderTexture = NULL;
static jfieldID field_GLRenderTexture_nativeGLRenderTexturePointer = NULL;

static jmethodID method_Vec2f_CtorFF = NULL;

Vec2f Vec2fFromJava(JNIEnv *env, jobject vec2Obj) {
    if (NULL == clazz_Vec2f)
    {
        clazz_Vec2f = env->GetObjectClass(vec2Obj);
        ALOGE("Vec2f class = %ld", clazz_Vec2f);
        field_Vec2f_x = env->GetFieldID(clazz_Vec2f, "x", "F");
        ALOGE("field_Vec2f_x = %ld", field_Vec2f_x);
        field_Vec2f_y = env->GetFieldID(clazz_Vec2f, "y", "F");
    }
    Vec2f ret;
    if (NULL != vec2Obj)
    {
        ret.width = (GLfloat) env->GetFloatField(vec2Obj, field_Vec2f_x);
        ret.height = (GLfloat) env->GetFloatField(vec2Obj, field_Vec2f_y);
    }
    else
    {
        ret.width = ret.height = 0;
    }
    return ret;
}

MadvGLRenderer_Android* getCppRendererFromJavaRenderer(JNIEnv* env, jobject self) {
    if (NULL == clazz_Renderer)
    {
        clazz_Renderer = env->GetObjectClass(self);
        ALOGE("My class = %ld", clazz_Renderer);
        field_Renderer_nativeGLRendererPointer = env->GetFieldID(clazz_Renderer, "nativeGLRendererPointer", "J");
        ALOGE("field_Renderer_nativeGLRendererPointer = %ld", field_Renderer_nativeGLRendererPointer);
    }
    jlong pointer = env->GetLongField(self, field_Renderer_nativeGLRendererPointer);
    MadvGLRenderer_Android* ret = (MadvGLRenderer_Android*) (void*) pointer;
    return ret;
}

void setCppRendererFromJavaRenderer(JNIEnv* env, jobject self, MadvGLRenderer_Android* pRenderer) {
    if (NULL == clazz_Renderer)
    {
        clazz_Renderer = env->GetObjectClass(self);
        field_Renderer_nativeGLRendererPointer = env->GetFieldID(clazz_Renderer, "nativeGLRendererPointer", "J");
    }
    jlong pointer = (jlong) pRenderer;
    env->SetLongField(self, field_Renderer_nativeGLRendererPointer, pointer);
}

GLFilterCache* getCppFilterCacheFromJavaFilterCache(JNIEnv* env, jobject self) {
    if (NULL == clazz_FilterCache)
    {
        clazz_FilterCache = env->GetObjectClass(self);
        field_FilterCache_nativeGLFilterCachePointer = env->GetFieldID(clazz_FilterCache, "nativeGLFilterCachePointer", "J");
    }
    jlong pointer = env->GetLongField(self, field_FilterCache_nativeGLFilterCachePointer);
    GLFilterCache* ret = (GLFilterCache*) (void*) pointer;
    return ret;
}

void setCppFilterCacheFromJavaFilterCache(JNIEnv* env, jobject self, GLFilterCache* pGLFilterCache) {
    if (NULL == clazz_FilterCache)
    {
        clazz_FilterCache = env->GetObjectClass(self);
        field_FilterCache_nativeGLFilterCachePointer = env->GetFieldID(clazz_FilterCache, "nativeGLFilterCachePointer", "J");
    }
    jlong pointer = (jlong) pGLFilterCache;
    env->SetLongField(self, field_FilterCache_nativeGLFilterCachePointer, pointer);
}

GLRenderTexture* getGLRenderTextureFromJava(JNIEnv* env, jobject self) {
    if (NULL == clazz_GLRenderTexture)
    {
        clazz_GLRenderTexture = env->GetObjectClass(self);
        field_GLRenderTexture_nativeGLRenderTexturePointer = env->GetFieldID(clazz_GLRenderTexture, "nativeGLRenderTexturePointer", "J");
    }
    jlong pointer = env->GetLongField(self, field_GLRenderTexture_nativeGLRenderTexturePointer);
    GLRenderTexture* ret = (GLRenderTexture*) (void*) pointer;
    return ret;
}

void setGLRenderTextureToJava(JNIEnv* env, jobject self, GLRenderTexture* pGLRenderTexture) {
    if (NULL == clazz_GLRenderTexture)
    {
        clazz_GLRenderTexture = env->GetObjectClass(self);
        field_GLRenderTexture_nativeGLRenderTexturePointer = env->GetFieldID(clazz_GLRenderTexture, "nativeGLRenderTexturePointer", "J");
    }
    jlong pointer = (jlong) pGLRenderTexture;
    env->SetLongField(self, field_GLRenderTexture_nativeGLRenderTexturePointer, pointer);
}

JNIEXPORT jlong JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_createNativeGLRenderer
        (JNIEnv* env, jobject self, jstring lutPath, jobject srcTextureSizeL, jobject srcTextureSizeR) {
    ALOGE("Java_com_madv360_glrenderer_MadvGLRenderer_createNativeGLRenderer");
    jboolean copied = false;
    const char* cstrLUTPath = (lutPath == NULL ? NULL : env->GetStringUTFChars(lutPath, &copied));
//    ALOGE("LUT Path : isCopy = %d, %s", copied, cstrLUTPath);
    Vec2f leftSrcTextureSize = Vec2fFromJava(env, srcTextureSizeL);
    Vec2f rightSrcTextureSize = Vec2fFromJava(env, srcTextureSizeR);

    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (glRenerer)
    {
        delete glRenerer;
    }
    glRenerer = new MadvGLRenderer_Android(cstrLUTPath, leftSrcTextureSize, rightSrcTextureSize);
    setCppRendererFromJavaRenderer(env, self, glRenerer);

    if (lutPath && copied)
    {
        env->ReleaseStringUTFChars(lutPath, cstrLUTPath);
    }

    return (long)(void*)glRenerer;
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_releaseNativeGLRenderer
                (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (glRenerer)
    {
        delete glRenerer;
    }
    setCppRendererFromJavaRenderer(env, self, NULL);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setSourceTextures
(JNIEnv* env, jobject self, jboolean separateSourceTexture, jint srcTextureL, jint srcTextureR, jobject srcTextureSizeL, jobject srcTextureSizeR, jint srcTextureTarget, jboolean isYUVColorSpace) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;
//    ALOGE("srcTextureL = %d, srcTextureR = %d, srcTextureTarget = %x", srcTextureL, srcTextureR, srcTextureTarget);
    glRenerer->setSourceTextures(separateSourceTexture, srcTextureL, srcTextureR, srcTextureTarget, isYUVColorSpace);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setSourceYUVTextures
        (JNIEnv* env, jobject self, jboolean separateSourceTexture, jintArray srcTextureL, jintArray srcTextureR, jobject srcTextureSizeL, jobject srcTextureSizeR, jint srcTextureTarget, jboolean isYUVColorSpace) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;
//    ALOGE("srcTextureL = %d, srcTextureR = %d, srcTextureTarget = %x", srcTextureL, srcTextureR, srcTextureTarget);
    jboolean isCopyR, isCopyL;
    jint* dataL = env->GetIntArrayElements(srcTextureL, &isCopyL);
    jint* dataR = env->GetIntArrayElements(srcTextureR, &isCopyR);

    glRenerer->setSourceTextures(separateSourceTexture, dataL, dataR, srcTextureTarget, isYUVColorSpace);

    if (isCopyL) free(dataL);
    if (isCopyR) free(dataR);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setCapsTexture
        (JNIEnv* env, jobject self, jint texture, jint textureTarget) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;
//    ALOGE("srcTextureL = %d, srcTextureR = %d, srcTextureTarget = %x", srcTextureL, srcTextureR, srcTextureTarget);
    glRenerer->setCapsTexture(texture, textureTarget);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setNeedDrawCaps
        (JNIEnv* env, jobject self, jboolean drawCaps) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;
//    ALOGE("srcTextureL = %d, srcTextureR = %d, srcTextureTarget = %x", srcTextureL, srcTextureR, srcTextureTarget);
    glRenerer->setNeedDrawCaps(drawCaps);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_draw__IIII
(JNIEnv* env, jobject self, jint x, jint y, jint width, jint height) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->draw(x,y,width,height);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_draw__IIIIIZLcom_madv360_glrenderer_Vec2f_2Lcom_madv360_glrenderer_Vec2f_2III
        (JNIEnv* env, jobject self, jint currentDisplayMode, jint x, jint y, jint width, jint height, jboolean separateSourceTextures, jobject leftSrcSize, jobject rightSrcSize, jint srcTextureType, jint leftSrcTexture, jint rightSrcTexture) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->draw(currentDisplayMode, x,y,width,height, separateSourceTextures, Vec2fFromJava(env,leftSrcSize), Vec2fFromJava(env,rightSrcSize), srcTextureType, leftSrcTexture, rightSrcTexture);
}

//JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setEulerAngles
//(JNIEnv* env, jobject self, jfloat yawRadius, jfloat pitchRadius, jfloat bankRadius) {
//    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
//    if (!glRenerer) return;
//
//    glRenerer->setEulerAngles(yawRadius, pitchRadius, bankRadius);
//}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setGyroQuaternion
(JNIEnv* env, jobject self, jfloat x, jfloat y, jfloat z, jfloat w) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->setGyroQuaternion({x,y,z,w});
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_resetGyroQuaternion
        (JNIEnv* env, jobject self, jfloat x, jfloat y, jfloat z, jfloat w) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->resetGyroQuaternion({x,y,z,w});
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_commitGyroQuaternion
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->commitGyroQuaternion();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_normalizeRotationMatrix
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->normalize();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setPanPoint(JNIEnv* env, jobject self, jobject point) {
//    ALOGE("Java_com_madv360_glrenderer_MadvGLRenderer_rotationByProjectedPoints");
    Vec2f v2Point = Vec2fFromJava(env, point);

    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->setPanPoint(v2Point);
}

JNIEXPORT jobject JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getPanPoint(JNIEnv* env, jobject self) {
    if (NULL == method_Vec2f_CtorFF)
    {
//        if (NULL == clazz_Vec2f)
        {
            // http://blog.k-res.net/archives/1525.html ; http://www.eoeandroid.com/thread-564427-1-1.html ;
            jclass tmp = env->FindClass("com/madv360/glrenderer/Vec2f");
            clazz_Vec2f = (jclass) env->NewGlobalRef(tmp);
        }
        method_Vec2f_CtorFF = env->GetMethodID(clazz_Vec2f, "<init>", "(FF)V");
    }

    Vec2f v2Point;
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer)
    {
        v2Point = {0,0};
    }
    else
    {
        v2Point = glRenerer->glCamera()->getPanPoint();
    }
    return env->NewObject(clazz_Vec2f, method_Vec2f_CtorFF, v2Point.x, v2Point.y);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_resetPanPoint(JNIEnv* env, jobject self, jobject point) {
//    ALOGE("Java_com_madv360_glrenderer_MadvGLRenderer_rotationByProjectedPoints");
    Vec2f v2Point = Vec2fFromJava(env, point);

    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->resetPanPoint(v2Point);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_commitPanPoint
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->commitPanPoint();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_commitMatrix
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->commitMatrix();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_resetRotation
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->resetRotation();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_adustPosition
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->adustPosition();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setGyroMatrix
        (JNIEnv* env, jobject self, jfloatArray matrix, jint rank) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    jboolean isCopy;
    jfloat* matrixData = env->GetFloatArrayElements(matrix, &isCopy);

    glRenerer->setGyroMatrix(matrixData, rank);
    if (isCopy)
    {
        env->ReleaseFloatArrayElements(matrix, matrixData, 0);
    }
}

JNIEXPORT jint JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getDisplayMode
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return 0;

    return glRenerer->getDisplayMode();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setDisplayMode
               (JNIEnv* env, jobject self, jint displayMode) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->setDisplayMode(displayMode);
}

JNIEXPORT jboolean JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getIsYUVColorSpace
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return 0;
    return glRenerer->getIsYUVColorSpace();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setIsYUVColorSpace
                   (JNIEnv* env, jobject self, jboolean isYUVColorSpace) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->setIsYUVColorSpace(isYUVColorSpace);
}

JNIEXPORT jfloat JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getFocalLength
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return CLIP_Z_NEAR;

    return glRenerer->getFocalLength();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setFocalLength
                 (JNIEnv* env, jobject self, jfloat focalLength) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->setFocalLength(focalLength);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setFOVDegree
        (JNIEnv* env, jobject self, jint fovDegree) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->setFOVDegree(fovDegree);
}

//JNIEXPORT jfloat JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getYawRadius
//        (JNIEnv* env, jobject self) {
//    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
//    if (!glRenerer) return 0;
//
//    return glRenerer->getYawRadius();
//}
//
//JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setYawRadius
//                 (JNIEnv* env, jobject self, jfloat yawRadius) {
//    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
//    if (!glRenerer) return;
//
//    glRenerer->setYawRadius(yawRadius);
//}
//
//JNIEXPORT jfloat JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getPitchRadius
//        (JNIEnv* env, jobject self) {
//    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
//    if (!glRenerer) return 0;
//
//    return glRenerer->getPitchRadius();
//}
//
//JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setPitchRadius
//        (JNIEnv* env, jobject self, jfloat pitchRadius) {
//    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
//    if (!glRenerer) return;
//
//    glRenerer->setPitchRadius(pitchRadius);
//}
//
//JNIEXPORT jfloat JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getBankRadius
//        (JNIEnv* env, jobject self) {
//    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
//    if (!glRenerer) return 0;
//
//    return glRenerer->getBankRadius();
//}
//
//JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setBankRadius
//        (JNIEnv* env, jobject self, jfloat bankRadius) {
//    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
//    if (!glRenerer) return;
//
//    glRenerer->setBankRadius(bankRadius);
//}

JNIEXPORT jboolean JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getFlipX
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return false;

    return glRenerer->getFlipX();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setFlipX
        (JNIEnv* env, jobject self, jboolean flipX) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->setFlipX(flipX);
}

JNIEXPORT jboolean JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getFlipY
        (JNIEnv* env, jobject self) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return false;

    return glRenerer->getFlipY();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setFlipY
        (JNIEnv* env, jobject self, jboolean flipY) {
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;
//    ALOGE("setFlipY : %d", flipY);
    glRenerer->setFlipY(flipY);
}

JNIEXPORT void JNICALL Java_com_madv360_madv_utils_FileUtil_saveFileChunk
        (JNIEnv* env, jclass selfClass, jstring filePath, jlong fileOffset, jbyteArray data, jlong dataOffset, jlong length) {
    ALOGE("Java_com_madv360_madv_utils_FileUtil_saveFileChunk");
    jboolean copied = false;
    const char* cstrFilePath = env->GetStringUTFChars(filePath, &copied);
//    ALOGE("cstrFilePath = %s, copied = %d", cstrFilePath, copied);

    jbyte* bytes = env->GetByteArrayElements(data,0);
    jsize bytesLength = env->GetArrayLength(data);
//    ALOGE("bytesLength = %d", bytesLength);

    FILE* fp = fopen(cstrFilePath, "ab+");
    int ferr = errno;//ferror(fp);//
    if (0 == fp)
    {
        ALOGE("CreateIfExist file %s : fp=%ld, ferr=%d, strerr=%s", cstrFilePath, (long)fp, ferr, strerror(ferr));
    }
//    ALOGE("CreateIfExist file %s OK: fp=%ld, ferr=%d, strerr=%s", cstrFilePath, (long)fp, ferr, strerror(ferr));
    fclose(fp);

    ofstream ofs(cstrFilePath, ios::out | ios::in | ios::binary);
    //*
    const uint64_t Limit2G = 0x80000000;
    if (fileOffset >= Limit2G)
    {
//        ALOGV("#0 : fileOffset = %ld", fileOffset);
        ofs.seekp(0x40000000, ios::beg);
        ofs.seekp(0x40000000, ios::cur);
        for (fileOffset -= Limit2G; fileOffset >= Limit2G; fileOffset -= Limit2G)
        {
            ofs.seekp(0x40000000, ios::cur);
            ofs.seekp(0x40000000, ios::cur);
//            ALOGV("#1 : fileOffset = %ld", fileOffset);
        }
//        ALOGV("#2 : fileOffset = %ld", fileOffset);
        ofs.seekp(fileOffset, ios::cur);
    }
    else
    {
//        ALOGV("#3 : fileOffset = %ld", fileOffset);
        ofs.seekp(fileOffset, ios::beg);
        fileOffset = 0;
    }
    /*/
    ofs.seekp(0x40000000, ios::beg);
    ofs.seekp(0x40000000, ios::cur);
    ofs.seekp(0x40000000, ios::cur);
    ofs.seekp(0x40000000, ios::cur);
    ofs.seekp(fileOffset, ios::cur);
    //*/
    ALOGV("saveFileChunk #4 : bytes=%lx, dataOffset=%ld, length=%ld", (long)bytes, dataOffset, length);
    if (length > 0)
        ofs.write((const char*)bytes + dataOffset, (int)length);
//    ALOGV("saveFileChunk #5");
    ofs.flush();
//    ALOGV("saveFileChunk #6");
    ofs.close();
//    ALOGV("saveFileChunk #7");
    if (copied)
    {
        env->ReleaseStringUTFChars(filePath, cstrFilePath);
    }
    ALOGE("saveFileChunk DONE");
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_rotationByProjectedPoints(JNIEnv* env, jobject self, jobject fromPoint, jobject toPoint) {
//    ALOGE("Java_com_madv360_glrenderer_MadvGLRenderer_rotationByProjectedPoints");
    Vec2f v2FromPoint = Vec2fFromJava(env, fromPoint);
    Vec2f v2ToPoint = Vec2fFromJava(env, toPoint);
    kmVec2 kmFromPoint = {v2FromPoint.x, v2FromPoint.y};
    kmVec2 kmToPoint = {v2ToPoint.x, v2ToPoint.y};

    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->rotationByProjectedPoints(kmFromPoint, kmToPoint);
}

JNIEXPORT jfloat JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getDiffYawRadius(JNIEnv* env, jobject self) {
//    ALOGE("Java_com_madv360_glrenderer_MadvGLRenderer_rotationByProjectedPoints");
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return 0;

    return glRenerer->glCamera()->getDiffYawRadius();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setDiffYawRadius(JNIEnv* env, jobject self, jfloat diffYawRadius) {
//    ALOGE("Java_com_madv360_glrenderer_MadvGLRenderer_rotationByProjectedPoints");
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->setDiffYawRadius(diffYawRadius);
}

JNIEXPORT jfloat JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_getDiffPitchRadius(JNIEnv* env, jobject self) {
//    ALOGE("Java_com_madv360_glrenderer_MadvGLRenderer_rotationByProjectedPoints");
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return 0;

    return glRenerer->glCamera()->getDiffPitchRadius();
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_setDiffPitchRadius(JNIEnv* env, jobject self, jfloat diffPitchRadius) {
//    ALOGE("Java_com_madv360_glrenderer_MadvGLRenderer_rotationByProjectedPoints");
    MadvGLRenderer_Android* glRenerer = getCppRendererFromJavaRenderer(env, self);
    if (!glRenerer) return;

    glRenerer->glCamera()->setDiffPitchRadius(diffPitchRadius);
}

JNIEXPORT void JNICALL Java_com_madv360_madv_utils_FileUtil_extractLUTFiles
        (JNIEnv* env, jclass selfClass, jstring destDirectory, jstring lutBinFilePath, jint fileOffset) {
    jboolean binFilePathCopied = false;
    const char* cstrBinFilePath = env->GetStringUTFChars(lutBinFilePath, &binFilePathCopied);
//    ALOGE("cstrFilePath = %s, copied = %d", cstrBinFilePath, binFilePathCopied);
    jboolean destDirCopied = false;
    const char* cstrDestDirectory = env->GetStringUTFChars(destDirectory, &destDirCopied);
//    ALOGE("cstrDestDir = %s, copied = %d", cstrDestDirectory, destDirCopied);

    ifstream ifs(cstrBinFilePath, ios::in | ios::binary);
//    ALOGE("extractLUTFiles : fileOffset = %u sizeof(long) = %d", fileOffset, sizeof(long));
//    fseek(fp, fileOffset, SEEK_CUR);
    const uint32_t Limit2G = 0x80000000;
    if (fileOffset >= Limit2G)
    {
        uint32_t fileOffsetLeft = fileOffset;
//        ALOGE("extractLUTFiles : #0 fileOffsetLeft = %u", fileOffsetLeft);
        ifs.seekg(0x40000000, ios::beg);
        ifs.seekg(0x40000000, ios::cur);
        for (fileOffsetLeft -= Limit2G; fileOffsetLeft >= Limit2G; fileOffsetLeft -= Limit2G)
        {
//            ALOGE("extractLUTFiles : #1 fileOffsetLeft = %u", fileOffsetLeft);
            ifs.seekg(0x40000000, ios::cur);
            ifs.seekg(0x40000000, ios::cur);
        }
//        ALOGE("extractLUTFiles : #2 fileOffsetLeft = %u", fileOffsetLeft);
        ifs.seekg(fileOffsetLeft, ios::cur);
    }
    else
    {
        ifs.seekg(fileOffset, ios::beg);
    }

    uint32_t offsets[8];
    uint32_t sizes[8];
    uint32_t totalSize = 0;
    uint32_t maxSize = 0;
    for (int i=0; i<8; ++i)
    {
        ifs.read((char*)&offsets[i], sizeof(uint32_t));
        ifs.read((char*)&sizes[i], sizeof(uint32_t));
//        ALOGE("offsets[%d] = %u, sizes[%d] = %u", i,offsets[i], i,sizes[i]);
        if (sizes[i] > maxSize) maxSize = sizes[i];
        totalSize += sizes[i];
    }
    ifs.close();
//    ALOGV("totalSize = %u", totalSize);

    const char* pngFileNames[] = {"/r_x_int.png", "/r_x_min.png",
                                  "/r_y_int.png", "/r_y_min.png",
                                  "/l_x_int.png", "/l_x_min.png",
                                  "/l_y_int.png", "/l_y_min.png"};
    char* pngFilePath = (char*) malloc(strlen(cstrDestDirectory) + strlen(pngFileNames[0]) + 1);

    uint8_t* pngData = (uint8_t*) malloc(maxSize);
    fstream ofs(cstrBinFilePath, ios::out | ios::in | ios::binary);
    if (fileOffset >= Limit2G)
    {
        ofs.seekp(0x40000000, ios::beg);
        ofs.seekp(0x40000000, ios::cur);
        for (fileOffset -= Limit2G; fileOffset >= Limit2G; fileOffset -= Limit2G)
        {
            ofs.seekp(0x40000000, ios::cur);
            ofs.seekp(0x40000000, ios::cur);
        }
        ofs.seekp(fileOffset, ios::cur);
    }
    else
    {
        ofs.seekp(fileOffset, ios::beg);
    }

    uint64_t currentOffset = 0;
    for (int i=0; i<8; ++i)
    {
        ofs.seekp(offsets[i] - currentOffset, ios::cur);
        ofs.read((char*)pngData, sizes[i]);
        sprintf(pngFilePath, "%s%s", cstrDestDirectory, pngFileNames[i]);
        FILE* fout = fopen(pngFilePath, "wb+");
        fwrite(pngData, sizes[i], 1, fout);
        fclose(fout);
        currentOffset = offsets[i] + sizes[i];
    }
    ofs.close();
    free(pngData);
    free(pngFilePath);

    if (binFilePathCopied)
    {
        env->ReleaseStringUTFChars(lutBinFilePath, cstrBinFilePath);
    }
    if (destDirCopied)
    {
        env->ReleaseStringUTFChars(destDirectory, cstrDestDirectory);
    }
}

JNIEXPORT jbyteArray JNICALL Java_com_madv360_glrenderer_MadvGLRenderer_renderThumbnail
        (JNIEnv* env, jclass selfClass, jint sourceTexture, jobject jobjSrcSize, jobject jobjDestSize, jstring lutPath) {
    ALOGE("Java_com_madv360_madv_utils_FileUtil_renderThubmnail");
    jboolean copied = false;
    const char* cstrLUTPath;
    if (NULL == lutPath)
    {
        cstrLUTPath = NULL;
    }
    else
    {
        cstrLUTPath = env->GetStringUTFChars(lutPath, &copied);
        ALOGE("cstrLUTPath = %s, copied = %d", cstrLUTPath, copied);
    }

    Vec2f srcSize = Vec2fFromJava(env, jobjSrcSize);
    Vec2f dstSize = Vec2fFromJava(env, jobjDestSize);

    jbyte* rgbData = (jbyte*) MadvGLRenderer_Android::renderThumbnail(sourceTexture, srcSize, dstSize, cstrLUTPath);
    int byteLength = 4 * dstSize.width * dstSize.height;
    jbyteArray jaRGBArray = env->NewByteArray(byteLength);
    env->SetByteArrayRegion(jaRGBArray, 0, byteLength, rgbData);

    if (copied)
    {
        env->ReleaseStringUTFChars(lutPath, cstrLUTPath);
    }
    ALOGE("renderThumbnail DONE");

    return jaRGBArray;
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLFilterCache_init(JNIEnv* env, jobject self, jstring resourceDirectory) {
    GLFilterCache* filterCache = getCppFilterCacheFromJavaFilterCache(env, self);
    if (filterCache)
    {
        delete filterCache;
    }

    jboolean copied = false;
    const char* cstrResDir = (resourceDirectory == NULL ? NULL : env->GetStringUTFChars(resourceDirectory, &copied));

    filterCache = new GLFilterCache(cstrResDir);
    setCppFilterCacheFromJavaFilterCache(env, self, filterCache);

    if (copied)
    {
        env->ReleaseStringUTFChars(resourceDirectory, cstrResDir);
    }
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLFilterCache_dealloc(JNIEnv* env, jobject self) {
    GLFilterCache* filterCache = getCppFilterCacheFromJavaFilterCache(env, self);
    if (filterCache)
    {
        delete filterCache;
    }
    setCppFilterCacheFromJavaFilterCache(env, self, NULL);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLFilterCache_render__IIIIIII(JNIEnv* env, jobject self,
                                                                        jint filterID, jint x, jint y, jint width, jint height, jint sourceTexture, jint sourceTextureTarget) {
    GLFilterCache* filterCache = getCppFilterCacheFromJavaFilterCache(env, self);
    if (filterCache)
    {
        filterCache->render(filterID, x, y, width, height, sourceTexture, sourceTextureTarget);
    }
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLFilterCache_render__IIIIIIIILcom_madv360_glrenderer_Vec2f_2Lcom_madv360_glrenderer_Vec2f_2(JNIEnv* env, jobject self,
                                                                        jint filterID, jint x, jint y, jint width, jint height, jint sourceTexture, jint sourceTextureTarget, jint sourceOrientation, jobject jTexcoordOrigin, jobject jTexcoordSize) {
    GLFilterCache* filterCache = getCppFilterCacheFromJavaFilterCache(env, self);
    if (filterCache)
    {
        Vec2f texcoordOrigin = Vec2fFromJava(env, jTexcoordOrigin);
        Vec2f texcoordSize = Vec2fFromJava(env, jTexcoordSize);
        filterCache->render(filterID, x, y, width, height, sourceTexture, sourceTextureTarget, (Orientation2D)sourceOrientation, texcoordOrigin, texcoordSize);
    }
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLFilterCache_releaseGLObjects(JNIEnv* env, jobject self) {
    GLFilterCache* filterCache = getCppFilterCacheFromJavaFilterCache(env, self);
    if (filterCache)
    {
        filterCache->releaseGLObjects();
    }
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLRenderTexture_init(JNIEnv* env, jobject self, jint width, jint height) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        delete renderTexture;
    }
    renderTexture = new GLRenderTexture(width, height);
    setGLRenderTextureToJava(env, self, renderTexture);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLRenderTexture_dealloc(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        delete renderTexture;
    }
    setGLRenderTextureToJava(env, self, NULL);
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLRenderTexture_releaseGLObjects(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        renderTexture->releaseGLObjects();
    }
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLRenderTexture_blit(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        renderTexture->blit();
    }
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLRenderTexture_unblit(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        renderTexture->unblit();
    }
}

JNIEXPORT jint JNICALL Java_com_madv360_glrenderer_GLRenderTexture_copyPixelData(JNIEnv* env, jobject self, jbyteArray jData) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
//        int bytesLength = renderTexture->bytesLength();
//        jbyteArray array = env->NewByteArray(bytesLength);
//        GLubyte* bytes = renderTexture->copyPixelData();
//        env->SetByteArrayRegion(array, 0, bytesLength, (const jbyte*)bytes);
//        return array;
        int bytesLength = env->GetArrayLength(jData);
        jboolean isCopy;
        jbyte* data = env->GetByteArrayElements(jData, &isCopy);
        jint ret = renderTexture->copyPixelData((uint8_t*)data, 0, bytesLength);
        if (isCopy)
        {
            free(data);
        }
        return ret;
    }
    return 0;
}

JNIEXPORT jbyteArray JNICALL Java_com_madv360_glrenderer_GLRenderTexture_copyPixelDataFromPBO(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        int bytesLength = renderTexture->bytesLength();
        GLubyte* pixels = renderTexture->copyPixelDataFromPBO(0, bytesLength);
        if (pixels)
        {
            jbyteArray ret = env->NewByteArray(bytesLength);
            ALOGE("copyPixelDataFromPBO : pixels = %lx, ret = %lx, bytesLength = %d", (long)pixels, (long)ret, bytesLength);
            env->SetByteArrayRegion(ret, 0, bytesLength, (const jbyte*) pixels);
            ALOGE("copyPixelDataFromPBO : SetByteArrayRegion OK");
            return ret;
        }
    }
    return NULL;
}

JNIEXPORT jint JNICALL Java_com_madv360_glrenderer_GLRenderTexture_getFramebuffer(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        return renderTexture->getFramebuffer();
    }
    return 0;
}

JNIEXPORT jint JNICALL Java_com_madv360_glrenderer_GLRenderTexture_getTexture(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        return renderTexture->getTexture();
    }
    return 0;
}

JNIEXPORT jint JNICALL Java_com_madv360_glrenderer_GLRenderTexture_getWidth(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        return renderTexture->getWidth();
    }
    return 0;
}

JNIEXPORT jint JNICALL Java_com_madv360_glrenderer_GLRenderTexture_getHeight(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        return renderTexture->getHeight();
    }
    return 0;
}

JNIEXPORT jint JNICALL Java_com_madv360_glrenderer_GLRenderTexture_bytesLength(JNIEnv* env, jobject self) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        return renderTexture->bytesLength();
    }
    return 0;
}

JNIEXPORT void JNICALL Java_com_madv360_glrenderer_GLRenderTexture_resizeIfNecessary(JNIEnv* env, jobject self, jint width, jint height) {
    GLRenderTexture* renderTexture = getGLRenderTextureFromJava(env, self);
    if (renderTexture)
    {
        renderTexture->resizeIfNecessary(width, height);
    }
}
