
//###########################################################################
//
// FILE:   DSP28x_Project.h
//
// TITLE:  DSP28x Project Headerfile and Examples Include File
//
//###########################################################################
// $TI Release: F2806x C/C++ Header Files and Peripheral Examples V151 $
// $Release Date: February  2, 2016 $
// $Copyright: Copyright (C) 2011-2016 Texas Instruments Incorporated -
//             http://www.ti.com/ ALL RIGHTS RESERVED $
//###########################################################################

#ifndef DSP28x_PROJECT_H
#define DSP28x_PROJECT_H

#include "F2806x_Cla_typedefs.h"// F2806x CLA Type definitions
#include "F2806x_Device.h"      // F2806x Headerfile Include File
#include "F2806x_Examples.h"   	// F2806x Examples Include File
#include <math.h>

#include "EPwm.h"    // EPwm���ͷ�ļ�
#include "IIC.h"    // IIC���ͷ�ļ�
#include "MPU6050.h"   // MPU6050���ͷ�ļ�
#include "QEP.h"   // QEP���ͷ�ļ�
#include "Key.h"   //�������ͷ�ļ�
#include "Adc.h"   //�������ͷ�ļ�


//$  ��������
int balance(float Angle,float Gyro);
int velocity(int encoder_left,int encoder_right);
int turn_CCD(Uint16 CCD,float gyro);
void Set_Pwm(int moto1,int moto2);
void Xianfu_Pwm(void);
int myabs(int a);
void InitLEDGPIO(void);
void LEDOverTurn(void);
Uint8 Turn_Off(float angle);
void Key(void);

#define Encoder1	EQep1Regs.QPOSCNT
#define Encoder2	EQep2Regs.QPOSCNT

#define PWM1A  	EPwm1Regs.CMPA.half.CMPA
#define PWM1B  	EPwm1Regs.CMPB
#define PWM2A  	EPwm2Regs.CMPA.half.CMPA
#define PWM2B  	EPwm2Regs.CMPB
#define CCD_SI_SET    GpioDataRegs.GPASET.bit.GPIO7   // GPIO7����ΪCCD_SI_SET
#define CCD_CLK_SET   GpioDataRegs.GPASET.bit.GPIO6   // GPIO6����ΪCCD_CLK_SET
#define CCD_SI_CLR    GpioDataRegs.GPACLEAR.bit.GPIO7   // GPIO7����ΪCCD_SI_CLR
#define CCD_CLK_CLR   GpioDataRegs.GPACLEAR.bit.GPIO6   // GPIO6����ΪCCD_CLK_CLR

#define PI 3.14159265
#define ZHONGZHI (10.5)//��Ϊ��װCCD֧�ܶ���������ı仯������ʵ�ʸı�ò���15


extern float Angle_Balance,Gyro_Balance,Gyro_Turn;
extern Uint16 ADV[128];
extern Uint16 CCD_Z,CCD_Yuzhi;
extern Uint16 ZZ;

#endif  // end of DSP28x_PROJECT_H definition

