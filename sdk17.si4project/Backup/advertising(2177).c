#define ADVERTISING_GLOBAL
#include "global.h"
#include "ble_advdata.h"
//#include "ble_hrs.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"
BLE_ADVERTISING_DEF(m_advertising);
//BLE_HRS_DEF(m_hrs); 

/*****************************************************
��������static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
������ble_adv_evt �¼�
���ã��㲥�¼�������
******************************************************/
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
	switch(ble_adv_evt)
	{
		//���ٹ㲥�¼�
		case BLE_ADV_EVT_FAST:
			NRF_LOG_INFO("FAST advertising.");
			//ble_advertising_on_ble_evt();
			break;		
		case BLE_ADV_EVT_IDLE:
			NRF_LOG_INFO("advertising timeout.");
			break;
		case BLE_ADV_EVT_DIRECTED_HIGH_DUTY:
			NRF_LOG_INFO("directed advertising.");
			break;
		case BLE_ADV_EVT_DIRECTED:
			NRF_LOG_INFO("advertising BLE_ADV_EVT_DIRECTED.");
			break;
		case BLE_ADV_EVT_SLOW:
			NRF_LOG_INFO("advertising BLE_ADV_EVT_SLOW.");
			break;
		case BLE_ADV_EVT_FAST_WHITELIST:      /**< Fast advertising mode using the whitelist has started. */
			NRF_LOG_INFO("advertising BLE_ADV_EVT_FAST_WHITELIST.");
			break;
		case BLE_ADV_EVT_SLOW_WHITELIST:     /**< Slow advertising mode using the whitelist has started. */
			NRF_LOG_INFO("advertising BLE_ADV_EVT_SLOW_WHITELIST.");
			break;
		case BLE_ADV_EVT_WHITELIST_REQUEST:   /**< Request a whitelist from the main application. For whitelist advertising to work, the whitelist must be set when this event occurs. */
			NRF_LOG_INFO("advertising BLE_ADV_EVT_WHITELIST_REQUEST.");
			break;
		case BLE_ADV_EVT_PEER_ADDR_REQUEST:
			NRF_LOG_INFO("advertising BLE_ADV_EVT_PEER_ADDR_REQUEST.");
			break;
		default:
			NRF_LOG_INFO("default.");
			break;
	}
}

/*****************************************************
��������void advertising_start(void)
��������
���ã���ʼ�㲥
******************************************************/
void advertising_start(void)
{
    
      NRF_LOG_INFO("advertising_start");
      ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);     
      APP_ERROR_CHECK(err_code);
		
}
/*****************************************************
��������void advertising_start(void)
��������
���ã���ʼ�㲥
******************************************************/
void advertising_free_start(void)
{
	//UNUSED_PARAMETER(p_context);
   //   NRF_LOG_INFO("advertising_start");
      ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);     
      APP_ERROR_CHECK(err_code);
}

/*****************************************************
��������void advertising_free_stop(void)
��������
���ã��Ͽ����ӣ��رչ㲥
******************************************************/
void advertising_free_stop(void)
{
	//�Ͽ�����
	//����ж��ٽ�����handle
	uint32_t i;
	ble_conn_state_conn_handle_list_t conn_handle;
	//��ȡrole(��ɫ)�����豸��hanle
	conn_handle = ble_conn_state_central_handles();
	for(i=0;i<conn_handle.len;i++)
		{
			sd_ble_gap_disconnect(conn_handle.conn_handles[i],BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION);
		}
	sd_ble_gap_adv_stop(m_advertising.adv_handle);
}

/*****************************************************
��������void advertising_free_hook(void)
��������
����:��ΪSDH ��hook ��������ʼ���رչ㲥
******************************************************/
void advertising_button(void)
{
	bool button = false;

	if(button)
		advertising_free_stop();
	else
		advertising_free_start();
}

//�������UUID����
static ble_uuid_t m_adv_uuids[] = 
{
 {BLE_UUID_HEART_RATE_SERVICE,BLE_UUID_TYPE_BLE},
  {BLE_UUID_DEVICE_INFORMATION_SERVICE,BLE_UUID_TYPE_BLE}
};


