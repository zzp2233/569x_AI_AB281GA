#include "include.h"
#include "func.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_heart.h"
#include "ute_drv_motor.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT

//血氧检测状态
enum
{
    BO_STA_IDLE,            //空闲
    BO_STA_TESTING,         //测量中
    BO_STA_UNWEAR,          //未佩戴
};

//组件ID
enum
{
    //按键
    COMPO_ID_SURE_BTN = 1,
    COMPO_ID_AGAIN_BTN,
    COMPO_ID_PIC_BG,
    COMPO_ID_TXT_VALUE,
    COMPO_ID_PIC_UNIT,
    COMPO_ID_SAHPE_BG,
    COMPO_ID_MAX_VLA,
    COMPO_ID_MIN_VLA,
    COMPO_ID_ABOUT_BTN,
    COMPO_ID_TXT_TEST,
};

typedef struct f_blood_oxygen_t_
{
    uint32_t tick;
    uint32_t tick_start;
    u8 blood_oxygen_state;
    u8 pic_type;
    bool pop_disp_flag;
    bool need_auto_test_flag;
} f_blood_oxygen_t;

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I335001_BLOOD_OXYGEN_OXYGEN_GIF_BIN);
    compo_picturebox_set_pos(picbox, 14+52/2, 54+52/2);
    compo_picturebox_cut(picbox, 10, 16);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,74,65);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 1 );
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,80+txt_leng.wid,80,100, widget_text_get_max_height());
    compo_textbox_set(textbox,"%");
    compo_setid(textbox,COMPO_ID_PIC_UNIT);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I335001_BLOOD_OXYGEN_PAUSE_BIN);///重新测量按钮
    compo_button_set_pos(btn,22+184,22+58);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    btn = compo_button_create_by_image(frm,UI_BUF_I335001_BLOOD_OXYGEN_ABOUT_BIN);///关于
    compo_button_set_pos(btn,144,10+66+15);
    compo_setid(btn,COMPO_ID_ABOUT_BTN);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_HEART_RATE_MAX_BIN);
    compo_picturebox_set_pos(picbox, 41,125);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",uteModuleHeartGetMaxHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_setid(textbox,COMPO_ID_MAX_VLA);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,61,114);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_HEART_RATE_MIN_BIN);
    compo_picturebox_set_pos(picbox, 149,125);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"--%%");
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_setid(textbox,COMPO_ID_MIN_VLA);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,169,114);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_HEART_RATE_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, 120,148+60);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_HEART_RATE_LINE_BIN);
    compo_picturebox_set_pos(picbox, 11,158+87/2);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_HEART_RATE_LINE_BIN);
    compo_picturebox_set_pos(picbox, 108,158+87/2);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_HEART_RATE_LINE_BIN);
    compo_picturebox_set_pos(picbox, 204,158+87/2);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///

#define CHART_NUM 24 //柱形图数量
    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X-10,158+87/2,192,87);
    compo_chartbox_set_pixel(chart, 1);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = 4;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0x8c,0x0f,0x2b));
    }
    ///创建图表
    chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X-10,158+87/2,192,87);
    compo_chartbox_set_pixel(chart, 1);
    for (int i=0; i<CHART_NUM; i++)
    {
        // heart_date[i] =100;
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = heart_date[i]*0.435;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    textbox = compo_textbox_create(frm, 5);///
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_12_BIN);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_textbox_set_pos(textbox,21,148+107);
    compo_textbox_set(textbox,"00:00");

    textbox = compo_textbox_create(frm, 5);///
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_12_BIN);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-7,148+107);
    compo_textbox_set(textbox,"12:00");

    textbox = compo_textbox_create(frm, 5);///
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_12_BIN);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_textbox_set_pos(textbox,197,148+107);
    compo_textbox_set(textbox,"24:00");

    textbox = compo_textbox_create(frm, 3);///
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_12_BIN);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_textbox_set_pos(textbox,220,148+10);
    compo_textbox_set(textbox,"100%");

    textbox = compo_textbox_create(frm, 3);///
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_12_BIN);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_textbox_set_pos(textbox,220,148+49);
    compo_textbox_set(textbox,"50%");

    return frm;
}
// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }


    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    compo_textbox_set_pos(pic_uint,80+txt_leng.wid,80);

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==16)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            compo_picturebox_cut(picbox, f_bo->pic_type, 16); ///图片动态显示
            compo_button_set_bgimg(btn,UI_BUF_I335001_BLOOD_OXYGEN_FINISH_BIN );
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        // msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_SURE, MSGBOX_MSG_TYPE_NONE);
        f_bo->blood_oxygen_state = BO_STA_IDLE;
    }
    else
    {
        compo_button_set_bgimg(btn,UI_BUF_I335001_BLOOD_OXYGEN_PAUSE_BIN );
    }
}
//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            if(f_bo->blood_oxygen_state == BO_STA_TESTING)
            {
                uteModuleBloodoxygenStopSingleTesting();
                f_bo->blood_oxygen_state = BO_STA_IDLE;
            }
            else
            {
                uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
                f_bo->blood_oxygen_state = BO_STA_TESTING;
            }

        }
        break;
    }
}

