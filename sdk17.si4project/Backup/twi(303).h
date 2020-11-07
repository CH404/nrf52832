#ifndef __TWI_H
#define __TWI_H

#include "nrfx_twi.h"

#ifdef TWI_GLOBAL
#define TWI_EXT
#else
#define TWI_EXT extern 
#endif

#define SCREEN_LCD_SDA_PIN 0
#define SCREEN_LCD_SCL_PIN 1

#define SCREEN_TOUCH_SDA_PIN 2
#define SCREEN_TOUCH_SCL_PIN 3
#define MPU6050_SCL_PIN 26
#define MPU6050_SDA_PIN 27
#define MAX30102_SDA_PIN 4
#define MAX30102_SCL_PIN 5


typedef enum
{
SCREEN_LCD = 0,			
SCREEN_TOUCH,	
MPU6050,
MAX30102
}deviceName;

TWI_EXT ret_code_t TwiDriverInit(deviceName name);
TWI_EXT ret_code_t TwiDriverSend( uint8_t address, uint8_t * p_data, size_t length);
TWI_EXT void test(void);

#endif