#include "st7789.h"

#include "nrf_gpio.h"
#include "nrf_drv_spi.h"
#include "nrf_delay.h"


#define SCREEN_LCD_SDA_PIN 12
#define SCREEN_LCD_SCL_PIN 10
#define SCREEN_LCD_DC_PIN		11
#define SCREEN_LCD_RESET_PIN	8
#define SCREEN_LCD_CS_PIN			9
#define SCREEN_LCD_FMARK_PIN	13
#define SCREEN_LCD_LED_PIN	15

static const nrfx_spim_t spim0 = NRFX_SPIM_INSTANCE(0);

void spim_evt_handler(nrfx_spim_evt_t const * p_event,void * p_context)
{

}

void HardWareInit(void)
{
	nrf_gpio_cfg_output(SCREEN_LCD_LED_PIN);
	nrf_gpio_cfg_output(SCREEN_LCD_DC_PIN);
	nrf_gpio_cfg_output(SCREEN_LCD_RESET_PIN);
	nrf_gpio_pin_set(SCREEN_LCD_LED_PIN);//ио╣Г
	nrf_gpio_pin_set(SCREEN_LCD_RESET_PIN);
	nrf_gpio_pin_set(SCREEN_LCD_DC_PIN);

	nrfx_spim_config_t config = NRFX_SPIM_DEFAULT_CONFIG;
	config.mosi_pin = SCREEN_LCD_SDA_PIN;
	config.sck_pin = SCREEN_LCD_SCL_PIN;
	nrfx_spim_init(&spim0,&config,spim_evt_handler,NULL)
}
void st7789Init(void)
{
	
}


