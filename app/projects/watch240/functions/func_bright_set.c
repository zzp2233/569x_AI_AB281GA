#include "include.h"
#include "func.h"
#include "ute_module_gui_common.h"
#include "ute_module_charencode.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_BRIGHT_SET_SUPPORT
enum
{
    COMPO_ID_LISTBOX = 1,
    COMPO_ID_LIGHT,
    COMPO_ID_TIME_SET,
    COMPO_ID_CLOCK_SWITCH,
    COMPO_ID_CLOCK_STYLE,
    COMPO_ID_CLOCK_TIME,
    COMPO_ID_TEXT,
    COMPO_ID_MAX,
};
typedef struct f_bright_set_list_t_
{
#if !GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    compo_listbox_t *listbox;
#else
    page_tp_move_t *ptm;
#endif

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

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

#define  ON_PIC     UI_BUF_I341001_28_SET_SWITCH_OPEN_BUTTON_BIN
#define  OFF_PIC    UI_BUF_I341001_28_SET_SWITCH_CLOSE_BUTTON_BIN

//亮度设置页面
compo_form_t *func_bright_set_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //亮度
    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, 320, 116);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,60+116/2,320,116);
    compo_setid(cardbox, COMPO_ID_LIGHT);

    compo_cardbox_rect_set_location(cardbox, 0, 0, 114/2, 320, 1, 16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29, 0x29, 0x29));

    compo_cardbox_icon_set_pos(cardbox, 0, 320-17-160, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 280, 48);
    compo_cardbox_text_set(cardbox, 0, i18n[STR_BRIGHTNESS]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_text_scroll_process(cardbox, true);


    //自动息屏
    cardbox = compo_cardbox_create(frm, 1, 1, 2, 320, 116);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60+116/2+116, 320, 116);
    compo_setid(cardbox, COMPO_ID_TIME_SET);

    compo_cardbox_rect_set_location(cardbox, 0, 0, 114/2, 320, 1, 16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));

    compo_cardbox_icon_set_pos(cardbox, 0, 320-17-160, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-39, 280, 48);
    compo_cardbox_text_set(cardbox, 0, i18n[STR_SETTING_DOUSING]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_text_scroll_process(cardbox, true);

    char second[4] = {0};
    char str_buf[10] = {0};
    snprintf(second, sizeof(second), "%d", uteModuleGuiCommonGetDisplayOffTime());
    uteModuleCharencodeReplaceSubString(i18n[STR_SECOND_JOINT], str_buf, "##", second);
    compo_cardbox_text_set_location(cardbox, 1, -GUI_SCREEN_CENTER_X+24, 4, 240, 48);
    compo_cardbox_text_set(cardbox, 1, str_buf);
    compo_cardbox_text_set_forecolor(cardbox, 1, make_color(102, 102, 102));
    compo_cardbox_text_set_align_center(cardbox, 1, false);
    compo_cardbox_text_scroll_process(cardbox, true);


    //息屏显示
    cardbox = compo_cardbox_create(frm, 1, 1, 1, 320, 116);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60+116/2+116*2, 320, 116);
    compo_setid(cardbox, COMPO_ID_CLOCK_SWITCH);

    compo_cardbox_rect_set_location(cardbox, 0, 0, 114/2, 320, 1, 16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));

    compo_cardbox_icon_set_pos(cardbox, 0, 320-62/2-160, 0);
    compo_cardbox_icon_set(cardbox, 0, OFF_PIC);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 240, 48);
    compo_cardbox_text_set(cardbox, 0, i18n[STR_ALWAS_DISPLAY]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_text_scroll_process(cardbox, true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_BATTER_REMIND]));
    compo_textbox_set_location(textbox, 24, 388+50, 320, 200);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox, i18n[STR_BATTER_REMIND]);
    compo_textbox_set_forecolor(textbox, make_color(102, 102, 102));
    compo_textbox_set_visible(textbox, uteModuleGuiCommonIsScreenSaverSetOn() ? false : true);
    compo_setid(textbox, COMPO_ID_TEXT);


    //息屏表盘
    cardbox = compo_cardbox_create(frm, 1, 1, 1, 320, 116);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60+116/2+116*3, 320, 116);
    compo_setid(cardbox, COMPO_ID_CLOCK_STYLE);

    compo_cardbox_rect_set_location(cardbox, 0, 0, 114/2, 320, 1, 16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));

    compo_cardbox_icon_set_pos(cardbox, 0, 320-17-160, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 280, 48);
    compo_cardbox_text_set(cardbox, 0,i18n[STR_AOD_DAIL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_text_scroll_process(cardbox, true);
    widget_set_visible(cardbox->page, uteModuleGuiCommonIsScreenSaverSetOn() ? true : false);

    //生效时段
    cardbox = compo_cardbox_create(frm, 1, 1, 1, 320, 116);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60+116/2+116*4, 320, 116);
    compo_setid(cardbox, COMPO_ID_CLOCK_TIME);

    //compo_cardbox_rect_set_location(cardbox, 0, 0, 114/2, 320, 1, 16);    //画线
    //compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));

    compo_cardbox_icon_set_pos(cardbox, 0, 320-17-160, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 280, 48);
    compo_cardbox_text_set(cardbox, 0,i18n[STR_EFFECT]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_text_scroll_process(cardbox, true);
    widget_set_visible(cardbox->page, uteModuleGuiCommonIsScreenSaverSetOn() ? true : false);

    return frm;
}

//更新显示界面
static void func_set_sub_bright_disp(void)
{
    compo_cardbox_t *cardbox_switch  = compo_getobj_byid(COMPO_ID_CLOCK_SWITCH);
    compo_cardbox_t *cardbox_style  = compo_getobj_byid(COMPO_ID_CLOCK_STYLE);
    compo_cardbox_t *cardbox_time  = compo_getobj_byid(COMPO_ID_CLOCK_TIME);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT);
    compo_textbox_set_visible(textbox, uteModuleGuiCommonIsScreenSaverSetOn() ? false : true);
    compo_cardbox_icon_set(cardbox_switch, 0, uteModuleGuiCommonIsScreenSaverSetOn() ? ON_PIC : OFF_PIC);
    widget_set_visible(cardbox_style->page, uteModuleGuiCommonIsScreenSaverSetOn() ? true : false);
    widget_set_visible(cardbox_time->page, uteModuleGuiCommonIsScreenSaverSetOn() ? true : false);
}