/*****************************************************
��������void advertising_init(void)
��������
���ã��㲥��ʼ���������������㲥
******************************************************/
void advertising_init(void)
{
	ret_code_t err_code = NRF_SUCCESS;
	int8_t tx_power_level = 0;                              //����㲥����Ϊ0
	ble_advertising_init_t init;
	 memset(&init, 0, sizeof(init));
	//ȫ��
	init.advdata.name_type = BLE_ADVDATA_FULL_NAME;         //�㲥�����豸ȫ��
	init.advdata.include_appearance = true;                 //�㲥������ò����ò��gap������
	init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
        
	init.advdata.p_tx_power_level = &tx_power_level;        //�㲥��������
        
        
        init.advdata.uuids_complete.uuid_cnt = (sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]));
        init.advdata.uuids_complete.p_uuids = m_adv_uuids;
	
      //  NRF_LOG_INFO("%d",(sizeof(m_adv_uuids) ))
     //     while(1);
	init.config.ble_adv_fast_enabled = true;	//���ٹ㲥

	init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;	//�㲥���
	init.config.ble_adv_fast_timeout = APP_ADV_DURATION;	//�㲥����ʱ��,��ʱʱ��
	
	init.evt_handler = on_adv_evt;	//�㲥�ص�����
	
	err_code = ble_advertising_init(&m_advertising,&init);//��ʼ���㲥
	//NRF_LOG_INFO("%d",err_code);
	G_CHECK_ERROR_CODE_INFO(err_code);

	ble_advertising_conn_cfg_tag_set(&m_advertising,APP_BLE_CONN_CFG_TAG);
        
}

