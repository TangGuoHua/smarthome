#ifndef __AM2301_H__
#define __AM2301_H__

//----------------------------------------------//
//----------------IO口定义区--------------------//
//----------------------------------------------//
sbit  AM2301_SDA = P1^6;


// 函数定义
void initAM2301(void);
unsigned char readAM2301(unsigned char mode);
unsigned char getAM2301Data( unsigned char index );
#endif
