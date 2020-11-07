#ifndef __SPI_H
#define __SPI_H
#include "stdint.h"

#define SWSPI
#ifdef SPI_GLOBAL
#define SPI_EXT 
#else
#define SPI_EXT extern
#endif

typedef uint32_t ret_code_t;

#define SPI_SDA_PIN  12
#define SPI_SCL_PIN 	10
#define SPI_DC_PIN		11
#define SPI_CS_PIN		9

#define SPI_CS_Select() nrf_gpio_pin_clear(SPI_CS_PIN)
#define SPI_CS_Release() nrf_gpio_pin_set(SPI_CS_PIN)
#define SPI_DATA_Select() nrf_gpio_pin_set(SPI_DC_PIN)
#define SPI_COMMAND_Select() nrf_gpio_pin_clear(SPI_DC_PIN)

SPI_EXT void SPI_HardWareInit(void);
SPI_EXT ret_code_t SPI_WriteData(uint8_t data);
SPI_EXT ret_code_t SPI_WriteCommand(uint8_t command);
SPI_EXT ret_code_t SPI_WriteDataLength(uint8_t *data,uint16_t length);
SPI_EXT uint8_t SPI_ReadByte(uint8_t command);
SPI_EXT void SPI_SoftWareInit(void);
SPI_EXT void SPI_WriteByte(uint8_t byte);
#endif