#include "include.h"
#include "func.h"
#include "func_menu.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SETTING_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define SET_LIST_CNT                       ((int)(sizeof(tbl_menu_football_list) / sizeof(tbl_menu_football_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_menu_football_list_t_
{
    compo_listbox_t *listbox;

} f_menu_football_list_t;

static const compo_listbox_item_t tbl_menu_football_list[] =
{
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                  UI_BUF_I330001_THEME_1_THEME_BIN,             .func_sta = FUNC_STYLE},                //风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {STR_FLASHLIGHT,             UI_BUF_I330001_THEME_1_FLASHLIGHT_BIN,        .func_sta = FUNC_FLASHLIGHT},
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {STR_WOMEN_HEALTH,           UI_BUF_I330001_THEME_1_PERIOD_BIN,            .func_sta = FUNC_WOMEN_HEALTH},        //女性健康
};

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_football_list_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_SETTING]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_menu_football_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_menu_football_list_icon_click(void)
{
    int icon_idx;
    f_menu_football_list_t *f_set = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_menu_football_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}

//切换到设置菜单页面
static void func_menu_football_list_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_menu_football_list_t *f_set = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    // sys_cb.set_idx = 0;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define SET_LIST_CNT                       ((int)(sizeof(tbl_menu_football_list) / sizeof(tbl_menu_football_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_menu_football_list_t_
{
    compo_listbox_t *listbox;

} f_menu_football_list_t;

static const compo_listbox_item_t tbl_menu_football_list[] =
{
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                  UI_BUF_I332001_THEME_ICON1_THEME_BIN,             .func_sta = FUNC_STYLE},                //风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {STR_FLASHLIGHT,             UI_BUF_I332001_THEME_ICON1_FLASHLIGHT_BIN,        .func_sta = FUNC_FLASHLIGHT},
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {STR_WOMEN_HEALTH,           UI_BUF_I332001_THEME_ICON1_PERIOD_BIN,            .func_sta = FUNC_WOMEN_HEALTH},        //女性健康
};

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_football_list_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_SETTING]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_menu_football_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_menu_football_list_icon_click(void)
{
    int icon_idx;
    f_menu_football_list_t *f_set = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_menu_football_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}

//切换到设置菜单页面
static void func_menu_football_list_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_menu_football_list_t *f_set = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    // sys_cb.set_idx = 0;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT

#define SET_LIST_CNT                       ((int)(sizeof(tbl_menu_football_list) / sizeof(tbl_menu_football_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_menu_football_list_t_
{
    compo_listbox_t *listbox;

} f_menu_football_list_t;

static const compo_listbox_item_t tbl_menu_football_list[] =
{
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    {STR_STYLE,                  UI_BUF_I340001_THEME_ICON1_THEME_BIN,             .func_sta = FUNC_STYLE},                //风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {STR_FLASHLIGHT,             UI_BUF_I340001_THEME_ICON1_FLASHLIGHT_BIN,        .func_sta = FUNC_FLASHLIGHT},
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    {STR_WOMEN_HEALTH,           UI_BUF_I340001_THEME_ICON1_PERIOD_BIN,            .func_sta = FUNC_WOMEN_HEALTH},        //女性健康
};

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_football_list_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_SETTING]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_menu_football_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I340001_FIRSTORDER_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_menu_football_list_icon_click(void)
{
    int icon_idx;
    f_menu_football_list_t *f_set = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_menu_football_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
    }
}

//切换到设置菜单页面
static void func_menu_football_list_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_menu_football_list_t *f_set = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    // sys_cb.set_idx = 0;
}

#else
#define SET_LIST_CNT                       ((int)(sizeof(tbl_menu_football_list) / sizeof(tbl_menu_football_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_menu_football_list_t_
{
    compo_listbox_t *listbox;

} f_menu_football_list_t;

static const compo_listbox_item_t tbl_menu_football_list[] =
{
    {0},
};
compo_form_t *func_menu_football_list_form_create(void)
{
}
static void func_menu_football_list_icon_click(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//主菜单功能事件处理
static void func_menu_football_list_process(void)
{
    f_menu_football_list_t *f_set = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_move(f_set->listbox);
    func_process();
}

//进入设置主菜单页面
static void func_menu_football_list_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_menu_football_list_t));
    func_cb.frm_main = func_menu_football_list_form_create();

    f_menu_football_list_t *f_menu_football_list = (f_menu_football_list_t *)func_cb.f_cb;
    f_menu_football_list->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_menu_football_list->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    // compo_listbox_move_init(listbox);
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
}

//主菜单功能消息处理
static void func_menu_football_list_message(size_msg_t msg)
{
    f_menu_football_list_t *f_menu_football_list = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_menu_football_list->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_menu_football_list_icon_click();                //单击图标
            break;
        default:
            func_message(msg);
            break;
    }
}

//退出菜单样式
void func_menu_football_list_exit(void)
{
//    printf("func_menu_football_list_exit\r\n");
    f_menu_football_list_t *f_menu_football_list = (f_menu_football_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_menu_football_list->listbox;
    func_free(listbox->mcb);                                            //释放移动控制块
    func_cb.last = FUNC_MENU_STYLE_FOOTBALL_LIST;
}

//主菜单功能
void func_menu_football_list(void)
{
//    printf("pic w:%d h:%d\n",gui_image_get_size(tbl_setting_list->res_addr).wid,gui_image_get_size(tbl_setting_list->res_addr).hei);
    func_menu_football_list_enter();
    while (func_cb.sta == FUNC_MENU_STYLE_FOOTBALL_LIST && func_cb.menu_style == MENU_STYLE_FOOTBALL)
    {
        func_menu_football_list_process();
        func_menu_football_list_message(msg_dequeue());
    }
    func_menu_football_list_exit();
}
#endif
