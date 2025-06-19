#include "include.h"
#include "func.h"
#include "ute_module_heart.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_HEART_WARNING_SUPPORT
enum
{
    COMPO_ID_TEXT_UINT=1,
    COMPO_ID_TEXT_HEART_VALUE,
};

enum
{
    COMPO_MAX_STATE=1,
    COMPO_MIN_STATE,
};

typedef struct f_heart_warning_t_
{
    u8 up_date_flag;
    u8 up_date_old_flag;
} f_heart_warning_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
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
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,140,123,90,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,26/2+168,230,widget_text_get_max_height());
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() < uteModuleHeartGetMinHeartValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_REPEAT_HEART_BIN);
    compo_picturebox_set_pos(picbox, 11+140, 98+14);

    return frm;
}

static void func_heart_warning_updata(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_HEART_VALUE);
    uint8_t heart_value = uteModuleHeartGetHeartValue();
    if(heart_value > 0 && heart_value < 0xff)
    {
        char txt_buf[50];
        memset(txt_buf, 0, sizeof(txt_buf));
        snprintf(txt_buf, sizeof(txt_buf), "%d", heart_value);
        compo_textbox_set(textbox, txt_buf);
    }
    if (uteModuleHeartGetHeartValue() < uteModuleHeartGetHeartWaringMaxValue() && uteModuleHeartGetHeartValue() > uteModuleHeartGetHeartWaringMinValue())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_REPEAT_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    char txt_buf[50];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,112/2+104,96/2+119);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,207,28+140,90,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_setid(textbox,COMPO_ID_TEXT_UINT);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,16+215,200,35);
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() < uteModuleHeartGetMinHeartValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_REPEAT_HEART_BIN);
    compo_picturebox_set_pos(picbox, 28/2+207, 28/2+136);

    return frm;
}

static void func_heart_warning_updata(void)
{
    f_heart_warning_t* f_heart_warning = (f_heart_warning_t*)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_HEART_VALUE);
    compo_textbox_t *txt_uint = compo_getobj_byid(COMPO_ID_TEXT_UINT);
    uint8_t heart_value = uteModuleHeartGetHeartValue();
    char txt_buf[100];

    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", heart_value);
    compo_textbox_set(textbox, txt_buf);

    if(heart_value >= uteModuleHeartGetHeartWaringMaxValue())
    {
        f_heart_warning->up_date_flag = COMPO_MAX_STATE;
    }
    else if (heart_value <= uteModuleHeartGetMinHeartValue())
    {
        f_heart_warning->up_date_flag = COMPO_MIN_STATE;
    }

    if(f_heart_warning->up_date_flag != f_heart_warning->up_date_old_flag)
    {
        memset(txt_buf, 0, sizeof(txt_buf));
        f_heart_warning->up_date_old_flag = f_heart_warning->up_date_flag;
        if(f_heart_warning->up_date_flag == COMPO_MAX_STATE)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_HIGHT]);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_LOW]);
        }
        compo_textbox_set(txt_uint, txt_buf);
    }

    if (uteModuleHeartGetHeartValue() < uteModuleHeartGetHeartWaringMaxValue() && uteModuleHeartGetHeartValue() > uteModuleHeartGetHeartWaringMinValue())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338002_6_HEART_ICON_HEART_BIN);
    compo_picturebox_set_pos(picbox,142, 135);  //需要更替为弹窗图标

    char txt_buf[100];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, 3);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,185,120);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_setid(textbox,COMPO_ID_TEXT_UINT);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,42/2+193,230,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(249,52,52));
    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() <= uteModuleHeartGetMinHeartValue())
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_LOW]);
    }
    compo_textbox_set(textbox,txt_buf);
#else
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I338001_6_HEART_HEART_GIF_BIN);
    compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X, 122/2+66);  //需要更替为弹窗图标
    compo_animation_set_radix(animation,3);
    compo_animation_set_interval(animation,30);

    char txt_buf[100];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, 3);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,0,211,GUI_SCREEN_CENTER_X,70);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_right_align(textbox,true);
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X+8,229,100,widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);


    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,284,230,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() < uteModuleHeartGetMinHeartValue())
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_LOW]);
    }
    compo_textbox_set(textbox,txt_buf);
#endif
    return frm;
}

