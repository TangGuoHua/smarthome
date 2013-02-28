/*
When         Who     Remarks
----------------------------------
2011-Oct-06  黄长浩  初始版本 
*/

#ifndef __STCEEPROM_H__
#define __STCEEPROM_H__

#define STC_EEPROM_START_ADDR 0x2000  //STC89C52RC片内EEPROM起始地址

unsigned char eepromRead( unsigned int address );
void eepromWrite( unsigned int address, unsigned char writeData );
void eepromEraseSector( unsigned int address );

#endif
