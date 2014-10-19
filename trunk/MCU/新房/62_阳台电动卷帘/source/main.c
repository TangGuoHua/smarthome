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
2014��10��19��  �Ƴ���  ����nrf����
                        ����BH1750/DS18B20

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
************************************************************************/

#include <reg52.h>
#include "nrf24L01Node.h"
#include "bh1750fvi.h"
#include "ds18B20.h"


sfr AUXR   = 0x8E;

// Node ID
#define NODE_ID 62

/*
�г̿���˵��:

ϵͳͨ�����Ƶ������ʱ�䣨��ticks�����������ƴ�����λ�á�
ϵͳ�趨timer0ÿ10ms�ж�һ�Σ�����һ��tick��ͨ����ticks�����Ϳ���֪���������ʱ�䡣
ͨ��ʵ�飬���������ϵ����£����У�����Լ41.5�룬����ȫ������Լ40.5�롣
�ڼ����ʱ�������г̶���ȫ��41.5����㡣ÿ10msһ��tick����ȫ��4150��ticks��

���ǵ�ʹ��unsigned int������ticks��Ϊ�������0��������������500��Ϊ��λ�ü�����4650��Ϊ���λ�ü���

����г�ʱ��Ϊ 4650-500=4150 ticks�� ��4150*10=41500ms=41.5s

���û�ѡ��ȫ����0%���ǣ�����ȫ�أ�100%���ǣ�ʱ��ϵͳ��������300*10ms����3�룩���������ۼ����
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

volatile unsigned char curtainMode = 0; //��������ģʽ
volatile unsigned char curtainCoverPercent; //�������ǶȰٷֱ�

//����˶�����
volatile bit motorDirection = DIRECTION_UP; 

//������ǰλ�� 10ms ticks
volatile unsigned int currentPosTicks = 0;

//Ŀ��λ�� 10ms ticks
volatile unsigned int targetPosTicks = 0;

// Flag for sending data to Pi
volatile bit sendDataNow = 0;
volatile unsigned char sendDataTimer10sCount = 0;
volatile unsigned char functionNum = 0;


void delayMainInterval(void)
{
	//��ʱ10�� @STC12C5616AD, 4MHz����
    unsigned char a,b,c,n;
    for(c=191;c>0;c--)
        for(b=209;b>0;b--)
            for(a=249;a>0;a--);
    for(n=4;n>0;n--);
}


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


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


//NRF24L01��ʼ�������ģʽ
void startRecv()
{
	unsigned char myAddr[5]= {97, 83, 62, 231, 62}; //���ڵ�Ľ��յ�ַ
	nrfSetRxMode( 92, 5, myAddr); //����92Ƶ��
}



// ��ʼ��Timer0, 4MHz, ÿ10ms����һ���ж�
void initTimer0(void)
{
	AUXR = AUXR|0x80;  //����T0Ϊ1Tģʽ
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
	//���ڸ��ϵ�,ϵͳ��֪����ǰ������λ��,�������������κ�λ�á�
	//Ϊ���þ���ȫ������ȥ����Ҫ���е���У������λ����������������Ҫ�������λ����������
	//ϵͳ���ʱ����磬�Ա��þ������������г�
	
	currentPosTicks = POS_BOTTOM ; //���赱ǰ����ײ�����ȫ��(100%����)
	targetPosTicks = CALIBRATION_LIMIT_TOP; //Ŀ����Ϊ��У������λ
	motorDirection = DIRECTION_UP;
	curtainCoverPercent = 0; //ȫ��������0%
	
	//�������
	startMotor();
}


//��������������
void sendDataToHost( float temperature, unsigned int brightness )
{
	unsigned char sendData[16];
	unsigned char toAddr[5]= {53, 69, 149, 231, 231}; //Pi, 3�ֽڵ�ַ
	unsigned char ret;
	int intTemperature;

	sendData[0] = NODE_ID;//Node ID
	sendData[1] = functionNum;// Function Number, 1-regular status update, 51-respond to 50

	sendData[2] = curtainMode; // working mode
	sendData[3] = curtainCoverPercent;
	
	sendData[4] = ~RELAY_POWER;
	sendData[5] = ~RELAY_DIRECTION;
	
	//����ֵ
	sendData[6] = brightness/256;
	sendData[7] = brightness%265;
	
	//�¶�ֵ
	//�������뵽ʮ��λ
	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5;
	sendData[8] = temperature<0 ? 1 : 0; //������������λ��1
	sendData[9] = intTemperature/10; //�¶ȵ���������
	sendData[10] = intTemperature%10; //�¶ȵ�С������

	//��������
	TR0 = 0;//Pause timer0
	ret = nrfSendData( 96, 5, toAddr, 16, sendData);
	
	//24L01��ʼ��������
	startRecv();
	TR0 = 1;//Resume timer0
	
}


//������
void main()
{
	float temperature;
	unsigned int brightness;
	
	//��ʼ���ж�0
	initINT0();
    
	//��ʼ���̵���
	initRelays();
	
	//��ʼ��24L01
	nrf24L01Init();
	
	// ��ʼ�� BH1750FVI
	bh1750Init();
	
	//��ʼ��DS18B20
	initDS18B20();
	
	//24L01��ʼ��������
	startRecv(); 
	
	//��ʼ��timer0
	initTimer0();
	
	//��ʼ������λ��
	initCurtain();
	
	
	while(1)
	{
		// DS18B20ȡ���¶�ֵ
		temperature = readTemperature(0);
		
		//��ȡ����ֵ
		brightness = bh1750GetBrightness();
		
		//��ʱ״̬����
		if( ++sendDataTimer10sCount >= 60 ) //ÿ10*N�뷢��һ������
		{
			sendDataTimer10sCount = 0;
			functionNum = 1; //regular status update
			sendDataNow = 1;
			
		}
		
		if( sendDataNow )
		{
			sendDataNow = 0;
			sendDataToHost( temperature, brightness );
		}
		
		delayMainInterval();

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
	// *(receivedData+1): ���ܺ� 
	// *(receivedData+2): ��������ģʽ ��Ŀǰ���Ըò�����
	// *(receivedData+3): �������ǶȰٷֱȡ�����Ҫ����30%�����ֵΪ30
	
	
	switch( *(receivedData+1) )
	{
		case 20: //ִ��������践��

			curtainMode = *(receivedData+2);
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
						targetPosTicks = POS_TOP + rCoverPercent/100.0 *(POS_BOTTOM - POS_TOP);
					}
					
					if( targetPosTicks != currentPosTicks)
					{
						//���Ŀ�꿪��С�ڵ�ǰ���ȣ���������
						motorDirection = targetPosTicks<currentPosTicks ? DIRECTION_UP : DIRECTION_DOWN;
						
						//�������
						startMotor();
					}
				}
			}
			break;
		

		
		case 50: //������������״̬
			functionNum = 51;//�ر�50
			sendDataTimer10sCount = 0; //���¼�ʱ
			sendDataNow = 1;
			break;
	
	}//end of switch
}


//��ʱ��0�жϴ������
void timer0Interrupt(void) interrupt 1
{
    TH0 = 0x63;
    TL0 = 0xC0;
	
	if(motorDirection) //�൱�� if( motorDirection == DIRECTION_UP ), ������������
	{
		//����Ѿ�������Ŀ��λ�û��߳����˶�У��������λ����ֹͣ���
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
		//����Ѿ�������Ŀ��λ�û��߳����˵�У��������λ����ֹͣ���
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
