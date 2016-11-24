//
//  GLBilateralFilter.hpp
//  Madv360_v1
//
//  Created by FutureBoy on 7/16/16.
//  Copyright © 2016 Cyllenge. All rights reserved.
//

#ifndef GLBilateralFilter_hpp
#define GLBilateralFilter_hpp

#include "GLFilter.h"
#include "../GLRenderTexture.h"
#include "GLFilterCache.h"
#include "GLPlainFilter.h"

class GLBilateralFilter : public GLFilter {
public:
    
    GLBilateralFilter();

    void render(GLVAORef vao, GLint sourceTexture, GLenum sourceTextureTarget);
    
    void render(int x, int y, int width, int height, GLint sourceTexture, GLenum sourceTextureTarget, Orientation2D sourceOrientation, Vec2f texcoordOrigin, Vec2f texcoordSize);

    void prepareGLProgramSlots(GLint program);
    
    void releaseGLObjects();
    
protected:
    
    GLint _uniTexture;
    GLint _uniTexWidthOffset;
    GLint _uniTexHeightOffset;
    GLint _uniDistanceNormalizationFactor;
    
    GLint _atrPosition;
    GLint _atrTexcoord;
    
    GLRenderTextureRef _renderTexture0 = NULL;
    GLRenderTextureRef _renderTexture1 = NULL;

    GLPlainFilterRef _plainFilter = NULL;
};

#endif /* GLBilateralFilter_hpp */
