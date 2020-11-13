
#define NUS_GLOBAL
#include "app_uart.h"
#include "global.h"
#include "nrf_uart.h"

#if BLE_NUS_ENABLED
#define LOG_INPUT 0
#define UARTS_INPUT 1

#define UARTS_BASE_UUID                  {{0x99, 0xCA, 0x76, 0x24, 0x0E, 0xE5, 0xA9, 0xE0, 0x93, 0xF3, 0xA3, 0xB5, 0x00, 0x00, 0x40, 0x6E}} /**< Used vendor specific UUID. */

#define BLE_UUID_UARTS_SERVICE            0x0001                      /**< The UUID of the Nordic UART Service. */
#define BLE_UUID_UARTS_RX_CHARACTERISTIC  0x0002                      /**< The UUID of the RX Characteristic. */
#define BLE_UUID_UARTS_TX_CHARACTERISTIC  0x0003                      /**< The UUID of the TX Characteristic. */

BLE_UARTS_DEF(m_uarts);  //定义实例
static uint8_t data_buff[BLE_UARTS_BUFF_MAX] = {0};		//队列buff
queue_manager queue_mgr;															//队列管理句柄
static uint8_t ble_data_buff[BLE_UARTS_BUFF_MAX] = {0};	//蓝牙接收数据buff
uint8_t ble_data_len = 0;																//蓝牙接收数据字节数


/**********************************************************
函数名：static void ble_uarts_data_tx_handler(ble_uarts_evt_t *p_evt)
输入：
输出：
作用：自定义event handler, 用于将蓝牙接受到的数据保存起来
**********************************************************/
static void ble_uarts_data_tx_handler(ble_uarts_evt_t *p_evt)
{
	if(p_evt->type == BLE_UARTS_EVT_RX_DATA)
	{
	uint8_t data_length = p_evt->params.rx_data.length;	//接收到的数据字节数
	const uint8_t *p_data = (uint8_t*)p_evt->params.rx_data.p_data;	//数据buff地址
#if LOG_INPUT
	NRF_LOG_INFO("%s",p_evt->params.rx_data.p_data);
#elif UARTS_INPUT
	memcpy(&ble_data_buff,p_data,data_length);
	ble_data_len = data_length;
	ble_RX_flage = true;
#endif
	}
}



/**********************************************************
函数名：on_write(ble_uarts_t * p_uarts,ble_evt_t const * p_ble_evt)
输入：
输出：
作用：处理BLE_GATTS_EVT_WRITE事件，将BLE读到的数据取出
**********************************************************/
static void on_write(ble_uarts_t * p_uarts,ble_evt_t const * p_ble_evt)
{
	ble_uarts_evt_t evt;	
	//取出此次BLE_GATTS_EVT_WRITE事件的 参数结构体
	ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	memset(&evt,0,sizeof(ble_uarts_evt_t));
	evt.p_uarts = p_uarts;		// p_uarts：uarts 的实例

	//一个服务是由一个或多个特征组成
	//一个特征内的参数可称为属性
	//一个特征要有句柄，类型，许可，值.
	if(p_evt_write->handle == p_uarts->RX_handler.value_handle)	//判断写事件是不是发生在RX_handler这个characteristic
	{
		evt.type = BLE_UARTS_EVT_RX_DATA;	//设置data_handler 所要执行的事件类型
		evt.params.rx_data.length = p_evt_write->len;	//接收到数据长度
		evt.params.rx_data.p_data = p_evt_write->data;	//指向接收到的数据首地址
		p_uarts-> data_handler(&evt);
		
	}
	
}


/**********************************************************
函数名：on_connect(ble_uarts_t * p_uarts,ble_evt_t const * p_ble_evt)
输入：
输出：
作用：connect event 处理函数，将蓝牙链接的handle保存，用于Notification(通知)时使用 
**********************************************************/
static void on_connect(ble_uarts_t * p_uarts,ble_evt_t const * p_ble_evt)
{
    p_uarts->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}





