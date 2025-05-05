#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_EMOTION_ABOUT_SUPPORT

typedef struct f_mood_about_t_
{
    page_tp_move_t *ptm;
    u16 page_size;
} f_mood_about_t;


#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
compo_form_t *func_mood_about_form_create(void)
{

    u16 str_id[] =
    {
        STR_POSITIVE, // 积极
        STR_CALM, // 平和
        STR_NEGATIVE, // 消极
    };
    u32 res_addr[] =
    {
        UI_BUF_I335001_15_EMOTIONS_3_EMOTIONAL_DESCRIPTION_ICON_8X8_X16_Y356_Y392_Y428_00_BIN, // 积极
        UI_BUF_I335001_15_EMOTIONS_3_EMOTIONAL_DESCRIPTION_ICON_8X8_X16_Y356_Y392_Y428_01_BIN, // 平和
        UI_BUF_I335001_15_EMOTIONS_3_EMOTIONAL_DESCRIPTION_ICON_8X8_X16_Y356_Y392_Y428_02_BIN, // 消极
    };

    compo_form_t *frm = compo_form_create(true);
    u16 page_size=0;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_EMOTION]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
    compo_textbox_set(textbox,i18n[STR_EMOTION]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_MOOD_EXPLAIN1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, 43, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_MOOD_EXPLAIN1]);
    compo_textbox_set_location(textbox,10, 43, 220, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_MOOD_EXPLAIN1]);
    page_size=widget_text_get_area(textbox->txt).hei+60;

    for (int i = 0; i < 3; i++)
    {
        textbox = compo_textbox_create(frm, strlen(i18n[str_id[i]]));
        compo_textbox_set_align_center(textbox,false);
        compo_textbox_set_location(textbox,20, page_size, 210, widget_text_get_max_height());
        compo_textbox_set(textbox,i18n[str_id[i]]);

        compo_picturebox_t *picbox = compo_picturebox_create(frm, res_addr[i]);
        compo_picturebox_set_pos(picbox,14,page_size+13);
        page_size+=40;
    }
    page_size+=20;
    textbox = compo_textbox_create(frm, strlen(i18n[STR_MOOD_EXPLAIN2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, page_size, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_MOOD_EXPLAIN2]);
    compo_textbox_set_location(textbox,10, page_size, 220, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_MOOD_EXPLAIN2]);
    page_size+=widget_text_get_area(textbox->txt).hei+20;

    if(func_cb.sta == FUNC_MOOD_ABOUT)
    {
        f_mood_about_t *f_mood_about = (f_mood_about_t *)func_cb.f_cb;
        f_mood_about->page_size = page_size;
    }

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
compo_form_t *func_mood_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    u16 page_size=0;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_EMOTION]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
    compo_textbox_set(textbox,i18n[STR_EMOTION]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_MOOD_EXPLAIN1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,58, 43, 244, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_MOOD_EXPLAIN1]);
    compo_textbox_set_location(textbox,58, 43, 244, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_MOOD_EXPLAIN1]);
    page_size=widget_text_get_area(textbox->txt).hei+43;

    compo_picturebox_t *picbox = compo_picturebox_create(frm, 0);
    compo_picturebox_set_pos(picbox,gui_image_get_size(0).wid/2+58, widget_text_get_area(textbox->txt).hei+(gui_image_get_size(0).hei)+5);
    page_size+=(gui_image_get_size(0).hei+10);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_MOOD_EXPLAIN2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,58, page_size, 244, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_MOOD_EXPLAIN2]);
    compo_textbox_set_location(textbox,58, page_size, 244, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_MOOD_EXPLAIN2]);
    page_size+=widget_text_get_area(textbox->txt).hei+20;

    if(func_cb.sta == FUNC_MOOD_ABOUT)
    {
        f_mood_about_t *f_mood_about = (f_mood_about_t *)func_cb.f_cb;
        f_mood_about->page_size = page_size+40;
    }

    return frm;
}
#else
compo_form_t *func_mood_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
#endif


//时钟表盘右滑菜单功能消息处理
static void func_mood_about_message(size_msg_t msg)
{
    f_mood_about_t *f_mood_about= (f_mood_about_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_mood_about->ptm)
            {
                compo_page_move_touch_handler(f_mood_about->ptm);
            }
            break;
        case MSG_CTP_SHORT_RIGHT:
            func_switch_to(FUNC_MOOD,FUNC_SWITCH_LR_ZOOM_RIGHT);
            break;
        case MSG_CTP_SHORT_LEFT:
            break;

        default:
            func_message(msg);
            break;
    }
}
//下滑事件流程处理
static void func_mood_about_process(void)
{
    f_mood_about_t *f_mood_about = (f_mood_about_t *)func_cb.f_cb;
    if(f_mood_about->ptm)
    {
        compo_page_move_process(f_mood_about->ptm);
    }
    func_process();
}

//创建界面
static void func_mood_about_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_mood_about_t));
    func_cb.frm_main = func_mood_about_form_create();
    f_mood_about_t *f_mood_about = (f_mood_about_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
    f_mood_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .page_size =  f_mood_about->page_size,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_mood_about->ptm, func_cb.frm_main->page_body, &info);
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_mood_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .page_size =  f_mood_about->page_size,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_mood_about->ptm, func_cb.frm_main->page_body, &info);
#endif
}
//退出任务
static void func_mood_about_exit(void)
{
    f_mood_about_t *f_mood_about = (f_mood_about_t *)func_cb.f_cb;
    if (f_mood_about->ptm)
    {
        func_free(f_mood_about->ptm);
    }
    func_cb.last = FUNC_MOOD_ABOUT;
}

//血氧功能
void func_mood_about(void)
{
    printf("%s\n", __func__);
    func_mood_about_enter();
    while (func_cb.sta == FUNC_MOOD_ABOUT)
    {
        func_mood_about_process();
        func_mood_about_message(msg_dequeue());
    }
    func_mood_about_exit();
}

#endif

