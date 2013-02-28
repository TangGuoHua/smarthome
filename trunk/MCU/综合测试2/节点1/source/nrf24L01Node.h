#ifndef __NRF24L01NODE_H__
#define __NRF24L01NODE_H__


#define RECEIVE_DATA_WIDTH 2 //���ڵ�Ľ��յ����ݿ�� 2�ֽ�
#define RECEIVE_RF_CHANNEL 92 //���ڵ�Ľ���Ƶ����
#define HOST_RF_CHANNEL 96  //�����Ľ���Ƶ����
#define HOST_DATA_WIDTH 15  //�������յ����ݿ�� 15�ֽ�
#define ADDRESS_WIDTH 3 //���ͺͽ��յĵ�ַ��� 3�ֽ�

static byte MY_ADDR[ADDRESS_WIDTH] = {97, 83, 149 }; //���ڵ��ַ
static byte HOST_ADDR[ADDRESS_WIDTH]= { 53, 69, 149 }; //���ڵ��ַ
//static byte dataBuffer[RECEIVE_DATA_WIDTH] ;

// ***** ��غ������� **********
void nrf24L01Init( void );
void nrfSendData( byte rfChannel, byte addrWidth, byte *txAddr, byte dataWidth, byte *txData );
void nrfSetRxMode( void );
byte* nrfGetReceivedData( void );

#endif