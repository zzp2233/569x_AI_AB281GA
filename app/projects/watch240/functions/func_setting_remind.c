#include "include.h"
#include "func.h"
#include "ute_application_common.h"
#include "ute_module_settingremind.h"
#include "ute_module_liftwrist.h"
#include "ute_module_localRingtone.h"
#include "ute_module_gui_common.h"
#include "ute_module_notdisturb.h"
#include "ute_module_sport.h"

#if UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
typedef struct f_setting_remind_t_
{
    bool click_state;
    bool saving_state;
    u32 tick;
} f_setting_remind_t;

enum
{
    COMPO_ID_BTN_SURE = 1,
    COMPO_ID_BTN_CLICK,
    COMPO_ID_PIC_CLICK,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

typedef struct
{
    u8 hour;
    u8 am_pm;
} time_12hour_convert_t;

time_12hour_convert_t time12Hourconversion(u8 time)
{
    u8 am_pm = (time >= 12) ? 2 : 1;    //2 PM, 1 AM
    time_12hour_convert_t hour12;
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

//bool UserNotDisturbIsTimeBucket(void)
//{
//    bool isNotDisturbTimeNow = false;
//    uint16_t hourMin = 0;
//    {
//        ute_module_systemtime_time_t time;
//        uteModuleSystemtimeGetTime(&time);
//        hourMin = time.hour*60+time.min;
//        if(uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) < uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME))
//        {
//            if((hourMin >= uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME)) && (hourMin <= uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME)))
//            {
//                isNotDisturbTimeNow = true;
//            }
//        }
//        else
//        {
//            if((hourMin >= uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME)) || (hourMin <= uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME)))
//            {
//                isNotDisturbTimeNow = true;
//            }
//        }
//    }
//    return isNotDisturbTimeNow;
//}

extern bool savingState;
extern bool scheduleOldState;
compo_form_t *func_setting_remind_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    char str_buff[200] = {0};
    compo_textbox_t *textbox = NULL;

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I341001_28_SET_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I341001_28_SET_CONFIRM_BIN).hei/2-20);
    compo_setid(btn, COMPO_ID_BTN_SURE);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, 80, 300, 58, 58);
    compo_setid(btn, COMPO_ID_BTN_CLICK);
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I341001_20_ALARM_CLOCK_SELECT_01_BIN);
    compo_picturebox_set_pos(pic, 94, 316);
    compo_setid(pic, COMPO_ID_PIC_CLICK);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_NO_PROMPT]));
    compo_textbox_set_location(textbox, 119, 300, 220, 50);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, i18n[STR_NO_PROMPT]);

    if (uteModuleSettingRemindGetRemindId() == SETTING_REMIND_ID_WRIST)
    {
        textbox = compo_textbox_create(frm, 200);
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/5*2, 328, 240);
        compo_textbox_set_multiline(textbox, true);
        widget_text_set_ellipsis(textbox->txt, false);
        if (uteModuleLiftWristIsTimeBucket() || uteModuleLiftWristIsOpen())
        {
            compo_textbox_set(textbox,  i18n[STR_WRIST_OFF]);
            if (widget_text_get_area(textbox->txt).wid <= 328 && widget_text_get_area(textbox->txt).hei < widget_text_get_height())
            {
                compo_textbox_set_multiline(textbox, false);
            }
        }
        else
        {
            if(uteModuleLiftWristGetOldScheduledOpenStatus() && !uteModuleLiftWristIsOpenScheduled())
            {
                if(uteModuleSystemtime12HOn())
                {
                    time_12hour_convert_t startHour = time12Hourconversion(uteModuleLiftWristGetTime(LIFT_WRIST_START_TIME) >> 8);
                    time_12hour_convert_t endHour = time12Hourconversion(uteModuleLiftWristGetTime(LIFT_WRIST_END_TIME) >> 8);
                    snprintf(str_buff, 200, "%s\n%s:\n%02d:%02d%s-%02d:%02d%s", i18n[STR_WRIST_ON], i18n[STR_EFFCT_TIME], \
                             startHour.hour, uteModuleLiftWristGetTime(LIFT_WRIST_START_TIME) & 0xff, startHour.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM],\
                             endHour.hour, uteModuleLiftWristGetTime(LIFT_WRIST_END_TIME) & 0xff, endHour.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM]);
                }
                else
                {
                    snprintf(str_buff, 200, "%s\n%s:\n%02d:%02d-%02d:%02d", i18n[STR_WRIST_ON], i18n[STR_EFFCT_TIME], \
                             (uteModuleLiftWristGetTime(LIFT_WRIST_START_TIME) >> 8) & 0xff, uteModuleLiftWristGetTime(LIFT_WRIST_START_TIME) & 0xff, \
                             (uteModuleLiftWristGetTime(LIFT_WRIST_END_TIME) >> 8) & 0xff, uteModuleLiftWristGetTime(LIFT_WRIST_END_TIME) & 0xff);
                }
                compo_textbox_set(textbox, str_buff);
            }
            else
            {
                compo_textbox_set(textbox, i18n[STR_WRIST_ON]);
                if (widget_text_get_area(textbox->txt).wid <= 328 && widget_text_get_area(textbox->txt).hei < widget_text_get_height())
                {
                    compo_textbox_set_multiline(textbox, false);
                }
            }
        }
    }
    else if (uteModuleSettingRemindGetRemindId() == SETTING_REMIND_ID_DISTUB)
    {
        textbox = compo_textbox_create(frm, 200);
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/5*2, 328, 240);
        compo_textbox_set_multiline(textbox, true);
        widget_text_set_ellipsis(textbox->txt, false);
        if (uteModuleNotDisturbGetOpenStatus() || uteModuleNotDisturbIsTimeBucket())
        {
            compo_textbox_set(textbox,  i18n[STR_DISTURD_CLOSE]);
            if (widget_text_get_area(textbox->txt).wid <= 328 && widget_text_get_area(textbox->txt).hei < widget_text_get_height())
            {
                compo_textbox_set_multiline(textbox, false);
            }
        }
        else
        {
            if (scheduleOldState && !uteModuleNotDisturbIsOpenScheduled())
            {
                if(uteModuleSystemtime12HOn())
                {
                    time_12hour_convert_t startHour = time12Hourconversion(uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) / 60);
                    time_12hour_convert_t endHour = time12Hourconversion(uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) / 60);
                    snprintf(str_buff, 200, "%s\n%s:\n%02d:%02d%s-%02d:%02d%s", i18n[STR_DISTURD_OPEN], i18n[STR_EFFCT_TIME], \
                             startHour.hour, (int)uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) % 60, startHour.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM],\
                             endHour.hour, (int)uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) % 60, endHour.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM]);
                }
                else
                {
                    snprintf(str_buff, 200, "%s\n%s:\n%02d:%02d-%02d:%02d", i18n[STR_DISTURD_OPEN], i18n[STR_EFFCT_TIME], \
                             (int)uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) / 60, (int)uteModuleNotDisturbGetTime(NOT_DISTURB_START_TIME) % 60, \
                             (int)uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) / 60, (int)uteModuleNotDisturbGetTime(NOT_DISTURB_END_TIME) % 60);
                }
                compo_textbox_set(textbox, str_buff);
            }
            else
            {
                compo_textbox_set(textbox, i18n[STR_DISTURD_OPEN]);
                if (widget_text_get_area(textbox->txt).wid <= 328 && widget_text_get_area(textbox->txt).hei < widget_text_get_height())
                {
                    compo_textbox_set_multiline(textbox, false);
                }
            }
        }
    }
    else if (uteModuleSettingRemindGetRemindId() == SETTING_REMIND_ID_MUTE)
    {
        textbox = compo_textbox_create(frm, 100);
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/5*2, 328, 240);
        compo_textbox_set_multiline(textbox, true);
        widget_text_set_ellipsis(textbox->txt, false);
        if(!uteModuleLocalRingtoneGetMuteStatus())
        {
            compo_textbox_set(textbox, i18n[STR_SILENT_ON]);
            if (widget_text_get_area(textbox->txt).wid <= 328 && widget_text_get_area(textbox->txt).hei < widget_text_get_height())
            {
                compo_textbox_set_multiline(textbox, false);
            }
        }
        else
        {
            compo_textbox_set(textbox, i18n[STR_SILENT_OFF]);
            if (widget_text_get_area(textbox->txt).wid <= 328 && widget_text_get_area(textbox->txt).hei < widget_text_get_height())
            {
                compo_textbox_set_multiline(textbox, false);
            }
        }
    }
    else if (uteModuleSettingRemindGetRemindId() == SETTING_REMIND_ID_SAVING)
    {
        compo_button_set_visible(btn, false);
        compo_picturebox_set_visible(pic, false);
        compo_textbox_set_visible(textbox, false);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_SAVING_TIP]));
        compo_textbox_set_multiline(textbox, true);
        widget_text_set_ellipsis(textbox->txt, false);
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/5*2, 328, 240);
        if (savingState)//(uteModuleGuiCommonGetPowerSavingModeOpen())
        {
            compo_textbox_set(textbox, i18n[STR_SAVING_TIP]);
        }
        else
        {
            compo_textbox_set(textbox, i18n[STR_POWER_SAVE_OFF]);
            if (widget_text_get_area(textbox->txt).wid <= 328 && widget_text_get_area(textbox->txt).hei < widget_text_get_height())
            {
                compo_textbox_set_multiline(textbox, false);
            }
        }
    }

    return frm;
}

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if (quick.isTurnTheWrist)
    {
        if (!uteModuleLiftWristIsTimeBucket() && !uteModuleLiftWristIsOpen())
        {
            return;
        }
    }
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    //uteApplicationCommonSendQuickSwitchStatus();
}

