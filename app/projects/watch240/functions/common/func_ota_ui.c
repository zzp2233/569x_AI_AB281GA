#include "include.h"
#include "func.h"
#include "func_idle.h"
#include "bsp_led.h"

#if FOTA_UI_EN

AT(.text.func.idle)
void func_ota_ui_process(void)
{
    reset_sleep_delay_all();
    func_process();
}

static void func_ota_ui_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    compo_picturebox_t *picbox;
    compo_form_t *frm = compo_form_create(true);

    picbox = compo_picturebox_create(frm, UI_BUF_UPGRADE_ROCKET_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
//    compo_setid(picbox,ROCKET_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_UPGRADE_ROCKET_BASE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*0.9);
//    compo_setid(picbox,ROCKET_BASE_ID);

    gui_process();
    delay_5ms(10);
    os_gui_draw_w4_done();
    printf("%s\n", __func__);
}

void func_ota_ui_message(u16 msg)
{
    evt_message(msg);
}

static void func_ota_ui_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_IDLE;
}

AT(.text.func.idle)
void func_ota_ui(void)
{
    printf("%s\n", __func__);

    func_ota_ui_enter();

    while (func_cb.sta == FUNC_OTA_UI_MODE)
    {
        func_ota_ui_process();
        func_ota_ui_message(msg_dequeue());
    }

    func_ota_ui_exit();
}

#endif