static void func_heart_warning_updata(void)
{
    f_heart_warning_t* f_heart_warning = (f_heart_warning_t*)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_HEART_VALUE);
    compo_textbox_t *txt_uint = compo_getobj_byid(COMPO_ID_TEXT_UINT);
    uint8_t heart_value = uteModuleHeartGetHeartValue();
    char txt_buf[100];

    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", heart_value);
    compo_textbox_set(textbox, txt_buf);

    if(heart_value >= uteModuleHeartGetHeartWaringMaxValue())
    {
        f_heart_warning->up_date_flag = COMPO_MAX_STATE;
    }
    else if (heart_value <= uteModuleHeartGetMinHeartValue())
    {
        f_heart_warning->up_date_flag = COMPO_MIN_STATE;
    }

    if(f_heart_warning->up_date_flag != f_heart_warning->up_date_old_flag)
    {
        memset(txt_buf, 0, sizeof(txt_buf));
        f_heart_warning->up_date_old_flag = f_heart_warning->up_date_flag;
        if(f_heart_warning->up_date_flag)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_HIGHT]);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_LOW]);
        }
        compo_textbox_set(txt_uint, txt_buf);
    }

    if ((heart_value < uteModuleHeartGetHeartWaringMaxValue() && heart_value > uteModuleHeartGetHeartWaringMinValue()) || (heart_value == 0 || heart_value == 0xff))
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I335001_6_HEART_4_GIF_HEART_104X92_X68_Y46_BIN);
    compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-50);  //需要更替为弹窗图标
    compo_animation_set_radix(animation,3);
    compo_animation_set_interval(animation,30);

    char txt_buf[10];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, 3);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,0,GUI_SCREEN_CENTER_Y+10,GUI_SCREEN_CENTER_X,60);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_right_align(textbox,true);
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X+6,GUI_SCREEN_CENTER_Y+30,GUI_SCREEN_CENTER_X,widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);


    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,230,230,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    compo_setid(textbox,COMPO_ID_TEXT_UINT);
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue() )
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() <= uteModuleHeartGetMinHeartValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }

    return frm;
}

static void func_heart_warning_updata(void)
{
    f_heart_warning_t* f_heart_warning = (f_heart_warning_t*)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_HEART_VALUE);
    compo_textbox_t *txt_uint = compo_getobj_byid(COMPO_ID_TEXT_UINT);
    uint8_t heart_value = uteModuleHeartGetHeartValue();
    char txt_buf[100];

    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", heart_value);
    compo_textbox_set(textbox, txt_buf);

    if(heart_value >= uteModuleHeartGetHeartWaringMaxValue())
    {
        f_heart_warning->up_date_flag = COMPO_MAX_STATE;
    }
    else if (heart_value <= uteModuleHeartGetMinHeartValue())
    {
        f_heart_warning->up_date_flag = COMPO_MIN_STATE;
    }

    if(f_heart_warning->up_date_flag != f_heart_warning->up_date_old_flag)
    {
        memset(txt_buf, 0, sizeof(txt_buf));
        f_heart_warning->up_date_old_flag = f_heart_warning->up_date_flag;
        if(f_heart_warning->up_date_flag == COMPO_MAX_STATE)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_HIGHT]);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_LOW]);
        }
        compo_textbox_set(txt_uint, txt_buf);
    }

    if (heart_value < uteModuleHeartGetHeartWaringMaxValue() && heart_value > uteModuleHeartGetHeartWaringMinValue())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I341001_6_HEART_HEART_GIF_BIN);
    compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-50);  //需要更替为弹窗图标
    compo_animation_set_radix(animation,3);
    compo_animation_set_interval(animation,30);

    char txt_buf[10];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, 3);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,0,GUI_SCREEN_CENTER_Y+10,GUI_SCREEN_CENTER_X,60);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_right_align(textbox,true);
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X+6,GUI_SCREEN_CENTER_Y+30,GUI_SCREEN_CENTER_X,widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);


    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,230,230,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() < uteModuleHeartGetMinHeartValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }

    return frm;
}

static void func_heart_warning_updata(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_HEART_VALUE);
    uint8_t heart_value = uteModuleHeartGetHeartValue();
    if(heart_value > 0 && heart_value < 0xff)
    {
        char txt_buf[10];
        memset(txt_buf, 0, sizeof(txt_buf));
        snprintf(txt_buf, sizeof(txt_buf), "%d", heart_value);
        compo_textbox_set(textbox, txt_buf);
    }
    if (uteModuleHeartGetHeartValue() < uteModuleHeartGetHeartWaringMaxValue() && uteModuleHeartGetHeartValue() > uteModuleHeartGetHeartWaringMinValue())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I343001_6_HEART_HEART_GIF_BIN);
    compo_animation_set_pos(animation,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-50);  //需要更替为弹窗图标
    compo_animation_set_radix(animation,3);
    compo_animation_set_interval(animation,30);

    char txt_buf[10];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, 3);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,0,GUI_SCREEN_CENTER_Y+10,GUI_SCREEN_CENTER_X,60);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_right_align(textbox,true);
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X+6,GUI_SCREEN_CENTER_Y+10,GUI_SCREEN_CENTER_X,widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);


    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,280,230,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() < uteModuleHeartGetMinHeartValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }

    return frm;
}

