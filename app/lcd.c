#include "lcd.h"
#include "st7789.h"
#include "spi.h"
#include "twi.h"
#include "nrf_gpio.h"
void LCD_Display(bool NO)
{
	if(NO)
		{
			nrf_gpio_cfg_output(SCREEN_LCD_LED_PIN);
			nrf_gpio_pin_set(SCREEN_LCD_LED_PIN);
		}
	else
		{
		nrf_gpio_cfg_output(SCREEN_LCD_LED_PIN);
			nrf_gpio_pin_clear(SCREEN_LCD_LED_PIN);
		}
		
}

void lcd_init(void)
{
	LCD_Display(true);
	ST7789_Init();
}




