//
// Created by QiuDong on 16/5/31.
//

#include "MadvGLRenderer_Android.h"
#include "GLRenderTexture.h"
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include "ImageCodec.h"

MadvGLRenderer_Android::~MadvGLRenderer_Android() {
}

MadvGLRenderer_Android::MadvGLRenderer_Android(const char* lutPath, Vec2f leftSrcSize, Vec2f rightSrcSize)
        : MadvGLRenderer(lutPath, leftSrcSize, rightSrcSize)
{
    prepareLUT(lutPath, leftSrcSize, rightSrcSize);
}

void MadvGLRenderer_Android::prepareLUT(const char* lutPath, Vec2f leftSrcSize, Vec2f rightSrcSize) {
    if (NULL == lutPath) return;

    pic_data lutDatas[8];
    const char* LUTFilePathPattern[] = {"%s/l_x_int.png", "%s/l_x_min.png", "%s/l_y_int.png", "%s/l_y_min.png", "%s/r_x_int.png", "%s/r_x_min.png", "%s/r_y_int.png", "%s/r_y_min.png"};

    char* lutFilePath = (char*) malloc(strlen(lutPath) + strlen("/L_x_int.png") + 1);
    for (int i=0; i<8; ++i)
    {
        sprintf(lutFilePath, LUTFilePathPattern[i], lutPath);
        int result = decodePNG((char*)lutFilePath, lutDatas + i);
        ALOGE("lutFilePath = %s, result = %d\n", lutFilePath, result);

        unsigned short* pShort = (unsigned short*) lutDatas[i].rgba;
        unsigned short min = 10240, max = 0;
        int length = lutDatas[i].width * lutDatas[i].height;
        for (int j=length; j>0; --j)
        {
            unsigned short s = *pShort;
            s = ((s << 8) & 0xff00) | ((s >> 8) & 0x00ff);
            *pShort++ = s;

            if (s > max) max = s;
            if (s < min) min = s;
        }
        ALOGE("Data length = %d, min = %d, max = %d", length, min, max);
    }

    ALOGE("lutDstSize = (%d,%d), leftSrcSize = (%f,%f), rightSrcSize = (%f,%f)", lutDatas[0].width, lutDatas[0].height, leftSrcSize.width,leftSrcSize.height, rightSrcSize.width,rightSrcSize.height);
    setLUTData(Vec2f{(GLfloat)lutDatas[0].width, (GLfloat)lutDatas[0].height}, leftSrcSize, rightSrcSize, lutDatas[0].width * lutDatas[0].height,
               (unsigned short*)lutDatas[0].rgba, (unsigned short*)lutDatas[1].rgba, (unsigned short*)lutDatas[2].rgba, (unsigned short*)lutDatas[3].rgba, (unsigned short*)lutDatas[4].rgba, (unsigned short*)lutDatas[5].rgba, (unsigned short*)lutDatas[6].rgba, (unsigned short*)lutDatas[7].rgba);

    for (int i=0; i<8; ++i)
    {
        free(lutDatas[i].rgba);
    }
    free(lutFilePath);
}

void MadvGLRenderer_Android::prepareTextureWithRenderSource(void* renderSource) {
    ALOGE("MadvGLRenderer_Android::prepareTextureWithRenderSource");
}

