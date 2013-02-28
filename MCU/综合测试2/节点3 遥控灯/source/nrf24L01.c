/*****************************************
��������־ǿ
ʱ�䣺2010.6.12
���ܣ�NRF24L01��Ƶģ��C�ļ�(�ڵ㷢�䲿��)
*****************************************/
#include <reg52.h>
#include "nrf24L01.h"
#include "nrf24L01Node.h"
#include "delay.h"

//sbit CE=P2^0;  //RX/TXģʽѡ���
//sbit IRQ=P2^5; //�������ж϶�
//sbit CSN=P2^3; //SPIƬѡ��//����SS
//sbit MOSI=P2^4; //SPI��������ӻ������
//sbit MISO=P2^2; //SPI��������ӻ������
//sbit SCLK=P2^1; //SPIʱ�Ӷ�

//extern byte bdata sta;
/*********************************************************/



/*****************״̬��־*****************************************/
byte bdata sta;   //״̬��־
sbit RX_DR=sta^6;
sbit TX_DS=sta^5;
sbit MAX_RT=sta^4;


byte nrfACK();
byte nrfCheckACK();
byte nrfSPI( byte spiData );

byte nrfReadReg(  byte regAddr );
byte nrfWriteReg( byte regAddr, byte writeData);
byte nrfReadRxData( byte regAddr, byte *rxData, byte dataLen);
byte nrfWriteTxData(byte regAddr, byte *txData, byte dataLen);


//********* ���º���������ģ���ڲ����� **************

//SPIʱ����
byte nrfSPI( byte spiData )
{
    byte i;
   	for(i=0;i<8;i++) //ѭ��8��
   	{
	  if( spiData & 0x80 )
	    MOSI=1;
	  else
	    MOSI=0; //byte���λ�����MOSI
   	  spiData<<=1; //��һλ��λ�����λ
   	  SCLK=1; 
	  if(MISO) //����SCK��nRF24L01��MOSI����1λ���ݣ�ͬʱ��MISO���1λ����
   	    spiData|=0x01; //��MISO��byte���λ
   	  SCLK=0; //SCK�õ�
   	}
    return(spiData); //���ض�����һ�ֽ�
}

//SPI���Ĵ���һ�ֽں���
byte nrfReadReg( byte regAddr )
{
	byte returnData;
	CSN=0;//����ʱ��
	nrfSPI( regAddr );//д�Ĵ�����ַ
	returnData=nrfSPI(0x00);//д����Ĵ���ָ��  
	CSN=1;
	return( returnData ); //����״̬
}

//SPIд�Ĵ���һ�ֽں���
byte nrfWriteReg( byte regAddr, byte writeData )
{
	byte returnData;
	CSN=0;//����ʱ��
	returnData=nrfSPI(regAddr);//д���ַ
	nrfSPI(writeData);//д��ֵ
	CSN=1;
	return(returnData);
}

//SPI��ȡRXFIFO�Ĵ�����ֵ
byte nrfReadRxData( byte regAddr, byte *rxData, byte dataLen )
{  //�Ĵ�����ַ//��ȡ���ݴ�ű���//��ȡ���ݳ���//���ڽ���
	byte returnData,i;
	CSN=0;//����ʱ��
	returnData=nrfSPI(regAddr);//д��Ҫ��ȡ�ļĴ�����ַ
	for(i=0;i<dataLen;i++) //��ȡ����
	{
		rxData[i]=nrfSPI(0);
	} 
	CSN=1;
	return(returnData); 
}


//SPIд��TXFIFO�Ĵ�����ֵ
byte nrfWriteTxData( byte regAddr, byte *txData, byte dataLen )
{ //�Ĵ�����ַ//д�����ݴ�ű���//��ȡ���ݳ���//���ڷ���
	byte returnData,i;
	CSN=0;
	returnData=nrfSPI(regAddr);//д��Ҫд��Ĵ����ĵ�ַ
	for(i=0;i<dataLen;i++)//д������
	{
		nrfSPI(*txData++);
	}
	CSN=1;
	return(returnData);
}



//******* ���º������ⲿģ����� ************

//NRF24L01��ʼ������
void nrf24L01Init()
{
	delay(2); //��ϵͳʲô������
	CE=0; //����ģʽ1 (Standy-I)
	CSN=1;
	SCLK=0;
	IRQ=1;
}


// ��24L01����Ϊ����ģʽPTX������������
// �������£�
// rfChannel: ���ߵ�Ƶ��  ȡֵ��Χ��0-125������0x00��0x7d��
// addrWidth: ���͵�ַ���
// txAddr: ���͵ĵ�ַ�����շ���ַ��
// dataWidth: �������ݵĿ��
// txData: ���͵�����
// ���ӣ�
// byte rfChannel = 0x64; //ѡ�����ߵ�Ƶ��
// byte rec_addr[3]= { 0x54, 0x53, 0x95 };  //���շ���ַ
// byte data_to_send[5] = {0x01, 0x02, 0x03, 0x04, 0x05 };��//Ҫ���͵�����
// nrfSendData( rfChannel, 3, rec_addr, 5, data_to_send );  //����
//
void nrfSendData( byte rfChannel, byte addrWidth, byte *txAddr, byte dataWidth, byte *txData )
{
	CE=0; 
	nrfWriteTxData( W_REGISTER+TX_ADDR, txAddr, addrWidth );//д�Ĵ���ָ��+���յ�ַʹ��ָ��+���յ�ַ+��ַ���
	nrfWriteTxData( W_REGISTER+RX_ADDR_P0, txAddr,addrWidth );//Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
	nrfWriteTxData( W_TX_PAYLOAD, txData, dataWidth );//д������ 
	/******�����йؼĴ�������**************/
	nrfWriteReg( W_REGISTER+EN_AA, 0x01 );       // ʹ�ܽ���ͨ��0�Զ�Ӧ��
	nrfWriteReg( W_REGISTER+EN_RXADDR, 0x01 );   // ʹ�ܽ���ͨ��0
	
	//NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // �Զ��ط���ʱ�ȴ�250us+86us���Զ��ط�10��
	nrfWriteReg( W_REGISTER+SETUP_RETR,0x7a );  // �Զ��ط���ʱ�ȴ�2000us+86us���Զ��ط�10��
	nrfWriteReg( W_REGISTER+RF_CH, rfChannel ); // ѡ����Ƶͨ��0x64
	
	nrfWriteReg( W_REGISTER+RF_SETUP,0x26 ); // ���ݴ�����250Kbps�����书��0dBm
	nrfWriteReg( W_REGISTER+CONFIG,0x7e ); //����3���жϣ�CRCʹ�ܣ�2�ֽ�CRCУ�飬�ϵ磬PTX
	CE=1;
	delay(5);//����10us������
	CE=0; //��������Ϻ�תΪStandby-1ģʽ
	
	while(nrfCheckACK());	//����Ƿ������
}


