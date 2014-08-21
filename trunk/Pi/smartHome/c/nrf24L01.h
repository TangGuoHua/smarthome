

#ifndef __NRF24L01_H__
#define __NRF24L01_H__

// ***** 相关函数声明 **********
void nrf24L01Init( void );
unsigned char nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData );
void nrfSetRxMode(  unsigned char rfChannel, unsigned char addrWidth, unsigned char *rxAddr);
unsigned char* nrfGetReceivedData( void );
unsigned char nrfIsDataReceived(void);
#endif