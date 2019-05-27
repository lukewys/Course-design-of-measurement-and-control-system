#include "MPU6050.h"
#include "DSP28x_Project.h"
#include <math.h>

Uint16  mpu6050_buffer[14];   // MPU6050����
Uint16  i;
T_INT16_XYZ	GYRO_OFFSET,ACC_OFFSET;
Uint8	GYRO_OFFSET_OK = 1;
Uint8	ACC_OFFSET_OK = 1;
T_INT16_XYZ	MPU6050_ACC_LAST,MPU6050_GYRO_LAST;
int Temperature;   // �¶ȱ���

//----------------------------Kalman Filter-----------------------------------------------
float K1 =0.02;
float angle=0, angle_dot=0;
float Q_angle=0.001;// ����������Э����
float Q_gyro=0.003;//0.003 ����������Э���� ����������Э����Ϊһ��һ�����о���
float R_angle=0.5;//����������Э���� ������ƫ��
float dt=0.005;//
char  C_0 = 1;
float Q_bias=0, Angle_err=0;
float PCt_0=0, PCt_1=0, E=0;
float K_0=0, K_1=0, t_0=0, t_1=0;
float Pdot[4] ={0,0,0,0};
float PP[2][2] = { { 1, 0 },{ 0, 1 } };

//###########################################################################
// $ ��������:  Uint16 MPU6050_Read(void)   $
// $ ��������:  ��ȡMPU6050���ݰ�������ļ��ٶȣ����ٶ��Լ��¶�����                                                              $
//###########################################################################
Uint16 MPU6050_Read(void)
{
	return ReadData(devAddr,MPU6050_RA_ACCEL_XOUT_H,14,mpu6050_buffer);
}


//###########################################################################
// $ ��������:  void MPU6050_setSleepEnabled(Uint8 enabled)   $
// $ ��������:  ��ʼ��MPU6050��0x6B�Ĵ��������˯��ģʽ                                                              $
//###########################################################################
void MPU6050_setSleepEnabled(Uint8 enabled)
{
	IICwriteBit(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_SLEEP_BIT, enabled);
}


//###########################################################################
// $ ��������:  void MPU6050_setClockSource(Uint8 source)   $
// $ ��������:  ��ʼ��MPU6050��0x6B�Ĵ���������MPU6050��ʱ��Դ                                        $
//###########################################################################
void MPU6050_setClockSource(Uint8 source)
{
	IICwriteBits(devAddr, MPU6050_RA_PWR_MGMT_1, MPU6050_PWR1_CLKSEL_BIT, MPU6050_PWR1_CLKSEL_LENGTH, source);
}


//###########################################################################
// $ ��������:  void MPU6050_setFullScaleGyroRange(Uint8 range)   $
// $ ��������:  ��ʼ��MPU6050��0x1B�Ĵ���������MPU6050�������ǵ������̷�Χ                                 $
//###########################################################################
void MPU6050_setFullScaleGyroRange(Uint8 range)
{
	IICwriteBits(devAddr, MPU6050_RA_GYRO_CONFIG, MPU6050_GCONFIG_FS_SEL_BIT, MPU6050_GCONFIG_FS_SEL_LENGTH, range);
}


//###########################################################################
// $ ��������:  void MPU6050_setFullScaleAccelRange(Uint8 range)   $
// $ ��������:  ��ʼ��MPU6050��0x1C�Ĵ���������MPU6050�ļ��ٶȼƵ������̷�Χ                                 $
//###########################################################################
void MPU6050_setFullScaleAccelRange(Uint8 range)
{
	IICwriteBits(devAddr, MPU6050_RA_ACCEL_CONFIG, MPU6050_ACONFIG_AFS_SEL_BIT, MPU6050_ACONFIG_AFS_SEL_LENGTH, range);
}


//###########################################################################
// $ ��������:  void MPU6050_setDLPF(Uint8 mode)  $
// $ ��������:  ��ʼ��MPU6050��0x1A�Ĵ���������MPU6050��  �����Ǻͼ��ٶȼƵ����ֵ�ͨ�˲���                                 $
//###########################################################################
void MPU6050_setDLPF(Uint8 mode)
{
	IICwriteBits(devAddr, MPU6050_RA_CONFIG, MPU6050_CFG_DLPF_CFG_BIT, MPU6050_CFG_DLPF_CFG_LENGTH, mode);
}


//###########################################################################
// $ ��������:  void MPU6050_setI2CMasterModeEnabled(Uint8 enabled)          $
// $ ��������:  ��ʼ��MPU6050��0x6A�Ĵ���������MPU6050IIC����ģʽ                                 $
//###########################################################################
void MPU6050_setI2CMasterModeEnabled(Uint8 enabled)
{
	IICwriteBit(devAddr, MPU6050_RA_USER_CTRL, MPU6050_USERCTRL_I2C_MST_EN_BIT, enabled);
}


