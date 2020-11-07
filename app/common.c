#define COMMON_GLOBAL
#include "common.h"
#include "nrfx_wdt.h"


uint16_t m_conn_handle= BLE_CONN_HANDLE_INVALID;


/**************************************************
*����:��ʼ��Ӳ�����Ź�
*����:��
*����ֵ:��
***************************************************/
void CommonWatchDogInit(void)
{
	ret_code_t err_code;
	nrfx_wdt_config_t config = NRFX_WDT_DEAFULT_CONFIG;
	config.reload_value = 49152;
	err_code = nrfx_wdt_init(&config,NULL);
	APP_ERROR_CHECK(err_code);
	nrfx_wdt_enable();
}




