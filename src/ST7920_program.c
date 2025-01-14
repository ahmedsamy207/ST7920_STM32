/*
 * ST7920_program.c
 *
 *  Created on: Jan 6, 2025
 *  Author  : Ahmed Samy Kamal
 */

#include "STD_TYPES.h"
#include "BIT_MATH.h"
#include "RCC_interface.h"
#include "DIO_interface.h"
#include "SPI_interface.h"
#include "STK_interface.h"
#include "ST7920_interface.h"

/*
 * CS-> PA0
 * RST->PA1
 */


u8 lcd_data[3];


u8 startRow, startCol, endRow, endCol; // coordinates of the dirty rectangle
u8 numRows = 64;
u8 numCols = 128;
u8 Graphic_Check = 0;
u8 image[(128 * 64)/8];



void ST7920_Init(void)
{
	MSTK_voidDelayMs(100);
	RCC_voidInitSysClock();
	RCC_voidEnablePeripheralClock(APB2_BUS,DIOA_RCC); //GPIOA
	RCC_voidEnablePeripheralClock(APB2_BUS,SPI1_RCC); //SPI

	MDIO_voidSetPinDirection(LCD_PORT,RS_CS_PIN,OUTPUT_SPEED_2MHZ_PP); 			//RS => CS
	MDIO_voidSetPinDirection(LCD_PORT,E_SCK_PIN,OUTPUT_SPEED_10MHZ_AFPP); 		//E => SCK
	MDIO_voidSetPinDirection(LCD_PORT,RW_MOSI_PIN,OUTPUT_SPEED_10MHZ_AFPP); 	//RW => MOSI
	MDIO_voidSetPinDirection(LCD_PORT,RST_PIN,OUTPUT_SPEED_2MHZ_PP); 			//RST

	//MDIO_voidSetPinValue(LCD_PORT,RS_CS_PIN,DIO_LOW); 			//RS => CS

	MSPI_voidInit();

	MDIO_voidSetPinValue(LCD_PORT,RST_PIN,DIO_LOW);
	MSTK_voidDelayMs(50);
	MDIO_voidSetPinValue(LCD_PORT,RST_PIN,DIO_HIGH);
	MSTK_voidDelayMs(100);
	ST7920_SendCmd(0x30);  		// 8bit mode
	MSTK_voidSetBusyWait_US(110);  				//  >100us delay

	ST7920_SendCmd(0x30);  		// 8bit mode
	MSTK_voidSetBusyWait_US(40);  				// >37us delay

	ST7920_SendCmd(0x08);  // D=0, C=0, B=0
	MSTK_voidSetBusyWait_US(110);  // >100us delay

	ST7920_SendCmd(0x01);  // clear screen
	MSTK_voidDelayMs(12);  // >10ms delay


	ST7920_SendCmd(0x06);  // cursor increment right no shift
	MSTK_voidDelayMs(1);  // 1ms delay

	ST7920_SendCmd(0x0C);  // D=1, C=0, B=0
	MSTK_voidDelayMs(1);  // 1ms delay

	ST7920_SendCmd(0x02);  // return to home
	MSTK_voidDelayMs(1);  // 1ms delay
}

void st7920_spi_transmit(u8 *data,u32 size)
{
	u32 i=0;

	while(i<size)
	{
		SPI1_u16Transceive(data[i]);
		i++;
	}

}

void ST7920_SendCmd (u8 cmd)
{
	MDIO_voidSetPinValue(LCD_PORT,RS_CS_PIN,DIO_HIGH);  // PUll the CS high

	lcd_data[0]=0xF8;
	lcd_data[1]=(cmd&0xf0);
	lcd_data[2]=((cmd<<4)&0xf0);

	st7920_spi_transmit(lcd_data,3);

	MSTK_voidSetBusyWait_US(30);

	MDIO_voidSetPinValue(LCD_PORT,RS_CS_PIN,DIO_LOW);  // PUll the CS LOW

}

void ST7920_SendData (u8 data)
{

	MDIO_voidSetPinValue(LCD_PORT,RS_CS_PIN,DIO_HIGH);

	lcd_data[0]=0xFA;
	lcd_data[1]=(data&0xf0);
	lcd_data[2]=((data<<4)&0xf0);

	st7920_spi_transmit(lcd_data,3);

	MSTK_voidSetBusyWait_US(30);

	MDIO_voidSetPinValue(LCD_PORT,RS_CS_PIN,DIO_LOW);  // PUll the CS LOW
}



void ST7920_SendString(u8 row, u8 col, u8* string)
{
	switch (row)
	{
	case 0:
		col |= 0x80;
		break;
	case 1:
		col |= 0x90;
		break;
	case 2:
		col |= 0x88;
		break;
	case 3:
		col |= 0x98;
		break;
	default:
		col |= 0x80;
		break;
	}

	ST7920_SendCmd(col);

	while (*string)
	{
		ST7920_SendData(*string++);
	}
}



