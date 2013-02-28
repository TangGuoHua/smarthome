/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include "delay.h"

/***************************延时函数****************/
void delay( uint t )
{
	uint x,y;
	for(x=t;x>0;x--)
		for(y=80;y>0;y--);  // 1T MCU @11.0592MHz y=120; 12T MCU @ 11.0592MHz y=10; 
		// 1T MCU @6.7 内容RC振荡 y=80
}


//// 延时100us，11.0592M晶体
//void delay100us( uint t )
//{
//	byte i;
//	for(;t>0;t--)
//		for(i=0;i<23;i++);
//}
//
//// 延时10us，11.0592M晶体
//void delay10us(void)
//{
//	byte i;
//	i++;
//	i++;
//	i++;
//	i++;
//	i++;
//	i++;
//}
//
//void delayMS( uint t )
//{
//	uint i;
//	for( ; t>0; t-- )
//		for( i=114; i>0; i-- );
//	
//}