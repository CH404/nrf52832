#define BLE_DATE_GLOBAL
#include "ble_date.h"
#include "ble_conn_state.h"

BLE_DATE_DEF(m_date);
bool ble_date_notification_enabled = false;
/************************************************
说明:初始rtc时间服务
函数名:ret_code_t ble_date_init(void)
参数:
返回值:
**************************************************/
ret_code_t ble_date_init(void)
{
	ret_code_t err_code;
	ble_date_init_t date_init;
	date_init.bl_cccd_wr_sec = SEC_OPEN;
	date_init.bl_write_sec = SEC_MITM;
	date_init.support_notification = true;
	ble_date_service_init(&m_date,&date_init);
}

/************************************************
说明:初始化rtc 服务和特征
函数名:void update_time(void)
参数:
返回值:
**************************************************/
#define OPCODE_LENGTH 1
#define HANDLE_LENGTH 2
#define BEL_RTC_MAX_DATA_LEN NRF_SDH_BLE_GATT_MAX_MTU_SIZE-HANDLE_LENGTH-OPCODE_LENGTH

#define DATE_BASE_UUID {{0x9d,0x14,0xa9,0x7e,0x53,0x73,0x4a,0xb5,0x8f,0x58,0xc0,0xf5,0x00,0x00,0xd2,0x92}}
#define DATE_SERVICE_UUID	0xee42
#define DATE_CHAR_UUID  0xee41

ret_code_t ble_date_service_init(ble_date_t *p_date, ble_date_init_t *p_date_init)
{
	ret_code_t err_code = NRF_SUCCESS;
	ble_uuid_t ble_uuid;
	uint8_t uuid_type;
    uint8_t data_buff[7];
	ble_date_notification_enabled = false;
	ble_uuid128_t date_base_uuid = DATE_BASE_UUID;

	ble_add_char_params_t add_char_params;

	err_code = sd_ble_uuid_vs_add(&date_base_uuid,&uuid_type);
	G_CHECK_ERROR_CODE_INFO(err_code);

	ble_uuid.uuid = DATE_SERVICE_UUID;
	ble_uuid.type = uuid_type;
	sd_ble_gatts_service_add(BLE_GATTS_SRVC_TYPE_PRIMARY,&ble_uuid,&p_date->rtc_service_handle);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
        
       p_date->support_notification =  p_date_init->support_notification;
        

	memset(&add_char_params,0,sizeof(add_char_params));
	add_char_params.is_var_len = true;	//特征值的字节长度是否可变
	add_char_params.max_len = BEL_RTC_MAX_DATA_LEN;		//特征可传输的最大长度
	add_char_params.cccd_write_access =p_date_init->bl_cccd_wr_sec; 	    //写CCCD的安全要求，应该是server端使能或禁用时的安全
	//add_char_params.char_ext_props					//特征扩展属性，允许写入特征用户描述描述符，允许使用排队写入操作写入值
	//特征属性
//	add_char_params.char_props.broadcast = 1;//广播此特征
        
	add_char_params.char_props.notify  = p_date->support_notification; //使能通知
	add_char_params.char_props.write = 1; //使能可写
	add_char_params.write_access = p_date_init->bl_write_sec;
	add_char_params.init_len = sizeof(data_buff);							//初始化时的特征值长度
//	add_char_params.is_defered_read = true;						//indicate 是否延时读操作
//	add_char_params.is_defered_write = true;					//indicate 是否延时写操作
	add_char_params.p_init_value = data_buff;						//初始化时特征的值
//	add_char_params.p_presentation_format				//指向特征的格式，如果需要使用的话
//	add_char_params.p_user_descr						//指向用户描述
//	add_char_params.read_access = SEC_OPEN;							//读特征值的安全需求
	add_char_params.uuid = 	DATE_CHAR_UUID;
	add_char_params.uuid_type = uuid_type;

	err_code = characteristic_add(p_date->rtc_service_handle,&add_char_params,&p_date->rtc_characteristic_date_handle);
	G_CHECK_ERROR_CODE_INFO(err_code);
}



/************************************************
说明: notification a handle
函数名:void ble_date_notification(uint8_t *data_buff,uint16_t data_length)
参数:
返回值:
**************************************************/
void ble_date_notification(uint8_t *data_buff,uint16_t data_length)
{
	if(!ble_date_notification_enabled)
		return;
	ble_date_lvl_on_reconnection_update(&m_date,data_buff,data_length,m_conn_handle);
}

/************************************************
说明: notification all handle
函数名:void ble_date_notification(uint8_t *data_buff,uint16_t data_length)
参数:
返回值:
**************************************************/
void ble_date_notification_all(uint8_t *data_buff,uint16_t data_length)
{
	if(!ble_date_notification_enabled)
		return;
	ble_date_on_reconnection_update(&m_date,data_buff,data_length,BLE_CONN_HANDLE_ALL);
}


