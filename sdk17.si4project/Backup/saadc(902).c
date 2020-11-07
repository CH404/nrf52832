#define SAADC_GLOBAL

#include "global.h"
void SAADC_Init(void)
{
	ret_code_t err_code;
	//定义adc通道配置结构体，并使用单端采样配置宏初始化
	 nrf_saadc_channel_config_t config =
        NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN0);
	err_code = nrf_drv_saadc_init(NULL,NULL);
	G_CHECK_ERROR_CODE_INFO(err_code);
	err_code = nrfx_saadc_channel_init(0,&config);
	G_CHECK_ERROR_CODE_INFO(err_code);
}