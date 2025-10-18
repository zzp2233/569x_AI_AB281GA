/**
*@file
*@brief        铃声音量处理模块
*@details
*@author       dengli.lu
*@date       2022-04-24
*@version      v1.0
*/
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_localRingtone.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"
#include "ute_drv_battery_common.h"
#include "ute_module_notdisturb.h"
#include "ute_module_music.h"
#include "include.h"
#include "res.h"

ute_local_ringtone_data_t uteModuleLocalRingtoneData;
void *uteModuleLocalRingtonePlayRingTimer = NULL;

static const ute_local_ringtone_res_info_t uteModuleLocalRingtoneResInfoTbl[] =
{
#ifdef RES_BUF_RING_CALL_MP3
    {.ptr = &RES_BUF_RING_CALL_MP3, .len = &RES_LEN_RING_CALL_MP3},
#else
#ifdef RES_BUF_RING_RING_MP3
    {.ptr = &RES_BUF_RING_RING_MP3, .len = &RES_LEN_RING_RING_MP3},
#endif
#endif
#ifdef RES_BUF_RING_ALARM_MP3
    {.ptr = &RES_BUF_RING_ALARM_MP3, .len = &RES_LEN_RING_ALARM_MP3},
#else
#ifdef RES_BUF_RING_REDIAL_MP3
    {.ptr = &RES_BUF_RING_REDIAL_MP3, .len = &RES_LEN_RING_REDIAL_MP3},
#endif
#endif
#ifdef RES_BUF_RING_FIND_MP3
    {.ptr = &RES_BUF_RING_FIND_MP3, .len = &RES_LEN_RING_FIND_MP3},
#endif
#ifdef RES_BUF_RING_FACTORY_MP3
    {.ptr = &RES_BUF_RING_FACTORY_MP3, .len = &RES_LEN_RING_FACTORY_MP3},
#else
#ifdef RES_BUF_RING_CALL_MP3
    {.ptr = &RES_BUF_RING_CALL_MP3, .len = &RES_LEN_RING_CALL_MP3},
#else
#ifdef RES_BUF_RING_RING_MP3
    {.ptr = &RES_BUF_RING_RING_MP3, .len = &RES_LEN_RING_RING_MP3},
#endif
#endif
#endif
#ifdef RES_BUF_RING_AGING_MP3
    {.ptr = &RES_BUF_RING_AGING_MP3, .len = &RES_LEN_RING_AGING_MP3}
#endif
};

/**
*@brief  初始化函数
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneInit(void)
{
    memset(&uteModuleLocalRingtoneData,0,sizeof(ute_local_ringtone_data_t));
    uteModuleLocalRingtoneReadConfig();
    uteModuleLocalRingtoneSetVolume(uteModuleLocalRingtoneData.localRingtoneVolume);
#if UTE_MODULE_OTHER_RING_POWER_SUPPORT
    uteModuleLocalRingtoneReadOtherPowerStatus();
#endif
}

/**
*@brief     RingVolume 读取绑定数据到内存
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneReadConfig(void)
{
    /*! 保存到文件dengli.lu, 2022-04-24*/
    void *file;
    uint8_t readbuff[3];
    memset(readbuff,0,sizeof(readbuff));
    readbuff[0] = UTE_MODULE_LOCAL_RINGTONE_DEFAULT_VOLUME;
    readbuff[1] = UTE_MODULE_LOCAL_RINGTONE_DEFAULT_MUTE;
    readbuff[2] = UTE_MODULE_LOCAL_RINGTONE_DEFAULT_MUTE;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOCAL_RINGTONE_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],sizeof(readbuff));
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleLocalRingtoneData.localRingtoneVolume = readbuff[0];
    uteModuleLocalRingtoneData.isLocalRingtoneMute = readbuff[1];
    uteModuleLocalRingtoneData.localRingtoneMuteStatusBeforeNotdisturb = readbuff[2];

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.localRingtoneVolume=%d", __func__,uteModuleLocalRingtoneData.localRingtoneVolume);
}
/**
*@brief     RingVolume 保存绑定数据到内存
*@details
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneSaveData(void)
{
    /*! 保存到文件，dengli.lu, 2022-04-24*/
    void *file;
    uint8_t writebuff[3];
    writebuff[0] = uteModuleLocalRingtoneData.localRingtoneVolume;
    writebuff[1] = uteModuleLocalRingtoneData.isLocalRingtoneMute;
    writebuff[2] = uteModuleLocalRingtoneData.localRingtoneMuteStatusBeforeNotdisturb;

    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_LOCAL_RINGTONE_INFO, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &writebuff[0], sizeof(writebuff));
        uteModuleFilesystemCloseFile(file);
    }

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.localRingtoneVolume=%d", __func__,uteModuleLocalRingtoneData.localRingtoneVolume);
}

