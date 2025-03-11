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


static uint8_t last_day = 0;
static uint8_t last_weekday = 0;

/**
* @brief  功率等级参数读取
* @details 读取总吸口数等配置参数。
*/
static void uteModuleSmokeDataReadConfig(void)
{

    void *file;
    uint8_t readbuff[24 * sizeof(uint32_t) + 7 * sizeof(uint32_t)];
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SMOKE_LEVEL_FILE,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],sizeof(readbuff));
        uteModuleFilesystemCloseFile(file);
    }

    // 读取每小时的口数信息
    for (int i = 0; i < 24; i++)
    {
        uint32_t *count_ptr = (uint32_t *)&readbuff[i * sizeof(uint32_t)];
        uteModuleSmokeData.smoking_count_per_hour[i] = *count_ptr;
    }

    // 读取本周每天的口数信息
    for (int i = 0; i < 7; i++)
    {
        uint32_t *count_ptr = (uint32_t *)&readbuff[24 * sizeof(uint32_t) + i * sizeof(uint32_t)];
        uteModuleSmokeData.smoking_count_per_day[i] = *count_ptr;
    }

}
/**
* @brief 抽烟参数保存
* @details 将结构体中的抽吸口数保存。
*/
void uteModuleSmokeDataSaveConfig(void)
{

    void *file;
    uint8_t writebuff[24 * sizeof(uint32_t) + 7 * sizeof(uint32_t)];

    // 保存每小时的口数信息
    for (int i = 0; i < 24; i++)
    {
        uint32_t *count_ptr = (uint32_t *)&writebuff[i * sizeof(uint32_t)];
        *count_ptr = uteModuleSmokeData.smoking_count_per_hour[i];
    }

    // 保存本周每天的口数信息
    for (int i = 0; i < 7; i++)
    {
        uint32_t *count_ptr = (uint32_t *)&writebuff[24 * sizeof(uint32_t) + i * sizeof(uint32_t)];
        *count_ptr = uteModuleSmokeData.smoking_count_per_day[i];
    }

    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SMOKE_LEVEL_FILE,&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],sizeof(writebuff));
        uteModuleFilesystemCloseFile(file);
    }

}



// 检查是否跨天并更新数据
void check_and_update_day()
{
    ute_module_systemtime_time_t current_time;
    uteModuleSystemtimeGetTime(&current_time);

    if (last_day == 0)
    {
        last_day = current_time.day;
    }

    if (current_time.day != last_day)
    {
        // 跨天，重置当天每小时的口数数据
        memset(uteModuleSmokeData.smoking_count_per_hour, 0, sizeof(uteModuleSmokeData.smoking_count_per_hour));
        // 跨天也重置当天总口数
        uteModuleSmokeData.total_smoking_count = 0;
        last_day = current_time.day;
    }
}

// 检查是否跨周并更新数据
void check_and_update_week()
{
    ute_module_systemtime_time_t current_time;
    uteModuleSystemtimeGetTime(&current_time);

    if (last_weekday == 0)
    {
        last_weekday = current_time.week;
    }

    if (current_time.week < last_weekday)
    {
        // 跨周，重置本周每天的口数数据
        memset(uteModuleSmokeData.smoking_count_per_day, 0, sizeof(uteModuleSmokeData.smoking_count_per_day));
    }
    last_weekday = current_time.week;
}

// 获取当天口数总计
uint32_t get_today_smoking_count()
{
    uint32_t total = 0;
    for (int i = 0; i < 24; i++)
    {
        total += uteModuleSmokeData.smoking_count_per_hour[i];
    }
    return total;
}

// 获取本周口数总计
uint32_t get_this_week_smoking_count()
{
    uint32_t total = 0;
    for (int i = 0; i < 7; i++)
    {
        total += uteModuleSmokeData.smoking_count_per_day[i];
    }
    return total;
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

// 获取本周每天的口数
void uteModuleGetSmokingCountPerDay(uint32_t *counts)
{
    for (int i = 0; i < 7; i++)
    {
        counts[i] = uteModuleSmokeData.smoking_count_per_day[i];
    }
}











