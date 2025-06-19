#include "func.h"
#include "func_menu.h"

typedef struct f_menu_ui_data_
{
    u8 func_sta;                            //普通菜单模式
    u32 str_idx;                                //文字
    u32 res_addr;                               //图标
    u32 fb_res_addr;                            //图标->球形菜单
} f_menu_ui_data;

#ifndef UTE_MENU_APP_MAX_CNT
#define MENU_APP_MAX_CNT     50
#else
#define MENU_APP_MAX_CNT     UTE_MENU_APP_MAX_CNT
#endif

#define MENU_APP_CNT                       ((int)(sizeof(f_menu_ui_data_all) / sizeof(f_menu_ui_data_all[0])))

#if GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
const static f_menu_ui_data f_menu_ui_data_all[] =//菜单全部资源
{
#if UTE_MODULE_SCREENS_CALL_SUPPORT//通话
    {.func_sta=FUNC_CALL,          .str_idx=STR_PHONE,                .res_addr=UI_BUF_I338001_2_HONEYCOMB_CALL_BIN,      .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_CALL_BIN,  },
#endif
#if UTE_MODULE_SCREENS_TIMER_SUPPORT   //计时器
    {.func_sta=FUNC_TIMER,         .str_idx=STR_TIMER,                .res_addr=UI_BUF_I338001_2_HONEYCOMB_TIMER_BIN,     .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_V_BIN,     },
#endif
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT//每日活动
    {.func_sta=FUNC_ACTIVITY,      .str_idx=STR_EVREY_DAY_ACTIVITY,   .res_addr=UI_BUF_I338001_2_HONEYCOMB_ACTIVITY_BIN,  .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_ACTIVITY_BIN },
#endif
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT//心率
    {.func_sta=FUNC_HEARTRATE,     .str_idx=STR_HEART_RATE,           .res_addr=UI_BUF_I338001_2_HONEYCOMB_HEART_BIN,     .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_HEART_BIN    },
#endif
#if UTE_MODULE_SCREENS_PRESSURE_SUPPORT//压力
    {.func_sta=FUNC_PRESSURE,      .str_idx=STR_STRESS,               .res_addr=UI_BUF_I338001_2_HONEYCOMB_STRESS_BIN,    .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_PRESSURE_BIN,  },
#endif
#if UTE_MODULE_SCREENS_SPORT_SUPPORT //运动
    {.func_sta=FUNC_SPORT,         .str_idx=STR_SPORTS,               .res_addr=UI_BUF_I338001_2_HONEYCOMB_SOPRT_BIN,     .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_SOPRT_BIN, },
#endif
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT  //血氧
    {.func_sta=FUNC_BLOOD_OXYGEN,  .str_idx=STR_BLOOD_OXYGEN,         .res_addr=UI_BUF_I338001_2_HONEYCOMB_SPO2_BIN,      .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_SPO2_BIN,    },
#endif
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT  //睡眠
    {.func_sta=FUNC_SLEEP,         .str_idx=STR_SLEEP,                .res_addr=UI_BUF_I338001_2_HONEYCOMB_SLEEP_BIN,     .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_SLEEP_BIN,    },
#endif
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT  //音乐
    {.func_sta=FUNC_BT,            .str_idx=STR_MUSIC,                .res_addr=UI_BUF_I338001_2_HONEYCOMB_MUSIC_BIN,     .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_MUSIC_BIN,    },
#endif
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT  //天气
    {.func_sta=FUNC_WEATHER,       .str_idx=STR_MUSIC,                .res_addr=UI_BUF_I338001_2_HONEYCOMB_WEATHER_BIN,   .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_WEATHER_BIN,    },
#endif
#if UTE_MODULE_SCREENS_ALARM_SUPPORT  //闹钟
    {.func_sta=FUNC_ALARM_CLOCK,   .str_idx=STR_ALARM_CLOCK,          .res_addr=UI_BUF_I338001_2_HONEYCOMB_ALARM_BIN,     .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_ALARM_BIN,    },
#endif
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT  //呼吸
    {.func_sta=FUNC_BREATHE,       .str_idx=STR_BREATHE_TRAIN,        .res_addr=UI_BUF_I338001_2_HONEYCOMB_BREATHING_BIN, .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_BREATHE_BIN,    },
#endif
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT  //消息
    {.func_sta=FUNC_MESSAGE,       .str_idx=STR_MESSAGE,              .res_addr=UI_BUF_I338001_2_HONEYCOMB_MESSAGE_BIN, .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_MESSAGE_BIN,    },
#endif
#if UTE_MODULE_SCREENS_VOICE_SUPPORT  //语音助手
    {.func_sta=FUNC_VOICE,         .str_idx=STR_VOICE,                .res_addr=UI_BUF_I338001_2_HONEYCOMB_VOICE_ASSISTANT_BIN, .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_VIOCE_BIN,    },
#endif
#if UTE_MODULE_SCREENS_TOOLBOX_SUPPORT  //工具箱
    {.func_sta=FUNC_TOOLBOX,       .str_idx=STR_TOOL_BOX,             .res_addr=UI_BUF_I338001_2_HONEYCOMB_TOOLBOX_BIN,     .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_MORE_BIN,    },
#endif
#if UTE_MODULE_SCREENS_SETTING_SUPPORT  //设置
    {.func_sta=FUNC_SETTING,       .str_idx=STR_SETTING,              .res_addr=UI_BUF_I338001_2_HONEYCOMB_SETTINGS_BIN,    .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_SETTINGS_BIN,    },
#endif
#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT //女性健康
    {.func_sta=FUNC_WOMEN_HEALTH,  .str_idx=STR_WOMEN_HEALTH,         .res_addr=UI_BUF_I338001_2_HONEYCOMB_PERIOD_BIN,      .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_SETTINGS_BIN,    },
#endif
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT//秒表
    {.func_sta=FUNC_STOPWATCH,     .str_idx=STR_STOP_WATCH,           .res_addr=UI_BUF_I338001_2_HONEYCOMB_STOPWATCH_BIN,   .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_SETTINGS_BIN,   },
#endif
#if UTE_MODULE_SCREENS_TIMER_SUPPORT//计时器
    {.func_sta=FUNC_TIMER,         .str_idx=STR_STOP_WATCH,           .res_addr=UI_BUF_I338001_2_HONEYCOMB_TIMER_BIN,       .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_V_BIN,   },
#endif
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT//查找手机
    {.func_sta=FUNC_FINDPHONE,     .str_idx=STR_FIND_PHONE,           .res_addr=UI_BUF_I338001_2_HONEYCOMB_FINDPHONE_BIN,   .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_FINDPHONE_BIN,   },
#endif
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT//相机
    {.func_sta=FUNC_CAMERA,        .str_idx=STR_CAMERA,               .res_addr=UI_BUF_I338001_2_HONEYCOMB_PHOTO_BIN,       .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_PHOTO_BIN   },
#endif
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT//计算器
    {.func_sta=FUNC_CALCULATOR,    .str_idx=STR_CALCULATOR,           .res_addr=UI_BUF_I338001_2_HONEYCOMB_CALCULATOR_BIN,  .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_CALCULATOR_BIN   },
#endif
#if UTE_MODULE_SCREENS_EMOTION_SUPPORT  //情绪
    {.func_sta=FUNC_MOOD,          .str_idx=STR_EMOTION,              .res_addr=UI_BUF_I338001_2_HONEYCOMB_MOOD_BIN,        .fb_res_addr=0,    },
#endif
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT//手电筒
    {.func_sta=FUNC_FLASHLIGHT,    .str_idx=STR_FLASHLIGHT,           .res_addr=UI_BUF_I338001_2_HONEYCOMB_FLASHLIGHT_BIN,  .fb_res_addr=0   },
#endif
};
#else
const static f_menu_ui_data f_menu_ui_data_all[] =//菜单全部资源
{
    {0},
}
#endif

//
f_menu_ui_data f_menu_data[MENU_APP_MAX_CNT];








