void serialPortInit(void)
{

	//28800bps@11.0592MHz
	PCON &= 0x7f; //波特率不倍速
	SCON = 0x50;  //8位数据,可变波特率 方式1
	REN = 0;      //不允许接收
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
void serialPortSendByte( unsigned char dat )
{
	//ES=0; //关闭串口中断
	SBUF = dat;
	while(!TI); //等待发送完毕
	TI = 0;  //清发送完毕标志
	//ES=1;  //开启串口中断
}

//void SendStr(unsigned char *s)
//{
// while(*s!='\0')
//  {
//  SendByte(*s);
//  s++;
//  }
//}