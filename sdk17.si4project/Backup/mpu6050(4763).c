#define MPU6050_GLOBAL
#include "mpu6050.h"
#include "twi.h"
#include "app_uart.h"
#include "nrf_uart.h"
uint8_t MPU_Init(void)
{
	uint8_t rx_data = 0;
	I2cSimulationInit();
	I2c_Tx_OneByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0x80);	////复位MPU6050
	nrf_delay_ms(100);
	I2c_Tx_OneByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0X00); //唤醒模块
	MPU_Set_Gyro_Fsr(3);					//陀螺仪传感器,±2000dps
	MPU_Set_Accel_Fsr(0);					//加速度传感器,±2g
	MPU_Set_Rate(50);						//设置采样率50Hz
	I2c_Tx_OneByte(MPU_ADDR,MPU_INT_EN_REG,0X00);	//中断除能
	I2c_Tx_OneByte(MPU_ADDR,MPU_USER_CTRL_REG,0X00);	//IIC主模式关闭
	I2c_Tx_OneByte(MPU_ADDR,MPU_FIFO_EN_REG,0X00);		//关闭FIFO
	I2c_Tx_OneByte(MPU_ADDR,MPU_INTBP_CFG_REG,0X80);	//INT引脚低有效
	
	rx_data = I2c_Rx_OneByte(MPU_ADDR,MPU_DEVICE_ID_REG);
	if(rx_data == MPU_ADDR)
		{
				I2c_Tx_OneByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0X01);//设置CLKSEL,PLL X轴为参考
				I2c_Tx_OneByte(MPU_ADDR,MPU_PWR_MGMT2_REG,0X00);//加速度与陀螺仪都工作
				MPU_Set_Rate(50);	//设置采样率为50Hz
		}else
			return 1;
		return 0;
}


/***************************************
* 函数名:uint8_t MPU_Read_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff)
* 参数: address 器件地址 
*				reg 寄存器地址
*				length 数据长度
*				buff 数据buff
* 功能:模拟IIC 写
* 返回:成功 0 ,失败1
****************************************/
uint8_t MPU_Read_Length(uint8_t address, uint8_t const reg,uint8_t length,uint8_t *buff)
{
	uint8_t result;

	result = I2c_Tx(address,&reg,1,true,true,0);
	if(result!=SUCCESS_I2C)
	{
		return 1;
	}
	result = I2c_Rx(address,buff,length,0);
	if(result==ERROR_I2C)
	{
		return 1;
	}
return 0;
}
/***************************************
* 函数名:uint8_t MPU_Write_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff)
* 参数: address 器件地址 
*				reg 寄存器地址
*				length 数据长度
*				buff 数据buff
* 功能:模拟IIC 读
* 返回:成功 0 ,失败1
****************************************/
uint8_t MPU_Write_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff)
{
	uint8_t result;
        
		result = I2c_Tx(address,&reg,1,true,true,0);
			if(result!=SUCCESS_I2C)
	{
		return 1;
	}
		result = I2c_Tx(address,buff,length,false,false,0);
						if(result!=SUCCESS_I2C)
	{
		return 1;
	}
						return 0;
}

//设置MPU6050陀螺仪传感器满量程范围
//fsr:0,±250dps;1,±500dps;2,±1000dps;3,±2000dps
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return I2c_Tx_OneByte(MPU_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//设置陀螺仪满量程范围  
}

//设置MPU6050加速度传感器满量程范围
//fsr:0,±2g;1,±4g;2,±8g;3,±16g
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return I2c_Tx_OneByte(MPU_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//设置加速度传感器满量程范围  
}
//设置MPU6050的数字低通滤波器
//lpf:数字低通滤波频率(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return I2c_Tx_OneByte(MPU_ADDR,MPU_CFG_REG,data);//设置数字低通滤波器  
}


//设置MPU6050的采样率(假定Fs=1KHz)
//rate:4~1000(Hz)
//返回值:0,设置成功
//    其他,设置失败 
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=I2c_Tx_OneByte(MPU_ADDR,MPU_SAMPLE_RATE_REG,data);	//设置数字低通滤波器
 	return MPU_Set_LPF(rate/2);	//自动设置LPF为采样率的一半
}

