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


MPU6050_ETX TimerHandle_t MPU_TIMER;

#define RTCUpdateSemMaxCount 1
#define RTCUpdateSemInitCount 0

#define RTC_UPDATE_TASK_SIZE 50
#define RTC_UPDATE_TASK_PRIO 2
APP_FREE_EXT TaskHandle_t RTCUpdateTaskHandle;
void RTCUpdateTaskHandler(void *pvParamenters);


#define WDT_TASK_SIZE 50
#define WDT_TASK_PRIO 5 			//喂狗优先级最高
TaskHandle_t wdtFeedTaskHanle;
void WdtFeedTask (void *paramenters);

APP_FREE_EXT EventGroupHandle_t wdtFeedEventHandle;


#define NRF_SDH_FREERTOS_SOFTDERVICE_TASK_SIZE 256
#define NRF_SDH_FREERTOS_SOFTDERVICE_TASK_PRIO 4

#define START_TASK_SIZE 50
#define START_TASK_PRIO  0
APP_FREE_EXT TaskHandle_t startTaskHandle;
void StartTask(void* pvParameters);



APP_FREE_EXT void os_timer_init(void);
APP_FREE_EXT void os_timer_start(void);
APP_FREE_EXT bool task_init(void);






#endif

