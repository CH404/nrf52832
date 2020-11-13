
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

BLE_UARTS_DEF(m_uarts);  //����ʵ��
static uint8_t data_buff[BLE_UARTS_BUFF_MAX] = {0};		//����buff
queue_manager queue_mgr;															//���й�����
static uint8_t ble_data_buff[BLE_UARTS_BUFF_MAX] = {0};	//������������buff
uint8_t ble_data_len = 0;																//�������������ֽ���


/**********************************************************
��������static void ble_uarts_data_tx_handler(ble_uarts_evt_t *p_evt)
���룺
�����
���ã��Զ���event handler, ���ڽ��������ܵ������ݱ�������
**********************************************************/
static void ble_uarts_data_tx_handler(ble_uarts_evt_t *p_evt)
{
	if(p_evt->type == BLE_UARTS_EVT_RX_DATA)
	{
	uint8_t data_length = p_evt->params.rx_data.length;	//���յ��������ֽ���
	const uint8_t *p_data = (uint8_t*)p_evt->params.rx_data.p_data;	//����buff��ַ
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
��������on_write(ble_uarts_t * p_uarts,ble_evt_t const * p_ble_evt)
���룺
�����
���ã�����BLE_GATTS_EVT_WRITE�¼�����BLE����������ȡ��
**********************************************************/
static void on_write(ble_uarts_t * p_uarts,ble_evt_t const * p_ble_evt)
{
	ble_uarts_evt_t evt;	
	//ȡ���˴�BLE_GATTS_EVT_WRITE�¼��� �����ṹ��
	ble_gatts_evt_write_t const *p_evt_write = &p_ble_evt->evt.gatts_evt.params.write;
	memset(&evt,0,sizeof(ble_uarts_evt_t));
	evt.p_uarts = p_uarts;		// p_uarts��uarts ��ʵ��

	//һ����������һ�������������
	//һ�������ڵĲ����ɳ�Ϊ����
	//һ������Ҫ�о�������ͣ���ɣ�ֵ.
	if(p_evt_write->handle == p_uarts->RX_handler.value_handle)	//�ж�д�¼��ǲ��Ƿ�����RX_handler���characteristic
	{
		evt.type = BLE_UARTS_EVT_RX_DATA;	//����data_handler ��Ҫִ�е��¼�����
		evt.params.rx_data.length = p_evt_write->len;	//���յ����ݳ���
		evt.params.rx_data.p_data = p_evt_write->data;	//ָ����յ��������׵�ַ
		p_uarts-> data_handler(&evt);
		
	}
	
}


/**********************************************************
��������on_connect(ble_uarts_t * p_uarts,ble_evt_t const * p_ble_evt)
���룺
�����
���ã�connect event �����������������ӵ�handle���棬����Notification(֪ͨ)ʱʹ�� 
**********************************************************/
static void on_connect(ble_uarts_t * p_uarts,ble_evt_t const * p_ble_evt)
{
    p_uarts->conn_handle = p_ble_evt->evt.gap_evt.conn_handle;
}





/**********************************************************
��������uint16_t ble_uarts_data_send(ble_uarts_t *p_uarts,uint8_t *data,uint16_t length)
���룺p_uarts������� data������buff��ַ��length: �������ݳ���
�����ʵ�ʷ��ͳ��ȣ���λbyte��
���ã��������� Notification transmission ����
**********************************************************/
uint16_t ble_uarts_data_send(ble_uarts_t *p_uarts,uint8_t *data,uint16_t length)
{
	ble_gatts_hvx_params_t hvx_params;
	memset(&hvx_params,0,sizeof(hvx_params));

	hvx_params.handle = p_uarts->TX_handler.value_handle;
	hvx_params.p_data = data;
	hvx_params.p_len = &length;
	hvx_params.type = BLE_GATT_HVX_NOTIFICATION;	//ʹ��notification ֪ͨ
	sd_ble_gatts_hvx(p_uarts->conn_handle,&hvx_params);	//notification ����
	return length;
}

/**********************************************************
��������void ble_uarts_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context)
���룺
�����
���ã����յ�Э��ջ�¼��󣬽����event������
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
		case BLE_GATTS_EVT_WRITE:			//������BLE_GATTS_EVT_WRITE д�¼�
			on_write(p_uarts,p_ble_evt);
			break;
		case BLE_GATTS_EVT_HVN_TX_COMPLETE://�������
			break;
		default:
			break;
		}
}

/**********************************************************
��������ret_code_t ble_uarts_init(ble_uarts_t *p_uarts,ble_uarts_init_t *p_uarts_init)
���룺
�����
���ã���ʼ��uarts service,���service ��� characteristic 
**********************************************************/
ret_code_t ble_uarts_init(ble_uarts_t *p_uarts,ble_uarts_init_t *p_uarts_init)
{
	ret_code_t err_code = NRF_SUCCESS;
	ble_uuid_t ble_uuid;

	ble_add_char_params_t add_char_params;
	memset(&add_char_params,0,sizeof(ble_add_char_params_t));
	//�Զ���128λUUID
	VERIFY_PARAM_NOT_NULL(p_uarts);
	VERIFY_PARAM_NOT_NULL(p_uarts_init);
	p_uarts->data_handler = p_uarts_init->data_handler;	
	ble_uuid128_t uarts_base_uuid = UARTS_BASE_UUID;
	//��base uuid��ӵ� Э��ջ��
	err_code = sd_ble_uuid_vs_add(&uarts_base_uuid,&p_uarts->uuid_type);
	G_CHECK_ERROR_CODE_INFO(err_code)
	
	ble_uuid.type = p_uarts->uuid_type;
	ble_uuid.uuid = BLE_UUID_UARTS_SERVICE;
	//��ӷ���
	err_code = sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&ble_uuid,&p_uarts->service_handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
	//�������
	//�ṹ����0
	memset(&ble_uuid,0,sizeof(ble_uuid));
	//�������	uuid
	add_char_params.uuid = BLE_UUID_UARTS_RX_CHARACTERISTIC;
	add_char_params.uuid_type = p_uarts->uuid_type;
	add_char_params.max_len = BLE_UARTS_MAX_DATA_LEN;
	add_char_params.init_len = sizeof(uint8_t);
	add_char_params.is_var_len = true;
	//�������������� ֧��write
	add_char_params.char_props.write = 1;
	//����֧������Ӧд
	add_char_params.char_props.write_wo_resp = 1;
	//���������İ�ȫ���� ���ţ��ް�ȫ
	add_char_params.read_access = SEC_OPEN;
	add_char_params.write_access = SEC_OPEN;
	//Ϊ�˷����������
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
��������void service_uarts_init(void)
���룺
�����
���ã���ʼ��uarts service, ���У� uarts��ʹ����app_uarts��
**********************************************************/

void service_uarts_init(void)
{
	uart_RX_flage = false;
	ble_RX_flage = false;
	
	uint8_t error;
	ble_uarts_init_t uarts_init;
	uarts_init.data_handler = ble_uarts_data_tx_handler;
	ble_uarts_init(&m_uarts,&uarts_init);
	//��ʼ��queue
	error = my_queue_init(&queue_mgr,data_buff,BLE_UARTS_BUFF_MAX);
	if(error == QUEUE_SUCCESS)
	{
		NRF_LOG_INFO("QUEUE_SUCCESSe");
	}
	//��ʼ��app_uarts  ʹ��uarts0
	uart_init();
}


/**********************************************************
��������void uart_send(uint8_t *p_data,uint8_t length)
���룺
�����
���ã�uarts ����
**********************************************************/
void uart_send(uint8_t *p_data,uint8_t length)
{
	for(uint8_t i=0;i<length;i++)
	{
		app_uart_put(p_data[i]);
	}
}



/**********************************************************
��������static void uart_event_handler(app_uart_evt_t * p_uart_evt)	
���룺
�����
���ã�uarts event handler
**********************************************************/
static void uart_event_handler(app_uart_evt_t * p_uart_evt)	
{
	app_uart_evt_type_t evt;
	uint8_t data;

	evt = p_uart_evt->evt_type;
	if(evt == APP_UART_DATA_READY)
	{
		app_uart_get(&data);		//��fifo ��ȡ������ push ��queue
		if(my_queue_push(&queue_mgr,data) == QUEUE_SUCCESS)
			uart_RX_flage = true;
	}
}


/**********************************************************
��������void uart_init(void)	
���룺
�����
���ã���ʼ��uarts ģ��
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
��������void data_transmit(void)
���룺
�����
���ã����ڷ�������
**********************************************************/
void data_transmit(void)
{
	uint8_t uarts_rx_buff[BLE_UARTS_BUFF_MAX];
	uint16_t length = 1;
	uint8_t data;
	if(ble_RX_flage)
	{
		ble_RX_flage = false;
		uart_send(ble_data_buff,ble_data_len);	//�����ݷ��͵�uarts
	}
	if(uart_RX_flage)
	{
		uart_RX_flage = false;
		//length = my_queue_pull_all(&queue_mgr,uarts_rx_buff,0);
		//ble_uarts_data_send(&m_uarts,uarts_rx_buff,length);
		data = my_queue_pull(&queue_mgr);
		ble_uarts_data_send(&m_uarts,&data,length);//������notification ��ble
	}

}


#endif


