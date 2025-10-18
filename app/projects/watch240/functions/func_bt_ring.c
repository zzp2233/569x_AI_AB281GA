#include "include.h"
#include "func.h"
#include "func_bt.h"
#include "ute_drv_motor.h"
#include "ute_module_message.h"
#include "ute_module_quickReply.h"
#include "ute_module_localRingtone.h"
#include "ute_module_notdisturb.h"
#include "func_cover.h"
#include "ute_module_quickReply.h"

#define TXT_X_MIN 20
enum
{
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_BTN_ANSWER,
    COMPO_ID_BTN_REJECT,

    COMPO_ID_COLOR_BLACK,//刷黑
    COMPO_ID_TXT_TIT,
    COMPO_ID_TXT_TIM,
    COMPO_ID_BTN_QUICKREPLY,
    COMPO_ID_TEXT_QUICK_START,
    COMPO_ID_TEXT_QUICK_END = COMPO_ID_TEXT_QUICK_START+UTE_MODUEL_QUICK_REPLY_MAX_MSG_COUNT,

};

typedef struct f_bt_call_t_
{
    char pbap_result_Name[50];//存放来电与接听联系人名字
    char tmp_pbap_result_Name[50];//存放来电与接听联系人名字
} f_bt_ring_t;

void func_bt_ring_number_update(void)
{
    if (bt_cb.number_sta)
    {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
        bt_pbap_lookup_number((char*)hfp_get_last_call_number(0));
    }
}
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    //printf("%s\n", __func__);
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);

    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+56, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    // compo_textbox_set_align_center(name_txt, false);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    // compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_max_height()/2);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 100, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
//    compo_setid(txt, COMPO_ID_TXT_IN_CALL);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X,138-widget_text_get_height()/2,GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);
    compo_textbox_set_align_center(txt, false);
    txt_leng = widget_text_get_area(txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    compo_textbox_set_pos(txt,txt_x,138-widget_text_get_height()/2);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_CALL_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, 240);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, 240);

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    //printf("%s\n", __func__);
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);

    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+56, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    // compo_textbox_set_align_center(name_txt, false);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    // compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_max_height()/2);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 100, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
//    compo_setid(txt, COMPO_ID_TXT_IN_CALL);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X,138-widget_text_get_height()/2,GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_forecolor(txt, COLOR_WHITE);
    compo_textbox_set_align_center(txt, false);
    txt_leng = widget_text_get_area(txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    compo_textbox_set_pos(txt,txt_x,138-widget_text_get_height()/2);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_12_ICON_CALLING_60X60_X14_90_166_Y202_01_ANSWER_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, 240);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_CALL_12_ICON_CALLING_60X60_X14_90_166_Y202_01_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, 240);

    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

typedef struct
{
    u8 hour;
    u8 am_pm;
} quick_12hour_convert_t;