/************************************************
说明:notify 函数,发送到单个接收端
函数名:static void ble_date_lvl_on_reconnection_update(ble_date_t *p_date,uint8_t *data_buff,uint16_t data_length,uint16_t conn_hanle)
参数:p_date handle 
		 data_buff:数据buff
		 data_length: 
		 conn_hanle:接收端handle
返回值:
**************************************************/

static void ble_date_lvl_on_reconnection_update(ble_date_t *p_date,uint8_t *data_buff,uint16_t data_length,uint16_t conn_hanle)
{
	ble_gatts_hvx_params_t hvx_params;
	memset(&hvx_params,0,sizeof(hvx_params));
	if(p_date->support_notification)
		{
			hvx_params.handle =p_date->rtc_characteristic_date_handle.value_handle;
			hvx_params.p_data = data_buff;
			hvx_params.p_len = &data_length;
			hvx_params.type = BLE_GATT_HVX_NOTIFICATION;	//使用notification 通知
			sd_ble_gatts_hvx(conn_hanle,&hvx_params);	//notification 函数	
		}
}
/************************************************
说明:notify 函数,发送到多个接收端
函数名:void ble_date_on_reconnection_update(ble_date_t *p_date,uint8_t *data_buff,uint16_t data_length)
参数:p_date handle 
		 data_buff:数据buff
		 data_length: 
返回值:
**************************************************/
static	void ble_date_on_reconnection_update(ble_date_t *p_date,uint8_t *data_buff,uint16_t data_length,uint16_t conn_handle)
{
    ret_code_t err_code;
	if(p_date->support_notification)
		{
				ble_gatts_hvx_params_t hvx_params;
				hvx_params.handle = p_date->rtc_characteristic_date_handle.value_handle;
				hvx_params.p_data = data_buff;
				hvx_params.p_len = &data_length;
				hvx_params.type = BLE_GATT_HVX_NOTIFICATION;
				hvx_params.offset = 0;
		 if (conn_handle == BLE_CONN_HANDLE_ALL)
		  {
		      ble_conn_state_conn_handle_list_t conn_handles = ble_conn_state_conn_handles();

		      // Try sending notifications to all valid connection handles.
		      for (uint32_t i = 0; i < conn_handles.len; i++)
		      {
		          if (ble_conn_state_status(conn_handles.conn_handles[i]) == BLE_CONN_STATUS_CONNECTED)
		          {
		            sd_ble_gatts_hvx(conn_handles.conn_handles[i],&hvx_params);	//notification 函数	
		          }
		      }
		  }
		  else
		  {
		    sd_ble_gatts_hvx(conn_handle,&hvx_params);
		  }

		}

	}



	/************************************************
	说明: BLE_GATTS_EVT_WRITE 的回调函数
	函数名:void on_write(ble_rtcdate_t * p_date, ble_evt_t const * p_ble_evt)
	参数: p_ble_evt: ble event
				p_context: 指向date servcer 的handle
	返回值:
	**************************************************/
	
	static void on_write(ble_date_t * p_date, ble_evt_t const * p_ble_evt)
	{
		uint8_t data_buff[7];
		uint8_t i;
		ble_gatts_evt_write_t const *p_write_evt = &p_ble_evt->evt.gatts_evt.params.write;
		//特征值发生write event
		if(p_date->rtc_characteristic_date_handle.value_handle == p_write_evt->handle)
			{
			
			} //CCCD_hande write event
		else if((p_date->rtc_characteristic_date_handle.cccd_handle == p_write_evt->handle)
						&& (p_write_evt->len == 2))
			{
				//使能时接收到的 data[0] = 1 data[1]=0	
				//ble_srv_is_notification_enabled 返回为假，次API可能是错的
				//ble_srv_is_notification_enabled(p_write_evt->data);	
					if(ble_ser_is_notification_enable_request(p_write_evt->data))
					{
						//接收到的是使能命令
						ble_date_notification_enabled = true;	
					}
				else
					{
						ble_date_notification_enabled = false;
					}
			}
		else
			{
				return;
			}
	//	evt.conn_handle = p_ble_evt->evt.gatts_evt.conn_handle;
	//	p_date->evt_handler(p_ble_evt,&evt);
	
	}
	/************************************************
	说明:date service 的evt handler
	函数名:void ble_date_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context)
	参数: p_ble_evt: ble event
				p_context: 指向date servcer 的handle
	返回值:
	**************************************************/
	
static void ble_date_on_ble_evt(ble_evt_t const * p_ble_evt,void * p_context)
	{
		ble_date_t* p_date = (ble_date_t *)p_context;
		NRF_LOG_INFO("ble_DATE_on_ble_evt");
		switch(p_ble_evt->header.evt_id)
			{
				case BLE_GATTS_EVT_WRITE:
					on_write(p_date,p_ble_evt);
					break;				
			}
	}



