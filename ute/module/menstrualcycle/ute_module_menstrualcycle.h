/**
*@file
*@brief        女性生理周期
*@details
*@author       dengli.lu
*@date       2021-11-24
*@version      v1.0
*/
#ifndef _UTE_MODULE_MENSTRUALCYCLE_H_
#define _UTE_MODULE_MENSTRUALCYCLE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"

/* 女性周期数据保存 zn.zeng 2022-01-27*/
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_MENSTRUALCYCLE_INFO  "systemparam/menstrualcycleInfo"

typedef struct
{
    bool reminderSwitch;
    uint8_t lastTimestamp[4]; //yyyy-mm-dd
    uint8_t keepDays;
    uint8_t cycleDays;
} ute_menstrual_cycle_param_t;

void uteModuleMenstrualCycleInit(void);
void uteModuleMenstrualCycleSetParam(ute_menstrual_cycle_param_t param);
bool uteModuleMenstrualCycleGetStatus(uint8_t *menstrualStatus);
bool uteModuleMenstrualCycleIsOpen(void);

#endif
