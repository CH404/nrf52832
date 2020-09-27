#ifndef __HRS_H
#define __HRS_H

#include "ble_hrs.h"
#include "app_timer.h"

#ifdef HRS_GLOBAL
#define HRS_EXT
#else
#define HRS_EXT extern
#endif

#ifdef __cplusplus
extern "C" {
#endif


HRS_EXT void hrs_handler_callback(void *p_context);
HRS_EXT void service_his_init(void);
HRS_EXT void hrs_timer_start(void);
HRS_EXT	void hrs_timer_create(void);

#ifdef __cplusplus
}
#endif


#endif

