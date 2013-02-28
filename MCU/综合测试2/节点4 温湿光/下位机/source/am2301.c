//
// ����Ϊ11.0592Mʱ  STC12C5608AD 1T��Ƭ�����Գɹ�
//

#include <reg52.h>
#include <intrins.h>
#include "am2301.h"

static unsigned char AM2301_Data[5]={0x00,0x00,0x00,0x00,0x00};


/********************************************\
|* ���ܣ� ��ʱ	����Ϊ11.0592Mʱ  STC12C5608AD 1T
|* ��ʱ��Լ 1ms
\********************************************/ 
void am2301DelayMS(unsigned int t)
{
	unsigned int i;
	unsigned int j;
	for(j=t;j>0;j--)
		for(i=848;i>0;i--);  //��ʱ��Լ 1ms  i=124(12T)  i=393 ���Ҽ���ģ�
}



/****************************************************
��������:��ȷ��ʱ����2*n��+1��us
����Ϊ11.0592Mʱ  STC12C5608AD 1T
*****************************************************/
void am2301DelayUS(unsigned char n)
{  
   unsigned char i;
   for(i=0;i<n;i++)
    {
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
	   _nop_();
    }
} 

//��������
void ClearData(void)
{
	int i;
	for(i=0;i<5;i++)
	{
		AM2301_Data[i] = 0x00;
	}
}


// ��AM2301��ȡһ���ֽڵ�����
unsigned char Read_SensorData(void)
{
	unsigned char i, temp, returnData, loopCount;
	
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 500;
		while( !AM2301_SDA && loopCount-- ); //���͵�ƽ�Ƿ����
		
		//��ʱMin=26us Max50us, ��������"0"�ĸߵ�ƽ
		am2301DelayUS( 18 ); //��ʱ30us
		temp= AM2301_SDA?1:0;

		loopCount = 500;
		while( AM2301_SDA && loopCount-- ); //�ȴ��ߵ�ƽ����
		//��ʱ������forѭ��		  
		
		returnData<<=1;
		returnData|=temp;
	}//rof
	
	return returnData;

}



/********************************************
���ܣ� ��������

����ֵ��
  0 - ��ȡAM2301�ɹ�
  1 - AM2301δ��Ӧ��������û�нӣ�
  2 - ����������У��ͳ���       
  3 - ��ȡ���� ��General error��

********************************************/
unsigned char readAM2301(void)
{
	unsigned char result = 0;
	unsigned char i;
	unsigned char checkSum;
	unsigned int cnt=0;
	unsigned int tmp;
	
	//�������
	ClearData();
	
	//��������(Min=800US Max=20Ms) 
	AM2301_SDA = 0;
	am2301DelayMS(3);  //��ʱ2Ms
	
	
	//�ͷ����� ��ʱ(Min=30us Max=50us)
	AM2301_SDA = 1;
	am2301DelayUS(16); //��ʱ30us
	
	
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ���������� 
	if(AM2301_SDA ==0)
	{
	
		//�յ���ʼ�ź�
		
		//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
		cnt=0;
		while((!AM2301_SDA))
		{
			if(++cnt>500) //��ֹ������ѭ��
			{
				//��ȡ����
				return 3;
			} 
		}
		
		cnt = 0;
		//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
		while((AM2301_SDA))
		{
			if(++cnt>500) //��ֹ������ѭ��
			{
				//��ȡ����
				return 3;
			}
		}
		// ���ݽ���	������������40λ����
		// ��5���ֽ� ��λ����  5���ֽڷֱ�Ϊʪ�ȸ�λ ʪ�ȵ�λ �¶ȸ�λ �¶ȵ�λ У���
		// У���Ϊ��ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ
		for(i=0;i<5;i++)
		{
			AM2301_Data[i] = Read_SensorData();
		}
		
		checkSum = AM2301_Data[0]+AM2301_Data[1]+AM2301_Data[2]+AM2301_Data[3];
		
		if( checkSum == 0 )
		{
			//��ȡ����һ����˵��ʪ�Ȳ���ͬʱΪ0
			return 3;
		}
		
		if( AM2301_Data[4] != checkSum )
		{
			//У��ͳ���
			return 2;
		}
	}
	else
	{
		return 1; // δ�յ���������Ӧ
	}
	
	//���е���������Ѿ�����ȷ�ض�����
	//���洦������
	
	//ʪ��
	tmp = AM2301_Data[0]*256 + AM2301_Data[1];
	AM2301_Data[0] = tmp/10; //ȡ����������
	AM2301_Data[1] = tmp%10; //ȡ��С������
	
	//�¶�
	if( (AM2301_Data[2] & 0x80) == 0x80 )
	{
		AM2301_Data[4]=1;
		AM2301_Data[2] = AM2301_Data[2] & 0x7f;
	}
	else
	{
		AM2301_Data[4]=0;
	}
	
	tmp = AM2301_Data[2]*256 + AM2301_Data[3];
	AM2301_Data[2] = tmp/10; //ȡ����������
	AM2301_Data[3] = tmp%10; //ȡ��С������
	
	return result;
}


/********************************
���ܣ�ȡ�ô�AM2301������ֵ

���ȣ�����readAM2301()��������ȷ���䷵��ֵΪ0����ȡAM2301�ɹ�����
������ñ������õ��������ǲ���ȷ�ġ�

Index   Value 
-----------------------------------
0       ʪ����������
1       ʪ��С������
2       �¶���������
3       �¶�С������
4       ���¶ȱ�� ��0:���¶ȣ�1:���¶ȣ�

��1��ʪ��53.8% �¶�18.3������ֵ���£�
getAM2301Data(0)  53
getAM2301Data(1)  8
getAM2301Data(2)  18
getAM2301Data(3)  3
getAM2301Data(4)  0 ��0��ʾ���¶ȣ�

��2��ʪ��20.5% �¶�-2.6������ֵ���£�
getAM2301Data(0)  20
getAM2301Data(1)  5
getAM2301Data(2)  2
getAM2301Data(3)  6
getAM2301Data(4)  1 ��1��ʾ���¶ȣ�

*********************************/
unsigned char getAM2301Data( unsigned char index )
{
	return AM2301_Data[index];
}
