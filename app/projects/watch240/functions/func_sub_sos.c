#include "include.h"
#include "func.h"
#include "ute_module_call.h"
#include "ute_drv_motor.h"
#include "ute_module_charencode.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODULE_SCREENS_SUB_SOS_SUPPORT
void func_sub_sos_dial(void);

static bool threeSta = false;
enum
{
    COMPO_ID_BTN_SURE = 1,
    COMPO_ID_BTN_BACK,

    COMPO_ID_TXT_NUM,
    COMPO_ID_TXT_SHAPE_BOX,
    COMPO_ID_TXT_SHAPE_BG,
    SHAPE_ID_KEYBOARD_BOX,
    COMPO_ID_INPUT_BOX,
    COMPO_ID_OPEN_BTN,
    COMPO_ID_TXT_CURSOR,
    COMPO_ID_TEXT_COUNT,

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
    u8 Countdown;
    u32 tick;
    bool touch_state;
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
    u8 sos_state = 0;

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
        threeSta = true;
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

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
//SOS页面
compo_form_t *func_sub_sos_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_picturebox_t* pic = NULL;
    compo_textbox_t* txt = NULL;
    compo_button_t *btn = NULL;

    if(!uteModuleCallBtIsConnected())
    {
        //创建蓝牙未连接界面
        pic = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_BLUETOOTH_BIN);
        compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 120+116/2);
        compo_setid(pic, COMPO_ID_BLE_NO_CON_PIC);

        txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 282+24, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
        compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
        compo_setid(txt, COMPO_ID_BLE_NO_CON_TXT);

    }
    else if(uteModuleCallGetSosContactSize() <= 0)
    {
        //创建没有数据界面
        pic = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_PHONE_BIN);
        compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 109+142/2);
        compo_setid(pic, COMPO_ID_SOS_NOT_DATA_PIC);

        txt = compo_textbox_create(frm, strlen(i18n[STR_ADDRESS_BOOK_SYNC]));
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 291+41, GUI_SCREEN_WIDTH / 1.2, 118);
        compo_textbox_set_multiline(txt, true);
        widget_text_set_ellipsis(txt->txt, false);
        compo_textbox_set(txt, i18n[STR_ADDRESS_BOOK_SYNC]);
        compo_setid(txt, COMPO_ID_SOS_NOT_DATA_TXT);

    }
    else if(uteModuleCallGetSosContactSize() > 0)
    {
        //创建是否拨打界面
        if (sys_cb.sos_open_flag == true)
        {
            pic = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_BG_BIN);
            compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
            compo_picturebox_set_size(pic, 430, 490);
            compo_setid(pic, COMPO_ID_TXT_SHAPE_BG);

            txt = compo_textbox_create(frm, strlen(i18n[STR_SOS_AUTO]));
            compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 192, 340, 40);
            compo_textbox_set(txt, i18n[STR_SOS_AUTO]);

            //接听
            btn = compo_button_create_by_image(frm, UI_BUF_I341001_11_CALL_BUTT_ANSWER_BIN);
            compo_setid(btn, SHAPE_ID_KEYBOARD_BOX);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, 310+48);

            //挂断
            btn = compo_button_create_by_image(frm, UI_BUF_I341001_27_MORE_TIMER_CANCEL2_BIN);
            compo_setid(btn, COMPO_ID_INPUT_BOX);
            compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, 310+48, 98, 98);

            txt = compo_textbox_create(frm, 5);
            compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 254, 200, 80);
            compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_54_BIN);
            compo_textbox_set(txt, (char *)"8");
            compo_setid(txt, COMPO_ID_TEXT_COUNT);

        }
        else
        {
            pic = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_BG_BIN);
            compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
            compo_picturebox_set_size(pic, 358, 438);
            compo_setid(pic, COMPO_ID_TXT_SHAPE_BG);

//          pic = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_SOS_BG_BIN);
//          compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 305 + 47);

            btn = compo_button_create_by_image(frm, UI_BUF_I341001_23_SOS_SOS_CALL_BIN);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 305 + 96/2);
            compo_setid(btn, SHAPE_ID_KEYBOARD_BOX);
        }
