#ifndef __GLOBAL_H
#define __GLOBAL_H

#define MY_QUEUE_ENABLE 0
#define USE_FREERTOS 1
#define OPEN_ADVERTISING 0
#define ADD_SERVICE_TO_ADVERTISING 0
#define USE_ADVERTISING_CODE_INIT 0
#define USE_TASK 1
#define USE_RTC_DATE_TASK 1
#define USE_MPU6050_TASK 0
#define USE_WATCH_DOG_TASK 0
#define USE_LCD_TASK 0

#include <stdio.h>
#include <stdlib.h>

#include "nrf_strerror.h"

#include "sdk_config.h"
#include "app_timer.h"
#include "nrf_gpio.h"
#include "nrf_drv_clock.h"	
#include "nrf_pwr_mgmt.h"

#include "nrf_sdh.h"
#include "nrf_sdh_ble.h"
#include "nrf_sdh_soc.h"

#include "ble_gap.h"
#include "ble_types.h"

#include "nrf_ble_gatt.h"
#include "nrf_ble_qwr.h"
#include "peer_manager.h"
#include "ble_conn_params.h"
#include "ble_dis.h"

#include "main.h"
#include "advertising.h"
#include "service.h"




//#include "hrs.h"
//#include "nus.h"
#include "rtc.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_error.h"
#include "security_manager.h"


#include "spi.h"
#include "twi.h"
#include "st7789.h"
#include "it725x.h"
#include "mpu6050.h"
#include "lcd.h"

//#include "my_queue.h"
#if USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "nrf_sdh_freertos.h"
#include "semphr.h"
#include "app_freertos.h"
#include "timers.h"

#endif

#include "ble_bas.h"
#include "common.h"
#include "ble_date.h"
#include "rtc.h"
#include "nrf_delay.h"
	//APP_ERROR_CHECK(error_code); 
#endif


