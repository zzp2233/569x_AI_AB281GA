#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "ute_application_common.h"
#include "ute_module_gui_common.h"
#include "ute_module_sport.h"
#include "ute_drv_battery_common.h"

#if UTE_MODULE_POWER_SAVING_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_saving_sub_show_t_
{
    u32 tick;
    u8 count;
} f_saving_sub_show_t;

enum
{
    COMPO_ID_DATE = 1,
    COMPO_ID_BATT,
    COMPO_ID_BATT_PIC,
    COMPO_ID_TIME_HOUR,
    COMPO_ID_TIME_MIN,
    COMPO_ID_STEP,
    COMPO_ID_RECT,
    COMPO_ID_PIC_COUNTDOWN,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

static bool key_press;

//省电模式页面
compo_form_t *func_saving_sub_show_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    char txt_buf[20] = {0};
    uint8_t hour = 0;
    ute_module_systemtime_time_t time;
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt, NULL, NULL);
    uteModuleSystemtimeGetTime(&time);  //获取系统时间

    snprintf(txt_buf, sizeof(txt_buf), "%02d/%02d %s", time.month, time.day, i18n[STR_SUNDAY+time.week]);
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4 + 40, GUI_SCREEN_CENTER_X, 50);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox, COMPO_ID_DATE);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I341001_28_SET_BATT_52X29_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X - 27, GUI_SCREEN_HEIGHT/5 + 12);
    compo_picturebox_cut(pic, uteDrvBatteryCommonGetBatteryIndex(11), 11);
    compo_setid(pic, COMPO_ID_BATT_PIC);
    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d%%", uteDrvBatteryCommonGetLvl());
    textbox = compo_textbox_create(frm, 4);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X + 2, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox, COMPO_ID_BATT);

    if(uteModuleSystemtime12HOn())
    {
        if (time.hour == 0)
            hour = 12;
        else if (time.hour > 12)
            hour = time.hour - 12;
        else
            hour = time.hour;
    }
    else
    {
        hour = time.hour;
    }
    compo_number_t *num = compo_number_create(frm, UI_BUF_I341001_12_WEATHER_102_56X76_BIN, 2);
    compo_number_set_margin(num, 5);
    compo_number_set_zfill(num, true);
    compo_number_set_radix(num, 11, true);
    compo_number_set(num, hour);
    compo_number_set_align(num, 1);
    compo_number_set_pos(num, GUI_SCREEN_CENTER_X/2 + 10, GUI_SCREEN_CENTER_Y + 20);
    compo_setid(num, COMPO_ID_TIME_HOUR);
    textbox = compo_textbox_create(frm, 2);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 20);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set(textbox, (char *)":");
    num = compo_number_create(frm, UI_BUF_I341001_12_WEATHER_102_56X76_BIN, 2);
    compo_number_set_margin(num, 5);
    compo_number_set_zfill(num, true);
    compo_number_set_radix(num, 11, true);
    compo_number_set(num, time.min);
    compo_number_set_align(num, 1);
    compo_number_set_pos(num, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X/2 - 10, GUI_SCREEN_CENTER_Y + 20);
    compo_setid(num, COMPO_ID_TIME_MIN);
    pic = compo_picturebox_create(frm, UI_BUF_I341001_28_SET_STEP_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X - 30, GUI_SCREEN_HEIGHT - GUI_SCREEN_HEIGHT/6 + 17);
    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", totalStepCnt);
    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X + 5, GUI_SCREEN_HEIGHT - GUI_SCREEN_HEIGHT/6);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox, COMPO_ID_STEP);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_BLACK);
    compo_shape_set_visible(shape, false);
    compo_setid(shape, COMPO_ID_RECT);

    pic = compo_picturebox_create(frm, UI_BUF_I341001_28_SET_POWER_SAVING_MODE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    widget_set_visible(pic->img, false);
    compo_setid(pic, COMPO_ID_PIC_COUNTDOWN);

    return frm;
}

