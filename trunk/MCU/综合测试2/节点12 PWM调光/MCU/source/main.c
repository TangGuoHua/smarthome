/*
STC 12C5608AD
4MHz外部晶振

日期        谁？    做了什么？
----------------------------------------------------
2013-APR-09 黄长浩  为调光台灯修改程序: initPWM()里面，设置CMOD = 0x00，
                    以使PCA的时钟源频率为SYSclk/12,
					因为新买的3W LED驱动模块推荐的PWM频率为1KHz左右。
2013-APR-29 黄长浩  调光台灯增加延时关、闪光提示、渐亮渐暗等功能
*/

#include "reg52.h"
#include "nrf24L01Node.h"

sfr CCON = 0xD8;
sbit CR = CCON^6;
sfr CMOD = 0xD9;
sfr CL = 0xE9;
sfr CH = 0xF9;
sfr CCAPM0 = 0xDA;
sfr CCAP0L = 0xEA;
sfr CCAP0H = 0xFA;

//sfr CCAPM2 = 0xDC;
//sfr CCAP2L = 0xEC;
//sfr CCAP2H = 0xFC;

//sbit LED=P3^7;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////
sfr  AUXR       =   0x8e;       //辅助寄存器            Bit7    Bit6    Bit5    Bit4    Bit3    Bit2    Bit1    Bit0
                                //位描述                T0x12   T1x12   UM0x6   EADCI   ESPI    ELVDI   -       -
                                //初始值=0000,00xx      0       0       0       0       0       0       x       x
								
unsigned char brightness = 0;
unsigned char workMode = 0; // 0:常灭, 1:常亮, 2:延时关
unsigned char delayOffTimerCount = 0;  //延时关计数器

void initPWM()
{
	CCON = 0;
	
	CL = 0;
	CH = 0;
	//CMOD = 0x02;
	CMOD = 0x00;
	CCAP0H = CCAP0L = 0;  //初始状态不亮
	CCAPM0 = 0X42;
	
	//CCAP2H = CCAP2L = 0; //初始状态不亮
	//CCAPM2 = 0X42;
	
	CR =1;
}


void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}

//delay 100ms
void delay(void)
{
    unsigned char a,b,c;
    for(c=4;c>0;c--)
        for(b=116;b>0;b--)
            for(a=214;a>0;a--);
}

//改变亮度时，渐亮渐暗延时
void delayTransition(void) //10ms
{
    unsigned char a,b,c;
    for(c=2;c>0;c--)
        for(b=38;b>0;b--)
            for(a=130;a>0;a--);
}

void delay10s(void)
{
    unsigned char a,b,c,n;
    for(c=191;c>0;c--)
        for(b=209;b>0;b--)
            for(a=249;a>0;a--);
    for(n=4;n>0;n--);
}

//blink LED once
void blink(void)
{
	CCAP0H = CCAP0L = 0;
	delay();
	CCAP0H = CCAP0L = brightness;
}


//设置LED亮度，0：关， 255：最亮
void setBrightness( unsigned bri )
{
	if( bri>brightness )
	{
		do
		{
			CCAP0H = CCAP0L = ++brightness;
			delayTransition();
		}while( bri>brightness );
	}
	else if( bri<brightness )
	{
		do
		{
			CCAP0H = CCAP0L = --brightness;
			delayTransition();
		}while( bri < brightness );	
	}
}

void main()
{
	
	//初始化中断0
	initINT0();

	//初始化24L01
	nrf24L01Init();
	
	//设置24L01为接收模式PRX
	nrfSetRxMode();
	
	initPWM();
	
	setBrightness( 255); //初始状态为最亮
	workMode = 1; //常亮
	
	while(1)
	{
		
		if( workMode == 2 && brightness > 0 )
		{
			if( delayOffTimerCount < 12 )  // delayOffTimerCount < N, 则延时关时间就是 10*N（秒）
				delayOffTimerCount++;
			else
				setBrightness( 0 );
		}
		
		delay10s();
		
	}
}

//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	//unsigned char level[25]={0, 1, 2, 3, 4, 6, 9, 13, 16, 20, 25, 32, 38, 45, 55, 65, 80, 90, 110, 130, 150, 170, 190, 220, 255};
	unsigned char level[11]={0, 3, 8, 16, 30, 50, 75, 100, 140, 190, 255};
	
	

	receivedData = nrfGetReceivedData();
	if( *receivedData <= 10 )
	{
//		brightness = level[*receivedData];
//		CCAP0H = CCAP0L = brightness;
		setBrightness( level[*receivedData] );
		workMode = 1;
	}
	else if ( *receivedData == 20 ) // delay off
	{
		blink();
		workMode = 2;
		delayOffTimerCount = 0;
	}
	
//	if( *++receivedData <= 10 )
//	{
//		CCAP2H = CCAP2L = level[*receivedData];
//	}
}