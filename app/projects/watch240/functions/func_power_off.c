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

#if UTE_MODULE_SCREENS_POWEROFF_SUPPORT

/*! 默认关机机动画时长,wang.luo 2025-04-12 */
#ifndef DEFAULT_POWEROFF_ANIMATION_SECOND
#define DEFAULT_POWEROFF_ANIMATION_SECOND  2
#endif

typedef struct f_power_off_t_
{
    u32 tick;
    u8 pic_num_disp;
    u16 animation_second;
} f_power_off_t;

enum
{
    COMPO_PIC_ID = 1,
};

#if GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//创建关机窗体
compo_form_t *func_power_off_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I340002_LOGO_01_BIN);///背景图片
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(pic, COMPO_PIC_ID);

    return frm;
}
static void func_power_off_disp_process(void)
{
    f_power_off_t* f_power_off = (f_power_off_t*)func_cb.f_cb;
    if(tick_check_expire(f_power_off->tick,200))
    {
        compo_picturebox_t *pic  = compo_getobj_byid(COMPO_PIC_ID);

        f_power_off->tick = tick_get();
        f_power_off->pic_num_disp ++;

        if(f_power_off->pic_num_disp == 24)
        {
            uteApplicationCommonPoweroff();
        }
    }
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
#if GUI_SCREEN_SIZE_240X284RGB_I335005_SUPPORT
/*! 默认开机动画logo图片数量 */
#ifndef DEFAULT_POWEROFF_ANIMATION_LOGO_IMAGE_NUMBER
#define DEFAULT_POWEROFF_ANIMATION_LOGO_IMAGE_NUMBER 5
#endif

#define DEFAULT_POWEROFF_ANIMATION_LOGO_IMAGE_NUMBER_TIME 500

//创建关机窗体
compo_form_t *func_power_off_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I335005_LOGO_00_BIN);///背景图片
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(pic, COMPO_PIC_ID);

    return frm;
}
static void func_power_off_disp_process(void)
{
    f_power_off_t* f_power_off = (f_power_off_t*)func_cb.f_cb;
    if(tick_check_expire(f_power_off->tick,DEFAULT_POWEROFF_ANIMATION_LOGO_IMAGE_NUMBER_TIME))
    {
        compo_picturebox_t *pic  = compo_getobj_byid(COMPO_PIC_ID);

        f_power_off->tick = tick_get();
        f_power_off->pic_num_disp ++;
        f_power_off->animation_second++;
#if DEFAULT_POWEROFF_ANIMATION_LOGO_IMAGE_NUMBER
        if(f_power_off->pic_num_disp <= DEFAULT_POWEROFF_ANIMATION_LOGO_IMAGE_NUMBER)
        {
            if(f_power_off->pic_num_disp == 1)
            {
                compo_picturebox_set(pic,UI_BUF_I335005_LOGO_00_BIN);
            }
            else if(f_power_off->pic_num_disp == 2)
            {
                compo_picturebox_set(pic,UI_BUF_I335005_LOGO_01_BIN);
            }
            else if(f_power_off->pic_num_disp == 3)
            {
                compo_picturebox_set(pic,UI_BUF_I335005_LOGO_02_BIN);
            }
            else if(f_power_off->pic_num_disp == 4)
            {
                compo_picturebox_set(pic,UI_BUF_I335005_LOGO_03_BIN);
            }
            else if(f_power_off->pic_num_disp == 5)
            {
                compo_picturebox_set(pic,UI_BUF_I335005_LOGO_04_BIN);
            }
        }
        else if(f_power_off->pic_num_disp == DEFAULT_POWEROFF_ANIMATION_LOGO_IMAGE_NUMBER + 1)
#else
        if(f_power_off->animation_second == DEFAULT_POWEROFF_ANIMATION_SECOND*1000/DEFAULT_POWEROFF_ANIMATION_LOGO_IMAGE_NUMBER_TIME)
#endif
        {
            uteApplicationCommonPoweroff();
        }
    }
}
#endif
#else
//创建关机窗体
compo_form_t *func_power_off_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
static void func_power_off_disp_process(void)
{
    f_power_off_t* f_power_off = (f_power_off_t*)func_cb.f_cb;

    if (tick_check_expire(f_power_off->tick, 1000))
    {
        f_power_off->tick = tick_get();
        f_power_off->animation_second ++;
    }

    if (f_power_off->animation_second > DEFAULT_POWEROFF_ANIMATION_SECOND)
    {
        uteApplicationCommonPoweroff();
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//关机界面处理
static void func_power_off_process(void)
{
    func_power_off_disp_process();
    func_process();
}

//关机界面消息处理
static void func_power_off_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_SYS_1S:
            break;
    }
}

//进入关机界面
static void func_power_off_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_power_off_t));
    func_cb.frm_main = func_power_off_form_create();
    f_power_off_t* f_power_off = (f_power_off_t*)func_cb.f_cb;

    f_power_off->tick = 0;
    f_power_off->pic_num_disp = 0;
    f_power_off->animation_second = 0;
}

//退出关机界面
static void func_power_off_exit(void)
{
    task_stack_init();  //任务堆栈
    latest_task_init(); //最近任务
}

//关机界面
void func_power_off(void)
{
    printf("%s\n", __func__);
    func_power_off_enter();
    while (func_cb.sta == FUNC_POWER_OFF)
    {
        func_power_off_process();
        func_power_off_message(msg_dequeue());
    }
    func_power_off_exit();
}

#endif
