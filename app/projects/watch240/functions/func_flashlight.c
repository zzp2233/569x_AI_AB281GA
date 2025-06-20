#include "include.h"
#include "func.h"
#include "ute_drv_led.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT

//组件ID
enum
{
    //按键
    COMPO_ID_BG_SHAPE = 1,
    COMPO_ID_TXT,
    COMPO_ID_PIC,
};

typedef struct f_flashlight_t_
{
    bool flashlight_flag;
    u8 light_level;
} f_flashlight_t;


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);
//
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_FLASHLIGHT_ICON_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 106);
    compo_picturebox_set_visible(picbox, true);
    compo_setid(picbox,COMPO_ID_PIC);

    //创建文本
    compo_textbox_t *txt_idle = compo_textbox_create(frm, strlen(i18n[STR_CLICK_OPEN]));
    compo_textbox_set(txt_idle,i18n[STR_CLICK_OPEN]);
    compo_textbox_set_location(txt_idle, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/1.55,GUI_SCREEN_WIDTH/1.1,widget_text_get_area(txt_idle->txt).hei);
    compo_textbox_set(txt_idle,i18n[STR_CLICK_OPEN]);
    compo_textbox_set_visible(txt_idle, true);
    compo_setid(txt_idle,COMPO_ID_TXT);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_setid(shape,COMPO_ID_BG_SHAPE);
    compo_shape_set_visible(shape, false);

    return frm;
}

static void func_flashlight_button_click(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    compo_shape_t * shape = compo_getobj_byid(COMPO_ID_BG_SHAPE);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC);
    compo_textbox_t *txt_idle = compo_getobj_byid(COMPO_ID_TXT);
    f_flashlight->flashlight_flag = !f_flashlight->flashlight_flag;

    if(f_flashlight->flashlight_flag == true)
    {
        f_flashlight ->light_level = sys_cb.light_level;
        sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
        tft_bglight_set_level(sys_cb.light_level,false);
        compo_shape_set_visible(shape, true);
        compo_picturebox_set_visible(picbox, false);
        compo_textbox_set_visible(txt_idle, false);
    }
    else
    {
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
        compo_shape_set_visible(shape, false);
        compo_picturebox_set_visible(picbox, true);
        compo_textbox_set_visible(txt_idle, true);
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I332001_FLASHLIGHT_ICON_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 87/2+103);
    compo_picturebox_set_visible(picbox, true);
    compo_setid(picbox,COMPO_ID_PIC);

    //创建文本
    compo_textbox_t *txt_idle = compo_textbox_create(frm, strlen(i18n[STR_CLICK_OPEN]));
    compo_textbox_set(txt_idle,i18n[STR_CLICK_OPEN]);
    compo_textbox_set_location(txt_idle, GUI_SCREEN_CENTER_X, 34/2+224,GUI_SCREEN_WIDTH/1.3,widget_text_get_area(txt_idle->txt).hei);
    compo_textbox_set(txt_idle,i18n[STR_CLICK_OPEN]);
    compo_textbox_set_visible(txt_idle, true);
    compo_setid(txt_idle,COMPO_ID_TXT);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_setid(shape,COMPO_ID_BG_SHAPE);
    compo_shape_set_visible(shape, false);

    return frm;
}

static void func_flashlight_button_click(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    compo_shape_t * shape = compo_getobj_byid(COMPO_ID_BG_SHAPE);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC);
    compo_textbox_t *txt_idle = compo_getobj_byid(COMPO_ID_TXT);
    f_flashlight->flashlight_flag = !f_flashlight->flashlight_flag;

    if(f_flashlight->flashlight_flag == true)
    {
        f_flashlight ->light_level = sys_cb.light_level;
        sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
        tft_bglight_set_level(sys_cb.light_level,false);
        compo_shape_set_visible(shape, true);
        compo_picturebox_set_visible(picbox, false);
        compo_textbox_set_visible(txt_idle, false);
    }
    else
    {
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
        compo_shape_set_visible(shape, false);
        compo_picturebox_set_visible(picbox, true);
        compo_textbox_set_visible(txt_idle, true);
    }
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);
//
#if !UTE_DRV_LED_SUPPORT
    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_setid(shape,COMPO_ID_BG_SHAPE);
    compo_shape_set_visible(shape, false);
#endif
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_27_MORE_4_FLASHLIGHT_ICON_44X88_X98_Y98_00_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 98+88/2);
    compo_setid(picbox,COMPO_ID_PIC);

    return frm;
}

