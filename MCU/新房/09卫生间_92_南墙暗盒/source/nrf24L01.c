/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2013年10月01日  黄长浩  初始版本
2014年10月01日  黄长浩  修改nrfSendData()函数以支持地址宽度设置和发送失败后重试Retry
                        修改nrfSetRxMode()函数以支持地址宽度设置

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#include <reg52.h>
#include "nrf24L01.h"
#include "nrf24L01Node.h"

#define SEND_MAX_RETRY 5  //发送数据时，如果失败，最大重试次数 (值可以为 0至9）

unsigned char nrfACK();
unsigned char nrfCheckACK();
unsigned char nrfSPI( unsigned char spiData );

unsigned char nrfReadReg(  unsigned char regAddr );
unsigned char nrfWriteReg( unsigned char regAddr, unsigned char writeData);
unsigned char nrfReadRxData( unsigned char regAddr, unsigned char *rxData, unsigned char dataLen);
unsigned char nrfWriteTxData(unsigned char regAddr, unsigned char *txData, unsigned char dataLen);

/****************************************************
函数功能:延时 15us
*****************************************************/
void delayFor24L01( )
{
	unsigned int i;
	unsigned int t;
	for( t=1; t>0; t-- )
		for( i=305; i>0; i-- ); // 1T STC11F04E 外部晶振4MHz
}


//********* 以下函数仅供本模块内部调用 **************
//SPI时序函数
unsigned char nrfSPI( unsigned char spiData )
{
	unsigned char i;
	for(i=0;i<8;i++) //循环8次
	{
		if( spiData & 0x80 )
			MOSI=1;
		else
			MOSI=0; //byte最高位输出到MOSI
		spiData<<=1; //低一位移位到最高位
		SCLK=1; 
		if(MISO) //拉高SCK，nRF24L01从MOSI读入1位数据，同时从MISO输出1位数据
			spiData|=0x01; //读MISO到byte最低位
		SCLK=0; //SCK置低
	}
	return(spiData); //返回读出的一字节
}

//SPI读寄存器一字节函数
unsigned char nrfReadReg( unsigned char regAddr )
{
	unsigned char returnData;
	CSN=0;//启动时序
	nrfSPI( regAddr );//写寄存器地址
	returnData=nrfSPI(0x00);//写入读寄存器指令  
	CSN=1;
	return( returnData ); //返回状态
}

//SPI写寄存器一字节函数
unsigned char nrfWriteReg( unsigned char regAddr, unsigned char writeData )
{
	unsigned char returnData;
	CSN=0;//启动时序
	returnData=nrfSPI(regAddr);//写入地址
	nrfSPI(writeData);//写入值
	CSN=1;
	return(returnData);
}

//SPI读取RXFIFO寄存器的值
unsigned char nrfReadRxData( unsigned char regAddr, unsigned char *rxData, unsigned char dataLen )
{  //寄存器地址//读取数据存放变量//读取数据长度//用于接收
	unsigned char returnData,i;
	CSN=0;//启动时序
	returnData=nrfSPI(regAddr);//写入要读取的寄存器地址
	for(i=0;i<dataLen;i++) //读取数据
	{
		rxData[i]=nrfSPI(0);
	} 
	CSN=1;
	return(returnData); 
}


//SPI写入TXFIFO寄存器的值
unsigned char nrfWriteTxData( unsigned char regAddr, unsigned char *txData, unsigned char dataLen )
{ //寄存器地址//写入数据存放变量//读取数据长度//用于发送
	unsigned char returnData,i;
	CSN=0;
	returnData=nrfSPI(regAddr);//写入要写入寄存器的地址
	for(i=0;i<dataLen;i++)//写入数据
	{
		nrfSPI(*txData++);
	}
	CSN=1;
	return(returnData);
}



//******* 以下函数供外部模块调用 ************

