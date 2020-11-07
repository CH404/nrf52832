#define SAADC_GLOBAL

#include "global.h"
#include "nrf_drv_saadc.h"
//��ʹ��ʹ��ҲҪ�ṩ
void SAADC_Handler(nrfx_saadc_evt_t const * p_event){};
void SAADC_Init(void)
{
	ret_code_t err_code;
	//����adcͨ�����ýṹ�壬��ʹ�õ��˲������ú��ʼ��
	//����ģ����������	AIN4��p28
	 nrf_saadc_channel_config_t config =
        NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);
	err_code = nrf_drv_saadc_init(NULL,SAADC_Handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
	//����SAADC��0ͨ��
	err_code = nrfx_saadc_channel_init(0,&config);
	G_CHECK_ERROR_CODE_INFO(err_code);
}