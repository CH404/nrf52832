#define SAADC_GLOBAL
#define USE_DMA_NOT 0

#include "global.h"
#include "nrf_drv_saadc.h"



#define SAADC_BUFF_LEN 5
nrf_saadc_value_t saadcBuff[2][SAADC_BUFF_LEN];
//static uint8_t adc_evt_counter;
float SAADC_Value;

//��ʹ��ʹ��ҲҪ�ṩ
void SAADC_Handler(nrfx_saadc_evt_t const * p_event)
{
 uint8_t i=0;
 	ret_code_t err_code;
	uint32_t val;

       
	if(p_event->type == NRFX_SAADC_EVT_DONE)//������ɣ���buff�����Ż�NRFX_SAADC_EVT_DONE
		{
			//��λ�´β�����׼��
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
	//����adcͨ�����ýṹ�壬��ʹ�õ��˲������ú��ʼ��
	//����ģ����������	AIN4��p28
#if USE_DMA_NOT
	 nrf_saadc_channel_config_t config =
   NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);
	err_code = nrf_drv_saadc_init(NULL,SAADC_Handler);
	G_CHECK_ERROR_CODE_INFO(err_code);
	//����SAADC��0ͨ��
	err_code = nrfx_saadc_channel_init(0,&config);
	G_CHECK_ERROR_CODE_INFO(err_code);

#else
	//���������
	nrf_saadc_channel_config_t config = NRFX_SAADC_DEFAULT_CHANNEL_CONFIG_SE(NRF_SAADC_INPUT_AIN4);
	//��nrf_drv_saadc_init(NULL,SAADC_Handler);��һ������ΪNULL,ʹ��sdk_config������
	//��ʼ��saadc
	err_code = nrf_drv_saadc_init(NULL,SAADC_Handler);
	G_CHECK_ERROR_CODE_INFO(err_code);

	//��ʼ��saadcͨ��0
	err_code = nrf_drv_saadc_channel_init(0,&config);
	G_CHECK_ERROR_CODE_INFO(err_code);

	//���û���		
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


