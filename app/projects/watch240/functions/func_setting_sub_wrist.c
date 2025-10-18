#include "include.h"
#include "func.h"
#include "ute_module_sport.h"
#include "ute_application_common.h"
#include "ute_module_liftwrist.h"
#include "ute_module_settingremind.h"
#include "ute_module_notdisturb.h"
#include "ute_application_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    UI_BUF_I330001_PUBLIC_SWITCH02_BIN,         //ON
    UI_BUF_I330001_PUBLIC_SWITCH00_BIN,         //OFF
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 1, 1, 1, 232, 72);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, 4 + 232/2, 54 + 72/2, 232, 72);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_rect_set_color(card, 0, make_color(41,41,41));
    compo_cardbox_rect_set_location(card, 0, 0, 0, 232, 72, 16);

    compo_cardbox_icon_set_location(card, 0,
                                    232/2 - 5 - gui_image_get_size(tbl_wrist_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_wrist_switch_res[0]).wid, gui_image_get_size(tbl_wrist_switch_res[0]).hei);

    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-232/2, -14,
                                    232 - gui_image_get_size(tbl_wrist_switch_res[0]).wid - 50,
                                    30);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_SETTING_UP]);

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    UI_BUF_I335001_27_MORE_28_SET_7_RAISE_SWITCH_ICON_PIC40X20_X184_Y66_OPEN_BIN,         //OFF
    UI_BUF_I335001_27_MORE_28_SET_7_RAISE_SWITCH_ICON_PIC40X20_X184_Y66_CLOSE_BIN,         //ON
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 1, 1, 1, 232, 72);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, 4 + 232/2, 54 + 72/2, 232, 72);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_rect_set_color(card, 0, make_color(41,41,41));
    compo_cardbox_rect_set_location(card, 0, 0, 0, 232, 72, 16);

    compo_cardbox_icon_set_location(card, 0,
                                    232/2 - 5 - gui_image_get_size(tbl_wrist_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_wrist_switch_res[0]).wid, gui_image_get_size(tbl_wrist_switch_res[0]).hei);

    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-232/2, -14,
                                    232 - gui_image_get_size(tbl_wrist_switch_res[0]).wid - 50,
                                    30);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_SETTING_UP]);

    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
static void func_set_sub_wrist_exit(void);
static void func_set_sub_wrist_enter(void);

#define         DRAG_AUTO_SPEED     10
#define         DRAG_MAX_DISTANCE   300
#define         DRAG_MAX_DISTANCE1  84
#define         DRAG_MIN_DISTANCE   4
#define         DRAG_MAX_BACK_DISTANCE      220
#define         DRAG_MIN_BACK_DISTANCE      49
#define         FOCUS_AUTO_STEP         5
#define         FOCUS_AUTO_STEP_DIV     16

typedef struct f_wrist_t_
{
    bool value;
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移动，惯性
    u32 tick;
    s32 focus_x;
    s32 focus_y;
    s32 focus_dx;
    s32 focus_dy;
    s32 focus_ofsx;
    s32 focus_ofsy;
    int moveto_ix;                  //设定自动移到的目标菜单ix
    int moveto_iy;                  //设置自动移到的目标菜单iy
    point_t moveto;                 //设定自动移到的坐标
    u8 time_scale;
} f_wrist_t;

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    //uteApplicationCommonSendQuickSwitchStatus();
}

enum
{
    CARD_ID_START = 1,
    COMPO_ID_CARD_WRIST_ALL,
    COMPO_ID_CARD_WRIST_DEF,
    COMPO_ID_CARD_WRIST_START_TIME,
    COMPO_ID_CARD_WRIST_END_TIME,
    CARD_ID_END,
};

typedef struct ui_handle_t_
{

    ///全天翻腕
    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t
        {
            u8 idx;
            s16 x,y;
            s16 w,h;
            u16 r;
        } rect[1];

        struct card_icon_t
        {
            u8 idx;
            s16 x,y;
            u16 w,h;
            u32 res_on;
            u32 res_off;
        } icon[1];

        struct card_text_t
        {
            u8 idx;
            s16 x,y;
            u16 w,h;
            u16 str_id;
            u32 res;
            bool center;
            bool wordwrap;
            color_t color;
            u16 rev;
        } text[1];
    } wrist_all_card;

    ///定时翻腕
    struct card1_t wrist_def_card;

    ///开始时间
    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct card_rect_t rect[1];
        struct card_text_t text[3];
    } wrist_start_time;

    ///结束时间
    struct card2_t wrist_end_time;

} ui_handle_t;

