#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "../../../../ute/module/smoke/ute_module_smoke.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if ECIG_POWER_CONTROL

enum
{
    COMPO_ID_PIC_SMOCKING = 1,
};
typedef struct f_ecig_reminder_t_
{
    u8 ecig_reminder_flag;
    u32 tick;
} f_ecig_reminder_t;

//创建窗体
compo_form_t *func_ecig_reminder_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    char buf[20] = {0};
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 20);
    compo_picturebox_t *picbox ;
    compo_animation_t *animation;
    compo_textbox_set_pos(txt, GUI_SCREEN_WIDTH/2, GUI_SCREEN_HEIGHT/2+20);
    if(sys_cb.smoke_index == SMOKING)
    {
        animation = compo_animation_create(frm, UI_BUF_I330001_SMOKING_00_BIN);
        compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
        compo_animation_set_radix(animation, 3);
        compo_animation_set_interval(animation, 30);
        compo_setid(animation,COMPO_ID_PIC_SMOCKING);
        compo_textbox_set(txt, "吸烟中...");
        uint32_t smokeing_count = uteModuleGetSomkeSomkeCount();
        smokeing_count++;
        uteModuleSetSomkeCount(smokeing_count);

        // 获取当前时间
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        int current_hour = time.hour;

        // 更新对应小时的口数
        uteModuleSmokeData.smoking_count_per_hour[current_hour]++;
        // 更新本周对应日期的口数
        int current_weekday = time.week;
        uteModuleSmokeData.smoking_count_per_day[current_weekday]++;
        uteModuleSmokeDataSaveConfig();
    }
    else if(sys_cb.smoke_index == SHORT_CIRCUIT)
    {
        compo_textbox_set_forecolor(txt, COLOR_RED);
        compo_textbox_set(txt, i18n[STR_SHORT_CIR]);
    }
    else if(sys_cb.smoke_index == OPEN_CIRCUIT)
    {
        picbox= compo_picturebox_create(frm, UI_BUF_I330001_YD_YANDAN_OUT_BIN);
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-40);
        compo_textbox_set(txt, i18n[STR_PULL_OUT]);
    }
    else if(sys_cb.smoke_index == IN_DEVICE)
    {
        if(get_gear_func()==0)
        {
            picbox= compo_picturebox_create(frm, UI_BUF_I330001_YD_YANDAN_IN_DANG_BIN);
            compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-40);
        }
        else
        {
            picbox= compo_picturebox_create(frm, UI_BUF_I330001_YD_YANDAN_IN_SHUANG_BIN);
            compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-40);
        }
        compo_textbox_set(txt, i18n[STR_INSERT]);
        txt = compo_textbox_create(frm, 20);
        compo_textbox_set_pos(txt, GUI_SCREEN_WIDTH/2, GUI_SCREEN_HEIGHT/2 + 50);


        buf[0] = ecig_get_res() / 10 + '0';
        buf[1] = '.';
        buf[2] = ecig_get_res() % 10 + '0';
        buf[3] = 'R';
        // buf[4] = ' ';
        // buf[5] = ' ';
        // buf[6] = ' ';
        // buf[7] = ecig_get_res2() / 10 + '0';
        // buf[8] = '.';
        // buf[9] = ecig_get_res2() % 10 + '0';
        // buf[10] = 'R';
        compo_textbox_set(txt, buf);

    }
    else if(sys_cb.smoke_index == LOW_POWER)
    {
        compo_textbox_set(txt, i18n[STR_LOW_BATTERY]);
    }
    else if(sys_cb.smoke_index == SMOKE_TIMEOUT)
    {
        compo_textbox_set_forecolor(txt, COLOR_RED);
        compo_textbox_set(txt, i18n[STR_SMOKING_OUT]);
    }
    return frm;
}

//事件处理
static void func_ecig_reminder_process(void)
{
    f_ecig_reminder_t *f_ecig_reminder = (f_ecig_reminder_t*)func_cb.f_cb;
    func_process();
    if(!bsp_ecig_is_working_sta())
    {
        if(tick_check_expire(f_ecig_reminder->tick, 1500))
        {
            func_directly_back_to();
        }
    }
    else
    {
        f_ecig_reminder->tick = tick_get();
    }
}

//单击按钮
//static void func_ecig_reminder_button_click(void)
//{
//    int id = compo_get_button_id();

//    switch(id) {
//    case COMPO_ID_BTN_FLASHON:

//    break;

//    default:
//    break;

//}

//消息处理
static void func_ecig_reminder_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            //func_ecig_reminder_button_click();
            // func_cb.sta = FUNC_ACTIVITY;
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入控制功能
static void func_ecig_reminder_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_ecig_reminder_t));
    func_cb.frm_main = func_ecig_reminder_form_create();
    f_ecig_reminder_t *f_ecig_reminder = (f_ecig_reminder_t*)func_cb.f_cb;
    f_ecig_reminder->tick = tick_get();
}

//退出
static void func_ecig_reminder_exit(void)
{
    sys_cb.smoke_index = 0;
    func_cb.last = FUNC_ECIG_REMINDER;
    ecig_clear_short_flag(0);
}

//功能
void func_ecig_reminder(void)
{
    printf("%s\n", __func__);
    func_ecig_reminder_enter();
    while (func_cb.sta == FUNC_ECIG_REMINDER)
    {
        func_ecig_reminder_process();
        func_ecig_reminder_message(msg_dequeue());
    }
    func_ecig_reminder_exit();
}
#endif
