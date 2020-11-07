#ifndef __IT725X_H
#define __IT725X_H

#ifdef IT725X_GLOBAL
#define IT725X_EXT 
#else
#define IT725X_EXT extern
#endif

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


enum dir_type
{
	not = 0,
	up,
	down,
	left,
	right
};
enum event_type
{
	tap = 0,
	drag	
};


typedef struct
{
	uint8_t x;
	uint8_t y;
}PointDate_t;

typedef struct
{
	PointDate_t start;
	PointDate_t end;
	uint8_t dir;
	uint8_t evt;
}ScreenEvent_t;

IT725X_EXT void IT725X_Init(void);
IT725X_EXT void IT725X_GetBuffData(uint8_t buffer_type,uint8_t*receive,uint8_t len);
IT725X_EXT void IT725X_GetCommand(uint8_t *commandAndData,uint8_t length,uint8_t*receive,uint8_t len);
IT725X_EXT uint16_t IT725X_SetCommand(uint8_t *commandAndData,uint8_t length);
IT725X_EXT void IT725X_InterruptInit(void);
IT725X_EXT void IT725X_HardwareInit(void);


void IT725X_GetCommand(uint8_t *commandAndData,uint8_t length,uint8_t*receive,uint8_t len);
#endif 