static const ui_handle_t ui_handle =
{
    .wrist_all_card = {
        .id = COMPO_ID_CARD_WRIST_ALL,
        .x  = 24+320/2,
        .y  = 57+106/2,
        .w  = 320,
        .h  = 106,

        .rect = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 106/2-2,
                .w      = 320,
                .h      = 1,
                .r      = 0,
            },
        },

        .icon = {
            [0] = {
                .idx    = 0,
                .x      = 282-24+62/2 - 320/2,
                .y      = 0,
                .w      = 62,
                .h      = 32,
                .res_on = UI_BUF_I341001_28_SET_SWITCH_OPEN_BUTTON_BIN,
                .res_off= UI_BUF_I341001_28_SET_SWITCH_CLOSE_BUTTON_BIN,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 0 - 320/2,
                .y      = 38 - 106/2,
                .w      = 240,
                .h      = 40,
                .str_id = STR_DISTURD_ALL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },
        },
    },

    .wrist_def_card = {
        .id = COMPO_ID_CARD_WRIST_DEF,
        .x  = 24+320/2,
        .y  = 163+106/2,
        .w  = 320,
        .h  = 106,

        .rect = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 106/2-2,
                .w      = 320,
                .h      = 1,
                .r      = 0,
            },
        },

        .icon = {
            [0] = {
                .idx    = 0,
                .x      = 282-24+62/2 - 320/2,
                .y      = 0,
                .w      = 62,
                .h      = 32,
                .res_on = UI_BUF_I341001_28_SET_SWITCH_OPEN_BUTTON_BIN,
                .res_off= UI_BUF_I341001_28_SET_SWITCH_CLOSE_BUTTON_BIN,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 0 - 320/2,
                .y      = 38 - 106/2,
                .w      = 240,
                .h      = 40,
                .str_id = STR_DISTURD_TIM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },
        },
    },

    .wrist_start_time = {
        .id = COMPO_ID_CARD_WRIST_START_TIME,
        .x  = 24+320/2,
        .y  = 270+146/2,
        .w  = 320,
        .h  = 146,

        .rect = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 146/2-2,
                .w      = 320,
                .h      = 1,
                .r      = 0,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 0 - 320/2,
                .y      = 35 - 146/2,
                .w      = 240,
                .h      = 40,
                .str_id = STR_DISTURD_TIM_START,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },

            [1] = {
                .idx    = 1,
                .x      = 0 - 320/2,
                .y      = 85 - 146/2,
                .w      = 100,
                .h      = 40,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = 0,
            },

            [2] = {
                .idx    = 2,
                .x      = 0 - 320/2 + 90,
                .y      = 85 - 146/2,
                .w      = 100,
                .h      = 40,
                .str_id = STR_AM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = STR_PM,
            },
        },
    },


    .wrist_end_time = {
        .id = COMPO_ID_CARD_WRIST_END_TIME,
        .x  = 24+320/2,
        .y  = 417+146/2,
        .w  = 320,
        .h  = 146,

        .rect = {
            [0] = {
                .idx    = 0,
                .x      = 0,
                .y      = 0,
                .w      = 0,
                .h      = 0,
                .r      = 0,
            },
        },

        .text = {
            [0] = {
                .idx    = 0,
                .x      = 0 - 320/2,
                .y      = 35 - 146/2,
                .w      = 240,
                .h      = 40,
                .str_id = STR_DISTURD_TIM_END,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {255,255,255},
                .rev    = 0,
            },

            [1] = {
                .idx    = 1,
                .x      = 0 - 320/2,
                .y      = 85 - 146/2,
                .w      = 100,
                .h      = 40,
                .str_id = STR_NULL,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = 0,
            },

            [2] = {
                .idx    = 2,
                .x      = 0 - 320/2 + 90,
                .y      = 85 - 146/2,
                .w      = 100,
                .h      = 40,
                .str_id = STR_AM,
                .res    = UI_BUF_0FONT_FONT_BIN,
                .center = false,
                .wordwrap = false,
                .color  = {148,148,148},
                .rev    = STR_PM,
            },
        },
    },
};

typedef struct func_alarm_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_alarm_hour_format_t;

