/*
���ڴ���task
*/
#define APP_FREE_GLOBAL
#include "app_freertos.h"
#include "global.h"


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


#define START_TASK_SIZE 50
#define START_TASK_PRIO  0
TaskHandle_t StartTask_Handle;
void Start_Task(void* pvParameters);

void Start_Task(void* pvParameters)
	{
	BaseType_t ret;
	uint32_t freemem;
		taskENTER_CRITICAL();

		RTCUpdateSem = xSemaphoreCreateCounting(RTCUpdateSemMaxCount,RTCUpdateSemInitCount);
                if(RTCUpdateSem == NULL)
									while(1);
		wdtFeedEventHandle = xEventGroupCreate();
								if(wdtFeedEventHandle == NULL)
									while(1);

		ret = xTaskCreate((TaskFunction_t)RTCUpdateTaskHandler,
														(const char *)"rtc",
														(uint16_t)RTC_UPDATE_TASK_SIZE,
														(void*)NULL,
														(UBaseType_t)RTC_UPDATE_TASK_PRIO,
														&RTCUpdateTaskHandle);
		if(ret != pdPASS )
		{
		freemem =	xPortGetFreeHeapSize();
		while(1);
		}
														
	/*ret =	xTaskCreate(WdtFeedTask,"wdt",WDT_TASK_PRIO,NULL,WDT_TASK_PRIO,&wdtFeedTaskHanle);
        
		if(ret != pdPASS )
		{
			freemem =	xPortGetFreeHeapSize();
			while(1);
		}*/
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
					(TaskHandle_t)&StartTask_Handle);
	if(ret == pdPASS)
		return true;
	else
		return false;
}






