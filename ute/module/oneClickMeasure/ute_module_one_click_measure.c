/**
 * @file       ute_module_one_click_measure
 * @brief      一键测量模块
 * @details
 * @author     Wang.Luo
 * @date       2025-08-11
 * @version    v1.0
 */
#include "ute_module_one_click_measure.h"
#include "ute_module_heart.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"
#include "ute_module_sport.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_emotionPressure.h"

#if UTE_MODULE_ONE_CLICK_MEASURE_SUPPORT

ute_module_one_click_measure_data_t uteModuleOneClickMeasureData;

/**
 * @brief        一键测量初始化
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-13
 */
void uteModuleOneClickMeasureInit(void)
{
    memset(&uteModuleOneClickMeasureData, 0, sizeof(ute_module_one_click_measure_data_t));
    uteModuleSystemtimeRegisterSecond(uteModuleOneClickMeasureEverySecond);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
}

/**
 * @brief         一键测量每秒处理
 * @details
 * @return        void
 * @author        Wang.Luo
 * @date          2025-08-13
 */
void uteModuleOneClickMeasureEverySecond(void)
{
    if (!uteApplicationCommonIsPowerOn())
    {
        return;
    }

    if (uteModuleOneClickMeasureData.isSingleTesting)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.testingSecond=%d", __func__, uteModuleOneClickMeasureData.testingSecond);
        if (uteModuleOneClickMeasureIsWear() && (uteModuleHeartGetWorkMode() == HR_WORK_MODE_HRSPO2) && bsp_sensor_hr_work_status())
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,testingSecond:%d", __func__, uteModuleOneClickMeasureData.testingSecond);
            if (uteModuleOneClickMeasureData.testingSecond >= UTE_MODULE_ONE_CLICK_MEASURE_TEST_SECOND)
            {
                uteModuleOneClickMeasureStopSingleTesting();
            }
            else
            {
                uteModuleOneClickMeasureData.testingSecond++;
            }
        }
        else
        {
            uteModuleOneClickMeasureData.result = MEASURE_RESULT_UNWEAR;
            uteModuleOneClickMeasureStopSingleTesting();
        }
    }
}

/**
 * @brief        开始一键测量
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-13
 */
