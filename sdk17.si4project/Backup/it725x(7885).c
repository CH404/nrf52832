#include "it725x.h"
#include "twi.h"
#include "nrf_drv_twi.h"
#include "nrf_drv_gpiote.h"
#include "global.h"
#include <math.h>
//cap sensor driver
//使用 IT7251 
#if 1
#define IT725X_Write
#define IT725X_WriteLength

#define IT725X_Read
#define IT725X_ReadLength

#define PI 3.14
#define MAX_ANGLE_DOWN 30
#define MIN_ANGLE_DOWN  -30

#define MAX_ANGLE_UP	150
#define MIN_ANGLE_UP  -150

#define MAX_ANGLE_LEFT	-60
#define MIN_ANGLE_LEFT  -120

#define MAX_ANGLE_RIGHT 120
#define MIN_ANGLE_RIGHT 60


#define IT725X_Reset_Pin 7
#define IT725X_Int_Pin 4
#define IT725X_SCL_PiN 6
#define IT725X_SDA_Pin 5


#define TWI0_SDA_PIN IT725X_SDA_Pin
#define TWI0_SCL_PIN IT725X_SCL_PiN
#define IT725X_ADDRESS 0x46

ScreenEvent_t handle;

#if 1
static void IT725X_handler(nrfx_gpiote_pin_t pin,nrf_gpiote_polarity_t action)
{
	  ret_code_t err_code;
  uint8_t buffer_type=0x80;
  uint8_t rx_buff[2] = {0x00};
	BaseType_t HigherPriorityTaskWoken;

	if(pin != IT725X_Int_Pin)
        {
		return;
        }
			if(action == NRF_GPIOTE_POLARITY_HITOLO)
				{
			//	NRF_LOG_INFO("HITOLO");
                   IT725X_GetBuffData(QUERY_BUFFER,rx_buff,2);
               //   NRF_LOG_INFO("data %d",rx_buff[0]);
      if((rx_buff[0]&0x03) == 0x00)
      {
              if(rx_buff[0]&0x80)	//new packet
                      {
                         if(IT725X_GenerateEvent(&handle))
                         	{
												 #if USE_LCD_TASK
												 xTaskNotifyFromISR(lcdRefreshHandle,IT725_TO_LCD_NOTIFY_BIT, eSetBits, &HigherPriorityTaskWoken);
												 portYIELD_FROM_ISR(HigherPriorityTaskWoken);
												 #endif
                         	}
                      }
              else if((rx_buff[0]&0x40))
                      {
                              NRF_LOG_INFO("No packet but finger/pen still touched");
                      }
      }
      else if((rx_buff[0]&0x03) == 0x01)
      {
              NRF_LOG_INFO("IIC bus busy");
              
      }
      else if(rx_buff[0]&0x02)
      {
              NRF_LOG_INFO("IIC bus ERROR");
         
             
      }

				}
			else if(action == NRF_GPIOTE_POLARITY_LOTOHI)
				{
			NRF_LOG_INFO("LOTOHI");

 
                                }
}
#endif

#define HWTWI0
#ifdef HWTWI0
static const nrf_drv_twi_t it725x = NRF_DRV_TWI_INSTANCE(0);
#endif
#ifdef HWTWI1
static const nrf_drv_twi_t it725x = NRF_DRV_TWI_INSTANCE(1);
#endif
bool twi_done = false;
static void twi_handler(nrf_drv_twi_evt_t const *p_evt,void *p_context)
{
	switch(p_evt->type)
		{
			case NRFX_TWI_EVT_DONE:
				NRF_LOG_INFO("NRFX_TWI_EVT_DONE");
				twi_done = true;
				break;
			case NRFX_TWI_EVT_ADDRESS_NACK:
				NRF_LOG_INFO("address Nack");
				break;
			case NRFX_TWI_EVT_DATA_NACK:
				NRF_LOG_INFO("date Nack");
				break;
			case NRFX_TWI_EVT_BUS_ERROR:
				NRF_LOG_INFO("bus");
				break;
			case NRFX_TWI_EVT_OVERRUN:
				NRF_LOG_INFO("overrun");
				break;
		}
}

