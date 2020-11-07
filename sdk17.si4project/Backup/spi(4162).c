#define SPI_GLOBAL

#include "spi.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"





static const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(0);
static bool SpiXferDone = true;

/************************************************
说明:spi配置
函数名:void SPI_HardWareInit(void)
参数:
返回值:
**************************************************/
void SPI_HardWareInit(void)
{
	nrf_gpio_cfg_output(SPI_CS_PIN);
	nrf_gpio_cfg_output(SPI_DC_PIN);
	nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG;
	config.mosi_pin = SPI_MOSI_PIN;
	config.sck_pin = SPI_SCL_PIN;
	nrf_drv_spi_init(&spi0,&config,NULL,NULL);
}


/************************************************
说明:spi 发送 data
函数名:ret_code_t SpiWrite(uint8_t data)
参数:
返回值:
**************************************************/
ret_code_t SPI_WriteData(uint8_t data)
{
	SPI_CS_Select();
	SPI_DATA_Select();
	nrf_drv_spi_transfer(&spi0,&data,sizof(data),NULL,0);
}

/************************************************
说明:spi 发送 command
函数名:ret_code_t SPI_SpiWritecommand(uint8_t command)
参数:
返回值:
**************************************************/
ret_code_t SPI_Writecommand(uint8_t command)
{
	SPI_CS_Select();
	SPI_COMMAND_Select();
	nrf_drv_spi_transfer(&spi0,&command,sizof(command),NULL,0);
}

/************************************************
说明:spi 发送 data
函数名:ret_code_t SPI_WriteDataLength(uint8_t data,uint16_t length)
参数:
返回值:
**************************************************/
ret_code_t SPI_WriteDataLength(uint8_t *data,uint16_t length)
{
	SPI_CS_Select();
	SPI_DATA_Select();
	nrf_drv_spi_transfer(&spi0,data,length,NULL,0);
}



