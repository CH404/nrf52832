#ifndef __MAIN_H
#define __MAIN_H

/*********************************************
	P0.25											NC
	P0.26											P0.24
	P0.27											P0.23
	P0.28											P0.22
	P0.29											SCLK
	P0.30											SWD
	P0.31											P0.21
	P0.02											P0.20
	P0.03											P0.19
	GND											P0.18
	VCC											P0.17
	NC	  P P	P P P	P  P	P	P	P	P	P  P0.16
		  4 5 6 7 8 9 10 11 12 13 14 15

***********************************************/

#ifdef MAIN_GLOBAL
#define MAIN_EXT
#else
#define MAIN_EXT	extern
#endif



#define APP_BLE_CONN_CFG_TAG            1                                       /**< A tag identifying the SoftDevice BLE configuration. */
#define APP_BLE_OBSERVER_PRIO				 3


#define MIN_CONN_INTERVAL               MSEC_TO_UNITS(400, UNIT_1_25_MS)        /**< Minimum acceptable connection interval (0.1 seconds). */
#define MAX_CONN_INTERVAL               MSEC_TO_UNITS(650, UNIT_1_25_MS)        /**< Maximum acceptable connection interval (0.2 second). */
#define SLAVE_LATENCY                   0                                       /**< Slave latency. */
#define CONN_SUP_TIMEOUT                MSEC_TO_UNITS(4000, UNIT_10_MS)         /**< Connection supervisory timeout (4 seconds). */




#define FIRST_CONN_PARAMS_UPDATE_DELAY  APP_TIMER_TICKS(5000)
#define NEXT_CONN_PARAMS_UPDATE_DELAY   APP_TIMER_TICKS(30000)
#define MAX_CONN_PARAMS_UPDATE_COUNT    3 

#define JUSTWORKS
//#define STATICPASSKEY

#ifdef JUSTWORKS
#define SEC_PARAM_BOND                  1                                      /**< Perform bonding. */
#define SEC_PARAM_MITM                  0                                      /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_NONE                    /**< No I/O capabilities. */
#define SET_PASSKEY						0
//分发本地的key(本设备) 4种key
#define KIDST_OWN_ENC 1
#define KIDST_OWN_ID	1
//分发远端的key(配对的设配)
#define KIDST_PEER_ENC 1
#define KIDST_PEER_ID  1

#elif defined STATICPASSKEY
#define SEC_PARAM_BOND                  1                                      /**< Perform bonding. */
#define SEC_PARAM_MITM                  1                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_DISPLAY_ONLY                    /**< No I/O capabilities. */
#define SET_PASSKEY						1
//分发本地的key(本设备) 4种key
#define KIDST_OWN_ENC 1
#define KIDST_OWN_ID	0
//分发远端的key(配对的设配)
#define KIDST_PEER_ENC 1
#define KIDST_PEER_ID  0

#else
#define SEC_PARAM_BOND                  1                                      /**< Perform bonding. */
#define SEC_PARAM_MITM                  1                                       /**< Man In The Middle protection not required. */
#define SEC_PARAM_IO_CAPABILITIES       BLE_GAP_IO_CAPS_DISPLAY_ONLY                    /**< No I/O capabilities. */
#define SET_PASSKEY						0
//分发本地的key(本设备) 4种key
#define KIDST_OWN_ENC 1
#define KIDST_OWN_ID	0
//分发远端的key(配对的设配)
#define KIDST_PEER_ENC 1
#define KIDST_PEER_ID  0

#endif





#define SEC_PARAM_LESC                  0                                       /**< LE Secure Connections not enabled. */
#define SEC_PARAM_KEYPRESS              0                                       /**< Keypress notifications not enabled. */
#define SEC_PARAM_OOB                   0                                       /**< Out Of Band data not available. */
#define SEC_PARAM_MIN_KEY_SIZE          7                                       /**< Minimum encryption key size. */
#define SEC_PARAM_MAX_KEY_SIZE          16                                      /**< Maximum encryption key size. */


#define LED_RED		15
#define LED_GREEN		18
#define LED_BLINKY_TIME    500ms





#define LED_TOGGLE_INTERVAL APP_TIMER_TICKS(500)

#define contact_TOGGLE_INTERVAL APP_TIMER_TICKS(5000)

MAIN_EXT void main_log_init(void);
MAIN_EXT void main_timer_init(void);
MAIN_EXT void main_leds_init(void);
MAIN_EXT void main_lfclk_config(void);
MAIN_EXT void main_led_off(void);
void main_sec_params_bond(uint16_t conn_handle);
void PreSleepProcessing(uint32_t ulExpectedIdleTime);
void PostSleepProcessing(uint32_t ulExpectedIdleTime);


#endif

