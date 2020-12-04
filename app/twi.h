#ifndef __TWI_H
#define __TWI_H

#include "nrfx_twi.h"

#ifdef TWI_GLOBAL
#define TWI_EXT
#else
#define TWI_EXT extern 
#endif

#define ERROR_I2C							0x00
#define SUCCESS_I2C						0x01
#define ERROR_ADDRESS_NO_ACK	ERROR_I2C
#define ERROR_TX_DATA_NO_ACK	ERROR_I2C


#define SCREEN_LCD_SDA_PIN 0
#define SCREEN_LCD_SCL_PIN 1

#define SCREEN_TOUCH_SDA_PIN 5
#define SCREEN_TOUCH_SCL_PIN 6

#define MPU6050_INT_PIN	25
#define MPU6050_SCL_PIN 26
#define MPU6050_SDA_PIN 27

#define MAX30102_SDA_PIN 21
#define MAX30102_SCL_PIN 20

#define TWI1_SDA_PIN MAX30102_SDA_PIN
#define TWI1_SCL_PIN MAX30102_SCL_PIN

#define TWI0_SDA_PIN MAX30102_SDA_PIN
#define TWI0_SCL_PIN MAX30102_SCL_PIN

#define SIMULATION_IIC_SCL_PIN MPU6050_SCL_PIN
#define SIMULATION_IIC_SDA_PIN MPU6050_SDA_PIN

#define GPIO_PORT_OUT (NRF_P0->OUT)
#define GPIO_PORT_DIR (NRF_P0->DIR)
#define GPIO_PORT_IN (NRF_P0->IN)
#define GPIO_PORT_CNF(x) (NRF_P0->PIN_CNF[x])

#define SDA_LOW()	  (GPIO_PORT_OUT &= ~(1<<SIMULATION_IIC_SDA_PIN))
#define SDA_HIGH() (GPIO_PORT_OUT |= (1<<SIMULATION_IIC_SDA_PIN))
#define SCL_LOW()	 (GPIO_PORT_OUT &= ~(1<<SIMULATION_IIC_SCL_PIN))
#define SCL_HIGH() (GPIO_PORT_OUT |= (1<<SIMULATION_IIC_SCL_PIN))

#define PIN_OUT_HIGH(x) GPIO_PORT_OUT_ADDR|(1<<x)
#define PIN_OUT_LOW(x)	GPIO_PORT_OUT_ADDR&(~(1<<x))

#define SDA_IN() GPIO_PORT_CNF(SIMULATION_IIC_SDA_PIN) = ((uint32_t)NRF_GPIO_PIN_DIR_INPUT<<GPIO_PIN_CNF_DIR_Pos)\
																												|((uint32_t)NRF_GPIO_PIN_PULLUP<<GPIO_PIN_CNF_PULL_Pos)\
																												|((uint32_t)NRF_GPIO_PIN_INPUT_CONNECT<<GPIO_PIN_CNF_INPUT_Pos)\
																												|((uint32_t)NRF_GPIO_PIN_S0S1<<GPIO_PIN_CNF_SENSE_Pos)\
																												|((uint32_t)NRF_GPIO_PIN_NOSENSE<<GPIO_PIN_CNF_SENSE_Pos)

#define SDA_OUT() GPIO_PORT_CNF(SIMULATION_IIC_SDA_PIN) = ((uint32_t)NRF_GPIO_PIN_DIR_OUTPUT<<GPIO_PIN_CNF_DIR_Pos)\
																											 |((uint32_t)NRF_GPIO_PIN_PULLUP<<GPIO_PIN_CNF_PULL_Pos)\
																											 |((uint32_t)NRF_GPIO_PIN_INPUT_DISCONNECT<<GPIO_PIN_CNF_INPUT_Pos)\
																											 |((uint32_t)NRF_GPIO_PIN_S0S1<<GPIO_PIN_CNF_SENSE_Pos)\
																											 |((uint32_t)NRF_GPIO_PIN_NOSENSE<<GPIO_PIN_CNF_SENSE_Pos)

#define READ_SDA  ((GPIO_PORT_IN>>SIMULATION_IIC_SDA_PIN)&0x01)

//#define SDA_IN() nrf_gpio_cfg_input(SIMULATION_IIC_SDA_PIN, NRF_GPIO_PIN_PULLUP)
//#define SDA_OUT() nrf_gpio_cfg_output(SIMULATION_IIC_SDA_PIN)
//#define READ_SDA nrf_gpio_pin_read(SIMULATION_IIC_SDA_PIN)

typedef enum
{
SCREEN_LCD = 0,			
SCREEN_TOUCH,	
MPU6050,
MAX30102
}deviceName;

//************************************IT7251 twi start*********************************************//
typedef struct
{
	uint8_t address;
	uint8_t *command;
	uint8_t command_length;
	uint8_t *receive_data;
	uint8_t recevice_length;
}TWI_IT725x_Config;
//************************************IT7251 twi end*********************************************//


//TWI_EXT ret_code_t TwiDriverInit(deviceName name);
//TWI_EXT ret_code_t TwiDriverSend( uint8_t address, uint8_t * p_data, size_t length);
//TWI_EXT void test(void);
TWI_EXT uint8_t I2c_Tx(uint8_t address, uint8_t const * pdata,uint16_t length,bool noStop,bool startAndAdd,uint8_t repetitionCnt);
TWI_EXT uint8_t I2c_Rx(uint8_t address, uint8_t *const  pdata,uint16_t length,uint8_t repetitionCnt);


TWI_EXT uint8_t I2c_Rx_OneByte(uint8_t address, uint8_t reg);
TWI_EXT uint8_t I2c_Tx_OneByte(uint8_t address, uint8_t reg,uint8_t data);

TWI_EXT void I2cSimulationInit(void);
TWI_EXT bool I2cSimulationSendByte(uint8_t data);
TWI_EXT void I2cSimulationStart(void);
TWI_EXT void I2cSimulationStop(void);
TWI_EXT bool I2cSimulationWaitAck(void);
TWI_EXT void I2cSimulationAck(void);
TWI_EXT void I2cSimulationNoAck(void);
TWI_EXT uint8_t I2cSimulationReadByte(bool ack);

#endif
