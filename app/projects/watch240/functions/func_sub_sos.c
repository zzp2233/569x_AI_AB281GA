#include "include.h"
#include "func.h"
#include "ute_module_call.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODULE_SCREENS_SUB_SOS_SUPPORT
enum
{
    COMPO_ID_BTN_SURE = 1,
    COMPO_ID_BTN_BACK,

    COMPO_ID_TXT_NUM,
    COMPO_ID_TXT_SHAPE_BOX,
    COMPO_ID_TXT_SHAPE_BG,
    SHAPE_ID_KEYBOARD_BG,
    SHAPE_ID_KEYBOARD_BOX,
    COMPO_ID_INPUT_BOX,
    COMPO_ID_OPEN_BTN,
    COMPO_ID_TXT_CURSOR,

    COMPO_ID_BLE_NO_CON_PIC,//蓝牙未连接
    COMPO_ID_BLE_NO_CON_TXT,//蓝牙未连接
    COMPO_ID_SOS_NOT_DATA_PIC,//SOS没有数据
    COMPO_ID_SOS_NOT_DATA_TXT,//SOS没有数据
};
enum
{
    SOS_STATE_BT_OFF=0,//蓝牙为连接
    SOS_STATE_NO_NUMBER,//未设置sos号码
    SOS_STATE_BT_CALL,//拨号中
};

typedef struct f_sos_t_
{
    u8 sos_state;
} f_sos_t;


static void get_sos_state(void)
{
    f_sos_t* f_sos = (f_sos_t*)func_cb.f_cb;

    if(!uteModuleCallBtIsConnected())
    {
        f_sos->sos_state = SOS_STATE_BT_OFF;
    }
    else if(uteModuleCallGetSosContactSize() <= 0)
    {
        f_sos->sos_state = SOS_STATE_NO_NUMBER;
    }
    else if(uteModuleCallGetSosContactSize() > 0)
    {
        f_sos->sos_state = SOS_STATE_BT_CALL;
    }
}
static void updata_sos_state(void)
{
    f_sos_t* f_sos = (f_sos_t*)func_cb.f_cb;
    u8 sos_state=0;

    if(!uteModuleCallBtIsConnected())
    {
        sos_state = SOS_STATE_BT_OFF;
    }
    else if(uteModuleCallGetSosContactSize() <= 0)
    {
        sos_state = SOS_STATE_NO_NUMBER;
    }
    else if(uteModuleCallGetSosContactSize() > 0)
    {
        sos_state = SOS_STATE_BT_CALL;
    }

    if(sos_state != f_sos->sos_state)
    {
        msg_enqueue(MSG_CHECK_LANGUAGE);
    }
}

// SOS拨打
void func_sub_sos_dial(void)
{
    memset(sys_cb.outgoing_number, 0, sizeof(sys_cb.outgoing_number));
    ute_module_call_addressbook_t sosData;
    memset(&sosData, 0, sizeof(ute_module_call_addressbook_t));
#if UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT
    uteModuleCallGetSosContact(&sosData);
#endif
    if (strlen((const char *)sosData.numberAscii) && uteModuleCallBtIsConnected())
    {
        memcpy(sys_cb.outgoing_number, sosData.numberAscii, strlen((const char *)sosData.numberAscii));
#if MODEM_CAT1_EN
        if (bsp_modem_get_init_flag())
        {
            modem_call_dial(sys_cb.outgoing_number);
        }
        else
#endif
        {
            bt_call_redial_number();
        }
    }
}

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//SOS页面
compo_form_t *func_sub_sos_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t* pic;
    compo_textbox_t* txt;
    compo_button_t *btn;

    if(!uteModuleCallBtIsConnected())
    {
        //创建蓝牙未连接界面
        pic = compo_picturebox_create(frm, UI_BUF_I335001_23_SOS_5_BLUETOOTH_NOT_CONNECTED_ICON_NOT_CONNECTED_72X74_X84_Y74_BIN);
        compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 74+74/2);
        compo_setid(pic, COMPO_ID_BLE_NO_CON_PIC);

        txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 182+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
        compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
        compo_setid(txt, COMPO_ID_BLE_NO_CON_TXT);
    }
    else if(uteModuleCallGetSosContactSize() <= 0)
    {
        //创建没有数据界面
        pic = compo_picturebox_create(frm, UI_BUF_I335001_23_SOS_1_SET_NUMBER_ICON_PHONE_56X92_X92_Y66_BIN);
        compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 66+92/2);
        compo_setid(pic, COMPO_ID_SOS_NOT_DATA_PIC);

        txt = compo_textbox_create(frm, strlen(i18n[STR_ADDRESS_BOOK_SYNC]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 188+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
        compo_textbox_set(txt, i18n[STR_ADDRESS_BOOK_SYNC]);
        compo_setid(txt, COMPO_ID_SOS_NOT_DATA_TXT);
    }
#if UTE_THREE_KEY_EVENT_SOS
    else if(sys_cb.sos_open_flag == true)
    {
        func_sub_sos_dial();
    }
#endif
    else if(uteModuleCallGetSosContactSize() > 0)
    {
        //创建是否拨打界面
        txt = compo_textbox_create(frm, strlen(i18n[STR_DIAL_SOS]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 110+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
        compo_textbox_set(txt, i18n[STR_DIAL_SOS]);
        compo_setid(txt, COMPO_ID_TXT_NUM);

        //取消按钮
        btn = compo_button_create_by_image(frm, UI_BUF_I335001_23_SOS_2_WHETHER_TO_DIAL_ICON_BUTTON_102X52_X16_Y222_BIN);
        compo_button_set_pos(btn, 16+102/2, 222+52/2);
        compo_setid(btn, COMPO_ID_BTN_BACK);

        //确定按钮
        btn = compo_button_create_by_image(frm, UI_BUF_I335001_23_SOS_2_WHETHER_TO_DIAL_ICON_BUTTON_102X52_X122_Y220_BIN);
        compo_button_set_pos(btn, 122+102/2, 222+52/2);
        compo_setid(btn, COMPO_ID_BTN_SURE);
    }

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//SOS页面
compo_form_t *func_sub_sos_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t* pic;
    compo_textbox_t* txt;
    compo_button_t *btn;


    if(!uteModuleCallBtIsConnected())
    {
        // icon
        compo_picturebox_t *icon = compo_picturebox_create(frm, UI_BUF_I338001_27_MORE_CONNECT_BIN);
        compo_picturebox_set_pos(icon, GUI_SCREEN_CENTER_X, 50+96);

        // 创建提示文本
        compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 42/2+274, 170, 28);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
    }
    else if(uteModuleCallGetSosContactSize() <= 0)
    {
        // icon
        compo_picturebox_t *icon = compo_picturebox_create(frm, UI_BUF_I338001_27_MORE_PHONE_BIN);
        compo_picturebox_set_pos(icon, GUI_SCREEN_CENTER_X, 104+134/2);

        // 创建提示文本
        compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_ADDRESS_BOOK_SYNC]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 42/2+274, 170, 28);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set(txt, i18n[STR_ADDRESS_BOOK_SYNC]);
    }
