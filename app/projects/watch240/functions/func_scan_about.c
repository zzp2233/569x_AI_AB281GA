#include "include.h"
#include "func.h"

#if UTE_MODULE_SCREENS_SCAN_ABOUT_SUPPORT

typedef struct f_scan_about_t_
{
    page_tp_move_t *ptm;
    u16 hei_page;
} f_scan_about_t;

enum
{
    COMPO_ID_BTN_SKIP = 1,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
compo_form_t *func_scan_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_ICON);
    compo_form_set_title_icon(frm, UI_BUF_I341001_1_START_RETURN_BIN);
    compo_textbox_t *title = compo_textbox_create_for_page(frm, frm->page, strlen(i18n[STR_DECLARE]));
    compo_textbox_set_align_center(title, false);
    compo_textbox_set_location(title, FORM_TITLE_LEFT + 21, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 4 - FORM_TITLE_LEFT, 50);
    compo_textbox_set(title, i18n[STR_DECLARE]);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, 35, 33, 70, 66);
    compo_setid(btn, COMPO_ID_BTN_SKIP);

    compo_textbox_t *textbox;
    u16 hei = 0;
    textbox = compo_textbox_create(frm, strlen(i18n[STR_BINDING_ABOUT]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox, 20, 67, 328, 300);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox, i18n[STR_BINDING_ABOUT]);
    compo_textbox_set_location(textbox, 20, 67, 328, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox, i18n[STR_BINDING_ABOUT]);
    hei += (70 + widget_text_get_area(textbox->txt).hei);

    if(func_cb.sta == FUNC_SCAN_ABOUT)
    {
        f_scan_about_t *f_scan_about = (f_scan_about_t *)func_cb.f_cb;
        f_scan_about->hei_page = hei;
    }


    return frm;
}
#endif

static void func_scan_about_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SKIP:
            uteTaskGuiStartScreen(FUNC_POWER_ON_SCAN, 0, __func__);
            break;

        default:
            break;
    }
}

static void func_scan_about_message(size_msg_t msg)
{
    f_scan_about_t *f_scan_about= (f_scan_about_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_scan_about_click_handler();
            break;
        case MSG_CTP_TOUCH:
            if(f_scan_about->ptm)
            {
                compo_page_move_touch_handler(f_scan_about->ptm);
            }
            break;

        case KU_BACK:
        case KU_DELAY_BACK:
        case MSG_CTP_SHORT_RIGHT:
            uteTaskGuiStartScreen(FUNC_POWER_ON_SCAN, 0, __func__);
            break;
    }
}
//下滑事件流程处理
static void func_scan_about_process(void)
{
    f_scan_about_t *f_scan_about = (f_scan_about_t *)func_cb.f_cb;
    if(f_scan_about->ptm)
    {
        compo_page_move_process(f_scan_about->ptm);
    }
    func_process();
}

//创建界面
static void func_scan_about_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_scan_about_t));
    func_cb.frm_main = func_scan_about_form_create();
    f_scan_about_t *f_scan_about = (f_scan_about_t *)func_cb.f_cb;
    f_scan_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = f_scan_about->hei_page,
        .page_count = 1,
        .quick_jump_perc =10,
    };
#endif
    compo_page_move_init(f_scan_about->ptm, func_cb.frm_main->page_body, &info);
}
//退出任务
static void func_scan_about_exit(void)
{
    f_scan_about_t *f_scan_about = (f_scan_about_t *)func_cb.f_cb;
    if (f_scan_about->ptm)
    {
        func_free(f_scan_about->ptm);
    }
    func_cb.last = FUNC_SCAN_ABOUT;
}

//说明功能
void func_scan_about(void)
{
    printf("%s\n", __func__);
    func_scan_about_enter();
    while (func_cb.sta == FUNC_SCAN_ABOUT)
    {
        func_scan_about_process();
        func_scan_about_message(msg_dequeue());
    }
    func_scan_about_exit();
}

#endif

