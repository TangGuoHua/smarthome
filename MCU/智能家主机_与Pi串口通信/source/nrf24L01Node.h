/*****************************************
NRF24L01+ ģ��ͷ�ļ�
���߱���һ��Ȩ����������������ϵhuangchanghao@gmail.com

All rights are reserved.
Please contact huangchanghao@gmail.com if any questions

Revision History
Date         Author      Remarks
-----------------------------------------------------------
2013-SEP-30  Changhao    �����ļ���ʽ
*****************************************/

#ifndef __NRF24L01NODE_H__
#define __NRF24L01NODE_H__

#define RECEIVE_DATA_WIDTH 16 //���ڵ�Ľ��յ����ݿ�� (0-32�ֽ�)
#define ADDRESS_WIDTH 3 //���ͺͽ��յĵ�ַ��� (3-5�ֽ�)  ���ͺͽ��ն�ʹ����ͬ��ַ���


// ***** ��غ������� **********
void nrf24L01Init( void );
unsigned char nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData );
void nrfSetRxMode(  unsigned char rfChannel, unsigned char addrWidth, unsigned char *rxAddr);
unsigned char* nrfGetReceivedData( void );
#endif