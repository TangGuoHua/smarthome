/* 
������������Pi�ϵ�Daemon����
���𽫸��ڵ㷢�������ݴ������ݿ�;
ͬʱ����һЩ���ݷ��������ڵ㡣

���������У�gcc -Wall -lwiringPi -l sqlite3 -o "smartHome2" "smartHome2.c" "nrf24L01.c"


�޸ļ�¼��
Date         Author   Remarks
-------------------------------------------------------------------------------
2013-SEP-02  �Ƴ���   ��ʼ�汾�����ܼ�Deamon v2
2014-SEP-23  �Ƴ���   �޸�checkSendDataToNode��������������transaction������tabDataSent��
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
void checkSendDataToNode();
void checkScheduledTask();


//NRF24L01���յ����ݺ�ִ�б�����
void onDataReceived()
{
	//unsigned char i;
	char sqlStr[400];
	unsigned char *data;
	int tmp;
	
	//��ȡnrf24L01�յ�������
	data = nrfGetReceivedData();

	//sqlite3_stmt * stmt;
	//sqlite3_prepare ( g_dbHandle, "INSERT INTO tabDataHistory  ( fldNodeID1, fldNodeID2, fldData1, fldData2 ) VALUES( ?, ?, ?, ? )", -1, &stmt, 0 );
	
	//�����ݴ������ݿ�
	sprintf( sqlStr, "INSERT INTO tabDataRecved (fldNodeID,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldData11,fldData12,fldData13,fldData14,fldData15) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", *(data+0), *(data+1), *(data+2), *(data+3), *(data+4), *(data+5), *(data+6), *(data+7), *(data+8), *(data+9), *(data+10), *(data+11), *(data+12), *(data+13), *(data+14), *(data+15));
	tmp = sqlite3_exec(g_dbHandle,sqlStr,0,0,0);
	//tmp = sqlite3_exec(g_dbHandle,sqlStr,NULL,NULL,NULL);
	
	if( tmp != SQLITE_OK )
    {
        fprintf(stderr,"����%s\n",sqlite3_errmsg(g_dbHandle));
        //fprintf(stderr,"����%s\n",errmsg);
    }
}

//NRF24L01�������ģʽ
void startRecv()
{
	unsigned char myAddr[3] = {53,70, 132};
	nrfSetRxMode( 92, 3, myAddr ); //����92Ƶ����3�ֽڵ�ַ
}

//������ݿ⣬���Ƿ���������Ҫ�������ڵ�
void checkSendDataToNode()
{
	int ret;
	char sqlStr[400];
	sqlite3_stmt* stmt = NULL;
	int fldID, fldNodeID;
	unsigned char fldRFChannel, fldRFPower, fldMaxRetry, fldDataLength;
	unsigned char toAddr[3];
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
		//printf("ID=%d, Node1=%d, Node2=%d, data0=%d, LastUpdatedBy=%s\n", sqlite3_column_int(stmt, 0),
		//			sqlite3_column_int(stmt, 1),
		//			sqlite3_column_int(stmt, 2),
		//			sqlite3_column_int(stmt, 3),
		//			sqlite3_column_text(stmt, 8));

		if( !sentAnything )
		{
			//��û�з����κ����ݣ�˵�����ǵ�һ����¼����ʼһ��transaction	
			ret = sqlite3_exec(g_dbHandle,"BEGIN",NULL,NULL,NULL);
			if (ret != SQLITE_OK){
				fprintf(stderr, "BEGIN transaction failed!!\n");
				return;
			}
		}

		fldID = sqlite3_column_int(stmt, 0);
		fldNodeID = sqlite3_column_int(stmt, 1);

		toAddr[0] = sqlite3_column_int(stmt, 3);
		toAddr[1] = sqlite3_column_int(stmt, 4);
		toAddr[2] = sqlite3_column_int(stmt, 5);
		fldRFChannel = sqlite3_column_int(stmt, 6);
		fldRFPower = 1;
		fldMaxRetry = 3;
		fldDataLength = sqlite3_column_int(stmt, 7);
		
		sendData[0] = sqlite3_column_int(stmt, 8);
		sendData[1] = sqlite3_column_int(stmt, 9);
		sendData[2] = sqlite3_column_int(stmt, 10);
		sendData[3] = sqlite3_column_int(stmt, 11);
		sendData[4] = sqlite3_column_int(stmt, 12);
		sendData[5] = sqlite3_column_int(stmt, 13);
		sendData[6] = sqlite3_column_int(stmt, 14);
		sendData[7] = sqlite3_column_int(stmt, 15);
		sendData[8] = sqlite3_column_int(stmt, 16);
		sendData[9] = sqlite3_column_int(stmt, 17);
		
		//��������
		sendResult = nrfSendData( fldRFChannel, fldRFPower, fldMaxRetry, 3, toAddr, fldDataLength, sendData);
		sentAnything = TRUE;

		printf( "send data to NodeID:[%d], result=%d\n", fldNodeID, sendResult );
		
		// update record
		sprintf( sqlStr, "UPDATE tabDataToNode SET fldUpdatedBy='robot', fldUpdatedOn=datetime('now', 'localtime') WHERE fldID=%d", fldID);
		ret = sqlite3_exec( g_dbHandle, sqlStr, 0, 0, 0);
		if( ret != SQLITE_OK )
		{
			fprintf(stderr,"����%s\n",sqlite3_errmsg(g_dbHandle));
			//fprintf(stderr,"����%s\n",errmsg);
		}

		//�����ݴ������ݿ�
		sprintf( sqlStr, "INSERT INTO tabDataSent (fldToNodeID,fldRFChannel, fldRFPower,fldMaxRetry,fldAddr1,fldAddr2,fldAddr3,fldDataLength,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldSentResult) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", fldNodeID, fldRFChannel, fldRFPower, fldMaxRetry, toAddr[0], toAddr[1], toAddr[2], fldDataLength, sendData[0], sendData[1], sendData[2], sendData[3], sendData[4], sendData[5], sendData[6], sendData[7], sendData[8], sendData[9], sendResult );
		ret = sqlite3_exec(g_dbHandle,sqlStr,NULL,NULL,NULL);
		
		if( ret != SQLITE_OK )
		{
			fprintf(stderr,"����%s\n",sqlite3_errmsg(g_dbHandle));
			//fprintf(stderr,"����%s\n",errmsg);
		}
	}

	//������͹����ݣ���˵���б�Ҫ���£�����transaction��
	//������͹����ݣ���˵��NRF24L01�Ѿ��˳���PRXģʽ
	//��������������ģʽ
	if(sentAnything)
	{
		//���½������ģʽ
		startRecv();
		
		//����transaction
		ret = sqlite3_exec(g_dbHandle, "COMMIT", NULL, NULL, NULL);
		if (ret != SQLITE_OK){
			fprintf(stderr, "COMMIT transaction failed!!\n");
			return;
		}
	}

	sqlite3_finalize(stmt);

}

// ��鿴�Ƿ���ScheduledTask��ִ����
void checkScheduledTask()
{
	/*
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
	*/
}


