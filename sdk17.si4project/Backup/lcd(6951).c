#define LCD_GLOBAL
#include "bmp.h"
#include "global.h"
#include "st7789.h"
#include "it725x.h"

#define LCD_ICON_FONT_POS_X 
#define LCD_ICON_FONT_POS_Y

#define LCD_DATE_POS_X 60
#define LCD_DATA_POS_Y 90
#define LCD_TIME_POS_X 140
#define LCD_TIME_POS_Y 60

#define LCD_ICON_POS_X
#define LCD_ICON_POS_Y 
void LCD_Display(bool NO)
{
	if(NO)
		{
			nrf_gpio_cfg_output(SCREEN_LCD_LED_PIN);
			nrf_gpio_pin_set(SCREEN_LCD_LED_PIN);
			ST7789_UnDisplay()
		}
	else
		{
		nrf_gpio_cfg_output(SCREEN_LCD_LED_PIN);
			nrf_gpio_pin_clear(SCREEN_LCD_LED_PIN);
			ST7789_Display();
		}
		
}
/************************************************
*说明:lcd屏的刷新
*函数名: void lcd_init(void)
*参数:
*返回值:
************************************************/
static uint8_t UI_Page = MAIN_PAGE;
FontDef Font_16x26 = {16,26,Font16x26};
FontDef Font_7x10 = {7,10,Font7x10};
const char 
void LCD_Refresh()
{
	if(handle.evt == drag)
		{
			switch(handle.dir)
				case up:
				case right:
					UI_Page++;
				 if(HRS_PAGE < UI_Page)	UI_Page = MAIN_PAGE;
				 	ST7789_WriteString(LCD_ICON_FONT_POS_X,LCD_ICON_FONT_POS_Y,,Font_16x26);
				 	ST7789_WriteString();
					ST7789_WriteString();
				break;
				case down:
				case left: UI_Page--;break;
		}
	if(MAIN_PAGE>UI_Page)
		UI_Page = HRS_PAGE;
	else if(HRS_PAGE < UI_Page)
		UI_Page = MAIN_PAGE;
}



/************************************************
*说明:lcd屏的初始化
*函数名: void lcd_init(void)
*参数:
*返回值:
************************************************/
	//FontDef Font_16x26 = {16,26,Font16x26};
//FontDef Font_7x10 = {7,10,Font7x10};
//FontDef Font_16x16 = {16,16,Font16x16};


void LCD_Init(void)
{
	//LCD_Display(true);
	ST7789_Init();
	//ST7789_Fill_Color(YELLOW);
//	ST7789_PictureDraw(0,0,gImage_bmp,sizeof(gImage_bmp));
//	ST7789_PictureDraw(LCD_BLE_POS_X,LCD_BLE_POS_Y,gImage_bleDisable,sizeof(gImage_bleDisable));
	//ST7789_WriteChar(40,40,'7',Font_16x26,WHITE,BLACK);
//	ST7789_WriteString(140,60,"16:27",Font_16x26,WHITE,BLACK);
//	ST7789_WriteString(60,90,"2020.11.2",Font_7x10,WHITE,BLACK);
//	ST7789_DrawCircle(120,120,30,BLUE);
//	ST7789_WriteChar(40,40,' ',Font_16x16,WHITE,BLACK);
	IT725X_Init();
	
	//LCD_Refresh();
}




