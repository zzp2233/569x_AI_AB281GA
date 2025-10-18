#include "include.h"
#include "func.h"
#include "func_menu.h"
#include "func_menu_ui_data.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

int menuCnt = 0;
Cooperation_menu_item_t coo_menu_list[] =
{
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {.func_sta=FUNC_SPORT,                      .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SOPRT_BIN,             .str_id=STR_SPORTS},                //运动
#endif
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
    {.func_sta=FUNC_SPORT_HISTORY,              .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SPORTS_RECORD_BIN,     .str_id=STR_EXERCISE_RECODE},                //运动记录
#endif
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {.func_sta=FUNC_ACTIVITY,                   .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_ACTIVITY_BIN,          .str_id=STR_EVREY_DAY_ACTIVITY},    //每日活动
#endif
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {.func_sta=FUNC_VOICE,                      .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_VOICE_ASSISTANT_BIN,   .str_id=STR_VOICE},                 //语音助手
#endif
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {.func_sta=FUNC_MESSAGE,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_MESSAGE_BIN,           .str_id=STR_MESSAGE},               //消息
#endif
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {.func_sta=FUNC_CALL,                       .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_CALL_BIN,              .str_id=STR_PHONE},                 //电话
#endif
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {.func_sta=FUNC_BT,                         .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_MUSIC_BIN,             .str_id=STR_MUSIC},                 //音乐
#endif
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {.func_sta=FUNC_HEARTRATE,                  .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_HEART_BIN,             .str_id=STR_HEART_RATE},            //心率
#endif
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {.func_sta=FUNC_SLEEP,                      .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SLEEP_BIN,             .str_id=STR_SLEEP},                 //睡眠
#endif
#if UTE_MODULE_SCREENS_MEASURE_SUPPORT
    {.func_sta = FUNC_MEASURE,                  .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_KEY_MEASURE_BIN,         .str_id=STR_MEASURE},       //一键测量
#endif
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {.func_sta=FUNC_BLOOD_OXYGEN,               .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SPO2_BIN,              .str_id=STR_BLOOD_OXYGEN},          //血氧
#endif
#if UTE_MODULE_SCREENS_PRESSURE_SUPPORT
    {.func_sta=FUNC_PRESSURE,                   .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_STRESS_BIN,            .str_id=STR_STRESS},               //压力
#endif
#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT
    {.func_sta=FUNC_WOMEN_HEALTH,               .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PERIOD_BIN,            .str_id=STR_WOMEN_HEALTH},          //女性健康
#endif
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {.func_sta=FUNC_BREATHE,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_BREATHING_BIN,         .str_id=STR_BREATHE_TRAIN},         //呼吸
#endif
#if APP_CUSTOM_REMIND_SETTING_SUPPORT
    {.func_sta = FUNC_REMIND_PRAYER,            .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PRAY_BIN,           .str_id=STR_PRAYER_REMIND},         //祈祷
#endif
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    {.func_sta = FUNC_CAMERA,                   .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PHOTO_BIN,          .str_id=STR_CAMERA},     //遥控拍照
#endif
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {.func_sta=FUNC_WEATHER,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_WEATHER_BIN,           .str_id=STR_WEATHER},               //天气
#endif
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {.func_sta=FUNC_ALARM_CLOCK,                .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_ALARM_BIN,             .str_id=STR_ALARM_CLOCK},           //闹钟
#endif
#if UTE_MODULE_SCREENS_WORLD_CLOCK_SUPPORT
    {.func_sta = FUNC_WORLD_CLOCK,              .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_WORLD_TIME_BIN,        .str_id=STR_WORLD_CLOCK},          //世界时钟
#endif
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    {.func_sta = FUNC_STOPWATCH,                .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_STOPWATCH_BIN,       .str_id=STR_STOP_WATCH},  //秒表
#endif
#if UTE_MODULE_SCREENS_TIMER_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    {.func_sta = FUNC_TIMER,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_TIMER_BIN,          .str_id= STR_TIMER},     //计时器
#endif
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    {.func_sta = FUNC_CALCULATOR,               .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_CALCULATOR_BIN,     .str_id= STR_CALCULATOR}, //计算器
#endif
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    {.func_sta = FUNC_FINDPHONE,                .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_FINDPHONE_BIN,       .str_id=STR_FIND_PHONE},  //找手机
#endif
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {.func_sta = FUNC_GAME,                     .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_GAME_BIN,              .str_id=STR_GAME},          //游戏
#endif
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    {.func_sta = FUNC_FLASHLIGHT,               .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_FLASHLIGHT_BIN,     .str_id= STR_FLASHLIGHT}, //手电筒
#endif
#if 0//UTE_MODULE_SCREENS_SUB_SOS_SUPPORT
    {.func_sta=FUNC_SUB_SOS,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SOS_BIN,               .str_id=STR_SOS},                  //SOS
#endif
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {.func_sta = FUNC_SETTING,                  .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SETTINGS_BIN,          .str_id=STR_SETTING},               //设置
#endif
#if UTE_MODULE_SCREENS_TOOLBOX_SUPPORT
    {.func_sta=FUNC_TOOLBOX,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_TOOLBOX_BIN,           .str_id=STR_TOOL_BOX},             //工具箱
#endif
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    {.func_sta = FUNC_CALENDAER,                .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PHOTO_BIN,          .str_id= STR_CALENDAR}, //日历
#endif
#if UTE_MODULE_SCREENS_EMOTION_SUPPORT
    {.func_sta = FUNC_MOOD,                     .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_MOOD_BIN,              .str_id=STR_EMOTION},              //情绪
#endif
};
//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_form_create(void)
{

#if UTE_MODULE_SCREENS_MENU_DATA_BIND
    f_menu_ui_data_init();
#endif
    switch (func_cb.menu_style)
    {
        case MENU_STYLE_CUM_HONEYGRID:
        case MENU_STYLE_HONEYCOMB:
            return func_menu_sub_honeycomb_form_create();

        case MENU_STYLE_WATERFALL:
            return func_menu_sub_waterfall_form_create();

        case MENU_STYLE_FOOTBALL:
            return func_menu_sub_football_form_create();

        case MENU_STYLE_LIST:
        case MENU_STYLE_CUM_FOURGRID:
            return func_menu_sub_list_form_create();

        case MENU_STYLE_SUDOKU:
        case MENU_STYLE_SUDOKU_HRZ:
            return func_menu_sub_sudoku_form_create();

        case MENU_STYLE_GRID:
        case MENU_STYLE_CUM_GRID:
            return func_menu_sub_grid_form_create();

        case MENU_STYLE_DISK:
            return func_menu_sub_disk_form_create();

        case MENU_STYLE_RING:
            return func_menu_sub_ring_form_create();

        case MENU_STYLE_KALE:
            return func_menu_sub_kale_form_create();

        case MENU_STYLE_SKYRER:
            return func_menu_sub_skyrer_form_create();

        case MENU_STYLE_CUM_SUDOKU:
            return func_menu_sub_cum_sudoku_form_create();

        case MENU_STYLE_CUM_HEXAGON:
            return func_menu_sub_hexagon_form_create();
#if UTE_MENU_STYLE_CUM_RING1_FUNCTION
        case MENU_STYLE_CUM_RING1:
            return func_menu_sub_cum_ring1_form_create();
#endif
#if UTE_MENU_STYLE_CUM_RING2_FUNCTION
        case MENU_STYLE_CUM_RING2:
            return func_menu_sub_cum_ring2_form_create();
#endif
        default:
            halt(HALT_FUNC_MENU_SUBSTA);
            return NULL;
    }
}

