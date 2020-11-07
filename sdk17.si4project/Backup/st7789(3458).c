#include "st7789.h"
#include "nrf_delay.h"
#include "spi.h"
#include "nrf_gpio.h"
#include "bmp.h"

#define ST7789_UnSelect SPI_CS_Release
#define ST7789_Select SPI_CS_Select
//#define ST7789_write8bit SPI_WriteByte
#define ST7789_WriteData SPI_WriteData
#define ST7789_WriteCommand SPI_WriteCommand
#define ST7789_WriteDataLength SPI_WriteDataLength
#define ST7789_Delay nrf_delay_ms
#define ST7789_Reset SPI_Rest
#define ST7789_SPIInit SPI_Init
#define ST7789_SPIUninit SPI_Uninit

#if 1
#define ST7789_MAX_TX_BUFF 100
uint8_t ST7789_TXBuffer[ST7789_MAX_TX_BUFF]={0};
#endif

/************************************************
说明:software reset
函数名:void ST7789_SoftwareReset(void)
参数:
返回值:
**************************************************/
void ST7789_SoftwareReset(void)
{
	ST7789_WriteCommand(ST7789_SWRESET);
}

void ST7789_SetRotation(uint8_t m)
{
	ST7789_WriteCommand(ST7789_MADCTL);	// MADCTL
	switch (m) {
	case 0:
		ST7789_WriteData(ST7789_MADCTL_MX | ST7789_MADCTL_MY | ST7789_MADCTL_RGB);
		break;
	case 1:
		ST7789_WriteData(ST7789_MADCTL_MY | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	case 2:
		ST7789_WriteData(ST7789_MADCTL_RGB);
		break;
	case 3:
		ST7789_WriteData(ST7789_MADCTL_MX | ST7789_MADCTL_MV | ST7789_MADCTL_RGB);
		break;
	default:
		break;
	}
}

static void ST7789_SetAddressWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
//	ST7789_Select();
	uint16_t x_start = x0 + X_SHIFT,x_end = x1+X_SHIFT;
	uint16_t y_start = y0 + Y_SHIFT,y_end = y1+Y_SHIFT;
        x_end = x_end > (ST7789_WIDTH+X_SHIFT) ? (ST7789_WIDTH+ X_SHIFT-1) : x_end;
        y_end = x_end > (ST7789_HEIGHT+Y_SHIFT)? (ST7789_HEIGHT+Y_SHIFT-1) : y_end;
	ST7789_WriteCommand(ST7789_CASET); 
	{
		uint8_t data[] = {x_start >> 8, x_start & 0xFF, x_end >> 8, x_end & 0xFF};
		ST7789_WriteDataLength(data, sizeof(data));
	}

	/* Row Address set */
	ST7789_WriteCommand(ST7789_RASET);
	{
		uint8_t data[] = {y_start >> 8, y_start & 0xFF, y_end >> 8, y_end & 0xFF};
		ST7789_WriteDataLength(data, sizeof(data));
	}
	/* Write to RAM */
	ST7789_WriteCommand(ST7789_RAMWR);
}


void ST7789_Fill_Color(uint16_t color)
{
	uint16_t i, j;
	uint8_t data[] = {color>>8,color&0xFF};
	ST7789_SetAddressWindow(0, 0,ST7789_WIDTH-1,ST7789_HEIGHT-1);
//	ST7789_Select();
	for (i = 0; i < 240; i++)
	 	for (j = 0; j < 240; j++) {
			ST7789_WriteDataLength(data, sizeof(data));
		}
//	ST7789_UnSelect();
		ST7789_WriteCommand(0x29); 

}

/**
 * @brief Draw a Pixel
 * @param x&y -> coordinate to Draw
 * @param color -> color of the Pixel
 * @return none
 */
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
	if ((x < 0) || (x >= ST7789_WIDTH) ||
		 (y < 0) || (y >= ST7789_HEIGHT))	return;
	
	ST7789_SetAddressWindow(x, y, x, y);
	uint8_t data[] = {color >> 8, color & 0xFF};
	ST7789_Select();
	ST7789_WriteDataLength(data,sizeof(data));
	ST7789_UnSelect();
}

