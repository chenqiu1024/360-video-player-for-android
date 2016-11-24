//
//  GLCamera.cpp
//  Madv360_v1
//
//  Created by QiuDong on 16/4/27.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#include "GLCamera.h"
#include "Log.h"

GLCamera::~GLCamera() {
    
}

GLCamera::GLCamera()
: _width(640)
, _height(960)
, _near(CLIP_Z_NEAR)
, _far(CLIP_Z_FAR)
, _fovDegree(80.f)
, _diffYawRadius(0)
, _diffPitchRadius(0)
, _diffBankRadius(0)
, _yawRadius(0)
, _pitchRadius(0)
, _bankRadius(0)
{
    kmMat4Identity(&_startMatrix);
    kmMat4Identity(&_currentMatrix);
    kmMat4Identity(&_biasMatrix);
    kmQuaternionIdentity(&_startGyroQuaternion);
    kmQuaternionIdentity(&_currentGyroQuaternion);
    _startPanPoint = {0,0};
    _currentPanPoint = {0,0};
}

void GLCamera::getProjectionMatrix(kmMat4* projectionMatrix) {
//    float scale = (float)REFERENCE_VIEWPORT_WIDTH / (float)_width;
    float zNear = 0;//_near * scale;
    float zFar = _far;// * scale;
    float aspect = (float)_width/(float)_height;
    float fovY = kmRadiansToDegrees(atan(tan(kmDegreesToRadians(_fovDegree / 2)) / aspect)) * 2;
//    ALOGE("GLCamera: getProjectionMatrix fovDegree=%d", _fovDegree);
    kmMat4PerspectiveProjection(projectionMatrix, fovY, aspect, zNear, zFar);
//    ALOGE("GLCamera::setProjectionMatrix : (width,height,near,far) = (%d,%d,%d,%d), fovY = %d, scale=%f, zNear=%f, zFar=%f", _width,_height,_near,_far, _fovDegree,scale,zNear,zFar);
}

void GLCamera::getStereoGraphicProjectionMatrix(kmMat4* projectionMatrix) {
    // Here, we use _near as z coordinates of eye (r), and z=_near as near clip plane:
    float e = _near;
    float fovRadians = kmDegreesToRadians(_fovDegree);
    float tanFOVX = tan(fovRadians / 4);
    float tanFOVY = tanFOVX * _height / _width;
//    float w = 2 * r * tanFOVX;
//    float h = _height * w / _width;

    float n = e - 2 * e / (1 + tanFOVX*tanFOVX + tanFOVY*tanFOVY);
    float f = _far;

    float a00 = 1.f / tanFOVX;//2 * r / w;
    float a11 = 1.f / tanFOVY;//2 * r / h;
    float a22 = (n + f - 2 * e) / (n - f);
    float a23 = e + 2 * f * (e - n) / (n - f);
    float a32 = -1;//-1
    float a33 = e;//r
    const float mat[] = {a00,0,0,0, 0,a11,0,0, 0,0,a22,a32, 0,0,a23,a33};
    kmMat4Fill(projectionMatrix, mat);
}

void GLCamera::getLittlePlanetProjectionMatrix(kmMat4* projectionMatrix) {
    // Here, we use _near as z coordinates of eye (r), and z=_near as near clip plane:
    float e = _near;
    float fovRadians = kmDegreesToRadians(_fovDegree);
    float tanFOVX = tan(fovRadians / 2);
    float tanFOVY = tanFOVX * _height / _width;
//    float w = 2 * r * tanFOVX;
//    float h = _height * w / _width;

    float n = e - 2 * e / (1 + tanFOVX*tanFOVX + tanFOVY*tanFOVY);
    float f = _far;

    float a00 = 1.f / tanFOVX;//2 * r / w;
    float a11 = 1.f / tanFOVY;//2 * r / h;
    float a22 = (n + f - 2 * e) / (n - f);
    float a23 = e + 2 * f * (e - n) / (n - f);
    float a32 = -1;//-1
    float a33 = e;//r
    const float mat[] = {a00,0,0,0, 0,a11,0,0, 0,0,a22,a32, 0,0,a23,a33};
    kmMat4Fill(projectionMatrix, mat);
}

void GLCamera::commitGyroQuaternion() {
    _currentMatrix = _startMatrix;
    rotateMatrixByGyroQuaternionDifference(&_currentMatrix, &_startGyroQuaternion, &_currentGyroQuaternion);
    _startMatrix = _currentMatrix;
    _startGyroQuaternion = _currentGyroQuaternion;
}

