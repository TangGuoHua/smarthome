/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#ifndef __HDT11_H__
#define __HDT11_H__

//----------------------------------------------//
//----------------IO口定义区--------------------//
//----------------------------------------------//
sbit  DHT11  = P2^7 ;

// 函数定义
unsigned char getHumidity(bit);
void initDHT11(void);
#endif