#endif
//省电模式事件处理
static void func_saving_sub_show_process(void)
{
    f_saving_sub_show_t *f_saving_sub_show = (f_saving_sub_show_t *)func_cb.f_cb;
    compo_picturebox_t *picount = compo_getobj_byid(COMPO_ID_PIC_COUNTDOWN);
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_RECT);
    if (key_press)
    {
        compo_shape_set_visible(shape, true);
        widget_set_visible(picount->img, true);
        tft_bglight_set_level(80, true);
        if (tick_check_expire(f_saving_sub_show->tick, 1000))
        {
            f_saving_sub_show->tick = tick_get();
            f_saving_sub_show->count++;
        }

        if (f_saving_sub_show->count < 5)
        {
            compo_picturebox_cut(picount, f_saving_sub_show->count, 5);
        }
        else
        {
            uteModuleGuiCommonSetPowerSavingModeOpen(false);
            tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(), true);
            task_stack_init();  // 任务堆栈
            latest_task_init(); // 最近任务
            uteTaskGuiStartScreen(FUNC_CLOCK, 0, __func__);
        }

        if (!IS_PWRKEY_PRESS())
        {
            compo_shape_set_visible(shape, false);
            widget_set_visible(picount->img, false);
            key_press = false;
            f_saving_sub_show->count = 0;
            tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(), true);
        }
    }

    compo_textbox_t *textdate = compo_getobj_byid(COMPO_ID_DATE);
    compo_textbox_t *textbatt = compo_getobj_byid(COMPO_ID_BATT);
    compo_picturebox_t *battpic = compo_getobj_byid(COMPO_ID_BATT_PIC);
    compo_number_t  *numHour  = compo_getobj_byid(COMPO_ID_TIME_HOUR);
    compo_number_t  *numMin   = compo_getobj_byid(COMPO_ID_TIME_MIN);
    compo_textbox_t *textstep = compo_getobj_byid(COMPO_ID_STEP);

    char txt_buf[20] = {0};
    uint8_t hour = 0;
    ute_module_systemtime_time_t time;
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt, NULL, NULL);
    uteModuleSystemtimeGetTime(&time);  //获取系统时间

    snprintf(txt_buf, sizeof(txt_buf), "%02d/%02d %s", time.month, time.day, i18n[STR_SUNDAY+time.week]);
    compo_textbox_set(textdate, txt_buf);

    compo_picturebox_cut(battpic, uteDrvBatteryCommonGetBatteryIndex(11), 11);
    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d%%", uteDrvBatteryCommonGetLvl());
    compo_textbox_set(textbatt, txt_buf);

    if(uteModuleSystemtime12HOn())
    {
        if (time.hour == 0)
            hour = 12;
        else if (time.hour > 12)
            hour = time.hour - 12;
        else
            hour = time.hour;
    }
    else
    {
        hour = time.hour;
    }
    compo_number_set(numHour, hour);
    compo_number_set(numMin, time.min);

    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", totalStepCnt);
    compo_textbox_set(textstep, txt_buf);

    func_process();
}

//单击按钮

//省电模式功能消息处理
static void func_saving_sub_show_message(size_msg_t msg)
{
    f_saving_sub_show_t *f_saving_sub_show = (f_saving_sub_show_t *)func_cb.f_cb;
    switch (msg)
    {
        case KL_BACK:
            key_press = true;
            f_saving_sub_show->tick = tick_get();
            break;
        default:
            //func_message(msg);
            break;
    }
}

//进入省电模式功能
static void func_saving_sub_show_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_saving_sub_show_t));
    func_cb.frm_main = func_saving_sub_show_form_create();
    f_saving_sub_show_t *f_saving_sub_show = (f_saving_sub_show_t *)func_cb.f_cb;
    key_press = false;
    f_saving_sub_show->count = 0;
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
}

//退出省电模式功能
static void func_saving_sub_show_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    //func_cb.last = FUNC_SAVING_SUB_SHOW;
}

//省电模式功能
void func_saving_sub_show(void)
{
    printf("%s\n", __func__);
    func_saving_sub_show_enter();
    while (func_cb.sta == FUNC_SAVING_SUB_SHOW)
    {
        func_saving_sub_show_process();
        func_saving_sub_show_message(msg_dequeue());
    }
    func_saving_sub_show_exit();
}

#endif

