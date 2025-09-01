/**
*@file
*@brief        女性生理周期处理模块
*@details
*@author       dengli.lu
*@date       2021-11-24
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_module_menstrualcycle.h"
#include "ute_module_systemtime.h"
#include "ute_application_common.h"

ute_menstrual_cycle_param_t uteModuleMenstrualCycleData;
/* menstrualcycle 互斥量 zn.zeng 2022-02-14*/
void *uteModuleMenstrualcycleMute;

/**
*@brief  读取配置文件
*@details
*@author        zn.zeng
*@date        2022-01-27
*/
void uteModuleMenstrualCycleReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[8];
    readbuff[0] = UTE_MODULE_MENSTRUALCYCLE_OPEN;
    readbuff[1] = (UTE_MODULE_MENSTRUALCYCLE_LAST_CYCLE_TIME_YEAR>>8)&0xFF;
    readbuff[2] = (UTE_MODULE_MENSTRUALCYCLE_LAST_CYCLE_TIME_YEAR>>0)&0xFF;;
    readbuff[3] = UTE_MODULE_MENSTRUALCYCLE_LAST_CYCLE_TIME_MONTH;
    readbuff[4] = UTE_MODULE_MENSTRUALCYCLE_LAST_CYCLE_TIME_DAY;
    readbuff[5] = UTE_MODULE_MENSTRUALCYCLE_KEEP_DAYS;
    readbuff[6] = UTE_MODULE_MENSTRUALCYCLE_CYCLE_DAYS;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_MENSTRUALCYCLE_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],7);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleMenstrualCycleData.reminderSwitch = readbuff[0];
    memcpy(&uteModuleMenstrualCycleData.lastTimestamp[0],&readbuff[1],4);
    uteModuleMenstrualCycleData.keepDays = readbuff[5];
    uteModuleMenstrualCycleData.cycleDays = readbuff[6];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,last time = %04d-%02d-%02d", __func__,uteModuleMenstrualCycleData.lastTimestamp[0]<<8|uteModuleMenstrualCycleData.lastTimestamp[1],uteModuleMenstrualCycleData.lastTimestamp[2],uteModuleMenstrualCycleData.lastTimestamp[3]);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,reminderSwitch=%d,keepDays=%02d,cycleDays=%02d", __func__,uteModuleMenstrualCycleData.reminderSwitch,uteModuleMenstrualCycleData.keepDays,uteModuleMenstrualCycleData.cycleDays);
}
/**
*@brief  初始化函数
*@details
*@author        dengli.lu
*@date        2021-11-24
*/
void uteModuleMenstrualCycleInit(void)
{
    uteModuleMenstrualCycleReadConfig();
    uteModulePlatformCreateMutex(&uteModuleMenstrualcycleMute);
}
/**
*@brief  设置女性生理周期参数
*@details
*@author        dengli.lu
*@date        2021-11-24
*/
void uteModuleMenstrualCycleSetParam(ute_menstrual_cycle_param_t param)
{
    uteModulePlatformTakeMutex(uteModuleMenstrualcycleMute);
    memcpy(&uteModuleMenstrualCycleData, &param, sizeof(ute_menstrual_cycle_param_t));
    uteModulePlatformGiveMutex(uteModuleMenstrualcycleMute);
    void *file = NULL;
    uint8_t writeBuff[8];
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_MENSTRUALCYCLE_INFO, &file, FS_O_TRUNC | FS_O_WRONLY | FS_O_CREAT))
    {
        writeBuff[0] = uteModuleMenstrualCycleData.reminderSwitch;
        writeBuff[1] = uteModuleMenstrualCycleData.lastTimestamp[0];
        writeBuff[2] = uteModuleMenstrualCycleData.lastTimestamp[1];
        writeBuff[3] = uteModuleMenstrualCycleData.lastTimestamp[2];
        writeBuff[4] = uteModuleMenstrualCycleData.lastTimestamp[3];
        writeBuff[5] = uteModuleMenstrualCycleData.keepDays;
        writeBuff[6] = uteModuleMenstrualCycleData.cycleDays;
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writeBuff[0], 7);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,last time = %04d-%02d-%02d", __func__,uteModuleMenstrualCycleData.lastTimestamp[0]<<8|uteModuleMenstrualCycleData.lastTimestamp[1],uteModuleMenstrualCycleData.lastTimestamp[2],uteModuleMenstrualCycleData.lastTimestamp[3]);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,reminderSwitch=%d,keepDays=%02d,cycleDays=%02d", __func__,uteModuleMenstrualCycleData.reminderSwitch,uteModuleMenstrualCycleData.keepDays,uteModuleMenstrualCycleData.cycleDays);
}
/**
*@brief  Casen add 计算给定日期到0年3月1日的天数，然后相减，获取天数的间隔。
*@details
*m1 = (month_start + 9) % 12; 用于判断日期是否大于3月（2月是判断闰年的标识），还用于纪录到3月的间隔月数。
*y1 = year_start - m1/10; 如果是1月和2月，则不包括当前年（因为是计算到0年3月1日的天数）。
*d1 = 365*y1 + y1/4 - y1/100 + y1/400 + (m1*306 + 5)/10 + (day_start - 1);其中 365*y1 是不算闰年多出那一天的天数，
*y1/4 - y1/100 + y1/400  是加所有闰年多出的那一天，
*(m2*306 + 5)/10 用于计算到当前月到3月1日间的天数，306=365-31-28（1月和2月），5是全年中不是31天月份的个数
*(day_start - 1) 用于计算当前日到1日的间隔天数。
*@author        dengli.lu
*@date        2021-11-24
*/
int uteModuleMenstrualCycleGetDiffDay(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end)
{
    int year1, month1, day1;
    int year2, month2, day2;
    month1 = (month_start + 9) % 12;
    year1 = year_start - month1/10;
    day1 = 365*year1 + year1/4 - year1/100 + year1/400 + (month1*306 + 5)/10 + (day_start - 1);
    month2 = (month_end + 9) % 12;
    year2 = year_end - month2/10;
    day2 = 365*year2 + year2/4 - year2/100 + year2/400 + (month2*306 + 5)/10 + (day_end - 1);
    return (day2 - day1);
}
/**
*@brief     获取女性生理周期参数
*@details
*@param[in] menstrualStatus=1，月经期;
*@param[in] menstrualStatus=2，排卵期;
*@param[in] menstrualStatus=3，安全期;
*@author       dengli.lu
*@date       2021-11-24
*/
bool uteModuleMenstrualCycleGetStatus(uint8_t *menstrualStatus)
{
    bool rtn = false;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if(uteModuleMenstrualCycleData.reminderSwitch)
    {
        uteModulePlatformTakeMutex(uteModuleMenstrualcycleMute);
        int menstrualYear,menstrualMonth,menstrualDay;
        menstrualYear = uteModuleMenstrualCycleData.lastTimestamp[0]<<8|uteModuleMenstrualCycleData.lastTimestamp[1];
        menstrualMonth = uteModuleMenstrualCycleData.lastTimestamp[2];
        menstrualDay = uteModuleMenstrualCycleData.lastTimestamp[3];
        int diffDays = uteModuleMenstrualCycleGetDiffDay(menstrualYear,menstrualMonth,menstrualDay,time.year,time.month,time.day);
        int judgeDays  = diffDays%uteModuleMenstrualCycleData.cycleDays;
        if(diffDays>=0)
        {
            if(judgeDays<uteModuleMenstrualCycleData.keepDays)
            {
                *menstrualStatus = 1;
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"Today is Menstrual period !!!");
            }
            else if((judgeDays>=(uteModuleMenstrualCycleData.cycleDays-19))&&(judgeDays<=(uteModuleMenstrualCycleData.cycleDays-10)))
            {
                *menstrualStatus = 2;
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"Today is Ovulation !!!");
            }
            else
            {
                *menstrualStatus = 3;
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"Today is Safety period !!!");
            }
            rtn = true;
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"The current time is wrong because it is less than the last time !!!");
        }
        uteModulePlatformGiveMutex(uteModuleMenstrualcycleMute);
    }
    return rtn;
}

/**
*@brief     获取女性生理周期是否开启
*@details
*@author       xjc
*@date       2022-01-10
*/
bool uteModuleMenstrualCycleIsOpen(void)
{
    bool ret = false;
    ute_personal_info_t personal;
    uteApplicationCommonGetPersonalInfo(&personal);
    if(uteModuleMenstrualCycleData.reminderSwitch && (!personal.isMan))
    {
        return true;
    }
    return ret;
}
