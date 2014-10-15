/* 
������������Pi�ϵ�Daemon����
���𽫸��ڵ㷢�������ݴ������ݿ�;
ͬʱ����һЩ���ݷ��������ڵ㡣

���������У�gcc -Wall -lwiringPi -lsqlite3 -o "smartHome2" "smartHome2.c" "nrf24L01.c"


�޸ļ�¼��
Date         Author   Remarks
-------------------------------------------------------------------------------
2013-SEP-02  �Ƴ���   ��ʼ�汾�����ܼ�Deamon v2
2014-SEP-23  �Ƴ���   �޸�checkSendDataToNode()����������transaction������tabDataSent��
2014-SEP-23  �Ƴ���   �޸�onDataReceived()������Pi�Ľ��յ�ַ��Ϊ5�ֽ�
                      �޸�checkSendDataToNode()����������Ӧ�µ�tabDataToNode��tabDataSent��ṹ
2014-OCT-05  �Ƴ���   tabDataSent������fldRequestor�ֶ�
2014-OCT-05  �Ƴ���   ����execSql()����������main()��������Whileѭ���ṹ
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <strings.h>
#include <unistd.h>
#include "nrf24L01.h"

#define FALSE 0
#define TRUE 1
#define DATABASE_FILE_NAME "/var/www/db/smarthome.sqlite3"


//Create a handle for database connection, create a pointer to sqlite3
sqlite3 *g_dbHandle = NULL;

//Declare functions
void startRecv();
void initDatabase();
void execSql( char *sqlStr );
void processDataToNode();
void processReceivedData();


//ִ��ָ����SQL
void execSql( char *sqlStr )
{
	int execResult;
	char *errMsg;

	execResult = sqlite3_exec(g_dbHandle, sqlStr, NULL, NULL, &errMsg);
	if ( execResult != SQLITE_OK )
	{
		printf ( "Execute SQL failed!! SQL:'%s', Error Code:%d, Error msg:%s\r\n", sqlStr, execResult, errMsg );
		sqlite3_free(errMsg);
	}
}


//NRF24L01�������ģʽ
void startRecv()
{
	unsigned char myAddr[5] = {53, 69, 149, 231, 231};
	nrfSetRxMode( 96, 5, myAddr ); //����96Ƶ����5�ֽڵ�ַ
}


//����NRF���յ�������
void processReceivedData()
{
	char sqlStr[450];
	unsigned char *data;
	unsigned char toDelay = FALSE;

	while( nrfDataAvailable() )
	{
		if( toDelay )//��һ�����ݰ�������ʱ
		{
			//��ʱ50ms��
			//�һ���:
			//���ͷ�����һ�����ݣ����շ��յ��������ͷ���û���յ�ACK����ʼ�ش���
			//������շ��Ѿ��Ѹİ������Ƴ�FIFO�����п����޷���ʶ�ش������ݰ���
			//�Ӷ��յ��ظ������ݡ�
			//��ʱ���Ǳ����������ķ���
			usleep( 50000 ); // sleep for x us
		}
		
		//��ȡnrf24L01+�յ���1�����ݰ�
		data = nrfGetOneDataPacket();

		//�����ݴ������ݿ�
		sprintf( sqlStr, "INSERT INTO tabDataRecved (fldNodeID,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldData11,fldData12,fldData13,fldData14,fldData15) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", *(data+0), *(data+1), *(data+2), *(data+3), *(data+4), *(data+5), *(data+6), *(data+7), *(data+8), *(data+9), *(data+10), *(data+11), *(data+12), *(data+13), *(data+14), *(data+15));
		execSql( sqlStr );

		toDelay = TRUE;
	}


}


//������ݿ⣬���Ƿ���������Ҫ�������ڵ�
void processDataToNode()
{
	int ret;
	char sqlStr[450];
	sqlite3_stmt* stmt = NULL;
	unsigned char fldNodeID;
	unsigned char fldRFChannel, fldRFPower, fldMaxRetry, fldAddrLength, fldDataLength;
	unsigned char toAddr[5];
	const unsigned char * fldRequestor = NULL;
	unsigned char sendData[10];
	unsigned char sendResult;
	
	unsigned char sentAnything = FALSE;


	//ѡ����Ҫ���ڵ㷢�͵�����	
	ret = sqlite3_prepare(g_dbHandle, "SELECT * FROM tabDataToNode WHERE fldUpdatedBy IS NULL OR fldUpdatedBy<>'robot'", -1, &stmt, 0);
	if (ret != SQLITE_OK){
		fprintf(stderr, "Could not execute SELECT!\n");
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{

		if( !sentAnything )
		{
			//��û�з����κ����ݣ�˵�����ǵ�һ����¼����ʼһ��transaction	
			execSql("BEGIN");
		}

		fldNodeID = sqlite3_column_int(stmt, 0);
		fldAddrLength = sqlite3_column_int(stmt, 3);
		toAddr[0] = sqlite3_column_int(stmt, 4);
		toAddr[1] = sqlite3_column_int(stmt, 5);
		toAddr[2] = sqlite3_column_int(stmt, 6);
		toAddr[3] = sqlite3_column_int(stmt, 7);
		toAddr[4] = sqlite3_column_int(stmt, 8);
		fldRFChannel = sqlite3_column_int(stmt, 9);
		fldRFPower = sqlite3_column_int(stmt, 10);
		fldMaxRetry = sqlite3_column_int(stmt, 11);
		fldDataLength = sqlite3_column_int(stmt, 12);
		
		sendData[0] = sqlite3_column_int(stmt, 13);
		sendData[1] = sqlite3_column_int(stmt, 14);
		sendData[2] = sqlite3_column_int(stmt, 15);
		sendData[3] = sqlite3_column_int(stmt, 16);
		sendData[4] = sqlite3_column_int(stmt, 17);
		sendData[5] = sqlite3_column_int(stmt, 18);
		sendData[6] = sqlite3_column_int(stmt, 19);
		sendData[7] = sqlite3_column_int(stmt, 20);
		sendData[8] = sqlite3_column_int(stmt, 21);
		sendData[9] = sqlite3_column_int(stmt, 22);

		fldRequestor = sqlite3_column_text(stmt, 30);

		//printf( "requestor:[%s]\n\r", fldRequestor );
		
		//��������
		sendResult = nrfSendData( fldRFChannel, fldRFPower, fldMaxRetry, fldAddrLength, toAddr, fldDataLength, sendData);
		//printf( "send data to NodeID:[%d], result=%d\n", fldNodeID, sendResult );

		sentAnything = TRUE;
		
		// update record
		sprintf( sqlStr, "UPDATE tabDataToNode SET fldUpdatedBy='robot', fldUpdatedOn=datetime('now', 'localtime'), fldLastSentResult=%d WHERE fldNodeID=%d", sendResult, fldNodeID );
		execSql( sqlStr );

		//�����ݴ���tabDataSent
		sprintf( sqlStr, "INSERT INTO tabDataSent (fldToNodeID,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldRequestor,fldSentResult) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d)", fldNodeID, sendData[0], sendData[1], sendData[2], sendData[3], sendData[4], sendData[5], sendData[6], sendData[7], sendData[8], sendData[9], fldRequestor, sendResult );
		execSql( sqlStr );
	}

	//������͹����ݣ���˵���б�Ҫ���£�����transaction��
	//������͹����ݣ���˵��NRF24L01�Ѿ��˳���PRXģʽ
	//��������������ģʽ
	if(sentAnything)
	{
		//���½������ģʽ
		startRecv();
		
		//����transaction
		execSql("COMMIT");
	}

	sqlite3_finalize(stmt);

}


//�����ݿ⣬�����һЩ�����ĳ�ʼ��
void initDatabase()
{
	int ret;

	// try to create the database. If it doesnt exist, it would be created
    // pass a pointer to the pointer to sqlite3, in short sqlite3**
    ret = sqlite3_open( DATABASE_FILE_NAME, &g_dbHandle );
    // If connection failed, handle returns NULL
    if( ret != SQLITE_OK )
    {
        printf("Database connection failed.\n");
        exit( -1 );
    }
	else
	{
		printf("Database connected successfully.\n");
	
		// set timeout to 3000ms
		sqlite3_busy_timeout(g_dbHandle, 3000);

		// set some parameters
		execSql( "PRAGMA synchronous = OFF;" );
		execSql( "PRAGMA journal_mode = OFF;" );	
		execSql( "PRAGMA temp_store = MEMORY;" );

		printf("Database initialization done successfully.\n");
	}

}


//���������
int main ( int argc, char **argv )
{
	printf( "Start SmartHome v2 dameon ... \n\r" );
	
	// database initialization
	initDatabase();

	//��ʼ��NRF24L01+�����������ģʽ
	nrf24L01Init();	
	startRecv();

	while( TRUE ) 
	{

		//������յ������ݣ�����еĻ�
		processReceivedData();

		//����Ƿ���������Ҫ���͵��ڵ�
		processDataToNode();

		//�߳�����300ms
		usleep( 300000 ); // sleep for x us
	}

	return 0;
}