void ST7789_PictureDraw(uint16_t x, uint16_t y,const uint8_t *pic,uint32_t size)
{
  uint32_t remain = size;
	uint32_t count = 0;
  remain-=8;
			ST7789_SetAddressWindow(x,y,pic[3]+x-1,pic[5]+y-1);
  //  ST7789_SetAddressWindow(0, 0,ST7789_WIDTH-1,ST7789_HEIGHT-1);     
  while(remain)
  {
    size = (remain >ST7789_MAX_TX_BUFF)?ST7789_MAX_TX_BUFF:remain;
    memcpy(ST7789_TXBuffer,pic+8+count,size);
		count+=size;
		remain-=size;
    ST7789_WriteDataLength(ST7789_TXBuffer,size);
  }
  ST7789_WriteCommand(0x29); 
}

void ST7789_WriteChar(uint16_t x, uint16_t y, char ch, FontDef font, uint16_t color, uint16_t bgcolor)
{
	uint32_t i, b, j;
	ST7789_Select();
	ST7789_SetAddressWindow(x, y, x + font.width - 1, y + font.height - 1);

	for (i = 0; i < font.height; i++) {
		b = font.data[(ch - 32) * font.height + i];
		for (j = 0; j < font.width; j++) {
			if ((b << j) & 0x8000) {
				uint8_t data[] = {color >> 8, color & 0xFF};
				 ST7789_WriteDataLength(data,sizeof(uint16_t));
			}
			else {
				uint8_t data[] = {bgcolor >> 8, bgcolor & 0xFF};
				ST7789_WriteDataLength(data,sizeof(uint16_t));
			}
		}
	}
//	ST7789_UnSelect();
}


void ST7789_WriteString(uint16_t x, uint16_t y, const char *str, FontDef font, uint16_t color, uint16_t bgcolor)
{
	ST7789_Select();
	while (*str) {
		if (x + font.width >= ST7789_WIDTH) {
			x = 0;
			y += font.height;
			if (y + font.height >= ST7789_HEIGHT) {
				break;
			}

			if (*str == ' ') {
				// skip spaces in the beginning of the new line
				str++;
				continue;
			}
		}
		ST7789_WriteChar(x, y, *str, font, color, bgcolor);
		x += font.width;
		str++;
	}
//	ST7789_UnSelect();
}
void ST7789_DrawCircle(uint16_t x0, uint16_t y0, uint8_t r, uint16_t color)
{
	int16_t f = 1 - r;
	int16_t ddF_x = 1;
	int16_t ddF_y = -2 * r;
	int16_t x = 0;
	int16_t y = r;

	ST7789_Select();
	ST7789_DrawPixel(x0, y0 + r, color);
	ST7789_DrawPixel(x0, y0 - r, color);
	ST7789_DrawPixel(x0 + r, y0, color);
	ST7789_DrawPixel(x0 - r, y0, color);

	while (x < y) {
		if (f >= 0) {
			y--;
			ddF_y += 2;
			f += ddF_y;
		}
		x++;
		ddF_x += 2;
		f += ddF_x;

		ST7789_DrawPixel(x0 + x, y0 + y, color);
		ST7789_DrawPixel(x0 - x, y0 + y, color);
		ST7789_DrawPixel(x0 + x, y0 - y, color);
		ST7789_DrawPixel(x0 - x, y0 - y, color);

		ST7789_DrawPixel(x0 + y, y0 + x, color);
		ST7789_DrawPixel(x0 - y, y0 + x, color);
		ST7789_DrawPixel(x0 + y, y0 - x, color);
		ST7789_DrawPixel(x0 - y, y0 - x, color);
	}
//	ST7789_UnSelect();
}