static void func_heart_warning_updata(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_HEART_VALUE);
    uint8_t heart_value = uteModuleHeartGetHeartValue();
    if(heart_value > 0 && heart_value < 0xff)
    {
        char txt_buf[10];
        memset(txt_buf, 0, sizeof(txt_buf));
        snprintf(txt_buf, sizeof(txt_buf), "%d", heart_value);
        compo_textbox_set(textbox, txt_buf);
    }
    if (uteModuleHeartGetHeartValue() < uteModuleHeartGetHeartWaringMaxValue() && uteModuleHeartGetHeartValue() > uteModuleHeartGetHeartWaringMinValue())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
}


#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT

//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I340001_REPEAT_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    char txt_buf[50];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,112/2+104,96/2+119);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,207,28+140,90,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(255,69,46));
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,16+215,200,35);
    compo_setid(textbox,COMPO_ID_TEXT_UINT);
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() < uteModuleHeartGetMinHeartValue())
    {
        compo_textbox_set(textbox,i18n[STR_HEART_LOW]);
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I340001_REPEAT_HEART_BIN);
    compo_picturebox_set_pos(picbox, 28/2+207, 28/2+136);

    return frm;
}

static void func_heart_warning_updata(void)
{
    f_heart_warning_t* f_heart_warning = (f_heart_warning_t*)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_HEART_VALUE);
    compo_textbox_t *txt_uint = compo_getobj_byid(COMPO_ID_TEXT_UINT);
    uint8_t heart_value = uteModuleHeartGetHeartValue();
    char txt_buf[100];

    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", heart_value);
    compo_textbox_set(textbox, txt_buf);

    if(heart_value >= uteModuleHeartGetHeartWaringMaxValue())
    {
        f_heart_warning->up_date_flag = COMPO_MAX_STATE;
    }
    else if (heart_value <= uteModuleHeartGetMinHeartValue())
    {
        f_heart_warning->up_date_flag = COMPO_MIN_STATE;
    }

    if(f_heart_warning->up_date_flag != f_heart_warning->up_date_old_flag)
    {
        memset(txt_buf, 0, sizeof(txt_buf));
        f_heart_warning->up_date_old_flag = f_heart_warning->up_date_flag;
        if(f_heart_warning->up_date_flag == COMPO_MAX_STATE)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_HIGHT]);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_LOW]);
        }
        compo_textbox_set(txt_uint, txt_buf);
    }

    if (uteModuleHeartGetHeartValue() < uteModuleHeartGetHeartWaringMaxValue() && uteModuleHeartGetHeartValue() > uteModuleHeartGetHeartWaringMinValue())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

//创建心率预警窗体
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
//    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I338002_6_HEART_ICON_HEART_BIN);
//    compo_animation_set_pos(animation,52/2+95, 52/2+112);  //需要更替为弹窗图标
//    compo_animation_set_radix(animation,1);
//    compo_animation_set_interval(animation,0);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I342001_6_HEART_HEART_BIN);
    compo_picturebox_set_pos(picbox, 60+34/2, 78+34/2);

    char txt_buf[100];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetHeartValue());
    compo_textbox_t *textbox = compo_textbox_create(frm, 3);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,103,69+10);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TEXT_HEART_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_HIGHT])+strlen(i18n[STR_HEART_LOW]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,48/2+128,230,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(249,52,52));
    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetHeartValue() >= uteModuleHeartGetHeartWaringMaxValue())
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_HIGHT]);
    }
    else if (uteModuleHeartGetHeartValue() < uteModuleHeartGetMinHeartValue())
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s!",i18n[STR_HEART_LOW]);
    }
    compo_textbox_set(textbox,txt_buf);

    return frm;
}

static void func_heart_warning_updata(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TEXT_HEART_VALUE);
    uint8_t heart_value = uteModuleHeartGetHeartValue();
    if(heart_value > 0 && heart_value < 0xff)
    {
        char txt_buf[10];
        memset(txt_buf, 0, sizeof(txt_buf));
        snprintf(txt_buf, sizeof(txt_buf), "%d", heart_value);
        compo_textbox_set(textbox, txt_buf);
    }
    if (uteModuleHeartGetHeartValue() < uteModuleHeartGetHeartWaringMaxValue() && uteModuleHeartGetHeartValue() > uteModuleHeartGetHeartWaringMinValue())
    {
        uteModuleGuiCommonGoBackLastScreen();
    }
}

#else
compo_form_t *func_heart_warning_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
static void func_heart_warning_updata(void)
{
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
