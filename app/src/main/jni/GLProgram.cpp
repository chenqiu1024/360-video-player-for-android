//
//  GLShaderProgram.m
//  Madv360
//
//  Created by FutureBoy on 11/5/15.
//  Copyright © 2015 Cyllenge. All rights reserved.
//

#include "GLProgram.h"
#include "OpenGLHelper.h"

GLProgram::~GLProgram() {
    glDeleteShader(_vertexShader);
    glDeleteShader(_fragmentShader);
    glDeleteProgram(_program);
}

GLProgram::GLProgram(const GLchar* const* vertexSources, int vertexSourcesCount, const GLchar* const* fragmentSources, int fragmentSourcesCount) {
    _program = compileAndLinkShaderProgramWithShaderPointers(vertexSources, vertexSourcesCount, fragmentSources, fragmentSourcesCount, &_vertexShader, &_fragmentShader);
    _positionSlot = glGetAttribLocation(_program, "a_position");
    _colorSlot = glGetAttribLocation(_program, "a_color");
    _texcoordSlot = glGetAttribLocation(_program, "a_texCoord");
    _textureSlot = glGetUniformLocation(_program, "u_texture");
    _dTexcoordSlot = glGetUniformLocation(_program, "u_dST");
    _projectionMatrixSlot = glGetUniformLocation(_program, "u_projectionMat");
    _cameraMatrixSlot = glGetUniformLocation(_program, "u_cameraMat");
    _modelMatrixSlot = glGetUniformLocation(_program, "u_modelMat");
    _screenMatrixSlot = glGetUniformLocation(_program, "u_screenMatrix");
}
