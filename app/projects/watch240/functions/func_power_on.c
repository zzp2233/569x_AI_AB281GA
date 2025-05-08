#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "ute_application_common.h"
#include "ute_module_appbinding.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_POWERON_SUPPORT

/*! 默认开机动画时长,wang.luo 2025-04-12 */
#ifndef DEFAULT_POWERON_ANIMATION_SECOND
#define DEFAULT_POWERON_ANIMATION_SECOND  2
#endif

typedef struct f_power_on_t_
{
    u32 tick;
    u8 pic_num_disp;
    u16 animation_second;
} f_power_on_t;

enum
{
    COMPO_PIC_ID = 1,
};

static void func_power_on_switch_screen_handle(void)
{
    if (0)
    {
        // pass
    }
#if UTE_MODULE_POWERON_LANGUAGE_SELECT_SUPPORT
    else if ((uteModuleAppBindingGetHasBindingBefore() != HAS_BEEN_CONNECTED) && (!uteApplicationCommonIsHasConnectOurApp()))
    {
#if UTE_MODULE_SCREENS_POWER_ON_LANGUAGE_SELECT_SUPPORT
        uteTaskGuiStartScreen(FUNC_POWER_ON_LANGUAGE, 0, __func__);
#else
        uteTaskGuiStartScreen(FUNC_LANGUAGE, 0, __func__);
#endif
    }
#endif
#if (UTE_MODULE_SCREENS_APP_BINDING_SUPPORT) // 如果没有开机界面，在这里跳转至绑定界面 dengli.lu 2022-02-28
    else if (uteModuleAppBindingGetHasBindingBefore() != HAS_BEEN_CONNECTED)
    {
        uteTaskGuiStartScreen(FUNC_POWER_ON_SCAN, 0, __func__);
    }
#endif
    else
    {
        uteTaskGuiStartScreen(FUNC_CLOCK, 0, __func__);
    }
}

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建开机窗体
compo_form_t *func_power_on_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I330001_KAIJI_ICON_LOGO_FRAME_1542_BIN);///背景图片
    compo_picturebox_cut(pic, 0, 24 );
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(pic, COMPO_PIC_ID);

    return frm;
}
static void func_power_on_disp_process(void)
{
    f_power_on_t* f_power_on = (f_power_on_t*)func_cb.f_cb;
    if(tick_check_expire(f_power_on->tick,200))
    {
        compo_picturebox_t *pic  = compo_getobj_byid(COMPO_PIC_ID);

        f_power_on->tick = tick_get();
        f_power_on->pic_num_disp ++;

        if(f_power_on->pic_num_disp<24)
        {
            compo_picturebox_cut(pic, f_power_on->pic_num_disp, 24);
        }
        else
        {
            func_cb.sta = FUNC_CLOCK;
            sys_cb.power_on_state=true;
            if(!uteApplicationCommonIsHasConnectOurApp())
            {
                sys_cb.power_on_state=false;
                ute_module_systemtime_time_t time;
                uteModuleSystemtimeGetTime(&time);

                if(time.isWatchSetLangage == false)
                {
                    func_cb.sta = FUNC_POWER_ON_LANGUAGE;
                }
                else
                {
                    func_cb.sta = FUNC_POWER_ON_SCAN;
                }
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

//创建开机窗体
compo_form_t *func_power_on_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I332001_KAIJI_ICON_LOGO_BIN);///背景图片
    compo_picturebox_cut(pic, 0, 24 );
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(pic, COMPO_PIC_ID);

    return frm;
}
static void func_power_on_disp_process(void)
{
    f_power_on_t* f_power_on = (f_power_on_t*)func_cb.f_cb;
    if(tick_check_expire(f_power_on->tick,200))
    {
        compo_picturebox_t *pic  = compo_getobj_byid(COMPO_PIC_ID);

        f_power_on->tick = tick_get();
        f_power_on->pic_num_disp ++;

        if(f_power_on->pic_num_disp<24)
        {
            compo_picturebox_cut(pic, f_power_on->pic_num_disp, 24);
        }
        else
        {
            func_cb.sta = FUNC_CLOCK;
            sys_cb.power_on_state=true;
            if(!uteApplicationCommonIsHasConnectOurApp())
            {
                sys_cb.power_on_state=false;
                ute_module_systemtime_time_t time;
                uteModuleSystemtimeGetTime(&time);

                if(time.isWatchSetLangage == false)
                {
                    func_cb.sta = FUNC_POWER_ON_LANGUAGE;
                }
                else
                {
                    func_cb.sta = FUNC_POWER_ON_SCAN;
                }
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//创建开机窗体
compo_form_t *func_power_on_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I335001_LOGO_ICON_LOGO_156X158_X40_Y75_BIN);///背景图片
    // compo_picturebox_cut(pic, 0, 24 );
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X, 75+158/2);
    compo_setid(pic, COMPO_PIC_ID);

    return frm;
}
static void func_power_on_disp_process(void)
{
    f_power_on_t* f_power_on = (f_power_on_t*)func_cb.f_cb;
    if(tick_check_expire(f_power_on->tick,200))
    {
        compo_picturebox_t *pic  = compo_getobj_byid(COMPO_PIC_ID);

        f_power_on->tick = tick_get();
        f_power_on->pic_num_disp ++;

        if(f_power_on->pic_num_disp<24)
        {
            // compo_picturebox_cut(pic, f_power_on->pic_num_disp, 24);
        }
        else
        {
            func_cb.sta = FUNC_CLOCK;
            sys_cb.power_on_state=true;
            if(!uteApplicationCommonIsHasConnectOurApp())
            {
                sys_cb.power_on_state=false;
                ute_module_systemtime_time_t time;
                uteModuleSystemtimeGetTime(&time);

                if(time.isWatchSetLangage == false)
                {
                    func_cb.sta = FUNC_POWER_ON_LANGUAGE;
                }
                else
                {
                    func_cb.sta = FUNC_POWER_ON_SCAN;
                }
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

//创建开机窗体
compo_form_t *func_power_on_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I338001_1_START_LOGO_BIN);///背景图片
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(pic, COMPO_PIC_ID);

    return frm;
}
static void func_power_on_disp_process(void)
{
    f_power_on_t* f_power_on = (f_power_on_t*)func_cb.f_cb;
    if(tick_check_expire(f_power_on->tick,200))
    {
        f_power_on->tick = tick_get();
        f_power_on->pic_num_disp ++;

        if(f_power_on->pic_num_disp<24)
        {
        }
        else
        {
            func_cb.sta = FUNC_CLOCK;
            sys_cb.power_on_state=true;
            if(!uteApplicationCommonIsHasConnectOurApp())
            {
                sys_cb.power_on_state=false;
                ute_module_systemtime_time_t time;
                uteModuleSystemtimeGetTime(&time);

                if(time.isWatchSetLangage == false)
                {
                    func_cb.sta = FUNC_POWER_ON_LANGUAGE;
                }
                else
                {
                    func_cb.sta = FUNC_POWER_ON_SCAN;
                }
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT

//创建开机窗体
compo_form_t *func_power_on_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I340001_KAIJI_ICON_LOGO_BIN);///背景图片
    compo_picturebox_cut(pic, 0, 24 );
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(pic, COMPO_PIC_ID);

    return frm;
}
static void func_power_on_disp_process(void)
{
    f_power_on_t* f_power_on = (f_power_on_t*)func_cb.f_cb;
    if(tick_check_expire(f_power_on->tick,200))
    {
        compo_picturebox_t *pic  = compo_getobj_byid(COMPO_PIC_ID);

        f_power_on->tick = tick_get();
        f_power_on->pic_num_disp ++;

        if(f_power_on->pic_num_disp<24)
        {
            compo_picturebox_cut(pic, f_power_on->pic_num_disp, 24);
        }
        else
        {
            func_cb.sta = FUNC_CLOCK;
            sys_cb.power_on_state=true;
            if(!uteApplicationCommonIsHasConnectOurApp())
            {
                sys_cb.power_on_state=false;
                ute_module_systemtime_time_t time;
                uteModuleSystemtimeGetTime(&time);

                if(time.isWatchSetLangage == false)
                {
                    func_cb.sta = FUNC_POWER_ON_LANGUAGE;
                }
                else
                {
                    func_cb.sta = FUNC_POWER_ON_SCAN;
                }
            }
        }
    }
}

#else
//创建开机窗体
compo_form_t *func_power_on_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
static void func_power_on_disp_process(void)
{
    f_power_on_t* f_power_on = (f_power_on_t*)func_cb.f_cb;

    if (tick_check_expire(f_power_on->tick, 1000))
    {
        f_power_on->tick = tick_get();
        f_power_on->animation_second ++;
    }

    if (f_power_on->animation_second > DEFAULT_POWERON_ANIMATION_SECOND)
    {
        func_power_on_switch_screen_handle();
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//开机界面处理
static void func_power_on_process(void)
{
    func_power_on_disp_process();
    func_process();
}

//开机界面消息处理
static void func_power_on_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_SYS_1S:
            break;
    }
}

//进入开机界面
static void func_power_on_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_power_on_t));
    func_cb.frm_main = func_power_on_form_create();
    f_power_on_t* f_power_on = (f_power_on_t*)func_cb.f_cb;

    f_power_on->tick = 0;
    f_power_on->pic_num_disp = 0;
}

//退出开机界面
static void func_power_on_exit(void)
{
    task_stack_init();  //任务堆栈
    latest_task_init(); //最近任务
}

//开机界面
void func_power_on(void)
{
    printf("%s\n", __func__);
    func_power_on_enter();
    while (func_cb.sta == FUNC_POWER_ON)
    {
        func_power_on_process();
        func_power_on_message(msg_dequeue());
    }
    func_power_on_exit();
}

#endif
