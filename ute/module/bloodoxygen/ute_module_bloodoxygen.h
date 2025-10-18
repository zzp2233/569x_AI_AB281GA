/**
*@file
*@brief     心氧模块
*@details   心氧模块头文件
*@author        zn.zeng
*@date        2021-07-21
*@version       v1.0
*/
#ifndef _UTE_MODULE_BLOODOXYGEN_H_
#define _UTE_MODULE_BLOODOXYGEN_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"

#if UTE_MODULE_BLOODOXYGEN_SUPPORT
/*!写入心率测试参数 zn.zeng, 2021-08-20  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SET_INFO "systemparam/bloodoxygenSetInfo"
/*! 心率24小时自动测试数据保存目录 zn.zeng, 2021-08-30  */
#define UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR "bloodoxygenAutoData"
/*! 最后一次血氧测量信息,wang.luo 2025-09-27 */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_LAST_INFO "systemparam/bloodoxygenLastInfo"
#if APP_SYNC_RECORD_TIME_FLAG_SUPPORT
/*!同步时间*/
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SYNC_TIME_INFO "systemparam/bloodoxygenSyncTime"
#endif

typedef struct
{
    uint8_t lastValue;
    ute_module_systemtime_measure_time_t lastTestTime;
} ute_module_bloodoxygen_test_last_info_t;

/*! 血氧数据结构zn.zeng, 2021-07-21  */
typedef struct
{
    bool isSingleTesting;
    uint8_t value;
    uint8_t testingSecond;
    bool isAutoTesting;
    bool isAutoTimeBucketTesting;
    uint16_t intervalMin;
    uint16_t startTimeHourMin;
    uint16_t endTimeHourMin;
    bool isBloodOxygenAutoTestFlag;
    uint8_t dayMinValue;
    uint8_t dayMaxValue;
    uint8_t lastValue;
    ute_module_bloodoxygen_test_last_info_t lastTestInfo;
    uint8_t notWearSecond;
#if APP_SYNC_RECORD_TIME_FLAG_SUPPORT
    ute_module_systemtime_measure_time_t syncTime;
#endif
} ute_module_bloodoxygen_data_t;
void uteModuleBloodoxygenInit(void);
void uteModuleBloodoxygenReadConfig(void);
void uteModuleBloodoxygenEverySecond(void);
void uteModuleBloodoxygenStartSingleTesting(void);
void uteModuleBloodoxygenStopSingleTesting(void);
bool uteModuleBloodoxygenIsWear(void);
uint8_t uteModuleBloodoxygenGetValue(void);
void uteModuleBloodoxygenSetValue(uint8_t value);
bool uteModuleBloodoxygenIsTesting(void);
void uteModuleBloodoxygenSaveAutoIntervalParam(bool isAutoTesting,uint16_t intervalMin);
void uteModuleBloodoxygenSaveAutoTimeBucketParam(bool isAutoTimeBucketTesting,uint16_t startTimeHourMin,uint16_t endTimeHourMin);
void uteModuleBloodoxygenStartSendAutoTestHistoryData(ute_module_systemtime_time_t time);
void uteModuleBloodoxygenAutoSaveOxygenData(void);
void uteModuleBloodoxygenDelHistoryData(void);
bool uteModuleBloodoxygenIsBloodOxygenAutoTesting(void);
uint8_t uteModuleBloodoxygenGetTestingSecond(void);
void uteModuleBloodoxygenSetFirstEnterAppFlag(bool flag);
void uteModuleBloodoxygenGetMinMaxValue(uint8_t *min,uint8_t *max);
bool uteModuleBloodoxygenGetTodayHistoryData(uint8_t *Bloodoxygen, uint8_t BloodoxygenCount);
void uteModuleBloodoxygenSetBloodOxygenAutoTesting(bool isAutoTest);
uint8_t uteModuleBloodoxygenSetLastTestInfo(uint8_t bloodoxygenValue, ute_module_systemtime_time_t time);
void uteModuleBloodoxygenReadLastTestInfo(void);
void uteModuleBloodoxygenGetLastTestInfo(ute_module_bloodoxygen_test_last_info_t *info);
#if APP_SYNC_RECORD_TIME_FLAG_SUPPORT
void uteModuleBloodoxygenReadSyncTimeInfo(void);
void uteModuleBloodoxygenSaveSyncTimeInfo(void);
void uteModuleBloodoxygenSetSyncTime(ute_module_systemtime_measure_time_t setTime);
void uteModuleBloodoxygenGetSyncTime(ute_module_systemtime_measure_time_t *time);
#endif
#endif //UTE_MODULE_BLOODOXYGEN_SUPPORT
#endif //_UTE_MODULE_BLOODOXYGEN_H_

