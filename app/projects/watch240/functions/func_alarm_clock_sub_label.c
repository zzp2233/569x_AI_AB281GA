#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_systemtime.h"
#include "ute_module_weather.h"
#include "ute_module_call.h"
#include "func_cover.h"
typedef struct f_alarm_clock_sub_label_t_
{
    u8 label;
} f_alarm_clock_sub_label_t;

enum
{
    COMD_PIC_LABEL_ALARM = 1,
    COMD_PIC_LABEL_SLEEP,
    COMD_PIC_LABEL_MEETING,
    COMD_PIC_LABEL_DATING,
    COMD_PIC_LABEL_DINNER,

    COMD_PIC_SWITCH_ALARM,
    COMD_PIC_SWITCH_SLEEP,
    COMD_PIC_SWITCH_MEETING,
    COMD_PIC_SWITCH_DATING,
    COMD_PIC_SWITCH_DINNER,

    COMD_BUTTON_CONFIRM,
};
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
#define ALARM_LABEL_COUNT (5)
typedef enum
{
    LABEL_ALARM = 0,
    LABEL_SLEEP,
    LABEL_MEETING,
    LABEL_DATING,
    LABEL_DINNER,
} ALARM_LABEL;
bool alarm_label_get_switch(ALARM_LABEL label)
{
    if(label == ute_moduleSystemtimeAlarmGetLabel(sys_cb.alarm_edit_idx))
    {
        return true;
    }
    return false;
}
const uint32_t alarm_label[ALARM_LABEL_COUNT] =
{
    UI_BUF_I341001_20_ALARM_CLOCK_ICON_ALARM_CLOCK_BIN,
    UI_BUF_I341001_20_ALARM_CLOCK_ICON_SLEEP_BIN,
    UI_BUF_I341001_20_ALARM_CLOCK_ICON_MEETING_BIN,
    UI_BUF_I341001_20_ALARM_CLOCK_ICON_DATING_BIN,
    UI_BUF_I341001_20_ALARM_CLOCK_ICON_DINNER_BIN,
};

compo_form_t *func_alarm_clock_sub_label_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    compo_picturebox_t * pic = NULL;
    compo_textbox_t *textbox = NULL;
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_TAG]);

