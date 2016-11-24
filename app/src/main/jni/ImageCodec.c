//
// Created by admin on 16/8/24.
//
#include "ImageCodec.h"
#include "Log.h"
#include <stdio.h>
#include <stdlib.h>
#include "libpng/png.h"

#define PNG_BYTES_TO_CHECK 4
#define HAVE_ALPHA 1
#define NO_ALPHA 0

int decodePNG(char *filepath, pic_data *out)
/* 用于解码png图片 */
{
    FILE *pic_fp;
    pic_fp = fopen(filepath, "rb");
    if(pic_fp == NULL) /* 文件打开失败 */
    {
        ALOGE("decodePNG : File open failed");
        return -1;
    }

    /* 初始化各种结构 */
    png_structp png_ptr;
    png_infop  info_ptr;
    char        buf[PNG_BYTES_TO_CHECK];
    int        temp;

    png_ptr  = png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0);
    info_ptr = png_create_info_struct(png_ptr);

    setjmp(png_jmpbuf(png_ptr)); // 这句很重要

    temp = fread(buf,1,PNG_BYTES_TO_CHECK,pic_fp);
    temp = png_sig_cmp((png_const_bytep)((void*)buf), (png_size_t)0, PNG_BYTES_TO_CHECK);

    /*检测是否为png文件*/
    if (temp!=0)
    {
        ALOGE("decodePNG : Not PNG file");
        return 1;
    }

    rewind(pic_fp);
    /*开始读文件*/
    png_init_io(png_ptr, pic_fp);
    // 读文件了
    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, 0);

    int color_type,channels;

    /*获取宽度，高度，位深，颜色类型*/
    channels      = png_get_channels(png_ptr, info_ptr); /*获取通道数*/
    out->channels = channels;
    out->bit_depth = png_get_bit_depth(png_ptr, info_ptr); /* 获取位深 */
    color_type    = png_get_color_type(png_ptr, info_ptr); /*颜色类型*/

    int i,j;
    int size, pos = 0;
    /* row_pointers里边就是rgba数据 */
    png_bytep* row_pointers;
    row_pointers = png_get_rows(png_ptr, info_ptr);
    out->width = png_get_image_width(png_ptr, info_ptr);
    out->height = png_get_image_height(png_ptr, info_ptr);

    size = out->width * out->height; /* 计算图片的总像素点数量 */
//    ALOGE("channels = %d, bits = %d, withAlpha = %d, width = %d, height = %d, colorType = %d\n", channels, out->bit_depth, out->flag, out->width, out->height, color_type);

    if(channels == 4 || color_type == PNG_COLOR_TYPE_RGB_ALPHA)
    {/*如果是RGB+alpha通道，或者RGB+其它字节*/
        size *= (4*sizeof(unsigned char)); /* 每个像素点占4个字节内存 */
        out->flag = HAVE_ALPHA;    /* 标记 */
        out->rgba = (unsigned char**) malloc(size);
        if(out->rgba == NULL)
        {/* 如果分配内存失败 */
            fclose(pic_fp);
            puts("错误(png):无法分配足够的内存供存储数据!");
            ALOGE("decodePNG : Not enough memory");
            return 1;
        }

        unsigned char* pDst = (unsigned char*) out->rgba;
        temp = (4 * out->width);/* 每行有3 * out->width个字节 */
        for(i = 0; i < out->height; i++)
        {
            memcpy(pDst, row_pointers[i], temp);
            pDst += temp;
//            for(j = 0; j < temp; j += 4)
//            {/* 一个字节一个字节的赋值 */
//                out->rgba[0][pos] = row_pointers[i][j]; // red
//                out->rgba[1][pos] = row_pointers[i][j+1]; // green
//                out->rgba[2][pos] = row_pointers[i][j+2];  // blue
//                out->rgba[3][pos] = row_pointers[i][j+3]; // alpha
//                ++pos;
//            }
        }
    }
    else if(channels == 3 || color_type == PNG_COLOR_TYPE_RGB)
    {/* 如果是RGB通道 */
        size *= (3*sizeof(unsigned char)); /* 每个像素点占3个字节内存 */
        out->flag = NO_ALPHA;    /* 标记 */
        out->rgba = (unsigned char**) malloc(size);
        if(out->rgba == NULL)
        {/* 如果分配内存失败 */
            fclose(pic_fp);
            puts("错误(png):无法分配足够的内存供存储数据!");
            ALOGE("decodePNG : File open failed 1");
            return 1;
        }

        unsigned char* pDst = (unsigned char*) out->rgba;
        temp = (3 * out->width);/* 每行有3 * out->width个字节 */
        for(i = 0; i < out->height; i++)
        {
            memcpy(pDst, row_pointers[i], temp);
            pDst += temp;
//            for(j = 0; j < temp; j += 3)
//            {/* 一个字节一个字节的赋值 */
//                out->rgba[0][pos] = row_pointers[i][j]; // red
//                out->rgba[1][pos] = row_pointers[i][j+1]; // green
//                out->rgba[2][pos] = row_pointers[i][j+2];  // blue
//                ++pos;
//            }
        }
//        ALOGE("HERE!#3 channels = %d, bits = %d, withAlpha = %d, width = %d, height = %d, colorType = %d\n", channels, out->bit_depth, out->flag, out->width, out->height, color_type);

    }
    else if (channels == 1 || color_type == PNG_COLOR_TYPE_GRAY)
    {
        size *= (2*sizeof(unsigned char)); /* 每个像素点占2个字节内存 */
        out->flag = NO_ALPHA;
        out->rgba = (unsigned char**) malloc(size);
        if(out->rgba == NULL)
        {/* 如果分配内存失败 */
            fclose(pic_fp);
            puts("错误(png):无法分配足够的内存供存储数据!");
            ALOGE("decodePNG : File open failed 2");
            return 1;
        }

        unsigned char* pDst = (unsigned char*) out->rgba;
        for(i = 0; i < out->height; i++)
        {
            memcpy(pDst, row_pointers[i], out->width * 2);
            pDst += (out->width * 2);
        }
    }
    else
    {
        ALOGE("decodePNG : Not handled PNG format : channel = %d, color_type = %d", channels, color_type);
        return 1;
    }

    /* 撤销数据占用的内存 */
    png_destroy_read_struct(&png_ptr, &info_ptr, 0);
    return 0;
}
