#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_systemtime.h"
#include "ute_module_weather.h"
#include "ute_module_call.h"
#include "func_cover.h"

#if 1

typedef struct f_clock_sub_sider_add_widget_t_
{
    compo_listbox_t *listbox;
} f_clock_sub_sider_add_widget_t;

enum
{
    COMPO_ID_LISTBOX = 1,
};

#define  ADD_WIDGET_CNT      ((int)(sizeof(clock_side_add_widget_list) / sizeof(clock_side_add_widget_list[0])))

static void clock_side_add_widget_shift_array(int arr[], int size)
{
    if (size <= 1) return;  // 数组长度不足，无需移动

    for (int i = size - 1; i > 0; i--)
    {
        arr[i] = arr[i - 1];  // 将前一个元素复制到当前位置
    }
}


#if GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
const compo_listbox_item_t clock_side_add_widget_list[] =
{
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {.func_sta=FUNC_CALL,                     .res_addr=UI_BUF_I338001_2_HONEYCOMB_CALL_BIN,                   .str_idx=STR_PHONE},                 //电话
#endif
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {.func_sta=FUNC_ACTIVITY,                 .res_addr=UI_BUF_I338001_2_HONEYCOMB_ACTIVITY_BIN,               .str_idx=STR_EVREY_DAY_ACTIVITY},    //每日活动
#endif
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {.func_sta=FUNC_HEARTRATE,                .res_addr=UI_BUF_I338001_2_HONEYCOMB_HEART_BIN,             .str_idx=STR_HEART_RATE},            //心率
#endif
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {.func_sta=FUNC_SPORT,                    .res_addr=UI_BUF_I338001_2_HONEYCOMB_SOPRT_BIN,               .str_idx=STR_SPORTS},                //运动
#endif
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {.func_sta=FUNC_BLOOD_OXYGEN,             .res_addr=UI_BUF_I338001_2_HONEYCOMB_SPO2_BIN,                .str_idx=STR_BLOOD_OXYGEN},          //血氧
#endif
#if UTE_MODULE_SCREENS_PRESSURE_SUPPORT
    {.func_sta=FUNC_PRESSURE,                .res_addr=UI_BUF_I338001_2_HONEYCOMB_STRESS_BIN,                  .str_idx=STR_STRESS},            //压力
#endif
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {.func_sta=FUNC_SLEEP,                    .res_addr=UI_BUF_I338001_2_HONEYCOMB_SLEEP_BIN,                 .str_idx=STR_SLEEP},                 //睡眠
#endif
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {.func_sta=FUNC_BT,                       .res_addr=UI_BUF_I338001_2_HONEYCOMB_MUSIC_BIN,                 .str_idx=STR_MUSIC},                 //音乐
#endif
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {.func_sta=FUNC_WEATHER,                  .res_addr=UI_BUF_I338001_2_HONEYCOMB_WEATHER_BIN,               .str_idx=STR_WEATHER},               //天气
#endif
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {.func_sta=FUNC_ALARM_CLOCK,              .res_addr=UI_BUF_I338001_2_HONEYCOMB_ALARM_BIN,                 .str_idx=STR_ALARM_CLOCK},           //闹钟
#endif
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {.func_sta=FUNC_BREATHE,                  .res_addr=UI_BUF_I338001_2_HONEYCOMB_BREATHING_BIN,               .str_idx=STR_BREATHE_TRAIN},         //呼吸
#endif
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {.func_sta=FUNC_MESSAGE,                  .res_addr=UI_BUF_I338001_2_HONEYCOMB_MESSAGE_BIN,                 .str_idx=STR_MESSAGE},               //消息
#endif
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {.func_sta=FUNC_VOICE,                    .res_addr=UI_BUF_I338001_2_HONEYCOMB_VOICE_ASSISTANT_BIN,       .str_idx=STR_VOICE},                 //语音助手
#endif
#if UTE_MODULE_SCREENS_TOOLBOX_SUPPORT
    {.func_sta=FUNC_TOOLBOX,                    .res_addr=UI_BUF_I338001_2_HONEYCOMB_TOOLBOX_BIN,               .str_idx=STR_TOOL_BOX},             //工具箱
#endif
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {.func_sta=FUNC_SETTING,                  .res_addr=UI_BUF_I338001_2_HONEYCOMB_SETTINGS_BIN,              .str_idx=STR_SETTING},               //设置
#endif
#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT
    {.func_sta=FUNC_WOMEN_HEALTH,             .res_addr=UI_BUF_I338001_2_HONEYCOMB_PERIOD_BIN,                .str_idx=STR_WOMEN_HEALTH},          //女性健康
#endif
};

u8 clock_side_add_widget_bit(uint n)
{
    if (sys_cb.clock_side_add_widget[n] == clock_side_add_widget_list[n].func_sta)
    {
        return true;
    }
    return false;
}
//创建边菜单
compo_form_t * func_clock_sub_sider_add_widget_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(false);

    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_LANGUAGE);
    compo_listbox_set_bgimg(listbox, UI_BUF_I338001_28_SET_LIST_BG_BIN);
    compo_listbox_set(listbox,clock_side_add_widget_list,ADD_WIDGET_CNT);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I338001_28_SET_CHECKED_BIN, UI_BUF_I338001_28_SET_UNCHECKED_BIN);
    compo_listbox_set_bithook(listbox, clock_side_add_widget_bit);

    return;
}
//单击按钮
static void func_clock_sub_sider_add_widget_button_click(void)
{
    int icon_idx;
    f_clock_sub_sider_add_widget_t *f_clock_sub_sider_add_widget = (f_clock_sub_sider_add_widget_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_clock_sub_sider_add_widget->listbox;

    int icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= ADD_WIDGET_CNT)
    {
        return;
    }

    clock_side_add_widget_shift_array(sys_cb.clock_side_add_widget,CLOCK_SIDE_ADD_WIDGET_NUM);
    sys_cb.clock_side_add_widget[0] = clock_side_add_widget_list[icon_idx].func_sta;
    compo_listbox_update(listbox);
}
#endif

//侧边栏添加小组件事件流程处理
static void func_clock_sub_sider_add_widget_process(void)
{
    func_clock_sub_process();
}

//侧边栏添加小组件功能消息处理
static void func_clock_sub_sider_add_widget_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:

            break;

        default:
            func_clock_sub_message(msg);
            break;
    }
}

//侧边栏添加小组件进入处理
static void func_clock_sub_sider_add_widget_enter(void)
{
    func_clock_sub_sider_add_widget_form_create();
}

//侧边栏添加小组件退出处理
static void func_clock_sub_sider_add_widget_exit(void)
{

}

//侧边栏添加小组件
void func_clock_sub_sider_add_widget(void)
{
    func_clock_sub_sider_add_widget_enter();
    while (func_cb.sta == FUNC_CLOCK)
    {
        func_clock_sub_sider_add_widget_process();
        func_clock_sub_sider_add_widget_message(msg_dequeue());
    }
    func_clock_sub_sider_add_widget_exit();
}

#endif
