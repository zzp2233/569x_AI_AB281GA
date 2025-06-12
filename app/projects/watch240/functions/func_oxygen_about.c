#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_INFO_SUPPORT

typedef struct f_oxygen_about_t_
{
    page_tp_move_t *ptm;
    u16 page_size;
} f_oxygen_about_t;


#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
compo_form_t *func_oxygen_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    u16 page_size=0;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
    compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OXYGEN_ABOUT_1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, 43, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_1]);
    compo_textbox_set_location(textbox,10, 43, 220, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_1]);
    page_size=widget_text_get_area(textbox->txt).hei+43;

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_7_SPO2_3_00_106X86_X16_Y205_BIN);
    compo_picturebox_set_pos(picbox,gui_image_get_size(UI_BUF_I335001_7_SPO2_3_00_106X86_X16_Y205_BIN).wid/2+10, widget_text_get_area(textbox->txt).hei+(gui_image_get_size(UI_BUF_I335001_7_SPO2_3_00_106X86_X16_Y205_BIN).hei)+5);
    page_size+=(gui_image_get_size(UI_BUF_I335001_7_SPO2_3_00_106X86_X16_Y205_BIN).hei+10);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OXYGEN_ABOUT_2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, page_size, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_2]);
    compo_textbox_set_location(textbox,10, page_size, 220, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_2]);
    page_size+=widget_text_get_area(textbox->txt).hei+20;

    if(func_cb.sta == FUNC_OXYGEN_ABOUT)
    {
        f_oxygen_about_t *f_oxygen_about = (f_oxygen_about_t *)func_cb.f_cb;
        f_oxygen_about->page_size = page_size;
    }

    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
compo_form_t *func_oxygen_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    u16 page_size=0;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 40);
    compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OXYGEN_ABOUT_1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, 43, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_1]);
    compo_textbox_set_location(textbox,10, 69, 348, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_1]);
    page_size=widget_text_get_area(textbox->txt).hei+48;

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_SPO2_ABOUT_BIN);
    compo_picturebox_set_pos(picbox,gui_image_get_size(UI_BUF_I341001_7_SPO2_SPO2_ABOUT_BIN).wid/2+20, widget_text_get_area(textbox->txt).hei+(gui_image_get_size(UI_BUF_I341001_7_SPO2_SPO2_ABOUT_BIN).hei)+10);
    page_size+=(gui_image_get_size(UI_BUF_I341001_7_SPO2_SPO2_ABOUT_BIN).hei+40);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OXYGEN_ABOUT_2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, page_size, 348, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_2]);
    compo_textbox_set_location(textbox,10, page_size, 348, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_2]);
    page_size+=widget_text_get_area(textbox->txt).hei+30;

    if(func_cb.sta == FUNC_OXYGEN_ABOUT)
    {
        f_oxygen_about_t *f_oxygen_about = (f_oxygen_about_t *)func_cb.f_cb;
        f_oxygen_about->page_size = page_size;
    }

    return frm;
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

