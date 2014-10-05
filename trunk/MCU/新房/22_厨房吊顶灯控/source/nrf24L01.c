/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2013��10��01��  �Ƴ���  ��ʼ�汾
2014��10��01��  �Ƴ���  �޸�nrfSendData()������֧�ֵ�ַ������úͷ���ʧ�ܺ�����Retry
                        �޸�nrfSetRxMode()������֧�ֵ�ַ�������

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include "nrf24L01.h"
#include "nrf24L01Node.h"

#define SEND_MAX_RETRY 5  //��������ʱ�����ʧ�ܣ�������Դ��� (ֵ����Ϊ 0��9��

unsigned char nrfACK();
unsigned char nrfCheckACK();
unsigned char nrfSPI( unsigned char spiData );

unsigned char nrfReadReg(  unsigned char regAddr );
unsigned char nrfWriteReg( unsigned char regAddr, unsigned char writeData);
unsigned char nrfReadRxData( unsigned char regAddr, unsigned char *rxData, unsigned char dataLen);
unsigned char nrfWriteTxData(unsigned char regAddr, unsigned char *txData, unsigned char dataLen);

/****************************************************
��������:��ʱ 15us
*****************************************************/
void delayFor24L01( )
{
	unsigned int i;
	unsigned int t;
	for( t=1; t>0; t-- )
		for( i=305; i>0; i-- ); // 1T STC11F04E �ⲿ����4MHz
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
	CSN=1;
	SCLK=0;
	IRQ=1;
	
	/***������Щ�Ĵ��������ã���������������ڼ䲻�仯�����ڳ�ʼ��оƬʱ���á�***/
	nrfWriteReg( W_REGISTER+EN_AA, 0x01 );     // ʹ�ܽ���ͨ��0�Զ�Ӧ��
	nrfWriteReg( W_REGISTER+EN_RXADDR, 0x01 ); // ʹ�ܽ���ͨ��0

	//nrfWriteReg( W_REGISTER+SETUP_AW, 0x03 ); // Set up address width to 5 bytes
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0x1f ); // �Զ��ط���ʱ�ȴ�500us���Զ��ط�15��
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0x3f ); // �Զ��ط���ʱ�ȴ�1000us���Զ��ط�15��
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0x5f ); // �Զ��ط���ʱ�ȴ�1500us���Զ��ط�15��
	nrfWriteReg( W_REGISTER+SETUP_RETR,0x7f ); // �Զ��ط���ʱ�ȴ�2000us���Զ��ط�15��
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0xbf ); // �Զ��ط���ʱ�ȴ�3000us���Զ��ط�15��
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0xff ); // �Զ��ط���ʱ�ȴ�4000us���Զ��ط�15��

	nrfWriteReg( W_REGISTER+STATUS, 0x7e ); //���RX_DR,TX_DS,MAX_RT״̬λ
	nrfWriteReg( W_REGISTER+CONFIG, 0x7e ); //����3���жϣ�CRCʹ�ܣ�2�ֽ�CRCУ�飬�ϵ磬PTX
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
//
// ����ֵ��
// 255-��ʾ�����Դ����ﵽ����Ȼδ�յ�ACK������ʧ��
// 0��159��һ��ֵ����ʾ��������ҳɹ�������ֵ���Զ��ش�(auto retransmit)�Ĵ��������磺
//     ����0��û��re-transmit��ֱ�ӷ��ͳɹ�
//     ����1: ��ʾоƬre-transmit 1�γɹ�
//     ����2����ʾоƬre-transmit 2�γɹ�
//     �Դ����ƣ����re-transmit 159�γɹ���(SEND_MAX_RETRY��������Ϊ9��
unsigned char nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData )
{
	unsigned char ret = 0;
	unsigned char retryCnt = 0;
	
	CE=0;
	
	nrfWriteReg( W_REGISTER+SETUP_AW, addrWidth-2 ); //���õ�ַ���
	nrfWriteTxData( W_REGISTER+TX_ADDR, txAddr, addrWidth );//д�Ĵ���ָ��+���յ�ַʹ��ָ��+���յ�ַ+��ַ���
	nrfWriteTxData( W_REGISTER+RX_ADDR_P0, txAddr,addrWidth );//Ϊ��Ӧ������豸������ͨ��0��ַ�ͷ��͵�ַ��ͬ
	 

	nrfWriteReg( W_REGISTER+RF_CH, rfChannel ); // ѡ����Ƶͨ��
	nrfWriteReg( W_REGISTER+CONFIG,0x7e ); //����3���жϣ�CRCʹ�ܣ�2�ֽ�CRCУ�飬�ϵ磬PTX
	
	for( retryCnt=0; retryCnt<= SEND_MAX_RETRY; retryCnt++ )
	{
		nrfWriteTxData( W_TX_PAYLOAD, txData, dataWidth );//д��Ҫ���͵�����
		
		CE=1;
		delayFor24L01();
		CE=0; //��������Ϻ�תΪStandby-1ģʽ
	
		do
		{
			ret=nrfCheckACK();
		}while( ret==100);//����Ƿ������
		
		//�������ֵС�ڵ���15��˵���������ݴ��ͳɹ�
		if( ret<=15 ) 
		{
			//���㺯������ֵ����Retransmission�Ĵ���
			//һ���Է��ͳɹ�������0����û���ش�(re-transmission)
			//retry��1��ʱ��һ���Է��ͳɹ�����retransit 16�� ��֮ǰ15�μӱ��Σ�
			//һ�����ƣ�
			//retry��9��ʱ�����ӿ�ʼretry֮ǰ��һ�Σ���������10�Σ���retransmit 15�γɹ������ܹ�retransmit��16*9+15=159��
			//ÿ��try��transmit 16�� ������1�Σ���15��retransmit��
			ret += (16*retryCnt);
			break;
		}
	}
	
	return ret;
}


