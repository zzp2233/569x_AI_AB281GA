/**
*@file
*@brief        系统时间模块
*@details  系统时间，闹钟接口，UTC时间等接口
*@author       zn.zeng
*@date       Jun 29, 2021
*@version      v1.0
*/

#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_drv_battery_common.h"
// #include "ute_drv_keys_common.h"
#include "ute_module_message.h"
#include "ute_project_config.h"
#include "ute_module_gui_common.h"
#include "ute_module_sleep.h"
#include "ute_module_sport.h"
#include "ute_module_heart.h"
#include "ute_drv_motor.h"
#include "ute_language_common.h"
#include "ute_module_protocol.h"
#include "ute_module_localRingtone.h"
#include "ute_module_filesystem.h"
#include "ute_module_music.h"
#include "ute_module_profile_ble.h"
#include "ute_module_call.h"
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT && UTE_MODULE_SCREENS_MENSTRUAL_NOTIFY_SUPPORT
#include "ute_module_menstrualcycle.h"
#endif
#include "func_cover.h"
#include "ute_module_charencode.h"

/*! 系统时间参数 */
ute_module_systemtime_time_t systemTime;
/*! 系统闹钟参数 */
static ute_module_systemtime_alarm_t systemAlarms;
/*! 注册每秒回调函数数据结构zn.zeng, 2021-07-12  */
ute_module_systemtime_register_t systemTimeRegisterData;
static uint8_t uteModuleSystemTimeLocalTimeStatus;
/*! 平年每月天数 */
const static uint8_t everyMonDays[13] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/*! 闰年每月天数 */
const static uint8_t leapEveryMonDays[13] = {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
/* 时间互斥量 zn.zeng 2022-02-14*/
void *uteModuleSystemtimeMute;

/**
*@brief  系统时间初始化
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*/

void uteModuleSystemtimeInit(void)
{
    memset(&systemAlarms, 0, sizeof(ute_module_systemtime_alarm_t));
    systemAlarms.isRemindingIndex = 0xff;

    uteModulePlatformCreateMutex(&uteModuleSystemtimeMute);
    uteModulePlatformRtcInit();
    uteModulePlatformRtcStart();
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR);
#if APP_MODULE_RECEIVE_SAVE_TO_FILESYS_SUPPORT
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_PROTOCAL_RECEIVE_DIR);
#endif
#if APP_CUSTOM_REMIND_SETTING_SUPPORT
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_CUSTOM_REMIND_DIR);
    uteModuleSystemtimeReadReminderConfig();
    //uteModuleSystemtimeCustomRemindInitData();
#endif
    uteModuleSystemtimeReadConfig();
    memset(&systemTimeRegisterData,0,sizeof(ute_module_systemtime_register_t));
    uteLanguageCommonSelect(systemTime.languageId);
}

/**
*@brief  读取闹钟重复参数
*@details
*@author        dengli.lu
*@date        2022-12-13
*/
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
void uteModuleSystemtimeReadRepeatAlarmConfig(void)
{
    void *file=NULL;
    uint8_t readbuff[4];
    memset(&readbuff[0], 0, 4);
    uint8_t path[40];
    memset(&path[0], 0, 40);
    for (uint8_t i = 0; i < systemAlarms.alarmTotalCnt; i++)
    {
        sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_ALARM_REPEAT_DIR, i);
        memset(&systemAlarms.alarmParam[i].alarmRepeat, 0, sizeof(ute_module_systemtime_one_alarm_repeat_t));
        readbuff[0] = ALARM_REPEAT_REMIND_DEFAULT_OPEN;
        readbuff[1] = 0xff;
        readbuff[2] = 0xff;
        readbuff[3] = ALARM_REPEAT_REMIND_DEFAULT_TIMES;
        if(uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &readbuff[0], 4);
            uteModuleFilesystemCloseFile(file);
        }
        systemAlarms.alarmParam[i].alarmRepeat.isRepeatRemindOpen = readbuff[0];
        systemAlarms.alarmParam[i].alarmRepeat.repeatRemindHour = readbuff[1];
        systemAlarms.alarmParam[i].alarmRepeat.repeatRemindMin = readbuff[2];
        systemAlarms.alarmParam[i].alarmRepeat.repeatRemindTimes = readbuff[3];
    }
}
/**
*@brief  写入闹钟重复参数
*@details
*@author        dengli.lu
*@date        2022-12-13
*/
void uteModuleSystemtimeSaveRepeatAlarmConfig(ute_module_systemtime_one_alarm_repeat_t value,uint8_t index)
{
    /*! 保存到文件zn.zeng, 2021-08-21*/
    void *file;
    uint8_t writebuff[4];
    memset(&writebuff[0], 0, 4);
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    memcpy(&systemAlarms.alarmParam[index].alarmRepeat,&value,sizeof(ute_module_systemtime_one_alarm_repeat_t));
    writebuff[0] = systemAlarms.alarmParam[index].alarmRepeat.isRepeatRemindOpen;
    writebuff[1] = systemAlarms.alarmParam[index].alarmRepeat.repeatRemindHour;
    writebuff[2] = systemAlarms.alarmParam[index].alarmRepeat.repeatRemindMin;
    writebuff[3] = systemAlarms.alarmParam[index].alarmRepeat.repeatRemindTimes;
    uint8_t path[40];
    memset(&path[0], 0, 40);
    sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_ALARM_REPEAT_DIR, index);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file, &writebuff[0], 4);
        uteModuleFilesystemCloseFile(file);
    }
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_ALARM_REPEAT_DIR, dirInfo, NULL);
    uteModulePlatformMemoryFree(dirInfo);
}
#endif

/**
*@brief  读取闹钟参数
*@details
*@author        黄鹤
*@date        2022-04-03
*/
void uteModuleSystemtimeReadAlarmConfig(void)
{
    void *file=NULL;
    uint8_t readbuff[UTE_MODULE_SYSTEMTIME_ALARM_SIZE];
    memset(&readbuff[0], 0, UTE_MODULE_SYSTEMTIME_ALARM_SIZE);
    uint8_t path[20];

    memset(&path[0], 0, 20);
    /*! 闹钟参数读取zn.zeng, 2021-08-21  */
    systemAlarms.alarmTotalCnt = 0;
    for (uint8_t i = 0; i < SYSTEM_TIME_ALARMS_MAX_CNT; i ++)
    {
        sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_ALARMINFO_DIR, i);
        memset(&systemAlarms.alarmParam[i], 0, sizeof(ute_module_systemtime_one_alarm_t));
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
        systemAlarms.alarmParam[i].alarmRepeat.repeatRemindHour = 0xff;
        systemAlarms.alarmParam[i].alarmRepeat.repeatRemindMin = 0xff;
#endif
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
            uteModuleFilesystemReadData(file, &readbuff[0], UTE_MODULE_SYSTEMTIME_ALARM_SIZE);
            uteModuleFilesystemCloseFile(file);
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].year = (readbuff[0] << 8) | readbuff[1];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].month = readbuff[2];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].day = readbuff[3];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].weekDay = readbuff[4];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].hour = readbuff[5];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].min = readbuff[6];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].sec = readbuff[7];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].isSingle = readbuff[8];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].durationTimeSec = readbuff[9];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].isOpen = readbuff[10];
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,i=%d.weekDay = 0x%02x,.hour = %d,.min = %d,.isOpen=%d", __func__,i,systemAlarms.alarmParam[i].weekDay,systemAlarms.alarmParam[i].hour,systemAlarms.alarmParam[i].min,systemAlarms.alarmParam[i].isOpen);
#if UTE_MODULE_SCREENS_LOCAL_ALARM_LABEL_SUPPORT
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].setYear = (readbuff[11] << 8) | readbuff[12];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].setMonth = readbuff[13];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].setDay = readbuff[14];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].setHour = readbuff[15];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].setMin = readbuff[16];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].setSec = readbuff[17];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].index = readbuff[18];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].isFinish = true;
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].labelLenth = readbuff[19];
            systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].alarmLabel = (readbuff[20] << 8) | readbuff[21];
            memcpy(&systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].label[0], &readbuff[22], systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].labelLenth);
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,i=%d,.alarmLabel=%d", __func__,i,systemAlarms.alarmParam[systemAlarms.alarmTotalCnt].alarmLabel);
#endif
            systemAlarms.alarmTotalCnt ++;
        }
    }
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
    uteModuleSystemtimeReadRepeatAlarmConfig();
#endif
}

/**
*@brief  读取时间参数
*@details
*@author        zn.zeng
*@date        2021-08-26
*/
void uteModuleSystemtimeReadConfig(void)
{
    /*! 闹钟参数读取zn.zeng, 2021-08-21  */
    void *file=NULL;
    uint8_t readbuff[15];

    memset(&readbuff[0], 0, 15);
    uteModuleSystemtimeReadAlarmConfig();
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,readAlarmTotalCnt = %d", __func__,systemAlarms.alarmTotalCnt);
    /*! 时间格式，当设置语言 zn.zeng, 2021-08-18  */
    readbuff[0] = DEFAULT_SYSTEM_TIME_FORMAT_MI;
    readbuff[1] = DEFAULT_SYSTEM_TIME_FORMAT_12HOUR;
    readbuff[2] = DEFAULT_SYSTEM_TIME_ZONE;
    readbuff[3] = DEFAULT_LANGUAGE>>8&0xff;
    readbuff[4] = DEFAULT_LANGUAGE&0xff;
    readbuff[5] = (DEFAULT_SYSTEM_TIME_YEAR>>8)&0xff;
    readbuff[6] = DEFAULT_SYSTEM_TIME_YEAR&0xff;
    readbuff[7] = DEFAULT_SYSTEM_TIME_MONTH;
    readbuff[8] = DEFAULT_SYSTEM_TIME_DAY;
    readbuff[9] = DEFAULT_SYSTEM_TIME_HOUR;
    readbuff[10] = DEFAULT_SYSTEM_TIME_MIN;
    readbuff[11] = DEFAULT_SYSTEM_TIME_SEC;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_FORMAT,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],14);
        uteModuleFilesystemCloseFile(file);
    }
    ute_module_systemtime_time_t set;
    memset(&set,0,sizeof(ute_module_systemtime_time_t));
    set.isDistanceMi = readbuff[0];
    set.is12HourTime = readbuff[1];
    set.zone = readbuff[2];
    set.languageId = readbuff[3]<<8|readbuff[4];

    tm_t rtc_tm;
    rtc_tm.year = readbuff[5] << 8 | readbuff[6];
    rtc_tm.mon = readbuff[7];
    rtc_tm.day = readbuff[8];
    rtc_tm.hour = readbuff[9];
    rtc_tm.min = readbuff[10];
    rtc_tm.sec = readbuff[11];

    if (tm_to_time(rtc_tm) < RTCCNT)
    {
        rtc_tm = time_to_tm(RTCCNT);
    }

    set.year = rtc_tm.year;
    set.month = rtc_tm.mon;
    set.day = rtc_tm.day;
    set.hour = rtc_tm.hour;
    set.min = rtc_tm.min;
    set.sec = rtc_tm.sec;

    set.isWatchSetLangage = readbuff[12];
    set.AppSetlanguageId = readbuff[13];
    uteModuleSystemtimeSetTime(set);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isDistanceMi=%d,is12HourTime=%d,time.zone=%d,languageId=%d", __func__,systemTime.isDistanceMi,systemTime.is12HourTime,systemTime.zone,systemTime.languageId);
}
/**
*@brief  系统时间设置时间
*@details
*@param[in] ute_module_systemtime_time_t set  传入要设置的时间参数
*@author        zn.zeng
*@date        Jun 29, 2021
*/

