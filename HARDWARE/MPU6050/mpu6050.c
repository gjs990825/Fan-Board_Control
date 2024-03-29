#include "mpu6050.h"
#include "iic.h"
#include <math.h>
#include "my_lib.h"

// MPU6050, 硬件I2c地址 0x68，模拟i2c地址0xD0   AD0高电平时地址为0x69 模拟IIC地址0xD2
#define MPU6050_ADDRESS 0xD0 // 0x69

#define DMP_MEM_START_ADDR 0x6E
#define DMP_MEM_R_W 0x6F

#define MPU_RA_XG_OFFS_TC 0x00  //[7] PWR_MODE, [6:1] XG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU_RA_YG_OFFS_TC 0x01  //[7] PWR_MODE, [6:1] YG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU_RA_ZG_OFFS_TC 0x02  //[7] PWR_MODE, [6:1] ZG_OFFS_TC, [0] OTP_BNK_VLD
#define MPU_RA_X_FINE_GAIN 0x03 //[7:0] X_FINE_GAIN
#define MPU_RA_Y_FINE_GAIN 0x04 //[7:0] Y_FINE_GAIN
#define MPU_RA_Z_FINE_GAIN 0x05 //[7:0] Z_FINE_GAIN
#define MPU_RA_XA_OFFS_H 0x06   //[15:0] XA_OFFS
#define MPU_RA_XA_OFFS_L_TC 0x07
#define MPU_RA_YA_OFFS_H 0x08 //[15:0] YA_OFFS
#define MPU_RA_YA_OFFS_L_TC 0x09
#define MPU_RA_ZA_OFFS_H 0x0A //[15:0] ZA_OFFS
#define MPU_RA_ZA_OFFS_L_TC 0x0B
#define MPU_RA_PRODUCT_ID 0x0C   // Product ID Register
#define MPU_RA_XG_OFFS_USRH 0x13 //[15:0] XG_OFFS_USR
#define MPU_RA_XG_OFFS_USRL 0x14
#define MPU_RA_YG_OFFS_USRH 0x15 //[15:0] YG_OFFS_USR
#define MPU_RA_YG_OFFS_USRL 0x16
#define MPU_RA_ZG_OFFS_USRH 0x17 //[15:0] ZG_OFFS_USR
#define MPU_RA_ZG_OFFS_USRL 0x18
#define MPU_RA_SMPLRT_DIV 0x19
#define MPU_RA_CONFIG 0x1A
#define MPU_RA_GYRO_CONFIG 0x1B
#define MPU_RA_ACCEL_CONFIG 0x1C
#define MPU_RA_FF_THR 0x1D
#define MPU_RA_FF_DUR 0x1E
#define MPU_RA_MOT_THR 0x1F
#define MPU_RA_MOT_DUR 0x20
#define MPU_RA_ZRMOT_THR 0x21
#define MPU_RA_ZRMOT_DUR 0x22
#define MPU_RA_FIFO_EN 0x23
#define MPU_RA_I2C_MST_CTRL 0x24
#define MPU_RA_I2C_SLV0_ADDR 0x25
#define MPU_RA_I2C_SLV0_REG 0x26
#define MPU_RA_I2C_SLV0_CTRL 0x27
#define MPU_RA_I2C_SLV1_ADDR 0x28
#define MPU_RA_I2C_SLV1_REG 0x29
#define MPU_RA_I2C_SLV1_CTRL 0x2A
#define MPU_RA_I2C_SLV2_ADDR 0x2B
#define MPU_RA_I2C_SLV2_REG 0x2C
#define MPU_RA_I2C_SLV2_CTRL 0x2D
#define MPU_RA_I2C_SLV3_ADDR 0x2E
#define MPU_RA_I2C_SLV3_REG 0x2F
#define MPU_RA_I2C_SLV3_CTRL 0x30
#define MPU_RA_I2C_SLV4_ADDR 0x31
#define MPU_RA_I2C_SLV4_REG 0x32
#define MPU_RA_I2C_SLV4_DO 0x33
#define MPU_RA_I2C_SLV4_CTRL 0x34
#define MPU_RA_I2C_SLV4_DI 0x35
#define MPU_RA_I2C_MST_STATUS 0x36
#define MPU_RA_INT_PIN_CFG 0x37
#define MPU_RA_INT_ENABLE 0x38
#define MPU_RA_DMP_INT_STATUS 0x39
#define MPU_RA_INT_STATUS 0x3A
#define MPU_RA_ACCEL_XOUT_H 0x3B
#define MPU_RA_ACCEL_XOUT_L 0x3C
#define MPU_RA_ACCEL_YOUT_H 0x3D
#define MPU_RA_ACCEL_YOUT_L 0x3E
#define MPU_RA_ACCEL_ZOUT_H 0x3F
#define MPU_RA_ACCEL_ZOUT_L 0x40
#define MPU_RA_TEMP_OUT_H 0x41
#define MPU_RA_TEMP_OUT_L 0x42
#define MPU_RA_GYRO_XOUT_H 0x43
#define MPU_RA_GYRO_XOUT_L 0x44
#define MPU_RA_GYRO_YOUT_H 0x45
#define MPU_RA_GYRO_YOUT_L 0x46
#define MPU_RA_GYRO_ZOUT_H 0x47
#define MPU_RA_GYRO_ZOUT_L 0x48
#define MPU_RA_EXT_SENS_DATA_00 0x49
#define MPU_RA_MOT_DETECT_STATUS 0x61
#define MPU_RA_I2C_SLV0_DO 0x63
#define MPU_RA_I2C_SLV1_DO 0x64
#define MPU_RA_I2C_SLV2_DO 0x65
#define MPU_RA_I2C_SLV3_DO 0x66
#define MPU_RA_I2C_MST_DELAY_CTRL 0x67
#define MPU_RA_SIGNAL_PATH_RESET 0x68
#define MPU_RA_MOT_DETECT_CTRL 0x69
#define MPU_RA_USER_CTRL 0x6A
#define MPU_RA_PWR_MGMT_1 0x6B
#define MPU_RA_PWR_MGMT_2 0x6C
#define MPU_RA_BANK_SEL 0x6D
#define MPU_RA_MEM_START_ADDR 0x6E
#define MPU_RA_MEM_R_W 0x6F
#define MPU_RA_DMP_CFG_1 0x70
#define MPU_RA_DMP_CFG_2 0x71
#define MPU_RA_FIFO_COUNTH 0x72
#define MPU_RA_FIFO_COUNTL 0x73
#define MPU_RA_FIFO_R_W 0x74
#define MPU_RA_WHO_AM_I 0x75

