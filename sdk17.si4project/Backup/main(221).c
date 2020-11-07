
#define MAIN_GLOBAL
#include "global.h"
#include "nrf_delay.h"
#include "app_freertos.h"
//#include "timers.h"
#include "twi.h"
#include "mpu6050.h"
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h" 
#include "spi.h"
#include "st7789.h"
#include "bmp.h"
//#include "it725x.h"
APP_TIMER_DEF(led_timer);	
NRF_BLE_GATT_DEF(m_gatt);//定义gatt模块实例



#define STATIC_PASSKEY "890731"
static ble_opt_t static_option;
ble_gap_sec_params_t sec_params;

#if NRF_LOG_ENABLED && USE_FREERTOS
static TaskHandle_t m_logger_thread;

/*********************************
*功能:freertos task 输出log后将自己挂起
*参数:无
*返回:无
**********************************/
static void logger_thread(void *arg)
{
	UNUSED_PARAMETER(arg);
	while(1)
	{
		NRF_LOG_FLUSH();
		vTaskSuspend(NULL);
	}
}

/***********************************************
*功能:IDLE hook,configUSE_IDLE_HOOK必须为1才可用
*没执行一次IDLE task 则调用一次，不可有阻塞
*参数:无
*返回:无
************************************************/
void vApplicationIdleHook( void )
{
#if NRF_LOG_ENABLED
    vTaskResume(m_logger_thread);
#endif
}
#endif


/***********************************************
*功能: generic attribute profile(gatt) 
*handler 提示发生ATT MTU 更新时间
*参数:无
*返回:无
************************************************/
void gatt_evt_handler(nrf_ble_gatt_t * p_gatt, nrf_ble_gatt_evt_t const * p_evt)
{
    if ((m_conn_handle == p_evt->conn_handle) && (p_evt->evt_id == NRF_BLE_GATT_EVT_ATT_MTU_UPDATED))
    {
    
    NRF_LOG_DEBUG("ATT MTU exchange completed. central 0x%x peripheral 0x%x",
                  p_gatt->att_mtu_desired_central,
                  p_gatt->att_mtu_desired_periph);
    	}
}

/***********************************************
*功能: 初始化power manager模块
*参数:无
*返回:无
************************************************/
static void power_management_init(void)
{
	ret_code_t err_code = nrf_pwr_mgmt_init();
	G_CHECK_ERROR_CODE_INFO(err_code);
}