//###########################################################################
// $ ��������:  void MPU6050_setI2CBypassEnabled(Uint8 enabled)           $
// $ ��������:  ��ʼ��MPU6050��0x37�Ĵ���������MPU6050IIC��·��Ч                              $
//###########################################################################
void MPU6050_setI2CBypassEnabled(Uint8 enabled)
{
	IICwriteBit(devAddr, MPU6050_RA_INT_PIN_CFG, MPU6050_INTCFG_I2C_BYPASS_EN_BIT, enabled);
}



//###########################################################################
// $ ��������:  void MPU6050_Init(void)         $
// $ ��������:  ��ʼ��MPU6050                     $
//###########################################################################
void MPU6050_Init(void)
{

	MPU6050_setSleepEnabled(0);   // �Ӵ�˯��
		DELAY_US(50);
	MPU6050_setClockSource(MPU6050_CLOCK_PLL_XGYRO);   // ����ʱ��Դ
		DELAY_US(50);
	MPU6050_setFullScaleGyroRange(MPU6050_GYRO_FS_2000);   // ���ý��ٶ����̷�Χ
		DELAY_US(50);
	MPU6050_setFullScaleAccelRange(MPU6050_ACCEL_FS_4);   // ���ü��ٶ����̷�Χ
		DELAY_US(50);
	MPU6050_setDLPF(MPU6050_DLPF_BW_42);   // ��������MPU6050��  �����Ǻͼ��ٶȼƵ����ֵ�ͨ�˲���
	DELAY_US(50);
	MPU6050_setI2CMasterModeEnabled(0);   // ����MPU6050IIC����ģʽ
		DELAY_US(50);
	MPU6050_setI2CBypassEnabled(1);   // ����MPU6050 IIC��·��Ч
		DELAY_US(50);
}


//###########################################################################
// $ ��������:  void Kalman_Filter(float Accel,float Gyro)        $Accel���ٶȼƲ�õ���X��ĽǶȣ�Gyro��X��Ľ��ٶ�
// $ ��������:  �������˲�����                     $
//###########################################################################
void Kalman_Filter(float Accel,float Gyro)
{
	angle+=(Gyro - Q_bias) * dt;  // �������dt=0.005s
	Pdot[0]=Q_angle - PP[0][1] - PP[1][0];  // Pk-����������Э�����΢��

	Pdot[1]=-PP[1][1];
	Pdot[2]=-PP[1][1];
	Pdot[3]=Q_gyro;
	PP[0][0] += Pdot[0] * dt;   // Pk-����������Э����΢�ֵĻ���
	PP[0][1] += Pdot[1] * dt;   // =����������Э����
	PP[1][0] += Pdot[2] * dt;
	PP[1][1] += Pdot[3] * dt;

	Angle_err = Accel - angle;	//zk-�������

	PCt_0 = C_0 * PP[0][0];
	PCt_1 = C_0 * PP[1][0];

	E = R_angle + PCt_0;//

	K_0 = PCt_0 / E;
	K_1 = PCt_1 / E;

	t_0 = PCt_0;
	t_1 = C_0 * PP[0][1];

	PP[0][0] -= K_0 * t_0;		 //����������Э����
	PP[0][1] -= K_0 * t_1;
	PP[1][0] -= K_1 * t_0;
	PP[1][1] -= K_1 * t_1;

	angle	+= K_0 * Angle_err;	  //�������
	Q_bias	+= K_1 * Angle_err;	  //�������
	angle_dot   = Gyro - Q_bias;	//���ֵ(�������)��΢��=���ٶ�
}


//###########################################################################
// $ ��������:  void Yijielvbo(float angle_m, float gyro_m)       $
// $ ��������:  һ���˲�����                  $
//###########################################################################
void Yijielvbo(float angle_m, float gyro_m)
{
   angle = K1 * angle_m+ (1-K1) * (angle + gyro_m * 0.005);
}


//###########################################################################
// $ ��������:  void Get_Angle(void)       $
// $ ��������:  MPU6050���ݴ������                                           $
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

	Gyro_Balance=Gyro_X;//����ƽ����ٶ�
	Accel_Angle=atan2(Accel_Y,Accel_Z)*180/PI;//�������
	Gyro_X=Gyro_X/16.4;	//����������ת��32768/2000=16.4

	Kalman_Filter(Accel_Angle,Gyro_X);//�������˲�angle
	//Yijielvbo(Accel_Angle,Gyro_X);//�����˲�

    Angle_Balance=angle;         //����ƽ�����
	Gyro_Turn=Gyro_Z;          //����ת����ٶ�
	//Acceleration_Z=Accel_Z;      //����Z����ٶȼ�

}