quick_12hour_convert_t quick_convert_to_12hour(u8 time)
{
    u8 am_pm = (time >= 12) ? 2 : 1;    //2 PM, 1 AM
    quick_12hour_convert_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (time == 0)
        {
            hour12.hour = 12;
        }
        else if (time > 12)
        {
            hour12.hour = time - 12;
        }
        else
        {
            hour12.hour = time;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = time;
    hour12.am_pm = 0;
    return hour12;
}

static bool call_quick_msg_flag = false;

static void func_bt_call_quick_click_handle(void)
{
    if (call_quick_msg_flag)
    {
        point_t pt = ctp_get_sxy();
        for (int i=0; i<uteModuleQuickReplyGetTotalContentCnt(); i++)
        {
            compo_cardbox_t *quick_card = compo_getobj_byid(COMPO_ID_TEXT_QUICK_START + i);
            if (compo_cardbox_get_visible(quick_card) && compo_cardbox_btn_is(quick_card, pt) == true)
            {
                uteModuleQuickReplySendContentData(i);
#if UTE_MODULE_SCREEN_QUICK_MESSAGE_SEND_RESAULT_SUPPORT && APP_QUICK_REPLY_DISPLAY_RESAULT_SCREEN_NOW
                if (uteModuleQuickReplyGetIsSendSuccess())
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    sys_cb.cover_index = REMIND_COVER_SUCC;
                    msgbox((char*)i18n[STR_QUICK_SUCC], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
                }
                else
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    sys_cb.cover_index = REMIND_COVER_FAIL;
                    msgbox((char*)i18n[STR_QUICK_FAIL], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
                }
#endif
            }
        }
    }
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    //printf("%s\n", __func__);
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);

    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 120, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    compo_textbox_set_visible(name_txt, false);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 120, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    compo_textbox_set_visible(number_txt, false);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
//    compo_setid(txt, COMPO_ID_TXT_IN_CALL);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X,191+widget_text_get_height()/2,GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_forecolor(txt, COLOR_WHITE);
    compo_textbox_set_align_center(txt, false);
    txt_leng = widget_text_get_area(txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    compo_textbox_set_pos(txt,txt_x,191-widget_text_get_height()/2);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I341001_11_CALL_BUTT_ANSWER_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, 310+48);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I341001_23_SOS_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, 310+48);

    //快捷回复按钮
    ute_quickReply_receive_t quick_data[UTE_MODUEL_QUICK_REPLY_MAX_MSG_COUNT] = {0};
    uteModuleQuickReplyGetContentData(quick_data);
    compo_button_t* quickbtn = compo_button_create_by_image(frm, UI_BUF_I341001_QUICK_MESSAGE_REPLY_MESSAGE_BIN);
    compo_button_set_location(quickbtn, 146+76/2, 228+76/2, 76, 76);
    compo_button_set_visible(quickbtn, false);
    printf("uteModuleCallIsCurrentConnectionIphone = %d\n", uteModuleCallIsCurrentConnectionIphone());
    if (uteModuleCallIsCurrentConnectionIphone() == false)   // 是否连接的是IOS; true: Iphone; false: 安卓
    {
        if (uteModuleQuickReplyGetStatus() && uteModuleQuickReplyGetTotalContentCnt() > 0)
        {
            compo_button_set_visible(quickbtn, true);
        }
    }
    compo_setid(quickbtn, COMPO_ID_BTN_QUICKREPLY);

    //刷黑
    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_setid(shape, COMPO_ID_COLOR_BLACK);
    compo_shape_set_color(shape, COLOR_BLACK);
    compo_shape_set_visible(shape, false);

    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);  //获取系统时间
    char time_buff[20] = {0};
    quick_12hour_convert_t timeHour = quick_convert_to_12hour(time.hour);
    if (timeHour.am_pm > 0)
    {
        snprintf(time_buff, sizeof(time_buff), "%s%02d:%02d", timeHour.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM], timeHour.hour, time.min);
    }
    else
    {
        snprintf(time_buff, sizeof(time_buff), "%02d:%02d", timeHour.hour, time.min);
    }
    compo_textbox_t *txtTil = compo_textbox_create(frm, strlen(i18n[STR_QUICK_REPLY]));
    compo_textbox_set_location(txtTil, FORM_TITLE_LEFT, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 4 - FORM_TITLE_LEFT, 50);
    compo_textbox_set_align_center(txtTil, false);
    compo_textbox_set(txtTil, i18n[STR_QUICK_REPLY]);
    compo_setid(txtTil, COMPO_ID_TXT_TIT);
    compo_textbox_set_visible(txtTil, false);
    compo_textbox_t *txtTime = compo_textbox_create(frm, 20);
    compo_textbox_set_location(txtTime, FORM_TITLE_RIGHT + 20, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12, FORM_TITLE_HEIGHT);
    compo_textbox_set_align_center(txtTime, false);
    compo_textbox_set_multiline(txtTime, false);
    compo_textbox_set(txtTime, time_buff);
    compo_setid(txtTime, COMPO_ID_TXT_TIM);
    compo_textbox_set_visible(txtTime, false);

    for (int i=0; i<uteModuleQuickReplyGetTotalContentCnt(); i++)
    {
        compo_cardbox_t *quick_card = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 80);
        compo_cardbox_set_location(quick_card, GUI_SCREEN_CENTER_X, 48+80/2+80*i, GUI_SCREEN_WIDTH, 80);
        if (i != uteModuleQuickReplyGetTotalContentCnt() - 1)
        {
            compo_cardbox_rect_set_color(quick_card, 0, make_color(0x29, 0x29, 0x29));
            compo_cardbox_rect_set_location(quick_card, 0, 0, 80/2-1, 320, 1, 0);
        }
        compo_cardbox_icon_set(quick_card, 0, UI_BUF_I341001_QUICK_MESSAGE_SEND_BIN);
        compo_cardbox_icon_set_location(quick_card, 0, 140, 0, 34, 34);
        compo_cardbox_text_set_location(quick_card, 0, 24-GUI_SCREEN_CENTER_X, -widget_text_get_height()/2, 253, 48);
