//
// Created by TangM on 2023/9/26.
//

#include "oled.h"
#include "i2c.h"
#include "string.h"
//从机设备地址
#define oled_address 0x78

uint8_t GRAM[8][128];



//send 0x7a 0x00 +0x__ 是发指令  0x__是指令具体内容
//send 0x7a 0x40 是发数据


void oled_sendcmd(uint8_t cmd)
{
    uint8_t sendbuffer[2]={0};
    sendbuffer[0]=0x00;
    sendbuffer[1]=cmd;
    HAL_I2C_Master_Transmit(&hi2c1,oled_address,sendbuffer,2,HAL_MAX_DELAY);
}

void oled_senddata(uint8_t data)
{
    uint8_t sendbuffer[2]={0};
    sendbuffer[0]=0x40;
    sendbuffer[1]=data;
    HAL_I2C_Master_Transmit(&hi2c1,oled_address,sendbuffer,2,HAL_MAX_DELAY);
}

/**
  * @brief  OLED光标设置
  * @param  x，y  y有8页  每页是128列和8行，指令0x10是发高4位 0x10中的1表示高位,0表示高位是多少，指令0x00是发低四位，同理  原理是将8位数据拆分为两个四位发送
  * 例如
  * 0x18 0x05  就是高位为1低位为5 对应0x85=133列
  * @retval 无
  */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
    oled_sendcmd(0xB0 | Y);					//设置Y位置
    oled_sendcmd(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
    oled_sendcmd(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{
    uint8_t i, j;
    for (j = 0; j < 8; j++)
    {
        OLED_SetCursor(j, 0);
        for(i = 0; i < 128; i++)
        {
            oled_senddata(0x00);
        }
    }
}

void oled_init()
{
    oled_sendcmd(0xAE);	//关闭显示

    oled_sendcmd(0xD5);	//设置显示时钟分频比/振荡器频率
    oled_sendcmd(0x80);

    oled_sendcmd(0xA8);	//设置多路复用率
    oled_sendcmd(0x3F);

    oled_sendcmd(0xD3);	//设置显示偏移
    oled_sendcmd(0x00);

    oled_sendcmd(0x40);	//设置显示开始行

    oled_sendcmd(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置

    oled_sendcmd(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

    oled_sendcmd(0xDA);	//设置COM引脚硬件配置
    oled_sendcmd(0x12);

    oled_sendcmd(0x81);	//设置对比度控制
    oled_sendcmd(0xCF);

    oled_sendcmd(0xD9);	//设置预充电周期
    oled_sendcmd(0xF1);

    oled_sendcmd(0xDB);	//设置VCOMH取消选择级别
    oled_sendcmd(0x30);

    oled_sendcmd(0xA4);	//设置整个显示打开/关闭

    oled_sendcmd(0xA6);	//设置正常/倒转显示

    oled_sendcmd(0x8D);	//设置充电泵
    oled_sendcmd(0x14);

    oled_sendcmd(0xAF);	//开启显示

    OLED_Clear();
}

void oled_test()
{
    oled_sendcmd(0xb0);
    oled_sendcmd(0x00);
    oled_sendcmd(0x10);
    uint8_t sentbuffer[]={0x40,0xff,0xff,0xff,0xff,0xff};
    HAL_I2C_Master_Transmit(&hi2c1,oled_address,sentbuffer,6,HAL_MAX_DELAY);
}

/*
 * 缓存内容清空
 */
void oled_newram()
{
    memset(GRAM,0,sizeof(GRAM));
}

/*void oled_writeram()的作用是将缓存的数据输出，如此只要对GRAM缓存操作即可
 *
 */
void oled_writeram()
{
    uint8_t sendbuffer[129];
    sendbuffer[0]=0x40;         //发数据指令
    for(int j=0;j<8;j++)
    {
        for (int i = 0; i<128; i++)
        {
            sendbuffer[i+1]=GRAM[j][i];
        }
        oled_sendcmd(0xb0+j);   //跳到对应页
        oled_sendcmd(0x02);     //从第二列开始写  屏幕问题
        oled_sendcmd(0x10);
        HAL_I2C_Master_Transmit(&hi2c1,oled_address,sendbuffer,sizeof (sendbuffer),HAL_MAX_DELAY);
    }

}

void oled_draw_pixel(uint8_t x,uint8_t y)
{
    uint8_t page=0;
    uint8_t row=0;
    if(x<128&&y<64)
    {
        page=y/8;       //第y行在第几页
        row=y%8;        //第y行在第n页的第几行  如此便获得具体的y坐标在屏幕上
        GRAM[page][x]|=0x01<<row;  //每页有八行，将对应行置为一
    }
}

void oled_draw_line(uint8_t x1,uint8_t x2,uint8_t y1,uint8_t y2)
{
    for(int x=x1;x<x2;x++)
        for(int y=y1;y<y2;y++)
        {
            oled_draw_pixel(x,y);
        }
}