//�����ݿ⣬�����һЩ�����ĳ�ʼ��
void initDatabase()
{
	int ret;
	char *errMsg;

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

		ret = sqlite3_exec(g_dbHandle, "PRAGMA synchronous = OFF; ", NULL, NULL, &errMsg);
		if ( ret != SQLITE_OK ) {
            printf ( "PRAGMA synchronous = OFF Failed! Error Code:%d, Error msg:%s\r\n", ret, errMsg );
        }

		ret = sqlite3_exec(g_dbHandle, "PRAGMA journal_mode = OFF; ", NULL, NULL, &errMsg);	
		if ( ret != SQLITE_OK ) {
            printf ( "PRAGMA journal_mode = OFF Failed! Error Code:%d, Error msg:%s\r\n", ret, errMsg );
        }

		ret = sqlite3_exec(g_dbHandle, "PRAGMA temp_store = MEMORY; ", NULL, NULL, &errMsg);
		if ( ret != SQLITE_OK ) {
            printf ( "PRAGMA temp_store = MEMORY Failed! Error Code:%d, Error msg:%s\r\n", ret, errMsg );
        }

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
	startRecv( );

	while( TRUE ) 
	{
		if( nrfIsDataReceived() )
		{
			printf( "data received.\n");
			// NRF24L01���յ�����
			onDataReceived();
		}
		else
		{		
			//���û���յ����ݣ���sleep 300ms
			usleep( 300000 ); // sleep for x us
		}

		//����Ƿ��мƻ�����
		//checkScheduledTask();
		
		//����Ƿ���������Ҫ���͵��ڵ�
		checkSendDataToNode();
	}

	return 0;
}