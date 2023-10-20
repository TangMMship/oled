//
// Created by TangM on 2023/9/26.
//

#ifndef OLED_H
#define OLED_H
#include "i2c.h"
#include "font.h"
// OLED参数
#define OLED_PAGE 8            // OLED页数
#define OLED_ROW 8 * OLED_PAGE // OLED行数
#define OLED_COLUMN 128        // OLED列数

extern uint8_t GRAM[8][128];

typedef enum {
    OLED_COLOR_NORMAL = 0, // 正常模式 黑底白字
    OLED_COLOR_REVERSED    // 反色模式 白底黑字
} OLED_ColorMode;


void oled_sendcmd(uint8_t cmd);
void oled_senddata(uint8_t data);
void oled_init();
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);
void oled_test();
void oled_newram();
void oled_writeram(uint8_t GRAM[8][128]);
void oled_draw_pixel(uint8_t x,uint8_t y);
//void oled_draw_line(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2);
void oled_draw_image(uint8_t x,uint8_t y,uint8_t* image,uint8_t sizex,uint8_t sizey);
void oled_drawimage(uint8_t x, uint8_t y, const Image *img, OLED_ColorMode color);
void oled_drawrectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_ColorMode mode);
void oled_drawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_ColorMode mode);


        void cache_copy(uint8_t cache,uint8_t cached);
#endif //OLED_H
