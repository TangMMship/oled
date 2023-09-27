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

#endif //OLED_H
