#define RTC_GLOBAL
#include "rtc.h"
#include "global.h"
#include "nrf_delay.h"

uint8_t month_table[12]={31,29,31,30,31,30,31,31,30,31,30,31};
real_time_t time_base = {
	.year = 2000,
    .month = 1,
    .day = 1,
    .hours = 0,
    .minute = 0,
    .second = 0,
    .week =   0,
};


static uint32_t real_time = 0;
const nrfx_rtc_t rtc2 = NRFX_RTC_INSTANCE(2);
/************************************************
说明:rtc2 handler
函数名:rtc2_handler(nrfx_rtc_int_type_t evt_type)
参数:evt_type: [in] rtc的event类型
返回值:
**************************************************/
static void rtc2_handler(nrfx_rtc_int_type_t evt_type)
{
	BaseType_t pxHigherPriorityTaskWoken;
	static uint8_t tick_cnt = 0;
	switch(evt_type)
	{
	case NRFX_RTC_INT_TICK:
	//	tick_event_handler();
		NRF_LOG_INFO("NRFX_RTC_INT_TICK");
		break;
	case NRFX_RTC_INT_COMPARE0:
		real_time++;
    nrfx_rtc_counter_clear(&rtc2);
		nrfx_rtc_cc_set(&rtc2,0, 8,true);
                //使用任务通知      
                NRF_LOG_INFO("NRFX_RTC_1S");
  //  vTaskNotifyGiveFromISR(RTCUpdateTaskHandle,&pxHigherPriorityTaskWoken);
		  xSemaphoreGiveFromISR(RTCUpdateSem, &pxHigherPriorityTaskWoken);
	portYIELD_FROM_ISR(pxHigherPriorityTaskWoken);
		break;
	}
}

