/**
*@file
*@brief     呼吸率模块
*@details   呼吸率模块头文件
*@author        zn.zeng
*@date        2021-08-16
*@version       v1.0
*/
#ifndef _UTE_MODULE_BREATHRATE_H_
#define _UTE_MODULE_BREATHRATE_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"

#if UTE_MODULE_BREATHRATE_SUPPORT
/*!写入心率测试参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_BREATHRATE_SET_INFO "systemparam/breathrateSetInfo"
/*! 心率24小时自动测试数据保存目录 zn.zeng, 2021-08-30  */
#define UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR "breathrateAutoData"

/*! 呼吸率数据结构zn.zeng, 2021-08-16  */
typedef struct
{
    bool isSingleTesting;
    uint8_t breathrateValue;
    bool isAutoTesting;
    bool isAutoTimeBucketTesting;
    uint16_t intervalMin;
    uint16_t startTimeHourMin;
    uint16_t endTimeHourMin;
} ute_module_breathrate_data_t;
void uteModuleBreathrateInit(void);
void uteModuleBreathrateReadConfig(void);
void uteModuleBreathrateEverySecond(void);
void uteModuleBreathrateStartSingleTesting(void);
void uteModuleBreathrateStopSingleTesting(void);
bool uteModuleBreathrateIsWear(void);
uint8_t uteModuleBreathrateGetValue(void);
bool uteModuleBreathrateIsTesting(void);
void uteModuleBreathrateSaveAutoIntervalParam(bool isAutoTesting,uint16_t intervalMin);
void uteModuleBreathrateSaveAutoTimeBucketParam(bool isAutoTimeBucketTesting,uint16_t startTimeHourMin,uint16_t endTimeHourMin);
void uteModuleBreathrateAutoSaveData(void);
void uteModuleBreathrateSendHistoryData(void);
void uteModuleBreathrateStartSendAutoTestHistoryData(ute_module_systemtime_time_t time);
void uteModuleBreathrateDelHistoryData(void);
void uteModuleBreathrateSetValue(uint8_t value);
#if UTE_DISPLAY_BREATH_RATE_HISTORY_HISTOGRAM_SUPPORT
bool uteModuleBreathrateGetTodayHistoryData(UT_GraphsParam *BreathRateHistoryGraph, uint8_t BreathRateHistoryGraphCount, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange);
#endif
#endif //UTE_MODULE_BREATHRATE_SUPPORT
#endif //_UTE_MODULE_BREATHRATE_H_