static void func_flashlight_button_click(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
#if !UTE_DRV_LED_SUPPORT
    compo_shape_t * shape = compo_getobj_byid(COMPO_ID_BG_SHAPE);
#endif
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC);
    // compo_textbox_t *txt_idle = compo_getobj_byid(COMPO_ID_TXT);
    f_flashlight->flashlight_flag = !f_flashlight->flashlight_flag;

    if(f_flashlight->flashlight_flag == true)
    {
#if !UTE_DRV_LED_SUPPORT
        f_flashlight ->light_level = sys_cb.light_level;
        sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
        tft_bglight_set_level(sys_cb.light_level,false);
        compo_shape_set_visible(shape, true);
#endif
        compo_picturebox_set(picbox,UI_BUF_I335001_27_MORE_4_FLASHLIGHT_ICON_44X88_X98_Y98_01_BIN);
    }
    else
    {
#if !UTE_DRV_LED_SUPPORT
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
        compo_shape_set_visible(shape, false);
#endif
        compo_picturebox_set(picbox,UI_BUF_I335001_27_MORE_4_FLASHLIGHT_ICON_44X88_X98_Y98_00_BIN);
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);
//
    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_setid(shape,COMPO_ID_BG_SHAPE);
    compo_shape_set_visible(shape, false);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_27_MORE_FLASHLIGHT_CLOSE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 172+104/2);
    compo_setid(picbox,COMPO_ID_PIC);

    return frm;
}

static void func_flashlight_button_click(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    compo_shape_t * shape = compo_getobj_byid(COMPO_ID_BG_SHAPE);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC);
    // compo_textbox_t *txt_idle = compo_getobj_byid(COMPO_ID_TXT);
    f_flashlight->flashlight_flag = !f_flashlight->flashlight_flag;

    if(f_flashlight->flashlight_flag == true)
    {
        f_flashlight ->light_level = sys_cb.light_level;
        sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
        tft_bglight_set_level(sys_cb.light_level,false);
        compo_shape_set_visible(shape, true);
        compo_picturebox_set(picbox,UI_BUF_I341001_27_MORE_FLASHLIGHT_OPEN_BIN);
    }
    else
    {
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
        compo_shape_set_visible(shape, false);
        compo_picturebox_set(picbox,UI_BUF_I341001_27_MORE_FLASHLIGHT_CLOSE_BIN);
    }
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);
//
    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_setid(shape,COMPO_ID_BG_SHAPE);
    compo_shape_set_visible(shape, false);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I343001_27_MORE_FLASHLIGHT_CLOSE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 133+119/2);
    compo_setid(picbox,COMPO_ID_PIC);

    return frm;
}

static void func_flashlight_button_click(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    compo_shape_t * shape = compo_getobj_byid(COMPO_ID_BG_SHAPE);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC);
    // compo_textbox_t *txt_idle = compo_getobj_byid(COMPO_ID_TXT);
    f_flashlight->flashlight_flag = !f_flashlight->flashlight_flag;

    if(f_flashlight->flashlight_flag == true)
    {
        f_flashlight ->light_level = sys_cb.light_level;
        sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
        tft_bglight_set_level(sys_cb.light_level,false);
        compo_shape_set_visible(shape, true);
        compo_picturebox_set(picbox,UI_BUF_I343001_27_MORE_FLASHLIGHT_OPEN_BIN);
    }
    else
    {
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
        compo_shape_set_visible(shape, false);
        compo_picturebox_set(picbox,UI_BUF_I343001_27_MORE_FLASHLIGHT_CLOSE_BIN);
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);
//
    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_setid(shape,COMPO_ID_BG_SHAPE);
    compo_shape_set_visible(shape, false);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338001_27_MORE_FLASHLIGHT_1_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(picbox,COMPO_ID_PIC);

    return frm;
}

static void func_flashlight_button_click(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    compo_shape_t * shape = compo_getobj_byid(COMPO_ID_BG_SHAPE);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC);
    // compo_textbox_t *txt_idle = compo_getobj_byid(COMPO_ID_TXT);
    f_flashlight->flashlight_flag = !f_flashlight->flashlight_flag;

    if(f_flashlight->flashlight_flag == true)
    {
        f_flashlight ->light_level = sys_cb.light_level;
        sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
        tft_bglight_set_level(sys_cb.light_level,false);
        compo_shape_set_visible(shape, true);
        compo_picturebox_set(picbox,UI_BUF_I338001_27_MORE_FLASHLIGHT_BIN);
    }
    else
    {
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
        compo_shape_set_visible(shape, false);
        compo_picturebox_set(picbox,UI_BUF_I338001_27_MORE_FLASHLIGHT_1_BIN);
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I340001_FLASHLIGHT_ICON_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 87/2+103);
    compo_picturebox_set_visible(picbox, true);
    compo_setid(picbox,COMPO_ID_PIC);

    //创建文本
    compo_textbox_t *txt_idle = compo_textbox_create(frm, strlen(i18n[STR_CLICK_OPEN]));
    compo_textbox_set(txt_idle,i18n[STR_CLICK_OPEN]);
    compo_textbox_set_location(txt_idle, GUI_SCREEN_CENTER_X, 34/2+224,GUI_SCREEN_WIDTH/1.3,widget_text_get_area(txt_idle->txt).hei);
    compo_textbox_set(txt_idle,i18n[STR_CLICK_OPEN]);
    compo_textbox_set_visible(txt_idle, true);
    compo_setid(txt_idle,COMPO_ID_TXT);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_setid(shape,COMPO_ID_BG_SHAPE);
    compo_shape_set_visible(shape, false);

    return frm;
}