#if defined (HWTWI0) || defined (HWTWI1)
/************************************************
说明:TWI 初始化
函数名:ret_code_t TwiInit(void)
参数:
返回值:
**************************************************/
static void TWI_Init(void)
{
	nrf_drv_twi_config_t config = NRFX_TWI_DEFAULT_CONFIG;
        ret_code_t err_code;
#ifdef HWTWI0
	config.scl = TWI0_SCL_PIN;
	config.sda = TWI0_SDA_PIN;
	config.frequency = NRF_DRV_TWI_FREQ_100K;
 config.interrupt_priority = APP_IRQ_PRIORITY_HIGH;
  config.clear_bus_init     = false;
	err_code = nrf_drv_twi_init(&it725x,&config,NULL,NULL);
  G_CHECK_ERROR_CODE_INFO(err_code);
	nrf_drv_twi_enable(&it725x);
#endif
}

#endif
/************************************************
*说明: Hardware reset
*函数名: void IT725X_Reset(void)
*参数:
*返回值:
************************************************/
static void IT725X_Reset(void)
{
  nrf_gpio_cfg_output(IT725X_Reset_Pin);
  nrf_gpio_pin_clear(IT725X_Reset_Pin);
  nrf_delay_ms(1000);
  nrf_gpio_pin_set(IT725X_Reset_Pin);
  nrf_delay_ms(1000);
}
/************************************************
*说明:   host中断配置
*函数名: void IT725X_InterruptInit(void)
*参数:
*返回值:
************************************************/
 void IT725X_InterruptInit(void)
{
	ret_code_t err_code;
	nrf_drv_gpiote_init();
	nrf_drv_gpiote_in_config_t config = NRFX_GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
	config.pull = NRF_GPIO_PIN_PULLUP;	
err_code = nrf_drv_gpiote_in_init(IT725X_Int_Pin,&config,IT725X_handler);	
G_CHECK_ERROR_CODE_INFO(err_code);
	nrf_drv_gpiote_in_event_enable(IT725X_Int_Pin,true);
}




/***************************************
* 功能: 发送set command
* 函数: uint16 IT725X_SetCommand(uint8_t address,uint8_t *commandAndData,uint8_t length)
* 参数: address 器件地址 
*				commandAndData 命令,数据
*				length 长度
* 返回: it7251 错误码
****************************************/
uint16_t IT725X_SetCommand(uint8_t *commandAndData,uint8_t length)
{
	uint8_t response[2] = {COMMAND_RESPONSE_BUFFER};
	ret_code_t err_code;
	nrf_drv_twi_tx(&it725x,IT725X_ADDRESS,commandAndData,length,false);
	nrf_drv_twi_tx(&it725x,IT725X_ADDRESS,response,1,true);
	nrf_drv_twi_rx(&it725x,IT725X_ADDRESS,response,2);
	return (((uint16_t)response[0]<<8) || (uint16_t)response[1]);
}

/************************************************
*说明: 发送get command
*函数名: void IT725X_GetCommand(uint8_t address,uint8_t *commandAndData,uint8_t length,uint8_t*receive,uint8_t len)
*参数: receive 接收buffer
*				len			长度
*
*返回值:
************************************************/
void IT725X_GetCommand(uint8_t *commandAndData,uint8_t length,uint8_t*receive,uint8_t len)
{
	uint8_t response[1] = {COMMAND_RESPONSE_BUFFER};
	ret_code_t err_code;
	nrf_drv_twi_tx(&it725x,IT725X_ADDRESS,commandAndData,length,false);
	nrf_drv_twi_tx(&it725x,IT725X_ADDRESS,response,1,true);
	nrf_drv_twi_rx(&it725x,IT725X_ADDRESS,receive,len);
}
/************************************************
*说明: 发送get 
*函数名: void IT725X_GetBuffData(uint8_t *buffer_type,uint8_t*receive,uint8_t len)
*参数: buffer_type IT725x buffer type
*				receive 接收buffer
*				len			长度
*
*返回值:
************************************************/
void IT725X_GetBuffData(uint8_t buffer_type,uint8_t*receive,uint8_t len)
{
	ret_code_t err_code;
	nrf_drv_twi_tx(&it725x,IT725X_ADDRESS,&buffer_type,1,false);
	nrf_drv_twi_rx(&it725x,IT725X_ADDRESS,receive,len);
}

