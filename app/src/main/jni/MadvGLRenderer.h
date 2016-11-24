//
//  MadvGLRenderer.h
//  Madv360_v1
//
//  Created by QiuDong on 16/2/26.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#include "OpenGLHelper.h"
#include "GLProgram.h"
#include "GLCamera.h"
#include "kazmath.h"
#include <pthread.h>

typedef enum {
    PanoramaDisplayModePlain = 0x00,
    PanoramaDisplayModeSphere = 0x01,
    PanoramaDisplayModeLittlePlanet = 0x02,
    PanoramaDisplayModeStereoGraphic = 0x03,

    PanoramaDisplayModeLUT = 0x10,
    PanoramaDisplayModePlainStitch = 0x20,

    PanoramaDisplayModeExclusiveMask = 0x0f,
} PanoramaDisplayMode;

#pragma mark    GLSL Shaders

class MadvGLProgram : public GLProgram {
public:
    
    MadvGLProgram(const GLchar* const* vertexSources, int vertexSourcesCount, const GLchar* const* fragmentSources, int fragmentSourcesCount);
    
    inline GLint getLeftTextureSlot() {return _leftTextureSlot;}
    inline GLint getRightTextureSlot() {return _rightTextureSlot;}
    
    inline GLint getLeftTexcoordSlot() {return _leftTexcoordSlot;}
    inline GLint getRightTexcoordSlot() {return _rightTexcoordSlot;}
    //    inline GLint getLxLUTSlot() {return _lxLUTSlot;}
    //    inline GLint getLyLUTSlot() {return _lyLUTSlot;}
    //    inline GLint getRxLUTSlot() {return _rxLUTSlot;}
    //    inline GLint getRyLUTSlot() {return _ryLUTSlot;}
    inline GLint getDstSizeSlot() {return _dstSizeSlot;}
    inline GLint getLeftSrcSizeSlot() {return _leftSrcSizeSlot;}
    inline GLint getRightSrcSizeSlot() {return _rightSrcSizeSlot;}
    
    inline GLint getLeftYTextureSlot() {return _yLeftTextureSlot;}
    inline GLint getLeftUTextureSlot() {return _uLeftTextureSlot;}
    inline GLint getLeftVTextureSlot() {return _vLeftTextureSlot;}
    inline GLint getRightYTextureSlot() {return _yRightTextureSlot;}
    inline GLint getRightUTextureSlot() {return _uRightTextureSlot;}
    inline GLint getRightVTextureSlot() {return _vRightTextureSlot;}

    inline GLint getLUTTextureSlot() {return _lutTextureSlot;}
    
//    inline GLint getTouchedTexcoordSlot() {return _touchedTexcoordSlot;}
    
protected:
    
    GLint _leftTextureSlot;
    GLint _rightTextureSlot;
    
    GLint _leftTexcoordSlot;
    GLint _rightTexcoordSlot;
    
    //    GLint _lxLUTSlot;
    //    GLint _lyLUTSlot;
    //    GLint _rxLUTSlot;
    //    GLint _ryLUTSlot;
    GLint _dstSizeSlot;
    GLint _leftSrcSizeSlot;
    GLint _rightSrcSizeSlot;
    
//    GLint _scaleSlot;
//    GLint _aspectSlot;
//    GLint _transformSlot;
    
    GLint _yLeftTextureSlot;
    GLint _uLeftTextureSlot;
    GLint _vLeftTextureSlot;
    GLint _yRightTextureSlot;
    GLint _uRightTextureSlot;
    GLint _vRightTextureSlot;

    GLint _lutTextureSlot;
//    GLint _touchedTexcoordSlot;
};

typedef AutoRef<MadvGLProgram> MadvGLProgramRef;

class MadvGLRenderer {
public:
    
    virtual ~MadvGLRenderer();
    
    MadvGLRenderer(const char* lutPath, Vec2f leftSrcSize, Vec2f rightSrcSize);
    
    virtual void setRenderSource(void* renderSource);
    
    void setGyroMatrix(float* matrix, int rank);
    
    void setLUTData(Vec2f lutDstSize, Vec2f leftSrcSize,Vec2f rightSrcSize, int dataSizeInShort, const GLushort* lxIntData, const GLushort* lxMinData, const GLushort* lyIntData, const GLushort* lyMinData, const GLushort* rxIntData, const GLushort* rxMinData, const GLushort* ryIntData, const GLushort* ryMinData);
    
    void draw(GLint x, GLint y, GLint width, GLint height);

    void draw(int currentDisplayMode, int x, int y, int width, int height, bool separateSourceTextures, Vec2f leftSrcSize, Vec2f rightSrcSize, int srcTextureType, int leftSrcTexture, int rightSrcTexture);

