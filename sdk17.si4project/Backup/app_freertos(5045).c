/*
���ڴ���task
*/
#define APP_FREE_GLOBAL
#include "app_freertos.h"
#include "global.h"


#include "mpu6050.h"
#include "rtc.h"
#include "ble_date.h"

uint8_t datebuff[10] = {0};
uint8_t timebuff[6] = {0};

#if 0
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
#endif

#define ULONG_MAX 0xFFFFFFFF
/**************************************************
*������:void MPU6050TimerBackCall(TimerHandle_t xTimer)
*˵��:freertos ��ʱ���ص���������ȡmpu����
*����:��
*����ֵ:��
***************************************************/
void LcdRefreshTaskHandler(void* paramenters)
{
	//�ȴ�����֪ͨ
	uint8_t UI_Page = MAIN_PAGE;
	uint32_t notifyVakue,freemem;
	while(1)
		{
                //  LCD_RefreshMainPage(datebuff,timebuff);
		xTaskNotifyWait(0x00,ULONG_MAX,&notifyVakue,portMAX_DELAY);
		//�ж����Ǹ�������֪ͨ
		if((notifyVakue&(~MPU6050_TO_LCD_NOTIFY_BIT))== 0x00)			//���notifyVakue�������֪ͨ�������жϸ����ȼ���ִ�У����������ȼ���
			{																												//~���ȼ�2��&���ȼ�8 С�� ==���ȼ�7 ���Լ�����
				LCD_Display(false);	//��ʾ
				xTimerStop(mpu_timer, portMAX_DELAY);	//stop mpu6050
			}
		
		 else if((notifyVakue&(~IT725_TO_LCD_NOTIFY_BIT)) == 0x00)
			{
				LCD_Refresh(datebuff,timebuff,&UI_Page,false);
			}
		else if((notifyVakue&(~RTC_TO_LCD_NOTIFY_BIT))  == 0x00)
			{
				if(UI_Page == MAIN_PAGE)	//������ˢ��ʱ��
					//LCD_RefreshMainPage(datebuff,timebuff);
					LCD_Refresh(datebuff,timebuff,&UI_Page,true);
			}
		 else if((notifyVakue&(~ESP_TO_LCD_NOTIFY_BIT))  ==0x00)
			{
			
			}
              
		}
		
}


/**************************************************
*������:void MPU6050TimerBackCall(TimerHandle_t xTimer)
*˵��:freertos ��ʱ���ص���������ȡmpu����
*����:��
*����ֵ:��
***************************************************/
void BatteryTimerBackCall(TimerHandle_t xTimer)
{
	//������
	//
}



/**************************************************
*������:void MPU6050TimerBackCall(TimerHandle_t xTimer)
*˵��:freertos ��ʱ���ص���������ȡmpu����
*����:��
*����ֵ:��
***************************************************/
void MPU6050TimerBackCall(TimerHandle_t xTimer)
{
	static float last_pitch;
	float pitch,roll,yaw;
	//MPU_Read();
	if(!MPU_Read_data(&pitch,&roll,&yaw))
//	MPU_Read();
		{
		if(-20<roll && roll < 20)
			{
				return;		//ˮƽ�Ƕ�̫���жϲ�����һ��ˮƽ
			}
			if(last_pitch>20 && last_pitch < 80)
				{
					if(2 > pitch && pitch > -30)
						{
            NRF_LOG_INFO("fangwan");
						#if USE_LCD_TASK
						xTaskNotify(lcdRefreshHandle,(uint32_t)MPU6050_TO_LCD_NOTIFY_BIT,eSetBits);
						#endif
						}
				}
                        last_pitch = pitch;
		}
#if USE_WATCH_DOG_TASK
	xTaskNotify(wdtFeedTaskHanle,(uint32_t)MPU6050_TO_DOG_NOTIFY_BIT,eSetBits);
#endif
}

/**************************************************
*����:freertos task feed watchdog
*����:��
*����ֵ:��
***************************************************/
void WdtFeedTask (void *paramenters)
{
	uint32_t notifyValue;
	uint8_t feedDogEnable =0;
	while(1)
		{
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
˵��:����֪ͨrtc �Ѽ�8��(1s)
������:void update_time(void)
����:
����ֵ:
**************************************************/
void RTCUpdateTaskHandler(void *pvParamenters)
{
 uint32_t freemem;
	int8_t last_minute = -1;

	RTC2_enable();

		while(1)
		{
     ulTaskNotifyTake(pdTRUE,portMAX_DELAY);		 	
          update_date();
		//	ble_date_notification(buff,7);

		if(current_time.minute != last_minute)
			{
					last_minute = current_time.minute;
					#if USE_LCD_TASK
					data_convert(current_time,datebuff,timebuff);   //ʹ����sprintf ,��task ��ʹ����sprintf ,��Ҫ��������Ķѵ�ַ���ó� 8����
			    xTaskNotify(lcdRefreshHandle,(uint32_t)RTC_TO_LCD_NOTIFY_BIT,eSetBits);
					#endif
			}
    #if USE_WATCH_DOG_TASK
    xTaskNotify(wdtFeedTaskHanle,(uint32_t)RTC_TO_DOG_NOTIFY_BIT,eSetBits);
    #endif
		}
}



void Start_Task(void* pvParameters)
	{
	BaseType_t ret;
	uint32_t freemem;
	taskENTER_CRITICAL();

#if USE_RTC_DATE_TASK
			//����rtc task�����ڽ�uint32 ����ת���� ���ڸ�ʽ
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
	//������ʱ��ѭ��ÿn��tick һ��             //mpu6050��ȡ���ݹ����ᵼ��mpu6050 DMP fifo ���
	mpu_timer = xTimerCreate("mpu",500,pdTRUE,(void*)1,MPU6050TimerBackCall);
	if(mpu_timer == NULL)
		{
			while(1);
		}
	//��ʼ��ʱ��
	if(pdFAIL ==xTimerStart(mpu_timer, 0))
							{
									while(1);
							}
		
#endif


#if USE_WATCH_DOG_TASK
	//����ι��task
	ret =	xTaskCreate(WdtFeedTask,"wdt",WDT_TASK_SIZE,NULL,WDT_TASK_PRIO,&wdtFeedTaskHanle);
        
		if(ret != pdPASS )
		{
			freemem =	xPortGetFreeHeapSize();
			while(1);
		}
#endif	


#if USE_LCD_TASK
	//����LCD ����
	ret = xTaskCreate(LcdRefreshTaskHandler,"lcd",LCD_REFRESH_TASK_SIZE,NULL,LCD_REFRESH_TASK_PRIO,&lcdRefreshHandle);
	if(ret != pdPASS)
		{
			freemem = xPortGetFreeHeapSize();
			while (1);

		}
#endif


#if USE_BATTERY_TASK
//���ڶ�ʱ����Դ����,�����³���
//���ڻ� ... (hrs����)
	//battery_timer = xTimerCreate("bty",1000,pdTRUE,(void*)2,)

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

#if 1
void vApplicationStackOverflowHook(TaskHandle_t xTask, char * pcTaskName)
{
	NRF_LOG_INFO("overflow: %s",pcTaskGetName);
	if(xTask == RTCUpdateTaskHandle)
		{
			NRF_LOG_INFO("RTCUpdateTaskHandle");
		}
	else if(xTask == lcdRefreshHandle)
		{
		NRF_LOG_INFO("lcdRefreshHandle");
		}
	else
		{
		NRF_LOG_INFO("%p",xTask);
		}
}

#endif

