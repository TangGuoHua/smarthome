#ifndef __NRF24L01NODE_H__
#define __NRF24L01NODE_H__

#define RECEIVE_DATA_WIDTH 2 //���ڵ�Ľ��յ����ݿ�� (0-32�ֽ�)
#define RECEIVE_RF_CHANNEL 92 //���ڵ�Ľ���Ƶ����
#define HOST_RF_CHANNEL 96  //�����Ľ���Ƶ����
#define HOST_DATA_WIDTH 15  //�������յ����ݿ�� (0-32�ֽ�)
#define ADDRESS_WIDTH 3 //���ͺͽ��յĵ�ַ��� (3-5�ֽ�)  ���ͺͽ��ն�ʹ����ͬ��ַ���
#define NODE_ID_0 100 //NODE ID 0
#define NODE_ID_1 11 //NODE ID 1

static unsigned char MY_ADDR[ADDRESS_WIDTH] = {97, 83, 168 }; //���ڵ��ַ
static unsigned char HOST_ADDR[ADDRESS_WIDTH]= { 53, 69, 149 }; //������ַ
//static unsigned char dataBuffer[RECEIVE_DATA_WIDTH] ;

// ***** ��غ������� **********
void nrf24L01Init( void );
void nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData );
void nrfSetRxMode( void );
unsigned char* nrfGetReceivedData( void );

#endif