/**
*@brief  设置音量
*@details
*@param[in] uint8_t localRingtoneVolume
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneSetVolume(uint8_t localRingtoneVolume)
{
    uteModuleLocalRingtoneData.localRingtoneVolume = localRingtoneVolume;
    bsp_change_volume(localRingtoneVolume);
    xcfg_cb.warning_volume = localRingtoneVolume;
}
/**
*@brief  获取音量
*@details
*@return uint8_t localRingtoneVolume
*@author        dengli.lu
*@date        2022-04-24
*/
uint8_t uteModuleLocalRingtoneGetVolume(void)
{
    return uteModuleLocalRingtoneData.localRingtoneVolume;
}
/**
*@brief  设置静音
*@details
*@param[in] bool isLocalRingtoneMute
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneSetMuteStatus(bool isLocalRingtoneMute, bool needToSaved)
{
    uteModuleLocalRingtoneData.isLocalRingtoneMute = isLocalRingtoneMute;
    if(needToSaved)
    {
        uteModuleLocalRingtoneSaveData();
    }
}
/**
*@brief  获取静音状态
*@details
*@return bool isLocalRingtoneMute
*@author        dengli.lu
*@date        2022-04-24
*/
bool uteModuleLocalRingtoneGetMuteStatus(void)
{
    return uteModuleLocalRingtoneData.isLocalRingtoneMute;
}
/**
*@brief  设置勿扰前静音状态
*@details
*@param[in] bool isLocalRingtoneMute
*@author        dengli.lu
*@date        2022-04-24
*/
void uteModuleLocalRingtoneSetMuteStatusBeforeNotdisturb(bool localRingtoneMuteStatusBeforeNotdisturb)
{
    uteModuleLocalRingtoneData.localRingtoneMuteStatusBeforeNotdisturb = localRingtoneMuteStatusBeforeNotdisturb;
}

/**
*@brief     保存播放的声音类型
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtoneSetPlayRingType(ute_local_ringtone_type type)
{
    /*!来电时有其他铃声在响，停止其它铃声的定时器*/
    if(type == RINGTON_TYPE_CALL)
    {
        if(uteModuleLocalRingtoneData.playingRingType > RINGTON_TYPE_CALL)
        {
            uteModuleLocalRingtonePlayRingTimerStop();
        }
    }
    uteModuleLocalRingtoneData.playingRingType = type;
}

/**
*@brief     获取播放的声音类型
*@details
*@author    xjc
*@date      2022-05-12
*/
ute_local_ringtone_type uteModuleLocalRingtoneGetPlayRingType(void)
{
    return uteModuleLocalRingtoneData.playingRingType;
}

