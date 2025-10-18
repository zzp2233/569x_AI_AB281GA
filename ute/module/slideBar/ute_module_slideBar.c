/**
*@file
*@brief     侧边滑动栏模块
*@details
*@author        ellison
*@date        ellison add ,2022-Jun-24 9:34
*@version       v1.0
*/

#include "ute_module_slideBar.h"
#include "ute_module_log.h"

#if UTE_MODULE_SLIDE_BAR_SUPPORT
/* 滑动栏模块的默认显示图标，如果需要客制化，则在自己的项目配置文件中定义ellison add ,2022-Jun-24 9:41 */
#ifndef UTE_MODULE_SLIDE_BAR_APP_ID0
#define UTE_MODULE_SLIDE_BAR_APP_ID0 FUNC_SET_SUB_ABOUT
#endif
#ifndef UTE_MODULE_SLIDE_BAR_APP_ID1
#define UTE_MODULE_SLIDE_BAR_APP_ID1 FUNC_ALARM_CLOCK
#endif
#ifndef UTE_MODULE_SLIDE_BAR_APP_ID2
#define UTE_MODULE_SLIDE_BAR_APP_ID2 FUNC_SCAN
#endif
#ifndef UTE_MODULE_SLIDE_BAR_APP_ID3
#define UTE_MODULE_SLIDE_BAR_APP_ID3 FUNC_VOICE
#endif
#ifndef UTE_MODULE_SLIDE_BAR_APP_ID4
#define UTE_MODULE_SLIDE_BAR_APP_ID4 FUNC_HEARTRATE
#endif
#ifndef UTE_MODULE_SLIDE_BAR_APP_ID5
#define UTE_MODULE_SLIDE_BAR_APP_ID5 FUNC_BLOOD_OXYGEN
#endif


/* 滑动栏模块ellison add ,2022-Jun-24 9:41 */
ute_module_SlideBar_saveApp_t uteModuleSlideBarShowAppId;