/***********************************************
*功能: ble事件handler
*参数:无
*返回:无
************************************************/
static void ble_evt_handler(ble_evt_t const *p_ble_evt,void *context)
{
	ret_code_t err_code = NRF_SUCCESS;
	switch(p_ble_evt->header.evt_id)
	{
		//断开连接事件
		case BLE_GAP_EVT_DISCONNECTED:
			NRF_LOG_INFO("Disconnected");
			advertising_start();
			break;
		case BLE_GAP_EVT_CONNECTED:
			
			NRF_LOG_INFO("Connected");
			m_conn_handle=p_ble_evt->evt.common_evt.conn_handle;
			err_code = sd_ble_gap_authenticate(m_conn_handle,&sec_params);
			G_CHECK_ERROR_CODE_INFO(err_code);
			// hrs_timer_start();
			break;
		case BLE_GAP_EVT_TIMEOUT:
			NRF_LOG_INFO("Timeout");
			break;
		case BLE_GAP_EVT_ADV_SET_TERMINATED:
			advertising_stop();
			NRF_LOG_INFO("advertising terminated");
			break;
		case BLE_GATTS_EVT_HVN_TX_COMPLETE:
			NRF_LOG_INFO("Notification transmission complete");
			break;
		case BLE_GAP_EVT_CONN_PARAM_UPDATE:		//收到BLE_GAP_EVT_SEC_PARAMS_REQUEST后设置安全参数后提示
			NRF_LOG_INFO("BLE_GAP_EVT_CONN_PARAM_UPDATE");		
			break;
		case BLE_GAP_EVT_SEC_PARAMS_REQUEST:	//收到配对的请求
			NRF_LOG_INFO("BLE_GAP_EVT_SEC_PARAMS_REQUEST");
			
			//err_code = sd_ble_gap_sec_params_reply(m_conn_handle,BLE_GAP_SEC_STATUS_SUCCESS,&sec_params,NULL);
		//	G_CHECK_ERROR_CODE_INFO(err_code);
			break;

		case BLE_GAP_EVT_SEC_INFO_REQUEST:
			NRF_LOG_INFO("BLE_GAP_EVT_SEC_INFO_REQUEST");

			break;
		case BLE_GAP_EVT_PASSKEY_DISPLAY: //显示密码
			NRF_LOG_INFO("passkey: %s",p_ble_evt->evt.gap_evt.params.passkey_display.passkey);
			break;
		case PM_EVT_CONN_SEC_CONFIG_REQ://重新连接
			// Reject pairing request from an already bonded peer.
			NRF_LOG_INFO("PM_EVT_CONN_SEC_CONFIG_REQ");
			pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
			//允许再次配对
			pm_conn_sec_config_reply(p_ble_evt->evt.common_evt.conn_handle, &conn_sec_config);
	        break;
		 case BLE_GAP_EVT_AUTH_STATUS:	//BLE_GAP_EVT_CONN_SEC_UPDATE后 身份认证完成，检查是否配对成功
		 	if(p_ble_evt->evt.gap_evt.params.auth_status.auth_status == BLE_GAP_SEC_STATUS_SUCCESS)
            {
                NRF_LOG_INFO("Pair success!");
            }
            else
            {
               NRF_LOG_INFO("Pair failed!");
               sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
            } 
            break;

		case BLE_GAP_EVT_CONN_SEC_UPDATE:	//配对输入密码后,触发次事件
			NRF_LOG_INFO("connection security update");
			break;
		case BLE_GATTC_EVT_HVX:
			NRF_LOG_INFO("BLE_GATTC_EVT_HVX");
			break;
		case BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP:
			NRF_LOG_INFO("BLE_GATTC_EVT_CHAR_VAL_BY_UUID_READ_RSP");
			break;
		default:
			NRF_LOG_INFO("%d: %s: %d:",p_ble_evt->header.evt_id ,__func__,__LINE__);
			break;
	}
}
/***********************************************
*功能: 初始化协议栈，需要先初始化协议栈才能初始化gap,gatt等
*参数:无
*返回:无
************************************************/
static void ble_stack_init(void)
{
	//请求使能softdevice,会根据sdk_config参数配置低频时钟
	ret_code_t err_code = nrf_sdh_enable_request();
	G_CHECK_ERROR_CODE_INFO(err_code);
	
	//根据sdk_config.h中的参数而皮质BLE协议栈，获取RAM起始地址
	uint32_t ram_start = 0;
	err_code = nrf_sdh_ble_default_cfg_set(APP_BLE_CONN_CFG_TAG,&ram_start);
	//G_CHECK_ERROR_CODE_INFO(err_code);
	
	//使能BLE协议栈
	err_code = nrf_sdh_ble_enable(&ram_start);
	G_CHECK_ERROR_CODE_INFO(err_code);
	
	//注册BLE事件回调函数
	NRF_SDH_BLE_OBSERVER(m_ble_observer,APP_BLE_OBSERVER_PRIO,ble_evt_handler,NULL);
	
	
}
/***********************************************
*功能: 初始化gap 参数，generic access profile(通用访问协议)
*参数:无
*返回:无
************************************************/
static void gap_params_init(void)
{
	ret_code_t err_code;
	uint8_t passkey[] = STATIC_PASSKEY;
	
	ble_gap_conn_params_t gap_conn_params;	//连接参数
	ble_gap_conn_sec_mode_t sec_mode;	//连接安全模式
	
	//配置gap安全模式
	BLE_GAP_CONN_SEC_MODE_SET_ENC_WITH_MITM(&sec_mode);
	
	err_code = sd_ble_gap_device_name_set(&sec_mode,
				             (const uint8_t*)DEVICE_NAME,
				      strlen(DEVICE_NAME));
   G_CHECK_ERROR_CODE_INFO(err_code);
													
	err_code = sd_ble_gap_appearance_set(BLE_APPEARANCE_GENERIC_MEDIA_PLAYER);
	G_CHECK_ERROR_CODE_INFO(err_code);
							
	memset(&gap_conn_params,0,sizeof(gap_conn_params));												
	gap_conn_params.min_conn_interval = MIN_CONN_INTERVAL;
	gap_conn_params.max_conn_interval = MAX_CONN_INTERVAL;
	gap_conn_params.slave_latency     = SLAVE_LATENCY;
	gap_conn_params.conn_sup_timeout  = CONN_SUP_TIMEOUT;

	err_code = sd_ble_gap_ppcp_set(&gap_conn_params);		
	G_CHECK_ERROR_CODE_INFO(err_code);
													
//	static ble_gap_addr_t my_addr;
//	err_code = sd_ble_gap_addr_get(&my_addr);
	// G_CHECK_ERROR_CODE_INFO(err_code);
/*	if(err_code == NRF_SUCCESS)
	{
		NRF_LOG_INFO("Address type: %02x",my_addr.addr_type);
		NRF_LOG_INFO("Address: %02x:%02x:%02x:%02x:%02x:%02x",my_addr.addr[0],my_addr.addr[1],my_addr.addr[2],
																				my_addr.addr[3],my_addr.addr[4],my_addr.addr[5]);
	}
	 NRF_LOG_INFO("name size %d",sizeof(my_addr));	*/	

//设置配对静态密码
#if SET_PASSKEY 
	sd_ble_opt_set(BLE_GAP_OPT_PASSKEY,&static_option);
	static_option.gap_opt.passkey.p_passkey = passkey;
	err_code = sd_ble_opt_set(BLE_GAP_OPT_PASSKEY,&static_option);
	G_CHECK_ERROR_CODE_INFO(err_code);
#endif
	//main_sec_params_bond(m_conn_handle);

}

