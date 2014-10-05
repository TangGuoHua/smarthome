/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2011年10月06日  黄长浩  初始版本
2014年02月09日  黄长浩  修改ENABLE_IAP为0x83（if SYSCLK<12MHz）

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
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

//擦除片内EEPROM的一个扇区
//擦除只能以扇区为最小单位进行，没法只擦除一个字节
//一个扇区是512个字节
//本函数参数里面的地址落在哪个扇区，则该扇区内数据都将被擦除
//例如：STC89C51RC片内EEPROM第一扇区开始地址为0x2000，结束地址为0x21ff
//如果调用 eepromEraseSector(0x2001)，则第一扇区内数据都将被擦除
//擦除成功后，该扇区内各字节都将变为0xff
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

//对STC片内EEPROM的指定地址写入数据（即，字节编程）。
//注意：字节编程是指将eeprom的1写成1或0，将0写成0，而无法将0写成1
//所以，在写入数据前，一定要用扇区擦除将所有字节变为0xff
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



//读取STC单片机内部EEPROM的一个字节
//主要不同的STC单片机EEPROM起始地址不同
//例如：STC89c52RC的片内EEPROM起始地址为0x2000
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