void GLCamera::commitPanPoint() {
    _currentMatrix = _startMatrix;
    rotateMatrixByPanDifference(&_currentMatrix, _startPanPoint, _currentPanPoint, _fovDegree, _width, _height);
    _startMatrix = _currentMatrix;
    _startPanPoint = _currentPanPoint;
}

void GLCamera::commitMatrix() {
//    getCameraMatrix(NULL);
    _startMatrix = _currentMatrix;
}

void GLCamera::resetRotation() {
    _startGyroQuaternion = _currentGyroQuaternion;
    _startPanPoint = _currentPanPoint;
    kmMat4Identity(&_startMatrix);
    kmMat4Identity(&_currentMatrix);
}

void GLCamera::normalize() {
    normalizeRotationMatrix(&_startMatrix);
    normalizeRotationMatrix(&_currentMatrix);
}

void GLCamera::setBiasMatrix(kmMat4* biasMatrix) {
//    kmMat4 prevBiasMatrixInv;
//    kmMat4Inverse(&prevBiasMatrixInv, &_biasMatrix);
//    kmMat4Multiply(&_startMatrix, &_startMatrix, &prevBiasMatrixInv);
//    kmMat4Multiply(&_currentMatrix, &_currentMatrix, &prevBiasMatrixInv);
    kmMat4Assign(&_biasMatrix, biasMatrix);
//    kmMat4Multiply(&_startMatrix, &_startMatrix, biasMatrix);
//    kmMat4Multiply(&_currentMatrix, &_currentMatrix, biasMatrix);
}

void GLCamera::adustPosition() {
    // (Fc0,Rc0,Uc0) : (Forward,Right,Up) directions of current scene projected on screen:
    kmVec3 Pc = {0,1,0}, Fc0 = {0,0,-1}, Uc0;
    kmVec3Transform(&Uc0, &Pc, &_currentMatrix);
    kmVec3 Rc0;
    kmVec3Cross(&Rc0, &Fc0, &Uc0);
    kmVec3Normalize(&Rc0, &Rc0);
    if (Rc0.x < 0)
    {
        Rc0.x = -Rc0.x;
        Rc0.y = -Rc0.y;
        Rc0.z = -Rc0.z;
    }
    kmVec3Cross(&Uc0, &Rc0, &Fc0);
    kmVec3Normalize(&Uc0, &Uc0);
//    if (Uc0.y < 0)
//    {
//        Uc0.x = -Uc0.x;
//        Uc0.y = -Uc0.y;
//        Uc0.z = -Uc0.z;
//    }

    // (Fc0,Rc0,Uc0) should be mapped to (Fc1,Rc1,Uc1), and Fc1 = Fc0:
    kmVec3 Rc1 = {1,0,0}, Uc1 = {0,1,0};
    kmMat4 inverseGyroMat;
    kmMat4RotationQuaternion(&inverseGyroMat, &_currentGyroQuaternion);
    kmVec3Transform(&Uc1, &Uc1, &inverseGyroMat);
    kmVec3Cross(&Rc1, &Fc0, &Uc1);
    kmVec3Normalize(&Rc1, &Rc1);
    if (Rc1.x < 0)
    {
        Rc1.x = -Rc1.x;
        Rc1.y = -Rc1.y;
        Rc1.z = -Rc1.z;
    }
    kmVec3Cross(&Uc1, &Rc1, &Fc0);
    kmVec3Normalize(&Uc1, &Uc1);
//    if (Uc1.y < 0)
//    {
//        Uc1.x = -Uc1.x;
//        Uc1.y = -Uc1.y;
//        Uc1.z = -Uc1.z;
//    }

    float M0[] = {
            Fc0.x, Fc0.y, Fc0.z, 0,
            Rc0.x, Rc0.y, Rc0.z, 0,
            Uc0.x, Uc0.y, Uc0.z, 0,
            0, 0, 0, 1,
    };
    float M1[] = {
            Fc0.x, Fc0.y, Fc0.z, 0,
            Rc1.x, Rc1.y, Rc1.z, 0,
            Uc1.x, Uc1.y, Uc1.z, 0,
            0, 0, 0, 1,
    };
    kmMat4 invMat0, mat1;
    kmMat4Fill(&invMat0, M0);
    kmMat4Inverse(&invMat0, &invMat0);
    kmMat4Fill(&mat1, M1);

    kmMat4Multiply(&_currentMatrix, &invMat0, &_currentMatrix);
    kmMat4Multiply(&_currentMatrix, &mat1, &_currentMatrix);

    _startMatrix = _currentMatrix;
}

