#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_PRESSURE_ABOUT_SUPPORT

typedef struct f_pressure_explain_t_
{
    page_tp_move_t *ptm;
    u16 page_size;
} f_pressure_explain_t;


#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
compo_form_t *func_pressure_explain_form_create(void)
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

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_STRESS]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
    compo_textbox_set(textbox,i18n[STR_STRESS]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, 43, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN1]);
    compo_textbox_set_location(textbox,10, 43, 220, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN1]);
    page_size=widget_text_get_area(textbox->txt).hei+60;

    u32 res_addre = 0;
    if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
    {
        res_addre = UI_BUF_I335001_16_PRESSURE_3_PRESSURE_DESCRIPTION_CHAR_TEXT2_146X126_X16_Y323_00_TEXT2_CH_BIN;
    }
    else
    {
        res_addre = UI_BUF_I335001_16_PRESSURE_3_PRESSURE_DESCRIPTION_CHAR_TEXT2_146X126_X16_Y323_01_TEXT2_EN_BIN;
    }

    compo_picturebox_t *picbox = compo_picturebox_create(frm,res_addre );
    compo_picturebox_set_pos(picbox,gui_image_get_size(res_addre).wid/2+10, widget_text_get_area(textbox->txt).hei+(gui_image_get_size(res_addre).hei)+5);
    page_size+=(gui_image_get_size(res_addre).hei+30);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, page_size, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN2]);
    compo_textbox_set_location(textbox,10, page_size, 220, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN2]);
    page_size+=widget_text_get_area(textbox->txt).hei+20;

    if(func_cb.sta == FUNC_PRESSURE_EXPLAIN)
    {
        f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
        f_pressure_explain->page_size = page_size;
    }

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

compo_form_t *func_pressure_explain_form_create(void)
{

    u16 str_id[] =
    {
        STR_POSITIVE, // 积极
        STR_CALM, // 平和
        STR_NEGATIVE, // 消极
    };
    u32 res_addr[] =
    {
        UI_BUF_I342001_MOOD_ICON_POSTIVE_BIN, // 积极
        UI_BUF_I342001_MOOD_ICON_CALM_BIN, // 平和
        UI_BUF_I342001_MOOD_ICON_NEGATIVE_BIN, // 消极
    };

    compo_form_t *frm = compo_form_create(true);
    u16 page_size=0;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_STRESS]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
    compo_textbox_set(textbox,i18n[STR_STRESS]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, 43, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN1]);
    compo_textbox_set_location(textbox,10, 43, 220, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN1]);
    page_size=widget_text_get_area(textbox->txt).hei+60;

    u32 res_addre = 0;
    if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
    {
        res_addre = 0;
    }
    else
    {
        res_addre = 0;
    }

    compo_picturebox_t *picbox = compo_picturebox_create(frm,res_addre );
    compo_picturebox_set_pos(picbox,gui_image_get_size(res_addre).wid/2+10, widget_text_get_area(textbox->txt).hei+(gui_image_get_size(res_addre).hei)+5);
    page_size+=(gui_image_get_size(res_addre).hei+30);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, page_size, 220, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN2]);
    compo_textbox_set_location(textbox,10, page_size, 220, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN2]);
    page_size+=widget_text_get_area(textbox->txt).hei+20;

    if(func_cb.sta == FUNC_PRESSURE_EXPLAIN)
    {
        f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
        f_pressure_explain->page_size = page_size;
    }

    return frm;
}


