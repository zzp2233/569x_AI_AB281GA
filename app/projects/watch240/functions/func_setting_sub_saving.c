#include "include.h"
#include "func.h"
#include "ute_module_sport.h"
#include "ute_application_common.h"
#include "ute_module_gui_common.h"
#include "ute_module_settingremind.h"

#if UTE_MODULE_POWER_SAVING_SUPPORT

typedef struct f_saving_t_
{
} f_saving_t;

enum
{
    PIC_BTN_POWER_SAVE = 1,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
static const u32 tbl_saving_switch_res[] =
{
    UI_BUF_I341001_28_SET_SWITCH_OPEN_BUTTON_BIN,   //ON
    UI_BUF_I341001_28_SET_SWITCH_CLOSE_BUTTON_BIN,  //OFF
};

//省电模式页面
compo_form_t *func_set_sub_saving_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_POWER_SAVING]);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_POWER_SAVING]));
    compo_textbox_set_location(textbox, 24, 94, 248, 40);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, i18n[STR_POWER_SAVING]);

    compo_button_t *saving_pic = compo_button_create_by_image(frm, tbl_saving_switch_res[0]);
    compo_button_set_pos(saving_pic, 24 + 320 - gui_image_get_size(tbl_saving_switch_res[0]).wid/2, 112);
    compo_setid(saving_pic, PIC_BTN_POWER_SAVE);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 160, 320, 1);
    compo_shape_set_color(shape, make_color(0x29, 0x29, 0x29));

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SAVING_TIP]));
    compo_textbox_set_location(textbox, 24, 180, 320, 240);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox, i18n[STR_SAVING_TIP]);
    compo_textbox_set_forecolor(textbox, make_color(102, 102, 102));

    return frm;
}

#endif
//省电模式事件处理
void func_set_sub_saving_process(void)
{
    compo_button_t *saving_pic = compo_getobj_byid(PIC_BTN_POWER_SAVE);

    if (uteModuleGuiCommonGetPowerSavingModeOpen())
    {
        compo_button_set_bgimg(saving_pic, UI_BUF_I341001_28_SET_SWITCH_OPEN_BUTTON_BIN);
    }
    else
    {
        compo_button_set_bgimg(saving_pic, UI_BUF_I341001_28_SET_SWITCH_CLOSE_BUTTON_BIN);
    }

    func_process();
}

//单击按钮
void func_saving_button_click(void)
{
    bool powersaving_state;
    int id = compo_get_button_id();
    if (id == PIC_BTN_POWER_SAVE)
    {
        powersaving_state = !uteModuleGuiCommonGetPowerSavingModeOpen();
        uteModuleGuiCommonSetPowerSavingModeOpen(powersaving_state);
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
#if 0//UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT
        if (uteModuleSettingRemindGetWhetherShowId(SETTING_REMIND_ID_SAVING))
        {
            uteModuleSettingRemindSetRemindId(SETTING_REMIND_ID_SAVING);
            uteTaskGuiStartScreen(FUNC_SETTING_REMIND, 0, __func__);
        }
#endif
    }
}

//省电模式功能消息处理
static void func_set_sub_saving_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_saving_button_click();
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入省电模式功能
static void func_set_sub_saving_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_saving_t));
    func_cb.frm_main = func_set_sub_saving_form_create();

}

//退出省电模式功能
static void func_set_sub_saving_exit(void)
{
    func_cb.last = FUNC_SET_SUB_SAVING;
}

//省电模式功能
void func_set_sub_saving(void)
{
    printf("%s\n", __func__);
    func_set_sub_saving_enter();
    while (func_cb.sta == FUNC_SET_SUB_SAVING)
    {
        func_set_sub_saving_process();
        func_set_sub_saving_message(msg_dequeue());
    }
    func_set_sub_saving_exit();
}

#endif