//        txt = compo_textbox_create(frm,strlen(i18n[STR_SLIDE_CALL]));
//        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X + 30, 305 + 47, 200, 40);
//        compo_textbox_set(txt,i18n[STR_SLIDE_CALL]);
//
//        /*创建一个椭圆用于滑动时覆盖字体*/
//        compo_shape_t *rect_cover = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//        compo_setid(rect_cover, COMPO_ID_INPUT_BOX);
//        compo_shape_set_color(rect_cover, make_color(0xff, 0x39, 0x10));
//        compo_shape_set_radius(rect_cover, GUI_SCREEN_HEIGHT/4/2);
//
//        //滑动按钮
//        compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I341001_23_SOS_SOS_CALL_BIN);
//        compo_button_set_pos(btn, 38 + 76/2, 315 + 76/2);
//        widget_set_size(btn->widget, 76, 76);
//        compo_setid(btn, SHAPE_ID_KEYBOARD_BOX);

//        txt = compo_textbox_create(frm, 5);
//        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 264, 200, 60);
//        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_54_BIN);
//        //compo_textbox_set(txt, i18n[STR_SLIDE_CALL]);
//        compo_setid(txt, COMPO_ID_TEXT_COUNT);

        ute_module_call_addressbook_t sosData;
        memset(&sosData, 0, sizeof(ute_module_call_addressbook_t));
#if UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT
        uteModuleCallGetSosContact(&sosData);
        uint8_t number[20] = {0}, name[41] = {0};
        uint16_t name_utf8_len = 0;

        if (sys_cb.sos_open_flag == true)
        {
            if (sosData.nameUnicodeLen && uteModuleCallBtIsConnected())
            {
                uteModuleCharencodeUnicodeConversionUtf8(&sosData.nameUnicode[0], (uint16_t)sosData.nameUnicodeLen, name, &name_utf8_len, 41);
                txt = compo_textbox_create(frm, strlen(name));
                compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 148, 330, widget_text_get_max_height());
                compo_textbox_set(txt, (char *)name);
            }
            else
            {
                memcpy(number, sosData.numberAscii, strlen((const char *)sosData.numberAscii));
                txt = compo_textbox_create(frm, strlen(number));
                compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 148, 330, widget_text_get_max_height());
                compo_textbox_set(txt, number);
            }
        }
        else
        {
            if (strlen((const char *)sosData.numberAscii) && uteModuleCallBtIsConnected())
            {
                memcpy(number, sosData.numberAscii, strlen((const char *)sosData.numberAscii));
                txt = compo_textbox_create(frm, strlen(number));
                compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 208, 330, widget_text_get_max_height());
                compo_textbox_set(txt, number);
            }
            if (sosData.nameUnicodeLen && uteModuleCallBtIsConnected())
            {
                uteModuleCharencodeUnicodeConversionUtf8(&sosData.nameUnicode[0], (uint16_t)sosData.nameUnicodeLen, name, &name_utf8_len, 41);
                txt = compo_textbox_create(frm, name_utf8_len);
                compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 158, 330, widget_text_get_max_height());
                compo_textbox_set(txt, (char *)name);
            }
        }
#endif
    }
    return frm;
}

static void func_long_press_button(void)
{
    f_sos_t *f_sos_press = (f_sos_t *)func_cb.f_cb;

    int id = compo_get_button_id();

    if (id == SHAPE_ID_KEYBOARD_BOX)    //触摸是否是按键图标
    {
        func_sub_sos_dial();
//        f_sos_press->touch_state = true;
    }
    if (sys_cb.sos_open_flag == true)
    {
        if (id == COMPO_ID_INPUT_BOX)
        {
            uteModuleGuiCommonGoBackLastScreen();
        }
    }
}

