#ifndef __AM2301_H__
#define __AM2301_H__

//----------------------------------------------//
//----------------IO�ڶ�����--------------------//
//----------------------------------------------//
sbit  AM2301_SDA  = P2^4;


// ��������
unsigned char readAM2301(void);
unsigned char getAM2301Data( unsigned char index );
#endif
