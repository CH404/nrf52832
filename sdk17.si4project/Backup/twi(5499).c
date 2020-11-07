#define TWI_GLOBAL
#include "twi.h"

//使能twi模块
//1、sdk_config.h 中使能twi的库函数
//2、实例化twi handle
//3、配置初始化

#define TWI_SCL_PIN 26
#define TWI_SDA_PIN  27
#define TWI_INSTANCE_ID 0
static const nrf_drv_twi_t m_twi = NRFX_TWI_INSTANCE(TWI_INSTANCE_ID);

ret_code_t TwiInit(void)
{
	nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
	config.scl = TWI_SCL_PIN;
	config.sda = TWI_SDA_PIN;
	nrfx_twi_init(&m_twi,&config,NULL,NULL);
}

