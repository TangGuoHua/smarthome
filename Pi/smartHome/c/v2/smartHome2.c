/* 
本程序是跑在Pi上的Daemon程序，
负责将各节点发来的数据存入数据库;
同时负责将一些数据发往各个节点。

编译命令行：gcc -Wall -lwiringPi -lsqlite3 -o "smartHome2" "smartHome2.c" "nrf24L01.c"


修改记录：
Date         Author   Remarks
-------------------------------------------------------------------------------
2013-SEP-02  黄长浩   初始版本，智能家Deamon v2
2014-SEP-23  黄长浩   修改checkSendDataToNode()方法，增加transaction，增加tabDataSent表
2014-SEP-23  黄长浩   修改onDataReceived()方法，Pi的接收地址变为5字节
                      修改checkSendDataToNode()方法，以适应新的tabDataToNode和tabDataSent表结构
2014-OCT-05  黄长浩   tabDataSent表增加fldRequestor字段
2014-OCT-05  黄长浩   增加execSql()方法，整理main()函数的主While循环结构
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


//执行指定的SQL
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


//NRF24L01进入接收模式
void startRecv()
{
	unsigned char myAddr[5] = {53, 69, 149, 231, 231};
	nrfSetRxMode( 96, 5, myAddr ); //监听96频道，5字节地址
}


//处理NRF接收到的数据
void processReceivedData()
{
	char sqlStr[450];
	unsigned char *data;
	unsigned char toDelay = FALSE;

	while( nrfDataAvailable() )
	{
		if( toDelay )//第一个数据包不用延时
		{
			//延时50ms。
			//我怀疑:
			//发送方发出一包数据，接收方收到，但发送方在没有收到ACK，开始重传。
			//如果接收方已经把改包数据移出FIFO，则有可能无法辨识重传的数据包，
			//从而收到重复的数据。
			//延时就是避免这个情况的发生
			usleep( 50000 ); // sleep for x us
		}
		
		//读取nrf24L01+收到的1个数据包
		data = nrfGetOneDataPacket();

		//将数据存入数据库
		sprintf( sqlStr, "INSERT INTO tabDataRecved (fldNodeID,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldData11,fldData12,fldData13,fldData14,fldData15) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)", *(data+0), *(data+1), *(data+2), *(data+3), *(data+4), *(data+5), *(data+6), *(data+7), *(data+8), *(data+9), *(data+10), *(data+11), *(data+12), *(data+13), *(data+14), *(data+15));
		execSql( sqlStr );

		toDelay = TRUE;
	}


}


//检查数据库，看是否有数据需要发送至节点
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


	//选出需要往节点发送的数据	
	ret = sqlite3_prepare(g_dbHandle, "SELECT * FROM tabDataToNode WHERE fldUpdatedBy IS NULL OR fldUpdatedBy<>'robot'", -1, &stmt, 0);
	if (ret != SQLITE_OK){
		fprintf(stderr, "Could not execute SELECT!\n");
		return;
	}

	while (sqlite3_step(stmt) == SQLITE_ROW)
	{

		if( !sentAnything )
		{
			//还没有发送任何数据，说明这是第一条记录，则开始一个transaction	
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
		
		//发送数据
		sendResult = nrfSendData( fldRFChannel, fldRFPower, fldMaxRetry, fldAddrLength, toAddr, fldDataLength, sendData);
		//printf( "send data to NodeID:[%d], result=%d\n", fldNodeID, sendResult );

		sentAnything = TRUE;
		
		// update record
		sprintf( sqlStr, "UPDATE tabDataToNode SET fldUpdatedBy='robot', fldUpdatedOn=datetime('now', 'localtime'), fldLastSentResult=%d WHERE fldNodeID=%d", sendResult, fldNodeID );
		execSql( sqlStr );

		//将数据存入tabDataSent
		sprintf( sqlStr, "INSERT INTO tabDataSent (fldToNodeID,fldData1,fldData2,fldData3,fldData4,fldData5,fldData6,fldData7,fldData8,fldData9,fldData10,fldRequestor,fldSentResult) VALUES (%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,'%s',%d)", fldNodeID, sendData[0], sendData[1], sendData[2], sendData[3], sendData[4], sendData[5], sendData[6], sendData[7], sendData[8], sendData[9], fldRequestor, sendResult );
		execSql( sqlStr );
	}

	//如果发送过数据，则说明有表要更新，结束transaction。
	//如果发送过数据，则说明NRF24L01已经退出了PRX模式
	//重新让其进入接收模式
	if(sentAnything)
	{
		//重新进入接收模式
		startRecv();
		
		//结束transaction
		execSql("COMMIT");
	}

	sqlite3_finalize(stmt);

}


//打开数据库，并完成一些参数的初始化
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


//主程序入口
int main ( int argc, char **argv )
{
	printf( "Start SmartHome v2 dameon ... \n\r" );
	
	// database initialization
	initDatabase();

	//初始化NRF24L01+，并进入接收模式
	nrf24L01Init();	
	startRecv();

	while( TRUE ) 
	{

		//处理接收到的数据，如果有的话
		processReceivedData();

		//检查是否有数据需要发送到节点
		processDataToNode();

		//线程休眠300ms
		usleep( 300000 ); // sleep for x us
	}

	return 0;
}