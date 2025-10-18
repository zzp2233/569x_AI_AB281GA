/**
*@file
*@brief        gsensor公共层驱动头文件
*@details       包括加速度、陀螺、地磁的驱动公共层
*@author       zn.zeng
*@date       2021-07-23
*@version      v1.0
*/


#ifndef _UTE_DRV_GSENSOR_COMMON_H_
#define _UTE_DRV_GSENSOR_COMMON_H_
#include "ute_module_platform.h"
#include "ute_project_config.h"

#if UTE_DRV_MAGNETIC_SENSOR_SUPPORT
/*
* INCLUDE FILES
****************************************************************************************
*/
typedef bool (*drv_magnetic_common_init_t)(void);
typedef void (*drv_magnetic_common_deep_sleep_t)(void);
typedef void (*drv_magnetic_common_get_data_t)(float *data,uint8_t *rawdata);
typedef bool (*drv_magnetic_common_is_used_if_mode_t)(void);
typedef void (*drv_magnetic_common_sensor_set_t)(void);

typedef struct
{
    drv_magnetic_common_init_t init;
    drv_magnetic_common_deep_sleep_t deepSleep;
    drv_magnetic_common_get_data_t getData;
    drv_magnetic_common_is_used_if_mode_t isUsedIfmode;
    drv_magnetic_common_sensor_set_t sensorSet;
} drv_magnetic_common_config_t;


extern const drv_magnetic_common_config_t *drvMagneticCommonFunction;

bool uteDrvMagneticCommonInit(void);
bool uteDrvMagneticCommonDeepSleep(void);
void uteDrvMagneticCommonConvertData(float *data,uint8_t *rawdata);
void uteDrvMagneticCommonSensorSet(void);


#endif
#endif // _UTE_DRV_GSENSOR_COMMON_H_

