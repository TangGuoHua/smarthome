/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 11.0592MHz
������
RDA5802E  FM������ģ�� ��ͨ��IIC��MCUͨѶ��

���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2013��05��20��  �Ƴ���  ��ʼ�汾

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include "RDA5802.h"

#define IR_COUNT_TIME_OUT 250  //�������ź�ʱ�ĵȴ����ʱ

unsigned int presetStations[]={911, 948, 965, 1000, 1011, 1028, 1048};
unsigned char currentStationIndex=0;
unsigned char currentVol=7;
bit powerUp = 0; //�Ƿ��ϵ�
	


//�������ͷ
sbit IR=P2^7;


sbit RELAY_POWER=P2^6;
sbit RELAY_SPEAKER=P2^5;

void delay50us(void)
{
    unsigned char a;
    for(a=135;a>0;a--);
}

void delay250ms(void)
{
    unsigned char a,b,c;
    for(c=74;c>0;c--)
        for(b=66;b>0;b--)
            for(a=140;a>0;a--);
}



//����ָ���ĵ�̨
void playStation()
{
	if(! powerUp) //���ģ��û���ϵ磬���ϵ粢��ʼ������
	{
		//���̵���
		RELAY_POWER=0;
		
		delay250ms();	//��ʱ��ģ���ϵ磬����̵����ĸ������ģ�鹤���쳣
		
		//Power Up
		RDA5802PowerUp(1);
		powerUp=1;
		delay250ms();

		//������
		RELAY_SPEAKER=0; //��ʱ�����ȣ����ⱬը��
		
		//Init volume
		RDA5802SetVolume(currentVol);
	}
	
	//��г��ָ���ĵ�̨
	RDA5802Tune( presetStations[currentStationIndex] );
}

void main(void)
{
	unsigned char cnt=0, i, j;
	unsigned char receivedByte;
	bit validKeyPressed = 0;
	union {
		unsigned char byteVal[2];
		unsigned int intVal;
	} keyValue;
	
	RDA5802Init();
//	RDA5802ForceMono( 0 );
//	RDA5802BoostBass( 1 );	
	RDA5802SetVolume( currentVol );

	//��ʼ�������
	IR=1;
	
	RELAY_SPEAKER=1;//�̵�����
	RELAY_POWER=1;

	while(1)
	{
		validKeyPressed = 0; //�����⵽�Ϸ���������˱���������1
		
		// �ȴ��½���
		while(IR);

		//��¼9ms�͵�ƽ�ĳ���
		cnt=0;
		while((!IR)&& cnt<IR_COUNT_TIME_OUT )
		{
			delay50us();
			cnt++;
		}
		if(cnt<170||cnt>180) continue; //9ms�͵�ƽ�����ϣ���Ϊ����
	
		
		//��¼4.5ms�ߵ�ƽ�ĳ���
		cnt=0;
		while(IR && cnt<IR_COUNT_TIME_OUT )
		{
			delay50us();
			cnt++;
		}
		if(cnt<80||cnt>95) continue; //4.5ms�ߵ�ƽ�����ϣ���Ϊ����
		
		//��ȡ4���ֽ�
		for( i=0; i<4; i++ )
		{
			receivedByte=0;
			
			for( j=0; j<8; j++ )
			{
				//��¼0.56ms�͵�ƽ
				cnt=0;
				while(!IR && cnt<IR_COUNT_TIME_OUT )
				{
					delay50us();
					cnt++;
				}
				if(cnt<8||cnt>15) continue; //0.56ms�͵�ƽ�����ϣ���Ϊ����

				//��¼�ߵ�ƽ���ȣ�������0����1
				cnt=0;
				while(IR && cnt<IR_COUNT_TIME_OUT )
				{
					delay50us();
					cnt++;
				}
				
				if( cnt>7 && cnt<16 )//0.56ms�ߵ�ƽ����Ϊ0
				{
					if( j!=0 ) receivedByte <<= 1; //��λ��������
				}
				else if(cnt>28 && cnt<40)//1.68ms�ߵ�ƽ����Ϊ0
				{
					//��λΪ1
					if( j!=0 ) receivedByte <<= 1; //��λ��������
					receivedByte |= 1;
				}
				else break; //�͵�ƽ������0.56ms��1.68ms����Ϊ����
			}
			
			if( cnt >= IR_COUNT_TIME_OUT) break; //����
			
			//��һ�ֽ�Ϊ0x00���ڶ��ֽ�Ϊ0xff
			//���������ֽ�Ϊ��ֵ
			if( i==0 && receivedByte!=0 ) break;
			else if( i==1 && receivedByte!=0xff ) break;
			else if( i==2 )
			{
				keyValue.byteVal[0] = receivedByte;
			}
			else if( i==3 )
			{
				keyValue.byteVal[1] = receivedByte;
				validKeyPressed = 1; //��⵽�˺Ϸ�����
			}
		}
		
		
		if(validKeyPressed)
		{
			//��⵽�Ϸ��İ�����
			
			switch( keyValue.intVal )
			{
//				case 0xa25d: //CH-
//				case 0x629d: //CH
//				case 0xe21d: //CH+
//					if(powerUp)
//					{
//						RDA5802PowerUp(0);
//						powerUp=0;
//					}
//					break;
					
//				case 0x22dd: //|<<
//					RDA5802PowerUp(0);
//					break;
//					
//				case 0x02fd: //>>|
//					RDA5802PowerUp(1);
//					break;
					
				case 0xe01f: //VOL-
					if( powerUp && (currentVol>1) )
					{
						currentVol--;
						RDA5802SetVolume(currentVol);
					}
					break;
					
				case 0xa857: //VOL+
					if( powerUp && (currentVol<15) )
					{
						currentVol++;
						RDA5802SetVolume(currentVol);
					}
					break;
					
				case 0x906f: //EQ (Power Off)
					if(powerUp)
					{
						RELAY_SPEAKER=1;
						delay250ms();
						delay250ms();
						RELAY_POWER=1; //��ʱ�ص�Դ�����ⱬը��
						
						RDA5802PowerUp(0); //��ģ��
						powerUp=0;
					}
					break;
				
				case 0x30cf: //1
					currentStationIndex=0;
					playStation();
					break;
					
				case 0x18e7: //2
					currentStationIndex=1;
					playStation();
					break;
					
				case 0x7a85: //3
					currentStationIndex=2;
					playStation();
					break;
					
				case 0x10ef: //4
					currentStationIndex=3;
					playStation();
					break;
					
				case 0x38c7: //5
					currentStationIndex=4;
					playStation();
					break;
					
				case 0x5aa5: //6
					currentStationIndex=5;
					playStation();
					break;
					
				case 0x42bd: //7
					currentStationIndex=6;
					playStation();
					break;

				default:
					break;
					
			}
		}
	}
}


