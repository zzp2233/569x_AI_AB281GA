/**
*@file
*@brief        喝水提醒模块
*@details
*@author       黄鹤
*@date       2022-03-29
*@version      v1.0
*/
#ifndef _UTE_MODULE_DRINKWATER_H_
#define _UTE_MODULE_DRINKWATER_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"
/*!写入喝水信息 黄鹤, 2022-03-29  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_DRINKWATER "systemparam/drinkWaterInfo"

/*! 喝水数据结构 黄鹤, 2022-03-29  */
typedef struct
{
    bool isOpen;
    uint8_t min;
    uint8_t startPeriodHour;
    uint8_t startPeriodMin;
    uint8_t endPeriodHour;
    uint8_t endPeriodMin;
    uint8_t isOpenNoonSleep;
    uint8_t startNoonSleepHour;
    uint8_t startNoonSleepMin;
    uint8_t endNoonSleepHour;
    uint8_t endNoonSleepMin;
    uint16_t RemindSecCnt;
} ute_module_drink_water_data_t;

void uteModuleDrinkWaterReadConfig(void);
void uteModuleDrinkWaterSaveParam(ute_module_drink_water_data_t *param);
void uteModuleDrinkWaterHandler(ute_module_systemtime_time_t time);
void uteModuleDrinkWaterStart(void);
void uteModuleDrinkWaterInit(void);
#endif //_UTE_MODULE_DRINKWATER_H_

