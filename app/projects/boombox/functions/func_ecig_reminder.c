#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if ECIG_POWER_CONTROL
//组件ID
//enum {
//按键
//COMPO_ID_BTN_FLASHON = 1,
//COMPO_ID_BTN_FLASHOFF,

//COMPO_ID_PIC_FLASHON,
//COMPO_ID_PIC_FLASHOFF,
//};

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
    char buf[5] = {0};
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 20);
    compo_textbox_set_pos(txt, GUI_SCREEN_WIDTH/2, GUI_SCREEN_HEIGHT/2);
    if(sys_cb.smoke_index == SMOKING)
    {
        compo_textbox_set(txt, "吸烟中...");
    }
    else if(sys_cb.smoke_index == SHORT_CIRCUIT)
    {
        compo_textbox_set_forecolor(txt, COLOR_RED);
        compo_textbox_set(txt, "短路,请检查设备...");
    }
    else if(sys_cb.smoke_index == OPEN_CIRCUIT)
    {
        compo_textbox_set(txt, "拔出...");
    }
    else if(sys_cb.smoke_index == IN_DEVICE)
    {
        compo_textbox_set(txt, "插入...");
        txt = compo_textbox_create(frm, 20);
        compo_textbox_set_pos(txt, GUI_SCREEN_WIDTH/2, GUI_SCREEN_HEIGHT/2 + 40);
        buf[0] = ecig_get_res() / 10 + '0';
        buf[1] = '.';
        buf[2] = ecig_get_res() % 10 + '0';
        buf[3] = 'R';
        compo_textbox_set(txt, buf);
    }
    else if(sys_cb.smoke_index == LOW_POWER)
    {
        compo_textbox_set(txt, "低电量...");
    }
    else if(sys_cb.smoke_index == SMOKE_TIMEOUT)
    {
        compo_textbox_set_forecolor(txt, COLOR_RED);
        compo_textbox_set(txt, "吸烟超时...");
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
            func_cb.sta = FUNC_ACTIVITY;
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
