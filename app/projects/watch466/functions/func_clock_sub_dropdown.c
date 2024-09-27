#include "include.h"
#include "func.h"
#include "func_clock.h"

#define MENU_DROPDOWN_ALPHA             200

//创建下拉菜单
static void func_clock_sub_dropdown_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 140);

    //创建状态
    compo_form_add_image(frm, UI_BUF_DROPDOWN_CONNECT_BIN, 180, 51);
    compo_form_add_image(frm, UI_BUF_DROPDOWN_POWER3_BIN, 281, 51);

    //创建天气
    compo_form_add_image(frm, UI_BUF_DROPDOWN_WINDY_BIN, 233, 207);

    //创建按钮
    compo_button_t *btn_connect = compo_button_create_by_image(frm, UI_BUF_DROPDOWN_LIGHT_BIN);
    compo_button_set_pos(btn_connect, 68, 207);
    compo_button_set_alpha(btn_connect, MENU_DROPDOWN_ALPHA);

    compo_button_t *btn_disurb = compo_button_create_by_image(frm, UI_BUF_DROPDOWN_CALL_BIN);
    compo_button_set_pos(btn_disurb, 76, 300);
    compo_button_set_alpha(btn_disurb, MENU_DROPDOWN_ALPHA);

    compo_button_t *btn_findphone = compo_button_create_by_image(frm, UI_BUF_DROPDOWN_CONNECT_PHONE_BIN);
    compo_button_set_pos(btn_findphone, 137, 373);
    compo_button_set_alpha(btn_findphone, MENU_DROPDOWN_ALPHA);

    compo_button_t *btn_mute = compo_button_create_by_image(frm, UI_BUF_DROPDOWN_FIND_PHONE_BIN);
    compo_button_set_pos(btn_mute, 233, 396);
    compo_button_set_alpha(btn_mute, MENU_DROPDOWN_ALPHA);

    compo_button_t *btn_flashlight = compo_button_create_by_image(frm, UI_BUF_DROPDOWN_SETTING_BIN);
    compo_button_set_pos(btn_flashlight, 329, 373);
    compo_button_set_alpha(btn_flashlight, MENU_DROPDOWN_ALPHA);

    compo_button_t *btn_light = compo_button_create_by_image(frm, UI_BUF_DROPDOWN_DISTURB_BIN);
    compo_button_set_pos(btn_light, 390, 300);
    compo_button_set_alpha(btn_light, MENU_DROPDOWN_ALPHA);

    compo_button_t *btn_scan = compo_button_create_by_image(frm, UI_BUF_DROPDOWN_MUTE_BIN);
    compo_button_set_pos(btn_scan, 398, 207);
    compo_button_set_alpha(btn_scan, MENU_DROPDOWN_ALPHA);

    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sub_frm = frm;
    f_clk->masklayer = masklayer;
}

//时钟表盘主要事件流程处理
static void func_clock_sub_dropdown_process(void)
{
    func_clock_sub_process();
}

//时钟表盘下拉菜单功能消息处理
static void func_clock_sub_dropdown_message(size_msg_t msg)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    switch (msg) {
    case MSG_CTP_SHORT_UP:
        if (func_switching(FUNC_SWITCH_MENU_DROPDOWN_UP, NULL)) {
            f_clk->sta = FUNC_CLOCK_MAIN;                   //上滑返回到时钟主界面
        }
        break;

    case KU_BACK:
    case MSG_QDEC_FORWARD:
        func_switching(FUNC_SWITCH_MENU_DROPDOWN_UP | FUNC_SWITCH_AUTO, NULL);
        f_clk->sta = FUNC_CLOCK_MAIN;                       //单击BACK键返回到时钟主界面
        break;

    case MSG_QDEC_BACKWARD:
        break;

    default:
        func_clock_sub_message(msg);
        break;
    }
}

//时钟表盘下拉菜单进入处理
static void func_clock_sub_dropdown_enter(void)
{
    func_clock_butterfly_set_light_visible(false);
    func_clock_sub_dropdown_form_create();
    if (!func_switching(FUNC_SWITCH_MENU_DROPDOWN_DOWN, NULL)) {
        return;                                             //下拉到一半取消
    }
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sta = FUNC_CLOCK_SUB_DROPDOWN;                   //进入到下拉菜单
}

//时钟表盘下拉菜单退出处理
static void func_clock_sub_dropdown_exit(void)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    compo_form_destroy(f_clk->sub_frm);
    func_clock_butterfly_set_light_visible(true);
    f_clk->sub_frm = NULL;
}

//时钟表盘下拉菜单
void func_clock_sub_dropdown(void)
{
    func_clock_sub_dropdown_enter();
    while (func_cb.sta == FUNC_CLOCK && ((f_clock_t *)func_cb.f_cb)->sta == FUNC_CLOCK_SUB_DROPDOWN) {
        func_clock_sub_dropdown_process();
        func_clock_sub_dropdown_message(msg_dequeue());
    }
    func_clock_sub_dropdown_exit();
}