//单击按钮
static void func_setting_remind_click(void)
{
    f_setting_remind_t *f_setting_remind = (f_setting_remind_t *)func_cb.f_cb;
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC_CLICK);

    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
        {
            if (f_setting_remind->click_state)
            {
                uteModuleSettingRemindSetNotNeedShowId(uteModuleSettingRemindGetRemindId());
            }
            if (uteModuleSettingRemindGetRemindId() == SETTING_REMIND_ID_SAVING)
            {
                if (savingState)
                {
                    uteModuleGuiCommonSetPowerSavingModeOpen(true);
                    tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(), true);
                }
                else
                {
                    uteModuleGuiCommonSetPowerSavingModeOpen(false);
                    tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(), true);
                }
                //uteTaskGuiStartScreen(FUNC_SAVING_SUB_SHOW, 0, __func__);
            }
            else if (uteModuleSettingRemindGetRemindId() == SETTING_REMIND_ID_MUTE)
            {
                if (uteModuleLocalRingtoneGetMuteStatus())
                {
                    uteModuleLocalRingtoneSetMuteStatus(false, true);
                }
                else
                {
                    uteModuleLocalRingtoneSetMuteStatus(true, true);
                }
            }
            else if (uteModuleSettingRemindGetRemindId() == SETTING_REMIND_ID_DISTUB)
            {
                ute_quick_switch_t quick;
                uteApplicationCommonGetQuickSwitchStatus(&quick);
                if(!quick.isNotDisturb)
                {
                    if (scheduleOldState)
                    {
                        uteModuleNotDisturbScheduledSwitch();
                    }
                    else
                    {
                        uteModuleNotDisturbAllDaySwitch();
                    }
                }
                else
                {
                    if (uteModuleNotDisturbGetOpenStatus())
                    {
                        uteModuleNotDisturbAllDaySwitch();
                    }
                    else
                    {
                        if (!uteModuleNotDisturbIsTimeBucket())
                        {
                            uteModuleNotDisturbScheduledSwitch();
                            uteModuleNotDisturbAllDaySwitch();
                            scheduleOldState = false;
                        }
                        else
                        {
                            uteModuleNotDisturbScheduledSwitch();
                        }
                    }
                }
            }
            else if (uteModuleSettingRemindGetRemindId() == SETTING_REMIND_ID_WRIST)
            {
                switch_set_sub_wrist();
                ute_quick_switch_t quick;
                uteApplicationCommonGetQuickSwitchStatus(&quick);
                if (quick.isTurnTheWrist)
                {
                    if (uteModuleLiftWristGetOldScheduledOpenStatus())
                    {
                        uteModuleLiftWristSetScheduled(true);
                        if (!uteModuleLiftWristIsTimeBucket())
                        {
                            uteModuleLiftWristSetOpenStatus(true);
                            uteModuleLiftWristSetScheduled(false);
                        }
                    }
                    else
                    {
                        uteModuleLiftWristSetOpenStatus(true);
                    }
                    uteModuleLiftWristRecordOpenStatus();
                }
                else
                {
                    uteModuleLiftWristSetOpenStatus(false);
                    uteModuleLiftWristSetScheduled(false);
                    uteModuleLiftWristSetNowOpenStatus(LIFT_WRIST_CLOSE);
                }
            }
            func_back_to();
        }
        break;
        case COMPO_ID_BTN_CLICK:
        {
            f_setting_remind->click_state = !f_setting_remind->click_state;
            compo_picturebox_set(picbox, f_setting_remind->click_state ? UI_BUF_I341001_20_ALARM_CLOCK_SELECT_00_BIN : UI_BUF_I341001_20_ALARM_CLOCK_SELECT_01_BIN);
        }
        break;
        default:
            break;
    }

}

