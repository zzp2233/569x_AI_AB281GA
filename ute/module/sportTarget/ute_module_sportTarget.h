/**
*@file
*@brief        运动目标设置处理模块
*@details
*@author       dengli.lu
*@date       2022-03-08
*@version      v1.0
*/
#ifndef _UTE_MODULE_SPORTTARGET_H_
#define _UTE_MODULE_SPORTTARGET_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"

/*运动目标设置保存*/
#define UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_DATA "systemparam/moreSportTarget"
#if APP_MODULE_SCREENS_TARGET_MORE_SUPPORT
#define UTE_MODULE_FILESYSTEM_MORE_SPORTS_TARGET_SET_DATA "systemparam/SportTargetSet"
#endif

typedef struct
{
    uint32_t sportDistanceTargeCnt;//米
    bool isSportDistanceTargetNotify;//目标距离提醒
    bool isSportDistanceTargetHasNotify;//目标时距离已经提醒过
    uint32_t sportTimeTargetSec;
    bool isSportTimeTargetNotify;//目标时间提醒
    bool isSportTimeTargetHasNotify;//目标时间已经提醒过
    uint16_t sportKcalTarget;
    bool isSportKcalTargetNotify;
    bool isSportKcalTargetHasNotify;
    bool isSportOneHourNotify;//运动一小时提醒
    bool isSportOneHourHasNotify;
    bool isSportOneKmNotify;//运动一千米提醒
    bool isSportOneKmHasNotify;
    uint8_t isNoTarget;//无目标
    uint8_t editStatus;
    uint8_t showType;
    uint8_t showIndex;
} ute_module_sport_target_data_t;

#if APP_MODULE_SCREENS_TARGET_MORE_SUPPORT
typedef struct
{
    uint32_t sportDistanceTargeCnt;//米
    uint32_t sportTimeTargetSec;
    uint16_t sportKcalTarget;
    uint8_t isNoTarget;//无目标
    uint8_t showType;
} ute_module_sport_target_set_data_t;
#endif

typedef enum
{
    SPORT_TARGET_IDLE = 0,    //显示运动目标开始界面
    SPORT_TARGET_SELECT = 1,            //显示运动目标选择界面
    SPORT_TARGET_DISTANCE_SET = 2,      //显示运动目标距离设置界面
    SPORT_TARGET_TIME_SET = 3,          //显示运动目标时间设置界面
    SPORT_TARGET_KCAL_SET = 4,          //显示运动目标卡路里设置界面
    SPORT_TARGET_HEART_INFO = 5,          //显示佩戴测试心率
    SPORT_TARGET_START = 6,          //开始运动
} sport_target_edit_status; //用于判断目前显示哪个状态

void uteModuleSportTargetInit(void);
void uteModuleSportTargetReadConfig(void);

void uteModuleSportTargetSetDistanceGoal(uint32_t sportDistanceTargeCnt);
uint32_t uteModuleSportTargetGetDistanceGoal(void);
void uteModuleSportTargetSetDistanceGoalNotify(bool isSportDistanceTargetNotify);
bool uteModuleSportTargetGetDistanceGoalNotify(void);
void uteModuleSportTargetSetDistanceGoalHasNotify(bool isSportDistanceTargetHasNotify);
bool uteModuleSportTargetGetDistanceGoalHasNotify(void);

void uteModuleSportTargetSetTimeGoal(uint32_t sportTimeTargetSec);
uint32_t uteModuleSportTargetGetTimeGoal(void);
void uteModuleSportTargetSetTimeGoalNotify(bool isSportDistanceTargetNotify);
bool uteModuleSportTargetGetTimeGoalNotify(void);
void uteModuleSportTargetSetTimeGoalHasNotify(bool isSportTimeTargetHasNotify);
bool uteModuleSportTargetGetTimeGoalHasNotify(void);

void uteModuleSportTargetSetKcalGoal(uint16_t sportKcalTarget);
uint16_t uteModuleSportTargetGetKcalGoal(void);
void uteModuleSportTargetSetKcalGoalNotify(bool isSportDistanceTargetNotify);
bool uteModuleSportTargetGetKcalGoalNotify(void);
void uteModuleSportTargetSetKcalGoalHasNotify(bool isSportKcalTargetHasNotify);
bool uteModuleSportTargetGetKcalGoalHasNotify(void);

void uteModuleSportTargetSetOneHourNotify(bool isSportOneHourNotify);
bool uteModuleSportTargetGetOneHourNotify(void);
void uteModuleSportTargetSetOneHourHasNotify(bool isSportOneHourHasNotify);
bool uteModuleSportTargetGetOneHourHasNotify(void);
void uteModuleSportTargetSetOneKmNotify(bool isSportOneKmNotify);
bool uteModuleSportTargetGetOneKmNotify(void);
void uteModuleSportTargetSetOneKmHasNotify(bool isSportOneKmHasNotify);
bool uteModuleSportTargetGetOneKmHasNotify(void);
void uteModuleSportTargetSetIsNoTarget(uint8_t isNoTarget);
uint8_t uteModuleSportTargetGetIsNoTarget(void);
void uteModuleSportTargetSetEditStatus(uint8_t status);
uint8_t uteModuleSportTargetGetEditStatus(void);
void uteModuleSportTargetSetShowType(uint8_t type);
uint8_t uteModuleSportTargetGetShowType(void);
#if APP_MODULE_SCREENS_TARGET_MORE_SUPPORT
void uteModuleSportTargetMoreReadConfig(void);
void uteModuleSportTargetMoreSaveData(bool reset);
void uteModuleSportTargetMoreSetMoreFromIndex(uint8_t index);
void uteModuleSportTargetMoreGetMoreFromIndex(uint8_t index);
void uteModuleSportTargetSetShowIndex(uint8_t index);
uint8_t uteModuleSportTargetGetShowIndex(void);
#endif

#endif
