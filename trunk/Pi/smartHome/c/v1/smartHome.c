/* 
������������Pi�ϵ�Daemon����
���𽫸��ڵ㷢�������ݴ������ݿ�;
ͬʱ����һЩ���ݷ��������ڵ㡣

���������У� gcc smartHome.c -o smartHome -l sqlite3

Todo:
1. �뵥Ƭ��ͨѶ������ߵ�115200bps ����Ҫ��Ƭ��ͬʱ�޸ĳ���֧�֣�
2. �޸�ͨ��Э�飺�䳤֡

�޸ļ�¼��
Date         Author      Remarks
-------------------------------------------------------------------------------
2013-FEB-25  Changhao    �������ڴ���ƻ�����Ĺ��ܺ���checkScheduledTask()
2013-SEP-30  Changhao    �޸���MCU��ͨѶ���ڲ�����Ϊ115200
2014-JUL-31  Changhao    �Ż�sqlite��synchronous/journal_mode/temp_store��������
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <strings.h>

//#define BAUDRATE B57600
#define BAUDRATE B115200
#define SERIAL_DEVICE "/dev/ttyAMA0"
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define DATABASE_FILE_NAME "/var/www/db/smarthome.sqlite3"


// ============= ����ͨѶЭ�� ============
//Header0��Header1���Ϊһ����ֵ��������ǿϵͳ���ݴ���
//��Ϊʵ�鷢��һ֡���ݵ�ͷһ���ֽ��ж�ʧ������

//MCU to PI
//BA9876543210
#define MCU2PI_FRAME_HEADER0 0xba
#define MCU2PI_FRAME_HEADER1 0xba
#define MCU2PI_FRAME_HEADER2 0x98
#define MCU2PI_FRAME_HEADER3 0x76

#define MCU2PI_FRAME_TAILER0 0x54
#define MCU2PI_FRAME_TAILER1 0x32
#define MCU2PI_FRAME_TAILER2 0x10

//PI to MCU
//0123456789AB
#define PI2MCU_FRAME_HEADER0 0x01
#define PI2MCU_FRAME_HEADER1 0x01
#define PI2MCU_FRAME_HEADER2 0x23
#define PI2MCU_FRAME_HEADER3 0x45

#define PI2MCU_FRAME_TAILER0 0x67
#define PI2MCU_FRAME_TAILER1 0x89
#define PI2MCU_FRAME_TAILER2 0xab

//����ͨ�ŵ�״̬
#define COM_STATUS_WAITING_FOR_FRAME_HEADER0 11
#define COM_STATUS_WAITING_FOR_FRAME_HEADER1 22
#define COM_STATUS_WAITING_FOR_FRAME_HEADER2 33
#define COM_STATUS_WAITING_FOR_FRAME_HEADER3 44
#define COM_STATUS_RECEIVING_DATA 55
#define COM_STATUS_WAITING_FOR_FRAME_TAILER1 66
#define COM_STATUS_WAITING_FOR_FRAME_TAILER2 77

//����ͨ�ŵ�buffer��С (�ֽ�����
//�൱�ڴ�MCU������ÿ֡���ݵĴ�С
#define MCU2PI_BUFFER_SIZE 45

//volatile int g_Stop = FALSE; 

// Create a handle for database connection, create a pointer to sqlite3
sqlite3 *g_dbHandle;

// serial port handle
int g_spHandle;

//�򿪴��ڣ�����ʼ����
void initSerialPort()
{
	//int spHandle;
	struct termios newtio;
	
	g_spHandle = open( SERIAL_DEVICE, O_RDWR | O_NOCTTY ); //"/dev/ttyAMA0"
	if (g_spHandle <0) {perror("serial open error! "); exit(-1); }

	//tcgetattr(fd,&oldtio); //��ȡ��ǰ����
	//tcsetattr(fd,TCSANOW,&oldtio); //�ָ�ԭ��������

	bzero(&newtio, sizeof(newtio));
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR;
	newtio.c_oflag = 0;

	/* set input mode (non-canonical, no echo,...) */
	newtio.c_lflag = 0;

	newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
	//newtio.c_cc[VMIN]     = 1;   /* blocking read until 5 chars received */

	tcflush(g_spHandle, TCIFLUSH);
	tcsetattr(g_spHandle,TCSANOW,&newtio);
	
	fcntl(g_spHandle, F_SETFL, 0);
	
	//return spHandle;
}


