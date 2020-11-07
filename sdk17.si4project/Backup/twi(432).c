/*
nrfx_err_t nrfx_twi_tx(nrfx_twi_t const * p_instance, twi句柄
                       uint8_t            address,	 器件地址，不需要早高8位加读写位，API会加
                       uint8_t const *    p_data,			发送的数据
                       size_t             length,			长度
                       bool               no_stop);		是否产生停止位
                       

*/

#define TWI_GLOBAL
#include "global.h"
#include "nrf_drv_twi.h"
#include "twi.h"
#include "nrfx_timer.h"
#include "nrf_delay.h"
#include "nrf_drv_gpiote.h"
//使能twi模块
//1、sdk_config.h 中使能twi的库函数
//2、实例化twi handle
//3、配置初始化

#define TWI0_SDA_PIN 5
#define TWI0_SCL_PIN 6
#define INT_PIN 4

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
说明:TWI 初始化
函数名:ret_code_t TwiInit(void)
参数:
返回值:
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
	IIC通信,
	1、主机发送起始信号，发送低7位从器件地址+高第8位W/R,
	等待ACK,
	2、发送寄存器地址，等待ACK
	3、W 发送8位数据/ 放弃SCL, R 接收8位数据

*/
#if 1
uint8_t pointDataBuff[255];

void test_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  ret_code_t err_code;
  uint8_t tx2_buff[]=0x80;
  uint8_t rx_buff[2] = {0xA0};
uint8_t pointDataAdrees = 0xE0;
			
			if(action == NRF_GPIOTE_POLARITY_HITOLO)
				{
				NRF_LOG_INFO("HITOLO");

				}
			else if(action == NRF_GPIOTE_POLARITY_LOTOHI)
				{
				NRF_LOG_INFO("LOTOHI");
                                                               
                                err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,tx2_buff,1,true);
G_CHECK_ERROR_CODE_INFO(err_code);
err_code = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,rx_buff,1);
G_CHECK_ERROR_CODE_INFO(err_code);
if((rx_buff[0]&0x07) == 0x00)
{
	//NRF_LOG_INFO("data %d",rx_buff[0]);
	if(rx_buff[0]&0x80)	//new packet
		{
			NRF_LOG_INFO("new packet");
			err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,&pointDataAdrees,1,true);
			G_CHECK_ERROR_CODE_INFO(err_code);
			err_code = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,pointDataBuff,32);
			G_CHECK_ERROR_CODE_INFO(err_code);
			//NRF_LOG_INFO("read point data end");
                        NRF_LOG_INFO("x:%d,y:%d",pointDataBuff[2],pointDataBuff[4]);
                        NRF_LOG_INFO("head:%d",pointDataBuff[16]);
		}
	else if((rx_buff[0]&0x40))
		{
			//NRF_LOG_INFO("No packet but finger/pen still touched");
		}
}
else if((rx_buff[0]&0x07) == 0x01)
{
	NRF_LOG_INFO("IIC bus busy");
	nrf_delay_ms(500);
}
else
{
	NRF_LOG_INFO("IIC bus ERROR");
	while (1);
}
                                
                                
                                
                                
                                
				}
			else
				{
				NRF_LOG_INFO("TOGGLE");
				}
                        
}

void test1(void)
{
	ret_code_t err_code = 0;
        uint8_t i;
	uint8_t rx_data_buff[10] = {0};
	uint8_t data_buff[] = {0x20,0x00};
        uint8_t data1_buff[] = {0xA0};
	nrf_gpio_cfg_output(7);
//	nrf_gpio_cfg_output(4);
	nrf_gpio_pin_set(7);
//	nrf_gpio_pin_set(4);
       // I2cSimulationInit();
        nrf_delay_ms(2000);
  TWI_Init();


//发送命令 后再读
#if 0
  err_code =  nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,data_buff,2,false);
  G_CHECK_ERROR_CODE_INFO(err_code);
  err_code =  nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,data1_buff,1,true);
   G_CHECK_ERROR_CODE_INFO(err_code);
   err_code =  nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,rx_data_buff,10);
  G_CHECK_ERROR_CODE_INFO(err_code);
#endif

//中断读
//配置host中断
//使用gpiote
err_code = nrf_drv_gpiote_init();
nrf_drv_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_LOTOHI(true);
config.pull = NRF_GPIO_PIN_PULLUP;
err_code = nrf_drv_gpiote_in_init(INT_PIN,&config,test_handler);
G_CHECK_ERROR_CODE_INFO(err_code);
nrf_drv_gpiote_in_event_enable(INT_PIN,true);
//配置sensor中断
//uint8_t tx_buff[] = {0x20,0x01,0x04};
//uint8_t rx_buff[2] = {0xA0};

/*err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,tx_buff,3,false);
G_CHECK_ERROR_CODE_INFO(err_code);
err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,rx_buff,1,true);	//read interrupt information
G_CHECK_ERROR_CODE_INFO(err_code);
err_code = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,rx_buff,2);
G_CHECK_ERROR_CODE_INFO(err_code);*/

uint8_t tx1_buff[] = {0x20,0x02,0x04,0x01,0x01};
uint8_t rx1_buff[2] = {0xA0};
err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,tx1_buff,5,false);	//set interrupt
G_CHECK_ERROR_CODE_INFO(err_code);
err_code = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,rx1_buff,2);
G_CHECK_ERROR_CODE_INFO(err_code);

