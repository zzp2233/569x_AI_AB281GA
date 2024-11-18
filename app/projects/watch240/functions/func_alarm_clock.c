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
} f_alarm_clock_t;

static widget_icon_t *icon_add;

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
        icon_add = widget_icon_create(frm->page, UI_BUF_ALARM_CLOCK_ADD_BIN);
        widget_set_pos(icon_add, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_ADD_BIN).hei / 2 - 10);

        //page_body结合compo_page_move实现列表滑动（先绘制所有组件，再将page平均分段）
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 42 / 100, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT * 6 / 10);//208);
    }
    else
    {
        icon_add = NULL;
    }

    //闹钟选项卡
    char str_buff[32];
    compo_cardbox_t *cardbox;
    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {

//            if(i == ALARM_ENABLE_CNT()-1  && !ALARM_GET_SWITCH(ALARM_ENABLE_CNT()-1))///开启新闹钟
//            {
//                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
//            }


            cardbox = compo_cardbox_create(frm, 1, 1, 2, GUI_SCREEN_WIDTH - 10, GUI_SCREEN_HEIGHT/4);
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4 + (GUI_SCREEN_HEIGHT/4 + 4) * i);
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_ALARM_CLOCK_SELECT1_ON_BIN : UI_BUF_ALARM_CLOCK_SELECT1_BIN);
            compo_cardbox_icon_set_pos(cardbox, 0,
                                       (GUI_SCREEN_WIDTH - 10) / 2 - gui_image_get_size(UI_BUF_ALARM_CLOCK_SELECT1_ON_BIN).wid / 2 - 2, 0);

            snprintf(str_buff, sizeof(str_buff), "%02d:%02d", ALARM_GET_HOUR(i), ALARM_GET_MIN(i));
            compo_cardbox_text_set_font(cardbox, 0, UI_BUF_0FONT_FONT_NUM_24_BIN);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set(cardbox, 0, str_buff);
            compo_cardbox_text_set_align_center(cardbox, 0, false);
            compo_cardbox_text_set_location(cardbox, 0, -100, -35, 180, 50);

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
                snprintf(str_buff, sizeof(str_buff), i18n[STR_REPEAT]);
                char *buff_pt = str_buff + strlen(str_buff);
                for (u8 j=0; j<7; j++)
                {
                    if (ALARM_GET_CYCLE(i) & BIT(j))
                    {
                        *buff_pt = '0' + j + 1;
                        buff_pt++;
                    }
                }
                *buff_pt = '\0';
            }
//            compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
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
            widget_icon_set(icon_add, UI_BUF_ALARM_CLOCK_ADD_CLICK_BIN);
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
        compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_ALARM_CLOCK_SELECT1_ON_BIN : UI_BUF_ALARM_CLOCK_SELECT1_BIN);
        compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
//        compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
    }

    if (icon_add)
    {
        widget_icon_set(icon_add, UI_BUF_ALARM_CLOCK_ADD_BIN);
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
            if (pt.x > (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_ALARM_CLOCK_SELECT1_ON_BIN).wid))   //开关
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

//闹钟功能事件处理
static void func_alarm_clock_process(void)
{
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    compo_page_move_process(f_aclock->ptm);

    func_process();
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
