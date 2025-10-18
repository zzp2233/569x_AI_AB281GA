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
#include "ute_drv_motor.h"
#include "func_cover.h"

ute_menstrual_cycle_param_t uteModuleMenstrualCycleData;
/* menstrualcycle 互斥量 zn.zeng 2022-02-14*/
void *uteModuleMenstrualcycleMute;
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
static uint8_t curSettingStatus;
static uint8_t remindStatus;
ute_menstrual_cycle_param_t uteModuleMenstrualSetParam;

#define UTE_MODULE_MENSTRUAL_CYCLE_DATA_LENGTH 18
#ifndef DEFAULT_MENSTRUAL_REMIND_SWITCH
#define DEFAULT_MENSTRUAL_REMIND_SWITCH false
#endif
#ifndef DEFAULT_OVULATION_REMIND_SWITCH
#define DEFAULT_OVULATION_REMIND_SWITCH false
#endif
#ifndef DEFAULT_MENSTRUAL_REMIND_INADVANCE
#define DEFAULT_MENSTRUAL_REMIND_INADVANCE 1
#endif
#ifndef DEFAULT_OVULATION_REMIND_INADVANCE
#define DEFAULT_OVULATION_REMIND_INADVANCE 1
#endif
#ifndef DEFAULT_MENSTRUAL_REMIND_HOUR_MIN_VALUE
#define DEFAULT_MENSTRUAL_REMIND_HOUR_MIN_VALUE 0X1400
#endif
#ifndef DEFAULT_OVULATION_REMIND_HOUR_MIN_VALUE
#define DEFAULT_OVULATION_REMIND_HOUR_MIN_VALUE 0X1400
#endif

#else
#define UTE_MODULE_MENSTRUAL_CYCLE_DATA_LENGTH  8
#endif
/**
*@brief  读取配置文件
*@details
*@author        zn.zeng
*@date        2022-01-27
*/
void uteModuleMenstrualCycleReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[UTE_MODULE_MENSTRUAL_CYCLE_DATA_LENGTH];
    readbuff[0] = UTE_MODULE_MENSTRUALCYCLE_OPEN;
    readbuff[1] = (UTE_MODULE_MENSTRUALCYCLE_LAST_CYCLE_TIME_YEAR>>8)&0xFF;
    readbuff[2] = (UTE_MODULE_MENSTRUALCYCLE_LAST_CYCLE_TIME_YEAR>>0)&0xFF;
    readbuff[3] = UTE_MODULE_MENSTRUALCYCLE_LAST_CYCLE_TIME_MONTH;
    readbuff[4] = UTE_MODULE_MENSTRUALCYCLE_LAST_CYCLE_TIME_DAY;
    readbuff[5] = UTE_MODULE_MENSTRUALCYCLE_KEEP_DAYS;
    readbuff[6] = UTE_MODULE_MENSTRUALCYCLE_CYCLE_DAYS;
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
    readbuff[7] = true; //默认初始化设置
    readbuff[8] = DEFAULT_MENSTRUAL_REMIND_SWITCH;
    readbuff[9] = DEFAULT_OVULATION_REMIND_SWITCH;
    readbuff[10] = DEFAULT_MENSTRUAL_REMIND_INADVANCE;
    readbuff[11] = (DEFAULT_MENSTRUAL_REMIND_HOUR_MIN_VALUE >> 8) & 0xff;
    readbuff[12] = DEFAULT_MENSTRUAL_REMIND_HOUR_MIN_VALUE & 0xff;
    readbuff[13] = DEFAULT_OVULATION_REMIND_INADVANCE;
    readbuff[14] = (DEFAULT_OVULATION_REMIND_HOUR_MIN_VALUE >> 8) & 0xff;
    readbuff[15] = DEFAULT_OVULATION_REMIND_HOUR_MIN_VALUE & 0xff;
    readbuff[16] = DEFAULT_MENSTRUAL_REMIND_END_SWITCH;
    readbuff[17] = DEFAULT_OVULATION_REMIND_END_SWITCH;
