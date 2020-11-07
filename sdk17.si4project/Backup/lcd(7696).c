#define LCD_GLOBAL
#include "global.h"
#include "bmp.h"

#define LCD_BLE_POS_X 89
#define LCD_BLE_POS_Y 78

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
/************************************************
*说明:lcd屏的刷新
*函数名: void lcd_init(void)
*参数:
*返回值:
************************************************/
void LCD_Refresh()
{
	
}



/************************************************
*说明:lcd屏的初始化
*函数名: void lcd_init(void)
*参数:
*返回值:
************************************************/
void LCD_Init(void)
{
	//LCD_Display(true);
	ST7789_Init();
	ST7789_PictureDraw(LCD_BLE_POS_X,LCD_BLE_POS_Y,gImage_ble,sizeof(gImage_ble));
	IT725X_Init();
	//LCD_Refresh();
}




