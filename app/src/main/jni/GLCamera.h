//
//  GLCamera.hpp
//  Madv360_v1
//
//  Created by QiuDong on 16/4/27.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#ifndef GLCamera_hpp
#define GLCamera_hpp

#include "gles2.h"

#include "AutoRef.h"
#include "kazmath.h"
#include "OpenGLHelper.h"

#define CLIP_WIDTH    6
#define CLIP_Z_NEAR   0
#define CLIP_Z_FAR    65536

#define REFERENCE_VIEWPORT_WIDTH    375
#define REFERENCE_VIEWPORT_HEIGHT   667

class GLCamera {
public:
    
    virtual ~GLCamera();
    
    GLCamera();
    
    void getProjectionMatrix(kmMat4* projectionMatrix);

    void getStereoGraphicProjectionMatrix(kmMat4* projectionMatrix);
    void getLittlePlanetProjectionMatrix(kmMat4* projectionMatrix);

    void getCameraMatrix(kmMat4* cameraMatrix);
    
    inline void setProjectionFrustum(GLint width, GLint height, GLint zNear, GLint zFar) {
        _width = width;
        _height = height;
        _near = zNear;
        _far = zFar;
    }
    
    inline void setDiffEulerAngles(GLfloat diffYawRadius, GLfloat diffPitchRadius, GLfloat diffBankRadius) {
        _diffYawRadius = diffYawRadius;
        _diffPitchRadius = diffPitchRadius;
        _diffBankRadius = diffBankRadius;
    }
    
    inline void setGyroQuaternion(kmQuaternion quaternion) { _currentGyroQuaternion = quaternion;}

    inline void resetGyroQuaternion(kmQuaternion quaternion) {
        _startGyroQuaternion = quaternion;
        _currentGyroQuaternion = quaternion;
    }

    void commitGyroQuaternion();

    inline void setPanPoint(Vec2f point) {_currentPanPoint = point;}

    inline Vec2f getPanPoint() {return _currentPanPoint;}

    inline void resetPanPoint(Vec2f point) {
        _startPanPoint = point;
        _currentPanPoint = point;
    }

    void commitPanPoint();

    void commitMatrix();

    void resetRotation();

    void adustPosition();

    void normalize();

    inline GLint getWidth() {return _width;}
    inline void setWidth(GLint width) {_width = width;}
    
    inline GLint getHeight() {return _height;}
    inline void setHeight(GLint height) {_height = height;}
    
    inline GLfloat getZNear() {return _near;}
    inline void setZNear(GLfloat zNear) {_near = zNear;}
    
    inline GLfloat getZFar() {return _far;}
    inline void setZFar(GLfloat zFar) {_far = zFar;}

    inline GLint getFOVDegree() {return _fovDegree;}
    inline void setFOVDegree(GLint fovDegree) {_fovDegree = fovDegree;}

    inline GLfloat getDiffYawRadius() {return _diffYawRadius;}
    inline void setDiffYawRadius(GLfloat diffYawRadius) {_diffYawRadius = diffYawRadius;}
    
    inline GLfloat getDiffPitchRadius() {return _diffPitchRadius;}
    inline void setDiffPitchRadius(GLfloat diffPitchRadius) {_diffPitchRadius = diffPitchRadius;}
    
    inline GLfloat getDiffBankRadius() {return _diffBankRadius;}
    inline void setDiffBankRadius(GLfloat diffBankRadius) {_diffBankRadius = diffBankRadius;}

    void setBiasMatrix(kmMat4* biasMatrix);
    
    kmVec2 sphereCoordinateOfProjectedPoint(kmVec2 projectedPoint);
    
    void rotationByProjectedPoints(kmVec2 fromPoint, kmVec2 toPoint);

    static void rotateMatrixByGyroQuaternionDifference(kmMat4* mat, kmQuaternion* startQuaternion, kmQuaternion* currentQuaternion);

    static void rotateMatrixByPanDifference(kmMat4* mat, Vec2f startPoint, Vec2f currentPoint, float fovDegree, float width, float height);

    static void normalizeRotationMatrix(kmMat4* rotationMat);

    static bool checkRotationMatrix(kmMat4* matrix);

private:

    GLfloat _diffYawRadius = 0;
    GLfloat _diffPitchRadius = 0;
    GLfloat _diffBankRadius = 0;
    GLfloat _yawRadius = 0;
    GLfloat _pitchRadius = 0;
    GLfloat _bankRadius = 0;

    Vec2f _startPanPoint;
    Vec2f _currentPanPoint;

    kmQuaternion _startGyroQuaternion;
    kmQuaternion _currentGyroQuaternion;

    kmMat4 _startMatrix;
    kmMat4 _currentMatrix;
    kmMat4 _biasMatrix;
    
    GLint _width;
    GLint _height;
    GLint _near;
    GLint _far;
    GLint _fovDegree;
};

typedef AutoRef<GLCamera> GLCameraRef;

#endif /* GLCamera_hpp */
