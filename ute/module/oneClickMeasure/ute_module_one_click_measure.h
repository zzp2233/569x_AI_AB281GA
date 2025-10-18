/**
 * @file       ute_module_one_click_measure
 * @brief      一键测量模块
 * @details
 * @author     Wang.Luo
 * @date       2025-08-11
 * @version    v1.0
 */
#ifndef _UTE_MODULE_ONE_CLICK_MEASURE_H_
#define _UTE_MODULE_ONE_CLICK_MEASURE_H_

#include "include.h"
#include "ute_module_systemtime.h"
#include "ute_project_config.h"

#if UTE_MODULE_ONE_CLICK_MEASURE_SUPPORT

typedef struct
{
    uint8_t heartRate;
    uint8_t bloodOxygen;
    uint8_t pressure;
} ute_module_one_click_measure_value_t;

typedef enum
{
    MEASURE_RESULT_NONE = 0,
    MEASURE_RESULT_SUCCESS,
    MEASURE_RESULT_FAIL, // 测量失败
    MEASURE_RESULT_UNWEAR, // 脱手
    MEASURE_RESULT_ACTIVITY, // 活动
} UTE_MODULE_ONE_CLICK_MEASURE_RESULT_T;

typedef struct
{
    bool isSingleTesting;
    uint8_t testingSecond;
    ute_module_one_click_measure_value_t value;
    ute_module_systemtime_measure_time_t lastTestTime;
    UTE_MODULE_ONE_CLICK_MEASURE_RESULT_T result;
} ute_module_one_click_measure_data_t;

void uteModuleOneClickMeasureInit(void);
void uteModuleOneClickMeasureEverySecond(void);
void uteModuleOneClickMeasureStartSingleTesting(void);
void uteModuleOneClickMeasureStopSingleTesting(void);
void uteModuleOneClickMeasureGetTestValue(uint8_t *heartRate, uint8_t *bloodOxygen, uint8_t *pressure);
void uteModuleOneClickMeasureSetTestValue(uint8_t heartRate, uint8_t bloodOxygen, uint8_t pressure);
bool uteModuleOneClickMeasureIsTesting(void);
uint8_t uteModuleOneClickMeasureGetTestResult(void);
uint8_t uteModuleOneClickMeasureGetTestingSecond(void);
bool uteModuleOneClickMeasureIsWear(void);

#endif

#endif

