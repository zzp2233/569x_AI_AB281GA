/**
*@file
*@brief     系统时间头文件
*@details
*@author        zn.zeng
*@date        Jun 29, 2021
*@version       v1.0
*/

#ifndef _UTE_MODULE_SYSTEMTIME_H_
#define _UTE_MODULE_SYSTEMTIME_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
/*!写入时间格式参数 zn.zeng, 2021-08-18  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_FORMAT  "systemparam/timeFormat"
/*!写入闹钟信息参数目录 zn.zeng, 2021-08-21  */
#define UTE_MODULE_FILESYSTEM_ALARMINFO_DIR "alarmInfo"
/*!用来保存最后一个闹钟被删除的时间戳*/
#define UTE_MODULE_FILESYSTEM_FINAL_ALARM_DELETE_TIME "delalarmtime"
/*!写入闹钟信息参数目录 zn.zeng, 2021-08-21  */
#define UTE_MODULE_FILESYSTEM_ALARM_REPEAT_DIR "alarmRepeat"
/*! 一个闹钟的最大数据长度，单位byte 黄鹤 ，2022-04-02 */
#ifndef UTE_MODULE_SYSTEMTIME_ALARM_MAX_NUMBER
#define UTE_MODULE_SYSTEMTIME_ALARM_MAX_NUMBER 115
#endif
#if UTE_MODULE_SCREENS_LOCAL_ALARM_LABEL_SUPPORT
#define UTE_MODULE_SYSTEMTIME_ALARM_SIZE 123
#else
#define UTE_MODULE_SYSTEMTIME_ALARM_SIZE 15
#endif
#if APP_MODULE_RECEIVE_SAVE_TO_FILESYS_SUPPORT
/*!写入从APP接收内容目录 wuhuowang, 2022-12-16  */
#define UTE_MODULE_FILESYSTEM_PROTOCAL_RECEIVE_DIR "protocolReceive"
#endif
#if APP_CUSTOM_REMIND_SETTING_SUPPORT
/*!写入祈祷提醒设置 wuhuowang, 2024-01-04  */
#define UTE_MODULE_FILESYSTEM_CUSTOM_REMIND_DIR "customRemind"
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_REMIND_INFO  "systemparam/reminder"
#endif

/**
 * @brief       系统时间数据结构
 * @details 系统时间数据结构
 */
typedef struct
{
    uint8_t month;/*!系统时间月份，数值为1~12月 */
    uint8_t day;/*!系统时间，一个月内的天，数值为1~31 */
    uint8_t hour;/*!系统时间，24小时格式的小时格式，数值为0~23 */
    uint8_t min;/*!系统时间，分钟，数值为0~59 */
    uint8_t sec;/*!系统时间，秒钟，数值为0~59 */
    uint8_t week;/*!系统时间，星期，数值为0~6，星期天开始算，星期天为0 */
    uint16_t year;/*!系统时间，年，数值为2000以上 */
    int8_t zone;/*!系统时间，当前时区，数值为   +-12 ，对应东12个时区，西12个时区，东时区为正，西时区为负*/
    bool is12HourTime;
    bool isDistanceMi;
    uint16_t languageId;
    uint16_t AppSetlanguageId;
    bool isSettingTime;
    bool isWatchSetLangage;/*手环端设置语言*/
    bool change12HourTime;
} ute_module_systemtime_time_t;
typedef enum
{
    NULL_LABEL=0x000,
    ALARM_BUTTON_CLICK=0x069,                               //闹钟 0x69
    WAKING_BUTTON_CLICK=0x132,                              //起床 0x132
    MEETING_BUTTON_CLICK=0x133,                             //开会 0x133
    APPOINTMENT_BUTTON_CLICK=0x134,                         //约会 0x134
    DINNER_BUTTON_CLICK=0x135,                              //吃饭 0x135
} ALARM_LABEL_COUNT_T;
#if APP_MODULE_ALARM_LABEL_SET_SUPPORT
typedef enum
{
    LABEL_TYPE_ALARM = 0x00,                               //闹钟
    LABEL_TYPE_WAKEUP = 0x01,                              //起床
    LABEL_TYPE_MEETING = 0x02,                             //开会
    LABEL_TYPE_APPOINTMENT = 0x03,                         //约会
    LABEL_TYPE_DINNER = 0x04,                              //吃饭
} ALARM_LABEL_TYPE_T;
#endif

