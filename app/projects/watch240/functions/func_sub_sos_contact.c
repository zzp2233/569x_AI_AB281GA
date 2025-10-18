#include "include.h"
#include "func.h"
#include "ute_module_call.h"
#include "ute_module_charencode.h"
#include "func_cover.h"
#include "ute_drv_motor.h"

#if UTE_MODUEL_CALL_SOS_CONTACT_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

enum
{
    COMPO_ID_LISTBOX = 1,

    COMPO_ID_COVER_PIC,
    COMPO_ID_COVER_TXT,
    COMPO_ID_TIME,
    COMPO_ID_TILTE,
    TEXT_ID_DISCONNECT,
    PIC_ID_DISCONNECT,
};

typedef struct f_sos_book_list_t_
{
    compo_listbox_t *listbox;
    u32 tick;
} f_sos_book_list_t;


#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
static const compo_listbox_item_t tbl_call_list[UTE_MODULE_CALL_ADDRESSBOOK_MAX_COUNT] = {0};       //list列表
static ute_module_call_addressbook_t* address_book_tbl = NULL;            //电话簿数据
static u16 address_book_cnt = 0;                                       //联系人个数

//更新电话簿列表回调函数
/*static bool address_book_update_callback2(u32 item_cnt, char* str_txt1, u16 str_txt1_len, char* str_txt2, u16 str_txt2_len, u16 index)
{

    if (index < item_cnt && index < address_book_cnt)
    {
        //需要使用静态数组作为中间变量，以防占用堆栈内存
        //中间缓存大小要比实际获取的名字要大
        //方便再接受到的名字过长时，方便后面使用uteModuleCharencodeGetUtf8String转换的时候可以自动加入省略号
        static char name_utf8[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH+5] = {0};
        uint16_t name_utf8_len = 0;

        static char name_utf8_l[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH+10] = {0};
        uint16_t name_utf8_len_l = 0;

        memset(name_utf8, 0, sizeof(name_utf8));
        memset(name_utf8_l, 0, sizeof(name_utf8_l));

        if (str_txt1_len > sizeof(name_utf8_l))
        {
            str_txt1_len = sizeof(name_utf8_l);
        }

        if (str_txt2_len > address_book_tbl[index].numberAsciiLen)
        {
            str_txt2_len = address_book_tbl[index].numberAsciiLen;
        }

        uteModuleCharencodeUnicodeConversionUtf8(address_book_tbl[index].nameUnicode,
                address_book_tbl[index].nameUnicodeLen,
                (uint8_t*)name_utf8,
                &name_utf8_len,
                sizeof(name_utf8));

        if (name_utf8_len >= UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH)
        {

            uteModuleCharencodeGetUtf8String((uint8_t*)name_utf8, name_utf8_len, (uint8_t*)name_utf8_l, &name_utf8_len_l);
            memcpy(str_txt1, name_utf8_l, str_txt1_len);
        }
        else
        {
            memcpy(name_utf8_l, name_utf8, sizeof(name_utf8));
            memcpy(str_txt1, name_utf8_l, str_txt1_len);
        }

        if (str_txt2_len > 14)
        {
            memcpy(str_txt2, address_book_tbl[index].numberAscii, 12);
            memcpy(str_txt2 + 12, "...", 3);
            str_txt2_len = 15;
        }
        else
        {
            memcpy(str_txt2, address_book_tbl[index].numberAscii, str_txt2_len);
        }
        return true;
    }
    return false;
}*/

