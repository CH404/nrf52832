/*
���ڴ���task
*/
#define APP_FREE_GLOBAL
#include "app_freertos.h"
//#include "global.h"

#include "mpu6050.h"
#include "rtc.h"
#include "ble_date.h"

#define DATE_TIMER_TICKS 1000
#define DATE_TIMER_WAIT 2	//��Ϣ���������£��ȴ����ٸ�tick
//ʵ����timer handle 
static TimerHandle_t m_date_timer;
static void date_timerout_handler(TimerHandle_t xTimer)
{
	UNUSED_PARAMETER(xTimer);
}
void os_timer_init(void)
{									
	m_date_timer = xTimerCreate("date_timer",//��ʱ������,���ڵ���
					DATE_TIMER_TICKS,		//��ʱʱ��
					pdTRUE,					//�Ƿ�ѭ��
					NULL,					//��ʱ��ID,�����timerʹ��һ��������ʱhandler����������timer
					date_timerout_handler);	//�ص�����handler
	if(m_date_timer == NULL)
	{
		NRF_LOG_ERROR("timer create error");	
	}
}
void os_timer_start(void)
{
	if(pdFAIL ==  xTimerStart(m_date_timer,DATE_TIMER_WAIT))//�ɹ�������ʱ��ǰ�����ȴ�ʱ�����ã���λϵͳʱ�ӽ��ģ���ʱ����Ĵ󲿷�API��������ֱ�����еģ���19.2С��˵����������ͨ����Ϣ���и���ʱ��������Ϣ��ʵ�ֵģ��˲������õĵȴ�ʱ����ǵ���Ϣ�����Ѿ���������£��ȴ���Ϣ�����пռ�ʱ�����ȴ�ʱ�䡣
		{
			NRF_LOG_ERROR("timer start error");	
		}

}

/**************************************************
*������:void MPU6050TimerBackCall(TimerHandle_t xTimer)
*˵��:freertos ��ʱ���ص���������ȡmpu����
*����:��
*����ֵ:��
***************************************************/
void MPU6050TimerBackCall(TimerHandle_t xTimer)
{
	MPU_Read();
}

/**************************************************
*����:freertos task feed watchdog
*����:��
*����ֵ:��
***************************************************/
void WdtFeedTask (void *paramenters)
{
	EventBits_t wdtFeedEvt = 0;
	while(1)
		{
			//xEventGroupGetBits(wdtFeedEventHandle);
			wdtFeedEvt = xEventGroupWaitBits(wdtFeedEventHandle,
			WDT_FEED_EVT_WAIT_BIT,pdTRUE,pdTRUE,portMAX_DELAY);
			if(wdtFeedEvt == WDT_FEED_EVT_WAIT_BIT)
					nrfx_wdt_feed();		
		}
}


/************************************************
˵��:����֪ͨrtc �Ѽ�8��(1s)
������:void update_time(void)
����:
����ֵ:
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
      xEventGroupSetBits(wdtFeedEventHandle,RTC_UPDATE_EVENT_BIT);
		}
}





void Start_Task(void* pvParameters)
	{
	BaseType_t ret;
	uint32_t freemem;
	taskENTER_CRITICAL();

#if USE_RTC_DATE_TASK
		RTCUpdateSem = xSemaphoreCreateCounting(RTCUpdateSemMaxCount,RTCUpdateSemInitCount);
                if(RTCUpdateSem == NULL)
									while(1);
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
	//ѭ��ÿ2000��tick һ�� 
	MPU_TIMER = xTimerCreate("mpu",2000,pdTRUE,(void*)1,MPU6050TimerBackCall);
	if(MPU_TIMER == NULL)
		{
			while(1);
		}
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
	ret =	xTaskCreate(WdtFeedTask,"wdt",WDT_TASK_SIZE,NULL,WDT_TASK_PRIO,&wdtFeedTaskHanle);
        
		if(ret != pdPASS )
		{
			freemem =	xPortGetFreeHeapSize();
			while(1);
		}
		
		wdtFeedEventHandle = xEventGroupCreate();
							if(wdtFeedEventHandle == NULL)
								while(1);

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