#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
typedef struct
{
    bool isRepeatRemindOpen;    //重复提醒开关
    uint8_t repeatRemindHour;
    uint8_t repeatRemindMin;
    uint8_t repeatRemindTimes;  //重复提醒剩余次数
} ute_module_systemtime_one_alarm_repeat_t;
#endif
/**
 * @brief       闹钟数据结构
 * @details 包括 年，月，日，时，分，秒，是否是重复闹钟，闹钟持续时长，闹钟标签
 */
typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t weekDay;/*!有效星期，0x01，0x02，0x04，0x08，0x10，0x20，0x40，0x7f分别代表星期天，星期一，星期二，星期三，星期四，星期五，星期六以及每天（0x7f）有效 */
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
    bool isSingle;/*!是否是单次闹钟，是单次闹钟的时候，weekDay参数无效，是重复闹钟的时候 year，month，day参数无效*/
    uint8_t durationTimeSec;/*!闹钟持续时长 */
    bool isOpen;
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
    ute_module_systemtime_one_alarm_repeat_t alarmRepeat;
#endif
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
    uint8_t jarLevel;
    uint8_t labelLenth;/*! 标签长度*/
    uint8_t label[101];/*!标签内容编码数据*/
    uint16_t setYear;/*! 设置闹钟的时间戳，时间戳：年 */
    uint8_t setMonth;/*! 设置闹钟的时间戳，时间戳：月 */
    uint8_t setDay;  /*! 设置闹钟的时间戳，时间戳：日 */
    uint8_t setHour; /*! 设置闹钟的时间戳，时间戳：时 */
    uint8_t setMin;  /*! 设置闹钟的时间戳，时间戳：分 */
    uint8_t setSec;  /*! 设置闹钟的时间戳，时间戳：秒 */
    uint8_t index;/*! 闹钟序号/索引：1,2,3.....代表第几个闹钟*/
    uint16_t alarmLabel;/*! 有效标签，NULL_LABEL、ALARM_BUTTON_CLICK、WAKING_BUTTON_CLICK、MEETING_BUTTON_CLICK、APPOINTMENT_BUTTON_CLICK、DINNER_BUTTON_CLICK，分别对应：空 闹钟，起床，会议，约会，聚餐  */
#endif
    bool isFinish;/*! 是否完成创建闹钟 */
} ute_module_systemtime_one_alarm_t;

typedef struct
{
    ute_module_systemtime_one_alarm_t alarmParam[SYSTEM_TIME_ALARMS_MAX_CNT];
    uint8_t alarmDisplayIndex;      //闹钟显示索引
    uint8_t alarmTotalCnt;          //闹钟总个数
    uint8_t isRemindingIndex;   /*!正在闹铃中的闹钟*/
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
    uint8_t cancelCurrAlarmIndex; //取消当前闹钟重复提醒
    uint8_t cancelRepeatAlarmIndex;//取消重复闹钟信息提醒
#endif
#if UTE_MODULE_LOCAL_ALARM_SUPPORT
    bool isAlarmClockRing;
#if !APP_MODULE_RECEIVE_SAVE_TO_FILESYS_SUPPORT
    uint8_t receiveDataBuff[SYSTEM_TIME_ALARMS_MAX_CNT*UTE_MODULE_SYSTEMTIME_ALARM_MAX_NUMBER];
#endif
    uint16_t receiveDataOffset;
    uint8_t receiveCrc;
#endif
} ute_module_systemtime_alarm_t;

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t min;
    uint8_t sec;
} ute_module_systemtime_measure_time_t;

#if APP_CUSTOM_REMIND_SETTING_SUPPORT
typedef struct
{
    uint8_t hour;
    uint8_t min;
    uint8_t type;/*提醒类型 */
    bool isOpen;
} ute_module_systemtime_one_remind_t;

typedef struct
{
    uint16_t year;
    uint8_t month;
    uint8_t day;
    ute_module_systemtime_one_remind_t oneDayRemind[APP_CUSTOM_REMIND_SETTING_DAY_CNT];
} ute_module_systemtime_oneday_remind_t;

typedef struct
{
    ute_module_systemtime_oneday_remind_t remindParam[APP_CUSTOM_REMIND_SETTING_MAX_CNT];
    bool isSupport; //是否支持该功能
    uint8_t remindIndex;      //显示索引
    uint8_t remindTotalCnt;          //总个数
    uint8_t isRemindingIndex;   /*!正在提醒中的提醒*/
    uint8_t sendRemindingIndex;
    uint16_t receiveDataLenth;
    uint16_t sendDataLenth;
    uint8_t receiveCrc;
    uint8_t sendCmdType;
} ute_module_systemtime_custom_remind_t;
#endif

typedef void (*ute_module_systemtime_reg_func_t)(void);

