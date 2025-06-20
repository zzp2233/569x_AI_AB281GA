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

#if UTE_MODULE_KEY_SET_FUNCTION_SUPPORT
#include "ute_module_keysetfunc.h"

ute_module_keysetfunc_data_t uteModuleKeySetFuncData;
/**
*@brief        初始化函数
*@details
*@author        Yin
*@date        2025.06.20
*/
void uteModuleKeySetFuncInit(void)
{
    memset(&uteModuleKeySetFuncData,0,sizeof(ute_module_keysetfunc_data_t));
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
    memset(readbuff,0,2);
    readbuff[0] = 0;
    readbuff[1] = false;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_KEY_SET_FUNCTION,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],2);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleKeySetFuncData.key_set_menu=readbuff[0];
    uteModuleKeySetFuncData.key_set_flag=readbuff[1];
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
    writebuff[0] = uteModuleKeySetFuncData.key_set_menu;
    writebuff[1] = uteModuleKeySetFuncData.key_set_flag;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_KEY_SET_FUNCTION,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],2);
        uteModuleFilesystemCloseFile(file);
    }
}

#endif //UTE_MODULE_KEY_SET_FUNCTION_SUPPORT