#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
compo_form_t *func_pressure_explain_form_create(void)
{

    u16 str_id[] =
    {
        STR_RELAXED, // 放松
        STR_NORMAL,  // 普通
        STR_MEDIUM,  // 中
        STR_HIGH,    // 高
    };
    u8 str_data[4][10] =
    {
        "1-29", // 放松
        "30-59",  // 普通
        "60-79",  // 中
        "80-100",    // 高
    };
    /*u32 res_addr[] =
    {
        UI_BUF_I335001_15_EMOTIONS_3_EMOTIONAL_DESCRIPTION_ICON_8X8_X16_Y356_Y392_Y428_00_BIN, // 积极
        UI_BUF_I335001_15_EMOTIONS_3_EMOTIONAL_DESCRIPTION_ICON_8X8_X16_Y356_Y392_Y428_01_BIN, // 平和
        UI_BUF_I335001_15_EMOTIONS_3_EMOTIONAL_DESCRIPTION_ICON_8X8_X16_Y356_Y392_Y428_02_BIN, // 消极
    };*/

    compo_form_t *frm = compo_form_create(true);
    u16 page_size=0;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_STRESS]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 20, 230, 30);
    compo_textbox_set(textbox,i18n[STR_STRESS]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, 43, 348, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN1]);
    compo_textbox_set_location(textbox,10, 43, 348, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN1]);
    page_size=widget_text_get_area(textbox->txt).hei+60;

    u32 res_addre = UI_BUF_I341001_16_PRESSURE_EXPALIN_BIN;
    //if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)

    compo_picturebox_t *picbox = compo_picturebox_create(frm, res_addre);
    compo_picturebox_set_pos(picbox,gui_image_get_size(res_addre).wid/2+10, widget_text_get_area(textbox->txt).hei+(gui_image_get_size(res_addre).hei)+5);
    s16 current_h = widget_text_get_area(textbox->txt).hei + (gui_image_get_size(res_addre).hei)/2 - 6;
    for(uint8_t i = 0; i < 4; i++)
    {
        textbox = compo_textbox_create(frm, strlen(i18n[str_id[i]]));
        compo_textbox_set_align_center(textbox,false);
        compo_textbox_set_location(textbox,58, current_h + i * 55, 100, 30);
        compo_textbox_set(textbox,i18n[str_id[i]]);

        compo_textbox_t *textnum = compo_textbox_create(frm, strlen(str_data[i]));
        compo_textbox_set_align_center(textnum,false);
        compo_textbox_set_location(textnum,58 + widget_text_get_area(textbox->txt).wid + 5, current_h + i * 55, 100, 30);
        compo_textbox_set(textnum,str_data[i]);
    }

    page_size+=(gui_image_get_size(res_addre).hei+70);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,10, page_size, 348, 380);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN2]);
    compo_textbox_set_location(textbox,10, page_size, 348, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN2]);
    page_size+=widget_text_get_area(textbox->txt).hei+40;

    if(func_cb.sta == FUNC_PRESSURE_EXPLAIN)
    {
        f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
        f_pressure_explain->page_size = page_size;
    }

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
compo_form_t *func_pressure_explain_form_create(void)
{

    char str_buf[50];

    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STRESS]);

    u16 page_size=0;


    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN1]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,36, 90, 290, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN1]);
    compo_textbox_set_location(textbox,36, 90, 290, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN1]);
    page_size=widget_text_get_area(textbox->txt).hei+90;

    memset(str_buf,0,sizeof(str_buf));
    snprintf(str_buf,sizeof(str_buf),"%s1-29",i18n[STR_RELAXED]);
    textbox = compo_textbox_create(frm, strlen(str_buf));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,55, page_size+30, 220, 280);
    compo_textbox_set(textbox,str_buf);

    memset(str_buf,0,sizeof(str_buf));
    snprintf(str_buf,sizeof(str_buf),"%s30-59",i18n[STR_NORMAL]);
    textbox = compo_textbox_create(frm, strlen(str_buf));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,55, page_size+30+47, 220, 280);
    compo_textbox_set(textbox,str_buf);

    memset(str_buf,0,sizeof(str_buf));
    snprintf(str_buf,sizeof(str_buf),"%s60-79",i18n[STR_MEDIUM]);
    textbox = compo_textbox_create(frm, strlen(str_buf));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,55, page_size+30+47*2, 220, 280);
    compo_textbox_set(textbox,str_buf);

    memset(str_buf,0,sizeof(str_buf));
    snprintf(str_buf,sizeof(str_buf),"%s80-100",i18n[STR_HIGH]);
    textbox = compo_textbox_create(frm, strlen(str_buf));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,55, page_size+30+47*3, 220, 280);
    compo_textbox_set(textbox,str_buf);

    compo_picturebox_t *picbox = compo_picturebox_create(frm,UI_BUF_I338001_16_PRESSURE_EXPALIN_BIN );
    compo_picturebox_set_pos(picbox,gui_image_get_size(UI_BUF_I338001_16_PRESSURE_EXPALIN_BIN).wid/2+36,page_size+20+47*2 );
    page_size+=(gui_image_get_size(UI_BUF_I338001_16_PRESSURE_EXPALIN_BIN).hei+70);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PRESSURE_EXPLAIN2]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,36, page_size, 290, 280);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN2]);
    compo_textbox_set_location(textbox,36, page_size, 290, widget_text_get_area(textbox->txt).hei);
    compo_textbox_set(textbox,i18n[STR_PRESSURE_EXPLAIN2]);
    page_size+=widget_text_get_area(textbox->txt).hei+70;

    if(func_cb.sta == FUNC_PRESSURE_EXPLAIN)
    {
        f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
        f_pressure_explain->page_size = page_size;
    }

    return frm;
}
#else
compo_form_t *func_pressure_explain_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
#endif


//时钟表盘右滑菜单功能消息处理
static void func_pressure_explain_message(size_msg_t msg)
{
    f_pressure_explain_t *f_pressure_explain= (f_pressure_explain_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_pressure_explain->ptm)
            {

                compo_page_move_touch_handler(f_pressure_explain->ptm);
            }
            break;
        case MSG_CTP_SHORT_RIGHT:
            func_switch_to(FUNC_PRESSURE,FUNC_SWITCH_LR_ZOOM_RIGHT);
            break;
        case MSG_CTP_SHORT_LEFT:
            break;

        default:
            func_message(msg);
            break;
    }
}
//下滑事件流程处理
static void func_pressure_explain_process(void)
{
    f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
    if(f_pressure_explain->ptm)
    {
        compo_page_move_process(f_pressure_explain->ptm);
    }
    func_process();
}

//创建界面
static void func_pressure_explain_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_pressure_explain_t));
    func_cb.frm_main = func_pressure_explain_form_create();
    f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT \
    || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    f_pressure_explain->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .page_size =  f_pressure_explain->page_size,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_pressure_explain->ptm, func_cb.frm_main->page_body, &info);
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_pressure_explain->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  f_pressure_explain->page_size,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_pressure_explain->ptm, func_cb.frm_main->page_body, &info);
#endif
}
//退出任务
static void func_pressure_explain_exit(void)
{
    f_pressure_explain_t *f_pressure_explain = (f_pressure_explain_t *)func_cb.f_cb;
    if (f_pressure_explain->ptm)
    {
        func_free(f_pressure_explain->ptm);
    }
    func_cb.last = FUNC_PRESSURE_EXPLAIN;
}

//血氧功能
void func_pressure_explain(void)
{
    printf("%s\n", __func__);
    func_pressure_explain_enter();
    while (func_cb.sta == FUNC_PRESSURE_EXPLAIN)
    {
        func_pressure_explain_process();
        func_pressure_explain_message(msg_dequeue());
    }
    func_pressure_explain_exit();
}

#endif