/***********************************************
*功能: 初始化gatt, generic attribute profile
*参数:无
*返回:无
************************************************/
static void gatt_init(void)
{
    ret_code_t err_code = nrf_ble_gatt_init(&m_gatt, gatt_evt_handler);
    G_CHECK_ERROR_CODE_INFO(err_code);
	err_code = nrf_ble_gatt_att_mtu_periph_set(&m_gatt, NRF_SDH_BLE_GATT_MAX_MTU_SIZE);
	 G_CHECK_ERROR_CODE_INFO(err_code);
}




/***********************************************
*功能: get rtc1 计数器
*参数:无
*返回:无
************************************************/
uint32_t get_rtc_counter(void)
{
    return NRF_RTC1->COUNTER;
}


/***********************************************
*功能: log初始化
*参数:无
*返回:无
************************************************/
void main_log_init(void)
{
#if NRF_LOG_ENABLED
	
	//初始化log,可输出时间
   //ret_code_t err_code = NRF_LOG_INIT(get_rtc_counter);
	ret_code_t err_code = NRF_LOG_INIT(NULL);

	//初始化RTT
	NRF_LOG_DEFAULT_BACKENDS_INIT();
	G_CHECK_ERROR_CODE_INFO(err_code);
#if NRF_LOG_ENABLED && USE_FREERTOS
	if(pdPASS != xTaskCreate(logger_thread,"LOGGER",256,NULL,1,&m_logger_thread))
	{
		APP_ERROR_HANDLER(NRF_ERROR_NO_MEM);
	}   
#endif
        
#endif  
	
}


/***********************************************
*功能: //配置低频时钟
*参数:无
*返回:无
************************************************/
void main_lfclk_config(void)
{
	//初始化时钟模块，设置低频时钟源
	ret_code_t err_code = nrf_drv_clock_init();
	G_CHECK_ERROR_CODE_INFO(err_code);
	//请求低频时钟，输入NULL启动时钟后不产生event
	nrf_drv_clock_lfclk_request(NULL);

	
}


