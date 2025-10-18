#include "include.h"
#include "func.h"

#if UTE_MODULE_SCREENS_CALL_ABOUT_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
typedef struct f_call_about_t_
{
    page_tp_move_t *ptm;
    compo_textbox_t *txt_page;
} f_call_about_t;


#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
compo_form_t *func_call_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_title(frm, i18n[STR_ABOUT_CALL]);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_textbox_t *textbox;
//  textbox = compo_textbox_create(frm, strlen(i18n[STR_MESSAGE]));
//  compo_textbox_set_align_center(textbox, false);
//  compo_textbox_set_location(textbox, 24, 28, 230, 50);
//  compo_textbox_set(textbox,i18n[STR_MESSAGE]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_TIP1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,20, 67, 328, 300);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_TIP1]);
    compo_textbox_set_location(textbox,20, 67, 328, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_TIP1]);

    if(func_cb.sta == FUNC_CALL_ABOUT)
    {
        f_call_about_t *f_scan_about = (f_call_about_t *)func_cb.f_cb;
        f_scan_about->txt_page = textbox;
    }


    return frm;
}

static void func_call_about_message(size_msg_t msg)
{
    f_call_about_t *f_scan_about= (f_call_about_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_scan_about->ptm)
            {
                compo_page_move_touch_handler(f_scan_about->ptm);
            }
            break;
        default:
            func_message(msg);
            break;
    }
}
//下滑事件流程处理
static void func_call_about_process(void)
{
    f_call_about_t *f_scan_about = (f_call_about_t *)func_cb.f_cb;
    if(f_scan_about->ptm)
    {
        compo_page_move_process(f_scan_about->ptm);
    }
    func_process();
}
#endif

//创建界面
static void func_call_about_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_call_about_t));
    func_cb.frm_main = func_call_about_form_create();
    f_call_about_t *f_scan_about = (f_call_about_t *)func_cb.f_cb;
    f_scan_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = widget_text_get_area(f_scan_about->txt_page->txt).hei + 80,
        .page_count = 1,
        .quick_jump_perc =10,
    };
#endif
    compo_page_move_init(f_scan_about->ptm, func_cb.frm_main->page_body, &info);
}
//退出任务
static void func_call_about_exit(void)
{
    f_call_about_t *f_scan_about = (f_call_about_t *)func_cb.f_cb;
    if (f_scan_about->ptm)
    {
        func_free(f_scan_about->ptm);
    }
    func_cb.last = FUNC_CALL_ABOUT;
}

//说明功能
void func_call_about(void)
{
    printf("%s\n", __func__);
    func_call_about_enter();
    while (func_cb.sta == FUNC_CALL_ABOUT)
    {
        func_call_about_process();
        func_call_about_message(msg_dequeue());
    }
    func_call_about_exit();
}

#endif

