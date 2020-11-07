/*
用于创建task
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
#endif

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
	uint8_t UI_Page = MAIN_PAGE;
	uint32_t notifyVakue,freemem;
	while(1)
		{
                //  LCD_RefreshMainPage(datebuff,timebuff);
		xTaskNotifyWait(0x00,ULONG_MAX,&notifyVakue,portMAX_DELAY);
		//判断是那个任务发送通知
		if((notifyVakue&(~MPU6050_TO_LCD_NOTIFY_BIT))== 0x00)			//如果notifyVakue包含多个通知，可以判断高优先级的执行，舍弃低优先级的
			{																												//~优先级2，&优先级8 小于 ==优先级7 所以加括号
				LCD_Display(false);	//显示
				xTimerStop(mpu_timer, portMAX_DELAY);	//stop mpu6050
			}
		
		 else if((notifyVakue&(~IT725_TO_LCD_NOTIFY_BIT)) == 0x00)
			{
				LCD_Refresh(datebuff,timebuff,&UI_Page,false);
			}
		else if((notifyVakue&(~RTC_TO_LCD_NOTIFY_BIT))  == 0x00)
			{
				if(UI_Page == MAIN_PAGE)	//主界面刷新时间
					//LCD_RefreshMainPage(datebuff,timebuff);
					LCD_Refresh(datebuff,timebuff,&UI_Page,true);
			}
		 else if((notifyVakue&(~ESP_TO_LCD_NOTIFY_BIT))  ==0x00)
			{
			
			}
              
		}
		
}


/**************************************************
*函数名:void MPU6050TimerBackCall(TimerHandle_t xTimer)
*说明:freertos 定时器回调函数，读取mpu数据
*参数:无
*返回值:无
***************************************************/
void BatteryTimerBackCall(TimerHandle_t xTimer)
{
	//检测电量
	//
}



/**************************************************
*函数名:void MPU6050TimerBackCall(TimerHandle_t xTimer)
*说明:freertos 定时器回调函数，读取mpu数据
*参数:无
*返回值:无
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
				return;		//水平角度太大，判断不是在一个水平
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
					data_convert(current_time,datebuff,timebuff);   //使用了sprintf ,在task 中使用了sprintf ,需要将此任务的堆地址设置成 8对齐
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
	mpu_timer = xTimerCreate("mpu",500,pdTRUE,(void*)1,MPU6050TimerBackCall);
	if(mpu_timer == NULL)
		{
			while(1);
		}
	//开始定时器
	if(pdFAIL ==xTimerStart(mpu_timer, 0))
							{
									while(1);
							}
		
#endif


#if USE_WATCH_DOG_TASK
	//创建喂狗task
	ret =	xTaskCreate(WdtFeedTask,"wdt",WDT_TASK_SIZE,NULL,WDT_TASK_PRIO,&wdtFeedTaskHanle);
        
		if(ret != pdPASS )
		{
			freemem =	xPortGetFreeHeapSize();
			while(1);
		}
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


#if USE_BATTERY_TASK
//用于定时检测电源电量,并更新出来
//用于画 ... (hrs开启)
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

