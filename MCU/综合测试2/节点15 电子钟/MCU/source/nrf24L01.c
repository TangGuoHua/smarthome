/*****************************************
//STC12c5608ad 4M External Crystal
*****************************************/
#include <reg52.h>
#include "nrf24L01.h"
#include "nrf24L01Node.h"



//extern unsigned char bdata sta;
/*********************************************************/



/*****************״̬��־****************/
unsigned char bdata sta;   //״̬��־
sbit RX_DR=sta^6;
sbit TX_DS=sta^5;
sbit MAX_RT=sta^4;

unsigned char nrfACK();
unsigned char nrfCheckACK();
unsigned char nrfSPI( unsigned char spiData );

unsigned char nrfReadReg(  unsigned char regAddr );
unsigned char nrfWriteReg( unsigned char regAddr, unsigned char writeData);
unsigned char nrfReadRxData( unsigned char regAddr, unsigned char *rxData, unsigned char dataLen);
unsigned char nrfWriteTxData(unsigned char regAddr, unsigned char *txData, unsigned char dataLen);


/****************************************************
��������:��ʱ 15us
����Ϊ�ڲ�RC 6.9MHz(STC-ISP��ʾ��Ƶ��)  STC11F04E 1T
*****************************************************/
//void delayFor24L01()
//{  
//
//    unsigned char a;
//    for(a=24;a>0;a--);
//    _nop_();  //if Keil,require use intrins.h
//} 


void delayFor24L01( )
{
	unsigned int i;
	unsigned int t;
	for( t=1; t>0; t-- )
		for( i=530; i>0; i-- ); // 1T STC11F04E �ڲ�RC 6.97MHz
}
//********* ���º���������ģ���ڲ����� **************

//SPIʱ����
unsigned char nrfSPI( unsigned char spiData )
{
    unsigned char i;
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
unsigned char nrfReadReg( unsigned char regAddr )
{
	unsigned char returnData;
	CSN=0;//����ʱ��
	nrfSPI( regAddr );//д�Ĵ�����ַ
	returnData=nrfSPI(0x00);//д����Ĵ���ָ��  
	CSN=1;
	return( returnData ); //����״̬
}

//SPIд�Ĵ���һ�ֽں���
unsigned char nrfWriteReg( unsigned char regAddr, unsigned char writeData )
{
	unsigned char returnData;
	CSN=0;//����ʱ��
	returnData=nrfSPI(regAddr);//д���ַ
	nrfSPI(writeData);//д��ֵ
	CSN=1;
	return(returnData);
}

//SPI��ȡRXFIFO�Ĵ�����ֵ
unsigned char nrfReadRxData( unsigned char regAddr, unsigned char *rxData, unsigned char dataLen )
{  //�Ĵ�����ַ//��ȡ���ݴ�ű���//��ȡ���ݳ���//���ڽ���
	unsigned char returnData,i;
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
unsigned char nrfWriteTxData( unsigned char regAddr, unsigned char *txData, unsigned char dataLen )
{ //�Ĵ�����ַ//д�����ݴ�ű���//��ȡ���ݳ���//���ڷ���
	unsigned char returnData,i;
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
	delayFor24L01();//��ϵͳʲô������
	delayFor24L01();//��ϵͳʲô������
	CE=0; //����ģʽ1 (Standy-I)
	CE=0;CE=0;CE=0;CE=0;CE=0;CE=0;
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
// unsigned char rfChannel = 0x64; //ѡ�����ߵ�Ƶ��
// unsigned char rec_addr[3]= { 0x54, 0x53, 0x95 };  //���շ���ַ
// unsigned char data_to_send[5] = {0x01, 0x02, 0x03, 0x04, 0x05 };��//Ҫ���͵�����
// nrfSendData( rfChannel, 3, rec_addr, 5, data_to_send );  //����
void nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData )
{
	CE=0;
	//delayUS( 10 ); // ���������ʵ��ó� STC12c5608AD 1T 11.0592MHz
	delayFor24L01();

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
	//delay10us();//����10us������
	//delay10us();
	//delayUS( 250 );  //���������ʵ��ó� STC12c5608AD 1T 11.0592MHz
	delayFor24L01();


	CE=0; //��������Ϻ�תΪStandby-1ģʽ

	delayFor24L01(); //���������ʵ��ó� STC12c5608AD 1T 11.0592MHz
	//delayFor24L01();

	
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

  	CE = 1;	//��Ϊ����ģʽ PRX


}

/****************************���Ӧ���ź�******************************/
unsigned char nrfCheckACK()
{  //���ڷ���
	sta=nrfReadReg(R_REGISTER+STATUS); // ����״̬�Ĵ���
	if(TX_DS||MAX_RT) //��������ж�
	{
		nrfWriteReg(W_REGISTER+STATUS,0xff);  // ���TX_DS��MAX_RT�жϱ�־
		CSN=0;
		nrfSPI(FLUSH_TX);//�������FIFO �����ؼ�������Ȼ��������벻���ĺ����������Ҽ�ס����  
		CSN=1; 
		return(0);
	}
	else
	{
		return(1);
	}
}


//��ȡ24L01���յ������ݡ�
//��24L01�յ����ݴ����жϺ󣬵��ñ�������ȡ��24L01�յ�������
unsigned char* nrfGetReceivedData()				 
{
	static unsigned char dataBuffer[RECEIVE_DATA_WIDTH];
	
	sta=nrfReadReg(R_REGISTER+STATUS);//�������ݺ��ȡ״̬�Ĵ���
	if(RX_DR) // �ж��Ƿ���յ�����
	{
		//CE=0;//����Standby-Iģʽ
		
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