#define SERVICE_GLOBAL
#include "global.h"
#include "rtc.h"
#include "ble_date.h"


NRF_BLE_QWR_DEF(m_qwr);
static void nrf_qwr_error_handler(uint32_t nrf_error)
{
    APP_ERROR_HANDLER(nrf_error);
}

/*****************************************************
函数名：void service_init(void)
参数：无
作用：服务初始化 无包含服务
******************************************************/
void service_init(void)
{
   ret_code_t err_code;
   nrf_ble_qwr_init_t qwr_init = {0};
	
   qwr_init.error_handler = nrf_qwr_error_handler;

   err_code = nrf_ble_qwr_init(&m_qwr, &qwr_init);

   G_CHECK_ERROR_CODE_INFO(err_code);
 //  service_date_init();

}

/*****************************************************
函数名：void service_date_init(void)
参数：无
作用：初始化date time 服务
******************************************************/
void service_date_init(void)
{
	ret_code_t err_code;
	err_code = ble_date_init();
	G_CHECK_ERROR_CODE_INFO(err_code);
}


#define START_TASK_SIZE 50
#define START_TASK_PRIO 2
TaskHandle_t StartTaskHandle;
void start_task(void *paramenters);

/*void start_task(void *paramenters)
{
	BaseType_t ret;
	taskENTER_CRITICAL();
	ret = xTaskCreate((TaskFunction_t)RTCUpdateTaskHandler,
								(const char *)"rtc", 
								(uint16_t) RTC_UPDATE_TASK_SIZE, 
								(void *) NULL,
								(UBaseType_t)RTC_UPDATE_TASK_PRIO,
								(TaskHandle_t *)&RTCUpdateTaskHandle);

		taskEXIT_CRITICAL();
			if(ret != pdPASS)
			NRF_LOG_INFO("start_task fist task faild");
}*/

void service_task_init(void)
{
//	rtc_task_init();

}



/*BLE_BAS_DEF(m_battery);
void service_bas_init(void)
{
    ret_code_t err_code;
	ble_bas_init_t battery_init;
	battery_init.bl_cccd_wr_sec = SEC_OPEN;
	battery_init.bl_rd_sec = SEC_OPEN;
	battery_init.bl_report_rd_sec = SEC_OPEN;
	battery_init.initial_batt_level = 50;
	battery_init.support_notification = true;
	battery_init.evt_handler = NULL;

	err_code = ble_bas_init(&m_battery,&battery_init);
	G_CHECK_ERROR_CODE_INFO(err_code);
}

void bas_notification_send(uint8_t data,uint16_t conn_handle)
{
        ble_bas_battery_level_update(&m_battery,data,BLE_CONN_HANDLE_ALL);
}*/








