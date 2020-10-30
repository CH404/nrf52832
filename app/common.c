#define COMMON_GLOBAL
#include "common.h"
#include "nrfx_wdt.h"


uint16_t m_conn_handle= BLE_CONN_HANDLE_INVALID;


//void WdtFeedTask (void *paramenters);




/**************************************************
*����:����feed watchdog task,�� eventGroup����ι��
*����:��
*����ֵ:��
***************************************************/
/*bool WdtFeedTaskInit(void)
{
	BaseType_t ret;
		ret = xTaskCreate((TaskFunction_t)WdtFeedTask,
											(const char *) "wdt", 
											(uint16_t)WDT_TASK_SIZE,
											NULL,
											(UBaseType_t)WDT_TASK_PRIO,
											(TaskHandle_t *)wdtFeedTaskHanle);
		if(ret == pdPASS)
			{
				wdtFeedEventHandle = xEventGroupCreate();
				if(wdtFeedEventHandle != NULL)
					{
						NRF_LOG_INFO("wdt task success");
					return true;
					}
				else
					return false;
			}
		return false;
	
}

*/

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




