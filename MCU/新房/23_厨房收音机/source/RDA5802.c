/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 11.0592MHz
������
RDA5802E  FM������ģ�� ��ͨ��IIC��MCUͨѶ��
SDA��P3^4, SCL��P3^5

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

#define I2C_ACK  1
#define I2C_NACK 0

sbit RDA5802_SDA = P3^4;
sbit RDA5802_SCL = P3^5;

/*
RDA5802 �Ĵ�����ʼֵ����
(���Ǹ�λ�ֽڣ����ǵ�λ�ֽڣ�
*/
unsigned char RDARegData[8] =
{
   0xf0,0x00,  // 02H  ǿ�Ƶ���������������
   0x00,0x00,  // 03H  BAND 87-108MHz, Channel Space 100kHz
   0x08,0x00,  // 04H  ȥ����50us, Soft Mute Disabled
   0x88,0xb8,  // 05H  LNAP, 3.0mA, ����8
};


void _RDADelay20ms(void)   //11.0592MHz
{
    unsigned char a,b,c;
    for(c=14;c>0;c--)
        for(b=168;b>0;b--)
            for(a=22;a>0;a--);
}

void _RDADelay500ms(void)  //11.0592MHz
{
    unsigned char a,b,c;
    for(c=246;c>0;c--)
        for(b=212;b>0;b--)
            for(a=25;a>0;a--);
}


void _RDAI2CDelay()
{
	_nop_();_nop_();_nop_();_nop_();
	_nop_();_nop_();_nop_();_nop_();
}

/* 
I2C�����ӳ���
�� RDA5802_SCL �ߵ�ƽ�ڼ� RDA5802_SDA ����������
*/
void _RDAI2CStart()
{
	RDA5802_SDA = 1;
	RDA5802_SCL = 1;
	_RDAI2CDelay();
	RDA5802_SDA = 0;
	_RDAI2CDelay();
	RDA5802_SCL = 0;
}

/*
I2Cֹͣ�Ӻ���
�� RDA5802_SCL �ߵ�ƽ�ڼ� RDA5802_SDA ����������
*/
void _RDAI2CStop()
{
	RDA5802_SDA = 0;
	RDA5802_SCL = 1;
	_RDAI2CDelay();
	RDA5802_SDA = 1;
	_RDAI2CDelay();
}

/*
I2C����һ���ֽ��ӳ���
*/
unsigned char _RDAI2CWriteByte(unsigned char indata)
{
	unsigned char i, ack;
	
	// I2C ����8 λ����
	for (i=0; i<8; i++)
	{
		// ����ǰ���ں�
		if (indata & 0x80)
			RDA5802_SDA = 1;
		else
			RDA5802_SDA = 0;
		
		// ��������һλ
		indata <<= 1;
		_RDAI2CDelay();
		RDA5802_SCL = 1;
		_RDAI2CDelay();
		RDA5802_SCL = 0;
	}
	
	// ����RDA5802_SDAΪ����
	RDA5802_SDA =1;;
	_RDAI2CDelay();
	
	// ��ȡ�ӻ�Ӧ��״̬
	RDA5802_SCL = 1;
	
	_RDAI2CDelay();
	
	if(RDA5802_SDA)
	{
		ack = I2C_NACK;
	}
	else
	{
		ack = I2C_ACK;
	}
	RDA5802_SCL = 0;
	
	return ack;
}

/*
����д�Ĵ����Ӻ���
*/
void _RDA5802WriteReg(void)
{
	unsigned char i;
	
	_RDAI2CStart();
	// ����ģ��д�����
	_RDAI2CWriteByte(0x20);
	// �Ĵ�������д����
	for(i=0; i<8; i++)
	{
		_RDAI2CWriteByte(RDARegData[i]);
	}
	_RDAI2CStop();
}


////////////////////////////////
//// ���º����ǹ��ⲿ���õ� ////
////////////////////////////////

