#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_CALL_SUPPORT
enum
{
    COMPO_ID_LISTBOX = 1,
    COMPO_ID_PIC_OPEN,
    COMPO_ID_PIC_CLOSE,
    COMPO_ID_TXT_BT_NOT_ENABLED,//BT未开启文字
};
typedef struct f_call_list_t_
{
    compo_listbox_t *listbox;

} f_call_list_t;

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
#define CALL_LIST_CNT                       ((int)(sizeof(tbl_call_list) / sizeof(tbl_call_list[0])))

static const compo_listbox_item_t tbl_call_list[] =
{
    {STR_CALL_LINK,        UI_BUF_I335001_CALL_00_ICON_36X40_X24_Y64_Y140_Y216_00_BIN,              .func_sta = FUNC_ADDRESS_BOOK},  //联系人
    {STR_CALL_RECENT,      UI_BUF_I335001_CALL_00_ICON_36X40_X24_Y64_Y140_Y216_01_BIN,             .func_sta = FUNC_CALL_SUB_RECORD},    //通话记录
    {STR_CALL_DIAL,        UI_BUF_I335001_CALL_00_ICON_36X40_X24_Y64_Y140_Y216_02_BIN,              .func_sta = FUNC_CALL_SUB_DIAL},     //拨号盘
};



//电话页面
compo_form_t *func_call_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);
    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
    compo_listbox_set(listbox, tbl_call_list, CALL_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I335001_CALL_00_ICON_BG_224X68_X8_Y48_Y126_Y202_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    // compo_listbox_set_sta_icon(listbox, UI_BUF_I335001_CALL_NEXT_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    // compo_listbox_set_bithook(listbox, true);

    compo_listbox_set_focus(listbox, 102);
    compo_listbox_update(listbox);
    // compo_listbox_set_visible(listbox,false);//默认隐藏

    // //取消按钮
    // compo_picturebox_t *pic = compo_picturebox_create(frm, UI_BUF_I335001_23_SOS_2_WHETHER_TO_DIAL_ICON_BUTTON_102X52_X16_Y222_BIN);
    // compo_picturebox_set_pos(pic, 16+102/2, 222+52/2);
    // compo_picturebox_set_visible(pic, true);
    // compo_setid(pic, COMPO_ID_PIC_CLOSE);

    // //确定按钮
    // pic = compo_picturebox_create(frm, UI_BUF_I335001_23_SOS_2_WHETHER_TO_DIAL_ICON_BUTTON_102X52_X122_Y220_BIN);
    // compo_picturebox_set_pos(pic, 122+102/2, 222+52/2);
    // compo_picturebox_set_visible(pic, true);
    // compo_setid(pic, COMPO_ID_PIC_OPEN);

    // compo_textbox_t* txt = compo_textbox_create(frm, strlen(i18n[STR_CALL_CLOSED_IS_OPEN]));
    // compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH / 1.2, 0);
    // compo_textbox_set_visible(txt, true);
    // compo_textbox_set(txt, i18n[STR_CALL_CLOSED_IS_OPEN]);
    // compo_textbox_set_multiline(txt,true);
    // compo_setid(txt, COMPO_ID_TXT_BT_NOT_ENABLED);

    // if(uteModuleCallBtIsPowerOn())
    // {
    //     compo_listbox_set_focus(listbox, 102);
    //     compo_listbox_update(listbox);
    //     compo_listbox_set_visible(listbox,true);
    //     pic = compo_getobj_byid(COMPO_ID_PIC_CLOSE);
    //     compo_picturebox_set_visible(pic, false);
    //     pic = compo_getobj_byid(COMPO_ID_PIC_OPEN);
    //     compo_picturebox_set_visible(pic, false);
    //     txt = compo_getobj_byid(COMPO_ID_TXT_BT_NOT_ENABLED);
    //     compo_textbox_set_multiline(txt,false);
    // }
    return frm;
}

//点进图标进入应用
void func_call_icon_click(void)
{
    int icon_idx;
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;
    u8 func_sta;
    int id = compo_get_button_id();

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= CALL_LIST_CNT)
    {
        return;
    }
    // if(!uteModuleCallBtIsPowerOn())
    // {
    //     switch (id)
    //     {
    //     case COMPO_ID_PIC_CLOSE:
    //         break;
    //     case COMPO_ID_PIC_OPEN:
    //         break;
    //     default:
    //         break;
    //     }
    // }
    // else
    {
        //根据图标索引获取应用ID
        func_sta = tbl_call_list[icon_idx].func_sta;
        //切入应用
        if (func_sta > 0)
        {
            func_cb.sta = func_sta;
        }
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
#define CALL_LIST_CNT                       ((int)(sizeof(tbl_call_list) / sizeof(tbl_call_list[0])))

static const compo_listbox_item_t tbl_call_list[] =
{
    {STR_CALL_LINK,        UI_BUF_I338001_11_CALL_ICON_CONTACT_PERSON_BIN,              .func_sta = FUNC_ADDRESS_BOOK},  //联系人
    {STR_CALL_RECENT,      UI_BUF_I338001_11_CALL_ICON_CALL_LOG_BIN,                    .func_sta = FUNC_CALL_SUB_RECORD},    //通话记录
    {STR_CALL_DIAL,        UI_BUF_I338001_11_CALL_ICON_DIAL_PLATE_BIN,                  .func_sta = FUNC_CALL_SUB_DIAL},     //拨号盘
};



//电话页面
compo_form_t *func_call_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);
    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_call_list, CALL_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I338001_11_CALL_NEXT_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_focus(listbox, 102);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
