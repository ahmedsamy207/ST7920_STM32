# ST7920 Graphical LCD Driver for STM32F103C8T6 (Blue Pill) - SPI Interface

This repository contains a hardware driver for the **ST7920 Graphical LCD**, tailored for the **STM32F103C8T6 Blue Pill microcontroller**, utilizing the **SPI communication protocol**. The driver provides a robust API for graphical and text-based operations, enabling developers to fully harness the ST7920's capabilities in their embedded systems projects.

## Features

- **Full Control**:
  - Initialization of the ST7920 in both basic and extended instruction modes.
  - Support for both **graphic** and **text modes**.
  - Drawing primitives, including **lines**, **rectangles**, **circles**, and **triangles** (filled and unfilled).
  - Bitmap drawing for custom graphics.
- **Optimized SPI Communication**: Fast data transmission with minimal overhead.
- **Flexible API**:
  - Send commands, data, or strings to the LCD.
  - Switch between text and graphical modes dynamically.
  - Define custom dirty rectangles for optimized updates.
- **Built-in Graphics Operations**:
  - Draw shapes and filled areas.
  - Render text in multiple lines (up to 4 lines in text mode).
- **Modular Design**: Easy integration with existing projects, leveraging standard peripheral libraries.

## Requirements

- **Microcontroller**: STM32F103C8T6 (Blue Pill)
- **Display**: ST7920 Graphical LCD (128x64 resolution)
- **Communication Protocol**: SPI
- **Development Environment**: STM32CubeIDE, Keil, or any ARM GCC-based IDE

## Pin Configuration

| Pin Name   | Function       | Microcontroller Pin |
|------------|----------------|---------------------|
| `RST`      | Reset          | PA1                |
| `RS/CS`    | Chip Select    | PA0                |
| `E/SCK`    | SPI Clock      | PA5 (SPI1_SCK)     |
| `RW/MOSI`  | SPI MOSI       | PA7 (SPI1_MOSI)    |

## Getting Started

1. Clone this repository:  
   ```bash
   git clone https://github.com/yourusername/ST7920_SPI_Driver.git
2. Include the files ST7920_program.c and ST7920_interface.h in your STM32 project.

3. Ensure the SPI and GPIO configurations match the provided setup in the source code.

4. Call the initialization function ST7920_Init() in your main.c.

5. Use the provided API to send commands, draw shapes, or render bitmaps.

## Example Code

```c
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
```

## API Documentation
**The driver provides functions for:**

- Initialization: ST7920_Init
- Text operations: ST7920_SendString, ST7920_Clear
- Graphics: DrawRectangle, DrawCircle, DrawLine, ST7920_DrawBitmap, etc.
- Switching modes: ST7920_GraphicMode
- For full details, refer to the function prototypes in ST7920_interface.h.

## Contributions
Contributions are welcome! Please fork the repository, make your changes, and submit a pull request.