/**
*@brief     uteModuleSlideBarAppIdToScreensIdMap
*@details   侧滑界面最近6个app图标的id和界面id的映射表，appid这里指的是"每一个应用图标的ID"
*注意:这里的 SLIDE_BAR_xxx_ID，必须是按照@slide_bar_id_type_t里面排序，有新的id往下添加
*@author      ellison
*@date        ellison add ,2022-Jul-13
*/
const static int uteModuleSlideBarAppIdToScreensIdMap[][2] =
{
    {SLIDE_BAR_ABOUT_WATCH_ID, FUNC_SET_SUB_ABOUT},        // 0 关于手环界面
    {SLIDE_BAR_LOCAL_ALARM_LIST_ID, FUNC_ALARM_CLOCK},     // 1 闹钟设置界面 AlarmSettings
    {SLIDE_BAR_QR_CODE_ID, FUNC_SCAN},                     // 2 二维码界面
    {SLIDE_BAR_VOICE_ASSISTANT_ID, FUNC_VOICE},            // 3 语音助手
    {SLIDE_BAR_BLOOD_OXYGEN_ID, FUNC_BLOOD_OXYGEN},        // 4 血氧界面 BloodOxygen
    {SLIDE_BAR_BLOOD_PRESSURE_ID, FUNC_BLOOD_PRESSURE},    // 5 血压界面 BloodPressure
    {SLIDE_BAR_BREATH_RATE_ID, NULL},                      // 6 呼吸率
    {SLIDE_BAR_BREATH_TRAINING_ID, FUNC_BREATHE},          // 7 呼吸训练 BreathTraining
    {SLIDE_BAR_BACK_LIGHT_ID, FUNC_LIGHT},                 // 8 亮度调节
    {SLIDE_BAR_CALCULATOR_ID, FUNC_CALCULATOR},            // 9 计算器
    {SLIDE_BAR_WATCH_PREVIEW0_ID, FUNC_CLOCK_PREVIEW},     // 10 预览表盘设置
    {SLIDE_BAR_FATIGUE_ID, NULL},                          // 11 疲劳度
    {SLIDE_BAR_FIND_PHONE_ID, FUNC_FINDPHONE},             // 12 查找手机 FindPhone
    {SLIDE_BAR_FLASHLIGHT_ID, FUNC_FLASHLIGHT},            // 13 手电筒 FlashLight
    {SLIDE_BAR_GAME_LIST_ID, FUNC_GAME},                   // 14 游戏列表
    {SLIDE_BAR_HEART_RATE_ID, FUNC_HEARTRATE},             // 15 心率界面 heartrateScreen
    {SLIDE_BAR_MESSAGE_LIST_ID, FUNC_MESSAGE},             // 16 消息列表 NotifycationList
    {SLIDE_BAR_EMOTION_ID, NULL},                          // 17 情绪
    {SLIDE_BAR_MORE_LIST_ID, NULL},                        // 18 更多功能列表
    {SLIDE_BAR_MUSIC_CONTROL_ID, FUNC_BT},                 // 19 音乐控制 MusicControl
    {SLIDE_BAR_WHETHER_POWEROFF_ID, FUNC_SET_SUB_OFF},     // 20 是否关机
    {SLIDE_BAR_LOCK_SCREEN_ID, FUNC_SET_SUB_PASSWORD},     // 21 锁屏
    {SLIDE_BAR_TELEPHONE_CALL_LIST_ID, FUNC_CALL},         // 22 拨号
    {SLIDE_BAR_PRESSURE_ID, FUNC_PRESSURE},                // 23 压力
    {SLIDE_BAR_SPORTS_HISTORY_LIST_ID, NULL},              // 24 运动记录 SportHistoryList,//UTE_MOUDLE_SCREENS_SPORTS_HISTORY_LIST_ID,
    {SLIDE_BAR_WHETHER_RESET_ID, FUNC_SET_SUB_RSTFY},      // 25 回复出厂设置
    {SLIDE_BAR_DISPLAY_OFF_TIME_ID, FUNC_SET_SUB_DOUSING}, // 26 灭屏时间
    {SLIDE_BAR_FUNCTION_LIST_ID, FUNC_SETTING},            // 27 设置列表 FunctionList
    {SLIDE_BAR_SLEEP_TIME_ID, FUNC_SLEEP},                 // 28 睡眠 SleepTimeDisplay
    {SLIDE_BAR_HOME_STATUS_ID, FUNC_ACTIVITY},             // 29 运动状态界面 homestatue
    {SLIDE_BAR_STOP_WATCH_ID, FUNC_STOPWATCH},             // 30 秒表Stopwatch
    {SLIDE_BAR_TEMPERATURE_ID, NULL},                      // 31 体温界面 Temperature
    {SLIDE_BAR_THEME_CHANGE_ID, FUNC_STYLE},               // 32 主题
    {SLIDE_BAR_COUNT_DOWN_ID, FUNC_TIMER},                 // 33  倒计时
    {SLIDE_BAR_TRAINING_LIST_ID, FUNC_SPORT},              // 34 运动列表界面 trainingList
    {SLIDE_BAR_SOUND_SETTINGS_ID, FUNC_SET_SUB_SAV},       // 35 音量设置
    {SLIDE_BAR_WEATHER_TODAY_ID, FUNC_WEATHER},            // 36 今天天气 WeatherToday
    {SLIDE_BAR_MENSTRUAL_CYCLE_ID, FUNC_WOMEN_HEALTH},     // 37 女性健康
    {SLIDE_BAR_SOS_ID, FUNC_SET_SUB_SOS},                  // 38 SOS
    {SLIDE_BAR_ALIPAY_ID, NULL},                           // 39 支付宝
    {SLIDE_BAR_TAKE_PICTURE_ID, FUNC_CAMERA},              // 40 相机
    {SLIDE_BAR_ECG_ID, NULL},                              // 41 心电
    {SLIDE_BAR_BLOOD_SUGAR_ID, NULL},                      // 42 血糖
};

