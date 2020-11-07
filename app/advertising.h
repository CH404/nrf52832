#ifndef __ADVERTISING_H
#define __ADVERTISING_H

#ifdef ADVERTISING_GLOBAL
#define ADVERTISING_EXT
#else
#define ADVERTISING_EXT extern
#endif

#include "ble_advertising.h"


#define APP_ADV_SLOW_INTERVAL                                    500 //0.625 * 500 = 312.5ms                                  
#define APP_ADV_SLOW_DURATION                                    12000;   //1 mim   

#define APP_ADV_DURATION					2000							//   单位10ms
#define APP_ADV_INTERVAL					300	//The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms)
#define DEVICE_NAME					    	"RED_MI"

typedef enum{
CLOSE = 0,
OPEN
}advertisingButtonState;

static void on_adv_evt(ble_adv_evt_t ble_adv_evt);

//extern FontDef Font_16x26;
//extern FontDef Font_7x10;

ADVERTISING_EXT void advertising_code_init(void);		
ADVERTISING_EXT void advertising_init(void);
ADVERTISING_EXT void advertising_all_params_init(void); //都是广播参数初始化

ADVERTISING_EXT void advertising_start(void);
ADVERTISING_EXT void advertising_stop(void);
ADVERTISING_EXT void advertising_button(void * p_context);

ADVERTISING_EXT void advertising_start1(void);	//与advertising_start一样，只是直接调用协议栈API

#endif

