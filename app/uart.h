#ifndef __UART_H
#define __UART_H

#ifdef UART_GLOBAL
#define UART_EXT
#else
#define UART_EXT extern 
#endif

UART_EXT uint8_t UART_Init(uint32_t rx_pin_no,uint32_t tx_pin_no,uint32_t baud_rate);

UART_EXT void UART_HardWareInit(void);
UART_EXT void UART_UnInit(void);


#endif