void GLCamera::rotateMatrixByGyroQuaternionDifference(kmMat4* mat, kmQuaternion* startQuaternion, kmQuaternion* currentQuaternion) {
    if (currentQuaternion->x != startQuaternion->x ||
        currentQuaternion->y != startQuaternion->y ||
        currentQuaternion->z != startQuaternion->z ||
        currentQuaternion->w != startQuaternion->w)
    {
        kmMat4 inverseGyroMatrix;
        kmMat4RotationQuaternion(&inverseGyroMatrix, currentQuaternion);
        kmMat4 inverseStartGyroMatrix;
        kmMat4RotationQuaternion(&inverseStartGyroMatrix, startQuaternion);
        kmMat4Inverse(&inverseStartGyroMatrix, &inverseStartGyroMatrix);
        kmMat4Multiply(&inverseGyroMatrix, &inverseGyroMatrix, &inverseStartGyroMatrix);

////        kmMat4 gyroMatrix;
//        float det = kmMat4Determinant(&inverseGyroMatrix);
//        if (abs(det - 1.0f) <= 0.001)
//        {
////            kmMat4Inverse(&gyroMatrix, &inverseGyroMatrix);// Rotation matrix of phone rotated by gyro
//        }
//        else
//        {
//            kmMat4Identity(&inverseGyroMatrix);//TODO: Regulation
////            kmMat4Identity(&gyroMatrix);
//        }

        kmMat4Multiply(mat, &inverseGyroMatrix, mat);
    }
}

void GLCamera::rotateMatrixByPanDifference(kmMat4* mat, Vec2f startPoint, Vec2f currentPoint, float fovDegree, float width, float height) {
    if (startPoint.x != currentPoint.x || startPoint.y != currentPoint.y)
    {
        kmVec3 yawAxis = {0,1,0}, bankAxis = {0,0,1}, pitchAxis;
        kmVec3Transform(&yawAxis, &yawAxis, mat);
        kmVec3Normalize(&yawAxis, &yawAxis);
        kmVec3Cross(&pitchAxis, &yawAxis, &bankAxis);
        kmVec3Normalize(&pitchAxis, &pitchAxis);

        kmVec3 panVector = {currentPoint.x - startPoint.x, -currentPoint.y + startPoint.y, 0};
        kmVec3 pitchComponentVector;
        kmVec3Cross(&pitchComponentVector, &pitchAxis, &panVector);
        float pitchComponent = pitchComponentVector.z;
        float yawComponent = -kmVec3Dot(&pitchAxis, &panVector);

        kmMat4RotationAxisAngleBy(mat, &yawAxis, yawComponent * kmDegreesToRadians(fovDegree) / width);
//        if (pitchAxis.x > 0)
        {
            kmMat4RotationAxisAngleBy(mat, &pitchAxis, pitchComponent * M_PI / height);
        }
//        else
//        {
//            kmMat4RotationAxisAngleBy(&_currentMatrix, &pitchAxis, -pitchComponent * M_PI / _height);
//        }
    }
}

