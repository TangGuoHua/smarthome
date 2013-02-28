/*
When        Who        Remarks
----------------------------------------------------------------------
2011-SEP-25 Changhao   Initial version
2011-OCT-07 Changhao   �ӳ�getHumidity()���������β������ʱ�䵽2800ms�����ղż�⵽һ�����ݳ����һ����ǲ������ʱ�䲻����ɵģ�
*/

#include <reg52.h>
#include "dht11.h"

/********************************************\
|* ���ܣ� ��ʱ	
|* ��ʱ��Լ 1ms
\********************************************/ 
void dth11DelayMS(unsigned int t)
{
	unsigned int i;
	unsigned int j;
	for(j=t;j>0;j--)
		//for(i=848;i>0;i--);  //1T STC12C5608AD ����Ϊ11.0592M
		//for( i=530; i>0; i-- ); // 1T STC11F04E �ڲ�RC 6.97MHz
		for( i=305; i>0; i-- ); // 1T STC12C5608AD 4M����
}



/****************************************************
��������: ��ʱ30us
// 1T STC12C5608AD 4M����
*****************************************************/
void dht11Delay30us(void)   //��� 0us
{
    unsigned char a;
    for(a=27;a>0;a--);
}

// ��DTH11��ȡһ���ֽڵ�����
unsigned char getOneByte(void)
{
	unsigned char i, temp, returnData, loopCount;
	
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 2000;
		while( !DHT11 && loopCount-- );
		dht11Delay30us();
		temp= DHT11?1:0;

		loopCount = 2000;
		while( DHT11 && loopCount-- );
		//��ʱ������forѭ��		  
		
		returnData<<=1;
		returnData|=temp;
	}//rof
	
	return returnData;

}

// ��ȡDHT11���͵�5���ֽڣ���ֻ����ʪ��ֵ
unsigned char getAllBytes(void)
{
	unsigned char temp, loopCount;
	unsigned char tempHHigh, tempHLow, tempTHigh, tempTLow, tempCheckSum;
	unsigned char returnData;
	
	//�����ȡʪ�Ȳ��ɹ����򷵻�0xff�����򷵻����ʪ��ֵ
	returnData = 0xff;
	
	//��������20ms (Ҫ�����18ms)
	DHT11=0;
	dth11DelayMS(20);
	DHT11=1;
	//������������������ ������ʱ20us
	dht11Delay30us();
	//������Ϊ���� �жϴӻ���Ӧ�ź� 
	DHT11=1;
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	if(!DHT11)
	{
		loopCount = 2000;
		//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ���������Ƿ�ʱ	 
		while( !DHT11 && loopCount-- );
		
		loopCount = 2000;
		//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬�����Ƿ�ʱ
		while( DHT11 && loopCount--  );
		//���ݽ���״̬		 

		tempHHigh    = getOneByte();
		tempHLow     = getOneByte();
		tempTHigh    = getOneByte();
		tempTLow     = getOneByte();
		tempCheckSum = getOneByte();
		DHT11=1;
		
		//����У�� 
		
		temp = tempHHigh + tempHLow + tempTHigh + tempTLow;
		if( temp == tempCheckSum && temp>0 )
		{
			// ������Ч ����ʪ��
			returnData = tempHHigh;
		}

	}// end if
	
	return returnData;

}

//--------------------------------
//-----ʪ�ȶ�ȡ�ӳ��� ------------
//--------------------------------
// ȡ��ʪ��ֵ
// ������ģʽ��0��1
// ģʽ0������getHumidity(0)ʵ��ȡ�õ����ϴζ�ȡʱ�Ĳ��ʪ��ֵ�������������ء�
// ģʽ1������getHumidity(1)��ȡ�õ�ǰ��ʪ��ֵ�����Ǻ�����Ҫ2�����ϲŷ��ء�
// �����ȡʪ�Ȳ��ɹ����򷵻�0xff�����򷵻����ʪ��ֵ
unsigned char getHumidity(bit mode)
{
	if( mode )
	{
	 	//ÿ�ζ�ȡDHT11�Ĳ����������ϴβ�����ֵ��������һ�β���
	 	//���ԣ�Ϊ�˵õ����µ���ʪ��ֵ������������2�ζ�ȡ���Ի������ֵ
		getAllBytes();
		dth11DelayMS(2800); // ���ζ�ȡ���������2������
	}
	return getAllBytes();
}

//��ʼ��DHT11
void initDHT11(void)
{
	DHT11 = 1;
}