#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define CONTROL_Y  8
//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_BLOODOXYGEN_GIF_BIN);
    compo_picturebox_cut(picbox, 0, 16);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///血氧数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,12,190+18-11);
    char txt_buf[5];
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_BLOODOXYGEN_PERCENT_BIN);///  % 图片
    compo_picturebox_set_pos(picbox, 19/2+66+txt_leng.wid-42,219+19/2-3);
    compo_setid(picbox,COMPO_ID_PIC_UNIT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));///血氧和饱和度
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,12,245,140,40);
    compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    // compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_BLOODOXYGEN_ICON_DETECTION_BIN);///重新测量按钮
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.5,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.5-CONTROL_Y);
//    compo_button_set_visible(btn, false);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    return frm;
}

// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }


    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    compo_picturebox_set_pos(pic_uint,19/2+66+txt_leng.wid-42,219+19/2-3);

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==16)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            compo_picturebox_cut(picbox, f_bo->pic_type, 16); ///图片动态显示
            compo_button_set_visible(btn, false);
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_SURE, MSGBOX_MSG_TYPE_NONE);
        f_bo->blood_oxygen_state = BO_STA_IDLE;
    }
    else
    {
        compo_picturebox_cut(picbox, 0, 16); ///图片动态显示
        compo_button_set_visible(btn, true);
    }


}

//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
        break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define CONTROL_Y  8

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_BLOODOXYGEN_GIF_BIN);
    compo_picturebox_cut(picbox, 0, 16);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 113/2+36);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///血氧数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,77/2+155);
    char txt_buf[5];
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    picbox = compo_picturebox_create(frm, UI_BUF_I332001_BLOODOXYGEN_PERCENT_BIN);///  % 图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+txt_leng.wid/2+20,77/2+77/4+155);
    compo_setid(picbox,COMPO_ID_PIC_UNIT);

    // textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));///血氧和饱和度
    // compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,22/2+234,140,24);
    // compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);
    // compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    // compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I332001_BLOODOXYGEN_CARD_BIN);///重新测量按钮
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,64/2+266);
//    compo_button_set_visible(btn, false);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_START_MEASURING])+strlen(i18n[STR_MEASURING]));///测量中...
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,64/2+266,200,64);
    compo_textbox_set(textbox,i18n[STR_START_MEASURING]);
    compo_setid(textbox,COMPO_ID_TXT_TEST);

    return frm;
}

// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    compo_textbox_t *textbox_gauge = compo_getobj_byid(COMPO_ID_TXT_TEST);
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }


    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    compo_picturebox_set_pos(pic_uint, GUI_SCREEN_CENTER_X+txt_leng.wid/2+20,77/2+77/4+155);

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==16)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            compo_picturebox_cut(picbox, f_bo->pic_type, 16); ///图片动态显示
            compo_button_set_visible(btn, false);
            compo_textbox_set(textbox_gauge,i18n[STR_MEASURING]);
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_SURE, MSGBOX_MSG_TYPE_NONE);
        f_bo->blood_oxygen_state = BO_STA_IDLE;
    }
    else
    {
        compo_picturebox_cut(picbox, 0, 16); ///图片动态显示
        compo_button_set_visible(btn, true);
        compo_textbox_set(textbox_gauge,i18n[STR_START_MEASURING]);
    }


}

//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
        break;
    }
}

#else
compo_form_t *func_blood_oxygen_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    return;
}

static void func_blood_oxygen_disp_handle(void)
{}
//单击按钮
static void func_blood_oxygen_button_click(void)
{}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//血氧界面刷新、事件处理
static void func_blood_oxygen_process(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    if(f_bo == NULL)
    {
        return;
    }
    if(uteModuleBloodoxygenIsWear() && uteModuleBloodoxygenIsTesting())
    {
        reset_guioff_delay();
    }
    if(f_bo->blood_oxygen_state == BO_STA_TESTING)
    {
        if (!uteModuleBloodoxygenIsWear() && !uteModuleBloodoxygenIsTesting())
        {
            f_bo->blood_oxygen_state = BO_STA_UNWEAR;
        }
        else if (!uteModuleBloodoxygenIsTesting())
        {
            f_bo->blood_oxygen_state = BO_STA_IDLE;
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        }
    }
    else
    {
        if (uteModuleBloodoxygenIsTesting())
        {
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
    }
    func_blood_oxygen_disp_handle();
    func_process();
}

//血氧功能消息处理
static void func_blood_oxygen_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_blood_oxygen_button_click();
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入血氧功能
static void func_blood_oxygen_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_blood_oxygen_t));
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    f_bo->blood_oxygen_state = BO_STA_IDLE;
    f_bo->need_auto_test_flag = true;
    func_cb.frm_main = func_blood_oxygen_form_create();
    f_bo->tick_start = tick_get();
    tft_set_temode(0);
}

//退出血氧功能
static void func_blood_oxygen_exit(void)
{
    if (uteModuleBloodoxygenIsTesting())
    {
        uteModuleBloodoxygenStopSingleTesting();///关闭测试
    }
    // f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    func_cb.last = FUNC_BLOOD_OXYGEN;
    tft_set_temode(DEFAULT_TE_MODE);
}

//血氧功能
void func_blood_oxygen(void)
{
    printf("%s\n", __func__);
    func_blood_oxygen_enter();
    while (func_cb.sta == FUNC_BLOOD_OXYGEN)
    {
        func_blood_oxygen_process();
        func_blood_oxygen_message(msg_dequeue());
    }
    func_blood_oxygen_exit();
}

#endif