void uteModuleSystemtimeSetTime(ute_module_systemtime_time_t set)
{
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,%04d-%02d-%02d,%02d:%02d:%02d,zone=%d", __func__, set.year, set.month, set.day, set.hour, set.min, set.sec,set.zone);
    if (set.year < 2000)
    {
        set.year = 2000;
    }
    if (set.month == 0)
    {
        set.month = 1;
    }
    if (set.day == 0)
    {
        set.day = 1;
    }

    ute_module_systemtime_time_t oldTime;
    memcpy(&oldTime,&systemTime,sizeof(ute_module_systemtime_time_t));
    uteModuleSleepSystemtimeChange(systemTime,set);
#if UTE_MODULE_CYWEE_MOTION_SUPPORT
    //sportSystemTimeChange 中有uteModuleCwmReadCurrDayStepFromFs 操作,所以这个放在它执行之前
    uteModuleCwmStepDataSystemtimeChange(systemTime,set);
#endif
    uteModuleSportSystemtimeChange(systemTime,set);
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(&systemTime, &set, sizeof(ute_module_systemtime_time_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
    systemTime.isSettingTime = true;
    systemTime.week = uteModuleSystemtimeGetWeek(systemTime.year, systemTime.month, systemTime.day);
    uteModulePlatformRtcSetTime(systemTime.year,systemTime.month,systemTime.day,systemTime.hour,systemTime.min,systemTime.sec);
    uteModuleSystemtimeSaveTimeInfo();
//    uteModulePlaformUpdateConnectParam(12,36,30*1000);
    uteModuleHeartSystemtimeChange(oldTime,set);
}
/**
*@brief  设置闹钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        Jun 29, 2021
*/

void uteModuleSystemtimeSetAlarm(ute_module_systemtime_one_alarm_t set, uint8_t index)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
    set.setYear=systemTime.year;
    set.setMonth=systemTime.month;
    set.setDay=systemTime.day;
    set.setHour=systemTime.hour;
    set.setMin=systemTime.min;
    set.setSec=systemTime.sec;
#endif
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
    set.isFinish = true;
#endif
    memcpy(&systemAlarms.alarmParam[index], &set, sizeof(ute_module_systemtime_one_alarm_t));
    uteModuleSystemtimeSaveAlarmInfo(systemAlarms.alarmParam[index],index);
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}
/**
*@brief  获取闹钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/
void uteModuleSystemtimeGetAlarm(ute_module_systemtime_one_alarm_t *set, uint8_t index)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(set,&systemAlarms.alarmParam[index], sizeof(ute_module_systemtime_one_alarm_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}
/**
*@brief  获取所有闹钟
*@details
*@param[in] ute_module_systemtime_alarm_t *data  传入要获取闹钟参数的指针
*@author        wuhuowang
*@date        2023-01-17
*/
void uteModuleSystemtimeGetAllAlarm(ute_module_systemtime_alarm_t *data)
{
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(data, &systemAlarms, sizeof(ute_module_systemtime_alarm_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}
/**
*@brief  删除闹钟
*@details
*@param[in] uint8_t index 所需要删除的闹钟序号，从0开始
*@author       dengli.lu
*@date        2021-11-29
*/
void uteModuleSystemtimeDeleteAlarm(uint8_t index)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    uint8_t beforeTotalCnt = systemAlarms.alarmTotalCnt;
    uint8_t path[20];
    memset(&path[0],0,20);
    sprintf((char *)&path[0],"%s/%02d",UTE_MODULE_FILESYSTEM_ALARMINFO_DIR,index);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index = %d,beforeTotalCnt=%d", __func__,index,beforeTotalCnt);
    memset(&systemAlarms.alarmParam[index],0,sizeof(ute_module_systemtime_one_alarm_t));
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
    systemAlarms.alarmParam[index].isFinish = false;
#endif
    uteModuleFilesystemDelFile((char *)&path[0]);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR, dirInfo, NULL);
    if((index+1)<beforeTotalCnt)//删除最后一个，不需要重新命名
    {
        uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
        for(uint8_t i = index+1; i <beforeTotalCnt ; i++)
        {
            uint8_t oldPath[20],newPath[20];
            memset(&oldPath[0],0,20);
            memset(&newPath[0],0,20);
            sprintf((char *)&oldPath[0],"%s/%02d",UTE_MODULE_FILESYSTEM_ALARMINFO_DIR,i);
            sprintf((char *)&newPath[0],"%s/%02d",UTE_MODULE_FILESYSTEM_ALARMINFO_DIR,i-1);
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,oldPath=%s,newPath=%s", __func__,oldPath,newPath);
            uteModuleFilesystemRenameFile((char *)oldPath,(char *)newPath);
            memcpy(&systemAlarms.alarmParam[i-1],&systemAlarms.alarmParam[i],sizeof(ute_module_systemtime_one_alarm_t));
        }
        systemAlarms.alarmParam[beforeTotalCnt-1].weekDay = 0;
        uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
    }
    systemAlarms.alarmTotalCnt = dirInfo->filesCnt;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,alarmTotalCnt = %d", __func__,systemAlarms.alarmTotalCnt);
#if (UTE_LOG_TIME_LVL&&UTE_MODULE_LOG_SUPPORT)
    for(uint8_t i = 0; i <systemAlarms.alarmTotalCnt ; i++)
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,hour=%d,min = %d", __func__,systemAlarms.alarmParam[i].hour,systemAlarms.alarmParam[i].min);
    }
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR, dirInfo, NULL);
#endif
    uteModulePlatformMemoryFree(dirInfo);
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
    if (systemAlarms.alarmTotalCnt == 0)
    {
        uteModuleSystemtimeSaveAlarmDelTime();
    }
#endif
}/**
*@brief  删除所有闹钟
*@details
*@param[in]
*@author       wuhuowang
*@date        2023-03-29
*/
void uteModuleSystemtimeDeleteAllAlarm(void)
{
    uint8_t i = 0;
    uint8_t path[40];
    uteModuleFilesystemDelDirectoryAllFiles(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR);
    for (i = 0; i < SYSTEM_TIME_ALARMS_MAX_CNT; i ++)
    {
        memset(&systemAlarms.alarmParam[i], 0, sizeof(ute_module_systemtime_one_alarm_t));
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
        systemAlarms.alarmParam[i].alarmRepeat.repeatRemindHour = 0xff;
        systemAlarms.alarmParam[i].alarmRepeat.repeatRemindMin = 0xff;
#endif
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
        systemAlarms.alarmParam[i].isFinish = false;
#endif
    }
    systemAlarms.alarmTotalCnt = 0;
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
    uteModuleSystemtimeSaveAlarmDelTime();
#endif
}
/**
*@brief  设置闹钟显示索引
*@details
*@param[in] uint8_t index 所需要设置的闹钟索引1~8
*@author       dengli.lu
*@date        2021-11-29
*/
void uteModuleSystemtimeSetAlarmDisplayIndex(uint8_t alarmDisplayIndex)
{
    systemAlarms.alarmDisplayIndex = alarmDisplayIndex;
}
/**
*@brief  获取闹钟显示索引
*@details
*@return  闹钟索引1~8
*@author       dengli.lu
*@date        2021-11-29
*/
uint8_t uteModuleSystemtimeGetAlarmDisplayIndex(void)
{
    return systemAlarms.alarmDisplayIndex;
}
/**
*@brief  设置当前闹钟数量
*@details
*@param[in] uint8_t index 所需要设置的闹钟数量1~8
*@author       dengli.lu
*@date        2021-11-29
*/
void uteModuleSystemtimeSetAlarmTotalCnt(uint8_t alarmTotalCnt)
{
    systemAlarms.alarmTotalCnt = alarmTotalCnt;
}
/**
*@brief  获取闹钟数量
*@details
*@return  闹钟索引1~8
*@author       dengli.lu
*@date        2021-11-29
*/
uint8_t uteModuleSystemtimeGetAlarmTotalCnt(void)
{
    return systemAlarms.alarmTotalCnt;
}

/**
*@brief  判断是否是闰年
*@details
*@param[in] uint16_t year  传入年份
*@author        zn.zeng
*@return  返回true，此年份是闰年
*@date        Jun 29, 2021
*/
static bool uteModuleSystemtimeIsLeapYear(uint16_t year)
{
    return (((year % 100 != 0) && (year % 4 == 0)) || (year % 400 == 0));
}
/**
*@brief  系统时间秒计时
*@details   秒加一
*@param[in] ute_module_systemtime_time_t *time  时间变量指针
*@author        zn.zeng
*@date        Jun 29, 2021
*/
static void uteModuleSystemtimeChange(ute_module_systemtime_time_t *time)
{
#if UTE_MODULE_CREATE_SYS_1S_TIMER_SUPPORT
    time->sec++;
    if (time->sec > 59)
    {
        time->sec = 0;
        time->min++;
        if (time->min > 59)
        {
            time->min = 0;
            time->hour++;
            if (time->hour > 23)
            {
                const uint8_t *monday;
                time->hour = 0;
                time->day++;
                if (uteModuleSystemtimeIsLeapYear(time->year))
                {
                    monday = &leapEveryMonDays[0];
                }
                else
                {
                    monday = &everyMonDays[0];
                }
                if (time->day > monday[time->month])
                {
                    time->day = 1;
                    time->month++;
                    if (time->month > 12)
                    {
                        time->month = 1;
                        time->year = time->year + 1;
                    }
                }
            }
        }
    }
#else
    bool lowpwr_sta = bsp_system_is_sleep();
    static u8 rtc_cal_cnt_bkp = 0;
    static bool lowpwr_sta_bkp = false;
    uint32_t rtc_cnt = 0;
    tm_t rtc_tm;
    bool allow_calibration = (time->sec % 10 != 0 && time->sec < 55 && time->sec > 5); //避免校准时间跳过关键时间点
    rtc_tm.year = time->year;
    rtc_tm.mon = time->month;
    rtc_tm.day = time->day;
    rtc_tm.hour = time->hour;
    rtc_tm.min = time->min;
    rtc_tm.sec = time->sec;
    rtc_cnt = tm_to_time(rtc_tm);
    if (!lowpwr_sta && allow_calibration)
    {
        rtc_cnt = RTCCNT;
    }
    else // 省电/休眠模式，RTC已休眠
    {
        if (lowpwr_sta_bkp == false) // 初次进入
        {
            rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
        }
        if (rtc_cal_cnt_bkp != sys_cb.rtc_cal_cnt && allow_calibration) // RTC已校准，同步校准
        {
            rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
            rtc_cnt = RTCCNT;
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,RTC calibrated,rtc_cal_cnt_bkp:%d,sys_cb.rtc_cal_cnt:%d", __func__, rtc_cal_cnt_bkp, sys_cb.rtc_cal_cnt);
        }
        else
        {
            rtc_cnt++;
        }
    }
    rtc_tm = time_to_tm(rtc_cnt);
    time->year = rtc_tm.year;
    time->month = rtc_tm.mon;
    time->day = rtc_tm.day;
    time->hour = rtc_tm.hour;
    time->min = rtc_tm.min;
    time->sec = rtc_tm.sec;

    // if (lowpwr_sta && allow_calibration && uteApplicationCommonIsAppClosed())
    // {
    //     uint32_t oneDaySec = time->hour * 3600 + time->min * 60 + time->sec;
    //     if (oneDaySec % 325 == 0)
    //     {
    //         rtc_clock_calc_by_rc(0);
    //         printf("[UTE] RTC calibration by RC\n");
    //     }
    // }
#endif
}

/**
*@brief
*@details  闹钟回调函数
*@param[in] 响应的闹钟持续时长
*@author        zn.zeng
*@date        Jun 29, 2021
*/
#if UTE_MODULE_LOCAL_ALARM_REMIND_USER_HANDLE_SUPPORT
void uteModuleSystemtimeAlarmCb(uint8_t duration,uint8_t index)
{
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    if(uteModuleGuiCommonGetCurrentScreenId()==UTE_MOUDLE_SCREENS_OTA_ID)
    {
        return;
    }
#if UTE_BT30_CALL_SUPPORT
    uint callStatus = bsp_bt_disp_status();
    if (uteModuleGuiCommonGetCurrentScreenId() == FUNC_BT_CALL || callStatus > BT_STA_PLAYING)
    {
        return;
    }
#endif
    systemAlarms.isRemindingIndex = index;
    // uteTaskGuiStartScreen(UTE_MOUDLE_SCREENS_ALARM_CLOCK_ID);
    sys_cb.cover_index = REMIND_COVER_ALARM;
    sys_cb.remind_tag = true;

#if UTE_CUSTOM_ALARM_MOTOR_DURATION_TIME//客户定制通闹钟规则
    uteDrvMotorStart(UTE_CUSTOM_ALARM_MOTOR_DURATION_TIME,UTE_CUSTOM_ALARM_MOTOR_INTERVAL_TIME,UTE_CUSTOM_ALARM_MOTOR_DURATION_TIMES);
#else
    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,duration);
#endif
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,duration=%02d", __func__, duration);
#if UTE_MODULE_LOCAL_RINGTONE_ALARM_SUPPORT
    if(!uteModuleLocalRingtoneGetMuteStatus())
    {
        uteModuleLocalRingtonePlayRing(RINGTON_TYPE_ALARM);
    }
#endif
}
#endif

#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
/**
*@brief  添加闹钟下次提醒时间信息
*@param[in] uint8_t i
*@param[in] bool isCurrAlarm 如果是当前闹钟，就设置三次重复提醒。如果是重复的闹钟就减少一次提醒次数，提醒次数为0时清除信息
*@author    Casen
*@date      2022-01-22
*/
void uteModuleSystemtimeAlarmAddRepeatRemindInfo(uint8_t i,bool isCurrAlarm)
{
    uint8_t hour = (isCurrAlarm)?systemAlarms.alarmParam[i].hour:systemAlarms.alarmParam[i].alarmRepeat.repeatRemindHour;
    uint8_t min = (isCurrAlarm)?systemAlarms.alarmParam[i].min:systemAlarms.alarmParam[i].alarmRepeat.repeatRemindMin;
    if(min+ALARM_REPEAT_REMIND_DEFAULT_TIME_MIN>=60)
    {
        min = min+ALARM_REPEAT_REMIND_DEFAULT_TIME_MIN-60;
        if(hour == 23)
        {
            hour = 0;
        }
        else
        {
            hour = hour + 1;
        }
    }
    else
    {
        min = min + ALARM_REPEAT_REMIND_DEFAULT_TIME_MIN;
    }
    if(isCurrAlarm == true)
    {
        systemAlarms.cancelCurrAlarmIndex = i+1; //表示第几个闹钟,使用uteAppAlarmRepeatReminderClear 时它会减一
        systemAlarms.alarmParam[i].alarmRepeat.repeatRemindTimes = ALARM_REPEAT_REMIND_DEFAULT_TIMES;
    }
    else
    {
        systemAlarms.cancelRepeatAlarmIndex = i+1; //表示第几个闹钟,使用uteAppAlarmRepeatReminderClear 时它会减一
        if(systemAlarms.alarmParam[i].alarmRepeat.repeatRemindTimes>0)
        {
            systemAlarms.alarmParam[i].alarmRepeat.repeatRemindTimes --;
        }
    }
    systemAlarms.alarmParam[i].alarmRepeat.repeatRemindHour = hour;
    systemAlarms.alarmParam[i].alarmRepeat.repeatRemindMin = min;
    uteModuleSystemtimeSaveRepeatAlarmConfig(systemAlarms.alarmParam[i].alarmRepeat,i);
}
/**
*@brief  清除闹钟重复提醒的信息
*@param[in] uint8_t currIndex
*@param[in] uint8_t repeatIndex
*@author    Casen
*@date      2022-01-22
*/
void uteModuleSystemtimeAlarmClearRepeatRemindInfo(uint8_t currIndex,uint8_t repeatIndex)
{
    if(currIndex > 0 && currIndex <= SYSTEM_TIME_ALARMS_MAX_CNT)
    {
        systemAlarms.alarmParam[currIndex - 1].alarmRepeat.repeatRemindTimes = 0;
        systemAlarms.alarmParam[currIndex - 1].alarmRepeat.repeatRemindHour = 0xff;
        systemAlarms.alarmParam[currIndex - 1].alarmRepeat.repeatRemindMin = 0xff;
        systemAlarms.cancelCurrAlarmIndex = 0;
    }
    if(repeatIndex>0 && repeatIndex<=SYSTEM_TIME_ALARMS_MAX_CNT)
    {
        systemAlarms.alarmParam[repeatIndex - 1].alarmRepeat.repeatRemindTimes = 0;
        systemAlarms.alarmParam[repeatIndex - 1].alarmRepeat.repeatRemindHour = 0xff;
        systemAlarms.alarmParam[repeatIndex - 1].alarmRepeat.repeatRemindMin = 0xff;
        systemAlarms.cancelRepeatAlarmIndex = 0;
    }
    if (systemAlarms.alarmParam[currIndex - 1].isSingle)
    {
        systemAlarms.alarmParam[currIndex - 1].isOpen = false;
    }
    uteModuleSystemtimeSetAlarm(systemAlarms.alarmParam[currIndex - 1],currIndex - 1);
    uteModuleSystemtimeSaveRepeatAlarmConfig(systemAlarms.alarmParam[currIndex - 1].alarmRepeat,currIndex - 1);
    uteModuleSystemtimeStartSendAllAlarmData();
}