/**
*@brief     开始播放铃声处理函数
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtonePlayRingDetail(void)
{
    ute_local_ringtone_res_info_t *res_info = uteModuleLocalGetRingtoneResInfo(uteModuleLocalRingtoneData.ringResourceIndex);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,playingRingType:%d,addr:0x%x,len:%d", __func__, uteModuleLocalRingtoneData.playingRingType, *res_info->ptr, *res_info->len);
    if (*res_info->len == 0 || *res_info->ptr == NULL)
    {
        return;
    }
    if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_FACTORY)
    {
        xcfg_cb.warning_volume = UTE_MODULE_NEW_FACTORY_TEST_RING_VOLUME_LEVEL;
    }
    else if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_AGING)
    {
        xcfg_cb.warning_volume = UTE_MODULE_NEW_FACTORY_TEST_AGING_RING_VOLUME_LEVEL;
    }
#if UTE_MODULE_LOCAL_RINGTONE_MAX_VOLUME_SUPPORT
    else if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_ALARM)
    {
        xcfg_cb.warning_volume = xcfg_cb.vol_max;
    }
    else
#endif
    {
        xcfg_cb.warning_volume = uteModuleLocalRingtoneGetVolume();
    }
    func_bt_mp3_res_play(*res_info->ptr, *res_info->len);
}

/**
*@brief     停止播放铃声处理函数
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtoneStopRingDetail(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
    uteModuleLocalRingtonePlayRingTimerStop();
    if (sys_cb.mp3_res_playing)
    {
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(sys_cb.vol);
}

/**
*@brief     停止播放铃声定时器
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtonePlayRingTimerStop(void)
{
    uteModulePlatformStopTimer(&uteModuleLocalRingtonePlayRingTimer);
    uteModulePlatformDeleteTimer(&uteModuleLocalRingtonePlayRingTimer);
    uteModuleLocalRingtonePlayRingTimer = NULL;
}

/**
*@brief     播放铃声定时器回调函数
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtonePlayRingTimerCallback(void *pxTimer)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s", __func__);
    uteModulePlatformSendMsgToAppTask(TO_APP_TASK_PLAY_RING, 0);
    uteModuleLocalRingtoneData.ringTime -= uteModuleLocalRingtoneData.ringResourceDuration;
    if (uteModuleLocalRingtoneData.ringTime > 0)
    {
        uteModuleLocalRingtonePlayRingDetail();
        uteModulePlatformRestartTimer(&uteModuleLocalRingtonePlayRingTimer, uteModuleLocalRingtoneData.ringResourceDuration * 1000);
    }
    else
    {
        uteModuleLocalRingtoneStopRing();
    }
}

/**
*@brief     停止播放铃声
*@details
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtoneStopRing(void)
{
    if(uteModuleLocalRingtoneData.playingRingType != RINGTON_TYPE_NONE)
    {
        uteModuleLocalRingtoneStopRingDetail();
    }
    uteModuleLocalRingtoneData.playingRingType = RINGTON_TYPE_NONE;
    uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_PLAY_RING);
}

/**
*@brief     开始播放铃声
*@details
*@param[in] playingRingType 铃声类型
*@author    xjc
*@date      2022-05-12
*/
void uteModuleLocalRingtonePlayRing(ute_local_ringtone_type playingRingType)
{
    if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_CALL)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.playingRingType is Call, return!", __func__);
        return;
    }
#if UTE_MODULE_OTHER_RING_POWER_SUPPORT
    if (!uteModuleLocalRingtoneGetOtherPowerStatus() && (playingRingType == RINGTON_TYPE_ALARM || playingRingType == RINGTON_TYPE_FIND))
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,.uteModuleLocalRingtoneGetOtherPowerStatus=true return!", __func__);
        return;
    }
#endif
    uteModuleLocalRingtoneData.playingRingType = playingRingType;
    if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_CALL)
    {
        uteModuleLocalRingtoneData.ringResourceIndex = UTE_MODULE_BT_RINGTONE_HF_CALL_IN_INDEX;
        uteModuleLocalRingtoneData.ringResourceDuration = UTE_MODULE_BT_LOCAL_RINGTONE_DURATION;
        uteModuleLocalRingtoneData.ringTime = 0xffff / 2;
    }
    else if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_ALARM)
    {
        uteModuleLocalRingtoneData.ringResourceIndex = UTE_MODULE_LOCAL_RINGTONE_ALARM_INDEX;
        uteModuleLocalRingtoneData.ringResourceDuration = UTE_MODULE_LOCAL_RINGTONE_ALARM_DURATION;
        uteModuleLocalRingtoneData.ringTime = UTE_MODULE_LOCAL_RINGTONE_ALARM_TIME;
    }
    else if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_FIND)
    {
        uteModuleLocalRingtoneData.ringResourceIndex = UTE_MODULE_LOCAL_RINGTONE_FIND_INDEX;
        uteModuleLocalRingtoneData.ringResourceDuration = UTE_MODULE_LOCAL_RINGTONE_FIND_DURATION;
        uteModuleLocalRingtoneData.ringTime = UTE_MODULE_LOCAL_RINGTONE_FIND_TIME;
    }
    else if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_FACTORY)
    {
        uteModuleLocalRingtoneData.ringResourceIndex = UTE_MODULE_NEW_FACTORY_TEST_RING_INDEX;
        uteModuleLocalRingtoneData.ringResourceDuration = UTE_MODULE_NEW_FACTORY_TEST_RING_DURATION;
        uteModuleLocalRingtoneData.ringTime = UTE_MODULE_NEW_FACTORY_TEST_RING_TIME;
    }
    else if (uteModuleLocalRingtoneData.playingRingType == RINGTON_TYPE_AGING)
    {
        uteModuleLocalRingtoneData.ringResourceIndex = UTE_MODULE_NEW_FACTORY_TEST_AGING_RING_INDEX;
        uteModuleLocalRingtoneData.ringResourceDuration = UTE_MODULE_NEW_FACTORY_TEST_RING_DURATION;
        uteModuleLocalRingtoneData.ringTime = 0xffff / 2;
    }
    if (uteModuleLocalRingtoneData.ringResourceIndex >= sizeof(uteModuleLocalRingtoneResInfoTbl) / sizeof(uteModuleLocalRingtoneResInfoTbl[0]))
    {
        uteModuleLocalRingtoneData.ringResourceIndex = 0;
    }
    uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_PLAY_RING);
    uteModuleLocalRingtonePlayRingDetail();
    if (uteModuleLocalRingtonePlayRingTimer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleLocalRingtonePlayRingTimer, "uteModuleLocalRingtonePlayRingTimer", 1, 1000, false, uteModuleLocalRingtonePlayRingTimerCallback);
    }
    uteModulePlatformRestartTimer(&uteModuleLocalRingtonePlayRingTimer, uteModuleLocalRingtoneData.ringResourceDuration * 1000);
