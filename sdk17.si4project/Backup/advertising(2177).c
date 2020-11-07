#define ADVERTISING_GLOBAL
#include "global.h"
#include "ble_advdata.h"
//#include "ble_hrs.h"
#include "ble_advertising.h"
#include "ble_conn_state.h"
BLE_ADVERTISING_DEF(m_advertising);
//BLE_HRS_DEF(m_hrs); 

/*****************************************************
函数名：static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
参数：ble_adv_evt 事件
作用：广播事件处理函数
******************************************************/
static void on_adv_evt(ble_adv_evt_t ble_adv_evt)
{
	switch(ble_adv_evt)
	{
		//快速广播事件
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
函数名：void advertising_start(void)
参数：无
作用：开始广播
******************************************************/
void advertising_start(void)
{
    
      NRF_LOG_INFO("advertising_start");
      ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);     
      APP_ERROR_CHECK(err_code);
		
}
/*****************************************************
函数名：void advertising_start(void)
参数：无
作用：开始广播
******************************************************/
void advertising_free_start(void)
{
	//UNUSED_PARAMETER(p_context);
   //   NRF_LOG_INFO("advertising_start");
      ret_code_t err_code = ble_advertising_start(&m_advertising, BLE_ADV_MODE_FAST);     
      APP_ERROR_CHECK(err_code);
}

/*****************************************************
函数名：void advertising_free_stop(void)
参数：无
作用：断开链接，关闭广播
******************************************************/
void advertising_free_stop(void)
{
	//断开链接
	//检测有多少接链接handle
	uint32_t i;
	ble_conn_state_conn_handle_list_t conn_handle;
	//获取role(角色)中心设备的hanle
	conn_handle = ble_conn_state_central_handles();
	for(i=0;i<conn_handle.len;i++)
		{
			sd_ble_gap_disconnect(conn_handle.conn_handles[i],BLE_HCI_LOCAL_HOST_TERMINATED_CONNECTION);
		}
	sd_ble_gap_adv_stop(m_advertising.adv_handle);
}

/*****************************************************
函数名：void advertising_free_hook(void)
参数：无
作用:作为SDH 的hook 函数，开始，关闭广播
******************************************************/
void advertising_button(void)
{
	bool button = false;

	if(button)
		advertising_free_stop();
	else
		advertising_free_start();
}

//定义服务UUID数组
static ble_uuid_t m_adv_uuids[] = 
{
 {BLE_UUID_HEART_RATE_SERVICE,BLE_UUID_TYPE_BLE},
  {BLE_UUID_DEVICE_INFORMATION_SERVICE,BLE_UUID_TYPE_BLE}
};


/*****************************************************
函数名：void advertising_init(void)
参数：无
作用：广播初始化，不包括启动广播
******************************************************/
void advertising_init(void)
{
	ret_code_t err_code = NRF_SUCCESS;
	int8_t tx_power_level = 0;                              //定义广播功耗为0
	ble_advertising_init_t init;
	 memset(&init, 0, sizeof(init));
	//全名
	init.advdata.name_type = BLE_ADVDATA_FULL_NAME;         //广播包含设备全名
	init.advdata.include_appearance = true;                 //广播包含外貌，外貌在gap里设置
	init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
        
	init.advdata.p_tx_power_level = &tx_power_level;        //广播包含功耗
        
        
        init.advdata.uuids_complete.uuid_cnt = (sizeof(m_adv_uuids) / sizeof(m_adv_uuids[0]));
        init.advdata.uuids_complete.p_uuids = m_adv_uuids;
	
      //  NRF_LOG_INFO("%d",(sizeof(m_adv_uuids) ))
     //     while(1);
	init.config.ble_adv_fast_enabled = true;	//快速广播

	init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;	//广播间隔
	init.config.ble_adv_fast_timeout = APP_ADV_DURATION;	//广播持续时间,超时时间
	
	init.evt_handler = on_adv_evt;	//广播回调函数
	
	err_code = ble_advertising_init(&m_advertising,&init);//初始化广播
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
函数名：void advertising1_init(void)
参数：无
作用：广播初始化，不包括启动广播，广播包含服务
******************************************************/
void advertising1_init(void)
{
  //定义广播参数结构体
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
  adv_params.primary_phy     = BLE_GAP_PHY_1MBPS;     //广播通道，可以设置自动选择 
  adv_params.duration        = APP_ADV_DURATION;      //广播持续时间
  adv_params.properties.type = BLE_GAP_ADV_TYPE_CONNECTABLE_SCANNABLE_UNDIRECTED;//广播类型，可连接可扫描，无目标
  adv_params.p_peer_addr     = NULL;            //对点地址
  adv_params.filter_policy   = BLE_GAP_ADV_FP_ANY;    //过略设置   允许任何设备的扫描request和连接request
  adv_params.interval        = APP_ADV_INTERVAL;       //广播间隔
  
  err_code = sd_ble_gap_adv_set_configure(&m_adv_handle, &m_adv_data, &adv_params);
  NRF_LOG_INFO("error : %d",err_code);
  G_CHECK_ERROR_CODE_INFO(err_code);
  ble_advertising_conn_cfg_tag_set(&m_advertising,APP_BLE_CONN_CFG_TAG);
  
}
/*****************************************************
函数名：void advertising_start1(void)
参数：无
作用：启动广播，直接调用的协议栈sd_ble_gap_adv_start函数
******************************************************/
void advertising_start1(void)
{                               //开启广播 ble_advertising_start是此函数的封装
   ret_code_t err_code = sd_ble_gap_adv_start(m_adv_handle, APP_BLE_CONN_CFG_TAG);
    APP_ERROR_CHECK(err_code);
}

 static void ble_adv_error_handler (uint32_t nrf_error)
 {
   
   NRF_LOG_INFO("%d:%s:%d",__LINE__,__func__,nrf_error);
   APP_ERROR_CHECK(nrf_error);
 }
/*****************************************************
函数名：void advertising1_init(void)
参数：无
作用：广播初始化，不包括启动广播，有整个广播结构体参数
******************************************************/
void advertising_all_params_init(void)
{
  ret_code_t err_code = NRF_SUCCESS;
  //define advertising struct
  ble_advertising_init_t init;
  memset(&init,0,sizeof(init));
  init.advdata.name_type = BLE_ADVDATA_FULL_NAME;
  //是否包含外貌
  init.advdata.include_appearance = true;
  init.advdata.flags = BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE;
  
  //广播数据是否再次包含设备地址
  init.advdata.include_ble_device_addr = false;
 /*     
  init.advdata.p_lesc_data = NULL;      //用于NFC,BLE advertising set NULL
  init.advdata.p_tk_value = NULL;    //用于NFC,BLE advertising set NULL
  init.advdata.p_sec_mgr_oob_flags = NULL;//用于NFC,BLE advertising set NULL
  init.advdata.le_role = NULL;//用于NFC,BLE advertising set NULL
   
  //广播中带服务数据
ble_advdata_service_data_t sever_data;
  memset(&sever_data,0,sizeof(sever_data));
  
  uint8_t data = 0x99;  //数据
  sever_data.service_uuid = BLE_UUID_BATTERY_SERVICE;   //服务UUID
  sever_data.data.size = sizeof(data);  //数据大小
  sever_data.data.p_data = &data;        //数据
  
  init.advdata.p_service_data_array = &sever_data;  //get 参数     
  init.advdata.service_data_count = 1;  //length of service data 
  init.advdata.p_slave_conn_int = NULL; //从机连接间隔范围
  init.advdata.p_manuf_specific_data = NULL;    //制造商自定义数据
  */
 
 //add service 部分列表			
// init.advdata.uuids_more_available.uuid_cnt = (sizeof(m_adv_uuids)/sizeof(m_adv_uuids[0]))-1; //包含UUID个数
// init.advdata.uuids_more_available.p_uuids= m_adv_uuids;

 //完整
//  init.advdata.uuids_complete.p_uuids = m_adv_uuids;
 // init.advdata.uuids_complete.uuid_cnt = (sizeof(m_adv_uuids)/sizeof(m_adv_uuids[0]));
 
 
 
  //error 回调函数
  init.error_handler = ble_adv_error_handler; 
  init.evt_handler = on_adv_evt;
  
  init.config.ble_adv_on_disconnect_disabled = true;  //断开链接广播停止，静止自动广播

	init.config.ble_adv_fast_enabled = true;	//快速广播
	init.config.ble_adv_fast_interval = APP_ADV_INTERVAL;	//广播间隔
	init.config.ble_adv_fast_timeout = APP_ADV_DURATION;	//广播持续时间,超时时间

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


