/**
 *@file
 *@brief        绑定app处理模块
 *@details
 *@author       dengli.lu
 *@date       2022-01-18
 *@version      v1.0
 */
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_appbinding.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"
#include "ute_drv_battery_common.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_sleep.h"
#include "ute_module_sport.h"

ute_appbinding_data_t uteModuleAppBindingData;

/**
 *@brief  初始化函数
 *@details
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingInit(void)
{
    uteModuleAppBindingData.newBindingUserId = 0xffffff;
    uteModuleAppBindingData.isHasBindingNotify = false;
    uteModuleAppBindingData.isOurAppApkConnection = false;
    uteModuleAppBindingData.isBindingStart = false;
    uteModuleAppBindingData.ourAppApkConnectionSecCnt = 0;
    uteModuleAppBindingData.userQRcodeRandom = 1 + get_random(0xffff);
    uteModuleAppBindingData.isHasDisplayedBindingScreen = false;
    uteModuleAppBindingReadConfig();
}

/**
 *@brief     appBinding 读取绑定数据到内存
 *@details
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingReadConfig(void)
{
    /*! 保存到文件dengli.lu, 2022-02-28*/
    void *file;
    uint8_t readbuff[2];
    memset(readbuff, 0, 2);
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_APP_BINDING_INFO, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 2);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleAppBindingData.hasBeenConnectedBefore = readbuff[0];
#if UTE_APP_BINDING_ONLY_ONCENOTIFY_SUPPORT
    uteModuleAppBindingData.isHasBindingNotify = readbuff[1];
#endif
}

/**
 *@brief     appBinding 保存绑定数据到内存
 *@details
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleSaveAppBindingData(void)
{
    /*! 保存到文件，dengli.lu, 2022-02-28*/
    void *file;
    uint8_t writebuff[2];
    memset(writebuff, 0, 2);
    writebuff[0] = uteModuleAppBindingData.hasBeenConnectedBefore;
#if UTE_APP_BINDING_ONLY_ONCENOTIFY_SUPPORT
    writebuff[1] = uteModuleAppBindingData.isHasBindingNotify;
#endif
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_APP_BINDING_INFO, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writebuff[0], 2);
        uteModuleFilesystemCloseFile(file);
    }
}

