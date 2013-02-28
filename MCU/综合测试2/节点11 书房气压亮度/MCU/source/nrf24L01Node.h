#ifndef __NRF24L01NODE_H__
#define __NRF24L01NODE_H__

#define RECEIVE_DATA_WIDTH 2 //本节点的接收的数据宽度 (0-32字节)
#define RECEIVE_RF_CHANNEL 92 //本节点的接收频道号
#define HOST_RF_CHANNEL 96  //主机的接收频道号
#define HOST_DATA_WIDTH 15  //主机接收的数据宽度 (0-32字节)
#define ADDRESS_WIDTH 3 //发送和接收的地址宽度 (3-5字节)  发送和接收都使用相同地址宽度
#define NODE_ID_0 100 //NODE ID 0
#define NODE_ID_1 11 //NODE ID 1

static unsigned char MY_ADDR[ADDRESS_WIDTH] = {97, 83, 168 }; //本节点地址
static unsigned char HOST_ADDR[ADDRESS_WIDTH]= { 53, 69, 149 }; //主机地址
//static unsigned char dataBuffer[RECEIVE_DATA_WIDTH] ;

// ***** 相关函数声明 **********
void nrf24L01Init( void );
void nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData );
void nrfSetRxMode( void );
unsigned char* nrfGetReceivedData( void );

#endif