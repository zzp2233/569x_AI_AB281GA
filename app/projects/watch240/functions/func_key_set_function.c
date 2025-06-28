#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "api_mem.h"
#include "compo_listbox.h"

#if UTE_MODULE_KEY_SET_FUNCTION_SUPPORT
#include "ute_module_keysetfunc.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define SET_LIST_CNT                       ((int)(sizeof(tbl_menu_list) / sizeof(tbl_menu_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};


typedef struct f_key_set_list_t_
{
    compo_listbox_t *listbox;

} f_key_set_list_t;

u8 func_sel_SetFunc_bit(uint n)
{
    if(uteModuleKeySetFuncGetMenu() == n)
    {
        return true;
    }

    return false;
}

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

static const compo_listbox_item_t tbl_menu_list[] =
{
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {.func_sta=FUNC_FLASHLIGHT,               .res_addr=UI_BUF_I335001_2_HONEYCOMB_FLASHLIGHT_BIN,            .str_idx=STR_FLASHLIGHT,          .vidx = FUNC_FLASHLIGHT},    //手电筒
#endif
#if UTE_MODULE_SCREENS_SUB_SOS_SUPPORT
    {.func_sta=FUNC_SUB_SOS,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_SOS_BIN,                 .str_idx=STR_SOS,                 .vidx = FUNC_SUB_SOS},     //SOS
#endif
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {.func_sta=FUNC_CALL,                     .res_addr=UI_BUF_I335001_2_HONEYCOMB_CALL_BIN,                  .str_idx=STR_PHONE,               .vidx = FUNC_CALL},    //电话
#endif //
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {.func_sta=FUNC_ACTIVITY,                 .res_addr=UI_BUF_I335001_2_HONEYCOMB_ACTIVITY_BIN,              .str_idx=STR_EVREY_DAY_ACTIVITY,  .vidx = FUNC_ACTIVITY}, //每日活动
#endif //
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {.func_sta=FUNC_HEARTRATE,                .res_addr=UI_BUF_I335001_2_HONEYCOMB_HEART_RATE_BIN,            .str_idx=STR_HEART_RATE,          .vidx = FUNC_HEARTRATE},   //心率
#endif //
#if UTE_MODULE_SCREENS_PRESSURE_SUPPORT
    {.func_sta=FUNC_PRESSURE,                 .res_addr=UI_BUF_I335001_2_HONEYCOMB_STRESS_BIN,                .str_idx=STR_STRESS,              .vidx = FUNC_PRESSURE},   //压力
#endif //
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {.func_sta=FUNC_SPORT,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_EXERCISE_BIN,              .str_idx=STR_SPORTS,              .vidx = FUNC_SPORT},   //运动
#endif //
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {.func_sta=FUNC_BLOOD_OXYGEN,             .res_addr=UI_BUF_I335001_2_HONEYCOMB_BLOOD_OXYGEN_BIN,          .str_idx=STR_BLOOD_OXYGEN,        .vidx = FUNC_BLOOD_OXYGEN},   //血氧
#endif //
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {.func_sta=FUNC_SLEEP,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_SLEEP_BIN,                 .str_idx=STR_SLEEP,               .vidx = FUNC_SLEEP},  //睡眠
#endif //
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {.func_sta=FUNC_BT,                       .res_addr=UI_BUF_I335001_2_HONEYCOMB_MUSIC_BIN,                 .str_idx=STR_MUSIC,               .vidx = FUNC_BT},  //音乐
#endif //
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {.func_sta=FUNC_WEATHER,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_WEATHER_BIN,               .str_idx=STR_WEATHER,             .vidx = FUNC_WEATHER},  //天气
#endif //
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {.func_sta=FUNC_ALARM_CLOCK,              .res_addr=UI_BUF_I335001_2_HONEYCOMB_ALARM_BIN,                 .str_idx=STR_ALARM_CLOCK,         .vidx = FUNC_ALARM_CLOCK},  //闹钟
#endif //
#if UTE_MODULE_SCREENS_EMOTION_SUPPORT
    {.func_sta=FUNC_MOOD,                     .res_addr=UI_BUF_I335001_2_HONEYCOMB_MOOD_BIN,                  .str_idx=STR_EMOTION,             .vidx = FUNC_MOOD},  //情绪
#endif //
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {.func_sta=FUNC_BREATHE,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_BREATHE_BIN,               .str_idx=STR_BREATHE_TRAIN,       .vidx = FUNC_BREATHE}, //呼吸
#endif //
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {.func_sta=FUNC_MESSAGE,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_INFORMATION_BIN,           .str_idx=STR_MESSAGE,             .vidx = FUNC_MESSAGE},   //消息
#endif //
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {.func_sta=FUNC_VOICE,                    .res_addr=UI_BUF_I335001_2_HONEYCOMB_VOICE_ASSISTANT_BIN,       .str_idx=STR_VOICE,               .vidx = FUNC_VOICE},   //语音助手
#endif //
#if UTE_MODULE_SCREENS_TOOLBOX_SUPPORT
    {.func_sta=FUNC_TOOLBOX,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_TOOLBOX_BIN,               .str_idx=STR_TOOL_BOX,            .vidx = FUNC_TOOLBOX},  //工具箱
#endif
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {.func_sta=FUNC_SETTING,                  .res_addr=UI_BUF_I335001_2_HONEYCOMB_SETTINGS_BIN,              .str_idx=STR_SETTING,             .vidx = FUNC_SETTING},   //设置
#endif //
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {.func_sta=FUNC_GAME,                     .res_addr=UI_BUF_I335001_2_HONEYCOMB_GAME_BIN,                  .str_idx=STR_GAME,                .vidx = FUNC_GAME},   //游戏
#endif //
#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT
    {.func_sta=FUNC_WOMEN_HEALTH,             .res_addr=UI_BUF_I335001_2_HONEYCOMB_PERIOD_BIN,                .str_idx=STR_WOMEN_HEALTH,        .vidx = FUNC_WOMEN_HEALTH},  //女性健康
#endif // 女性健康始终放到最后一位
};

