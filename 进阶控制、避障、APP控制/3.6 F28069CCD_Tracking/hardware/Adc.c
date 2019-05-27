#include "DSP28x_Project.h"

#include "ADC.h"


//###########################################################################
// $ 函数名称:  void InitCCDGPIO(void)       $
// $ 函数功能:  初始化GPIO8和GPIO9              $
//###########################################################################
void InitCCDGPIO(void)
{
	EALLOW;
	GpioCtrlRegs.GPAMUX1.bit.GPIO6 =0;   // 设置为GPIO功能
	GpioCtrlRegs.GPAMUX1.bit.GPIO7 =0;   // 设置为GPIO功能
	GpioCtrlRegs.GPADIR.bit.GPIO6 = 1;   // 设置GPIO8位输出
	GpioCtrlRegs.GPADIR.bit.GPIO7 = 1;   // 设置GPIO9位输出
	EDIS;
}


//###########################################################################
// $ 函数名称:  Init_Adc_CCD(void)   $
// $ 函数功能:  ADC模块初始化                                            $
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
// $ 函数名称:  Dly_us(void)          $
// $ 函数功能:  延时                                            $
//###########################################################################
void Dly_us(void)
{
	int ii;
	for(ii=0;ii<10;ii++);//假设一条FOR语句执行周期为8个，则8*10*12.5ns=1us
}


//###########################################################################
// $ 函数名称:  CCD_Start(void)         $
// $ 函数功能:  CCD起始信号                                                        $
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
		Dly_us();//延时2us

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
// $ 函数名称:  Read_CCD(void)         $
// $ 函数功能:  读取CCD数据                                                    $
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
	Dly_us();//延时2us
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
// $ 函数名称:  Find_CCD_Zhongzhi(void)         $
// $ 函数功能:  计算轨道黑线中值                                                    $
//###########################################################################
void  Find_CCD_Zhongzhi(void)
{
	 static Uint16 i,j,Left,Right;
	 static Uint16 value1_max,value1_min;

	   value1_max=ADV[0];  //动态阈值算法，读取最大和最小值
     for(i=5;i<123;i++)   //两边各去掉5个点
     {
        if(value1_max<=ADV[i])
        value1_max=ADV[i];
     }
	   value1_min=ADV[0];  //最小值
     for(i=5;i<123;i++)
     {
        if(value1_min>=ADV[i])
        value1_min=ADV[i];
     }
   CCD_Yuzhi=(value1_max+value1_min)/2;	  //计算出本次中线提取的阈值，开始观察数组值我还加了100，但是后期发现在光线弱的时候就会导致读线不清楚，阈值就不准了

	 for(i = 5;i<118; i++)   //寻找左边跳变沿
	 {
		 //cnt1++;
		if(ADV[i]>CCD_Yuzhi&&ADV[i+1]>CCD_Yuzhi&&ADV[i+2]>CCD_Yuzhi&&ADV[i+3]<CCD_Yuzhi&&ADV[i+4]<CCD_Yuzhi&&ADV[i+5]<CCD_Yuzhi)
		{
		  Left=i;
		  break;
		}

	 }
	 for(j = 118;j>5; j--)//寻找右边跳变沿
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