/**
 * @brief   注册每秒数据结构
 * @details
 */
typedef struct
{
    uint8_t regCnt;/*!已注册数量 zn.zeng, 2021-07-12  */
    ute_module_systemtime_reg_func_t function[UTE_MODULE_SYSTEMTEIM_REGISTER_MAX_CNT];/*! 注册的函数指针zn.zeng, 2021-07-12  */
} ute_module_systemtime_register_t;
enum
{
    SETTING_TIME = 0,
    SETTING_DATE,
};
void uteModuleSystemtimeInit(void);
void uteModuleSystemtimeSetTime(ute_module_systemtime_time_t set);
void uteModuleSystemtimeSetAlarm(ute_module_systemtime_one_alarm_t set,uint8_t index);
void uteModuleSystemtimeGetAlarm(ute_module_systemtime_one_alarm_t *set, uint8_t index);

void uteModuleSystemtimeSetAlarmDisplayIndex(uint8_t alarmDisplayIndex);
uint8_t uteModuleSystemtimeGetAlarmDisplayIndex(void);
void uteModuleSystemtimeSetAlarmTotalCnt(uint8_t alarmDisplayIndex);
uint8_t uteModuleSystemtimeGetAlarmTotalCnt(void);
void uteModuleSystemtimeDeleteAlarm(uint8_t index);
void uteModuleSystemtimeSaveAllAlarmInfo(uint8_t *AlarmData,uint16_t length);
void uteModuleSystemtimeStartSendAllAlarmData(void);
void uteModuleSystemtimeSaveAlarmDelTime(void);
void uteModuleSystemtimeReadAlarmDelTime(void);
void uteModuleSystemtimeUpdataAllAlarm(void);