compo_form_t *func_oxygen_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    u16 page_size=0;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 17, 230, 40);
    compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OXYGEN_ABOUT_1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,16, 60, 290, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_1]);
    compo_textbox_set_location(textbox,16, 60, 290, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_1]);
    page_size=widget_text_get_area(textbox->txt).hei+48;

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I343001_6_HEART_ABOUT_BIN);
    compo_picturebox_set_pos(picbox,gui_image_get_size(UI_BUF_I343001_6_HEART_ABOUT_BIN).wid/2+20, widget_text_get_area(textbox->txt).hei+(gui_image_get_size(UI_BUF_I343001_6_HEART_ABOUT_BIN).hei)+10);
    page_size+=(gui_image_get_size(UI_BUF_I343001_6_HEART_ABOUT_BIN).hei+40);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OXYGEN_ABOUT_2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,16, page_size, 290, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_2]);
    compo_textbox_set_location(textbox,16, page_size, 290, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_2]);
    page_size+=widget_text_get_area(textbox->txt).hei+30;

    if(func_cb.sta == FUNC_OXYGEN_ABOUT)
    {
        f_oxygen_about_t *f_oxygen_about = (f_oxygen_about_t *)func_cb.f_cb;
        f_oxygen_about->page_size = page_size;
    }

    return frm;
}


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
compo_form_t *func_oxygen_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    u16 page_size=0;

    // compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));
    // compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
    // compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);
    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_OXYGEN_ABOUT_1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,58, 60, 255, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_1]);
    compo_textbox_set_location(textbox,58, 60, 255, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_1]);
    page_size=widget_text_get_area(textbox->txt).hei+60;

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338001_7_SPO2_ABOUT_DATE_BIN);
    compo_picturebox_set_pos(picbox,gui_image_get_size(UI_BUF_I338001_7_SPO2_ABOUT_DATE_BIN).wid/2+58, widget_text_get_area(textbox->txt).hei+(gui_image_get_size(UI_BUF_I338001_7_SPO2_ABOUT_DATE_BIN).hei)+22);
    page_size+=(gui_image_get_size(UI_BUF_I338001_7_SPO2_ABOUT_DATE_BIN).hei+40);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_OXYGEN_ABOUT_2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,58, page_size, 255, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_2]);
    compo_textbox_set_location(textbox,58, page_size, 255, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_OXYGEN_ABOUT_2]);
    page_size+=widget_text_get_area(textbox->txt).hei+20;

    // printf("oxyge_about:%d\n",strlen(i18n[STR_OXYGEN_ABOUT_2])+strlen(i18n[STR_OXYGEN_ABOUT_1]));

    if(func_cb.sta == FUNC_OXYGEN_ABOUT)
    {
        f_oxygen_about_t *f_oxygen_about = (f_oxygen_about_t *)func_cb.f_cb;
        f_oxygen_about->page_size = page_size+40;
    }

    return frm;
}
#else
compo_form_t *func_oxygen_about_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
#endif


//时钟表盘右滑菜单功能消息处理
static void func_oxygen_about_message(size_msg_t msg)
{
    f_oxygen_about_t *f_oxygen_about= (f_oxygen_about_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_oxygen_about->ptm)
            {
                compo_page_move_touch_handler(f_oxygen_about->ptm);
            }
            break;
        // case MSG_CTP_SHORT_RIGHT:
        //     func_switch_to(FUNC_BLOOD_OXYGEN,FUNC_SWITCH_LR_ZOOM_RIGHT);
        //     break;
        case MSG_CTP_SHORT_LEFT:
            break;

        default:
            func_message(msg);
            break;
    }
}
//下滑事件流程处理
static void func_oxygen_about_process(void)
{
    f_oxygen_about_t *f_oxygen_about = (f_oxygen_about_t *)func_cb.f_cb;
    if(f_oxygen_about->ptm)
    {
        compo_page_move_process(f_oxygen_about->ptm);
    }
    func_process();
}

//创建界面
static void func_oxygen_about_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_oxygen_about_t));
    func_cb.frm_main = func_oxygen_about_form_create();
    f_oxygen_about_t *f_oxygen_about = (f_oxygen_about_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    f_oxygen_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .page_size =  f_oxygen_about->page_size,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_oxygen_about->ptm, func_cb.frm_main->page_body, &info);
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_oxygen_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  f_oxygen_about->page_size,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_oxygen_about->ptm, func_cb.frm_main->page_body, &info);
#endif
}
//退出任务
static void func_oxygen_about_exit(void)
{
    f_oxygen_about_t *f_oxygen_about = (f_oxygen_about_t *)func_cb.f_cb;
    if (f_oxygen_about->ptm)
    {
        func_free(f_oxygen_about->ptm);
    }
    func_cb.last = FUNC_OXYGEN_ABOUT;
}

//血氧功能
void func_oxygen_about(void)
{
    printf("%s\n", __func__);
    func_oxygen_about_enter();
    while (func_cb.sta == FUNC_OXYGEN_ABOUT)
    {
        func_oxygen_about_process();
        func_oxygen_about_message(msg_dequeue());
    }
    func_oxygen_about_exit();
}

#endif

