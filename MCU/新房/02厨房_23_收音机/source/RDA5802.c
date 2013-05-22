/***************************************************************************
【硬件信息】
单片机型号: STC12C5616AD
工作频率: 11.0592MHz
其它：
RDA5802E  FM收音机模块 （通过IIC与MCU通讯）
SDA接P3^4, SCL接P3^5

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

#define I2C_ACK  1
#define I2C_NACK 0

sbit RDA5802_SDA = P3^4;
sbit RDA5802_SCL = P3^5;

/*
RDA5802 寄存器初始值数据
(先是高位字节，再是低位字节）
*/
unsigned char RDARegData[8] =
{
   0xf0,0x00,  // 02H  强制单声道，低音加重
   0x00,0x00,  // 03H  BAND 87-108MHz, Channel Space 100kHz
   0x08,0x00,  // 04H  去加重50us, Soft Mute Disabled
   0x88,0xb8,  // 05H  LNAP, 3.0mA, 音量8
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
I2C启动子程序
在 RDA5802_SCL 高电平期间 RDA5802_SDA 发生负跳变
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
I2C停止子函数
在 RDA5802_SCL 高电平期间 RDA5802_SDA 发生正跳变
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
I2C发送一个字节子程序
*/
unsigned char _RDAI2CWriteByte(unsigned char indata)
{
	unsigned char i, ack;
	
	// I2C 发送8 位数据
	for (i=0; i<8; i++)
	{
		// 高在前低在后
		if (indata & 0x80)
			RDA5802_SDA = 1;
		else
			RDA5802_SDA = 0;
		
		// 发送左移一位
		indata <<= 1;
		_RDAI2CDelay();
		RDA5802_SCL = 1;
		_RDAI2CDelay();
		RDA5802_SCL = 0;
	}
	
	// 设置RDA5802_SDA为输入
	RDA5802_SDA =1;;
	_RDAI2CDelay();
	
	// 读取从机应答状态
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
连续写寄存器子函数
*/
void _RDA5802WriteReg(void)
{
	unsigned char i;
	
	_RDAI2CStart();
	// 收音模块写入操作
	_RDAI2CWriteByte(0x20);
	// 寄存器连续写操作
	for(i=0; i<8; i++)
	{
		_RDAI2CWriteByte(RDARegData[i]);
	}
	_RDAI2CStop();
}


////////////////////////////////
//// 以下函数是供外部调用的 ////
////////////////////////////////

/*
初始化RDA5802模块: 模块复位
*/
void RDA5802Init(void)
{
	_RDADelay20ms();
	
	// 发送软件复位指令(Soft Reset)
	RDARegData[1] = 0x02; //Soft Reset:Reset, Power Up:Disabled
	_RDA5802WriteReg();
	
	_RDADelay20ms();
	_RDADelay20ms();
	_RDADelay20ms();
	
}

/*
给RDA5802模块上电/断电

参数 powerUp: 1-上电；0-断电
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
调谐到指定的频率

参数tuneFreq: 指定调谐的频率，100kHz的倍数
取值范围：870-1080，即87-108MHz
例1：要调谐到104.8MHz则frequency=1048
例2：要调谐到91.1MHz则frequency=911
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
	
	//设置频率, Tune:1-Enabled, BAND:87-108MHz, Channel Spacing:100kHz
	RDARegData[2] = channel.byteVal[0];
	RDARegData[3] = channel.byteVal[1] & 0xD0;
	
	_RDA5802WriteReg();
	// tune 需10ms才能完成
	_RDADelay20ms();
	//delayms(50);
}

/*
设置音量大小

参数setVolume: 要设置的音量值
取值范围：0～15  （0最小声，15最大声）

注意：如果在静音状态下设置音量，静音将被解除
*/
void RDA5802SetVolume( unsigned char setVolume )
{
	//要是设置的音量超过15，则使用15
	if( setVolume >15 ) setVolume=15;
	

	RDARegData[0] |= (1 << 6); //Bit6置1，如果在静音状态，则将解除静音
	RDARegData[3] &= ~(1 << 4); //disable Tune
	
	RDARegData[7] &= 0xf0;
	RDARegData[7] |= setVolume; // 音量递减
	_RDA5802WriteReg();

}
/*
是否静音

参数：toMute，1:静音，0:解除静音
*/
void RDA5802Mute( bit toMute )
{
	if( toMute )
	{
		RDARegData[0] &= ~(1 << 6); //Bit6置0
	}
	else
	{
		RDARegData[0] |= (1 << 6); //Bit6置1
	}
	RDARegData[3] &= ~(1 << 4); //disable Tune
	_RDA5802WriteReg();
}

/*
是否强制单声道

参数：toForceMono，1:强制单声道，0:立体声
*/
void RDA5802ForceMono( bit toForceMono )
{
	if( toForceMono )
	{
		RDARegData[0] |= (1 << 5); //Bit5置1
	}
	else
	{
		RDARegData[0] &= ~(1 << 5); //Bit5置0
	}
	RDARegData[3] &= ~(1 << 4); //disable Tune
	_RDA5802WriteReg();
}


/*
是否加强低音

参数：toBoostBase，1:加强低音，0:不加强低音
*/
void RDA5802BoostBass( bit toBoostBase )
{
	if( toBoostBase )
	{
		RDARegData[0] |= (1 << 4); //Bit4置1
	}
	else
	{
		RDARegData[0] &= ~(1 << 4); //Bit4置0
	}
	RDARegData[3] &= ~(1 << 4); //disable Tune
	_RDA5802WriteReg();
}


