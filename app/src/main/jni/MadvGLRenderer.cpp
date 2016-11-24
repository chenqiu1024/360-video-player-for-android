//
//  MadvGLRenderer.mm
//  Madv360_v1
//
//  Created by QiuDong on 16/2/26.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#include "MadvGLRenderer.h"
#include "gles2.h"
#include "Log.h"
#include "ImageCodec.h"


// Macros for Shader:

//#define EXPAND_AS_PLANE
//#define USE_MSAA

#define SPHERE_RADIUS 128
#define LONGITUDE_SEGMENTS 108
#define LATITUDE_SEGMENTS  216

#ifdef EXPAND_AS_PLANE
#define Z_SHIFT -1024
#else
#define Z_SHIFT  0
#endif

#define STRINGIZE0(...) __VA_ARGS__
#define STRINGIZE(...) #__VA_ARGS__
#define STRINGIZE2(...) STRINGIZE(__VA_ARGS__)

/*
 * Beta = Longitude = 2*PI * s;
 * Alpha = Latitude = PI - PI * t;
 *
 * x = -sinA * sinB;
 * y = cosA;
 * z = sinA * cosB;
 *
 * Alpha = arccos(y);
 * float x1, z1;
 * if (abs(x) <= abs(z))
 *     x1 = x;
 *     z1 = z;
 * else
 *     x1 = z;
 *     z1 = x;
 *
 * Beta = arctan(-x1 / z1);
 * if (z1 < 0)
 *     Beta += PI;
 * else if (x1 > 0)
 *     Beta += 2*PI;
 *
 * if (abs(x) > abs(z))
 *     Beta = 5*PI/2 - Beta;
 *     if (Beta > 2*PI)
 *         Beta -= 2*PI;
 */
const char* VertexShaderSource = STRINGIZE
(
 //#ifdef GLES
 precision highp float;
 //#endif

 attribute vec4 a_position; // 1
 attribute vec2 a_texCoord;
 
 attribute vec2 a_texCoordL;
 attribute vec2 a_texCoordR;
 
 uniform mat4 u_projectionMat;
 uniform mat4 u_cameraMat;
 uniform mat4 u_modelMat;

 uniform mat4 u_screenMatrix;

 varying vec2 v_texCoord;
 
 varying vec2 v_texCoordL;
 varying vec2 v_texCoordR;

 void main(void) { // 4
     v_texCoordL = a_texCoordL;
     v_texCoordR = a_texCoordR;
     v_texCoord = a_texCoord;
     // Modify gl_Position line as follows
     vec4 position = u_projectionMat * u_cameraMat * u_modelMat * a_position;
     gl_Position = u_screenMatrix * position;
 }
 
 );

