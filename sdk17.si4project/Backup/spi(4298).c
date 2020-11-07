#define SPI_GLOBAL

#include "spi.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "stdio.h"




static const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(0);
static bool SpiXferDone = true;

/************************************************
˵��:spi����
������:void SPI_HardWareInit(void)
����:
����ֵ:
**************************************************/
void SPI_HardWareInit(void)
{
	ret_code_t err_code;
	nrf_gpio_cfg_output(SPI_CS_PIN);
	nrf_gpio_cfg_output(SPI_DC_PIN);
	nrf_drv_spi_config_t config = NRF_DRV_SPI_DEFAULT_CONFIG;
	config.mosi_pin = SPI_MOSI_PIN;
	config.sck_pin = SPI_SCL_PIN;
	err_code = nrf_drv_spi_init(&spi0,&config,NULL,NULL);
	if(err_code != NRF_SUCCESS)
		{
		while(1);
		}
}


/************************************************
˵��:spi ���� data
������:ret_code_t SpiWrite(uint8_t data)
����:
����ֵ:
**************************************************/
ret_code_t SPI_WriteData(uint8_t data)
{
	SPI_CS_Select();
	SPI_DATA_Select();
	nrf_drv_spi_transfer(&spi0,&data,sizeof(data),NULL,0);
}

/************************************************
˵��:spi ���� command
������:ret_code_t SPI_SpiWritecommand(uint8_t command)
����:
����ֵ:
**************************************************/
ret_code_t SPI_Writecommand(uint8_t command)
{
	SPI_CS_Select();
//	SPI_COMMAND_Select();
  SPI_DATA_Select();
while(1);
	//nrf_drv_spi_transfer(&spi0,&command,sizeof(command),NULL,0);
}

/************************************************
˵��:spi ���� data
������:ret_code_t SPI_WriteDataLength(uint8_t data,uint16_t length)
����:
����ֵ:
**************************************************/
ret_code_t SPI_WriteDataLength(uint8_t *data,uint16_t length)
{
	SPI_CS_Select();
	SPI_DATA_Select();
        while(1);
//	nrf_drv_spi_transfer(&spi0,data,length,NULL,0);
}