//NRF24L01初始化函数
void nrf24L01Init()
{
	delayFor24L01();//让系统什么都不干
	delayFor24L01();//让系统什么都不干
	CE=0; //待机模式1 (Standy-I)
	CSN=1;
	SCLK=0;
	IRQ=1;
	
	/***下面这些寄存器的配置，在这个程序运行期间不变化，故在初始化芯片时配置。***/
	nrfWriteReg( W_REGISTER+EN_AA, 0x01 );     // 使能接收通道0自动应答
	nrfWriteReg( W_REGISTER+EN_RXADDR, 0x01 ); // 使能接收通道0

	//nrfWriteReg( W_REGISTER+SETUP_AW, 0x03 ); // Set up address width to 5 bytes
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0x1f ); // 自动重发延时等待500us，自动重发15次
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0x3f ); // 自动重发延时等待1000us，自动重发15次
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0x5f ); // 自动重发延时等待1500us，自动重发15次
	nrfWriteReg( W_REGISTER+SETUP_RETR,0x7f ); // 自动重发延时等待2000us，自动重发15次
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0xbf ); // 自动重发延时等待3000us，自动重发15次
	//nrfWriteReg( W_REGISTER+SETUP_RETR,0xff ); // 自动重发延时等待4000us，自动重发15次

	nrfWriteReg( W_REGISTER+STATUS, 0x7e ); //清除RX_DR,TX_DS,MAX_RT状态位
	nrfWriteReg( W_REGISTER+CONFIG, 0x7e ); //屏蔽3个中断，CRC使能，2字节CRC校验，上电，PTX
}


// 将24L01设置为发送模式PTX，并发送数据
// 参数如下：
// rfChannel: 无线电频道  取值范围是0-125（即，0x00到0x7d）
// addrWidth: 发送地址宽度
// txAddr: 发送的地址（接收方地址）
// dataWidth: 发送数据的宽度
// txData: 发送的数据
// 例子：
// unsigned char rfChannel = 0x64; //选择无线电频道
// unsigned char rec_addr[3]= { 0x54, 0x53, 0x95 };  //接收方地址
// unsigned char data_to_send[5] = {0x01, 0x02, 0x03, 0x04, 0x05 };　//要发送的数据
// nrfSendData( rfChannel, 3, rec_addr, 5, data_to_send );  //发送
//
// 返回值：
// 255-表示大重试次数达到后仍然未收到ACK，发送失败
// 0到159的一个值，表示发送完成且成功。返回值是自动重传(auto retransmit)的次数，例如：
//     返回0：没有re-transmit，直接发送成功
//     返回1: 表示芯片re-transmit 1次成功
//     返回2：表示芯片re-transmit 2次成功
//     以此类推，最多re-transmit 159次成功。(SEND_MAX_RETRY最大可以设为9）
unsigned char nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData )
{
	unsigned char ret = 0;
	unsigned char retryCnt = 0;
	
	CE=0;
	
	nrfWriteReg( W_REGISTER+SETUP_AW, addrWidth-2 ); //设置地址宽度
	nrfWriteTxData( W_REGISTER+TX_ADDR, txAddr, addrWidth );//写寄存器指令+接收地址使能指令+接收地址+地址宽度
	nrfWriteTxData( W_REGISTER+RX_ADDR_P0, txAddr,addrWidth );//为了应答接收设备，接收通道0地址和发送地址相同
	 

	nrfWriteReg( W_REGISTER+RF_CH, rfChannel ); // 选择射频通道
	nrfWriteReg( W_REGISTER+CONFIG,0x7e ); //屏蔽3个中断，CRC使能，2字节CRC校验，上电，PTX
	
	for( retryCnt=0; retryCnt<= SEND_MAX_RETRY; retryCnt++ )
	{
		nrfWriteTxData( W_TX_PAYLOAD, txData, dataWidth );//写入要发送的数据
		
		CE=1;
		delayFor24L01();
		CE=0; //待发送完毕后转为Standby-1模式
	
		do
		{
			ret=nrfCheckACK();
		}while( ret==100);//检测是否发送完毕
		
		//如果返回值小于等于15，说明本次数据传送成功
		if( ret<=15 ) 
		{
			//计算函数返回值，即Retransmission的次数
			//一次性发送成功，返回0，即没有重传(re-transmission)
			//retry第1次时，一次性发送成功，则retransit 16次 （之前15次加本次）
			//一次类推，
			//retry第9次时，（加开始retry之前的一次，共发射了10次），retransmit 15次成功，则，总共retransmit了16*9+15=159次
			//每次try是transmit 16次 （本身1次，加15次retransmit）
			ret += (16*retryCnt);
			break;
		}
	}
	
	return ret;
}


