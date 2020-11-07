#ifndef __COMMON_H
#define __COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "ble_types.h"
#include "nrf_log.h"
#include "FreeRTOS.h"
#include "event_groups.h"

#ifdef COMMON_GLOBAL
#define COMMON_EXT
#else
#define COMMON_EXT extern
#endif



#define ble_ser_is_notification_enable_request(date)	((uint16_t)((uint8_t*)date[0]) | ((uint16_t)((uint8_t*)date[1]) << 8))



#define RTC_UPDATE_EVENT_BIT 0x01



#define WDTFEEDEVTWAITBIT  0x01
#define WDTTASKSIZE 50
#define WDTTASKPRIO 2

COMMON_EXT void WdtFeedTask (void *paramenters);

COMMON_EXT TaskHandle_t wdtFeedTaskHanle;
COMMON_EXT EventGroupHandle_t wdtFeedEventHandle;






COMMON_EXT uint16_t  m_conn_handle;  
COMMON_EXT void CommonWatchDogInit(void);
//COMMON_EXT bool WdtFeedTaskInit(void);






#define VERSION_DEBUG
#ifdef VERSION_DEBUG
#define G_CHECK_ERROR_CODE_INFO(err_code)    if(err_code != NRF_SUCCESS)\
{																									\
	NRF_LOG_DEBUG("Function: %s error code: %s line: %d.",__func__,NRF_LOG_ERROR_STRING_GET(err_code),__LINE__);\
}
#else
#define G_CHECK_ERROR_CODE_INFO(err_code)    if(err_code != NRF_SUCCESS)\																									\
          while(1);
#endif



#endif

