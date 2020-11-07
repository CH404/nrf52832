#ifndef __ST7789_H
#define __ST7789_H
#include <stdint.h>
#include "bmp.h"
/**
 * Comment one to use another one.
 * two parameters can be choosed
 * 135x240(0.96 inch) and 240x240(1.3inch) 
 * X_SHIFT&Y_SHIFT are used to correct different display's resolution
 */

/* Choose a type you are using */
//#define USING_135X240
#define USING_240X240


/* Choose a display rotation you want to use: (0-3) */
#define ST7789_ROTATION 0
//#define ST7789_ROTATION 1      
//#define ST7789_ROTATION 2				//  use Normally on 240x240
//#define ST7789_ROTATION 3

#ifdef USING_240X240

    #define ST7789_WIDTH 240
    #define ST7789_HEIGHT 240
		
		#if ST7789_ROTATION == 0
			#define X_SHIFT 0
			#define Y_SHIFT 40
		#elif ST7789_ROTATION == 1
			#define X_SHIFT 40
			#define Y_SHIFT 0
		#elif ST7789_ROTATION == 2
			#define X_SHIFT 0
			#define Y_SHIFT 0
		#elif ST7789_ROTATION == 3
			#define X_SHIFT 0
			#define Y_SHIFT 0
		#endif

#endif


#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define MAGENTA 0xF81F
#define GREEN 0x07E0
#define CYAN 0x7FFF
#define YELLOW 0xFFE0
#define GRAY 0X8430
#define BRED 0XF81F
#define GRED 0XFFE0
#define GBLUE 0X07FF
#define BROWN 0XBC40
#define BRRED 0XFC07
#define DARKBLUE 0X01CF
#define LIGHTBLUE 0X7D7C
#define GRAYBLUE 0X5458

#define LIGHTGREEN 0X841F
#define LGRAY 0XC618
#define LGRAYBLUE 0XA651
#define LBBLUE 0X2B12


//ST7789 command 
#define ST7789_SWRESET 0x01
#define ST7789_SET_PORCH 0xB2
#define ST7789_RDDID   0x04
#define ST7789_RDDST   0x09

#define ST7789_SLPIN   0x10
#define ST7789_SLPOUT  0x11
#define ST7789_PTLON   0x12
#define ST7789_NORON   0x13

#define ST7789_INVOFF  0x20
#define ST7789_INVON   0x21
#define ST7789_DISPOFF 0x28
#define ST7789_DISPON  0x29
#define ST7789_CASET   0x2A
#define ST7789_RASET   0x2B
#define ST7789_RAMWR   0x2C
#define ST7789_RAMRD   0x2E

#define ST7789_PTLAR   0x30
#define ST7789_COLMOD  0x3A
#define ST7789_MADCTL  0x36


#define ST7789_COLOR_MODE_16Bit	0x55
#define ST7789_COLOR_MODE_18Bit 0x66


/* Page Address Order ('0': Top to Bottom, '1': the opposite) */
#define ST7789_MADCTL_MY  0x80  
/* Column Address Order ('0': Left to Right, '1': the opposite) */
#define ST7789_MADCTL_MX  0x40  
/* Page/Column Order ('0' = Normal Mode, '1' = Reverse Mode) */
#define ST7789_MADCTL_MV  0x20  
/* Line Address Order ('0' = LCD Refresh Top to Bottom, '1' = the opposite) */
#define ST7789_MADCTL_ML  0x10
/* RGB/BGR Order ('0' = RGB, '1' = BGR) */
#define ST7789_MADCTL_RGB 0x00


void ST7789_SoftwareReset(void);
void ST7789_SetRotation(uint8_t m);
static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void ST7789_Fill_Color(uint16_t color);
void ST7789_Init(void);
void LCD_Init(void);
void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend);
void ST7789_command_list(void);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_PictureDraw(uint16_t x, uint16_t y,const uint8_t *pic,uint32_t size);
void ST7789_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor);



#endif