#define MPU6050_SMPLRT_DIV 0 // 8000Hz

#define MPU6050_LPF_256HZ 0
#define MPU6050_LPF_188HZ 1
#define MPU6050_LPF_98HZ 2
#define MPU6050_LPF_42HZ 3
#define MPU6050_LPF_20HZ 4
#define MPU6050_LPF_10HZ 5
#define MPU6050_LPF_5HZ 6

#define MPU6050A_2mg ((float)0.00006103f) // 0.00006250 g/LSB
#define MPU6050A_4mg ((float)0.00012207f) // 0.00012500 g/LSB
#define MPU6050A_8mg ((float)0.00024414f) // 0.00025000 g/LSB

#define MPU6050G_s250dps ((float)0.0076335f)  // 0.0087500 dps/LSB
#define MPU6050G_s500dps ((float)0.0152671f)  // 0.0175000 dps/LSB
#define MPU6050G_s2000dps ((float)0.0609756f) // 0.0700000 dps/LSB

#define M_PI 3.141592653f
#define DEG_TO_RAD 0.01745329f
#define RAD_TO_DEG 57.29577951f

uint8_t MPU6050_Buffer[14];

//角度转弧度
float radians(float deg)
{
	return deg * DEG_TO_RAD;
}

void delayms(uint16_t ms)
{
	uint16_t i = 200;
	uint8_t j;
	for (j = 0; j < ms; j++)
		while (i--)
			;
}

