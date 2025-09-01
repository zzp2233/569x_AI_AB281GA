#ifndef _UTE_MODULE_BEDSIDE_MODE_H_
#define _UTE_MODULE_BEDSIDE_MODE_H_

#include "ute_project_config.h"
#include "include.h"

#if UTE_MODULE_BEDSIDE_MODE_SUPPORT

/*!写入心率测试参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_BEDSIDE_MODE_SET_INFO "systemparam/bedsideModeSetInfo"

typedef struct
{
    bool isOpen;
} ute_module_bedside_mode_data_t;

void uteModuleBedsideModeInit(void);
void uteModuleBedsideModeReadConfig(void);
void uteModuleBedsideModeSaveConfig(void);
bool uteModuleBedsideModeIsOpen(void);
void uteModuleBedsideModeSetOpen(bool isOpen);
void uteModuleBedsideModeSwitch(void);

#endif

#endif
