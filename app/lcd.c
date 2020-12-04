#define LCD_GLOBAL
//#include "bmp.h"
#include "global.h"
//#include "st7789.h"
//#include "it725x.h"


#define LCD_BATTERY_LEVEL1	1
#define LCD_BATTERY_LEVEL2  2
#define LCD_BATTERY_LEVEL3	3
#define LCD_BATTERY_LEVEL0	0

#define LCD_BATTERT_ONE_LEVEL_WIDTH 4



#define LCD_ICON_BATTERY_POS_X 60
#define LCD_ICON_BATTERY_POS_Y 30
#define LCD_ICON_BATTERY_WIDTH 15
#define LCD_ICON_BATTERY_HIGHT 6


#define LCD_ICON_FONT_POS_X 35
#define LCD_ICON_FONT_POS_Y 35

#define LCD_DATE_POS_X 140
#define LCD_DATE_POS_Y 48
#define LCD_TIME_POS_X 140
#define LCD_TIME_POS_Y 60

#define LCD_ICON_POS_X 140
#define LCD_ICON_POS_Y 48
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


bool BDbutton = false;
bool HRSbutton = false;
bool ESPbutton = false;
void LCD_Refresh(char *datebuff,char *timebuff,uint8_t *UI_Page,bool rtc_evt)
{
	if(!rtc_evt)
		{
		if(handle.evt == drag)
			{
				switch(handle.dir)
	                        {
					case up:
					case right:
						(*UI_Page)--;
					 if(MAIN_PAGE > *UI_Page)
					 	*UI_Page = ESP_PAGE;
					 break;
					case down:
					case left:
						(*UI_Page)++;
					if(ESP_PAGE < *UI_Page)
	         *UI_Page = MAIN_PAGE;
					break;
	                        }
			}
		else
	    {
	    	if(handle.start.x > 133 && handle.start.x < 230)
	    		{
	    			if(handle.start.y > 2 && handle.start.y < 86)
	    				{
	    					switch(*UI_Page)
	    						{
	    						case BLE_PAGE:
	    					BDbutton = !BDbutton;break;
									case HRS_PAGE:
								HRSbutton = !HRSbutton;break;
								case ESP_PAGE:
								ESPbutton = !ESPbutton;break;
	    						}
	    				}
	    		}
	    }
		}
	LCD_RefreshPage(datebuff,timebuff,*UI_Page);
}
#if 0
void LCD_RefreshMainPage(char *datebuff,char *timebuff )
{
	 	ST7789_WriteString(LCD_ICON_FONT_POS_X,LCD_ICON_FONT_POS_Y,mainPageFont,Font_16x26,GREEN,BLACK);
	 	ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,timebuff,Font_16x26,GREEN,BLACK);
		ST7789_WriteString(LCD_DATE_POS_X,LCD_DATE_POS_Y,datebuff,Font_7x10,GREEN,BLACK);
}
#endif
void LCD_RefreshPage(char *datebuff,char *timebuff,uint8_t ui_page)
{
	uint8_t button;
	switch(ui_page){
		case MAIN_PAGE:
			ST7789_WriteString(LCD_ICON_FONT_POS_X,LCD_ICON_FONT_POS_Y,mainPageFont,Font_16x26,GREEN,BLACK);
			ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,timebuff,Font_16x26,GREEN,BLACK);
			ST7789_WriteString(LCD_DATE_POS_X,LCD_DATE_POS_Y,datebuff,Font_7x10,GREEN,BLACK);
		break;
		
		case BLE_PAGE:
			ST7789_WriteString(LCD_ICON_FONT_POS_X,LCD_ICON_FONT_POS_Y,blePageFont,Font_16x26,GREEN,BLACK);
			if(BDbutton){
				button = 1;
			ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,onFont,Font_16x26,GREEN,BLACK);
				}
			else{
				button = 0;
			ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,offFont,Font_16x26,GREEN,BLACK);
				}
			advertising_button(&button);
			break;
			
			case HRS_PAGE:
			ST7789_WriteString(LCD_ICON_FONT_POS_X,LCD_ICON_FONT_POS_Y,hrsPageFont,Font_16x26,GREEN,BLACK);
			if(HRSbutton)
				{
					ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,hrsfont,Font_16x26,GREEN,BLACK);
					//与WIFI 通信，启动HRS
					//开启动app time,定时画 ...
				}
			else
			ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,offFont,Font_16x26,GREEN,BLACK);
			break;
			
			case ESP_PAGE:
			ST7789_WriteString(LCD_ICON_FONT_POS_X,LCD_ICON_FONT_POS_Y,espPageFont,Font_16x26,GREEN,BLACK);
			if(ESPbutton)
			ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,onFont,Font_16x26,GREEN,BLACK);
			else
			ST7789_WriteString(LCD_TIME_POS_X,LCD_TIME_POS_Y,offFont,Font_16x26,GREEN,BLACK);
			break;
			
		}
}

/************************************************
*说明:lcd屏画电量
*函数名: void lcd_RefreshBattery(void)
*参数:
*返回值:
************************************************/
void LCD_RefreshBattery(uint8_t level)
{
	uint16_t x,color;
	ST7789_Fill(LCD_ICON_BATTERY_POS_X,LCD_ICON_BATTERY_POS_Y,LCD_ICON_BATTERY_POS_X+LCD_ICON_BATTERY_WIDTH,LCD_ICON_BATTERY_POS_Y+LCD_ICON_BATTERY_HIGHT,BLACK);
	ST7789_DrawRectangle(LCD_ICON_BATTERY_POS_X,LCD_ICON_BATTERY_POS_Y,\
		LCD_ICON_BATTERY_POS_X+LCD_ICON_BATTERY_WIDTH,									\
		LCD_ICON_BATTERY_POS_Y+LCD_ICON_BATTERY_HIGHT,									\
		WHITE);
	ST7789_Fill(LCD_ICON_BATTERY_POS_X-3,LCD_ICON_BATTERY_POS_Y+2,LCD_ICON_BATTERY_POS_X-2,LCD_ICON_BATTERY_POS_Y+4,WHITE);

		
		switch(level)
		{

			case LCD_BATTERY_LEVEL0:
			case LCD_BATTERY_LEVEL1:
				color = GREEN;
			break;
			case LCD_BATTERY_LEVEL2:
					
				color = YELLOW;
			break;
			case LCD_BATTERY_LEVEL3:
					color = RED;
			break;
					color = WHITE;
			default:
			break;			
		}
		x = LCD_ICON_BATTERY_POS_X+1+(LCD_BATTERT_ONE_LEVEL_WIDTH*level);
ST7789_Fill(x,LCD_ICON_BATTERY_POS_Y+1,\
		LCD_ICON_BATTERY_POS_X+LCD_ICON_BATTERY_WIDTH-1,									\
		LCD_ICON_BATTERY_POS_Y+LCD_ICON_BATTERY_HIGHT-1,									\
		color);
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




