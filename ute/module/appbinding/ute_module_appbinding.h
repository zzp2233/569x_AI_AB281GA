/**
*@file
*@brief        绑定app处理模块
*@details
*@author       dengli.lu
*@date       2022-02-25
*@version      v1.0
*/
#ifndef _UTE_MODULE_APPBINDING_H_
#define _UTE_MODULE_APPBINDING_H_

#include "ute_project_config.h"
#include "ute_module_platform.h"
/*! 绑定用户ID数据 dengli.lu, 2021-10-26  */
#define UTE_MODULE_FILESYSTEM_APP_BINDING_INFO "systemparam/appBinding"

typedef struct
{
    uint8_t hasBeenConnectedBefore;
    uint32_t newBindingUserId;
    bool isHasBindingNotify;
    bool isOurAppApkConnection;
    bool isBindingStart;
    uint8_t ourAppApkConnectionSecCnt;
    uint32_t userQRcodeRandom;//二维码的随机码
    bool isHasDisplayedBindingScreen;//已经显示过二维码界面
} ute_appbinding_data_t;


void uteModuleAppBindingInit(void);
void uteModuleAppBindingReadConfig(void);
void uteModuleAppBindingClearUserId(void);
void uteModuleAppBindingSetNewUserId(uint32_t newBindingUserId);
uint32_t uteModuleAppBindingGetNewUserId(void);
void uteModuleAppBindingStrat(void);
void uteModuleAppBindingSetOurAppConnection(bool isOurAppApkConnection);
bool uteModuleAppBindingGetOurAppConnection(void);
void uteModuleAppBindingSetBindingStart(bool isBindingStart);
bool uteModuleAppBindingGetBindingStart(void);
void uteModuleAppBindingSetBindingNotify(bool isHasBindingNotify);
bool uteModuleAppBindingGetBindingNotify(void);
void uteModuleAppBindingSetHasBindingBefore(uint8_t hasBeenConnectedBefore);
uint8_t uteModuleAppBindingGetHasBindingBefore(void);
void uteModuleAppBindingSetBindingSecCnt(uint8_t ourAppApkConnectionSecCnt);
uint8_t uteModuleAppBindingGetBindingSecCnt(void);
void uteModuleAppBindingSetQRcodeRandom(uint32_t userQRcodeRandom);
uint32_t uteModuleAppBindingGetQRcodeRandom(void);
void uteModuleAppBindingSetIsHasDisplayedBindingScreen(bool isHasDisplayedBindingScreen);
bool uteModuleAppBindingGetIsHasDisplayedBindingScreen(void);
#endif
