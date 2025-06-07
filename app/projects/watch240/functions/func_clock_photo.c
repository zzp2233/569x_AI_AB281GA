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

#define UTE_WATCHS_PHOTO_TIME_NUM_FONT UI_BUF_0FONT_FONT_NUM_48_BIN

#define UTE_WATCHS_PHOTO_DATE_NUM_FONT UI_BUF_0FONT_FONT_NUM_32_BIN

// 时间小时数字位置
#define WATCHS_PHOTO_HOUR_X         (GUI_SCREEN_CENTER_X - 40)
#define WATCHS_PHOTO_HOUR_Y         (GUI_SCREEN_CENTER_Y)
#define WATCHS_PHOTO_HOUR_WIDTH     300
#define WATCHS_PHOTO_HOUR_HEIGHT    70

// 时间:位置
#define WATCHS_PHOTO_DOT_X          GUI_SCREEN_CENTER_X
#define WATCHS_PHOTO_DOT_Y          (GUI_SCREEN_CENTER_Y)
#define WATCHS_PHOTO_DOT_WIDTH      260
#define WATCHS_PHOTO_DOT_HEIGHT     70

// 时间分钟数字位置
#define WATCHS_PHOTO_MIN_X          (GUI_SCREEN_CENTER_X + 40)
#define WATCHS_PHOTO_MIN_Y          (GUI_SCREEN_CENTER_Y)
#define WATCHS_PHOTO_MIN_WIDTH      300
#define WATCHS_PHOTO_MIN_HEIGHT     70

// 日期数字位置
#define WATCHS_PHOTO_DATE_X          GUI_SCREEN_CENTER_X
#define WATCHS_PHOTO_DATE_Y          (GUI_SCREEN_CENTER_Y)
#define WATCHS_PHOTO_DATE_WIDTH      290
#define WATCHS_PHOTO_DATE_HEIGHT     70

// 蝴表盘
compo_form_t *func_clock_photo_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    uint32_t preview = 0;
    uint32_t photo = 0;
    uteModuleWatchOnlineGetCurrPhotoAddress(&preview,&photo);
    photoWatchConfig_t config;
    uteModuleWatchOnlineGetCurrPhotoWatchConfig(config);

    compo_picturebox_t *pic = compo_picturebox_create(frm, photo);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    u16 color = make_color(config.fontColor >> 16 & 0xff, config.fontColor >> 8 & 0xff, config.fontColor & 0xff);

    int16_t y_offset = 0;

    switch (config.timePosition)
    {
        case 0:
            y_offset = -(GUI_SCREEN_HEIGHT / 3);
            break;

        case 1:
            y_offset = 0;
            break;

        case 2:
            y_offset = (GUI_SCREEN_HEIGHT / 3);
            break;

        default:
            break;
    }

    // hour
    compo_textbox_t *txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UTE_WATCHS_PHOTO_TIME_NUM_FONT);
    compo_textbox_set_location(txt, WATCHS_PHOTO_HOUR_X, WATCHS_PHOTO_HOUR_Y + y_offset, WATCHS_PHOTO_HOUR_WIDTH, WATCHS_PHOTO_HOUR_HEIGHT);
    compo_textbox_set_forecolor(txt, color);
    compo_bonddata(txt, COMPO_BOND_HOUR);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    // :
    txt = compo_textbox_create(frm, 1);
    compo_textbox_set_font(txt, UTE_WATCHS_PHOTO_TIME_NUM_FONT);
    compo_textbox_set_location(txt, WATCHS_PHOTO_DOT_X, WATCHS_PHOTO_DOT_Y + y_offset, WATCHS_PHOTO_DOT_WIDTH, WATCHS_PHOTO_DOT_HEIGHT);
    compo_textbox_set_forecolor(txt, color);
    compo_textbox_set(txt, ":");

    // minute
    txt = compo_textbox_create(frm, 2);
    compo_textbox_set_font(txt, UTE_WATCHS_PHOTO_TIME_NUM_FONT);
    compo_textbox_set_location(txt, WATCHS_PHOTO_MIN_X, WATCHS_PHOTO_MIN_Y + y_offset, WATCHS_PHOTO_MIN_WIDTH, WATCHS_PHOTO_MIN_HEIGHT);
    compo_textbox_set_forecolor(txt, color);
    compo_bonddata(txt, COMPO_BOND_MINUTE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    // date
    txt = compo_textbox_create(frm, 10);
    compo_textbox_set_font(txt, UTE_WATCHS_PHOTO_DATE_NUM_FONT);
    compo_textbox_set_location(txt, WATCHS_PHOTO_DATE_X,  WATCHS_PHOTO_MIN_Y + y_offset + 60, WATCHS_PHOTO_DATE_WIDTH, WATCHS_PHOTO_DATE_HEIGHT);
    compo_textbox_set_forecolor(txt, color);
    compo_bonddata(txt, COMPO_BOND_DATE);
    compo_set_bonddata((component_t *)txt, time_to_tm(compo_cb.rtc_cnt));

    return frm;
}

// 地图功能事件处理
void func_clock_photo_process(void)
{
    if (sys_cb.dialplate_index != UTE_WATCHS_DIALPLATE_BTF_INDEX || sys_cb.dialplate_btf_ready == false)
    {
        return;
    }

}

// 地图功能消息处理
bool func_clock_photo_message(size_msg_t msg)
{
    if (sys_cb.dialplate_index != UTE_WATCHS_DIALPLATE_BTF_INDEX)
    {
        return false;
    }

    bool intercept_msg = false;
    static bool time_visible = 0;

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