static uint8_t m_adv_handle = BLE_GAP_ADV_SET_HANDLE_NOT_SET;                   /**< Advertising handle used to identify an advertising set. */
static uint8_t m_enc_advdata[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
static uint8_t m_enc_scan_respone_data[BLE_GAP_ADV_SET_DATA_SIZE_MAX];
static ble_gap_adv_data_t m_adv_data = 
{
  .adv_data = 
  {
    .p_data = m_enc_advdata,
    .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX
  },
  .scan_rsp_data = 
  {
    .p_data = m_enc_scan_respone_data,
    .len = BLE_GAP_ADV_SET_DATA_SIZE_MAX
  }

};


/*****************************************************
��������void advertising1_init(void)
��������
���ã��㲥��ʼ���������������㲥���㲥��������
******************************************************/
void advertising1_init(void)
{
  //����㲥�����ṹ��
  ble_advdata_t advdata;
  ble_advdata_t srdata;
  int8_t tx_power_level = 100;
  ret_code_t err_code = NRF_SUCCESS;
  
  memset(&advdata,0,sizeof(advdata));
  advdata.name_type = BLE_ADVDATA_SHORT_NAME;
  advdata.include_appearance = true;
  advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  advdata.short_name_len = 3;
  advdata.p_tx_power_level = &tx_power_level;
  

  err_code = ble_advdata_encode(&advdata,m_adv_data.adv_data.p_data,&m_adv_data.adv_data.len);
  //G_CHECK_ERROR_CODE_INFO(err_code);
  
  memset(&srdata,0,sizeof(srdata));
  err_code = ble_advdata_encode(&srdata,m_adv_data.scan_rsp_data.p_data,&m_adv_data.scan_rsp_data.len);
 // G_CHECK_ERROR_CODE_INFO(err_code);
  
  ble_gap_adv_params_t adv_params;
  
  memset(&adv_params,0,sizeof(adv_params));
  
  //set gap adversting parameters 
  adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;     //�㲥ͨ�������������Զ�ѡ�� 
  adv_params.duration        = APP_ADV_DURATION;      //�㲥����ʱ��
  adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;//�㲥���ͣ������ӿ�ɨ�裬��Ŀ��
  adv_params.p_peer_addr     = NULL;            //�Ե��ַ
  adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;    //��������   �����κ��豸��ɨ��request������request
  adv_params.interval        = APP_ADV_INTERVAL;       //�㲥���
  
  err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
  NRF_LOG_INFO("error : %d",err_code);
  G_CHECK_ERROR_CODE_INFO(err_code);
  ble_advertising_conn_cfg_tag_set(&m_advertising,APP_BLE_CONN_CFG_TAG);
  
}
/*****************************************************
��������void advertising_start1(void)
��������
���ã������㲥��ֱ�ӵ��õ�Э��ջsd_ble_gap_adv_start����
******************************************************/
void advertising_start1(void)
{                               //�����㲥 ble_advertising_start�Ǵ˺����ķ�װ
   ret_code_t err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

 static void ble_adv_error_handler (uint32_t nrf_error)
 {
   
   NRF_LOG_INFO("%d:%s:%d",__LINE__,__func__,nrf_error);
   APP_ERROR_CHECK(nrf_error);
 }
/*****************************************************
��������void advertising1_init(void)
��������
���ã��㲥��ʼ���������������㲥���������㲥�ṹ�����
******************************************************/
void advertising_all_params_init(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  //define advertising struct
  ble_advertising_init_t init;
  memset(&init,0,sizeof(init));
  init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
  //�Ƿ������ò
  init.advdata.include_appearance = true;
  init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  
  //�㲥�����Ƿ��ٴΰ����豸��ַ
  init.advdata.include_ble_device_addr = false;
 /*     
  init.advdata.p_lesc_data = NULL;      //����NFC,BLE advertising set NULL
  init.advdata.p_tk_value = NULL;    //����NFC,BLE advertising set NULL
  init.advdata.p_sec_mgr_oob_flags = NULL;//����NFC,BLE advertising set NULL
  init.advdata.le_role = NULL;//����NFC,BLE advertising set NULL
   
  //�㲥�д���������
ble_advdata_service_data_t sever_data;
  memset(&sever_data,0,sizeof(sever_data));
  
  uint8_t data = 0x99;  //����
  sever_data.service_uuid = BLE_UUID_BATTERY_SERVICE;   //����UUID
  sever_data.data.size = sizeof(data);  //���ݴ�С
  sever_data.data.p_data = &data;        //����
  
  init.advdata.p_service_data_array = &sever_data;  //get ����     
  init.advdata.service_data_count = 1;  //length of service data 
  init.advdata.p_slave_conn_int = NULL; //�ӻ����Ӽ����Χ
  init.advdata.p_manuf_specific_data = NULL;    //�������Զ�������
  */
 
 //add service �����б�			
// init.advdata.uuids_more_available.uuid_cnt = (sizeof(m_adv_uuids)/sizeof(m_adv_uuids[0]))-1; //����UUID����
// init.advdata.uuids_more_available.p_uuids= m_adv_uuids;

 //����
//  init.advdata.uuids_complete.p_uuids = m_adv_uuids;
 // init.advdata.uuids_complete.uuid_cnt = (sizeof(m_adv_uuids)/sizeof(m_adv_uuids[0]));
 
 
 
  //error �ص�����
  init.error_handler = ble_adv_error_handler; 
  init.evt_handler = on_adv_evt;
  
  init.config.ble_adv_on_disconnect_disabled = true;  //�Ͽ����ӹ㲥ֹͣ����ֹ�Զ��㲥

	init.config.ble_adv_fast_enabled = true;	//���ٹ㲥
	init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;	//�㲥���
	init.config.ble_adv_fast_timeout = APP_ADV_DURATION;	//�㲥����ʱ��,��ʱʱ��

/*	init.config.ble_adv_directed_enabled = true;
	init.config.ble_adv_directed_interval = APP_ADV_INTERVAL;
    init.config.ble_adv_directed_timeout =  APP_ADV_DURATION;   */
  init.config.ble_adv_slow_enabled = true;
  init.config.ble_adv_slow_interval = APP_ADV_SLOW_INTERVAL;
  init.config.ble_adv_slow_timeout = APP_ADV_SLOW_DURATION;
  
  err_code = ble_advertising_init(&m_advertising,&init);
  G_CHECK_ERROR_CODE_INFO(err_code);

  ble_advertising_conn_cfg_tag_set(&m_advertising,APP_BLE_CONN_CFG_TAG);
}


