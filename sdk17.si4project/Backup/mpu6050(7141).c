#define MPU6050_GLOBAL
#include "mpu6050.h"
#include "twi.h"
uint8_t MPU_Init(void)
{
	uint8_t rx_data = 0;
	I2cSimulationInit();
	I2c_Tx_OneByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0x80);	////��λMPU6050
	nrf_delay_ms(100);
	I2c_Tx_OneByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0X00); //����ģ��
	MPU_Set_Gyro_Fsr(3);					//�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);					//���ٶȴ�����,��2g
	MPU_Set_Rate(50);						//���ò�����50Hz
	I2c_Tx_OneByte(MPU_ADDR,MPU_INT_EN_REG,0X00);	//�жϳ���
	I2c_Tx_OneByte(MPU_ADDR,MPU_USER_CTRL_REG,0X00);	//IIC��ģʽ�ر�
	I2c_Tx_OneByte(MPU_ADDR,MPU_FIFO_EN_REG,0X00);		//�ر�FIFO
	I2c_Tx_OneByte(MPU_ADDR,MPU_INTBP_CFG_REG,0X80);	//INT���ŵ���Ч
	
	rx_data = I2c_Rx_OneByte(MPU_ADDR,MPU_DEVICE_ID_REG);
	if(rx_data == MPU_ADDR)
		{
				I2c_Tx_OneByte(MPU_ADDR,MPU_PWR_MGMT1_REG,0X01);//����CLKSEL,PLL X��Ϊ�ο�
				I2c_Tx_OneByte(MPU_ADDR,MPU_PWR_MGMT2_REG,0X00);//���ٶ��������Ƕ�����
				MPU_Set_Rate(50);	//���ò�����Ϊ50Hz
		}else
			return 1;
		return 0;
}


/***************************************
* ������:uint8_t MPU_Read_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff)
* ����: address ������ַ 
*				reg �Ĵ�����ַ
*				length ���ݳ���
*				buff ����buff
* ����:ģ��IIC д
* ����:�ɹ� 0 ,ʧ��1
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
* ������:uint8_t MPU_Write_Length(uint8_t address,uint8_t reg,uint8_t length,uint8_t *buff)
* ����: address ������ַ 
*				reg �Ĵ�����ַ
*				length ���ݳ���
*				buff ����buff
* ����:ģ��IIC ��
* ����:�ɹ� 0 ,ʧ��1
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

//����MPU6050�����Ǵ����������̷�Χ
//fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return I2c_Tx_OneByte(MPU_ADDR,MPU_GYRO_CFG_REG,fsr<<3);//���������������̷�Χ  
}

//����MPU6050���ٶȴ����������̷�Χ
//fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return I2c_Tx_OneByte(MPU_ADDR,MPU_ACCEL_CFG_REG,fsr<<3);//���ü��ٶȴ����������̷�Χ  
}
//����MPU6050�����ֵ�ͨ�˲���
//lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_LPF(uint16_t lpf)
{
	uint8_t data=0;
	if(lpf>=188)data=1;
	else if(lpf>=98)data=2;
	else if(lpf>=42)data=3;
	else if(lpf>=20)data=4;
	else if(lpf>=10)data=5;
	else data=6; 
	return I2c_Tx_OneByte(MPU_ADDR,MPU_CFG_REG,data);//�������ֵ�ͨ�˲���  
}


//����MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
//rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//    ����,����ʧ�� 
uint8_t MPU_Set_Rate(uint16_t rate)
{
	uint8_t data;
	if(rate>1000)rate=1000;
	if(rate<4)rate=4;
	data=1000/rate-1;
	data=I2c_Tx_OneByte(MPU_ADDR,MPU_SAMPLE_RATE_REG,data);	//�������ֵ�ͨ�˲���
 	return MPU_Set_LPF(rate/2);	//�Զ�����LPFΪ�����ʵ�һ��
}

//�õ��¶�ֵ
//����ֵ:�¶�ֵ(������100��)
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
//�õ�������ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
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
//�õ����ٶ�ֵ(ԭʼֵ)
//gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//    ����,�������
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

