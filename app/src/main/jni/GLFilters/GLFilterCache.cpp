//
//  GLFilterCache.cpp
//  Madv360_v1
//
//  Created by QiuDong on 16/7/15.
//  Copyright © 2016年 Cyllenge. All rights reserved.
//

#include "mat4.h"
#include "../ImageCodec.h"
#include "GLFilterCache.h"
#include "GLSimpleBeautyFilter.h"
#include "GLInverseColorFilter.h"
#include "GLBilateralFilter.h"
#include "GLKuwaharaFilter.h"
#include "GLColorMatrixFilter.h"
#include "GLColorLookupFilter.h"
#include "GLPlainFilter.h"
#include "GLTestFilter.h"

using namespace std;

GLFilterCache::~GLFilterCache() {
    releaseGLObjects();
    if (NULL != _resourceDirectory)
    {
        free((void*)_resourceDirectory);
    }
}

GLFilterCache::GLFilterCache(const char* resourceDirectory) {
    if (NULL != resourceDirectory)
    {
        _resourceDirectory = (char*) malloc(strlen(resourceDirectory) + 1);
        strcpy((char*)_resourceDirectory, resourceDirectory);
    }
}

void GLFilterCache::releaseGLObjects() {
    if (_filtersOfID.size() == 0)
        return;

    for (map<int,GLFilterRef>::iterator iter = _filtersOfID.begin();
            iter != _filtersOfID.end();
            iter++)
    {
        GLFilterRef filter = iter->second;
        if (filter != NULL)
        {
            filter->releaseGLObjects();
        }
    }
    _filtersOfID.clear();
}

void GLFilterCache::render(int filterID, GLVAORef vao, GLint sourceTexture, GLenum sourceTextureTarget) {
    GLFilterRef filter = obtainFilter(filterID);
    if (filter)
    {
        filter->render(vao, sourceTexture, sourceTextureTarget);
    }
}

void GLFilterCache::render(int filterID, int x, int y, int width, int height, GLint sourceTexture, GLenum sourceTextureTarget) {
    GLFilterRef filter = obtainFilter(filterID);
    if (filter)
    {
        filter->render(x,y, width,height, sourceTexture, sourceTextureTarget);
    }
}

void GLFilterCache::render(int filterID, int x, int y, int width, int height, GLint sourceTexture, GLenum sourceTextureTarget, Orientation2D sourceOrientation, Vec2f texcoordOrigin, Vec2f texcoordSize) {
    GLFilterRef filter = obtainFilter(filterID);
    if (filter)
    {
        filter->render(x,y,width,height, sourceTexture, sourceTextureTarget, sourceOrientation, texcoordOrigin, texcoordSize);
    }
    CHECK_GL_ERROR();
}

GLFilterRef GLFilterCache::obtainFilter(int filterID) {
    GLFilterRef filter;
    map<int,GLFilterRef>::iterator found = _filtersOfID.find(filterID);
    if (found == _filtersOfID.end())
    {
        filter = createFilter(filterID);
        _filtersOfID.insert(make_pair(filterID, filter));
    }
    else
    {
        filter = found->second;
    }
    return filter;
}

GLFilterRef GLFilterCache::createFilter(int filterID) {
    switch (filterID)
    {
        case GLFilterTestID:
            return new GLTestFilter;
        case GLFilterSimpleBeautyID:
            return new GLSimpleBeautyFilter;
        case GLFilterInverseColorID:
            return new GLInverseColorFilter;
        case GLFilterBilateralID:
            return new GLBilateralFilter;
        case GLFilterKuwaharaID:
            return new GLKuwaharaFilter;
        case GLFilterSepiaToneID:
        {
            kmMat4 colorMatrix;
            float matData[] = {0.3588, 0.7044, 0.1368, 0.0,
                               0.2990, 0.5870, 0.1140, 0.0,
                               0.2392, 0.4696, 0.0912 ,0.0,
                               0,0,0,1.0};
            kmMat4Fill(&colorMatrix, matData);
            return new GLColorMatrixFilter(colorMatrix, 1.0f);
        }
        case GLFilterAmatorkaID:
        case GLFilterMissEtikateID:
        {
            const char* lookupImageName = "lookup_amatorka.png";//
            switch (filterID)
            {
                case GLFilterAmatorkaID:
                    lookupImageName = "lookup_amatorka.png";
                    break;
                case GLFilterMissEtikateID:
                    lookupImageName = "lookup_miss_etikate.png";
                    break;
            }
            char* lookupImagePath = (char*) malloc(strlen(_resourceDirectory) + strlen(lookupImageName) + 1);
            sprintf(lookupImagePath, "%s/%s", _resourceDirectory, lookupImageName);
            pic_data lookupImage;
            decodePNG(lookupImagePath, &lookupImage);

            GLint format = GL_RGBA;
            switch (lookupImage.channels)
            {
                case 4:
                    format = GL_RGBA;
                    break;
                case 3:
                    format = GL_RGB;
                    break;
                case 1:
                    format = GL_LUMINANCE;
                    break;
            }
            GLenum type = GL_UNSIGNED_BYTE;
            switch (lookupImage.bit_depth)
            {
                case 16:
                    format = GL_UNSIGNED_SHORT;
                    break;
            }
            GLuint lookupTexture;
            GLint prevTextureBinding;
            glGetIntegerv(GL_TEXTURE_BINDING_2D, &prevTextureBinding);
            glGenTextures(1, &lookupTexture);
            glBindTexture(GL_TEXTURE_2D, lookupTexture);
            glTexImage2D(GL_TEXTURE_2D, 0, format, lookupImage.width, lookupImage.height, 0, format, type, lookupImage.rgba);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);//GL_LINEAR//GL_NEAREST
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);//GL_LINEAR//GL_NEAREST
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);//GL_CLAMP_TO_EDGE);//
            glPixelStorei(GL_PACK_ALIGNMENT, 1);
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
            glBindTexture(GL_TEXTURE_2D, prevTextureBinding);

            GLColorLookupFilter* filter = new GLColorLookupFilter;
            filter->setIntensity(1.0f);
            filter->setLookupTexture(lookupTexture);
            free(lookupImagePath);

            return filter;
        }
        default:
            return new GLPlainFilter;
    }
}
