/**
*@file
*@brief        音乐模块
*@details
*@author       zn.zeng
*@date       2021-11-20
*@version      v1.0
*/
#include "ute_module_music.h"
#include "ute_module_log.h"
#include "ute_module_charencode.h"
#include "ute_module_protocol.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_filesystem.h"
#include "ute_drv_motor.h"
#include "ute_module_call.h"
#include "include.h"
#include <ctype.h>

ute_module_music_data_t uteModuleMusicData;

/* module music 互斥量 zn.zeng 2022-02-14*/
void *uteModuleMusicMute;

int user_atoi(const char *str)
{
    if (str == NULL)
    {
        return 0; // 空指针保护
    }

    int result = 0;
    int sign = 1; // 默认是正数

    // 跳过前导空格
    while (*str == ' ')
    {
        str++;
    }

    // 处理正负号
    if (*str == '+' || *str == '-')
    {
        if (*str == '-')
        {
            sign = -1;
        }
        str++;
    }

    // 遍历字符
    while (*str >= '0' && *str <= '9')
    {
        result = result * 10 + (*str - '0');
        str++;
    }

    return sign * result;
}

/**
*@brief  初始化
*@details
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicInit(void)
{
    memset(&uteModuleMusicData,0,sizeof(ute_module_music_data_t));
    uteModuleMusicData.isPaused = true;
    uteModulePlatformCreateMutex(&uteModuleMusicMute);
    uteModuleMusicData.playChannel = UTE_MUSIC_PLAY_CHANNEL_PHONE_TO_SPEAKER;
}
/**
*@brief  设置暂停状态
*@details
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicSetPlayerPaused(bool isPaused,ute_module_music_play_channel_t chn)
{
    uteModuleMusicData.isPaused = isPaused;
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL, "%s,.isPaused=%d", __func__, uteModuleMusicData.isPaused);
}

/**
*@brief  获取暂停状态
*@details
*@author        pcm
*@date        2022-08-08
*/
bool uteModuleMusicGetPlayerPaused(void)
{
    return  uteModuleMusicData.isPaused;
}

/**
*@brief  ams服务转换播放时间
*@details
*@param[in] uint8_t *data，时间数据
*@param[in] uint16_t size，数据长度
*@author        zn.zeng
*@date        2021-11-20
*/
uint32_t uteModuleMusicAmsConversionPlayerTimeMs(uint8_t *data,uint16_t size)
{
    uint32_t timeMs = 0;
    uint16_t startCpyIndex = 0;
    timeMs = user_atoi((const char *)&data[size-3]);
    for(int i=size-1; i>0; i--)
    {
        if(data[i]==0x2c)
        {
            startCpyIndex = i+1;
            break;
        }
    }
    data[size-4] = 0;
    timeMs = timeMs+user_atoi((const char *)&data[startCpyIndex])*1000;
    return timeMs;
}