/**
*@brief
*@details  获取取消当前闹钟重复提醒
*@author        dengli.lu
*@date        2022.04.24
*/
uint8_t uteModuleSystemtimeAlarmGetCancelCurrAlarmIndex(void)
{
    return systemAlarms.cancelCurrAlarmIndex;
}

/**
*@brief
*@details  设置取消当前闹钟重复提醒
*@author        dengli.lu
*@date        2022.04.24
*/
void uteModuleSystemtimeAlarmSetCancelCurrAlarmIndex(uint8_t cancelCurrAlarmIndex)
{
    systemAlarms.cancelCurrAlarmIndex = cancelCurrAlarmIndex;
}
/**
*@brief
*@details  获取取消重复闹钟信息提醒
*@author        dengli.lu
*@date        2022.04.24
*/
uint8_t uteModuleSystemtimeAlarmGetCancelRepeatAlarmIndex(void)
{
    return systemAlarms.cancelRepeatAlarmIndex;
}

/**
*@brief
*@details  设置取消重复闹钟信息提醒
*@author        dengli.lu
*@date        2022.04.24
*/
void uteModuleSystemtimeAlarmSetCancelRepeatAlarmIndex(uint8_t cancelRepeatAlarmIndex)
{
    systemAlarms.cancelRepeatAlarmIndex = cancelRepeatAlarmIndex;
}

/**
*@brief
*@details  获取闹钟剩余提醒次数
*@author        zhenhua.cai
*@date        2025.09.25
*/
uint8_t uteModuleSystemtimeAlarmGetRepeatRemindTimes(uint8_t index)
{
    return systemAlarms.alarmParam[index].alarmRepeat.repeatRemindTimes;
}
#endif
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
/*! 有效闹钟数,除去正在设置的闹钟 */
static uint8_t validAlarmCnt = 0;
/*! 已发送闹钟数,对数据进行分包处理*/
static uint8_t hasSendAlarmCnt = 0;
/**
*@brief       闹钟数据同步
*@details
*@param[in] uint8_t*receive
*@param[in] uint8_t length
*@author       huanghe
*@date       2022-03-24
*/
void uteModuleSystemtimeAlarmCmd(uint8_t *receive, uint8_t length)
{
    if (receive[1] == 0x00)
    {
#if APP_MODULE_RECEIVE_SAVE_TO_FILESYS_SUPPORT
        uteModuleSystemTimeSaveReceiveData(&receive[2], length - 2, 0x01, true);
#else
        memset(systemAlarms.receiveDataBuff, 0, SYSTEM_TIME_ALARMS_MAX_CNT * UTE_MODULE_SYSTEMTIME_ALARM_MAX_NUMBER);
        memcpy(&systemAlarms.receiveDataBuff[0], &receive[2], length - 2);
#endif
        systemAlarms.receiveDataOffset = length - 2;
        systemAlarms.receiveCrc = 0;
        for (uint8_t i = 0; i < length-1; i ++)
        {
            systemAlarms.receiveCrc ^= receive[i + 1];
        }
        uint8_t response[3];
        memset(response, 0, 3);
        response[0] = receive[0];
        response[1] = 0xFB;
        response[2] = 0x00;
        uteModuleProfileBleSendToPhone((uint8_t *)&response[0], 3);
    }
    else
    {
        if (receive[1] == 0xFD)
        {
            uint8_t response[3];
            memset(response, 0, 3);
            response[0] = receive[0];
            if ((systemAlarms.receiveCrc == receive[2]) && (systemAlarms.receiveDataOffset >= 15))
            {
                response[1] = receive[1];
                response[2] = systemAlarms.receiveCrc;
                UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,Crc success = %02x", __func__,systemAlarms.receiveCrc);
#if APP_MODULE_RECEIVE_SAVE_TO_FILESYS_SUPPORT
                {
                    uint8_t path[30];
                    uint8_t *readBuff = uteModulePlatformMemoryAlloc(SYSTEM_TIME_ALARMS_MAX_CNT * UTE_MODULE_SYSTEMTIME_ALARM_MAX_NUMBER);
                    void *file;
                    int32_t fileSize = 0;

                    memset(&path[0], 0, 30);
                    sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_PROTOCAL_RECEIVE_DIR, 0x01);
                    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
                    {
                        uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
                        fileSize = (uint32_t)uteModuleFilesystemGetFileSize(file);
                        UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,fileSize=%d %d", __func__, fileSize, systemAlarms.receiveDataOffset);
                        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
                        uteModuleFilesystemReadData(file, &readBuff[0], fileSize);
                        uteModuleFilesystemCloseFile(file);
                        if (systemAlarms.receiveDataOffset == fileSize)
                            uteModuleSystemtimeSaveAllAlarmInfo(readBuff, systemAlarms.receiveDataOffset);
                    }
                    uteModulePlatformMemoryFree(readBuff);
                }
#else
                uteModuleSystemtimeSaveAllAlarmInfo(systemAlarms.receiveDataBuff, systemAlarms.receiveDataOffset);
#endif
                uteModuleProfileBleSendToPhone((uint8_t *)&response[0], 3);
            }
            else
            {
                response[1] = 0xFF;
                UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,Crc fail = %02x", __func__,systemAlarms.receiveCrc);
                uteModuleProfileBleSendToPhone((uint8_t *)&response[0], 2);
            }
        }
        else if (receive[1] == 0xAA)
        {
            if (uteModuleSystemtimeGetAlarmTotalCnt() == 0)
            {
                if (uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_FINAL_ALARM_DELETE_TIME, NULL, NULL))
                {
                    uteModuleSystemtimeReadAlarmDelTime();
                }
                else
                {
                    uint8_t response[10];
                    memset(response, 0, 10);
                    response[0] = receive[0];
                    response[1] = receive[1];
                    response[2] = 0xff;
                    response[3] = 0x07;//1970/01/01/00:00:00
                    response[4] = 0xb2;
                    response[5] = 0x01;
                    response[6] = 0x01;
                    response[7] = 0x00;
                    response[8] = 0x00;
                    response[9] = 0x00;
                    uteModuleProfileBleSendToPhone((uint8_t *)&response[0], 10);
                }
            }
            else
            {
                uteModuleSystemtimeStartSendAllAlarmData();
            }
        }
        else if (receive[1] == 0xFC)
        {
            uint8_t response[2];
            memset(response, 0, 2);
            response[0] = receive[0];
            response[1] = receive[1];
            //删除所有闹钟
            uteModuleSystemtimeDeleteAllAlarm();
            uteModuleProfileBleSendToPhone((uint8_t *)&response[0], 2);
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,receiveIndex = %d", __func__,receive[1]);
#if APP_MODULE_RECEIVE_SAVE_TO_FILESYS_SUPPORT
            uteModuleSystemTimeSaveReceiveData(&receive[2], length - 2, 0x01, false);
#else
            memcpy(&systemAlarms.receiveDataBuff[systemAlarms.receiveDataOffset], &receive[2], length - 2);
#endif
            systemAlarms.receiveDataOffset += (length - 2);
            for(uint8_t i = 0; i < length - 1; i ++)
            {
                systemAlarms.receiveCrc ^= receive[i + 1];
                UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,receiveCrc = %02x,receive[%d]=%02x", __func__,systemAlarms.receiveCrc,i+1,receive[i+1]);
            }
            uint8_t response[3];
            memset(response, 0, 3);
            response[0] = receive[0];
            response[1] = 0xFB;
            response[2] = receive[1];
            uteModuleProfileBleSendToPhone((uint8_t *)&response[0], 3);
        }
    }
}

/**
*@brief  开始发送闹钟数据
*@details
*@param
*@author        黄鹤
*@date        2022-04-07
*/
void uteModuleSystemtimeStartSendOneAlarmData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t sendSize = 0;
    uint16_t sendMaxLenth = uteApplicationCommonGetMtuSize() - 3;   //每个包发送多少字节
    uint16_t responseBufferSize = 3 + sendMaxLenth;           //每个包发送的运动数据需要开多大的buffer
    uint8_t *response = uteModulePlatformMemoryAlloc(responseBufferSize);
    memset(response, 0, responseBufferSize);
    response[0] = CMD_ALARM_DATA;
    response[1] = 0xAA;//查询指令
    response[2] = sendParam->currSendMinIndex;//第几个包
    sendSize = 3;
    if (validAlarmCnt == 0)
    {
        // end
        response[2] = 0xFD;
        response[3] = sendParam->crc;
        sendSize = 4;
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, has send alarm cnt = %d", __func__,hasSendAlarmCnt);
    }
    else
    {
        uint8_t sendOffset = 0;
        for (uint8_t i = hasSendAlarmCnt; i < systemAlarms.alarmTotalCnt; i ++)
        {
            if (systemAlarms.alarmParam[i].isFinish)
            {
#if APP_MODULE_ALARM_LABEL_SET_SUPPORT
                if ((sendSize + systemAlarms.alarmParam[i].labelLenth + 16) <= sendMaxLenth)
                {
                    response[3 + sendOffset] = (systemAlarms.alarmParam[i].setYear >> 8) & 0xff;
                    response[4 + sendOffset] = systemAlarms.alarmParam[i].setYear & 0xff;
                    response[5 + sendOffset] = systemAlarms.alarmParam[i].setMonth;
                    response[6 + sendOffset] = systemAlarms.alarmParam[i].setDay;
                    response[7 + sendOffset] = systemAlarms.alarmParam[i].setHour;
                    response[8 + sendOffset] = systemAlarms.alarmParam[i].setMin;
                    response[9 + sendOffset] = systemAlarms.alarmParam[i].setSec;
                    response[10 + sendOffset] = i + 1;
                    response[11 + sendOffset] = systemAlarms.alarmParam[i].isOpen;
                    response[12 + sendOffset] = systemAlarms.alarmParam[i].hour;
                    response[13 + sendOffset] = systemAlarms.alarmParam[i].min;
                    response[14 + sendOffset] = systemAlarms.alarmParam[i].weekDay;
                    response[15 + sendOffset] = systemAlarms.alarmParam[i].durationTimeSec;
                    response[16 + sendOffset] = systemAlarms.alarmParam[i].jarLevel;
                    response[17 + sendOffset] = systemAlarms.alarmParam[i].alarmLabel & 0xff;
                    response[18 + sendOffset] = systemAlarms.alarmParam[i].labelLenth;
                    memcpy(&response[19 + sendOffset], &systemAlarms.alarmParam[i].label[0], systemAlarms.alarmParam[i].labelLenth);
                    sendOffset += (systemAlarms.alarmParam[i].labelLenth + 16);
                    sendSize += (systemAlarms.alarmParam[i].labelLenth + 16);
                    validAlarmCnt --;
                    hasSendAlarmCnt ++;
                }
#else
                if ((sendSize + systemAlarms.alarmParam[i].labelLenth + 15) <= sendMaxLenth)
                {
                    response[3 + sendOffset] = (systemAlarms.alarmParam[i].setYear >> 8) & 0xff;
                    response[4 + sendOffset] = systemAlarms.alarmParam[i].setYear & 0xff;
                    response[5 + sendOffset] = systemAlarms.alarmParam[i].setMonth;
                    response[6 + sendOffset] = systemAlarms.alarmParam[i].setDay;
                    response[7 + sendOffset] = systemAlarms.alarmParam[i].setHour;
                    response[8 + sendOffset] = systemAlarms.alarmParam[i].setMin;
                    response[9 + sendOffset] = systemAlarms.alarmParam[i].setSec;
                    response[10 + sendOffset] = i + 1;
                    response[11 + sendOffset] = systemAlarms.alarmParam[i].isOpen;
                    response[12 + sendOffset] = systemAlarms.alarmParam[i].hour;
                    response[13 + sendOffset] = systemAlarms.alarmParam[i].min;
                    response[14 + sendOffset] = systemAlarms.alarmParam[i].weekDay;
                    response[15 + sendOffset] = systemAlarms.alarmParam[i].durationTimeSec;
                    response[16 + sendOffset] = systemAlarms.alarmParam[i].jarLevel;
                    response[17 + sendOffset] = systemAlarms.alarmParam[i].labelLenth;
                    memcpy(&response[18 + sendOffset], &systemAlarms.alarmParam[i].label[0], systemAlarms.alarmParam[i].labelLenth);
                    sendOffset += (systemAlarms.alarmParam[i].labelLenth + 15);
                    sendSize += (systemAlarms.alarmParam[i].labelLenth + 15);
                    validAlarmCnt --;
                    hasSendAlarmCnt ++;
                }
#endif
            }
        }
    }
    if (response[2] == 0xFD)
    {
        uteApplicationCommonSyncDataTimerStop();
    }
    else
    {
        sendParam->currSendMinIndex ++;
        uteModuleCrc8Bit(&sendParam->crc, &response[2],sendSize - 2);
    }
    bool state = uteModuleProfileBle50SendToPhone(&response[0], sendSize);
    uteModulePlatformMemoryFree(response);
}

/**
*@brief  开始发送所有闹钟数据
*@details
*@author        黄鹤
*@date        2022-04-02
*/
void uteModuleSystemtimeStartSendAllAlarmData(void)
{
    ute_ble_connect_state_t state;
    uteApplicationCommonGetBleConnectionState(&state);
    if (!state.isConnected || uteApplicationCommonGetMtuSize() <= 20)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isConnected=%d,mtuSize=%d", __func__, state.isConnected, uteApplicationCommonGetMtuSize());
        return;
    }
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->currSendMinIndex = 0; //用作发送的包数量
    param->currSendFileIndex = 0; //已发送的闹钟数据的长度
    param->currSendMtuSize = uteApplicationCommonGetMtuSize();
    validAlarmCnt = 0;
    for (uint8_t i = 0; i < systemAlarms.alarmTotalCnt; i ++)//计算有效闹钟个数
    {
        if (systemAlarms.alarmParam[i].isFinish)
        {
            validAlarmCnt ++;
        }
    }
    param->currSendFileSize = validAlarmCnt; //有效闹钟个数
    hasSendAlarmCnt = 0;
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSystemtimeStartSendOneAlarmData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,validAlarmCnt = %d", __func__,validAlarmCnt);
}

