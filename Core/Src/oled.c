//
// Created by TangM on 2023/9/26.
//

#include "oled.h"
#include "i2c.h"

#define oled_address 0x7a

//send 0x7a 0x00 +0x__ 是发指令  0x__是指令具体内容
//send 0x7a 0x40 是发数据


void oled_sendcmd(uint8_t cmd)
{
    uint8_t sendbuffer[2]={0};
    sendbuffer[0]=0x00;
    sendbuffer[1]=cmd;
    HAL_I2C_Master_Transmit(&hi2c1,oled_address,sendbuffer,2,HAL_MAX_DELAY);
}

void oled_init()
{

}