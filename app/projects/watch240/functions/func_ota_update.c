#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

enum
{
    ROCKET_ID=1,
    ROCKET_BASE_ID,
};

typedef struct f_ota_update_t_
{
    u32 launch_roket_time;
} f_ota_update_t;



compo_form_t *func_ota_update_form_create(void)
{
    compo_picturebox_t *picbox;
    compo_form_t *frm = compo_form_create(true);

    picbox = compo_picturebox_create(frm, UI_BUF_UPGRADE_ROCKET_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*0.7);
    compo_setid(picbox,ROCKET_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_UPGRADE_ROCKET_BASE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*0.9);
    compo_setid(picbox,ROCKET_BASE_ID);

    return frm;
}

//显示升级界面处理
static void func_ota_update_disp(void)
{
    compo_picturebox_t *picbox;
    static u32 ticks = 0;
    f_ota_update_t *f_ota_update = (f_ota_update_t *)func_cb.f_cb;

    if (tick_check_expire(ticks, 100))
    {
        ticks = tick_get();
        reset_sleep_delay_all();
        f_ota_update->launch_roket_time -=10;
        if(f_ota_update->launch_roket_time < 40)
        {
            f_ota_update->launch_roket_time = GUI_SCREEN_HEIGHT*0.7;
        }

        picbox = compo_getobj_byid(ROCKET_ID); //遍历按键图标控件
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, f_ota_update->launch_roket_time);
    }
}


//刷新显示
static void func_ota_update_process(void)
{

    func_ota_update_disp();
    func_process();
}

//进入升级界面
static void func_ota_update_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_ota_update_t));
    func_cb.frm_main = func_ota_update_form_create();
    f_ota_update_t *f_ota_update = (f_ota_update_t *)func_cb.f_cb;

    f_ota_update->launch_roket_time = GUI_SCREEN_HEIGHT*0.7;
}

//退出升级界面
static void func_ota_update_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    f_ota_update_t *f_ota_update = (f_ota_update_t *)func_cb.f_cb;
    func_cb.last = FUNC_OTA_MODE;
}

//升级界面
void func_ota_update(void)
{
    printf("%s\n", __func__);
    func_ota_update_enter();
    reset_sleep_delay_all();
    while (func_cb.sta == FUNC_OTA_MODE)
    {
        func_ota_update_process();

    }
    func_ota_update_exit();
}
