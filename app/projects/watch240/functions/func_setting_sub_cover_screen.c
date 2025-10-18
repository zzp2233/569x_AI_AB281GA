#include "include.h"
#include "func.h"
#include "ute_module_sport.h"
#include "ute_application_common.h"
#include "ute_module_gui_common.h"

#if UTE_MODULE_COVER_DISPLAY_OFF_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_cover_screen_t_
{
} f_cover_screen_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
static const u32 tbl_saving_switch_res[] =
{
    UI_BUF_I341001_28_SET_SWITCH_OPEN_BUTTON_BIN,   //ON
    UI_BUF_I341001_28_SET_SWITCH_CLOSE_BUTTON_BIN,  //OFF
};

//覆盖息屏页面
compo_form_t *func_set_sub_cover_screen_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_COVER_SCREEN]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 1, 1, 1, 320, 100);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, 24 + 320/2, 60 + 100/2, 320, 100);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_rect_set_color(card, 0, make_color(0x29, 0x29, 0x29));
    compo_cardbox_rect_set_location(card, 0, 0, 100/2-1, 320, 1, 16);

    compo_cardbox_icon_set_location(card, 0,
                                    320/2 - 5 - gui_image_get_size(tbl_saving_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_saving_switch_res[0]).wid, gui_image_get_size(tbl_saving_switch_res[0]).hei);

    if(uteModuleGuiCommonGetCoverDisplayOffSwitch())
    {
        compo_cardbox_icon_set(card, 0, tbl_saving_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_saving_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-320/2, -14,
                                    320 - gui_image_get_size(tbl_saving_switch_res[0]).wid - 50,
                                    50);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_COVER_SCREEN]);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_WHEN_ENABLED]));
    compo_textbox_set_location(textbox, 24, 180, 320, 300);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox, i18n[STR_WHEN_ENABLED]);
    compo_textbox_set_forecolor(textbox, make_color(102, 102, 102));

    return frm;
}

#endif
//覆盖息屏事件处理
static void func_set_sub_cover_screen_process(void)
{
    compo_cardbox_t * card = compo_getobj_byid(COMPO_CARD_1);
    if(uteModuleGuiCommonGetCoverDisplayOffSwitch())
    {
        compo_cardbox_icon_set(card, 0, tbl_saving_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_saving_switch_res[1]);
    }


    for(u8 i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)      //文本滚动
    {
        u16 id = COMPO_CARD_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
    func_process();
}

//获取点击卡片的id
static u16 func_cover_screen_card_get_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t* cardbox = NULL;
    for(i = 0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
        id = COMPO_CARD_START + 1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (compo_cardbox_get_visible(cardbox) && abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }

    return ret;
}

//单击按钮
static void func_cover_screen_button_click(void)
{
//    f_cover_screen_t *saving = (f_cover_screen_t *)func_cb.f_cb;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_cover_screen_card_get_id(pt);
    if (compo_id <= 0 || compo_id > COMPO_CARD_END - 1)
    {
        return;
    }
//    printf("click compo_id:%d\n", compo_id);

    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == COMPO_CARD_1)
        {
            compo_cardbox_t* card = compo_getobj_byid(compo_id);
            rect_t play_rect = compo_cardbox_get_icon_absolute(card, 0);
            if (abs_s(pt.x - play_rect.x) * 2 <= (play_rect.wid + 20) && abs_s(pt.y - play_rect.y) * 2 <= (play_rect.hei + 20))
            {
                uteModuleGuiCommonSetCoverDisplayOffSwitch(!uteModuleGuiCommonGetCoverDisplayOffSwitch());
            }
        }
    }

}

//覆盖息屏功能消息处理
static void func_set_sub_cover_screen_message(size_msg_t msg)
{
    //f_cover_screen_t *f_cover_screen = (f_cover_screen_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_cover_screen_button_click();
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入覆盖息屏功能
static void func_set_sub_cover_screen_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_cover_screen_t));
    func_cb.frm_main = func_set_sub_cover_screen_form_create();

}

//退出覆盖息屏功能
static void func_set_sub_cover_screen_exit(void)
{
    func_cb.last = FUNC_SET_COVER_SCREEN;
}

//覆盖息屏功能
void func_set_sub_cover_screen(void)
{
    printf("%s\n", __func__);
    func_set_sub_cover_screen_enter();
    while (func_cb.sta == FUNC_SET_COVER_SCREEN)
    {
        func_set_sub_cover_screen_process();
        func_set_sub_cover_screen_message(msg_dequeue());
    }
    func_set_sub_cover_screen_exit();
}
#endif
