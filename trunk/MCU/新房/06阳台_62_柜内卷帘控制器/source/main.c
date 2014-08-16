/***********************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
�̵���ģ��


���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��07��30��  �Ƴ���  ��ʼ�汾
2014��08��01��  �Ƴ���  ʵ��ָ�����Ƕȣ������ۻ����


����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
//#include "adc.h"
//#include "stcEEPROM.h"
//#include "ds18B20.h"

sfr AUXR   = 0x8E;

// Node ID
#define NODE_ID 62

/*
�г̿���˵��:
ϵͳͨ�����Ƶ������ʱ�䣨��ticks�����������ƴ�����λ�á�
ϵͳ�趨timer0ÿ10ms�ж�һ�Σ�����һ��tick��ͨ����ticks�Ϳ���֪������ʱ�䡣
ͨ��ʵ�飬���������ϵ����£����У�����Լ41.5�룬����ȫ������Լ40.5�롣
�ڼ����ʱ�������г̶���ȫ��41.5����㡣ÿ10msһ��tick����ȫ��4150��ticks��

���ǵ�ʹ��unsigned int������ticks��Ϊ�������0��������������500��Ϊ��λ�ü�����4650��Ϊ���λ�ü���

����г�ʱ��Ϊ 4650-500=4150 ticks�� ��4150*10=41500ms=41.5s

���û�ѡ��ȫ����0%���ǣ�����ȫ�أ�100%���ǣ�ʱ��ϵͳ��������300*10ms���������ۼ����
����λ������ֵ����ͼ��ʾ��

              ��                   ��
�����£� |_____|____________________|_____|___| �����ϣ�
       4950   4650                 500   200  0
         ^                                ^
  ��У������                              ��У������
  
*/
//#define MAX_TICKS 4150 //�������ʱ�䣨10ms����ֵ�������������ʱ��Ϊ MAX_TICKS*10���� ��
#define POS_BOTTOM 4650
#define POS_TOP 500
#define CALIBRATION_LIMIT_TOP 200
#define CALIBRATION_LIMIT_BOTTOM 4950

//����˶�����
//1������������������0���½����ؾ�����
#define DIRECTION_UP 1
#define DIRECTION_DOWN 0


//sbit LDR = P1^7;  //�������� ��10K������

sbit RELAY_POWER = P1^3; //�̵���1�����Ƶ�Դ
sbit RELAY_DIRECTION = P3^5; //�̵���2�����Ʒ��򣬳���-����������-�½�

//sbit RELAY_POWER = P2^1; //�̵���1�����Ƶ�Դ
//sbit RELAY_DIRECTION = P2^0; //�̵���2�����Ʒ��򣬳���-����������-�½�

//unsigned char curtainMode = 1; //1:�ֶ���2���Զ�
unsigned char curtainCoverPercent; //�������ǶȰٷֱ�

//����˶�����
bit motorDirection = DIRECTION_UP; 


//������ǰλ�� 10ms ticks
unsigned int currentPosTicks = 0;

//Ŀ��λ�� 10ms ticks
unsigned int targetPosTicks = 0;

//�̵�����ʱ
void delayRelay()
{
	//4Mhz, 1T, Լ300ms
    unsigned char a,b,c;
    for(c=244;c>0;c--)
        for(b=8;b>0;b--)
            for(a=152;a>0;a--);
}

//�������
//�����������motorDirection�趨�ķ�����ת��ͬʱ����timer0������
void startMotor()
{
	//�趨����
	RELAY_DIRECTION = motorDirection;
	
	delayRelay();
	
	//ͨ��
	RELAY_POWER = 0;
	
	//��timer0
	TR0 = 1;
}

//�رյ����ͬʱֹͣtimer0
void stopMotor()
{
	//ͣ����Դ
	RELAY_POWER = 1;
	
	//ֹͣtimer0
	TR0=0;
	
	delayRelay();
	
	//����ѡ��̵����ϵ�
	RELAY_DIRECTION = 1;
}


////������ʱ 
////����NodeID������ԼΪ500*NodeID�������ʱ
////�����Ǳ������нڵ�ͬʱ�ϵ磬������5���Ӽ������������ɵ�ͨѶ��ײ
//void initDelay(void)
//{
//	//4MHz Crystal, 1T STC11F04E
//    unsigned char a,b,c,d;
//    for(d=NODE_ID;d>0;d--)
//	    for(c=167;c>0;c--)
//	        for(b=171;b>0;b--)
//	            for(a=16;a>0;a--);
//}

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 62}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 3, myAddr); //����92Ƶ����3�ֽڵ�ַ
}
//
//
////���ص�ǰ����ֵ
//unsigned char getBrightness()
//{
//	return (255-getADCResult(7));
//}
//
//
//
////��������ֵ����Ƭ��eeprom
//void saveLightOnThreshold( unsigned char threshold1, unsigned char threshold2 )
//{
//	eepromEraseSector( 0x0000 );
//	eepromWrite( 0x0000, threshold1 );
//	eepromWrite( 0x0002, threshold2 );
//}
//
//////��Ƭ��eepromȡ�ÿ�����ֵ
////unsigned char getLightOnThreshold()
////{
////	return eepromRead(0x0000);
////}
//
//

