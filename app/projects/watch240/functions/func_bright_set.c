#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_BRIGHT_SET_SUPPORT
enum
{
    COMPO_ID_LISTBOX = 1,
};
typedef struct f_bright_set_list_t_
{
    compo_listbox_t *listbox;

} f_bright_set_list_t;  //

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
#define BRIGHT_SET_LIST_CNT                       ((int)(sizeof(tbl_bright_set_list) / sizeof(tbl_bright_set_list[0])))

static const compo_listbox_item_t tbl_bright_set_list[] =
{
    {STR_SETTING_LIGHT,      0,        .func_sta = FUNC_LIGHT}, //亮度
    {STR_SETTING_DOUSING,    0,        .func_sta = FUNC_SET_SUB_DOUSING}, //亮屏时长
};

//亮度设置页面
compo_form_t *func_bright_set_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
    compo_listbox_set(listbox, tbl_bright_set_list, BRIGHT_SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I335001_3_EXERCISE_LIST_BIN);


    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus(listbox, 102);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
void func_bright_set_icon_click(void)
{
    int icon_idx;
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_bright_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= BRIGHT_SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_bright_set_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
#define BRIGHT_SET_LIST_CNT                       ((int)(sizeof(tbl_bright_set_list) / sizeof(tbl_bright_set_list[0])))

static const compo_listbox_item_t tbl_bright_set_list[] =
{
    {STR_SETTING_LIGHT,      0,        .func_sta = FUNC_LIGHT}, //亮度
    {STR_SETTING_DOUSING,    0,        .func_sta = FUNC_SET_SUB_DOUSING}, //亮屏时长
};

//亮度设置页面
compo_form_t *func_bright_set_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_bright_set_list, BRIGHT_SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I338001_11_CALL_BG_BIN);


    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus(listbox, 102);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
void func_bright_set_icon_click(void)
{
    int icon_idx;
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_bright_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= BRIGHT_SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_bright_set_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}
#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

#define BRIGHT_SET_LIST_CNT                       ((int)(sizeof(tbl_bright_set_list) / sizeof(tbl_bright_set_list[0])))

static const compo_listbox_item_t tbl_bright_set_list[] =
{
    {STR_SETTING_LIGHT,      0,        .func_sta = FUNC_LIGHT}, //亮度
    {STR_SETTING_DOUSING,    0,        .func_sta = FUNC_SET_SUB_DOUSING}, //亮屏时长
};

//亮度设置页面
compo_form_t *func_bright_set_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_bright_set_list, BRIGHT_SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I342001_28_SET_BG_BIN);


    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus(listbox, 90);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
void func_bright_set_icon_click(void)
{
    int icon_idx;
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_bright_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= BRIGHT_SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_bright_set_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}

#else
//亮度设置页面
compo_form_t *func_bright_set_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
void func_bright_set_icon_click(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//亮度设置能消息处理
static void func_bright_set_list_message(size_msg_t msg)
{
// #if (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT)
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_bright_set->listbox;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_bright_set_icon_click();                //单击图标
            break;


        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
// #endif
}

//亮度设置事件处理
static void func_bright_set_process(void)
{
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
    if(f_bright_set->listbox != NULL)
    {
        compo_listbox_move(f_bright_set->listbox);
    }

    func_process();
}

//进入表盘&主题功能
static void func_bright_set_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_bright_set_list_t));
    func_cb.frm_main = func_bright_set_form_create();

    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
    f_bright_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_bright_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init(listbox);

    func_cb.enter_tick = tick_get();
}

//退出亮度设置功能
static void func_bright_set_exit(void)
{
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_bright_set->listbox;
    func_free(listbox->mcb);
    func_cb.last = FUNC_BRIGHT_SET;
}

//亮度设置功能
void func_bright_set(void)
{
    printf("%s\n", __func__);
    func_bright_set_enter();
    while (func_cb.sta == FUNC_BRIGHT_SET)
    {
        func_bright_set_process();
        func_bright_set_list_message(msg_dequeue());
    }
    func_bright_set_exit();
}

#endif