#if 0
void ST77891_Init(void)
{
	//复位
	//ST7789_Select();
	ST7789_Reset();
	ST7789_WriteCommand (ST7789_SLPOUT);	//	Out of sleep mode
	ST7789_Delay(200);

		ST7789_WriteCommand(0x36);
	ST7789_WriteData(0x00);
//	ST7789_WriteCommand(ST7789_SWRESET);
//	ST7789_Delay(10);	
	ST7789_WriteCommand(ST7789_COLMOD); 	//	Set color mode

	ST7789_WriteData(0x55);//set color mode 设置RGB 色彩

	
	ST7789_WriteCommand(ST7789_SET_PORCH);
//	ST7789_WriteDataLength(data,sizeof(data));
	ST7789_WriteData(0x0C);
	ST7789_WriteData(0x0C);
	ST7789_WriteData(0x00);
	ST7789_WriteData(0x33);
	ST7789_WriteData(0x33);

		//显示方式
	//	ST7789_SetRotation(ST7789_ROTATION);	//	MADCTL (Display Rotation)
	
	/* Internal LCD Voltage generator settings */
    ST7789_WriteCommand(0XB7);				//	Gate Control	//LCD控制电路板.是提供给LCD显示屏（Panel）控制信号的电路板.提供的信号一般有两种,
    																										//gate信号和source信号,两种信号分别从横向和纵向输入.
    																										//叠加形成电势差从而控制一个子像素（对于黑白是像素）的亮暗
   // ST7789_WriteData(0x35);			//	Default value			
   ST7789_WriteData(0x75);
    ST7789_WriteCommand(0xBB);				//	VCOM setting
  //  ST7789_WriteData(0x19);			//	0.725v (default 0.75v for 0x20)
   ST7789_WriteData(0x3D);	
	
 //   ST7789_WriteCommand(0xC0);				//	LCMCTRL	
 //   ST7789_WriteData (0x2C);			//	Default value
    ST7789_WriteCommand (0xC2);				//	VDV and VRH command Enable
    ST7789_WriteData (0x01);			//	Default value
    ST7789_WriteCommand (0xC3);				//	VRH set
//    ST7789_WriteData (0x12);			//	+-4.45v (defalut +-4.1v for 0x0B)
		ST7789_WriteData (0x19);

    ST7789_WriteCommand (0xC4);				//	VDV set
    ST7789_WriteData (0x20);			//	Default value
    ST7789_WriteCommand (0xC6);				//	Frame rate control in normal mode
    ST7789_WriteData (0x0F);			//	Default value (60HZ)
    ST7789_WriteCommand (0xD0);				//	Power control
    ST7789_WriteData (0xA4);			//	Default value
    ST7789_WriteData (0xA1);			//	Default value
	/**************** Division line ****************/
		ST7789_WriteCommand(	0xD6);		 
		ST7789_WriteData(  0xA1);

	ST7789_WriteCommand(0xE0);
	{
		//uint8_t data[] = {0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23};
		ST7789_WriteData(  0xD0);   
ST7789_WriteData(  0x04);   
ST7789_WriteData(  0x0D);   
ST7789_WriteData(  0x11);   
ST7789_WriteData(  0x13);   
ST7789_WriteData(  0x2B);   
ST7789_WriteData(  0x3F);   
ST7789_WriteData( 0x54);   
ST7789_WriteData( 0x4C);   
ST7789_WriteData(  0x18);   
ST7789_WriteData(  0x0D);   
ST7789_WriteData(  0x0B);   
ST7789_WriteData( 0x1F);   
ST7789_WriteData(  0x23);  
	/*		ST7789_WriteData(  0x70); 	
					ST7789_WriteData(  0x04); 	
					ST7789_WriteData(  0x08); 	
					ST7789_WriteData(  0x09); 	
					ST7789_WriteData(  0x09); 	
					ST7789_WriteData(  0x05); 	
					ST7789_WriteData(  0x2A); 	
					ST7789_WriteData( 0x33);	 
					ST7789_WriteData( 0x41);	 
					ST7789_WriteData(  0x07); 	
					ST7789_WriteData(  0x13); 	
					ST7789_WriteData(  0x13); 	
					ST7789_WriteData( 0x29);	 
					ST7789_WriteData(  0x2F); 	*/

	//	ST7789_WriteDataLength(data, sizeof(data));
	}

    ST7789_WriteCommand(0xE1);
	{
//		uint8_t data[] = {0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23};
		ST7789_WriteData(  0xD0);   
ST7789_WriteData(  0x04);     
ST7789_WriteData(  0x0C);   
ST7789_WriteData(  0x11);   
ST7789_WriteData( 0x13);   
ST7789_WriteData(  0x2C);   
ST7789_WriteData( 0x3F);   
ST7789_WriteData( 0x44);   
ST7789_WriteData(  0x51);   
ST7789_WriteData(0x2F);   
ST7789_WriteData( 0x1F);   
ST7789_WriteData( 0x1F);   
ST7789_WriteData( 0x20);
	ST7789_WriteData(  0x23); 
	/*		ST7789_WriteData(  0x70); 	
		ST7789_WriteData(  0x03); 	
		ST7789_WriteData(  0x09); 	
		ST7789_WriteData(  0x0A); 	
		ST7789_WriteData(  0x09); 	
		ST7789_WriteData( 0x06);	 
		ST7789_WriteData(  0x2B); 	
		ST7789_WriteData( 0x34);	 
		ST7789_WriteData( 0x41);	 
		ST7789_WriteData(  0x07); 	
		ST7789_WriteData(0x12); 	
		ST7789_WriteData( 0x14);	 
		ST7789_WriteData( 0x28);	 
		ST7789_WriteData( 0x2E);*/

	//	ST7789_WriteDataLength(data, sizeof(data));
	}
    ST7789_WriteCommand (ST7789_INVON);		//	Inversion ON
	
  //	ST7789_WriteCommand (ST7789_NORON);		//	Normal Display on
  	ST7789_WriteCommand (ST7789_DISPON);	//	Main screen turned on	

	ST7789_Delay(50);
/*	ST7789_WriteCommand(0x2A);		 //Column Address Set
	ST7789_WriteData(0x00); 	
	ST7789_WriteData( 0x00);	 //0
	ST7789_WriteData( 0x00);	 
	ST7789_WriteData(0xEF); 	//239
	
	ST7789_WriteCommand(0x2B);		 //Row Address Set
	ST7789_WriteData(0x00); 	
	ST7789_WriteData(0x00); 	//0
	ST7789_WriteData(0x00); 	
	ST7789_WriteData(0xEF); 	//239*/
	ST7789_WriteCommand(0x2C);

	//ST7789_Fill_Color(RED);				//	Fill with Black.
	
}
#endif