static bool address_book_update_callback(u32 item_cnt, char* str_txt, u16 index)
{
    if (index < item_cnt && index < address_book_cnt)
    {
        //需要使用静态数组作为中间变量，以防占用堆栈内存
        //中间缓存大小要比实际获取的名字要大
        //方便再接受到的名字过长时，方便后面使用uteModuleCharencodeGetUtf8String转换的时候可以自动加入省略号
        if (address_book_tbl[index].nameUnicode > 0)
        {
            static char name_utf8[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH+5] = {0};
            uint16_t name_utf8_len = 0;

            static char name_utf8_l[UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH+10] = {0};
            uint16_t name_utf8_len_l = 0;

            memset(name_utf8, 0, sizeof(name_utf8));
            memset(name_utf8_l, 0, sizeof(name_utf8_l));

            uteModuleCharencodeUnicodeConversionUtf8(address_book_tbl[index].nameUnicode,
                    address_book_tbl[index].nameUnicodeLen,
                    (uint8_t*)name_utf8,
                    &name_utf8_len,
                    sizeof(name_utf8));
            //printf("name=%s",name_utf8);
            if (name_utf8_len >= UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH)
            {
                uteModuleCharencodeGetUtf8String((uint8_t*)name_utf8, name_utf8_len, (uint8_t*)name_utf8_l, &name_utf8_len_l);
                memcpy(str_txt, name_utf8_l, name_utf8_len_l);
                memcpy(str_txt + name_utf8_len_l, "...", 3);
            }
            else
            {
                memcpy(str_txt, name_utf8, sizeof(name_utf8));
                if (address_book_tbl[index].nameUnicodeLen >= UTE_MODULE_CALL_ADDRESSBOOK_NAME_MAX_LENGTH - 1)
                {
                    memcpy(str_txt + name_utf8_len, "...", 3);
                }
            }
        }
        else
        {
            if (address_book_tbl[index].numberAsciiLen > 14)
            {
                memcpy(str_txt, address_book_tbl[index].numberAscii, 12);
                memcpy(str_txt + 12, "...", 3);
            }
            else
            {
                memcpy(str_txt, address_book_tbl[index].numberAscii, address_book_tbl[index].numberAsciiLen);
            }
        }
        return true;
    }
    return false;
}

static u8 func_sos_book_update(void)
{
    //获取通讯录联系人
    if (address_book_cnt != uteModuleCallGetSosContactSize())
    {
        address_book_cnt = uteModuleCallGetSosContactSize();
        printf("address_book_cnt:%d\n", address_book_cnt);
        if (address_book_tbl == NULL)
        {
            address_book_tbl = ab_zalloc(sizeof(ute_module_call_addressbook_t) * UTE_MODUEL_CALL_SOS_CONTACT_MAX);
        }
        if (address_book_tbl != NULL)
        {
            uteModuleCallGetSosContact(address_book_tbl);
            //uteModuleCallGetAllAddressBookContactContent(address_book_cnt, address_book_tbl);
            return true;
        }
    }

    return false;
}

