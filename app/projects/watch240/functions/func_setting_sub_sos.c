#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT
enum
{

    COMPO_ID_BTN_NUM0 = 1,
    COMPO_ID_BTN_NUM1,
    COMPO_ID_BTN_NUM2,
    COMPO_ID_BTN_NUM3,
    COMPO_ID_BTN_NUM4,
    COMPO_ID_BTN_NUM5,
    COMPO_ID_BTN_NUM6,
    COMPO_ID_BTN_NUM7,
    COMPO_ID_BTN_NUM8,
    COMPO_ID_BTN_NUM9,
    COMPO_ID_BTN_SURE,
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
typedef struct f_sos_t_
{
    uint32_t tick;
    u8 phone_number_cnt;
    bool input_mode_flag;
    u8 cursor_disp_time;
} f_sos_t;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//SOS页面
compo_form_t *func_set_sub_sos_form_create(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);


    //创建蓝牙未连接界面
    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I335001_23_SOS_5_BLUETOOTH_NOT_CONNECTED_ICON_NOT_CONNECTED_72X74_X84_Y74_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 74+74/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BLE_NO_CON_PIC);

    compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 182+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
    compo_setid(txt, COMPO_ID_BLE_NO_CON_TXT);

    //创建没有数据界面
    pic = compo_picturebox_create(frm, UI_BUF_I335001_23_SOS_1_SET_NUMBER_ICON_PHONE_56X92_X92_Y66_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 66+92/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_SOS_NOT_DATA_PIC);

    txt = compo_textbox_create(frm, strlen(i18n[STR_ADDRESS_BOOK_SYNC]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 188+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_ADDRESS_BOOK_SYNC]);
    compo_setid(txt, COMPO_ID_SOS_NOT_DATA_TXT);

    //创建是否拨打界面
    txt = compo_textbox_create(frm, strlen(i18n[STR_DIAL_SOS]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 110+28/2, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set_visible(txt, false);
    compo_textbox_set(txt, i18n[STR_DIAL_SOS]);
    compo_setid(txt, COMPO_ID_TXT_NUM);

    //取消按钮
    pic = compo_picturebox_create(frm, UI_BUF_I335001_23_SOS_2_WHETHER_TO_DIAL_ICON_BUTTON_102X52_X16_Y222_BIN);
    compo_picturebox_set_pos(pic, 16+102/2, 222+52/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_BTN_BACK);

    //确定按钮
    pic = compo_picturebox_create(frm, UI_BUF_I335001_23_SOS_2_WHETHER_TO_DIAL_ICON_BUTTON_102X52_X122_Y220_BIN);
    compo_picturebox_set_pos(pic, 122+102/2, 222+52/2);
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
#endif /*GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT*/
///SOS功能事件处理
static void func_set_sub_sos_process(void)
{
    f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
    compo_textbox_t *txt_cursor = compo_getobj_byid(COMPO_ID_TXT_CURSOR);
    compo_textbox_t *txt_number = compo_getobj_byid(COMPO_ID_TXT_NUM);

//    if(tick_check_expire(f_sos->tick, 100))
//    {
//        f_sos->tick = tick_get();

//        if(f_sos->input_mode_flag == true)///输入模式显示光标
//        {
//            if(++f_sos->cursor_disp_time==10)f_sos->cursor_disp_time=0;

//            if(f_sos->cursor_disp_time<7)
//            {
//                compo_textbox_set(txt_cursor,"2");
//                compo_textbox_set_pos(txt_cursor, GUI_SCREEN_CENTER_X+(widget_text_get_area(txt_cursor->txt).wid*strlen(sys_cb.sos_call_number))/2+widget_text_get_area(txt_cursor->txt).wid/3, GUI_SCREEN_CENTER_Y/2.5);
//                compo_textbox_set(txt_cursor,"I");
//                if(strlen(sys_cb.sos_call_number)!=11)
//                {
//                    compo_textbox_set_visible(txt_cursor, true);
//                }
//                else
//                {
//                    compo_textbox_set_visible(txt_cursor, false);
//                }
//            }
//            else
//            {
//                compo_textbox_set_visible(txt_cursor, false);
//            }
//        }
//        else
//        {

//            if(sys_cb.sos_call_number[0]=='\0' || sys_cb.sos_call_number[0]==0)
//            {
//                if(++f_sos->cursor_disp_time==10)f_sos->cursor_disp_time=0;

//                if(f_sos->cursor_disp_time<7)
//                {
//                    compo_textbox_set_visible(txt_number, true);
//                }
//                else
//                {
//                    compo_textbox_set_visible(txt_number, false);
//                }
//            }
//        }

//    }

    func_process();
}

///按键事件处理
static void func_sos_button_click(void)
{
    int id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BTN_SURE:///确认
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            break;
        case COMPO_ID_BTN_BACK://取消
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            break;
        default:
            break;
    }
//    f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
//    int id = compo_get_button_id();
//    compo_textbox_t *txt_number = compo_getobj_byid(COMPO_ID_TXT_NUM);
//    compo_button_t *img_btn = compo_getobj_byid(COMPO_ID_OPEN_BTN);
//    char buf[30];

//    switch (id)
//    {
//        case COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM9:
//            if(f_sos->input_mode_flag == true)
//            {
//                if(f_sos->phone_number_cnt < 11)
//                {
//                    sys_cb.sos_call_number[f_sos->phone_number_cnt++] = id+0x30 - 1;
//                }
//            }
//            break;
//        case COMPO_ID_BTN_BACK:///删除
//            if(f_sos->phone_number_cnt > 0)
//            {
//                sys_cb.sos_call_number[--f_sos->phone_number_cnt] = '\0';
//            }
//            break;
//        case COMPO_ID_BTN_SURE:///确认
//            input_mode(false);
//            f_sos->input_mode_flag = false;
//            break;
//        case COMPO_ID_INPUT_BOX:///点击输入框
//            input_mode(true);
//            f_sos->input_mode_flag = true;
//            break;
//        case COMPO_ID_OPEN_BTN:
//            sys_cb.sos_open_flag ^=1;
//            if(sys_cb.sos_open_flag == true)compo_button_set_bgimg(img_btn, UI_BUF_COMMON_ON_BIN);
//            else compo_button_set_bgimg(img_btn, UI_BUF_COMMON_OFF_BIN);
//            compo_button_set_location(img_btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y/4 );
//            break;
//        default:
//            break;
//    }

//    if(f_sos->input_mode_flag == true)
//    {
//        snprintf(buf, sizeof(buf), "%s", sys_cb.sos_call_number);
//        compo_textbox_set(txt_number, buf);
//    }

//    printf("number:%s  touch:%d  length:%d\n",sys_cb.sos_call_number, f_sos->input_mode_flag ,f_sos->phone_number_cnt);
}

///SOS功能消息处理
static void func_set_sub_sos_message(size_msg_t msg)
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
static void func_set_sub_sos_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sos_t));
//    f_sos_t *f_sos = (f_sos_t *)func_cb.f_cb;
    func_cb.frm_main = func_set_sub_sos_form_create();
//    input_mode(false);
//    f_sos->phone_number_cnt = strlen(sys_cb.sos_call_number);
}

///退出SOS功能
static void func_set_sub_sos_exit(void)
{
    func_cb.last = FUNC_SET_SUB_SOS;
}

///SOS功能
void func_set_sub_sos(void)
{
    printf("%s\n", __func__);
    func_set_sub_sos_enter();
    while (func_cb.sta == FUNC_SET_SUB_SOS)
    {
        func_set_sub_sos_process();
        func_set_sub_sos_message(msg_dequeue());
    }
    func_set_sub_sos_exit();
}
#endif