#endif
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_MENSTRUALCYCLE_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], UTE_MODULE_MENSTRUAL_CYCLE_DATA_LENGTH);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleMenstrualCycleData.reminderSwitch = readbuff[0];
    memcpy(&uteModuleMenstrualCycleData.lastTimestamp[0],&readbuff[1],4);
    uteModuleMenstrualCycleData.keepDays = readbuff[5];
    uteModuleMenstrualCycleData.cycleDays = readbuff[6];
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
    uteModuleMenstrualCycleData.isInitSet = readbuff[7];
    uteModuleMenstrualCycleData.isMenstrualRemindOpen = readbuff[8];
    uteModuleMenstrualCycleData.isOvulationRemindOpen = readbuff[9];
    memcpy(&uteModuleMenstrualCycleData.menstrualRemindTime[0], &readbuff[10], 3);
    memcpy(&uteModuleMenstrualCycleData.ovulationRemindTime[0], &readbuff[13], 3);
    memset(&uteModuleMenstrualSetParam,0,sizeof(uteModuleMenstrualSetParam));
    memcpy(&uteModuleMenstrualSetParam,&uteModuleMenstrualCycleData,sizeof(uteModuleMenstrualSetParam));
    uteModuleMenstrualCycleData.isMenstrualEndRemindOpen = readbuff[16];
    uteModuleMenstrualCycleData.isOvulationEndRemindOpen = readbuff[17];
#endif
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,last time = %04d-%02d-%02d", __func__,uteModuleMenstrualCycleData.lastTimestamp[0]<<8|uteModuleMenstrualCycleData.lastTimestamp[1],uteModuleMenstrualCycleData.lastTimestamp[2],uteModuleMenstrualCycleData.lastTimestamp[3]);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,reminderSwitch=%d,keepDays=%02d,cycleDays=%02d", __func__,uteModuleMenstrualCycleData.reminderSwitch,uteModuleMenstrualCycleData.keepDays,uteModuleMenstrualCycleData.cycleDays);
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isInitSet=%d,isMenstrualRemindOpen=%02d,isOvulationRemindOpen=%02d", __func__, uteModuleMenstrualCycleData.isInitSet, uteModuleMenstrualCycleData.isMenstrualRemindOpen, uteModuleMenstrualCycleData.isOvulationRemindOpen);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,menstrualRemindTime = %d-%02d-%02d", __func__, uteModuleMenstrualCycleData.menstrualRemindTime[0], uteModuleMenstrualCycleData.menstrualRemindTime[1], uteModuleMenstrualCycleData.menstrualRemindTime[2]);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,ovulationRemindTime = %d-%02d-%02d", __func__, uteModuleMenstrualCycleData.ovulationRemindTime[0], uteModuleMenstrualCycleData.ovulationRemindTime[1], uteModuleMenstrualCycleData.ovulationRemindTime[2]);
#endif
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
    uint8_t writeBuff[UTE_MODULE_MENSTRUAL_CYCLE_DATA_LENGTH];
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_MENSTRUALCYCLE_INFO, &file, FS_O_TRUNC | FS_O_WRONLY | FS_O_CREAT))
    {
        writeBuff[0] = uteModuleMenstrualCycleData.reminderSwitch;
        writeBuff[1] = uteModuleMenstrualCycleData.lastTimestamp[0];
        writeBuff[2] = uteModuleMenstrualCycleData.lastTimestamp[1];
        writeBuff[3] = uteModuleMenstrualCycleData.lastTimestamp[2];
        writeBuff[4] = uteModuleMenstrualCycleData.lastTimestamp[3];
        writeBuff[5] = uteModuleMenstrualCycleData.keepDays;
        writeBuff[6] = uteModuleMenstrualCycleData.cycleDays;
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
        writeBuff[7] = uteModuleMenstrualCycleData.isInitSet;
        writeBuff[8] = uteModuleMenstrualCycleData.isMenstrualRemindOpen;
        writeBuff[9] = uteModuleMenstrualCycleData.isOvulationRemindOpen;
        memcpy(&writeBuff[10], &uteModuleMenstrualCycleData.menstrualRemindTime[0], 3);
        memcpy(&writeBuff[13], &uteModuleMenstrualCycleData.ovulationRemindTime[0], 3);
        writeBuff[16] = uteModuleMenstrualCycleData.isMenstrualEndRemindOpen;
        writeBuff[17] = uteModuleMenstrualCycleData.isOvulationEndRemindOpen;
#endif
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writeBuff[0], UTE_MODULE_MENSTRUAL_CYCLE_DATA_LENGTH);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,last time = %04d-%02d-%02d", __func__,uteModuleMenstrualCycleData.lastTimestamp[0]<<8|uteModuleMenstrualCycleData.lastTimestamp[1],uteModuleMenstrualCycleData.lastTimestamp[2],uteModuleMenstrualCycleData.lastTimestamp[3]);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,reminderSwitch=%d,keepDays=%02d,cycleDays=%02d", __func__,uteModuleMenstrualCycleData.reminderSwitch,uteModuleMenstrualCycleData.keepDays,uteModuleMenstrualCycleData.cycleDays);
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isInitSet=%d,isMenstrualRemindOpen=%02d,isOvulationRemindOpen=%02d", __func__, uteModuleMenstrualCycleData.isInitSet, uteModuleMenstrualCycleData.isMenstrualRemindOpen, uteModuleMenstrualCycleData.isOvulationRemindOpen);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isInitSet=%d,isMenstrualEndRemindOpen=%02d,isOvulationEndRemindOpen=%02d", __func__, uteModuleMenstrualCycleData.isInitSet, uteModuleMenstrualCycleData.isMenstrualEndRemindOpen, uteModuleMenstrualCycleData.isOvulationEndRemindOpen);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,menstrualRemindTime = %d-%02d-%02d", __func__, uteModuleMenstrualCycleData.menstrualRemindTime[0], uteModuleMenstrualCycleData.menstrualRemindTime[1], uteModuleMenstrualCycleData.menstrualRemindTime[2] );
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,ovulationRemindTime = %d-%02d-%02d", __func__, uteModuleMenstrualCycleData.ovulationRemindTime[0], uteModuleMenstrualCycleData.ovulationRemindTime[1], uteModuleMenstrualCycleData.ovulationRemindTime[2]);
#endif
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
#if !APP_MODULE_MENSTRUAL_CYCLE_NOT_GENDER_SUPPORT
    ute_personal_info_t personal;
    uteApplicationCommonGetPersonalInfo(&personal);
    if(uteModuleMenstrualCycleData.reminderSwitch && (!personal.isMan))
    {
        return true;
    }
