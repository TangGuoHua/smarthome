/***************************************************************************
��Ӳ����Ϣ��
��Ƭ���ͺ�: STC12C5616AD
����Ƶ��: 4MHz �ⲿ����
������
����DHT11��ʪ�ȴ�����


���޸���ʷ��
����            ����    ��ע
----------------------------------------------------------------------
2014��09��04��  �Ƴ���  ��ʼ�汾

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#include <reg52.h>
#include "dht11.h"

static unsigned char DHT11_HUMIDITY=255;
static unsigned char DHT11_TEMPERATURE=255;

void dht11Delay10Us()
{
    unsigned char a;
    for(a=7;a>0;a--);
}


void dht11Delay20Ms(void)   //��� 0us
{
    unsigned char a,b,c;
    for(c=1;c>0;c--)
        for(b=203;b>0;b--)
            for(a=97;a>0;a--);
}

void dht11Delay5S(void)   //��� 0us
{
    unsigned char a,b,c,n;
    for(c=189;c>0;c--)
        for(b=191;b>0;b--)
            for(a=137;a>0;a--);
    for(n=3;n>0;n--);

}

// ��DTH11��ȡһ���ֽڵ�����
unsigned char getOneByte(void)
{
	unsigned char i, temp, returnData;
	unsigned int loopCount;
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 2000;
		while( !DHT11 && loopCount-- );
		
		//delay 30us
		dht11Delay10Us();
		dht11Delay10Us();
		dht11Delay10Us();
		
		temp= DHT11?1:0;

		loopCount = 2000;
		while( DHT11 && loopCount-- );
		//��ʱ������forѭ��		  
		
		returnData<<=1;
		returnData|=temp;
	}//rof
	
	return returnData;

}

//����һ�β���
void triggerMesurment()
{
	//��������20ms (Ҫ�����18ms)
	DHT11=0;
	dht11Delay20Ms();
	DHT11=1;
}


/////////���²��ֺ������ⲿ�������/////////////////////

/********************************************
���ܣ� ��������

������mode - ��ȡģʽ
  1 - ��ȡDHT11��ֵ������������ ��
      ע�⣬ͨ����ģʽ��ȡDHT11�󣬵���getDHT11Humidity()��getDHT11Temperature()�õ�����ֵ����һ�β�����ֵ
	  
  2 - �ȴ���һ��DHT11�������ȴ�5�룬�ٶ�ȡDHT11��
      ע�⣬ͨ����ģʽ��ȡDHT11��Ҫ5���Ӳ��ܷ��ء�
	  ������getDHT11Humidity()��getDHT11Temperature()�õ����β�����ֵ

����ֵ��
  0 - ��ȡDHT11�ɹ�
  1 - DHT11δ��Ӧ��������û�нӣ�
  2 - ����������У��ͳ���       
  3 - ��ȡ���� ��General error��
  
ע�⣬һ��Ҫ��֤readDHT11()����ֵΪ0ʱ������ͨ������getDHT11Humidity()��getDHT11Temperature()�õ���ȷ����ʪ��ֵ��
����getDHT11Humidity()��getDHT11Temperature()������255����ʾ��ȡDHT11ʱ������
**********************************************************************************************************/
unsigned char readDHT11( unsigned char mode)
{
	unsigned int loopCount;
	unsigned char temp;
	unsigned char tempHHigh, tempHLow, tempTHigh, tempTLow, tempCheckSum;
	
	//�����ȡDHT11ʧ�ܣ���getDHT11Humidity()��getDHT11Temperature()������255
	DHT11_HUMIDITY = 255;
	DHT11_TEMPERATURE = 255;

	if(mode==2)
	{
		triggerMesurment();
		dht11Delay5S();
	}
	
	//��������20ms (Ҫ�����18ms)
	DHT11=0;
	dht11Delay20Ms();
	DHT11=1;
	
	//������������������ ������ʱ20us
	dht11Delay10Us();
	dht11Delay10Us();
	
	//������Ϊ���� �жϴӻ���Ӧ�ź� 
	DHT11=1;
	dht11Delay10Us();
	
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ����������	  
	if(!DHT11)
	{
		loopCount = 2000;
		//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ���������Ƿ�ʱ	 
		while( !DHT11 && --loopCount );
		if( loopCount==0 )
		{
			//��ʱ
			return 3;
		}
		
		loopCount = 2000;
		//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬�����Ƿ�ʱ
		while( DHT11 && --loopCount  );
		if( loopCount==0 )
		{
			//��ʱ
			return 3;
		}
				
		//���ݽ���״̬����ȡ5���ֽ�
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
			// ��ȡ������Ч���ɹ�
			DHT11_HUMIDITY = tempHHigh;
			DHT11_TEMPERATURE = tempTHigh;
			
			return 0;
		}
	}

	//�ӻ�û����Ӧ
	return 1;
}


//ȡ��ʪ��ֵ
//ע�⣬һ��ҪreadDHT11()����ֵΪ0ʱ������ͨ������getDHT11Humidity()�õ���ȷ��ʪ��ֵ
//����getHumidity()������255
unsigned char getDHT11Humidity()
{
	return DHT11_HUMIDITY;
}


//ȡ���¶�ֵ
//ע�⣬һ��ҪreadDHT11()����ֵΪ0ʱ������ͨ������getDHT11Temperature()�õ���ȷ���¶�ֵ
//����getDHT11Temperature()������255
unsigned char getDHT11Temperature()
{
	return DHT11_TEMPERATURE;
}


//��ʼ��DHT11
void initDHT11()
{
	DHT11 = 1;
}
