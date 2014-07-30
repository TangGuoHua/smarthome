/* 
本程序是跑在Pi上的Daemon程序，
负责将各节点发来的数据存入数据库;
同时负责将一些数据发往各个节点。

编译命令行： gcc smartHome.c -o smartHome -l sqlite3

Todo:
1. 与单片机通讯码率提高到115200bps （需要单片机同时修改程序支持）
2. 修改通信协议：变长帧

修改记录：
Date         Author      Remarks
-------------------------------------------------------------------------------
2013-FEB-25  Changhao    增加用于处理计划任务的功能函数checkScheduledTask()
2013-SEP-30  Changhao    修改与MCU的通讯串口波特率为115200
2014-JUL-31  Changhao    优化sqlite的synchronous/journal_mode/temp_store三个参数
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


// ============= 串口通讯协议 ============
//Header0和Header1设计为一样的值，可以增强系统的容错性
//因为实验发现一帧数据的头一个字节有丢失的现象

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

//串口通信的状态
#define COM_STATUS_WAITING_FOR_FRAME_HEADER0 11
#define COM_STATUS_WAITING_FOR_FRAME_HEADER1 22
#define COM_STATUS_WAITING_FOR_FRAME_HEADER2 33
#define COM_STATUS_WAITING_FOR_FRAME_HEADER3 44
#define COM_STATUS_RECEIVING_DATA 55
#define COM_STATUS_WAITING_FOR_FRAME_TAILER1 66
#define COM_STATUS_WAITING_FOR_FRAME_TAILER2 77

//串口通信的buffer大小 (字节数）
//相当于从MCU发来的每帧数据的大小
#define MCU2PI_BUFFER_SIZE 45

//volatile int g_Stop = FALSE; 

// Create a handle for database connection, create a pointer to sqlite3
sqlite3 *g_dbHandle;

// serial port handle
int g_spHandle;

//打开串口，并初始化。
void initSerialPort()
{
	//int spHandle;
	struct termios newtio;
	
	g_spHandle = open( SERIAL_DEVICE, O_RDWR | O_NOCTTY ); //"/dev/ttyAMA0"
	if (g_spHandle <0) {perror("serial open error! "); exit(-1); }

	//tcgetattr(fd,&oldtio); //获取当前设置
	//tcsetattr(fd,TCSANOW,&oldtio); //恢复原来的设置

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


// 当串口收到一帧数据时，onFrameReceived()将会被调用
//
// 参数说明：
//    * data - the pointer to the data
//    dataLength - how many bytes of data received
//    目前单片机通过串口发来的数据都是定长的，每帧16字节（除帧头帧尾之外）。
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
        fprintf(stderr,"错误%s\n",sqlite3_errmsg(g_dbHandle));
        //fprintf(stderr,"错误%s\n",errmsg);
    }
}

// 发送数据到指定的节点
// Pi通过串口发送数据到MCU，再由MCU控制nrf24L01发送至节点
// sendRFChannel: 发送的nrf24L01频道数
// sendAddr1-3: 发送nrf24L01的地址字节1、2、3
// dataLength：发送的字节数 （1至32，不应该出现1－32以外的值)
// data: 指向发送数据的指针
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

//检查数据库，看是否有数据需要发送至节点
void checkSendDataToNode()
{
	int tmp;
	unsigned char sqlStr[250];
	sqlite3_stmt* stmt = NULL;
	int fldID, fldNodeID;
	unsigned char fldAddr1, fldAddr2, fldAddr3, fldRFChannel, fldDataLength;
	unsigned char buf[10];
	
	//选出需要往节点发送的数据	
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
			fprintf(stderr,"错误%s\n",sqlite3_errmsg(g_dbHandle));
			//fprintf(stderr,"错误%s\n",errmsg);
		}
	}
	sqlite3_finalize(stmt);
}

// 检查看是否有ScheduledTask该执行了
void checkScheduledTask()
{
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
}

main()
{
	int cnt, tmp;
	
	unsigned char readBuf[1]; //读取串口的buffer。因为每次只读1个字节，所以buffer只要1个字节
	unsigned char recvByte; // received byte
	
	unsigned char stillLoop; 
	
	// 通信状态初始化为等待header0
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
		
		do{ //当Pi收到一帧有效数据就会退出这个循环。
		
		
			//读取串口，如果有数据，则读一个字节到readBuf
			cnt = read( g_spHandle, readBuf, 1 );
			
			if( cnt > 0 )
			{ 
				// 如果串口收到数据，则处理数据
				recvByte = readBuf[0]; //得到接收的一个字节
				
				switch( comStatus )
				{
					case COM_STATUS_WAITING_FOR_FRAME_HEADER0:  //当前状态是等待header0
						if( recvByte == MCU2PI_FRAME_HEADER0 )  //收到了header0
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER1; //当前状态变为等待header1
						}
						break;
					
					case COM_STATUS_WAITING_FOR_FRAME_HEADER1:  //当前状态是等待header1
						if( recvByte == MCU2PI_FRAME_HEADER1 )  //收到了header1
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER2; //当前状态变为等待header2
						}
						else if( recvByte == MCU2PI_FRAME_HEADER2 )  //收到了header2。这是一个容错设计，参考上位机程序的注释
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER3; //当前状态变为等待header3
						}
						else
						{
							//状态退回等待header0
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
						}
						break;
						
					case COM_STATUS_WAITING_FOR_FRAME_HEADER2:  //当前状态是等待header2
						if( recvByte == MCU2PI_FRAME_HEADER2 )  //收到了header2
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER3; //当前状态变为等待header3
						}
						else
						{
							//状态退回等待header0
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
						}
						break;
						
					case COM_STATUS_WAITING_FOR_FRAME_HEADER3:  //当前状态是等待header3
						if( recvByte == MCU2PI_FRAME_HEADER3 )  //收到了header3
						{
							comStatus = COM_STATUS_RECEIVING_DATA; //当前状态变为接收数据
							bufferCount = 0; //清除buffer
						}
						else
						{
							//状态退回等待header0
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
						}
						break;
						
					//以上把帧头收齐了，下面开始接收数据
					
					case COM_STATUS_RECEIVING_DATA:  //当前状态是接收数据
						buffer[bufferCount++] = recvByte; //把接收的数据放入buffer
						if( recvByte == MCU2PI_FRAME_TAILER0 )  //收到了tailer0。当然有可能是巧合
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_TAILER1; //当前状态变为等待tailer1
						}
						break;
						
					case COM_STATUS_WAITING_FOR_FRAME_TAILER1:  //当前状态是等待tailer1
						buffer[bufferCount++] = recvByte; //把接收的数据放入buffer
						if( recvByte == MCU2PI_FRAME_TAILER1 )  //收到了tailer1。当然有可能是巧合
						{
							comStatus = COM_STATUS_WAITING_FOR_FRAME_TAILER2; //当前状态变为等待tailer2
						}
						else
						{
							comStatus = COM_STATUS_RECEIVING_DATA; //否则说明前面的数据是巧合，故当前状态变为接收数据
						}
						break;

					case COM_STATUS_WAITING_FOR_FRAME_TAILER2:  //当前状态是等待tailer2
						if( recvByte == MCU2PI_FRAME_TAILER2 )  //收到了tailer2。结束一帧
						{
							//已经收到tailer2，结束一帧
							
							//从缓存中去掉Tailer0和Tailer1
							bufferCount -=2;
							
							stillLoop = FALSE; // ends the loop
							
							comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0; //当前状态变为等待header0，为接收下一帧做准备
						}
						else
						{
							buffer[bufferCount++] = recvByte; //把接收的数据放入buffer
							comStatus = COM_STATUS_RECEIVING_DATA; //否则说明前面的数据是巧合，故当前状态变为接收数据
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
				//检查是否有计划任务
				checkScheduledTask();
				
				//检查是否有数据需要发送到节点
				checkSendDataToNode();
				
				//如果串口没有收到数据，则sleep 300ms
				//printf( "-" );
				usleep( 300000 ); // sleep for x us
			}
		}while( stillLoop);
		
		//printf( "onFrameReceived ... \n" );
		onFrameReceived( buffer, bufferCount );

	}
	
}