#ifndef _THIRD_KWS_EVENT_H__
#define _THIRD_KWS_EVENT_H__

#include "include.h"

#if (ASR_SELECT == ASR_YJ)

#if THIRD_UI_LIGHT_EN

enum
{
    UI_EVENT_START=0x000,
    UI_EVENT_XIAO_LAN_XIAO_LAN, //С��С��  1
    UI_EVENT_DA_KAI_SHE_ZHI,    //������  2
    UI_EVENT_DA_KAI_YIN_YUE,    //������  3
    UI_EVENT_DUAN_SHI_PIN_YAO_KONG, //�򿪶���Ƶң��  4
    UI_EVENT_DA_KAI_TIAN_QI,    //������  5
    UI_EVENT_DA_KAI_XUE_YANG,   //��Ѫ��  6
    UI_EVENT_DA_KAI_SHUI_MIAN,  //��˯��  7
    UI_EVENT_DA_KAI_NAO_ZHONG,  //������  8
    UI_EVENT_DA_KAI_XIN_LV, //������  9
    UI_EVENT_DA_KAI_ZHI_FU_BAO, //��֧����  10
    UI_EVENT_DA_KAI_WEI_XIN_ZHI_FU, //��΢��֧��  11
    UI_EVENT_DA_KAI_SHOU_KUAN_MA,   //���տ���  12
    UI_EVENT_DA_KAI_CHENG_CHE_MA,   //�򿪳˳���  13
    UI_EVENT_DA_KAI_YU_YIN_ZHU_SHOU,    //����������  14
    UI_EVENT_DA_KAI_JI_PI_TI,   //�򿪼Ǳ���  15
    UI_EVENT_DA_KAI_YUN_DONG,   //���˶�  16
    UI_EVENT_TING_ZHI_YUN_DONG, //ֹͣ�˶�  17
    UI_EVENT_JI_XU_YUN_DONG,    //�����˶�  18
    UI_EVENT_ZAN_TING_YUN_DONG, //��ͣ�˶�  19
    UI_EVENT_DA_KAI_CHANG_LIANG_MO_SHI, //�򿪳���ģʽ  20
    UI_EVENT_GUAN_BI_CHANG_LIANG_MO_SHI,    //�رճ���ģʽ  21
    UI_EVENT_DA_KAI_SAO_MA_LIAN_JIE,    //��ɨ������  22
    UI_EVENT_CHA_ZHAO_SHOU_JI,  //�����ֻ�  23
    UI_EVENT_FAN_HUI_BIAO_PAN,  //���ر���  24
    UI_EVENT_MAX
};

#endif // THIRD_UI_LIGHT_EN

#endif //ASR_SELECT

#endif