//MPU6050初始化
void MPU6050_Init(void)
{
	I2C_Single_Write(MPU6050_ADDRESS, PWR_MGMT_1, 0x00);   //Wakeup
	I2C_Single_Write(MPU6050_ADDRESS, SMPLRT_DIV, 0x07);   //Sampling Rate 125HZ
	I2C_Single_Write(MPU6050_ADDRESS, CONFIG, 0x06);	   //config
	I2C_Single_Write(MPU6050_ADDRESS, ACCEL_CONFIG, 0x00); //No self checking, sccelerometer range set 0  ±2g	 16384 LSB/g
														   //	I2C_Single_Write(MPU6050_ADDRESS, GYRO_CONFIG, 0x00); //不自测，陀螺仪测量范围设置	 0  ±250°/s  131LSB/(°/s)
	I2C_Single_Write(MPU6050_ADDRESS, GYRO_CONFIG, 0x08);  //不自测，陀螺仪测量范围设置	 1  ±500°/s  65.5LSB/(°/s)
}

int16_t MPU6050_Get_Data(uint8_t addr)
{
	uint8_t H, L;
	H = I2C_Single_Read(MPU6050_ADDRESS, addr);
	L = I2C_Single_Read(MPU6050_ADDRESS, addr + 1);
	return (H << 8) + L;
}

uint8_t MPU6050_Check(void)
{
	return (I2C_Single_Read(MPU6050_ADDRESS, WHO_AM_I) == 0x68);
}

uint8_t MPU6050_Sequence_Read(void)
{
	return I2C_Mult_Read(MPU6050_ADDRESS, ACCEL_XOUT_H, MPU6050_Buffer, 14);
}

void MPU6050_Single_Read(void)
{
	MPU6050_Buffer[0] = I2C_Single_Read(MPU6050_ADDRESS, ACCEL_XOUT_H);
	MPU6050_Buffer[1] = I2C_Single_Read(MPU6050_ADDRESS, ACCEL_XOUT_L);
	MPU6050_Buffer[2] = I2C_Single_Read(MPU6050_ADDRESS, ACCEL_YOUT_H);
	MPU6050_Buffer[3] = I2C_Single_Read(MPU6050_ADDRESS, ACCEL_YOUT_L);
	MPU6050_Buffer[4] = I2C_Single_Read(MPU6050_ADDRESS, ACCEL_ZOUT_H);
	MPU6050_Buffer[5] = I2C_Single_Read(MPU6050_ADDRESS, ACCEL_ZOUT_L);

	MPU6050_Buffer[8] = I2C_Single_Read(MPU6050_ADDRESS, GYRO_XOUT_H);
	MPU6050_Buffer[9] = I2C_Single_Read(MPU6050_ADDRESS, GYRO_XOUT_L);
	MPU6050_Buffer[10] = I2C_Single_Read(MPU6050_ADDRESS, GYRO_YOUT_H);
	MPU6050_Buffer[11] = I2C_Single_Read(MPU6050_ADDRESS, GYRO_YOUT_L);
	MPU6050_Buffer[12] = I2C_Single_Read(MPU6050_ADDRESS, GYRO_ZOUT_H);
	MPU6050_Buffer[13] = I2C_Single_Read(MPU6050_ADDRESS, GYRO_ZOUT_L);
}

#define TrueVal_0 0.0f
#define TrueVal_45 40.6f
#define TrueVal_70 67.5f
#define TrueVal_90 90.0f
#define TrueVal_110 111.8f
#define TrueVal_135 139.5f
#define TrueVal_180 180.0f

float Remap(float input)
{
	if (input < TrueVal_45)
		return map(input, TrueVal_0, TrueVal_45, 0, 45);
	else if (input < TrueVal_70)
		return map(input, TrueVal_45, TrueVal_70, 45, 70);
	else if (input < TrueVal_90)
		return map(input, TrueVal_70, TrueVal_90, 70, 90);
	else if (input < TrueVal_110)
		return map(input, TrueVal_90, TrueVal_110, 90, 110);
	else if (input < TrueVal_135)
		return map(input, TrueVal_110, TrueVal_135, 110, 135);
	else if (input < TrueVal_180)
		return map(input, TrueVal_135, TrueVal_180, 135, 180);
	else
		return 0;
}

double MPU6050_GetAngle(void)
{
	float angle = -atan2((MPU6050_Get_Data(ACCEL_XOUT_H)), (MPU6050_Get_Data(ACCEL_ZOUT_H))) * RAD_TO_DEG + 90.0 + angleCorretionValue;
	return Remap(angle);
}
