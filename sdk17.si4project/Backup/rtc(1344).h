#ifndef __RTC_H
#define __RTC_H

#ifdef RTC_GLOBAL
#define RTC_EXT 
#else
#define RTC_EXT extern
#endif

#include "nrf_drv_rtc.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include <stdint.h>
#include <stdbool.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"

typedef struct
{
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hours;
	uint8_t minute;
	uint8_t second;
	uint8_t week;
}real_time_t;


#define RTCUpdateSemMaxCount 1
#define RTCUpdateSemInitCount 0
RTC_EXT SemaphoreHandle_t RTCUpdateSem;

#define RTC_UPDATE_TASK_SIZE 50
#define RTC_UPDATE_TASK_PRIO 2
RTC_EXT TaskHandle_t RTCUpdateTaskHandle;
RTC_EXT void RTCUpdateTaskHandler(void *pvParamenters);


RTC_EXT SemaphoreHandle_t RTCUpdateSem;
RTC_EXT TaskHandle_t RTCUpdateTaskHandle;

RTC_EXT void RTC2_init(nrfx_rtc_handler_t handler);
RTC_EXT void data_convert(real_time_t data,uint8_t * buff);
RTC_EXT void update_time(real_time_t *current_time,uint32_t real_time);
RTC_EXT void update_date(void);
RTC_EXT bool RTC_semaphore_init(void);
RTC_EXT void rtc_task_init(void);



#endif

