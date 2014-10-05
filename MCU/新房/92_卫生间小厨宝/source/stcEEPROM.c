/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2011��10��06��  �Ƴ���  ��ʼ�汾
2014��02��09��  �Ƴ���  �޸�ENABLE_IAPΪ0x83��if SYSCLK<12MHz��
2014��02��09��  �Ƴ���  �޸�ΪSTC11Fϵͳ��Ƭ�����õĳ���
                        ���ӵ�Ƭ�������ͺ�˵��

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/


/*********************************
!!!�ر�ע��!!!

������������STC11Fxx��Ƭ��!!!

���Ҫ����������Ƭ����
����������ֲ��޸����¼Ĵ�����ַ
�Լ�Trigger����(isp_trig)
*********************************/

sfr isp_data=0xc2;
sfr isp_addrh=0xc3;
sfr isp_addrl=0xc4;
sfr isp_cmd=0xc5;
sfr isp_trig=0xc6;
sfr isp_contr=0xc7;

//#define ENABLE_IAP 0x80  //if SYSCLK<30MHz
//#define ENABLE_IAP 0x81  //if SYSCLK<24MHz
//#define ENABLE_IAP 0x82  //if SYSCLK<20MHz
#define ENABLE_IAP 0x83  //if SYSCLK<12MHz
//#define ENABLE_IAP 0x84  //if SYSCLK<6MHz
//#define ENABLE_IAP 0x85  //if SYSCLK<3MHz
//#define ENABLE_IAP 0x86  //if SYSCLK<2MHz
//#define ENABLE_IAP 0x87  //if SYSCLK<1MHz

//����Ƭ��EEPROM��һ������
//����ֻ��������Ϊ��С��λ���У�û��ֻ����һ���ֽ�
//һ��������512���ֽ�
//��������������ĵ�ַ�����ĸ�������������������ݶ���������
//���磺STC89C51RCƬ��EEPROM��һ������ʼ��ַΪ0x2000��������ַΪ0x21ff
//������� eepromEraseSector(0x2001)�����һ���������ݶ���������
//�����ɹ��󣬸������ڸ��ֽڶ�����Ϊ0xff
void eepromEraseSector (unsigned int address)
{
	unsigned char i;
	isp_addrl=address;
	isp_addrh=address>>8;
	isp_contr=0x01; 
	isp_contr=isp_contr|ENABLE_IAP; // 0x80 if SYSCLK<40MHz, 0x81 if SYSCLK<20MHz, 0x82 if SYSCLK<10MHz, 0x83 if SYSCLK<5MHz
	isp_cmd=0x03; 
	isp_trig=0x5a;  //��ͬ��Ƭ��Trigger���ͬ
	isp_trig=0xa5;
	
	for(i=0;i<5;i++);
	
	//Idle
	isp_addrh=0x80;
	isp_addrl=0x00;
	isp_contr=0x00;
	isp_cmd=0x00;
	isp_trig=0x00;
}

//��STCƬ��EEPROM��ָ����ַд�����ݣ������ֽڱ�̣���
//ע�⣺�ֽڱ����ָ��eeprom��1д��1��0����0д��0�����޷���0д��1
//���ԣ���д������ǰ��һ��Ҫ�����������������ֽڱ�Ϊ0xff
void eepromWrite(unsigned int address, unsigned char write_data)
{
	unsigned char i;
	isp_data=write_data;
	isp_addrl=address;
	isp_addrh=address>>8;
	isp_contr=0x01; 
	isp_contr=isp_contr|ENABLE_IAP; // 0x80 if SYSCLK<40MHz, 0x81 if SYSCLK<20MHz, 0x82 if SYSCLK<10MHz, 0x83 if SYSCLK<5MHz
	isp_cmd=0x02;
	isp_trig=0x5a;  //��ͬ��Ƭ��Trigger���ͬ
	isp_trig=0xa5;
	
	for(i=0;i<5;i++);
	
	//Idle
	isp_addrh=0x80;
	isp_addrl=0x00;
	isp_contr=0x00;
	isp_cmd=0x00;
	isp_trig=0x00;
}



//��ȡSTC��Ƭ���ڲ�EEPROM��һ���ֽ�
//��Ҫ��ͬ��STC��Ƭ��EEPROM��ʼ��ַ��ͬ
//���磺STC89c52RC��Ƭ��EEPROM��ʼ��ַΪ0x2000
unsigned char eepromRead(unsigned int address)
{
	unsigned char i,z;
	isp_addrl=address;
	isp_addrh=address>>8;
	isp_contr=0x01;
	isp_contr=isp_contr|ENABLE_IAP; // 0x80 if SYSCLK<40MHz, 0x81 if SYSCLK<20MHz, 0x82 if SYSCLK<10MHz, 0x83 if SYSCLK<5MHz
	isp_cmd=0x01;
	isp_trig=0x5a;  //��ͬ��Ƭ��Trigger���ͬ
	isp_trig=0xa5;
	
	for(i=0;i<5;i++);

	//Idle
	isp_addrh=0x80;
	isp_addrl=0x00;
	isp_contr=0x00;
	isp_cmd=0x00;
	isp_trig=0x00;
	
	z=isp_data;
	return(z);
}