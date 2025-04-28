#include "include.h"
#include "func.h"
#include "ute_module_heart.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT

#define FINT_HEIGHT 24*2+8

#define FUNC_HR_SIMULATE_DATA_EN  0
#define CHART_NUM 24 //柱形图数量
#define CHART_200_LENGTH GUI_SCREEN_CENTER_Y*6.25/8  //柱形图框高度
#define CONTROL_Y  8
//组件ID;
enum
{
    COMPO_ID_HEART_PIC=1,
    COMPO_ID_HEART_MAX_TXT,
    COMPO_ID_HEART_MIN_TXT,
    COMPO_ID_HEART_STATIC_VALUE_TXT,
    COMPO_ID_HEART_STATIC_BPM_TXT,
    COMPO_ID_HEART_BPM_TXT,
    COMPO_ID_HEART_VALUE_TXT,
    COMPO_ID_CHART,
    COMPO_ID_HEART_BTN_ABOUT,
};

typedef struct f_heartrate_t_
{
    page_tp_move_t *ptm;
    uint32_t tick;
    u8 last_hr;
    u8 max_hr;
    u8 min_hr;
    bool heart_pic_state;
    u8 heart_pic_size;
    bool no_wear_flag;
} f_heartrate_t;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I335001_6_HEART_1_1_GIF_HEART_64X62_X10_Y54_GIF_HEART_BIN);
    compo_picturebox_set_pos(picbox, 14+52/2, 54+52/2);
    compo_picturebox_cut(picbox, 0, 18);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,74,65);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,80+txt_leng.wid,77,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    // compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,14,114,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,20+txt_leng.wid,114);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,120,114,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,126+txt_leng.wid,114);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_6_HEART_1_1_ICON_BG_DATA_HEART_DATE_BG_224X110_X8_Y162_BIN);
    compo_picturebox_set_pos(picbox, 120,148+60);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+14,202,156,71);
    compo_chartbox_set_pixel(chart, 1);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        // heart_date[i] =200;
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = heart_date[i]*0.34;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }
////////////////////////////////////////////////////////////////
    textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]) );///静态心率
    compo_textbox_set_location(textbox,14,285,88, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);


    picbox = compo_picturebox_create(frm, UI_BUF_I335001_6_HEART_1_1_ICON_ABOUT_18X18_X176_Y299_BIN);
    compo_picturebox_set_pos(picbox,115,295);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,115,295,30,30);
    compo_setid(btn,COMPO_ID_HEART_BTN_ABOUT);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,14,320);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,20+txt_leng.wid,320+10,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_BPM_TXT);
//////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_6_HEART_1_1_ICON_BG_DATA_RESTING_HEART_BG_224X220_X8_Y373_BIN);
    compo_picturebox_set_pos(picbox, 120,480);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK7_HEART]) );///7天平均静态心率
    compo_textbox_set_location(textbox,14,380,205, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_WEEK7_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if( uteModuleHeartrGetCurrDayStaticHeartData() > 0 && uteModuleHeartrGetCurrDayStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );///7天平均静态心率 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-106,415,100,50);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_right_align(textbox,true);
    // compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,415+11,107, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    // compo_setid(textbox,COMPO_ID_HEART_STATIC_BPM_TXT);

    uint8_t heart_week_date[7];
    uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+13,512,190,94);
    compo_chartbox_set_pixel(chart, 1);

    chart_info.y = 0;
    chart_info.width = 18;   ///像素点
    for (int i=0; i<7; i++)
    {
        // heart_week_date[i] =80;
        chart_info.x = i*chart_info.width + i*10;
        chart_info.height = heart_week_date[i]*1.14;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 100))
    {
        f_heartrate->tick = tick_get();
//        area_t pic_size;
        char txt_buf[20];
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_textbox_t *txt_static_value = compo_getobj_byid(COMPO_ID_HEART_STATIC_VALUE_TXT);
        compo_textbox_t *txt_static_bpm = compo_getobj_byid(COMPO_ID_HEART_STATIC_BPM_TXT);
        compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_HEART_PIC);


        if(uteModuleHeartIsWear() == true)   ///佩戴处理
        {
            if(++f_heartrate->heart_pic_size >= 18)
                f_heartrate->heart_pic_size =0;
        }
        else
        {
            f_heartrate->heart_pic_size = 0;
            if(f_heartrate->no_wear_flag == false)
            {
                msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
            }
            f_heartrate->no_wear_flag = true;
        }


        compo_picturebox_cut(picbox, f_heartrate->heart_pic_size, 18);
        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_min,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
            compo_textbox_set(textbox_min,txt_buf);
        }

        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            compo_textbox_set(textbox_max,"--");
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
            compo_textbox_set(textbox_max,txt_buf);
        }

        if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)  ///佩戴处理
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
            compo_textbox_set(textbox_value,txt_buf);
        }
        else
        {
            compo_textbox_set(textbox_value,"--");
        }
        area_t txt_leng = widget_text_get_area(textbox_value->txt);
        compo_textbox_set_location(textbox_bpm,80+txt_leng.wid,77,100, widget_text_get_max_height());
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if( uteModuleHeartrGetCurrDayStaticHeartData() > 0 && uteModuleHeartrGetCurrDayStaticHeartData() != 255)  ///佩戴处理
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
            compo_textbox_set(txt_static_value,txt_buf);
        }
        else
        {
            compo_textbox_set(txt_static_value,"--");
        }
        txt_leng = widget_text_get_area(txt_static_value->txt);
        compo_textbox_set_location(txt_static_bpm,20+txt_leng.wid,320+10,100, widget_text_get_max_height());
    }

}
static void func_heartrate_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_HEART_BTN_ABOUT:
            uteTaskGuiStartScreen(FUNC_HEAR_ABOUT, 0, __func__);
            break;

        default:
            break;
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[50];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I338001_6_HEART_HEART_GIF_62X60_BIN);
    compo_picturebox_set_pos(picbox, 31+94/2, 78+86/2);
    compo_picturebox_cut(picbox, 0, 17);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,129,86);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,134+txt_leng.wid,122,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    // compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,42,180,48, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    // txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,98,180);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,188,180,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    // txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,244,180);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I338001_6_HEART_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, 180,75+210);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+18,235+93/2,213,93);
    compo_chartbox_set_pixel(chart, 1);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 5;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        // heart_date[i] =200;
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = heart_date[i]*0.465;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }
////////////////////////////////////////////////////////////////
    textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]) );///静态心率
    compo_textbox_set_location(textbox,41,389,88, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);


    // picbox = compo_picturebox_create(frm, UI_BUF_I338001_6_HEART_RESTING_HEART_RATE_BIN);
    // compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,292/2+497);

    // compo_button_t *btn = compo_button_create(frm);
    // compo_button_set_location(btn,115,295,30,30);
    // compo_setid(btn,COMPO_ID_HEART_BTN_ABOUT);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,41,425);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    // compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,46+txt_leng.wid,439,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_BPM_TXT);