//创建电话簿窗体
compo_form_t *func_sos_book_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SOS_CONT]);
    compo_setid(frm->time, COMPO_ID_TIME);
    compo_setid(frm->title, COMPO_ID_TILTE);

    //创建无消息界面
    compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_PHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 107+124/2);
    compo_picturebox_set_visible(pic, false);
    compo_setid(pic, COMPO_ID_COVER_PIC);

    compo_picturebox_t *picDis = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_BLUETOOTH_BIN);
    compo_picturebox_set_pos(picDis, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 20);
    compo_picturebox_set_visible(picDis, false);
    compo_setid(picDis, PIC_ID_DISCONNECT);

    compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_ADDRESS_BOOK_SYNC]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*3/4, 320, 116);
    compo_textbox_set_visible(txt, false);
    compo_textbox_set_multiline(txt, true);
    widget_text_set_ellipsis(txt->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(txt, i18n[STR_ADDRESS_BOOK_SYNC]);
    compo_setid(txt, COMPO_ID_COVER_TXT);

    compo_textbox_t *txtDis = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
    compo_textbox_set_location(txtDis, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*4/5, 348, widget_text_get_max_height());
    compo_textbox_set_visible(txtDis, false);
    compo_textbox_set(txtDis, i18n[STR_CONNECT_BLUETOOTH]);
    compo_setid(txtDis, TEXT_ID_DISCONNECT);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    //更新联系人
    func_sos_book_update();
    if (address_book_cnt)
    {
        compo_listbox_set(listbox, tbl_call_list, (address_book_cnt < 2) ? 2 : address_book_cnt);
    }
    compo_listbox_set_alike_icon(listbox, UI_BUF_I341001_11_CALL_CONTACTS_BIN);
//    compo_listbox_set_text_modify_by_idx_callback2(listbox, address_book_update_callback2);       //双行显示名字加号码
    compo_listbox_set_text_modify_by_idx_callback(listbox, address_book_update_callback);
    compo_listbox_set_bgimg(listbox, UI_BUF_I341001_28_SET_BG1_BIN);
    compo_listbox_set_focus(listbox, 160);
    compo_listbox_set_item_icon_pos(listbox, gui_image_get_size(UI_BUF_I341001_11_CALL_CONTACTS_BIN).wid/2, listbox->line_center_y);
    compo_listbox_update_with_text_scroll_rst(listbox);
    compo_listbox_update(listbox);
    compo_listbox_set_visible(listbox, false);

    if (uteModuleCallBtIsConnected())
    {
        compo_textbox_set_visible(txt, address_book_cnt > 0 ? false : true);
        compo_picturebox_set_visible(pic, address_book_cnt > 0 ? false : true);
        compo_textbox_set_visible(frm->time, address_book_cnt > 0 ? true : false);
        compo_textbox_set_visible(frm->title, address_book_cnt > 0 ? true : false);
        compo_listbox_set_visible(listbox, true);
    }
    else
    {
        compo_textbox_set_visible(txtDis, true);
        compo_picturebox_set_visible(picDis, true);
        compo_listbox_set_visible(listbox, false);
        compo_textbox_set_visible(frm->time, false);
        compo_textbox_set_visible(frm->title, false);
    }

    return frm;
}

//点进图标进入应用
void func_sos_book_icon_click(void)
{
    int icon_idx;
    //u8 i = 0;
    f_sos_book_list_t *f_book = (f_sos_book_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_book->listbox;
    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= address_book_cnt)
    {
        return;
    }

    if(!uteModuleCallBtIsConnected())
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
        msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
    }
    else
    {
        uteTaskGuiStartScreen(FUNC_SUB_SOS, 0, __func__);
//        if (bt_is_connected())
//        {
//            memset(sys_cb.outgoing_number, 0, 16);
//            for (i = 0; i < 16; i++)
//            {
//                sys_cb.outgoing_number[i] = address_book_tbl[icon_idx].numberAscii[i];
//            }
//            bt_call_redial_number();
//        }
    }
}

#else
compo_form_t *func_sos_book_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
#endif

