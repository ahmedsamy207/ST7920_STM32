/*
 * main.c
 *
 *  Created on: Jan 8, 2025
 *      Author: Ahmed Samy
 */

#include "STD_TYPES.h"
#include "BIT_MATH.h"

#include "RCC_interface.h"
#include "DIO_interface.h"
#include "STK_interface.h"
#include "SPI_interface.h"
#include "ST7920_interface.h"
#include "bitmap.h"

int main(void)
{
	ST7920_Init();

	while (1)
	{
		ST7920_GraphicMode(0);
		ST7920_SendString(0,0, "HELLO WORLD");
		ST7920_SendString(1,0, "FROM");
		ST7920_SendString(2,0, "AIO");
		MSTK_voidDelayMs(5000);
		ST7920_Clear();
		ST7920_GraphicMode(1);
		ST7920_DrawBitmap(logo);
		MSTK_voidDelayMs(5000);
		ST7920_Clear();
	}
	return 0;
}