/*
��ʼ��RDA5802ģ��: ģ�鸴λ
*/
void RDA5802Init(void)
{
	_RDADelay20ms();
	
	// ���������λָ��(Soft Reset)
	RDARegData[1] = 0x02; //Soft Reset:Reset, Power Up:Disabled
	_RDA5802WriteReg();
	
	_RDADelay20ms();
	_RDADelay20ms();
	_RDADelay20ms();
	
}

/*
��RDA5802ģ���ϵ�/�ϵ�

���� powerUp: 1-�ϵ磻0-�ϵ�
*/
void RDA5802PowerUp( bit powerUp )
{
	if( powerUp )
	{
		// Power Up
		RDARegData[1] = 0x01; //CLK_mode:000-32.768kHz, Soft Reset:Not-Reset, Power Up:Enabled
	}
	else
	{
		// Power Down
		RDARegData[1] = 0x00; //CLK_mode:000-32.768kHz, Soft Reset:Not-Reset, Power Up:Enabled
	}
	
	_RDA5802WriteReg();
	_RDADelay500ms();
}

/* 
��г��ָ����Ƶ��

����tuneFreq: ָ����г��Ƶ�ʣ�100kHz�ı���
ȡֵ��Χ��870-1080����87-108MHz
��1��Ҫ��г��104.8MHz��frequency=1048
��2��Ҫ��г��91.1MHz��frequency=911
*/
void RDA5802Tune( unsigned int tuneFreq )
{
	union {
		unsigned int intVal;
		unsigned char byteVal[2];
		} channel;
			
	channel.intVal = tuneFreq-870;
	
	channel.intVal <<= 6;
	channel.intVal |= 0x003f;
	
	//����Ƶ��, Tune:1-Enabled, BAND:87-108MHz, Channel Spacing:100kHz
	RDARegData[2] = channel.byteVal[0];
	RDARegData[3] = channel.byteVal[1] & 0xD0;
	
	_RDA5802WriteReg();
	// tune ��10ms�������
	_RDADelay20ms();
	//delayms(50);
}

/*
����������С

����setVolume: Ҫ���õ�����ֵ
ȡֵ��Χ��0��15  ��0��С����15�������

ע�⣺����ھ���״̬�����������������������
*/
void RDA5802SetVolume( unsigned char setVolume )
{
	//Ҫ�����õ���������15����ʹ��15
	if( setVolume >15 ) setVolume=15;
	

	RDARegData[0] |= (1 << 6); //Bit6��1������ھ���״̬���򽫽������
	RDARegData[3] &= ~(1 << 4); //disable Tune
	
	RDARegData[7] &= 0xf0;
	RDARegData[7] |= setVolume; // �����ݼ�
	_RDA5802WriteReg();

}
/*
�Ƿ���

������toMute��1:������0:�������
*/
void RDA5802Mute( bit toMute )
{
	if( toMute )
	{
		RDARegData[0] &= ~(1 << 6); //Bit6��0
	}
	else
	{
		RDARegData[0] |= (1 << 6); //Bit6��1
	}
	RDARegData[3] &= ~(1 << 4); //disable Tune
	_RDA5802WriteReg();
}

/*
�Ƿ�ǿ�Ƶ�����

������toForceMono��1:ǿ�Ƶ�������0:������
*/
void RDA5802ForceMono( bit toForceMono )
{
	if( toForceMono )
	{
		RDARegData[0] |= (1 << 5); //Bit5��1
	}
	else
	{
		RDARegData[0] &= ~(1 << 5); //Bit5��0
	}
	RDARegData[3] &= ~(1 << 4); //disable Tune
	_RDA5802WriteReg();
}


/*
�Ƿ��ǿ����

������toBoostBase��1:��ǿ������0:����ǿ����
*/
void RDA5802BoostBass( bit toBoostBase )
{
	if( toBoostBase )
	{
		RDARegData[0] |= (1 << 4); //Bit4��1
	}
	else
	{
		RDARegData[0] &= ~(1 << 4); //Bit4��0
	}
	RDARegData[3] &= ~(1 << 4); //disable Tune
	_RDA5802WriteReg();
}


