//
// Created by TangM on 2023/9/26.
//

#ifndef OLED_H
#define OLED_H
#include "i2c.h"

void oled_sendcmd(uint8_t cmd);
void oled_senddata(uint8_t data);
void oled_init();
void OLED_SetCursor(uint8_t Y, uint8_t X);
void OLED_Clear(void);
void oled_test();
void oled_newram();
void oled_writeram();
void oled_draw_pixel(uint8_t x,uint8_t y);
void oled_draw_line(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2);
#endif //OLED_H