#define MOVE_X 102
#define MOVE_Y 133

    pic = compo_picturebox_create(frm,alarm_label[0]);
    compo_picturebox_set_pos(pic,53 + 58 / 2,84 + 58 / 2);
    compo_button_t * btn_ok = compo_button_create(frm);
    compo_button_set_location(btn_ok,53 + 58 / 2,84 + 58,60,58 * 2);
    compo_setid(btn_ok,COMD_PIC_LABEL_ALARM);

    pic = compo_picturebox_create(frm,alarm_label[1]);
    compo_picturebox_set_pos(pic,53 + 58 / 2 + MOVE_X,84 + 58 / 2);
    btn_ok = compo_button_create(frm);
    compo_button_set_location(btn_ok,53 + 58 / 2 + MOVE_X,84 + 58,60,58 * 2);
    compo_setid(btn_ok,COMD_PIC_LABEL_SLEEP);

    pic = compo_picturebox_create(frm,alarm_label[2]);
    compo_picturebox_set_pos(pic,53 + 58 / 2 + MOVE_X * 2,84 + 58 / 2);
    btn_ok = compo_button_create(frm);
    compo_button_set_location(btn_ok,53 + 58 / 2 + MOVE_X * 2,84 + 58,60,58 * 2);
    compo_setid(btn_ok,COMD_PIC_LABEL_MEETING);

    pic = compo_picturebox_create(frm,alarm_label[3]);
    compo_picturebox_set_pos(pic,53 + 58 / 2 + MOVE_X / 2,84 + 58 / 2 + MOVE_Y);
    btn_ok = compo_button_create(frm);
    compo_button_set_location(btn_ok,53 + 58 / 2 + MOVE_X / 2,84 + 58 + MOVE_Y,60,58 * 2);
    compo_setid(btn_ok,COMD_PIC_LABEL_DATING);

    pic = compo_picturebox_create(frm,alarm_label[4]);
    compo_picturebox_set_pos(pic,53 + 58 / 2 + MOVE_X + MOVE_X / 2,84 + 58 / 2 + MOVE_Y);
    btn_ok = compo_button_create(frm);
    compo_button_set_location(btn_ok,53 + 58 / 2 + MOVE_X + MOVE_X / 2,84 + 58 + MOVE_Y,60,58 * 2);
    compo_setid(btn_ok,COMD_PIC_LABEL_DINNER);

    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,GUI_SCREEN_WIDTH,GUI_SCREEN_HEIGHT);
    widget_page_scale_to(widget_page,GUI_SCREEN_WIDTH * 0.8,GUI_SCREEN_HEIGHT * 0.8);

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(i18n[STR_ALARM_CLOCK]));
    compo_textbox_set_location(textbox,33 + 48 / 2, 141 + 34 /2 - 13,100,40);
    compo_textbox_set(textbox, i18n[STR_ALARM_CLOCK]);

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(i18n[STR_GETUP]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 141 + 34 /2 - 13,100,40);
    compo_textbox_set(textbox, i18n[STR_GETUP]);

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(i18n[STR_METTING]));
    compo_textbox_set_location(textbox,84 + 48 / 2 + MOVE_X * 2, 141 + 34 /2 - 13,100,40);
    compo_textbox_set(textbox, i18n[STR_METTING]);

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(i18n[STR_APPOINTMENT]));
    compo_textbox_set_location(textbox,45 + 48 / 2 + MOVE_X / 2, 141 + 34 /2 + MOVE_Y + 15,100,40);
    compo_textbox_set(textbox, i18n[STR_APPOINTMENT]);

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(i18n[STR_DINNER_PARTY]));
    compo_textbox_set_location(textbox,74 + 48 / 2 + MOVE_X / 2 + MOVE_X, 141 + 34 /2 + MOVE_Y + 15,100,40);
    compo_textbox_set(textbox, i18n[STR_DINNER_PARTY]);


    pic = compo_picturebox_create(frm,alarm_label_get_switch(LABEL_ALARM)? UI_BUF_I341001_20_ALARM_CLOCK_SELECT_00_BIN:UI_BUF_I341001_20_ALARM_CLOCK_SELECT_01_BIN);
    compo_picturebox_set_pos(pic,72 + 20 / 2,178 + 20 / 2);
    compo_setid(pic,COMD_PIC_SWITCH_ALARM);

    pic = compo_picturebox_create(frm,alarm_label_get_switch(LABEL_SLEEP)? UI_BUF_I341001_20_ALARM_CLOCK_SELECT_00_BIN:UI_BUF_I341001_20_ALARM_CLOCK_SELECT_01_BIN);
    compo_picturebox_set_pos(pic,72 + 20 / 2 + MOVE_X,178 + 20 / 2);
    compo_setid(pic,COMD_PIC_SWITCH_SLEEP);

    pic = compo_picturebox_create(frm,alarm_label_get_switch(LABEL_MEETING)? UI_BUF_I341001_20_ALARM_CLOCK_SELECT_00_BIN:UI_BUF_I341001_20_ALARM_CLOCK_SELECT_01_BIN);
    compo_picturebox_set_pos(pic,72 + 20 / 2 + MOVE_X*2,178 + 20 / 2);
    compo_setid(pic,COMD_PIC_SWITCH_MEETING);

    pic = compo_picturebox_create(frm,alarm_label_get_switch(LABEL_DATING)? UI_BUF_I341001_20_ALARM_CLOCK_SELECT_00_BIN:UI_BUF_I341001_20_ALARM_CLOCK_SELECT_01_BIN);
    compo_picturebox_set_pos(pic,72 + 20 / 2 + MOVE_X / 2,178 + 20 / 2 + MOVE_Y);
    compo_setid(pic,COMD_PIC_SWITCH_DATING);

    pic = compo_picturebox_create(frm,alarm_label_get_switch(LABEL_DINNER)? UI_BUF_I341001_20_ALARM_CLOCK_SELECT_00_BIN:UI_BUF_I341001_20_ALARM_CLOCK_SELECT_01_BIN);
    compo_picturebox_set_pos(pic,72 + 20 / 2 + MOVE_X + MOVE_X / 2,178 + 20 / 2 + MOVE_Y);
    compo_setid(pic,COMD_PIC_SWITCH_DINNER);

    btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_20_ALARM_CLOCK_CONFIRM_BIN);
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,346 + 80 / 2);
    compo_setid(btn_ok,COMD_BUTTON_CONFIRM);



    return frm;
}