// switch to graphic mode or normal mode::: enable = 1 -> graphic mode enable = 0 -> normal mode

void ST7920_GraphicMode (u8 enable)   // 1-enable, 0-disable
{
	if (enable == 1)
	{
		ST7920_SendCmd(0x30);  // 8 bit mode
		MSTK_voidDelayMs (1);
		ST7920_SendCmd(0x34);  // switch to Extended instructions
		MSTK_voidDelayMs (1);
		ST7920_SendCmd(0x36);  // enable graphics
		MSTK_voidDelayMs (1);
		Graphic_Check = 1;  // update the variable
	}

	else if (enable == 0)
	{
		ST7920_SendCmd(0x30);  // 8 bit mode
		MSTK_voidDelayMs (1);
		Graphic_Check = 0;  // update the variable
	}
}

void ST7920_DrawBitmap(const u8* graphic)
{
	u8 x, y;
	for(y = 0; y < 64; y++)
	{
		if(y < 32)
		{
			for(x = 0; x < 8; x++)							// Draws top half of the screen.
			{												// In extended instruction mode, vertical and horizontal coordinates must be specified before sending data in.
				ST7920_SendCmd(0x80 | y);				// Vertical coordinate of the screen is specified first. (0-31)
				ST7920_SendCmd(0x80 | x);				// Then horizontal coordinate of the screen is specified. (0-8)
				ST7920_SendData(graphic[2*x + 16*y]);		// Data to the upper byte is sent to the coordinate.
				ST7920_SendData(graphic[2*x+1 + 16*y]);	// Data to the lower byte is sent to the coordinate.
			}
		}
		else
		{
			for(x = 0; x < 8; x++)							// Draws bottom half of the screen.
			{												// Actions performed as same as the upper half screen.
				ST7920_SendCmd(0x80 | (y-32));			// Vertical coordinate must be scaled back to 0-31 as it is dealing with another half of the screen.
				ST7920_SendCmd(0x88 | x);
				ST7920_SendData(graphic[2*x + 16*y]);
				ST7920_SendData(graphic[2*x+1 + 16*y]);
			}
		}

	}
}


// Update the display with the selected graphics
void ST7920_Update(void)
{
	ST7920_DrawBitmap(image);
}



void ST7920_Clear()
{
	if (Graphic_Check == 1)  // if the graphic mode is set
	{
		u8 x, y;
		for(y = 0; y < 64; y++)
		{
			if(y < 32)
			{
				ST7920_SendCmd(0x80 | y);
				ST7920_SendCmd(0x80);
			}
			else
			{
				ST7920_SendCmd(0x80 | (y-32));
				ST7920_SendCmd(0x88);
			}
			for(x = 0; x < 8; x++)
			{
				ST7920_SendData(0);
				ST7920_SendData(0);
			}
		}
	}

	else
	{
		ST7920_SendCmd(0x01);   // clear the display using command
		MSTK_voidDelayMs(20); // delay >1.6 ms
	}
}




static void SetPixel(u8 x, u8 y)
{
	if (y < numRows && x < numCols)
	{
		u8 *p = image + ((y * (numCols/8)) + (x/8));
		*p |= 0x80u >> (x%8);

		*image = *p;

		// Change the dirty rectangle to account for a pixel being dirty (we assume it was changed)
		if (startRow > y) { startRow = y; }
		if (endRow <= y)  { endRow = y + 1; }
		if (startCol > x) { startCol = x; }
		if (endCol <= x)  { endCol = x + 1; }


	}

}

/* draw a line
 * start point (X0, Y0)
 * end point (X1, Y1)
 */
