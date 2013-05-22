/***************************************************************************
【修改历史】

日期            作者    备注
----------------------------------------------------------------------
2013年05月20日  黄长浩  初始版本

【版权声明】
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
版权所有者：黄长浩 HuangChangHao@gmail.com

未经作者书面授权，不可以将本程序此程序用于任何商业目的。
用于学习与参考目的，请在引用处注明版权和作者信息。
****************************************************************************/

#ifndef  __RDA5802_H__  //条件汇编指令
#define  __RDA5802_H__  //避免重复调用

extern void RDA5802Init( void );
extern void RDA5802PowerUp( bit powerUp );
extern void RDA5802Tune( unsigned int tuneFreq );
extern void RDA5802SetVolume( unsigned char setVolume );
extern void RDA5802Mute( bit toMute );
extern void RDA5802ForceMono( bit toForceMono );
extern void RDA5802BoostBass( bit toBoostBase );

#endif  //条件汇编结束
