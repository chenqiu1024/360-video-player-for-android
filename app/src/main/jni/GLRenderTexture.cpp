


//
//  GLRenderTexture.cpp
//  Madv360_v1
//
//  Created by FutureBoy on 4/2/16.
//  Copyright © 2016 Cyllenge. All rights reserved.
//

#include "GLRenderTexture.h"
#include "OpenGLHelper.h"
#include "gles3.h"
#include "gles3ext.h"

void GLRenderTexture::releaseGLObjects() {
    if (_texture && _ownTexture)
    {
        glDeleteTextures(1, &_texture);
        _texture = 0;
    }

    if (_framebuffer)
    {
        glDeleteFramebuffers(1, &_framebuffer);
        _framebuffer = 0;
    }

    if (_pboIDs[0] || _pboIDs[1])
    {
        glDeleteBuffers(2, _pboIDs);
        _pboIDs[0] = _pboIDs[1] = 0;
    }
}

GLRenderTexture::GLRenderTexture(GLint texture, GLenum textureType, GLint width, GLint height)
: _framebuffer(0)
, _texture(texture)
, _textureType(textureType)
, _ownTexture(false)
, _width(0)
, _height(0)
, _pboIDs{0,0}
, _pboIndex(0)
{
    const char* extensions = (const char*) glGetString(GL_EXTENSIONS);
    ALOGE("GL Extensions : %s", extensions);
    _isPBOSupported = true;///!!!(NULL != strstr(extensions, "pixel_buffer_object"));
    
    resizeIfNecessary(width, height);
}

GLRenderTexture::GLRenderTexture(GLint width, GLint height)
: _framebuffer(0)
, _texture(0)
, _textureType(GL_TEXTURE_2D)
, _ownTexture(true)
, _width(0)
, _height(0)
, _pboIDs{0,0}
, _pboIndex(0)
{
    const char* extensions = (const char*) glGetString(GL_EXTENSIONS);
    ALOGE("GL Extensions : %s", extensions);
    _isPBOSupported = true;///!!!(NULL != strstr(extensions, "pixel_buffer_object"));

    resizeIfNecessary(width, height);
}

void GLRenderTexture::resizeIfNecessary(GLint width, GLint height) {
    if (_width != width || _height != height)
    {
        _width = width;
        _height = height;

        GLint prevFramebuffer, prevTexture;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &prevFramebuffer);
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTexture);

        if (_texture && _ownTexture)
        {
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textureType, 0, 0);
            glDeleteTextures(1, &_texture);
            _texture = 0;
        }

        glGenFramebuffers(1, &_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);

        if (_ownTexture)
        {
            glGenTextures(1, &_texture);
        }
        glBindTexture(_textureType, _texture);
        if (_ownTexture)
        {
            glTexParameteri(_textureType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_LINEAR//GL_NEAREST
            glTexParameteri(_textureType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR//GL_NEAREST
            glTexParameterf(_textureType, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
            glTexParameterf(_textureType, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            
            //    GLubyte* pixelData = (GLubyte*) malloc(destSize.width * destSize.height * 4);
            glTexImage2D(_textureType, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        }
        
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, _textureType, _texture, 0);
        //    GLenum error = glGetError();
        //    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        //    NSLog(@"error = %x, status = %d", error, status);

        glBindFramebuffer(GL_FRAMEBUFFER, prevFramebuffer);
        glBindTexture(GL_TEXTURE_2D, prevTexture);

        if (_isPBOSupported)
        {
            GLint prevPboBinding;
            glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &prevPboBinding);

            if (_pboIDs[0] > 0)
            {
                glDeleteBuffers(2, _pboIDs);
                _pboIDs[0] = _pboIDs[1] = 0;
            }

            glGenBuffers(2, _pboIDs);
            for (int i=0; i<2; ++i)
            {
                glBindBuffer(GL_PIXEL_PACK_BUFFER, _pboIDs[i]);
                glBufferData(GL_PIXEL_PACK_BUFFER, width * height * 4, NULL, GL_DYNAMIC_READ);//GL_STREAM_READ);//
                CHECK_GL_ERROR();
            }

            glBindBuffer(GL_PIXEL_PACK_BUFFER, prevPboBinding);
        }
    }
}

GLint GLRenderTexture::bytesLength() {
    return _width * _height * 4;//TODO:
}

void GLRenderTexture::blit() {
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &_prevFramebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, _framebuffer);
    CHECK_GL_ERROR();
}

void GLRenderTexture::unblit() {
    glBindFramebuffer(GL_FRAMEBUFFER, _prevFramebuffer);
    CHECK_GL_ERROR();
}

int GLRenderTexture::copyPixelData(uint8_t* data, int offset, int length) {
    if (_width <= 0 || _height <= 0)
    {
        ALOGE("GLRenderTexture::copyPixelData() Error : _width = %d, _height = %d", _width, _height);
    }
    glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, data + offset);
    return _width * _height * 4;
}

GLubyte* GLRenderTexture::copyPixelDataFromPBO(int offset, int length) {
    GLboolean success = GL_FALSE;
    GLubyte* pixels = NULL;
    if (_isPBOSupported)
    {
        GLint prevPboBinding;
        glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING, &prevPboBinding);

        glReadBuffer(GL_COLOR_ATTACHMENT0);//GL_BACK);//
        glBindBuffer(GL_PIXEL_PACK_BUFFER, _pboIDs[_pboIndex]);
        glReadPixels(0, 0, _width, _height, GL_RGBA, GL_UNSIGNED_BYTE, 0);

        _pboIndex = (_pboIndex + 1) % 2;
        glBindBuffer(GL_PIXEL_PACK_BUFFER, _pboIDs[_pboIndex]);
        pixels = (GLubyte*) glMapBufferRange(GL_PIXEL_PACK_BUFFER, 0, bytesLength(), GL_MAP_READ_BIT);
        CHECK_GL_ERROR();
        if (pixels)
        {
            success = glUnmapBufferOES(GL_PIXEL_PACK_BUFFER);
        }
        CHECK_GL_ERROR();
        glBindBuffer(GL_PIXEL_PACK_BUFFER, prevPboBinding);
    }
    ALOGE("GLRenderTexture::copyPixelDataFromPBO : success = %d", success);
    return  (GL_TRUE == success) ? pixels : NULL;
}
