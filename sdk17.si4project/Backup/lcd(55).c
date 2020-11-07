#define LCD_GLOBAL
#include "bmp.h"
#include "global.h"
#include "st7789.h"
#include "it725x.h"

#define LCD_ICON_FONT_POS_X 50
#define LCD_ICON_FONT_POS_Y 40

#define LCD_DATE_POS_X 150
#define LCD_DATE_POS_Y 34
#define LCD_TIME_POS_X 140
#define LCD_TIME_POS_Y 60

#define LCD_ICON_POS_X 140
#define LCD_ICON_POS_Y 60
void LCD_Display(bool NO)
{
	if(NO)
		{
			nrf_gpio_cfg_output(SCREEN_LCD_LED_PIN);
			nrf_gpio_pin_set(SCREEN_LCD_LED_PIN);
			ST7789_UnDisplay();
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

FontDef Font_16x26 = {16,26,Font16x26};
FontDef Font_7x10 = {7,10,Font7x10};
void LCD_Refresh(char *datebuff,char *timebuff,uint8_t *UI_Page,uint8_t evt)
{
	if(handle.evt == drag)
		{
			switch(handle.dir)
                        {
				case up:
				case right:
					*UI_Page++;
				 if(HRS_PAGE < *UI_Page){
				 	*UI_Page = MAIN_PAGE;
					LCD_RefreshMainPage(datebuff,timebuff);}
					else{
						}
				break;
				case down:
				case left:
					*UI_Page--;
				if(HRS_PAGE < *UI_Page)
                                {
                                  *UI_Page = MAIN_PAGE;
				break;
                                }
                          }
                }

	else
        {}
}

void LCD_RefreshMainPage(char *datebuff,char *timebuff )
{
	 	ST7789_WriteString(LCD_ICON_FONT_POS_X,LCD_ICON_FONT_POS_Y,mainPageFont,Font_16x26,GREEN,BLACK);
	 	ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,timebuff,Font_16x26,GREEN,BLACK);
		ST7789_WriteString(LCD_DATE_POS_X,LCD_DATE_POS_Y,datebuff,Font_7x10,GREEN,BLACK);
}

void LCD_RefreshPage(char *datebuff,char *timebuff,uint8_t ui_page)
{
	switch(ui_page){
		case MAIN_PAGE:
	 	ST7789_WriteString(LCD_ICON_FONT_POS_X,LCD_ICON_FONT_POS_Y,"WHEN TIME?",Font_16x26,GREEN,BLACK);
	 	ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,timebuff,Font_16x26,GREEN,BLACK);
		ST7789_WriteString(LCD_DATE_POS_X,LCD_DATE_POS_Y,datebuff,Font_7x10,GREEN,BLACK);
		break;
		case BLE_PAGE:
		}
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
	IT725X_Init();
	ST7789_Init();
	//ST7789_Fill_Color(YELLOW);
//	ST7789_PictureDraw(0,0,gImage_bmp,sizeof(gImage_bmp));
//	ST7789_PictureDraw(LCD_BLE_POS_X,LCD_BLE_POS_Y,gImage_bleDisable,sizeof(gImage_bleDisable));
	//ST7789_WriteChar(40,40,'7',Font_16x26,WHITE,BLACK);
//	ST7789_WriteString(140,60,"16:27",Font_16x26,WHITE,BLACK);
//	ST7789_WriteString(60,90,"2020.11.2",Font_7x10,WHITE,BLACK);
//	ST7789_DrawCircle(120,120,30,BLUE);
//	ST7789_WriteChar(40,40,' ',Font_16x16,WHITE,BLACK);

	
	//LCD_Refresh();
}




