//
// Created by TangM on 2023/9/26.
//
#include "font.h"
#include "oled.h"
#include "i2c.h"
#include "string.h"
//从机设备地址
#define oled_address 0x78

uint8_t GRAM[8][128];       //为什么要头文件和源文件一起声明



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
void oled_writeram(uint8_t GRAM[8][128])
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
        GRAM[page][x]|=0x01<<row;  //每页有八行，将对应行和对应的x列结合得到点的坐标，将其置为一
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

/***********************        图形库             **********************************************/


/**
 * @brief 设置显存中一字节数据的某几位
 * @param page 页地址
 * @param column 列地址
 * @param data 数据
 * @param start 起始位
 * @param end 结束位
 * @param color 颜色
 * @note 此函数将显存中的某一字节的第start位到第end位设置为与data相同
 * @note start和end的范围为0-7, start必须小于等于end
 * @note 此函数与OLED_SetByte_Fine的区别在于此函数只能设置显存中的某一真实字节
 */
void OLED_SetByte_Fine(uint8_t page, uint8_t column, uint8_t data, uint8_t start, uint8_t end, OLED_ColorMode color) {
    static uint8_t temp;
    if (page >= OLED_PAGE || column >= OLED_COLUMN) return;
    if (color) data = ~data;

    temp = data | (0xff << (end + 1)) | (0xff >> (8 - start));
    GRAM[page][column] &= temp;
    temp = data & ~(0xff << (end + 1)) & ~(0xff >> (8 - start));
    GRAM[page][column] |= temp;
    // 使用OLED_SetPixel实现
    // for (uint8_t i = start; i <= end; i++) {
    //   OLED_SetPixel(column, page * 8 + i, !((data >> i) & 0x01));
    // }
}

/**
 * @brief 设置显存中的一字节数据的某几位
 * @param x 横坐标
 * @param y 纵坐标
 * @param data 数据
 * @param len 位数
 * @param color 颜色
 * @note 此函数将显存中从(x,y)开始向下数len位设置为与data相同
 * @note len的范围为1-8
 * @note 此函数与OLED_SetByte_Fine的区别在于此函数的横坐标和纵坐标是以像素为单位的, 可能出现跨两个真实字节的情况(跨页)
 */
void OLED_SetBits_Fine(uint8_t x, uint8_t y, uint8_t data, uint8_t len, OLED_ColorMode color) {
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    if (bit + len > 8) {
        OLED_SetByte_Fine(page, x, data << bit, bit, 7, color);
        OLED_SetByte_Fine(page + 1, x, data >> (8 - bit), 0, len + bit - 1 - 8, color);
    } else {
        OLED_SetByte_Fine(page, x, data << bit, bit, bit + len - 1, color);
    }
    // 使用OLED_SetPixel实现
    // for (uint8_t i = 0; i < len; i++) {
    //   OLED_SetPixel(x, y + i, !((data >> i) & 0x01));
    // }
}

/**
 * @brief 设置显存中的一字节数据
 * @param page 页地址
 * @param column 列地址
 * @param data 数据
 * @param color 颜色
 * @note 此函数将显存中的某一字节设置为data的值
 */
void OLED_SetByte(uint8_t page, uint8_t column, uint8_t data, OLED_ColorMode color) {
    if (page >= OLED_PAGE || column >= OLED_COLUMN) return;
    if (color) data = ~data;
    GRAM[page][column] = data;
}


/**
 * @brief 设置显存中一字节长度的数据
 * @param x 横坐标
 * @param y 纵坐标
 * @param data 数据
 * @param color 颜色
 * @note 此函数将显存中从(x,y)开始向下数8位设置为与data相同
 * @note 此函数与OLED_SetByte的区别在于此函数的横坐标和纵坐标是以像素为单位的, 可能出现跨两个真实字节的情况(跨页)
 */
void OLED_SetBits(uint8_t x, uint8_t y, uint8_t data, OLED_ColorMode color) {
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    OLED_SetByte_Fine(page, x, data << bit, bit, 7, color);
    if (bit) {
        OLED_SetByte_Fine(page + 1, x, data >> (8 - bit), 0, bit - 1, color);       //跨页
    }
}


/**
 * @brief 设置一块显存区域
 * @param x 起始横坐标
 * @param y 起始纵坐标
 * @param data 数据的起始地址
 * @param w 宽度
 * @param h 高度
 * @param color 颜色
 * @note 此函数将显存中从(x,y)开始的w*h个像素设置为data中的数据
 * @note data的数据应该采用列行式排列
 */