//static void func_long_press_event_handle(s32 distance)
//{
//    if(distance == 330 - 76/2) //滑动到设定X轴区间松开屏幕处理
//    {
//        func_sub_sos_dial();
//        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
//    }
//}
//
//static void func_long_press_slide_disp_handle()
//{
//    f_sos_t *f_sos_press = (f_sos_t *)func_cb.f_cb;
//
//    if (f_sos_press->touch_state && (f_sos_press->sos_state == SOS_STATE_BT_CALL ))
//    {
//        s32 distance,y;
//        compo_button_t *img_btn = compo_getobj_byid(SHAPE_ID_KEYBOARD_BOX); //遍历按键图标控件
//        compo_shape_t  *rect_cover = compo_getobj_byid(COMPO_ID_INPUT_BOX);//遍历红色拖尾控件
//
//        f_sos_press->touch_state = ctp_get_dxy(&distance, &y);//获取触屏状态与滑动长度
//        y = widget_get_location(img_btn->widget).y; //获取控件y轴
//        distance += 38 + 76/2;//获取滑动长度加上图标初始位置
//
//        if(distance<38 + 76/2)  //最小滑动距离
//        {
//            distance = 38 + 76/2;
//        }
//        else if(distance>330 - 76/2)  //最大滑动距离
//        {
//            distance = 330 - 76/2;
//        }
//
//        if(f_sos_press->touch_state == false)  //松开处理
//        {
//            func_long_press_event_handle(distance); //事件处理
//            distance = 38 + 76/2;//回弹
//        }
//
//        compo_button_set_pos(img_btn, distance, y);
//        compo_shape_set_location(rect_cover,distance/2+76/2, y, distance, 76);
//    }
//}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//SOS页面
compo_form_t *func_sub_sos_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    //创建蓝牙未连接界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I343001_23_SOS_CONNECT_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 102+98/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BLE_NO_CON_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 242+24, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
    compo_setid(txt, COMPO_ID_BLE_NO_CON_TXT);

    //创建没有数据界面
    pic = compo_picturebox_create(frm, UI_BUF_I343001_23_SOS_PHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 91+123/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_SOS_NOT_DATA_PIC);

    txt = compo_textbox_create(frm, strlen(i18n[STR_ADDRESS_BOOK_SYNC]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 250+24, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_ADDRESS_BOOK_SYNC]);
    compo_setid(txt, COMPO_ID_SOS_NOT_DATA_TXT);

    //创建是否拨打界面
    txt = compo_textbox_create(frm, strlen(i18n[STR_DIAL_SOS]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 147+24, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_DIAL_SOS]);
    compo_setid(txt, COMPO_ID_TXT_NUM);

    //取消按钮
    pic = compo_picturebox_create(frm, UI_BUF_I343001_COMM_CANCEL_BIN);
    compo_picturebox_set_pos(pic, 20+136/2, 299+70/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BTN_BACK);

    //确定按钮
    pic = compo_picturebox_create(frm, UI_BUF_I343001_COMM_CONFIRM_BIN);
    compo_picturebox_set_pos(pic, 164+136/2, 299+70/2);
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
    f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
//#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
//    func_long_press_slide_disp_handle();
//#endif
    updata_sos_state();
    if (sys_cb.sos_open_flag == true)
    {
        if (f_sos->sos_state == SOS_STATE_BT_CALL)
        {
            if (tick_check_expire(f_sos->tick, 1000))
            {
                reset_sleep_delay_all();
                compo_textbox_t *txt =  compo_getobj_byid(COMPO_ID_TEXT_COUNT);
                char buf[5] = {0};
                f_sos->tick = tick_get();
                if (f_sos->Countdown)
                {
                    snprintf(buf, sizeof(buf), "%d", f_sos->Countdown);
                    compo_textbox_set(txt, buf);
                    uteDrvMotorStart(100, 100, 1);
                    f_sos->Countdown--;
                }
                else
                {
                    func_sub_sos_dial();
                    uteDrvMotorStart(500, 500, 1);
                }
            }
        }
    }
    func_process();
}

///按键事件处理
#if !GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
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
#endif

///SOS功能消息处理
static void func_sub_sos_message(size_msg_t msg)
{
    f_sos_t *f_sos_press = (f_sos_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            func_long_press_button();
#else
            func_sos_button_click();                  //单击按钮
#endif
            break;
        case MSG_CTP_SHORT_RIGHT:
            if (!f_sos_press->touch_state)
            {
                if (func_cb.left_sta == func_cb.sta)
                {
                    break;
                }
                if (func_cb.flag_sort)
                {
                    func_switch_prev(false);                    //切到上一个任务
                }
                else /*if (ctp_get_sxy().x <= 32)*/
                {
                    func_backing_to();                          //右滑缓慢退出任务
                }
            }
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
            if (!sys_cb.sos_open_flag && uteModuleCallBtIsConnected())
            {
                func_message(msg);
            }
            break;
        default:
            func_message(msg);
            break;
    }
}

///进入SOS功能
static void func_sub_sos_enter(void)
{
    if (threeSta)
    {
        sys_cb.sos_open_flag = true;
        threeSta = false;
    }

    func_cb.f_cb = func_zalloc(sizeof(f_sos_t));
    func_cb.frm_main = func_sub_sos_form_create();
    get_sos_state();

    if (sys_cb.sos_open_flag == true)
    {
        f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
        f_sos->Countdown = 7;
        f_sos->tick = tick_get();
    }
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