#else
    ret = uteModuleMenstrualCycleData.reminderSwitch;
#endif
    return ret;
}
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
/**
*@brief  设置临时女性生理周期参数
*@details
*@author        raymond
*@date        2022-03-05
*/
void uteModuleMenstrualCycleSetTempParam(ute_menstrual_cycle_param_t *param)
{
    uteModulePlatformTakeMutex(uteModuleMenstrualcycleMute);
    memcpy(&uteModuleMenstrualCycleData, param, sizeof(ute_menstrual_cycle_param_t));
    uteModulePlatformGiveMutex(uteModuleMenstrualcycleMute);
}
/**
*@brief  设置提醒设置界面
*@details
*@author        raymond
*@date        2022-03-05
*/
void uteModuleMenstrualCycleSetSettingStatus(uint8_t setStatus)
{
    curSettingStatus = setStatus;
}
/**
*@brief  获取设置界面
*@details
*@author        raymond
*@date        2022-03-05
*/
uint8_t uteModuleMenstrualCycleGetSettingStatus(void)
{
    return curSettingStatus;
}

/**
*@brief  获取女性生理周期参数
*@details
*@author        raymond
*@date        2022-03-05
*/
void uteModuleMenstrualCycleGetParam(ute_menstrual_cycle_param_t *param)
{
    uteModulePlatformTakeMutex(uteModuleMenstrualcycleMute);
    memcpy(param, &uteModuleMenstrualCycleData, sizeof(ute_menstrual_cycle_param_t));
    uteModulePlatformGiveMutex(uteModuleMenstrualcycleMute);
}