void GLCamera::normalizeRotationMatrix(kmMat4* rotationMat) {
    kmVec3 b = {0,0,0};
    kmVec3Transform(&b, &b, rotationMat);

    kmVec3 x = {1,0,0}, y = {0,1,0}, z = {0,0,1};
    kmVec3Transform(&x, &x, rotationMat);
    kmVec3Transform(&y, &y, rotationMat);
    kmVec3Transform(&z, &z, rotationMat);
//    kmVec3 x1 = x, y1 = y, z1 = z;

    kmVec3Subtract(&x, &x, &b);
    kmVec3Subtract(&y, &y, &b);
    kmVec3Subtract(&z, &z, &b);
//    kmVec3 x2 = x, y2 = y, z2 = z;

    // Schmidt orthogonalization
    kmVec3Normalize(&x, &x);
    float xy = kmVec3Dot(&x, &y);
    y.x -= xy * x.x;
    y.y -= xy * x.y;
    y.z -= xy * x.z;
    kmVec3Normalize(&y, &y);
//    float xz = kmVec3Dot(&x, &z);
//    float yz = kmVec3Dot(&y, &z);
//    z.x -= (xz * x.x + yz * y.x);
//    z.y -= (xz * x.y + yz * y.y);
//    z.z -= (xz * x.z + yz * y.z);
//    kmVec3Normalize(&z, &z);
    kmVec3Cross(&z, &x, &y);
//    kmVec3 x3 = x, y3 = y, z3 = z;

    float matData[] = {
            x.x, x.y, x.z, 0.f,
            y.x, y.y, y.z, 0.f,
            z.x, z.y, z.z, 0.f,
            0.f, 0.f, 0.f, 1.f,
    };
    kmMat4Fill(rotationMat, matData);

//    float det = kmMat4Determinant(rotationMat);
//    if (abs(abs(det) - 1.f) > 0.01)
//    {
//        kmVec3 x1y1, x1z1, z1y1, x2y2, x2z2, z2y2, x3y3, x3z3, z3y3;
//        kmVec3Cross(&x1y1, &x1, &y1); kmVec3Cross(&x1z1, &x1, &z1); kmVec3Cross(&z1y1, &z1, &y1);
//        kmVec3Cross(&x2y2, &x2, &y2); kmVec3Cross(&x2z2, &x2, &z2); kmVec3Cross(&z2y2, &z2, &y2);
//        kmVec3Cross(&x3y3, &x3, &y3); kmVec3Cross(&x3z3, &x3, &z3); kmVec3Cross(&z3y3, &z3, &y3);
//        ALOGE("Matrix : det = %f\n[%.5f, %.5f, %.5f, %.5f;\n %.5f, %.5f, %.5f, %.5f;\n %.5f, %.5f, %.5f, %.5f;\n %.5f, %.5f, %.5f, %.5f]\n\n"
//                      "b = {%.5f, %.5f, %.5f}(%.5f)\n"
//                      "After Transform : x1 = {%.5f, %.5f, %.5f}(%.5f), y1 = {%.5f, %.5f, %.5f}(%.5f), z1 = {%.5f, %.5f, %.5f}(%.5f)\n"
//                      "x1 * y1 = %.5f, x1 * z1 = %.5f, z1 * y1 = %.5f\n"
//                      "After Translate : x2 = {%.5f, %.5f, %.5f}(%.5f), y2 = {%.5f, %.5f, %.5f}(%.5f), z2 = {%.5f, %.5f, %.5f}(%.5f)\n"
//                      "x2 * y2 = %.5f, x2 * z2 = %.5f, z2 * y2 = %.5f\n"
//                      "After Normalize : x3 = {%.5f, %.5f, %.5f}(%.5f), y3 = {%.5f, %.5f, %.5f}(%.5f), z3 = {%.5f, %.5f, %.5f}(%.5f)"
//                      "x3 * y3 = %.5f, x3 * z3 = %.5f, z3 * y3 = %.5f\n\n\n\n",
//              det, x.x,y.x,z.x,0.f, x.y,y.y,z.y,0.f, x.z,y.z,z.z,0.f, 0.f,0.f,0.f,1.f,
//              b.x,b.y,b.z,kmVec3Length(&b),
//              x1.x,x1.y,x1.z,kmVec3Length(&x1), y1.x,y1.y,y1.z,kmVec3Length(&y1), z1.x,z1.y,z1.z,kmVec3Length(&z1),
//              kmVec3Length(&x1y1), kmVec3Length(&x1z1), kmVec3Length(&z1y1),
//              x2.x,x2.y,x2.z,kmVec3Length(&x2), y2.x,y2.y,y2.z,kmVec3Length(&y2), z2.x,z2.y,z2.z,kmVec3Length(&z2),
//              kmVec3Length(&x2y2), kmVec3Length(&x2z2), kmVec3Length(&z2y2),
//              x3.x,x3.y,x3.z,kmVec3Length(&x3), y3.x,y3.y,y3.z,kmVec3Length(&y3), z3.x,z3.y,z3.z,kmVec3Length(&z3),
//              kmVec3Length(&x3y3), kmVec3Length(&x3z3), kmVec3Length(&z3y3)
//        );
//    }
}

