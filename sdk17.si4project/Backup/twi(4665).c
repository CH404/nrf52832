/*
nrfx_err_t nrfx_twi_tx(nrfx_twi_t const * p_instance, twi���
                       uint8_t            address,	 ������ַ������Ҫ���8λ�Ӷ�дλ��API���
                       uint8_t const *    p_data,			���͵�����
                       size_t             length,			����
                       bool               no_stop);		�Ƿ����ֹͣλ
                       

*/

#define TWI_GLOBAL
#include "global.h"
#include "nrf_drv_twi.h"
#include "twi.h"
#include "nrfx_timer.h"
#include "nrf_delay.h"
//ʹ��twiģ��
//1��sdk_config.h ��ʹ��twi�Ŀ⺯��
//2��ʵ����twi handle
//3�����ó�ʼ��

#define TWI0_SDA_PIN 5
#define TWI0_SCL_PIN 6

#define HWTWI0
#ifdef HWTWI0
static const nrf_drv_twi_t twi0 = NRF_DRV_TWI_INSTANCE(0);
#endif
#ifdef HWTWI1
static const nrf_drv_twi_t twi1 = NRF_DRV_TWI_INSTANCE(1)
#endif
bool twi_done = false;
void twi_handler(nrf_drv_twi_evt_t const *p_evt,void *p_context)
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
˵��:TWI ��ʼ��
������:ret_code_t TwiInit(void)
����:
����ֵ:
**************************************************/
ret_code_t TWI_Init(void)
{
	nrf_drv_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
        ret_code_t err_code;
#ifdef HWTWI0
	config.scl = TWI0_SCL_PIN;
	config.sda = TWI0_SDA_PIN;
	config.frequency = NRF_DRV_TWI_FREQ_100K;
        config.interrupt_priority = APP_IRQ_PRIORITY_HIGH,
        config.clear_bus_init     = false;
	err_code = nrf_drv_twi_init(&twi0,&config,NULL,NULL);
  G_CHECK_ERROR_CODE_INFO(err_code);
	 nrf_drv_twi_enable(&twi0);
#endif
#ifdef HWTWI1
	config.scl = TWI1_SCL_PIN;
	config.sda = TWI1_SDA_PIN;
	nrf_drv_twi_init(&twi1,&config,NULL,NULL);
         nrf_drv_twi_enable(&twi1);
#endif 
}
ret_code_t TWI_Send(nrf_drv_twi_t const * p_instance,uint8_t address, uint8_t * p_data, size_t length)
{
	return nrf_drv_twi_tx(p_instance,address,p_data,length,true);
}

#endif

#define MPU6050_ADDRESS 0x68
#define CMD_WRITE 0xD0
#define CMD_READ 0xD1

#define WHO_AM_I_ADDRESS 0x75
#define IT725X_ADDRESS 0x46
/*
	IICͨ��,
	1������������ʼ�źţ����͵�7λ��������ַ+�ߵ�8λW/R,
	�ȴ�ACK,
	2�����ͼĴ�����ַ���ȴ�ACK
	3��W ����8λ����/ ����SCL, R ����8λ����

*/
#if 1
void test1(void)
{
	ret_code_t err = 0;
        uint8_t i;
	uint8_t rx_data_buff[10] = {0};
	uint8_t data_buff[] = {0x00};
	nrf_gpio_cfg_output(7);
	nrf_gpio_cfg_output(4);
	nrf_gpio_pin_set(7);
	nrf_gpio_pin_set(4);
        I2cSimulationInit();
        nrf_delay_ms(2000);
  TWI_Init();
  err =  nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,data_buff,1,true);
  
	//err = TwiDriverSend(READ_CMD,data_buff,1);
//    err =  nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,data_buff,0,true);
	NRF_LOG_INFO("twi %d",err);
 //     while(twi_done == false);
//	twi_done = false;
//	err = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,rx_data_buff,10);
//			 while(twi_done == false);
	//NRF_LOG_INFO("twi %d",err);
//	NRF_LOG_INFO("rx_data_buff %d",rx_data_buff[0]);


