#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_HEART_WARNING_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    COMPO_ID_TEXT_UINT=1,
};

typedef struct f_heart_warning_t_
{

} f_heart_warning_t;

//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_REPEAT_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    char txt_buf[50];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,44+88/2,85+72/2);
    compo_textbox_set(textbox,txt_buf);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,140,123,90,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,26/2+168,230,widget_text_get_max_height());
    if(uteModuleHeartGetMaxHeartValue()<bsp_sensor_hrs_data_get())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }
    compo_setid(textbox,COMPO_ID_TEXT_UINT);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_REPEAT_HEART_BIN);
    compo_picturebox_set_pos(picbox, 11+140, 98+14);

    return frm;
}

static void func_heart_warning_updata(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_UINT);
    if(uteModuleHeartGetMaxHeartValue()<bsp_sensor_hrs_data_get())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
typedef struct f_heart_warning_t_
{

} f_heart_warning_t;

//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_REPEAT_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    char txt_buf[50];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",150);
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,44+88/2,85+72/2);
    compo_textbox_set(textbox,txt_buf);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,true);
    compo_textbox_set_location(textbox,158,widget_text_get_max_height()/2+113,90,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    textbox = compo_textbox_create(frm, strlen(i18n[0]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,26/2+168,230,widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[0]);

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_REPEAT_HEART_BIN);
    compo_picturebox_set_pos(picbox, 11+140, 98+14);

    return frm;
}

static void func_heart_warning_updata(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_UINT);
    if(uteModuleHeartGetMaxHeartValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }
}
#endif

//心率预警功能事件处理
static void func_heart_warning_process(void)
{
    func_heart_warning_updata();
    func_process();
}
static void func_heart_warning_message(size_msg_t msg)
{
    switch (msg)
    {

        default:
            func_message(msg);
            break;
    }
}

//进入心率预警功能
static void func_heart_warning_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_heart_warning_t));
    func_cb.frm_main = func_heart_warning_form_create();
}

//退出心率预警功能
static void func_heart_warning_exit(void)
{
    func_cb.last = FUNC_HEART_WARNING;
    uteTaskGuiStackRemoveScreenId(FUNC_HEART_WARNING);
}

//心率预警功能
void func_heart_warning(void)
{
    printf("%s\n", __func__);
    func_heart_warning_enter();
    while (func_cb.sta == FUNC_HEART_WARNING)
    {
        func_heart_warning_process();
        func_heart_warning_message(msg_dequeue());
    }
    func_heart_warning_exit();
}

#endif