/**
*@brief  保存所有闹钟参数
*@details
*@param[in] uint8_t *AlarmData 闹钟数据
*@param[in] uint16_t length 数据长度
*@author        黄鹤
*@date        2022-04-02
*/
void uteModuleSystemtimeSaveAllAlarmInfo(uint8_t *AlarmData,uint16_t length)
{
    uint8_t index = 0;//闹钟个数
    uint8_t i = 0;
    uint16_t oneAlarmDataLen = 0;//闹钟总长度
    uint16_t numAlarmDataLen = 0;
    UTE_MODULE_LOG_BUFF(UTE_LOG_PROTOCOL_LVL, AlarmData, length);
    if (length > (SYSTEM_TIME_ALARMS_MAX_CNT * UTE_MODULE_SYSTEMTIME_ALARM_MAX_NUMBER))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,alarm clock too many", __func__);
        return;
    }
    //删除所有闹钟
    uteModuleSystemtimeDeleteAllAlarm();
#if APP_MODULE_ALARM_LABEL_SET_SUPPORT
    uint8_t alarmClockIndex = 0;
    //得到接收到的闹钟个数
    for (i = 0; i < SYSTEM_TIME_ALARMS_MAX_CNT; i ++)
    {
        oneAlarmDataLen += (16 + AlarmData[oneAlarmDataLen + 15]);
        if (oneAlarmDataLen >= length)
        {
            index = i + 1;
            break;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,index = %d", __func__,index);
    for (i = 0; i < index; i ++)
    {
        ute_module_systemtime_alarm_t oneAlarmInfo;
        //alarmClockIndex = AlarmData[numAlarmDataLen + 7] - 1;
        alarmClockIndex = i;
        memset(&oneAlarmInfo.alarmParam[alarmClockIndex], 0, sizeof(ute_module_systemtime_one_alarm_t));
        oneAlarmInfo.alarmParam[alarmClockIndex].setYear = (AlarmData[numAlarmDataLen + 0] << 8) | AlarmData[numAlarmDataLen + 1];
        oneAlarmInfo.alarmParam[alarmClockIndex].setMonth = AlarmData[numAlarmDataLen + 2];
        oneAlarmInfo.alarmParam[alarmClockIndex].setDay = AlarmData[numAlarmDataLen + 3];
        oneAlarmInfo.alarmParam[alarmClockIndex].setHour = AlarmData[numAlarmDataLen + 4];
        oneAlarmInfo.alarmParam[alarmClockIndex].setMin = AlarmData[numAlarmDataLen + 5];
        oneAlarmInfo.alarmParam[alarmClockIndex].setSec = AlarmData[numAlarmDataLen + 6];
        oneAlarmInfo.alarmParam[alarmClockIndex].index = alarmClockIndex;
        oneAlarmInfo.alarmParam[alarmClockIndex].isOpen = AlarmData[numAlarmDataLen + 8];
        oneAlarmInfo.alarmParam[alarmClockIndex].hour = AlarmData[numAlarmDataLen + 9];
        oneAlarmInfo.alarmParam[alarmClockIndex].min = AlarmData[numAlarmDataLen + 10];
        oneAlarmInfo.alarmParam[alarmClockIndex].weekDay = AlarmData[numAlarmDataLen + 11];
        oneAlarmInfo.alarmParam[alarmClockIndex].durationTimeSec = AlarmData[numAlarmDataLen + 12];
        oneAlarmInfo.alarmParam[alarmClockIndex].jarLevel = AlarmData[numAlarmDataLen + 13];
        if (AlarmData[numAlarmDataLen + 14] > 0x04)
            AlarmData[numAlarmDataLen + 14] = 0x00;
        oneAlarmInfo.alarmParam[alarmClockIndex].alarmLabel = AlarmData[numAlarmDataLen + 14];
        oneAlarmInfo.alarmParam[alarmClockIndex].labelLenth = AlarmData[numAlarmDataLen + 15];
        memcpy(&oneAlarmInfo.alarmParam[alarmClockIndex].label[0], &AlarmData[numAlarmDataLen + 16], AlarmData[numAlarmDataLen + 15]);
        if (AlarmData[numAlarmDataLen + 11] == 0)
        {
            oneAlarmInfo.alarmParam[alarmClockIndex].isSingle = true;
        }
        else
        {
            oneAlarmInfo.alarmParam[alarmClockIndex].isSingle = false;
        }
        oneAlarmInfo.alarmParam[alarmClockIndex].isFinish = true;
        memcpy(&systemAlarms.alarmParam[alarmClockIndex], &oneAlarmInfo.alarmParam[alarmClockIndex], sizeof(ute_module_systemtime_one_alarm_t));
        uteModuleSystemtimeSaveAlarmInfo(systemAlarms.alarmParam[alarmClockIndex], alarmClockIndex);
        numAlarmDataLen += (16 + AlarmData[numAlarmDataLen + 15]);
    }
#else
    //得到接收到的闹钟个数
    for (i = 0; i < SYSTEM_TIME_ALARMS_MAX_CNT; i ++)
    {
        oneAlarmDataLen += (15 + AlarmData[oneAlarmDataLen + 14]);
        if (oneAlarmDataLen >= length)
        {
            index = i + 1;
            break;
        }

    }
    for (i = 0; i < index; i ++)
    {
        ute_module_systemtime_alarm_t oneAlarmInfo;
        uint8_t alarmClockIndex = AlarmData[numAlarmDataLen + 7] - 1;
        memset(&oneAlarmInfo.alarmParam[alarmClockIndex], 0, sizeof(ute_module_systemtime_one_alarm_t));
        oneAlarmInfo.alarmParam[alarmClockIndex].setYear = (AlarmData[numAlarmDataLen + 0] << 8) | AlarmData[numAlarmDataLen + 1];
        oneAlarmInfo.alarmParam[alarmClockIndex].setMonth = AlarmData[numAlarmDataLen + 2];
        oneAlarmInfo.alarmParam[alarmClockIndex].setDay = AlarmData[numAlarmDataLen + 3];
        oneAlarmInfo.alarmParam[alarmClockIndex].setHour = AlarmData[numAlarmDataLen + 4];
        oneAlarmInfo.alarmParam[alarmClockIndex].setMin = AlarmData[numAlarmDataLen + 5];
        oneAlarmInfo.alarmParam[alarmClockIndex].setSec = AlarmData[numAlarmDataLen + 6];
        oneAlarmInfo.alarmParam[alarmClockIndex].index = alarmClockIndex;
        oneAlarmInfo.alarmParam[alarmClockIndex].isOpen = AlarmData[numAlarmDataLen + 8];
        oneAlarmInfo.alarmParam[alarmClockIndex].hour = AlarmData[numAlarmDataLen + 9];
        oneAlarmInfo.alarmParam[alarmClockIndex].min = AlarmData[numAlarmDataLen + 10];
        oneAlarmInfo.alarmParam[alarmClockIndex].weekDay = AlarmData[numAlarmDataLen + 11];
        oneAlarmInfo.alarmParam[alarmClockIndex].durationTimeSec = AlarmData[numAlarmDataLen + 12];
        oneAlarmInfo.alarmParam[alarmClockIndex].jarLevel = AlarmData[numAlarmDataLen + 13];
        oneAlarmInfo.alarmParam[alarmClockIndex].labelLenth = AlarmData[numAlarmDataLen + 14];
        memcpy(&oneAlarmInfo.alarmParam[alarmClockIndex].label[0], &AlarmData[numAlarmDataLen + 15], AlarmData[numAlarmDataLen + 14]);
        oneAlarmInfo.alarmParam[alarmClockIndex].alarmLabel = 0;
        if (AlarmData[numAlarmDataLen + 11] == 0)
        {
            oneAlarmInfo.alarmParam[alarmClockIndex].isSingle = true;
        }
        else
        {
            oneAlarmInfo.alarmParam[alarmClockIndex].isSingle = false;
        }
        oneAlarmInfo.alarmParam[alarmClockIndex].isFinish = true;
        memcpy(&systemAlarms.alarmParam[alarmClockIndex], &oneAlarmInfo.alarmParam[alarmClockIndex], sizeof(ute_module_systemtime_one_alarm_t));
        uteModuleSystemtimeSaveAlarmInfo(systemAlarms.alarmParam[alarmClockIndex], alarmClockIndex);
        numAlarmDataLen += (15 + AlarmData[numAlarmDataLen + 14]);
    }
#endif
    uteModuleSystemtimeSetAlarmTotalCnt(index);
}

/**
*@brief    保存最后一个闹钟被删除的时间戳
*@details
*@author        黄鹤
*@date        2022-04-25
*/
void uteModuleSystemtimeSaveAlarmDelTime(void)
{
    /*! 保存到文件zn.zeng, 2021-08-18  */
    void *file;
    uint8_t writebuff[7];
    writebuff[0] = (systemTime.year >> 8) & 0xff;
    writebuff[1] = systemTime.year & 0xff;
    writebuff[2] = systemTime.month;
    writebuff[3] = systemTime.day;
    writebuff[4] = systemTime.hour;
    writebuff[5] = systemTime.min;
    writebuff[6] = systemTime.sec;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isDistanceMi=%d,is12HourTime=%d,time.zone=%d,languageId=%d", __func__,systemTime.isDistanceMi,systemTime.is12HourTime,systemTime.zone,systemTime.languageId);
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_FINAL_ALARM_DELETE_TIME, &file, FS_O_WRONLY | FS_O_CREAT))//0 0 8 2 0
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writebuff[0], 7);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief    发送最后一个闹钟被删除的时间戳
*@details
*@author        黄鹤
*@date        2022-04-25
*/
void uteModuleSystemtimeReadAlarmDelTime(void)
{
    /*! 保存到文件zn.zeng, 2021-08-18  */
    void *file;
    uint8_t readbuff[7];
    memset(&readbuff[0], 0, 7);
    uint8_t *response = uteModulePlatformMemoryAlloc(10);
    memset(response, 0, 10);
    response[0] = CMD_ALARM_DATA;
    response[1] = 0xAA;//查询指令
    response[2] = 0xFF;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_FINAL_ALARM_DELETE_TIME, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 7);
        uteModuleFilesystemCloseFile(file);
    }
    response[3] =  readbuff[0];
    response[4] =  readbuff[1];
    response[5] =  readbuff[2];
    response[6] =  readbuff[3];
    response[7] =  readbuff[4];
    response[8] =  readbuff[5];
    response[9] =  readbuff[6];
    uteModuleProfileBleSendToPhone((uint8_t *)&response[0], 10);
    uteModulePlatformMemoryFree(response);
}

/**
*@brief  更新闹钟时间戳，删除一个闹钟，其他闹钟时间戳全部变为最新
*@details
*@return  闹钟个数1~8
*@author       dengli.lu
*@date        2022-05-09
*/
void uteModuleSystemtimeUpdataAllAlarm(void)
{
    for(uint8_t i = 0; i < systemAlarms.alarmTotalCnt; i++)
    {
        ute_module_systemtime_one_alarm_t setAlarm;
        uteModuleSystemtimeGetAlarm(&setAlarm,i);
        uteModuleSystemtimeSetAlarm(setAlarm,i);
    }
}
#endif

/**
*@brief
*@details  获取闹钟正在提醒的索引函数
*@author        dengli.lu
*@date        2022.04.24
*/
uint8_t uteModuleSystemtimeAlarmGetRemindingIndex(void)
{
    return systemAlarms.isRemindingIndex;
}

/**
*@brief
*@details  获取最近将要响铃的闹钟
*@author        dengli.lu
*@date        2022.04.24
*/
int uteModuleSystemtimeAlarmGetNextIndex(void)
{

    ute_module_systemtime_one_alarm_t* alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    int cur_hour = time.hour;
    int cur_minute = time.min;
    int cur_weekday = uteModuleSystemtimeGetWeek(time.year,time.month,time.day);
    int current_time = cur_hour * 60 + cur_minute;
    int min_time = INT_MAX;
    int result = -1;
    for (int i = 0; i < uteModuleSystemtimeGetAlarmTotalCnt(); i++)
    {
        uteModuleSystemtimeGetAlarm(alarm, i);
        int alarm_time = alarm->hour * 60 + alarm->min;
        int next_time = -1;
        uint8_t cycle = uteModuleSystemtimeGetAlarmCycle(i);
        if (cycle == 0x80)   // 单次响铃
        {
            if (alarm_time >= current_time)
            {
                next_time = alarm_time;
            }
            else
            {
                next_time = alarm_time + 24 * 60;
            }
        }
        else if (cycle == 0x7F)     // 每天响铃
        {
            if (alarm_time >= current_time)
            {
                next_time = alarm_time;
            }
            else
            {
                next_time = alarm_time + 24 * 60;
            }
        }
        else     // 每周特定天数
        {
            int current_week = cur_weekday; // 假设cur_weekday为0-6（周日、周一...到周六）
            int today_valid = (cycle & BIT(current_week)) != 0;
            if (alarm_time >= current_time && today_valid)
            {
                next_time = alarm_time;
            }
            else
            {
                int min_days = 7;
                for (int s = 0; s < 7; s++)
                {
                    if (cycle & (1 << s))
                    {
                        int days_diff;
                        if (s > current_week)
                        {
                            days_diff = s - current_week;
                        }
                        else if (s < current_week)
                        {
                            days_diff = (7 - current_week) + s;
                        }
                        else
                        {
                            days_diff = alarm_time >= current_time ? 0 : 7;
                        }
                        if (days_diff < min_days)
                        {
                            min_days = days_diff;
                        }
                    }
                }
                if (min_days != 7)
                {
                    next_time = alarm_time + min_days * 24 * 60;
                }
            }
        }

        // 更新最小时间和结果
        if (next_time != -1)
        {
            if (next_time < min_time)
            {
                min_time = next_time;
                result = i;
            }
            else if (next_time == min_time && i < result)
            {
                result = i;
            }
            else if(result == -1)
            {
                result = i;
            }
        }
    }
    if(result == -1)
    {
        result = 0;
    }
    ab_free(alarm);
    return result;
}

