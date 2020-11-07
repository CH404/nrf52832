#define SPI_GLOBAL

#include "spi.h"
#include "nrf_drv_spi.h"
#include "nrf_gpio.h"
#include "stdio.h"
#include "global.h"
static const nrf_drv_spi_t spi0 = NRF_DRV_SPI_INSTANCE(0);
static bool SpiXferDone = true;
#define SPI_delay() nrf_delay_us(4)
void spi_evt_handler(nrf_drv_spi_evt_t const * p_event,void *p_context)
{
  SpiXferDone = true;
	NRF_LOG_INFO("spi evt handler");
	if(p_event->type == NRF_DRV_SPI_EVENT_DONE)
		{
			NRF_LOG_INFO("NRF_DRV_SPI_EVENT_DONE");
		}
}




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
	//config.frequency = NRF_DRV_SPI_FREQ_125K;
	config.mode = NRF_SPI_MODE_0;
	config.mosi_pin = SPI_SDA_PIN;
	config.sck_pin = SPI_SCL_PIN;
	config.ss_pin = SPI_CS_PIN;
	err_code = nrf_drv_spi_init(&spi0,&config,spi_evt_handler,NULL);
        G_CHECK_ERROR_CODE_INFO(err_code);
				  SpiXferDone = true;
}


/************************************************
说明:spi 发送 data
函数名:ret_code_t SpiWrite(uint8_t data)
参数:
返回值:
**************************************************/
ret_code_t SPI_WriteData(uint8_t data)
{
#ifdef SWSPI
nrf_gpio_pin_clear(SPI_CS_PIN);
nrf_gpio_pin_set(SPI_DC_PIN);
SPI_WriteByte(data);
//nrf_gpio_pin_set(SPI_CS_PIN);

#else
	ret_code_t err_code;
	SPI_CS_Select();
	SPI_DATA_Select();
//	nrf_delay_ms(1);
	err_code = nrf_drv_spi_transfer(&spi0,&data,sizeof(data),NULL,0);
	G_CHECK_ERROR_CODE_INFO(err_code);
//	SPI_CS_Release();
#endif
}

/************************************************
说明:spi 发送 command
函数名:ret_code_t SPI_SpiWritecommand(uint8_t command)
参数:
返回值:
**************************************************/
ret_code_t SPI_WriteCommand(uint8_t command)
{
#ifdef SWSPI
	nrf_gpio_pin_clear(SPI_CS_PIN);
	nrf_gpio_pin_clear(SPI_DC_PIN);
	SPI_WriteByte(command);
	nrf_gpio_pin_set(SPI_CS_PIN);
#else
	ret_code_t err_code;
      
      //  while(SpiXferDone == false);
	//SPI_CS_Select();
	SPI_COMMAND_Select();
//		nrf_delay_ms(1);
err_code = nrf_drv_spi_transfer(&spi0,&command,sizeof(command),NULL,0);
SpiXferDone = false;
while(SpiXferDone == false);
G_CHECK_ERROR_CODE_INFO(err_code);
//SPI_CS_Release();


   /*       memset(m_rx_buf, 0, m_length);
        spi_xfer_done = false;

        APP_ERROR_CHECK(nrf_drv_spi_transfer(&spi0, m_tx_buf, m_length, m_rx_buf, m_length));

        while (!spi_xfer_done)
        {
            __WFE();
        }*/
        #endif
}

/************************************************
说明:spi 发送 data
函数名:ret_code_t SPI_WriteDataLength(uint8_t data,uint16_t length)
参数:
返回值:
**************************************************/
ret_code_t SPI_WriteDataLength(uint8_t *data,uint16_t length)
{
	ret_code_t err_code;

	//SPI_CS_Select();
	SPI_DATA_Select();
	nrf_delay_ms(1);
	//SPI_delay();
	err_code = nrf_drv_spi_transfer(&spi0,data,length,NULL,0);
	G_CHECK_ERROR_CODE_INFO(err_code);
//	SPI_CS_Release();
}

void SPI_WriteByte(uint8_t byte)
{
	uint8_t i;
 nrf_gpio_cfg_output(SPI_SDA_PIN);
	nrf_gpio_pin_clear(SPI_SCL_PIN);
	for(i=0;i<8;i++)
		{
		nrf_gpio_pin_clear(SPI_SCL_PIN);
			if((byte&0x80) >> 7)
				{
					nrf_gpio_pin_set(SPI_SDA_PIN);
				}
			else
				{
					nrf_gpio_pin_clear(SPI_SDA_PIN);
				}
			//nrf_delay_ms(5);
				byte <<= 1;
			SPI_delay();
			nrf_gpio_pin_set(SPI_SCL_PIN);
			//nrf_delay_ms(5);
			SPI_delay();
		}
	nrf_gpio_pin_clear(SPI_SCL_PIN);
//	nrf_delay_ms(5);
//	nrf_gpio_pin_set(SPI_SCL_PIN);
}

uint8_t SPI_ReadByte(uint8_t command)
{
		uint8_t i;
		uint8_t data;
#ifdef SWSPI
		nrf_gpio_pin_clear(SPI_CS_PIN);
		nrf_gpio_pin_clear(SPI_DC_PIN);
		SPI_WriteByte(command);
#else
	nrf_drv_spi_uninit(&spi0);
		SPI_SoftWareInit();
#endif
		nrf_gpio_cfg(SPI_SDA_PIN,
							 NRF_GPIO_PIN_DIR_INPUT,
							 NRF_GPIO_PIN_INPUT_CONNECT,
							 NRF_GPIO_PIN_PULLUP,
							 NRF_GPIO_PIN_S0S1,
							 NRF_GPIO_PIN_NOSENSE);
                nrf_gpio_pin_clear(SPI_DC_PIN);
		nrf_gpio_pin_clear(SPI_SCL_PIN);
		//nrf_delay_ms(10);
		SPI_delay();
		for(i=0;i<8;i++)
		{		
			nrf_gpio_pin_set(SPI_SCL_PIN);
                        	data<<=1;		
			if(nrf_gpio_pin_read(SPI_SDA_PIN))
				{
					data++;
				}
             //             nrf_delay_ms(5);
             SPI_delay();
			nrf_gpio_pin_clear(SPI_SCL_PIN);
		//	nrf_delay_ms(5);
		SPI_delay();
		}
		return data;
}
void SPI_Rest(void)
{
	nrf_gpio_cfg_output(8);
	nrf_gpio_pin_clear(8);
	nrf_delay_ms(200);
	nrf_gpio_pin_set(8);
	nrf_delay_ms(200);
	
}
void SPI_SoftWareInit(void)
{
	nrf_gpio_cfg_output(SPI_SDA_PIN);
	nrf_gpio_cfg_output(SPI_SCL_PIN);
	nrf_gpio_cfg_output(SPI_CS_PIN);
	nrf_gpio_cfg_output(SPI_DC_PIN);
  nrf_gpio_cfg_output(8); 
        
	nrf_gpio_pin_set(SPI_SDA_PIN);
	nrf_gpio_pin_set(SPI_CS_PIN);
	nrf_gpio_pin_clear(SPI_SCL_PIN);
	nrf_gpio_pin_clear(SPI_DC_PIN);
	nrf_gpio_pin_set(8);
}




