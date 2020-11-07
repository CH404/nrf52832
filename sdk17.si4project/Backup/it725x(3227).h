#ifndef __IT725X_H
#define __IT725X_H
#include "stdint.h" 
typedef struct
{
	uint8_t address;
	uint8_t mode_regiseter;
	uint8_t *P_data;
	uint8_t data_length;
}IT725X_config_t;


#define QUERY_BUFFER 0x80
#define POINT_INFORMATION_BUFFER 0xE0
#define COMMAND_BUFFER 0x20
#define COMMAND_RESPONSE_BUFFER 0xA0

#define IT725X_SUCCESS 0x00



void IT725X_GetCommand(uint8_t *commandAndData,uint8_t length,uint8_t*receive,uint8_t len);
#endif 



