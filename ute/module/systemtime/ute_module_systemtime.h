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

void uteModuleSystemtimeSecondCb(void);
uint8_t uteModuleSystemtimeGetWeek(uint16_t year, uint8_t month, uint8_t day);
void uteModuleSystemtimeGetTime(ute_module_systemtime_time_t *time);
void uteModuleSystemtimeInputDateCalDay(ute_module_systemtime_time_t *time,uint16_t addDay);
void uteModuleSystemtimeInputDateIncreaseDecreaseDay(ute_module_systemtime_time_t *time, int day);
void uteModuleSystemtimeRegisterSecond(void* function);
void uteModuleSystemtimeRegisterSecondCb(uint32_t param);
bool uteModuleSystemtimeIsNight(void);
bool uteModuleSystemtimeIsSleepTime(void);
void uteModuleSystemtimeSetTimeFormat(bool is12Hour,bool isMi);
void uteModuleSystemtimeSetTimeZone(int8_t zone);
void uteModuleSystemtimeSetLanguage(uint16_t id);
void uteModuleSystemtimeAPPSetLanguage(uint16_t id);
void uteModuleSystemtimeReadConfig(void);
uint16_t uteModuleSystemtimeReadLanguage(void);
bool uteModuleSystemtimeCompareLanguage(uint16_t languageId);
bool uteModuleSystemtimeGetDistanceMiType(void);
void uteModuleSystemtimeSaveTimeInfo(void);
void uteModuleSystemtimeResetChange12HourTime(void);
bool uteModuleSystemtimeWhetherChange12HourTime(void);
void uteModuleSystemtime12HSwitchStatus(bool isopen);
bool uteModuleSystemtime12HOn(void);
void uteModuleSystemTimeLocalTimeSetStatus(uint8_t status);
uint8_t uteModuleSystemTimeLocalTimeGetStatus(void);
void uteModuleSystemTimeLocalSetDate(uint16_t year,uint8_t month,uint8_t day);
void uteModuleSystemTimeLocalSetHourMin(uint8_t hour,uint8_t min);

#endif //_UTE_MODULE_SYSTEMTIME_H_

