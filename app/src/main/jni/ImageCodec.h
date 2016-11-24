//
// Created by admin on 16/8/24.
//

#ifndef APP_ANDROID_IMAGECODEC_H
#define APP_ANDROID_IMAGECODEC_H

/******************************图片数据*********************************/
typedef struct _pic_data pic_data;
struct _pic_data
{
    int width, height; /* 尺寸 */
    int bit_depth;  /* 位深 */
    int channels; /* 多少个颜色通道 */
    int flag;   /* 一个标志，表示是否有alpha通道 */

    unsigned char **rgba; /* 图片数组 */
};
/**********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

int decodePNG(char *filepath, pic_data *out);

#ifdef __cplusplus
}
#endif

#endif //APP_ANDROID_IMAGECODEC_H
