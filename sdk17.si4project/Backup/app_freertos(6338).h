#ifndef __APP_FREE_H
#define __APP_FREE_H
#include "FreeRTOS.h"
#include "semphr.h"
#include "timers.h"
#include "mpu6050.h"
#ifdef APP_FREE_GLOBAL
#define APP_FREE_EXT
#else
#define APP_FREE_EXT extern 
#endif

#define MPU6050_DATA_TASK_SIZE 256
#define MPU6050_DATA_TASK_PRIO 4

#define RTCUpdateSemMaxCount 1
#define RTCUpdateSemInitCount 0

#define RTC_UPDATE_TASK_SIZE 50
#define RTC_UPDATE_TASK_PRIO 2

#define WDT_TASK_SIZE 50
#define WDT_TASK_PRIO 5 			//喂狗优先级最高

#define NRF_SDH_FREERTOS_SOFTDERVICE_TASK_SIZE 256
#define NRF_SDH_FREERTOS_SOFTDERVICE_TASK_PRIO 4

APP_FREE_EXT void os_timer_init(void);
APP_FREE_EXT void os_timer_start(void);
APP_FREE_EXT bool task_init(void);






#endif

