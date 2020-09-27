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
��������void service_init(void)
��������
���ã������ʼ�� ����his����
******************************************************/
void service_his_init(void)
{
	//��ʼ��his
	ret_code_t err_code;
	ble_hrs_init_t hrs_init;
	memset(&hrs_init,0,sizeof(hrs_init));
	
	uint8_t boby_sensor_location = BLE_HRS_BODY_SENSOR_LOCATION_WRIST;
	//��������λ������ֵʱ�İ�ȫ����
	hrs_init.bsl_rd_sec = SEC_OPEN;
	hrs_init.hrm_cccd_wr_sec = SEC_OPEN;	//дCCCD(�ͻ�������������)ʱ�İ�ȫ����
	//�����¼��ص�����
	hrs_init.evt_handler =	NULL;
	//�������Ӵ����֧�ֱ�־
	hrs_init.is_sensor_contact_supported = true;
	//������λ��
	hrs_init.p_body_sensor_location = &boby_sensor_location;
	
	err_code = ble_hrs_init(&m_hrs,&hrs_init);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
}

#endif
