/**
*@file
*@brief        LED驱动
*@details
*@author       zn.zeng
*@date       2021-10-12
*@version      v1.0
*/
#include "ute_drv_led.h"
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_drv_battery_common.h"
#include "ute_application_common.h"
#include "ute_module_platform.h"
#include "ute_module_notdisturb.h"
#include "ute_module_gui_common.h"
#include "ute_module_newFactoryTest.h"
#include "include.h"
#include "ute_module_compass.h"

#if UTE_DRV_LED_SUPPORT

/*! LED运行数据zn.zeng, 2021-10-12  */
ute_drv_led_t uteDrvLedData;

/**
*@brief        LED初始化
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvLedInit(void)
{
    memset(&uteDrvLedData,0,sizeof(ute_drv_led_t));
    uteDrvLedBrightnessLevelReadConfig();
}

/**
*@brief     读取LED数据到内存
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
void uteDrvLedBrightnessLevelReadConfig(void)
{
    /*! 保存到文件dengli.lu, 2022-06-16*/
    void *file;
    uint8_t readbuff[2];
    memset(readbuff, 0, 2);
    readbuff[0] = UTE_MODULE_LED_PWM_DEFAULT_LEVEL;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LED_BRIGHTNESS_LEVEL_INFO, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 2);
        uteModuleFilesystemCloseFile(file);
    }
    uteDrvLedData.brightnessLevel = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,uteDrvLedData.brightnessLevel=%d", __func__, uteDrvLedData.brightnessLevel);
}

/**
*@brief     保存LED数据到内存
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
void uteDrvLedVibrationSaveData(void)
{
    /*! 保存到文件，dengli.lu, 2022-06-16*/
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteDrvLedData.brightnessLevel;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LED_BRIGHTNESS_LEVEL_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],2);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief        打开LED灯
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvLedEnable(void)
{
    uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_LED);
#if UTE_DRV_REUSE_HEART_POWER_SUPPORT
    if (!bsp_sensor_hr_work_status()
#if UTE_DRV_REUSE_HEART_POWER_SUPPORT && UTE_MODULE_MAGNETIC_SUPPORT
        && (!uteModulecompassOnoff())
#endif
       )
#endif
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_LED_POWER_GPIO_PIN, true);
    }
    uteDrvLedData.isOpen = true;
#if UTE_DRV_LED_PWM_MODE_SUPPORT
    uint8_t motorDuty = 0;
    if (0)
    {
    }
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
    else if (uteModuleNewFactoryTestGetMode() != FACTORY_TEST_MODE_NULL)
    {
        motorDuty = UTE_DRV_LED_PWM_LEVEL_HIGH;
    }
#endif
    else
    {
        motorDuty = uteDrvLedData.brightnessLevel;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,motorDuty=%d,brightnessLevel=%d", __func__, motorDuty, uteDrvLedData.brightnessLevel);
    uteModulePlatformPwmInit(UTE_DRV_LED_ENABLE_PWM_ID, UTE_DRV_LED_ENABLE_GPIO_PIN, motorDuty, UTE_DRV_DEFAULT_PWM_HZ);
    uteModulePlatformPwmEnable(UTE_DRV_LED_ENABLE_PWM_ID);
#else
    uteModulePlatformOutputGpioSet(UTE_DRV_LED_ENABLE_GPIO_PIN, true);
#endif
}

/**
*@brief        关闭LED灯
*@details
*@author       zn.zeng
*@date       2021-10-12
*/
void uteDrvLedDisable(void)
{
#if UTE_DRV_LED_PWM_MODE_SUPPORT
    uteModulePlatformPwmDisable(UTE_DRV_LED_ENABLE_PWM_ID,UTE_DRV_LED_ENABLE_GPIO_PIN);
#else
    uteModulePlatformOutputGpioSet(UTE_DRV_LED_ENABLE_GPIO_PIN, false);
#endif
#if UTE_DRV_REUSE_HEART_POWER_SUPPORT
    if (!bsp_sensor_hr_work_status()
#if UTE_DRV_REUSE_HEART_POWER_SUPPORT && UTE_MODULE_MAGNETIC_SUPPORT
        && (!uteModulecompassOnoff())
#endif
       )
#endif
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_LED_POWER_GPIO_PIN, false);
    }
    uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_LED);
    uteDrvLedData.isOpen = false;
}

/**
*@brief        获取LED开启状态
*@details
*@author       dengli.lu
*@date       2021-12-09
*/
AT(.com_text.ute_drv_led)
bool uteDrvLedIsOpen(void)
{
    return uteDrvLedData.isOpen;
}

/**
*@brief     设置LED亮度等级
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
void uteDrvLedSetBrightnessLevel(uint8_t brightnessLevel)
{
    if(brightnessLevel > 100)
    {
        brightnessLevel = 100;
    }
    uteDrvLedData.brightnessLevel = brightnessLevel;
    uteDrvLedVibrationSaveData();
}
/**
*@brief     获取LED亮度等级
*@details
*@author        dengli.lu
*@date        2022-06-16
*/
uint8_t uteDrvLedGetBrightnessLevel(void)
{
    return uteDrvLedData.brightnessLevel;
}

/**
*@brief     设置LED亮度等级不保存
*@details
*@author      casen
*@date        2022-11-14
*/
void uteDrvLedSetTempBrightnessLevel(uint8_t brightnessLevel)
{
    if(brightnessLevel > 100)
    {
        brightnessLevel = 100;
    }
    uteDrvLedData.brightnessLevel = brightnessLevel;
}

#endif