/**
*@brief  系统时间每秒处理函数
*@details  系统时间每秒处理总入口
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModuleSystemtimeSecondCb(void)
{
    if (!sys_cb.sys_init_complete)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,sys_init_complete:%d",__func__,sys_cb.sys_init_complete);
        return;
    }
    if (uteApplicationCommonIsStartupFinish())
    {
        uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
        uteModuleSystemtimeChange(&systemTime);
        systemTime.week = uteModuleSystemtimeGetWeek(systemTime.year,systemTime.month,systemTime.day);
        uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
        if(systemTime.isSettingTime)
        {
            systemTime.isSettingTime = false;
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL,"%s,now is setting time",__func__);
        }
#if 0//(UTE_LOG_TIME_LVL && UTE_MODULE_LOG_SUPPORT)
        else
        {
            uteModulePlatformCalibrationSystemTimer();
        }
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,%04d-%02d-%02d %02d:%02d:%02d", __func__, systemTime.year, systemTime.month, systemTime.day, systemTime.hour, systemTime.min, systemTime.sec);
#endif
        if (systemTime.sec == 0)
        {
#if UTE_MODULE_LOCAL_ALARM_REMIND_USER_HANDLE_SUPPORT
            for (uint8_t i = 0; i < systemAlarms.alarmTotalCnt; i++)
            {
                if (systemAlarms.alarmParam[i].isSingle)
                {
                    //单次闹钟只判断时分，最新一次到达该时间后闹钟响，之后关闭该闹钟 xjc modify 20220104
                    if (
                        (systemTime.hour == systemAlarms.alarmParam[i].hour) &&
                        (systemTime.min == systemAlarms.alarmParam[i].min) &&
                        (systemAlarms.alarmParam[i].durationTimeSec) &&
                        (systemAlarms.alarmParam[i].isOpen)
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
                        && (systemAlarms.alarmParam[i].isFinish)
#endif
                    )
                    {
                        uteModuleSystemtimeAlarmCb(systemAlarms.alarmParam[i].durationTimeSec,i);
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
                        uteModuleSystemtimeAlarmAddRepeatRemindInfo(i,true);
                        if((!systemAlarms.alarmParam[i].alarmRepeat.isRepeatRemindOpen)||(systemAlarms.alarmParam[i].alarmRepeat.repeatRemindTimes == 0))
#endif
                        {
                            systemAlarms.alarmParam[i].isOpen = false;
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
                            systemAlarms.alarmParam[i].alarmRepeat.repeatRemindHour = 0xff;
                            systemAlarms.alarmParam[i].alarmRepeat.repeatRemindMin = 0xff;
                            systemAlarms.alarmParam[i].alarmRepeat.repeatRemindTimes = 0;
                            uteModuleSystemtimeSaveRepeatAlarmConfig(systemAlarms.alarmParam[i].alarmRepeat,i);
#endif
                            uteModuleSystemtimeSetAlarm(systemAlarms.alarmParam[i],i);
                            uteModuleSystemtimeStartSendAllAlarmData();
                        }
                    }
                }
                else
                {
                    uint8_t week = uteModuleSystemtimeGetWeek(systemTime.year, systemTime.month, systemTime.day);
                    if (((1 << week)&systemAlarms.alarmParam[i].weekDay)
                        && (systemTime.hour == systemAlarms.alarmParam[i].hour)
                        && (systemTime.min == systemAlarms.alarmParam[i].min)
                        && (systemAlarms.alarmParam[i].durationTimeSec)
                        && (systemAlarms.alarmParam[i].isOpen)
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
                        && (systemAlarms.alarmParam[i].isFinish)
#endif
                       )
                    {
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
                        uteModuleSystemtimeAlarmAddRepeatRemindInfo(i,true);
#endif
                        uteModuleSystemtimeAlarmCb(systemAlarms.alarmParam[i].durationTimeSec,i);
                    }
                }
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
                if((systemAlarms.alarmParam[i].alarmRepeat.isRepeatRemindOpen)
                   && (systemAlarms.alarmParam[i].alarmRepeat.repeatRemindTimes > 0)
                   && (systemAlarms.alarmParam[i].alarmRepeat.repeatRemindHour == systemTime.hour)
                   && (systemAlarms.alarmParam[i].alarmRepeat.repeatRemindMin == systemTime.min))
                {
                    uteModuleSystemtimeAlarmAddRepeatRemindInfo(i,false);
                    uteModuleSystemtimeAlarmCb(systemAlarms.alarmParam[i].durationTimeSec,i);
                }
#endif
            }
#endif
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT && UTE_MODULE_SCREENS_MENSTRUAL_NOTIFY_SUPPORT
            uteModuleMenstrualCycleRemindCb();
#endif
#if APP_CUSTOM_REMIND_SETTING_SUPPORT
            uteModuleSystemtimeCustomRemindCb();
#endif
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
            uteModuleGuiCommonScreenSaverUpdateCb();
#endif
            uteModuleSystemtimeSaveTimeInfo();
        }
        for (uint8_t i = 0; i < systemTimeRegisterData.regCnt; i++)
        {
            uteModuleSystemtimeRegisterSecondCb(i);
        }
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,uteApplicationCommonStartupSecond",__func__);
        /*! 启动流程 读取电池电压 zn.zeng  modify Jul 01, 2021 */
        uteDrvBatteryCommonUpdateBatteryInfo();
        if (uteDrvBatteryCommonGetVoltage() > UTE_DRV_BATTERY_POWER_ON_VOLTAGE)
        {
            uteApplicationCommonStartupSecond();
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,!!! battery voltage:%d", __func__, uteDrvBatteryCommonGetVoltage());
        }
    }
}
/**
*@brief  获取week
*@details   传入年月日，计算星期
*@param[in] uint16_t year
*@param[in] uint16_t month
*@param[in] uint16_t day
*@return  返回星期，星期天为0
*@author        zn.zeng
*@date        Jun 29, 2021
*/
uint8_t uteModuleSystemtimeGetWeek(uint16_t year, uint8_t month, uint8_t day)
{
    uint8_t week;  //代表星期几
    if (month == 1 || month == 2)
    {
        month += 12;
        year--;
    }
    week = (day + 1 + 2 * month + 3 * (month + 1) / 5 + year + year / 4 - year / 100 + year / 400) % 7;
    return week;
}

/**
*@brief  获取系统时间
*@details
*@param[out] ute_module_systemtime_time_t *time  时间变量指针
*@author        zn.zeng
*@date        Jun 29, 2021
*/
void uteModuleSystemtimeGetTime(ute_module_systemtime_time_t *time)
{
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(time, &systemTime, sizeof(ute_module_systemtime_time_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}
/**
*@brief
*@details 计算增加天数的日期
*@param[out] ute_module_systemtime_time_t *time  时间变量指针
*@param[in] uint8_t addDay 增加的具体天数
*@author        zn.zeng
*@date        Jun 30, 2021
*/
void uteModuleSystemtimeInputDateCalDay(ute_module_systemtime_time_t *time, uint16_t addDay)
{
    for (int i = 0; i < addDay; i++)
    {
        const uint8_t *monday;
        time->hour = 0;
        time->day++;
        if (uteModuleSystemtimeIsLeapYear(time->year))
        {
            monday = &leapEveryMonDays[0];
        }
        else
        {
            monday = &everyMonDays[0];
        }
        if (time->day > monday[time->month])
        {
            time->day = 1;
            time->month++;
            if (time->month > 12)
            {
                time->month = 1;
                time->year = time->year + 1;
            }
        }
    }
}
/**
*@brief
*@details 计算增加减少天数的日期
*@param[out] ute_module_systemtime_time_t *time  时间变量指针
*@param[in] int  增加减少的具体天数，正为增加，负为减少，0为不操作
*@author        zn.zeng
*@date        2022-08-19
*/
void uteModuleSystemtimeInputDateIncreaseDecreaseDay(ute_module_systemtime_time_t *time, int day)
{
    tm_t timeptr;
    time_t timestamp = 0;
    timeptr.year = time->year;
    timeptr.mon = systemTime.month;
    timeptr.day = systemTime.day;
    timeptr.hour = 0;
    timeptr.min = 0;
    timeptr.sec = 0;
    time_t tmp = tm_to_time(timeptr);
    timestamp = tmp + day * 86400;
    timeptr = time_to_tm(timestamp);
    timeptr.weekday = get_weekday(timeptr.year, timeptr.mon, timeptr.day);
    time->year = timeptr.year;
    time->month = timeptr.mon;
    time->day = timeptr.day;
    time->week = timeptr.weekday;
}
/**
*@brief
*@details 计算指定日期开始增加减少天数的日期
*@param[out] ute_module_systemtime_time_t *time  时间变量指针
*@param[in] int  增加减少的具体天数，正为增加，负为减少，0为不操作
*@author        wuhuowang
*@date        2023-02-19
*/
void uteModuleSystemtimeFixDateIncreaseDecreaseDay(ute_module_systemtime_time_t *time, int day)
{
    tm_t timeptr;
    tm_t timeout;
    time_t timestamp = 0;
    timeptr.year = time->year;
    timeptr.mon = time->month;
    timeptr.day = time->day;
    timeptr.hour = 0;
    timeptr.min = 0;
    timeptr.sec = 0;
    time_t tmp = tm_to_time(timeptr);
    timestamp = tmp + day * 86400;
    timeout = time_to_tm(&timestamp);
    timeout.weekday = get_weekday(timeout.year, timeout.mon, timeout.day);
    time->year = timeout.year;
    time->month = timeout.mon;
    time->day = timeout.day;
    time->week = timeout.weekday;
}
/**
 *@brief
 *@details 计算两个指点日期相差天数
 *@author        dengli.lu
 *@date        2023-03-14
 */
int uteModuleSystemtimeGetDiffDay(uint16_t year_start, uint8_t month_start, uint8_t day_start, uint16_t year_end, uint8_t month_end, uint8_t day_end)
{
    int diffDay = 0;
    tm_t timeptrFirst;
    tm_t timeptrSecond;
    time_t timestampFirst = 0, timestampSecond = 0;

    timeptrFirst.year = year_start;
    timeptrFirst.mon = month_start;
    timeptrFirst.day = day_start;
    timeptrFirst.hour = 0;
    timeptrFirst.min = 0;
    timeptrFirst.sec = 0;
    timestampFirst = tm_to_time(timeptrFirst);

    timeptrSecond.year = year_end;
    timeptrSecond.mon = month_end;
    timeptrSecond.day = day_end;
    timeptrSecond.hour = 0;
    timeptrSecond.min = 0;
    timeptrSecond.sec = 0;
    timestampSecond = tm_to_time(timeptrSecond);
    if (timestampSecond >= timestampFirst)
    {
        diffDay = (timestampSecond - timestampFirst) / 86400;
    }
    else
    {
        diffDay = (timestampFirst - timestampSecond) / 86400;
        diffDay = 0 - diffDay;
    }
    return diffDay;
}

/**
*@brief 注册系统每秒时间函数
*@details
*@param[in] void* function 函数指针
*@author        zn.zeng
*@date        2021-07-12
*/
void uteModuleSystemtimeRegisterSecond(void* function)
{
    for(uint8_t i = 0; i<UTE_MODULE_SYSTEMTEIM_REGISTER_MAX_CNT; i++)
    {
        if(systemTimeRegisterData.function[i]==NULL)
        {
            systemTimeRegisterData.function[i] = (ute_module_systemtime_reg_func_t)function;
            systemTimeRegisterData.regCnt++;
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,systemTimeRegisterData.regCnt=%d,.function[i]=%p", __func__, systemTimeRegisterData.regCnt,systemTimeRegisterData.function[i]);
            return;
        }
        else if(systemTimeRegisterData.function[i]==(ute_module_systemtime_reg_func_t)function)
        {
            UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,has register function=0x%p", __func__, function);
            return;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,systemTimeRegisterData.regCnt is max", __func__);
}
/**
*@brief 注册系统每秒时间回调函数
*@details
*@param[in] uint32_t param 参数
*@author        zn.zeng
*@date        2021-07-12
*/
void uteModuleSystemtimeRegisterSecondCb(uint32_t param)
{
    if(param>(UTE_MODULE_SYSTEMTEIM_REGISTER_MAX_CNT-1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,param is too lager", __func__);
    }
    if(systemTimeRegisterData.function[param]!=NULL)
    {
//        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,function[%d]=%p", __func__,param,systemTimeRegisterData.function[param]);
        systemTimeRegisterData.function[param]();
    }
    else
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,param is error,function is NULL", __func__);
    }
}
/**
*@brief 注册系统每分钟时间回调函数
*@details
*@param[in] uint32_t param 参数
*@author        cxd
*@date        2022-04-26
*/
void uteModuleSystemtimeRegisterMinuteCb(void)
{
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s", __func__);
//    if(!uteApplicationCommonIsPowerOn())
//    {
//        return;
//    }
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    if(uteModuleGuiCommonIsInScreenSaver())
    {
        // uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_SCREEN);
        // uteDrvScreenCommonInterfaceInit();
        // uteModulePlatformDelayMs(10);
        uteModuleGuiCommonAnimationTimerCallback(NULL);
        // uteModulePlatformDelayMs(100);
        // uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_SCREEN);
    }
#endif
}
/**
*@brief  获取是否晚上时间
*@details
*@return true为晚上时间定义
*@author        zn.zeng
*@date        2021-08-05
*/
bool uteModuleSystemtimeIsNight(void)
{
    uint32_t oneDaySecond = systemTime.hour*3600+systemTime.min*60+systemTime.sec;
    if((oneDaySecond<28800||oneDaySecond>64800))  // 18:00~8:00
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief  获取是否睡眠时间
*@details
*@return true 为睡眠时间
*@author        zn.zeng
*@date        2021-08-05
*/
bool uteModuleSystemtimeIsSleepTime(void)
{
    uint32_t oneDaySecond = systemTime.hour*3600+systemTime.min*60+systemTime.sec;
    if((oneDaySecond<43200||oneDaySecond>64800))  // 18:00~12:00
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief  保存闹钟参数
*@details
*@param[in](ute_module_systemtime_one_alarm_t value,uint8_t index)
*@author        zn.zeng
*@date        2021-08-21
*/
void uteModuleSystemtimeSaveAlarmInfo(ute_module_systemtime_one_alarm_t value,uint8_t index)
{
    /*! 保存到文件zn.zeng, 2021-08-21*/
    void *file;
    uint8_t writebuff[UTE_MODULE_SYSTEMTIME_ALARM_SIZE];
    memset(&writebuff[0], 0, UTE_MODULE_SYSTEMTIME_ALARM_SIZE);
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    memcpy(&systemAlarms.alarmParam[index],&value,sizeof(ute_module_systemtime_one_alarm_t));
    writebuff[0] = systemAlarms.alarmParam[index].year >> 8 & 0xff;
    writebuff[1] = systemAlarms.alarmParam[index].year & 0xff;
    writebuff[2] = systemAlarms.alarmParam[index].month;
    writebuff[3] = systemAlarms.alarmParam[index].day;
    writebuff[4] = systemAlarms.alarmParam[index].weekDay;
    writebuff[5] = systemAlarms.alarmParam[index].hour;
    writebuff[6] = systemAlarms.alarmParam[index].min;
    writebuff[7] = systemAlarms.alarmParam[index].sec;
    writebuff[8] = systemAlarms.alarmParam[index].isSingle;
    writebuff[9] = systemAlarms.alarmParam[index].durationTimeSec;
    writebuff[10] = systemAlarms.alarmParam[index].isOpen;
#if UTE_MODULE_SCREENS_LOCAL_ALARM_LABEL_SUPPORT
    writebuff[11] = systemAlarms.alarmParam[index].setYear >>8 & 0xff;
    writebuff[12] = systemAlarms.alarmParam[index].setYear & 0xff;
    writebuff[13] = systemAlarms.alarmParam[index].setMonth;
    writebuff[14] = systemAlarms.alarmParam[index].setDay;
    writebuff[15] = systemAlarms.alarmParam[index].setHour;
    writebuff[16] = systemAlarms.alarmParam[index].setMin;
    writebuff[17] = systemAlarms.alarmParam[index].setSec;
    writebuff[18] = systemAlarms.alarmParam[index].index;
    writebuff[19] = systemAlarms.alarmParam[index].labelLenth;
    writebuff[20] = systemAlarms.alarmParam[index].alarmLabel >> 8 & 0xff;
    writebuff[21] = systemAlarms.alarmParam[index].alarmLabel & 0xff;
    memcpy(&writebuff[22], &systemAlarms.alarmParam[index].label[0], systemAlarms.alarmParam[index].labelLenth);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,alarmLabel=%d", __func__, systemAlarms.alarmParam[index].alarmLabel);
#endif
    uint8_t path[20];
    memset(&path[0], 0, 20);
    sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_ALARMINFO_DIR, index);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file, &writebuff[0], UTE_MODULE_SYSTEMTIME_ALARM_SIZE);
        uteModuleFilesystemCloseFile(file);
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isSingle=%d,hour=%d,min=%d", __func__,value.isSingle,value.hour,value.min);
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index=%d,weekDay=0x%x,durationTimeSec=%d.isOpen=%d", __func__,index,value.weekDay,value.durationTimeSec,value.isOpen);
    }
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_ALARMINFO_DIR, dirInfo, NULL);
    systemAlarms.alarmTotalCnt = dirInfo->filesCnt;
    uteModulePlatformMemoryFree(dirInfo);
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,alarmTotalCnt = %d", __func__,systemAlarms.alarmTotalCnt);
}
/**
*@brief    保存时间信息
*@details
*@author        zn.zeng
*@date        2021-11-08
*/
void uteModuleSystemtimeSaveTimeInfo(void)
{
    /*! 保存到文件zn.zeng, 2021-08-18  */
    void *file;
    uint8_t writebuff[14];
    writebuff[0] = systemTime.isDistanceMi;
    writebuff[1] = systemTime.is12HourTime;
    writebuff[2] = systemTime.zone;
    writebuff[3] = (systemTime.languageId>>8)&0xff;
    writebuff[4] = systemTime.languageId&0xff;
    writebuff[5] = (systemTime.year>>8)&0xff;
    writebuff[6] = systemTime.year&0xff;
    writebuff[7] = systemTime.month;
    writebuff[8] = systemTime.day;
    writebuff[9] = systemTime.hour;
    writebuff[10] = systemTime.min;
    writebuff[11] = systemTime.sec;
    writebuff[12] = systemTime.isWatchSetLangage;
    writebuff[13] = systemTime.AppSetlanguageId;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isDistanceMi=%d,is12HourTime=%d,time.zone=%d,languageId=%d", __func__,systemTime.isDistanceMi,systemTime.is12HourTime,systemTime.zone,systemTime.languageId);
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_FORMAT,&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],14);
        uteModuleFilesystemCloseFile(file);
    }

}
/**
*@brief    设置显示时间格式
*@details   包括距离的单位，英里和公里的切换，并保存
*@author        zn.zeng
*@date        2021-08-18
*/
void uteModuleSystemtimeSetTimeFormat(bool is12Hour,bool isMi)
{
    if(is12Hour != systemTime.is12HourTime)
    {
        systemTime.change12HourTime = true;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言消息更新界面
    }
    systemTime.isDistanceMi = isMi;
    systemTime.is12HourTime = is12Hour;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,isDistanceMi=%d,is12HourTime=%d", __func__,systemTime.isDistanceMi,systemTime.is12HourTime);
    uteModuleSystemtimeSaveTimeInfo();
}
/**
*@brief  保存时区
*@details
*@param[in] (int8_t zone)
*@author        zn.zeng
*@date        2021-08-18
*/
void uteModuleSystemtimeSetTimeZone(int8_t zone)
{
    systemTime.zone=zone;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,zone=%d", __func__,systemTime.zone);
    uteModuleSystemtimeSaveTimeInfo();
}
/**
*@brief  保存语言
*@details
*@param[in] (uint8_t id)
*@author        zn.zeng
*@date        2021-08-23
*/