/************************************************
*说明: 获取触摸的第一点 和 最后一点
*函数名: bool IT725X_GetTwoPoint(ScreenEvent_t *point)
*参数: 
*				
*				
*
*返回值:	true 落下离开 一次操作
************************************************/
bool IT725X_GetTwoPoint(ScreenEvent_t *point)
{       
	uint8_t point_buff[6] = {0x00};
	static uint8_t x_tmp,y_tmp;
	static bool frist = true;
	IT725X_GetBuffData(POINT_INFORMATION_BUFFER,point_buff,6);
	//NRF_LOG_INFO("x2:%d,y2:%d,pre %d",point_buff[2],point_buff[4],point_buff[5]);
	
	if(frist)
		{
			point->start.x = point_buff[2];
			point->start.y = point_buff[4];
			frist = false;
			return false;
		}
//	if((point_buff[5]&0x0F) == 0)
	if(point_buff[2] == 0 && point_buff[4]==0)
		{
			point->end.x =x_tmp ; 
			point->end.y =y_tmp;
			frist = true;
                       // NRF_LOG_INFO("\nx2:%d,y2:%d\nx1:%d,y1:%d",point->end.x,point->end.y,point->start.x,point->start.y);
                        NRF_LOG_INFO("x2:%d,y2:%d",point->end.x,point->end.y);
        NRF_LOG_INFO("x1:%d,y1:%d",point->start.x,point->start.y);
			return true;
                        
		}
	x_tmp = point_buff[2];
	y_tmp = point_buff[4];
	return false;
}

bool IT725X_GenerateEvent(ScreenEvent_t *point)
{
	if(!IT725X_GetTwoPoint(point))
		return false;
	if(point->start.x == point->end.x || point->start.y == point->end.y)
		{
			point->evt = tap;
			point->dir = not;
		NRF_LOG_INFO("%d,%d",point->dir,point->evt);
			return true;
		}
	double_t angle;
#if 0
	double_t x1 =(double_t)point->start.x;
	double_t y1 =(double_t)point->start.y;
	double_t x2 =(double_t)point->end.x;
	double_t y2 =(double_t)point->end.y;
	angle = atan2((x2-x1),(y2-y1))*(180.0/PI);
#else	
	angle = atan2(((double_t)point->end.x-(double_t)point->start.x),((double_t)point->end.y-(double_t)point->start.y))*(180.0/PI);
#endif
	if(MIN_ANGLE_LEFT<angle && MAX_ANGLE_LEFT >angle)
		point->dir = left;
	else if(MIN_ANGLE_DOWN<angle && MAX_ANGLE_DOWN >angle)
		point->dir = down;
	else if(MIN_ANGLE_RIGHT<angle && MAX_ANGLE_RIGHT >angle)
		point->dir = right;
	else if((MIN_ANGLE_UP>angle && angle < -180)|| (angle>MAX_ANGLE_UP && angle <180))
		point->dir = up;
	else
		point->dir = not;

	point->evt = drag;
	
	NRF_LOG_INFO("%d,%d",point->dir,point->evt);
	NRF_LOG_INFO("angle:" NRF_LOG_FLOAT_MARKER , NRF_LOG_FLOAT(angle));
        return true;
	
	
}

/************************************************
*说明: IT7251 配置中断通知
*函数名: void IT725X_GetBuffData(uint8_t *buffer_type,uint8_t*receive,uint8_t len)
*参数: buffer_type IT725x buffer type
*				receive 接收buffer
*				len			长度
*
*返回值:
************************************************/
static uint16_t IT725X_InterruptNotificationEnable(void)
{
	uint16_t err_code;
	uint8_t tx1_buff[] = {COMMAND_BUFFER,0x02,0x04,0x01,0x00};
	uint8_t tx2_buff[] = {COMMAND_BUFFER,0x01,0x04};
	uint8_t rx2_buff[2];
	err_code = IT725X_SetCommand(tx1_buff,sizeof(tx1_buff));
	if(err_code != IT725X_SUCCESS)
		return err_code;
	IT725X_GetCommand(tx2_buff,sizeof(tx2_buff),rx2_buff,2);
	if(rx2_buff[0] == tx1_buff[3] && rx2_buff[1] == tx1_buff[4])
		{
			return IT725X_SUCCESS;
		}
}
/************************************************
*说明: 空闲低功耗使能
*函数名: void IT725X_IdleModeEnable(void)
*参数:
*返回值:
************************************************/
static void IT725X_IdleModeEnable(void)
{
	uint8_t tx1_buff[]={COMMAND_BUFFER,0x04,0x00,0x01};
	IT725X_SetCommand(tx1_buff,sizeof(tx1_buff));
}

/************************************************
*说明: IT725X 初始化硬件
*函数名: void IT725X_HardwareInit(void)
*参数:
*返回值:
************************************************/
 void IT725X_HardwareInit(void)
{
	TWI_Init();
	IT725X_Reset();
	IT725X_InterruptInit();
}



/************************************************
*说明: 做IT725x 的初始化
*函数名: void IT725X_Init(void)
*参数:
*返回值:
************************************************/
void IT725X_Init(void)
{
	IT725X_HardwareInit();
	IT725X_InterruptNotificationEnable();
}

#endif

