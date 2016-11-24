//
//  GLFilter.hpp
//  Madv360_v1
//
//  Created by QiuDong on 16/7/15.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#ifndef GLFilter_hpp
#define GLFilter_hpp

#include "../OpenGLHelper.h"
#include "../AutoRef.h"

class GLFilter {
public:

    virtual ~GLFilter() {
        releaseGLObjects();
    }

    GLFilter(const GLchar** vertexShaderSources, int vertexShaderSourceCount, const GLchar** fragmentShaderSources, int fragmentShaderSourceCount)
    : vertexShaderSources(vertexShaderSources)
    , vertexShaderSourceCount(vertexShaderSourceCount)
    , fragmentShaderSources(fragmentShaderSources)
    , fragmentShaderSourceCount(fragmentShaderSourceCount)
    {

    }

    virtual void render(GLVAORef vao, GLint sourceTexture, GLenum sourceTextureTarget);

    virtual void prepareGLProgramSlots(GLint program);

    virtual void render(int x, int y, int width, int height, GLint sourceTexture, GLenum sourceTextureTarget);

    virtual void render(int x, int y, int width, int height, GLint sourceTexture, GLenum sourceTextureTarget, Orientation2D sourceOrientation, Vec2f texcoordOrigin, Vec2f texcoordSize);

    virtual void initGLObjects();

    virtual void releaseGLObjects();
    
protected:

    Vec2f getDestRectSize();

    GLVAORef _vao = NULL;

private:

    GLint _glProgram = -1;
    GLint _glExtProgram = -1;

    GLint _uniScreenMatrix = -1;
    GLint _uniTexcoordOrigin = -1;
    GLint _uniTexcoordSize = -1;

    const GLchar** vertexShaderSources = NULL;
    int vertexShaderSourceCount = 0;
    const GLchar** fragmentShaderSources = NULL;
    int fragmentShaderSourceCount = 0;

    Vec2f boundRectOrigin;
    Vec2f boundRectSize;

    void prepareGLProgram();
    void prepareExtGLProgram();
};

typedef AutoRef<GLFilter> GLFilterRef;

#endif /* GLFilter_hpp */
