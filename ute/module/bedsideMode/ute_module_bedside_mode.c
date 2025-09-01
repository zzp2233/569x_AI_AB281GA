/**
 * @file       ute_module_bedside_mode
 * @brief      床头钟模块
 * @details
 * @author     Wang.Luo
 * @date       2025-04-29
 * @version    v1.0
 */
#include "ute_module_bedside_mode.h"
#include "ute_module_filesystem.h"
#include "ute_module_platform.h"
#include "ute_module_log.h"

#if UTE_MODULE_BEDSIDE_MODE_SUPPORT

ute_module_bedside_mode_data_t uteModuleBedsideModeData;

/**
 * @brief        床头钟模块初始化
 * @details
 * @author       Wang.Luo
 * @date         2025-04-29
 */
void uteModuleBedsideModeInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s",__func__);
    memset(&uteModuleBedsideModeData,0,sizeof(ute_module_bedside_mode_data_t));
    uteModuleBedsideModeReadConfig();
    if (uteModuleBedsideModeData.isOpen)
    {
#if CHARGE_EN
        bsp_charge_set_stop_time(18000);
        bsp_charge_sta(sys_cb.charge_sta); // update充灯状态
        charge_set_detect_cnt(5);
#if CHARGE_VOL_DYNAMIC_DET
        if (sys_cb.chg_on)
        {
            gradient_process(0);
            RTCCON8 &= ~BIT(1); // charge open
        }
#endif
#endif // CHARGE_EN
    }
}

/**
 * @brief        床头钟模块读取配置
 * @details
 * @author       Wang.Luo
 * @date         2025-04-29
 */
void uteModuleBedsideModeReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[2];
    readbuff[0] = UTE_MODULE_BEDSIDE_MODE_DEFAULT_OPEN;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BEDSIDE_MODE_SET_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],sizeof(readbuff));
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleBedsideModeData.isOpen = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,isOpen=%d",__func__,uteModuleBedsideModeData.isOpen);
}

/**
 * @brief        床头钟模块写入配置
 * @details
 * @author       Wang.Luo
 * @date         2025-04-29
 */
void uteModuleBedsideModeSaveConfig(void)
{
    void *file = NULL;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleBedsideModeData.isOpen;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BEDSIDE_MODE_SET_INFO,&file,FS_O_CREAT | FS_O_WRONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],sizeof(writebuff));
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,isOpen=%d",__func__,uteModuleBedsideModeData.isOpen);
}

/**
 * @brief        获取床头钟模块是否打开
 * @details
 * @return       true:打开 false:关闭
 * @author       Wang.Luo
 * @date         2025-04-29
 */
bool uteModuleBedsideModeIsOpen(void)
{
    return uteModuleBedsideModeData.isOpen;
}

/**
 * @brief        设置床头钟模块是否打开
 * @details
 * @param[in]    isOpen:true:打开 false:关闭
 * @author       Wang.Luo
 * @date         2025-04-29
 */
void uteModuleBedsideModeSetOpen(bool isOpen)
{
    uteModuleBedsideModeData.isOpen = isOpen;
    uteModuleBedsideModeSaveConfig();
    if (uteModuleBedsideModeData.isOpen)
    {
#if CHARGE_EN
        bsp_charge_set_stop_time(18000);
        bsp_charge_sta(sys_cb.charge_sta); // update充灯状态
        charge_set_detect_cnt(5);
#if CHARGE_VOL_DYNAMIC_DET
        if (sys_cb.chg_on)
        {
            gradient_process(0);
            RTCCON8 &= ~BIT(1); // charge open
        }
#endif
#endif // CHARGE_EN
    }
}

/**
 * @brief        床头钟模块开关切换
 * @details
 * @author       Wang.Luo
 * @date         2025-04-29
 */
void uteModuleBedsideModeSwitch(void)
{
    uteModuleBedsideModeData.isOpen = !uteModuleBedsideModeData.isOpen;
    uteModuleBedsideModeSaveConfig();
    if (uteModuleBedsideModeData.isOpen)
    {
#if CHARGE_EN
        bsp_charge_set_stop_time(18000);
        bsp_charge_sta(sys_cb.charge_sta); // update充灯状态
        charge_set_detect_cnt(5);
#if CHARGE_VOL_DYNAMIC_DET
        if (sys_cb.chg_on)
        {
            gradient_process(0);
            RTCCON8 &= ~BIT(1); // charge open
        }
#endif
#endif // CHARGE_EN
    }
}

#endif
