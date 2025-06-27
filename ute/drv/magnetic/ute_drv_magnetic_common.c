/**
*@file
*@brief        gsensor公共层驱动文件
*@details       包括加速度、陀螺、地磁的驱动公共层
*@author        pcm
*@date        2024-11-25
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_drv_magnetic_common.h"
#include "ute_project_config.h"
#include "ute_module_message.h"

#if UTE_DRV_MAGNETIC_SENSOR_SUPPORT
#include "ute_module_compass.h"
#if UTE_DRV_SENSOR_MAG_MMC5603NJ_SUPPORT
#include "ute_drv_magnetic_mmc5603.h"
#endif

const drv_magnetic_common_config_t *drvMagneticCommonFunction = NULL;
/**
*@brief  mag 驱动初始化
*@details
*@return bool true为成功
*@author        pcm
*@date        2024-11-25
*/
bool uteDrvMagneticCommonInit(void)
{
#if UTE_DRV_SENSOR_MAG_MMC5603NJ_SUPPORT
    drvMagneticCommonFunction = &drvMagneticMmc5603Function;
#endif
    if (drvMagneticCommonFunction != NULL)
    {
        drvMagneticCommonFunction->init();
    }
    uteModuleCompassAlgoInit();
    return true;
}
/**
*@brief  mag 休眠
*@details
*@return bool true为成功
*@author        pcm
*@date        2024-11-25
*/
bool uteDrvMagneticCommonDeepSleep(void)
{
    if (drvMagneticCommonFunction != NULL)
    {
        drvMagneticCommonFunction->deepSleep();
    }
    return true;
}
/**
*@brief  mag 数据转换，单位为高斯
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteDrvMagneticCommonConvertData(float *data,uint8_t *rawdata)
{
    if (drvMagneticCommonFunction != NULL)
    {
        drvMagneticCommonFunction->getData(data,rawdata);
    }
}
/**
*@brief   Sensor进行软件复位
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteDrvMagneticCommonSensorSet(void)
{
    if (drvMagneticCommonFunction != NULL)
    {
        drvMagneticCommonFunction->sensorSet();
    }
}
#endif
