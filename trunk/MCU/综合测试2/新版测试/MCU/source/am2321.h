#ifndef __AM2321_H__
#define __AM2321_H__

//----------------------------------------------//
//----------------IO�ڶ�����--------------------//
//----------------------------------------------//
sbit  AM2321_SDA = P1^7;


// ��������
void initAM2321(void);
unsigned char readAM2321(); //readAM2321( unsigned char mode);
unsigned char getAM2321Data( unsigned char index );
#endif