static void func_alarm_clock_sub_label_process(void)
{
    f_alarm_clock_sub_label_t *f_alarm_clock_sub_label = (f_alarm_clock_sub_label_t *)func_cb.f_cb;
    static u8 label = 0;
    compo_picturebox_t* pic = NULL;
    if(label != f_alarm_clock_sub_label->label)
    {
        label = f_alarm_clock_sub_label->label;
        for(u8 i = 0; i < ALARM_LABEL_COUNT; i++)
        {
            pic = compo_getobj_byid(COMD_PIC_SWITCH_ALARM + i);
            if(label == i)
            {
                compo_picturebox_set(pic,UI_BUF_I341001_20_ALARM_CLOCK_SELECT_00_BIN);
            }
            else
            {
                compo_picturebox_set(pic,UI_BUF_I341001_20_ALARM_CLOCK_SELECT_01_BIN);
            }
        }

    }
    func_process();
}

void func_alarm_clock_sub_label_click()
{
    f_alarm_clock_sub_label_t *f_alarm_clock_sub_label = (f_alarm_clock_sub_label_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMD_PIC_LABEL_ALARM:
            f_alarm_clock_sub_label->label = LABEL_ALARM;
            break;
        case COMD_PIC_LABEL_SLEEP:
            f_alarm_clock_sub_label->label = LABEL_SLEEP;
            break;
        case COMD_PIC_LABEL_MEETING:
            f_alarm_clock_sub_label->label = LABEL_MEETING;
            break;
        case COMD_PIC_LABEL_DATING:
            f_alarm_clock_sub_label->label = LABEL_DATING;
            break;
        case COMD_PIC_LABEL_DINNER:
            f_alarm_clock_sub_label->label = LABEL_DINNER;
            break;
        case COMD_BUTTON_CONFIRM:
            ute_moduleSystemtimeAlarmSetLabel(sys_cb.alarm_edit_idx,f_alarm_clock_sub_label->label);
#if UTE_MODULE_LOCAL_ALARM_REPEAT_REMIND_SUPPORT
            if(ble_is_connect())
            {
                uteModuleSystemtimeStartSendAllAlarmData();
            }
#endif
            func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
            break;
        default:
            break;
    }
}

static void func_alarm_clock_sub_label_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            func_alarm_clock_sub_label_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:

            break;

            break;

        default:
            func_message(msg);
            break;
    }
}

#else
compo_form_t *func_alarm_clock_sub_label_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    return frm;
}

static void func_alarm_clock_sub_label_process(void)
{
    func_process();
}
static void func_alarm_clock_sub_label_message(size_msg_t msg)
{
    func_message(msg);
}
#endif

static void func_alarm_clock_label_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_sub_label_t));
    func_cb.frm_main = func_alarm_clock_sub_label_form_create();

    f_alarm_clock_sub_label_t *f_alarm_clock_sub_label = (f_alarm_clock_sub_label_t*)func_cb.f_cb;
    f_alarm_clock_sub_label->label = ute_moduleSystemtimeAlarmGetLabel(sys_cb.alarm_edit_idx);
}

static func_alarm_clock_sub_label_exit(void)
{
    func_cb.last = FUNC_ALARM_CLOCK_SUB_LABEL;
}

void func_alarm_clock_sub_label(void)
{
    printf("%s\n", __func__);
    func_alarm_clock_label_enter();
    while (func_cb.sta == FUNC_ALARM_CLOCK_SUB_LABEL)
    {
        func_alarm_clock_sub_label_process();
        func_alarm_clock_sub_label_message(msg_dequeue());
    }
    func_alarm_clock_sub_label_exit();
}