//void delay10s(void)   //��� 0us
//{
//    unsigned char a,b,c,n;
//    for(c=191;c>0;c--)
//        for(b=209;b>0;b--)
//            for(a=249;a>0;a--);
//    for(n=4;n>0;n--);
//
//}
//
//
//void sendDataToHost( unsigned int tar, unsigned int cur, unsigned char c)
//{
//	unsigned char sendData[16];
//	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 96Ƶ����3�ֽڵ�ַ������16�ֽ�
//	unsigned char tmp;
//
//		
//	sendData[0] = NODE_ID;//Node ID
//
//	sendData[1] = 11;
//	sendData[2] = 22;
//	sendData[3] = 33; 
//	sendData[4] = (tar >> 8); 
//	sendData[5] = (tar & 0x00ff); 
//	sendData[6] = (cur >> 8); 
//	sendData[7] = (cur & 0x00ff); 
//	sendData[8] = c ; 
//
//
//	tmp = nrfSendData( 96, 3, toAddr, 16, sendData);//��Pi����, 96Ƶ����3�ֽڵ�ַ��16�ֽ�����
//	
//	//24L01��ʼ��������
//	startRecv(); 
//}


// ��ʼ��Timer0, 4MHz, ÿ10ms����һ���ж�
void initTimer0(void)
{
    TMOD = 0x01;
    TH0 = 0x63;
    TL0 = 0xC0;
    EA = 1;
    ET0 = 1;
    //TR0 = 1; //start timer 0
}

//��ʼ���̵���
void initRelays()
{
	RELAY_POWER=1;
	RELAY_DIRECTION=1;
}

//��ʼ������λ��
void initCurtain()
{
	//ϵͳ�ϵ��ʼ��ʱ������Ҫ�Ѿ���ȫ������ȥ��
	//Ϊ���ü��������ǰ���������棬����Ҫȫ������ȥ����Ҫ���е���У������λ�����������
	//���������ǰ���������棬����Ҫȫ������ȥ����Ҫ���е���У������λ�����������
	
	currentPosTicks = POS_BOTTOM ; //���赱ǰ����ײ�����ȫ��(100%����)
	targetPosTicks = CALIBRATION_LIMIT_TOP; //Ŀ����Ϊ��У������λ
	motorDirection = DIRECTION_UP;
	curtainCoverPercent = 0; //ȫ��������0%
	
	//�������
	startMotor();
}



void main()
{

	
	AUXR = AUXR|0x80;  // T0, 1T Mode
	
	//��ʼ���ж�0
	initINT0();
    
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��24L01
	nrf24L01Init();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ��timer0
	initTimer0();
	
	//��ʼ������λ��
	initCurtain();
	
	
	while(1)
	{
//		sendDataToHost(targetPosTicks, currentPosTicks , curtainCoverPercent);
//		delay10s();
	}
}


//�ⲿ�ж�0�������
//���ڴ�������24L01���ж�
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	unsigned char rCoverPercent;
	
	//��ȡ���յ�������
	receivedData = nrfGetReceivedData();
	
	// *(receivedData+0): ������NodeID ��Ŀǰ���Ըò�����
	// *(receivedData+1): ���ܺ� ��Ŀǰ���Ըò�����
	// *(receivedData+2): ��������ģʽ ��Ŀǰ���Ըò�����
	// *(receivedData+3): �������ǶȰٷֱȡ�����Ҫ����30%�����ֵΪ30

	rCoverPercent = *(receivedData+3);
	

	if( rCoverPercent< 101 ) //rCoverPercentӦ����[0,100]���������һ����
	{
		if (curtainCoverPercent != rCoverPercent ) //����յ��ĸ��Ƕ��뵱ǰ�ĸ��ǶȲ�ͬ
		{
			curtainCoverPercent = rCoverPercent;
			
			if( rCoverPercent== 0 ) 
			{
				//����û�ѡ��ȫ����������������������У��
				//���õ�����е���У������λ
				targetPosTicks = CALIBRATION_LIMIT_TOP ;
			}
			else if( rCoverPercent== 100 ) 
			{
				//����û�ѡ��ȫ�ء�������������������У��
				//���õ�����е���У������λ			
				targetPosTicks = CALIBRATION_LIMIT_BOTTOM ;
			}
			else
			{
				//����ǽ���(1%,100%)֮��ĸ��Ƕȣ�����ݰٷֱȼ���������Ŀ��λ��
				targetPosTicks = POS_TOP + rCoverPercent /100.0 *(POS_BOTTOM - POS_TOP);
			}
			
			if( targetPosTicks != currentPosTicks)
			{
				//���Ŀ�꿪��С�ڵ�ǰ���ȣ���������
				motorDirection = targetPosTicks<currentPosTicks?1:0;
				
				//�������
				startMotor();
			}

		}
	}
}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
	
	if(motorDirection) //�൱�� if( motorDirection == DIRECTION_UP ), ������������
	{
		if( (--currentPosTicks <= targetPosTicks) || ( currentPosTicks < CALIBRATION_LIMIT_TOP  ) )
		{
			//ֹͣ���
			stopMotor();
			
			//���������е�У������������ض�λ��
			if( currentPosTicks <= CALIBRATION_LIMIT_TOP )
			{
				currentPosTicks = POS_TOP;
			}
		}
	}
	else //�൱�� else if( motorDirection == DIRECTION_DOWN )��������������
	{
		if( ( ++currentPosTicks >= targetPosTicks ) || ( currentPosTicks > CALIBRATION_LIMIT_BOTTOM  ) )
		{
			//ֹͣ���
			stopMotor();
			
			//���������е�У������������ص�λ��
			if(currentPosTicks >= CALIBRATION_LIMIT_BOTTOM)
			{
				currentPosTicks = POS_BOTTOM;
			}
		}
	}
    

}
