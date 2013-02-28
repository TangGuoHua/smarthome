/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#include <intrins.h>
#include "delay.h"



// 延时100us，11.0592M晶体
void delay100us( unsigned int t )
{
	unsigned char i;
	for(;t>0;t--)
		for(i=0;i<253;i++);  //12T 23; 1T 255
}

// 延时10us，11.0592M晶体
//void delay10us(void)
//{
//	unsigned char i;
//	i++;
//	i++;
//	i++;
//	i++;
//	i++;
//	i++;
//		i++;
//	i++;
//	i++;
//	i++;
//	i++;
//	i++;
//		i++;
//	i++;
//	i++;
//	i++;
//	i++;
//	i++;
//		i++;
//	i++;
//	i++;
//	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
////	i++;
//}

/****************************************************
函数功能:精确延时（（2*n）+1）us
晶振为11.0592M时  STC12C5608AD 1T
*****************************************************/
void delayUS(unsigned char n)
{  
   unsigned char i;
   for(i=0;i<n;i++)
    {
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
    }
} 

void delayMS( unsigned int t )
{
	unsigned int i;
	for( ; t>0; t-- )
		for( i=848; i>0; i-- ); //12T  114;  1T 1368 (i calculated), 1T 848 (got from internet)
	
}