/**
*@brief  设置当前播放时间
*@details 单位ms
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicSetPlayerCurrTime(uint32_t ms)
{
    uteModuleMusicData.playerCurrTimeMs = ms;
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL,"%s,uteModuleMusicData.playerCurrTimeMs=%d", __func__,uteModuleMusicData.playerCurrTimeMs);
}

/**
*@brief  设置总播放时间
*@details 单位ms
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicSetPlayerTotalTime(uint32_t ms)
{
    uteModuleMusicData.playerTatolTimeMs = ms;
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL,"%s,uteModuleMusicData.playerTatolTimeMs=%d", __func__,uteModuleMusicData.playerTatolTimeMs);
}

/**
*@brief  设置音量
*@details
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicSetPlayerVolume(uint8_t volume)
{
    uteModuleMusicData.volume = volume;
    if(uteModuleMusicData.volume>100)
    {
        uteModuleMusicData.volume = 100;
    }
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL,"%s,uteModuleMusicData.volume=%d", __func__,uteModuleMusicData.volume);
}
/**
*@brief  设置歌词
*@details  输入编码为utf-8
*@param[in] uint8_t *data,数据
*@param[in] uint16_t size,长度
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicSetPlayerArtist(uint8_t *data,uint16_t size)
{
    uteModulePlatformTakeMutex(uteModuleMusicMute);
    uteModuleMusicData.playerArtistSize = UTE_MUSIC_ARTLIST_MAX_SIZE;
    memset(&uteModuleMusicData.playerArtist[0],0,UTE_MUSIC_ARTLIST_MAX_SIZE);
    // uteModuleCharencodeUtf8ConversionUnicode(&uteModuleMusicData.playerArtist[0],&uteModuleMusicData.playerArtistSize,data,size);
    uteModuleMusicData.playerArtistSize = size > UTE_MUSIC_ARTLIST_MAX_SIZE ? UTE_MUSIC_ARTLIST_MAX_SIZE : size;
    memcpy(&uteModuleMusicData.playerArtist[0],data,uteModuleMusicData.playerArtistSize);
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL,"%s,artist:", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_MUSIC_LVL,&uteModuleMusicData.playerArtist[0],uteModuleMusicData.playerArtistSize);
    uteModulePlatformGiveMutex(uteModuleMusicMute);
}

/**
*@brief  设置歌曲名
*@details  输入编码为utf-8
*@param[in] uint8_t *data,数据
*@param[in] uint16_t size,长度
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicSetPlayerTitle(uint8_t *data,uint16_t size)
{
    uteModulePlatformTakeMutex(uteModuleMusicMute);
    uteModuleMusicData.playerTitleSize = UTE_MUSIC_TITLE_MAX_SIZE;
    memset(&uteModuleMusicData.playerTitle[0],0,UTE_MUSIC_TITLE_MAX_SIZE);
    // uteModuleCharencodeUtf8ConversionUnicode(&uteModuleMusicData.playerTitle[0],&uteModuleMusicData.playerTitleSize,data,size);
    uteModuleMusicData.playerTitleSize = size > UTE_MUSIC_TITLE_MAX_SIZE ? UTE_MUSIC_TITLE_MAX_SIZE : size;
    memcpy(&uteModuleMusicData.playerTitle[0],data,uteModuleMusicData.playerTitleSize);
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL,"%s,title:", __func__);
    UTE_MODULE_LOG_BUFF(UTE_LOG_MUSIC_LVL,&uteModuleMusicData.playerTitle[0],uteModuleMusicData.playerTitleSize);
    uteModulePlatformGiveMutex(uteModuleMusicMute);
}

/**
*@brief  获取歌曲名
*@details  输入编码为utf-8
*@param[in] uint8_t *data,数据
*@param[in] uint16_t size,长度
*@author        pcm
*@date        2022-7-30
*/
void uteModuleMusicGetPlayerTitle(uint8_t *data,uint16_t *size)
{
    uteModulePlatformTakeMutex(uteModuleMusicMute);
    memcpy(data,&uteModuleMusicData.playerTitle[0],uteModuleMusicData.playerTitleSize);
    *size = uteModuleMusicData.playerTitleSize;
    uteModulePlatformGiveMutex(uteModuleMusicMute);
}
/**
*@brief  获取数据
*@details
*@param[in] ute_module_music_data_t *data,数据
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicGetPlayerData(ute_module_music_data_t *data)
{
    uteModulePlatformTakeMutex(uteModuleMusicMute);
    memcpy(data,&uteModuleMusicData,sizeof(ute_module_music_data_t));
    uteModulePlatformGiveMutex(uteModuleMusicMute);
}
/**
*@brief  设置palyer 数据
*@details
*@param[in] ute_module_music_data_t data,数据
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicSetPlayerData(ute_module_music_data_t data)
{
    uteModulePlatformTakeMutex(uteModuleMusicMute);
    memcpy(&uteModuleMusicData,&data,sizeof(ute_module_music_data_t));
    uteModulePlatformGiveMutex(uteModuleMusicMute);
}

/**
*@brief  蓝牙断开里连接恢复暂停状态
*@details
*@author        xjc
*@date        2022-04-28
*/
void uteModuleMusicResetPlayStatus(void)
{
    if (uteModuleMusicData.playChannel == UTE_MUSIC_PLAY_CHANNEL_PHONE_TO_SPEAKER)
    {
        uteModuleMusicData.isPaused = true;
    }
}

/**
*@brief      控制开始、暂停播放音乐
*@details
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicCtrlPaused(bool isNeedVibration)
{
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL, "%s,.playChannel=%d", __func__, uteModuleMusicData.playChannel);
    ute_ble_connect_state_t status;
    uint8_t response[2];
    response[0] = CMD_SEND_KEYCODE;
    uteApplicationCommonGetBleConnectionState(&status);
    if (status.isConnected)
    {
        if(isNeedVibration)
        {
            uteDrvMotorStart(200, UTE_MOTOR_INTERVAL_TIME, 1);
        }
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_PLAY_PAUSE);
        response[1] = 0x07;
        uteModuleProfileBleSendToPhone(&response[0], 2);
    }
}
/**      在音乐播放时候去操作录音、响铃等先暂停音乐
*@details
*@author        pcm
*@date        2022-12-11
*/
void uteModuleMusicCtrlPausedBeforeOperationStop(void)
{
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL, "%s,.playChannel=%d", __func__, uteModuleMusicData.playChannel);
    ute_ble_connect_state_t status;
    uint8_t response[2];
    response[0] = CMD_SEND_KEYCODE;
    uteApplicationCommonGetBleConnectionState(&status);
    if (status.isConnected)
    {
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_PLAY_PAUSE);
        response[1] = 0x07;
        uteModuleProfileBleSendToPhone(&response[0], 2);
        uteModuleMusicSetPlayerPaused(true,UTE_MUSIC_PLAY_CHANNEL_PHONE_TO_SPEAKER);
    }
}
/**
*@brief      切换歌曲
*@details
*@param[in] bool isCutNext,是下一曲还是上一曲
*@param[in] bool isCutManual,是否为手动切歌。防止单曲自动循环模式时，手动切歌还停留在当前单曲
*@param[in] bool isNeedShake,是否需要震动
*@author        zn.zeng
*@date        2021-11-20
*/
void uteModuleMusicCtrl(bool isCutNext,bool isCutManual,bool isNeedShake)
{
    ute_ble_connect_state_t status;
    uint8_t response[2];
    response[0] = CMD_SEND_KEYCODE;
    uteApplicationCommonGetBleConnectionState(&status);
    if (status.isConnected)
    {
        if (isNeedShake)
        {
            uteDrvMotorStart(200, UTE_MOTOR_INTERVAL_TIME, 1);
        }
        if(isCutNext)
        {
            ble_ams_remote_ctrl(AMS_REMOTE_CMD_NEXT_TRACK);
            response[1] = 0x08;  //下一曲
        }
        else
        {
            ble_ams_remote_ctrl(AMS_REMOTE_CMD_PREV_TRACK);
            response[1] = 0x09;  //上一曲
        }
        uteModuleProfileBleSendToPhone(&response[0], 2);
    }
}

