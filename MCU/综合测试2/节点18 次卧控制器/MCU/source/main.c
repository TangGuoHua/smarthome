//STC11F04E 1T , Internal RC 6.38MHz

#include <reg52.h>
#include "nrf24L01Node.h"
#include "am2321.h"

sbit RELAY_1 = P1^4;
sbit RELAY_2 = P1^5;
unsigned char relay1Mode = 0;
unsigned char relay2Mode = 0;
unsigned char relay1DelayOffTimerCount = 0; //1号继电器延时关 10s计数器
unsigned char tUpper, tLower; //温度上下限

void initINT0(void)
{
	EA=1;
	EX0=1; // Enable int0 interrupt.
}


void delay10s(void) 
{
	// 6.38MHz RC internal, 1T STC11F04E
    unsigned char a,b,c;
    for(c=215;c>0;c--) //5s
        for(b=252;b>0;b--)
            for(a=143;a>0;a--);

    for(c=215;c>0;c--) //5s
        for(b=252;b>0;b--)
            for(a=143;a>0;a--); 

}


//NRF24L01开始进入接收模式
void startRecv()
{
	unsigned char myAddr[3]= {97, 83, 192}; //本节点的接收地址
	nrfSetRxMode( 92, 3, myAddr);
}

////return 温度的10倍，温度四舍五入到0.1度
////例如：测得温度12.355，则返回124。
////      测得温度0.215，则返回2。
//int getTemperatureX10()
//{
//	float temperature;
//	int intTemperature;
//	
//	temperature = readTemperature();
//	
//	//四舍五入到十分位
//	intTemperature = temperature<0? (temperature*(-10)+0.5) : temperature*10+0.5; 
//	return intTemperature;
//}

//向主机发送当前的温度和继电器状态
void sendDataToHost( unsigned char readAm2321Result )
{
	unsigned char sendData[15];
	unsigned char toAddr[3]= {53, 69, 149}; //Pi, 96频道，3字节地址，接收15字节
	unsigned char tmp;
	
	sendData[0]=100;//ID0
	sendData[1]=18; //ID1
	sendData[2]= ~RELAY_1;
	sendData[3]= ~RELAY_2;
	sendData[4] = readAm2321Result;
	sendData[5] = getAM2321Data(0);
	sendData[6] = getAM2321Data(1);
	sendData[7] = getAM2321Data(2);
	sendData[8] = getAM2321Data(3);
	sendData[9] = getAM2321Data(4);
	
	tmp = nrfSendData( 96, 3, toAddr, 15, sendData);//Pi, 96频道，3字节地址，接收15字节
	
	//24L01开始接收数据
	startRecv(); 
}



void main()
{

	unsigned char sendDataTimerCount = 0;
	unsigned char readAm2321Result = 0;
	int temperatureX10 = 0;
	
	RELAY_1=1;
	RELAY_2=1;
	
	
	//初始化中断0
	initINT0();
	
	//init 2321
	initAM2321();

	//初始化24L01
	nrf24L01Init();
	
	//24L01开始接收数据
	startRecv(); 
			
	while(1)
	{
		
		//触发AM3231测量，并读取上一次测量的温湿度值
		readAm2321Result = readAM2321();
		if( readAm2321Result == 0 ) //读取am2321成功
		{
			//获得当前温度 10倍数
			temperatureX10 = getAM2321Data(2)*10 + getAM2321Data(3);
			if( getAM2321Data(4) == 1 )
			{
				//负温度
				temperatureX10 *= -1;
			}
			
			// 工作在自动温控模式
			if(relay2Mode==2)
			{
				if( temperatureX10>=tUpper )
				{
					RELAY_2 = 1; //关继电器，停止加热
					//sendDataToHost( temperatureX10, 0 );
				}
				else if( temperatureX10<=tLower )
				{
					RELAY_2 = 0; //开继电器，开始加热
					//sendDataToHost( temperatureX10, 0 );
				}
			}	
		}
		

		//继电器延时关
		if( relay1Mode == 2 && RELAY_1 == 0 )
		{
			if( relay1DelayOffTimerCount < 9 ) //如果设置为9，则延时9*10＝90 ～ 100秒关闭1号继电器
			{
				relay1DelayOffTimerCount++;
			}
			else
			{
				RELAY_1 = 1; //关继电器1
			}
		}
		
		
		//向主机发送数据
		if( ++sendDataTimerCount >= 1 )  //如果设置为30，则每30*10＝300秒发送一次数据
		{
			sendDataToHost( readAm2321Result );	
			sendDataTimerCount=0;
		}
		
		
		delay10s();
	}
}



//外部中断0处理程序
//用于处理来自24L01的中断
void interrupt24L01(void) interrupt 0
{
	unsigned char * receivedData;
	
	//获取接收到的数据
	receivedData = nrfGetReceivedData();
	
	//字节1，控制1号继电器工作模式（0：常关，1：常开，2：延时关）
	relay1Mode = *(receivedData++);
	if( relay1Mode == 0 ) RELAY_1=1;
	else if( relay1Mode == 1 ) RELAY_1=0;
	else if( relay1Mode == 2 )
	{
		relay1DelayOffTimerCount = 0; //延时关计数清零
	}

	
	//字节2，控制2号继电器的工作模式 （0：常关，1：常开，2：温控）
	relay2Mode = *(receivedData++);
	if( relay2Mode == 0 ) RELAY_2=1;
	else if( relay2Mode == 1 ) RELAY_2=0;
	
	//字节3，温度上限 10倍数
	tUpper = *(receivedData++);
	
	//字节4，温度下限 10倍数
	tLower = *(receivedData++);
}