/************************************************
说明:ST7789 显示
函数名:ST7789_display(void)
返回值:
**************************************************/
void ST7789_Display(void)
{
	ST7789_WriteCommand(ST7789_DISPON); 
}

/************************************************
说明:ST7789 关闭
函数名:ST7789_display(void)
返回值:
**************************************************/
void ST7789_UnDisplay(void)
{
	ST7789_WriteCommand(ST7789_DISPOFF); 
}


/************************************************
说明:ST7789 屏幕初始化
函数名:ST7789_Init
返回值:
**************************************************/
void ST7789_Init(void)
{
	ST7789_SPIInit();
	ST7789_command_list();
}

/************************************************
说明:spi 发送 data
函数名:ret_code_t SPI_WriteDataLength(uint8_t data,uint16_t length)
参数:
返回值:
**************************************************/
static void ST7789_command_list(void)
{
	ST7789_Reset();

	ST7789_WriteCommand(0x11);
	
	ST7789_Delay(200);
	ST7789_WriteCommand(0x36);
	ST7789_WriteData(0x00);

	ST7789_WriteCommand(0x3A);
//	ST7789_WriteData(0x06);
	ST7789_WriteData(0x65);


	ST7789_WriteCommand(0xB2);
	ST7789_WriteData(0x0C);
	ST7789_WriteData(0x0C);
	ST7789_WriteData(0x00);
	ST7789_WriteData(0x33);
	ST7789_WriteData(0x33);

	ST7789_WriteCommand(0xB7);
	ST7789_WriteData(0x75);
//	ST7789_WriteData(0x72);


	ST7789_WriteCommand(0xBB);     
ST7789_WriteData(0x3D);   //Vcom=1.625V


//ST7789_WriteCommand( 0xC0);     
//ST7789_WriteData( 0x2C);  


ST7789_WriteCommand( 0xC2);     
ST7789_WriteData( 0x01);   

ST7789_WriteCommand( 0xC3);     
ST7789_WriteData( 0x19);   //GVDD=4.8V 

ST7789_WriteCommand(  0xC4);     
ST7789_WriteData( 0x20);   //VDV, 0x20:0v

ST7789_WriteCommand(  0xC6);     
ST7789_WriteData(  0x0F);   //0x0F:60Hz        	

ST7789_WriteCommand(  0xD0);     
ST7789_WriteData(  0xA4);   
ST7789_WriteData(  0xA1);   

ST7789_WriteCommand(  0xD6);     
ST7789_WriteData(  0xA1);

ST7789_WriteCommand(0xE0);     
ST7789_WriteData(  0x70);   
ST7789_WriteData(  0x04);   
ST7789_WriteData(  0x08);   
ST7789_WriteData(  0x09);   
ST7789_WriteData(  0x09);   
ST7789_WriteData(  0x05);   
ST7789_WriteData(  0x2A);   
ST7789_WriteData( 0x33);   
ST7789_WriteData( 0x41);   
ST7789_WriteData(  0x07);   
ST7789_WriteData(  0x13);   
ST7789_WriteData(  0x13);   
ST7789_WriteData( 0x29);   
ST7789_WriteData(  0x2F);   

ST7789_WriteCommand(0xE1);     
ST7789_WriteData(  0x70);   
ST7789_WriteData(  0x03);   
ST7789_WriteData(  0x09);   
ST7789_WriteData(  0x0A);   
ST7789_WriteData(  0x09);   
ST7789_WriteData( 0x06);   
ST7789_WriteData(  0x2B);   
ST7789_WriteData( 0x34);   
ST7789_WriteData( 0x41);   
ST7789_WriteData(  0x07);   
ST7789_WriteData(0x12);   
ST7789_WriteData( 0x14);   
ST7789_WriteData( 0x28);   
ST7789_WriteData( 0x2E);   

ST7789_WriteCommand(0x21); 
ST7789_UnDisplay();
//ST7789_PictureDraw(0,0,gImage_bmp,sizeof(gImage_bmp));
ST7789_Display();

//ST7789_WriteCommand(0x29);   
//ST7789_Fill_Color(BLACK);

//ST7789_PictureDraw(gImage_bmp,sizeof(gImage_bmp));

//ST7789_PictureDraw(78,40,gImage_ble,sizeof(gImage_ble));
//ST7789_WriteCommand(0x2C);	
}


void BlockWrite(unsigned int Xstart,unsigned int Xend,unsigned int Ystart,unsigned int Yend)
{

	ST7789_WriteCommand(0x2a);   
	ST7789_WriteData((Xstart)>>8);
	ST7789_WriteData((Xstart)&0xff);
	
	ST7789_WriteData((Xend)>>8);
	ST7789_WriteData((Xend)&0xff);
	

	ST7789_WriteCommand(0x2b);   
	ST7789_WriteData((Ystart+40)>>8);
	
	ST7789_WriteData((Ystart+40)&0xff);
	ST7789_WriteData((Yend+40)>>8);
	
	ST7789_WriteData((Yend+40)&0xff);
	ST7789_WriteCommand(0x2c);
}




