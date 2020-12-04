#include "global.h"


#define MAX30102_SDA_PIN 27
#define MAX30102_SCL_PIN 26

#define TWI1_SDA_PIN MAX30102_SDA_PIN
#define TWI1_SCL_PIN MAX30102_SCL_PIN

#define MAX_DRV_WADDRESS 0xAE
#define MAX_DRV_RADDRESS 0xAF


#define REG_INTR_STATUS_1 0x00
#define REG_INTR_STATUS_2 0x01
#define REG_INTR_ENABLE_1 0x02
#define REG_INTR_ENABLE_2 0x03
#define REG_FIFO_WR_PTR 0x04
#define REG_OVF_COUNTER 0x05
#define REG_FIFO_RD_PTR 0x06
#define REG_FIFO_DATA 0x07
#define REG_FIFO_CONFIG 0x08
#define REG_MODE_CONFIG 0x09
#define REG_SPO2_CONFIG 0x0A
#define REG_LED1_PA 0x0C
#define REG_LED2_PA 0x0D
#define REG_PILOT_PA 0x10
#define REG_MULTI_LED_CTRL1 0x11
#define REG_MULTI_LED_CTRL2 0x12
#define REG_TEMP_INTR 0x1F
#define REG_TEMP_FRAC 0x20
#define REG_TEMP_CONFIG 0x21
#define REG_PROX_INT_THRESH 0x30
#define REG_REV_ID 0xFE
#define REG_PART_ID 0xFF

nrf_drv_twi_t max = NRF_DRV_TWI_INSTANCE(1);

static void TWI_Init(void)
{
	nrf_drv_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
        ret_code_t err_code;
	config.scl = TWI1_SCL_PIN;
	config.sda = TWI1_SDA_PIN;
	config.frequency = NRF_DRV_TWI_FREQ_100K;
 config.interrupt_priority = APP_IRQ_PRIORITY_HIGH;
  config.clear_bus_init     = false;
	err_code = nrf_drv_twi_init(&max,&config,NULL,NULL);
  G_CHECK_ERROR_CODE_INFO(err_code);
	nrf_drv_twi_enable(&max);
}

/************************************************
*说明:
*函数名:
*参数:
*返回值:
**************************************************/
uint8_t MAX_ReadData(uint8_t address,uint8_t* data_buff)
{
	ret_code_t err;
	nrf_drv_twi_tx(&max,MAX_DRV_WADDRESS,&address,1,true);
	err = nrf_drv_twi_rx(&max,MAX_DRV_RADDRESS,data_buff,1);
	if(err != NRF_SUCCESS)
		return 1;
	else
		return 0;
	
}
/************************************************
*说明:
*函数名:
*参数:
*返回值:
**************************************************/
uint8_t MAX_WriteData(uint8_t address,uint8_t data)
{
	ret_code_t err;
	uint8_t tmp[2];
	tmp[0] = address;
	tmp[1] = data;
	err = nrf_drv_twi_tx(&max,MAX_DRV_WADDRESS,tmp,2,false);
	if(err != NRF_SUCCESS)
		return 1;
	else
		return 0;
}
/************************************************
*说明:
*函数名:
*参数:
*返回值:
**************************************************/
uint8_t MAX_ReadDataLength(uint8_t address,uint8_t *p_data,uint8_t length)
{
		ret_code_t err;

		err = nrf_drv_twi_tx(&max,MAX_DRV_WADDRESS,&address,1,true);
		if(err != NRF_SUCCESS)
			return 1;
		err = nrf_drv_twi_tx(&max,MAX_DRV_WADDRESS,p_data,length,false);
		if(err != NRF_SUCCESS)
			return 1;
		else
			return 0;
}

void MAX_Init(void)
{
		TWI_Init();
}


void max_test(void)
{
	uint8_t tmp;
	MAX_Init();
	MAX_ReadData(0x15,&tmp);
	NRF_LOG_INFO("max %d",tmp);
	while(1);
}
