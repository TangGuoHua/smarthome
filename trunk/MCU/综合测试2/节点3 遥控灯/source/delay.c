/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include "delay.h"

/***************************��ʱ����****************/
void delay( uint t )
{
	uint x,y;
	for(x=t;x>0;x--)
		for(y=80;y>0;y--);  // 1T MCU @11.0592MHz y=120; 12T MCU @ 11.0592MHz y=10; 
		// 1T MCU @6.7 ����RC�� y=80
}


//// ��ʱ100us��11.0592M����
//void delay100us( uint t )
//{
//	byte i;
//	for(;t>0;t--)
//		for(i=0;i<23;i++);
//}
//
//// ��ʱ10us��11.0592M����
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