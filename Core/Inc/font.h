//
// Created by TangM on 2023/9/27.
//

#ifndef FONT_H
#define FONT_H
#include "main.h"

typedef struct Image {
    uint8_t w;           // 图片宽度
    uint8_t h;           // 图片高度
    const uint8_t *data; // 图片数据
} Image;
extern const Image dsImg;


#endif //OLED_FONT_H
