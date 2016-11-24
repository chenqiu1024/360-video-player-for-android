//
// Created by QiuDong on 16/5/31.
//

#ifndef GLES3JNI_MADVGLRENDERER_ANDROID_H
#define GLES3JNI_MADVGLRENDERER_ANDROID_H

#include "MadvGLRenderer.h"

class MadvGLRenderer_Android : public MadvGLRenderer {
public:

    virtual ~MadvGLRenderer_Android();

    MadvGLRenderer_Android(const char* lutPath, Vec2f leftSrcSize, Vec2f rightSrcSize);

//    static const char* renderThumbnail(const char* thumbnailPath, CGSize destSize);

    void prepareLUT(const char* lutPath, Vec2f leftSrcSize, Vec2f rightSrcSize);

    static GLubyte* renderThumbnail(GLint sourceTexture, Vec2f srcSize, Vec2f destSize, const char* lutPath);

protected:

    void prepareTextureWithRenderSource(void* renderSource);
};


#endif //GLES3JNI_MADVGLRENDERER_ANDROID_H