/**
 *@brief  开始绑定函数
 *@details
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingStrat(void)
{
    uint8_t response[3];
    uint32_t oldId = 0xffffffff;
    response[0] = CMD_USER_ID_FOR_BINDING;
    ute_application_sn_data_t snData;
    uint32_t size = sizeof(ute_application_sn_data_t);
    uteModulePlatformFlashNorRead((uint8_t *)&snData, UTE_USER_PARAM_ADDRESS, size);
    if (uteModuleProfileGetPublicProtocol())
    {
        response[0] = PUBLIC_CMD_USER_ID_FOR_BINDING;
    }
    response[1] = 0x04;
    response[2] = 0x02;
    uteModuleProfileBleSendToPhone(&response[0], 3);
    uteModuleAppBindingData.isOurAppApkConnection = true;
    oldId = snData.userId;
    snData.userId = uteModuleAppBindingData.newBindingUserId;
    uteModuleAppBindingData.hasBeenConnectedBefore = HAS_BEEN_CONNECTED;
    uteModuleSaveAppBindingData();
    uteModulePlatformFlashNorErase(UTE_USER_PARAM_ADDRESS);
    uteModulePlatformFlashNorWrite((uint8_t *)&snData, UTE_USER_PARAM_ADDRESS, size);
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,oldId=0x%8x,uteModuleAppBindingData.userId = 0x%8x", __func__, oldId, snData.userId);
    if (oldId == 0xffffffff)
    {
        response[0] = CMD_USER_ID_FOR_BINDING;
        if (uteModuleProfileGetPublicProtocol())
        {
            response[0] = PUBLIC_CMD_USER_ID_FOR_BINDING;
        }
        response[1] = 0x04;
        response[2] = 0x01;
        uteModulePlatformDelayUs(200000);
        uteModuleProfileBleSendToPhone(&response[0], 3);
        uteModuleAppBindingData.isBindingStart = false;
        uteModuleAppBindingSetBindingNotify(false);
    }
    else
    {
#if UTR_APP_BINDING_NOT_RESET_SUPPORT // 绑定时不需要重启清空数据 dengli.lu 2022/08/09
        response[0] = CMD_USER_ID_FOR_BINDING;
        if (uteModuleProfileGetPublicProtocol())
        {
            response[0] = PUBLIC_CMD_USER_ID_FOR_BINDING;
        }
        response[1] = 0x04;
        response[2] = 0x01;
        uteModulePlatformDelayUs(200000);
        uteModuleProfileBleSendToPhone(&response[0], 3);
        uteModuleAppBindingData.isBindingStart = false;
        uteModuleAppBindingSetBindingNotify(false);
#else
        uteModuleAppBindingData.isBindingStart = false;
        uteModulePlatformWdgFeed(); // 多加一次喂狗
        uteModulePlatformDelayUs(1000000);
        uteModulePlatformWdgFeed(); // 多加一次喂狗
#if UTE_MODULE_PLAYBACK_SUPPORT
        uteModuleMicRecordDeInit();
        uteModuleEarphoneBondDeleteALLByIndex();
#endif
        uteModuleFilesystemDelAllData();
#if UTE_MODULE_BATTERY_SAVE_LAST_LVL_BEFORE_FACTORY_SUPPORT
        uteDrvBatteryCommonSaveLastLvlToSN1();
#endif
        uteModulePlatformSystemReboot();
#endif
    }
#if APP_SYNC_RECORD_TIME_FLAG_SUPPORT
    ute_module_systemtime_measure_time_t setTime;
    memset(&setTime,0,sizeof(ute_module_systemtime_measure_time_t));
    uteModuleHeartSetSyncTime(setTime);
    uteModuleBloodoxygenSetSyncTime(setTime);
    uteModuleSleepSetSyncTime(setTime);
    uteModuleSportSetStepSyncTime(setTime);
    uteModuleSportSetExerciseTimeSyncTime(setTime);
    uteModuleSportSetMoreSportsSyncTime(setTime);
#endif
}

/**
 *@brief  设置绑定用户ID
 *@details
 *@param[in] uint32_t userId
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingClearUserId(void)
{
    ute_application_sn_data_t snData;
    uint32_t size = sizeof(ute_application_sn_data_t);
    uteModulePlatformFlashNorRead((uint8_t *)&snData, UTE_USER_PARAM_ADDRESS, size);
    uteModulePlatformFlashNorErase(UTE_USER_PARAM_ADDRESS);
    snData.userId = 0xffffffff;
    uteModulePlatformFlashNorWrite((uint8_t *)&snData, UTE_USER_PARAM_ADDRESS, size);
}

/**
 *@brief  设置新的绑定用户ID
 *@details
 *@param[in] uint32_t newBindingUserId
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingSetNewUserId(uint32_t newBindingUserId)
{
    uteModuleAppBindingData.newBindingUserId = newBindingUserId;
}

/**
 *@brief  获取新的绑定用户ID
 *@details
 *@return uint32_t newBindingUserId
 *@author        dengli.lu
 *@date        2022-02-28
 */
uint32_t uteModuleAppBindingGetNewUserId(void)
{
    return uteModuleAppBindingData.newBindingUserId;
}

/**
 *@brief  设置是否为UTE自己APP连接
 *@details
 *@param[in] bool isOurAppApkConnection
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingSetOurAppConnection(bool isOurAppApkConnection)
{
    uteModuleAppBindingData.isOurAppApkConnection = isOurAppApkConnection;
}

/**
 *@brief  获取是否为UTE自己APP连接
 *@details
 *@return bool isOurAppApkConnection
 *@author        dengli.lu
 *@date        2022-02-28
 */
bool uteModuleAppBindingGetOurAppConnection(void)
{
    return uteModuleAppBindingData.isOurAppApkConnection;
}

