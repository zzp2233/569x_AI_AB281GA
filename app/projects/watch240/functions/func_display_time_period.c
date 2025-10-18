#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "ute_module_gui_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_time_period_t_
{
} f_time_period_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_2,
    COMPO_CARD_END,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

typedef struct func_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_hour_format_t;

static func_hour_format_t func_convert_to_12hour(s8 hour24)
{
    // printf("time_apm%d\n",hour24);
    u8 am_pm = (hour24 <= 12 && hour24) ? 2 : 1;    //1 PM, 2 AM
    func_hour_format_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (hour24 == 0)
        {
            hour12.hour = 12;
        }
        else if (hour24 > 12)
        {
            hour12.hour = hour24 - 12;
        }
        else
        {
            hour12.hour = hour24;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = hour24;
    hour12.am_pm = 0;
    return hour12;
}

//抬腕亮屏页面
compo_form_t *func_display_time_period_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_EFFECT]);

    //开始时间
    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 2, 320, 148);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60+148/2, 320, 148);
    compo_setid(cardbox, COMPO_CARD_1);

    compo_cardbox_rect_set_location(cardbox, 0, 0, 146/2, 320, 1, 16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29, 0x29, 0x29));

    compo_cardbox_icon_set_pos(cardbox, 0, 320-20/2-160, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_cardbox_text_scroll_process(cardbox, true);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-39, 240, 48);
    compo_cardbox_text_set(cardbox, 0, i18n[STR_DISTURD_TIM_START]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);

    u16 startMin = 0;
    u16 endMin = 0;
    uteModuleGuiCommonGetScreenSaverTimeBucket(&startMin, &endMin);
    u8 hour = startMin / 60;
    u8 min  = startMin % 60;
    u8 am_pm = 0;
    func_hour_format_t hour_cov = func_convert_to_12hour(hour);
    hour = hour_cov.hour;
    am_pm = hour_cov.am_pm;
    char aclock_str[20] = {0};
    memset(aclock_str, '\0', sizeof(aclock_str));
    if(am_pm == 2)      //AM
    {
        snprintf(aclock_str, sizeof(aclock_str), "%02d:%02d  %s", hour, min, i18n[STR_AM]);
    }
    else if (am_pm == 1)
    {
        snprintf(aclock_str, sizeof(aclock_str), "%02d:%02d  %s", hour, min, i18n[STR_PM]);
    }
    else
    {
        sprintf(aclock_str, "%02d:%02d", hour, min);
    }
    compo_cardbox_text_scroll_process(cardbox, true);
    compo_cardbox_text_set_location(cardbox, 1, -GUI_SCREEN_CENTER_X+24, 0+24, 240, 48);
    compo_cardbox_text_set(cardbox, 1, aclock_str);
    compo_cardbox_text_set_align_center(cardbox, 1, false);
    compo_cardbox_text_set_forecolor(cardbox, 1, make_color(148,148,148));

    //结束时间
    cardbox = compo_cardbox_create(frm, 1, 1, 2, 320, 148);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60+148/2+148, 320, 148);
    compo_setid(cardbox, COMPO_CARD_2);

    compo_cardbox_icon_set_pos(cardbox, 0, 320-20/2-160, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_cardbox_text_scroll_process(cardbox, true);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-39, 240, 48);
    compo_cardbox_text_set(cardbox, 0, i18n[STR_DISTURD_TIM_END]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);

    hour = endMin / 60;
    min = endMin % 60;
    hour_cov = func_convert_to_12hour(hour);
    hour = hour_cov.hour;
    am_pm = hour_cov.am_pm;
    memset(aclock_str, '\0', sizeof(aclock_str));
    if(am_pm == 2)      //AM
    {
        snprintf(aclock_str, sizeof(aclock_str), "%02d:%02d  %s", hour, min, i18n[STR_AM]);
    }
    else if (am_pm == 1)
    {
        snprintf(aclock_str, sizeof(aclock_str), "%02d:%02d  %s", hour, min, i18n[STR_PM]);
    }
    else
    {
        sprintf(aclock_str, "%02d:%02d", hour, min);
    }
    compo_cardbox_text_scroll_process(cardbox, true);
    compo_cardbox_text_set_location(cardbox, 1, -GUI_SCREEN_CENTER_X+24, 0+24, 240, 48);
    compo_cardbox_text_set(cardbox, 1, aclock_str);
    compo_cardbox_text_set_align_center(cardbox, 1, false);
    compo_cardbox_text_set_forecolor(cardbox, 1, make_color(148,148,148));

    return frm;
}

#endif
//抬腕亮屏事件处理
static void func_display_time_period_process(void)
{
    for(u8 i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)      //文本滚动
    {
        u16 id = COMPO_CARD_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
    func_process();
}

//获取点击卡片的id
static u16 func_time_period_card_get_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t* cardbox = NULL;
    for(i = 0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
        id = COMPO_CARD_START + 1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (compo_cardbox_get_visible(cardbox) && abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }

    return ret;
}

//单击按钮
static void func_time_period_button_click(void)
{
//    f_time_period_t *saving = (f_time_period_t *)func_cb.f_cb;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_time_period_card_get_id(pt);
    if (compo_id <= 0 || compo_id > COMPO_CARD_END - 1)
    {
        return;
    }
//    printf("click compo_id:%d\n", compo_id);

    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == COMPO_CARD_1)
        {
            uteModuleGuiCommonSetScreenSaverTimeBucketType(true);
            uteTaskGuiStartScreen(FUNC_DISPLAY_SUB_SET, 0, __func__);
        }
        if (compo_id == COMPO_CARD_2)
        {
            uteModuleGuiCommonSetScreenSaverTimeBucketType(false);
            uteTaskGuiStartScreen(FUNC_DISPLAY_SUB_SET, 0, __func__);
        }
    }

}

//抬腕亮屏功能消息处理
static void func_display_time_period_message(size_msg_t msg)
{
    //f_time_period_t *f_saving = (f_time_period_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_time_period_button_click();
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入抬腕亮屏功能
static void func_display_time_period_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_time_period_t));
    func_cb.frm_main = func_display_time_period_form_create();

    //初始化变量
    //f_time_period_t *wrs = (f_time_period_t *)func_cb.f_cb;

}

//退出抬腕亮屏功能
static void func_display_time_period_exit(void)
{
    func_cb.last = FUNC_DISPLAY_TIME_PERIOD;
}

//抬腕亮屏功能
void func_display_time_period(void)
{
    printf("%s\n", __func__);
    func_display_time_period_enter();
    while (func_cb.sta == FUNC_DISPLAY_TIME_PERIOD)
    {
        func_display_time_period_process();
        func_display_time_period_message(msg_dequeue());
    }
    func_display_time_period_exit();
}