bool GLCamera::checkRotationMatrix(kmMat4* matrix) {
    float det = kmMat4Determinant(matrix);
    if (fabsf(fabsf(det) - 1.f) <= 0.01f) return true;

    ALOGE("GLCamera::checkRotationMatrix : Determinant=%.5f\n, diff = %f", det, fabsf(fabsf(det) - 1.f));
    kmVec3 bias = {0,0,0};
    kmVec3Transform(&bias, &bias, matrix);
    ALOGE("GLCamera::checkRotationMatrix : bias={%.5f, %.5f, %.5f}\n", bias.x, bias.y, bias.z);

    kmVec3 x = {1,0,0}, y = {0,1,0}, z = {0,0,1};
    kmVec3Transform(&x, &x, matrix);
    kmVec3Transform(&y, &y, matrix);
    kmVec3Transform(&z, &z, matrix);
    kmVec3Subtract(&x, &x, &bias);
    kmVec3Subtract(&y, &y, &bias);
    kmVec3Subtract(&z, &z, &bias);
    float lx = kmVec3Length(&x);
    float ly = kmVec3Length(&y);
    float lz = kmVec3Length(&z);
    ALOGE("GLCamera::checkRotationMatrix : After transform and translate to origin:\n"
            "x' = (%.5f){%.5f, %.5f, %.5f}, y' = (%.5f){%.5f, %.5f, %.5f}, z' = (%.5f){%.5f, %.5f, %.5f}\n",
            lx, x.x, x.y, x.z,
            ly, y.x, y.y, y.z,
            lz, z.x, z.y, z.z);

    kmVec3 xy, xz, zy;
    kmVec3Cross(&xy, &x, &y);
    kmVec3Cross(&xz, &x, &z);
    kmVec3Cross(&zy, &z, &y);
    float sinA = kmVec3Length(&xy) / lx / ly;
    float sinB = kmVec3Length(&xz) / lx / lz;
    float sinC = kmVec3Length(&zy) / lz / ly;
    ALOGE("GLCamera::checkRotationMatrix : SinA=%.5f, A=%.5f\n"
            "GLCamera::checkRotationMatrix : SinB=%.5f, B=%.5f\n"
            "GLCamera::checkRotationMatrix : SinC=%.5f, C=%.5f\n",
            sinA, kmRadiansToDegrees(asin(sinA)), sinB, kmRadiansToDegrees(asin(sinB)), sinC, kmRadiansToDegrees(asin(sinC)));
    return false;
}

void GLCamera::getCameraMatrix(kmMat4* cameraMatrix) {
    _currentMatrix = _startMatrix;

    rotateMatrixByGyroQuaternionDifference(&_currentMatrix, &_startGyroQuaternion, &_currentGyroQuaternion);
    rotateMatrixByPanDifference(&_currentMatrix, _startPanPoint, _currentPanPoint, _fovDegree, _width, _height);

    normalizeRotationMatrix(&_currentMatrix);///!!!

    if (NULL != cameraMatrix)
    {
        kmMat4Assign(cameraMatrix, &_currentMatrix);
    }
}

kmVec2 GLCamera::sphereCoordinateOfProjectedPoint(kmVec2 projectedPoint) {
    kmVec3 v0 = {
        (projectedPoint.x - _width / 2.f) * CLIP_WIDTH / _width,
        (_height / 2.f - projectedPoint.y) * CLIP_WIDTH / _width,
        -CLIP_Z_NEAR};
    
    kmMat4 matrix = _currentMatrix;
    
    kmVec3 v1;
    kmVec3InverseTransform(&v1, &v0, &matrix);

    GLfloat longitudeRadius = atanf(-v1.x / v1.z);
    if (v1.z < 0)
        longitudeRadius += M_PI;
    else if (v1.x > 0)
        longitudeRadius += (2.f * M_PI);
    
    GLfloat latitudeRadius = atanf(sqrtf(v1.x * v1.x + v1.z * v1.z) / v1.y);
    if (v1.y < 0)
        latitudeRadius += M_PI;
    
    kmVec2 ret = {longitudeRadius, latitudeRadius};
    return ret;
}

void GLCamera::rotationByProjectedPoints(kmVec2 fromPoint, kmVec2 toPoint) {
    kmVec2 fromSphereCoord = sphereCoordinateOfProjectedPoint(fromPoint);
    kmVec2 toSphereCoord = sphereCoordinateOfProjectedPoint(toPoint);
    
    _diffYawRadius = (fromSphereCoord.x - toSphereCoord.x);
    _diffPitchRadius = (fromSphereCoord.y - toSphereCoord.y);
    if (fabsf(_diffYawRadius) > fabsf(_diffPitchRadius))
    {
        _diffPitchRadius = 0;
    }
    else
    {
        _diffYawRadius = 0;
    }

    _yawRadius += _diffYawRadius;
    _pitchRadius += _diffPitchRadius;
}

