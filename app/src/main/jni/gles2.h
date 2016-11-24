//
// Created by QiuDong on 16/5/28.
//

#ifndef GLES3JNI_GLES2STUB_H
#define GLES3JNI_GLES2STUB_H

#include "TargetConditionals.h"

#ifdef TARGET_OS_ANDROID
#include <GLES2/gl2.h>
#elif TARGET_OS_IOS
#include <OpenGLES/ES2/gl.h>
#endif

#ifndef GL_TEXTURE_EXTERNAL_OES
#define GL_TEXTURE_EXTERNAL_OES 0x8D65
#endif

#endif //GLES3JNI_GLES2STUB_H
