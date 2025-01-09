/*
 * ST7920_interface.h
 *
 *  Created on: Jan 6, 2025
*  Author  : Ahmed Samy Kamal
 */

#ifndef ST7920_INTERFACE_H_
#define ST7920_INTERFACE_H_

#define LCD_PORT       	DIOA

#define RST_PIN       		3
#define RS_CS_PIN       	4
#define E_SCK_PIN       	5
#define RW_MOSI_PIN      	7


#define LCD_CLS         0x01
#define LCD_HOME        0x02
#define LCD_ADDRINC     0x06
#define LCD_DISPLAYON   0x0C
#define LCD_DISPLAYOFF  0x08
#define LCD_CURSORON    0x0E
#define LCD_CURSORBLINK 0x0F
#define LCD_BASIC       0x30
#define LCD_EXTEND      0x34
#define LCD_GFXMODE     0x36
#define LCD_TXTMODE     0x34
#define LCD_STANDBY     0x01
#define LCD_SCROLL      0x03
#define LCD_SCROLLADDR  0x40
#define LCD_ADDR        0x80
#define LCD_LINE0       0x80
#define LCD_LINE1       0x90
#define LCD_LINE2       0x88
#define LCD_LINE3       0x98


void ST7920_Init(void);
void st7920_spi_transmit(u8 *data,u32 size);
void ST7920_SendCmd (u8 cmd);
void ST7920_SendData (u8 data);
void ST7920_SendString(u8 row, u8 col, u8* string);
void ST7920_GraphicMode (u8 enable) ;
void ST7920_DrawBitmap(const u8* graphic);
void ST7920_Update(void);
void ST7920_Clear();
void ST7920_init();
void DrawLine(u8 x0, u8 y0, u8 x1, u8 y1);
void DrawRectangle(u16 x, u16 y, u16 w, u16 h);
void DrawFilledRectangle(u16 x, u16 y, u16 w, u16 h);
void DrawCircle(u8 x0, u8 y0, u8 radius);
void DrawFilledCircle(u16 x0, u16 y0, u16 r);
void DrawTriangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 x3, u16 y3);
void DrawFilledTriangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 x3, u16 y3);


#endif /* ST7920_INTERFACE_H_ */
