#define SAADC_GLOBAL
#define USE_DMA_NOT 0

#include "global.h"
#include "nrf_drv_saadc.h"



#define SAADC_BUFF_LEN 5
nrf_saadc_value_t saadcBuff[2][SAADC_BUFF_LEN];
//static uint8_t adc_evt_counter;
float SAADC_Value;

//即使不使用也要提供
void SAADC_Handler(nrfx_saadc_evt_t const * p_event)
{
 uint8_t i=0;
 	ret_code_t err_code;
	uint32_t val;

       
	if(p_event->type == NRFX_SAADC_EVT_DONE)//采样完成，将buff填满才会NRFX_SAADC_EVT_DONE
		{
			//在位下次采样做准备
			nrfx_saadc_buffer_convert(p_event->data.done.p_buffer,SAADC_BUFF_LEN);
			G_CHECK_ERROR_CODE_INFO(err_code);

			for(i=0;i<SAADC_BUFF_LEN;i++)
				{
					val += p_event->data.done.p_buffer[i];
					NRF_LOG_INFO("sample Value:%d ",p_event->data.done.p_buffer[i]);
				}
			//NRF_LOG_INFO("voltage = %.3fv",NRF_LOG_FLOAT_MARKER,NRF_LOG_FLOAT(val*7.2/1024));				
			val /= SAADC_BUFF_LEN;
			SAADC_Value = val*7.2/1024;
		//	NRF_LOG_INFO("voltage",NRF_LOG_FLOAT_MARKER "V",NRF_LOG_FLOAT(ret));
                       
		//	adc_evt_counter++;
		//	NRF_LOG_INFO("counter = %d",adc_evt_counter);				
		
		}
}
void SAADC_Init(void)
{
	ret_code_t err_code;
	//定义adc通道配置结构体，并使用单端采样配置宏初始化
	//配置模拟输入引脚	AIN4是p28
#if USE_DMA_NOT
	 nrf_saadc_channel_config_t config =
   NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);
	err_code = nrf_drv_saadc_init(NULL,SAADC_Handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
	//配置SAADC的0通道
	err_code = nrfx_saadc_channel_init(0,&config);
	G_CHECK_ERROR_CODE_INFO(err_code);

#else
	//单缓存采样
	nrf_saadc_channel_config_t config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);
	//当nrf_drv_saadc_init(NULL,SAADC_Handler);第一个参数为NULL,使用sdk_config的配置
	//初始化saadc
	err_code = nrf_drv_saadc_init(NULL,SAADC_Handler);
	G_CHECK_ERROR_CODE_INFO(err_code);

	//初始化saadc通道0
	err_code = nrf_drv_saadc_channel_init(0,&config);
	G_CHECK_ERROR_CODE_INFO(err_code);

	//设置缓存		
	err_code = nrf_drv_saadc_buffer_convert(saadcBuff[0],SAADC_BUFF_LEN);
	G_CHECK_ERROR_CODE_INFO(err_code);

	err_code = nrf_drv_saadc_buffer_convert(saadcBuff[0],SAADC_BUFF_LEN);
	G_CHECK_ERROR_CODE_INFO(err_code);


#endif

}
#if USE_DMA_NOT
void SAADC_Start(uint8_t channel, nrf_saadc_value_t * p_value)
{
	nrfx_saadc_sample_convert(channel,p_value);
}
#else
void SAADC_Start()
{
		nrfx_saadc_sample();
}
#endif