#if UTE_MENU_STYLE_DOUBLE_NEXT_ENABLE
#define MENU_CNT                       sizeof(SWITCH_NEXT_MENU) / sizeof(SWITCH_NEXT_MENU[0])
const u8 SWITCH_NEXT_MENU[]=UTE_CUI_SCREEN_MENU_STYLE;
static void func_menu_sub_switch_next(void)
{
    int idx;
    for(idx=0; idx<MENU_CNT; idx++)
    {
        if(SWITCH_NEXT_MENU[idx] == func_cb.menu_style)
        {
            idx++;
            if(idx == MENU_CNT)
            {
                idx = 0;
            }
            func_cb.menu_style = SWITCH_NEXT_MENU[idx];
            if (func_cb.menu_style == MENU_STYLE_SKYRER)
            {
                u8 func_menu_sub_skyrer_get_first_idx(void);
                func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
            }
            else
            {
                func_cb.menu_idx = 0;           //切换风格后进入回中心位置
            }
            uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
            return;
        }
    }
}
#endif

//菜单样式公用消息
void func_menu_sub_message(size_msg_t msg)
{
    switch (msg)
    {
        case KU_BACK:
        case KU_DELAY_BACK:
            //不响应
            break;

        case KD_BACK:
#if UTE_MODULE_SCREENS_ROTARY_MENUSTYLE_SUPPORT
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_cb.sta = FUNC_MENUSTYLE;
            }
#elif UTE_MENU_STYLE_DOUBLE_NEXT_ENABLE
            func_menu_sub_switch_next();
#endif
            break;

        default:
            func_message(msg);
            break;
    }
}

