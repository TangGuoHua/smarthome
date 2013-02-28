/*
All rights are reserved.
Please contact huangchanghao@gmail.com if any questions

When        Who        Remarks
--------------------------------------
2011-OCT-05 Changhao   Initial version
*/

#include <reg52.h>
#include "dataType.h"

//Header0��Header1���Ϊһ����ֵ��������ǿϵͳ���ݴ���
//������μ���λ����MCUCommunicator�������ע��

//MCU to PC
//BA9876543210
#define MCU2PC_FRAME_HEADER0 0xba
#define MCU2PC_FRAME_HEADER1 0xba
#define MCU2PC_FRAME_HEADER2 0x98
#define MCU2PC_FRAME_HEADER3 0x76

#define MCU2PC_FRAME_TAILER0 0x54
#define MCU2PC_FRAME_TAILER1 0x32
#define MCU2PC_FRAME_TAILER2 0x10

//PC to MCU
//0123456789AB
#define PC2MCU_FRAME_HEADER0 0x01
#define PC2MCU_FRAME_HEADER1 0x01
#define PC2MCU_FRAME_HEADER2 0x23
#define PC2MCU_FRAME_HEADER3 0x45

#define PC2MCU_FRAME_TAILER0 0x67
#define PC2MCU_FRAME_TAILER1 0x89
#define PC2MCU_FRAME_TAILER2 0xab

//PC to MCU�Ĵ���ͨ�ŵ�״̬
#define COM_STATUS_WAITING_FOR_FRAME_HEADER0 11
#define COM_STATUS_WAITING_FOR_FRAME_HEADER1 22
#define COM_STATUS_WAITING_FOR_FRAME_HEADER2 33
#define COM_STATUS_WAITING_FOR_FRAME_HEADER3 44
#define COM_STATUS_RECEIVING_DATA 55
#define COM_STATUS_WAITING_FOR_FRAME_TAILER1 66
#define COM_STATUS_WAITING_FOR_FRAME_TAILER2 77


//������PCͨ�ŵ�buffer��С (�ֽ�����
//�൱�ڴ�PC������ÿ֡���ݵĴ�С
#define PC2MCU_BUFFER_SIZE 45


/*-------------
����ͨѶ��ʼ��
----------------*/
void serialPortInit(void)
{

	//28800bps@11.0592MHz
	PCON &= 0x7f; //�����ʲ�����
	SCON = 0x50;  //8λ����,�ɱ䲨���� ��ʽ1
	REN = 1;      //�������
	TMOD &= 0x0f; //�����ʱ��1ģʽλ
	TMOD |= 0x20; //�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFF;   //�趨��ʱ��ֵ
	TH1 = 0xFF;   //�趨��ʱ����װֵ
	ET1 = 0;      //��ֹ��ʱ��1�ж�
	//EA = 1;       //�����ж�
	//ES = 1;       //�򿪴����ж�
	ES=0;          //�رմ����ж�
	TR1 = 1;      //������ʱ��1
}

/*-----------
����һ���ֽ�
-----------*/
void serialPortSendByte( byte dat )
{
	//ES=0; //�رմ����ж�
	SBUF = dat;
	while(!TI); //�ȴ��������
	TI = 0;  //�巢����ϱ�־
	//ES=1;  //���������ж�
}
/*-------------
����һ���ַ���
�� '\0'����
----------------*/
void serialPortSendString( unsigned char *s )
{
	while(*s!='\0')
	{
		serialPortSendByte(*s);
		s++;
	}
}


