#ifndef __ADVERTISING_H
#define __ADVERTISING_H

#ifndef ADVERTISING_GLOBAL
#define ADVERTISING_EXT
#else
#define ADVERTISING_EXT extern
#endif

#include "ble_advertising.h"


#define APP_ADV_SLOW_INTERVAL                                    500 //0.625 * 500 = 312.5ms                                  
#define APP_ADV_SLOW_DURATION                                    12000;   //1 mim   

#define APP_ADV_DURATION					2000							//   µ¥Î»10ms
#define APP_ADV_INTERVAL					300	//The advertising interval (in units of 0.625 ms. This value corresponds to 187.5 ms)
#define DEVICE_NAME					    	"RED_MI"


ADVERTISING_EXT void advertising_init(void);
ADVERTISING_EXT void advertising_start(void);
static void on_adv_evt(ble_adv_evt_t ble_adv_evt);
ADVERTISING_EXT void advertising1_init(void);
ADVERTISING_EXT void advertising_start1(void);
ADVERTISING_EXT void advertising_all_params_init(void);

ADVERTISING_EXT void advertising_free_start(void);
ADVERTISING_EXT void advertising_free_stop(void);
#endif

