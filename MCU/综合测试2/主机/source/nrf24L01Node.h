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

#define RECEIVE_DATA_WIDTH 15 //���ڵ�Ľ��յ����ݿ�� 15�ֽ�
#define RECEIVE_RF_CHANNEL 96 //���ڵ�Ľ���Ƶ����
#define ADDRESS_WIDTH 3 //���ͺͽ��յĵ�ַ��� 3�ֽ�

static byte MY_ADDR[ADDRESS_WIDTH] = { 53, 69, 149 }; //���ڵ��ַ


// ***** ��غ������� **********
void nrf24L01Init( void );
void nrfSendData( byte rfChannel, byte addrWidth, byte *txAddr, byte dataWidth, byte *txData );
void nrfSetRxMode( void );
byte* nrfGetReceivedData( void );

#endif