void OLED_SetBlock(uint8_t x, uint8_t y, const uint8_t *data, uint8_t w, uint8_t h, OLED_ColorMode color) {
    uint8_t fullRow = h / 8; // 完整的行数
    uint8_t partBit = h % 8; // 不完整的字节中的有效位数
    for (uint8_t i = 0; i < w; i++) {
        for (uint8_t j = 0; j < fullRow; j++) {
            OLED_SetBits(x + i, y + j * 8, data[i + j * w], color);
        }
    }
    if (partBit) {
        uint16_t fullNum = w * fullRow; // 完整的字节数
        for (uint8_t i = 0; i < w; i++) {
            OLED_SetBits_Fine(x + i, y + (fullRow * 8), data[fullNum + i], partBit, color);
        }
    }
    // 使用OLED_SetPixel实现
    // for (uint8_t i = 0; i < w; i++) {
    //   for (uint8_t j = 0; j < h; j++) {
    //     for (uint8_t k = 0; k < 8; k++) {
    //       if (j * 8 + k >= h) break; // 防止越界(不完整的字节
    //       OLED_SetPixel(x + i, y + j * 8 + k, !((data[i + j * w] >> k) & 0x01));
    //     }
    //   }
    // }
}

/**
 * @brief 绘制一张图片
 * @param x 起始点横坐标
 * @param y 起始点纵坐标
 * @param img 图片
 * @param color 颜色
 */
void oled_drawimage(uint8_t x, uint8_t y, const Image *img, OLED_ColorMode color) {
    OLED_SetBlock(x, y, img->data, img->w, img->h, color);
}



//void oled_draw_image(uint8_t x,uint8_t y,uint8_t* image,uint8_t sizex,uint8_t sizey)
//{
//    uint8_t page=0;
//    for(int j=0;j<sizey/8;j++)
//    {
//        for(int i=0;i<sizex;i++)
//        {
//            page=sizey/8;       //第y行在第几页
//            GRAM[page][i+x]=image[i+j*sizex];
//        }
//    }
//}
/*
 * 缓存复制，多缓存结构，把cache的内容拷贝到cached上
 */

/**
 * @brief 画一条直线
 *
 * @param x0 起点X坐标 [0,127]
 * @param y0 起点Y坐标 [0,63]
 * @param x1 终点X坐标 [0,127]
 * @param y1 终点Y坐标 [0,63]
 * @param mode 颜色模式
*/
void oled_drawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_ColorMode mode) {
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, uRow, uCol;
    delta_x = x1 - x0; //计算坐标增量
    delta_y = y1 - y0;
    uRow = x0;//画线起点坐标
    uCol = y0;
    if (delta_x > 0) {
        incx = 1;
    }
    else if (delta_x == 0) {
        incx = 0;
    }
    else {
        incx = -1;
        delta_x = -delta_x;
    }
    if (delta_y > 0) {
        incy = 1;
    }
    else if (delta_y == 0) {
        incy = 0;
    }
    else {
        incy = -1;
        delta_y = -delta_y;
    }
    if (delta_x > delta_y) {
        distance = delta_x;
    }
    else {
        distance = delta_y;
    }
    for (int t = 0; t <= distance + 1; t++) {
        oled_draw_pixel(uRow, uCol);
        xerr += delta_x;
        yerr += delta_y;
        if (xerr > distance) {
            xerr -= distance;
            uRow += incx;
        }
        if (yerr > distance) {
            yerr -= distance;
            uCol += incy;
        }
    }
}



/**
 * @brief 画一个空心的矩形
 *
 * @param x0 左端点X坐标 [0,127]
 * @param y0 上端点Y坐标 [0,63]
 * @param x1 右端点X坐标 [0,127]
 * @param y1 下端点Y坐标 [0,63]
 * @param mode 颜色模式
*/
void oled_drawrectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, OLED_ColorMode mode) {
    // 判断输入参数是否合理
    if (x0 >= x1 || y0 >= y1) {
        return;
    }
    oled_drawline(x0, y0, x1, y0, mode);
    oled_drawline(x0, y1, x1, y1, mode);
    oled_drawline(x0, y0, x0, y1, mode);
    oled_drawline(x1, y0, x1, y1, mode);
}

void cache_copy(uint8_t cache,uint8_t cached)
{
    memcpy(cached, cache , sizeof(cache));
}