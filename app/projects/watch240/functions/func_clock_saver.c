#include "func_clock_saver.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_gui_common.h"
#include "ute_module_watchonline.h"
#include "ute_project_config.h"
#include "ute_module_log.h"
#include "ute_module_systemtime.h"
#include "ute_drv_motor.h"
#include "ute_application_common.h"
#include "ute_drv_screen_common.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

const static u32 dialplate_saver_info[UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX] = UTE_MODULE_DIAL_SCREEN_SAVER_SORT_ADDRESS_ARRAYS;

typedef struct
{

} f_clock_saver_t;

u32 func_clock_saver_get_saver_info_addr(u32 index)
{
    if (index >= UTE_MODULE_SCREENS_DIAL_SCREEN_SAVER_CNT_MAX)
    {
        return 0;
    }

    return dialplate_saver_info[index];
}

u32 *func_clock_saver_get_saver_info_array(void)
{
    return (u32 *)dialplate_saver_info;
}

compo_form_t *func_clock_saver_form_create(void)
{
    compo_form_t *frm;
    uint8_t dialplate_index = 0;
    uteModuleGuiCommonGetCurrScreenSaverIndex(&dialplate_index);
    u32 base_addr = dialplate_saver_info[dialplate_index];
    u16 compo_num = bsp_uitool_header_phrase(base_addr);
    if (!compo_num)
    {
        halt(HALT_GUI_DIALPLATE_HEAD);
    }
    frm = compo_form_create(true);
    bsp_uitool_create(frm, base_addr, compo_num);
    return frm;
}

//时钟表盘功能事件处理
static void func_clock_saver_process(void)
{
    func_process();                                  //刷新UI

    if (sys_cb.guioff_delay)
    {
        sys_cb.guioff_delay = 1;
    }

    if (uteModuleGuiCommonIsUpdateScreenSaver())
    {
        uteModuleGuiCommonSetUpdateScreenSaver(false);
        if (sys_cb.sleep_delay)
        {
            sys_cb.sleep_delay = 1;
        }
    }
}

//时钟表盘功能消息处理
static void func_clock_saver_message(size_msg_t msg)
{
    if(!sys_cb.gui_sleep_sta)
    {
        return;
    }
    switch (msg)
    {
        default:
            func_message(msg);
            break;
    }
}

//进入时钟表盘功能
static void func_clock_saver_enter(void)
{
#if UTE_MODULE_SCREENS_SET_BACK_LIGHT_AFTER_IDLE_MODE_SUPPORT
    uteDrvScreenCommonIdleMode(true);
    delay_ms(10);
#endif
    uteDrvScreenCommonSetBacklightInIdleMode();
    uteModuleGuiCommonSetUpdateScreenSaver(false);
    func_cb.frm_main = func_clock_saver_form_create();
    uteModuleGuiCommonSetInScreenSaver(true);
}

//退出时钟表盘功能
static void func_clock_saver_exit(void)
{
    uteModuleGuiCommonSetInScreenSaver(false);
    uteTaskGuiStackRemoveScreenId(FUNC_CLOCK_SAVER);
    func_cb.last = FUNC_CLOCK_SAVER;
}

//时钟表盘功能
void func_clock_saver(void)
{
    printf("%s\n", __func__);
    func_clock_saver_enter();
    while (func_cb.sta == FUNC_CLOCK_SAVER)
    {
        func_clock_saver_process();
        func_clock_saver_message(msg_dequeue());
    }
    func_clock_saver_exit();
}
