#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if (USE_APP_TYPE == USE_AB_APP)
#define ALARM_ENABLE_CNT()              alarm_num_get()
#define ALARM_ENABLE(idx, en)           alarm_clock_enable(idx, en)
#define ALARM_GET_HOUR(idx)             app_data.sector0.alarm[idx].hour
#define ALARM_GET_MIN(idx)              app_data.sector0.alarm[idx].minute
#define ALARM_GET_CYCLE(idx)            app_data.sector0.alarm[idx].cycle
#define ALARM_GET_SWITCH(idx)           app_data.sector0.alarm[idx].swi
#define ALARM_CLOCK_NUM_MAX             ALARM_NUM_MAX
#elif (USE_APP_TYPE == USE_UTE_APP)
#define ALARM_ENABLE_CNT()              uteModuleSystemtimeGetAlarmTotalCnt()
#define ALARM_ENABLE(idx, en)           uteModuleSystemtimeEnableAlarm(idx, en)
#define ALARM_GET_HOUR(idx)             uteModuleSystemtimeGetAlarmHour(idx)
#define ALARM_GET_MIN(idx)              uteModuleSystemtimeGetAlarmMin(idx)
#define ALARM_GET_CYCLE(idx)            uteModuleSystemtimeGetAlarmCycle(idx)
#define ALARM_GET_SWITCH(idx)           uteModuleSystemtimeGetAlarmEnableState(idx)
#define ALARM_CLOCK_NUM_MAX             uteModuleSystemtimeGetAlarmMaxCnt()
#else
#define ALARM_ENABLE_CNT()              0
#define ALARM_ENABLE(idx, en)
#define ALARM_GET_HOUR(idx)             0
#define ALARM_GET_MIN(idx)              0
#define ALARM_GET_CYCLE(idx)            0
#define ALARM_GET_SWITCH(idx)           0
#define ALARM_CLOCK_NUM_MAX             0
#endif

#define MAKE_GRAY(lv)                   make_color(lv, lv, lv)

#define ACLOCK_LIST_CNT                 ((int)(sizeof(tbl_aclock_list) / sizeof(tbl_aclock_list[0])))

static void func_alarm_clock_exit(void);
static void func_alarm_clock_enter(void);

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_ADD = 1,

    //图像
    COMPO_ID_PIC_ADD_CLICK,

    //卡片
    COMPO_ID_CARD_0,
};

typedef struct f_alarm_clock_t_
{
    page_tp_move_t *ptm;
    bool time_scale;        //时间制 0:24小时; 1:12小时
} f_alarm_clock_t;

static widget_icon_t *icon_add;

typedef struct func_alarm_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_alarm_hour_format_t;

static func_alarm_hour_format_t func_alarm_convert_to_12hour(s8 hour24)
{
    u8 am_pm = (hour24 >= 12) ? 2 : 1;    //2 PM, 1 AM
    func_alarm_hour_format_t hour12;
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

#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK]);

    //添加闹钟按钮图标
    if (ALARM_ENABLE_CNT() < ALARM_CLOCK_NUM_MAX)
    {
        icon_add = widget_icon_create(frm->page, UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN);
        widget_set_pos(icon_add, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei / 2 - 10);

        //page_body结合compo_page_move实现列表滑动（先绘制所有组件，再将page平均分段）
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 42 / 100, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT * 6 / 10);//208);
    }
    else
    {
        icon_add = NULL;
    }

    //添加闹钟按钮文字
