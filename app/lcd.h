#ifndef __LCD_H
#define __LCD_H




#ifdef LCD_GLOBAL
#define LCD_EXT
#else
#define LCD_EXT extern
#endif

//#define SCREEN_LCD_RESET_PIN	8
//#define SCREEN_LCD_CS_PIN			9
//#define SCREEN_LCD_SCL_PIN 10
//#define SCREEN_LCD_DC_PIN		11
//#define SCREEN_LCD_SDA_PIN 12
//#define SCREEN_LCD_FMARK_PIN	13
#define SCREEN_LCD_LED_PIN	15





enum UI_PAGE
{
	MAIN_PAGE = 1,
	BLE_PAGE,	
	HRS_PAGE,
  ESP_PAGE
};


//FontDef Font_16x26 = {16,26,Font16x26};
//FontDef Font_7x10 = {7,10,Font7x10};
LCD_EXT bool BDbutton;
LCD_EXT bool HRSbutton;
LCD_EXT bool ESPbutton;


LCD_EXT void LCD_Init(void);

LCD_EXT void LCD_RefreshMainPage(char *datebuff,char *timebuff );

LCD_EXT void LCD_Refresh(char *datebuff,char *timebuff,uint8_t *UI_Page,bool rtc_evt);

//void LCD_RefreshMainPage(char *datebuff,char *timebuff );
LCD_EXT void LCD_RefreshPage(char *datebuff,char *timebuff,uint8_t ui_page);

//LCD_EXT uint8_t currentUIPage;
LCD_EXT void LCD_RefreshBattery(uint8_t level);


#endif
