#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_FLASHON = 1,
    COMPO_ID_BTN_FLASHOFF,

    COMPO_ID_PIC_FLASHON,
    COMPO_ID_PIC_FLASHOFF,
};

typedef struct f_flashlight_t_
{
    u8 flashlight_flag;
    u8 light_level;
} f_flashlight_t;

//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);
//
//    //新建按钮
//    compo_button_t *btn;
//    btn = compo_button_create_by_image(frm, UI_BUF_ICON_FLASHLIGHT_BIN);
//    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
//
//    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_BUTTON_BIN);
//    compo_setid(btn, COMPO_ID_BTN_FLASHON);
//    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 258);
//
//  //创建文本
//    compo_textbox_t *txt_idle = compo_textbox_create(frm, 2);
//    compo_setid(txt_idle, COMPO_ID_PIC_FLASHON);
//    compo_textbox_set_pos(txt_idle, GUI_SCREEN_CENTER_X, 258);
//    compo_textbox_set(txt_idle, "打开");
//    compo_textbox_set_visible(txt_idle, false);
//
//    compo_textbox_t *txt = compo_textbox_create(frm, 2);
//    compo_setid(txt, COMPO_ID_PIC_FLASHOFF);
//    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 258);
//    compo_textbox_set(txt, "关闭");
//    compo_textbox_set_visible(txt, true);



    return frm;
}

//手电筒功能事件处理
static void func_flashlight_process(void)
{
    func_process();
}

//单击按钮
//static void func_flashlight_button_click(void)
//{
//    int id = compo_get_button_id();
//    compo_textbox_t *txt_off = compo_getobj_byid(COMPO_ID_PIC_FLASHOFF);
//    compo_textbox_t *txt_on  = compo_getobj_byid(COMPO_ID_PIC_FLASHON);
//    f_flashlight_t *fst = (f_flashlight_t *)func_cb.f_cb;
//    switch(id)
//    {
//        case COMPO_ID_BTN_FLASHON:
//            if (fst->flashlight_flag)
//            {
//                fst->flashlight_flag = 0;
//            }
//            else
//            {
//                fst->flashlight_flag = 1;
//            }
//            break;
//
//        default:
//            break;
//    }
//    if (fst->flashlight_flag)
//    {
//        compo_textbox_set_visible(txt_on, true);
//        compo_textbox_set_visible(txt_off, false);
//    }
//    else
//    {
//        compo_textbox_set_visible(txt_on, false);
//        compo_textbox_set_visible(txt_off, true);
//    }
//}

//手电筒功能消息处理
static void func_flashlight_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            //func_flashlight_button_click();
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

//进入手电筒控制功能
static void func_flashlight_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_flashlight_t));
    func_cb.frm_main = func_flashlight_form_create();
//    f_flashlight_t f * f_flashlight = (f_flashlight_t*)func_cb.f_cb;
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    f_flashlight ->light_level = sys_cb.light_level;
    sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
    tft_bglight_set_level(sys_cb.light_level,false);
}

//退出手电筒功能
static void func_flashlight_exit(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    sys_cb.light_level = f_flashlight ->light_level;
    tft_bglight_set_level(sys_cb.light_level,false);
    func_cb.last = FUNC_FLASHLIGHT;
}

//手电筒功能
void func_flashlight(void)
{
    printf("%s\n", __func__);
    func_flashlight_enter();
    while (func_cb.sta == FUNC_FLASHLIGHT)
    {
        func_flashlight_process();
        func_flashlight_message(msg_dequeue());
    }
    func_flashlight_exit();
}