//ͨ����ѯ��ʽ������PC������һ֡����
//���û�յ���ѭ���ȴ���ֱ���յ�һ֡����
//���ؽ��յ�������ָ��
//ע�⣬���յ������ݳ��ȼ��㣺������7��38���ֽ�֮�䣩
//�裺�ֽ�6Ӧ����LEN  (ֵΪ1��32��
//���յ��������ܳ��ȵ��� LEN+6
//
//PC��MCU��ÿ֡��ʽ�̶�Ϊ��
//�ֽ�0���ڵ�ID�ֽ�0
//�ֽ�1���ڵ�ID�ֽ�1
//�ֽ�2���ڵ�24L01����Ƶ��
//�ֽ�3���ڵ�24L01���յ�ַ�ֽ�0
//�ֽ�4���ڵ�24L01���յ�ַ�ֽ�1
//�ֽ�5���ڵ�24L01���յ�ַ�ֽ�2
//�ֽ�6���ڵ�24L01���������ֽ��� (LEN=1��32����Ӧ�ó���1��32�����ֵ)
//�ֽ�7��6+LEN��Ҫ���͸��ڵ������
byte* serialPortReceive(void)
{
	bit stillLoop = 1;
	byte recvByte;
	
	// ͨ��״̬��ʼ��Ϊ�ȴ�header0
	byte comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;

	byte bufferCount = 0;
	static byte buffer[PC2MCU_BUFFER_SIZE];
	//PC��MCU��ÿ֡��ʽ�̶�Ϊ������buffer[]����������������£���
	//�ֽ�0���ڵ�ID�ֽ�0
	//�ֽ�1���ڵ�ID�ֽ�1
	//�ֽ�2���ڵ�24L01����Ƶ��
	//�ֽ�3���ڵ�24L01���յ�ַ�ֽ�0
	//�ֽ�4���ڵ�24L01���յ�ַ�ֽ�1
	//�ֽ�5���ڵ�24L01���յ�ַ�ֽ�2
	//�ֽ�6���ڵ�24L01���������ֽ��� (LEN=1��32����Ӧ�ó���1��32�����ֵ)
	//�ֽ�7��6+LEN��Ҫ���͸��ڵ������
	
	do
	{
		//ѭ���ȴ���ֱ�����ڽ��յ�����
		while(!RI);
		RI=0;
		
		//ȡ���յ���һ���ֽ�
		recvByte = SBUF;
		
		//serialPortSendByte( recvByte );
		
		//serialPortSendByte( comStatus );
		
		switch( comStatus )
		{
			case COM_STATUS_WAITING_FOR_FRAME_HEADER0:  //��ǰ״̬�ǵȴ�header0
				if( recvByte == PC2MCU_FRAME_HEADER0 )  //�յ���header0
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER1; //��ǰ״̬��Ϊ�ȴ�header1
				}
				break;
			
			case COM_STATUS_WAITING_FOR_FRAME_HEADER1:  //��ǰ״̬�ǵȴ�header1
				if( recvByte == PC2MCU_FRAME_HEADER1 )  //�յ���header1
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER2; //��ǰ״̬��Ϊ�ȴ�header2
				}
				else if( recvByte == PC2MCU_FRAME_HEADER2 )  //�յ���header2������һ���ݴ���ƣ��ο���λ�������ע��
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER3; //��ǰ״̬��Ϊ�ȴ�header3
				}
				else
				{
					//״̬�˻صȴ�header0
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
				}
				break;
				
			case COM_STATUS_WAITING_FOR_FRAME_HEADER2:  //��ǰ״̬�ǵȴ�header2
				if( recvByte == PC2MCU_FRAME_HEADER2 )  //�յ���header2
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER3; //��ǰ״̬��Ϊ�ȴ�header3
				}
				else
				{
					//״̬�˻صȴ�header0
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
				}
				break;
				
			case COM_STATUS_WAITING_FOR_FRAME_HEADER3:  //��ǰ״̬�ǵȴ�header3
				if( recvByte == PC2MCU_FRAME_HEADER3 )  //�յ���header3
				{
					comStatus = COM_STATUS_RECEIVING_DATA; //��ǰ״̬��Ϊ��������
					bufferCount = 0; //���buffer
				}
				else
				{
					//״̬�˻صȴ�header0
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
				}
				break;
				
			//���ϰ�֡ͷ�����ˣ����濪ʼ��������
			
			case COM_STATUS_RECEIVING_DATA:  //��ǰ״̬�ǽ�������
				buffer[bufferCount++] = recvByte; //�ѽ��յ����ݷ���buffer
				if( recvByte == PC2MCU_FRAME_TAILER0 )  //�յ���tailer0����Ȼ�п������ɺ�
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_TAILER1; //��ǰ״̬��Ϊ�ȴ�tailer1
				}
				break;
				
			case COM_STATUS_WAITING_FOR_FRAME_TAILER1:  //��ǰ״̬�ǵȴ�tailer1
				buffer[bufferCount++] = recvByte; //�ѽ��յ����ݷ���buffer
				if( recvByte == PC2MCU_FRAME_TAILER1 )  //�յ���tailer1����Ȼ�п������ɺ�
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_TAILER2; //��ǰ״̬��Ϊ�ȴ�tailer2
				}
				else
				{
					comStatus = COM_STATUS_RECEIVING_DATA; //����˵��ǰ����������ɺϣ��ʵ�ǰ״̬��Ϊ��������
				}
				break;

			case COM_STATUS_WAITING_FOR_FRAME_TAILER2:  //��ǰ״̬�ǵȴ�tailer2
				if( recvByte == PC2MCU_FRAME_TAILER2 )  //�յ���tailer2������һ֡
				{
					//�Ѿ��յ�tailer2������һ֡
					
					//�ӻ�����ȥ��Tailer0��Tailer1
					bufferCount -=2;
					
					stillLoop = 0; // ends the loop
					
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0; //��ǰ״̬��Ϊ�ȴ�header0��Ϊ������һ֡��׼��
				}
				else
				{
					buffer[bufferCount++] = recvByte; //�ѽ��յ����ݷ���buffer
					comStatus = COM_STATUS_RECEIVING_DATA; //����˵��ǰ����������ɺϣ��ʵ�ǰ״̬��Ϊ��������
				}
				break;
			
			default:
				bufferCount=0;
				comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
				break;
		}
		
		//serialPortSendByte( comStatus );
		
		//buffer����
		if( bufferCount == PC2MCU_BUFFER_SIZE)
		{
			//��������£�buffer��Ӧ����
			//buffer�ܱ��õ������ֵ�� 6+32+2��40
			//���buffer���˶�û�а�һ֡���꣬˵����֡�����ˣ�Ҫ������֡
			
			// ͨ��״̬��ʼ��Ϊ�ȴ�header0��Ϊ��һ֡������׼��
			comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
			
			//���buffer
			bufferCount = 0;
		}


	}while( stillLoop );
	

	return buffer;
}

// ͨ�����ڷ���һ֡���ݸ���λ��
void serialPortSendFrame( byte nodeID0, byte nodeID1, byte* dataArr)
{
	serialPortSendByte( MCU2PC_FRAME_HEADER0 );
	serialPortSendByte( MCU2PC_FRAME_HEADER1 );
	serialPortSendByte( MCU2PC_FRAME_HEADER2 );
	serialPortSendByte( MCU2PC_FRAME_HEADER3 );
	serialPortSendByte( nodeID0 );
	serialPortSendByte( nodeID1 );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr++ );
	serialPortSendByte( *dataArr );
	
	serialPortSendByte( MCU2PC_FRAME_TAILER0 );
	serialPortSendByte( MCU2PC_FRAME_TAILER1 );
	serialPortSendByte( MCU2PC_FRAME_TAILER2 );
}