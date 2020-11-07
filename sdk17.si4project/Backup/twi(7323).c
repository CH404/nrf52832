/*
nrfx_err_t nrfx_twi_tx(nrfx_twi_t const * p_instance, twi���
                       uint8_t            address,	 ������ַ������Ҫ���8λ�Ӷ�дλ��API���
                       uint8_t const *    p_data,			���͵�����
                       size_t             length,			����
                       bool               no_stop);		�Ƿ����ֹͣλ
                       

*/

#define TWI_GLOBAL
#include "global.h"
#include "twi.h"
#include "nrfx_timer.h"
#include "nrf_delay.h"
//ʹ��twiģ��
//1��sdk_config.h ��ʹ��twi�Ŀ⺯��
//2��ʵ����twi handle
//3�����ó�ʼ��

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

bool twi_done = false;
void twi_handler(nrfx_twi_evt_t const *p_evt,void *p_context)
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

ret_code_t TwiDriverInit(deviceName name)
{
	ret_code_t err_code;
	//���ò���
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
		//��ʼ�������ṩevent_handler,twi����ģʽ
	err_code =	nrfx_twi_init(&m_twi,&config,twi_handler,NULL);
	G_CHECK_ERROR_CODE_INFO(err_code);

	nrfx_twi_enable(&m_twi);

}

ret_code_t TwiDriverSend( uint8_t address, uint8_t * p_data, size_t length)
{
	return nrfx_twi_tx(&m_twi,address,p_data,length,true);
}


#define MPU6050_ADDRESS 0x68
#define WHO_AM_I_ADDRESS 0x75
/*
	IICͨ��,
	1������������ʼ�źţ����͵�7λ��������ַ+�ߵ�8λW/R,
	�ȴ�ACK,
	2�����ͼĴ�����ַ���ȴ�ACK
	3��W ����8λ����/ ����SCL, R ����8λ����

*/
void test(void)
{
	ret_code_t err = 0;

	uint8_t data_buff[1];
	uint8_t rx_data_buff[3] = {0};
	data_buff[0] = WHO_AM_I_ADDRESS;
	
	//err = TwiDriverSend(READ_CMD,data_buff,1);
       err =  nrfx_twi_tx(&m_twi,MPU6050_ADDRESS,data_buff,1,true);
	//NRF_LOG_INFO("twi %d",err);
       while(twi_done == false);
      //  nrfx_twi_tx(&m_twi,WRITE_CMD,data_buff,1,true);
	twi_done = false;
	err = nrfx_twi_rx(&m_twi,MPU6050_ADDRESS,rx_data_buff,3);
			 while(twi_done == false);
	//NRF_LOG_INFO("twi %d",err);
	NRF_LOG_INFO("rx_data_buff %d",rx_data_buff[0]);


}





/****************
* ������:void I2cSimulationInit(void)
* 
* ����:��ʼ��gpio��timer
*/
void I2cSimulationInit(void)
{
	//����gpio scl sad����
	nrf_gpio_cfg(SIMULATION_IIC_SCL_PIN,NRF_GPIO_PIN_DIR_OUTPUT,NRF_GPIO_PIN_INPUT_DISCONNECT,NRF_GPIO_PIN_PULLUP,NRF_GPIO_PIN_S0D1,NRF_GPIO_PIN_NOSENSE );
	nrf_gpio_cfg(SIMULATION_IIC_SDA_PIN,
							 NRF_GPIO_PIN_DIR_OUTPUT,
							 NRF_GPIO_PIN_INPUT_DISCONNECT,
							 NRF_GPIO_PIN_PULLUP,
							 NRF_GPIO_PIN_S0S1,
							 NRF_GPIO_PIN_NOSENSE
							);
	nrf_gpio_pin_write(SIMULATION_IIC_SCL_PIN,1);
	nrf_gpio_pin_write(SIMULATION_IIC_SDA_PIN,1);
}


void I2cSimulationSendByte(uint8_t data)
{
	uint8_t i=0;
	for(i=0;i<8;i++)
		{
			if((data&0x80) >> 7)	//MSB �ȷ�
			SDA_HIGH;
			else			
                        SDA_LOW;
			data <<= 1;
			nrf_delay_us(2);
                        
		}
}