/**
*@brief        读取参数
*@details
*@author      ellison
*@date        ellison add ,2022-Jul-13
*/
static void uteModuleSlideBarReadConfig(void)
{
    /*! 保存到文件*/
    void *file;
    uint8_t readbuff[24];
    memset(readbuff,0,24);
    // APP_ID
    readbuff[0] = (UTE_MODULE_SLIDE_BAR_APP_ID0>>24)&0xff;
    readbuff[1] = (UTE_MODULE_SLIDE_BAR_APP_ID0>>16)&0xff;
    readbuff[2] = (UTE_MODULE_SLIDE_BAR_APP_ID0>>8)&0xff;
    readbuff[3] = (UTE_MODULE_SLIDE_BAR_APP_ID0)&0xff;

    readbuff[4] = (UTE_MODULE_SLIDE_BAR_APP_ID1>>24)&0xff;
    readbuff[5] = (UTE_MODULE_SLIDE_BAR_APP_ID1>>16)&0xff;
    readbuff[6] = (UTE_MODULE_SLIDE_BAR_APP_ID1>>8)&0xff;
    readbuff[7] = (UTE_MODULE_SLIDE_BAR_APP_ID1)&0xff;

    readbuff[8] = (UTE_MODULE_SLIDE_BAR_APP_ID2>>24)&0xff;
    readbuff[9] = (UTE_MODULE_SLIDE_BAR_APP_ID2>>16)&0xff;
    readbuff[10] = (UTE_MODULE_SLIDE_BAR_APP_ID2>>8)&0xff;
    readbuff[11] = (UTE_MODULE_SLIDE_BAR_APP_ID2)&0xff;

    readbuff[12] = (UTE_MODULE_SLIDE_BAR_APP_ID3>>24)&0xff;
    readbuff[13] = (UTE_MODULE_SLIDE_BAR_APP_ID3>>16)&0xff;
    readbuff[14] = (UTE_MODULE_SLIDE_BAR_APP_ID3>>8)&0xff;
    readbuff[15] = (UTE_MODULE_SLIDE_BAR_APP_ID3)&0xff;

    readbuff[16] = (UTE_MODULE_SLIDE_BAR_APP_ID4>>24)&0xff;
    readbuff[17] = (UTE_MODULE_SLIDE_BAR_APP_ID4>>16)&0xff;
    readbuff[18] = (UTE_MODULE_SLIDE_BAR_APP_ID4>>8)&0xff;
    readbuff[19] = (UTE_MODULE_SLIDE_BAR_APP_ID4)&0xff;

    readbuff[20] = (UTE_MODULE_SLIDE_BAR_APP_ID5>>24)&0xff;
    readbuff[21] = (UTE_MODULE_SLIDE_BAR_APP_ID5>>16)&0xff;
    readbuff[22] = (UTE_MODULE_SLIDE_BAR_APP_ID5>>8)&0xff;
    readbuff[23] = (UTE_MODULE_SLIDE_BAR_APP_ID5)&0xff;

    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SLIDEBAR_SET_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],UTE_MODULE_SLIDEBAR_MAX_APP*4); //sizeof(int) = 4 byte
        uteModuleFilesystemCloseFile(file);
    }

    for(uint8_t i = 0 ; i < UTE_MODULE_SLIDEBAR_MAX_APP; i++)
    {
        uteModuleSlideBarShowAppId.screenIds[i] = ((readbuff[4*i] << 24) & 0xff000000) | ((readbuff[4*i+1] << 16) & 0xff0000) | ((readbuff[4*i+2] << 8) & 0xff00) | (readbuff[4*i+3]  & 0xff);
    }

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,screenId=%d,screenIds =",__func__,uteModuleSlideBarShowAppId.screenIds[0]);
#if UTE_LOG_SYSTEM_LVL
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,(uint8_t *)&uteModuleSlideBarShowAppId.screenIds[0],UTE_MODULE_SLIDEBAR_MAX_APP*4)
#endif

}
/**
*@brief        保存测试参数
*@details
*@author       zn.zeng
*@date       2021-12-04
*/
void uteModuleSlideBarSaveConfig(void)
{
    /*! 保存到文件*/
    void *file;
    uint8_t writebuff[24];
    memset(writebuff,0,24);
    // APP_ID
    for(uint8_t i = 0 ; i < UTE_MODULE_SLIDEBAR_MAX_APP; i++)
    {
        writebuff[4*i + 0] = (uteModuleSlideBarShowAppId.screenIds[i]>>24)&0xff;
        writebuff[4*i + 1] = (uteModuleSlideBarShowAppId.screenIds[i]>>16)&0xff;
        writebuff[4*i + 2] = (uteModuleSlideBarShowAppId.screenIds[i]>>8)&0xff;
        writebuff[4*i + 3] = (uteModuleSlideBarShowAppId.screenIds[i])&0xff;
    }
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_SLIDEBAR_SET_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],UTE_MODULE_SLIDEBAR_MAX_APP*4);
        uteModuleFilesystemCloseFile(file);
    }

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,screenId=%d,writebuff =",__func__,uteModuleSlideBarShowAppId.screenIds[0]);
#if UTE_LOG_SYSTEM_LVL
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,writebuff,UTE_MODULE_SLIDEBAR_MAX_APP*4);
#endif

}