//////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I338001_6_HEART_RESTING_HEART_RATE_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, 292/2+497);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK7_HEART]) );///7天平均静态心率
    compo_textbox_set_location(textbox,41,522,250, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_WEEK7_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if( uteModuleHeartrGetCurrDayStaticHeartData() > 0 && uteModuleHeartrGetCurrDayStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d %s",uteModuleHeartrGetLast7DayAvgStaticHeartData(),i18n[STR_PER_MINUTE]);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"-- %s",i18n[STR_PER_MINUTE]);
    }
    textbox = compo_textbox_create(frm, strlen(i18n[STR_CLOSE])  );///平均
    compo_textbox_set_location(textbox,35,563,108,widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_CLOSE]);
    compo_textbox_set_right_align(textbox,true);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    // compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) +20);///--次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,154,563,140, widget_text_get_max_height());
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    uint8_t heart_week_date[7];
    uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+13,126/2+626,246,126);
    compo_chartbox_set_pixel(chart, 1);

    chart_info.y = 0;
    chart_info.width = 24;   ///像素点
    for (int i=0; i<7; i++)
    {
        // heart_week_date[i] =80;
        chart_info.x = i*chart_info.width + i*13;
        chart_info.height = heart_week_date[i]*1.575;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(255,99,31));
    }

    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 100))
    {
        f_heartrate->tick = tick_get();
//        area_t pic_size;
        char txt_buf[50];
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_textbox_t *txt_static_value = compo_getobj_byid(COMPO_ID_HEART_STATIC_VALUE_TXT);
        // compo_textbox_t *txt_static_bpm = compo_getobj_byid(COMPO_ID_HEART_STATIC_BPM_TXT);
        compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_HEART_PIC);


        if(uteModuleHeartIsWear() == true)   ///佩戴处理
        {
            if(++f_heartrate->heart_pic_size >= 17)
                f_heartrate->heart_pic_size =0;
        }
        else
        {
            f_heartrate->heart_pic_size = 0;
        }
        compo_picturebox_cut(picbox, f_heartrate->heart_pic_size, 17);
        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_min,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
            compo_textbox_set(textbox_min,txt_buf);
        }

        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            compo_textbox_set(textbox_max,"--");
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
            compo_textbox_set(textbox_max,txt_buf);
        }

        if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)  ///佩戴处理
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
            compo_textbox_set(textbox_value,txt_buf);
        }
        else
        {
            compo_textbox_set(textbox_value,"--");
        }
        area_t txt_leng = widget_text_get_area(textbox_value->txt);
        compo_textbox_set_location(textbox_bpm,134+txt_leng.wid,122,100, widget_text_get_max_height());
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        // if( uteModuleHeartrGetCurrDayStaticHeartData() > 0 && uteModuleHeartrGetCurrDayStaticHeartData() != 255)  ///佩戴处理
        // {
        //     memset(txt_buf,0,sizeof(txt_buf));
        //     snprintf(txt_buf,sizeof(txt_buf),"%d %s",uteModuleHeartrGetCurrDayStaticHeartData(),i18n[STR_PER_MINUTE]);
        //     compo_textbox_set(txt_static_value,txt_buf);
        // }
        // else
        // {
        //     memset(txt_buf,0,sizeof(txt_buf));
        //     snprintf(txt_buf,"-- %s",i18n[STR_PER_MINUTE]);
        //     compo_textbox_set(txt_static_value,txt_buf);
        // }
        // txt_leng = widget_text_get_area(txt_static_value->txt);
        // compo_textbox_set_location(txt_static_bpm,20+txt_leng.wid,320+10,100, widget_text_get_max_height());
    }

}
static void func_heartrate_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_HEART_BTN_ABOUT:
            uteTaskGuiStartScreen(FUNC_HEAR_ABOUT, 0, __func__);
            break;

        default:
            break;
    }
}

