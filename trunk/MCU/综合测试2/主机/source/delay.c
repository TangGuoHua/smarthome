/*
All rights are reserved.
Please contact huangchanghao@gmail.com if any questions

When        Who        Remarks
--------------------------------------
2011-OCT-05 Changhao   Initial version
*/


#include "delay.h"

/***************************��ʱ����****************/
void delay( uint t )
{
	uint x,y;
	for(x=t;x>0;x--)
		for(y=10;y>0;y--);
}


// ��ʱ100us��11.0592M����
void delay100us( uint t )
{
	byte i;
	for(;t>0;t--)
		for(i=0;i<23;i++);
}

// ��ʱ10us��11.0592M����
void delay10us(void)
{
	byte i;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
}

// ��ʱ1ms, 11.0592Mhz����
void delayMS( uint t )
{
	uint i;
	for( ; t>0; t-- )
		for( i=114; i>0; i-- );
	
}