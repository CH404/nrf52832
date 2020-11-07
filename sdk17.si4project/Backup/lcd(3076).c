#define LCD_GLOBAL
#include "bmp.h"
#include "global.h"


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
*˵��:lcd����ˢ��
*������: void lcd_init(void)
*����:
*����ֵ:
************************************************/
void LCD_Refresh()
{
	
}



/************************************************
*˵��:lcd���ĳ�ʼ��
*������: void lcd_init(void)
*����:
*����ֵ:
************************************************/
FontDef Font_7x10 = {7,10,Font7x10};

void LCD_Init(void)
{
	//LCD_Display(true);
	ST7789_Init();
	ST7789_PictureDraw(LCD_BLE_POS_X,LCD_BLE_POS_Y,gImage_bleDisable,sizeof(gImage_bleDisable));
	ST7789_WriteChar(20,20,7,Font_7x10,WHITE,BLACK);
	IT725X_Init();
	//LCD_Refresh();
}



