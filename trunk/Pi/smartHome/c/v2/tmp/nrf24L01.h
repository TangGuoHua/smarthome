/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2014年08月20日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#ifndef __NRF24L01_H__
#define __NRF24L01_H__

// ***** 相关函数声明 **********
void nrf24L01Init( void );
unsigned char nrfIsDataReceived(void);
unsigned char* nrfGetReceivedData( void );
void nrfSetRxMode(  unsigned char rfChannel, unsigned char addrWidth, unsigned char *rxAddr);
unsigned char nrfSendData( unsigned char rfChannel, unsigned char rfPower, unsigned char maxRetry, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData );
unsigned char nrfDataAvailable();
unsigned char* nrfGetOneDataPacket();
#endif
