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
#define ALARM_DELETE(idx)               alarm_clock_delete(idx)
#define ALARM_GET_HOUR(idx)             app_data.sector0.alarm[idx].hour
#define ALARM_GET_MIN(idx)              app_data.sector0.alarm[idx].minute
#define ALARM_GET_CYCLE(idx)            app_data.sector0.alarm[idx].cycle
#elif (USE_APP_TYPE == USE_UTE_APP)
#define ALARM_ENABLE_CNT()              uteModuleSystemtimeGetAlarmTotalCnt()
#define ALARM_DELETE(idx)               uteModuleSystemtimeDeleteAlarm(idx)
#define ALARM_GET_HOUR(idx)             uteModuleSystemtimeGetAlarmHour(idx)
#define ALARM_GET_MIN(idx)              uteModuleSystemtimeGetAlarmMin(idx)
#define ALARM_GET_CYCLE(idx)            uteModuleSystemtimeGetAlarmCycle(idx)
#else
#define ALARM_ENABLE_CNT()              0
#define ALARM_DELETE(idx)
#define ALARM_GET_HOUR(idx)             0
#define ALARM_GET_MIN(idx)              0
#define ALARM_GET_CYCLE(idx)            0
#endif

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_DEL = 1,
    COMPO_ID_BTN_SET,
    COMPO_ID_BTN_REPEAT,

    //图像
    COMPO_ID_PIC_DEL_CLICK,

};

typedef struct f_alarm_clock_sub_edit_t_
{

} f_alarm_clock_sub_edit_t;

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_sub_edit_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_EDIT]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_ALARM_CLOCK_DELETE_BIN);
    compo_setid(btn, COMPO_ID_BTN_DEL);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei / 2 - 5);

#define SELF_TXT_OFFSET     50
#define SELF_PIC_OFFSET     10

    u8 hour, min;
    hour = ALARM_GET_HOUR(sys_cb.alarm_edit_idx);
    min = ALARM_GET_MIN(sys_cb.alarm_edit_idx);
    char aclock_str[8];
    sprintf(aclock_str, "%02d:%02d", hour, min);
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 5);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, 30, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3);
    compo_textbox_set(txt, aclock_str);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_SET);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X,
                              (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3 + SELF_TXT_OFFSET + SELF_PIC_OFFSET,
                              GUI_SCREEN_WIDTH, 50);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_REPEAT);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X,
                              (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3 + SELF_TXT_OFFSET*2 + SELF_PIC_OFFSET,
                              GUI_SCREEN_WIDTH, 50);

    txt = compo_textbox_create(frm, strlen(i18n[STR_REVISE_TIMR]));
    compo_textbox_set_align_center(txt, false);
    //compo_textbox_set_pos(txt, 30, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3 + SELF_TXT_OFFSET);
    compo_textbox_set(txt, i18n_zh_rcn[STR_REVISE_TIMR]);
    compo_textbox_set_location(txt, 30, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3 + SELF_TXT_OFFSET
                               , widget_text_get_area(txt->txt).wid, widget_text_get_area(txt->txt).hei);
    compo_textbox_set(txt, i18n[STR_REVISE_TIMR]);

    txt = compo_textbox_create(frm, strlen(i18n[STR_SET_REPEAT]));
    compo_textbox_set_align_center(txt, false);
//    compo_textbox_set_pos(txt, 30, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3 + SELF_TXT_OFFSET*2);
    compo_textbox_set(txt, i18n_zh_rcn[STR_SET_REPEAT]);
    compo_textbox_set_location(txt, 30, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3 + SELF_TXT_OFFSET*2
                               , widget_text_get_area(txt->txt).wid, widget_text_get_area(txt->txt).hei);
    compo_textbox_set(txt, i18n[STR_SET_REPEAT]);

    //新建图像
    compo_picturebox_t *pic_click = compo_picturebox_create(frm, UI_BUF_ALARM_CLOCK_DELETE_CLICK_BIN);
    compo_setid(pic_click, COMPO_ID_PIC_DEL_CLICK);
    compo_picturebox_set_pos(pic_click, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei / 2 - 5);
    compo_picturebox_set_visible(pic_click, false);

    pic_click = compo_picturebox_create(frm, UI_BUF_ALARM_CLOCK_OPEN_BIN);
    compo_picturebox_set_pos(pic_click, GUI_SCREEN_WIDTH /1.2,(GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3 + SELF_TXT_OFFSET + SELF_PIC_OFFSET);

    pic_click = compo_picturebox_create(frm, UI_BUF_ALARM_CLOCK_OPEN_BIN);
    compo_picturebox_set_pos(pic_click, GUI_SCREEN_WIDTH /1.2, (GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_DELETE_BIN).hei)/3 + SELF_TXT_OFFSET*2 + SELF_PIC_OFFSET);



    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_sub_edit_button_touch_handle(void)
{

    int id = compo_get_button_id();
    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_DEL_CLICK);

    switch (id)
    {
        case COMPO_ID_BTN_DEL:
            compo_picturebox_set_visible(pic_click, true);
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_alarm_clock_sub_edit_button_release_handle(void)
{
    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_DEL_CLICK);
    compo_picturebox_set_visible(pic_click, false);
}

//单击按钮
static void func_alarm_clock_sub_edit_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_DEL:
            ALARM_DELETE(sys_cb.alarm_edit_idx);
            func_cb.sta = FUNC_ALARM_CLOCK;
            break;

        case COMPO_ID_BTN_SET:
            func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
            break;

        case COMPO_ID_BTN_REPEAT:
            func_cb.sta = FUNC_ALARM_CLOCK_SUB_REPEAT;
            break;

        default:
            break;
    }

    func_alarm_clock_sub_edit_button_release_handle();
}

//闹钟功能事件处理
static void func_alarm_clock_sub_edit_process(void)
{
    func_process();
}

//闹钟功能消息处理
static void func_alarm_clock_sub_edit_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_alarm_clock_sub_edit_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_alarm_clock_sub_edit_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_alarm_clock_sub_edit_button_release_handle();
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_alarm_clock_sub_edit_button_release_handle();
            func_message(msg);
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入闹钟功能
static void func_alarm_clock_sub_edit_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_sub_edit_t));
    func_cb.frm_main = func_alarm_clock_sub_edit_form_create();
}

//退出闹钟功能
static void func_alarm_clock_sub_edit_exit(void)
{
}

//闹钟功能
void func_alarm_clock_sub_edit(void)
{
    printf("%s\n", __func__);
    func_alarm_clock_sub_edit_enter();
    while (func_cb.sta == FUNC_ALARM_CLOCK_SUB_EDIT)
    {
        func_alarm_clock_sub_edit_process();
        func_alarm_clock_sub_edit_message(msg_dequeue());
    }
    func_alarm_clock_sub_edit_exit();
}
