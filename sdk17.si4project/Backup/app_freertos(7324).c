/*
用于创建task
*/
#define APP_FREE_GLOBAL
#include "app_freertos.h"
#include "global.h"


#include "mpu6050.h"
#include "rtc.h"
#include "ble_date.h"

#define DATE_TIMER_TICKS 1000
#define DATE_TIMER_WAIT 2	//消息队列已满下，等待多少个tick
//实例化timer handle 
static TimerHandle_t m_date_timer;
static void date_timerout_handler(TimerHandle_t xTimer)
{
	UNUSED_PARAMETER(xTimer);
}
void os_timer_init(void)
{									
	m_date_timer = xTimerCreate("date_timer",//定时器名字,用于调试
					DATE_TIMER_TICKS,		//定时时间
					pdTRUE,					//是否循环
					NULL,					//定时器ID,当多个timer使用一个处理函数时handler，用于区分timer
					date_timerout_handler);	//回调函数handler
	if(m_date_timer == NULL)
	{
		NRF_LOG_ERROR("timer create error");	
	}
}
void os_timer_start(void)
{
	if(pdFAIL ==  xTimerStart(m_date_timer,DATE_TIMER_WAIT))//成功启动定时器前的最大等待时间设置，单位系统时钟节拍，定时器组的大部分API函数不是直接运行的（见19.2小节说明），而是通过消息队列给定时器任务发消息来实现的，此参数设置的等待时间就是当消息队列已经满的情况下，等待消息队列有空间时的最大等待时间。
		{
			NRF_LOG_ERROR("timer start error");	
		}

}

#define ULONG_MAX 0xFFFFFFFF
/**************************************************
*函数名:void MPU6050TimerBackCall(TimerHandle_t xTimer)
*说明:freertos 定时器回调函数，读取mpu数据
*参数:无
*返回值:无
***************************************************/
void LcdRefreshTaskHandler(void* paramenters)
{
	//等待任务通知
	uint32_t notifyVakue;
	xTaskNotifyWait(0x00,ULONG_MAX,&notifyVakue,portMAX_DELAY);
	//判断是那个任务发送通知
	if(notifyVakue & ~MPU6050_TO_LCD_NOTIFY_BIT)
		{
			LCD_Display(false);	//显示
		}
	else if(notifyVakue & ~IT725_TO_LCD_NOTIFY_BIT)
		{
			LCD_Refresh();
		}
}



/**************************************************
*函数名:void MPU6050TimerBackCall(TimerHandle_t xTimer)
*说明:freertos 定时器回调函数，读取mpu数据
*参数:无
*返回值:无
***************************************************/
void MPU6050TimerBackCall(TimerHandle_t xTimer)
{
	float last_pitch;
	float pitch,roll,yaw;
	//MPU_Read();
	if(!MPU_Read_data(pitch,roll,yaw))
		{
			if(last_pitch>20 && last_pitch < 80)
				{
					if(2 > pitch && pitch > -20)
						{
						xTaskNotify(LcdRefreshTaskHandler,(uint32_t)MPU6050_TO_LCD_NOTIFY_BIT,eSetBits);
						}
				}
		}
		
	xTaskNotify(wdtFeedTaskHanle,(uint32_t)MPU6050_TO_DOG_NOTIFY_BIT,eSetBits);
}

/**************************************************
*作用:freertos task feed watchdog
*参数:无
*返回值:无
***************************************************/
void WdtFeedTask (void *paramenters)
{
	uint32_t notifyValue;
	uint8_t feedDogEnable =0;
	while(1)
		{
			//xEventGroupGetBits(wdtFeedEventHandle);
		//	wdtFeedEvt = xEventGroupWaitBits(wdtFeedEventHandle,
		//	WDT_FEED_EVT_WAIT_BIT,pdTRUE,pdTRUE,portMAX_DELAY);

			xTaskNotifyWait(0x00,ULONG_MAX,&notifyValue,portMAX_DELAY);
			feedDogEnable |= (uint8_t)notifyValue;
			if(feedDogEnable == FEED_DOG_ENABLE_BIT)
				{
					nrfx_wdt_feed();
					feedDogEnable = 0;
				}
		}
}


