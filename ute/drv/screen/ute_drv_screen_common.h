/**
*@file
*@brief     屏驱动文件
*@details    屏驱动公共层文件
*@author        zn.zeng
*@date        2021-09-06
*@version       v1.0
*/


#ifndef _UTE_DRV_SCREEN_COMMON_H_
#define _UTE_DRV_SCREEN_COMMON_H_
#include "ute_project_config.h"
#include "ute_module_platform.h"

typedef void (*ute_drv_screen_common_init_t)(void);
typedef void (*ute_drv_screen_common_power_on_t)(void);
typedef void (*ute_drv_screen_common_power_off_t)(void);
typedef void (*ute_drv_screen_common_set_window_t)(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
typedef void (*ute_drv_screen_common_set_BackLight_t)(uint8_t backLightPercent);
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
typedef void (*ute_drv_screen_common_idle_mode_t)(bool enable);
#endif

typedef struct
{
    const char*name;
    ute_drv_screen_common_init_t init;
    ute_drv_screen_common_power_on_t powerOn;
    ute_drv_screen_common_power_off_t powerOff;
    ute_drv_screen_common_set_window_t setWindow;
    ute_drv_screen_common_set_BackLight_t setBackLight;
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
    ute_drv_screen_common_idle_mode_t idleMode;
    ute_drv_screen_common_set_BackLight_t setBackLightIdleMode;
#endif
    uint8_t refreshRate;
    uint16_t startLine;
    uint16_t gramMaxLine;
} ute_drv_screen_common_config_t;

typedef struct
{
    uint8_t currBackLightPercent;
    bool isInit;
} ute_drv_screen_common_data_t;

void uteDrvScreenCommonInit(void);
bool uteDrvScreenCommonIsInit(void);
void uteDrvScreenCommonSetResetPin(bool isHeight);
void uteDrvScreenCommonSetPowerEnable(bool isEnable);
void uteDrvScreenCommonSetPower(bool isPoweron);
void uteDrvScreenCommonSetWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd);
void uteDrvScreenCommonClearWindow(uint16_t xStart, uint16_t yStart, uint16_t xEnd, uint16_t yEnd,uint16_t color);
void uteDrvScreenCommonOpenBacklight(uint8_t percent);
#if UTE_MODULE_SCREENS_SCREEN_SAVER_SUPPORT
void uteDrvScreenCommonSetBacklightInIdleMode(void);
void uteDrvScreenCommonIdleMode(bool enable);
#endif
void uteDrvScreenCommonDisplayOff(void);
uint32_t uteDrvScreenCommonReadId(void);
#if UTE_DRV_QSPI_FOR_SCREEN_SUPPORT
void uteDrvScreenCommonGc9c01QspiWriteCmdParams(uint8_t cmd,uint8_t *data,uint8_t size);
void uteModulePlatformScreenWriteDataStart(void);
void uteDrvScreenCommonGc9c01QspiWriteCmdParam(uint8_t cmd,uint8_t data);
void uteDrvScreenCommonFt2308QspiWrite16bitCmdParams(uint16_t cmd,uint8_t *data,uint8_t size);
void uteDrvScreenCommonFt2308QspiWrite8bitCmdParams(uint8_t cmd,uint8_t *data,uint8_t size);
void uteDrvScreenCommonNv3030bQspiWrite8bitCmdParams(uint8_t cmd,uint8_t *data,uint8_t size);
uint32_t uteDrvScreenCommonGc9c01QspiReadId(void);
#elif UTE_DRV_DSPI_FOR_SCREEN_SUPPORT
/**
*@brief   st7789p3 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
void uteDrvScreenCommonSt7789p3DspiWriteCmdParams(uint8_t cmd,uint8_t *data,uint8_t size);
/**
*@brief   st7789p3 qspi write
*@details
*@author        zn.zeng
*@date        2021-10-12
*/
void uteDrvScreenCommonSt7789p3DspiWriteCmdParam(uint8_t cmd,uint8_t data);
#endif
#if UTE_DRV_SCREEN_ESD_TE_INT_ERROR_RESET_SUPPORT
extern uint8_t  EsdTeIntErrorCheckCnt;
void uteDrvScreenEsdTeIntErrorCheckHandlerMsg(void);
void uteDrvScreenEsdTeIntErrorCheckTimerCallback(void);
void uteDrvScreenEsdTeIntErrorCheckTimerCreate(void);
void uteDrvScreenEsdTeIntErrorCheckTimerStart(uint16_t ms);
void uteDrvScreenEsdTeIntErrorCheckTimerStop(void);
void uteDrvScreenEsdTeIntErrorCheckCntReset(void);
bool uteDrvScreenEsdTeIntErrorCheckIsErrorState(void);
#endif
#endif //_UTE_DRV_SCREEN_COMMON_H_