//电话簿功能事件处理
static void func_sos_book_process(void)
{
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_sos_book_list_t *f_book = (f_sos_book_list_t *)func_cb.f_cb;

    //两秒更新一次
    if (tick_check_expire(f_book->tick, 1000))
    {
        f_book->tick = tick_get();

        compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_COVER_TXT);
        compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_COVER_PIC);
        compo_textbox_t* txtDis = compo_getobj_byid(TEXT_ID_DISCONNECT);
        compo_picturebox_t* picDis = compo_getobj_byid(PIC_ID_DISCONNECT);
        compo_textbox_t* txtTime = compo_getobj_byid(COMPO_ID_TIME);
        compo_textbox_t* txtTitle = compo_getobj_byid(COMPO_ID_TILTE);
        if (uteModuleCallBtIsConnected())
        {
            //更新联系人
            u8 sta = func_sos_book_update();

            compo_textbox_set_visible(txtDis, false);
            compo_picturebox_set_visible(picDis, false);
            compo_textbox_set_visible(txt, address_book_cnt > 0 ? false : true);
            compo_picturebox_set_visible(pic, address_book_cnt > 0 ? false : true);
            compo_textbox_set_visible(txtTime, address_book_cnt > 0 ? true : false);
            compo_textbox_set_visible(txtTitle, address_book_cnt > 0 ? true : false);
            compo_listbox_set_visible(f_book->listbox, true);
            if (sta)
            {
                if (address_book_cnt)
                {
                    compo_listbox_set_visible(f_book->listbox, true);
                    compo_listbox_set(f_book->listbox, tbl_call_list, (address_book_cnt < 2) ? 2 : address_book_cnt);
                    compo_listbox_set_alike_icon(f_book->listbox, UI_BUF_I341001_11_CALL_CONTACTS_BIN);
                    compo_listbox_move_init_modify(f_book->listbox, 127-30, compo_listbox_gety_byidx(f_book->listbox, (address_book_cnt - 2 > 0) ? address_book_cnt - 2 : 1));
                    compo_listbox_update_with_text_scroll_rst(f_book->listbox);
                    compo_listbox_set_bgimg(f_book->listbox, UI_BUF_I341001_28_SET_BG1_BIN);
                    compo_listbox_set_focus_byidx(f_book->listbox, 1);
                    compo_listbox_update(f_book->listbox);
                }
                else
                {
                    compo_listbox_set_visible(f_book->listbox, false);
                    compo_listbox_set(f_book->listbox, tbl_call_list, address_book_cnt);
                }
            }
        }
        else
        {
            compo_textbox_set_visible(txtDis, true);
            compo_picturebox_set_visible(picDis, true);
            compo_textbox_set_visible(txt, false);
            compo_picturebox_set_visible(pic, false);
            compo_listbox_set_visible(f_book->listbox, false);
            compo_textbox_set_visible(txtTime, false);
            compo_textbox_set_visible(txtTitle, false);
        }
    }

    compo_listbox_move(f_book->listbox);
#endif
    func_process();
}

//电话簿功能消息处理
static void func_sos_book_message(size_msg_t msg)
{
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_sos_book_list_t *f_book = (f_sos_book_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_book->listbox;

    if (address_book_cnt > 3)
    {
        if (compo_listbox_message(listbox, msg))
        {
            return;                                         //处理列表框信息
        }
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            if (address_book_cnt)
            {
                func_sos_book_icon_click();
            }
            break;

        case MSG_CTP_LONG:
            break;

//        case KU_DELAY_BACK:
//            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
//            {
//
//            }
//            break;

        case MSG_CTP_SHORT_RIGHT:
            if (func_cb.flag_sort)
            {
                func_backing_to();
            }
            else
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_LEFT:
            if (func_cb.flag_sort)
            {

            }
            else
            {
                func_message(msg);
            }
            break;

        default:
            func_message(msg);
            break;
    }
#else
    switch (msg)
    {
        default:
            func_message(msg);
            break;
    }
#endif
}


//进入电话簿功能
static void func_sos_book_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sos_book_list_t));
    func_cb.frm_main = func_sos_book_form_create();

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_sos_book_list_t *f_book = (f_sos_book_list_t *)func_cb.f_cb;
    f_book->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_book->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    //listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    //compo_listbox_move_init_modify(f_book->listbox, 160, compo_listbox_gety_byidx(f_book->listbox, (address_book_cnt - 2 > 0) ? address_book_cnt - 2 : 1));
    func_cb.enter_tick = tick_get();
#endif
}


//退出电话簿功能
static void func_sos_book_exit(void)
{
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    //f_sos_book_list_t *f_book = (f_sos_book_list_t *)func_cb.f_cb;
    //compo_listbox_t *listbox = f_book->listbox;

    if (address_book_tbl != NULL)
    {
        ab_free(address_book_tbl);
        address_book_tbl = NULL;
    }
    address_book_cnt = 0;
    //func_free(listbox->mcb);
#endif
    func_cb.last = FUNC_SOS_BOOK;
}

//电话簿功能
void func_sos_book(void)
{
    printf("%s\n", __func__);
    func_sos_book_enter();
    while (func_cb.sta == FUNC_SOS_BOOK)
    {
        func_sos_book_process();
        func_sos_book_message(msg_dequeue());
    }
    func_sos_book_exit();
}
#endif
