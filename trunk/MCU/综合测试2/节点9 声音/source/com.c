#include <reg52.h>
#include "com.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////////

sfr  AUXR       =   0x8e;       //辅助寄存器            Bit7    Bit6    Bit5    Bit4    Bit3    Bit2    Bit1    Bit0
                                //位描述                T0x12   T1x12   UM0x6   EADCI   ESPI    ELVDI   -       -
                                //初始值=0000,00xx      0       0       0       0       0       0       x       x
								
void serialPortInit(void)
{
	//57600bps@3.6864MHz
	PCON &= 0x7f;		//波特率不倍速
	SCON = 0x50;		//8位数据,可变波特率
	AUXR |= 0x40;		//定时器1时钟为Fosc,即1T
	AUXR &= 0xfe;		//串口1选择定时器1为波特率发生器
	TMOD &= 0x0f;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xFE;		//设定定时初值
	TH1 = 0xFE;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1

	//EA = 1;       //打开总中断
	//ES = 1;       //打开串口中断
	ES=0;          //关闭串口中断
}


/*-----------
发送一个字节
-----------*/
void serialPortSendByte( unsigned char dat )
{
	//ES=0; //关闭串口中断
	SBUF = dat;
	while(!TI); //等待发送完毕
	TI = 0;  //清发送完毕标志
	//ES=1;  //开启串口中断
}

//void SendStr(unsigned char *s)
//{
// while(*s!='\0')
//  {
//  SendByte(*s);
//  s++;
//  }
//}