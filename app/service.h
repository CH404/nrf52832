#ifndef __SERVICE_H
#define __SERVICE_H

#ifdef SERVICE_GLOBAL
#define SERVICE_EXT
#else
#define SERVICE_EXT	extern
#endif

#include "nrf_drv_rtc.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"




SERVICE_EXT void service_init(void);
SERVICE_EXT void service_his_init(void);
SERVICE_EXT void service_date_init(void);

SERVICE_EXT void service_dis_init(void);
SERVICE_EXT void bas_notification_send(uint8_t data,uint16_t conn_handle);
SERVICE_EXT void service_bas_init(void);

void ble_date_task_delete(void);
bool ble_date_task_init(void);
bool ble_date_semaphore_init(void);

#endif