//设置24L01为接收模式PRX，准备接收数据
//参数如下：
//rfChannel：接收的频道号。取值范围是0－125
//addrWidth: 地址宽度（3-5字节）
//rxAddr：本节点接收的地址
//本节点的接收数据宽度在"nrf24L01Node.h"头文件里面设置
//例：
//   unsigned char myAddr[3] = {53, 69, 160};
//   nrfSetRxMode( 76, 3, myAddr);
//那么节点将在76频道上接收数据。地址宽度为3字节，地址是：53/69/160。
//一旦接收到数据，将触发INT0 (硬件接线提示：IRQ需连接到INT0上）
void nrfSetRxMode(  unsigned char rfChannel, unsigned char addrWidth, unsigned char *rxAddr)
{
    CE=0;

	nrfWriteReg( W_REGISTER+SETUP_AW, addrWidth-2 ); //设置地址宽度
  	nrfWriteTxData( W_REGISTER+RX_ADDR_P0, rxAddr, addrWidth ); //接收设备接收通道0使用和发送设备相同的发送地址


	nrfWriteReg( W_REGISTER+RF_CH, rfChannel ); //设置射频通道
  	nrfWriteReg( W_REGISTER+RX_PW_P0, RECEIVE_DATA_WIDTH ); //接收通道0选择和发送通道相同有效数据宽度

	//nrfWriteReg( W_REGISTER+RF_SETUP, 0x26 ); // 数据传输率250Kbps，发射功率0dBm
	nrfWriteReg( W_REGISTER+RF_SETUP, 0x27 ); // 数据传输率250Kbps，发射功率0dBm, LNA_HCURR (Low Noise Amplifier, High Current?)
	//nrfWriteReg( W_REGISTER+RF_SETUP,0x21 ); // 数据传输率250Kbps，发射功率-18dBm, LNA_HCURR (Low Noise Amplifier, High Current?)
	nrfWriteReg( W_REGISTER+CONFIG, 0x3f ); //使能RX_DR中断，屏蔽TX_DS和MAX_RT中断，CRC使能，2字节CRC校验，上电，接收模式

  	CE = 1;	//设为接收模式 PRX
}


// 用于检查发送结果(Ack)
// 返回值：100-表示还在发送中
//         255-表示大重发次数达到后仍然未收到ACK，发送失败
//         0到15的一个值，表示发送完成且成功。返回值是自动重发的次数，例如：
//           0：没有重发，直接发送成功
//           1: 重发了1次后成功收到ack
//           2: 重发了2次后成功收到ack
//           以此类推
//           最大值是SETUP_RETR这个寄存器里面设置的最大重发次数。（不会超过15）
unsigned char nrfCheckACK()
{  
	unsigned char status;
	
	status = nrfReadReg(R_REGISTER+STATUS); // 读取状态寄存器
	
	if( status & 0x20 ) //检查TX_DS位，置位则发送成功
	{
		nrfWriteReg(W_REGISTER+STATUS,0x7f);  // 清除TX_DS标志
		
		//发送成功后，FIFO自动清空，这里就不用再清了
		
		//返回自动重发次数
		return( nrfReadReg(R_REGISTER+OBSERVE_TX) & 0x0f ); 
	}
	else if( status & 0x10) //检查MAX_RT位，置位则说明最大重发次数达到后仍然未收到ACK，发送失败
	{
		nrfWriteReg(W_REGISTER+STATUS,0x7f);  // 清除MAX_RT标志
		
		//发送失败，FIFO不会自动清空，必须手动清空 ！！
		//关键！！不然会出现意想不到的后果！！！
		CSN=0;
		nrfSPI(FLUSH_TX);
		CSN=1; 
			
		return 255;
	}
	else //还在发送中...
	{
		return 100;
	}
}

//获取24L01接收到的数据。
//当24L01收到数据触发中断后，调用本方法来取得24L01收到的数据
unsigned char* nrfGetReceivedData()				 
{
	static unsigned char dataBuffer[RECEIVE_DATA_WIDTH];
	unsigned char status;
	
	//读取状态寄存器
	status = nrfReadReg(R_REGISTER+STATUS);
	if( status & 0x40 ) //检查RX_DR位，如果置位，则说明接收到数据
	{
		//CE=0;//进入Standby-I模式
		
		// 从RX FIFO读取数据
		nrfReadRxData(R_RX_PAYLOAD,dataBuffer,RECEIVE_DATA_WIDTH);
		
		//接收到数据后RX_DR,TX_DS,MAX_PT都置高为1，通过写1来清除中断标
		//nrfWriteReg(W_REGISTER+STATUS,0xff);
		//网上找到的例子是用0xff来清中断标志，我通过试验发现用0x40也可以清掉RX_DR
		nrfWriteReg(W_REGISTER+STATUS,0x40);
		
		//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！
		//我经过试验发现，不清FIFO的话，发送方有时候会出现收不到ACK的现象
		CSN=0;
		nrfSPI(FLUSH_RX); 
		CSN=1;
	}
	return dataBuffer;
} 