/**
*@brief  获取某年某月第一天是星期几
*@details
公元1年1月1日为星期一
利用uteModuleMenstrualCycleGetDiffDay计算target_year年target_month月第一天到公元1年1月1日的天数
返回值weekDay = 1,即为星期一
*@author        raymond
*@date        2022-03-05
*/
uint8_t  uteModuleMenstrualCycleWeekDay(int target_year, int target_month)
{
    uint8_t weekDay = 0;
    int diffDays = 0;
    diffDays = uteModuleMenstrualCycleGetDiffDay(1, 1, 1, target_year, target_month,1);
    weekDay = diffDays%7 + 1;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,diffDays=%d,weekDay=%d target_month =%d", __func__, diffDays, weekDay, target_month);
    return weekDay;
}
/**
*@brief     获取女性生理周期参数
*@details
*@return    menstrualStatus   1,月经期; 2,排卵期; 3,排卵日; 4,安全期1; 5,安全期2;
*@author       raymond
*@date       2022-03-07
*/
uint8_t uteModuleMenstrualCycleGetStatusByTime(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t menstrualStatus = 0;
    uteModulePlatformTakeMutex(uteModuleMenstrualcycleMute);
    int menstrualYear, menstrualMonth, menstrualDay;
    int judgeDays;
    menstrualYear = uteModuleMenstrualCycleData.lastTimestamp[0] << 8 | uteModuleMenstrualCycleData.lastTimestamp[1];
    menstrualMonth = uteModuleMenstrualCycleData.lastTimestamp[2];
    menstrualDay = uteModuleMenstrualCycleData.lastTimestamp[3];
    int diffDays = uteModuleMenstrualCycleGetDiffDay(menstrualYear, menstrualMonth, menstrualDay,year,month,day);
    if (diffDays >= 0)
    {
        judgeDays = diffDays % uteModuleMenstrualCycleData.cycleDays;
    }
    else
    {
        judgeDays = (uteModuleMenstrualCycleData.cycleDays - (abs(diffDays) % uteModuleMenstrualCycleData.cycleDays) ) % uteModuleMenstrualCycleData.cycleDays;
        // menstrualStatus = 0;
        // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "The current time is wrong because it is less than the last time !!!");
    }
    if (judgeDays < uteModuleMenstrualCycleData.keepDays)
    {
        menstrualStatus = 1;
        // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "Today is Menstrual period !!!");
    }
    else if((judgeDays >= uteModuleMenstrualCycleData.keepDays) && (judgeDays < (uteModuleMenstrualCycleData.cycleDays - 19)))
    {
        menstrualStatus = 4;
    }
    else if ((judgeDays >= (uteModuleMenstrualCycleData.cycleDays - 19)) && (judgeDays <= (uteModuleMenstrualCycleData.cycleDays - 10)))
    {
        if(judgeDays == (uteModuleMenstrualCycleData.cycleDays - 14))
        {
            menstrualStatus = 3;
        }
        else
        {
            menstrualStatus = 2;
        }
        // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "Today is Ovulation !!!");
    }
    else
    {
        menstrualStatus = 5;
        // UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "Today is Safety period !!!");
    }
    uteModulePlatformGiveMutex(uteModuleMenstrualcycleMute);
    return menstrualStatus;
}

/**
*@brief     当前时间到下一次经期的时间
*@details
*@author       raymond
*@date       2022-03-07
*/
int8_t uteModuleMenstrualCycleGetRemaindays(void)
{
    int8_t remaindays = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uteModulePlatformTakeMutex(uteModuleMenstrualcycleMute);
    int menstrualYear, menstrualMonth, menstrualDay;
    menstrualYear = uteModuleMenstrualCycleData.lastTimestamp[0] << 8 | uteModuleMenstrualCycleData.lastTimestamp[1];
    menstrualMonth = uteModuleMenstrualCycleData.lastTimestamp[2];
    menstrualDay = uteModuleMenstrualCycleData.lastTimestamp[3];
    int diffDays = uteModuleMenstrualCycleGetDiffDay(menstrualYear, menstrualMonth, menstrualDay, time.year, time.month, time.day);
    int judgeDays = uteModuleMenstrualCycleData.cycleDays > 0 ? diffDays % uteModuleMenstrualCycleData.cycleDays : 0;
    if (diffDays >= 0)
    {
        remaindays = uteModuleMenstrualCycleData.cycleDays - judgeDays;
        if (remaindays == 0)
        {
            remaindays = uteModuleMenstrualCycleData.cycleDays;
        }
    }
    else
    {
        remaindays = -1;
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "The current time is wrong because it is less than the last time !!!");
    }
    uteModulePlatformGiveMutex(uteModuleMenstrualcycleMute);
//    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s remaindays = %d",__func__,remaindays);
    return remaindays;
}