uint8_t tx_buff[] = {0x20,0x01,0x04};
uint8_t rx_buff[2] = {0xA0};
err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,tx_buff,3,false);
G_CHECK_ERROR_CODE_INFO(err_code);
err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,rx_buff,1,true);	//read interrupt information
G_CHECK_ERROR_CODE_INFO(err_code);
err_code = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,rx_buff,2);
NRF_LOG_INFO("%d",rx_buff[0]);
G_CHECK_ERROR_CODE_INFO(err_code);

uint8_t tx2_buff[]=0x80;
uint8_t pointDataAdrees = 0xE0;

while(1)
{
/*err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,tx2_buff,1,true);
G_CHECK_ERROR_CODE_INFO(err_code);
err_code = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,rx_buff,1);
G_CHECK_ERROR_CODE_INFO(err_code);
if((rx_buff[0]&0x07) == 0x00)
{
	//NRF_LOG_INFO("data %d",rx_buff[0]);
	if(rx_buff[0]&0x80)	//new packet
		{
			NRF_LOG_INFO("new packet");
			err_code = nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,&pointDataAdrees,1,true);
			G_CHECK_ERROR_CODE_INFO(err_code);
			err_code = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,pointDataBuff,28);
			G_CHECK_ERROR_CODE_INFO(err_code);
			NRF_LOG_INFO("read point data end");
		}
	else if((rx_buff[0]&0x40))
		{
			NRF_LOG_INFO("No packet but finger/pen still touched");
		}
}
else if((rx_buff[0]&0x07) == 0x01)
{
	NRF_LOG_INFO("IIC bus busy");
	nrf_delay_ms(500);
}
else
{
	NRF_LOG_INFO("IIC bus ERROR");
	while (1);
}*/
}








	
	//err = TwiDriverSend(READ_CMD,data_buff,1);
//    err =  nrf_drv_twi_tx(&twi0,IT725X_ADDRESS,data_buff,0,true);
//	NRF_LOG_INFO("twi %d",err_code);
 //     while(twi_done == false);
//	twi_done = false;
//	err = nrf_drv_twi_rx(&twi0,IT725X_ADDRESS,rx_data_buff,10);
//			 while(twi_done == false);
	//NRF_LOG_INFO("twi %d",err);
//	NRF_LOG_INFO("rx_data_buff %d",rx_data_buff[0]);


//发送器件地址+写命令
//发送寄存器地址
//发送器件地址+读命令
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
* 函数名:uint8_t I2c_Tx_OneByte(uint8_t address, uint8_t reg,uint8_t data)
* 参数: address 器件地址 
*				reg 寄存器地址
*				data 数据
* 功能:模拟IIC 发送单字节数据
* 返回:成功 SUCCESS_I2C
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
* 函数名:uint8_t I2c_Rx_OneByte(uint8_t address, uint8_t reg)
* 参数: address 器件地址 
*				reg 寄存器地址
*				data 数据
* 功能:模拟IIC 接收单字节数据
* 返回:data 数据
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
		
		data = I2cSimulationReadByte(0);	//主机接收完数据要发送应答非					
 		I2cSimulationStop();
	return data;
}


/***************************************
* 函数名:uint8_t I2c_Tx(uint8_t address, uint8_t const * pdata,uint16_t length,bool noStop,uint8_t repetitionCnt)
* 参数: address 器件地址 
*				pdata 发送数据地址
*				length 数据长度
*				noStop 是否发送停止位
*				repetitionCnt 失败重发
* 功能:模拟IIC 发送数据
* 返回:成功 SUCCESS_I2C
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
* 函数名:uint8_t I2c_Rx(uint8_t address, uint8_t *const  pdata,uint16_t length,uint8_t repetitionCnt)
* 参数: address 器件地址
*				pdata 接收数据buff
*				length 接收数据长度
*				repetitionCnt 失败重发次数
* 功能:模拟IIC 接收数据
* 返回:成功 接收数据长度
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
	 	if(!retAck && repetitionCnt) //发送无应答
	 		{

	 		repetitionCnt--;
	 			goto I2C_RX_flags;
	 		}
		if(!retAck)
			return ERROR_ADDRESS_NO_ACK;
		
		length--;
		do
			{
					pdata[i] = I2cSimulationReadByte(length);	//主机接收完数据要发送应答非					
					i++;
		}
		while (length-->0);
		return i;
//	return SUCCESS_I2C;
}


/***************************************
* 函数名:void I2cSimulationInit(void)
* 
* 功能:初始化gpio和timer
****************************************/
void I2cSimulationInit(void)
{
	//配置gpio scl sad拉高
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
			if((data&0x80) >> 7)	//MSB 先发
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
	SDA_LOW();		//SDA 从高拉低，产生ACK
	nrf_delay_us(5);
	SCL_HIGH();
	nrf_delay_us(5);
	SCL_LOW();
}

void I2cSimulationNoAck(void)
{
	SCL_LOW();
	SDA_OUT();
	SDA_HIGH();		//无拉低，无ACK
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
	I2cSimulationAck();	//应答
	else
		{
	I2cSimulationNoAck();	//应答非
	I2cSimulationStop();
		}
	return receiveDate;
}



