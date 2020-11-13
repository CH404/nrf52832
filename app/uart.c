#define UART_GLOBAL
#include "global"

//configuration parameter of app_uart
#define UART_RX_BUF_SIZE	255
#define UART_TX_BUF_SIZE	255

static void uart_error_handle(app_uart_evt_t * p_event)
{
	if(p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
		{
		//	NRF_LOG_INFO(APP_UART_COMMUNICATION_ERROR);
		}
	else if(p_event ->evt_type == APP_UART_FIFO_ERROR)
		{
	//	NRF_LOG_INFO(APP_UART_FIFO_ERROR);
		}
}

/**********************************************************
函数名：void app_uart_init(void)	
输入：
输出：
作用：初始化app uarts (带fifo的) nrf52832只有一个uart
**********************************************************/
static uint8_t app_uart_init(uint32_t rx_pin_no,uint32_t tx_pin_no,uint32_t baud_rate)
{
//	uart_RX_flage = false;
	ret_code_t err_code;
	app_uart_comm_params_t const comm_params = {
		.rx_pin_no = rx_pin_no,
		.tx_pin_no = tx_pin_no,
		.flow_control = APP_UART_FLOW_CONTROL_DISABLED,
		.use_parity = false,
	};
		switch(baud_rate){	
		case 4800:comm_params.baud_rate =  NRF_UART_BAUDRATE_4800;break;  
		case 9600:comm_params.baud_rate =  NRF_UART_BAUDRATE_9600;break;
		case 19200:comm_params.baud_rate =  NRF_UART_BAUDRATE_19200;break;
		case 57600:comm_params.baud_rate =  NRF_UART_BAUDRATE_57600;break;  
		case 76800:comm_params.baud_rate =  NRF_UART_BAUDRATE_76800;break;  
		case 115200:comm_params.baud_rate =  NRF_UART_BAUDRATE_115200;break; 
		case default:return 1;
			}
	 APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
		return 0;
}
uint8_t UART_Init(uint32_t rx_pin_no,uint32_t tx_pin_no,uint32_t baud_rate)
{
	//带fifo的硬件uart
	return app_uart_init(rx_pin_no,tx_pin_no,baud_rate);
}

void UART_UnInit(void)
{
	app_uart_close();
}