#if UTE_MODULE_PLAYBACK_SUPPORT
    /*2022 08-17  闹钟开始 暂停本地音乐*/
    if (uteModuleMusicGetPlayerPaused() == false)
    {
        uteModuleMusicSetPlayerPaused(true, UTE_MUSIC_PLAY_CHANNEL_LOCAL_TO_SPEAKER);
    }
#endif
}

/**
 * @brief        获取对应铃声类型的资源文件
 * @details
 * @author       Wang.Luo
 * @date         2025-01-17
 */
const ute_local_ringtone_res_info_t *uteModuleLocalGetRingtoneResInfo(uint8_t ringResourceIndex)
{
    uint8_t ringtoneCount = sizeof(uteModuleLocalRingtoneResInfoTbl) / sizeof(uteModuleLocalRingtoneResInfoTbl[0]);
    if (ringResourceIndex < ringtoneCount)
    {
        return &uteModuleLocalRingtoneResInfoTbl[ringResourceIndex];
    }
    else if (ringtoneCount > 0)
    {
        return &uteModuleLocalRingtoneResInfoTbl[0];
    }
    return NULL;
}

/*! 其他铃声的开关状态,包括查找手机、闹钟铃声开关 pcm, 2022-11-11  */
#if UTE_MODULE_OTHER_RING_POWER_SUPPORT
/**
*@brief     保存其他铃声的开关状态
*@details
*@author    pcm
*@date      2022-11-11
*/
void uteModuleLocalRingtoneSaveOtherPowerStatus(void)
{
    void *file;
    uint8_t writebuff[2];
    writebuff[0] = uteModuleLocalRingtoneData.OtherLocalRingtoneStatu;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_OTHER_LOCAL_RINGTONE_POWER_STATUS,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writebuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief     读取其他铃声的开关状态
*@details
*@author    pcm
*@date      2022-11-11
*/
void uteModuleLocalRingtoneReadOtherPowerStatus(void)
{
    void *file;
    uint8_t readbuff[2];
    readbuff[0] = UTE_MODULE_OTHER_RING_POWER_DEFAULT_ON;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_OTHER_LOCAL_RINGTONE_POWER_STATUS,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],1);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleLocalRingtoneData.OtherLocalRingtoneStatu = readbuff[0];
    UTE_MODULE_LOG(UTE_LOG_CALL_LVL, "%s,uteModuleLocalRingtoneData.OtherLocalRingtoneStatu=%d", __func__,uteModuleLocalRingtoneData.OtherLocalRingtoneStatu);
}

/**
*@brief  获取其他铃声开关状态
*@details
*@return
*@author      pcm
*@date        2022-11-11
*/
bool uteModuleLocalRingtoneGetOtherPowerStatus(void)
{
    return uteModuleLocalRingtoneData.OtherLocalRingtoneStatu;
}
/**
*@brief  设置其他铃声开关状态
*@details
*@param[in]
*@author      pcm
*@date        2022-11-11
*/
void uteModuleLocalRingtoneSetOtherPowerStatus(bool isOtherPowerStatus)
{
    uteModuleLocalRingtoneData.OtherLocalRingtoneStatu = isOtherPowerStatus;
    uteModuleLocalRingtoneSaveOtherPowerStatus();
}
#endif
