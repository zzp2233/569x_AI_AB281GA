/**
*@file
*@brief     按键设置功能模块模块
*@details
*@author        Yin
*@date        2025.06.20
*@version       v1.0
*/
#include "ute_application_common.h"
#include "ute_module_filesystem.h"
#include "ute_module_keysetfunc.h"

#if UTE_MODULE_KEY_SET_FUNCTION_SUPPORT

ute_module_keysetfunc_data_t uteModuleKeySetFuncData;
/**
*@brief        初始化函数
*@details
*@author        Yin
*@date        2025.06.20
*/
void uteModuleKeySetFuncInit(void)
{
    memset(&uteModuleKeySetFuncData, 0, sizeof(ute_module_keysetfunc_data_t));
    uteModuleKeySetFuncReadConfig();
}

/**
*@brief        读取config
*@details
*@author        Yin
*@date        2025.06.19
*/
void uteModuleKeySetFuncReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[2];
    memset(readbuff, 0, 2);
    readbuff[0] = 0;
    readbuff[1] = false;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_KEY_SET_FUNCTION, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 2);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleKeySetFuncData.keySetMenu = readbuff[0];
    uteModuleKeySetFuncData.keySetFlag = readbuff[1];
}

/**
*@brief     保存数据到内存
*@details
*@author        Yin
*@date        2025.06.20
*/
void uteModuleKeySetFuncSaveData(void)
{
    /*! 保存到文件，dengli.lu, 2022-06-16*/
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleKeySetFuncData.keySetMenu;
    writebuff[1] = uteModuleKeySetFuncData.keySetFlag;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_KEY_SET_FUNCTION, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writebuff[0], 2);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
 * @brief        设置快捷键菜单
 * @details
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-06-20
 */
void uteModuleKeySetFuncSetMenu(uint8_t menu)
{
    if (menu && menu != FUNC_KEY_SET_FUNCTION && uteModuleKeySetFuncData.keySetMenu != menu)
    {
        uteModuleKeySetFuncData.keySetMenu = menu;
        uteModuleKeySetFuncData.keySetFlag = true;
        uteModuleKeySetFuncSaveData();
    }
}

/**
 * @brief        获取快捷键菜单
 * @details
 * @return       uint8_t
 * @author       Wang.Luo
 * @date         2025-06-20
 */
uint8_t uteModuleKeySetFuncGetMenu(void)
{
    uint8_t menu = uteModuleKeySetFuncData.keySetMenu;
    if (!uteModuleKeySetFuncData.keySetFlag || !uteModuleKeySetFuncData.keySetMenu)
    {
        menu = 0;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,menu", __func__, menu);
    return menu;
}

#endif //UTE_MODULE_KEY_SET_FUNCTION_SUPPORT