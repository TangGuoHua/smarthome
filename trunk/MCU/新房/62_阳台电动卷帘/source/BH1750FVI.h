#ifndef __BH1750FVI_H__
#define __BH1750FVI_H__

// ��������
#define	  SlaveAddress   0x46 //����������IIC�����еĴӵ�ַ,����ALT  ADDRESS��ַ���Ų�ͬ�޸�
                              //ALT  ADDRESS���Žӵ�ʱ��ַΪ0xA6���ӵ�Դʱ��ַΪ0x3A

// ���Ŷ���
sbit BH1750_SCL=P1^7; //IICʱ�����Ŷ���
sbit BH1750_SDA=P1^6; //IIC�������Ŷ���

// ��������
void bh1750Init();
unsigned int bh1750GetBrightness();

#endif