/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include "delay.h"



// ��ʱ100us��11.0592M����
void delay100us( unsigned int t )
{
	unsigned char i;
	for(;t>0;t--)
		for(i=0;i<253;i++);  //12T 23; 1T 255
}

// ��ʱ10us��11.0592M����
void delay10us(void)
{
	unsigned char i;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
		i++;
	i++;
	i++;
	i++;
	i++;
	i++;
		i++;
	i++;
	i++;
	i++;
	i++;
	i++;
		i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
	i++;
}

void delayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=1368; i>0; i-- ); //12T  114;  1T 1368
	
}