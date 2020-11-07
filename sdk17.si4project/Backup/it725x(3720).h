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



#endif 



