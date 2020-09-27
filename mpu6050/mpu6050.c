#define MPU6050_GLOBAL
#include "mpu6050.h"

uint8_t MPU_Read_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff)
{
	uint8_t result;

	result = I2c_Tx(address,reg,1,true,0);
	if(result!=SUCCESS_I2C)
	{
		return 1;
	}
	result = I2c_Rx(address,buff,length,0);
	if(result==ERROR_I2C)
	{
		return 1;
	}
return 0;
}

uint8_t MPU_Write_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff)
{
	uint8_t result;

		result = I2c_Tx(address,reg,1,true,0);
			if(result!=SUCCESS_I2C)
	{
		return 1;
	}
		result = I2c_Tx(address,buff,length,false,0);
						if(result!=SUCCESS_I2C)
	{
		return 1;
	}
						return 0;
}

