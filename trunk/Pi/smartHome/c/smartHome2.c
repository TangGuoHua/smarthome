/* 
本程序是跑在Pi上的Daemon程序，
负责将各节点发来的数据存入数据库;
同时负责将一些数据发往各个节点。

编译命令行： gcc smartHome.c -o smartHome -l sqlite3
编译命令行 gcc -Wall -lwiringPi -o "main" "main.c" "nrf24L01.c"

gcc -Wall -lwiringPi -l sqlite3 -o "main" "main.c" "nrf24L01.c"
 gcc -Wall -lwiringPi -l sqlite3 -o "smartHome2" "smartHome2.c" "nrf24L01.c"


修改记录：
Date         Author   Remarks
-------------------------------------------------------------------------------
2013-FEB-25  黄长浩   初始版本
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <stdlib.h>
#include <stdio.h>
#include <sqlite3.h>
#include <strings.h>
#include "nrf24L01.h"
#include <unistd.h>

//#define BAUDRATE B57600
//#define BAUDRATE B115200
//#define SERIAL_DEVICE "/dev/ttyAMA0"
//#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define FALSE 0
#define TRUE 1
#define DATABASE_FILE_NAME "/var/www/db/smarthome.sqlite3"


// Create a handle for database connection, create a pointer to sqlite3
sqlite3 *g_dbHandle = NULL;


void initDatabase();
void checkSendDataToNode();
void startRecv();


void onDataReceived()
{
	//unsigned char i;
	char sqlStr[400];
	unsigned char *data;
	int tmp;
	
	data = nrfGetReceivedData();

	//sqlite3_stmt * stmt;
	//sqlite3_prepare ( g_dbHandle, "INSERT INTO tabDataHistory  ( fldNodeID1, fldNodeID2, fldData1, fldData2 ) VALUES( ?, ?, ?, ? )", -1, &stmt, 0 );
	
	//sprintf( sqlStr, "INSERT INTO tabDataHistory (fldNodeID1,fldNodeID2,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldData11,fldData12) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data);
	sprintf( sqlStr, "INSERT INTO tabDataRecved (fldNodeID,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldData11,fldData12,fldData13,fldData14,fldData15) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data++, *data);
	tmp = sqlite3_exec(g_dbHandle,sqlStr,0,0,0);
	//tmp = sqlite3_exec(g_dbHandle,sqlStr,NULL,NULL,NULL);
	
	if( tmp != SQLITE_OK )
    {
        fprintf(stderr,"错误%s\n",sqlite3_errmsg(g_dbHandle));
        //fprintf(stderr,"错误%s\n",errmsg);
    }
}


void startRecv()
{
	unsigned char myAddr[3] = {53,70, 132};
	nrfSetRxMode( 92, 3, myAddr );
	
	//printf( "Starting listening on channel [%d]... \n\r" , rfChannel);
	
}

//检查数据库，看是否有数据需要发送至节点
void checkSendDataToNode()
{

	int ret;
	char sqlStr[250];
	sqlite3_stmt* stmt = NULL;
	int fldID, fldNodeID;
	//unsigned char fldAddr1, fldAddr2, fldAddr3, 
	unsigned char fldRFChannel, fldDataLength;
	unsigned char buf[10];
	unsigned char toAddr[3];
	unsigned char sendResult;
	
	unsigned char sendAnything = FALSE;

	//选出需要往节点发送的数据	
	ret = sqlite3_prepare(g_dbHandle, "select * from tabDataToNode where fldUpdatedBy is null OR fldUpdatedBy<>'robot'", -1, &stmt, 0);
	if (ret != SQLITE_OK){
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

		toAddr[0] = sqlite3_column_int(stmt, 3);
		toAddr[1] = sqlite3_column_int(stmt, 4);
		toAddr[2] = sqlite3_column_int(stmt, 5);
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


		sendResult = nrfSendData( fldRFChannel, 1, 3, 3, toAddr, fldDataLength, buf);
		sendAnything = TRUE;

		printf( "send result=%d\n", sendResult );

		
		// update record
		sprintf( sqlStr, "UPDATE tabDataToNode SET fldUpdatedBy='robot', fldUpdatedOn=datetime('now', 'localtime') WHERE fldID=%d", fldID);
		ret = sqlite3_exec( g_dbHandle, sqlStr, 0, 0, 0);
		
		if( ret != SQLITE_OK )
		{
			fprintf(stderr,"错误%s\n",sqlite3_errmsg(g_dbHandle));
			//fprintf(stderr,"错误%s\n",errmsg);
		}
	}

	if(sendAnything)
	{
		startRecv();
	}

	sqlite3_finalize(stmt);

}

// 检查看是否有ScheduledTask该执行了
void checkScheduledTask()
{
	/*
	int tmp, taskStatus;
	unsigned char sqlStr[250];
	sqlite3_stmt* stmt = NULL;
	int fldID;
	
	//选出应该被执行的计划任务	
	tmp = sqlite3_prepare(g_dbHandle, "SELECT * FROM tabScheduledTask WHERE fldScheduledDateTime <= datetime('now', 'localtime') AND fldTaskStatus=0", -1, &stmt, 0);
	if (tmp != SQLITE_OK){
		fprintf(stderr, "Could not execute SELECT/n");
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{

		fldID = sqlite3_column_int(stmt, 0);
		//fldSQL = sqlite3_column_text(stmt, 2);
		
		//执行计划任务的sql语句
		tmp = sqlite3_exec( g_dbHandle, sqlite3_column_text(stmt, 2), 0, 0, 0);
		
		if( tmp != SQLITE_OK )
		{
			fprintf(stderr,"error happens while executing scheduled Task SQL. %s\n",sqlite3_errmsg(g_dbHandle));
			taskStatus=2; //计划任务执行出错
		}
		else
		{
			taskStatus=1; //计划任务执行成功
		}
		
		//更新计划任务表tabScheduledTask里面的记录状态		
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




//打开数据库，并完成一些参数的初始化
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


int main ( int argc, char **argv )
{
	


	printf( "Start testing NRF24L01+ ... \n\r" );
	


	// database initialization
	initDatabase();

	//初始化NRF24L01+，并进入接收模式
	nrf24L01Init();	
	startRecv( );


	

	// initialize serial port
	//initSerialPort();
	
	while( TRUE ) 
	{
		if( nrfIsDataReceived() )
		{
			printf( "data received.\n");
			// NRF24L01接收到数据
			onDataReceived();

		}

		//检查是否有计划任务
		//checkScheduledTask();
		
		//检查是否有数据需要发送到节点
		checkSendDataToNode();
		
		//如果串口没有收到数据，则sleep 300ms
		//printf( "-" );
		usleep( 300000 ); // sleep for x us


	}

	return 0;
	
}