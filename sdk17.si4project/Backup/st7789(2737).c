#include "st7789.h"



#define ST7789_WriteData SPI_WriteData
#define ST7789_writeCommand SPI_Writecommand
#define ST7789_WriteDataLength SPI_WriteDataLength
#define ST7789_Delay nrf_delay_ms

/************************************************
说明:software reset
函数名:void ST7789_SoftwareReset(void)
参数:
返回值:
**************************************************/
void ST7789_SoftwareReset(void)
{
	ST7789_writeCommand(ST7789_RESET);
}



/************************************************
说明:spi 发送 data
函数名:ret_code_t SPI_WriteDataLength(uint8_t data,uint16_t length)
参数:
返回值:
**************************************************/

void ST7789_Init(void)
{
	//复位
	
}


