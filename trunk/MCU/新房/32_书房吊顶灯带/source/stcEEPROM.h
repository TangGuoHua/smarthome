/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2011��10��06��  �Ƴ���  ��ʼ�汾

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#ifndef __STCEEPROM_H__
#define __STCEEPROM_H__

//#define STC_EEPROM_START_ADDR 0x0000  //STC12C5616ADƬ��EEPROM��ʼ��ַ

unsigned char eepromRead( unsigned int address );
void eepromWrite( unsigned int address, unsigned char writeData );
void eepromEraseSector( unsigned int address );

#endif