///Ref: http://blog.csdn.net/opengl_es/article/details/17787495
const char* FragmentShaderSource = STRINGIZE2
(
 STRINGIZE0(#ifdef EXTERNAL) \n
 STRINGIZE0(#extension GL_)STRINGIZE0(OES_EGL_image_external) : require \n
 STRINGIZE0(#endif) \n
 precision highp float;
 
 ///Ref: http://blog.csdn.net/opengl_es/article/details/17787495
 
 varying highp vec2 v_texCoord;
 
 varying highp vec2 v_texCoordL;
 varying highp vec2 v_texCoordR;
 \n
 STRINGIZE0(#ifndef FLAG_YUV_COLORSPACE) \n
     STRINGIZE0(#ifdef EXTERNAL) \n
         uniform samplerExternalOES u_textureL;
         uniform samplerExternalOES u_textureR; \n
     STRINGIZE0(#else) \n
         uniform sampler2D u_textureL;
         uniform sampler2D u_textureR; \n
     STRINGIZE0(#endif) \n
 STRINGIZE0(#else) \n
     STRINGIZE0(#ifdef EXTERNAL) \n
         uniform samplerExternalOES u_textureYL;
         uniform samplerExternalOES u_textureUL;
         uniform samplerExternalOES u_textureVL; \n
         uniform samplerExternalOES u_textureYR;
         uniform samplerExternalOES u_textureUR;
         uniform samplerExternalOES u_textureVR; \n
     STRINGIZE0(#else) \n
         uniform sampler2D u_textureYL;
         uniform sampler2D u_textureUL;
         uniform sampler2D u_textureVL; \n
         uniform sampler2D u_textureYR;
         uniform sampler2D u_textureUR;
         uniform sampler2D u_textureVR; \n
     STRINGIZE0(#endif) \n
 STRINGIZE0(#endif) \n
 uniform highp vec2 u_dstSize;
 uniform highp vec2 u_srcSizeL;
 uniform highp vec2 u_srcSizeR;
\n
 STRINGIZE0(#ifdef FLAG_STITCH_WITH_LUT) \n
 uniform highp sampler2D u_lutTexture; \n
 STRINGIZE0(#endif) \n
 uniform mat4 u_modelMat;
 
// uniform highp vec2 u_touchedTexcoord;

 const float PI = 3.141592653589793;
      \n
 STRINGIZE0(#ifdef FLAG_REFLATTERNING) \n
 uniform mat4 u_cameraMat;
 uniform mat4 u_modelMat; \n

 vec2 transformTexcoord(vec2 texcoord) {
     float beta = 2.0 * PI * texcoord.s;
     float alpha = PI - PI * texcoord.t;
     highp float sinA = sin(alpha);
     highp float sinB = sin(beta);
     vec4 texcoordVector;
     texcoordVector.x = -sinA * sinB;
     texcoordVector.y = cosA;
     texcoordVector.z = sinA * cosB;
     texcoordVector.w = 1.0;

     texcoordVector = u_cameraMat * u_modelMat * texcoordVector;

     alpha = acos(texcoordVector.y);
     float x1, z1;
     if (abs(x) <= abs(z))
     {
         x1 = texcoordVector.x;
         z1 = texcoordVector.z;
     }
     else
     {
         x1 = texcoordVector.z;
         z1 = texcoordVector.x;
     }
     beta = atan(-x1, z1);
//     if (z1 < 0)
//         Beta += PI;
//     else if (x1 > 0)
//         Beta += 2*PI;
//
//     if (abs(x) > abs(z))
//         Beta = 5*PI/2 - Beta;
//     if (Beta > 2*PI)
//         Beta -= 2*PI;
     return vec2(beta / 2.0 / PI, 1.0 - alpha / PI);
 } \n
 STRINGIZE0(#endif) \n

 vec3 YUVTexel2RGB(vec3 yuvTexel) {
     float y = yuvTexel.r;
     float u = yuvTexel.g - 0.5;
     float v = yuvTexel.b - 0.5;
     vec3 rgb;
     rgb.r = y +             1.402 * v;
     rgb.g = y - 0.344 * u - 0.714 * v;
     rgb.b = y + 1.772 * u;
     return rgb;
 }

// highp vec4 gridCoord(highp vec2 texcoord, highp vec2 textureSize) {
//     highp vec2 grid = texcoord * textureSize - vec2(0.5, 0.5);
//     highp vec2 major = floor(grid);
//     highp vec2 minor = grid - major;
//     return vec4(major, minor);
// }
//
// highp vec4 texture2D_AA(sampler2D tex, highp vec2 textureSize, highp vec4 gridCoord) {
//     if (ANTI_ALIAS == 1)
//     {
//         highp vec2 gridLB = gridCoord.xy;
//         highp float p = gridCoord.z;
//         highp float q = gridCoord.w;
//         vec4 LB = texture2D(tex, gridLB / textureSize);
//         vec4 LT = texture2D(tex, (gridLB + vec2(0.0, 1.0)) / textureSize);
//         vec4 RT = texture2D(tex, (gridLB + vec2(1.0, 1.0)) / textureSize);
//         vec4 RB = texture2D(tex, (gridLB + vec2(1.0, 0.0)) / textureSize);
//         return (RT * p + LT * (1.0 - p)) * q + (RB * p + LB * (1.0 - p)) * (1.0 - q);
//     }
//     else
//     {
//         return texture2D(tex, (gridCoord.xy + gridCoord.zw) / textureSize);
//     }
// }
 
 vec2 lutWeights(vec2 dstTexCoord) {
     float weight = 1.0;
     float weight1 = 0.0;

     float MOLT_BAND_WIDTH = u_dstSize.x / 64.0;
     highp float bound0 = (u_dstSize.x * 0.25 - MOLT_BAND_WIDTH / 2.0) / u_dstSize.x;//
     highp float bound1 = (u_dstSize.x * 0.25 + MOLT_BAND_WIDTH / 2.0) / u_dstSize.x;//
     highp float bound2 = (u_dstSize.x * 0.75 - MOLT_BAND_WIDTH / 2.0) / u_dstSize.x;//
     highp float bound3 = (u_dstSize.x * 0.75 + MOLT_BAND_WIDTH / 2.0) / u_dstSize.x;//
     highp float band0Width = (bound1 - bound0) * u_dstSize.x;
     highp float band1Width = (bound3 - bound2) * u_dstSize.x;
     
     if (dstTexCoord.s >= bound1 && dstTexCoord.s < bound2)
     {
         // Use right LUT:
         weight1 = 1.0;
         weight = 0.0;
     }
     else if (dstTexCoord.s < bound0 || dstTexCoord.s >= bound3)
     {
         // Use left LUT:
         weight = 1.0;
         weight1 = 0.0;
     }
     else if (dstTexCoord.s >= bound2)
     {
         // Molt:
         weight = (dstTexCoord.s - bound2) * u_dstSize.x / band1Width;
         weight1 = 1.0 - weight;
     }
     else
     {
         weight1 = (dstTexCoord.s - bound0) * u_dstSize.x / band0Width;
         weight = 1.0 - weight1;
     }
     
     return vec2(weight, weight1);
 }
 
 vec4 color4OfTexCoord(vec2 dstTexCoord) { \n
 STRINGIZE0(#ifdef FLAG_PLAIN_STITCH) \n
     {
         highp vec2 srcTexcoord;
         float H = (u_srcSizeL.y > u_srcSizeR.y ? u_srcSizeL.y : u_srcSizeR.y);
         if (dstTexCoord.s <= u_srcSizeL.x / (u_srcSizeL.x + u_srcSizeR.x))
         {
             srcTexcoord.s = dstTexCoord.s * (u_srcSizeL.x + u_srcSizeR.x) / u_srcSizeL.x;
             srcTexcoord.t = dstTexCoord.t * H / u_srcSizeL.y; \n
             STRINGIZE0(#ifdef FLAG_YUV_COLORSPACE) \n
                 float texelY = texture2D(u_textureYL, srcTexcoord).r;
                 float texelU = texture2D(u_textureUL, srcTexcoord).r;
                 float texelV = texture2D(u_textureVL, srcTexcoord).r;
                 return vec4(YUVTexel2RGB(vec3(texelY, texelU, texelV)), 1.0); \n
             STRINGIZE0(#else) \n
                 return texture2D(u_textureL, srcTexcoord).rgba; \n
             STRINGIZE0(#endif) \n
         }
         else
         {
             srcTexcoord.s = (dstTexCoord.s - u_srcSizeL.x / (u_srcSizeL.x + u_srcSizeR.x)) * (u_srcSizeL.x + u_srcSizeR.x) / u_srcSizeR.x;
             srcTexcoord.t = dstTexCoord.t * H / u_srcSizeR.y; \n
             STRINGIZE0(#ifdef FLAG_YUV_COLORSPACE) \n
                 float texelY = texture2D(u_textureYL, srcTexcoord).r;//R
                 float texelU = texture2D(u_textureUL, srcTexcoord).r;//R
                 float texelV = texture2D(u_textureVL, srcTexcoord).r;//R
                 return vec4(YUVTexel2RGB(vec3(texelY, texelU, texelV)), 1.0); \n
             STRINGIZE0(#else) \n
                 return texture2D(u_textureR, srcTexcoord).rgba; \n
             STRINGIZE0(#endif) \n
         }
     } \n
 STRINGIZE0(#elif defined(FLAG_STITCH_WITH_LUT)) \n
     {
//        \n
//        STRINGIZE0(#ifndef FLAG_YUV_COLORSPACE) \n
//        return texture2D(u_lutTexture, dstTexCoord); \n
//        STRINGIZE0(#else) \n
//        return texture2D(u_lutTexture, dstTexCoord); \n
//        STRINGIZE0(#endif) \n
//         return texture2D(u_lutTexture, dstTexCoord);
//        return vec4(v_texCoordL, v_texCoordR);
//        return vec4(0.25, 1.0, 0.5, 1.0);

         vec4 lTexel;
         vec4 rTexel;
         highp vec4 lutTexel = texture2D(u_lutTexture, dstTexCoord);
         highp vec2 texcoordL = lutTexel.rg;
         highp vec2 texcoordR = lutTexel.ba;
//        highp vec2 texcoordL = v_texCoordL;
//        highp vec2 texcoordR = v_texCoordR;
        \n
        STRINGIZE0(#ifndef FLAG_YUV_COLORSPACE) \n
         {
            lTexel = texture2D(u_textureL, texcoordL);//texture2D_AA_External(u_textureL, u_srcSizeL, gridCoordL);
\n
            STRINGIZE0(#ifdef FLAG_SEPARATE_SOURCE) \n
                rTexel = texture2D(u_textureR, texcoordR);//texture2D_AA_External(u_textureR, u_srcSizeR, gridCoordR);
        \n
            STRINGIZE0(#else) \n
                rTexel = texture2D(u_textureL, texcoordR + vec2(0.5,0.0));//texture2D_AA_External(u_textureR, u_srcSizeR, gridCoordR);
        \n
            STRINGIZE0(#endif) \n
//        rTexel = texture2D(u_textureL, texcoordR + vec2(0.5,0.0));
         } \n
        STRINGIZE0(#else) \n
         {
            float texelYL = texture2D(u_textureYL, texcoordL).r;
            float texelUL = texture2D(u_textureUL, texcoordL).r;
            float texelVL = texture2D(u_textureVL, texcoordL).r;
            lTexel = vec4(YUVTexel2RGB(vec3(texelYL, texelUL, texelVL)), 1.0);

            float texelYR, texelUR, texelVR; \n
            STRINGIZE0(#ifdef FLAG_SEPARATE_SOURCE) \n
            {
               texelYR = texture2D(u_textureYL, texcoordR).r;//R
               texelUR = texture2D(u_textureUL, texcoordR).r;//R
               texelVR = texture2D(u_textureVL, texcoordR).r;//R
            } \n
            STRINGIZE0(#else) \n
            {
                texelYR = texture2D(u_textureYL, texcoordR + vec2(0.5,0.0)).r;
                texelUR = texture2D(u_textureUL, texcoordR + vec2(0.5,0.0)).r;
                texelVR = texture2D(u_textureVL, texcoordR + vec2(0.5,0.0)).r;
            } \n
            STRINGIZE0(#endif) \n
            rTexel = vec4(YUVTexel2RGB(vec3(texelYR, texelUR, texelVR)), 1.0);
         } \n
        STRINGIZE0(#endif) \n
         vec2 weights = lutWeights(dstTexCoord);
         return (lTexel * weights.s + rTexel * weights.t).rgba;
     }
\n
 STRINGIZE0(#else) \n
     { \n
        STRINGIZE0(#ifndef FLAG_YUV_COLORSPACE) \n
             return texture2D(u_textureL, dstTexCoord).rgba; \n
        STRINGIZE0(#else) \n
             float texelY = texture2D(u_textureYL, dstTexCoord).r;
             float texelU = texture2D(u_textureUL, dstTexCoord).r;
             float texelV = texture2D(u_textureVL, dstTexCoord).r;
             return vec4(YUVTexel2RGB(vec3(texelY, texelU, texelV)), 1.0); \n
        STRINGIZE0(#endif) \n
     }
\n
 STRINGIZE0(#endif) \n
 }
 
 void main()
{
    highp vec2 texcoord = v_texCoord;
    gl_FragColor = color4OfTexCoord(texcoord);
//            gl_FragColor = texture2D(u_textureL, texcoord);

//    if (u_touchedTexcoord.s >= 0.0 && u_touchedTexcoord.s <= 1.0 && u_touchedTexcoord.t >= 0.0 && u_touchedTexcoord.t <= 1.0)
//    {
//        if (abs(v_texCoord.s - u_touchedTexcoord.s) <= 0.001 || abs(v_texCoord.t - u_touchedTexcoord.t) <= 0.001)
//        {
//            gl_FragColor = vec4(1.0,1.0,0.0,1.0);
//        }
//    }
}
 
 );

/// 000:Plain 001:Sphere 010:PlainV1 011:SphereV1 10X:LittlePlanet 11X:LittlePlanetV1
#define FLAG_STITCH_WITH_LUT    0x01
#define FLAG_YUV_COLORSPACE     0x02
#define FLAG_SEPARATE_SOURCE    0x04
#define FLAG_PLAIN_STITCH       0x08
#define FLAG_REFLATTERNING      0x10
#ifdef TARGET_OS_ANDROID
#define FLAG_IMAGE_EXTERNAL     0x20
#endif

#ifdef TARGET_OS_ANDROID
#define FLAG_BITS 6
#else
#define FLAG_BITS 5
#endif

MadvGLProgram::MadvGLProgram(const GLchar* const* vertexSources, int vertexSourcesCount, const GLchar* const* fragmentSources, int fragmentSourcesCount)
: GLProgram(vertexSources, vertexSourcesCount, fragmentSources, fragmentSourcesCount)
, _leftTextureSlot(-1)
, _rightTextureSlot(-1)
, _leftTexcoordSlot(-1)
, _rightTexcoordSlot(-1)
, _dstSizeSlot(-1)
, _leftSrcSizeSlot(-1)
, _rightSrcSizeSlot(-1)
//, _scaleSlot(-1)
//, _aspectSlot(-1)
//, _transformSlot(-1)
, _yLeftTextureSlot(-1)
, _uLeftTextureSlot(-1)
, _vLeftTextureSlot(-1)
, _yRightTextureSlot(-1)
, _uRightTextureSlot(-1)
, _vRightTextureSlot(-1)
, _lutTextureSlot(-1)
//, _touchedTexcoordSlot(-1)
{
    _leftTexcoordSlot = glGetAttribLocation(_program, "a_texCoordL");
    _rightTexcoordSlot = glGetAttribLocation(_program, "a_texCoordR");
    
    _leftTextureSlot = glGetUniformLocation(_program, "u_textureL");
    _rightTextureSlot = glGetUniformLocation(_program, "u_textureR");
    
    _dstSizeSlot = glGetUniformLocation(_program, "u_dstSize");
    _leftSrcSizeSlot = glGetUniformLocation(_program, "u_srcSizeL");
    _rightSrcSizeSlot = glGetUniformLocation(_program, "u_srcSizeR");
    
    _yLeftTextureSlot = glGetUniformLocation(_program, "u_textureYL");
    _uLeftTextureSlot = glGetUniformLocation(_program, "u_textureUL");
    _vLeftTextureSlot = glGetUniformLocation(_program, "u_textureVL");
    _yRightTextureSlot = glGetUniformLocation(_program, "u_textureYR");
    _uRightTextureSlot = glGetUniformLocation(_program, "u_textureUR");
    _vRightTextureSlot = glGetUniformLocation(_program, "u_textureVR");

    _lutTextureSlot = glGetUniformLocation(_program, "u_lutTexture");
//    _touchedTexcoordSlot = glGetUniformLocation(_program, "u_touchedTexcoord");
}

static GLfloat* s_gridColors;

MadvGLRenderer::~MadvGLRenderer() {
    //    free(_gridColors);
    _quadMesh = NULL;
//    _lutQuadMesh = NULL;
    _sphereMesh = NULL;
//    _lutSphereMesh = NULL;
    _capsMesh = NULL;

    _currentVAO = NULL;
    _capsVAO = NULL;
    _quadVAO = NULL;
//    _lutQuadVAO = NULL;
    _sphereVAO = NULL;
//    _lutSphereVAO = NULL;

    delete[] _glPrograms;
    //    glDeleteFramebuffers(1, &_framebuffer);
    //        glDeleteRenderbuffers(1, &_depthbuffer);
#ifdef USE_MSAA
    glDeleteFramebuffers(1, &_msaaFramebuffer);
    glDeleteRenderbuffers(1, &_msaaRenderbuffer);
    glDeleteRenderbuffers(1, &_msaaDepthbuffer);
#endif
    printf("MadvGLRenderer $ dealloc\n");
//    if (-1 != _srcTextureL) glDeleteTextures(1, (GLuint*)&_srcTextureL);
//    if (-1 != _srcTextureR) glDeleteTextures(1, (GLuint*)&_srcTextureR);
//    if (-1 != _capsTexture) glDeleteTextures(1, (GLuint*)&_capsTexture);
//    if (-1 != _yuvTextures[0]) glDeleteTextures(3, (GLuint*)_yuvTextures);
//    _srcTextureL = _srcTextureR = -1;
//    for (int i=0; i<3; i++)
//    {
//        _yuvTextures[i] = -1;
//    }
    if (_lutTexture > 0)
    {
        glDeleteTextures(1, (const GLuint*)&_lutTexture);
    }
    
    _glCamera = NULL;
}

MadvGLRenderer::MadvGLRenderer(const char* lutPath, Vec2f leftSrcSize, Vec2f rightSrcSize)
: _srcTextureL(-1)
, _srcTextureR(-1)
, _srcTextureTarget(GL_TEXTURE_2D)
, _drawCaps(false)
, _capsTexture(-1)
, _capsTextureTarget(GL_TEXTURE_2D)
, _separateSourceTexture(false)
, _flipX(false)
, _flipY(false)
, _currentGLProgram(NULL)
, _quadVAO(NULL)
//, _lutQuadVAO(NULL)
, _sphereVAO(NULL)
//, _lutSphereVAO(NULL)
, _currentVAO(NULL)
{
    int maxProgramsCount = 1 << FLAG_BITS;
    _glPrograms = new MadvGLProgramRef[maxProgramsCount];
    for (int i=0; i < maxProgramsCount; ++i)
    {
        _glPrograms[i] = NULL;
    }

    for (int i=0; i<3; ++i)
    {
        _yuvTexturesL[i] = -1;
        _yuvTexturesR[i] = -1;
    }

    _needRenderNewSource = false;
    
#ifdef EXPAND_AS_PLANE
    _mesh = Mesh3DCcreateGrids(2160, 1080, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS);
#else
    _quadMesh = Mesh3D::createGrids(2.f, 2.f, 1, 1, false, false);
//    _lutQuadMesh = Mesh3D::createGrids(2.f, 2.f, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS, false, false);
    _sphereMesh = Mesh3D::createSphereV0(SPHERE_RADIUS, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS, false, false);
//    _lutSphereMesh = Mesh3D::createSphereV0(SPHERE_RADIUS, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS, false, false);
    _capsMesh = Mesh3D::createSphereGaps(SPHERE_RADIUS, M_PI/3, M_PI*2/3, 518.f/230.f);
#endif
    
    _glCamera = new GLCamera;
    
    _yuvTexturesL[0] = _yuvTexturesL[1] = _yuvTexturesL[2] = _yuvTexturesR[0] = _yuvTexturesR[1] = _yuvTexturesR[2] = -1;
    
    pthread_mutex_init(&_mutex, NULL);
}


void MadvGLRenderer::prepareGLCanvas(GLint x, GLint y, GLint width, GLint height) {
    //    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ZERO);
    glViewport(x, y, width, height);
    CHECK_GL_ERROR();
}

void MadvGLRenderer::setRenderSource(void* renderSource) {
    pthread_mutex_lock(&_mutex);
    {
        _renderSource = renderSource;
        _needRenderNewSource = true;
    }
    pthread_mutex_unlock(&_mutex);
}

void MadvGLRenderer::setGyroMatrix(float* matrix, int rank) {
    pthread_mutex_lock(&_mutex);
    {
        memcpy(_gyroMatrix, matrix, sizeof(float) * rank * rank);
        _gyroMatrixRank = rank;
    }
    pthread_mutex_unlock(&_mutex);
}

Vec4f gridCoord(Vec2f texcoord, Vec2f textureSize) {
    Vec2f grid = Vec2f{texcoord.s * textureSize.s, texcoord.t * textureSize.t};
    Vec2f major = Vec2f{floorf(grid.s), floorf(grid.t)};
    Vec2f minor = Vec2f{grid.s - major.s, grid.t - major.t};
    return Vec4f{major.s,major.t, minor.s,minor.t};
}

int ushortTexture(const GLushort* data, Vec2f lutDstSize, int x, int y) {
    if (x < 0) x = 0;
    if (x >= lutDstSize.s) x = lutDstSize.s - 1;
    if (y < 0) y = 0;
    if (y >= lutDstSize.t) y = lutDstSize.t - 1;
    
    return (int) data[(int)(lutDstSize.s * y + x)];
}

float ushortTexture(const GLushort* data, Vec2f lutDstSize, Vec4f gridCoord) {
    int x0 = gridCoord.x;
    int y0 = gridCoord.y;
    float p = gridCoord.z;
    float q = gridCoord.w;
    float LB = ushortTexture(data, lutDstSize, x0, y0);
    float LT = ushortTexture(data, lutDstSize, x0, y0 + 1);
    float RT = ushortTexture(data, lutDstSize, x0 + 1, y0 + 1);
    float RB = ushortTexture(data, lutDstSize, x0 + 1, y0);
    return (RT * p + LT * (1.0 - p)) * q + (RB * p + LB * (1.0 - p)) * (1.0 - q);
}

void convertMesh3DWithLUT(Mesh3DRef mesh, Vec2f lutDstSize, Vec2f leftSrcSize,Vec2f rightSrcSize, int dataSizeInShort, const GLushort* lxIntData, const GLushort* lxMinData, const GLushort* lyIntData, const GLushort* lyMinData, const GLushort* rxIntData, const GLushort* rxMinData, const GLushort* ryIntData, const GLushort* ryMinData) {
    ALOGE("convertMesh3DWithLUT : leftSrcSize = (%f,%f), rightSrcSize = (%f,%f), lutDstSize = (%f,%f)", leftSrcSize.width,leftSrcSize.height, rightSrcSize.width,rightSrcSize.height, lutDstSize.width,lutDstSize.height);
    Vec2f minL = {65536.f, 65536.f}, maxL = {-65536.f, -65536.f};
    Vec2f minR = {65536.f, 65536.f}, maxR = {-65536.f, -65536.f};
    for (int i=0; i<mesh->vertexCount; ++i)
    {
        P4C4T2f& vertex = mesh->vertices[i];
        
        Vec2f dstTexCoord = Vec2f{vertex.s, vertex.t};
        
        /// lutMappedTexcoords() :
        Vec2f lutDstSize1 = lutDstSize;
        lutDstSize1.s--;
        Vec4f dstGridCoord = gridCoord(dstTexCoord, lutDstSize1);
        float lxInt = ushortTexture(lxIntData, lutDstSize, dstGridCoord);
        float lxMin = ushortTexture(lxMinData, lutDstSize, dstGridCoord);
        float lyInt = ushortTexture(lyIntData, lutDstSize, dstGridCoord);
        float lyMin = ushortTexture(lyMinData, lutDstSize, dstGridCoord);
        float rxInt = ushortTexture(rxIntData, lutDstSize, dstGridCoord);
        float rxMin = ushortTexture(rxMinData, lutDstSize, dstGridCoord);
        float ryInt = ushortTexture(ryIntData, lutDstSize, dstGridCoord);
        float ryMin = ushortTexture(ryMinData, lutDstSize, dstGridCoord);
        
        Vec2f texcoordL = Vec2f{(lxInt + lxMin / 1000.f) / leftSrcSize.s, (lyInt + lyMin / 1000.f) / leftSrcSize.t};
        Vec2f texcoordR = Vec2f{(rxInt + rxMin / 1000.f) / rightSrcSize.s, (ryInt + ryMin / 1000.f) / rightSrcSize.t};
        vertex.r = texcoordL.s;
        vertex.g = texcoordL.t;
        vertex.b = texcoordR.s;
        vertex.a = texcoordR.t;

        if (texcoordL.s < minL.s) minL.s = texcoordL.s;
        if (texcoordL.t < minL.t) minL.t = texcoordL.t;
        if (texcoordR.s < minR.s) minR.s = texcoordR.s;
        if (texcoordR.t < minR.t) minR.t = texcoordR.t;
        if (texcoordL.s > maxL.s) maxL.s = texcoordL.s;
        if (texcoordL.t > maxL.t) maxL.t = texcoordL.t;
        if (texcoordR.t > maxR.t) maxR.t = texcoordR.t;
        if (texcoordR.s > maxR.s) maxR.s = texcoordR.s;
    }
    ALOGE("convertMesh3DWithLUT : minL = (%f,%f), maxL = (%f,%f), minR = (%f,%f), maxR = (%f,%f)", minL.s,minL.t, maxL.s,maxL.t, minR.s,minR.t, maxR.s,maxR.t);
}

GLint createLUTTexture(Vec2f lutDstSize, Vec2f leftSrcSize,Vec2f rightSrcSize, int dataSizeInShort, const GLushort* lxIntData, const GLushort* lxMinData, const GLushort* lyIntData, const GLushort* lyMinData, const GLushort* rxIntData, const GLushort* rxMinData, const GLushort* ryIntData, const GLushort* ryMinData) {
    //ALOGE("createLUTTexture : leftSrcSize = (%f,%f), rightSrcSize = (%f,%f), lutDstSize = (%f,%f)", leftSrcSize.width,leftSrcSize.height, rightSrcSize.width,rightSrcSize.height, lutDstSize.width,lutDstSize.height);
    GLint prevTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);
    GLfloat* textureData = (GLfloat*) malloc(sizeof(GLfloat) * 4 * lutDstSize.width * lutDstSize.height);
    GLfloat* pDst = textureData;
    Vec2f lutDstSize1 = lutDstSize;
    lutDstSize1.s--;
    Vec2f minL = {65536.f, 65536.f}, maxL = {-65536.f, -65536.f};
    Vec2f minR = {65536.f, 65536.f}, maxR = {-65536.f, -65536.f};
    for (int iT=0; iT < lutDstSize.height; ++iT)
    {
        for (int iS = 0; iS < lutDstSize.width; ++iS)
        {
            Vec2f dstTexCoord = Vec2f{(2 * iS + 1) * 0.5f / lutDstSize.width, (2 * iT + 1) * 0.5f / lutDstSize.height};
            Vec2f texcoordL, texcoordR;
            if (lxIntData)
            {
                Vec4f dstGridCoord = gridCoord(dstTexCoord, lutDstSize1);
                float lxInt = ushortTexture(lxIntData, lutDstSize, dstGridCoord);
                float lxMin = ushortTexture(lxMinData, lutDstSize, dstGridCoord);
                float lyInt = ushortTexture(lyIntData, lutDstSize, dstGridCoord);
                float lyMin = ushortTexture(lyMinData, lutDstSize, dstGridCoord);
                float rxInt = ushortTexture(rxIntData, lutDstSize, dstGridCoord);
                float rxMin = ushortTexture(rxMinData, lutDstSize, dstGridCoord);
                float ryInt = ushortTexture(ryIntData, lutDstSize, dstGridCoord);
                float ryMin = ushortTexture(ryMinData, lutDstSize, dstGridCoord);

                texcoordL = Vec2f{(lxInt + lxMin / 1000.f) / leftSrcSize.s, (lyInt + lyMin / 1000.f) / leftSrcSize.t};
                texcoordR = Vec2f{(rxInt + rxMin / 1000.f) / rightSrcSize.s, (ryInt + ryMin / 1000.f) / rightSrcSize.t};

                if (texcoordL.s < minL.s) minL.s = texcoordL.s;
                if (texcoordL.t < minL.t) minL.t = texcoordL.t;
                if (texcoordR.s < minR.s) minR.s = texcoordR.s;
                if (texcoordR.t < minR.t) minR.t = texcoordR.t;
                if (texcoordL.s > maxL.s) maxL.s = texcoordL.s;
                if (texcoordL.t > maxL.t) maxL.t = texcoordL.t;
                if (texcoordR.t > maxR.t) maxR.t = texcoordR.t;
                if (texcoordR.s > maxR.s) maxR.s = texcoordR.s;
            }
            *(pDst++) = texcoordL.s;
            *(pDst++) = texcoordL.t;
            *(pDst++) = texcoordR.s;
            *(pDst++) = texcoordR.t;
        }
    }
    ALOGE("createLUTTexture : minL = (%f,%f), maxL = (%f,%f), minR = (%f,%f), maxR = (%f,%f)", minL.s,minL.t, maxL.s,maxL.t, minR.s,minR.t, maxR.s,maxR.t);
    GLuint lutTexture;
    glGenTextures(1, &lutTexture);
    glBindTexture(GL_TEXTURE_2D, lutTexture);
    CHECK_GL_ERROR();
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);//GL_LINEAR//GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR//GL_NEAREST
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);//GL_CLAMP_TO_EDGE);//
    CHECK_GL_ERROR();
    glPixelStorei(GL_PACK_ALIGNMENT, 4);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    CHECK_GL_ERROR();
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F_EXT, lutDstSize.width, lutDstSize.height, 0, GL_RGBA, GL_FLOAT, textureData);
    CHECK_GL_ERROR();
    free(textureData);
    glBindTexture(GL_TEXTURE_2D, prevTexture);
    return lutTexture;
}

void MadvGLRenderer::setLUTData(Vec2f lutDstSize, Vec2f leftSrcSize,Vec2f rightSrcSize, int dataSizeInShort, const GLushort* lxIntData, const GLushort* lxMinData, const GLushort* lyIntData, const GLushort* lyMinData, const GLushort* rxIntData, const GLushort* rxMinData, const GLushort* ryIntData, const GLushort* ryMinData) {
    _lutDstSize = lutDstSize;
    _lutSrcSizeL = leftSrcSize;
    _lutSrcSizeR = rightSrcSize;
    
//    _lutSphereMesh = NULL;
//    _lutQuadMesh = NULL;
//
//    _lutSphereMesh = Mesh3D::createSphereV0(SPHERE_RADIUS, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS, false, false);
//    _lutQuadMesh = Mesh3D::createGrids(2.f, 2.f, LONGITUDE_SEGMENTS, LATITUDE_SEGMENTS, false, false);
//    convertMesh3DWithLUT(_lutSphereMesh, lutDstSize, leftSrcSize, rightSrcSize, dataSizeInShort, lxIntData, lxMinData, lyIntData, lyMinData, rxIntData, rxMinData, ryIntData, ryMinData);
//    convertMesh3DWithLUT(_lutQuadMesh, lutDstSize, leftSrcSize, rightSrcSize, dataSizeInShort, lxIntData, lxMinData, lyIntData, lyMinData, rxIntData, rxMinData, ryIntData, ryMinData);

    if (_lutTexture > 0)
    {
        glDeleteTextures(1, (const GLuint*)&_lutTexture);
    }
    _lutTexture = createLUTTexture(lutDstSize, leftSrcSize, rightSrcSize, dataSizeInShort, lxIntData, lxMinData, lyIntData, lyMinData, rxIntData, rxMinData, ryIntData, ryMinData);
}

void MadvGLRenderer::setSourceTextures(bool separateSourceTexture, GLint srcTextureL, GLint srcTextureR, GLenum srcTextureTarget, bool isYUVColorSpace) {
    _separateSourceTexture = separateSourceTexture;
    _srcTextureL = srcTextureL;
    _srcTextureR = srcTextureR;
    _srcTextureTarget = (0 >= srcTextureTarget) ? GL_TEXTURE_2D : srcTextureTarget;
    _isYUVColorSpace = isYUVColorSpace;
}

void MadvGLRenderer::setSourceTextures(bool separateSourceTexture, GLint* srcTextureL, GLint* srcTextureR, GLenum srcTextureTarget, bool isYUVColorSpace) {
    _separateSourceTexture = separateSourceTexture;
    for (int i=0; i<3; ++i)
    {
        _yuvTexturesL[i] = srcTextureL[i];
        _yuvTexturesR[i] = srcTextureR[i];
    }
    _srcTextureTarget = (0 >= srcTextureTarget) ? GL_TEXTURE_2D : srcTextureTarget;
    _isYUVColorSpace = isYUVColorSpace;
}

void MadvGLRenderer::setCapsTexture(GLint texture, GLenum textureTarget) {
    _capsTexture = texture;
    _capsTextureTarget = (0 >= textureTarget) ? GL_TEXTURE_2D : textureTarget;
}

void MadvGLRenderer::prepareGLPrograms() {
    int flags = 0;
    int sourcesCount = 1;
#ifdef TARGET_OS_ANDROID
    if (_srcTextureTarget != GL_TEXTURE_2D) {
        flags |= FLAG_IMAGE_EXTERNAL;
        sourcesCount++;
    }
#endif
    if (_isYUVColorSpace) {
        flags |= FLAG_YUV_COLORSPACE;
        sourcesCount++;
    }
    if (_currentDisplayMode & PanoramaDisplayModeLUT)
    {
        flags |= FLAG_STITCH_WITH_LUT;
        sourcesCount++;
    }
    if (_separateSourceTexture)
    {
        flags |= FLAG_SEPARATE_SOURCE;
        sourcesCount++;
    }
    if (_currentDisplayMode & PanoramaDisplayModePlainStitch)
    {
        flags |= FLAG_PLAIN_STITCH;
        sourcesCount++;
    }

    _currentGLProgram = _glPrograms[flags];
    if (NULL == _currentGLProgram)
    {
        int iSource = 0;
        const GLchar** fragmentShaderSources = (const GLchar**) malloc(sizeof(GLchar*) * sourcesCount);
#ifdef TARGET_OS_ANDROID
        if (flags & FLAG_IMAGE_EXTERNAL)
        {
            fragmentShaderSources[iSource++] = "#define EXTERNAL\n#define FOR_520\n";///!!!#extension GL_OES_EGL_image_external : require\n";
        }
#endif
        if (flags & FLAG_YUV_COLORSPACE)
        {
            fragmentShaderSources[iSource++] = "#define FLAG_YUV_COLORSPACE\n";
        }
        if (flags & FLAG_STITCH_WITH_LUT)
        {
            fragmentShaderSources[iSource++] = "#define FLAG_STITCH_WITH_LUT\n";
        }
        if (flags & FLAG_SEPARATE_SOURCE)
        {
            fragmentShaderSources[iSource++] = "#define FLAG_SEPARATE_SOURCE\n";
        }
        if (flags & FLAG_PLAIN_STITCH)
        {
            fragmentShaderSources[iSource++] = "#define FLAG_PLAIN_STITCH\n";
        }
        fragmentShaderSources[iSource++] = FragmentShaderSource;

        _currentGLProgram = new MadvGLProgram(&VertexShaderSource, 1, fragmentShaderSources, sourcesCount);
        _glPrograms[flags] = _currentGLProgram;

        free(fragmentShaderSources);
    }
}

void MadvGLRenderer::setGLProgramVariables(GLint x, GLint y, GLint width, GLint height) {
    glUseProgram(_currentGLProgram->getProgram());
    if (_currentDisplayMode & PanoramaDisplayModeLUT)
    {
        glUniform1i(_currentGLProgram->getLUTTextureSlot(), 7);
        glActiveTexture(GL_TEXTURE7);
        glBindTexture(GL_TEXTURE_2D, _lutTexture);
    }
    CHECK_GL_ERROR();
    if (_isYUVColorSpace)
    {
        GLint yTextureSlot = _currentGLProgram->getLeftYTextureSlot();
        GLint uTextureSlot = _currentGLProgram->getLeftUTextureSlot();
        GLint vTextureSlot = _currentGLProgram->getLeftVTextureSlot();
        GLint yuvTextureSlots[3] = {yTextureSlot, uTextureSlot, vTextureSlot};
        for (int i = 0; i < 3; ++i)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, _yuvTexturesL[i]);
            glUniform1i(yuvTextureSlots[i], i);
        }
        
        if (_currentGLProgram->getRightYTextureSlot() >= 0)
        {
            GLint yTextureSlotR = _currentGLProgram->getRightYTextureSlot();
            GLint uTextureSlotR = _currentGLProgram->getRightUTextureSlot();
            GLint vTextureSlotR = _currentGLProgram->getRightVTextureSlot();
            GLint yuvTextureSlotsR[3] = {yTextureSlotR, uTextureSlotR, vTextureSlotR};
            for (int i = 0; i < 3; ++i)
            {
                glActiveTexture(GL_TEXTURE0 + i + 3);
                glBindTexture(GL_TEXTURE_2D, _yuvTexturesR[i]);
                glUniform1i(yuvTextureSlotsR[i], i + 3);
            }
        }
    }
    else
    {
        glUniform1i(_currentGLProgram->getLeftTextureSlot(), 0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(_srcTextureTarget, _srcTextureL);
        CHECK_GL_ERROR();
        if (_currentGLProgram->getRightTextureSlot() >= 0)
        {
            glUniform1i(_currentGLProgram->getRightTextureSlot(), 1);
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(_srcTextureTarget, _srcTextureR);
            CHECK_GL_ERROR();
        }
    }
    
    //#ifdef SPHERE_RENDERING
    kmMat4 projection;
    switch (_currentDisplayMode & PanoramaDisplayModeExclusiveMask)
    {
        case PanoramaDisplayModeSphere:
        {
            _glCamera->setWidth(width);
            _glCamera->setHeight(height);
            _glCamera->getProjectionMatrix(&projection);
            break;
        }
        case PanoramaDisplayModeStereoGraphic:
        {
            _glCamera->setWidth(width);
            _glCamera->setHeight(height);
            _glCamera->setZFar(-65536);
            _glCamera->setZNear(SPHERE_RADIUS);
            _glCamera->getStereoGraphicProjectionMatrix(&projection);
            break;
        }
        case PanoramaDisplayModePlain:
        {
            kmMat4Identity(&projection);
            break;
        }
        case PanoramaDisplayModeLittlePlanet:
        {
            _glCamera->setWidth(width);
            _glCamera->setHeight(height);
            _glCamera->setZFar(65536);
            _glCamera->setZNear(SPHERE_RADIUS);
            _glCamera->getLittlePlanetProjectionMatrix(&projection);
            break;
        }
        default:
            break;
    }
    glUniformMatrix4fv(_currentGLProgram->getProjectionMatrixSlot(), 1, 0, projection.mat);

    CHECK_GL_ERROR();

    kmMat4 modelMatrix;
    kmMat4Identity(&modelMatrix);
#ifdef EXPAND_AS_PLANE
    kmMat4TranslationBy(&modelView, -500, -450, Z_SHIFT);
#else
    kmMat4TranslationBy(&modelMatrix, 0, 0, Z_SHIFT);
#endif
    if (PanoramaDisplayModeLittlePlanet == (_currentDisplayMode & PanoramaDisplayModeExclusiveMask))
    {
        kmMat4 rotationMat;
        kmScalar matData[] = {1,0,0,0, 0,0,-1,0, 0,1,0,0, 0,0,0,1};
        kmMat4Fill(&rotationMat, matData);
        kmMat4Multiply(&modelMatrix, &rotationMat, &modelMatrix);
    }
    glUniformMatrix4fv(_currentGLProgram->getModelMatrixSlot(), 1, 0, modelMatrix.mat);

    kmMat4 cameraMatrix;
    switch (_currentDisplayMode & PanoramaDisplayModeExclusiveMask)
    {
        case PanoramaDisplayModeSphere:
        case PanoramaDisplayModeLittlePlanet:
        case PanoramaDisplayModeStereoGraphic:
            _glCamera->getCameraMatrix(&cameraMatrix);
            if (!GLCamera::checkRotationMatrix(&cameraMatrix))
            {
                ALOGE("Check cameraMatrix failed");
            }
//        bool flipX, flipY;
//        if (PanoramaDisplayModeStereoGraphic == (_currentDisplayMode & PanoramaDisplayModeExclusiveMask))
//        {
//            flipX = !_flipX;
//            flipY = !_flipY;
//        }
//        else if (PanoramaDisplayModeSphere == (_currentDisplayMode & PanoramaDisplayModeExclusiveMask))
//        {
//            flipX = _flipX;
//            flipY = _flipY;
//        }
            if (_flipY)
            {
                kmMat4 flipYMatrix;
                kmScalar flipYMatrixData[] = {1,0,0,0, 0,-1,0,0, 0,0,1,0, 0,0,0,1};
                kmMat4Fill(&flipYMatrix, flipYMatrixData);
                kmMat4Multiply(&cameraMatrix, &cameraMatrix, &flipYMatrix);
                kmMat4Multiply(&cameraMatrix, &flipYMatrix, &cameraMatrix);
            }
            if (_flipX)
            {
                kmMat4 flipXMatrix;
                kmScalar flipXMatrixData[] = {-1,0,0,0, 0,1,0,0, 0,0,1,0, 0,0,0,1};
                kmMat4Fill(&flipXMatrix, flipXMatrixData);
                kmMat4Multiply(&cameraMatrix, &cameraMatrix, &flipXMatrix);
                kmMat4Multiply(&cameraMatrix, &flipXMatrix, &cameraMatrix);
            }
            break;
        default:
            kmMat4Identity(&cameraMatrix);
            break;
    }

    if (_gyroMatrixRank > 0)
    {
        kmMat4 gyroMatrix;
        for (int col=0; col<3; ++col)
        {
            for (int row=0; row<3; ++row)
            {
                gyroMatrix.mat[col * 4 + row] = _gyroMatrix[row * _gyroMatrixRank + col];
            }
        }
        if (_gyroMatrixRank == 3)
        {
            for (int rc=0; rc<3; ++rc)
            {
                gyroMatrix.mat[4 * 3 + rc] = 0;
                gyroMatrix.mat[4 * rc + 3] = 0;
            }
            gyroMatrix.mat[15] = 1;
        }
        if (!GLCamera::checkRotationMatrix(&gyroMatrix))
        {
            ALOGE("Check gyroMatrix failed");
            GLCamera::normalizeRotationMatrix(&gyroMatrix);
        }

//        ALOGV("MadvGLRenderer::setGyroMatrix Src : %f,  %f,  %f;  %f,  %f,  %f;  %f,  %f,  %f;",
//              _gyroMatrix[0],_gyroMatrix[1],_gyroMatrix[2],
//              _gyroMatrix[3],_gyroMatrix[4],_gyroMatrix[5],
//              _gyroMatrix[6],_gyroMatrix[7],_gyroMatrix[8]);
//
//        ALOGV("MadvGLRenderer::setGyroMatrix Dest: %f,  %f,  %f,  %f;  %f,  %f,  %f,  %f;  %f,  %f,  %f,  %f;  %f,  %f,  %f,  %f;",
//              gyroMatrix.mat[0],gyroMatrix.mat[1],gyroMatrix.mat[2],gyroMatrix.mat[3],
//              gyroMatrix.mat[4],gyroMatrix.mat[5],gyroMatrix.mat[6],gyroMatrix.mat[7],
//              gyroMatrix.mat[8],gyroMatrix.mat[9],gyroMatrix.mat[10],gyroMatrix.mat[11],
//              gyroMatrix.mat[12],gyroMatrix.mat[13],gyroMatrix.mat[14],gyroMatrix.mat[15]);

        kmMat4 M, inverseM;
        kmScalar matrixData[] = {1,0,0,0, 0,0,1,0, 0,-1,0,0, 0,0,0,1};//Column-major order
        kmMat4Fill(&M, matrixData);
        kmMat4Inverse(&inverseM, &M);

        kmMat4Multiply(&gyroMatrix, &gyroMatrix, &inverseM);
        kmMat4Multiply(&gyroMatrix, &M, &gyroMatrix);
//        ALOGV("CMatrix : %f,%f,%f,%f ; %f,%f,%f,%f ; %f,%f,%f,%f ; %f,%f,%f,%f ;", gyroMatrix.mat[0],gyroMatrix.mat[1],gyroMatrix.mat[2],gyroMatrix.mat[3]
//        ,gyroMatrix.mat[4],gyroMatrix.mat[5],gyroMatrix.mat[6],gyroMatrix.mat[7],gyroMatrix.mat[8],gyroMatrix.mat[9],gyroMatrix.mat[10]
//        ,gyroMatrix.mat[11],gyroMatrix.mat[12],gyroMatrix.mat[13],gyroMatrix.mat[14],gyroMatrix.mat[15]);
        kmMat4Inverse(&gyroMatrix, &gyroMatrix);
        kmMat4Multiply(&cameraMatrix, &cameraMatrix, &gyroMatrix);
//        _glCamera->setBiasMatrix(&gyroMatrix);
    }

    glUniformMatrix4fv(_currentGLProgram->getCameraMatrixSlot(), 1, 0, cameraMatrix.mat);

    CHECK_GL_ERROR();

    Orientation2D sourceOrientation = OrientationNormal;
    if (_flipX && _flipY)
        sourceOrientation = OrientationRotate180Degree;
    else if (_flipX && !_flipY)
        sourceOrientation = OrientationMirror;
    else if (!_flipX && !_flipY)
        sourceOrientation = OrientationNormal;
    else if (!_flipX && _flipY)
        sourceOrientation = OrientationRotate180DegreeMirror;
    kmMat4 screenMatrix;
    Vec2f viewportOrigin = {(GLfloat)x, (GLfloat)y}, viewportSize = {(GLfloat)width, (GLfloat)height};
    Vec2f boundRectOrigin = {(GLfloat)x, (GLfloat)y}, boundRectSize = {(GLfloat)width, (GLfloat)height};
    transformMatrix4InNormalizedCoordSystem2D(&screenMatrix, viewportOrigin, viewportSize, boundRectOrigin, boundRectSize, sourceOrientation);
    glUniformMatrix4fv(_currentGLProgram->getScreenMatrixSlot(), 1, 0, screenMatrix.mat);

    GLint uni_dstSize = _currentGLProgram->getDstSizeSlot();
    GLint uni_srcSizeL = _currentGLProgram->getLeftSrcSizeSlot();
    GLint uni_srcSizeR = _currentGLProgram->getRightSrcSizeSlot();
    glUniform2f(uni_dstSize, _lutDstSize.width, _lutDstSize.height);
    glUniform2f(uni_srcSizeL, _lutSrcSizeL.width, _lutSrcSizeL.height);
    glUniform2f(uni_srcSizeR, _lutSrcSizeR.width, _lutSrcSizeR.height);
    
//    glUniform2f(_currentGLProgram->getTouchedTexcoordSlot(), _touchedTexcoord.s, _touchedTexcoord.t);
    CHECK_GL_ERROR();
}

void MadvGLRenderer::updateSourceTextureIfNecessary() {
    bool shouldUpdateTexture = false;
    void* currentRenderSource = NULL;
    pthread_mutex_lock(&_mutex);
    {
        if (_renderSource && _needRenderNewSource)
        {
            shouldUpdateTexture = true;
            _needRenderNewSource = false;
            currentRenderSource = _renderSource;
        }
    }
    pthread_mutex_unlock(&_mutex);
    if (shouldUpdateTexture)
    {
        prepareTextureWithRenderSource(currentRenderSource);
    }
}

void MadvGLRenderer::drawPrimitives() {
    _currentVAO->drawMadvSphere(_currentGLProgram->getPositionSlot(), _currentGLProgram->getLeftTexcoordSlot(), _currentGLProgram->getRightTexcoordSlot(), _currentGLProgram->getTexcoordSlot());
    //    _currentVAO->drawVAO(_currentGLProgram->getPositionSlot(), _currentGLProgram->getColorSlot(), _currentGLProgram->getTexcoordSlot());

    if (_drawCaps &&
        (PanoramaDisplayModeSphere == (_currentDisplayMode & PanoramaDisplayModeExclusiveMask)
        || PanoramaDisplayModeStereoGraphic == (_currentDisplayMode & PanoramaDisplayModeExclusiveMask)
        || PanoramaDisplayModeLittlePlanet == (_currentDisplayMode & PanoramaDisplayModeExclusiveMask))
            )
    {
        glUniform1i(_currentGLProgram->getLeftTextureSlot(), 2);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(_capsTextureTarget, _capsTexture);

        if (_currentGLProgram->getRightTextureSlot() >= 0)
        {
            glUniform1i(_currentGLProgram->getRightTextureSlot(), 2);
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(_capsTextureTarget, _capsTexture);
        }

        _capsVAO->drawMadvSphere(_currentGLProgram->getPositionSlot(), _currentGLProgram->getLeftTexcoordSlot(), _currentGLProgram->getRightTexcoordSlot(), _currentGLProgram->getTexcoordSlot());
        CHECK_GL_ERROR();
    }
}

void MadvGLRenderer::draw(GLint x, GLint y, GLint width, GLint height) {
    Mesh3DRef currentMesh;
    GLVAORef* pCurrentVAO;
    switch (_currentDisplayMode & PanoramaDisplayModeExclusiveMask)
    {
        case PanoramaDisplayModeSphere:
        case PanoramaDisplayModeStereoGraphic:
        case PanoramaDisplayModeLittlePlanet:
//            if (_currentDisplayMode & PanoramaDisplayModeLUT)
//            {
//                currentMesh = _lutSphereMesh;
//                pCurrentVAO = &_lutSphereVAO;
//            }
//            else
            {
                currentMesh = _sphereMesh;
                pCurrentVAO = &_sphereVAO;
            }
            break;

        default:
//            if (_currentDisplayMode & PanoramaDisplayModeLUT)
//            {
//                currentMesh = _lutQuadMesh;
//                pCurrentVAO = &_lutQuadVAO;
//            }
//            else
            {
                currentMesh = _quadMesh;
                pCurrentVAO = &_quadVAO;
            }
            break;
    }

    if (NULL == *pCurrentVAO)
    {
        *pCurrentVAO = new GLVAO(currentMesh, GL_STATIC_DRAW);
    }
    _currentVAO = *pCurrentVAO;

    switch (_currentDisplayMode & PanoramaDisplayModeExclusiveMask)
    {
        case PanoramaDisplayModeSphere:
        case PanoramaDisplayModeStereoGraphic:
        case PanoramaDisplayModeLittlePlanet:
            if (NULL == _capsVAO)
            {
                _capsVAO = new GLVAO(_capsMesh, GL_STATIC_DRAW);
            }
            break;
        default:
            break;
    }
    
    updateSourceTextureIfNecessary();
//*///!!!
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);

    prepareGLCanvas(x,y, width,height);
    CHECK_GL_ERROR();
    //    ALOGE("prepareGLCanvas(%d,%d, %d,%d)",x,y,width,height);
    
    prepareGLPrograms();
    CHECK_GL_ERROR();
    setGLProgramVariables(x,y, width,height);

    drawPrimitives();

    CHECK_GL_ERROR();
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

#ifdef USE_MSAA
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER_APPLE, _framebuffer);
    glBindFramebuffer(GL_READ_FRAMEBUFFER_APPLE, _msaaFramebuffer);
    glResolveMultisampleFramebufferAPPLE();
    
    if (_supportDiscardFramebuffer)
    {
        const GLenum discards[]  = {GL_COLOR_ATTACHMENT0,GL_DEPTH_ATTACHMENT};
        glDiscardFramebuffer(GL_READ_FRAMEBUFFER_APPLE,2,discards);
    }
#endif
 //*/
}

void MadvGLRenderer::draw(int currentDisplayMode, int x, int y, int width, int height, bool separateSourceTextures, Vec2f leftSrcSize, Vec2f rightSrcSize, int srcTextureType, int leftSrcTexture, int rightSrcTexture) {
    setIsYUVColorSpace(false);
    setDisplayMode(currentDisplayMode);
    setSourceTextures(separateSourceTextures, leftSrcTexture, rightSrcTexture, srcTextureType, false);
    draw(x, y, width, height);
}
