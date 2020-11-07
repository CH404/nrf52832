/*
*/

#define TWI_GLOBAL
#include "twi.h"

//ʹ��twiģ��
//1��sdk_config.h ��ʹ��twi�Ŀ⺯��
//2��ʵ����twi handle
//3�����ó�ʼ��

#define TWI_SCL_PIN 26
#define TWI_SDA_PIN  27
#define TWI_INSTANCE_ID 0
static const nrfx_ m_twi = NRFX_TWI_INSTANCE(TWI_INSTANCE_ID);

ret_code_t TwiInit(void)
{
	nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
	config.scl = TWI_SCL_PIN;
	config.sda = TWI_SDA_PIN;
	nrfx_twi_init(&m_twi,&config,NULL,NULL);
}

ret_code_t TwiDriverInit(deviceName name)
{
	nrfx_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
	
	config.scl = MPU6050_SCL_PIN;
	config.sda = MPU6050_SDA_PIN;
	
}