//    compo_textbox_t* icon_add_txt = compo_textbox_create(frm, strlen(i18n[STR_ADD_CLOCK]));
    if (icon_add) {
        compo_textbox_t* icon_add_txt = compo_textbox_create_for_page(frm, frm->page, 50);
        compo_textbox_set_location(icon_add_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei / 2 - 10,
                                   gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).wid - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei,
                                   gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei);
        compo_textbox_set(icon_add_txt, "添加闹钟");
    }

    //闹钟选项卡
    char str_buff[50];
    compo_cardbox_t *cardbox;
    int buf_num=0;
    int str_week_buf[7]={
        STR_MONDAY, // 周一
        STR_TUESDAY, // 周二
        STR_WEDNESDAY, // 周三
        STR_THURSDAY, // 周四
        STR_FRIDAY, // 周五
        STR_SATURDAY, // 周六
        STR_SUNDAY, // 周日
    };

    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {
            memset(str_buff,0,sizeof(str_buff));
            buf_num=0;
            cardbox = compo_cardbox_create(frm, 1, 1, 3, GUI_SCREEN_WIDTH - 10, GUI_SCREEN_HEIGHT/4);
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4 + (GUI_SCREEN_HEIGHT/4 + 4) * i);
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I330001_PUBLIC_SWITCH01_BIN : UI_BUF_I330001_PUBLIC_SWITCH00_BIN);
            compo_cardbox_icon_set_pos(cardbox, 0,
                                       (GUI_SCREEN_WIDTH - 10) / 2 - gui_image_get_size(UI_BUF_I330001_PUBLIC_SWITCH01_BIN).wid / 2 - 2, 0);

            snprintf(str_buff, sizeof(str_buff), "%02d:%02d", func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).hour, ALARM_GET_MIN(i));
            compo_cardbox_text_set_font(cardbox, 0, UI_BUF_0FONT_FONT_NUM_24_BIN);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set(cardbox, 0, str_buff);
            compo_cardbox_text_set_align_center(cardbox, 0, false);
            compo_cardbox_text_set_location(cardbox, 0, -100, -35, 180, 50);

            compo_cardbox_text_set_font(cardbox, 2, UI_BUF_0FONT_FONT_BIN);
            compo_cardbox_text_set_forecolor(cardbox, 2, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set_align_center(cardbox, 2, false);
            compo_cardbox_text_set_location(cardbox, 2, -20, -30, 80, 50);
            compo_cardbox_text_set_visible(cardbox, 2, true);
            if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 0)
            {
                compo_cardbox_text_set_visible(cardbox, 2, false);
            }
            else if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 1)           //AM
            {
                compo_cardbox_text_set(cardbox, 2, i18n[STR_AM]);
            }
            else if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 2)           //PM
            {
                compo_cardbox_text_set(cardbox, 2, i18n[STR_PM]);
            }


            memset(str_buff,0,sizeof(str_buff));

            if (ALARM_GET_CYCLE(i) & BIT(7))
            {
                snprintf(str_buff, sizeof(str_buff), i18n[STR_ONCE]);
            }
            else if (ALARM_GET_CYCLE(i) == 0x7f)
            {
                snprintf(str_buff, sizeof(str_buff), i18n[STR_EVERY_DAY]);
            }
            else
            {
                for (u8 j=0; j<7; j++)
                {
                    char string_handle[50];
                    memset(string_handle,0,sizeof(string_handle));
                    if (ALARM_GET_CYCLE(i) & BIT(j))
                    {
                        snprintf(string_handle, sizeof(string_handle),i18n[str_week_buf[j]]);
                        for(int k=0;k<strlen(i18n[str_week_buf[j]]);k++)
                        {
                            str_buff[buf_num] = string_handle[k];
                            buf_num++;
                        }
                        str_buff[buf_num] = ' ';
                        buf_num++;
                    }
                }
            }
            compo_cardbox_text_set_forecolor(cardbox, 1, MAKE_GRAY(128));
            compo_cardbox_text_set(cardbox, 1, str_buff);
            compo_cardbox_text_set_align_center(cardbox, 1, false);
            compo_cardbox_text_set_location(cardbox, 1, -100, 10, 180, 40);

            compo_cardbox_rect_set_color(cardbox, 0, MAKE_GRAY(26));
            compo_cardbox_rect_set_location(cardbox, 0, 0, 0, GUI_SCREEN_WIDTH - 10, GUI_SCREEN_HEIGHT/4, 20);
        }
    }
    else
    {
        cardbox = compo_cardbox_create(frm, 1, 0, 1, GUI_SCREEN_WIDTH - 10, GUI_SCREEN_HEIGHT/4);
        compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4);
        compo_setid(cardbox, COMPO_ID_CARD_0);

        snprintf(str_buff, sizeof(str_buff), i18n[STR_NO_CLOCK]);
        compo_cardbox_text_set_forecolor(cardbox, 0, MAKE_GRAY(128));
        compo_cardbox_text_set(cardbox, 0, str_buff);
        compo_cardbox_text_set_location(cardbox, 0, 0, 0, GUI_SCREEN_WIDTH - 10, widget_text_get_height());

        compo_cardbox_rect_set_color(cardbox, 0, MAKE_GRAY(26));
        compo_cardbox_rect_set_location(cardbox, 0, 0, 0, GUI_SCREEN_WIDTH - 10, GUI_SCREEN_HEIGHT/4, 20);
    }
    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_button_touch_handle(void)
{
    if (icon_add)
    {
        point_t pt = ctp_get_sxy();
        rect_t rect = widget_get_absolute(icon_add);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)   //添加闹钟
        {
            widget_icon_set(icon_add, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
        }
    }
}

