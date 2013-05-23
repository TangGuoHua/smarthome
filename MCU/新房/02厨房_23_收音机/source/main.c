/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 11.0592MHz
其它：
RDA5802E  FM收音机模块 （通过IIC与MCU通讯）

【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2013年05月20日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>
#include <intrins.h>
#include "RDA5802.h"

#define IR_COUNT_TIME_OUT 250  //检测红外信号时的等待最大超时

unsigned int presetStations[]={911, 948, 965, 1000, 1011, 1028, 1048};
unsigned char currentStationIndex=0;
unsigned char currentVol=7;
bit powerUp = 0; //是否上电
	


//红外接收头
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



//播放指定的电台
void playStation()
{
	if(! powerUp) //如果模块没有上电，则上电并初始化音量
	{
		//开继电器
		RELAY_POWER=0;
		
		delay250ms();	//延时给模块上电，避免继电器的干扰造成模块工作异常
		
		//Power Up
		RDA5802PowerUp(1);
		powerUp=1;
		delay250ms();

		//开喇叭
		RELAY_SPEAKER=0; //延时开喇叭，避免爆炸声
		
		//Init volume
		RDA5802SetVolume(currentVol);
	}
	
	//调谐到指定的电台
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

	//初始化红外口
	IR=1;
	
	RELAY_SPEAKER=1;//继电器关
	RELAY_POWER=1;

	while(1)
	{
		validKeyPressed = 0; //如果检测到合法按键，则此变量将被置1
		
		// 等待下降沿
		while(IR);

		//记录9ms低电平的长度
		cnt=0;
		while((!IR)&& cnt<IR_COUNT_TIME_OUT )
		{
			delay50us();
			cnt++;
		}
		if(cnt<170||cnt>180) continue; //9ms低电平不符合，判为干扰
	
		
		//记录4.5ms高电平的长度
		cnt=0;
		while(IR && cnt<IR_COUNT_TIME_OUT )
		{
			delay50us();
			cnt++;
		}
		if(cnt<80||cnt>95) continue; //4.5ms高电平不符合，判为干扰
		
		//收取4个字节
		for( i=0; i<4; i++ )
		{
			receivedByte=0;
			
			for( j=0; j<8; j++ )
			{
				//记录0.56ms低电平
				cnt=0;
				while(!IR && cnt<IR_COUNT_TIME_OUT )
				{
					delay50us();
					cnt++;
				}
				if(cnt<8||cnt>15) continue; //0.56ms低电平不符合，判为干扰

				//记录高电平长度，决定是0还是1
				cnt=0;
				while(IR && cnt<IR_COUNT_TIME_OUT )
				{
					delay50us();
					cnt++;
				}
				
				if( cnt>7 && cnt<16 )//0.56ms高电平，判为0
				{
					if( j!=0 ) receivedByte <<= 1; //首位不用左移
				}
				else if(cnt>28 && cnt<40)//1.68ms高电平，判为0
				{
					//该位为1
					if( j!=0 ) receivedByte <<= 1; //首位不用左移
					receivedByte |= 1;
				}
				else break; //低电平不符合0.56ms或1.68ms，判为干扰
			}
			
			if( cnt >= IR_COUNT_TIME_OUT) break; //干扰
			
			//第一字节为0x00，第二字节为0xff
			//第三、四字节为键值
			if( i==0 && receivedByte!=0 ) break;
			else if( i==1 && receivedByte!=0xff ) break;
			else if( i==2 )
			{
				keyValue.byteVal[0] = receivedByte;
			}
			else if( i==3 )
			{
				keyValue.byteVal[1] = receivedByte;
				validKeyPressed = 1; //检测到了合法按键
			}
		}
		
		
		if(validKeyPressed)
		{
			//检测到合法的按键了
			
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
						RELAY_POWER=1; //延时关电源，避免爆炸声
						
						RDA5802PowerUp(0); //关模块
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


