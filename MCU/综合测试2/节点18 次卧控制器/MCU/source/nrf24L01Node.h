#ifndef __NRF24L01NODE_H__
#define __NRF24L01NODE_H__

#define RECEIVE_DATA_WIDTH 4 //本节点的接收的数据宽度 (0-32字节)

// ***** 相关函数声明 **********
void nrf24L01Init( void );
unsigned char nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData );
void nrfSetRxMode(  unsigned char rfChannel, unsigned char addrWidth, unsigned char *rxAddr);
unsigned char* nrfGetReceivedData( void );

#endif