/**
*@brief        判断当前月份的天数函数
*@details
*@author       raymond
*@date       2022-03-07
*/
uint8_t isLeapYear(uint16_t year)
{
    uint8_t leapAdd = (((year % 100 != 0) && (year % 4 == 0)) || (year % 400 == 0));
    return leapAdd;
}
/**
*@brief        判断当前月份的天数函数
*@details
*@author       raymond
*@date       2022-03-07
*/
uint8_t uteModuleMenstrualCycleJudgeMonthDay(uint16_t year,uint8_t month)
{
    uint8_t leapAdd = isLeapYear(year);
    switch(month)
    {
        case 2:
            return (28 + leapAdd);
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            return (31);
        default :
            return (30);
    }
}
/**
*@brief  提醒回调函数
*@details
*@author        raymond
*@date        2022-03-14
*/
#if UTE_MODULE_SCREENS_MENSTRUAL_NOTIFY_SUPPORT
void uteModuleMenstrualCycleRemindCb(void)
{
    uint8_t remainday = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    remainday = uteModuleMenstrualCycleGetRemaindays();
#if 0
    if(time.sec==0)
    {
        if (
            (remainday == uteModuleMenstrualCycleData.menstrualRemindTime[0])&&
            (time.hour == uteModuleMenstrualCycleData.menstrualRemindTime[1]) &&
            (time.min == uteModuleMenstrualCycleData.menstrualRemindTime[2]) &&
            uteModuleMenstrualCycleData.isMenstrualRemindOpen)
        {
            //uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_MENSTRUAL_NOTIFY_ID);
            sys_cb.cover_index = REMIND_COVER_MENSTRUAL;
            sys_cb.remind_tag = true;
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
            remindStatus = REMIND_MENSTRUAL;
        }
        else if (
            (remainday == (uteModuleMenstrualCycleData.ovulationRemindTime[0]+14))&&
            (time.hour == uteModuleMenstrualCycleData.ovulationRemindTime[1]) &&
            (time.min == uteModuleMenstrualCycleData.ovulationRemindTime[2]) &&
            uteModuleMenstrualCycleData.isOvulationRemindOpen)
        {
            //uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_MENSTRUAL_NOTIFY_ID);
            sys_cb.cover_index = REMIND_COVER_MENSTRUAL;
            sys_cb.remind_tag = true;
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,2);
            remindStatus = REMIND_OVULATION;
        }
        else
        {
            remindStatus = REMIND_IDLE;
        }
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s isMenstrualRemindOpen = %d,isOvulationRemindOpen=%d",__func__,remainday);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "mensday = %d,menshour = %d,mensmin = %d",uteModuleMenstrualCycleData.menstrualRemindTime[0],uteModuleMenstrualCycleData.menstrualRemindTime[1],uteModuleMenstrualCycleData.menstrualRemindTime[2]);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "ovusday = %d,ovuhour = %d,ovumin = %d",uteModuleMenstrualCycleData.ovulationRemindTime[0],uteModuleMenstrualCycleData.ovulationRemindTime[1],uteModuleMenstrualCycleData.ovulationRemindTime[2]);
    }
#else
    if(time.hour == 8 && time.min == 0)
    {
        if (uteModuleMenstrualCycleData.keepDays == 0 || uteModuleMenstrualCycleData.lastTimestamp[3] == 0 || uteModuleMenstrualCycleData.cycleDays < 15 || !uteModuleMenstrualCycleIsOpen())
        {
            return;
        }
        else
        {
            u8 menstrualStatus = uteModuleMenstrualCycleGetStatusByTime(time.year,time.month,time.day);
            if(remainday == uteModuleMenstrualCycleData.cycleDays && (menstrualStatus == 1))
            {
                if (uteModuleMenstrualCycleData.isMenstrualRemindOpen)
                {
                    sys_cb.cover_index = REMIND_COVER_MENSTRUAL;
                    remindStatus = 1;
                    sys_cb.remind_tag = true;
                }
            }
            else if(remainday == (uteModuleMenstrualCycleData.cycleDays - uteModuleMenstrualCycleData.keepDays + 1) && (menstrualStatus == 1))
            {
                if (uteModuleMenstrualCycleData.isMenstrualEndRemindOpen)
                {
                    sys_cb.cover_index = REMIND_COVER_MENSTRUAL;
                    remindStatus = 2;
                    sys_cb.remind_tag = true;
                }
            }
            else if(remainday == (uteModuleMenstrualCycleData.cycleDays - 9) && (menstrualStatus == 2))
            {
                if (uteModuleMenstrualCycleData.isOvulationRemindOpen)
                {
                    sys_cb.cover_index = REMIND_COVER_MENSTRUAL;
                    remindStatus = 3;
                    sys_cb.remind_tag = true;
                }
            }
            else if(remainday == (uteModuleMenstrualCycleData.cycleDays - 19 + 1) && (menstrualStatus == 2))
            {
                if (uteModuleMenstrualCycleData.isOvulationEndRemindOpen)
                {
                    sys_cb.cover_index = REMIND_COVER_MENSTRUAL;
                    remindStatus = 4;
                    sys_cb.remind_tag = true;
                }
            }
        }
    }