void uteModuleSystemtimeSetLanguage(uint16_t id)
{
#if (!SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT)
    if(id == 0x01)
    {
        id = 0x02;
    }
#endif
    if (id == BAHASA_LANGUAGE_ID)
    {
        id = INDONESUAN_LANGUAGE_ID; //印尼语APP发的是0x60，暂时先改为0x13
    }
    systemTime.languageId=id;
    systemTime.isWatchSetLangage = true;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,languageId=%d", __func__,systemTime.languageId);
    uteModuleSystemtimeSaveTimeInfo();
    uteLanguageCommonSelect(systemTime.languageId);
    msg_enqueue(MSG_CHECK_LANGUAGE);
}
/**
*@brief  保存APP设计的语言
*@details
*@param[in] (uint8_t id)
*@author        gyj
*@date        2023-09-7
*/

void uteModuleSystemtimeAPPSetLanguage(uint16_t id)
{
    systemTime.AppSetlanguageId = id;
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,AppSetlanguageId=%d", __func__,systemTime.languageId);
    uteModuleSystemtimeSaveTimeInfo();
}
/**
*@brief  读取当前设置语言
*@details
*@author        zn.zeng
*@date        2021-08-23
*/
uint16_t uteModuleSystemtimeReadLanguage(void)
{
    uint16_t id = 0;
    id = systemTime.languageId;
    return id;
}
/**
*@brief  对比当前设置语言
*@details
*@author        xjc
*@date        2022-01-22
*/
bool uteModuleSystemtimeCompareLanguage(uint16_t languageId)
{
    if(systemTime.languageId == languageId)
    {
        return true;
    }
    return false;
}
/**
*@brief  读取距离类型
*@details
*@author        dengli.lu
*@date        2021-10-26
*/
bool uteModuleSystemtimeGetDistanceMiType(void)
{
    return systemTime.isDistanceMi;
}
/**
*@brief  复位时间制切换状态
*@details
*@author        xjc
*@date        2022-08-26
*/
void uteModuleSystemtimeResetChange12HourTime(void)
{
    systemTime.change12HourTime = false;
}
/**
*@brief  读取时间制是否切换
*@details
*@author        xjc
*@date        2022-08-26
*/
bool uteModuleSystemtimeWhetherChange12HourTime(void)
{
    return systemTime.change12HourTime;
}
/**
*@brief  是否打开24H制
*@details
*@return
*@author        raymond
*@date        2021-08-27
*/
bool uteModuleSystemtime12HOn(void)
{
    return systemTime.is12HourTime;
}
/**
*@brief  设置24H制
*@details
*@return
*@author        raymond
*@date        2021-08-27
*/
void uteModuleSystemtime12HSwitchStatus(bool isopen)
{
    systemTime.is12HourTime = isopen;
}
/**
*@brief  设置时间和日期状态
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
void uteModuleSystemTimeLocalTimeSetStatus(uint8_t status)
{
    uteModuleSystemTimeLocalTimeStatus = status;
}

/**
*@brief  获取时间和日期状态
*@details
*@return
*@author        raymond
*@date        2022-03-01
*/
uint8_t uteModuleSystemTimeLocalTimeGetStatus(void)
{
    return uteModuleSystemTimeLocalTimeStatus;
}

/**
*@brief             设置日期
*@details
*@return
*@author      casen
*@date        2022-10-29
*/
void uteModuleSystemTimeLocalSetDate(uint16_t year,uint8_t month,uint8_t day)
{
    if(year<2000 || year>2099) return;
    if(month<1 || month >12) return;
    if(day<1) return;
    if (uteModuleSystemtimeIsLeapYear(year))
    {
        if(day>leapEveryMonDays[month])
        {
            return;
        }
    }
    else
    {
        if(day>everyMonDays[month])
        {
            return;
        }
    }
    ute_module_systemtime_time_t set;
    uteModuleSystemtimeGetTime(&set);
    set.year  = year;
    set.month = month;
    set.day     =   day;
    uteModuleSystemtimeSetTime(set);
}

/**
*@brief             设置时分
*@details
*@return
*@author      casen
*@date        2022-10-29
*/
void uteModuleSystemTimeLocalSetHourMin(uint8_t hour,uint8_t min)
{
    if(hour>23) return;
    if(min>59) return;
    ute_module_systemtime_time_t set;
    uteModuleSystemtimeGetTime(&set);
    set.hour  = hour;
    set.min = min;
    uteModuleSystemtimeSetTime(set);
}


#if APP_MODULE_RECEIVE_SAVE_TO_FILESYS_SUPPORT
/**
*@brief             保存从APP接收到的数据
*@details
*@return
*@author      wuhuowang
*@date        2022-12-16
*/
void uteModuleSystemTimeSaveReceiveData(uint8_t *data, uint8_t length, uint8_t index, bool isfirstFlag)
{
    uint8_t path[40];
    void *file;
    int32_t fileSize = 0;

    if (isfirstFlag == true)
    {
        uteModuleSystemTimeDeleteReceiveData();
    }
    memset(&path[0], 0, 40);
    sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_PROTOCAL_RECEIVE_DIR, index);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,save file=%s", __func__, &path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
        fileSize = uteModuleFilesystemGetFileSize(file);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,fileSize=%d", __func__, fileSize);
        uteModuleFilesystemWriteData(file, data, length);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief             删除从APP接收到的数据
*@details
*@return
*@author      wuhuowang
*@date        2022-12-16
*/
void uteModuleSystemTimeDeleteReceiveData(void)
{
    uint8_t path[40];
    uint8_t i = 0;
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));

    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_PROTOCAL_RECEIVE_DIR, dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,dirInfo->filesCnt=%d", __func__, dirInfo->filesCnt);
    if (dirInfo->filesCnt > 0)
    {
        for (i = 0; i < dirInfo->filesCnt; i ++)
        {
            memset(&path[0], 0, 40);
            sprintf((char *)&path[0],"%s/%s", UTE_MODULE_FILESYSTEM_PROTOCAL_RECEIVE_DIR, &dirInfo->filesName[i][0]);
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,del file=%s", __func__, &path[i]);
            uteModuleFilesystemDelFile((char *)&path[0]);
        }
    }
    uteModulePlatformMemoryFree(dirInfo);
}
#endif


///适配中科平台接口（闹钟）

/**
*@brief  使能闹钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        Jun 29, 2021
*/

void uteModuleSystemtimeEnableAlarm(uint8_t index, bool en)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index:%d en:%d", __func__,index,en);
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    systemAlarms.alarmParam[index].isOpen = en;
    uteModuleSystemtimeSaveAlarmInfo(systemAlarms.alarmParam[index],index);
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}

/**
*@brief  获取闹钟设置的小时
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmHour(uint8_t index)
{
    uint8_t hour = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);
    hour = alarm->hour;
    ab_free(alarm);
    return hour;
}

/**
*@brief  获取闹钟设置的分钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmMin(uint8_t index)
{
    uint8_t min = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);
    min = alarm->min;
    ab_free(alarm);
    return min;
}

/**
*@brief  获取闹钟设置的秒
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmSec(uint8_t index)
{
    uint8_t sec = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);
    sec = alarm->sec;
    ab_free(alarm);
    return sec;
}

/**
*@brief  获取闹钟设置的提醒周期
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmCycle(uint8_t index)
{
    uint8_t ret = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);

    ret |= alarm->weekDay;

    if (alarm->isSingle)
    {
        ret |= BIT(7);
    }
    else
    {
        ret &= ~BIT(7);
    }

    ab_free(alarm);
    return ret;
}

/**
*@brief  获取闹钟是否使能
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmEnableState(uint8_t index)
{
    uint8_t isOpen = 0;
    ute_module_systemtime_one_alarm_t *alarm = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (alarm == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    uteModuleSystemtimeGetAlarm(alarm, index);
    isOpen = alarm->isOpen;
    ab_free(alarm);
    return isOpen;
}

/**
*@brief  获取闹钟可以设置的最大数量
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/

uint8_t uteModuleSystemtimeGetAlarmMaxCnt(void)
{
    return SYSTEM_TIME_ALARMS_MAX_CNT;
}

/**
*@brief  获取闹钟是否空闲
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/
uint8_t uteModuleSystemtimeGetAlarmIsFree(uint8_t index)
{
    return (index < uteModuleSystemtimeGetAlarmMaxCnt()) && (index >= uteModuleSystemtimeGetAlarmTotalCnt());
}

/**
*@brief  获取闹钟标签说明内容长度
*@details
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zhenhua.cai
*@date        2025-08-12
*/
uint8_t uteModuleSystemtimeGetAlarmLableDataLen(uint8_t index)
{
    return systemAlarms.alarmParam[index].labelLenth;
}
/**
*@brief  获取闹钟标签说明内容
*@details
*@param[in] labe *set  传入要设置标签数组的指针
*@param[in] uint8_t *len 传入要设置标签数组的长度
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@param[out] uint8_t *len 获取标签说明的长度
*@author        zhenhua.cai
*@date        2025-08-12
*/
void uteModuleSystemtimeGetAlarmLableData(char *labe, uint8_t *len, uint8_t index)
{
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    if (*len == 0)
    {
        *len = sizeof(systemAlarms.alarmParam[index].label);
    }
    char *data = ab_zalloc(*len);
    uint8_t data_len = 0;
    uteModuleCharencodeUnicodeConversionUtf8(&systemAlarms.alarmParam[index].label[0], systemAlarms.alarmParam[index].labelLenth, data, &data_len, *len);
    memcpy(&labe[0], data, data_len);
    *len = data_len;
    ab_free(data);
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}