/************************************************
说明:用于通知rtc 已计8次(1s)
函数名:void update_time(void)
参数:
返回值:
**************************************************/
void RTCUpdateTaskHandler(void *pvParamenters)
{
 
	uint8_t buff[7] = {0};
		while(1)
		{
			xSemaphoreTake(RTCUpdateSem,portMAX_DELAY);
			update_date();   
			data_convert(current_time,buff);
                     //   vTaskSuspend(NULL);
			ble_date_notification(buff,7);
    //  xEventGroupSetBits(wdtFeedEventHandle,RTC_UPDATE_EVENT_BIT);
    xTaskNotify(lcdRefreshHandle,(uint32_t)RTC_TO_LCD_NOTIFY_BIT,eSetBits);
    xTaskNotify(wdtFeedTaskHanle,(uint32_t)RTC_TO_DOG_NOTIFY_BIT,eSetBits);
		}
}





void Start_Task(void* pvParameters)
	{
	BaseType_t ret;
	uint32_t freemem;
	taskENTER_CRITICAL();

#if USE_RTC_DATE_TASK
	//创建rtc计数型信号量，接收rtc中断
		RTCUpdateSem = xSemaphoreCreateCounting(RTCUpdateSemMaxCount,RTCUpdateSemInitCount);
                if(RTCUpdateSem == NULL)
									while(1);
			//创建rtc task，用于将uint32 数据转换成 日期格式
		ret = xTaskCreate((TaskFunction_t)RTCUpdateTaskHandler,
															(const char *)"rtc",
															(uint16_t)RTC_UPDATE_TASK_SIZE,
															(void*)NULL,
															(UBaseType_t)RTC_UPDATE_TASK_PRIO,
															&RTCUpdateTaskHandle);
			if(ret != pdPASS )
			{
			freemem = xPortGetFreeHeapSize();
			while(1);
			}

#endif

#if USE_MPU6050_TASK		
	//创建定时器循环每n个tick 一次             //mpu6050读取数据过慢会导致mpu6050 DMP fifo 溢出
	MPU_TIMER = xTimerCreate("mpu",1000,pdTRUE,(void*)1,MPU6050TimerBackCall);
	if(MPU_TIMER == NULL)
		{
			while(1);
		}
	//开始定时器
	if(pdFAIL ==xTimerStart(MPU_TIMER, 0))
							{
									while(1);
							}
	/*MPURdyDateSem = xSemaphoreCreateCounting(RTCUpdateSemMaxCount,RTCUpdateSemInitCount);
										if(MPURdyDateSem == NULL)
											while(1);
		ret = xTaskCreate((TaskFunction_t)MPU_task,
														(const char *)"mpu",
														(uint16_t)MPU6050_DATA_TASK_SIZE,
														(void*)NULL,
														(UBaseType_t)MPU6050_DATA_TASK_PRIO,									
														&MPU_TASK);
		if(ret != pdPASS )
		{
		freemem =	xPortGetFreeHeapSize();
		while(1);
		}*/
		
#endif


#if USE_WATCH_DOG_TASK
	//创建喂狗task
	ret =	xTaskCreate(WdtFeedTask,"wdt",WDT_TASK_SIZE,NULL,WDT_TASK_PRIO,&wdtFeedTaskHanle);
        
		if(ret != pdPASS )
		{
			freemem =	xPortGetFreeHeapSize();
			while(1);
		}
/*		//创建事件标志组
		wdtFeedEventHandle = xEventGroupCreate();
							if(wdtFeedEventHandle == NULL)
								while(1);
*/
#endif	


#if USE_LCD_TASK
	//创建LCD 任务
	ret = xTaskCreate(LcdRefreshTaskHandler,"lcd",LCD_REFRESH_TASK_SIZE,NULL,LCD_REFRESH_TASK_PRIO,&lcdRefreshHandle);
	if(ret != pdPASS)
		{
			freemem = xPortGetFreeHeapSize();
			while (1);

		}
#endif



		freemem =	xPortGetFreeHeapSize();
    NRF_LOG_INFO("freemem %d",freemem);
		vTaskDelete(NULL);
		taskEXIT_CRITICAL();
		
	}


bool task_init(void)
{
	BaseType_t ret;

	ret = xTaskCreate((TaskFunction_t)Start_Task,
					(const char *)"Start_task",
					(uint16_t)START_TASK_SIZE,
					(void *)NULL,
					(UBaseType_t)START_TASK_PRIO,
					(TaskHandle_t)&startTaskHandle);
	if(ret == pdPASS)
		return true;
	else
		return false;
}






