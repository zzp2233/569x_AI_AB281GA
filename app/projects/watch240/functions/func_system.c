#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SYSTEM_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

#define SYSTEM_LIST_CNT                       ((int)(sizeof(tbl_system_list) / sizeof(tbl_system_list[0])))

enum
{
    COMPO_ID_SYSTEM = 1,
};

typedef struct f_set_list_t_
{
    compo_listbox_t *listbox;

} f_set_list_t;

static const compo_listbox_item_t tbl_system_list[] =
{
    {STR_SETTING_RESTART,              UI_BUF_I335001_27_MORE_28_SET_9_SYSTEM_MENU_1_SYSTEM_SETTINGS_ICON_PIC32X33_X16_Y63_137_209_REBOOT_BIN,       .func_sta = FUNC_RESTART},    //重启
    {STR_SETTING_OFF,              UI_BUF_I335001_27_MORE_28_SET_9_SYSTEM_MENU_1_SYSTEM_SETTINGS_ICON_PIC32X33_X16_Y63_137_209_OFF_BIN,          .func_sta = FUNC_OFF},        //关机
    {STR_SETTING_RSTFY,              UI_BUF_I335001_27_MORE_28_SET_9_SYSTEM_MENU_1_SYSTEM_SETTINGS_ICON_PIC32X33_X16_Y63_137_209_REST_BIN,         .func_sta = FUNC_RSTFY},    //恢复出厂
};

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_system_sub_system_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SYSTEM_SET]);

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_system_list, SYSTEM_LIST_CNT);
    compo_setid(listbox, COMPO_ID_SYSTEM);

    u8 set_idx = sys_cb.set_idx;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);

    return frm;
}
#else
#define SET_LIST_CNT                       ((int)(sizeof(tbl_setting_list) / sizeof(tbl_setting_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_set_list_t_
{
    compo_listbox_t *listbox;

} f_set_list_t;

static const compo_listbox_item_t tbl_setting_list[] =
{
    {0},
};
//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_set_sub_list_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE);
    compo_listbox_set(listbox, tbl_setting_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, 0);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    u8 set_idx = sys_cb.set_idx;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);
    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//点进图标进入应用
static void func_system_sub_system_icon_click(void)
{
    int icon_idx;
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SYSTEM_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_system_list[icon_idx].func_sta;
    //切入应用
    if (func_sta > 0)
    {

//        func_switch_to(func_sta, FUNC_SWITCH_ZOOM_FADE | FUNC_SWITCH_AUTO);
        func_cb.sta = func_sta;
        sys_cb.set_idx = listbox->focus_icon_idx;
    }
}

//切换到设置菜单页面
static void func_system_sub_system_switch_to_clock(void)
{
    u8 func_sta = FUNC_CLOCK;
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    widget_icon_t *icon = compo_listbox_select_byidx(listbox, 0);
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    // sys_cb.set_idx = 0;
}
//主菜单功能事件处理
static void func_system_sub_system_process(void)
{
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_move(f_set->listbox);
    func_process();
}

//进入设置主菜单页面
static void func_system_sub_system_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_set_list_t));
    func_cb.frm_main = func_system_sub_system_form_create();

    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    f_set->listbox = compo_getobj_byid(COMPO_ID_SYSTEM);
    compo_listbox_t *listbox = f_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    compo_listbox_move_init_modify(listbox, 80, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2)+40);
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, SYSTEM_LIST_CNT - 2));
#endif
    func_cb.enter_tick = tick_get();
}

//主菜单功能消息处理
static void func_system_sub_system_message(size_msg_t msg)
{
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_system_sub_system_icon_click();                //单击图标
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_message(msg);
            // sys_cb.set_idx = 0;
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_system_sub_system_switch_to_clock();       //返回设置界面
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//退出菜单样式
void func_system_sub_exit(void)
{
//    printf("func_set_sub_exit\r\n");
    f_set_list_t *f_set = (f_set_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    func_free(listbox->mcb);                                            //释放移动控制块
    func_cb.last = FUNC_SYSTEM;
}

//主菜单功能
void func_system_sub_system(void)
{
//    printf("pic w:%d h:%d\n",gui_image_get_size(tbl_setting_list->res_addr).wid,gui_image_get_size(tbl_setting_list->res_addr).hei);
    func_system_sub_system_enter();
    while (func_cb.sta == FUNC_SYSTEM)
    {
        func_system_sub_system_process();
        func_system_sub_system_message(msg_dequeue());
    }
    func_system_sub_exit();
}
#endif