#endif
}
#endif
/**
*@brief  获取提醒类型
*@details
*@author        raymond
*@date        2022-03-05
*/
uint8_t uteModuleMenstrualCycleGetRemindStatus(void)
{
    return remindStatus;
}

/**
*@brief  计算输入日期在该年的天数
*@details
*@author        raymond
*@date        2022-03-05
*/
uint16_t uteModuleMenstrualCycledaysCaculate(uint16_t year,uint8_t month,uint8_t day)
{
    uint16_t retDay = 0;
    uint8_t every_month[12] = {31,28,31,30,31,30,31,31,30,31,30,31};
    every_month[1] = uteModuleMenstrualCycleJudgeMonthDay(year, 2);
    for(uint8_t i=0; i<(month-1); i++)
    {
        retDay = retDay + every_month[i];
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s every_month[1] = %d,retDay=%d ,day = %d", __func__, every_month[1], retDay, day);

    return (retDay + day);
}
/**
*@brief  计算输入天数在该年的日期
*@details
*@author        raymond
*@date        2022-03-05
*/
uint16_t uteModuleMenstrualCycleCalDateFromDays(int16_t totalday,uint16_t year,uint8_t *month, uint8_t *day)
{
    uint16_t retYear = 0;
    uint16_t day_sum = 0;
    int16_t tempTtotalday = totalday;
    uint8_t every_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    every_month[1] = (isLeapYear(year)) ? 29 : 28;
    for(uint8_t i=0; i<12; i++)
    {
        day_sum += every_month[i];
    }
    if(tempTtotalday <= 0) //跨上一年
    {
        every_month[1] = (isLeapYear(year-1)) ? 29 : 28;
        day_sum = 0;
        for(uint8_t i=0; i<12; i++)
        {
            day_sum += every_month[i];
        }
        tempTtotalday = day_sum + tempTtotalday;
        retYear = year-1;
    }
    else if(tempTtotalday > day_sum) //跨下一年
    {
        every_month[1] = (isLeapYear(year+1)) ? 29 : 28;
        tempTtotalday = tempTtotalday - day_sum;
        retYear = year+1;
    }
    else
    {
        retYear = year;
    }

    day_sum = 0;
    for(uint8_t i=0; i<11; i++)
    {
        day_sum += every_month[i];
        if ((tempTtotalday > day_sum) && (tempTtotalday <= (day_sum + every_month[i + 1])))
        {
            *month = i+2;
            *day = tempTtotalday - day_sum;
            break;
        }
        else if (day_sum == tempTtotalday)
        {
            *month = i + 1;
            *day = every_month[i];
            break;
        }
        else if (tempTtotalday < day_sum)
        {
            *month = 1;
            *day = tempTtotalday;
            break;
        }
    }
    return retYear;
}
/**
 *@brief  根据当前日期计算整个周期各个阶段的开始时间、结束时间
 *@details
 *@author        xjc
 *@date        2022-05-01
 */
bool uteModuleMenstrualCycleCalEachPhaseDate(uint16_t year, uint8_t month, uint8_t day, ute_menstrual_cycle_each_phase_date_t *eachPhaseDate,int8_t *dayLeft)
{
    bool isDisPlayLeft = false;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,date = %d-%d-%d", __func__,year, month, day);
    uteModulePlatformTakeMutex(uteModuleMenstrualcycleMute);
    int diffDays,judgeDays;
    int menstrualYear, menstrualMonth, menstrualDay;
    menstrualYear = uteModuleMenstrualCycleData.lastTimestamp[0] << 8 | uteModuleMenstrualCycleData.lastTimestamp[1];
    menstrualMonth = uteModuleMenstrualCycleData.lastTimestamp[2];
    menstrualDay = uteModuleMenstrualCycleData.lastTimestamp[3];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,last time = %04d-%02d-%02d", __func__,uteModuleMenstrualCycleData.lastTimestamp[0]<<8|uteModuleMenstrualCycleData.lastTimestamp[1],uteModuleMenstrualCycleData.lastTimestamp[2],uteModuleMenstrualCycleData.lastTimestamp[3]);
    diffDays = uteModuleMenstrualCycleGetDiffDay(menstrualYear, menstrualMonth, menstrualDay, year, month, day);
    if (diffDays >= 0)
    {
        judgeDays = diffDays % uteModuleMenstrualCycleData.cycleDays;
    }
    else
    {
        judgeDays = uteModuleMenstrualCycleData.cycleDays - (abs(diffDays) % uteModuleMenstrualCycleData.cycleDays);
    }
    /*!计算每个周期时间区间*/
    {
        uint16_t yearTotalDay = 0;
        uint8_t every_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
        uint16_t beginningYear;
        int16_t beginningDay; //整个经期在当前年份的开始时间
        uint16_t tempYear = 0;
        uint8_t tempMonth = 0,tempDay = 0;
        /*!计算输入日期在该年的天数*/
        uint16_t curdays = uteModuleMenstrualCycledaysCaculate(year, month, day);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,curdays = %d,judgeDays = %d", __func__,curdays,judgeDays);
        if((curdays-judgeDays) < 0)
        {
            /*!月经周期开始时间在上一年*/
            beginningYear = year-1;
            every_month[1] = (isLeapYear(beginningYear)) ? 29 : 28;
            for(uint8_t i=0; i<12; i++)
            {
                yearTotalDay += every_month[i];
            }
            beginningDay = yearTotalDay-(judgeDays-curdays);
        }
        else
        {
            beginningYear = year;
            beginningDay = curdays-judgeDays;
        }
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,beginningYear = %d,beginningDay = %d", __func__,beginningYear,beginningDay);
        //wholeCycle start
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay,beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->wholeCycle.startYear = tempYear;
        eachPhaseDate->wholeCycle.startMonth = tempMonth;
        eachPhaseDate->wholeCycle.startDay = tempDay;
        //menstruationCycle start
        eachPhaseDate->menstruationCycle.startYear = eachPhaseDate->wholeCycle.startYear;
        eachPhaseDate->menstruationCycle.startMonth = eachPhaseDate->wholeCycle.startMonth;
        eachPhaseDate->menstruationCycle.startDay = eachPhaseDate->wholeCycle.startDay;
        //menstruationCycle end
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay+uteModuleMenstrualCycleData.keepDays-1,beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->menstruationCycle.endYear = tempYear;
        eachPhaseDate->menstruationCycle.endMonth = tempMonth;
        eachPhaseDate->menstruationCycle.endDay = tempDay;
        //safety1Cycle start
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay+uteModuleMenstrualCycleData.keepDays,beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->safety1Cycle.startYear = tempYear;
        eachPhaseDate->safety1Cycle.startMonth = tempMonth;
        eachPhaseDate->safety1Cycle.startDay = tempDay;
        //safety1Cycle end
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay+(uteModuleMenstrualCycleData.cycleDays-19-1),beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->safety1Cycle.endYear = tempYear;
        eachPhaseDate->safety1Cycle.endMonth = tempMonth;
        eachPhaseDate->safety1Cycle.endDay = tempDay;
        //ovulatoryCycle start
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay+(uteModuleMenstrualCycleData.cycleDays-19),beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->ovulatoryCycle.startYear = tempYear;
        eachPhaseDate->ovulatoryCycle.startMonth = tempMonth;
        eachPhaseDate->ovulatoryCycle.startDay = tempDay;
        //ovulatoryCycle end
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay+(uteModuleMenstrualCycleData.cycleDays-10),beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->ovulatoryCycle.endYear = tempYear;
        eachPhaseDate->ovulatoryCycle.endMonth = tempMonth;
        eachPhaseDate->ovulatoryCycle.endDay = tempDay;
        //ovulatoryCycle day
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay+(uteModuleMenstrualCycleData.cycleDays-14),beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->ovulatoryDay.startYear = tempYear;
        eachPhaseDate->ovulatoryDay.startMonth = tempMonth;
        eachPhaseDate->ovulatoryDay.startDay = tempDay;
        //safety2Cycle start
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay+(uteModuleMenstrualCycleData.cycleDays-10)+1,beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->safety2Cycle.startYear = tempYear;
        eachPhaseDate->safety2Cycle.startMonth = tempMonth;
        eachPhaseDate->safety2Cycle.startDay = tempDay;
        //safety2Cycle end
        tempYear = uteModuleMenstrualCycleCalDateFromDays(beginningDay+uteModuleMenstrualCycleData.cycleDays-1,beginningYear,&tempMonth, &tempDay);
        eachPhaseDate->safety2Cycle.endYear = tempYear;
        eachPhaseDate->safety2Cycle.endMonth = tempMonth;
        eachPhaseDate->safety2Cycle.endDay = tempDay;
        //wholeCycle end
        eachPhaseDate->wholeCycle.endYear = eachPhaseDate->safety2Cycle.endYear;
        eachPhaseDate->wholeCycle.endMonth =eachPhaseDate->safety2Cycle.endMonth;
        eachPhaseDate->wholeCycle.endDay = eachPhaseDate->safety2Cycle.endDay;
    }

    /*!判断是否为当前日期的生理周期*/
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uint32_t currentTime=0,curDisplayTime=0;
    uint8_t currentTimeStr[10], curDisplayTimeStr[10];
    uint8_t mensStartTimeStr[10], mensEndTimeStr[10], ovulStartTimeStr[10], ovulEndTimeStr[10];
    uint32_t mensStartTime = 0, mensEndTime = 0, ovulStartTime = 0, ovulEndtime = 0;
    sprintf((char *)&currentTimeStr[0], "%04d%02d%02d", time.year, time.month, time.day);
    currentTime = atoi((const char *)&currentTimeStr[0]);

    sprintf((char *)&curDisplayTimeStr[0], "%04d%02d%02d", year, month, day);
    curDisplayTime = atoi((const char *)&curDisplayTimeStr[0]);

    sprintf((char *)&mensStartTimeStr[0], "%04d%02d%02d", eachPhaseDate->menstruationCycle.startYear, eachPhaseDate->menstruationCycle.startMonth, eachPhaseDate->menstruationCycle.startDay);
    sprintf((char *)&mensEndTimeStr[0], "%04d%02d%02d", eachPhaseDate->menstruationCycle.endYear, eachPhaseDate->menstruationCycle.endMonth, eachPhaseDate->menstruationCycle.endDay);
    sprintf((char *)&ovulStartTimeStr[0], "%04d%02d%02d", eachPhaseDate->ovulatoryCycle.startYear, eachPhaseDate->ovulatoryCycle.startMonth, eachPhaseDate->ovulatoryCycle.startDay);
    sprintf((char *)&ovulEndTimeStr[0], "%04d%02d%02d", eachPhaseDate->ovulatoryCycle.endYear, eachPhaseDate->ovulatoryCycle.endMonth, eachPhaseDate->ovulatoryCycle.endDay);
    mensStartTime = atoi((const char *)&mensStartTimeStr[0]);
    mensEndTime = atoi((const char *)&mensEndTimeStr[0]);
    ovulStartTime = atoi((const char *)&ovulStartTimeStr[0]);
    ovulEndtime = atoi((const char *)&ovulEndTimeStr[0]);
    if ((currentTime >= mensStartTime) && (currentTime <= mensEndTime))
    {
        *dayLeft = uteModuleMenstrualCycleGetDiffDay(eachPhaseDate->menstruationCycle.startYear, eachPhaseDate->menstruationCycle.startMonth, eachPhaseDate->menstruationCycle.startDay, time.year, time.month, time.day);
        *dayLeft += 1;
        if ((curDisplayTime >= mensStartTime) && (curDisplayTime <= mensEndTime))
        {
            isDisPlayLeft = true;
        }
    }
    else if ((currentTime >= ovulStartTime) && (currentTime <= ovulEndtime))
    {
        *dayLeft = uteModuleMenstrualCycleGetDiffDay(eachPhaseDate->ovulatoryCycle.startYear, eachPhaseDate->ovulatoryCycle.startMonth, eachPhaseDate->ovulatoryCycle.startDay, time.year, time.month, time.day);
        *dayLeft += 1;
        if ((curDisplayTime >= ovulStartTime) && (curDisplayTime <= ovulEndtime))
        {
            isDisPlayLeft = true;
        }
    }
    else
    {
        *dayLeft = 0;
        isDisPlayLeft = false;
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,dayLeft = %d", __func__, *dayLeft);


    uteModulePlatformGiveMutex(uteModuleMenstrualcycleMute);
    return isDisPlayLeft;
}
#endif
