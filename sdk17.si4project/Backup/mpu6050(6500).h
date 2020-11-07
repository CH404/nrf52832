#ifndef __MPU6050_H
#define __MPU6050_H

#ifdef MPU6050_GLOBAL
#define MPU6050_ETX
#else
#define MPU6050_ETX extern
#endif

MPU6050_ETX uint8_t MPU_Read_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff);
MPU6050_ETX uint8_t MPU_Write_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff);

#endif