//����24L01Ϊ����ģʽPRX��׼����������
//�������£�
//rfChannel�����յ�Ƶ���š�ȡֵ��Χ��0��125
//addrWidth: ��ַ��ȣ�3-5�ֽڣ�
//rxAddr�����ڵ���յĵ�ַ
//���ڵ�Ľ������ݿ����"nrf24L01Node.h"ͷ�ļ���������
//����
//   unsigned char myAddr[3] = {53, 69, 160};
//   nrfSetRxMode( 76, 3, myAddr);
//��ô�ڵ㽫��76Ƶ���Ͻ������ݡ���ַ���Ϊ3�ֽڣ���ַ�ǣ�53/69/160��
//һ�����յ����ݣ�������INT0 (Ӳ��������ʾ��IRQ�����ӵ�INT0�ϣ�
void nrfSetRxMode(  unsigned char rfChannel, unsigned char addrWidth, unsigned char *rxAddr)
{
    CE=0;

	nrfWriteReg( W_REGISTER+SETUP_AW, addrWidth-2 ); //���õ�ַ���
  	nrfWriteTxData( W_REGISTER+RX_ADDR_P0, rxAddr, addrWidth ); //�����豸����ͨ��0ʹ�úͷ����豸��ͬ�ķ��͵�ַ


	nrfWriteReg( W_REGISTER+RF_CH, rfChannel ); //������Ƶͨ��
  	nrfWriteReg( W_REGISTER+RX_PW_P0, RECEIVE_DATA_WIDTH ); //����ͨ��0ѡ��ͷ���ͨ����ͬ��Ч���ݿ��

	//nrfWriteReg( W_REGISTER+RF_SETUP, 0x26 ); // ���ݴ�����250Kbps�����书��0dBm
	nrfWriteReg( W_REGISTER+RF_SETUP, 0x27 ); // ���ݴ�����250Kbps�����书��0dBm, LNA_HCURR (Low Noise Amplifier, High Current?)
	//nrfWriteReg( W_REGISTER+RF_SETUP,0x21 ); // ���ݴ�����250Kbps�����书��-18dBm, LNA_HCURR (Low Noise Amplifier, High Current?)
	nrfWriteReg( W_REGISTER+CONFIG, 0x3f ); //ʹ��RX_DR�жϣ�����TX_DS��MAX_RT�жϣ�CRCʹ�ܣ�2�ֽ�CRCУ�飬�ϵ磬����ģʽ

  	CE = 1;	//��Ϊ����ģʽ PRX
}


// ���ڼ�鷢�ͽ��(Ack)
// ����ֵ��100-��ʾ���ڷ�����
//         255-��ʾ���ط������ﵽ����Ȼδ�յ�ACK������ʧ��
//         0��15��һ��ֵ����ʾ��������ҳɹ�������ֵ���Զ��ط��Ĵ��������磺
//           0��û���ط���ֱ�ӷ��ͳɹ�
//           1: �ط���1�κ�ɹ��յ�ack
//           2: �ط���2�κ�ɹ��յ�ack
//           �Դ�����
//           ���ֵ��SETUP_RETR����Ĵ����������õ�����ط������������ᳬ��15��
unsigned char nrfCheckACK()
{  
	unsigned char status;
	
	status = nrfReadReg(R_REGISTER+STATUS); // ��ȡ״̬�Ĵ���
	
	if( status & 0x20 ) //���TX_DSλ����λ���ͳɹ�
	{
		nrfWriteReg(W_REGISTER+STATUS,0x7f);  // ���TX_DS��־
		
		//���ͳɹ���FIFO�Զ���գ�����Ͳ���������
		
		//�����Զ��ط�����
		return( nrfReadReg(R_REGISTER+OBSERVE_TX) & 0x0f ); 
	}
	else if( status & 0x10) //���MAX_RTλ����λ��˵������ط������ﵽ����Ȼδ�յ�ACK������ʧ��
	{
		nrfWriteReg(W_REGISTER+STATUS,0x7f);  // ���MAX_RT��־
		
		//����ʧ�ܣ�FIFO�����Զ���գ������ֶ���� ����
		//�ؼ�������Ȼ��������벻���ĺ��������
		CSN=0;
		nrfSPI(FLUSH_TX);
		CSN=1; 
			
		return 255;
	}
	else //���ڷ�����...
	{
		return 100;
	}
}

//��ȡ24L01���յ������ݡ�
//��24L01�յ����ݴ����жϺ󣬵��ñ�������ȡ��24L01�յ�������
unsigned char* nrfGetReceivedData()				 
{
	static unsigned char dataBuffer[RECEIVE_DATA_WIDTH];
	unsigned char status;
	
	//��ȡ״̬�Ĵ���
	status = nrfReadReg(R_REGISTER+STATUS);
	if( status & 0x40 ) //���RX_DRλ�������λ����˵�����յ�����
	{
		//CE=0;//����Standby-Iģʽ
		
		// ��RX FIFO��ȡ����
		nrfReadRxData(R_RX_PAYLOAD,dataBuffer,RECEIVE_DATA_WIDTH);
		
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