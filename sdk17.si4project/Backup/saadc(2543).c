#define SAADC_GLOBAL

#include "global.h"
#include "nrf_drv_saadc.h"
//即使不使用也要提供
void SAADC_Handler(nrfx_saadc_evt_t const * p_event){};
void SAADC_Init(void)
{
	ret_code_t err_code;
	//定义adc通道配置结构体，并使用单端采样配置宏初始化
	//配置模拟输入引脚	AIN4是p28
	 nrf_saadc_channel_config_t config =
        NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);
	err_code = nrf_drv_saadc_init(NULL,SAADC_Handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
	//配置SAADC的0通道
	err_code = nrfx_saadc_channel_init(0,&config);
	G_CHECK_ERROR_CODE_INFO(err_code);
}