/**
*@brief  编辑闹钟
*@details
*@param[in] ute_module_systemtime_one_alarm_t *set  传入要设置闹钟参数的指针
*@param[in] uint8_t index 所需要设置的闹钟序号，从0开始
*@author        zn.zeng
*@date        2021-08-21
*/
uint8_t uteModuleSystemtimeAlarmEdit(uint8_t index, bool enable, uint8_t cycle, uint8_t alarm_hour, uint8_t alarm_minute, uint8_t motor_mode, uint8_t remind_later)
{

    ute_module_systemtime_one_alarm_t *set = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));
    if (set == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return 0;
    }
    set->isOpen = enable;
    set->weekDay = cycle & (~BIT(7));
    set->isSingle = (cycle & BIT(7)) ? true : false;
    set->hour = alarm_hour;
    set->min = alarm_minute;
    set->sec = 0;
#if UTE_MODULE_SCREENS_LOCAL_ALARM_LABEL_SUPPORT
    set->alarmLabel = ute_moduleSystemtimeAlarmGetLabel(index);
    memcpy(set->label,systemAlarms.alarmParam[index].label,101);
    set->labelLenth = systemAlarms.alarmParam[index].labelLenth;
#endif
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT    //编辑闹钟默认添加贪睡提醒次数
    set->alarmRepeat.repeatRemindTimes = ALARM_REPEAT_REMIND_DEFAULT_TIMES;
#endif
//    uteModuleSystemtimeSetAlarmTotalCnt(uteModuleSystemtimeGetAlarmTotalCnt()+1);
    uteModuleSystemtimeSetAlarm(*set, index);
    ab_free(set);
    return 0;
}

uint8_t uteModuleSystemtimeGetAlarmRingIndex(void)
{
    return systemAlarms.isRemindingIndex;
}

void uteModuleSystemtimeSetAlarmRingIndex(uint8_t index)
{
    systemAlarms.isRemindingIndex = index;
}
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT

// 贪睡时显示对应闹钟为打开状态
#ifndef UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_DISPLAY_ALARM_OPEN
#define UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_DISPLAY_ALARM_OPEN 0
#endif

/**
 * @brief 处理闹钟的重复提醒功能
 *
 * 该函数用于处理闹钟的贪睡/重复提醒功能，主要完成以下操作：
 * 1. 开启重复提醒标志
 * 2. 减少剩余重复提醒次数
 * 3. 计算并设置下一次提醒时间
 * 4. 更新闹钟设置
 * 5. 对于单次闹钟的特殊处理
 *
 * @param alarm 指向闹钟结构体的指针，包含闹钟的各种设置信息
 * @param index 闹钟的索引号，用于标识具体是哪个闹钟
 */
void ute_moduleSystemtimeAlarmRepeatRemindHandle(ute_module_systemtime_one_alarm_t* alarm,int index)
{
    // 参数有效性检查
    if(alarm == NULL)return;

    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,alarm->repeatRemindTimes = %d", __func__, alarm->alarmRepeat.repeatRemindTimes);
    // 开启重复提醒功能并减少剩余提醒次数
    alarm->alarmRepeat.isRepeatRemindOpen = true;
    alarm->isOpen = true;
    if(alarm->alarmRepeat.repeatRemindTimes > 0)
    {
        alarm->alarmRepeat.repeatRemindTimes--;
    }

    // 计算下一次提醒时间（当前时间 + 默认贪睡时间）
    uint8_t hour_later = alarm->alarmRepeat.repeatRemindHour;
    uint8_t min_later = alarm->alarmRepeat.repeatRemindMin + ALARM_REPEAT_REMIND_DEFAULT_TIME_MIN;

    // 处理分钟进位和小时溢出
    if(min_later > 59)
    {
        hour_later += 1;
        min_later -= 60;
    }
    if(hour_later > 24)
    {
        hour_later -= 24;
    }

    // 更新闹钟的下次提醒时间并设置闹钟
    alarm->alarmRepeat.repeatRemindHour = hour_later;
    alarm->alarmRepeat.repeatRemindMin = min_later;
    uteModuleSystemtimeSetAlarm(*alarm,index);

#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_DISPLAY_ALARM_OPEN || (GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT || GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT)
    // 对于单次闹钟的特殊处理：根据剩余重复次数决定是否保持闹钟使能状态
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,alarm->repeatRemindTimes = %d", __func__, alarm->repeatRemindTimes);
    if (uteModuleSystemtimeGetAlarmCycle(index) & BIT(7))
    {
        if(alarm->repeatRemindTimes > 0)
        {
            uteModuleSystemtimeEnableAlarm(index, true);
        }
        else
        {
            uteModuleSystemtimeEnableAlarm(index, false);
        }
    }
#endif
}

#if UTE_MODULE_SCREENS_LOCAL_ALARM_LABEL_SUPPORT
/**
 * @brief        对某个闹钟设置标签
 * @details      有效标签，NULL_LABEL、ALARM_BUTTON_CLICK、WAKING_BUTTON_CLICK、MEETING_BUTTON_CLICK、APPOINTMENT_BUTTON_CLICK、DINNER_BUTTON_CLICK，分别对应：空 闹钟，起床，会议，约会，聚餐
 * @param[in]    index 闹钟索引
 * @param[int]   label 闹钟标签
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-07-14
 */
void ute_moduleSystemtimeAlarmSetLabel(uint8_t index, uint16_t label)
{
    if (index > (SYSTEM_TIME_ALARMS_MAX_CNT - 1))
    {
        UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index is too max", __func__);
        return;
    }
    UTE_MODULE_LOG(UTE_LOG_TIME_LVL, "%s,index:%d label:%d", __func__,index,label);
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
#if APP_MODULE_ALARM_LABEL_SET_SUPPORT
    systemAlarms.alarmParam[index].alarmLabel = label & 0xff;
#else
    systemAlarms.alarmParam[index].alarmLabel = label;
#endif
    uteModuleSystemtimeSaveAlarmInfo(systemAlarms.alarmParam[index],index);
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}
/**
 * @brief        获取某个闹钟标签
 * @details      有效标签，NULL_LABEL、ALARM_BUTTON_CLICK、WAKING_BUTTON_CLICK、MEETING_BUTTON_CLICK、APPOINTMENT_BUTTON_CLICK、DINNER_BUTTON_CLICK，分别对应：空 闹钟，起床，会议，约会，聚餐
 * @param[in]    index 闹钟索引
 * @return       uint16_t*
 * @author       caizhenhua
 * @date         2025-08-07
 */
uint16_t ute_moduleSystemtimeAlarmGetLabel(uint8_t index)
{
    uint16_t label = 0;
    label = systemAlarms.alarmParam[index].alarmLabel;
    return label;
}
#endif

#endif

#if APP_CUSTOM_REMIND_SETTING_SUPPORT
static ute_module_systemtime_custom_remind_t systemRemind;
static uint8_t systemRemindBuf[APP_CUSTOM_REMIND_SETTING_MAX_CNT * APP_CUSTOM_REMIND_SETTING_DAY_SIZE];

void uteModuleSystemtimeCustomRemindInitData(void)
{
    uint8_t i = 0;
    uint8_t j = 0;

    systemRemind.remindTotalCnt = APP_CUSTOM_REMIND_SETTING_MAX_CNT;
    for (i = 0; i < APP_CUSTOM_REMIND_SETTING_MAX_CNT; i ++)
    {
        systemRemind.remindParam[i].year = 2024;
        systemRemind.remindParam[i].month = 1;
        systemRemind.remindParam[i].day = 25 + i;
        for (j = 0; j < 5; j ++)
        {
            systemRemind.remindParam[i].oneDayRemind[j].type = j;
            systemRemind.remindParam[i].oneDayRemind[j].isOpen = j % 2;
            systemRemind.remindParam[i].oneDayRemind[j].hour = j * 3 + 5;
            systemRemind.remindParam[i].oneDayRemind[j].min = j * 3 + 10;
        }
    }
}

/**
*@brief             保存提醒数据
*@details
*@return
*@author      wuhuowang
*@date        2024-01-22
*/
void uteModuleSystemTimeSaveRemindData(uint8_t *data, uint16_t length)
{
    uint8_t path[30];
    void *file;
//    int32_t fileSize = 0;

    uteModuleSystemTimeDeleteRemindData();
    memset(&path[0], 0, 30);
    sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_CUSTOM_REMIND_DIR, 0);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,save file=%s", __func__, &path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
//        fileSize = uteModuleFilesystemGetFileSize(file);
//        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,fileSize=%d", __func__, fileSize);
        uteModuleFilesystemWriteData(file, data, length);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief  读取提醒数据
*@details
*@author        wuhuowang
*@date        2024-01-22
*/
void uteModuleSystemtimeReadRemindData(void)
{
    uint8_t path[30];
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(APP_CUSTOM_REMIND_SETTING_MAX_CNT * sizeof(ute_module_systemtime_oneday_remind_t));
    void *file;
    int32_t fileSize = 0;

    memset(&path[0], 0, sizeof(path));
    sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_CUSTOM_REMIND_DIR, 0x00);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
        fileSize = (uint32_t)uteModuleFilesystemGetFileSize(file);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,fileSize=%d", __func__, fileSize);
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readBuff[0], fileSize);
        uteModuleFilesystemCloseFile(file);
        uteModuleSystemtimeSaveAllRemindData(readBuff, fileSize, false);
    }
    uteModulePlatformMemoryFree(readBuff);
}

/**
*@brief             删除从APP接收到的数据
*@details
*@return
*@author      wuhuowang
*@date        2024-01-22
*/
void uteModuleSystemTimeDeleteRemindData(void)
{
    uint8_t path[30];
    uint8_t i = 0;
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));

    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_CUSTOM_REMIND_DIR, dirInfo, NULL);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,dirInfo->filesCnt=%d", __func__, dirInfo->filesCnt);
    if (dirInfo->filesCnt > 0)
    {
        for (i = 0; i < dirInfo->filesCnt; i ++)
        {
            memset(&path[0], 0, 30);
            sprintf((char *)&path[0],"%s/%s", UTE_MODULE_FILESYSTEM_CUSTOM_REMIND_DIR, &dirInfo->filesName[i][0]);
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,del file=%s", __func__, &path[i]);
            uteModuleFilesystemDelFile((char *)&path[0]);
        }
    }
    uteModulePlatformMemoryFree(dirInfo);
}

/**
*@brief             手表端操作获取提醒数据
*@details
*@return
*@author      wuhuowang
*@date        2024-01-22
*/
void uteModuleSystemTimeResetRemindData(void)
{
    systemRemind.remindTotalCnt = 0;
    uteModuleSystemtimeSaveReminderConfig();
    memset(systemRemindBuf, 0, sizeof(systemRemindBuf));
    uteModuleSystemTimeSaveRemindData(systemRemindBuf, APP_CUSTOM_REMIND_SETTING_MAX_CNT * APP_CUSTOM_REMIND_SETTING_DAY_SIZE);
}
/**
*@brief             手表端操作获取提醒数据
*@details
*@return
*@author      wuhuowang
*@date        2024-01-22
*/
void uteModuleSystemTimeGetRemindDataFromWatch(void)
{
    //uint16_t totalSize = APP_CUSTOM_REMIND_SETTING_MAX_CNT * APP_CUSTOM_REMIND_SETTING_DAY_SIZE;
    //uint8_t *readBuff = uteModulePlatformMemoryAlloc(totalSize);
    uint8_t i = 0;
    uint8_t j = 0;
    uint16_t index = 0;

    memset(systemRemindBuf, 0, sizeof(systemRemindBuf));
    for (i = 0; i < APP_CUSTOM_REMIND_SETTING_MAX_CNT; i ++)
    {
        systemRemindBuf[index ++] = (systemRemind.remindParam[i].year) >> 8 & 0xFF;
        systemRemindBuf[index ++] = systemRemind.remindParam[i].year & 0xFF;
        systemRemindBuf[index ++] = systemRemind.remindParam[i].month;
        systemRemindBuf[index ++] = systemRemind.remindParam[i].day;
        for (j = 0; j < APP_CUSTOM_REMIND_SETTING_DAY_CNT; j ++)
        {
            systemRemindBuf[index ++] = systemRemind.remindParam[i].oneDayRemind[j].type;
            systemRemindBuf[index ++] = systemRemind.remindParam[i].oneDayRemind[j].isOpen;
            systemRemindBuf[index ++] = systemRemind.remindParam[i].oneDayRemind[j].hour;
            systemRemindBuf[index ++] = systemRemind.remindParam[i].oneDayRemind[j].min;
        }
    }
    //uteModulePlatformMemoryFree(readBuff);
}
/**
*@brief             手表端操作保存提醒数据
*@details
*@return
*@author      wuhuowang
*@date        2024-01-22
*/
void uteModuleSystemTimeSaveRemindDataByWatch(void)
{
    uint16_t totalSize = APP_CUSTOM_REMIND_SETTING_MAX_CNT * APP_CUSTOM_REMIND_SETTING_DAY_SIZE;

    uteModuleSystemTimeGetRemindDataFromWatch();
    uteModuleSystemTimeSaveRemindData(systemRemindBuf, totalSize);
}

