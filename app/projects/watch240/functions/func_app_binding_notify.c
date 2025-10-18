#include "include.h"
#include "func.h"
#include "ute_module_appbinding.h"

#if UTE_MODULE_SCREENS_APP_BINDING_NOTIFY_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
typedef struct f_binding_notify_t_
{
    u32 tick;
} f_binding_notify_t;

enum
{
    COMPO_ID_RETURN = 1,
};

compo_form_t *func_binding_notify_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox;
    compo_picturebox_t *picbox;
    if (uteModuleAppBindingGetOurAppConnection())
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I341001_1_START_SUCCEED_BIN);
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 104+72);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_BINDING_SUCC]));
        compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+95,320,50);
        compo_textbox_set(textbox,i18n[STR_BINDING_SUCC]);
    }
    else
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I341001_1_START_RETURN_BIN);
        compo_picturebox_set_pos(picbox, 50, 36);
        compo_button_t *btn = compo_button_create(frm);
        compo_button_set_location(btn, 50, 36, 70, 60);
        compo_setid(btn, COMPO_ID_RETURN);

        picbox = compo_picturebox_create(frm, UI_BUF_I341001_1_START_FAIL_BIN);
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 104+72);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_BINDING_FAIL]));
        compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+95,320,50);
        compo_textbox_set(textbox,i18n[STR_BINDING_FAIL]);
    }

    return frm;
}

void func_binding_notify_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_RETURN:
            func_switch_to(FUNC_POWER_ON_SCAN, FUNC_SWITCH_LR_RIGHT | FUNC_SWITCH_AUTO);
            break;

        default:
            break;
    }
}

static void func_binding_notify_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_binding_notify_click_handler();
            break;
        default:
            break;
    }
}
//下滑事件流程处理
static void func_binding_notify_process(void)
{
    f_binding_notify_t *f_binding = (f_binding_notify_t *)func_cb.f_cb;
    func_process();
    reset_sleep_delay_all();
    if (tick_check_expire(f_binding->tick, 5 * 1000))
    {
        if (uteModuleAppBindingGetOurAppConnection())
        {
            if (uteModuleGuiCommonGetLastScreenId() == FUNC_POWER_ON_SCAN || task_stack_contains(FUNC_POWER_ON_SCAN)\
                || uteModuleGuiCommonGetLastScreenId() == FUNC_POWER_ON_LANGUAGE || task_stack_contains(FUNC_POWER_ON_LANGUAGE))
            {
                task_stack_init();  // 任务堆栈
                latest_task_init(); // 最近任务
                // func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_LEFT | FUNC_SWITCH_AUTO);
                uteTaskGuiStartScreen(FUNC_CLOCK, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, __func__);
            }
            else
            {
                uteModuleGuiCommonGoBackLastScreen();
            }
        }
        else
        {
            uteTaskGuiStartScreen(FUNC_POWER_ON_SCAN, FUNC_SWITCH_LR_RIGHT | FUNC_SWITCH_AUTO, __func__);
        }
    }
}

//创建界面
static void func_binding_notify_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_binding_notify_t));
    func_cb.frm_main = func_binding_notify_form_create();
    f_binding_notify_t *f_binding = (f_binding_notify_t *)func_cb.f_cb;
    f_binding->tick = tick_get();
}
//退出任务
static void func_binding_notify_exit(void)
{
    func_cb.last = FUNC_BINDING_NOTIFY;
    uteTaskGuiStackRemoveScreenId(FUNC_BINDING_NOTIFY);
}

//说明功能
void func_binding_notify(void)
{
    printf("%s\n", __func__);
    func_binding_notify_enter();
    while (func_cb.sta == FUNC_BINDING_NOTIFY)
    {
        func_binding_notify_process();
        func_binding_notify_message(msg_dequeue());
    }
    func_binding_notify_exit();
}

#endif