void DrawLine(u8 x0, u8 y0, u8 x1, u8 y1)
{
	int dx = (x1 >= x0) ? x1 - x0 : x0 - x1;
	int dy = (y1 >= y0) ? y1 - y0 : y0 - y1;
	int sx = (x0 < x1) ? 1 : -1;
	int sy = (y0 < y1) ? 1 : -1;
	int err = dx - dy;

	for (;;)
	{
		SetPixel(x0, y0);
		if (x0 == x1 && y0 == y1) break;
		int e2 = err + err;
		if (e2 > -dy)
		{
			err -= dy;
			x0 += sx;
		}
		if (e2 < dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}




/* Draw rectangle
 * start point (x,y)
 * w -> width
 * h -> height
 */
void DrawRectangle(u16 x, u16 y, u16 w, u16 h)
{
	/* Check input parameters */
	if (
			x >= numCols ||
			y >= numRows
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= numCols) {
		w = numCols - x;
	}
	if ((y + h) >= numRows) {
		h = numRows - y;
	}

	/* Draw 4 lines */
	DrawLine(x, y, x + w, y);         /* Top line */
	DrawLine(x, y + h, x + w, y + h); /* Bottom line */
	DrawLine(x, y, x, y + h);         /* Left line */
	DrawLine(x + w, y, x + w, y + h); /* Right line */
}




/* Draw filled rectangle
 * Start point (x,y)
 * w -> width
 * h -> height
 */
void DrawFilledRectangle(u16 x, u16 y, u16 w, u16 h)
{
	u8 i;

	/* Check input parameters */
	if (
			x >= numCols ||
			y >= numRows
	) {
		/* Return error */
		return;
	}

	/* Check width and height */
	if ((x + w) >= numCols) {
		w = numCols - x;
	}
	if ((y + h) >= numRows) {
		h = numRows - y;
	}

	/* Draw lines */
	for (i = 0; i <= h; i++) {
		/* Draw lines */
		DrawLine(x, y + i, x + w, y + i);
	}
}




/* draw circle
 * centre (x0,y0)
 * radius = radius
 */
void DrawCircle(u8 x0, u8 y0, u8 radius)
{
	int f = 1 - (int)radius;
	int ddF_x = 1;

	int ddF_y = -2 * (int)radius;
	int x = 0;

	SetPixel(x0, y0 + radius);
	SetPixel(x0, y0 - radius);
	SetPixel(x0 + radius, y0);
	SetPixel(x0 - radius, y0);

	int y = radius;
	while(x < y)
	{
		if(f >= 0)
		{
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;
		SetPixel(x0 + x, y0 + y);
		SetPixel(x0 - x, y0 + y);
		SetPixel(x0 + x, y0 - y);
		SetPixel(x0 - x, y0 - y);
		SetPixel(x0 + y, y0 + x);
		SetPixel(x0 - y, y0 + x);
		SetPixel(x0 + y, y0 - x);
		SetPixel(x0 - y, y0 - x);
	}
}


// Draw Filled Circle

void DrawFilledCircle(u16 x0, u16 y0, u16 r)
{
	u16 f = 1 - r;
	u16 ddF_x = 1;
	u16 ddF_y = -2 * r;
	u16 x = 0;
	u16 y = r;

	SetPixel(x0, y0 + r);
	SetPixel(x0, y0 - r);
	SetPixel(x0 + r, y0);
	SetPixel(x0 - r, y0);
	DrawLine(x0 - r, y0, x0 + r, y0);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		DrawLine(x0 - x, y0 + y, x0 + x, y0 + y);
		DrawLine(x0 + x, y0 - y, x0 - x, y0 - y);

		DrawLine(x0 + y, y0 + x, x0 - y, y0 + x);
		DrawLine(x0 + y, y0 - x, x0 - y, y0 - x);
	}
}



// Draw Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void DrawTriangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 x3, u16 y3)
{
	/* Draw lines */
	DrawLine(x1, y1, x2, y2);
	DrawLine(x2, y2, x3, y3);
	DrawLine(x3, y3, x1, y1);
}



// Draw Filled Traingle with coordimates (x1, y1), (x2, y2), (x3, y3)
void DrawFilledTriangle(u16 x1, u16 y1, u16 x2, u16 y2, u16 x3, u16 y3)
{
	u16 deltax = 0, deltay = 0, x = 0, y = 0, xinc1 = 0, xinc2 = 0,
			yinc1 = 0, yinc2 = 0, den = 0, num = 0, numadd = 0, numpixels = 0,
			curpixel = 0;

#define ABS(x)   ((x) > 0 ? (x) : -(x))

	deltax = ABS(x2 - x1);
	deltay = ABS(y2 - y1);
	x = x1;
	y = y1;

	if (x2 >= x1) {
		xinc1 = 1;
		xinc2 = 1;
	} else {
		xinc1 = -1;
		xinc2 = -1;
	}

	if (y2 >= y1) {
		yinc1 = 1;
		yinc2 = 1;
	} else {
		yinc1 = -1;
		yinc2 = -1;
	}

	if (deltax >= deltay){
		xinc1 = 0;
		yinc2 = 0;
		den = deltax;
		num = deltax / 2;
		numadd = deltay;
		numpixels = deltax;
	} else {
		xinc2 = 0;
		yinc1 = 0;
		den = deltay;
		num = deltay / 2;
		numadd = deltax;
		numpixels = deltay;
	}

	for (curpixel = 0; curpixel <= numpixels; curpixel++)
	{
		DrawLine(x, y, x3, y3);

		num += numadd;
		if (num >= den) {
			num -= den;
			x += xinc1;
			y += yinc1;
		}
		x += xinc2;
		y += yinc2;
	}
}


