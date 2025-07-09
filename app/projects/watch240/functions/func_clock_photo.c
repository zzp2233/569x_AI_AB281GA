#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_watchonline.h"
#include "ute_project_config.h"

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_WATCH_PHOTO_SUPPORT

enum
{
    COMPO_ID_BG_IMG = 0x32,
    COMPO_ID_TIME_TEXT,
    COMPO_ID_DATE_TEXT,
};

/*! 时间数字字体,wang.luo 2025-06-24 */
#ifndef UTE_WATCHS_PHOTO_TIME_NUM_FONT
#define UTE_WATCHS_PHOTO_TIME_NUM_FONT UI_BUF_0FONT_FONT_NUM_48_BIN
#endif

const u16 week_char_id[] = {STR_SUNDAY, STR_MONDAY, STR_TUESDAY, STR_WEDNESDAY, STR_THURSDAY, STR_FRIDAY, STR_SATURDAY};

// 相册表盘
compo_form_t *func_clock_photo_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    uint32_t preview = 0;
    uint32_t photo = 0;
    uteModuleWatchOnlineGetCurrPhotoAddress(&preview, &photo);
    photoWatchConfig_t config;
    uteModuleWatchOnlineGetCurrPhotoWatchConfig(&config);
    u16 color = make_color(config.fontColor >> 16 & 0xff, config.fontColor >> 8 & 0xff, config.fontColor & 0xff);

    // 背景图片
    compo_picturebox_t *pic = compo_picturebox_create(frm, photo);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    tm_t rtc_tm;
    rtc_tm = time_to_tm(compo_cb.rtc_cnt);
    u8 hour = rtc_tm.hour;
    if(uteModuleSystemtime12HOn())
    {
        hour%=12;
        if (hour == 0)
        {
            hour = 12;
        }
    }

    u16 timeStr_leng = strlen(i18n[week_char_id[rtc_tm.weekday]])+10;
    u8 *timeStr = (u8 *)ab_zalloc(timeStr_leng);

    // hour
    memset(timeStr, 0, timeStr_leng);
    sprintf(timeStr, "%02d:%02d", hour, rtc_tm.min);
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(timeStr));
    compo_textbox_set_font(textbox, UTE_WATCHS_PHOTO_TIME_NUM_FONT);

    // 获取字体高度
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_textbox_set(textbox, timeStr);
    area_t text_area = widget_text_get_area(textbox->txt);

    // 设置日期文字
    memset(timeStr, 0, timeStr_leng);
    snprintf(timeStr, timeStr_leng, "%s %02d", i18n[week_char_id[rtc_tm.weekday]], rtc_tm.day);
    compo_textbox_t *txt_week = compo_textbox_create(frm, timeStr_leng);
    compo_textbox_set_pos(txt_week, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    compo_textbox_set_forecolor(txt_week, color);
    compo_setid(txt_week, COMPO_ID_DATE_TEXT);
    compo_textbox_set(txt_week, timeStr);
    area_t week_area = widget_text_get_area(txt_week->txt);
    u8 date_str_hei = week_area.hei;

    u16 all_str_hei = text_area.hei + date_str_hei + date_str_hei / 2;
    int16_t y_offset = 0;

    switch (config.timePosition)
    {
        case 0: // 上
            y_offset = all_str_hei;
            break;

        case 1: // 中
            y_offset = GUI_SCREEN_CENTER_Y;
            break;

        case 2: // 下
            y_offset = GUI_SCREEN_HEIGHT - all_str_hei;
            break;

        default:
            y_offset = all_str_hei;
            break;
    }

    // 设置时间数字位置
    memset(timeStr, 0, timeStr_leng);
    sprintf(timeStr, "%02d:%02d", hour, rtc_tm.min);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, y_offset, GUI_SCREEN_WIDTH, text_area.hei);
    compo_textbox_set_align_center(textbox, true);
    compo_textbox_set_forecolor(textbox, color);
    compo_textbox_set(textbox, timeStr);
    compo_setid(textbox, COMPO_ID_TIME_TEXT);

    // 设置日期文字
    memset(timeStr, 0, timeStr_leng);
    snprintf(timeStr, timeStr_leng, "%s %02d", i18n[week_char_id[rtc_tm.weekday]], rtc_tm.day);
    compo_textbox_set_location(txt_week, GUI_SCREEN_CENTER_X, y_offset + date_str_hei * 2, GUI_SCREEN_WIDTH, date_str_hei);
    compo_textbox_set(txt_week, timeStr);

    ab_free(timeStr);

    return frm;
}

// 相册表盘事件处理
void func_clock_photo_process(void)
{
    if (sys_cb.dialplate_id != UTE_MODULE_WATCH_PHOTO_DEFAULT_ID)
    {
        return;
    }

    compo_textbox_t *time_textbox = compo_getobj_byid(COMPO_ID_TIME_TEXT);
    compo_textbox_t *date_textbox = compo_getobj_byid(COMPO_ID_DATE_TEXT);

    tm_t rtc_tm;
    rtc_tm = time_to_tm(compo_cb.rtc_cnt);
    u8 hour = rtc_tm.hour;
    if(uteModuleSystemtime12HOn())
    {
        hour%=12;
        if (hour == 0)
        {
            hour = 12;
        }
    }
    u16 timeStr_leng = strlen(i18n[week_char_id[rtc_tm.weekday]])+10;
    u8 *timeStr = (u8 *)ab_zalloc(timeStr_leng);
    // hour
    memset(timeStr, 0, timeStr_leng);
    sprintf(timeStr, "%02d:%02d", hour, rtc_tm.min);
    compo_textbox_set(time_textbox, timeStr);

    // date
    memset(timeStr, 0, timeStr_leng);
    snprintf(timeStr, timeStr_leng, "%s %02d", i18n[week_char_id[rtc_tm.weekday]], rtc_tm.day);
    compo_textbox_set(date_textbox, timeStr);
    ab_free(timeStr);
}

// 相册表盘消息处理
bool func_clock_photo_message(size_msg_t msg)
{
    if (sys_cb.dialplate_id != UTE_MODULE_WATCH_PHOTO_DEFAULT_ID)
    {
        return false;
    }

    bool intercept_msg = false;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            break;

        case MSG_SYS_500MS: // 秒跳动处理
            break;

    }

    return intercept_msg;
}

#endif
