#include "include.h"
#include "func.h"
#include "ute_module_log.h"
#include "string.h"


#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define Card_height 66
#define Card_Y 74

#define CARD_RECT_MAX   2
#define CARD_ICON_MAX   5
#define CARD_TEXT_MAX   5

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
    compo_cardbox_t *cardbox;
    uint8_t davName[40];
    uint8_t Ble_Address[6];
    uint8_t Ble_Address_str_buf[17+2];
    uint8_t davNameLength = sizeof(davName);

    memset(Ble_Address,'\0',sizeof(Ble_Address));
    memset(davName,'\0',sizeof(davName));

    uteModulePlatformGetDevName(davName,&davNameLength);//获取设备名称
    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址
   // UTE_MODULE_LOG(UTE_LOG_PROTOCOL_LVL,"11111:%s",davName);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_ABOUT]);

    //设备名称
    cardbox = compo_cardbox_create(frm, 0, 1, 2, GUI_SCREEN_WIDTH-10, Card_height);
    compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, Card_Y);
    compo_setid(cardbox, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_SIDEBAR_BG_308_156_BIN);  //方框
    compo_cardbox_icon_set_location(cardbox, 0, 0, 0, GUI_SCREEN_WIDTH-10,Card_height);

    compo_cardbox_text_set(cardbox, 0, i18n[STR_DEV_NEME]);    //文字
    compo_cardbox_text_set_location(cardbox,0,-3*24+6,-12,GUI_SCREEN_WIDTH-10,Card_height/2);

    compo_cardbox_text_set_font(cardbox, 1, UI_BUF_0FONT_FONT_NUM_16_BIN);
    compo_cardbox_text_set(cardbox, 1, (const char *)davName);
    compo_cardbox_text_set_location(cardbox,1,-(9-strlen(davName)/2)*12+4,12+4,GUI_SCREEN_WIDTH-10,Card_height/2);

    //系统版本
    cardbox = compo_cardbox_create(frm, 0, 1, 2, GUI_SCREEN_WIDTH-10, Card_height);
    compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, Card_Y*2);
    compo_setid(cardbox, 1);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_SIDEBAR_BG_308_156_BIN);  //方框
    compo_cardbox_icon_set_location(cardbox, 0, 0, 0, GUI_SCREEN_WIDTH-10,Card_height);

    compo_cardbox_text_set(cardbox, 0, i18n[STR_SYS_VERSION]);    //文字
    compo_cardbox_text_set_location(cardbox,0,-3*24+6,-12,GUI_SCREEN_WIDTH-10,Card_height/2);

    compo_cardbox_text_set_font(cardbox, 1, UI_BUF_0FONT_FONT_NUM_16_BIN);
    compo_cardbox_text_set(cardbox, 1, UTE_SW_VERSION);
    compo_cardbox_text_set_location(cardbox,1,-(9-sizeof(UTE_SW_VERSION)/2)*12+4,12+6,GUI_SCREEN_WIDTH-10,Card_height/2);

    //蓝牙地址
    cardbox = compo_cardbox_create(frm, 0, 1, 2, GUI_SCREEN_WIDTH-10, Card_height);
    compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, Card_Y*3);
    compo_setid(cardbox, 2);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_SIDEBAR_BG_308_156_BIN);  //方框
    compo_cardbox_icon_set_location(cardbox, 0, 0, 0, GUI_SCREEN_WIDTH-10,Card_height);

    compo_cardbox_text_set(cardbox, 0, i18n[STR_BLE_MAC]);    //文字
    compo_cardbox_text_set_location(cardbox,0,-3*24+6,-12,GUI_SCREEN_WIDTH-10,Card_height/2);

    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02x:%02x:%02x:%02x:%02x:%02x",Ble_Address[0],Ble_Address[1],Ble_Address[2],Ble_Address[3],Ble_Address[4],Ble_Address[5]); //信息
    compo_cardbox_text_set_font(cardbox, 1, UI_BUF_0FONT_FONT_NUM_16_BIN);
    compo_cardbox_text_set(cardbox, 1, Ble_Address_str_buf);
    compo_cardbox_text_set_location(cardbox,1,-(9-(sizeof(Ble_Address_str_buf)/2-3))*12,12+6,GUI_SCREEN_WIDTH-10,Card_height/2);

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
