#include "MPU6050.h"
#include "DSP28x_Project.h"
#include <math.h>

Uint16  mpu6050_buffer[14];   // MPU6050数据
Uint16  i;
T_INT16_XYZ	GYRO_OFFSET,ACC_OFFSET;
Uint8	GYRO_OFFSET_OK = 1;
Uint8	ACC_OFFSET_OK = 1;
T_INT16_XYZ	MPU6050_ACC_LAST,MPU6050_GYRO_LAST;
int Temperature;   // 温度变量

//----------------------------Kalman Filter-----------------------------------------------
float K1 =0.02;
float angle=0, angle_dot=0;
float Q_angle=0.001;// 过程噪声的协方差
float Q_gyro=0.003;//0.003 过程噪声的协方差 过程噪声的协方差为一个一行两列矩阵
float R_angle=0.5;//测量噪声的协方差 及测量偏差
float dt=0.005;//
char  C_0 = 1;
float Q_bias=0, Angle_err=0;
float PCt_0=0, PCt_1=0, E=0;
float K_0=0, K_1=0, t_0=0, t_1=0;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };

//###########################################################################
// $ 函数名称:  Uint16 MPU6050_Read(void)   $
// $ 函数功能:  读取MPU6050数据包括三轴的加速度，角速度以及温度数据                                                              $
//###########################################################################
Uint16 MPU6050_Read(void)
{
	return ReadData(devAddr,MPU6050_RA_ACCEL_XOUT_H,14,mpu6050_buffer);
}


//###########################################################################
// $ 函数名称:  void MPU6050_setSleepEnabled(Uint8 enabled)   $
// $ 函数功能:  初始化MPU6050的0x6B寄存器，解除睡眠模式                                                              $
//###########################################################################
void MPU6050_setSleepEnabled(Uint8 enabled)
{
	IICwriteBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}


//###########################################################################
// $ 函数名称:  void MPU6050_setClockSource(Uint8 source)   $
// $ 函数功能:  初始化MPU6050的0x6B寄存器，设置MPU6050的时钟源                                        $
//###########################################################################
void MPU6050_setClockSource(Uint8 source)
{
	IICwriteBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}


//###########################################################################
// $ 函数名称:  void MPU6050_setFullScaleGyroRange(Uint8 range)   $
// $ 函数功能:  初始化MPU6050的0x1B寄存器，设置MPU6050的陀螺仪的满量程范围                                 $
//###########################################################################
void MPU6050_setFullScaleGyroRange(Uint8 range)
{
	IICwriteBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}


//###########################################################################
// $ 函数名称:  void MPU6050_setFullScaleAccelRange(Uint8 range)   $
// $ 函数功能:  初始化MPU6050的0x1C寄存器，设置MPU6050的加速度计的满量程范围                                 $
//###########################################################################
void MPU6050_setFullScaleAccelRange(Uint8 range)
{
	IICwriteBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}


//###########################################################################
// $ 函数名称:  void MPU6050_setDLPF(Uint8 mode)  $
// $ 函数功能:  初始化MPU6050的0x1A寄存器，设置MPU6050的  陀螺仪和加速度计的数字低通滤波器                                 $
//###########################################################################
void MPU6050_setDLPF(Uint8 mode)
{
	IICwriteBits(devAddr, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, mode);
}


//###########################################################################
// $ 函数名称:  void MPU6050_setI2CMasterModeEnabled(Uint8 enabled)          $
// $ 函数功能:  初始化MPU6050的0x6A寄存器，设置MPU6050IIC总线模式                                 $
//###########################################################################
void MPU6050_setI2CMasterModeEnabled(Uint8 enabled)
{
	IICwriteBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}


//###########################################################################
// $ 函数名称:  void MPU6050_setI2CBypassEnabled(Uint8 enabled)           $
// $ 函数功能:  初始化MPU6050的0x37寄存器，设置MPU6050IIC旁路有效                              $
//###########################################################################
void MPU6050_setI2CBypassEnabled(Uint8 enabled)
{
	IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}



