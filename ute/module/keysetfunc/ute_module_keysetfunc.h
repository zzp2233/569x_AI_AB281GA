/**
*@file
*@brief     按键设置功能模块模块
*@details
*@author        Yin
*@date        2025.06.20
*@version       v1.0
*/

#ifndef _UTE_MODULE_KEYSETFUNC_H_
#define _UTE_MODULE_KEYSETFUNC_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"

#if UTE_MODULE_KEY_SET_FUNCTION_SUPPORT

/*! 下按键数据保存 Yin, 2025.06.19  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_KEY_SET_FUNCTION "systemparam/KeySetFuncion"

typedef struct
{
    u8 keySetMenu;
    bool keySetFlag;
} ute_module_keysetfunc_data_t;

void uteModuleKeySetFuncInit(void);
void uteModuleKeySetFuncReadConfig(void);
void uteModuleKeySetFuncSaveData(void);
uint8_t uteModuleKeySetFuncGetMenu(void);
void uteModuleKeySetFuncSetMenu(uint8_t menu);

#endif //UTE_MODULE_KEY_SET_FUNCTION_SUPPORT
#endif//_UTE_MODULE_KEYSETFUNC_H_