#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_HEART_ICON_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6-CONTROL_Y);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
//    if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255){  ///佩戴处理
//        memset(txt_buf,0,sizeof(txt_buf));
//        snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
//        textbox = compo_textbox_create(frm, 3 );///次/分 数据
//        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
//        compo_textbox_set_align_center(textbox, false);
//        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2.3-CONTROL_Y);
//        compo_textbox_set(textbox,txt_buf);
//        compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);
//    }else{
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2+2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2.3-CONTROL_Y);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);
//    }

    area_t txt_leng = widget_text_get_area(textbox->txt);
//    printf("leng:%d\n",txt_leng.wid);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.3+txt_leng.wid,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.75-CONTROL_Y,58,28);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.45,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y,50,23);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.45+26,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X*1.1,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y,50,23);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X*1.1+26,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.4);
    compo_textbox_set_align_center(textbox, false);


//    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255){
//        memset(txt_buf,0,sizeof(txt_buf));
//        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
//        textbox = compo_textbox_create(frm, 3);///最低数据
//        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2.85,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y);
//        compo_textbox_set(textbox,txt_buf);
//        compo_textbox_set_forecolor(textbox, COLOR_GRAY);
//        compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
//    }else{
//        textbox = compo_textbox_create(frm, 3);///最低数据
//        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2.85,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y);
//        compo_textbox_set(textbox,"--");
//        compo_textbox_set_forecolor(textbox, COLOR_GRAY);
//        compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
//    }

    ///*第二页*/
    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

