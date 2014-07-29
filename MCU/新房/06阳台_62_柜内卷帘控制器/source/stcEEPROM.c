/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2011��10��06��  �Ƴ���  ��ʼ�汾
2014��02��09��  �Ƴ���  �޸�ENABLE_IAPΪ0x83��if SYSCLK<12MHz��

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

sfr isp_data=0xe2;
sfr isp_addrh=0xe3;
sfr isp_addrl=0xe4;
sfr isp_cmd=0xe5;
sfr isp_trig=0xe6;
sfr isp_contr=0xe7;

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
	isp_trig=0x46;
	isp_trig=0xb9;
	for(i=0;i<3;i++);
	isp_addrl=0xff;
	isp_addrh=0xff;
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
	isp_trig=0x46;
	isp_trig=0xb9;
	for(i=0;i<3;i++);
	isp_addrl=0xff;
	isp_addrh=0xff;
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
	isp_trig=0x46;
	isp_trig=0xb9;
	for(i=0;i<3;i++);
	isp_addrl=0xff;
	isp_addrh=0xff;
	isp_contr=0x00;
	isp_cmd=0x00;
	isp_trig=0x00;
	z=isp_data;
	return(z);
}