//连接参数更新事件
//static uint16_t m_conn_handle = BLE_CONN_HANDLE_INVALID;                        /**< Handle of the current connection. */
static void on_conn_params_evt(ble_conn_params_evt_t * p_evt)
{
    ret_code_t err_code;
	NRF_LOG_INFO("on_conn_params_evt");
    if (p_evt->evt_type == BLE_CONN_PARAMS_EVT_FAILED)
    {
        err_code = sd_ble_gap_disconnect(m_conn_handle, BLE_HCI_CONN_INTERVAL_UNACCEPTABLE);
        APP_ERROR_CHECK(err_code);
    }
}
//连接参数更新失败
static void conn_params_error_handler(uint32_t nrf_error)
{
	NRF_LOG_INFO("conn_params_error_handler");
    APP_ERROR_HANDLER(nrf_error);
}

static void conn_params_init(void)
{
    ret_code_t             err_code;
    ble_conn_params_init_t cp_init;

    memset(&cp_init, 0, sizeof(cp_init));

    cp_init.p_conn_params                  = NULL;
    cp_init.first_conn_params_update_delay = FIRST_CONN_PARAMS_UPDATE_DELAY;
    cp_init.next_conn_params_update_delay  = NEXT_CONN_PARAMS_UPDATE_DELAY;
    cp_init.max_conn_params_update_count   = MAX_CONN_PARAMS_UPDATE_COUNT;
    cp_init.start_on_notify_cccd_handle    = BLE_GATT_HANDLE_INVALID;
    cp_init.disconnect_on_fail             = false;
    cp_init.evt_handler                    = on_conn_params_evt;
    cp_init.error_handler                  = conn_params_error_handler;

    err_code = ble_conn_params_init(&cp_init);
    APP_ERROR_CHECK(err_code);
}

/*****************************************************
函数名:static void pm_evt_handler(pm_evt_t const * p_evt)
参数:   
return: 
作用： peer_manager handler
******************************************************/