//        widget_text_set_wordwrap(quick_card->text[0], true);
        compo_cardbox_text_set(quick_card, 0, quick_data[i].receiveContent);
//        if (widget_text_get_area(quick_card->text[0]).hei 40)
//        {
//            compo_cardbox_text_set_location(quick_card, 0, 24-GUI_SCREEN_CENTER_X, -widget_text_get_height(), 253, 80);
//            compo_cardbox_text_set(quick_card, 0, quick_data[i].receiveContent);
//        }
        compo_cardbox_text_set_align_center(quick_card, 0, false);
        compo_cardbox_set_visible(quick_card, false);
        compo_setid(quick_card, COMPO_ID_TEXT_QUICK_START + i);
        compo_cardbox_text_scroll_process(quick_card, true);
    }

    return frm;
}

static void func_bt_call_quick_msg_update(void)
{
    static bool call_quick_msg_flag_rev = false;
    if (call_quick_msg_flag_rev == call_quick_msg_flag) return;
    else call_quick_msg_flag_rev = call_quick_msg_flag;
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_COLOR_BLACK);
    compo_button_t* quickbtn = compo_getobj_byid(COMPO_ID_BTN_QUICKREPLY);
    compo_textbox_t *time = compo_getobj_byid(COMPO_ID_TXT_TIM);
    compo_textbox_t *title = compo_getobj_byid(COMPO_ID_TXT_TIT);
    if(call_quick_msg_flag)
    {
        compo_textbox_set_visible(time, true);
        compo_textbox_set_visible(title, true);
        for (int i=0; i<uteModuleQuickReplyGetTotalContentCnt(); i++)
        {
            compo_cardbox_t *quick_card = compo_getobj_byid(COMPO_ID_TEXT_QUICK_START + i);
            compo_cardbox_set_visible(quick_card, true);
            compo_cardbox_text_scroll_reset(quick_card);
        }
        compo_shape_set_visible(shape, true);
        compo_button_set_visible(quickbtn, false);
    }
    else
    {
        compo_textbox_set_visible(time, false);
        compo_textbox_set_visible(title, false);
        for (int i=0; i<uteModuleQuickReplyGetTotalContentCnt(); i++)
        {
            compo_cardbox_t *quick_card = compo_getobj_byid(COMPO_ID_TEXT_QUICK_START + i);
            compo_cardbox_set_visible(quick_card, false);
        }
        compo_shape_set_visible(shape, false);
        compo_button_set_visible(quickbtn, true);
    }
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    //printf("%s\n", __func__);
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_PHONE]);

    s16 txt_leng=0;
    s16 txt_x=0;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, 28/2+80, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);
    // compo_textbox_set_align_center(name_txt, false);
    txt_leng = widget_text_get_area(name_txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    // compo_textbox_set_pos(name_txt,txt_x,74-widget_text_get_max_height()/2);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, 126+10, GUI_SCREEN_WIDTH/1.2, widget_text_get_max_height());
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
//    compo_setid(txt, COMPO_ID_TXT_IN_CALL);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X,191+widget_text_get_height()/2,GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_forecolor(txt, COLOR_WHITE);
    compo_textbox_set_align_center(txt, false);
    txt_leng = widget_text_get_area(txt->txt).wid;
    txt_x = GUI_SCREEN_CENTER_X-txt_leng/2;
    if(TXT_X_MIN>txt_x)txt_x = TXT_X_MIN;
    compo_textbox_set_pos(txt,txt_x,191-widget_text_get_height()/2);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I343001_11_CALL_ANSWER_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, 272+80/2);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I343001_23_SOS_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, 272+80/2);

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X,83, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_CALL_CALLING_CALL_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, 196+98/2, 279);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, 108, 279);


    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X,83, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    // compo_textbox_set_forecolor(txt, COLOR_GREEN);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I338001_11_CALL_ANSWER_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, 196+98/2, 279);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I338001_11_CALL_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, 108, 279);


    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X,83, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,126, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 169, GUI_SCREEN_WIDTH/1.2, 30);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    compo_textbox_set_forecolor(txt, COLOR_GREEN);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_CALL_CALLING_CALL_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, 196+98/2, 279);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, 108, 279);


    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_ring_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X,43, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(name_txt, sys_cb.pbap_result_Name);
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_font( number_txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X,77, GUI_SCREEN_WIDTH/1.2, 50);
    compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_ME]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 103, GUI_SCREEN_WIDTH/1.2, 30);
    compo_textbox_set(txt, i18n[STR_CALL_ME]);
    // compo_textbox_set_forecolor(txt, COLOR_GREEN);

    //接听
    btn = compo_button_create_by_image(frm, UI_BUF_I342001_11_CALL_BUTTON_PRESS_ANSWER_BIN);
    compo_setid(btn, COMPO_ID_BTN_ANSWER);
    compo_button_set_pos(btn, 44+54/2, 159+54/2);

    //挂断
    btn = compo_button_create_by_image(frm, UI_BUF_I342001_11_CALL_BUTTON_PRESS_HANG_UP_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, 142+54/2, 159+54/2);


    return frm;
}

