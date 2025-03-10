/**
*@file
*@brief        抽烟模块
*@details
*@author       zn.zeng
*@date       2021-08-25
*@version      v1.0
*/
#include "ute_module_smoke.h"
#include "ute_module_log.h"
#include "ute_module_filesystem.h"
#include "ute_module_charencode.h"
ute_module_smoke_data_t uteModuleSmokeData;
// 定义吸烟等级的默认值
#define SMOKE_LEVEL  1
// 定义总吸烟时间的默认值
#define SMOKE_TIME_SUM  0
// 定义总抽吸口数的默认值
#define SMOKE_COUNT_SUM  0
// 定义抽吸功率的默认值
#define SMOKE_WATTAGE   5

/**
* @brief  功率等级参数读取
* @details 从文件系统中读取与吸烟相关的功率等级、总吸烟时间、总抽吸口数等配置参数，并更新到全局的烟弹数据结构体中。
*/
static void uteModuleSmokeDataReadConfig(void)
{

    void *file;
    uint8_t readbuff[7+ 24 * sizeof(uint32_t)];
    memset(&readbuff[0],0,7);
    readbuff[0] = SMOKE_LEVEL;
    readbuff[1] = (SMOKE_TIME_SUM>>8)&0xff;
    readbuff[2] = SMOKE_TIME_SUM&0xff;
    readbuff[3] =  (SMOKE_COUNT_SUM>>8)&0xff;
    readbuff[4] =  SMOKE_COUNT_SUM&0xff;
    readbuff[5] =  SMOKE_WATTAGE;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SMOKE_LEVEL_FILE,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],sizeof(readbuff));
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleSmokeData.somke_capacity_level = readbuff[0];
    uteModuleSmokeData.total_smoking_times = readbuff[1]<<8|readbuff[2];
    uteModuleSmokeData.total_smoking_count = readbuff[3]<<8|readbuff[4];
    uteModuleSmokeData.total_smoking_wattage = readbuff[5];

    // 读取每小时的口数信息
    for (int i = 0; i < 24; i++)
    {
        uint32_t *count_ptr = (uint32_t *)&readbuff[7 + i * sizeof(uint32_t)];
        uteModuleSmokeData.smoking_count_per_hour[i] = *count_ptr;
    }
    UTE_MODULE_LOG(0, "%s,.somke_capacity_level=%d,total_smoking_times=%d,total_smoking_count=%d",
                   __func__,uteModuleSmokeData.somke_capacity_level,uteModuleSmokeData.total_smoking_times,uteModuleSmokeData.total_smoking_count);

}
/**
* @brief 抽烟参数保存
* @details 将全局烟弹数据结构体中的吸烟相关配置参数（吸烟容量等级、总吸烟时间、总抽吸口数、抽吸功率）保存到文件系统中。
*/
void uteModuleSmokeDataSaveConfig(void)
{

    void *file;
    uint8_t writebuff[7 + 24 * sizeof(uint32_t)];
    writebuff[0] = uteModuleSmokeData.somke_capacity_level;
    writebuff[1] = (uteModuleSmokeData.total_smoking_times>>8)&0xff;
    writebuff[2] = uteModuleSmokeData.total_smoking_times&0xff;
    writebuff[3] = (uteModuleSmokeData.total_smoking_count>>8)&0xff;
    writebuff[4] = uteModuleSmokeData.total_smoking_count&0xff;
    writebuff[5] = uteModuleSmokeData.total_smoking_wattage;

    // 保存每小时的口数信息
    for (int i = 0; i < 24; i++)
    {
        uint32_t *count_ptr = (uint32_t *)&writebuff[7 + i * sizeof(uint32_t)];
        *count_ptr = uteModuleSmokeData.smoking_count_per_hour[i];
    }

    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SMOKE_LEVEL_FILE,&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],sizeof(writebuff));
        uteModuleFilesystemCloseFile(file);
    }

}

// 设置总吸烟时间的函数
void uteModuleSetSomkeTime(uint32_t time)
{
    uteModuleSmokeData.total_smoking_times = time;
    uteModuleSmokeDataSaveConfig();
}
// 获取总吸烟时间的函数
uint32_t uteModuleGetSomkeSomkeTime(void)
{
    return uteModuleSmokeData.total_smoking_times;
}

// 设置总抽吸口数的函数
void uteModuleSetSomkeCount(uint32_t count)
{
    uteModuleSmokeData.total_smoking_count = count;
    uteModuleSmokeDataSaveConfig();
}
// 获取总抽吸口数的函数
uint32_t uteModuleGetSomkeSomkeCount(void)
{
    return uteModuleSmokeData.total_smoking_count;
}

// 获取一天中每小时的口数
void uteModuleGetSmokingCountPerHour(uint32_t *counts)
{
    for (int i = 0; i < 24; i++)
    {
        counts[i] = uteModuleSmokeData.smoking_count_per_hour[i];
    }
}