// �������յ�һ֡����ʱ��onFrameReceived()���ᱻ����
//
// ����˵����
//    * data - the pointer to the data
//    dataLength - how many bytes of data received
//    Ŀǰ��Ƭ��ͨ�����ڷ��������ݶ��Ƕ����ģ�ÿ֡16�ֽڣ���֡ͷ֡β֮�⣩��
void onFrameReceived( unsigned char * data, unsigned char dataLength )
{
	unsigned char i;
	unsigned char sqlStr[400];
	int tmp;
	
	//sqlite3_stmt * stmt;
	//sqlite3_prepare ( g_dbHandle, "INSERT INTO tabDataHistory  ( fldNodeID1, fldNodeID2, fldData1, fldData2 ) VALUES( ?, ?, ?, ? )", -1, &stmt, 0 );
	
	//sprintf( sqlStr, "INSERT INTO tabDataHistory (fldNodeID1,fldNodeID2,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldData11,fldData12) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data);
	sprintf( sqlStr, "INSERT INTO tabDataRecved (fldNodeID,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldData11,fldData12,fldData13,fldData14,fldData15) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data);
	tmp = sqlite3_exec(g_dbHandle,sqlStr,0,0,0);
	
	if( tmp != SQLITE_OK )
    {
        fprintf(stderr,"����%s\n",sqlite3_errmsg(g_dbHandle));
        //fprintf(stderr,"����%s\n",errmsg);
    }
}

// �������ݵ�ָ���Ľڵ�
// Piͨ�����ڷ������ݵ�MCU������MCU����nrf24L01�������ڵ�
// sendRFChannel: ���͵�nrf24L01Ƶ����
// sendAddr1-3: ����nrf24L01�ĵ�ַ�ֽ�1��2��3
// dataLength�����͵��ֽ��� ��1��32����Ӧ�ó���1��32�����ֵ)
// data: ָ�������ݵ�ָ��
void send2MCU( unsigned char sendRFChannel, 
				unsigned char sendAddr1, 
				unsigned char sendAddr2,
				unsigned char sendAddr3,
				unsigned char dataLength,
				unsigned char * data)
{
	unsigned char buffer[dataLength+14];
	unsigned char i=0, j=0;
	
	//frame header
	buffer[i++] = PI2MCU_FRAME_HEADER0;
	buffer[i++] = PI2MCU_FRAME_HEADER1;
	buffer[i++] = PI2MCU_FRAME_HEADER2;
	buffer[i++] = PI2MCU_FRAME_HEADER3;
	
	//frame content
	buffer[i++] = sendRFChannel;
	buffer[i++] = sendAddr1;
	buffer[i++] = sendAddr2;
	buffer[i++] = sendAddr3;
	buffer[i++] = dataLength;
	for( ; j<dataLength; j++)
	{
		buffer[i++] = *(data++);
	}

	//frame tailer
	buffer[i++] = PI2MCU_FRAME_TAILER0;
	buffer[i++] = PI2MCU_FRAME_TAILER1;
	buffer[i++] = PI2MCU_FRAME_TAILER2;
	
	write(g_spHandle, buffer, i);
	//printf( "send %d", cnt );
}

//������ݿ⣬���Ƿ���������Ҫ�������ڵ�
void checkSendDataToNode()
{
	int tmp;
	unsigned char sqlStr[250];
	sqlite3_stmt* stmt = NULL;
	int fldID, fldNodeID;
	unsigned char fldAddr1, fldAddr2, fldAddr3, fldRFChannel, fldDataLength;
	unsigned char buf[10];
	
	//ѡ����Ҫ���ڵ㷢�͵�����	
	tmp = sqlite3_prepare(g_dbHandle, "select * from tabDataToNode where fldUpdatedBy is null OR fldUpdatedBy<>'robot'", -1, &stmt, 0);
	if (tmp != SQLITE_OK){
		fprintf(stderr, "Could not execute SELECT/n");
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{
		//printf("ID=%d, Node1=%d, Node2=%d, data0=%d, LastUpdatedBy=%s\n", sqlite3_column_int(stmt, 0),
		//			sqlite3_column_int(stmt, 1),
		//			sqlite3_column_int(stmt, 2),
		//			sqlite3_column_int(stmt, 3),
		//			sqlite3_column_text(stmt, 8));

		fldID = sqlite3_column_int(stmt, 0);
		fldNodeID = sqlite3_column_int(stmt, 1);

		fldAddr1 = sqlite3_column_int(stmt, 3);
		fldAddr2 = sqlite3_column_int(stmt, 4);
		fldAddr3 = sqlite3_column_int(stmt, 5);
		fldRFChannel = sqlite3_column_int(stmt, 6);
		fldDataLength = sqlite3_column_int(stmt, 7);
		
		buf[0] = sqlite3_column_int(stmt, 8);
		buf[1] = sqlite3_column_int(stmt, 9);
		buf[2] = sqlite3_column_int(stmt, 10);
		buf[3] = sqlite3_column_int(stmt, 11);
		buf[4] = sqlite3_column_int(stmt, 12);
		buf[5] = sqlite3_column_int(stmt, 13);
		buf[6] = sqlite3_column_int(stmt, 14);
		buf[7] = sqlite3_column_int(stmt, 15);
		buf[8] = sqlite3_column_int(stmt, 16);
		buf[9] = sqlite3_column_int(stmt, 17);

		send2MCU(fldRFChannel, fldAddr1, fldAddr2, fldAddr3, fldDataLength, buf);
		
		// update record
		sprintf( sqlStr, "UPDATE tabDataToNode SET fldUpdatedBy='robot', fldUpdatedOn=datetime('now', 'localtime') WHERE fldID=%d", fldID);
		tmp = sqlite3_exec( g_dbHandle, sqlStr, 0, 0, 0);
		
		if( tmp != SQLITE_OK )
		{
			fprintf(stderr,"����%s\n",sqlite3_errmsg(g_dbHandle));
			//fprintf(stderr,"����%s\n",errmsg);
		}
	}
	sqlite3_finalize(stmt);
}

// ��鿴�Ƿ���ScheduledTask��ִ����
void checkScheduledTask()
{
	int tmp, taskStatus;
	unsigned char sqlStr[250];
	sqlite3_stmt* stmt = NULL;
	int fldID;
	
	//ѡ��Ӧ�ñ�ִ�еļƻ�����	
	tmp = sqlite3_prepare(g_dbHandle, "SELECT * FROM tabScheduledTask WHERE fldScheduledDateTime <= datetime('now', 'localtime') AND fldTaskStatus=0", -1, &stmt, 0);
	if (tmp != SQLITE_OK){
		fprintf(stderr, "Could not execute SELECT/n");
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{

		fldID = sqlite3_column_int(stmt, 0);
		//fldSQL = sqlite3_column_text(stmt, 2);
		
		//ִ�мƻ������sql���
		tmp = sqlite3_exec( g_dbHandle, sqlite3_column_text(stmt, 2), 0, 0, 0);
		
		if( tmp != SQLITE_OK )
		{
			fprintf(stderr,"error happens while executing scheduled Task SQL. %s\n",sqlite3_errmsg(g_dbHandle));
			taskStatus=2; //�ƻ�����ִ�г���
		}
		else
		{
			taskStatus=1; //�ƻ�����ִ�гɹ�
		}
		
		//���¼ƻ������tabScheduledTask����ļ�¼״̬		
		sprintf( sqlStr, "UPDATE tabScheduledTask SET fldUpdatedBy='robot', fldUpdatedOn=datetime('now', 'localtime'), fldTaskStatus=%d WHERE fldID=%d", taskStatus, fldID );
		tmp = sqlite3_exec( g_dbHandle, sqlStr, 0, 0, 0);
		if( tmp != SQLITE_OK )
		{
			fprintf(stderr,"error happens trying to update tabScheduledTask %s\n",sqlite3_errmsg(g_dbHandle));
		}
	}
	sqlite3_finalize(stmt);
}

main()
{
	int cnt, tmp;
	
	unsigned char readBuf[1]; //��ȡ���ڵ�buffer����Ϊÿ��ֻ��1���ֽڣ�����bufferֻҪ1���ֽ�
	unsigned char recvByte; // received byte
	
	unsigned char stillLoop; 
	
	// ͨ��״̬��ʼ��Ϊ�ȴ�header0
	unsigned char comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
	
	unsigned char bufferCount = 0;
	unsigned char buffer[MCU2PI_BUFFER_SIZE];  // static unsigned char buffer[MCU2PI_BUFFER_SIZE];
	
	// try to create the database. If it doesnt exist, it would be created
    // pass a pointer to the pointer to sqlite3, in short sqlite3**
    tmp = sqlite3_open( DATABASE_FILE_NAME, &g_dbHandle );
    // If connection failed, handle returns NULL
    if( tmp != SQLITE_OK )
    {
        printf("Database connection failed.\n");
        exit( -1 );
    }
    printf("Database connected successfully.\n");
	
	// set timeout to 3000ms
	sqlite3_busy_timeout(g_dbHandle, 3000);

	sqlite3_exec(g_dbHandle, "PRAGMA synchronous = OFF; ", 0,0,0);
	sqlite3_exec(g_dbHandle, "PRAGMA journal_mode = OFF; ", 0,0,0);	
	sqlite3_exec(g_dbHandle, "PRAGMA temp_store = MEMORY; ", 0,0,0);	
	

	// initialize serial port
	initSerialPort();
	
	while( TRUE ) 
	{       
		stillLoop = TRUE;
		
		do{ //��Pi�յ�һ֡��Ч���ݾͻ��˳����ѭ����
		
		
			//��ȡ���ڣ���������ݣ����һ���ֽڵ�readBuf
			cnt = read( g_spHandle, readBuf, 1 );
			
			if( cnt > 0 )
			{ 
				// ��������յ����ݣ���������
				recvByte = readBuf[0]; //�õ����յ�һ���ֽ�
				
				switch( comStatus )
				{
					case COM_STATUS_WAITING_FOR_FRAME_HEADER0:  //��ǰ״̬�ǵȴ�header0
						if( recvByte == MCU2PI_FRAME_HEADER0 )  //�յ���header0
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER1; //��ǰ״̬��Ϊ�ȴ�header1
						}
						break;
					
					case COM_STATUS_WAITING_FOR_FRAME_HEADER1:  //��ǰ״̬�ǵȴ�header1
						if( recvByte == MCU2PI_FRAME_HEADER1 )  //�յ���header1
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER2; //��ǰ״̬��Ϊ�ȴ�header2
						}
						else if( recvByte == MCU2PI_FRAME_HEADER2 )  //�յ���header2������һ���ݴ���ƣ��ο���λ�������ע��
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
						if( recvByte == MCU2PI_FRAME_HEADER2 )  //�յ���header2
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
						if( recvByte == MCU2PI_FRAME_HEADER3 )  //�յ���header3
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
						if( recvByte == MCU2PI_FRAME_TAILER0 )  //�յ���tailer0����Ȼ�п������ɺ�
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_TAILER1; //��ǰ״̬��Ϊ�ȴ�tailer1
						}
						break;
						
					case COM_STATUS_WAITING_FOR_FRAME_TAILER1:  //��ǰ״̬�ǵȴ�tailer1
						buffer[bufferCount++] = recvByte; //�ѽ��յ����ݷ���buffer
						if( recvByte == MCU2PI_FRAME_TAILER1 )  //�յ���tailer1����Ȼ�п������ɺ�
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_TAILER2; //��ǰ״̬��Ϊ�ȴ�tailer2
						}
						else
						{
							comStatus = COM_STATUS_RECEIVING_DATA; //����˵��ǰ����������ɺϣ��ʵ�ǰ״̬��Ϊ��������
						}
						break;

					case COM_STATUS_WAITING_FOR_FRAME_TAILER2:  //��ǰ״̬�ǵȴ�tailer2
						if( recvByte == MCU2PI_FRAME_TAILER2 )  //�յ���tailer2������һ֡
						{
							//�Ѿ��յ�tailer2������һ֡
							
							//�ӻ�����ȥ��Tailer0��Tailer1
							bufferCount -=2;
							
							stillLoop = FALSE; // ends the loop
							
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

			}
			else 
			{
				//����Ƿ��мƻ�����
				checkScheduledTask();
				
				//����Ƿ���������Ҫ���͵��ڵ�
				checkSendDataToNode();
				
				//�������û���յ����ݣ���sleep 300ms
				//printf( "-" );
				usleep( 300000 ); // sleep for x us
			}
		}while( stillLoop);
		
		//printf( "onFrameReceived ... \n" );
		onFrameReceived( buffer, bufferCount );

	}
	
}