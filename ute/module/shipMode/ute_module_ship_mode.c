/**
 *@file
 *@brief        船运模式
 *@details
 *@author       zn.zeng
 *@date       2022-08-02
 *@version      v1.0
 */

#include "ute_module_ship_mode.h"
#include "ute_module_log.h"
#include "ute_module_filesystem.h"
#include "ute_application_common.h"
#include "ute_drv_battery_common.h"
#include "ute_module_factorytest.h"
#include "ute_module_newFactoryTest.h"

#if UTE_MODULE_SHIP_MODE_SUPPORT

ute_module_ship_mode_data_t uteModuleShipModeData;
/**
 *@brief    船运模式初始化
 *@details
 *@author        zn.zeng
 *@date        2022-08-02
 */
void uteModuleShipModeInit(void)
{
    memset(&uteModuleShipModeData, 0, sizeof(ute_module_ship_mode_data_t));
    void *file = NULL;
    uint8_t readbuff[2];
    readbuff[0] = 0;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SHIP_MODE, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleShipModeData.mode = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,mode=%d", __func__, uteModuleShipModeData.mode);
}
/**
 *@brief    当前是否在船运模式
 *@details
 *@author        zn.zeng
 *@date        2022-08-02
 */
bool uteModuleShipModeIsShip(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.mode=%d", __func__, uteModuleShipModeData.mode);
    if (uteModuleShipModeData.mode == SHIP_SHIP_MODE)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
 *@brief    进入船运模式
 *@details
 *@author        zn.zeng
 *@date        2022-08-02
 */
void uteModuleShipModeEnter(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,", __func__);
    uteModuleShipModeData.mode = SHIP_SHIP_MODE;
    void *file = NULL;
    uint8_t readbuff[2];
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SHIP_MODE, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        readbuff[0] = uteModuleShipModeData.mode;
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &readbuff[0], 1);
        uteModuleFilesystemCloseFile(file);
    }
    func_deepsleep();
}

/**
 *@brief    退出船运模式
 *@details
 *@author        zn.zeng
 *@date        2022-08-02
 */
void uteModuleShipModeExit(void)
{
    uteModuleShipModeData.berforeSecondCnt = 0;
    uteModuleShipModeData.mode = SHIP_NORMAL_MODE;
    void *file = NULL;
    uint8_t readbuff[2];
    readbuff[0] = uteModuleShipModeData.mode;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SHIP_MODE, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &readbuff[0], 1);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,mode=%d", __func__, uteModuleShipModeData.mode);
}
/**
 *@brief    每秒运行一次
 *@details
 *@author        zn.zeng
 *@date        2022-08-02
 */
void uteModuleShipModeEverySecond(void)
{
#if UTE_MODULE_SHIP_MODE_UNBOUND_AUTO_POWER_OFF_SUPPORT
    if (uteModuleShipModeData.mode == SHIP_SHIP_MODE)
    {
        if (uteApplicationCommonIsPowerOn() && (uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_NO_CHARGE) && (uteModuleFactoryTestGetCurrTestItem() == TEST_ITEM_NONE)
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
            && (uteModuleNewFactoryTestGetMode() == FACTORY_TEST_MODE_NULL)
#endif
           )
        {
            uteModuleShipModeData.berforeSecondCnt++;
            if (!uteModuleGuiCommonIsDisplayOn() && uteModuleShipModeData.berforeSecondCnt >= UTE_MODULE_SHIP_MODE_UNBOUND_AUTO_POWER_OFF_SECONDS)
            {
                func_deepsleep();
            }
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,berforeSecondCnt=%d", __func__, uteModuleShipModeData.berforeSecondCnt);
        }
    }
#endif
}
#endif
