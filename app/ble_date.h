#ifndef __BLE_DATE_H
#define __BLE_DATE_H


#include <stdbool.h>
#include <stdint.h>
#include "ble.h"
#include "ble_srv_common.h"
#include "nrf_sdh_ble.h"
#include "common.h"

#ifdef BEL_DATE_GLOABAL
#define BLE_DARE_EXT
#else
#define BLE_DARE_EXT extern 
#endif

typedef struct ble_date_s ble_date_t;
struct ble_date_s
{
	bool				support_notification;      
	uint16_t 			rtc_service_handle;
	ble_gatts_char_handles_t rtc_characteristic_date_handle;
};

typedef struct
{
    bool                   support_notification;           /**< TRUE if notification of Battery Level measurement is supported. */
    security_req_t         bl_cccd_wr_sec;                 /**< Security requirement for writing the BL characteristic CCCD. */
    security_req_t 		   bl_write_sec;
}ble_date_init_t;




#define BLE_DATE_BLE_OBSERVER_PRIO 2


#define BLE_DATE_DEF(_name)\
static ble_date_t _name;\
NRF_SDH_BLE_OBSERVER(_name ## _obs,\
	BLE_DATE_BLE_OBSERVER_PRIO,\
        ble_date_on_ble_evt,\
        &_name)

BLE_DARE_EXT bool ble_date_notification_enabled;

BLE_DARE_EXT ret_code_t ble_date_init(void);
BLE_DARE_EXT ret_code_t ble_date_service_init(ble_date_t *p_date, ble_date_init_t *p_date_init);
BLE_DARE_EXT void ble_date_notification(uint8_t *data_buff,uint16_t data_length);
BLE_DARE_EXT void ble_date_notification_all(uint8_t *data_buff,uint16_t data_length);
void ble_date_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context);
void ble_date_lvl_on_reconnection_update(ble_date_t *p_date,uint8_t *data_buff,uint16_t data_length,uint16_t conn_hanle);
void ble_date_on_reconnection_update(ble_date_t *p_date,uint8_t *data_buff,uint16_t data_length,uint16_t conn_handle);
#endif