//得到温度值
//返回值:温度值(扩大了100倍)
short MPU_Get_Temperature(void)
{
    uint8_t buf[2]; 
    short raw;
	float temp;
	MPU_Read_Length(MPU_ADDR,MPU_TEMP_OUTH_REG,2,buf); 
    raw=((uint16_t)buf[0]<<8)|buf[1];  
    temp=36.53+((double)raw)/340;  
    return temp*100;;
}
//得到陀螺仪值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Gyroscope(short *gx,short *gy,short *gz)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Length(MPU_ADDR,MPU_GYRO_XOUTH_REG,6,buf);
	if(res==0)
	{
		*gx=((uint16_t)buf[0]<<8)|buf[1];  
		*gy=((uint16_t)buf[2]<<8)|buf[3];  
		*gz=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
//得到加速度值(原始值)
//gx,gy,gz:陀螺仪x,y,z轴的原始读数(带符号)
//返回值:0,成功
//    其他,错误代码
uint8_t MPU_Get_Accelerometer(short *ax,short *ay,short *az)
{
    uint8_t buf[6],res;  
	res=MPU_Read_Length(MPU_ADDR,MPU_ACCEL_XOUTH_REG,6,buf);
	if(res==0)
	{
		*ax=((uint16_t)buf[0]<<8)|buf[1];  
		*ay=((uint16_t)buf[2]<<8)|buf[3];  
		*az=((uint16_t)buf[4]<<8)|buf[5];
	} 	
    return res;;
}
void uart_error_handle(app_uart_evt_t * p_event)
{
	if(p_event->evt_type == APP_UART_COMMUNICATION_ERROR)
		{
		//	NRF_LOG_INFO(APP_UART_COMMUNICATION_ERROR);
		}
	else if(p_event ->evt_type == APP_UART_FIFO_ERROR)
		{
	//	NRF_LOG_INFO(APP_UART_FIFO_ERROR);
		}
}
#define RX_PIN_NUMBER 29
#define TX_PIN_NUMBER	30
#define RTS_PIN_NUMBER	7
#define CTS_PIN_NUMBER	5
#define UART_RX_BUF_SIZE	256
#define UART_TX_BUF_SIZE	256
#define BLE_UARTS_BUFF_MAX 25

/**********************************************************
函数名：void uart_init(void)	
输入：
输出：
作用：初始化uarts 模块
**********************************************************/
void uart_init(void)
{
//	uart_RX_flage = false;
	ret_code_t err_code;
	app_uart_comm_params_t const comm_params = {
		.rx_pin_no = RX_PIN_NUMBER,
		.tx_pin_no = TX_PIN_NUMBER,
		.rts_pin_no = RTS_PIN_NUMBER,
		.cts_pin_no = CTS_PIN_NUMBER,
		.flow_control = APP_UART_FLOW_CONTROL_DISABLED,
		.use_parity = false,
		.baud_rate = NRF_UART_BAUDRATE_115200
	};
	 APP_UART_FIFO_INIT(&comm_params,
                       UART_RX_BUF_SIZE,
                       UART_TX_BUF_SIZE,
                       uart_error_handle,
                       APP_IRQ_PRIORITY_LOWEST,
                       err_code);
}


//传送数据给匿名四轴上位机软件(V2.6版本)
//fun:功能字. 0XA0~0XAF
//data:数据缓存区,最多28字节!!
//len:data区有效数据个数
void usart1_niming_report(uint8_t fun,uint8_t*data,uint8_t len)
{
	uint8_t send_buf[32];
	uint8_t i;
	if(len>28)return;	//最多28字节数据 
	send_buf[len+3]=0;	//校验数置零
	send_buf[0]=0X88;	//帧头
	send_buf[1]=fun;	//功能字
	send_buf[2]=len;	//数据长度
	for(i=0;i<len;i++)send_buf[3+i]=data[i];			//复制数据
	for(i=0;i<len+3;i++)send_buf[len+3]+=send_buf[i];	//计算校验和	
	for(i=0;i<len+4;i++)app_uart_put(send_buf[i]);	//发送数据到串口1 
}
//发送加速度传感器数据和陀螺仪数据
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
void mpu6050_send_data(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz)
{
	uint8_t tbuf[12]; 
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;
	usart1_niming_report(0XA1,tbuf,12);//自定义帧,0XA1
}	
//通过串口1上报结算后的姿态数据给电脑
//aacx,aacy,aacz:x,y,z三个方向上面的加速度值
//gyrox,gyroy,gyroz:x,y,z三个方向上面的陀螺仪值
//roll:横滚角.单位0.01度。 -18000 -> 18000 对应 -180.00  ->  180.00度
//pitch:俯仰角.单位 0.01度。-9000 - 9000 对应 -90.00 -> 90.00 度
//yaw:航向角.单位为0.1度 0 -> 3600  对应 0 -> 360.0度
void usart1_report_imu(short aacx,short aacy,short aacz,short gyrox,short gyroy,short gyroz,short roll,short pitch,short yaw)
{
	uint8_t tbuf[28]; 
	uint8_t i;
	for(i=0;i<28;i++)tbuf[i]=0;//清0
	tbuf[0]=(aacx>>8)&0XFF;
	tbuf[1]=aacx&0XFF;
	tbuf[2]=(aacy>>8)&0XFF;
	tbuf[3]=aacy&0XFF;
	tbuf[4]=(aacz>>8)&0XFF;
	tbuf[5]=aacz&0XFF; 
	tbuf[6]=(gyrox>>8)&0XFF;
	tbuf[7]=gyrox&0XFF;
	tbuf[8]=(gyroy>>8)&0XFF;
	tbuf[9]=gyroy&0XFF;
	tbuf[10]=(gyroz>>8)&0XFF;
	tbuf[11]=gyroz&0XFF;	
	tbuf[18]=(roll>>8)&0XFF;
	tbuf[19]=roll&0XFF;
	tbuf[20]=(pitch>>8)&0XFF;
	tbuf[21]=pitch&0XFF;
	tbuf[22]=(yaw>>8)&0XFF;
	tbuf[23]=yaw&0XFF;
	usart1_niming_report(0XAF,tbuf,28);//飞控显示帧,0XAF
} 


void MPU_task(void *parameters)
{
	float pitch;
	float roll;
	float yaw;
		short aacx,aacy,aacz;		//加速度传感器原始数据
	short gyrox,gyroy,gyroz;	//陀螺仪原始数据
	short temp;	
	uint8_t report=1;
	while(1)
		{
			if(0 == mpu_dmp_get_data(&pitch,&roll,&yaw))
				{
					temp = MPU_Get_Temperature();
					MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//得到加速度传感器数据
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//得到陀螺仪数据
			if(report)mpu6050_send_data(aacx,aacy,aacz,gyrox,gyroy,gyroz);//用自定义帧发送加速度和陀螺仪原始数据
				vTaskDelay(200);
			//if(report)usart1_report_imu(aacx,aacy,aacz,gyrox,gyroy,gyroz,(int)(roll*100),(int)(pitch*100),(int)(yaw*10));
		//app_uart_put(report);
		vTaskDelay(200);
			}
		}
}