/************************************************
说明:初始化rtc2
函数名:void RTC2_init(nrfx_rtc_handler_t handler)
参数:handler: [in] rtc的event处理函数
返回值:
**************************************************/
void RTC2_init(nrfx_rtc_handler_t handler)
{
	//初始化为sdk.config.h 里的参数
	nrfx_rtc_config_t config = NRFX_RTC_DEFAULT_CONFIG;	
	config.prescaler = 4095;
	if(handler == NULL)
		handler = rtc2_handler;
	ret_code_t err_code = nrfx_rtc_init(&rtc2,&config,handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
	//nrfx_rtc_enable(&rtc2);
  //  nrfx_rtc_tick_enable(&rtc2,true);  //使能rtc2 和 tick event
   err_code =  nrfx_rtc_cc_set(&rtc2,0,8,true);
	 G_CHECK_ERROR_CODE_INFO(err_code);
	// RTC_semaphore_init(); 	 		
}

void RTC2_enable(void)
{
	nrfx_rtc_enable(&rtc2);
}

/************************************************
说明:将real_time_t 转成 uint8_t
函数名:void data_convert(real_time_t data,uint8_t * buff)
参数:
返回值:
**************************************************/
void data_convert(real_time_t data,char * datebuff,char* timebuff)
{
#if 0
        uint8_t yearH,yearL;
        yearH = (uint8_t)(data.year / 100);
        yearL = (uint8_t)(data.year % 100);
  
	buff[0] = yearH;
	buff[1] = yearL;
	buff[2] = data.month;
	buff[3] = data.day;
	buff[4] = data.hours;
	buff[5] = data.minute;
	buff[6] = data.second;
	buff[7] = data.week;	
#endif
#if 1
	sprintf(datebuff,"%d.%d.%d",data.year,data.month,data.day);
	sprintf(timebuff,"%d:%d",data.hours,data.minute);
#endif

}

/************************************************
说明:将real_time换算成日期时间
函数名:void update_time(void)
参数:
返回值:
**************************************************/
void update_time(real_time_t *current_time,uint32_t real_time)
{
	   uint8_t month_ad = 0;
     uint8_t year_ad = 0;
     uint8_t i = 0;
     uint32_t hours_tmp  = 0U;
     uint32_t days_elapsed = 0U;
     uint16_t a_yes_days = 366U;
     uint16_t year_tmp = time_base.year;
     current_time->minute = (uint8_t)((real_time/60)%60);
     current_time->second = (uint8_t)(real_time%60U);
     hours_tmp = (real_time / 3600U);

     if(hours_tmp >= 24)
     {
         days_elapsed = (hours_tmp/24);
         current_time->hours = (hours_tmp%24);
     }
     else
         current_time->hours = hours_tmp;

     while (days_elapsed >= a_yes_days)
         {
             days_elapsed -= a_yes_days;
             year_tmp++;
             if((year_tmp%4 == 0)&&((year_tmp%100 !=0)||(year_tmp%400 == 0)))
             {
                 a_yes_days =366;
                 month_table[1] = 29;
                 time_base.year = year_tmp;
                 real_time -= ((real_time / 3600U)/24 - days_elapsed)*24U*3600U;
             }
             else
             {
                 a_yes_days = 365;
                 month_table[1] = 28;
             }
         }

     while(days_elapsed >= month_table[i])
         {
             days_elapsed-= month_table[i];
             i++;
         }
     current_time->day = time_base.day + days_elapsed;
     current_time->month = time_base.month+i;
     current_time->year = year_tmp;
    month_table[1] = 29;
    if(current_time->month < 3)
    {
        month_ad = 12;
        year_ad = 1;
    }
     current_time->week = (current_time->day+2*(current_time->month+month_ad)+
                          3*(current_time->month+1+month_ad)/5+
                          (current_time->year+year_ad)+
                          (current_time->year+year_ad)/4-
                          (current_time->year+year_ad)/100+
                          (current_time->year+year_ad)/400)%7;
}

/************************************************
说明:将real_time换算成日期时间
函数名:void update_time(void)
参数:
返回值:
**************************************************/
void update_date(void)
{
	 uint8_t month_ad = 0;
	 uint8_t year_ad = 0;
	 uint8_t i = 0;
	 uint32_t hours_tmp  = 0U;
	 uint32_t days_elapsed = 0U;
	 uint16_t a_yes_days = 366U;
	 uint16_t year_tmp = time_base.year;
	 uint32_t tmp_real_time = real_time;
	 current_time.minute = (uint8_t)((tmp_real_time/60)%60);
	 current_time.second = (uint8_t)(tmp_real_time%60U);
	 hours_tmp = (tmp_real_time / 3600U);
	
	 if(hours_tmp >= 24)
	 {
			 days_elapsed = (hours_tmp/24);
			 current_time.hours = (hours_tmp%24);
	 }
	 else
			 current_time.hours = hours_tmp;
	
	 while (days_elapsed >= a_yes_days)
			 {
					 days_elapsed -= a_yes_days;
					 year_tmp++;
					 if((year_tmp%4 == 0)&&((year_tmp%100 !=0)||(year_tmp%400 == 0)))
					 {
							 a_yes_days =366;
							 month_table[1] = 29;
							 time_base.year = year_tmp;
							 tmp_real_time -= ((tmp_real_time / 3600U)/24 - days_elapsed)*24U*3600U;
					 }
					 else
					 {
							 a_yes_days = 365;
							 month_table[1] = 28;
					 }
			 }
	
	 while(days_elapsed >= month_table[i])
			 {
					 days_elapsed-= month_table[i];
					 i++;
			 }
	 current_time.day = time_base.day + days_elapsed;
	 current_time.month = time_base.month+i;
	 current_time.year = year_tmp;
	month_table[1] = 29;
	if(current_time.month < 3)
	{
			month_ad = 12;
			year_ad = 1;
	}
	 current_time.week = (current_time.day+2*(current_time.month+month_ad)+
												3*(current_time.month+1+month_ad)/5+
												(current_time.year+year_ad)+
												(current_time.year+year_ad)/4-
												(current_time.year+year_ad)/100+
												(current_time.year+year_ad)/400)%7;

}


/*****************************freertos********************************************/


/*void rtc_task_init(void)
{
	BaseType_t ret;

	ret = xTaskCreate((TaskFunction_t)RTCUpdateTaskHandler,
								(const char *)"rtc", 
								(uint16_t) RTC_UPDATE_TASK_SIZE, 
								(void *) NULL,
								(UBaseType_t)RTC_UPDATE_TASK_PRIO,
								(TaskHandle_t *)RTCUpdateTaskHandle);
	if(ret != pdPASS)
			NRF_LOG_INFO("service_task_init faild");
}*/

/************************************************
说明:用于通知rtc 已计8次(1s)
函数名:void update_time(void)
参数:
返回值:
**************************************************/
/*	void RTCUpdateTaskHandler(void *pvParamenters)
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
*/
/************************************************
说明:创建freertos 计数型信号
函数名:void ble_date_task_init(void)
参数:
返回值:
**************************************************/
/*bool RTC_semaphore_init(void) 
{
	//创建计数信号量
	RTCUpdateSem = xSemaphoreCreateCounting(RTCUpdateSemMaxCount,RTCUpdateSemInitCount);
	if(RTCUpdateSem == NULL)
	{
		NRF_LOG_ERROR("Semaphore create error");
		return false;
	}
	return true;
}*/