//    for (int i=0; i<24; i++)heart_date[i]=120;

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_HEART_MAP_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y/1.1);
    ///创建图表
    compo_chartbox_t *chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/7, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y/1.15,196,153);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart, COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 7;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i;
        chart_info.height = heart_date[i]/1.31;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 10+(f_heartrate->heart_pic_size/20)))
    {
        f_heartrate->tick = tick_get();
        area_t pic_size;
        char txt_buf[20];
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_HEART_PIC);

        pic_size = gui_image_get_size(UI_BUF_I330001_HEART_ICON_BIN);

        if(uteModuleHeartIsWear() == true)   ///佩戴处理
        {
            if(f_heartrate->heart_pic_size <= 72)
            {
                f_heartrate->heart_pic_state = true;
            }
            else if(f_heartrate->heart_pic_size >= 110)
            {
                f_heartrate->heart_pic_state = false;
            }

            if(f_heartrate->heart_pic_state == true)
            {
                f_heartrate->heart_pic_size ++ ;
            }
            else
            {
                f_heartrate->heart_pic_size -- ;
            }

        }
        else
        {
            f_heartrate->heart_pic_size = 100;
        }

        compo_picturebox_set_size(picbox,f_heartrate->heart_pic_size*pic_size.wid/100,f_heartrate->heart_pic_size*pic_size.hei/100);

        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_min,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
            compo_textbox_set(textbox_min,txt_buf);
        }

        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_max,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
            compo_textbox_set(textbox_max,txt_buf);
        }

        if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)  ///佩戴处理
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
            compo_textbox_set(textbox_value,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"--");
            compo_textbox_set(textbox_value,txt_buf);
        }
        area_t txt_leng = widget_text_get_area(textbox_value->txt);
        compo_textbox_set_location(textbox_bpm,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.3+txt_leng.wid,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.75-CONTROL_Y,58,28);
    }

}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_ICON_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6-CONTROL_Y);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_location(textbox,104,250-37,80,65);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_right_align(textbox, true);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X+10,250,67,34);
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_SHANG_BIN);
    compo_picturebox_set_pos(picbox, 93+28/2,304+22/2);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,125,302);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_XIA_BIN);
    compo_picturebox_set_pos(picbox, 235-28/2,308+22/2);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,239,302);
    compo_textbox_set_align_center(textbox, false);

    ///*第二页*/

    ///设置标题栏名字///
    textbox = compo_textbox_create(frm,strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_location(textbox,GUI_SCREEN_WIDTH/2,390+38/2,120,38);
    compo_textbox_set(textbox, i18n[STR_HEART_RATE]);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    // for (int i=0; i<24; i++)heart_date[i]=120;

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_HEART_MAP_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y+7);
    ///创建图表
    compo_chartbox_t *chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, 200, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y+20,238,157);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart, COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 8;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i*2;
        chart_info.height = heart_date[i]/1.04347;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 10+(f_heartrate->heart_pic_size/20)))
    {
        f_heartrate->tick = tick_get();
        area_t pic_size;
        char txt_buf[20];
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_HEART_PIC);

        pic_size = gui_image_get_size(UI_BUF_I332001_HEART_ICON_BIN);

        if(uteModuleHeartIsWear() == true)   ///佩戴处理
        {
            if(f_heartrate->heart_pic_size <= 72)
            {
                f_heartrate->heart_pic_state = true;
            }
            else if(f_heartrate->heart_pic_size >= 110)
            {
                f_heartrate->heart_pic_state = false;
            }

            if(f_heartrate->heart_pic_state == true)
            {
                f_heartrate->heart_pic_size ++ ;
            }
            else
            {
                f_heartrate->heart_pic_size -- ;
            }

        }
        else
        {
            f_heartrate->heart_pic_size = 100;
        }

        compo_picturebox_set_size(picbox,f_heartrate->heart_pic_size*pic_size.wid/100,f_heartrate->heart_pic_size*pic_size.hei/100);

        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_min,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
            compo_textbox_set(textbox_min,txt_buf);
        }

        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
            compo_textbox_set(textbox_max,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
            compo_textbox_set(textbox_max,txt_buf);
        }

        if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)  ///佩戴处理
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_hrs_data_get());
            compo_textbox_set(textbox_value,txt_buf);
        }
        else
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"--");
            compo_textbox_set(textbox_value,txt_buf);
        }
        // area_t txt_leng = widget_text_get_area(textbox_value->txt);
        // compo_textbox_set_pos(textbox_bpm,120+txt_leng.wid,250);
    }

}

#else
compo_form_t *func_heartrate_form_create(void)
{
}
static void func_heartrate_refresh(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//心率功能事件处理
static void func_heartrate_process(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;

    if(f_heartrate->ptm)
    {
        compo_page_move_process(f_heartrate->ptm);
    }

    if(bsp_sensor_hr_work_status() && uteModuleHeartGetWorkMode() == HR_WORK_MODE_HR && uteModuleHeartGetHeartValue() == 0)
    {
        reset_guioff_delay();
    }

    func_heartrate_refresh();
    func_process();
}

//心率功能消息处理
static void func_heartrate_message(size_msg_t msg)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_heartrate->ptm)
            {
                compo_page_move_touch_handler(f_heartrate->ptm);
            }
            break;
        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
            func_heartrate_click_handler();
#endif
            break;
        case MSG_SYS_1S:
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入心率功能
static void func_heartrate_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_heartrate_t));
    func_cb.frm_main = func_heartrate_form_create();

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 562,
        .page_count = 1,
        .quick_jump_perc = 80,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    func_cb.flag_animation = false;
    if(uteModuleHeartIsWear() == false)
    {
        msgbox(i18n[STR_PLEASE_WEAR], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 860,
        .page_count = 1,
        .quick_jump_perc = 80,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    func_cb.flag_animation = false;
#elif (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT)
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
        .title_used = true,
        .page_size = GUI_SCREEN_HEIGHT - (GUI_SCREEN_HEIGHT / 8),
#else
        .title_used = false,
        .page_size = GUI_SCREEN_HEIGHT,
#endif
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = 100,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    // bsp_sensor_hr_init(0);
    func_cb.flag_animation = false;

    f_heartrate->heart_pic_size = 100;
#endif
    uteModuleHeartStartSingleTesting(TYPE_HEART);
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
}

//退出心率功能
static void func_heartrate_exit(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if (f_heartrate->ptm)
    {
        func_free(f_heartrate->ptm);
    }
    // bsp_sensor_hr_stop();
    uteModuleHeartStopSingleTesting(TYPE_HEART);
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
}

//心率功能
void func_heartrate(void)
{
    printf("%s\n", __func__);
    func_heartrate_enter();
    while (func_cb.sta == FUNC_HEARTRATE)
    {
        func_heartrate_process();
        func_heartrate_message(msg_dequeue());
    }
    func_heartrate_exit();
}

#endif
