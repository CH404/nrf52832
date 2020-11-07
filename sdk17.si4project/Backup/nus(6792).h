#ifndef __NUS_H
#define __NUS_H
#define MY_QUEUE_ENABLE 1
#include <stdint.h>
#include "ble_gatts.h"
#include "my_queue.h"

#ifdef NUS_GLOBAL
#define NUS_ETX
#else
#define NUS_ETX	extern
#endif

#ifdef __cplusplus
extern "C" {
#endif



#define OPCODE_LENGTH        1
#define HANDLE_LENGTH        2

#define BLE_UARTS_MAX_DATA_LEN (NRF_SDH_BLE_GATT_MAX_MTU_SIZE - OPCODE_LENGTH - HANDLE_LENGTH)

#define RX_PIN_NUMBER 8
#define TX_PIN_NUMBER	6
#define RTS_PIN_NUMBER	7
#define CTS_PIN_NUMBER	5
#define UART_RX_BUF_SIZE	256
#define UART_TX_BUF_SIZE	256
#define BLE_UARTS_BUFF_MAX 25

//	实例化宏
#define BLE_UARTS_DEF(_name) \
static ble_uarts_t _name;			\
NRF_SDH_BLE_OBSERVER(_name ## _obs,	\
	BLE_NUS_BLE_OBSERVER_PRIO,		\
	ble_uarts_on_ble_evt,           \
	&_name)

//ble 接收数据
typedef struct
{
	uint8_t const *p_data;
	uint16_t 	   length;
}ble_uarts_evt_rx_data_t;


typedef enum
{
	BLE_UARTS_EVT_RX_DATA,
	BLE_UARTS_EVT_TX_RDY
}ble_uarts_evt_type_t;


typedef struct _ble_uarts_t ble_uarts_t;
typedef struct
{
	ble_uarts_evt_type_t		 type;		  /**< Event type. */
	ble_uarts_t 			   * p_uarts; 	  /**< A pointer to the instance. */
	uint16_t				   conn_handle; /**< Connection handle. */
	union
	{
		ble_uarts_evt_rx_data_t rx_data; /**< @ref BLE_NUS_EVT_RX_DATA event data. */
	} params;
} ble_uarts_evt_t;

typedef void (*ble_uarts_data_handler_t)(ble_uarts_evt_t *p_evt);

struct _ble_uarts_t
{
	uint16_t conn_handle;
	uint8_t uuid_type;
	uint16_t service_handler;
	ble_gatts_char_handles_t TX_handler;
	ble_gatts_char_handles_t RX_handler;
	ble_uarts_data_handler_t data_handler;
	
};

typedef struct
{
	ble_uarts_data_handler_t data_handler;
}ble_uarts_init_t;

NUS_ETX bool uart_RX_flage;
NUS_ETX bool ble_RX_flage;
void ble_uarts_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context);
NUS_ETX ret_code_t ble_uarts_init(ble_uarts_t *p_uarts,ble_uarts_init_t *p_uarts_init);

NUS_ETX void service_uarts_init(void);
NUS_ETX void uart_init(void);

NUS_ETX void data_transmit(void);

#ifdef __cplusplus
}			
#endif



#endif