/**
*@brief      控制音量加
*@details
*@author        dengli.lu
*@date        2021-12-02
*/
void uteModuleMusicCtrlVolumeIncrease(void)
{
    ute_ble_connect_state_t status;
    uint8_t response[2];
    response[0] = CMD_SEND_KEYCODE;
    uteApplicationCommonGetBleConnectionState(&status);
    if (status.isConnected)
    {
#if (!UTE_MODULE_MUSIC_CONTROL_VOLUME_NO_MOTOR_SUPPORT)
        uteDrvMotorStart(200, UTE_MOTOR_INTERVAL_TIME, 1);
#endif
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_VOL_UP);
        response[1] = 0x0D;
        uteModuleProfileBleSendToPhone(&response[0], 2);
    }
}
/**
*@brief      控制音量减
*@details
*@author        dengli.lu
*@date        2021-12-02
*/
void uteModuleMusicCtrlVolumeDecrease(void)
{
    ute_ble_connect_state_t status;
    uint8_t response[2];
    response[0] = CMD_SEND_KEYCODE;
    uteApplicationCommonGetBleConnectionState(&status);
    if (status.isConnected)
    {
#if (!UTE_MODULE_MUSIC_CONTROL_VOLUME_NO_MOTOR_SUPPORT)
        uteDrvMotorStart(200, UTE_MOTOR_INTERVAL_TIME, 1);
#endif
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_VOL_DOWN);
        response[1] = 0x0E;
        uteModuleProfileBleSendToPhone(&response[0], 2);
    }
}

/**
*@brief      控制音量服务状态Ok
*@details
*@author         zn.zeng
*@date        2022-04-07
*/
void uteModuleMusicAvrcpConnCmpl(void)
{
#if 0
    uteModuleMusicData.avrvpConnCmplRtc = uteModulePlatformGetSystemTick();
#endif
}
/**
*@brief      绑定时是否设置最大音量
*@details
*@author        zn.zeng
*@date        2022-04-07
*/
bool uteModuleMusicIsSetMaxVolume(void)
{
    bool isMaxVolume = false;
#if 0
    uint32_t diff = (uteModulePlatformGetSystemTick()- uteModuleMusicData.avrvpConnCmplRtc);
    if(diff<(3200*5))//100ms*5
    {
        isMaxVolume = true;
    }
    UTE_MODULE_LOG(UTE_LOG_MUSIC_LVL,"%s,diff=%d,isMaxVolume0=%d", __func__,diff,isMaxVolume);
#endif
    return isMaxVolume;
}
/**
*@brief      获取当前播放通道
*@details
*@return      ute_module_music_play_channel_t
*@author        zn.zeng
*@date        2022-05-30
*/
ute_module_music_play_channel_t uteModuleMusicGetPlayChannel(void)
{
    return uteModuleMusicData.playChannel;
}

/*
*@brief  是否正在播放蓝牙音乐
*@details
*@author        zn.zeng
*@date        2022-06-10
*/
void uteModuleMusicSetPlayingA2dpStatus(bool isPlaying)
{
    uteModuleMusicData.isPlayingA2dp = isPlaying;
}
/**
*@brief  获取播放蓝牙音乐
*@details
*@author        zn.zeng
*@date        2022-06-10
*/
bool uteModuleMusicGetPlayingA2dpStatus(void)
{
    return uteModuleMusicData.isPlayingA2dp;
}
/**
*@brief  设置音乐控制界面
*@details
*@author        zn.zeng
*@date        2022-06-10
*/
void uteModuleMusicSetControlView(UTE_MUSIC_UI_VIEW_T controlView)
{
    uteModuleMusicData.musicControlView = controlView;
}
/**
*@brief  获取音乐控制界面
*@details
*@author        zn.zeng
*@date        2022-06-10
*/
UTE_MUSIC_UI_VIEW_T uteModuleMusicGetControlView(void)
{
    return uteModuleMusicData.musicControlView;
}
