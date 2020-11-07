#include "it725x.h"
#include "twi.h"
#include "nrf_drv_twi.h"
//cap sensor driver
//使用 IT7251 
#if 0
#define IT725X_Write
#define IT725X_WriteLength

#define IT725X_Read
#define IT725X_ReadLength

#define IT725X_Reset

#define TWI0_SDA_PIN 5
#define TWI0_SCL_PIN 6

//#define HWTWI0
#ifdef HWTWI0
static const nrf_drv_twi_t it725x = NRF_DRV_TWI_INSTANCE(0);
#endif
#ifdef HWTWI1
static const nrf_drv_twi_t it725x = NRF_DRV_TWI_INSTANCE(1)
#endif
bool twi_done = false;
static void twi_handler(nrf_drv_twi_evt_t const *p_evt,void *p_context)
{
	switch(p_evt->type)
		{
			case NRFX_TWI_EVT_DONE:
				NRF_LOG_INFO("NRFX_TWI_EVT_DONE");
				twi_done = true;
				break;
			case NRFX_TWI_EVT_ADDRESS_NACK:
				NRF_LOG_INFO("address Nack");
				break;
			case NRFX_TWI_EVT_DATA_NACK:
				NRF_LOG_INFO("date Nack");
				break;
			case NRFX_TWI_EVT_BUS_ERROR:
				NRF_LOG_INFO("bus");
				break;
			case NRFX_TWI_EVT_OVERRUN:
				NRF_LOG_INFO("overrun");
				break;
		}
}

#if defined (HWTWI0) || defined (HWTWI1)
/************************************************
说明:TWI 初始化
函数名:ret_code_t TwiInit(void)
参数:
返回值:
**************************************************/
static void TWI_Init(void)
{
	nrf_drv_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
        ret_code_t err_code;
#ifdef HWTWI0
	config.scl = TWI0_SCL_PIN;
	config.sda = TWI0_SDA_PIN;
	config.frequency = NRF_DRV_TWI_FREQ_100K;
  config.interrupt_priority = APP_IRQ_PRIORITY_HIGH,
  config.clear_bus_init     = false;
	err_code = nrf_drv_twi_init(&it725x,&config,NULL,NULL);
  G_CHECK_ERROR_CODE_INFO(err_code);
	nrf_drv_twi_enable(&it725x);
#endif
#ifdef HWTWI1
	config.scl = TWI1_SCL_PIN;
	config.sda = TWI1_SDA_PIN;
	nrf_drv_twi_init(&it725x,&config,NULL,NULL);
   nrf_drv_twi_enable(&it725x);
#endif 
}

#endif




/***************************************
* 功能: 用于发送1byte (IT725x 发送DMA mode 数据)
* 函数: 
* 参数: address 器件地址 
*				commandAndData 命令,数据
*				length 长度
* 返回:成功 SUCCESS_I2C
****************************************/
ret_code_t IT725X_WriteLength(uint8_t address,uint8_t *commandAndData,uint8_t length)
{
	return nrf_drv_twi_tx(&it725x,address,commandAndData,length,false);
}


ret_code_t TWI_ReadLength(uint8_t address,uint8_t *command,uint8_t command_length,uint8_t *recevice,uint8_t receive_length)
{
	ret_code_t err_code;
	err_code = nrf_drv_twi_tx(&it725x,address,command,command_length,false); 
	if(err_code != NRF_SUCCESS)
		{
			return err_code;
		}
		err_code = nrf_drv_twi_tx(&it725x,address,,command_length,false);
}



/***************************************
* 功能: 用于发送读命令，然后在读取数据
* 函数: ret_code_t IT725X_CommandRead(IT725X_config_t *config)
* 参数: address 器件地址 
*				commandAndData 命令,数据
*				length 长度
* 返回:成功 SUCCESS_I2C
****************************************/
ret_code_t IT725X_CommandRead(IT725X_config_t *config)
{ 
	nrf_drv_twi_tx(&it725x,config->mode_regiseter,);
}




/************************************************
*说明: 做IT725x 的初始化
*函数名: void IT725X_Init(void)
*参数:
*返回值:
************************************************/
void IT725X_Init(void)
{
	
}

#endif

