/**
*@file
*@brief        船运模式
*@details
*@author       zn.zeng
*@date       2022-08-02
*@version      v1.0
*/
#ifndef _UTE_MODULE_SHIP_MODE_H_
#define _UTE_MODULE_SHIP_MODE_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
#if UTE_MODULE_SHIP_MODE_SUPPORT
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_SHIP_MODE  "systemparam/shipMode"
typedef enum
{
    SHIP_NORMAL_MODE,
    SHIP_SHIP_MODE,
} ute_module_ship_mode_t;
typedef struct
{
    ute_module_ship_mode_t mode;
    uint16_t berforeSecondCnt;
} ute_module_ship_mode_data_t;
void uteModuleShipModeInit(void);
void uteModuleShipModeEnter(void);
bool uteModuleShipModeIsShip(void);
void uteModuleShipModeExit(void);
void uteModuleShipModeEverySecond(void);
#endif
#endif //_UTE_MODULE_SHIP_MODE_H_

