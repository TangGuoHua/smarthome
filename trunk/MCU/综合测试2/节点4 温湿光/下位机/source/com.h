void serialPortInit(void)
{

	//28800bps@11.0592MHz
	PCON &= 0x7f; //�����ʲ�����
	SCON = 0x50;  //8λ����,�ɱ䲨���� ��ʽ1
	REN = 0;      //���������
	TMOD &= 0x0f; //�����ʱ��1ģʽλ
	TMOD |= 0x20; //�趨��ʱ��1Ϊ8λ�Զ���װ��ʽ
	TL1 = 0xFF;   //�趨��ʱ��ֵ
	TH1 = 0xFF;   //�趨��ʱ����װֵ
	ET1 = 0;      //��ֹ��ʱ��1�ж�
	//EA = 1;       //�����ж�
	//ES = 1;       //�򿪴����ж�
	ES=0;          //�رմ����ж�
	TR1 = 1;      //������ʱ��1
}


/*-----------
����һ���ֽ�
-----------*/
void serialPortSendByte( unsigned char dat )
{
	//ES=0; //�رմ����ж�
	SBUF = dat;
	while(!TI); //�ȴ��������
	TI = 0;  //�巢����ϱ�־
	//ES=1;  //���������ж�
}

//void SendStr(unsigned char *s)
//{
// while(*s!='\0')
//  {
//  SendByte(*s);
//  s++;
//  }
//}