//����������ַ+д����
//���ͼĴ�����ַ
//����������ַ+������
//rx_data_buff[0] =  I2c_Rx_OneByte(IT725X_ADDRESS,0x00);
//NRF_LOG_INFO("rx_data_buff %d",rx_data_buff[0]);
//       I2c_Tx_OneByte(IT725X_ADDRESS,0x00,0x00);
/*I2c_Tx(MPU6050_ADDRESS,data_buff,1,true,1);
I2c_Rx(MPU6050_ADDRESS,rx_data_buff,3,1);
NRF_LOG_INFO("0x%x",rx_data_buff[0]);
NRF_LOG_INFO("0x%x",rx_data_buff[1]);
NRF_LOG_INFO("0x%x",rx_data_buff[2]);
	
	*/

								
}
#endif

/***************************************
* ������:uint8_t I2c_Tx_OneByte(uint8_t address, uint8_t reg,uint8_t data)
* ����: address ������ַ 
*				reg �Ĵ�����ַ
*				data ����
* ����:ģ��IIC ���͵��ֽ�����
* ����:�ɹ� SUCCESS_I2C
****************************************/
uint8_t I2c_Tx_OneByte(uint8_t address, uint8_t reg,uint8_t data)
{
	uint16_t i = 0;
	bool retAck = false;
	address = (address<<1|0);
	I2cSimulationStart();
	retAck = 	I2cSimulationSendByte(address);
		if(!retAck)
			return ERROR_ADDRESS_NO_ACK;
	
		retAck = I2cSimulationSendByte(reg);
		if(!retAck)
			return ERROR_TX_DATA_NO_ACK;

			retAck = I2cSimulationSendByte(data);
		if(!retAck)
			return ERROR_TX_DATA_NO_ACK;
		
   I2cSimulationStop();
	return SUCCESS_I2C;
}
/***************************************
* ������:uint8_t I2c_Rx_OneByte(uint8_t address, uint8_t reg)
* ����: address ������ַ 
*				reg �Ĵ�����ַ
*				data ����
* ����:ģ��IIC ���յ��ֽ�����
* ����:data ����
****************************************/
uint8_t I2c_Rx_OneByte(uint8_t address, uint8_t reg)
{
	uint8_t data;
        bool retAck;
	I2cSimulationStart();
	retAck = I2cSimulationSendByte(address<<1|0);
		if(!retAck)
			return ERROR_ADDRESS_NO_ACK;
	
		retAck = I2cSimulationSendByte(reg);
		if(!retAck)
			return ERROR_TX_DATA_NO_ACK;
                I2cSimulationStart();
			retAck = I2cSimulationSendByte(address<<1|1);
		if(!retAck)
			return ERROR_TX_DATA_NO_ACK;
		
		data = I2cSimulationReadByte(0);	//��������������Ҫ����Ӧ���					
 		I2cSimulationStop();
	return data;
}


/***************************************
* ������:uint8_t I2c_Tx(uint8_t address, uint8_t const * pdata,uint16_t length,bool noStop,uint8_t repetitionCnt)
* ����: address ������ַ 
*				pdata �������ݵ�ַ
*				length ���ݳ���
*				noStop �Ƿ���ֹͣλ
*				repetitionCnt ʧ���ط�
* ����:ģ��IIC ��������
* ����:�ɹ� SUCCESS_I2C
****************************************/
uint8_t I2c_Tx(uint8_t address, uint8_t const * pdata,uint16_t length,bool noStop,bool startAndAdd,uint8_t repetitionCnt)
{
	uint16_t i = 0;
	bool retAck = false;
        if(startAndAdd)
        {
	address = (address<<1|0);
I2C_TX_flags:	I2cSimulationStart();
	retAck = 	I2cSimulationSendByte(address);
	if(!retAck && repetitionCnt)
		{
			repetitionCnt--;
			goto I2C_TX_flags;
		}

		if(!retAck)
			return ERROR_ADDRESS_NO_ACK;
	}
	for(i=0;i<length;i++)
		{
		retAck = I2cSimulationSendByte(pdata[i]);
		if(!retAck)
			return ERROR_TX_DATA_NO_ACK;
		}

	if(!noStop)
   I2cSimulationStop();
	return SUCCESS_I2C;
}