//###########################################################################
// $ 函数名称:  void MPU6050_Init(void)         $
// $ 函数功能:  初始化MPU6050                     $
//###########################################################################
void MPU6050_Init(void)
{

	MPU6050_setSleepEnabled(0);   // 接触睡眠
		DELAY_US(50);
	MPU6050_setClockSource(MPU6050_CLOCK_PLL_XGYRO);   // 设置时钟源
		DELAY_US(50);
	MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);   // 设置角速度量程范围
		DELAY_US(50);
	MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_4);   // 设置加速度量程范围
		DELAY_US(50);
	MPU6050_setDLPF(MPU6050_DLPF_BW_42);   // 设置设置MPU6050的  陀螺仪和加速度计的数字低通滤波器
	DELAY_US(50);
	MPU6050_setI2CMasterModeEnabled(0);   // 设置MPU6050IIC总线模式
		DELAY_US(50);
	MPU6050_setI2CBypassEnabled(1);   // 设置MPU6050 IIC旁路有效
		DELAY_US(50);
}


//###########################################################################
// $ 函数名称:  void Kalman_Filter(float Accel,float Gyro)        $Accel加速度计测得的绕X轴的角度，Gyro绕X轴的角速度
// $ 函数功能:  卡尔曼滤波处理                     $
//###########################################################################
void Kalman_Filter(float Accel,float Gyro)
{
	angle+=(Gyro - Q_bias) * dt;  // 先验估计dt=0.005s
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0];  // Pk-先验估计误差协方差的微分

	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-先验估计误差协方差微分的积分
	PP[0][1] += Pdot[1] * dt;   // =先验估计误差协方差
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;

	Angle_err = Accel - angle;	//zk-先验估计

	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];

	E = R_angle + PCt_0;//

	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;

	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //后验估计误差协方差
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;

	angle	+= K_0 * Angle_err;	  //后验估计
	Q_bias	+= K_1 * Angle_err;	  //后验估计
	angle_dot   = Gyro - Q_bias;	//输出值(后验估计)的微分=角速度
}


//###########################################################################
// $ 函数名称:  void Yijielvbo(float angle_m, float gyro_m)       $
// $ 函数功能:  一阶滤波处理                  $
//###########################################################################
void Yijielvbo(float angle_m, float gyro_m)
{
   angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * 0.005);
}


//###########################################################################
// $ 函数名称:  void Get_Angle(void)       $
// $ 函数功能:  MPU6050数据处理计算                                           $
//###########################################################################
void Get_Angle(void)
{
	float Accel_Y,Accel_Angle,Accel_Z,Gyro_X,Gyro_Z,Temp;

	MPU6050_ACC_LAST.X=((int16)(mpu6050_buffer[0] << 8) | (mpu6050_buffer[1]&0xff));
	MPU6050_ACC_LAST.Y=((int16)(mpu6050_buffer[2] << 8) | (mpu6050_buffer[3]&0xff));
	MPU6050_ACC_LAST.Z=((int16)(mpu6050_buffer[4] << 8) | (mpu6050_buffer[5]&0xff));

	MPU6050_GYRO_LAST.X=((int16)(mpu6050_buffer[8] << 8) | (mpu6050_buffer[9]&0xff));
	MPU6050_GYRO_LAST.Y=((int16)(mpu6050_buffer[10]<< 8) | (mpu6050_buffer[11]&0xff));
	MPU6050_GYRO_LAST.Z=((int16)(mpu6050_buffer[12] << 8)| (mpu6050_buffer[13]&0xff));

	Temp=((int16)(mpu6050_buffer[6] << 8) | (mpu6050_buffer[7]&0xff));
	Temp=(36.53+Temp/340)*10;
	Temperature=(int)Temp;

	Gyro_X=MPU6050_GYRO_LAST.X;
	Gyro_Z=MPU6050_GYRO_LAST.Z;
	Accel_Y=MPU6050_ACC_LAST.Y;
	Accel_Z=MPU6050_ACC_LAST.Z;

	Gyro_Balance=Gyro_X;//计算平衡角速度
	Accel_Angle=atan2(Accel_Y,Accel_Z)*180/PI;//计算倾角
	Gyro_X=Gyro_X/16.4;	//陀螺仪量程转换32768/2000=16.4

	Kalman_Filter(Accel_Angle,Gyro_X);//卡尔曼滤波angle
	//Yijielvbo(Accel_Angle,Gyro_X);//互补滤波

    Angle_Balance=angle;         //更新平衡倾角
	Gyro_Turn=Gyro_Z;          //更新转向角速度
	//Acceleration_Z=Accel_Z;      //更新Z轴加速度计

}
