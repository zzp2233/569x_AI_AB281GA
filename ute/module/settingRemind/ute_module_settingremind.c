/**
*@file
*@brief     功能开启关闭提醒模块
*@details
*@author        Yin
*@date        2025.07.23
*@version       v1.0
*/
#include "ute_application_common.h"
#include "ute_module_filesystem.h"
#include "ute_module_settingremind.h"

#if UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT

ute_module_settingremind_data_t uteModuleSettingRemindData;
/**
*@brief        初始化函数
*@details
*@author        Yin
*@date        2025.07.23
*/
void uteModuleSettingRemindInit(void)
{
    memset(&uteModuleSettingRemindData, 0, sizeof(ute_module_settingremind_data_t));
    uteModuleSettingRemindReadConfig();
}

/**
*@brief        读取config
*@details
*@author        Yin
*@date        2025.07.23
*/
void uteModuleSettingRemindReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[1];
    memset(readbuff, 0, 1);
    readbuff[0] = 0;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SETTING_REMIND, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleSettingRemindData.id_state = readbuff[0];
}

/**
*@brief     保存数据到内存
*@details
*@author        Yin
*@date        2025.07.23
*/
void uteModuleSettingRemindSaveData(void)
{
    /*! 保存到文件，dengli.lu, 2022-06-16*/
    void *file;
    uint8_t writebuff[1];
    writebuff[0] = uteModuleSettingRemindData.id_state;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SETTING_REMIND, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writebuff[0], 1);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
 * @brief        设置提醒ID
 * @details
 * @return       void
 * @author        YIN
 * @date         2025-07-23
 */
void uteModuleSettingRemindSetRemindId(uint8_t id)
{
    uteModuleSettingRemindData.func_id = id;
}

/**
 * @brief        获取提醒ID
 * @details
 * @return       uint8_t
 * @author        YIN
 * @date         2025-07-23
 */
uint8_t uteModuleSettingRemindGetRemindId(void)
{
    return uteModuleSettingRemindData.func_id;
}


/**
 * @brief        设置不需要展示的提示界面
 * @details      1:不需要展示
 * @return       void
 * @author        YIN
 * @date         2025-07-23
 */
void uteModuleSettingRemindSetNotNeedShowId(uint8_t id_bit)
{
    uteModuleSettingRemindData.id_state |= BIT(id_bit);
    uteModuleSettingRemindSaveData();
}

/**
 * @brief        获取是否需要展示提示界面
 * @details
 * @return       bool 0:不展示    1:展示
 * @author        YIN
 * @date         2025-07-23
 */
bool uteModuleSettingRemindGetWhetherShowId(uint8_t id_bit)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, menu = %x\n", __func__, uteModuleSettingRemindData.id_state);
    if ((uteModuleSettingRemindData.id_state >> id_bit) & 0x1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

#endif

