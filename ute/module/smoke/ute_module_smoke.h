/**
*@file
*@brief        抽烟模块
*@details
*@author       zn.zeng
*@date       2021-08-25
*@version      v1.0
*/
#ifndef _UTE_MODULE_SMOKE_H_
#define _UTE_MODULE_SMOKE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"

#define UTE_MODULE_FILESYSTEM_SMOKE_DATA_DIR "smoke"
#define UTE_MODULE_FILESYSTEM_SMOKE_LEVEL_FILE "smoke/data"

typedef struct
{
    uint8_t somke_capacity_level;//
    uint32_t total_smoking_times;//计算抽吸总时长
    uint32_t total_smoking_count; // 抽吸口数
    uint32_t smoking_count_per_hour[24];  // 存储每小时的口数
    u8 total_smoking_wattage; // 抽吸功率
    uint8_t total_smoking_resistance;  //电阻阻值

} ute_module_smoke_data_t;
extern ute_module_smoke_data_t uteModuleSmokeData;
typedef struct
{
    int power_curr_nubmer;
    u16 Heating_wire_power;
    int power_watt_index;
} smoke_app_data_t;


static void uteModuleSmokeDataReadConfig(void);
void uteModuleSmokeDataSaveConfig(void);
void uteModuleSetSomkeTime(uint32_t time);
uint32_t uteModuleGetSomkeSomkeTime(void);

//抽吸口数
void uteModuleSetSomkeCount(uint32_t count);
uint32_t uteModuleGetSomkeSomkeCount(void);
void uteModuleGetSmokingCountPerHour(uint32_t *counts);
#endif
