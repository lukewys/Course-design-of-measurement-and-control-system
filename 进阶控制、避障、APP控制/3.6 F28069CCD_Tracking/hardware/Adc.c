#include "DSP28x_Project.h"

#include "ADC.h"


//###########################################################################
// $ ��������:  void InitCCDGPIO(void)       $
// $ ��������:  ��ʼ��GPIO8��GPIO9              $
//###########################################################################
void InitCCDGPIO(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 =0;   // ����ΪGPIO����
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 =0;   // ����ΪGPIO����
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;   // ����GPIO8λ���
	GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;   // ����GPIO9λ���
	EDIS;
}


//###########################################################################
// $ ��������:  Init_Adc_CCD(void)   $
// $ ��������:  ADCģ���ʼ��                                            $
//###########################################################################
void Init_Adc_CCD(void)
{
	Uint8 c=0;
	for(c=0;c<128;c++)
	{
		ADV[c]=0;
	}
	InitAdc();
    //AdcOffsetSelfCal();
    EALLOW;
  	AdcRegs.ADCCTL2.bit.ADCNONOVERLAP = 0;	//Enable non-overlap mode
    AdcRegs.ADCSOC0CTL.bit.CHSEL  = 1; 		//Set SOC0 channel select to ADCINA2
  	AdcRegs.ADCSOC0CTL.bit.ACQPS  = 6; 		//Set SOC0 acquisition period to 7 ADCCLK
    EDIS;
}


//###########################################################################
// $ ��������:  Dly_us(void)          $
// $ ��������:  ��ʱ                                            $
//###########################################################################
void Dly_us(void)
{
	int ii;
	for(ii=0;ii<10;ii++);//����һ��FOR���ִ������Ϊ8������8*10*12.5ns=1us
}


//###########################################################################
// $ ��������:  CCD_Start(void)         $
// $ ��������:  CCD��ʼ�ź�                                                        $
//###########################################################################
void CCD_Start(void)
{
	Uint8 i=0;
	CCD_CLK_SET=1;
		Dly_us();
		CCD_SI_CLR=1;
		Dly_us();

		CCD_CLK_CLR=1;
		Dly_us();
		CCD_SI_SET=1;
		Dly_us();//��ʱ2us

		CCD_CLK_SET=1;
		Dly_us();
		CCD_SI_CLR=1;
		Dly_us();

		for(i=0;i<127;i++)
		{
			CCD_CLK_CLR=1;
			Dly_us();
			Dly_us();
			CCD_CLK_SET=1;
			Dly_us();
			Dly_us();
		}
}


//###########################################################################
// $ ��������:  Read_CCD(void)         $
// $ ��������:  ��ȡCCD����                                                    $
//###########################################################################
void Read_CCD(void)
{
	Uint8 i=0,samp=0;
	CCD_CLK_SET=1;
	Dly_us();
	CCD_SI_CLR=1;
	Dly_us();
	CCD_CLK_CLR=1;
	Dly_us();
	CCD_SI_SET=1;
	Dly_us();//��ʱ2us
	CCD_CLK_SET=1;
	Dly_us();
	CCD_SI_CLR=1;
	Dly_us();
	DELAY_US(20);
	AdcRegs.ADCSOCFRC1.bit.SOC0 = 1;
	ADV[samp]=AdcResult.ADCRESULT0;

	CCD_CLK_CLR=1;
	for(i=0;i<127;i++)
	{

		Dly_us();
		Dly_us();
		CCD_CLK_SET=1;
		Dly_us();
		Dly_us();
		samp++;
		AdcRegs.ADCSOCFRC1.bit.SOC0=1;//Force start of conversion on SOC0
		//while(AdcRegs.ADCSOCFLG1.bit.SOC0);
		ADV[samp]=AdcResult.ADCRESULT0;

		CCD_CLK_CLR=1;
	}
	ADV[0]=ADV[1];
}


//###########################################################################
// $ ��������:  Find_CCD_Zhongzhi(void)         $
// $ ��������:  ������������ֵ                                                    $
//###########################################################################
void  Find_CCD_Zhongzhi(void)
{
	 static Uint16 i,j,Left,Right;
	 static Uint16 value1_max,value1_min;

	   value1_max=ADV[0];  //��̬��ֵ�㷨����ȡ������Сֵ
     for(i=5;i<123;i++)   //���߸�ȥ��5����
     {
        if(value1_max<=ADV[i])
        value1_max=ADV[i];
     }
	   value1_min=ADV[0];  //��Сֵ
     for(i=5;i<123;i++)
     {
        if(value1_min>=ADV[i])
        value1_min=ADV[i];
     }
   CCD_Yuzhi=(value1_max+value1_min)/2;	  //���������������ȡ����ֵ����ʼ�۲�����ֵ�һ�����100�����Ǻ��ڷ����ڹ�������ʱ��ͻᵼ�¶��߲��������ֵ�Ͳ�׼��

	 for(i = 5;i<118; i++)   //Ѱ�����������
	 {
		 //cnt1++;
		if(ADV[i]>CCD_Yuzhi&&ADV[i+1]>CCD_Yuzhi&&ADV[i+2]>CCD_Yuzhi&&ADV[i+3]<CCD_Yuzhi&&ADV[i+4]<CCD_Yuzhi&&ADV[i+5]<CCD_Yuzhi)
		{
		  Left=i;
		  break;
		}

	 }
	 for(j = 118;j>5; j--)//Ѱ���ұ�������
	 {
		// cnt2++;
		if(ADV[j]<CCD_Yuzhi&&ADV[j+1]<CCD_Yuzhi&&ADV[j+2]<CCD_Yuzhi&&ADV[j+3]>CCD_Yuzhi&&ADV[j+4]>CCD_Yuzhi&&ADV[j+5]>CCD_Yuzhi)
		{
		  Right=j;
		  break;
		}

	 }
	 ZZ=(Right+Left)/2;
}