void func_call_icon_click(void)
{
    int icon_idx;
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;
    u8 func_sta;
    int id = compo_get_button_id();

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= CALL_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_call_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }

}
#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define CALL_LIST_CNT                       ((int)(sizeof(tbl_call_list) / sizeof(tbl_call_list[0])))

static const compo_listbox_item_t tbl_call_list[] =
{
    {STR_CALL_LINK,        UI_BUF_I330001_CALL_CONTACTS_BIN,                .func_sta = FUNC_CALL_SUB_LINKMAN},
    {STR_CALL_RECENT,      UI_BUF_I330001_CALL_CALLRECORDS_BIN,             .func_sta = FUNC_CALL_SUB_RECORD},
    {STR_CALL_DIAL,        UI_BUF_I330001_CALL_DIALPAD_BIN,                 .func_sta = FUNC_CALL_SUB_DIAL},
};



//电话页面
compo_form_t *func_call_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_call_list, CALL_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus(listbox, 102);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
void func_call_icon_click(void)
{
    int icon_idx;
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= CALL_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_call_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define CALL_LIST_CNT                       ((int)(sizeof(tbl_call_list) / sizeof(tbl_call_list[0])))

static const compo_listbox_item_t tbl_call_list[] =
{
    {STR_CALL_LINK,        UI_BUF_I332001_CALL_CALL_LIST_CONTACTS_BIN,                .func_sta = FUNC_CALL_SUB_LINKMAN},
    {STR_CALL_RECENT,      UI_BUF_I332001_CALL_CALL_LIST_CALLRECORDS_BIN,             .func_sta = FUNC_CALL_SUB_RECORD},
    {STR_CALL_DIAL,        UI_BUF_I332001_CALL_CALL_LIST_DIALPAD_BIN,                 .func_sta = FUNC_CALL_SUB_DIAL},
};
//电话页面
compo_form_t *func_call_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);

    // 新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_call_list, CALL_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus(listbox, 102);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
void func_call_icon_click(void)
{
    int icon_idx;
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= CALL_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_call_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT

#define CALL_LIST_CNT                       ((int)(sizeof(tbl_call_list) / sizeof(tbl_call_list[0])))

static const compo_listbox_item_t tbl_call_list[] =
{
    {STR_CALL_LINK,        UI_BUF_I340001_CALL_CALL_LIST_CONTACTS_BIN,                .func_sta = FUNC_CALL_SUB_LINKMAN},
    {STR_CALL_RECENT,      UI_BUF_I340001_CALL_CALL_LIST_CALLRECORDS_BIN,             .func_sta = FUNC_CALL_SUB_RECORD},
    {STR_CALL_DIAL,        UI_BUF_I340001_CALL_CALL_LIST_DIALPAD_BIN,                 .func_sta = FUNC_CALL_SUB_DIAL},
};
//电话页面
compo_form_t *func_call_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);

    // 新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_call_list, CALL_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I340001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus(listbox, 102);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
void func_call_icon_click(void)
{
    int icon_idx;
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= CALL_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_call_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

#define CALL_LIST_CNT                       ((int)(sizeof(tbl_call_list) / sizeof(tbl_call_list[0])))

static const compo_listbox_item_t tbl_call_list[] =
{
    {STR_CALL_LINK,        UI_BUF_I342001_11_CALL_ICON_CONTACT_PERSON_BIN,              .func_sta = FUNC_ADDRESS_BOOK},  //联系人
    {STR_CALL_RECENT,      UI_BUF_I342001_11_CALL_ICON_CALL_LOG_BIN,                    .func_sta = FUNC_CALL_SUB_RECORD},    //通话记录
    {STR_CALL_DIAL,        UI_BUF_I342001_11_CALL_ICON_DIAL_PLATE_BIN,                  .func_sta = FUNC_CALL_SUB_DIAL},     //拨号盘
};



//电话页面
compo_form_t *func_call_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_PHONE]);
    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_MENU_CIRCLE);
    compo_listbox_set(listbox, tbl_call_list, CALL_LIST_CNT);
    compo_listbox_set_item_height(listbox, 50);
//    compo_listbox_set_bgimg(listbox, UI_BUF_I342001_11_CALL_BG_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_focus(listbox, 75);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
void func_call_icon_click(void)
{
    int icon_idx;
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;
    u8 func_sta;
    int id = compo_get_button_id();

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= CALL_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_call_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }

}

#else
compo_form_t *func_call_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
//点进图标进入应用
void func_call_icon_click(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//电话功能消息处理
static void func_call_list_message(size_msg_t msg)
{
// #if (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT)
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;

#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    if(f_call->listbox != NULL)
    {
        if (compo_listbox_message(listbox, msg))
        {
            return;                                         //处理列表框信息
        }
    }

#endif
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_call_icon_click();                //单击图标
            break;


        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
// #endif
}

//电话事件处理
static void func_call_process(void)
{
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    if(f_call->listbox != NULL)
    {
        compo_listbox_move(f_call->listbox);
    }

    func_process();
}

//进入电话功能
static void func_call_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_call_list_t));
    func_cb.frm_main = func_call_form_create();

    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    f_call->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_call->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init(listbox);

    func_cb.enter_tick = tick_get();
}

//退出电话功能
static void func_call_exit(void)
{
    f_call_list_t *f_call = (f_call_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_call->listbox;
    func_free(listbox->mcb);
    func_cb.last = FUNC_CALL;
}

//电话功能
void func_call(void)
{
    printf("%s\n", __func__);
    func_call_enter();
    while (func_cb.sta == FUNC_CALL)
    {
        func_call_process();
        func_call_list_message(msg_dequeue());
    }
    func_call_exit();
}

#endif
