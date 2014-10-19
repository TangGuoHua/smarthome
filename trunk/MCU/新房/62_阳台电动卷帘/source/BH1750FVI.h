#ifndef __BH1750FVI_H__
#define __BH1750FVI_H__

// 常量定义
#define	  SlaveAddress   0x46 //定义器件在IIC总线中的从地址,根据ALT  ADDRESS地址引脚不同修改
                              //ALT  ADDRESS引脚接地时地址为0xA6，接电源时地址为0x3A

// 引脚定义
sbit BH1750_SCL=P1^7; //IIC时钟引脚定义
sbit BH1750_SDA=P1^6; //IIC数据引脚定义

// 函数声明
void bh1750Init();
unsigned int bh1750GetBrightness();

#endif