static void func_setting_remind_message(size_msg_t msg)
{
    f_setting_remind_t *f_setting_remind = (f_setting_remind_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            f_setting_remind->tick = tick_get();
            break;
        case MSG_CTP_CLICK:
            func_setting_remind_click();
            break;

//        case KU_BACK:
//        case MSG_CTP_SHORT_RIGHT:
//            func_switch_to(FUNC_POWER_ON_SCAN, FUNC_SWITCH_LR_RIGHT | FUNC_SWITCH_AUTO);
//            break;
        default:
            func_message(msg);
            break;
    }
}
//下滑事件流程处理
static void func_setting_remind_process(void)
{
    f_setting_remind_t *f_setting_remind = (f_setting_remind_t *)func_cb.f_cb;

    if (tick_check_expire(f_setting_remind->tick, 5000))
    {
        func_backing_to();
    }
    func_process();
}
#endif

//创建界面
static void func_setting_remind_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_setting_remind_t));
    func_cb.frm_main = func_setting_remind_form_create();
    f_setting_remind_t *f_setting_remind = (f_setting_remind_t *)func_cb.f_cb;
    f_setting_remind->click_state = false;
    f_setting_remind->tick = tick_get();
}

//退出任务
static void func_setting_remind_exit(void)
{
    func_cb.last = FUNC_SETTING_REMIND;
    savingState = false;
    //task_stack_remove(FUNC_SETTING_REMIND);
}

//说明功能
void func_setting_remind(void)
{
    printf("%s\n", __func__);
    func_setting_remind_enter();
    while (func_cb.sta == FUNC_SETTING_REMIND)
    {
        func_setting_remind_process();
        func_setting_remind_message(msg_dequeue());
    }
    func_setting_remind_exit();
}

#endif

