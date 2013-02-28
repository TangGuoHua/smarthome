/*
When        Who        Remarks
----------------------------------------------------------------------
2011-SEP-25 Changhao   Initial version
2011-OCT-07 Changhao   �ӳ�getHumidity()���������β������ʱ�䵽2800ms�����ղż�⵽һ�����ݳ����һ����ǲ������ʱ�䲻����ɵģ�
*/

#include <reg52.h>

#include "delay.h"
#include "dht11.h"


// ��DTH11��ȡһ���ֽڵ�����
unsigned char getOneByte(void)
{
	unsigned char i, temp, returnData, loopCount;
	
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 200;
		while( !DHT11 && loopCount-- );
		delay10us();
		delay10us();
		delay10us();
		temp= DHT11?1:0;

		loopCount = 200;
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
	delay100us(200);
	DHT11=1;
	//������������������ ������ʱ20us
	delay10us();
	delay10us();
	delay10us();
	delay10us();
	//������Ϊ���� �жϴӻ���Ӧ�ź� 
	DHT11=1;
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	if(!DHT11)
	{
		loopCount = 200;
		//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ���������Ƿ�ʱ	 
		while( !DHT11 && loopCount-- );
		
		loopCount = 200;
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
unsigned char getHumidity(bit mode)
{
	if( mode )
	{
	 	//ÿ�ζ�ȡDHT11�Ĳ����������ϴβ�����ֵ��������һ�β���
	 	//���ԣ�Ϊ�˵õ����µ���ʪ��ֵ������������2�ζ�ȡ���Ի������ֵ
		getAllBytes();
		delayMS(2800); // ���ζ�ȡ���������2������
	}
	return getAllBytes();
}


void initDHT11(void)
{
	DHT11 = 1;
}
