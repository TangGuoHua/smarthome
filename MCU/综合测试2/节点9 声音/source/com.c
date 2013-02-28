#include <reg52.h>
#include "com.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sfr  AUXR       =   0x8e;       //�����Ĵ���            Bit7    Bit6    Bit5    Bit4    Bit3    Bit2    Bit1    Bit0
                                //λ����                T0x12   T1x12   UM0x6   EADCI   ESPI    ELVDI   -       -
                                //��ʼֵ=0000,00xx      0       0       0       0       0       0       x       x
								
void serialPortInit(void)
{
	//57600bps@3.6864MHz
	PCON &= 0x7f;		//�����ʲ�����
	SCON = 0x50;		//8λ����,�ɱ䲨����
	AUXR |= 0x40;		//��ʱ��1ʱ��ΪFosc,��1T
	AUXR &= 0xfe;		//����1ѡ��ʱ��1Ϊ�����ʷ�����
	TMOD &= 0x0f;		//�����ʱ��1ģʽλ
	TMOD |= 0x20;		//�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFE;		//�趨��ʱ��ֵ
	TH1 = 0xFE;		//�趨��ʱ����װֵ
	ET1 = 0;		//��ֹ��ʱ��1�ж�
	TR1 = 1;		//������ʱ��1

	//EA = 1;       //�����ж�
	//ES = 1;       //�򿪴����ж�
	ES=0;          //�رմ����ж�
}


/*-----------
����һ���ֽ�
-----------*/
void serialPortSendByte( unsigned char dat )
{
	//ES=0; //�رմ����ж�
	SBUF = dat;
	while(!TI); //�ȴ��������
	TI = 0;  //�巢����ϱ�־
	//ES=1;  //���������ж�
}

//void SendStr(unsigned char *s)
//{
// while(*s!='\0')
//  {
//  SendByte(*s);
//  s++;
//  }
//}