#if UTE_THREE_KEY_EVENT_SOS
    else if(sys_cb.sos_open_flag == true)
    {
        func_sub_sos_dial();
    }
#endif
    else if(uteModuleCallGetSosContactSize() > 0)
    {
        //创建是否拨打界面
        txt = compo_textbox_create(frm, strlen(i18n[STR_DIAL_SOS]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 154, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
        compo_textbox_set(txt, i18n[STR_DIAL_SOS]);
        compo_setid(txt, COMPO_ID_TXT_NUM);

        //取消按钮
        btn = compo_button_create_by_image(frm, UI_BUF_I338001_20_ALARM_CLOCK_CANCEL_BIN);
        compo_button_set_pos(btn, 80/2+71,80/2+224);
        compo_setid(btn, COMPO_ID_BTN_BACK);

        //确定按钮
        btn = compo_button_create_by_image(frm, UI_BUF_I338001_20_ALARM_CLOCK_COMFIRM_BIN);
        compo_button_set_pos(btn, 80/2+209,80/2+224);
        compo_setid(btn, COMPO_ID_BTN_SURE);
    }

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
//SOS页面
compo_form_t *func_sub_sos_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建蓝牙未连接界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I342001_23_SOS_BLUETOOTH_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 64+64/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BLE_NO_CON_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 158+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
    compo_setid(txt, COMPO_ID_BLE_NO_CON_TXT);

    //创建没有数据界面
    pic = compo_picturebox_create(frm, UI_BUF_I342001_23_SOS_PHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 53+82/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_SOS_NOT_DATA_PIC);

    txt = compo_textbox_create(frm, strlen(i18n[STR_ADDRESS_BOOK_SYNC]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 159+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_ADDRESS_BOOK_SYNC]);
    compo_setid(txt, COMPO_ID_SOS_NOT_DATA_TXT);

    //创建是否拨打界面
    txt = compo_textbox_create(frm, strlen(i18n[STR_DIAL_SOS]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 98+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_DIAL_SOS]);
    compo_setid(txt, COMPO_ID_TXT_NUM);

    //取消按钮
    pic = compo_picturebox_create(frm, UI_BUF_I342001_23_SOS_CANCEL_BIN);
    compo_picturebox_set_pos(pic, 49+52/2, 163+52/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BTN_BACK);

    //确定按钮
    pic = compo_picturebox_create(frm, UI_BUF_I342001_23_SOS_CONFIRM_BIN);
    compo_picturebox_set_pos(pic, 137+54/2, 163+52/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BTN_SURE);

    if(!uteModuleCallBtIsConnected())
    {
        pic = compo_getobj_byid(COMPO_ID_BLE_NO_CON_PIC);
        compo_picturebox_set_visible(pic, true);
        txt = compo_getobj_byid(COMPO_ID_BLE_NO_CON_TXT);
        compo_textbox_set_visible(txt, true);
    }
    else if(uteModuleCallGetSosContactSize() <= 0)
    {
        pic = compo_getobj_byid(COMPO_ID_SOS_NOT_DATA_PIC);
        compo_picturebox_set_visible(pic, true);
        txt = compo_getobj_byid(COMPO_ID_SOS_NOT_DATA_TXT);
        compo_textbox_set_visible(txt, true);
    }
    else if(uteModuleCallGetSosContactSize() > 0)
    {
        pic = compo_getobj_byid(COMPO_ID_BTN_BACK);
        compo_picturebox_set_visible(pic, true);
        pic = compo_getobj_byid(COMPO_ID_BTN_SURE);
        compo_picturebox_set_visible(pic, true);
        txt = compo_getobj_byid(COMPO_ID_TXT_NUM);
        compo_textbox_set_visible(txt, true);
    }
    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
//SOS页面
compo_form_t *func_sub_sos_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建蓝牙未连接界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_BLUETOOTH_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 120+116/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BLE_NO_CON_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 282+24, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
    compo_setid(txt, COMPO_ID_BLE_NO_CON_TXT);

    //创建没有数据界面
    pic = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_PHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 109+142/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_SOS_NOT_DATA_PIC);

    txt = compo_textbox_create(frm, strlen(i18n[STR_ADDRESS_BOOK_SYNC]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 291+24, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_ADDRESS_BOOK_SYNC]);
    compo_setid(txt, COMPO_ID_SOS_NOT_DATA_TXT);

    //创建是否拨打界面
    txt = compo_textbox_create(frm, strlen(i18n[STR_DIAL_SOS]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 166+24, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_DIAL_SOS]);
    compo_setid(txt, COMPO_ID_TXT_NUM);

    //取消按钮
    pic = compo_picturebox_create(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
    compo_picturebox_set_pos(pic, 24+156/2, 347+80/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BTN_BACK);

    //确定按钮
    pic = compo_picturebox_create(frm, UI_BUF_I341001_28_SET_CONFIRM_2_BIN);
    compo_picturebox_set_pos(pic, 188+156/2, 347+80/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BTN_SURE);

    if(!uteModuleCallBtIsConnected())
    {
        pic = compo_getobj_byid(COMPO_ID_BLE_NO_CON_PIC);
        compo_picturebox_set_visible(pic, true);
        txt = compo_getobj_byid(COMPO_ID_BLE_NO_CON_TXT);
        compo_textbox_set_visible(txt, true);
    }
    else if(uteModuleCallGetSosContactSize() <= 0)
    {
        pic = compo_getobj_byid(COMPO_ID_SOS_NOT_DATA_PIC);
        compo_picturebox_set_visible(pic, true);
        txt = compo_getobj_byid(COMPO_ID_SOS_NOT_DATA_TXT);
        compo_textbox_set_visible(txt, true);
    }
    else if(uteModuleCallGetSosContactSize() > 0)
    {
        pic = compo_getobj_byid(COMPO_ID_BTN_BACK);
        compo_picturebox_set_visible(pic, true);
        pic = compo_getobj_byid(COMPO_ID_BTN_SURE);
        compo_picturebox_set_visible(pic, true);
        txt = compo_getobj_byid(COMPO_ID_TXT_NUM);
        compo_textbox_set_visible(txt, true);
    }
    return frm;
}

#else
compo_form_t *func_sub_sos_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
#endif /*GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT*/

///SOS功能事件处理
static void func_sub_sos_process(void)
{
    // f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
    updata_sos_state();
    func_process();
}

///按键事件处理
static void func_sos_button_click(void)
{
    int id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BTN_SURE:///确认
            func_sub_sos_dial();
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            break;
        case COMPO_ID_BTN_BACK://取消
            uteModuleGuiCommonGoBackLastScreen();
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            break;
        default:
            break;
    }
}

///SOS功能消息处理
static void func_sub_sos_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sos_button_click();                  //单击按钮
            break;

        case KU_DELAY_BACK:
            break;

        default:
            func_message(msg);
            break;
    }
}

///进入SOS功能
static void func_sub_sos_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sos_t));
    func_cb.frm_main = func_sub_sos_form_create();
    get_sos_state();
}

///退出SOS功能
static void func_sub_sos_exit(void)
{
    sys_cb.sos_open_flag = false;
    func_cb.last = FUNC_SUB_SOS;
    uteTaskGuiStackRemoveScreenId(FUNC_SUB_SOS);
}

///SOS功能
void func_sub_sos(void)
{
    printf("%s\n", __func__);
    func_sub_sos_enter();
    while (func_cb.sta == FUNC_SUB_SOS)
    {
        func_sub_sos_process();
        func_sub_sos_message(msg_dequeue());
    }
    func_sub_sos_exit();
}
#endif