/***************************************
* ������:uint8_t I2c_Rx(uint8_t address, uint8_t *const  pdata,uint16_t length,uint8_t repetitionCnt)
* ����: address ������ַ
*				pdata ��������buff
*				length �������ݳ���
*				repetitionCnt ʧ���ط�����
* ����:ģ��IIC ��������
* ����:�ɹ� �������ݳ���
****************************************/
uint8_t I2c_Rx(uint8_t address, uint8_t *const  pdata,uint16_t length,uint8_t repetitionCnt)
{
		uint16_t i = 0;
		bool result= false;
		bool retAck;
		if(length <= 0)
			return 0;
		
		address = (address << 1)|0x01;
I2C_RX_flags:I2cSimulationStart();
		retAck = I2cSimulationSendByte(address);
	 	if(!retAck && repetitionCnt) //������Ӧ��
	 		{

	 		repetitionCnt--;
	 			goto I2C_RX_flags;
	 		}
		if(!retAck)
			return ERROR_ADDRESS_NO_ACK;
		
		length--;
		do
			{
					pdata[i] = I2cSimulationReadByte(length);	//��������������Ҫ����Ӧ���					
					i++;
		}
		while (length-->0);
		return i;
//	return SUCCESS_I2C;
}


/***************************************
* ������:void I2cSimulationInit(void)
* 
* ����:��ʼ��gpio��timer
****************************************/
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


bool I2cSimulationSendByte(uint8_t data)
{
	uint8_t i=0;
	SDA_OUT();
	for(i=0;i<8;i++)
		{
			if((data&0x80) >> 7)	//MSB �ȷ�
			SDA_HIGH();
			else			
			SDA_LOW();
			
			data <<= 1;
			nrf_delay_us(5);
			SCL_HIGH();
			nrf_delay_us(5);
			SCL_LOW();
			nrf_delay_us(5);
		}
	return I2cSimulationWaitAck();

}
void I2cSimulationStart(void)
{
	SDA_OUT();
	SCL_HIGH();
	SDA_HIGH();
	nrf_delay_us(5);
	SDA_LOW();
	nrf_delay_us(5);
	SCL_LOW();

}
void I2cSimulationStop(void)
{
	SDA_OUT();
	SCL_LOW();
	SDA_LOW();
	nrf_delay_us(5);
	SCL_HIGH();
	SDA_HIGH();
	nrf_delay_us(5);
}
bool I2cSimulationWaitAck(void)
{
uint16_t waitAckTime = 0;
	SDA_HIGH();
	SDA_IN();
	nrf_delay_us(5);
	SCL_HIGH();
	nrf_delay_us(5);
	
	while (READ_SDA)
		{
			waitAckTime++;
			if(waitAckTime>65534)
				{
					I2cSimulationStop();
					return false;
				}
		}
	SCL_LOW();
	return true;
}

void I2cSimulationAck(void)
{
	SCL_LOW();
	SDA_OUT();
	SDA_LOW();		//SDA �Ӹ����ͣ�����ACK
	nrf_delay_us(5);
	SCL_HIGH();
	nrf_delay_us(5);
	SCL_LOW();
}

void I2cSimulationNoAck(void)
{
	SCL_LOW();
	SDA_OUT();
	SDA_HIGH();		//�����ͣ���ACK
	nrf_delay_us(5);
	SCL_HIGH();
	nrf_delay_us(5);
	SCL_LOW();
	
}
uint8_t I2cSimulationReadByte(bool ack)
{
	uint8_t  receiveDate = 0;
  uint8_t i;
	SDA_IN();
	for(i=0;i<8;i++)
		{
			SCL_LOW();
			nrf_delay_us(5);
			SCL_HIGH();
			receiveDate <<= 1;
			if(READ_SDA)
				receiveDate++;
			nrf_delay_us(5);
		}
	if(ack)
	I2cSimulationAck();	//Ӧ��
	else
		{
	I2cSimulationNoAck();	//Ӧ���
	I2cSimulationStop();
		}
	return receiveDate;
}