static func_alarm_hour_format_t func_alarm_convert_to_12hour(s8 hour24)
{
    // printf("time_apm%d\n",hour24);
    u8 am_pm = (hour24 < 12) ? 2 : 1;    //1 PM, 2 AM
    func_alarm_hour_format_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (hour24 == 0)
        {
            hour12.hour = 12;
        }
        else if (hour24 > 12)
        {
            hour12.hour = hour24 - 12;
        }
        else
        {
            hour12.hour = hour24;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = hour24;
    hour12.am_pm = 0;
    return hour12;
}

static void func_set_sub_wrist_state_update(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if(quick.isTurnTheWrist)
    {
        if(uteModuleLiftWristGetNowOpenStatus() == LIFT_WRIST_CLOSE)
        {
            switch_set_sub_wrist();
        }
    }
    else
    {
        if(uteModuleLiftWristGetNowOpenStatus() != LIFT_WRIST_CLOSE)
        {
            switch_set_sub_wrist();
        }
    }
}

//翻腕模式页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    widget_page_set_client(frm->page_body, 0, -DRAG_MIN_BACK_DISTANCE);

    ///创建全天翻腕卡片
    compo_cardbox_t* wrist_all_card = compo_cardbox_create(frm,
                                      sizeof(ui_handle.wrist_all_card.rect)/sizeof(ui_handle.wrist_all_card.rect[0]),
                                      sizeof(ui_handle.wrist_all_card.icon)/sizeof(ui_handle.wrist_all_card.icon[0]),
                                      sizeof(ui_handle.wrist_all_card.text)/sizeof(ui_handle.wrist_all_card.text[0]),
                                      ui_handle.wrist_all_card.w, ui_handle.wrist_all_card.h);
    compo_cardbox_set_visible(wrist_all_card, true);
    compo_cardbox_set_location(wrist_all_card, ui_handle.wrist_all_card.x, ui_handle.wrist_all_card.y, ui_handle.wrist_all_card.w, ui_handle.wrist_all_card.h);
    compo_setid(wrist_all_card, ui_handle.wrist_all_card.id);
    for (u8 i=0; i<sizeof(ui_handle.wrist_all_card.rect)/sizeof(ui_handle.wrist_all_card.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(wrist_all_card, ui_handle.wrist_all_card.rect[i].idx, ui_handle.wrist_all_card.rect[i].x, ui_handle.wrist_all_card.rect[i].y,
                                        ui_handle.wrist_all_card.rect[i].w, ui_handle.wrist_all_card.rect[i].h, ui_handle.wrist_all_card.rect[i].r);
        compo_cardbox_rect_set_color(wrist_all_card, ui_handle.wrist_all_card.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.wrist_all_card.icon)/sizeof(ui_handle.wrist_all_card.icon[0]); i++)
    {
        compo_cardbox_icon_set_location(wrist_all_card, ui_handle.wrist_all_card.icon[i].idx, ui_handle.wrist_all_card.icon[i].x, ui_handle.wrist_all_card.icon[i].y,
                                        ui_handle.wrist_all_card.icon[i].w, ui_handle.wrist_all_card.icon[i].h);
        compo_cardbox_icon_set(wrist_all_card, ui_handle.wrist_all_card.icon[i].idx,
                               uteModuleLiftWristIsOpen() > 0 ? ui_handle.wrist_all_card.icon[i].res_on : ui_handle.wrist_all_card.icon[i].res_off);
    }

    for (u8 i=0; i<sizeof(ui_handle.wrist_all_card.text)/sizeof(ui_handle.wrist_all_card.text[0]); i++)
    {
        compo_cardbox_text_set_font(wrist_all_card, ui_handle.wrist_all_card.text[i].idx, ui_handle.wrist_all_card.text[i].res);
        widget_text_set_ellipsis(wrist_all_card->text[ui_handle.wrist_all_card.text[i].idx], false);
        compo_cardbox_text_set_align_center(wrist_all_card, ui_handle.wrist_all_card.text[i].idx, ui_handle.wrist_all_card.text[i].center);
        widget_text_set_wordwrap(wrist_all_card->text[ui_handle.wrist_all_card.text[i].idx], ui_handle.wrist_all_card.text[i].wordwrap);
        widget_text_set_color(wrist_all_card->text[ui_handle.wrist_all_card.text[i].idx], make_color(ui_handle.wrist_all_card.text[i].color.r, ui_handle.wrist_all_card.text[i].color.g, ui_handle.wrist_all_card.text[i].color.b));
        compo_cardbox_text_set_location(wrist_all_card, ui_handle.wrist_all_card.text[i].idx, ui_handle.wrist_all_card.text[i].x, ui_handle.wrist_all_card.text[i].y,
                                        ui_handle.wrist_all_card.text[i].w, ui_handle.wrist_all_card.text[i].h);
        compo_cardbox_text_set(wrist_all_card, ui_handle.wrist_all_card.text[i].idx, i18n[ui_handle.wrist_all_card.text[i].str_id]);
    }

    ///创建定时翻腕卡片
    compo_cardbox_t* wrist_def_card = compo_cardbox_create(frm,
                                      sizeof(ui_handle.wrist_def_card.rect)/sizeof(ui_handle.wrist_def_card.rect[0]),
                                      sizeof(ui_handle.wrist_def_card.icon)/sizeof(ui_handle.wrist_def_card.icon[0]),
                                      sizeof(ui_handle.wrist_def_card.text)/sizeof(ui_handle.wrist_def_card.text[0]),
                                      ui_handle.wrist_def_card.w, ui_handle.wrist_def_card.h);
    compo_cardbox_set_visible(wrist_def_card, true);
    compo_cardbox_set_location(wrist_def_card, ui_handle.wrist_def_card.x, ui_handle.wrist_def_card.y, ui_handle.wrist_def_card.w, ui_handle.wrist_def_card.h);
    compo_setid(wrist_def_card, ui_handle.wrist_def_card.id);
    for (u8 i=0; i<sizeof(ui_handle.wrist_def_card.rect)/sizeof(ui_handle.wrist_def_card.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(wrist_def_card, ui_handle.wrist_def_card.rect[i].idx, ui_handle.wrist_def_card.rect[i].x, ui_handle.wrist_def_card.rect[i].y,
                                        ui_handle.wrist_def_card.rect[i].w, ui_handle.wrist_def_card.rect[i].h, ui_handle.wrist_def_card.rect[i].r);
        compo_cardbox_rect_set_color(wrist_def_card, ui_handle.wrist_def_card.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.wrist_def_card.icon)/sizeof(ui_handle.wrist_def_card.icon[0]); i++)
    {
        compo_cardbox_icon_set_location(wrist_def_card, ui_handle.wrist_def_card.icon[i].idx, ui_handle.wrist_def_card.icon[i].x, ui_handle.wrist_def_card.icon[i].y,
                                        ui_handle.wrist_def_card.icon[i].w, ui_handle.wrist_def_card.icon[i].h);
        compo_cardbox_icon_set(wrist_def_card, ui_handle.wrist_def_card.icon[i].idx,
                               uteModuleLiftWristIsOpenScheduled() > 0 ? ui_handle.wrist_def_card.icon[i].res_on : ui_handle.wrist_def_card.icon[i].res_off);
    }

    for (u8 i=0; i<sizeof(ui_handle.wrist_def_card.text)/sizeof(ui_handle.wrist_def_card.text[0]); i++)
    {
        compo_cardbox_text_set_font(wrist_def_card, ui_handle.wrist_def_card.text[i].idx, ui_handle.wrist_def_card.text[i].res);
        widget_text_set_ellipsis(wrist_def_card->text[ui_handle.wrist_def_card.text[i].idx], false);
        compo_cardbox_text_set_align_center(wrist_def_card, ui_handle.wrist_def_card.text[i].idx, ui_handle.wrist_def_card.text[i].center);
        widget_text_set_wordwrap(wrist_def_card->text[ui_handle.wrist_def_card.text[i].idx], ui_handle.wrist_def_card.text[i].wordwrap);
        widget_text_set_color(wrist_def_card->text[ui_handle.wrist_def_card.text[i].idx], make_color(ui_handle.wrist_def_card.text[i].color.r, ui_handle.wrist_def_card.text[i].color.g, ui_handle.wrist_def_card.text[i].color.b));
        compo_cardbox_text_set_location(wrist_def_card, ui_handle.wrist_def_card.text[i].idx, ui_handle.wrist_def_card.text[i].x, ui_handle.wrist_def_card.text[i].y,
                                        ui_handle.wrist_def_card.text[i].w, ui_handle.wrist_def_card.text[i].h);
        compo_cardbox_text_set(wrist_def_card, ui_handle.wrist_def_card.text[i].idx, i18n[ui_handle.wrist_def_card.text[i].str_id]);
    }

    ///创建开始时间卡片
    compo_cardbox_t* wrist_start_time = compo_cardbox_create(frm,
                                        sizeof(ui_handle.wrist_start_time.rect)/sizeof(ui_handle.wrist_start_time.rect[0]),
                                        0,
                                        sizeof(ui_handle.wrist_start_time.text)/sizeof(ui_handle.wrist_start_time.text[0]),
                                        ui_handle.wrist_start_time.w, ui_handle.wrist_start_time.h);
    compo_picturebox_t *picbox = compo_picturebox_create_for_page(frm,wrist_start_time->page,UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_picturebox_set_pos(picbox,ui_handle.wrist_def_card.icon[0].x+15, ui_handle.wrist_def_card.icon[0].y);
    compo_cardbox_set_visible(wrist_start_time, uteModuleLiftWristIsOpenScheduled() > 0 ? true : false);
    compo_cardbox_set_location(wrist_start_time, ui_handle.wrist_start_time.x, ui_handle.wrist_start_time.y, ui_handle.wrist_start_time.w, ui_handle.wrist_start_time.h);
    compo_setid(wrist_start_time, ui_handle.wrist_start_time.id);
    for (u8 i=0; i<sizeof(ui_handle.wrist_start_time.rect)/sizeof(ui_handle.wrist_start_time.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(wrist_start_time, ui_handle.wrist_start_time.rect[i].idx, ui_handle.wrist_start_time.rect[i].x, ui_handle.wrist_start_time.rect[i].y,
                                        ui_handle.wrist_start_time.rect[i].w, ui_handle.wrist_start_time.rect[i].h, ui_handle.wrist_start_time.rect[i].r);
        compo_cardbox_rect_set_color(wrist_start_time, ui_handle.wrist_start_time.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.wrist_start_time.text)/sizeof(ui_handle.wrist_start_time.text[0]); i++)
    {
        compo_cardbox_text_set_font(wrist_start_time, ui_handle.wrist_start_time.text[i].idx, ui_handle.wrist_start_time.text[i].res);
        widget_text_set_ellipsis(wrist_start_time->text[ui_handle.wrist_start_time.text[i].idx], false);
        compo_cardbox_text_set_align_center(wrist_start_time, ui_handle.wrist_start_time.text[i].idx, ui_handle.wrist_start_time.text[i].center);
        widget_text_set_wordwrap(wrist_start_time->text[ui_handle.wrist_start_time.text[i].idx], ui_handle.wrist_start_time.text[i].wordwrap);
        widget_text_set_color(wrist_start_time->text[ui_handle.wrist_start_time.text[i].idx], make_color(ui_handle.wrist_start_time.text[i].color.r, ui_handle.wrist_start_time.text[i].color.g, ui_handle.wrist_start_time.text[i].color.b));
        compo_cardbox_text_set_location(wrist_start_time, ui_handle.wrist_start_time.text[i].idx, ui_handle.wrist_start_time.text[i].x, ui_handle.wrist_start_time.text[i].y,
                                        ui_handle.wrist_start_time.text[i].w, ui_handle.wrist_start_time.text[i].h);

        u8 hour = uteModuleLiftWristGetTime(LIFT_WRIST_START_TIME) >> 8 & 0xff;
        u8 min  = uteModuleLiftWristGetTime(LIFT_WRIST_START_TIME) & 0xff;
        u8 am_pm = 0;
        func_alarm_hour_format_t hour_cov = func_alarm_convert_to_12hour(hour);
        hour = hour_cov.hour;
        am_pm = hour_cov.am_pm;
        char aclock_str[20] = {0};
        memset(aclock_str, '\0', sizeof(aclock_str));
        sprintf(aclock_str, "%02d:%02d", hour, min);

        //printf("pm_flag:%d\n",am_pm);
        if (ui_handle.wrist_start_time.text[i].idx == 2)
        {
            if(am_pm == 2)      //AM
            {
                compo_cardbox_text_set(wrist_start_time, ui_handle.wrist_start_time.text[i].idx, i18n[ui_handle.wrist_start_time.text[i].str_id]);
            }
            else if (am_pm == 1)
            {
                compo_cardbox_text_set(wrist_start_time, ui_handle.wrist_start_time.text[i].idx, i18n[ui_handle.wrist_start_time.text[i].rev]);
            }
        }
        else if (ui_handle.wrist_start_time.text[i].idx == 1)
        {
            compo_cardbox_text_set(wrist_start_time, ui_handle.wrist_start_time.text[i].idx, aclock_str);
        }
        else if (ui_handle.wrist_start_time.text[i].idx == 0)
        {
            compo_cardbox_text_set(wrist_start_time, ui_handle.wrist_start_time.text[i].idx, i18n[ui_handle.wrist_start_time.text[i].str_id]);
        }
    }

    ///创建结束时间卡片
    compo_cardbox_t* wrist_end_time = compo_cardbox_create(frm,
                                      sizeof(ui_handle.wrist_end_time.rect)/sizeof(ui_handle.wrist_end_time.rect[0]),
                                      0,
                                      sizeof(ui_handle.wrist_end_time.text)/sizeof(ui_handle.wrist_end_time.text[0]),
                                      ui_handle.wrist_end_time.w, ui_handle.wrist_end_time.h);
    picbox = compo_picturebox_create_for_page(frm,wrist_end_time->page,UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_picturebox_set_pos(picbox,ui_handle.wrist_def_card.icon[0].x+15, ui_handle.wrist_def_card.icon[0].y);
    compo_cardbox_set_visible(wrist_end_time, uteModuleLiftWristIsOpenScheduled() > 0 ? true : false);
    compo_cardbox_set_location(wrist_end_time, ui_handle.wrist_end_time.x, ui_handle.wrist_end_time.y, ui_handle.wrist_end_time.w, ui_handle.wrist_end_time.h);
    compo_setid(wrist_end_time, ui_handle.wrist_end_time.id);
    for (u8 i=0; i<sizeof(ui_handle.wrist_end_time.rect)/sizeof(ui_handle.wrist_end_time.rect[0]); i++)
    {
        compo_cardbox_rect_set_location(wrist_end_time, ui_handle.wrist_end_time.rect[i].idx, ui_handle.wrist_end_time.rect[i].x, ui_handle.wrist_end_time.rect[i].y,
                                        ui_handle.wrist_end_time.rect[i].w, ui_handle.wrist_end_time.rect[i].h, ui_handle.wrist_end_time.rect[i].r);
        compo_cardbox_rect_set_color(wrist_end_time, ui_handle.wrist_end_time.rect[i].idx, make_color(41, 41, 41));
    }

    for (u8 i=0; i<sizeof(ui_handle.wrist_end_time.text)/sizeof(ui_handle.wrist_end_time.text[0]); i++)
    {
        compo_cardbox_text_set_font(wrist_end_time, ui_handle.wrist_end_time.text[i].idx, ui_handle.wrist_end_time.text[i].res);
        widget_text_set_ellipsis(wrist_end_time->text[ui_handle.wrist_end_time.text[i].idx], false);
        compo_cardbox_text_set_align_center(wrist_end_time, ui_handle.wrist_end_time.text[i].idx, ui_handle.wrist_end_time.text[i].center);
        widget_text_set_wordwrap(wrist_end_time->text[ui_handle.wrist_end_time.text[i].idx], ui_handle.wrist_end_time.text[i].wordwrap);
        widget_text_set_color(wrist_end_time->text[ui_handle.wrist_end_time.text[i].idx], make_color(ui_handle.wrist_end_time.text[i].color.r, ui_handle.wrist_end_time.text[i].color.g, ui_handle.wrist_end_time.text[i].color.b));
        compo_cardbox_text_set_location(wrist_end_time, ui_handle.wrist_end_time.text[i].idx, ui_handle.wrist_end_time.text[i].x, ui_handle.wrist_end_time.text[i].y,
                                        ui_handle.wrist_end_time.text[i].w, ui_handle.wrist_end_time.text[i].h);

        u8 hour = uteModuleLiftWristGetTime(LIFT_WRIST_END_TIME) >> 8 & 0xff;
        u8 min  = uteModuleLiftWristGetTime(LIFT_WRIST_END_TIME) & 0xff;
        u8 am_pm = 0;
        func_alarm_hour_format_t hour_cov = func_alarm_convert_to_12hour(hour);
        hour = hour_cov.hour;
        am_pm = hour_cov.am_pm;
        char aclock_str[20] = {0};
        memset(aclock_str, '\0', sizeof(aclock_str));
        sprintf(aclock_str, "%02d:%02d", hour, min);

        if (ui_handle.wrist_end_time.text[i].idx == 2)
        {
            if(am_pm == 2)      //AM
            {
                compo_cardbox_text_set(wrist_end_time, ui_handle.wrist_end_time.text[i].idx, i18n[ui_handle.wrist_end_time.text[i].str_id]);
            }
            else if (am_pm == 1)
            {
                compo_cardbox_text_set(wrist_end_time, ui_handle.wrist_end_time.text[i].idx, i18n[ui_handle.wrist_end_time.text[i].rev]);
            }
            else
            {
//                compo_cardbox_text_set(wrist_end_time, ui_handle.wrist_end_time.text[i].idx, i18n[ui_handle.wrist_end_time.text[i].rev]);
            }
        }
        else if (ui_handle.wrist_end_time.text[i].idx == 1)
        {
            compo_cardbox_text_set(wrist_end_time, ui_handle.wrist_end_time.text[i].idx, aclock_str);
        }
        else if (ui_handle.wrist_end_time.text[i].idx == 0)
        {
            compo_cardbox_text_set(wrist_end_time, ui_handle.wrist_end_time.text[i].idx, i18n[ui_handle.wrist_end_time.text[i].str_id]);
        }
    }
    return frm;
}

//获取点击卡片的id
static u16 func_set_sub_wrist_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_ID_END-CARD_ID_START-1; i++)
    {
        id = CARD_ID_START + 1 + i;
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

//更新显示翻腕模式界面
static void func_set_sub_wrist_disp_update(void)
{
    compo_cardbox_t* cardbox_wrist_all = compo_getobj_byid(ui_handle.wrist_all_card.id);
    if (uteModuleLiftWristIsOpen() == 0)                              //全天翻腕关闭状态
    {
        compo_cardbox_icon_set(cardbox_wrist_all, ui_handle.wrist_all_card.icon[0].idx, ui_handle.wrist_all_card.icon[0].res_off);
    }
    else
    {
        compo_cardbox_icon_set(cardbox_wrist_all, ui_handle.wrist_all_card.icon[0].idx, ui_handle.wrist_all_card.icon[0].res_on);
    }

    compo_cardbox_t* cardbox_wrist_def = compo_getobj_byid(ui_handle.wrist_def_card.id);
    if(uteModuleLiftWristIsOpenScheduled() == 0)                             //定时翻腕关闭
    {
        compo_cardbox_icon_set(cardbox_wrist_def, ui_handle.wrist_def_card.icon[0].idx, ui_handle.wrist_def_card.icon[0].res_off);
        compo_cardbox_t* cardbox_start_time = compo_getobj_byid(ui_handle.wrist_start_time.id);
        compo_cardbox_set_visible(cardbox_start_time, false);
        compo_cardbox_t* cardbox_end_time = compo_getobj_byid(ui_handle.wrist_end_time.id);
        compo_cardbox_set_visible(cardbox_end_time, false);
    }
    else
    {
        compo_cardbox_icon_set(cardbox_wrist_def, ui_handle.wrist_def_card.icon[0].idx, ui_handle.wrist_def_card.icon[0].res_on);
        compo_cardbox_t* cardbox_start_time = compo_getobj_byid(ui_handle.wrist_start_time.id);
        compo_cardbox_set_visible(cardbox_start_time, true);
        compo_cardbox_t* cardbox_end_time = compo_getobj_byid(ui_handle.wrist_end_time.id);
        compo_cardbox_set_visible(cardbox_end_time, true);
    }

}

//单击按钮
static void func_wrist_card_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_set_sub_wrist_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > CARD_ID_END-1)
    {
        return;
    }

    compo_cardbox_t* cardbox = compo_getobj_byid(compo_id);
    if (compo_cardbox_get_visible(cardbox))
    {
        if (compo_id == ui_handle.wrist_all_card.id)              //全天翻腕
        {
            rect_t play_rect = compo_cardbox_get_icon_absolute(cardbox, 0);
            if (abs_s(pt.x - play_rect.x) * 2 <= (play_rect.wid + 20) && abs_s(pt.y - play_rect.y) * 2 <= (play_rect.hei + 20))
            {
                if (uteModuleLiftWristIsOpen() == false)
                {
                    //if (msgbox((char *)i18n[STR_WRIST_TIM_CTT], NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE) == MSGBOX_RES_OK)
                    {
                        uteModuleLiftWristSetOpenStatus(true);
                        uteModuleLiftWristSetScheduled(false);
                        uteModuleLiftWristSetNowOpenStatus(LIFT_WRIST_ALLDAY_OPEN);
                    }
                }
                else
                {
                    uteModuleLiftWristSetOpenStatus(false);
                    uteModuleLiftWristSetNowOpenStatus(LIFT_WRIST_CLOSE);
                }
#if 0//UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT
                if (uteModuleSettingRemindGetWhetherShowId(SETTING_REMIND_ID_WRIST))
                {
                    uteModuleSettingRemindSetRemindId(SETTING_REMIND_ID_WRIST);
                    uteTaskGuiStartScreen(FUNC_SETTING_REMIND, 0, __func__);
                }
#endif
                uteModuleLiftWristRecordOpenStatus();
                func_set_sub_wrist_state_update();
                func_set_sub_wrist_disp_update();
            }
        }
        else if (compo_id == ui_handle.wrist_def_card.id)         //定时翻腕
        {
            rect_t play_rect = compo_cardbox_get_icon_absolute(cardbox, 0);
            if (abs_s(pt.x - play_rect.x) * 2 <= (play_rect.wid + 20) && abs_s(pt.y - play_rect.y) * 2 <= (play_rect.hei + 20))
            {
                if (uteModuleLiftWristIsOpenScheduled() == false)
                {
                    //if (msgbox((char *)i18n[STR_WRIST_TIM_CTT], NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE) == MSGBOX_RES_OK)
                    {
                        uteModuleLiftWristSetScheduled(true);
                        uteModuleLiftWristSetOpenStatus(false);
                        uteModuleLiftWristSetNowOpenStatus(LIFT_WRIST_SCHEDULED_OPEN);
                    }
                }
                else
                {
                    uteModuleLiftWristSetScheduled(false);
                    uteModuleLiftWristSetNowOpenStatus(LIFT_WRIST_CLOSE);
                    //uteModuleNotDisturbScheduledSwitch();
                    uteModuleLiftWristSetTimeStatus(LIFT_WRIST_START_TIME);
                    uteModuleLiftWristSetTime(0x800);
                    uteModuleLiftWristSetTimeStatus(LIFT_WRIST_END_TIME);
                    uteModuleLiftWristSetTime(0x1600);
                    msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
                }
#if 0//UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT
                if (uteModuleSettingRemindGetWhetherShowId(SETTING_REMIND_ID_WRIST))
                {
                    uteModuleSettingRemindSetRemindId(SETTING_REMIND_ID_WRIST);
                    uteTaskGuiStartScreen(FUNC_SETTING_REMIND, 0, __func__);
                }
#endif
                uteModuleLiftWristRecordOpenStatus();
                func_set_sub_wrist_state_update();
                func_set_sub_wrist_disp_update();
            }
        }
        else if (compo_id == ui_handle.wrist_start_time.id)      //开始时间
        {
            if(uteModuleLiftWristIsOpenScheduled())
            {
                func_cb.sta = FUNC_WRIST_SUB_SET;
                uteModuleLiftWristSetTimeStatus(LIFT_WRIST_START_TIME);
            }
        }
        else if (compo_id == ui_handle.wrist_end_time.id)      //结束时间
        {
            if(uteModuleLiftWristIsOpenScheduled())
            {
                func_cb.sta = FUNC_WRIST_SUB_SET;
                uteModuleLiftWristSetTimeStatus(LIFT_WRIST_END_TIME);
            }
        }
    }
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    UI_BUF_I343001_28_SET_OPEN_BIN,         //ON
    UI_BUF_I343001_28_SET_CLOSE_BIN,        //OFF
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 1, 1, 1, 278, 100);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, 24 + 278/2, 60 + 100/2, 278, 100);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_rect_set_color(card, 0, COLOR_BLACK);
    compo_cardbox_rect_set_location(card, 0, 0, 0, 278, 100, 16);

    compo_cardbox_icon_set_location(card, 0,
                                    278/2 - 5 - gui_image_get_size(tbl_wrist_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_wrist_switch_res[0]).wid, gui_image_get_size(tbl_wrist_switch_res[0]).hei);

    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-278/2, -14,
                                    278 - gui_image_get_size(tbl_wrist_switch_res[0]).wid - 50,
                                    30);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_SETTING_UP]);

    return frm;
}


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    UI_BUF_I338001_28_SET_OPEN_BIN,         //ON
    UI_BUF_I338001_28_SET_CLOSE_BIN,         //OFF
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,80/2+102,324, 80);
    compo_setid(cardbox, COMPO_CARD_1);
    compo_cardbox_rect_set_location(cardbox,0,0,30,278, 1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, 25-324/2,  -(34/2), 200, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_SETTING_UP]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 116, 0, 50, 30);
    compo_cardbox_icon_set(cardbox,0,uteModuleSportGetIsOpenHandScreenOn() ? tbl_wrist_switch_res[0] : tbl_wrist_switch_res[1]);
    compo_cardbox_text_scroll_process(cardbox, true);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    UI_BUF_I332001_PUBLIC_SWITCH02_BIN,         //ON
    UI_BUF_I332001_PUBLIC_SWITCH00_BIN,         //OFF
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, GUI_SCREEN_CENTER_X, 80/2+102, 324, 80);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_rect_set_color(card, 0, make_color(41,41,41));
    compo_cardbox_rect_set_location(card, 0, 0, 0, 324, 80, 10);

    compo_cardbox_icon_set_location(card, 0,
                                    254/2-36 + gui_image_get_size(tbl_wrist_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_wrist_switch_res[0]).wid, gui_image_get_size(tbl_wrist_switch_res[0]).hei);

    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-324/2,-(34/2), 200, 30);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_SETTING_UP]);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    UI_BUF_I340001_PUBLIC_SWITCH02_BIN,         //ON
    UI_BUF_I340001_PUBLIC_SWITCH00_BIN,         //OFF
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, GUI_SCREEN_CENTER_X, 80/2+102, 324, 80);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_rect_set_color(card, 0, make_color(41,41,41));
    compo_cardbox_rect_set_location(card, 0, 0, 0, 324, 80, 10);

    compo_cardbox_icon_set_location(card, 0,
                                    254/2-36 + gui_image_get_size(tbl_wrist_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_wrist_switch_res[0]).wid, gui_image_get_size(tbl_wrist_switch_res[0]).hei);

    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-324/2,-(34/2), 200, 30);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_SETTING_UP]);

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT


typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    UI_BUF_I342001_28_SET_OPEN_BIN,         //OFF
    UI_BUF_I342001_28_SET_CLOSE_BIN,         //ON
};

static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_UP]);

    //创建卡片
    compo_cardbox_t * card = compo_cardbox_create(frm, 0, 1, 1, 232, 72);
    compo_cardbox_set_visible(card, true);
    compo_cardbox_set_location(card, 4 + 232/2, 54 + 72/2, 232, 72);
    compo_setid(card, COMPO_CARD_1);

    compo_cardbox_icon_set_location(card, 0,
                                    232/2 - 5 - gui_image_get_size(tbl_wrist_switch_res[0]).wid/2,
                                    0, gui_image_get_size(tbl_wrist_switch_res[0]).wid, gui_image_get_size(tbl_wrist_switch_res[0]).hei);

    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }

    compo_cardbox_text_set_font(card, 0, UI_BUF_0FONT_FONT_BIN);
    compo_cardbox_text_set_location(card, 0, 10-232/2, -14,
                                    232 - gui_image_get_size(tbl_wrist_switch_res[0]).wid - 50,
                                    30);
    compo_cardbox_text_set_align_center(card, 0, false);
    widget_text_set_color(card->text[0], make_color(255,255,255));
    compo_cardbox_text_set(card, 0, i18n[STR_SETTING_UP]);

    return frm;
}


