#ifndef __APP_FREE_H
#define __APP_FREE_H
#include "FreeRTOS.h"
#include "semphr.h"
#include "event_groups.h"

#include "timers.h"

#ifdef APP_FREE_GLOBAL
#define APP_FREE_EXT
#else
#define APP_FREE_EXT extern 
#endif

//mpu6050 定时器handle
APP_FREE_EXT TimerHandle_t MPU_TIMER;
//#define RTCUpdateSemMaxCount 1
//#define RTCUpdateSemInitCount 0
//APP_FREE_EXT SemaphoreHandle_t RTCUpdateSem;

//rtc task
#define RTC_UPDATE_TASK_SIZE 100	//使用uxTaskGetStackHighWaterMark 测得最小剩余8，几秒中内而已
#define RTC_UPDATE_TASK_PRIO 3
APP_FREE_EXT TaskHandle_t RTCUpdateTaskHandle;
void RTCUpdateTaskHandler(void *pvParamenters);


//watch dog task
#define WDT_TASK_SIZE 50
#define WDT_TASK_PRIO 5 			//喂狗优先级最高
APP_FREE_EXT TaskHandle_t wdtFeedTaskHanle;
void WdtFeedTask (void *paramenters);
// watch dog eventGroup handle
APP_FREE_EXT EventGroupHandle_t wdtFeedEventHandle;

#define LCD_REFRESH_TASK_SIZE 100
#define LCD_REFRESH_TASK_PRIO	2

APP_FREE_EXT TaskHandle_t lcdRefreshHandle;
void LcdRefreshTaskHandler(void* paramenters);



//rtc task eventGroup set bit
#define RTC_UPDATE_EVENT_BIT 0x01
//watch dog eventGroup set bit
#define WDT_FEED_EVT_WAIT_BIT  0x01

//协议栈task 
#define NRF_SDH_FREERTOS_SOFTDERVICE_TASK_SIZE 256
#define NRF_SDH_FREERTOS_SOFTDERVICE_TASK_PRIO 4

//start task 
#define START_TASK_SIZE 200
#define START_TASK_PRIO  1
APP_FREE_EXT TaskHandle_t startTaskHandle;
void StartTask(void* pvParameters);



//任务通知
#define ESP_TO_LCD_NOTIFY_BIT 0xFFFFFFF1
#define RTC_TO_LCD_NOTIFY_BIT 0xFFFFFFF2
#define IT725_TO_LCD_NOTIFY_BIT 0xFFFFFFF4
#define MPU6050_TO_LCD_NOTIFY_BIT 0xFFFFFFF8

#define RTC_TO_DOG_NOTIFY_BIT	0x01
#define MPU6050_TO_DOG_NOTIFY_BIT 0x02

#define FEED_DOG_ENABLE_BIT 0x03


APP_FREE_EXT void os_timer_init(void);
APP_FREE_EXT void os_timer_start(void);
APP_FREE_EXT bool task_init(void);



void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName);



#endif

