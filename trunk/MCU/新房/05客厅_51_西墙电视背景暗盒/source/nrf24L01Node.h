/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014-JUN-29     �Ƴ���  ��ʼ�汾

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#ifndef __NRF24L01NODE_H__
#define __NRF24L01NODE_H__

#define RECEIVE_DATA_WIDTH 9 //���ڵ�Ľ��յ����ݿ�� (1-32�ֽ�)

// ***** ��غ������� **********
void nrf24L01Init( void );
unsigned char nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData );
void nrfSetRxMode(  unsigned char rfChannel, unsigned char addrWidth, unsigned char *rxAddr);
unsigned char* nrfGetReceivedData( void );

#endif