#else
typedef struct f_wrist_t_
{
    bool value;
} f_wrist_t;

enum
{
    //card
    COMPO_CARD_START = 1,
    COMPO_CARD_1,
    COMPO_CARD_END,
};

static const u32 tbl_wrist_switch_res[] =
{
    0,         //ON
    0,         //OFF
};

static void switch_set_sub_wrist(void)
{
}


//抬腕亮屏页面
compo_form_t *func_set_sub_wrist_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//抬腕亮屏事件处理
static void func_set_sub_wrist_process(void)
{
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_wrist_t* f_wrist = (f_wrist_t*)func_cb.f_cb;
    if (f_wrist->flag_drag)
    {
//        s32 dx = f_wrist->focus_dx;
//        s32 dy = f_wrist->focus_dy;
        f_wrist->flag_drag = ctp_get_dxy(&f_wrist->focus_dx, &f_wrist->focus_dy);
        if (f_wrist->flag_drag)
        {
            //拖动菜单图标
//            f_wrist->focus_xstep = f_wrist->focus_dx - dx;
//            f_wrist->focus_ystep = f_wrist->focus_dy - dy;
            f_wrist->focus_ofsx = 0;
            f_wrist->focus_ofsy = f_wrist->focus_y - f_wrist->focus_dy;

            if (uteModuleLiftWristIsOpenScheduled())
            {
                if (f_wrist->focus_ofsy > DRAG_MAX_DISTANCE)
                {
                    f_wrist->focus_ofsy = DRAG_MAX_DISTANCE;
                }
            }
            else
            {
                if (f_wrist->focus_ofsy > DRAG_MAX_DISTANCE1)
                {
                    f_wrist->focus_ofsy = DRAG_MAX_DISTANCE1;
                }
            }

            if (f_wrist->focus_ofsy < DRAG_MIN_DISTANCE)
            {
                f_wrist->focus_ofsy = DRAG_MIN_DISTANCE;
            }

            widget_page_set_client(func_cb.frm_main->page_body, f_wrist->focus_ofsx, -f_wrist->focus_ofsy);

//            printf("f_wrist->focus_ofsy =%d\n", f_wrist->focus_ofsy);
        }
        else
        {
            //抬手后开始自动移动
            point_t last_dxy = ctp_get_last_dxy();
            s32 to_x;
            s32 to_y;
            f_wrist->focus_x = f_wrist->focus_ofsx;
            f_wrist->focus_y = f_wrist->focus_ofsy;
            f_wrist->flag_move_auto = true;
            to_x = 0;
            to_y = f_wrist->focus_y - (last_dxy.y * DRAG_AUTO_SPEED);
            f_wrist->moveto.x = to_x;
            f_wrist->moveto.y = to_y;
            if (uteModuleLiftWristIsOpenScheduled() == 1)
            {
                if (f_wrist->moveto.y > DRAG_MAX_DISTANCE)
                {
                    f_wrist->moveto.y = DRAG_MAX_DISTANCE;
                }
            }
            else
            {
                if (f_wrist->moveto.y > DRAG_MAX_DISTANCE1)
                {
                    f_wrist->moveto.y = DRAG_MAX_DISTANCE1;
                }
            }

            if (f_wrist->moveto.y < DRAG_MIN_DISTANCE)
            {
                f_wrist->moveto.y = DRAG_MIN_DISTANCE;
            }
            f_wrist->tick = tick_get();
        }
    }
    else if (f_wrist->flag_move_auto)
    {
        //自动移动
        if (f_wrist->focus_x == f_wrist->moveto.x && f_wrist->focus_y == f_wrist->moveto.y)
        {
            if (uteModuleLiftWristIsOpenScheduled())
            {
                if (f_wrist->focus_y < DRAG_MIN_BACK_DISTANCE)
                {
                    f_wrist->moveto.y = DRAG_MIN_BACK_DISTANCE;
                }
                else if (f_wrist->focus_y > DRAG_MAX_BACK_DISTANCE)
                {
                    f_wrist->moveto.y = DRAG_MAX_BACK_DISTANCE;
                }
                else
                {
                    f_wrist->flag_move_auto = false;            //移动完成
                }
            }
            else
            {
                if (f_wrist->focus_y < DRAG_MIN_BACK_DISTANCE)
                {
                    f_wrist->moveto.y = DRAG_MIN_BACK_DISTANCE;
                }
                else if (f_wrist->focus_y > DRAG_MIN_BACK_DISTANCE)
                {
                    f_wrist->moveto.y = DRAG_MIN_BACK_DISTANCE;
                }
                else
                {
                    f_wrist->flag_move_auto = false;            //移动完成
                }
            }
        }
        else if (tick_check_expire(f_wrist->tick, 10))
        {
            TRACE("[%d %d]--move_to->[%d %d]\n", f_wrist->focus_x, f_wrist->focus_y, f_wrist->moveto.x, f_wrist->moveto.y);
            s16 dx, dy;
            f_wrist->tick = tick_get();
            dx = 0;
            dy = f_wrist->moveto.y - f_wrist->focus_y;
            if (dy > 0)
            {
                if (dy > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy > FOCUS_AUTO_STEP)
                {
                    dy = FOCUS_AUTO_STEP;
                }
                else
                {
                    dy = 1;
                }
            }
            else if (dy < 0)
            {
                if (dy < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy < -FOCUS_AUTO_STEP)
                {
                    dy = -FOCUS_AUTO_STEP;
                }
                else
                {
                    dy = -1;
                }
            }
            f_wrist->focus_x += dx;
            f_wrist->focus_y += dy;
            widget_page_set_client(func_cb.frm_main->page_body, f_wrist->focus_x, -f_wrist->focus_y);
        }
    }


    func_set_sub_wrist_disp_update();

    //卡片文本滚动
    for(u8 i=0; i<CARD_ID_END - CARD_ID_START - 1; i++)      //文本滚动
    {
        u16 id = CARD_ID_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }

#else
    compo_cardbox_t * card = compo_getobj_byid(COMPO_CARD_1);
    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[0]);
    }
    else
    {
        compo_cardbox_icon_set(card, 0, tbl_wrist_switch_res[1]);
    }


    for(u8 i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)      //文本滚动
    {
        u16 id = COMPO_CARD_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
#endif
    func_process();
}

