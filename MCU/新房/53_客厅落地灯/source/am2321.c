/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2014��01��26��  �Ƴ���  ��ʼ�汾
2014��10��07��  �Ƴ���  ����һЩ��ʱ����

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/
//
// STC11F04E 1T��Ƭ�� 4MHz���� ���Գɹ�
//

#include <reg52.h>
#include "am2321.h"

static unsigned char AM2321_Data[5]={0x00,0x00,0x00,0x00,0x00};

void am2321Delay2S(void)
{
	//����Ϊ4MHzʱ 1T STC11F04E
	//��ʱ2.5s
    unsigned char a,b,c,n;
    for(c=165;c>0;c--)
        for(b=100;b>0;b--)
            for(a=150;a>0;a--);
    for(n=1;n>0;n--);
}

void am2321DelayMS()
{
	//����Ϊ4MHzʱ 1T STC11F04E
	//��ʱ2ms
    unsigned char a,b,c;
    for(c=7;c>0;c--)
        for(b=8;b>0;b--)
            for(a=34;a>0;a--);
}


/******************
��������: ��ʱ40us
*******************/
void am2321DelayUS()
{  
	//����Ϊ4MHzʱ 1T STC11F04E
	//��ʱ40us
    unsigned char a,b;
    for(b=11;b>0;b--)
        for(a=2;a>0;a--);
} 

//��������
void clearData(void)
{
	int i;
	for(i=0;i<5;i++)
	{
		AM2321_Data[i] = 0x00;
	}
}




// ��AM2321��ȡһ���ֽڵ�����
unsigned char am2321ReadOneByte(void)
{
	unsigned char i, temp, returnData;
	unsigned int loopCount;
	
	
	for(i=0;i<8;i++)	   
	{
		loopCount = 9000;
		while( !AM2321_SDA && loopCount-- ); //���͵�ƽ�Ƿ����
		
		//��ʱMin=26us Max50us, ��������"0"�ĸߵ�ƽ
		am2321DelayUS(); //��ʱ40us
		temp= AM2321_SDA?1:0;

		loopCount = 9000;
		while( AM2321_SDA && loopCount-- ); //�ȴ��ߵ�ƽ����
		//��ʱ������forѭ��		  
		
		returnData<<=1;
		returnData|=temp;
	}//rof
	
	return returnData;

}



//��ʼ��AM2321
//��Ҫ2.5�����ܷ���
void initAM2321()
{
	AM2321_SDA = 1;
	am2321Delay2S();  //��ʱ2.5s
}


/********************************************
���ܣ� ��������

������mode - ��ȡģʽ
  1 - ��ȡ1��AM2321��ֵ �����ؿ� ������ֵ����һ�β�����ֵ
  2 - �ȴ���һ��AM2321�������ȴ�2.9�룬�ٶ�ȡһ��AM2321��������Ҫ3�룬������ֵ�Ǳ��β�����ֵ

����ֵ��
  0 - ��ȡAM2321�ɹ�
  1 - AM2321δ��Ӧ��������û�нӣ�
  2 - ����������У��ͳ���       
  3,4 - ��ȡ���� ��General error��
  5 - ��ȡ�������ж���Ϊ�㣩
********************************************/
unsigned char readAM2321(unsigned char mode ) 
{
	unsigned char result = 0;
	unsigned char i;
	unsigned char checkSum;
	unsigned int cnt=0;
	unsigned int tmp;
	
	//�������
	clearData();
	
	//��������(Min=800US Max=20Ms) 
	AM2321_SDA = 0;
	am2321DelayMS();  //��ʱ2Ms
	//�ͷ����� ��ʱ(Min=30us Max=50us)
	AM2321_SDA = 1;
	
	if( mode==2)
	{
		am2321Delay2S();
		AM2321_SDA = 0;
		am2321DelayMS();  //��ʱ2Ms
		//�ͷ����� ��ʱ(Min=30us Max=50us)
		AM2321_SDA = 1;
	}

	am2321DelayUS(); //��ʱ40us
	
	
	//�жϴӻ��Ƿ��е͵�ƽ��Ӧ�ź� �粻��Ӧ����������Ӧ���������� 
	if(AM2321_SDA ==0)
	{
		//�յ���ʼ�ź�
		
		//�жϴӻ��Ƿ񷢳� 80us �ĵ͵�ƽ��Ӧ�ź��Ƿ����
		cnt=0;
		while(!AM2321_SDA)
		{
			if(++cnt>9000) //��ֹ������ѭ��
			{
				//��ȡ����
				return 3;
			} 
		}
		
		cnt = 0;
		//�жϴӻ��Ƿ񷢳� 80us �ĸߵ�ƽ���緢����������ݽ���״̬
		while((AM2321_SDA))
		{
			if(++cnt>9000) //��ֹ������ѭ��
			{
				//��ȡ����
				return 4;
			}
		}
		
		// ���ݽ���	������������40λ����
		// ��5���ֽ� ��λ����  5���ֽڷֱ�Ϊʪ�ȸ�λ ʪ�ȵ�λ �¶ȸ�λ �¶ȵ�λ У���
		// У���Ϊ��ʪ�ȸ�λ+ʪ�ȵ�λ+�¶ȸ�λ+�¶ȵ�λ
		for(i=0;i<5;i++)
		{
			AM2321_Data[i] = am2321ReadOneByte();
		}
		
		checkSum = AM2321_Data[0]+AM2321_Data[1]+AM2321_Data[2]+AM2321_Data[3];
		
		if( checkSum == 0 )
		{
			//��ȡ����һ����˵��ʪ�Ȳ���ͬʱΪ0
			return 5;
		}
		
		if( AM2321_Data[4] != checkSum )
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
	tmp = AM2321_Data[0]*256 + AM2321_Data[1];
	AM2321_Data[0] = tmp/10; //ȡ����������
	AM2321_Data[1] = tmp%10; //ȡ��С������
	
	//�¶�
	if( (AM2321_Data[2] & 0x80) == 0x80 )
	{
		AM2321_Data[4]=1;
		AM2321_Data[2] = AM2321_Data[2] & 0x7f;
	}
	else
	{
		AM2321_Data[4]=0;
	}
	
	tmp = AM2321_Data[2]*256 + AM2321_Data[3];
	AM2321_Data[2] = tmp/10; //ȡ����������
	AM2321_Data[3] = tmp%10; //ȡ��С������
	
	return result;
}


/********************************
���ܣ�ȡ�ô�AM2321������ֵ

���ȣ�����readAM2321()��������ȷ���䷵��ֵΪ0����ȡAM2321�ɹ�����
������ñ������õ��������ǲ���ȷ�ġ�

Index   Value 
-----------------------------------
0       ʪ����������
1       ʪ��С������
2       �¶���������
3       �¶�С������
4       ���¶ȱ�� ��0:���¶ȣ�1:���¶ȣ�

��1��ʪ��53.8% �¶�18.3������ֵ���£�
getAM2321Data(0)  53
getAM2321Data(1)  8
getAM2321Data(2)  18
getAM2321Data(3)  3
getAM2321Data(4)  0 ��0��ʾ���¶ȣ�

��2��ʪ��20.5% �¶�-2.6������ֵ���£�
getAM2321Data(0)  20
getAM2321Data(1)  5
getAM2321Data(2)  2
getAM2321Data(3)  6
getAM2321Data(4)  1 ��1��ʾ���¶ȣ�

*********************************/
unsigned char getAM2321Data( unsigned char index )
{
	return AM2321_Data[index];
}