// ����24L01Ϊ����ģʽPRX׼����������
// ���ڵ�Ľ�����Ƶͨ������ַ����ַ��ȡ����ݿ�Ⱦ���"nrf24L01Node.h"ͷ�ļ���������
void nrfSetRxMode( void )
{
    CE=0;
  	nrfWriteTxData( W_REGISTER+RX_ADDR_P0, MY_ADDR, ADDRESS_WIDTH ); //�����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ
  	nrfWriteReg( W_REGISTER+EN_AA, 0x01 ); //ʹ�ܽ���ͨ��0�Զ�Ӧ��
  	nrfWriteReg( W_REGISTER+EN_RXADDR, 0x01 ); //ʹ�ܽ���ͨ��0

	nrfWriteReg( W_REGISTER+RF_CH, RECEIVE_RF_CHANNEL ); //������Ƶͨ��
  	nrfWriteReg( W_REGISTER+RX_PW_P0, RECEIVE_DATA_WIDTH ); //����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��

	nrfWriteReg( W_REGISTER+RF_SETUP, 0x26 ); // ���ݴ�����250Kbps�����书��0dBm
	nrfWriteReg( W_REGISTER+CONFIG, 0x3f ); //ʹ��RX_DR�жϣ�����TX_DS��MAX_RT�жϣ�CRCʹ�ܣ�2�ֽ�CRCУ�飬�ϵ磬����ģʽ
  	CE = 1; 
}

/****************************���Ӧ���ź�******************************/
byte nrfCheckACK()
{  //���ڷ���
	sta=nrfReadReg(R_REGISTER+STATUS);                    // ����״̬�Ĵ���
	if(TX_DS||MAX_RT) //��������ж�
	{
	   nrfWriteReg(W_REGISTER+STATUS,0xff);  // ���TX_DS��MAX_RT�жϱ�־
	   CSN=0;
	   nrfSPI(FLUSH_TX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����  
       CSN=1; 
	   return(0);
	}
	else
	   return(1);
}


//��ȡ24L01���յ������ݡ�
//��24L01�յ����ݴ����жϺ󣬵��ñ�������ȡ��24L01�յ�������
//byte* nrfGetReceivedData()				 
//{
//	static byte dataBuffer[RECEIVE_DATA_WIDTH];
//	
//	sta=nrfReadReg(R_REGISTER+STATUS);//�������ݺ��ȡ״̬�Ĵ���
//	if(RX_DR)				// �ж��Ƿ���յ�����
//	{
//		CE=0;//����Standby-Iģʽ
//		
//		nrfReadRxData(R_RX_PAYLOAD,dataBuffer,RECEIVE_DATA_WIDTH);// ��RXFIFO��ȡ����
//		
//		//���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�
//		//nrfWriteReg(W_REGISTER+STATUS,0xff);
//		//�����ҵ�����������0xff�����жϱ�־����ͨ�����鷢����0x40Ҳ�������RX_DR
//		nrfWriteReg(W_REGISTER+STATUS,0x40);
//		
//		//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����
//		//�Ҿ������鷢�֣�����FIFO�Ļ������ͷ���ʱ�������ղ���ACK������
//		CSN=0;
//		nrfSPI(FLUSH_RX); 
//		CSN=1;
//	}
//	return dataBuffer;
//} 

//��ȡ24L01���յ������ݡ�
//��24L01�յ����ݴ����жϺ󣬵��ñ�������ȡ��24L01�յ�������
byte* nrfGetReceivedData()			 
{
	static byte dataBuffer[RECEIVE_DATA_WIDTH];
	
	sta=nrfReadReg(R_REGISTER+STATUS);//�������ݺ��ȡ״̬�Ĵ���
	if(RX_DR)				// �ж��Ƿ���յ�����
	{
		CE=0;//����Standby-Iģʽ
		
		nrfReadRxData(R_RX_PAYLOAD,dataBuffer,RECEIVE_DATA_WIDTH);// ��RXFIFO��ȡ����
		
		//���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�
		//nrfWriteReg(W_REGISTER+STATUS,0xff);
		//�����ҵ�����������0xff�����жϱ�־����ͨ�����鷢����0x40Ҳ�������RX_DR
		nrfWriteReg(W_REGISTER+STATUS,0x40);
		
		//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����
		//�Ҿ������鷢�֣�����FIFO�Ļ������ͷ���ʱ�������ղ���ACK������
		CSN=0;
		nrfSPI(FLUSH_RX); 
		CSN=1;
	}
	return dataBuffer;
}