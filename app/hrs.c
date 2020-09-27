#ifndef BLE_HRS_ENABLED
#define HRS_GLOBAL
#include "global.h"

APP_TIMER_DEF(hrs_timer);
APP_TIMER_DEF(contact_timer);
BLE_HRS_DEF(m_hrs);
static void hrs_timerout_handler(void *p_context)
{
	UNUSED_PARAMETER(p_context);
	uint16_t rate = 330;
	ret_code_t err_code;
	
	err_code = ble_hrs_heart_rate_measurement_send(&m_hrs,rate);
	G_CHECK_ERROR_CODE_INFO(err_code);	
}

static void sensor_contact_timeout_handler(void *p_context)
{
	static bool sensor_contact_detected = false;
	UNUSED_PARAMETER(p_context);
	
	sensor_contact_detected = !sensor_contact_detected;
	ble_hrs_sensor_contact_detected_update(&m_hrs,sensor_contact_detected);
}

void hrs_timer_create()
{
	ret_code_t err_code;
	err_code = app_timer_create(&hrs_timer,APP_TIMER_MODE_REPEATED,hrs_timerout_handler);
	
	G_CHECK_ERROR_CODE_INFO(err_code);
	
	err_code = app_timer_create(&contact_timer,APP_TIMER_MODE_REPEATED,sensor_contact_timeout_handler);
}

void hrs_timer_start()
{
	ret_code_t err_code;

	app_timer_start(hrs_timer,LED_TOGGLE_INTERVAL, NULL);
	G_CHECK_ERROR_CODE_INFO(err_code);
	app_timer_start(contact_timer,contact_TOGGLE_INTERVAL, NULL);
	G_CHECK_ERROR_CODE_INFO(err_code);
}



/*****************************************************
函数名：void service_init(void)
参数：无
作用：服务初始化 包含his服务
******************************************************/
void service_his_init(void)
{
	//初始化his
	ret_code_t err_code;
	ble_hrs_init_t hrs_init;
	memset(&hrs_init,0,sizeof(hrs_init));
	
	uint8_t boby_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_WRIST;
	//读传感器位置特征值时的安全需求
	hrs_init.bsl_rd_sec = SEC_OPEN;
	hrs_init.hrm_cccd_wr_sec = SEC_OPEN;	//写CCCD(客户端特征描述符)时的安全需求
	//服务事件回调函数
	hrs_init.evt_handler =	NULL;
	//传感器接触检测支持标志
	hrs_init.is_sensor_contact_supported = true;
	//身体测点位置
	hrs_init.p_body_sensor_location = &boby_sensor_location;
	
	err_code = ble_hrs_init(&m_hrs,&hrs_init);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
}

#endif