#else
compo_form_t *func_bt_ring_form_create(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT


// 判断从指定位置开始的字节序列是否是一个完整的UTF - 8字符
static int is_complete_utf8_char(const char *str, int pos)
{
    unsigned char byte = str[pos];
    if ((byte & 0x80) == 0)
    {
        // 单字节字符，肯定是完整的
        return 1;
    }
    int num_bytes = 0;
    if ((byte & 0xE0) == 0xC0)
    {
        num_bytes = 2;
    }
    else if ((byte & 0xF0) == 0xE0)
    {
        num_bytes = 3;
    }
    else if ((byte & 0xF8) == 0xF0)
    {
        num_bytes = 4;
    }
    for (int i = 1; i < num_bytes; i++)
    {
        if ((str[pos + i] & 0xC0)!= 0x80)
        {
            return 0;
        }
    }
    return 1;
}

// 截取有用数据并在尾部补上三个...
void truncate_and_append(const char *src, char *dst, int dst_size)
{
    int i = 0;
    int j = 0;
    while (i < strlen(src) && j < dst_size - 1)
    {
        if (is_complete_utf8_char(src, i))
        {
            dst[j++] = src[i++];
        }
        else
        {
            break;
        }
    }
    if (j < dst_size - 3)
    {
        if (strlen(src) > PBAP_MAX_NAME_LEN)
        {
            dst[j++] = '.';
            dst[j++] = '.';
            dst[j++] = '.';
        }
    }
    dst[j] = '\0';
}

void func_bt_ring_up_date_process(void)
{
    f_bt_ring_t *f_bt_ring = (f_bt_ring_t*)func_cb.f_cb;

    compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
    compo_textbox_t *name_txt  = compo_getobj_byid(COMPO_ID_TXT_NAME);
    if(strcmp(f_bt_ring->pbap_result_Name, sys_cb.pbap_result_Name)!=0)
    {
        compo_textbox_set_visible(number_txt, false);
        memcpy(f_bt_ring->pbap_result_Name, sys_cb.pbap_result_Name, 50);

        memset(f_bt_ring->tmp_pbap_result_Name, '\0', sizeof(f_bt_ring->tmp_pbap_result_Name));
        truncate_and_append(sys_cb.pbap_result_Name, f_bt_ring->tmp_pbap_result_Name, sizeof(f_bt_ring->tmp_pbap_result_Name));
        compo_textbox_set(name_txt, f_bt_ring->tmp_pbap_result_Name);
        compo_textbox_set_visible(name_txt, true);
    }
    else
    {
        if (strlen(f_bt_ring->pbap_result_Name) == 0)
        {
            compo_textbox_set_visible(number_txt, true);
        }
    }
}
void func_bt_ring_process(void)
{
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    if (call_quick_msg_flag)
    {
        //卡片文本滚动
        for (int i=0; i<uteModuleQuickReplyGetTotalContentCnt(); i++)
        {
            compo_cardbox_t *quick_card = compo_getobj_byid(COMPO_ID_TEXT_QUICK_START + i);
            //        if (id - COMPO_ID_TEXT_QUICK_START > uteModuleQuickReplyGetTotalContentCnt() - 1)
            compo_cardbox_text_scroll_process(quick_card, true);
        }

    }
#endif // GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

    bsp_bt_ring_process();
    func_bt_ring_up_date_process();

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_INCOMING)  //退出来电页面
    {
        func_directly_back_to();
#if UTE_MODULE_CALL_NOT_SHOW_CALL_SCREEN_WHEN_SCO_OFF_SUPPORT
        if ((bt_cb.disp_status == BT_STA_OUTGOING || bt_cb.disp_status == BT_STA_INCALL) && sys_cb.sco_state)
        {
            func_cb.sta = FUNC_BT_CALL;
        }
#else
        if (bt_cb.disp_status == BT_STA_OUTGOING || bt_cb.disp_status == BT_STA_INCALL)
        {
            func_cb.sta = FUNC_BT_CALL;
        }
#endif
    }
#endif
}

