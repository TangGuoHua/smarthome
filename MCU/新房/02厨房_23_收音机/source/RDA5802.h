/***************************************************************************
���޸���ʷ��

����            ����    ��ע
----------------------------------------------------------------------
2013��05��20��  �Ƴ���  ��ʼ�汾

����Ȩ������
Copyright(C) All Rights Reserved by Changhao Huang (HuangChangHao@gmail.com)
��Ȩ�����ߣ��Ƴ��� HuangChangHao@gmail.com

δ������������Ȩ�������Խ�������˳��������κ���ҵĿ�ġ�
����ѧϰ��ο�Ŀ�ģ��������ô�ע����Ȩ��������Ϣ��
****************************************************************************/

#ifndef  __RDA5802_H__  //�������ָ��
#define  __RDA5802_H__  //�����ظ�����

extern void RDA5802Init( void );
extern void RDA5802PowerUp( bit powerUp );
extern void RDA5802Tune( unsigned int tuneFreq );
extern void RDA5802SetVolume( unsigned char setVolume );
extern void RDA5802Mute( bit toMute );
extern void RDA5802ForceMono( bit toForceMono );
extern void RDA5802BoostBass( bit toBoostBase );

#endif  //����������
