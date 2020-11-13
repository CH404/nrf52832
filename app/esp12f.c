#define ESP_GLOBAL
#include "global.h"

#define ESP_RT_NO 2
#define ESP_TX_NO	3
#define ESP_BAUD_RATE	115200

/***************************************
* 函数名:void ESP_HardWareInit(void)
* 参数: 
* 功能:初始化ESP-12f通信uart
* 返回: success 0
****************************************/
void ESP_Init(void)
{
	UART_Init(ESP_RT_NO,ESP_TX_NO,ESP_BAUD_RATE);
}
void ESP_UnInit(void)
{
	UART_UnInit();
}