//退出菜单样式
void func_menu_sub_exit(void)
{
    compo_form_destroy(func_cb.frm_main);
    func_free(func_cb.f_cb);
    func_cb.frm_main = NULL;
    func_cb.f_cb = NULL;
    tft_set_temode(DEFAULT_TE_MODE);
}


//进入主菜单功能
static void func_menu_enter(void)
{
    if (uteModuleGuiCommonGetLastScreenId() == FUNC_STYLE)
    {
        task_stack_push(FUNC_CLOCK);
        task_stack_push(FUNC_MENU);
    }
}

//退出主菜单功能
static void func_menu_exit(void)
{
    func_cb.last = FUNC_MENU;
}

//主菜单功能
void func_menu(void)
{
    printf("%s\n", __func__);
    func_menu_enter();
    while (func_cb.sta == FUNC_MENU)
    {
        printf("func_cb.menu_style:[%d]\n",func_cb.menu_style);
        switch (func_cb.menu_style)
        {
            case MENU_STYLE_CUM_HONEYGRID:
            case MENU_STYLE_HONEYCOMB:
                func_menu_sub_honeycomb();
                break;

            case MENU_STYLE_WATERFALL:
                func_menu_sub_waterfall();
                break;

            case MENU_STYLE_FOOTBALL:
                func_menu_sub_football();
                break;

            case MENU_STYLE_LIST:
            case MENU_STYLE_CUM_FOURGRID:
                func_menu_sub_list();
                break;

            case MENU_STYLE_SUDOKU:
            case MENU_STYLE_SUDOKU_HRZ:
                func_menu_sub_sudoku();
                break;

            case MENU_STYLE_GRID:
            case MENU_STYLE_CUM_GRID:
                func_menu_sub_grid();
                break;

            case MENU_STYLE_DISK:
                func_menu_sub_disk();
                break;

            case MENU_STYLE_RING:
                func_menu_sub_ring();
                break;

            case MENU_STYLE_KALE:
                func_menu_sub_kale();
                break;

            case MENU_STYLE_SKYRER:
                func_menu_sub_skyrer();
                break;

            case MENU_STYLE_CUM_SUDOKU:
                func_menu_sub_cum_sudoku();
                break;

            case MENU_STYLE_CUM_HEXAGON:
                func_menu_sub_hexagon();
                break;
#if UTE_MENU_STYLE_CUM_RING1_FUNCTION
            case MENU_STYLE_CUM_RING1:
                func_menu_sub_cum_ring1();
                break;
#endif
#if UTE_MENU_STYLE_CUM_RING2_FUNCTION
            case MENU_STYLE_CUM_RING2:
                func_menu_sub_cum_ring2();
                break;
#endif
            default:
                halt(HALT_FUNC_MENU_SUBSTA);
                break;
        }
    }
    func_menu_exit();
}