//获取点击的卡片组件id
static u16 func_setting_sav_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<COMPO_ID_MAX; i++)
    {
        id = COMPO_ID_LIGHT + i;
        cardbox = compo_getobj_byid(id);
        if(cardbox == NULL)//如果没有这个组件就不需要处理
        {
            continue;
        }
        rect = compo_cardbox_get_absolute(cardbox);
        if (compo_cardbox_get_visible(cardbox) && abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//点进图标进入应用
void func_bright_set_icon_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_setting_sav_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > COMPO_ID_MAX)
    {
        return;
    }

    switch(compo_id)
    {
        case COMPO_ID_LIGHT:
            uteTaskGuiStartScreen(FUNC_LIGHT, 0, __func__);
            break;
        case COMPO_ID_TIME_SET:
            uteTaskGuiStartScreen(FUNC_SET_SUB_DOUSING, 0, __func__);
            break;
        case COMPO_ID_CLOCK_SWITCH:
        {
            compo_cardbox_t* card = compo_getobj_byid(compo_id);
            rect_t play_rect = compo_cardbox_get_icon_absolute(card, 0);
            if (abs_s(pt.x - play_rect.x) * 2 <= (play_rect.wid + 20) && abs_s(pt.y - play_rect.y) * 2 <= (play_rect.hei + 20))
            {
                uteModuleGuiCommonScreenSaverSetSwitch();
            }
        }
        break;
        case COMPO_ID_CLOCK_STYLE:
            uteTaskGuiStartScreen(FUNC_CLOCK_SAVER_PREVIEW, 0, __func__);
            break;
        case COMPO_ID_CLOCK_TIME:
            uteTaskGuiStartScreen(FUNC_DISPLAY_TIME_PERIOD, 0, __func__);
            break;
        default:
            break;
    }
    func_set_sub_bright_disp();
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

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
    compo_listbox_set_bgimg(listbox, UI_BUF_I343001_28_SET_BG_BIN);
//    compo_listbox_set_sta_icon(listbox,UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN,UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

//    compo_shape_t *shape = compo_shape_create_for_page(frm, listbox->page, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 104, 320, 1);
//    compo_shape_set_color(shape,make_color(0x29,0x29,0x29));

    compo_listbox_set_focus(listbox, 140);
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
static u8 set_back_pic(uint n)
{
    return true;
}
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
    compo_listbox_set_bgimg(listbox, UI_BUF_I338001_28_SET_LIST_BG_BIN);

    compo_listbox_set_sta_icon(listbox, UI_BUF_I338001_28_SET_MORE_BIN, 0);
    compo_listbox_set_bithook(listbox, set_back_pic);

    compo_listbox_set_item_text(listbox, 0, 30, 180, 35, false);

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
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
    //compo_listbox_t *listbox = f_bright_set->listbox;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_bright_set->ptm)
            {
                compo_page_move_touch_handler(f_bright_set->ptm);
            }
            break;
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
#if !GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    if(f_bright_set->listbox != NULL)
    {
        compo_listbox_move(f_bright_set->listbox);
    }
#else
    if(f_bright_set->ptm)
    {
        compo_page_move_process(f_bright_set->ptm);
    }
    func_set_sub_bright_disp();
#endif
    func_process();
}

//进入表盘&主题功能
static void func_bright_set_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_bright_set_list_t));
    func_cb.frm_main = func_bright_set_form_create();
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
#if !GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_bright_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_bright_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init(listbox);
#else
    f_bright_set->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  610,
        .page_count = 1,
        .quick_jump_perc = 10,
    };
    compo_page_move_init(f_bright_set->ptm, func_cb.frm_main->page_body, &info);
#endif
    func_cb.enter_tick = tick_get();
}

//退出亮度设置功能
static void func_bright_set_exit(void)
{
    f_bright_set_list_t *f_bright_set = (f_bright_set_list_t *)func_cb.f_cb;
#if !GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    compo_listbox_t *listbox = f_bright_set->listbox;
    func_free(listbox->mcb);
#else
    if (f_bright_set->ptm)
    {
        func_free(f_bright_set->ptm);
    }
#endif
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
