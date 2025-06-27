/**
*@file
*@brief        gsensor�����������ļ�
*@details       �������ٶȡ����ݡ��شŵ�����������
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
*@brief  mag ������ʼ��
*@details
*@return bool trueΪ�ɹ�
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
*@brief  mag ����
*@details
*@return bool trueΪ�ɹ�
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
*@brief  mag ����ת������λΪ��˹
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
*@brief   Sensor���������λ
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