static void func_flashlight_button_click(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    compo_shape_t * shape = compo_getobj_byid(COMPO_ID_BG_SHAPE);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC);
    compo_textbox_t *txt_idle = compo_getobj_byid(COMPO_ID_TXT);
    f_flashlight->flashlight_flag = !f_flashlight->flashlight_flag;

    if(f_flashlight->flashlight_flag == true)
    {
        f_flashlight ->light_level = sys_cb.light_level;
        sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
        tft_bglight_set_level(sys_cb.light_level,false);
        compo_shape_set_visible(shape, true);
        compo_picturebox_set_visible(picbox, false);
        compo_textbox_set_visible(txt_idle, false);
    }
    else
    {
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
        compo_shape_set_visible(shape, false);
        compo_picturebox_set_visible(picbox, true);
        compo_textbox_set_visible(txt_idle, true);
    }
}

#elif  GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

//创建手电筒窗体
compo_form_t *func_flashlight_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_FLASHLIGHT]);
//
    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_setid(shape,COMPO_ID_BG_SHAPE);
    compo_shape_set_visible(shape, false);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I342001_27_MORE_FLASHLIGHT_CLOSE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(picbox,COMPO_ID_PIC);

    return frm;
}

static void func_flashlight_button_click(void)
{
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    compo_shape_t * shape = compo_getobj_byid(COMPO_ID_BG_SHAPE);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC);
    // compo_textbox_t *txt_idle = compo_getobj_byid(COMPO_ID_TXT);
    f_flashlight->flashlight_flag = !f_flashlight->flashlight_flag;

    if(f_flashlight->flashlight_flag == true)
    {
        f_flashlight ->light_level = sys_cb.light_level;
        sys_cb.light_level = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
        tft_bglight_set_level(sys_cb.light_level,false);
        compo_shape_set_visible(shape, true);
        compo_picturebox_set(picbox,UI_BUF_I342001_27_MORE_FLASHLIGHT_OPEN_BIN);
    }
    else
    {
        tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
        compo_shape_set_visible(shape, false);
        compo_picturebox_set(picbox,UI_BUF_I342001_27_MORE_FLASHLIGHT_CLOSE_BIN);
    }
}


#else
compo_form_t *func_flashlight_form_create(void)
{
}
static void func_flashlight_button_click(void)
{
}
#endif

//手电筒功能事件处理
static void func_flashlight_process(void)
{
#if !UTE_DRV_LED_SUPPORT
    f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
    if(f_flashlight->flashlight_flag == true)
    {
        reset_guioff_delay();
    }
#endif
    func_process();
}

//手电筒功能消息处理
static void func_flashlight_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_flashlight_button_click();
#if UTE_DRV_LED_SUPPORT
            f_flashlight_t *f_flashlight = (f_flashlight_t *)func_cb.f_cb;
            if (f_flashlight->flashlight_flag == true)
            {
                uteDrvLedEnable();
                uteModuleGuiCommonDisplayOffAllowGoBack(false);
            }
            else
            {
                uteDrvLedDisable();
                uteModuleGuiCommonDisplayOffAllowGoBack(true);
            }
#endif
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        case KU_LEFT:
            msg_enqueue(MSG_CTP_CLICK);
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入手电筒控制功能
static void func_flashlight_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_flashlight_t));
    func_cb.frm_main = func_flashlight_form_create();
}

//退出手电筒功能
static void func_flashlight_exit(void)
{
#if UTE_DRV_LED_SUPPORT
    uteDrvLedDisable();
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
#endif
    tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(),true);
    func_cb.last = FUNC_FLASHLIGHT;
}

//手电筒功能
void func_flashlight(void)
{
    printf("%s\n", __func__);
    func_flashlight_enter();
    while (func_cb.sta == FUNC_FLASHLIGHT)
    {
        func_flashlight_process();
        func_flashlight_message(msg_dequeue());
    }
    func_flashlight_exit();
}

#endif
