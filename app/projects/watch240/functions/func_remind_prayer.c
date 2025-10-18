#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "ute_module_gui_common.h"
#include "ctp.h"
#include "func_cover.h"

#if APP_CUSTOM_REMIND_SETTING_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
typedef struct f_remind_prayer_t_
{
    page_tp_move_t *ptm;
    bool praySta;
} f_remind_prayer_t;

enum
{
    COMPO_ID_START = 0,
    COMPO_ID_CARD1,
    COMPO_ID_CARD_MAX = COMPO_ID_CARD1 + APP_CUSTOM_REMIND_SETTING_DAY_CNT,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

uint8_t uteModuleScreenGetRemindSettingIndex(void)
{
    uint8_t i = 0;
    ute_module_systemtime_custom_remind_t RemindData;
    uteModuleSystemtimeGetRemindData(&RemindData);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    for (i = 0; i < APP_CUSTOM_REMIND_SETTING_MAX_CNT; i ++)
    {
        if (time.year == RemindData.remindParam[i].year
            && time.month == RemindData.remindParam[i].month
            && time.day == RemindData.remindParam[i].day)
        {
            return i;
        }
    }
    return 0xff;
}

compo_form_t *func_remind_prayer_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PRAYER_REMIND]);

    uint16_t str_pray_buff[5] = {STR_FAJR, STR_DHUHR, STR_ASR, STR_MAGHRIB, STR_ISHA};

    uint8_t RemindDataCnt = uteModuleSystemtimeGetRemindTotalCnt();
    uint8_t RemindIndex = uteModuleScreenGetRemindSettingIndex();
    if ((RemindDataCnt > 0) && (RemindIndex != 0xff) && uteModuleSystemtimeGetSupportRemindSetting())
    {
        //有数据
        ute_module_systemtime_custom_remind_t RemindData;
        uteModuleSystemtimeGetRemindData(&RemindData);

        bool hoursOpt;
        char str_buff[20] = {0};
        uint8_t hour_num = 0;
        compo_cardbox_t *cardbox;
        for (uint8_t i = 0; i < APP_CUSTOM_REMIND_SETTING_DAY_CNT; i++)
        {
            cardbox = compo_cardbox_create(frm, 1, 1, 3, 320, 130);
            compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60 + 130/2 + 130*i, 320, 130);
            compo_setid(cardbox, COMPO_ID_CARD1 + i);
            if (i < APP_CUSTOM_REMIND_SETTING_DAY_CNT - 1)
            {
                compo_cardbox_rect_set_location(cardbox, 0, 0, 128/2, 320, 1, 16);    //画线
                compo_cardbox_rect_set_color(cardbox, 0, make_color(0x29, 0x29, 0x29));
            }
            compo_cardbox_icon_set_pos(cardbox, 0, 320-46/2-160, 0);
            if (RemindData.remindParam[RemindIndex].oneDayRemind[i].isOpen)
            {
                compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_10_PRAY_OPEN_BIN);
            }
            else
            {
                compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_10_PRAY_CLOSE_BIN);
            }
            memset(str_buff, 0, 20);
            hour_num = RemindData.remindParam[RemindIndex].oneDayRemind[i].hour;
            if (uteModuleSystemtime12HOn())
            {
                if(hour_num < 12)
                {
                    snprintf(str_buff, 20, "%02d:%02d", hour_num, RemindData.remindParam[RemindIndex].oneDayRemind[i].min);//, i18n[STR_AM]);
                    hoursOpt = true;
                }
                else
                {
                    hour_num %= 12;
                    if(hour_num == 0)
                    {
                        hour_num = 12;
                    }
                    snprintf(str_buff, 20, "%02d:%02d", hour_num, RemindData.remindParam[RemindIndex].oneDayRemind[i].min);//, i18n[STR_PM]);
                    hoursOpt = false;
                }
            }
            else
            {
                snprintf(str_buff, 20, "%02d:%02d", hour_num, RemindData.remindParam[RemindIndex].oneDayRemind[i].min);
            }
            compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-44, 240, 48);
            compo_cardbox_text_set_font(cardbox, 0, UI_BUF_0FONT_FONT_NUM_48_BIN);
            compo_cardbox_text_set(cardbox, 0, str_buff);
            compo_cardbox_text_set_align_center(cardbox, 0, false);
            if (uteModuleSystemtime12HOn())
            {
                compo_cardbox_text_set_location(cardbox, 2, -GUI_SCREEN_CENTER_X+34 + widget_text_get_area(cardbox->text[0]).wid, 0-36, 120, 48);
                compo_cardbox_text_set(cardbox, 2, hoursOpt ? i18n[STR_AM] : i18n[STR_PM]);
                compo_cardbox_text_set_align_center(cardbox, 2, false);
            }
            compo_cardbox_text_set_location(cardbox, 1, -GUI_SCREEN_CENTER_X+24, 0+11, 240, 48);
            compo_cardbox_text_set(cardbox, 1, i18n[str_pray_buff[RemindData.remindParam[RemindIndex].oneDayRemind[i].type]]);
            compo_cardbox_text_set_align_center(cardbox, 1, false);
            compo_cardbox_text_scroll_process(cardbox, true);
        }
    }
    else
    {
        //无数据
        compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_10_PRAY_ICON_NODATE_BIN);
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 89 + 132/2);

        compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_PRAYER_FAILED]));
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 270 + 80, 338, 193);
        compo_textbox_set_multiline(textbox, true);
        widget_text_set_ellipsis(textbox->txt, false);
        compo_textbox_set(textbox, i18n[STR_PRAYER_FAILED]);
    }

    return frm;
}

