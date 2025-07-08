/**
*@file
*@brief        时区设置模块
*@details
*@author       黄鹤
*@date       2022-03-29
*@version      v1.0
*/
#ifndef _UTE_MODULE_TIME_ZONE_SETTING_H_
#define _UTE_MODULE_TIME_ZONE_SETTING_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#include "ute_module_systemtime.h"
/*!用来保存时区数据*/
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_TIME_ZONE_DATA "systemparam/timeZoneData"
#define UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH 100

#if UTE_MODULE_TIME_ZONE_SETTING_SUPPORT
/*! 时区设置数据结构 黄鹤, 2022-03-29  */
typedef struct
{
    uint8_t countryNameLen; /*！城市名长度 */
    uint8_t countryNameU8[UTE_MODULE_TIMEZONE_DATA_NAME_LENGTH];/*！城市名utf-8编码 */
    int8_t timezone;/*！ 时区整数，带正负号，东时区为正，西时区为负 */
    uint8_t timezone_dec;/*！时区小数*10 */
} ute_module_timezonesetting_one_timezone_t;//struct len=103
typedef struct
{
    uint8_t receiveCrc;
    uint8_t number;
    ute_module_timezonesetting_one_timezone_t info[UTE_MODULE_WORLD_CLOCK_TOTAL_CNT];
    uint8_t reserve[3];//预留变量,用于4对齐
} ute_module_timezonesetting_data_t;//struct len=520

void uteModuleTimezoneInit(void);
void uteModuleTimeZoneSettingCmd(uint8_t *receive, uint8_t length);
void uteModuleTimeZoneSettingGetData(ute_module_timezonesetting_data_t *data);
void uteModuleTimeZoneSettingSaveData(ute_module_timezonesetting_data_t *data);
bool uteModuleTimeZoneSettingIsLeapYear(uint16_t year);
uint8_t uteModuleTimeZoneSettingGetNum(void);
#endif
#endif //_UTE_MODULE_TIME_ZONE_SETTING_H_