GLubyte* MadvGLRenderer_Android::renderThumbnail(GLint sourceTexture, Vec2f srcSize, Vec2f destSize, const char* lutPath) {
    // EGL config attributes
    const EGLint confAttr[] =
            {
                    EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT_KHR,// very important!
                    EGL_SURFACE_TYPE,EGL_PBUFFER_BIT,//EGL_WINDOW_BIT EGL_PBUFFER_BIT we will create a pixelbuffer surface
                    EGL_RED_SIZE,   8,
                    EGL_GREEN_SIZE, 8,
                    EGL_BLUE_SIZE,  8,
                    EGL_ALPHA_SIZE, 8,// if you need the alpha channel
                    EGL_DEPTH_SIZE, 8,// if you need the depth buffer
                    EGL_STENCIL_SIZE,8,
                    EGL_NONE
            };
    // EGL context attributes
    const EGLint ctxAttr[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,// very important!
            EGL_NONE
    };
    // surface attributes
    // the surface size is set to the input frame size
    const EGLint surfaceAttr[] = {
            EGL_WIDTH, (EGLint)srcSize.width,
            EGL_HEIGHT, (EGLint)srcSize.height,
            EGL_NONE
    };
    EGLint eglMajVers, eglMinVers;
    EGLint numConfigs;

    EGLDisplay eglDisp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if(eglDisp == EGL_NO_DISPLAY)
    {
        //Unable to open connection to local windowing system
        ALOGE("Unable to open connection to local windowing system");
    }
    if(!eglInitialize(eglDisp, &eglMajVers, &eglMinVers))
    {
        // Unable to initialize EGL. Handle and recover
        ALOGE("Unable to initialize EGL");
    }
    ALOGV("EGL init with version %d.%d", eglMajVers, eglMinVers);
    // choose the first config, i.e. best config
    EGLConfig eglConf;
    if(!eglChooseConfig(eglDisp, confAttr, &eglConf, 1, &numConfigs))
    {
        ALOGE("some config is wrong");
    }
    else
    {
        ALOGE("all configs is OK");
    }
    // create a pixelbuffer surface
    EGLSurface eglSurface = eglCreatePbufferSurface(eglDisp, eglConf, surfaceAttr);
    if(eglSurface == EGL_NO_SURFACE)
    {
        switch(eglGetError())
        {
            case EGL_BAD_ALLOC:
                // Not enough resources available. Handle and recover
                ALOGE("Not enough resources available");
                break;
            case EGL_BAD_CONFIG:
                // Verify that provided EGLConfig is valid
                ALOGE("provided EGLConfig is invalid");
                break;
            case EGL_BAD_PARAMETER:
                // Verify that the EGL_WIDTH and EGL_HEIGHT are
                // non-negative values
                ALOGE("provided EGL_WIDTH and EGL_HEIGHT is invalid");
                break;
            case EGL_BAD_MATCH:
                // Check window and EGLConfig attributes to determine
                // compatibility and pbuffer-texture parameters
                ALOGE("Check window and EGLConfig attributes");
                break;
        }
    }
    EGLContext eglCtx = eglCreateContext(eglDisp, eglConf, EGL_NO_CONTEXT, ctxAttr);
    if(eglCtx == EGL_NO_CONTEXT)
    {
        EGLint error = eglGetError();
        if(error == EGL_BAD_CONFIG)
        {
            // Handle error and recover
            ALOGE("EGL_BAD_CONFIG");
        }
    }
    if(!eglMakeCurrent(eglDisp, eglSurface, eglSurface, eglCtx))
    {
        ALOGE("MakeCurrent failed");
    }
    ALOGV("initialize success!");

    MadvGLRenderer_Android* renderer = new MadvGLRenderer_Android(lutPath, srcSize, srcSize);

    glViewport(0,0, destSize.width, destSize.height);
    glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    renderer->setSourceTextures(false, sourceTexture, sourceTexture, GL_TEXTURE_2D, false);
    renderer->setIsYUVColorSpace(false);
    renderer->setDisplayMode(0);
    renderer->draw(0,0, destSize.width,destSize.height);
    eglSwapBuffers(eglDisp, eglSurface);
//    ALOGV("QD:GL", "Before glReadPixels");
    GLubyte* pixelData = (GLubyte*) malloc(destSize.width * destSize.height * 4);
    glReadPixels(0, 0, destSize.width, destSize.height, GL_RGBA, GL_UNSIGNED_BYTE, pixelData);
//    ALOGV("QD:GL", "After glReadPixels");
    delete renderer;

    eglMakeCurrent(eglDisp, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    eglDestroyContext(eglDisp, eglCtx);
    eglDestroySurface(eglDisp, eglSurface);
    eglTerminate(eglDisp);

    eglDisp = EGL_NO_DISPLAY;
    eglSurface = EGL_NO_SURFACE;
    eglCtx = EGL_NO_CONTEXT;

    return pixelData;
}
