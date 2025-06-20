#ifndef _UTE_DRV_LED_H_
#define _UTE_DRV_LED_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"

#if UTE_DRV_LED_SUPPORT

#define UTE_MODULE_FILESYSTEM_LED_BRIGHTNESS_LEVEL_INFO "systemparam/ledLevel"

/*! LED数据结构 wang.luo, 2025-06-18  */
typedef struct
{
    bool isOpen;
    uint8_t brightnessLevel;
} ute_drv_led_t;

void uteDrvLedInit(void);
void uteDrvLedBrightnessLevelReadConfig(void);
bool uteDrvLedIsOpen(void);
void uteDrvLedEnable(void);
void uteDrvLedDisable(void);
void uteDrvLedSetBrightnessLevel(uint8_t brightnessLevel);
uint8_t uteDrvLedGetBrightnessLevel(void);
void uteDrvLedSetTempBrightnessLevel(uint8_t brightnessLevel);

#endif

#endif//_UTE_DRV_MOTOR_H_
