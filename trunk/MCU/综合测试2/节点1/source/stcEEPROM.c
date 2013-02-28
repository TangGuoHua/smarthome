/*
When         Who     Remarks
----------------------------------
2011-Oct-06  黄长浩  初始版本 
*/

sfr isp_data=0xe2;
sfr isp_addrh=0xe3;
sfr isp_addrl=0xe4;
sfr isp_cmd=0xe5;
sfr isp_trig=0xe6;
sfr isp_contr=0xe7;

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
	isp_contr=isp_contr|0x81; // 0x80 if SYSCLK<40MHz, 0x81 if SYSCLK<20MHz, 0x82 if SYSCLK<10MHz, 0x83 if SYSCLK<5MHz
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
	isp_contr=isp_contr|0x81; // 0x80 if SYSCLK<40MHz, 0x81 if SYSCLK<20MHz, 0x82 if SYSCLK<10MHz, 0x83 if SYSCLK<5MHz
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
	isp_contr=isp_contr|0x81; // 0x80 if SYSCLK<40MHz, 0x81 if SYSCLK<20MHz, 0x82 if SYSCLK<10MHz, 0x83 if SYSCLK<5MHz
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