/**
 *@brief  设置是否开始绑定
 *@details
 *@param[in] bool isBindingStart
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingSetBindingStart(bool isBindingStart)
{
    uteModuleAppBindingData.isBindingStart = isBindingStart;
}

/**
 *@brief  获取是否开始绑定
 *@details
 *@return bool isBindingStart
 *@author        dengli.lu
 *@date        2022-02-28
 */
bool uteModuleAppBindingGetBindingStart(void)
{
    return uteModuleAppBindingData.isBindingStart;
}

/**
 *@brief  设置是否已经提醒
 *@details
 *@param[in] bool isHasBindingNotify
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingSetBindingNotify(bool isHasBindingNotify)
{
    uteModuleAppBindingData.isHasBindingNotify = isHasBindingNotify;
#if UTE_APP_BINDING_ONLY_ONCENOTIFY_SUPPORT
    uteModuleSaveAppBindingData();
#endif
}

/**
 *@brief  获取是否已经提醒
 *@details
 *@param[in] bool isHasBindingNotify
 *@author        dengli.lu
 *@date        2022-02-28
 */
bool uteModuleAppBindingGetBindingNotify(void)
{
    return uteModuleAppBindingData.isHasBindingNotify;
}

/**
 *@brief  设置是否已经连接过
 *@details
 *@param[in] uint8_t hasBeenConnectedBefore
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingSetHasBindingBefore(uint8_t hasBeenConnectedBefore)
{
    uteModuleAppBindingData.hasBeenConnectedBefore = hasBeenConnectedBefore;
    uteModuleSaveAppBindingData();
}

/**
 *@brief  设置随机码
 *@details
 *@param[in] uint32_t userQRcodeRandom
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingSetQRcodeRandom(uint32_t userQRcodeRandom)
{
    uteModuleAppBindingData.userQRcodeRandom = userQRcodeRandom;
}

/**
 *@brief  获取随机码
 *@details
 *@param[in] uint32_t userQRcodeRandom
 *@author        dengli.lu
 *@date        2022-02-28
 */
uint32_t uteModuleAppBindingGetQRcodeRandom(void)
{
    UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL, "%s,userQRcodeRandom=%d", __func__, uteModuleAppBindingData.userQRcodeRandom);
    return uteModuleAppBindingData.userQRcodeRandom;
}

/**
 *@brief  获取是否已经连接过
 *@details
 *@param[in] uint8_t hasBeenConnectedBefore
 *@author        dengli.lu
 *@date        2022-02-28
 */
uint8_t uteModuleAppBindingGetHasBindingBefore(void)
{
    return uteModuleAppBindingData.hasBeenConnectedBefore;
}

/**
 *@brief  设置连接时长
 *@details
 *@param[in] uint8_t ourAppApkConnectionSecCnt
 *@author        dengli.lu
 *@date        2022-02-28
 */
void uteModuleAppBindingSetBindingSecCnt(uint8_t ourAppApkConnectionSecCnt)
{
    uteModuleAppBindingData.ourAppApkConnectionSecCnt = ourAppApkConnectionSecCnt;
}

/**
 *@brief  获取连接时长
 *@details
 *@return uint8_t ourAppApkConnectionSecCnt
 *@author        dengli.lu
 *@date        2022-02-28
 */
uint8_t uteModuleAppBindingGetBindingSecCnt(void)
{
    return uteModuleAppBindingData.ourAppApkConnectionSecCnt;
}

/**
 *@brief  已经显示过绑定界面
 *@details
 *@author      dengli.lu
 *@date        2022-06-17
 */
void uteModuleAppBindingSetIsHasDisplayedBindingScreen(bool isHasDisplayedBindingScreen)
{
    uteModuleAppBindingData.isHasDisplayedBindingScreen = isHasDisplayedBindingScreen;
}

/**
 *@brief  已经显示过绑定界面
 *@details
 *@author      dengli.lu
 *@date        2022-06-17
 */
bool uteModuleAppBindingGetIsHasDisplayedBindingScreen(void)
{
    return uteModuleAppBindingData.isHasDisplayedBindingScreen;
}
