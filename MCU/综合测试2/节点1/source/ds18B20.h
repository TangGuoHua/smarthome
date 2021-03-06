/*
When        Who        Remarks
--------------------------------------
2011-SEP-25 Changhao   Initial version
*/


#ifndef __DS18B20_H__
#define __DS18B20_H__

#include <reg52.h>     //包含头文件，一般情况不需要改动，头文件包含特殊功能寄存器的定义
#include "dataType.h"

/*------------------------------------------------
                    端口定义
------------------------------------------------*/
sbit DS18B20 = P3^6;//ds18b20 端口

/*------------------------------------------------
                  函数声明
------------------------------------------------*/
bit initDS18B20(void);
float readTemperature( bit mode );

#endif
