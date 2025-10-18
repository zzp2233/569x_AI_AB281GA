#include "func_menu_ui_data.h"
#include "func.h"
#include "func_menu.h"
#include "ute_module_menstrualcycle.h"

#if UTE_MODULE_SCREENS_MENU_DATA_BIND


/*! 注释：使用f_menu_ui_data_all结构体里面的 fb_res_addr 前20个数据为足球菜单资源*/

#if GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
const f_menu_ui_data f_menu_ui_data_all[] =//菜单全部资源
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
    {.func_sta=FUNC_WEATHER,       .str_idx=STR_WEATHER,              .res_addr=UI_BUF_I338001_2_HONEYCOMB_WEATHER_BIN,   .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_WEATHER_BIN,    },
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
    {.func_sta=FUNC_WOMEN_HEALTH,  .str_idx=STR_WOMEN_HEALTH,         .res_addr=UI_BUF_I338001_2_HONEYCOMB_PERIOD_BIN,      .fb_res_addr=0,    },
#endif
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT//秒表
    {.func_sta=FUNC_STOPWATCH,     .str_idx=STR_STOP_WATCH,           .res_addr=UI_BUF_I338001_2_HONEYCOMB_STOPWATCH_BIN,   .fb_res_addr=UI_BUF_I338001_28_SET_MENU_FOOTBALL_STOPWATCH_BIN,   },
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
const f_menu_ui_data f_menu_ui_data_all[] =//菜单全部资源
{
    {0},
}
#endif

f_menu_ui_data f_menu_data[MENU_APP_MAX_CNT];
/*! 菜单UI数据初始化->重新加载菜单应用数量排序 */
void f_menu_ui_data_init(void)
{
    memset(f_menu_data,0,sizeof(f_menu_data));
    int j=0;
    for(int i=0; i<MENU_APP_MAX_CNT; i++)
    {
#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT //女性健康
        if(f_menu_ui_data_all[i].func_sta == FUNC_WOMEN_HEALTH && !uteModuleMenstrualCycleIsOpen())
        {
            j++;
        }
#endif
        if(j < MENU_APP_MAX_CNT)
        {
            f_menu_data[i] = f_menu_ui_data_all[j];
            j++;
        }
    }
}


/*!
入口参数：index:菜单类型
出口参数：菜单类型app数量
*/
u8 f_menu_ui_data_get_app_num(void)
{
    u8 app_num = MENU_APP_MAX_CNT;
    switch (func_cb.menu_style)
    {
        case MENU_STYLE_LIST:      /*!  列表菜单 显示应用最多数量*/
            app_num = MENU_STYLE_LIST_APP_MAX_NUM;
            break;
        case MENU_STYLE_CUM_SUDOKU:/*!  宫格菜单 显示应用最多数量*/
            app_num = MENU_STYLE_CUM_SUDOKU_APP_MAX_NUM;
            break;
        case MENU_STYLE_HONEYCOMB:/*!  蜂窝菜单 显示应用最多数量*/
            app_num = MENU_STYLE_HONEYCOMB_APP_MAX_NUM;
            break;
        case MENU_STYLE_FOOTBALL: /*!  球体菜单 显示应用最多数量*/
            app_num = MENU_STYLE_FOOTBALL_APP_MAX_NUM;
            break;
        case MENU_STYLE_GRID:     /*!  棋盘菜单 显示应用最多数量*/
            app_num = MENU_STYLE_GRID_APP_MAX_NUM;
            break;
        case MENU_STYLE_KALE:     /*!  光环菜单 显示应用最多数量*/
            app_num = MENU_STYLE_KALE_APP_MAX_NUM;
            break;
        case MENU_STYLE_SKYRER:     /*! 天圆菜单 显示应用最多数量*/
            app_num = MENU_STYLE_SKYRER_APP_MAX_NUM;
            break;
        case MENU_STYLE_CUM_GRID:     /*! 网格菜单 显示应用最多数量*/
            app_num = MENU_STYLE_CUM_GRID_APP_MAX_NUM;
            break;
        case MENU_STYLE_WATERFALL:     /*! 瀑布菜单 显示应用最多数量*/
            app_num = MENU_STYLE_WATERFALL_APP_MAX_NUM;
            break;
#if UTE_MENU_STYLE_CUM_RING1_FUNCTION
        case MENU_STYLE_CUM_RING1:     /*! 环形菜单风格1 显示应用最多数量*/
            app_num = MENU_MENU_STYLE_CUM_RING1_APP_MAX_NUM;
            break;
#endif
#if UTE_MENU_STYLE_CUM_RING2_FUNCTION
        case MENU_STYLE_CUM_RING2:     /*! 环形菜单风格2 显示应用最多数量*/
            app_num = MENU_MENU_STYLE_CUM_RING2_APP_MAX_NUM;
            break;
#endif
        default:
            break;
    }
#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT //女性健康
    if(!uteModuleMenstrualCycleIsOpen())
    {
        app_num--;
    }
#endif
    return app_num;
}

/*!
出口参数：任务号获取资源
*/
f_menu_ui_data f_menu_find_ui_data(u8 func_sta)
{
    f_menu_ui_data ui_data;
    memset(&ui_data, 0, sizeof(f_menu_ui_data));
    for (int index = 0; index < MENU_APP_MAX_CNT; index++)
    {
        printf("index:[%d] sta:[%d,%d]\n",index,f_menu_ui_data_all[index].func_sta,func_sta);
        if(f_menu_ui_data_all[index].func_sta == func_sta)
        {
            ui_data.func_sta    = func_sta;
            ui_data.res_addr    = f_menu_ui_data_all[index].res_addr;
            ui_data.str_idx     = f_menu_ui_data_all[index].str_idx;
            ui_data.fb_res_addr = f_menu_ui_data_all[index].fb_res_addr;
            return ui_data;
        }
    }
    return ui_data;
}

/**
 * @brief 最近任务列表资源
 * @param[in] num : 获取最近任务资源数量
 * @return 获取最近任务资源结构体
 **/
void func_get_latest_task_data(u8 num,f_menu_ui_data *ui_data)
{
    u8 func_sta = FUNC_NULL;
    u8 latest_cnt = latest_task_count();
    u8 index = 0;
    f_menu_ui_data_init();

#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT
    if(!uteModuleMenstrualCycleIsOpen())
    {
        if (latest_task_find(FUNC_WOMEN_HEALTH) != -1)
        {
            uteTaskGuiStackRemoveScreenId(FUNC_WOMEN_HEALTH);
        }
    }
#endif

    for (u8 index = 0; index < num; index++)
    {
        if(latest_cnt > index)
        {
            func_sta = latest_task_get(index);
        }
        else
        {
            for (index; index <= num; index++)
            {
                if (latest_task_find(f_menu_data[index].func_sta) == -1)
                {
                    func_sta = f_menu_data[index].func_sta;
                    break;
                }
            }
        }
        printf("func_sta = %d\n", func_sta);
        ui_data[index] = f_menu_find_ui_data(func_sta);
    }
}

#endif






