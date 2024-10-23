#include "include.h"
#include "func.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define COVER_ITEM_CNT            ((int)(sizeof(tbl_cover_item) / sizeof(tbl_cover_item[0])))


typedef struct f_cover_item_t_ {
    u32 res_addr;
    u8 str_idx;
    s16 pic_y;
    s16 txt_y;
    s16 title_y
} f_cover_item_t;

//和func_cover.h里的宏定义顺序一致
static const f_cover_item_t tbl_cover_item[] = {
    /*   res_addr,                              str_idx,                pic_y,                txt_y                  title_y   */
    {UI_BUF_ICON_ALARM_CLOCK_BIN,             STR_ALARM_CLOCK,        GUI_SCREEN_HEIGHT/5+30, GUI_SCREEN_HEIGHT/5,  GUI_SCREEN_CENTER_Y},
//    {UI_BUF_POP_UP_SEDENTARY_CLOCK_BIN,         STR_SEDENTARY_REMIND,   102,            190},
//    {UI_BUF_POP_UP_WATER_CLOCK_BIN,             STR_DRINK_REMIND,       102,            190},
//    {UI_BUF_POP_UP_FIND_WATCH_BIN,              STR_FIND_WATCH,         175,            290},
//    {UI_BUF_POP_UP_GOAL_ACHIEVEMENT_BIN,        STR_GOAL_ACHIEVE,       175,            290},
//    {UI_BUF_POP_UP_BLUETOOTH_CONNECTION_BIN,    STR_BT_CONNECT,         175,            290},
//    {UI_BUF_POP_UP_APP_CONNECTION_BIN,          STR_APP_CONNECT,        175,            290},
};

typedef struct f_cover_t_ {

} f_cover_t;


u32 func_cover_get_pic_res_addr()
{
    return tbl_cover_item[sys_cb.cover_index - 1].res_addr;
}

u8 func_cover_get_str_idx()
{
    return tbl_cover_item[sys_cb.cover_index - 1].str_idx;
}

s16 func_cover_get_pic_y()
{
    return tbl_cover_item[sys_cb.cover_index - 1].pic_y;
}

s16 func_cover_get_txt_y()
{
    return tbl_cover_item[sys_cb.cover_index - 1].txt_y;
}

s16 func_cover_get_title_txt_y()
{
    return tbl_cover_item[sys_cb.cover_index - 1].title_y;
}

void gui_cover_msg_enqueue(uint8_t index)
{
    if (bt_cb.disp_status >= BT_STA_INCOMING && bt_cb.disp_status <= BT_STA_INCALL) {
        return;
    }
    sys_cb.cover_index = index;
    msg_enqueue(EVT_MSGBOX_EXIT);
    msg_enqueue(EVT_CLOCK_DROPDOWN_EXIT);
    msg_enqueue(EVT_WATCH_SET_COVER);
}

void gui_set_cover_index(uint8_t index)
{
    printf("%s:%d\n", __func__, index);

    ute_module_systemtime_one_alarm_t alarm = {0};
    ute_module_systemtime_one_alarm_t* alarm_p = NULL;

    if (index > COVER_ITEM_CNT) {
        printf("cover index exceed!\n");
        return;
    }
    if (!COVER_ITEM_CNT) {
        return;
    }
    sys_cb.cover_index = index;
    u8 mode = MSGBOX_MODE_BTN_NONE;
    if (sys_cb.cover_index) {
        char *msg = (char *)i18n[tbl_cover_item[sys_cb.cover_index - 1].str_idx];
        char title[8] = {'\0'};
        if ((sys_cb.cover_index >= COVER_ALARM) && (sys_cb.cover_index <= COVER_HEALTH_DRINK)) {
            if (sys_cb.cover_index == COVER_ALARM) {

                //获取用户闹钟
                if (uteModuleSystemtimeGetAlarmRingIndex() != 0xff) {
                    uteModuleSystemtimeGetAlarm(&alarm, uteModuleSystemtimeGetAlarmRingIndex());
                }
                alarm_p = &alarm;
                if (alarm_p->isRepeatRemindOpen) {
                    snprintf(title, sizeof(title), "%02d:%02d", alarm_p->repeatRemindHour, alarm_p->repeatRemindMin);
                } else {
                    snprintf(title, sizeof(title), "%02d:%02d", alarm_p->hour, alarm_p->min);
                }
            }
//            else if (sys_cb.cover_index == COVER_HEALTH_DRINK || sys_cb.cover_index == COVER_HEALTH_SEDENTARY) {
//                tm_t tm = time_to_tm(RTCCNT);
//                snprintf(title, sizeof(title), "%02d:%02d", tm.hour, tm.min);
//            }
            mode = MSGBOX_MODE_BTN_REMIND_LATER_CLOSE;
        }

        int res = msgbox(msg, title, mode, MSGBOX_MSG_TYPE_COVER);

        //稍后提醒
        if (res == MSGBOX_RES_REMIND_LATER) {
            if (sys_cb.cover_index == COVER_ALARM) {
                printf("COVER_ALARM MSGBOX_RES_REMIND_LATER\n");
                //开启配置贪睡时钟 参数
                if (alarm_p != NULL) {
                    alarm_p->isRepeatRemindOpen = true;

                    uint8_t hour_later = alarm_p->repeatRemindHour;
                    uint8_t min_later = alarm_p->repeatRemindMin + ALARM_REPEAT_REMIND_DEFAULT_TIME_MIN;
                    if (min_later > 59) {
                        hour_later += 1;
                        min_later -= 60;
                    }

                    if (hour_later > 24) {
                        hour_later -= 24;
                    }

                    alarm_p->repeatRemindHour = hour_later;
                    alarm_p->repeatRemindMin = min_later;
                    uteModuleSystemtimeSetAlarm(*alarm_p, uteModuleSystemtimeGetAlarmRingIndex());
                    printf("repeat alarm[%d] ring, [%02d:%02d]\n", uteModuleSystemtimeGetAlarmRingIndex(), alarm_p->repeatRemindHour, alarm_p->repeatRemindMin);
                }

                //关闭 喇叭 马达

           }
        } else {
            if (sys_cb.cover_index == COVER_ALARM) {
                alarm_p->isRepeatRemindOpen = false;
                uteModuleSystemtimeSetAlarm(*alarm_p, uteModuleSystemtimeGetAlarmRingIndex());
            }
        }

        //强制退出弹窗
        if (res == MSGBOX_RES_EXIT) {
            if (sys_cb.cover_index == COVER_ALARM) {
                printf("COVER_ALARM MSGBOX_RES_EXIT\n");
                //关闭 喇叭 马达

            }
        }

        //提醒界面超时退出
        if (res == MSGBOX_RES_TIMEOUT_EXIT) {
            if (sys_cb.cover_index == COVER_ALARM) {
                printf("COVER_ALARM MSGBOX_RES_TIMEOUT_EXIT\n");
                //关闭 喇叭 马达
            }
        }


    }
}
