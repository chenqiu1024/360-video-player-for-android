//
//  GLBilateralFilter.cpp
//  Madv360_v1
//
//  Created by FutureBoy on 7/16/16.
//  Copyright © 2016 Cyllenge. All rights reserved.
//

#include "GLBilateralFilter.h"

static const char* VertexShaderSource = STRINGIZE
(
 attribute vec4 position;
 attribute vec2 inputTextureCoordinate;
 
 const int GAUSSIAN_SAMPLES = 9;
 
 uniform float texelWidthOffset;
 uniform float texelHeightOffset;

        uniform mat4 u_screenMatrix;
        uniform vec2 u_texcoordOrigin;
        uniform vec2 u_texcoordSize;

 varying vec2 textureCoordinate;
 varying vec2 blurCoordinates[GAUSSIAN_SAMPLES];
 
 void main()
 {
     gl_Position = u_screenMatrix * (position / position.w);
//     textureCoordinate = inputTextureCoordinate.xy;
     textureCoordinate = inputTextureCoordinate.xy * u_texcoordSize + u_texcoordOrigin;

     // Calculate the positions for the blur
     int multiplier = 0;
     vec2 blurStep;
     vec2 singleStepOffset = vec2(texelWidthOffset, texelHeightOffset);
     
     for (int i = 0; i < GAUSSIAN_SAMPLES; i++)
     {
         multiplier = (i - ((GAUSSIAN_SAMPLES - 1) / 2));
         // Blur in x (horizontal)
         blurStep = float(multiplier) * singleStepOffset;
         blurCoordinates[i] = textureCoordinate + blurStep;
     }
 }
 );

static const char* FragmentShaderSource = STRINGIZE2
(
        precision highp float;

        \n
        STRINGIZE0(#ifdef EXTERNAL) \n
        STRINGIZE0(#define sourceSampler2D samplerExternalOES) \n
        STRINGIZE0(#else) \n
        STRINGIZE0(#define sourceSampler2D sampler2D) \n
        STRINGIZE0(#endif) \n
        uniform sourceSampler2D inputImageTexture;
 
 const lowp int GAUSSIAN_SAMPLES = 9;
 
 varying highp vec2 textureCoordinate;
 varying highp vec2 blurCoordinates[GAUSSIAN_SAMPLES];
 
 uniform mediump float distanceNormalizationFactor;
 
 void main()
 {
     lowp vec4 centralColor;
     lowp float gaussianWeightTotal;
     lowp vec4 sum;
     lowp vec4 sampleColor;
     lowp float distanceFromCentralColor;
     lowp float gaussianWeight;
     
     centralColor = texture2D(inputImageTexture, blurCoordinates[4]);
     gaussianWeightTotal = 0.18;
     sum = centralColor * 0.18;
     
     sampleColor = texture2D(inputImageTexture, blurCoordinates[0]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     sampleColor = texture2D(inputImageTexture, blurCoordinates[1]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     sampleColor = texture2D(inputImageTexture, blurCoordinates[2]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     sampleColor = texture2D(inputImageTexture, blurCoordinates[3]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     sampleColor = texture2D(inputImageTexture, blurCoordinates[5]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.15 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     sampleColor = texture2D(inputImageTexture, blurCoordinates[6]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.12 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     sampleColor = texture2D(inputImageTexture, blurCoordinates[7]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.09 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     sampleColor = texture2D(inputImageTexture, blurCoordinates[8]);
     distanceFromCentralColor = min(distance(centralColor, sampleColor) * distanceNormalizationFactor, 1.0);
     gaussianWeight = 0.05 * (1.0 - distanceFromCentralColor);
     gaussianWeightTotal += gaussianWeight;
     sum += sampleColor * gaussianWeight;
     
     gl_FragColor = sum / gaussianWeightTotal;
 }
 );

GLBilateralFilter::GLBilateralFilter()
: GLFilter(&VertexShaderSource, 1, &FragmentShaderSource, 1)
{

}

void GLBilateralFilter::render(GLVAORef vao, GLint sourceTexture, GLenum sourceTextureTarget) {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(sourceTextureTarget, sourceTexture);
    glUniform1i(_uniTexture, 0);
    
    glUniform1f(_uniDistanceNormalizationFactor, 8.0);

    Vec2f destSize = getDestRectSize();
    glUniform1f(_uniTexWidthOffset, 1.f / destSize.width);
    glUniform1f(_uniTexHeightOffset, 1.f / destSize.height);
    
    vao->draw(_atrPosition, -1, _atrTexcoord);
}

void GLBilateralFilter::render(int x, int y, int width, int height, GLint sourceTexture, GLenum sourceTextureTarget, Orientation2D sourceOrientation, Vec2f texcoordOrigin, Vec2f texcoordSize) {
    if (!_renderTexture0)
    {
        _renderTexture0 = new GLRenderTexture(height,width);
        _renderTexture1 = new GLRenderTexture(width,height);
    }
    else
    {
        _renderTexture0->resizeIfNecessary(height,width);
        _renderTexture1->resizeIfNecessary(width,height);
    }

    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glViewport(x, y, height, width);
//*
    _renderTexture0->blit();
    GLFilter::render(0, 0, height, width, sourceTexture, sourceTextureTarget, OrientationRotateLeft, texcoordOrigin, texcoordSize);
    _renderTexture0->unblit();

    glViewport(0, 0, width, height);

    _renderTexture1->blit();
    GLFilter::render(0, 0, width, height, _renderTexture0->getTexture(), GL_TEXTURE_2D, OrientationRotateRight, Vec2f{0,0}, Vec2f{1,1});
    _renderTexture1->unblit();

    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

    if (!_plainFilter)
    {
        _plainFilter = new GLPlainFilter;
    }

    GLFilter* filter = _plainFilter;
    filter->render(x, y, width, height, _renderTexture1->getTexture(), GL_TEXTURE_2D, sourceOrientation, Vec2f{0,0}, Vec2f{1,1});
    /*/
    GLFilter::render(x, y, width, height, sourceTexture, sourceTextureTarget, OrientationNormal, texcoordOrigin, Vec2f{texcoordSize.width, texcoordSize.height});
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    //*/
}

void GLBilateralFilter::prepareGLProgramSlots(GLint program) {
    _uniTexture = glGetUniformLocation(program, "inputImageTexture");
    _uniTexWidthOffset = glGetUniformLocation(program, "texelWidthOffset");
    _uniTexHeightOffset = glGetUniformLocation(program, "texelHeightOffset");
    _uniDistanceNormalizationFactor = glGetUniformLocation(program, "distanceNormalizationFactor");
    _atrPosition = glGetAttribLocation(program, "position");
    _atrTexcoord = glGetAttribLocation(program, "inputTextureCoordinate");
}

void GLBilateralFilter::releaseGLObjects() {
    ALOGV("GLBilateralFilter::releaseGLObjects");
    _renderTexture0->releaseGLObjects();
    _renderTexture1->releaseGLObjects();
    _plainFilter->releaseGLObjects();

    _renderTexture0 = NULL;
    _renderTexture1 = NULL;
    _plainFilter = NULL;

    GLFilter::releaseGLObjects();
}
