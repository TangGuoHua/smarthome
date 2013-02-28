/*****************************************
//STC12c5608ad 4M External Crystal
*****************************************/
#include <reg52.h>
#include "nrf24L01.h"
#include "nrf24L01Node.h"



//extern unsigned char bdata sta;
/*********************************************************/



/*****************状态标志****************/
unsigned char bdata sta;   //状态标志
sbit RX_DR=sta^6;
sbit TX_DS=sta^5;
sbit MAX_RT=sta^4;

unsigned char nrfACK();
unsigned char nrfCheckACK();
unsigned char nrfSPI( unsigned char spiData );

unsigned char nrfReadReg(  unsigned char regAddr );
unsigned char nrfWriteReg( unsigned char regAddr, unsigned char writeData);
unsigned char nrfReadRxData( unsigned char regAddr, unsigned char *rxData, unsigned char dataLen);
unsigned char nrfWriteTxData(unsigned char regAddr, unsigned char *txData, unsigned char dataLen);


/****************************************************
函数功能:延时 15us
晶振为内部RC 6.9MHz(STC-ISP显示的频率)  STC11F04E 1T
*****************************************************/
//void delayFor24L01()
//{  
//
//    unsigned char a;
//    for(a=24;a>0;a--);
//    _nop_();  //if Keil,require use intrins.h
//} 


void delayFor24L01( )
{
	unsigned int i;
	unsigned int t;
	for( t=1; t>0; t-- )
		for( i=530; i>0; i-- ); // 1T STC11F04E 内部RC 6.97MHz
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
	CE=0;CE=0;CE=0;CE=0;CE=0;CE=0;
	CSN=1;
	SCLK=0;
	IRQ=1;
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
void nrfSendData( unsigned char rfChannel, unsigned char addrWidth, unsigned char *txAddr, unsigned char dataWidth, unsigned char *txData )
{
	CE=0;
	//delayUS( 10 ); // 本条语句由实验得出 STC12c5608AD 1T 11.0592MHz
	delayFor24L01();

	nrfWriteTxData( W_REGISTER+TX_ADDR, txAddr, addrWidth );//写寄存器指令+接收地址使能指令+接收地址+地址宽度
	nrfWriteTxData( W_REGISTER+RX_ADDR_P0, txAddr,addrWidth );//为了应答接收设备，接收通道0地址和发送地址相同
	nrfWriteTxData( W_TX_PAYLOAD, txData, dataWidth );//写入数据 

	/******下面有关寄存器配置**************/
	nrfWriteReg( W_REGISTER+EN_AA, 0x01 );       // 使能接收通道0自动应答
	nrfWriteReg( W_REGISTER+EN_RXADDR, 0x01 );   // 使能接收通道0
	
	
	//NRFWriteReg(W_REGISTER+SETUP_RETR,0x0a);  // 自动重发延时等待250us+86us，自动重发10次
	nrfWriteReg( W_REGISTER+SETUP_RETR,0x7a );  // 自动重发延时等待2000us+86us，自动重发10次
	nrfWriteReg( W_REGISTER+RF_CH, rfChannel ); // 选择射频通道0x64
	
	nrfWriteReg( W_REGISTER+RF_SETUP,0x26 ); // 数据传输率250Kbps，发射功率0dBm
	nrfWriteReg( W_REGISTER+CONFIG,0x7e ); //屏蔽3个中断，CRC使能，2字节CRC校验，上电，PTX
	
	CE=1;
	//delay10us();//保持10us秒以上
	//delay10us();
	//delayUS( 250 );  //本条语句由实验得出 STC12c5608AD 1T 11.0592MHz
	delayFor24L01();


	CE=0; //待发送完毕后转为Standby-1模式

	delayFor24L01(); //本条语句由实验得出 STC12c5608AD 1T 11.0592MHz
	//delayFor24L01();

	
	while(nrfCheckACK());	//检测是否发送完毕
}


// 设置24L01为接收模式PRX准备接收数据
// 本节点的接收射频通道、地址、地址宽度、数据宽度均在"nrf24L01Node.h"头文件里面设置
void nrfSetRxMode( void )
{
    CE=0;

	
  	nrfWriteTxData( W_REGISTER+RX_ADDR_P0, MY_ADDR, ADDRESS_WIDTH ); //接收设备接收通道0使用和发送设备相同的发送地址
  	nrfWriteReg( W_REGISTER+EN_AA, 0x01 ); //使能接收通道0自动应答
  	nrfWriteReg( W_REGISTER+EN_RXADDR, 0x01 ); //使能接收通道0

	nrfWriteReg( W_REGISTER+RF_CH, RECEIVE_RF_CHANNEL ); //设置射频通道
  	nrfWriteReg( W_REGISTER+RX_PW_P0, RECEIVE_DATA_WIDTH ); //接收通道0选择和发送通道相同有效数据宽度

	nrfWriteReg( W_REGISTER+RF_SETUP, 0x26 ); // 数据传输率250Kbps，发射功率0dBm
	nrfWriteReg( W_REGISTER+CONFIG, 0x3f ); //使能RX_DR中断，屏蔽TX_DS和MAX_RT中断，CRC使能，2字节CRC校验，上电，接收模式

  	CE = 1;	//设为接收模式 PRX


}

/****************************检测应答信号******************************/
unsigned char nrfCheckACK()
{  //用于发射
	sta=nrfReadReg(R_REGISTER+STATUS); // 返回状态寄存器
	if(TX_DS||MAX_RT) //发送完毕中断
	{
		nrfWriteReg(W_REGISTER+STATUS,0xff);  // 清除TX_DS或MAX_RT中断标志
		CSN=0;
		nrfSPI(FLUSH_TX);//用于清空FIFO ！！关键！！不然会出现意想不到的后果！！！大家记住！！  
		CSN=1; 
		return(0);
	}
	else
	{
		return(1);
	}
}


//获取24L01接收到的数据。
//当24L01收到数据触发中断后，调用本方法来取得24L01收到的数据
unsigned char* nrfGetReceivedData()				 
{
	static unsigned char dataBuffer[RECEIVE_DATA_WIDTH];
	
	sta=nrfReadReg(R_REGISTER+STATUS);//发送数据后读取状态寄存器
	if(RX_DR) // 判断是否接收到数据
	{
		//CE=0;//进入Standby-I模式
		
		nrfReadRxData(R_RX_PAYLOAD,dataBuffer,RECEIVE_DATA_WIDTH);// 从RXFIFO读取数据
		
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