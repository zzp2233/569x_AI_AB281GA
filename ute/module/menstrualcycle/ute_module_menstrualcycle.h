/**
*@file
*@brief        女性生理周期
*@details
*@author       dengli.lu
*@date       2021-11-24
*@version      v1.0
*/
#ifndef _UTE_MODULE_MENSTRUALCYCLE_H_
#define _UTE_MODULE_MENSTRUALCYCLE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_application_common.h"
/* 女性周期数据保存 zn.zeng 2022-01-27*/
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_MENSTRUALCYCLE_INFO  "systemparam/menstrualcycleInfo"

#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
typedef struct
{
    uint16_t startYear;
    uint8_t startMonth;
    uint8_t startDay;
    uint16_t endYear;
    uint8_t endMonth;
    uint8_t endDay;
} ute_menstrual_cycle_time_t;
typedef struct
{
    ute_menstrual_cycle_time_t wholeCycle; /*!完整的经期包含：月经期-安全期-排卵期(排卵日)-安全期*/
    ute_menstrual_cycle_time_t menstruationCycle;
    ute_menstrual_cycle_time_t safety1Cycle;
    ute_menstrual_cycle_time_t ovulatoryCycle;
    ute_menstrual_cycle_time_t ovulatoryDay;
    ute_menstrual_cycle_time_t safety2Cycle;
} ute_menstrual_cycle_each_phase_date_t;
#endif

typedef struct
{
    bool reminderSwitch;
    uint8_t lastTimestamp[4]; //yyyy-mm-dd
    uint8_t keepDays;
    uint8_t cycleDays;
#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
    bool isInitSet;
    bool isMenstrualRemindOpen;
    bool isOvulationRemindOpen;
    uint8_t menstrualRemindTime[3]; //(days in advance)-hour-min  d-hh-mm
    uint8_t ovulationRemindTime[3];
    ute_menstrual_cycle_each_phase_date_t eachPhaseDate;
    bool isMenstrualEndRemindOpen;
    bool isOvulationEndRemindOpen;
#endif
} ute_menstrual_cycle_param_t;

#if UTE_MODULE_LOCAL_SET_MENSTRUAL_CYCLE_SUPPORT
enum
{
    MENSTRUAL_SET_IDLE = 0,
    MENSTRUAL_SET_MAIN,   //周期、经期等设置界面
    MENSTRUAL_SET_REMIND, //提醒设置界面
    MENSTRUAL_SET_KEEPDAY,//经期长度
    MENSTRUAL_SET_LAST,   //上次经期时间
    MENSTRUAL_SET_CYCLEDAY,//周期
    MENSTRUAL_SET_TIME_MENS,//月经期提醒时间
    MENSTRUAL_SET_ADVANCE_MENS,//月经期提前提醒天数
    MENSTRUAL_SET_TIME_OVU,//排卵期提醒时间
    MENSTRUAL_SET_ADVANCE_OVU,//排卵期提前提醒天数
};

enum
{
    REMIND_IDLE = 0,
    REMIND_MENSTRUAL,
    REMIND_OVULATION,
};

void uteModuleMenstrualCycleSetTempParam(ute_menstrual_cycle_param_t *param);
void uteModuleMenstrualCycleSetSettingStatus(uint8_t setStatus);
uint8_t uteModuleMenstrualCycleGetSettingStatus(void);
void uteModuleMenstrualCycleGetParam(ute_menstrual_cycle_param_t *param);
uint8_t uteModuleMenstrualCycleWeekDay(int target_year, int target_month);
uint8_t uteModuleMenstrualCycleGetStatusByTime(uint16_t year, uint8_t month, uint8_t day);
int8_t uteModuleMenstrualCycleGetRemaindays(void);
uint8_t uteModuleMenstrualCycleJudgeMonthDay(uint16_t year,uint8_t month);
void uteModuleMenstrualCycleRemindCb(void);
uint8_t uteModuleMenstrualCycleGetRemindStatus(void);
uint16_t uteModuleMenstrualCycledaysCaculate(uint16_t year, uint8_t month, uint8_t day);
uint16_t uteModuleMenstrualCycleCalDateFromDays(int16_t totalday, uint16_t year, uint8_t *month, uint8_t *day);
bool uteModuleMenstrualCycleCalEachPhaseDate(uint16_t year, uint8_t month, uint8_t day, ute_menstrual_cycle_each_phase_date_t *eachPhaseDate,int8_t *dayLeft);
#endif

void uteModuleMenstrualCycleInit(void);
void uteModuleMenstrualCycleSetParam(ute_menstrual_cycle_param_t param);
bool uteModuleMenstrualCycleGetStatus(uint8_t *menstrualStatus);
bool uteModuleMenstrualCycleIsOpen(void);
int uteModuleMenstrualCycleGetDiffDay(int year_start, int month_start, int day_start, int year_end, int month_end, int day_end);

#endif
