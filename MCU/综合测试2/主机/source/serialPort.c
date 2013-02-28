/*
All rights are reserved.
Please contact huangchanghao@gmail.com if any questions

When        Who        Remarks
--------------------------------------
2011-OCT-05 Changhao   Initial version
*/

#include <reg52.h>
#include "dataType.h"

//Header0和Header1设计为一样的值，可以增强系统的容错性
//具体请参见上位机的MCUCommunicator类里面的注释

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

//PC to MCU的串口通信的状态
#define COM_STATUS_WAITING_FOR_FRAME_HEADER0 11
#define COM_STATUS_WAITING_FOR_FRAME_HEADER1 22
#define COM_STATUS_WAITING_FOR_FRAME_HEADER2 33
#define COM_STATUS_WAITING_FOR_FRAME_HEADER3 44
#define COM_STATUS_RECEIVING_DATA 55
#define COM_STATUS_WAITING_FOR_FRAME_TAILER1 66
#define COM_STATUS_WAITING_FOR_FRAME_TAILER2 77


//串口与PC通信的buffer大小 (字节数）
//相当于从PC发来的每帧数据的大小
#define PC2MCU_BUFFER_SIZE 45


/*-------------
串口通讯初始化
----------------*/
void serialPortInit(void)
{

	//28800bps@11.0592MHz
	PCON &= 0x7f; //波特率不倍速
	SCON = 0x50;  //8位数据,可变波特率 方式1
	REN = 1;      //允许接收
	TMOD &= 0x0f; //清除定时器1模式位
	TMOD |= 0x20; //设定定时器1为8位自动重装方式
	TL1 = 0xFF;   //设定定时初值
	TH1 = 0xFF;   //设定定时器重装值
	ET1 = 0;      //禁止定时器1中断
	//EA = 1;       //打开总中断
	//ES = 1;       //打开串口中断
	ES=0;          //关闭串口中断
	TR1 = 1;      //启动定时器1
}

/*-----------
发送一个字节
-----------*/
void serialPortSendByte( byte dat )
{
	//ES=0; //关闭串口中断
	SBUF = dat;
	while(!TI); //等待发送完毕
	TI = 0;  //清发送完毕标志
	//ES=1;  //开启串口中断
}
/*-------------
发送一个字符串
以 '\0'结束
----------------*/
void serialPortSendString( unsigned char *s )
{
	while(*s!='\0')
	{
		serialPortSendByte(*s);
		s++;
	}
}


//通过查询方式，接收PC发来的一帧数据
//如果没收到就循环等待，直到收到一帧数据
//返回接收到的数据指针
//注意，接收到的数据长度计算：（介于7至38个字节之间）
//设：字节6应等于LEN  (值为1至32）
//接收到的数据总长度等于 LEN+6
//
//PC给MCU的每帧格式固定为：
//字节0：节点ID字节0
//字节1：节点ID字节1
//字节2：节点24L01接收频道
//字节3：节点24L01接收地址字节0
//字节4：节点24L01接收地址字节1
//字节5：节点24L01接收地址字节2
//字节6：节点24L01接收数据字节数 (LEN=1至32，不应该出现1－32以外的值)
//字节7至6+LEN：要发送给节点的数据
byte* serialPortReceive(void)
{
	bit stillLoop = 1;
	byte recvByte;
	
	// 通信状态初始化为等待header0
	byte comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;

	byte bufferCount = 0;
	static byte buffer[PC2MCU_BUFFER_SIZE];
	//PC给MCU的每帧格式固定为：（即buffer[]数组里面的内容如下：）
	//字节0：节点ID字节0
	//字节1：节点ID字节1
	//字节2：节点24L01接收频道
	//字节3：节点24L01接收地址字节0
	//字节4：节点24L01接收地址字节1
	//字节5：节点24L01接收地址字节2
	//字节6：节点24L01接收数据字节数 (LEN=1至32，不应该出现1－32以外的值)
	//字节7至6+LEN：要发送给节点的数据
	
	do
	{
		//循环等待，直到串口接收到数据
		while(!RI);
		RI=0;
		
		//取出收到的一个字节
		recvByte = SBUF;
		
		//serialPortSendByte( recvByte );
		
		//serialPortSendByte( comStatus );
		
		switch( comStatus )
		{
			case COM_STATUS_WAITING_FOR_FRAME_HEADER0:  //当前状态是等待header0
				if( recvByte == PC2MCU_FRAME_HEADER0 )  //收到了header0
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER1; //当前状态变为等待header1
				}
				break;
			
			case COM_STATUS_WAITING_FOR_FRAME_HEADER1:  //当前状态是等待header1
				if( recvByte == PC2MCU_FRAME_HEADER1 )  //收到了header1
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER2; //当前状态变为等待header2
				}
				else if( recvByte == PC2MCU_FRAME_HEADER2 )  //收到了header2。这是一个容错设计，参考上位机程序的注释
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
				if( recvByte == PC2MCU_FRAME_HEADER2 )  //收到了header2
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
				if( recvByte == PC2MCU_FRAME_HEADER3 )  //收到了header3
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
				if( recvByte == PC2MCU_FRAME_TAILER0 )  //收到了tailer0。当然有可能是巧合
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_TAILER1; //当前状态变为等待tailer1
				}
				break;
				
			case COM_STATUS_WAITING_FOR_FRAME_TAILER1:  //当前状态是等待tailer1
				buffer[bufferCount++] = recvByte; //把接收的数据放入buffer
				if( recvByte == PC2MCU_FRAME_TAILER1 )  //收到了tailer1。当然有可能是巧合
				{
					comStatus = COM_STATUS_WAITING_FOR_FRAME_TAILER2; //当前状态变为等待tailer2
				}
				else
				{
					comStatus = COM_STATUS_RECEIVING_DATA; //否则说明前面的数据是巧合，故当前状态变为接收数据
				}
				break;

			case COM_STATUS_WAITING_FOR_FRAME_TAILER2:  //当前状态是等待tailer2
				if( recvByte == PC2MCU_FRAME_TAILER2 )  //收到了tailer2。结束一帧
				{
					//已经收到tailer2，结束一帧
					
					//从缓存中去掉Tailer0和Tailer1
					bufferCount -=2;
					
					stillLoop = 0; // ends the loop
					
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
		
		//serialPortSendByte( comStatus );
		
		//buffer满了
		if( bufferCount == PC2MCU_BUFFER_SIZE)
		{
			//正常情况下，buffer不应该满
			//buffer能被用到的最大值是 6+32+2＝40
			//如果buffer满了都没有把一帧收完，说明这帧出错了，要丢弃本帧
			
			// 通信状态初始化为等待header0，为下一帧接收做准备
			comStatus = COM_STATUS_WAITING_FOR_FRAME_HEADER0;
			
			//清掉buffer
			bufferCount = 0;
		}


	}while( stillLoop );
	

	return buffer;
}

// 通过串口发送一帧数据给上位机
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