static void func_bt_ring_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_ANSWER:
            printf("COMPO_ID_BTN_ANSWER\n");
            bt_call_answer_incoming();
            break;

        case COMPO_ID_BTN_REJECT:
            printf("COMPO_ID_BTN_REJECT\n");
            bt_call_terminate();
            break;

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case COMPO_ID_BTN_QUICKREPLY:       //快捷回复
            //使用弹窗显示
            if(uteModuleCallBtIsConnected())  //连接BT后，apk不会发号码下来
            {
                uint8_t* number = hfp_get_last_call_number(0);
                printf("COMPO_ID_BTN_QUICKREPLY -> number=%s\n", number);
                uteModuleQuickReplySetNumber(number, strlen(number));
            }
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_MODULE_QUICK_REPLY_READ_DATA, 0);
            call_quick_msg_flag = true;

//            int res = msgbox("测试测试", NULL, NULL, 0, 0);
            break;
#endif // GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

        default:
            break;
    }

}

//蓝牙通话消息处理
static void func_bt_ring_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            if (call_quick_msg_flag)
            {
                func_bt_call_quick_click_handle();
            }
            else
#endif // GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
            {
                func_bt_ring_click();                         //单击按钮
            }
            break;

        case EVT_CALL_NUMBER_UPDATE:
            func_bt_ring_number_update();
            break;

        case MSG_SYS_500MS:
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            func_bt_call_quick_msg_update();
#endif // GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
            reset_sleep_delay_all();                           //来电不休眠
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
        case MSG_CTP_SHORT_RIGHT:
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            if (call_quick_msg_flag)                            //退出快速消息回复页面
            {
                call_quick_msg_flag = false;
//                func_bt_call_quick_msg_update();
            }
#endif // GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
            break;

        case MSG_CTP_SHORT_LEFT:
            break;

        default:
            func_message(msg);
            break;
    }
}

void func_bt_ring_enter(void)
{
    call_quick_msg_flag = false;
    func_cb.f_cb = func_zalloc(sizeof(f_bt_ring_t));
    func_cb.frm_main = func_bt_ring_form_create();

    func_cb.mp3_res_play = func_bt_mp3_res_play;
    bsp_bt_ring_enter();

    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }

    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,0xff);
}

void func_bt_ring_exit(void)
{
    bsp_bt_ring_exit();
    uteDrvMotorStop();

#if UTE_MODULE_BT_RINGTONE_USE_LOCAL_RESOURCE
    uteModuleLocalRingtoneStopRing();
#endif

    func_cb.last = FUNC_BT_RING;
}

void func_bt_ring(void)
{
    u16 interval = 0, latency = 0, tout = 0;

    printf("%s\n", __func__);

    // Adjust BLE connection parameter when incoming
    if (ble_is_connect() && (ble_get_conn_interval() < 400))
    {
        interval = ble_get_conn_interval();
        latency = ble_get_conn_latency();
        tout = ble_get_conn_timeout();
        ble_update_conn_param(480, 0, 500);
    }

    func_bt_ring_enter();
    while (func_cb.sta == FUNC_BT_RING)
    {
        func_bt_ring_process();
        func_bt_ring_message(msg_dequeue());
    }
    func_bt_ring_exit();

    // Restore BLE connection parameter when call stop
    if (bt_cb.disp_status != BT_STA_INCALL)
    {
        if (interval | latency | tout)
        {
            ble_update_conn_param(interval, latency, tout);
        }
    }
}
