/**
*@file
*@brief     滑动栏模块,包括侧边栏和上边栏
*@details
*@author        ellison
*@date        ellison add ,2022-Jun-24 9:34
*@version       v1.0
*/
#ifndef _UTE_MODULE_SLIDE_BAR_H_
#define _UTE_MODULE_SLIDE_BAR_H_

#include "ute_application_common.h"
#include "ute_project_config.h"
#include "ute_module_platform.h"


#if UTE_MODULE_SLIDE_BAR_SUPPORT

#ifndef UTE_MODULE_SLIDEBAR_MAX_APP
#define UTE_MODULE_SLIDEBAR_MAX_APP 6 //最大保存6个最近启动的APP
#endif
/*!写入参数 ellison  */
#define UTE_MODULE_FILESYSTEM_SYSTEMPARM_SLIDEBAR_SET_INFO "systemparam/slideBarInfo"
/* ellison add ,2022-Jul-13
*注意:这里的 SLIDE_BAR_xxx_ID，必须是按照@slide_bar_id_type_t里面排序，有新的id往下添加
*/
typedef enum
{
    SLIDE_BAR_ABOUT_WATCH_ID = 0x00,    //关于手环界面
    SLIDE_BAR_LOCAL_ALARM_LIST_ID,      // 闹钟设置界面 AlarmSettings
    SLIDE_BAR_QR_CODE_ID,               //二维码界面
    SLIDE_BAR_VOICE_ASSISTANT_ID,       //语音助手
    SLIDE_BAR_BLOOD_OXYGEN_ID,          // 血氧界面 BloodOxygen
    SLIDE_BAR_BLOOD_PRESSURE_ID = 5,    // 血压界面 BloodPressure
    SLIDE_BAR_BREATH_RATE_ID,           //呼吸率
    SLIDE_BAR_BREATH_TRAINING_ID,       // 呼吸训练 BreathTraining
    SLIDE_BAR_BACK_LIGHT_ID,            //亮度调节
    SLIDE_BAR_CALCULATOR_ID,            //计算器
    SLIDE_BAR_WATCH_PREVIEW0_ID = 10,   //预览表盘设置
    SLIDE_BAR_FATIGUE_ID,               //疲劳度
    SLIDE_BAR_FIND_PHONE_ID,            // 查找手机 FindPhone
    SLIDE_BAR_FLASHLIGHT_ID,            // 手电筒 FlashLight
    SLIDE_BAR_GAME_LIST_ID,             //游戏列表
    SLIDE_BAR_HEART_RATE_ID = 15,       // 心率界面 heartrateScreen
    SLIDE_BAR_MESSAGE_LIST_ID,          // 消息列表 NotifycationList
    SLIDE_BAR_EMOTION_ID,               //情绪
    SLIDE_BAR_MORE_LIST_ID,             //更多功能列表
    SLIDE_BAR_MUSIC_CONTROL_ID,         // 音乐控制 MusicControl
    SLIDE_BAR_WHETHER_POWEROFF_ID = 20, //是否关机
    SLIDE_BAR_LOCK_SCREEN_ID,           //锁屏
    SLIDE_BAR_TELEPHONE_CALL_LIST_ID,   //拨号
    SLIDE_BAR_PRESSURE_ID,              // 压力
    SLIDE_BAR_SPORTS_HISTORY_LIST_ID,   //运动记录 SportHistoryList
    SLIDE_BAR_WHETHER_RESET_ID = 25,    // 回复出厂设置
    SLIDE_BAR_DISPLAY_OFF_TIME_ID,      //灭屏时间
    SLIDE_BAR_FUNCTION_LIST_ID,         // 设置列表 FunctionList
    SLIDE_BAR_SLEEP_TIME_ID,            // 睡眠 SleepTimeDisplay
    SLIDE_BAR_HOME_STATUS_ID,           // 运动状态界面 homestatue
    SLIDE_BAR_STOP_WATCH_ID = 30,       // 秒表Stopwatch
    SLIDE_BAR_TEMPERATURE_ID,           //体温界面 Temperature
    SLIDE_BAR_THEME_CHANGE_ID,          //主题
    SLIDE_BAR_COUNT_DOWN_ID,            // 倒计时
    SLIDE_BAR_TRAINING_LIST_ID,         // 运动列表界面 trainingList
    SLIDE_BAR_SOUND_SETTINGS_ID = 35,   //音量设置
    SLIDE_BAR_WEATHER_TODAY_ID,         // 今天天气 WeatherToday
    SLIDE_BAR_MENSTRUAL_CYCLE_ID,       //女性健康
    SLIDE_BAR_SOS_ID,                   // SOS
    SLIDE_BAR_ALIPAY_ID,                // 支付宝
    SLIDE_BAR_TAKE_PICTURE_ID = 40,     // 相机
    SLIDE_BAR_ECG_ID,                   //心电
    SLIDE_BAR_BLOOD_SUGAR_ID,           //血糖
    //

    // ...
    SLIDE_BAR_TYPE_MAX,
    // ...
} slide_bar_id_type_t;

typedef struct
{
    int screenIds[UTE_MODULE_SLIDEBAR_MAX_APP]; // 界面的id，用于存储之前从哪个APP跳转到了对应界面，最大保存6个id,这里的排序从0开始显示,即ids[0]是最近一个
} ute_module_SlideBar_saveApp_t;

/**
*@brief        初始化函数
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarInit(void);

/**
*@brief        设置当前点击的界面ID，到侧边栏显示的AppId中
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarSetCurrentScreenIdToAppIds(int screenId);

/**
*@brief        获取当前要显示的所有的界面id
*@details
*@author       ellison
*@date       2022-06-24
*/
void uteModuleSlideBarGetScreenIds(int *screenIds);

/**
*@brief        获取当前要显示的APPid
*@details
*@input  int screenId ,界面的ID
*@return int appId
*@author       ellison
*@date       2022-06-24
*/
int uteModuleSlideBarGetAppIdFormScreenId(int screenId);

#endif

#endif
