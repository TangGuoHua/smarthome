/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/

#ifndef __HDT11_H__
#define __HDT11_H__

//----------------------------------------------//
//----------------IO�ڶ�����--------------------//
//----------------------------------------------//
sbit  DHT11  = P3^5 ;

// ��������
byte getHumidity(bit);
void initDHT11(void);
#endif