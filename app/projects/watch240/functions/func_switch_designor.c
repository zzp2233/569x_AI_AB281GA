#include "include.h"
#include "func.h"
#include "func_menu.h"
#include "ute_module_gui_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
#define SET_LIST_CNT                       ((int)(sizeof(tbl_style_list) / sizeof(tbl_style_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

//风格列表tbl
static const compo_listbox_item_t tbl_style_list[] =
{
    { .vidx = SWITCH_MODE_ROTARY},   //柱面体
    { .vidx = SWITCH_MODE_NORMAL},   //平移
    { .vidx = SWITCH_PODE_SINGLE_SCALE},   //单边缩放
    { .vidx = SWITCH_MODE_BILATERAL_SCALE},   //双边缩放
};

typedef struct func_switch_t_
{
    compo_listbox_t *listbox;
    u8 sel_idx;
    bool flags;
} func_switch_t;


u8 func_sel_switch_bit(uint n)
{
    //TRACE("%s -> [%d,%d]\n", __func__, n, func_cb.menu_style);
    if (uteModuleGuiCommonGetDisplayCtrlSwitchMode() == n)
    {
        return true;
    }

    return false;
}


//创建 风格窗体
compo_form_t *func_switch_designor_form_create(void)
{
    u16 strArr[] = {STR_CYLINDER, STR_STRANSLATION, STR_SINGLE_SCAL, STR_DOUBLE_SCAL};

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SWITCH_EFFECT]);

    //新建 选择列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_LANGUAGE);
    compo_listbox_set(listbox, tbl_style_list, SET_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I341001_28_SET_BG1_BIN);
    compo_listbox_set_item_height(listbox, 114);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I341001_28_SET_CHECKED_BIN, UI_BUF_I341001_28_SET_UNCHECKED_BIN);
    compo_listbox_set_bithook(listbox, func_sel_switch_bit);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
    compo_listbox_update(listbox);

    compo_shape_t *shape;
    compo_textbox_t *textbox;
    for (uint8_t i = 1; i < SET_LIST_CNT+1; i++)
    {
        if (i != SET_LIST_CNT)
        {
            shape = compo_shape_create_for_page(frm, listbox->page, COMPO_SHAPE_TYPE_RECTANGLE); //画线
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, listbox->line_height_total * i - listbox->line_space / 2, 320, 1);
            compo_shape_set_color(shape, make_color(0x29, 0x29, 0x29));
        }
        textbox = compo_textbox_create_for_page(frm, listbox->page, 100);
        compo_textbox_set_location(textbox, 24, listbox->line_height_total * i - listbox->line_height_total / 2 - 21, 260, 48);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set(textbox, i18n[strArr[i-1]]);
    }

    return frm;
}

//点进图标进入对应的风格
static void func_set_sub_list_icon_click(void)
{
    int icon_idx;
    func_switch_t *f_set = (func_switch_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    u8 move_style;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= SET_LIST_CNT)
    {
        return;
    }

    //根据图标索引获取应用ID
    move_style = tbl_style_list[icon_idx].vidx;

    //切换风格
    uteModuleGuiCommonSetDisplayCtrlSwitchMode(move_style);
    //printf("uteModuleGuiCommonGetDisplayCtrlSwitchMode = %d\n", uteModuleGuiCommonGetDisplayCtrlSwitchMode());
}

#else

#define SET_LIST_CNT                       ((int)(sizeof(tbl_style_list) / sizeof(tbl_style_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

//风格列表tbl
static const compo_listbox_item_t tbl_style_list[] =
{
    {0},
};

typedef struct func_switch_t_
{
    compo_listbox_t *listbox;
    u8 sel_idx;
    bool flags;
} func_switch_t;

u8 func_sel_switch_bit(uint n)
{

}
//创建 风格窗体
compo_form_t *func_switch_designor_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    return frm;
}

//点进图标进入对应的 风格
static void func_set_sub_list_icon_click(void)
{
}

#endif


// 风格功能事件处理
static void func_switch_designor_process(void)
{
    func_switch_t *f_set = (func_switch_t *)func_cb.f_cb;
    compo_listbox_move(f_set->listbox);
    compo_listbox_update(f_set->listbox);
    func_process();
}

// 风格功能消息处理
static void func_switch_designor_message(size_msg_t msg)
{
    func_switch_t *f_set = (func_switch_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_set_sub_list_icon_click();                //单击图标
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_message(msg);
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入 风格功能
static void func_switch_designor_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(func_switch_t));
    func_cb.frm_main = func_switch_designor_form_create();

    func_switch_t *f_set = (func_switch_t *)func_cb.f_cb;
    f_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init_modify(listbox, 173, compo_listbox_gety_byidx(listbox, SET_LIST_CNT - 2));
    func_cb.enter_tick = tick_get();
}

//退出 风格功能
static void func_switch_designor_exit(void)
{
    printf("%s exit\n", __func__);
    func_switch_t *f_set = (func_switch_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;
    if(listbox->mcb != NULL)
    {
        func_free(listbox->mcb);
    }
    func_cb.last = FUNC_SWITCH_DESIGOR;
}

// 风格功能
void func_switch_designor(void)
{
    printf("%s\n", __func__);
    func_switch_designor_enter();
    while (func_cb.sta == FUNC_SWITCH_DESIGOR)
    {
        func_switch_designor_process();
        func_switch_designor_message(msg_dequeue());
    }
    func_switch_designor_exit();
}