    inline int getDisplayMode() {return _currentDisplayMode;}
    inline void setDisplayMode(int displayMode) {_currentDisplayMode = displayMode;}
    
    inline bool getIsYUVColorSpace() {return _isYUVColorSpace;}
    inline void setIsYUVColorSpace(bool isYUVColorSpace) {_isYUVColorSpace = isYUVColorSpace;}
    
    inline GLfloat getFocalLength() {return _glCamera->getZNear();}
    inline void setFocalLength(GLfloat focalLength) {_glCamera->setZNear(focalLength);}
    
    inline GLfloat getDiffBankRadius() {return _glCamera->getDiffBankRadius();}
    inline void setDiffBankRadius(GLfloat diffBankRadius) {_glCamera->setDiffBankRadius(diffBankRadius);}
    
    inline bool getFlipX() {return _flipX;}
    inline void setFlipX(bool flipX) {_flipX = flipX;}
    
    inline bool getFlipY() {return _flipY;}
    inline void setFlipY(bool flipY) {_flipY = flipY;}
    
    inline GLCameraRef glCamera() {return _glCamera;}
    
    inline void setTouchedTexcoord(Vec2f touchedTexcoord) {_touchedTexcoord = touchedTexcoord;}
    
    virtual void prepareLUT(const char* lutPath, Vec2f leftSrcSize, Vec2f rightSrcSize) = 0;
    
    void setSourceTextures(bool separateSourceTexture, GLint srcTextureL, GLint srcTextureR, GLenum srcTextureTarget, bool isYUVColorSpace);

    void setSourceTextures(bool separateSourceTexture, GLint* srcTextureL, GLint* srcTextureR, GLenum srcTextureTarget, bool isYUVColorSpace);

    void setCapsTexture(GLint texture, GLenum textureTarget);

    inline GLint getLeftSourceTexture() {return _srcTextureL;}
    inline GLint getRightSourceTexture() {return _srcTextureR;}
    inline GLenum getSourceTextureTarget() {return _srcTextureTarget;}
    
    inline void setNeedDrawCaps(bool drawCaps) {_drawCaps = drawCaps;}

protected:
    
    virtual void prepareTextureWithRenderSource(void* renderSource) = 0;
    
    void updateSourceTextureIfNecessary();
    
    void prepareGLPrograms();
    
    void prepareGLCanvas(GLint x, GLint y, GLint width, GLint height);
    
    void setGLProgramVariables(GLint x, GLint y, GLint width, GLint height);
    void drawPrimitives();
    
private:
    
    void* _renderSource;
    bool _needRenderNewSource;
    
    float _gyroMatrix[16];
    int _gyroMatrixRank = 0;
    
#ifdef USE_MSAA
    GLuint _msaaFramebuffer;
    GLuint _msaaRenderbuffer;
    GLuint _msaaDepthbuffer;
    
    bool   _supportDiscardFramebuffer;
#endif
    GLint _srcTextureL;
    GLint _srcTextureR;
    GLenum _srcTextureTarget;
    bool   _separateSourceTexture;

    GLint _capsTexture;
    GLenum _capsTextureTarget;

    Vec2f _lutDstSize;
    Vec2f _lutSrcSizeL, _lutSrcSizeR;
    
    Vec2f _touchedTexcoord;
    
    GLint _yuvTexturesL[3];
    GLint _yuvTexturesR[3];
    
    bool _flipX;
    bool _flipY;

    bool _drawCaps;

    MadvGLProgramRef _currentGLProgram = NULL;
    MadvGLProgramRef* _glPrograms = NULL;
    
#ifdef DRAW_GRID_SPHERE
    GLint _uniGridColors;
    GLint _uniLongitudeFragments;
    GLint _uniLatitudeFragments;
#endif
    
    GLCameraRef _glCamera = NULL;
    
    Mesh3DRef _quadMesh = NULL;
//    Mesh3DRef _lutQuadMesh = NULL;
    Mesh3DRef _sphereMesh = NULL;
//    Mesh3DRef _lutSphereMesh = NULL;
    Mesh3DRef _capsMesh = NULL;

    GLVAORef _quadVAO = NULL;
//    GLVAORef _lutQuadVAO = NULL;
    GLVAORef _sphereVAO = NULL;
//    GLVAORef _lutSphereVAO = NULL;

    GLVAORef _currentVAO = NULL;
    GLVAORef _capsVAO = NULL;

    GLint _lutTexture = -1;

    bool _isYUVColorSpace;
    
    int _currentDisplayMode;
    
    pthread_mutex_t _mutex;
};

typedef AutoRef<MadvGLRenderer> MadvGLRendererRef;
