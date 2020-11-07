#define COMMON_GLOBAL
#include "common.h"
#include "nrfx_wdt.h"


uint16_t m_conn_handle= BLE_CONN_HANDLE_INVALID;


/**************************************************
*作用:初始化硬件看门狗
*参数:无
*返回值:无
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