//释放按钮效果处理
static void func_alarm_clock_button_release_handle(void)
{
    compo_cardbox_t *cardbox;
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I330001_PUBLIC_SWITCH01_BIN : UI_BUF_I330001_PUBLIC_SWITCH00_BIN);
        compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
        compo_cardbox_text_set_forecolor(cardbox, 2, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
//        compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
    }

    if (icon_add)
    {
        widget_icon_set(icon_add, UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN);
    }
}

//单击按钮
static void func_alarm_clock_button_click(void)
{
    rect_t rect;
    point_t pt = ctp_get_sxy();

    if (icon_add)
    {
        rect = widget_get_absolute(icon_add);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)   //添加闹钟
        {
            if (ALARM_ENABLE_CNT() < ALARM_CLOCK_NUM_MAX)
            {
                sys_cb.alarm_edit_idx = ALARM_ENABLE_CNT();
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
                return;
            }
        }
    }

    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        if (compo_cardbox_btn_is(compo_getobj_byid(COMPO_ID_CARD_0 + i), pt))
        {
            if (pt.x > (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_I330001_PUBLIC_SWITCH01_BIN).wid))   //开关
            {
                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
            }
            else        //编辑
            {
                sys_cb.alarm_edit_idx = i;
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
            }
        }
    }

    func_alarm_clock_button_release_handle();
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//闹钟功能事件处理
static void func_alarm_clock_process(void)
{
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    compo_page_move_process(f_aclock->ptm);

    func_process();

    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)      //文本滚动
    {
        compo_cardbox_t* cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        compo_cardbox_text_scroll_process(cardbox, true);
    }
}


//闹钟功能消息处理
static void func_alarm_clock_message(size_msg_t msg)
{
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;



    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_alarm_clock_button_touch_handle();
            compo_page_move_touch_handler(f_aclock->ptm);
            break;

        case MSG_CTP_CLICK:
            func_alarm_clock_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_alarm_clock_button_release_handle();
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_alarm_clock_button_release_handle();
            func_message(msg);
            break;

        case MSG_QDEC_BACKWARD:
            compo_page_move_set_by_pages(f_aclock->ptm, -1);
            break;

        case MSG_QDEC_FORWARD:
            compo_page_move_set_by_pages(f_aclock->ptm, 1);
            break;

        case MSG_SYS_500MS:
            //时间制检测
            if (f_aclock->time_scale != uteModuleSystemtime12HOn())
            {
                if (func_cb.frm_main != NULL)
                {
                    compo_form_destroy(func_cb.frm_main);
                }
                func_alarm_clock_exit();
                if (func_cb.f_cb != NULL)
                {
                    func_free(func_cb.f_cb);
                    func_cb.f_cb = NULL;
                }
                func_alarm_clock_enter();
            }
            break;

        default:
            func_message(msg);
            break;
    }

}

//进入闹钟功能
static void func_alarm_clock_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_t));
    func_cb.frm_main = func_alarm_clock_form_create();

    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    f_aclock->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 104,
        .page_count = ALARM_ENABLE_CNT(),
        .jump_perc = 20,
        .quick_jump_perc = 200,
        .up_over_perc = ALARM_ENABLE_CNT() ? 50 : 0,
        .down_over_perc = ALARM_ENABLE_CNT() ? 50 : 0,
        .down_spring_perc = icon_add ? 0 : 40,
    };
    compo_page_move_init(f_aclock->ptm, func_cb.frm_main->page_body, &info);

    //当前显示的时间制
    f_aclock->time_scale = uteModuleSystemtime12HOn();
}

//退出闹钟功能
static void func_alarm_clock_exit(void)
{
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    if (f_aclock->ptm)
    {
        func_free(f_aclock->ptm);
    }
}

//闹钟功能
void func_alarm_clock(void)
{
    printf("%s\n", __func__);
    func_alarm_clock_enter();
    while (func_cb.sta == FUNC_ALARM_CLOCK)
    {
        func_alarm_clock_process();
        func_alarm_clock_message(msg_dequeue());
    }
    func_alarm_clock_exit();
}