compo_form_t *func_key_set_function_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_KEY_SET]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_menu_list, uteModuleMenstrualCycleIsOpen() ? SET_LIST_CNT : SET_LIST_CNT - 1);

    compo_listbox_set_sta_icon(listbox, UI_BUF_I335001_1_START_1_OK_22X22_X202_Y25_Y95_Y162_Y232_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_SetFunc_bit);

    compo_setid(listbox, COMPO_ID_LISTBOX);
    uint8_t set_idx = 1;
    if (set_idx < 1)
    {
        set_idx = 1;
    }
    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);


    return frm;
}

//点进图标进入应用
void func_key_set_function_list_icon_click(void)
{
    int icon_idx;
    f_key_set_list_t *f_set = (f_key_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());

    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    u8 func_sta = tbl_menu_list[icon_idx].func_sta;

    if (func_sta > 0)
    {
        uteModuleKeySetFuncSetMenu(func_sta);
        uteModuleGuiCommonGoBackLastScreen();
    }

}

#else
compo_form_t *func_key_set_function_form_create(void)
{
}
void func_key_set_function_list_icon_click(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I3350001_SUPPORT

//语言设置功能事件处理
static void func_key_set_function_list_process(void)
{
    f_key_set_list_t *f_set = (f_key_set_list_t *)func_cb.f_cb;
    compo_listbox_move(f_set->listbox);
    func_process();
}

//语言设置功能消息处理
static void func_key_set_function_list_message(size_msg_t msg)
{
    f_key_set_list_t *f_set = (f_key_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }


    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_key_set_function_list_icon_click();                //单击图标
            break;


        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }

}

//进入语言设置功能
static void func_key_set_function_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_key_set_list_t));
    func_cb.frm_main = func_key_set_function_form_create();

    f_key_set_list_t *f_set = (f_key_set_list_t *)func_cb.f_cb;
    f_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    // compo_listbox_move_init(listbox);
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
}

//退出语言设置功能
static void func_key_set_function_exit(void)
{
    f_key_set_list_t *f_set = (f_key_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    func_free(listbox->mcb);                                            //释放移动控制块
    func_cb.last = FUNC_KEY_SET_FUNCTION;
}

//语言设置功能
void func_key_set_function(void)
{
    printf("%s\n", __func__);
    func_key_set_function_enter();
    while (func_cb.sta == FUNC_KEY_SET_FUNCTION)
    {
        func_key_set_function_list_process();
        func_key_set_function_list_message(msg_dequeue());
    }
    func_key_set_function_exit();
}

#endif
