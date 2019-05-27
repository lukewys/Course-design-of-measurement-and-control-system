//!
//!$˵������ʵ������TMS320F28069��SCIBʵ�ִ���ͨ��     $
//������96008 λ����λ��1 λֹͣλ������żУ��Ĺ�����ʽ
//###########################################################################
//$ file        main.c         $
//$ author      HeDong         $
//$ version     V1.0           $
//$ project     F28069SCI      $
//###########################################################################

#include "DSP28x_Project.h"     // ͷ�ļ�

void scib_echoback_init();   // scib_echoback_init��������
void scib_xmit( int a);    // scib_xmit ��������
void scib_msg(  char * msg);   // scib_msg��������

//Flash��д��ر���
extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 RamfuncsLoadSize;

//###########################################################################
// $ ��������:  int main(void)     $
//###########################################################################
int main(void)
{
   Uint16 ReceiveChar;   // ��������
   char *msg;   // ��������

   InitSysCtrl();   //��ʼ��ϵͳʱ�ӣ�90MHz

   DINT;   // ��������ж�

   InitPieCtrl();   // ��ʼ��PIE���ƼĴ���

   IER = 0x0000;   // ��ֹCPU�ж�
   IFR = 0x0000;   // ���CPU�жϱ�־

   InitPieVectTable();   // ��ʼ��PIE�жϱ�

   //Flash��д������
   memcpy(&RamfuncsRunStart,&RamfuncsLoadStart,(Uint32)&RamfuncsLoadSize);
   InitFlash();

   InitScibGpio();   // SCIB���ų�ʼ��
   scib_echoback_init();   // ��ʼ��scibģ��

   EINT;   // ʹ��ȫ���ж�
   ERTM;   // ʹ��ȫ���ж�DBGM

   msg="\r\n��ӭʹ�ú϶�����ƽ�⳵!  \0";
   scib_msg(msg);
   msg= "\r\n������һ���ַ�,DSP���Ὣ���ַ��ش��������ն�!\n\0";
   scib_msg(msg);
   for(;;)
   {
      if(ScibRegs.SCIRXST.bit.RXRDY==1)
      {
    	  ReceiveChar=ScibRegs.SCIRXBUF.all;    // �����յ����ַ����͸�����
    	  msg=" ��������ַ���: \0";
    	  scib_msg(msg);   // �����ַ���
    	  scib_xmit(ReceiveChar);   //���ͽ��յ��ַ�
    	  scib_msg("\r\n");
      }
   }
}

//###########################################################################
// $ ��������:  void scib_echoback_init()     $
// $ ��������:  ��ʼ��scibģ��        $
//###########################################################################
void scib_echoback_init()
{
    ScibRegs.SCICCR.all =0x0007;   // 1λֹͣλ������żУ��λ��8������λ
    ScibRegs.SCICTL1.all =0x0003;   // ʹ��TX��RX���ر�˯��ģʽ���رս��վ���
    ScibRegs.SCICTL2.all =0x0003;   // ʹ�ܽ����жϣ���ֹ�����ж�
    ScibRegs.SCICTL2.bit.TXINTENA =1;   // ʹ��TXRDY�ж�
    ScibRegs.SCICTL2.bit.RXBKINTENA =1;   // ʹ��RXRDY/BRKDT�ж�
    ScibRegs.SCIHBAUD    =0x0001;
    ScibRegs.SCILBAUD    =0x0024;   // 9600 ������  @LSPCLK = 22.5MHz (90 MHz SYSCLK)
    ScibRegs.SCICTL1.all =0x0023;   //��λSCIB
}

//###########################################################################
// $ ��������:  void scib_xmit( int a)     $
// $ ��������:  scib���͵�������        $
//###########################################################################
void scib_xmit( int a)
{
   while (ScibRegs.SCICTL2.bit.TXEMPTY == 0)  // ��ѯ�Ƿ������
   {

   }

   ScibRegs.SCITXBUF=a;   //��������a
}

//###########################################################################
// $ ��������:  void scib_msg(  char * msg)     $
// $ ��������:  scib������������        $
//###########################################################################
void scib_msg(  char * msg)
{
   int i=0;
   while(msg[i] != '\0')   //�ж������Ƿ����
   {
    scib_xmit(msg[i]);    //����ǰ�淢�͵�����ĸ�ĺ���
    i++;
   }
}




