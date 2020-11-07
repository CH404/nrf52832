#define SPI_GLOBAL

#include "spi.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "stdio.h"
#include "global.h"

void spi_evt_handler(nrf_drv_spi_evt_t const * p_event,void *p_context)
{
	NRF_LOG_INFO("spi evt handler");

	if(p_event.type == NRF_DRV_SPI_EVENT_DONE)
		{
			NRF_LOG_INFO("NRF_DRV_SPI_EVENT_DONE");
		}
}


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
说明:spi 发送 data
函数名:ret_code_t SpiWrite(uint8_t data)
参数:
返回值:
**************************************************/
ret_code_t SPI_WriteData(uint8_t data)
{
	SPI_CS_Select();
	SPI_DATA_Select();
	nrf_drv_spi_transfer(&spi0,&data,sizeof(data),NULL,0);
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
//	SPI_COMMAND_Select();
  SPI_DATA_Select();
while(1);
	//nrf_drv_spi_transfer(&spi0,&command,sizeof(command),NULL,0);
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
        while(1);
//	nrf_drv_spi_transfer(&spi0,data,length,NULL,0);
}



