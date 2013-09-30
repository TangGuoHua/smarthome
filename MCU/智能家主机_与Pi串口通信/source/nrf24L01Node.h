/*****************************************
NRF24L01+ 模块头文件
作者保留一切权利。如有问题请联系huangchanghao@gmail.com

All rights are reserved.
Please contact huangchanghao@gmail.com if any questions

Revision History
Date         Author      Remarks
-----------------------------------------------------------
2013-SEP-30  Changhao    整理文件格式
*****************************************/

#ifndef __NRF24L01NODE_H__
#define __NRF24L01NODE_H__

#define RECEIVE_DATA_WIDTH 16 //本节点的接收的数据宽度 (0-32字节)
#define ADDRESS_WIDTH 3 //发送和接收的地址宽度 (3-5字节)  发送和接收都使用相同地址宽度


// ***** 相关函数声明 **********
void nrf24L01Init( void );
unsigned char nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData );
void nrfSetRxMode(  unsigned char rfChannel, unsigned char addrWidth, unsigned char *rxAddr);
unsigned char* nrfGetReceivedData( void );
#endif