//获取点击的卡片组件id
static u16 func_remind_pray_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<COMPO_ID_CARD_MAX - COMPO_ID_CARD1; i++)
    {
        id = COMPO_ID_CARD1 + i;
        cardbox = compo_getobj_byid(id);
        if(cardbox == NULL)//如果没有这个组件就不需要处理
        {
            continue;
        }
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//单击按钮
static void func_remind_prayer_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_remind_pray_card_get_btn_id(pt);
    if (compo_id < COMPO_ID_CARD1 || compo_id >= COMPO_ID_CARD_MAX)
    {
        return;
    }
    compo_cardbox_t *cardbox = compo_getobj_byid(compo_id);
    ute_module_systemtime_custom_remind_t RemindData;
    uteModuleSystemtimeGetRemindData(&RemindData);
    uint8_t RemindIndex = uteModuleScreenGetRemindSettingIndex();
    for (uint8_t i = 0; i < COMPO_ID_CARD_MAX - COMPO_ID_CARD1; i++)
    {
        if (compo_id == COMPO_ID_CARD1 + i)
        {
            if (RemindData.remindParam[RemindIndex].oneDayRemind[i].isOpen)
            {
                compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_10_PRAY_CLOSE_BIN);
                RemindData.remindParam[RemindIndex].oneDayRemind[i].isOpen = false;
            }
            else
            {
                compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_10_PRAY_OPEN_BIN);
                RemindData.remindParam[RemindIndex].oneDayRemind[i].isOpen = true;
            }
            uteModuleSystemtimeSetRemindData(&RemindData);
            uteModuleSystemtimeReportAllCustomRemindData();
        }
    }
}

static void func_remind_prayer_message(size_msg_t msg)
{
    f_remind_prayer_t *f_remind_prayer = (f_remind_prayer_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_remind_prayer->ptm)
            {
                compo_page_move_touch_handler(f_remind_prayer->ptm);
            }
            break;
        case MSG_CTP_CLICK:
            func_remind_prayer_click();
            break;
        default:
            func_message(msg);
            break;
    }
}
//下滑事件流程处理
static void func_remind_prayer_process(void)
{
    f_remind_prayer_t *f_remind_prayer = (f_remind_prayer_t *)func_cb.f_cb;
    if (f_remind_prayer->praySta != uteModuleSystemtimeGetSupportRemindSetting())
    {
        msg_enqueue(MSG_CHECK_LANGUAGE);
        return;
    }

    if(f_remind_prayer->ptm)
    {
        compo_page_move_process(f_remind_prayer->ptm);
    }
    func_process();
}
#endif

//创建界面
static void func_remind_prayer_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_remind_prayer_t));
    func_cb.frm_main = func_remind_prayer_form_create();

    f_remind_prayer_t *f_remind_prayer = (f_remind_prayer_t *)func_cb.f_cb;
    f_remind_prayer->praySta = uteModuleSystemtimeGetSupportRemindSetting();
    f_remind_prayer->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  ((uteModuleScreenGetRemindSettingIndex() != 0xff) && uteModuleSystemtimeGetSupportRemindSetting()) ? 699 : 0,
        .page_count = 1,
        .quick_jump_perc = 10,
    };
    compo_page_move_init(f_remind_prayer->ptm, func_cb.frm_main->page_body, &info);
}

//退出任务
static void func_remind_prayer_exit(void)
{
    func_cb.last = FUNC_REMIND_PRAYER;

    f_remind_prayer_t *f_remind_prayer = (f_remind_prayer_t *)func_cb.f_cb;
    if (f_remind_prayer->ptm)
    {
        func_free(f_remind_prayer->ptm);
    }
    //task_stack_remove(FUNC_REMIND_PRAYER);
}

//说明功能
void func_remind_prayer(void)
{
    printf("%s\n", __func__);
    func_remind_prayer_enter();
    while (func_cb.sta == FUNC_REMIND_PRAYER)
    {
        func_remind_prayer_process();
        func_remind_prayer_message(msg_dequeue());
    }
    func_remind_prayer_exit();
}

#endif