/**********************************************************
函数名：uint16_t ble_uarts_data_send(ble_uarts_t *p_uarts,uint8_t *data,uint16_t length)
输入：p_uarts：句柄， data：数据buff地址，length: 发生数据长度
输出：实际发送长度（单位byte）
作用：用于蓝牙 Notification transmission 数据
**********************************************************/
uint16_t ble_uarts_data_send(ble_uarts_t *p_uarts,uint8_t *data,uint16_t length)
{
	ble_gatts_hvx_params_t hvx_params;
	memset(&hvx_params,0,sizeof(hvx_params));

	hvx_params.handle = p_uarts->TX_handler.value_handle;
	hvx_params.p_data = data;
	hvx_params.p_len = &length;
	hvx_params.type = BLE_GATT_HVX_NOTIFICATION;	//使用notification 通知
	sd_ble_gatts_hvx(p_uarts->conn_handle,&hvx_params);	//notification 函数
	return length;
}

/**********************************************************
函数名：void ble_uarts_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context)
输入：
输出：
作用：接收到协议栈事件后，进入此event处理函数
**********************************************************/
void ble_uarts_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context)
{
	//p_ble_evt :
	
	if((p_context == NULL)||(p_ble_evt == NULL))
		return;
	

	ble_uarts_t *p_uarts = (ble_uarts_t *)p_context;
	switch(p_ble_evt->header.evt_id)
		{
		case BLE_GAP_EVT_CONNECTED:
			on_connect(p_uarts,p_ble_evt);
			NRF_LOG_INFO("func:%s line:%d: event:CONNECTED",__func__,__LINE__);
			break;
		case BLE_GATTS_EVT_WRITE:			//发生了BLE_GATTS_EVT_WRITE 写事件
			on_write(p_uarts,p_ble_evt);
			break;
		case BLE_GATTS_EVT_HVN_TX_COMPLETE://发生完成
			break;
		default:
			break;
		}
}

