/*
*/

#define TWI_GLOBAL
#include "global.h"
#include "twi.h"

//使能twi模块
//1、sdk_config.h 中使能twi的库函数
//2、实例化twi handle
//3、配置初始化

#define TWI_SCL_PIN 26
#define TWI_SDA_PIN  27
#define TWI_INSTANCE_ID 0
static const nrfx_twi_t m_twi = NRFX_TWI_INSTANCE(TWI_INSTANCE_ID);

ret_code_t TwiInit(void)
{
	nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
	config.scl = TWI_SCL_PIN;
	config.sda = TWI_SDA_PIN;
	nrfx_twi_init(&m_twi,&config,NULL,NULL);
}

void twi_handler(nrfx_twi_evt_t const *p_evt,void *p_context)
{
  p_evt
}

ret_code_t TwiDriverInit(deviceName name)
{
	ret_code_t err_code;
	//配置参数
	nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
	switch(name)
		{
			case MPU6050:
			config.scl = MPU6050_SCL_PIN;
			config.sda = MPU6050_SDA_PIN;
			break;
			default:
				break;
		}
		//初始化，不提供event_handler,twi阻塞模式
	err_code =	nrfx_twi_init(&m_twi,&config,twi_handler,NULL);
	G_CHECK_ERROR_CODE_INFO(err_code);

	nrfx_twi_enable(&m_twi);

}

ret_code_t TwiDriverSend( uint8_t address, uint8_t * p_data, size_t length)
{
	return nrfx_twi_tx(&m_twi,address,p_data,length,true);
}


#define MPU6050_ADDRESS 0x68
#define WRITE_CMD ((MPU6050_ADDRESS>>1)|0x80)
#define	READ_CMD ((MPU6050_ADDRESS>>1)&0x7F)
#define WHO_AM_I_ADDRESS 0x75
void test(void)
{
	ret_code_t err = 0;

	uint8_t data_buff[1];
	uint8_t rx_data_buff[3] = {0};
	data_buff[0] = WHO_AM_I_ADDRESS;
	
	//err = TwiDriverSend(READ_CMD,data_buff,1);
       err =  nrfx_twi_tx(&m_twi,READ_CMD,data_buff,1,true);
	NRF_LOG_INFO("twi %d",err);
	
	//err = nrfx_twi_rx(&m_twi,MPU6050_ADDRESS,rx_data_buff,3);
	//NRF_LOG_INFO("twi %d",err);
	//NRF_LOG_INFO("rx_data_buff %d",rx_data_buff[0]);
}