void uteModuleOneClickMeasureStartSingleTesting(void)
{
    if(bsp_sensor_hr_work_status() && uteModuleHeartGetWorkMode() == HR_WORK_MODE_HRSPO2)
    {
        uteModuleOneClickMeasureData.isSingleTesting = true;
        return;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s",__func__);
    uteModuleOneClickMeasureData.isSingleTesting = true;
    uteModuleOneClickMeasureData.result = MEASURE_RESULT_NONE;
    uteModuleOneClickMeasureData.testingSecond = 0;
    uteModuleOneClickMeasureData.value.heartRate = 0;
    uteModuleOneClickMeasureData.value.bloodOxygen = 0;
    uteModuleOneClickMeasureData.value.pressure = 0;
    uteModuleHeartStartSingleTesting(TYPE_HR_SPO2);
}

/**
 * @brief        停止一键测量
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-13
 */
void uteModuleOneClickMeasureStopSingleTesting(void)
{
    if (uteModuleHeartGetWorkMode() != HR_WORK_MODE_HRSPO2 || !bsp_sensor_hr_work_status())
    {
        uteModuleOneClickMeasureData.isSingleTesting = false;
        return;
    }

    uteModuleOneClickMeasureData.isSingleTesting = false;

    if (uteModuleOneClickMeasureData.result == MEASURE_RESULT_NONE)
    {
        if (uteModuleOneClickMeasureData.testingSecond >= UTE_MODULE_ONE_CLICK_MEASURE_TEST_SECOND && uteModuleOneClickMeasureData.value.heartRate > 0 && uteModuleOneClickMeasureData.value.bloodOxygen > 0 && uteModuleOneClickMeasureData.value.pressure > 0)
        {
            uteModuleOneClickMeasureData.result = MEASURE_RESULT_SUCCESS;
            uteModuleHeartSetHeartValue(uteModuleOneClickMeasureData.value.heartRate);
            uteModuleHeartAutoSaveHeartData();
            uteModuleBloodoxygenSetValue(uteModuleOneClickMeasureData.value.bloodOxygen);
            uteModuleBloodoxygenAutoSaveOxygenData();
            uint8_t EmotionValue = 2;
            if (uteModuleOneClickMeasureData.value.pressure == 0xFF)
            {
                EmotionValue = 0xFF;
            }
            else
            {
                if (uteModuleOneClickMeasureData.value.pressure > 80)
                {
                    EmotionValue = 0;
                }
                else if (uteModuleOneClickMeasureData.value.pressure > 30)
                {
                    EmotionValue = 1;
                }
            }
            uteModuleEmotionPressureSetEmotionPressureValue(EmotionValue, uteModuleOneClickMeasureData.value.pressure, 0);
            uteModuleEmotionPressureAutoSaveData();
        }
        else
        {
            uteModuleOneClickMeasureData.result = MEASURE_RESULT_FAIL;
        }
    }

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,result=%d", __func__, uteModuleOneClickMeasureData.result);

    uteModuleHeartStopSingleTesting(TYPE_HR_SPO2);
}

/**
 * @brief        获取一键测量数据
 * @details
 * @param[out]   heartRate     心率
 * @param[out]   bloodOxygen   血氧
 * @param[out]   pressure      压力
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-13
 */
void uteModuleOneClickMeasureGetTestValue(uint8_t *heartRate, uint8_t *bloodOxygen, uint8_t *pressure)
{
    if (heartRate)
    {
        *heartRate = uteModuleOneClickMeasureData.value.heartRate;
    }
    if (bloodOxygen)
    {
        *bloodOxygen = uteModuleOneClickMeasureData.value.bloodOxygen;
    }
    if (pressure)
    {
        *pressure = uteModuleOneClickMeasureData.value.pressure;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,heartRate=%d,bloodOxygen=%d,pressure=%d", __func__, *heartRate, *bloodOxygen, *pressure);
}

/**
 * @brief        设置一键测量数据
 * @details
 * @param[in]    heartRate 心率
 * @param[in]    bloodOxygen 血氧
 * @param[in]    pressure 血压
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-08-13
 */
void uteModuleOneClickMeasureSetTestValue(uint8_t heartRate, uint8_t bloodOxygen, uint8_t pressure)
{
    uteModuleOneClickMeasureData.value.heartRate = heartRate;
    uteModuleOneClickMeasureData.value.bloodOxygen = bloodOxygen;
    uteModuleOneClickMeasureData.value.pressure = pressure;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,heartRate=%d,bloodOxygen=%d,pressure=%d",__func__,heartRate, bloodOxygen, pressure);
}

/**
 * @brief       是否正在测试
 * @details
 * @return      bool
 * @author      Wang.Luo
 * @date        2025-08-13
 */
bool uteModuleOneClickMeasureIsTesting(void)
{
    return uteModuleOneClickMeasureData.isSingleTesting;
}

/**
 * @brief       获取测试结果
 * @details     用于界面状态显示
 * @return      uint8_t
 * @author      Wang.Luo
 * @date        2025-08-13
 */
uint8_t uteModuleOneClickMeasureGetTestResult(void)
{
    return uteModuleOneClickMeasureData.result;
}

/**
 * @brief       获取测试时间
 * @details     用于界面状态显示
 * @return      uint8_t
 * @author      Wang.Luo
 * @date        2025-08-13
 */
uint8_t uteModuleOneClickMeasureGetTestingSecond(void)
{
    return uteModuleOneClickMeasureData.testingSecond;
}

/**
*@brief        是否佩戴
*@details
*@author       zn.zeng
*@date       2021-07-21
*/
bool uteModuleOneClickMeasureIsWear(void)
{
    bool isWear = false;
    isWear = uteModuleHeartIsWear();
    return isWear;
}

#endif