#if !GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
//获取点击卡片的id
static u16 func_wrist_card_get_id(point_t pt)
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
static void func_wrist_button_click(void)
{
//    u8 ret = 0;
//    f_wrist_t *wrs = (f_wrist_t *)func_cb.f_cb;

    point_t pt = ctp_get_sxy();
    u16 compo_id = func_wrist_card_get_id(pt);
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
            switch_set_sub_wrist();
            if(uteModuleSportGetIsOpenHandScreenOn())
            {
                compo_cardbox_icon_set(cardbox, 0, tbl_wrist_switch_res[0]);
            }
            else
            {
                compo_cardbox_icon_set(cardbox, 0, tbl_wrist_switch_res[1]);
            }
        }
    }

}
#endif

//抬腕亮屏功能消息处理
static void func_set_sub_wrist_message(size_msg_t msg)
{
    f_wrist_t *f_wrist = (f_wrist_t *)func_cb.f_cb;
    switch (msg)
    {
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case MSG_CTP_CLICK:
//            if ((uteModuleNotDisturbGetOpenStatus() == NOT_DISTURB_ALLDAY_OPEN) || uteModuleNotDisturbIsTimeBucket() \
//                || uteModuleGuiCommonGetPowerSavingModeOpen())
//                break;
            func_wrist_card_click();
            break;
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
            f_wrist->flag_drag = true;
            f_wrist->flag_move_auto = true;
            break;
        case MSG_SYS_500MS:
            if (f_wrist->time_scale != uteModuleSystemtime12HOn())
            {
                if (func_cb.frm_main != NULL)
                {
                    compo_form_destroy(func_cb.frm_main);
                }
                func_set_sub_wrist_exit();
                if (func_cb.f_cb != NULL)
                {
                    func_free(func_cb.f_cb);
                    func_cb.f_cb = NULL;
                }
                func_set_sub_wrist_enter();
            }
            break;
#else
        case MSG_CTP_CLICK:
            func_wrist_button_click();
            break;
#endif

        default:
            func_message(msg);
            break;
    }
}

//进入抬腕亮屏功能
static void func_set_sub_wrist_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_wrist_t));
    func_cb.frm_main = func_set_sub_wrist_form_create();

    //初始化变量
    f_wrist_t *wrs = (f_wrist_t *)func_cb.f_cb;

    if(uteModuleSportGetIsOpenHandScreenOn())
    {
        wrs->value = true;
    }
    else
    {
        wrs->value = false;
    }
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    wrs->focus_y = DRAG_MIN_BACK_DISTANCE;
    widget_page_set_client(func_cb.frm_main->page_body, 0, -wrs->focus_y);
    wrs->time_scale = uteModuleSystemtime12HOn();
#endif
}

//退出抬腕亮屏功能
static void func_set_sub_wrist_exit(void)
{
    func_cb.last = FUNC_SET_SUB_WRIST;
}

//抬腕亮屏功能
void func_set_sub_wrist(void)
{
    printf("%s\n", __func__);
    func_set_sub_wrist_enter();
    while (func_cb.sta == FUNC_SET_SUB_WRIST)
    {
        func_set_sub_wrist_process();
        func_set_sub_wrist_message(msg_dequeue());
    }
    func_set_sub_wrist_exit();
}