static void pm_evt_handler(pm_evt_t const * p_evt)
{
      ret_code_t err_code;
      pm_handler_on_pm_evt(p_evt);

	if(p_evt->evt_id == PM_EVT_CONN_SEC_CONFIG_REQ)
		{
			pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
			pm_conn_sec_config_reply(p_evt->conn_handle,&conn_sec_config);
		}
	else if(p_evt->evt_id == PM_EVT_CONN_SEC_FAILED)
		{
			if(p_evt->params.conn_sec_failed.procedure == PM_CONN_SEC_PROCEDURE_ENCRYPTION&&
				p_evt->params.conn_sec_failed.error == PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING)
				{
					
				}
                        else
                          sd_ble_gap_disconnect(p_evt->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
		}
    pm_handler_flash_clean(p_evt);
#if 0
    switch (p_evt->evt_id)

	{
		case :		//已绑定设备建立链接
			NRF_LOG_INFO("PM_EVT_BONDED_PEER_CONNECTED");
			//  conn_secure(p_evt->conn_handle, false);
			break;
	
		case PM_EVT_CONN_SEC_START:	//已启动安全链接(配对)，后面回收到建立链接成功或失败event
			NRF_LOG_INFO("PM_EVT_CONN_SEC_START");
			break;
	
		case PM_EVT_CONN_SEC_SUCCEEDED:	//安全链接成功
			NRF_LOG_INFO("PM_EVT_CONN_SEC_SUCCEEDED");
			break;
	
		case PM_EVT_CONN_SEC_FAILED://安全链接失败
			NRF_LOG_INFO("PM_EVT_CONN_SEC_FAILED");
			if(p_evt->params.conn_sec_failed.procedure == PM_CONN_SEC_PROCEDURE_ENCRYPTION&&
				p_evt->params.conn_sec_failed.error == PM_CONN_SEC_ERROR_PIN_OR_KEY_MISSING)
				{
					 NRF_LOG_INFO("Waiting for host to fix bonding\r\n");
				}
				else
				sd_ble_gap_disconnect(p_evt->conn_handle, BLE_HCI_REMOTE_USER_TERMINATED_CONNECTION);
			break;
	
		case PM_EVT_CONN_SEC_CONFIG_REQ://中心设备发生来配对请求(已绑定)，使用pm_conn_sec_config_reply回应，不然使用peer_init时配置的默认参数
			NRF_LOG_INFO("PM_EVT_CONN_SEC_CONFIG_REQ");
		    pm_conn_sec_config_t conn_sec_config = {.allow_repairing = true};
            pm_conn_sec_config_reply(p_evt->conn_handle, &conn_sec_config);
			break;
	
		case PM_EVT_CONN_SEC_PARAMS_REQ://请求获取安全链接参数，调用pm_conn_sec_config_reply发生参数，不然使用peer_init时配置的默认参数
			NRF_LOG_INFO("PM_EVT_CONN_SEC_PARAMS_REQ");
		NRF_LOG_INFO("kdist_own.enc :%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_own.enc);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_own.id);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_own.sign);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_own.link);
		
		NRF_LOG_INFO("kdist_peer.enc: %d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_peer.enc);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_peer.id);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_peer.sign);
		NRF_LOG_INFO("%d",p_evt->params.conn_sec_params_req.p_peer_params->kdist_peer.link);
			err_code =  pm_conn_sec_params_reply(p_evt->conn_handle, &sec_params, p_evt->params.conn_sec_params_req.p_context);
			G_CHECK_ERROR_CODE_INFO(err_code);
			break;
	
		case PM_EVT_STORAGE_FULL:
			NRF_LOG_INFO("Flash storage is full");
			break;
	
		case PM_EVT_ERROR_UNEXPECTED:
			NRF_LOG_INFO("PM_EVT_ERROR_UNEXPECTED");
			break;
	
		case PM_EVT_PEER_DATA_UPDATE_SUCCEEDED:
			NRF_LOG_INFO("PM_EVT_PEER_DATA_UPDATE_SUCCEEDED");
			break;
	
		case PM_EVT_PEER_DATA_UPDATE_FAILED:
			// This can happen if the SoftDevice is too busy with BLE operations.
			NRF_LOG_INFO("PM_EVT_PEER_DATA_UPDATE_FAILED");
			break;
	
		case PM_EVT_PEER_DELETE_SUCCEEDED:
			NRF_LOG_INFO("PM_EVT_PEER_DELETE_SUCCEEDED");
			break;
	
		case PM_EVT_PEER_DELETE_FAILED:
			NRF_LOG_INFO("PM_EVT_PEER_DELETE_FAILED");
			break;
	
		case PM_EVT_PEERS_DELETE_SUCCEEDED:
			NRF_LOG_INFO("All peers deleted.");
			break;
	
		case PM_EVT_PEERS_DELETE_FAILED:
			NRF_LOG_INFO("PM_EVT_PEERS_DELETE_FAILED");
			break;
	
		case PM_EVT_LOCAL_DB_CACHE_APPLIED:
			NRF_LOG_INFO("PM_EVT_LOCAL_DB_CACHE_APPLIED");
			break;
	
		case PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED:
			// This can happen when the local DB has changed.
			NRF_LOG_INFO("PM_EVT_LOCAL_DB_CACHE_APPLY_FAILED");
			break;
	
		case PM_EVT_SERVICE_CHANGED_IND_SENT:
			NRF_LOG_INFO("Sending Service Changed indication.");
			break;
	
		case PM_EVT_SERVICE_CHANGED_IND_CONFIRMED:
			NRF_LOG_INFO("Service Changed indication confirmed.");
			break;
	
		case PM_EVT_SLAVE_SECURITY_REQ:
			NRF_LOG_INFO("Security Request received from peer.");
			break;
	
		case PM_EVT_FLASH_GARBAGE_COLLECTED:
			NRF_LOG_INFO("Flash garbage collection complete.");
			break;
	
		case PM_EVT_FLASH_GARBAGE_COLLECTION_FAILED:
			NRF_LOG_INFO("PM_EVT_FLASH_GARBAGE_COLLECTION_FAILED");
			break;
	
		default:
		NRF_LOG_INFO("default");
			break;
	}
#endif
}

/*****************************************************
函数名：static void peer_manager_init(void)
参数：无
作用：初始化peer manager module,并配置sec manager参数
相当于设备管理器
******************************************************/

static void peer_manager_init(void)
{
//    ble_gap_sec_params_t sec_param;
    ret_code_t           err_code;
    bool erase_bonds = true;
    err_code = pm_init();
    APP_ERROR_CHECK(err_code);

	if(erase_bonds)
	{
		err_code = pm_peers_delete();
		G_CHECK_ERROR_CODE_INFO(err_code);
	}
    memset(&sec_params, 0, sizeof(ble_gap_sec_params_t));
	
	//Bonding just works 
	sec_params.bond = SEC_PARAM_BOND;                //绑定
	sec_params.mitm = SEC_PARAM_MITM;                         //中间人攻击保护 MITM
	sec_params.lesc = SEC_PARAM_LESC;                            //使能LE secure connetion 配对
	sec_params.keypress = SEC_PARAM_KEYPRESS;     
	sec_params.io_caps = SEC_PARAM_IO_CAPABILITIES;
	sec_params.oob = SEC_PARAM_OOB;
	sec_params.min_key_size = SEC_PARAM_MIN_KEY_SIZE;
	sec_params.max_key_size = SEC_PARAM_MAX_KEY_SIZE;

	//分发本地的key(本设备) 4种key
	sec_params.kdist_own.enc = KIDST_OWN_ENC;	
	sec_params.kdist_own.id = KIDST_OWN_ID;	
	//分发远端的key(配对的设配)
	sec_params.kdist_peer.enc = KIDST_PEER_ENC;
	sec_params.kdist_peer.id = KIDST_PEER_ID;

	
    err_code = pm_sec_params_set(&sec_params);
	G_CHECK_ERROR_CODE_INFO(err_code);
   
    err_code = pm_register(pm_evt_handler);
    G_CHECK_ERROR_CODE_INFO(err_code);

}
/*****************************************************
函数名：
参数：无
作用：服务初始化 包含his服务
******************************************************/

static void idle_state_handle(void)
{
	 NRF_LOG_INFO("idle_state_handle");
    if (NRF_LOG_PROCESS() == false)
    {
        nrf_pwr_mgmt_run();
    }
}

/*#define WDTFEEDEVTWAITBIT  0x01
#define WDTTASKSIZE 50
#define WDTTASKPRIO 2
void WdtFeedTask (void *paramenters);
TaskHandle_t wdtFeedTaskHanle;*/


/*#define START_TASK_SIZE 50
#define START_TASK_PRIO  0
TaskHandle_t StartTask_Handle;
void Start_Task(void* pvParameters);*/

/*#define RTCUpdateSemMaxCount 1
#define RTCUpdateSemInitCount 0
SemaphoreHandle_t RTCUpdateSem;

EventGroupHandle_t wdtFeedEventHandle;


#define RTC_UPDATE_TASK_SIZE 50
#define RTC_UPDATE_TASK_PRIO 2
TaskHandle_t RTCUpdateTaskHandle;
void RTCUpdateTaskHandler(void *pvParamenters);

EventGroupHandle_t wdtFeedEventHandle;*/

/*void WdtFeedTask (void *paramenters)
{
	EventBits_t wdtFeedEvt = 0;
	while(1)
		{
			//xEventGroupGetBits(wdtFeedEventHandle);
			wdtFeedEvt = xEventGroupWaitBits(wdtFeedEventHandle,
			WDTFEEDEVTWAITBIT,pdTRUE,pdTRUE,portMAX_DELAY);
			if(wdtFeedEvt == WDTFEEDEVTWAITBIT)
					nrfx_wdt_feed();		
		}
}



	void RTCUpdateTaskHandler(void *pvParamenters)
{
	
	uint8_t buff[7] = {0};
		while(1)
		{
		
			xSemaphoreTake(RTCUpdateSem,portMAX_DELAY);
			xEventGroupSetBits(wdtFeedEventHandle,RTC_UPDATE_EVENT_BIT);
			update_date();
		//	data_convert(current_time,buff);
		//	ble_date_notification();
		}
}


*/
//extern void WdtFeedTask (void *paramenters);
/*
	void Start_Task(void* pvParameters)
	{
		taskENTER_CRITICAL();

		RTCUpdateSem = xSemaphoreCreateCounting(RTCUpdateSemMaxCount,RTCUpdateSemInitCount);
                if(RTCUpdateSem == NULL)
									while(1);
		wdtFeedEventHandle = xEventGroupCreate();
								if(wdtFeedEventHandle == NULL)
									while(1);

		ret = xTaskCreate((TaskFunction_t)RTCUpdateTaskHandler,
														(const char *)"rtc",
														(uint16_t)RTC_UPDATE_TASK_SIZE,
														(void*)NULL,
														(UBaseType_t)RTC_UPDATE_TASK_PRIO,
														&RTCUpdateTaskHandle);
		if(ret != pdPASS )
                  	{
                      freemem =	xPortGetFreeHeapSize();
			while(1);
                  	}
														
	ret =	xTaskCreate(WdtFeedTask,"wdt",WDTTASKSIZE,NULL,WDTTASKPRIO,&wdtFeedTaskHanle);
        
				if(ret != pdPASS )
                                {
                                   freemem =	xPortGetFreeHeapSize();
			while(1);
        }
			freemem =	xPortGetFreeHeapSize();
	vTaskDelete(NULL);
		taskEXIT_CRITICAL();
		
	}*/
	
 
void HardFault_Handler(void)
{
  NRF_LOG_INFO("HardFault_handler reset");
  while(1);
}
void PreSleepProcessing(uint32_t ulExpectedIdleTime)
{
   

}
void PostSleepProcessing(uint32_t ulExpectedIdleTime)
{

}

void MotorPinInit(void)
{
	nrf_gpio_cfg(16,NRF_GPIO_PIN_DIR_OUTPUT,NRF_GPIO_PIN_INPUT_DISCONNECT,NRF_GPIO_PIN_PULLDOWN,NRF_GPIO_PIN_S0S1,NRF_GPIO_PIN_NOSENSE);
}
#define SCREEN_LCD_LED_PIN	15

 int main(void)
{
	ret_code_t err_code;
	bool ret;
  uint8_t a = 0x67;
	uint8_t advertisingButton = 0;
	main_log_init();
	main_lfclk_config();
#if 1
//	power_management_init();
	SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
	ble_stack_init();
	gap_params_init();
	gatt_init();
	service_init();
  advertising_all_params_init();
	
	conn_params_init();
	peer_manager_init();
#endif
// CommonWatchDogInit();	//watchdog init
// TwiDriverInit(MPU6050);//hardware iic init
// I2cSimulationInit();   //software iic init
//	uart_init();					//hardware uart init
//	MotorPinInit();				//motor init
//nrf_gpio_cfg_output(SCREEN_LCD_LED_PIN);
//nrf_gpio_pin_set(SCREEN_LCD_LED_PIN);

//SPI_HardWareInit();				//spi hardware init
 //SPI_Init();
//while(1)
//{
//SPI_WriteCommand(0xDA);
//SPI_WriteData(0x88);
//nrf_delay_ms(50);
//}
//SPI_WriteData(0x88);
//ST7789_Init();
//	LCD_Init();
//	SPI_SoftWareInit();
//nrf_delay_ms(2000);	
//ST7789_Init();
  //LCD_Init();
//ST7789_Init();
// ST7789_Fill_Color(WHITE);

//ST7789_PictureDraw(gImage_bmp,sizeof(gImage_bmp));
LCD_Init();

//test1();
//IT725X_Init();

while(1);


	/*MPU_Init();						//mpu6050 module init
	 while( a = mpu_dmp_init())
		{
        NRF_LOG_INFO("mpu_dmp_init error:%d",a);
		}
*/
/*	mpu6050 interrupt configure 有bug
	MPU_INT_Init();
  MPU_Pin_int();
  */

#if 1

#endif


NRF_LOG_INFO("mpu_dmp_init success");

//advertisingButton = OPEN;
advertisingButton = CLOSE;

#if USE_FREERTOS 
#if USE_TASK
 task_init();
#endif


 nrf_sdh_freertos_init(advertising_button,&advertisingButton);

   vTaskStartScheduler();

    for (;;)
    {
        APP_ERROR_HANDLER(NRF_ERROR_FORBIDDEN);
    }
#else
    advertising_button(&advertisingButton);
	  for (;;)
    {
    }
#endif
}

