#include "st7789.h"



#define ST7789_WriteData SPI_WriteData
#define ST7789_writeCommand SPI_Writecommand
#define ST7789_WriteDataLength SPI_WriteDataLength
#define ST7789_Delay nrf_delay_ms

/************************************************
˵��:software reset
������:void ST7789_SoftwareReset(void)
����:
����ֵ:
**************************************************/
void ST7789_SoftwareReset(void)
{
	ST7789_writeCommand(ST7789_RESET);
}



/************************************************
˵��:spi ���� data
������:ret_code_t SPI_WriteDataLength(uint8_t data,uint16_t length)
����:
����ֵ:
**************************************************/

void ST7789_Init(void)
{
	//��λ
	
}