/**********************************************************
函数名：ret_code_t ble_uarts_init(ble_uarts_t *p_uarts,ble_uarts_init_t *p_uarts_init)
输入：
输出：
作用：初始化uarts service,添加service 添加 characteristic 
**********************************************************/
ret_code_t ble_uarts_init(ble_uarts_t *p_uarts,ble_uarts_init_t *p_uarts_init)
{
	ret_code_t err_code = NRF_SUCCESS;
	ble_uuid_t ble_uuid;

	ble_add_char_params_t add_char_params;
	memset(&add_char_params,0,sizeof(ble_add_char_params_t));
	//自定义128位UUID
	VERIFY_PARAM_NOT_NULL(p_uarts);
	VERIFY_PARAM_NOT_NULL(p_uarts_init);
	p_uarts->data_handler = p_uarts_init->data_handler;	
	ble_uuid128_t uarts_base_uuid = UARTS_BASE_UUID;
	//将base uuid添加到 协议栈中
	err_code = sd_ble_uuid_vs_add(&uarts_base_uuid,&p_uarts->uuid_type);
	G_CHECK_ERROR_CODE_INFO(err_code)
	
	ble_uuid.type = p_uarts->uuid_type;
	ble_uuid.uuid = BLE_UUID_UARTS_SERVICE;
	//添加服务
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&ble_uuid,&p_uarts->service_handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
	//添加特征
	//结构体清0
	memset(&ble_uuid,0,sizeof(ble_uuid));
	//添加特征	uuid
	add_char_params.uuid = BLE_UUID_UARTS_RX_CHARACTERISTIC;
	add_char_params.uuid_type = p_uarts->uuid_type;
	add_char_params.max_len = BLE_UARTS_MAX_DATA_LEN;
	add_char_params.init_len = sizeof(uint8_t);
	add_char_params.is_var_len = true;
	//设置特征的性质 支持write
	add_char_params.char_props.write = 1;
	//设置支持无响应写
	add_char_params.char_props.write_wo_resp = 1;
	//设置特征的安全需求 开放，无安全
	add_char_params.read_access = SEC_OPEN;
	add_char_params.write_access = SEC_OPEN;
	//为此服务添加特性
	err_code = characteristic_add(p_uarts->service_handler,&add_char_params,&p_uarts->RX_handler);
	G_CHECK_ERROR_CODE_INFO(err_code);

	//add TX characteristic 
	memset(&add_char_params,0,sizeof(ble_add_char_params_t));
	add_char_params.uuid = BLE_UUID_UARTS_TX_CHARACTERISTIC;
	add_char_params.uuid_type = p_uarts->uuid_type;
	add_char_params.max_len = BLE_UARTS_MAX_DATA_LEN;
	add_char_params.init_len = sizeof(uint8_t);
	add_char_params.is_var_len = true;

	add_char_params.char_props.notify = 1;
	add_char_params.read_access = SEC_OPEN;
	add_char_params.write_access = SEC_OPEN;
	add_char_params.cccd_write_access = SEC_OPEN;
									
	err_code = characteristic_add(p_uarts->service_handler,&add_char_params,&p_uarts->TX_handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
	return err_code;
}



/**********************************************************
函数名：void service_uarts_init(void)
输入：
输出：
作用：初始化uarts service, 队列， uarts（使用了app_uarts）
**********************************************************/

void service_uarts_init(void)
{
	uart_RX_flage = false;
	ble_RX_flage = false;
	
	uint8_t error;
	ble_uarts_init_t uarts_init;
	uarts_init.data_handler = ble_uarts_data_tx_handler;
	ble_uarts_init(&m_uarts,&uarts_init);
	//初始化queue
	error = my_queue_init(&queue_mgr,data_buff,BLE_UARTS_BUFF_MAX);
	if(error == QUEUE_SUCCESS)
	{
		NRF_LOG_INFO("QUEUE_SUCCESSe");
	}
	//初始化app_uarts  使用uarts0
	uart_init();
}


/**********************************************************
函数名：void uart_send(uint8_t *p_data,uint8_t length)
输入：
输出：
作用：uarts 发送
**********************************************************/
void uart_send(uint8_t *p_data,uint8_t length)
{
	for(uint8_t i=0;i<length;i++)
	{
		app_uart_put(p_data[i]);
	}
}



/**********************************************************
函数名：static void uart_event_handler(app_uart_evt_t * p_uart_evt)	
输入：
输出：
作用：uarts event handler
**********************************************************/
static void uart_event_handler(app_uart_evt_t * p_uart_evt)	
{
	app_uart_evt_type_t evt;
	uint8_t data;

	evt = p_uart_evt->evt_type;
	if(evt == APP_UART_DATA_READY)
	{
		app_uart_get(&data);		//从fifo 中取出数据 push 到queue
		if(my_queue_push(&queue_mgr,data) == QUEUE_SUCCESS)
			uart_RX_flage = true;
	}
}


/**********************************************************
函数名：void uart_init(void)	
输入：
输出：
作用：初始化uarts 模块
**********************************************************/
void uart_init(void)
{
	uart_RX_flage = false;
	ret_code_t err_code;
	app_uart_comm_params_t const comm_params = {
		.rx_pin_no = RX_PIN_NUMBER,
		.tx_pin_no = TX_PIN_NUMBER,
		.rts_pin_no = RTS_PIN_NUMBER,
		.cts_pin_no = CTS_PIN_NUMBER,
		.flow_control = APP_UART_FLOW_CONTROL_DISABLED,
		.use_parity = false,
		.baud_rate = NRF_UART_BAUDRATE_115200
	};
	 APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_event_handler,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
}
/**********************************************************
函数名：void data_transmit(void)
输入：
输出：
作用：用于发送数据
**********************************************************/
void data_transmit(void)
{
	uint8_t uarts_rx_buff[BLE_UARTS_BUFF_MAX];
	uint16_t length = 1;
	uint8_t data;
	if(ble_RX_flage)
	{
		ble_RX_flage = false;
		uart_send(ble_data_buff,ble_data_len);	//将数据发送到uarts
	}
	if(uart_RX_flage)
	{
		uart_RX_flage = false;
		//length = my_queue_pull_all(&queue_mgr,uarts_rx_buff,0);
		//ble_uarts_data_send(&m_uarts,uarts_rx_buff,length);
		data = my_queue_pull(&queue_mgr);
		ble_uarts_data_send(&m_uarts,&data,length);//将数据notification 到ble
	}

}


#endif


