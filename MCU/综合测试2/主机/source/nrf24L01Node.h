/*
All rights are reserved.
Please contact huangchanghao@gmail.com if any questions

When        Who        Remarks
--------------------------------------
2011-OCT-05 Changhao   Initial version
*/

#ifndef __NRF24L01NODE_H__
#define __NRF24L01NODE_H__
#include "dataType.h"

#define RECEIVE_DATA_WIDTH 15 //本节点的接收的数据宽度 15字节
#define RECEIVE_RF_CHANNEL 96 //本节点的接收频道号
#define ADDRESS_WIDTH 3 //发送和接收的地址宽度 3字节

static byte MY_ADDR[ADDRESS_WIDTH] = { 53, 69, 149 }; //本节点地址


// ***** 相关函数声明 **********
void nrf24L01Init( void );
void nrfSendData( byte rfChannel, byte addrWidth, byte *txAddr, byte dataWidth, byte *txData );
void nrfSetRxMode( void );
byte* nrfGetReceivedData( void );

#endif