/**
*@brief  读取祈祷提醒参数
*@details
*@author        wuhuowang
*@date        2024-01-22
*/
void uteModuleSystemtimeReadReminderConfig(void)
{
    void *file=NULL;
    uint8_t readbuff[4];

    memset(&systemRemind, 0, sizeof(ute_module_systemtime_custom_remind_t));
    memset(&readbuff[0], 0, 4);
    uteModuleSystemtimeReadRemindData();
    readbuff[0] = DEFAULT_APP_CUSTOM_REMIND_SUPPORT;
    readbuff[1] = APP_CUSTOM_REMIND_SETTING_MAX_CNT;
    readbuff[2] = APP_CUSTOM_REMIND_SETTING_DAY_CNT;
    readbuff[3] = APP_CUSTOM_REMIND_SETTING_DAY_SIZE;

    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_REMIND_INFO, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 4);
        uteModuleFilesystemCloseFile(file);
    }
    systemRemind.isSupport = readbuff[0];
    systemRemind.remindTotalCnt = readbuff[1];
}

/**
*@brief  保存祈祷提醒参数
*@details
*@author        wuhuowang
*@date        2024-01-22
*/
void uteModuleSystemtimeSaveReminderConfig(void)
{
    /*! 保存到文件zn.zeng, 2021-08-18  */
    void *file;
    uint8_t writebuff[4];
    writebuff[0] = systemRemind.isSupport;
    writebuff[1] =  systemRemind.remindTotalCnt;
    writebuff[2] = APP_CUSTOM_REMIND_SETTING_DAY_CNT;
    writebuff[3] = APP_CUSTOM_REMIND_SETTING_DAY_SIZE;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_REMIND_INFO, &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writebuff[0], 4);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
*@brief  发送祈祷设置
*@details
*@param
*@author        wuhuowang
*@date        2024-01-04
*/
void uteModuleSystemtimeStartSendCustomRemindData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t sendSize = 0;
    uint16_t sendMaxLenth = sendParam->currSendMtuSize - 3;   //每个包发送多少字节
    uint16_t responseBufferSize = 3 + sendMaxLenth;           //每个包发送的运动数据需要开多大的buffer
    uint8_t *response = uteModulePlatformMemoryAlloc(responseBufferSize);
    memset(response, 0, responseBufferSize);
    response[0] = CMD_CUSTOM_REIMIND_SETTING;
    response[1] = systemRemind.sendCmdType;//查询指令
    response[2] = sendParam->currSendMinIndex;//第几个包
    sendSize = 3;

#if 10
    if (systemRemind.sendDataLenth >= APP_CUSTOM_REMIND_SETTING_MAX_CNT * APP_CUSTOM_REMIND_SETTING_DAY_SIZE)
    {
        //end
        response[2] = 0xFD;
        response[3] = sendParam->crc;
        sendSize = 4;
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, has send cnt = %d", __func__,systemRemind.sendDataLenth);
    }
    else
    {
        uint8_t sendOffset = 0;
        for (uint8_t i = systemRemind.remindIndex; i < APP_CUSTOM_REMIND_SETTING_MAX_CNT * APP_CUSTOM_REMIND_SETTING_DAY_SIZE; i ++)
        {
            if (sendSize + 1 <= sendMaxLenth)
            {
                response[3 + sendOffset] = systemRemindBuf[systemRemind.sendDataLenth ++];
                sendOffset += 1;
                sendSize += 1;
            }
            else
            {
                systemRemind.remindIndex = systemRemind.sendDataLenth;
                UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, systemRemind.remindIndex = %d", __func__,systemRemind.remindIndex);
                break;
            }
        }
    }
#else
    if (systemRemind.sendRemindingIndex == 0)
    {
        // end
        response[2] = 0xFD;
        response[3] = sendParam->crc;
        sendSize = 4;
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, has send alarm cnt = %d", __func__,hasSendAlarmCnt);
    }
    else
    {
        uint8_t sendOffset = 0;
        for (uint8_t i = systemRemind.remindIndex; i < systemRemind.remindTotalCnt; i ++)
        {
            if (sendSize < 20)
            {
                response[3 + sendOffset] = (systemRemind.remindParam[i].year >> 8) & 0xff;
                response[4 + sendOffset] = systemRemind.remindParam[i].year & 0xff;
                response[5 + sendOffset] = systemRemind.remindParam[i].month;
                response[6 + sendOffset] = systemRemind.remindParam[i].day;
                response[7 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[0].type;
                response[8 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[0].isOpen;
                response[9 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[0].hour;
                response[10 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[0].min;
                response[11 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[1].type;
                response[12 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[1].isOpen;
                response[13 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[1].hour;
                response[14 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[1].min;
                response[15 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[2].type;
                response[16 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[2].isOpen;
                response[17 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[2].hour;
                response[18 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[2].min;
                response[19 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[3].type;
                response[20 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[3].isOpen;
                response[21 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[3].hour;
                response[22 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[3].min;
                response[23 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[4].type;
                response[24 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[4].isOpen;
                response[25 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[4].hour;
                response[26 + sendOffset] = systemRemind.remindParam[i].oneDayRemind[4].min;
                systemRemind.remindIndex ++;
                systemRemind.sendRemindingIndex --;
                sendSize += 24;
            }
        }
    }
#endif
    if (response[2] == 0xFD)
    {
        uteApplicationCommonSyncDataTimerStop();
    }
    else
    {
        sendParam->currSendMinIndex ++;
        uteModuleCrc8Bit(&sendParam->crc, &response[2], sendSize - 2);
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s, sendSize = %d", __func__, sendSize);
    uteModuleProfileBle50SendToPhone(&response[0], sendSize);
    uteModulePlatformMemoryFree(response);
}

/**
*@brief  开始发送所有祈祷设置
*@details
*@author        wuhuowang
*@date        2024-01-04
*/
void uteModuleSystemtimeSendAllCustomRemindData(void)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->currSendMinIndex = 0; //用作发送的包数量
    param->currSendFileIndex = 0; //已发送的闹钟数据的长度
    param->currSendMtuSize = uteApplicationCommonGetMtuSize();
    param->currSendFileSize = systemRemind.remindTotalCnt; //有效闹钟个数
    systemRemind.remindIndex = 0;
    systemRemind.sendRemindingIndex = systemRemind.remindTotalCnt;
    systemRemind.sendDataLenth = 0;
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleSystemtimeStartSendCustomRemindData);
    uteModuleSystemTimeGetRemindDataFromWatch();
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,remindTotalCnt = %d", __func__,systemRemind.remindTotalCnt);
}

/**
*@brief  设置发送所有祈祷设置类型
*@details 查询指令或者主动上报指令
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeSetSendCustomRemindType(uint8_t type)
{
    systemRemind.sendCmdType = type;
}

/**
*@brief  修改设置后主动发送所有祈祷设置
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeReportAllCustomRemindData(void)
{
    uteModuleSystemtimeSetSendCustomRemindType(0xAB);
    uteModuleSystemtimeSendAllCustomRemindData();
}

/**
*@brief  设置是否支持祈祷设置功能
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeSetSupportRemindSetting(bool flag)
{
    systemRemind.isSupport = flag;
    uteModuleSystemtimeSaveReminderConfig();
}

/**
*@brief  获取是否支持祈祷设置功能
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
bool uteModuleSystemtimeGetSupportRemindSetting(void)
{
    return systemRemind.isSupport;
}

/**
*@brief  获取祈祷设置提醒数量
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
uint8_t uteModuleSystemtimeGetRemindTotalCnt(void)
{
    return systemRemind.remindTotalCnt;
}

/**
*@brief  获取祈祷设置提醒index
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
uint8_t uteModuleSystemtimeGetRemindIndex(void)
{
    if (systemRemind.isRemindingIndex < APP_CUSTOM_REMIND_SETTING_DAY_CNT)
        return systemRemind.isRemindingIndex;
    else
        return 0;
}

/**
*@brief  设置接收数据长度
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeSetCustomRemindLenth(uint16_t value)
{
    systemRemind.receiveDataLenth = value;
}

/**
*@brief  获取接收数据长度
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
uint16_t uteModuleSystemtimeGetCustomRemindLenth(void)
{
    return systemRemind.receiveDataLenth;
}

/**
*@brief  获取当前时间是否有祈祷设置
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
bool uteModuleSystemtimeGetRemindSettingHaveData(void)
{
    bool ret =false;
    uint8_t i = 0;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    for (i = 0; i < APP_CUSTOM_REMIND_SETTING_MAX_CNT; i ++)
    {
        if (time.year == systemRemind.remindParam[i].year
            && time.month == systemRemind.remindParam[i].month
            && time.day == systemRemind.remindParam[i].day)
        {
            ret = true;
            break;
        }
    }
    return ret;
}

/**
*@brief  解析接收到的数据
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeProcessRemindData(void)
{
    uint8_t path[30];
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(APP_CUSTOM_REMIND_SETTING_MAX_CNT * sizeof(ute_module_systemtime_oneday_remind_t));
    void *file;
    int32_t fileSize = 0;

    memset(&path[0], 0, sizeof(path));
    sprintf((char *)&path[0], "%s/%02d", UTE_MODULE_FILESYSTEM_PROTOCAL_RECEIVE_DIR, 0x04);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_END);
        fileSize = (uint32_t)uteModuleFilesystemGetFileSize(file);
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,fileSize=%d %d", __func__, fileSize, systemAlarms.receiveDataOffset);
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readBuff[0], fileSize);
        uteModuleFilesystemCloseFile(file);
        if (systemRemind.receiveDataLenth == fileSize)
            uteModuleSystemtimeSaveAllRemindData(readBuff, systemRemind.receiveDataLenth, true);
    }
    uteModulePlatformMemoryFree(readBuff);
}

/**
*@brief  保存所有祈祷设置提醒
*@details
*@param[in] uint8_t *receiveData 接收数据
*@param[in] uint16_t length 数据长度
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeSaveAllRemindData(uint8_t *receiveData, uint16_t length, bool saveFlag)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t totalCnt = 0;//个数
    uint16_t numRemindDataLen = 0;

    totalCnt = length / APP_CUSTOM_REMIND_SETTING_DAY_SIZE;
    for (i = 0; i < totalCnt; i ++)
    {
        systemRemind.remindParam[i].year = (receiveData[numRemindDataLen + 0] << 8) | receiveData[numRemindDataLen + 1];
        systemRemind.remindParam[i].month = receiveData[numRemindDataLen + 2];
        systemRemind.remindParam[i].day = receiveData[numRemindDataLen + 3];
        for (j = 0; j < APP_CUSTOM_REMIND_SETTING_DAY_CNT; j ++)
        {
            systemRemind.remindParam[i].oneDayRemind[j].type = receiveData[j * 4 + numRemindDataLen + 4];
            systemRemind.remindParam[i].oneDayRemind[j].isOpen = receiveData[j * 4 + numRemindDataLen + 5];
            systemRemind.remindParam[i].oneDayRemind[j].hour = receiveData[j * 4 + numRemindDataLen + 6];
            systemRemind.remindParam[i].oneDayRemind[j].min = receiveData[j * 4 + numRemindDataLen + 7];
        }
        numRemindDataLen += APP_CUSTOM_REMIND_SETTING_DAY_SIZE;
    }
    if (totalCnt > 0 && saveFlag == true)
    {
        uteModuleSystemTimeSaveRemindData(receiveData, length);
        systemRemind.remindTotalCnt = totalCnt;
        uteModuleSystemtimeSaveReminderConfig();
    }
}

/**
*@brief  获取所有祈祷设置提醒
*@details
*@param[in] ute_module_systemtime_custom_remind_t *data  传入要获取祈祷提醒的指针
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeGetRemindData(ute_module_systemtime_custom_remind_t *data)
{
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(data, &systemRemind, sizeof(ute_module_systemtime_custom_remind_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
}

/**
*@brief   设置祈祷设置提醒
*@details
*@author       wuhuowang
*@date       2024-01-13
*/
void uteModuleSystemtimeSetRemindData(ute_module_systemtime_custom_remind_t *data)
{
    uteModulePlatformTakeMutex(uteModuleSystemtimeMute);
    memcpy(&systemRemind, data, sizeof(ute_module_systemtime_custom_remind_t));
    uteModulePlatformGiveMutex(uteModuleSystemtimeMute);
    uteModuleSystemTimeSaveRemindDataByWatch();
}

/**
*@brief  祈祷提醒函数
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeCustomRemindShow(uint8_t index)
{
    if (!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    if (uteModuleGuiCommonGetCurrentScreenId() == UTE_MOUDLE_SCREENS_OTA_ID)
    {
        return;
    }
#if UTE_BT30_CALL_SUPPORT
    if (uteModuleGuiCommonGetCurrentScreenId() == FUNC_BT_CALL || uteModuleGuiCommonGetCurrentScreenId() == FUNC_BT_RING)
    {
        return;
    }
#endif
    systemRemind.isRemindingIndex = index;
    sys_cb.cover_index = REMIND_COVER_PRAY;
    sys_cb.remind_tag = true;
    //uteDrvMotorStart(500, 500, 2);
#if 0//UTE_MODULE_LOCAL_RINGTONE_ALARM_SUPPORT
    if (!uteModuleLocalRingtoneGetMuteStatus())
    {
        uteModuleLocalRingtonePlayRing(RINGTON_TYPE_ALARM);
    }
#endif

}
/**
*@brief  祈祷提醒回调函数
*@details
*@author        wuhuowang
*@date        2024-01-13
*/
void uteModuleSystemtimeCustomRemindCb(void)
{
    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t activeDay = 0;
    bool isValid = false;
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);

    if (time.sec == 0 && uteModuleSystemtimeGetSupportRemindSetting())
    {
        for (i = 0; i < APP_CUSTOM_REMIND_SETTING_MAX_CNT; i ++)
        {
            if (time.year == systemRemind.remindParam[i].year
                && time.month == systemRemind.remindParam[i].month
                && time.day == systemRemind.remindParam[i].day)
            {
                activeDay = i;
                isValid = true;
                break;
            }
        }
        if (isValid == true)
        {
            for (j = 0; j < APP_CUSTOM_REMIND_SETTING_DAY_CNT; j ++)
            {
                if (systemRemind.remindParam[activeDay].oneDayRemind[j].isOpen
                    && systemRemind.remindParam[activeDay].oneDayRemind[j].hour == time.hour
                    && systemRemind.remindParam[activeDay].oneDayRemind[j].min == time.min)
                {
                    uteModuleSystemtimeCustomRemindShow(systemRemind.remindParam[activeDay].oneDayRemind[j].type);
                    break;
                }
            }
        }
    }
}
#endif
