#ifndef __COMMON_H
#define __COMMON_H

#include <stdint.h>
#include <stdbool.h>
#include "ble_types.h"
#include "nrf_log.h"

#ifdef COMMON_GLOBAL
#define COMMON_EXT
#else
#define COMMON_EXT extern
#endif



#define ble_ser_is_notification_enable_request(date)	((uint16_t)((uint8_t*)date[0]) | ((uint16_t)((uint8_t*)date[1]) << 8))





COMMON_EXT uint16_t  m_conn_handle;  //ble 链接后的peer句柄,只存一个
COMMON_EXT void CommonWatchDogInit(void);
//COMMON_EXT bool WdtFeedTaskInit(void);







#define VERSION_DEBUG
#ifdef VERSION_DEBUG
#define G_CHECK_ERROR_CODE_INFO(err_code)    if(err_code != NRF_SUCCESS)\
{												\
	NRF_LOG_DEBUG("Function: %s error code: %s line: %d.",__func__,NRF_LOG_ERROR_STRING_GET(err_code),__LINE__);\
}
#else	//卡住等待看门狗复位
#define G_CHECK_ERROR_CODE_INFO(err_code)    if(err_code != NRF_SUCCESS)\
{                                                                        \
          while(1);                     \
}
#endif



#endif

