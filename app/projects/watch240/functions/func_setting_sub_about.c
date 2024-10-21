#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define Card_height 66
#define Card_Y 74

enum{
   Name=1,
   Edition,
   BT_address
};

extern void func_debug_info_check();

typedef struct f_about_t_ {
    bool flag_drag;                 //开始拖动
    s32 x_pos;
    s32 y_pos;
    s32 x;
    s32 y;

} f_about_t;

//关于页面
compo_form_t *func_set_sub_about_form_create(void)
{
    component_t *compo;
    char buf[16];


    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_ABOUT]);

    //设备名称
    compo = compo_cardbox_create(frm, 0, 1, 2, GUI_SCREEN_WIDTH-10, Card_height);
    compo_cardbox_set_pos(compo, GUI_SCREEN_CENTER_X, Card_Y);
    compo_setid(compo, 0);
    compo_cardbox_icon_set(compo, 0, UI_BUF_SIDEBAR_BG_308_156_BIN);  //方框
    compo_cardbox_icon_set_location(compo, 0, 0, 0, GUI_SCREEN_WIDTH-10,Card_height);

    compo_cardbox_text_set(compo, 0, i18n[STR_Equipment_name]);    //文字
    compo_cardbox_text_set_location(compo,0,-3*24+6,-12,GUI_SCREEN_WIDTH-10,Card_height/2);

    snprintf(buf, sizeof(buf), "%s",DEFAULT_BLE_DEV_NEME);    //信息
    compo_cardbox_text_set_font(compo, 1, UI_BUF_0FONT_FONT_NUM_16_BIN);
    compo_cardbox_text_set(compo, 1, buf);
    compo_cardbox_text_set_location(compo,1,-7*12+4,12+6,GUI_SCREEN_WIDTH-10,Card_height/2);

    //系统版本
    compo = compo_cardbox_create(frm, 0, 1, 2, GUI_SCREEN_WIDTH-10, Card_height);
    compo_cardbox_set_pos(compo, GUI_SCREEN_CENTER_X, Card_Y*2);
    compo_setid(compo, 1);
    compo_cardbox_icon_set(compo, 0, UI_BUF_SIDEBAR_BG_308_156_BIN);  //方框
    compo_cardbox_icon_set_location(compo, 0, 0, 0, GUI_SCREEN_WIDTH-10,Card_height);

    compo_cardbox_text_set(compo, 0, i18n[STR_System_version]);    //文字
    compo_cardbox_text_set_location(compo,0,-3*24+6,-12,GUI_SCREEN_WIDTH-10,Card_height/2);

    uteModulePlatformGetDevName("12345",5);
    snprintf(buf, sizeof(buf), "%s",DEFAULT_BLE_DEV_NEME);    //信息
    compo_cardbox_text_set_font(compo, 1, UI_BUF_0FONT_FONT_NUM_16_BIN);
    compo_cardbox_text_set(compo, 1, buf);
    compo_cardbox_text_set_location(compo,1,-7*12+4,12+6,GUI_SCREEN_WIDTH-10,Card_height/2);

    //蓝牙地址
    compo = compo_cardbox_create(frm, 0, 1, 2, GUI_SCREEN_WIDTH-10, Card_height);
    compo_cardbox_set_pos(compo, GUI_SCREEN_CENTER_X, Card_Y*3);
    compo_setid(compo, 2);
    compo_cardbox_icon_set(compo, 0, UI_BUF_SIDEBAR_BG_308_156_BIN);  //方框
    compo_cardbox_icon_set_location(compo, 0, 0, 0, GUI_SCREEN_WIDTH-10,Card_height);

    compo_cardbox_text_set(compo, 0, i18n[STR_Bluetooth_address]);    //文字
    compo_cardbox_text_set_location(compo,0,-3*24+6,-12,GUI_SCREEN_WIDTH-10,Card_height/2);

    uteModulePlatformGetBleMacAddress(buf);//获取蓝牙地址
    snprintf(buf, sizeof(buf), "%s",buf);    //信息
    compo_cardbox_text_set_font(compo, 1, UI_BUF_0FONT_FONT_NUM_16_BIN);
    compo_cardbox_text_set(compo, 1, buf);
    compo_cardbox_text_set_location(compo,1,-4*24+6,12+6,GUI_SCREEN_WIDTH-10,Card_height/2);

    return frm;
}

//关于功能事件处理
static void func_set_sub_about_process(void)
{
    s32 h_x = 0, h_y = 0;
    f_about_t *slp = (f_about_t *)func_cb.f_cb;
    widget_page_t *page_body = func_cb.frm_main->page_body;

    if(slp->flag_drag) {
        slp->flag_drag = ctp_get_dxy(&h_x, &h_y);
         if (slp->flag_drag ) {
             slp->y = slp->y_pos + h_y;
             widget_page_set_client(page_body, 0, slp->y);
         } else {
              slp->y_pos = slp->y;
         }
    }
    func_debug_info_check();
    func_process();
}


//关于功能消息处理
static void func_set_sub_about_message(size_msg_t msg)
{
    f_about_t *slp = (f_about_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (msg) {

    case MSG_CTP_CLICK:
         if(id == 1)
         func_directly_back_to();
        break;
    case MSG_CTP_SHORT_UP:
    case MSG_CTP_SHORT_DOWN:
        slp->flag_drag = true;

    case KU_DELAY_BACK:
        break;

    default:
		func_message(msg);
        break;
    }
}

//进入关于功能
static void func_set_sub_about_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_about_t));
    func_cb.frm_main = func_set_sub_about_form_create();
}

//退出关于功能
static void func_set_sub_about_exit(void)
{
    func_cb.last = FUNC_SET_SUB_ABOUT;
}

//关于功能
void func_set_sub_about(void)
{
    printf("%s\n", __func__);
    func_set_sub_about_enter();
    while (func_cb.sta == FUNC_SET_SUB_ABOUT) {
        //func_set_sub_about_process();
        func_set_sub_about_message(msg_dequeue());
    }
    func_set_sub_about_exit();
}