void uteModuleSystemtimeSecondCb(void);
uint8_t uteModuleSystemtimeAlarmGetRemindingIndex(void);
uint8_t uteModuleSystemtimeGetWeek(uint16_t year, uint8_t month, uint8_t day);
void uteModuleSystemtimeGetTime(ute_module_systemtime_time_t *time);
void uteModuleSystemtimeInputDateCalDay(ute_module_systemtime_time_t *time,uint16_t addDay);
void uteModuleSystemtimeInputDateIncreaseDecreaseDay(ute_module_systemtime_time_t *time, int day);
void uteModuleSystemtimeFixDateIncreaseDecreaseDay(ute_module_systemtime_time_t *time, int day);
int uteModuleSystemtimeGetDiffDay(uint16_t year_start, uint8_t month_start, uint8_t day_start, uint16_t year_end, uint8_t month_end, uint8_t day_end);
void uteModuleSystemtimeRegisterSecond(void* function);
void uteModuleSystemtimeRegisterSecondCb(uint32_t param);
bool uteModuleSystemtimeIsNight(void);
bool uteModuleSystemtimeIsSleepTime(void);
void uteModuleSystemtimeSaveAlarmInfo(ute_module_systemtime_one_alarm_t value,uint8_t index);
void uteModuleSystemtimeSetTimeFormat(bool is12Hour,bool isMi);
void uteModuleSystemtimeSetTimeZone(int8_t zone);
void uteModuleSystemtimeSetLanguage(uint16_t id);
void uteModuleSystemtimeAPPSetLanguage(uint16_t id);
void uteModuleSystemtimeReadConfig(void);
void uteModuleSystemtimeSaveIsWatchSetLangage(bool isWatchSetLangage);
uint16_t uteModuleSystemtimeReadLanguage(void);
bool uteModuleSystemtimeCompareLanguage(uint16_t languageId);
bool uteModuleSystemtimeGetDistanceMiType(void);
void uteModuleSystemtimeSaveTimeInfo(void);
void uteModuleSystemtimeResetChange12HourTime(void);
bool uteModuleSystemtimeWhetherChange12HourTime(void);
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
void uteModuleSystemtimeReadRepeatAlarmConfig(void);
void uteModuleSystemtimeSaveRepeatAlarmConfig(ute_module_systemtime_one_alarm_repeat_t value,uint8_t index);
void uteModuleSystemtimeAlarmAddRepeatRemindInfo(uint8_t i,bool isCurrAlarm);
void uteModuleSystemtimeAlarmClearRepeatRemindInfo(uint8_t currIndex,uint8_t repeatIndex);
uint8_t uteModuleSystemtimeAlarmGetCancelCurrAlarmIndex(void);
void uteModuleSystemtimeAlarmSetCancelCurrAlarmIndex(uint8_t cancelCurrAlarmIndex);
uint8_t uteModuleSystemtimeAlarmGetCancelRepeatAlarmIndex(void);
void uteModuleSystemtimeAlarmSetCancelRepeatAlarmIndex(uint8_t cancelRepeatAlarmIndex);
uint8_t uteModuleSystemtimeAlarmGetRepeatRemindTimes(uint8_t index);
#endif
void uteModuleSystemtimeAlarmCmd(uint8_t *receive, uint8_t length);
void uteModuleSystemtime12HSwitchStatus(bool isopen);
bool uteModuleSystemtime12HOn(void);
void uteModuleSystemTimeLocalTimeSetStatus(uint8_t status);
uint8_t uteModuleSystemTimeLocalTimeGetStatus(void);
void uteModuleSystemTimeLocalSetDate(uint16_t year,uint8_t month,uint8_t day);
void uteModuleSystemTimeLocalSetHourMin(uint8_t hour,uint8_t min);
#if APP_MODULE_RECEIVE_SAVE_TO_FILESYS_SUPPORT
void uteModuleSystemTimeSaveReceiveData(uint8_t *data, uint8_t length, uint8_t index, bool isfirstFlag);
void uteModuleSystemTimeDeleteReceiveData(void);
#endif
#if APP_CUSTOM_REMIND_SETTING_SUPPORT
void uteModuleSystemTimeSaveRemindData(uint8_t *data, uint16_t length);
void uteModuleSystemTimeDeleteRemindData(void);
void uteModuleSystemtimeReadReminderConfig(void);
void uteModuleSystemtimeSaveReminderConfig(void);
void uteModuleSystemtimeCustomRemindInitData(void);
void uteModuleSystemtimeStartSendCustomRemindData(void);
void uteModuleSystemtimeSendAllCustomRemindData(void);
void uteModuleSystemtimeSetSendCustomRemindType(uint8_t type);
void uteModuleSystemtimeReportAllCustomRemindData(void);
void uteModuleSystemtimeSetSupportRemindSetting(bool flag);
bool uteModuleSystemtimeGetSupportRemindSetting(void);
uint8_t uteModuleSystemtimeGetRemindTotalCnt(void);
void uteModuleSystemtimeSetCustomRemindLenth(uint16_t value);
uint16_t uteModuleSystemtimeGetCustomRemindLenth(void);
void uteModuleSystemtimeProcessRemindData(void);
void uteModuleSystemtimeSaveAllRemindData(uint8_t *receiveData, uint16_t length, bool saveFlag);
void uteModuleSystemtimeGetRemindData(ute_module_systemtime_custom_remind_t *data);
void uteModuleSystemtimeSetRemindData(ute_module_systemtime_custom_remind_t *data);
void uteModuleSystemtimeCustomRemindCb(void);
uint8_t uteModuleSystemtimeGetRemindIndex(void);
void uteModuleSystemTimeResetRemindData(void);
bool uteModuleSystemtimeGetRemindSettingHaveData(void);
#endif

///适配中科平台接口（闹钟）
void uteModuleSystemtimeEnableAlarm(uint8_t index, bool en);
uint8_t uteModuleSystemtimeGetAlarmHour(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmMin(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmSec(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmCycle(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmEnableState(uint8_t index);
uint8_t uteModuleSystemtimeGetAlarmMaxCnt(void);
uint8_t uteModuleSystemtimeGetAlarmIsFree(uint8_t index);
uint8_t uteModuleSystemtimeAlarmEdit(uint8_t index, bool enable, uint8_t cycle, uint8_t alarm_hour, uint8_t alarm_minute, uint8_t motor_mode, uint8_t remind_later);
uint8_t uteModuleSystemtimeGetAlarmRingIndex(void);
void uteModuleSystemtimeSetAlarmRingIndex(uint8_t index);
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
void ute_moduleSystemtimeAlarmRepeatRemindHandle(ute_module_systemtime_one_alarm_t* alarm,int index);
#endif //UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
#if UTE_MODULE_SCREENS_LOCAL_ALARM_LABEL_SUPPORT
void ute_moduleSystemtimeAlarmSetLabel(uint8_t index, uint16_t label);
uint16_t ute_moduleSystemtimeAlarmGetLabel(uint8_t index);
#endif
uint8_t uteModuleSystemtimeGetAlarmLableDataLen(uint8_t index);
void uteModuleSystemtimeGetAlarmLableData(char *labe, uint8_t *len, uint8_t index);
int uteModuleSystemtimeAlarmGetNextIndex(void);
#endif //_UTE_MODULE_SYSTEMTIME_H_

