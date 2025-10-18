/**
*@file
*@brief     功能开启关闭提醒模块
*@details
*@author        Yin
*@date        2025.07.23
*@version       v1.0
*/

#ifndef _UTE_MODULE_SETTINGREMIND_H_
#define _UTE_MODULE_SETTINGREMIND_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"

#if UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT

/*! 数据保存 Yin, 2025.07.23  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_SETTING_REMIND "systemparam/SettingRemind"

typedef struct
{
    uint8_t func_id;
    uint8_t id_state;
} ute_module_settingremind_data_t;

enum
{
    SETTING_REMIND_ID_SAVING = 0,
    SETTING_REMIND_ID_WRIST,
    SETTING_REMIND_ID_MUTE,
    SETTING_REMIND_ID_DISTUB,
};

void uteModuleSettingRemindInit(void);
void uteModuleSettingRemindReadConfig(void);
void uteModuleSettingRemindSaveData(void);
void uteModuleSettingRemindSetRemindId(uint8_t id);
uint8_t uteModuleSettingRemindGetRemindId(void);
void uteModuleSettingRemindSetNotNeedShowId(uint8_t id_bit);
bool uteModuleSettingRemindGetWhetherShowId(uint8_t id_bit);

#endif
#endif