/**
*@brief        初始化函数
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarInit(void)
{
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s, line= %d",__func__,__LINE__);
    uteModuleSlideBarReadConfig();
}

/**
*@brief        设置当前点击的界面ID，到侧边栏显示的AppId中
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarSetCurrentScreenIdToAppIds(int screenId)
{
    int isSameIdIndex = -1;
    uint8_t i = 0;

    bool hasScreenIdInMap = false;
    for(i = 0; i < SLIDE_BAR_TYPE_MAX; i++)
    {
        if(uteModuleSlideBarAppIdToScreensIdMap[i][1] == screenId)
        {
            hasScreenIdInMap = true;
            break;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,screenId=%d,hasScreenIdInMap=%d",__func__,screenId,hasScreenIdInMap);
    if (!hasScreenIdInMap)
    {
        return;
    }

    for(i = 0; i < UTE_MODULE_SLIDEBAR_MAX_APP; i++)
    {
        if(uteModuleSlideBarShowAppId.screenIds[i]==screenId)
        {
            isSameIdIndex = i;
            break;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"isSameIdIndex=%d,",isSameIdIndex);
    if (isSameIdIndex == -1)
    {
        for (i = UTE_MODULE_SLIDEBAR_MAX_APP - 1; i > 0; i--)
        {
            uteModuleSlideBarShowAppId.screenIds[i] = uteModuleSlideBarShowAppId.screenIds[i - 1];
        }
        uteModuleSlideBarShowAppId.screenIds[0] = screenId;
    }
    else if (isSameIdIndex == 0)
    {
        return;
    }
    else
    {
        for(i = isSameIdIndex; i >0 ; i--)
        {
            uteModuleSlideBarShowAppId.screenIds[i] = uteModuleSlideBarShowAppId.screenIds[i - 1];
        }
        uteModuleSlideBarShowAppId.screenIds[0] = screenId;
    }

    uteModuleSlideBarSaveConfig();
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"screenId=%d,",screenId);
#if UTE_LOG_SYSTEM_LVL
    UTE_MODULE_LOG_BUFF(UTE_LOG_SYSTEM_LVL,(uint8_t *)uteModuleSlideBarShowAppId.screenIds,UTE_MODULE_SLIDEBAR_MAX_APP*4);
#endif
}

/**
*@brief          获取当前要显示的所有的界面id
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarGetScreenIds(int *screenIds)
{
    memcpy(screenIds,&uteModuleSlideBarShowAppId.screenIds[0],sizeof(int)*UTE_MODULE_SLIDEBAR_MAX_APP);
}

/**
*@brief        获取当前要显示的APPid
*@details
*@input  int screenId ,界面的ID
*@return int appId
*@author       ellison
*@date       2022-06-24
*/
int uteModuleSlideBarGetAppIdFormScreenId(int screenId)
{
    int appId =0;
    for(uint8_t i = 0; i < SLIDE_BAR_TYPE_MAX; i++ )
    {
        if(uteModuleSlideBarAppIdToScreensIdMap[i][1] ==screenId)
        {
            appId = uteModuleSlideBarAppIdToScreensIdMap[i][0];
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,appId = %d,screenId=%d",__func__,appId,screenId);
            return appId;
        }
    }
    return appId;
}

#endif //UTE_MODULE_BLOODOXYGEN_SUPPORT

