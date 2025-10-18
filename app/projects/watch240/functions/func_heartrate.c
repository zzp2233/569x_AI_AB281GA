#include "include.h"
#include "func.h"
#include "ute_module_heart.h"
#include "ute_module_menstrualcycle.h"
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
    COMPO_ID_HEART_VALUE_TXT,
    COMPO_ID_HEART_BPM_TXT,
    COMPO_ID_HEART_STATIC_WEEK_VALUE_TXT,
    COMPO_ID_CHART,
    COMPO_ID_CHART_WEEK,
    COMPO_ID_HEART_BTN_ABOUT,


    COMPO_ID_PIC_BG,
    COMPO_ID_TXT_VALUE,
    COMPO_ID_PIC_UNIT,
    COMPO_ID_MAX_VLA,
    COMPO_ID_MIN_VLA,
    COMPO_ID_PIC_ABOUT,
    COMPO_ID_TXT_TEST,
    COMPO_ID_CHART_VALUE,

    COMPO_ID_TXT_TMEE_TIP,
    COMPO_ID_TXT_TMEE,
    COMPO_ID_TXT_START,
    COMPO_ID_BTN_START,
    COMPO_ID_PIC_MAX,
    COMPO_ID_PIC_MIN,
    COMPO_ID_PIC_CHART,
    COMPO_ID_TXT_NODATE,

    COMPO_ID_PIC_GIF,
    COMPO_ID_TXT_TIP,
    COMPO_ID_TITLE,
    COMPO_ID_TITLE_TIME,

    COMPO_ID_TXT_MES_FAIL,

    COMPO_ID_TXT_WEAR_FAIL,

    COMPO_ID_BTN_RSTART,

    COMPO_ID_TXT_REST,
    COMPO_ID_TXT_WEEK,
    COMPO_ID_TXT_DATE,
    COMPO_ID_TXT_UINT,
    COMPO_ID_TXT_CHART,
    COMPO_ID_PIC_STAC_BG,

    COMPO_ID_SCROLLBAR,

    COMPO_ID_SHAPE_CHART,
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
    u8 heart_state;
    u32 tick_start;
    bool touch_flag;
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
    // compo_textbox_set_location(textbox,14,114,60, widget_text_get_max_height());//有些语言自适应长度处理不好，暂时不做自适应长度
    compo_textbox_set_location(textbox,14,114,50, widget_text_get_max_height());
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
    // compo_textbox_set_pos(textbox,20+txt_leng.wid,114);
    compo_textbox_set_pos(textbox,70,114);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    // compo_textbox_set_location(textbox,120,114,60, widget_text_get_max_height());
    compo_textbox_set_location(textbox,120,114,50, widget_text_get_max_height());
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
    // compo_textbox_set_pos(textbox,126+txt_leng.wid,114);
    compo_textbox_set_pos(textbox,180,114);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_6_HEART_1_1_ICON_BG_DATA_HEART_DATE_BG_224X110_X8_Y162_BIN);
    compo_picturebox_set_pos(picbox, 120,148+60);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+38/2,202,168,71);
    compo_chartbox_set_pixel(chart, 1);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        // heart_date[i] =200;
        chart_info.x = i*chart_info.width + i*3;
        chart_info.height = heart_date[i]*0.34;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }
////////////////////////////////////////////////////////////////
#if GUI_SCREEN_SIZE_240X296RGB_I335004_SUPPORT
    textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]) );///静态心率
    compo_textbox_set_location(textbox,14,285+10,88, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_6_HEART_1_1_ICON_ABOUT_18X18_X176_Y299_BIN);
    compo_picturebox_set_pos(picbox,115,295+10);

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
    if( uteModuleHeartrGetLast7DayAvgStaticHeartData() > 0 && uteModuleHeartrGetLast7DayAvgStaticHeartData() != 255)
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

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,415+11,107, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    uint8_t heart_week_date[7];
    uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+13,512,190,94);
    compo_chartbox_set_pixel(chart, 1);

    ute_module_systemtime_time_t time, readTime;
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    chart_info.y = 0;
    chart_info.width = 18;   ///像素点
    for (int i=0; i<7; i++)
    {
        chart_info.x = i*chart_info.width + i*10;
        chart_info.height = heart_week_date[i]*1.14;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);

        memcpy(&readTime, &time, sizeof(ute_module_systemtime_time_t));
        uteModuleSystemtimeInputDateIncreaseDecreaseDay(&readTime, i - 6);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",readTime.day);
        textbox = compo_textbox_create(frm, 2);//七天
        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_12_BIN);
        compo_textbox_set_pos(textbox,48+i*27,566);
        compo_textbox_set(textbox,txt_buf);
        compo_textbox_set_forecolor(textbox,COLOR_GRAY);
    }

#else
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
    if( uteModuleHeartrGetLast7DayAvgStaticHeartData() > 0 && uteModuleHeartrGetLast7DayAvgStaticHeartData() != 255)
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

    ute_module_systemtime_time_t time, readTime;
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    chart_info.y = 0;
    chart_info.width = 18;   ///像素点
    for (int i=0; i<7; i++)
    {
        // heart_week_date[i] =80;
        chart_info.x = i*chart_info.width + i*10;
        chart_info.height = heart_week_date[i]*1.14;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);

        memcpy(&readTime, &time, sizeof(ute_module_systemtime_time_t));
        uteModuleSystemtimeInputDateIncreaseDecreaseDay(&readTime, i - 6);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",readTime.day);
        textbox = compo_textbox_create(frm, 2);//七天
        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_12_BIN);
        compo_textbox_set_pos(textbox,46+i*28,566);
        compo_textbox_set(textbox,txt_buf);
        compo_textbox_set_forecolor(textbox,COLOR_GRAY);
    }
#endif
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

            f_heartrate->no_wear_flag = false;
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
#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
enum
{
    HEART_STA_IDLE,            //空闲
    HEART_STA_TESTING,         //测量中
    HEART_STA_SUCCSEE,         //测量成功
    HEART_STA_FAIL,            //测量失败
    HEART_STA_UNWEAR,          //未佩戴
};
#define DATE_DEBUG 0
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
extern bool start_testing;
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[100];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    ute_module_heart_test_last_info_t lastInfo;
    uteModuleHeartGetLastTestInfo(&lastInfo);
    int heart_value = lastInfo.lastValue;

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    bool isWoking = (start_testing && !uteModuleHeartIsAutoTestFlag());//(uteModuleHeartGetWorkMode() == HR_WORK_MODE_HR) ? true :false;
    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_HEART_RATE]);
    compo_setid(frm->title, COMPO_ID_TITLE);
    compo_setid(frm->time, COMPO_ID_TITLE_TIME);
///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_00_BIN);
    compo_picturebox_set_pos(picbox, 25 + 60 / 2, 79 + 60 / 2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_BG);
    compo_textbox_t *textbox;

    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X,154,GUI_SCREEN_WIDTH,70);
    widget_page_scale_to(widget_page,GUI_SCREEN_WIDTH * 0.8,70 * 0.8);
    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(i18n[STR_LAST_UPDATED]) + 2);
    compo_textbox_set_location(textbox,0,30,230,46);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_visible(textbox,isWoking ? false : (heart_value ? true :false));
    compo_textbox_set_alpha(textbox,180);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s:",i18n[STR_LAST_UPDATED]);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_TMEE_TIP);
    area_t txt_leng = widget_text_get_area(textbox->txt);

    ute_module_systemtime_time_t sysTime;
    uteModuleSystemtimeGetTime(&sysTime);

    memset(txt_buf,0,sizeof(txt_buf));
    if (lastInfo.lastTestTime.day == sysTime.day)
    {
        if (sysTime.is12HourTime)
        {
            uint8_t hour = lastInfo.lastTestTime.hour;
            hour = hour % 12;
            hour = hour ? hour : 12;
            if (lastInfo.lastTestTime.hour >= 12)
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_PM]);
            }
            else
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_AM]);
            }
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d", lastInfo.lastTestTime.hour, lastInfo.lastTestTime.min);
        }
    }
    else
    {
        if (lastInfo.lastTestTime.year == sysTime.year)
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d/%02d", lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%04d/%02d/%02d", lastInfo.lastTestTime.year, lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
    }

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(txt_buf));
    compo_textbox_set_visible(textbox,isWoking ? false : (heart_value ? true :false));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox, 10 + (txt_leng.wid > 230 ? 230 : txt_leng.wid), 30, 133, 46);
    compo_textbox_set_alpha(textbox,180);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_TMEE);

    textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,92,64 + 20);
    compo_textbox_set_align_center(textbox, false);
    if(heart_value)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,102+txt_leng.wid,106,100, widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,make_color(0xff,0x74,0x00));
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_PIC_UNIT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_START_MEAR]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,392 + 44 / 2,300,46);
    compo_textbox_set(textbox,i18n[STR_START_MEAR]);
    compo_textbox_set_forecolor(textbox,make_color(0xff,0x74,0x00));
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_TXT_START);

    compo_button_t *btn = compo_button_create(frm);///重新/测量按钮
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X,390 + 40 / 2, 300, 80);
    compo_button_set_visible(btn, !isWoking);
    compo_setid(btn,COMPO_ID_BTN_START);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMaxHeartValue() > 0 && uteModuleHeartGetMaxHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 10);///最高数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,79,344,100,56);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_MAX_VLA);


    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 10);///最低数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,267,344,100,56);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_MIN_VLA);
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_ICON_MAX_1_BIN);
    compo_picturebox_set_pos(picbox, 50 + 26 / 2,358+21/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_MAX);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_ICON_MINI_1_BIN);
    compo_picturebox_set_pos(picbox, 235 + 26 / 2,358+21/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_MIN);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,171 + 170/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_CHART);

#if DATE_DEBUG
    for (int i=0; i<24; i++)
    {
        heart_date[i] = get_random(101);
    }
    heart_date[1] = 0;
    heart_date[23] = 100;
#endif
    int crc = 0;
    for (int i=0; i<24; i++)
    {
        crc = crc + heart_date[i];
    }
    textbox = compo_textbox_create(frm, strlen(i18n[STR_NO_DATA]));///
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,230 + 40 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking ? false : (crc ? false :true));
    compo_textbox_set_alpha(textbox,180);
    compo_textbox_set(textbox,i18n[STR_NO_DATA]);
    compo_setid(textbox,COMPO_ID_TXT_NODATE);
    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC_U, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,193 + 125 / 2,312,125);
    compo_chartbox_set_pixel(chart, 1);
    widget_set_visible(chart->chart,!isWoking);
    compo_setid(chart,COMPO_ID_CHART_VALUE);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*7 + 4;
        chart_info.height = heart_date[i]*0.625f;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0xff,0x74,0x00));
    }

    //测量中界面
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_ICON_HEART_GIF_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 137 + 124 / 2);
    compo_picturebox_cut(picbox, 0, 20);
    compo_picturebox_set_visible(picbox,isWoking);
    compo_setid(picbox,COMPO_ID_PIC_GIF);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_MESSAGE1]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,300 + 46 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking);
    compo_textbox_set(textbox,i18n[STR_MESSAGE1]);
    compo_setid(textbox,COMPO_ID_TXT_TEST);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SOS_QBCSWJZ]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,358 + 46 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking);
    compo_textbox_set(textbox,i18n[STR_SOS_QBCSWJZ]);
    compo_setid(textbox,COMPO_ID_TXT_TIP);

    //测量失败
    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEAR_CHECK]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,300,150);
    compo_textbox_set_visible(textbox,false);
    widget_text_set_ellipsis(textbox->txt, false);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox,i18n[STR_WEAR_CHECK]);
    compo_setid(textbox,COMPO_ID_TXT_MES_FAIL);

    btn = compo_button_create_by_image(frm,UI_BUF_I341001_6_HEART_AGAIN01_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X,346 + 80 / 2);
    compo_button_set_visible(btn, false);
    compo_setid(btn,COMPO_ID_BTN_RSTART);
    //佩戴检测失败
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PLEASE_WEAR]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,300,100);
    compo_textbox_set_visible(textbox,false);
    widget_text_set_ellipsis(textbox->txt, false);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox,i18n[STR_PLEASE_WEAR]);
    compo_setid(textbox,COMPO_ID_TXT_WEAR_FAIL);

    //第二页
////////////////////////////////////////////////////////////////
    textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]) );///静态心率
    compo_textbox_set_location(textbox,24,455,320, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox,make_color(0xff,0x74,0x00));
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);
    compo_setid(textbox,COMPO_ID_TXT_REST);

    memset(txt_buf,0,sizeof(txt_buf));
    if( uteModuleHeartrGetCurrDayStaticHeartData() > 0 && uteModuleHeartrGetCurrDayStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_pos(textbox,24,481);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,37+txt_leng.wid,512,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_BPM_TXT);
//////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_RESTING_HEART_RATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,550 + 146 / 2);
    compo_setid(picbox,COMPO_ID_PIC_STAC_BG);
    ute_module_systemtime_time_t sys_time = {0};
    uteModuleSystemtimeGetTime(&sys_time);
    uint8_t year = sys_time.year;
    uint8_t month = sys_time.month;
    if(sys_time.day < 7)
    {
        month = (sys_time.month + 12 - 1) % 13 + 1;
        year = (sys_time.month < 1) ? (year - 1) : year;
    }
    uint16_t mon_day_count = uteModuleMenstrualCycleJudgeMonthDay(year, month) + 1;
    for(u8 i = 0; i < 7; i++)
    {
        textbox = compo_textbox_create(frm,2);///7天平均静态心率
        compo_textbox_set_location(textbox,GUI_SCREEN_WIDTH / 8 * (i + 1),550 + 170,44,44);
        memset(txt_buf,0,sizeof(txt_buf));
        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_24_BIN);
        int day = ((sys_time.day - 6 + i) % mon_day_count > 0) ? (sys_time.day - 6 + i) % mon_day_count : (mon_day_count - 1 + sys_time.day - 6 + i) % mon_day_count;
        snprintf(txt_buf,sizeof(txt_buf),"%d",day);
        compo_textbox_set(textbox,txt_buf);
        compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    }

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK7_HEART]) );///7天平均静态心率
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,747 + 46 / 2,300, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_WEEK7_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_TXT_WEEK);

    memset(txt_buf,0,sizeof(txt_buf));
    if( uteModuleHeartrGetLast7DayAvgStaticHeartData() > 0 && uteModuleHeartrGetLast7DayAvgStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///7天平均静态心率 数据
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X-114, 784 + 11, 100, 50);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_right_align(textbox, true);
    compo_setid(textbox, COMPO_ID_TXT_DATE);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X-14+8, 784+11, 107, widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
    compo_setid(textbox, COMPO_ID_TXT_UINT);

    uint8_t heart_week_date[7];
    uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率
    chart = compo_chartbox_create(frm, CHART_TYPE_BAR, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,570 + 124 / 2,318,126);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_TXT_CHART);
#if DATE_DEBUG
    for (int i=0; i<7; i++)
    {
        heart_week_date[i] = get_random(101);
    }
    heart_week_date[1] = 0;
    heart_week_date[3] = 150;
    heart_week_date[5] = 100;
#endif
    compo_shape_t *shape;
    chart_info.y = 0;
    chart_info.width = 18;   ///像素点
    for (int i=0; i<7; i++)
    {
        if (heart_week_date[i] > 100)
        {
            heart_week_date[i] = 100;
        }
        chart_info.x = i*chart_info.width + i*28 + 14;
        chart_info.height = heart_week_date[i]*1.26;///心率数据转换为柱形条显示数据
        if (chart_info.height > 10)
        {
            chart_info.height -= 2;
        }
        compo_chartbox_set_value(chart, i, chart_info, make_color(0xff,0x74,0x00));
        shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_color(shape, make_color(0xff,0x74,0x00));
        if (heart_week_date[i] > 10)
        {
            compo_shape_set_location(shape, 34 + i*chart_info.width + i*28 + 14, 570 + (126 - heart_week_date[i]*1.26) + 2, 18, 4);
        }
        compo_shape_set_radius(shape, 2);
        compo_setid(shape, COMPO_ID_SHAPE_CHART + i);
    }

    compo_scroll_t* scroll = compo_scroll_create(frm, SCROLL_TYPE_VERTICAL);
    compo_setid(scroll, COMPO_ID_SCROLLBAR);
    compo_scroll_set_w_r(scroll, 10);
    compo_scroll_set_range(scroll, 96);
    compo_scroll_set_color(scroll, COLOR_WHITE, make_color(51, 51, 51));
    compo_scroll_set_pos(scroll, 352, 78);
    compo_scroll_set_value(scroll, 0);
    compo_scroll_set_edge_circle(scroll, true);
    widget_set_visible(scroll->scroll_bg, false);
    widget_set_visible(scroll->scroll_fg, false);

    return frm;
}

static void func_blood_heart_date_refresh(void)
{
    compo_textbox_t *text_value = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    compo_textbox_t *txt_nodate = compo_getobj_byid(COMPO_ID_TXT_NODATE);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);
    compo_textbox_t *text_tip = compo_getobj_byid(COMPO_ID_TXT_TMEE_TIP);
    compo_textbox_t *text_time = compo_getobj_byid(COMPO_ID_TXT_TMEE);
    compo_textbox_t *text_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);

    compo_textbox_t *text_sta_value = compo_getobj_byid(COMPO_ID_HEART_STATIC_VALUE_TXT);
    compo_textbox_t *text_sta_value_unit = compo_getobj_byid(COMPO_ID_HEART_STATIC_BPM_TXT);
    compo_textbox_t *text_static_value = compo_getobj_byid(COMPO_ID_TXT_DATE);
    char txt_buf[100];
    uint8_t heart_max;
    uint8_t heart_min;

    ute_module_heart_test_last_info_t lastInfo;
    uteModuleHeartGetLastTestInfo(&lastInfo);

    int heart_val = lastInfo.lastValue;

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的血氧

    if(heart_val)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(text_value,txt_buf);
    area_t txt_leng = widget_text_get_area(text_value->txt);
    compo_textbox_set_location(text_uint,102+txt_leng.wid,106,100, widget_text_get_max_height());
    compo_textbox_set_visible(text_uint,heart_val ? true : false);

    compo_textbox_set_visible(text_tip,heart_val ? true : false);

    ute_module_systemtime_time_t sysTime;
    uteModuleSystemtimeGetTime(&sysTime);

    memset(txt_buf, 0, sizeof(txt_buf));
    if (lastInfo.lastTestTime.day == sysTime.day)
    {
        if (sysTime.is12HourTime)
        {
            uint8_t hour = lastInfo.lastTestTime.hour;
            hour = hour % 12;
            hour = hour ? hour : 12;
            if (lastInfo.lastTestTime.hour >= 12)
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_PM]);
            }
            else
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_AM]);
            }
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d", lastInfo.lastTestTime.hour, lastInfo.lastTestTime.min);
        }
    }
    else
    {
        if (lastInfo.lastTestTime.year == sysTime.year)
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d/%02d", lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%04d/%02d/%02d", lastInfo.lastTestTime.year, lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
    }

    compo_textbox_set(text_time,txt_buf);
    compo_textbox_set_visible(text_time,heart_val ? true : false);
    memset(txt_buf,0,sizeof(txt_buf));
    if(heart_val > 0 && heart_val != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",heart_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(max_value,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(min_value,txt_buf);

#if DATE_DEBUG
    for (int i=0; i<24; i++)
    {
        heart_date[i] = get_random(101);
    }
    heart_date[1] = 0;
    heart_date[23] = 100;
#endif
    int crc = 0;
    for (int i=0; i<24; i++)
    {
        crc = crc + heart_date[i];
    }
    compo_textbox_set_visible(txt_nodate,crc ? false : true);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*7 + 4;
        chart_info.height = heart_date[i]*0.625f;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0xff,0x74,0x00));
    }
    memset(txt_buf,0,sizeof(txt_buf));
    if( uteModuleHeartrGetCurrDayStaticHeartData() > 0 && uteModuleHeartrGetCurrDayStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set_visible(text_sta_value, true);
    compo_textbox_set(text_sta_value,txt_buf);
    compo_textbox_set_location(text_sta_value_unit, 37 + widget_text_get_area(text_sta_value->txt).wid, 512, 100, widget_text_get_max_height());

    memset(txt_buf,0,sizeof(txt_buf));
    if( uteModuleHeartrGetLast7DayAvgStaticHeartData() > 0 && uteModuleHeartrGetLast7DayAvgStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(text_static_value,txt_buf);
    compo_textbox_set_visible(text_static_value, true);

}

void func_shape_update(bool visible)
{
    u8 i;
    for (i = 0; i< 7; i++)
    {
        compo_shape_t *shape = compo_getobj_byid(COMPO_ID_SHAPE_CHART + i);
        compo_shape_set_visible(shape, visible);
    }
}

static void func_heart_layout_init(u8 state)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_textbox_t *text_tip = compo_getobj_byid(COMPO_ID_TXT_TMEE_TIP);
    compo_textbox_t *text_time = compo_getobj_byid(COMPO_ID_TXT_TMEE);
    compo_textbox_t *text_value = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_textbox_t *text_start = compo_getobj_byid(COMPO_ID_TXT_START);
    compo_textbox_t *text_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);

    compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START);

    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    compo_picturebox_t * picbox_max = compo_getobj_byid(COMPO_ID_PIC_MAX);
    compo_picturebox_t * picbox_min = compo_getobj_byid(COMPO_ID_PIC_MIN);
    compo_picturebox_t * picbox_chart = compo_getobj_byid(COMPO_ID_PIC_CHART);
    compo_textbox_t *txt_nodate = compo_getobj_byid(COMPO_ID_TXT_NODATE);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);

    compo_picturebox_t * picbox_gif = compo_getobj_byid(COMPO_ID_PIC_GIF);
    compo_textbox_t *txt_test = compo_getobj_byid(COMPO_ID_TXT_TEST);
    compo_textbox_t *txt_tip = compo_getobj_byid(COMPO_ID_TXT_TIP);

    compo_textbox_t *txt_fail = compo_getobj_byid(COMPO_ID_TXT_MES_FAIL);
    compo_textbox_t *txt_w_fail = compo_getobj_byid(COMPO_ID_TXT_WEAR_FAIL);
    compo_button_t *btn_rstart = compo_getobj_byid(COMPO_ID_BTN_RSTART);

    compo_textbox_t *text_sta_value = compo_getobj_byid(COMPO_ID_HEART_STATIC_VALUE_TXT);
    compo_picturebox_t *picbox_sta_bg = compo_getobj_byid(COMPO_ID_PIC_STAC_BG);
    compo_textbox_t *txt_uint = compo_getobj_byid(COMPO_ID_HEART_STATIC_BPM_TXT);
    compo_textbox_t *txt_rest = compo_getobj_byid(COMPO_ID_TXT_REST);
    compo_textbox_t *txt_week = compo_getobj_byid(COMPO_ID_TXT_WEEK);
    compo_textbox_t *text_static_value = compo_getobj_byid(COMPO_ID_TXT_DATE);
    compo_textbox_t *txt_uint1 = compo_getobj_byid(COMPO_ID_TXT_UINT);
    compo_chartbox_t* chart_sta  = compo_getobj_byid(COMPO_ID_TXT_CHART);
    compo_textbox_t *txt_title = compo_getobj_byid(COMPO_ID_TITLE);
    compo_textbox_t *txt_time = compo_getobj_byid(COMPO_ID_TITLE_TIME);
    area_t txt_leng = {0};
    switch (state)
    {
        case HEART_STA_IDLE:
            compo_picturebox_set_visible(picbox,true);
            compo_textbox_set_visible(text_tip,true);
            compo_textbox_set_visible(text_time,true);
            compo_textbox_set_visible(text_value,true);
            txt_leng = widget_text_get_area(text_value->txt);
            compo_textbox_set_location(text_uint,102+txt_leng.wid,106,100, widget_text_get_max_height());
            compo_textbox_set_visible(text_uint,true);
            compo_textbox_set(text_start,i18n[STR_START_MEAR]);
            compo_textbox_set_visible(text_start,true);
            compo_button_set_visible(btn_start, true);
            compo_textbox_set_visible(max_value,true);
            compo_textbox_set_visible(min_value,true);
            compo_picturebox_set_visible(picbox_max,true);
            compo_picturebox_set_visible(picbox_min,true);
            compo_picturebox_set_visible(picbox_chart,true);
            compo_textbox_set_visible(txt_nodate,true);
            widget_set_visible(chart->chart, true);
            compo_textbox_set_visible(txt_title,true);
            compo_textbox_set_visible(txt_time,true);

            compo_textbox_set_visible(text_sta_value,true);
            compo_picturebox_set_visible(picbox_sta_bg,true);
            compo_textbox_set_visible(txt_uint,true);
            compo_textbox_set_visible(txt_rest,true);
            compo_textbox_set_visible(txt_week,true);
            compo_textbox_set_visible(text_static_value,true);
            compo_textbox_set_visible(txt_uint1,true);
            widget_set_visible(chart_sta->chart,true);
            func_shape_update(true);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            compo_button_set_visible(btn_rstart, false);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            func_blood_heart_date_refresh();
            break;
        case HEART_STA_TESTING:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(text_start,false);
            compo_button_set_visible(btn_start, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);
            compo_textbox_set_visible(text_uint,false);
            compo_textbox_set_visible(txt_title,true);
            compo_textbox_set_visible(txt_time,true);

            compo_textbox_set_visible(text_sta_value,false);
            compo_picturebox_set_visible(picbox_sta_bg,false);
            compo_textbox_set_visible(txt_uint,false);
            compo_textbox_set_visible(txt_rest,false);
            compo_textbox_set_visible(txt_week,false);
            compo_textbox_set_visible(text_static_value,false);
            compo_textbox_set_visible(txt_uint1,false);
            widget_set_visible(chart_sta->chart,false);
            func_shape_update(false);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_button_set_visible(btn_rstart, false);

            compo_picturebox_set_visible(picbox_gif,true);
            compo_textbox_set_visible(txt_test,true);
            compo_textbox_set_visible(txt_tip,true);
            break;
        case HEART_STA_SUCCSEE:
            compo_picturebox_set_visible(picbox,true);
            compo_textbox_set_visible(text_tip,true);
            compo_textbox_set_visible(text_time,true);
            compo_textbox_set_visible(text_value,true);
            txt_leng = widget_text_get_area(text_value->txt);
            compo_textbox_set_location(text_uint,102+txt_leng.wid,106,100, widget_text_get_max_height());
            compo_textbox_set_visible(text_uint,true);
            compo_textbox_set(text_start,i18n[STR_AGAIN]);
            compo_textbox_set_visible(text_start,true);
            compo_button_set_visible(btn_start, true);
            compo_textbox_set_visible(max_value,true);
            compo_textbox_set_visible(min_value,true);
            compo_picturebox_set_visible(picbox_max,true);
            compo_picturebox_set_visible(picbox_min,true);
            compo_picturebox_set_visible(picbox_chart,true);
            compo_textbox_set_visible(txt_nodate,true);
            widget_set_visible(chart->chart, true);
            compo_textbox_set_visible(txt_title,true);
            compo_textbox_set_visible(txt_time,true);

            compo_textbox_set_visible(text_sta_value,true);
            compo_picturebox_set_visible(picbox_sta_bg,true);
            compo_textbox_set_visible(txt_uint,true);
            compo_textbox_set_visible(txt_rest,true);
            compo_textbox_set_visible(txt_week,true);
            compo_textbox_set_visible(text_static_value,true);
            compo_textbox_set_visible(txt_uint1,true);
            widget_set_visible(chart_sta->chart,true);
            func_shape_update(true);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_button_set_visible(btn_rstart, false);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            func_blood_heart_date_refresh();
            break;
        case HEART_STA_FAIL:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(text_start,false);
            compo_textbox_set_visible(text_uint,false);
            compo_button_set_visible(btn_start, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);

            compo_textbox_set_visible(text_sta_value,false);
            compo_picturebox_set_visible(picbox_sta_bg,false);
            compo_textbox_set_visible(txt_uint,false);
            compo_textbox_set_visible(txt_rest,false);
            compo_textbox_set_visible(txt_week,false);
            compo_textbox_set_visible(text_static_value,false);
            compo_textbox_set_visible(txt_uint1,false);
            widget_set_visible(chart_sta->chart,false);
            func_shape_update(false);

            compo_textbox_set_visible(txt_fail,true);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_button_set_visible(btn_rstart, true);
            compo_textbox_set_visible(txt_title,false);
            compo_textbox_set_visible(txt_time, false);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            break;
        case HEART_STA_UNWEAR:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(text_uint,false);
            compo_textbox_set_visible(text_start,false);
            compo_button_set_visible(btn_start, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);
            compo_textbox_set_visible(txt_title,false);
            compo_textbox_set_visible(txt_time,false);

            compo_textbox_set_visible(text_sta_value,false);
            compo_picturebox_set_visible(picbox_sta_bg,false);
            compo_textbox_set_visible(txt_uint,false);
            compo_textbox_set_visible(txt_rest,false);
            compo_textbox_set_visible(txt_week,false);
            compo_textbox_set_visible(text_static_value,false);
            compo_textbox_set_visible(txt_uint1,false);
            widget_set_visible(chart_sta->chart,false);
            func_shape_update(false);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,true);
            compo_button_set_visible(btn_rstart, true);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            break;
        default :
            break;
    }
}
#define TIME_OUT (20)

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    compo_picturebox_t * picbox_gif = compo_getobj_byid(COMPO_ID_PIC_GIF);
    compo_scroll_t* scroll = compo_getobj_byid(COMPO_ID_SCROLLBAR);
    uint8_t state = f_heartrate->heart_state;
    static uint8_t index_pic = 0;
    static int time_count = 0;
    switch(state)
    {
        case HEART_STA_TESTING:
            if (widget_get_visble(scroll->scroll_bg))
            {
                widget_set_visible(scroll->scroll_bg, false);
                widget_set_visible(scroll->scroll_fg, false);
            }
            if(tick_check_expire(f_heartrate->tick_start, 100))
            {
                f_heartrate->tick_start = tick_get();
                index_pic++;
                index_pic > 19 ? (index_pic = 0) : index_pic;
                compo_picturebox_cut(picbox_gif, index_pic, 20);
                time_count++;
                if (!uteModuleHeartIsWear()&& (time_count / 10 > 0))
                {
                    time_count = 0;
                    f_heartrate->heart_state = HEART_STA_UNWEAR;
                    func_heart_layout_init(f_heartrate->heart_state);
                    uteModuleHeartStopSingleTesting(TYPE_HEART);
                    uteModuleGuiCommonDisplayOffAllowGoBack(true);
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                }
                else if (uteModuleHeartIsWear() && (time_count / 10 > (TIME_OUT * 3)) && (!bsp_sensor_hrs_data_get() || bsp_sensor_hrs_data_get() == 255))
                {
                    time_count = 0;
                    f_heartrate->heart_state = HEART_STA_FAIL;
                    func_heart_layout_init(f_heartrate->heart_state);
                    uteModuleHeartStopSingleTesting(TYPE_HEART);
                    uteModuleGuiCommonDisplayOffAllowGoBack(true);
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);

                }
                else if(uteModuleHeartIsWear() == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255&& time_count / 10 >= TIME_OUT)  ///佩戴处理
                {
                    time_count = 0;
                    f_heartrate->heart_state = HEART_STA_SUCCSEE;
                    uteModuleHeartStopSingleTesting(TYPE_HEART);
                    uteModuleGuiCommonDisplayOffAllowGoBack(true);
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    func_heart_layout_init(f_heartrate->heart_state);
                }
            }
            break;
        case HEART_STA_SUCCSEE:
            if (widget_get_visble(scroll->scroll_bg))
            {
                widget_set_visible(scroll->scroll_bg, false);
                widget_set_visible(scroll->scroll_fg, false);
            }
            break;
        case HEART_STA_IDLE:
        {
            s32 dx, dy;
            s16 value = abs_s(f_heartrate->ptm->move_offset) * 1000 / abs_s(f_heartrate->ptm->down_spring_offset);
            compo_scroll_set_pos(scroll, 352, 78 - f_heartrate->ptm->move_offset);
            compo_scroll_set_value(scroll, value);
            if (f_heartrate->touch_flag)
            {
                f_heartrate->tick = tick_get();
                widget_set_visible(scroll->scroll_bg, true);
                widget_set_visible(scroll->scroll_fg, true);
                f_heartrate->touch_flag = false;
            }
            f_heartrate->touch_flag = ctp_get_dxy(&dx, &dy);
            if (tick_check_expire(f_heartrate->tick, 3000))
            {
                widget_set_visible(scroll->scroll_bg, false);
                widget_set_visible(scroll->scroll_fg, false);
            }
        }
        break;
        default:
            if (widget_get_visble(scroll->scroll_bg))
            {
                widget_set_visible(scroll->scroll_bg, false);
                widget_set_visible(scroll->scroll_fg, false);
            }
            break;
    }

}
static void func_heartrate_click_handler(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_START:
        {
            if(f_heartrate->heart_state == HEART_STA_IDLE || f_heartrate->heart_state == HEART_STA_SUCCSEE)
            {
                f_heartrate->ptm->move_offset = - (GUI_SCREEN_HEIGHT / 8); //系统标题栏高度
                //compo_page_move_process(f_heartrate->ptm);
                widget_page_set_client(f_heartrate->ptm->page, 0, f_heartrate->ptm->move_offset);
                uteModuleHeartStartSingleTesting(TYPE_HEART);
                uteModuleGuiCommonDisplayOffAllowGoBack(false);
                f_heartrate->heart_state = HEART_STA_TESTING;
                func_heart_layout_init(f_heartrate->heart_state);
            }
        }
        break;
        case COMPO_ID_BTN_RSTART:
        {
            uteModuleHeartStartSingleTesting(TYPE_HEART);
            uteModuleGuiCommonDisplayOffAllowGoBack(false);
            f_heartrate->heart_state = HEART_STA_TESTING;
            func_heart_layout_init(f_heartrate->heart_state);

        }
        break;
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
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
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_ICON_HEART_GIF_BIN);
    compo_picturebox_set_pos(picbox, 12+90/2, 80+84/2);
    compo_picturebox_cut(picbox, 0, 18);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3);///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,136,78);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,146+txt_leng.wid,130,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    // compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,25,206,80, widget_text_get_max_height());
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
    compo_textbox_set_pos(textbox,30+txt_leng.wid,206);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,193,206,80, widget_text_get_max_height());
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
    compo_textbox_set_pos(textbox,198+txt_leng.wid,206);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, 342/2+12,170/2+249);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, 87+244/2,277+106/2,244,106);
    compo_chartbox_set_pixel(chart, 1);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 5;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i*5;
        chart_info.height = heart_date[i]*0.53;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }
////////////////////////////////////////////////////////////////
    textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]) );///静态心率
    compo_textbox_set_location(textbox,24,455,245, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    txt_leng = widget_text_get_area(textbox->txt);
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_ABOUT_BIN);
    compo_picturebox_set_pos(picbox,txt_leng.wid < 240 ? 34+txt_leng.wid+12 : 306,475);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,txt_leng.wid < 240 ? 34+txt_leng.wid+12 : 306,475,40,40);
    compo_setid(btn,COMPO_ID_HEART_BTN_ABOUT);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_pos(textbox,27,476);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,37+txt_leng.wid,512,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_BPM_TXT);
//////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_RESTING_HEART_RATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,572+336/2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK7_HEART]) );///7天平均静态心率
    compo_textbox_set_location(textbox,33,592,280, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_WEEK7_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if( uteModuleHeartrGetLast7DayAvgStaticHeartData() > 0 && uteModuleHeartrGetLast7DayAvgStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );///7天平均静态心率 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-106,647,100,50);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_right_align(textbox,true);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,647+11,107, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    // compo_setid(textbox,COMPO_ID_HEART_STATIC_BPM_TXT);

    uint8_t heart_week_date[7];
    uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC_U, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,793,282,144);
    compo_chartbox_set_pixel(chart, 1);

    chart_info.y = 0;
    chart_info.width = 28;   ///像素点
    for (int i=0; i<7; i++)
    {
        chart_info.x = i*chart_info.width + i*15+18;
        chart_info.height = heart_week_date[i]*1.8;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(255, 60, 32));
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
        compo_textbox_set_location(textbox_bpm,146+txt_leng.wid,130,100, widget_text_get_max_height());
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
        compo_textbox_set_location(txt_static_bpm,37+txt_leng.wid,512,100, widget_text_get_max_height());
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

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

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
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I343001_6_HEART_HEART_GIF_62X60_BIN);
    compo_picturebox_set_pos(picbox, 12+93/2, 70+86/2);
    compo_picturebox_cut(picbox, 0, 17);
    compo_setid(picbox,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3);///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,110,81);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,110+txt_leng.wid,113,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    // compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,23,179,80, widget_text_get_max_height());
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
    compo_textbox_set_pos(textbox,23+txt_leng.wid,178);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,169,179,80, widget_text_get_max_height());
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
    compo_textbox_set_pos(textbox,169+txt_leng.wid,179);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I343001_6_HEART_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, 298/2+11,148/2+215);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, 77+212/2,237+93/2,212,93);
    compo_chartbox_set_pixel(chart, 1);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 5;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = heart_date[i]*0.53;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }
////////////////////////////////////////////////////////////////
    textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]) );///静态心率
    compo_textbox_set_location(textbox,22,396,245, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    txt_leng = widget_text_get_area(textbox->txt);
    picbox = compo_picturebox_create(frm, UI_BUF_I343001_6_HEART_ABOUT_BIN);
    compo_picturebox_set_pos(picbox,txt_leng.wid < 240 ? 22+txt_leng.wid+10+20/2 : 306,403+20/2);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,txt_leng.wid < 240 ? 22+txt_leng.wid+10+20/2 : 306,403+20/2,40,40);
    compo_setid(btn,COMPO_ID_HEART_BTN_ABOUT);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_pos(textbox,24,431);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,24+txt_leng.wid,460,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_BPM_TXT);
//////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I343001_6_HEART_RESTING_HEART_RATE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,497+292/2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK7_HEART]) );///7天平均静态心率
    compo_textbox_set_location(textbox,29,515,280, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_WEEK7_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if( uteModuleHeartrGetLast7DayAvgStaticHeartData() > 0 && uteModuleHeartrGetLast7DayAvgStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );///7天平均静态心率 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-106,563,100,50);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_right_align(textbox,true);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,567,107, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    // compo_setid(textbox,COMPO_ID_HEART_STATIC_BPM_TXT);

    uint8_t heart_week_date[7];
    uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC_U, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, 53+246/2,652+101/2,246,101);
    compo_chartbox_set_pixel(chart, 1);

    chart_info.y = 0;
    chart_info.width = 24;   ///像素点
    for (int i=0; i<7; i++)
    {
        chart_info.x = i*chart_info.width + i*13;
        chart_info.height = heart_week_date[i]*1.8;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(255, 60, 32));
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
            if(++f_heartrate->heart_pic_size >= 17)
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
        compo_textbox_set_location(textbox_bpm,110+txt_leng.wid,113,100, widget_text_get_max_height());
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
        compo_textbox_set_location(txt_static_bpm,24+txt_leng.wid,460,100, widget_text_get_max_height());
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

#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I338002_6_HEART_HEART_GIF_BIN);
    compo_animation_set_pos(animation,94+48/2,41+46/2);  //需要更替为弹窗图标
    compo_animation_set_radix(animation,17);
    compo_animation_set_interval(animation,20);
    compo_setid(animation,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,154,40);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);
    area_t txt_leng = widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,164+txt_leng.wid,57,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338002_6_HEART_DATE_BGZ_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,180);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,173,292,102);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        // heart_date[i] =200;
        chart_info.x = i*chart_info.width + i*6;
        chart_info.height = heart_date[i]*0.465;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(252,25,82));
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_6_HEART_MAX_BIN);
    compo_picturebox_set_pos(picbox, 90,293);

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
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,108,280);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_6_HEART_MIN_BIN);
    compo_picturebox_set_pos(picbox, 216,293);

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
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,234,280);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_5_ACTIVITY_NEXT_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,332);
////////////////////////////////////////////////////////////////////////////////////

    textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]) );///静态心率
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,35+GUI_SCREEN_HEIGHT,105, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartrGetCurrDayStaticHeartData() > 0 && uteModuleHeartrGetCurrDayStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(textbox,0,65+GUI_SCREEN_HEIGHT,GUI_SCREEN_CENTER_X-10,40);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_right_align(textbox, true);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,180,71+GUI_SCREEN_HEIGHT,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    uint8_t heart_week_date[7];
    uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_6_HEART_RESTING_HEART_RATE_BG_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+108+144/2);

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+5,GUI_SCREEN_HEIGHT+175,312-13,90);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART_WEEK);

    ute_module_systemtime_time_t time, readTime;
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    chart_info.y = 0;
    chart_info.width = 20;   ///像素点
    for (int i=0; i<7; i++)
    {
        // heart_week_date[i] =80;
        chart_info.x = i*chart_info.width + i*25;
        chart_info.height = heart_week_date[i]*1.125;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(252,25,82));

        memcpy(&readTime, &time, sizeof(ute_module_systemtime_time_t));
        uteModuleSystemtimeInputDateIncreaseDecreaseDay(&readTime, i - 6);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",readTime.day);
        textbox = compo_textbox_create(frm, 2);//七天
        // compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_12_BIN);
        compo_textbox_set_pos(textbox,43+i*45,233+GUI_SCREEN_HEIGHT);
        compo_textbox_set(textbox,txt_buf);
        compo_textbox_set_forecolor(textbox,COLOR_GRAY);
    }

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK7_HEART]) );///7天平均静态心率
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,12+266+GUI_SCREEN_HEIGHT,250, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_WEEK7_HEART]);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartrGetLast7DayAvgStaticHeartData() > 0 && uteModuleHeartrGetLast7DayAvgStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(textbox,0,303+GUI_SCREEN_HEIGHT,GUI_SCREEN_CENTER_X-10,40);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_right_align(textbox, true);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_WEEK_VALUE_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,180,310+GUI_SCREEN_HEIGHT,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    return frm;
#else
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
    compo_textbox_set_location(textbox,41,389,105, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);


    picbox = compo_picturebox_create(frm, UI_BUF_I338001_7_SPO2_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,20+146,403);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,20+146,403,40,40);///关于
    compo_setid(btn,COMPO_ID_HEART_BTN_ABOUT);

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
#endif
}

// 刷新心率
static void func_heartrate_refresh(void)
{
#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 200))
    {
        f_heartrate->tick = tick_get();

        char txt_buf[20];
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *txt_static_value = compo_getobj_byid(COMPO_ID_HEART_STATIC_VALUE_TXT);
        compo_textbox_t *txt_static_week_value = compo_getobj_byid(COMPO_ID_HEART_STATIC_WEEK_VALUE_TXT);
        compo_chartbox_t* chart_heart = compo_getobj_byid(COMPO_ID_CHART);
        compo_chartbox_t* chart_heart_week = compo_getobj_byid(COMPO_ID_CHART_WEEK);
        compo_animation_t *animation = compo_getobj_byid(COMPO_ID_HEART_PIC);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(uteModuleHeartIsWear() != f_heartrate->no_wear_flag)
        {
            f_heartrate->no_wear_flag = uteModuleHeartIsWear();
            if(f_heartrate->no_wear_flag)
            {
                compo_animation_set_interval(animation,20);
            }
            else
            {
                compo_animation_set_interval(animation,0);
                msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_NONE);
            }
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(f_heartrate->no_wear_flag == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)   //心率
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

        compo_textbox_set_pos(textbox_bpm,164+txt_leng.wid,57);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(txt_buf,0,sizeof(txt_buf));   //心率最小
        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
        }
        compo_textbox_set(textbox_min,txt_buf);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(txt_buf,0,sizeof(txt_buf));   //心率最大
        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
        }
        compo_textbox_set(textbox_max,txt_buf);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(txt_buf,0,sizeof(txt_buf));   //静息心率
        if(uteModuleHeartrGetCurrDayStaticHeartData() == 0 || uteModuleHeartrGetCurrDayStaticHeartData() == 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
        }
        compo_textbox_set(txt_static_value,txt_buf);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(txt_buf,0,sizeof(txt_buf));   //静息心率7 day
        if(uteModuleHeartrGetLast7DayAvgStaticHeartData() == 0 || uteModuleHeartrGetLast7DayAvgStaticHeartData() == 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
        }
        compo_textbox_set(txt_static_week_value,txt_buf);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        uint8_t heart_date[24];
        uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率
        chart_t chart_info;
        chart_info.y = 0;
        chart_info.width = 6;   ///像素点
        for (int i=0; i<CHART_NUM; i++)
        {
            // heart_date[i] =200;
            chart_info.x = i*chart_info.width + i*6;
            chart_info.height = heart_date[i]*0.465;///心率数据转换为柱形条显示数据
            compo_chartbox_set_value(chart_heart, i, chart_info, make_color(252,25,82));
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        uint8_t heart_week_date[7];
        uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

        chart_info.y = 0;
        chart_info.width = 20;   ///像素点
        for (int i=0; i<7; i++)
        {
            // heart_week_date[i] =80;
            chart_info.x = i*chart_info.width + i*25;
            chart_info.height = heart_week_date[i]*1.125;///心率数据转换为柱形条显示数据
            compo_chartbox_set_value(chart_heart_week, i, chart_info, make_color(252,25,82));
        }

    }
#else
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 100))
    {
        f_heartrate->tick = tick_get();
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
#endif
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
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
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
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I340001_HEART_ICON_BIN);
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

    picbox = compo_picturebox_create(frm, UI_BUF_I340001_HEART_SHANG_BIN);
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

    picbox = compo_picturebox_create(frm, UI_BUF_I340001_HEART_XIA_BIN);
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

    picbox = compo_picturebox_create(frm, UI_BUF_I340001_HEART_MAP_BIN);
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

        pic_size = gui_image_get_size(UI_BUF_I340001_HEART_ICON_BIN);

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

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I342001_6_HEART_GIF_BIN);
    compo_animation_set_pos(animation,58+42/2,21+42/2);  //需要更替为弹窗图标
    compo_animation_set_radix(animation,17);
    compo_animation_set_interval(animation,20);
    compo_setid(animation,COMPO_ID_HEART_PIC);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(textbox,102,35);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_HEART_VALUE_TXT);
    area_t txt_leng = widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,110+txt_leng.wid,35,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    compo_setid(textbox,COMPO_ID_HEART_BPM_TXT);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I342001_6_HEART_DATE_BGZ_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,73+94/2);

    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,72+96/2-10,206,80);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<CHART_NUM; i++)
    {
//         heart_date[i] =200;
        chart_info.x = i*chart_info.width + i*5;
        chart_info.height = heart_date[i]*0.465;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(252,25,82));
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_6_HEART_MAX_BIN);
    compo_picturebox_set_pos(picbox, 53+12/2, 186+12/2);

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
    compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,72,180);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_6_HEART_MIN_BIN);
    compo_picturebox_set_pos(picbox, 137+12/2,186+12/2);

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
    compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,156,180);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_6_HEART_NEXT_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,213+16/2);
////////////////////////////////////////////////////////////////////////////////////

    textbox = compo_textbox_create(frm, strlen(i18n[STR_RESTING_HEART]) );///静态心率
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,15+GUI_SCREEN_HEIGHT,105, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_RESTING_HEART]);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartrGetCurrDayStaticHeartData() > 0 && uteModuleHeartrGetCurrDayStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_20_BIN);
    compo_textbox_set_location(textbox,0,38+GUI_SCREEN_HEIGHT,GUI_SCREEN_CENTER_X-20,40);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_right_align(textbox, true);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_VALUE_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,120,38+GUI_SCREEN_HEIGHT,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    uint8_t heart_week_date[7];
    uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_6_HEART_RESTING_HEART_RATE_BG_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+72+96/2-10);

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+5,GUI_SCREEN_HEIGHT+72+96/2-20,206,70);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART_WEEK);

    chart_info.y = 0;
    chart_info.width = 14;   ///像素点
    for (int i=0; i<7; i++)
    {
//         heart_week_date[i] =80;
        chart_info.x = i*chart_info.width + i*16;
        chart_info.height = heart_week_date[i]*1.125;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(252,25,82));
    }

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK7_HEART]) );///7天平均静态心率
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,177+GUI_SCREEN_HEIGHT,220, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_WEEK7_HEART]);

    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartrGetLast7DayAvgStaticHeartData() > 0 && uteModuleHeartrGetLast7DayAvgStaticHeartData() != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );///次/分 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_20_BIN);
    compo_textbox_set_location(textbox,0,200+GUI_SCREEN_HEIGHT,GUI_SCREEN_CENTER_X-20,40);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_right_align(textbox, true);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_HEART_STATIC_WEEK_VALUE_TXT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]) );///次/分
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,120,200+GUI_SCREEN_HEIGHT,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);

    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if(tick_check_expire(f_heartrate->tick, 200))
    {
        f_heartrate->tick = tick_get();

        char txt_buf[20];
        compo_textbox_t *textbox_bpm = compo_getobj_byid(COMPO_ID_HEART_BPM_TXT);
        compo_textbox_t *textbox_max = compo_getobj_byid(COMPO_ID_HEART_MAX_TXT);
        compo_textbox_t *textbox_min = compo_getobj_byid(COMPO_ID_HEART_MIN_TXT);
        compo_textbox_t *textbox_value = compo_getobj_byid(COMPO_ID_HEART_VALUE_TXT);
        compo_textbox_t *txt_static_value = compo_getobj_byid(COMPO_ID_HEART_STATIC_VALUE_TXT);
        compo_textbox_t *txt_static_week_value = compo_getobj_byid(COMPO_ID_HEART_STATIC_WEEK_VALUE_TXT);
        compo_chartbox_t* chart_heart = compo_getobj_byid(COMPO_ID_CHART);
        compo_chartbox_t* chart_heart_week = compo_getobj_byid(COMPO_ID_CHART_WEEK);
        compo_animation_t *animation = compo_getobj_byid(COMPO_ID_HEART_PIC);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(uteModuleHeartIsWear() != f_heartrate->no_wear_flag)
        {
            f_heartrate->no_wear_flag = uteModuleHeartIsWear();
            if(f_heartrate->no_wear_flag)
            {
                compo_animation_set_interval(animation,20);
            }
            else
            {
                compo_animation_set_interval(animation,0);
                msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_NONE);
            }
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        if(f_heartrate->no_wear_flag == true && bsp_sensor_hrs_data_get() > 0 && bsp_sensor_hrs_data_get() != 255)   //心率
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

        compo_textbox_set_pos(textbox_bpm,110+txt_leng.wid,35);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(txt_buf,0,sizeof(txt_buf));   //心率最小
        if(uteModuleHeartGetMinHeartValue() == 0 || uteModuleHeartGetMinHeartValue() == 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMinHeartValue());
        }
        compo_textbox_set(textbox_min,txt_buf);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(txt_buf,0,sizeof(txt_buf));   //心率最大
        if(uteModuleHeartGetMaxHeartValue() == 0 || uteModuleHeartGetMaxHeartValue() == 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartGetMaxHeartValue());
        }
        compo_textbox_set(textbox_max,txt_buf);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(txt_buf,0,sizeof(txt_buf));   //静息心率
        if(uteModuleHeartrGetCurrDayStaticHeartData() == 0 || uteModuleHeartrGetCurrDayStaticHeartData() == 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetCurrDayStaticHeartData());
        }
        compo_textbox_set(txt_static_value,txt_buf);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        memset(txt_buf,0,sizeof(txt_buf));   //静息心率7 day
        if(uteModuleHeartrGetLast7DayAvgStaticHeartData() == 0 || uteModuleHeartrGetLast7DayAvgStaticHeartData() == 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleHeartrGetLast7DayAvgStaticHeartData());
        }
        compo_textbox_set(txt_static_week_value,txt_buf);
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        uint8_t heart_date[24];
        uteModuleHeartGetTodayHistoryData(heart_date,24);///获取一天的心率
        chart_t chart_info;
        chart_info.y = 0;
        chart_info.width = 4;   ///像素点
        for (int i=0; i<CHART_NUM; i++)
        {
//             heart_date[i] =200;
            chart_info.x = i*chart_info.width + i*5;
            chart_info.height = heart_date[i]*0.465;///心率数据转换为柱形条显示数据
            compo_chartbox_set_value(chart_heart, i, chart_info, make_color(252,25,82));
        }
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
        uint8_t heart_week_date[7];
        uteModuleHeartrGetWeekDayStaticHeartData(heart_week_date);///获取心率

        chart_info.y = 0;
        chart_info.width = 14;   ///像素点
        for (int i=0; i<7; i++)
        {
//             heart_week_date[i] =80;
            chart_info.x = i*chart_info.width + i*16;
            chart_info.height = heart_week_date[i]*1.125;///心率数据转换为柱形条显示数据
            compo_chartbox_set_value(chart_heart_week, i, chart_info, make_color(252,25,82));
        }

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
        f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
        s32 dx, dy;
        s16 value = abs_s(f_heartrate->ptm->move_offset) * 1000 / abs_s(f_heartrate->ptm->down_spring_offset);
        compo_scroll_t* scroll = compo_getobj_byid(COMPO_ID_SCROLLBAR);
        compo_scroll_set_pos(scroll, 352, 78 - f_heartrate->ptm->move_offset);
        compo_scroll_set_value(scroll, value);

        if (f_heartrate->touch_flag)
        {
            f_heartrate->tick = tick_get();
            widget_set_visible(scroll->scroll_bg, true);
            widget_set_visible(scroll->scroll_fg, true);
            f_heartrate->touch_flag = false;
        }
        f_heartrate->touch_flag = ctp_get_dxy(&dx, &dy);
        if (tick_check_expire(f_heartrate->tick, 3000))
        {
            widget_set_visible(scroll->scroll_bg, false);
            widget_set_visible(scroll->scroll_fg, false);
        }
    }
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    if(f_heartrate->heart_state == HEART_STA_TESTING)
    {
        reset_guioff_delay();
    }
#else
    if(bsp_sensor_hr_work_status() && uteModuleHeartGetWorkMode() == HR_WORK_MODE_HR && uteModuleHeartGetHeartValue() == 0 && uteModuleHeartIsWear())//解决未佩戴时心率界面不灭屏问题
    {
        reset_guioff_delay();
    }
#endif
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
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            if(f_heartrate->heart_state == HEART_STA_IDLE || f_heartrate->heart_state == HEART_STA_SUCCSEE)
            {
                if(f_heartrate->ptm)
                {
                    compo_page_move_touch_handler(f_heartrate->ptm);
                }
            }
#else
            if(f_heartrate->ptm)
            {
                compo_page_move_touch_handler(f_heartrate->ptm);
            }
#endif
            break;
        case MSG_CTP_CLICK:
//            func_cb.sta = FUNC_HEART_WARNING;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT \
    || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            func_heartrate_click_handler();
#endif
            break;
        case MSG_SYS_1S:
            break;
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case MSG_CTP_SHORT_RIGHT:
        case KU_BACK:
        case KU_DELAY_BACK:
        {
            if(f_heartrate->heart_state != HEART_STA_IDLE)
            {
                f_heartrate->heart_state = HEART_STA_IDLE;
                uteModuleHeartStopSingleTesting(TYPE_HEART);
                func_heart_layout_init(f_heartrate->heart_state);
            }
            else
            {
                func_shape_update(false);
                func_message(msg);
                func_shape_update(true);
            }
        }
        break;
#endif
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
#if GUI_SCREEN_SIZE_240X296RGB_I335004_SUPPORT
        .page_size = 572,
#else
#if UTE_SCREEN_SCROLL_OFFSET_HEIGHT
        .page_size = 562+UTE_SCREEN_SCROLL_OFFSET_HEIGHT,
#else
        .page_size = 562,
#endif
#endif
        .page_count = 1,
        .quick_jump_perc = 80,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    func_cb.flag_animation = false;
#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 858,
        .page_count = 1,
        .quick_jump_perc = 80,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    f_heartrate->touch_flag = true;
    func_cb.flag_animation = false;
    f_heartrate->heart_state = HEART_STA_IDLE;
    if(start_testing)//小组件主动测量
    {
        start_testing = false;
        if(!uteModuleHeartIsAutoTestFlag())
        {
            f_heartrate->heart_state = HEART_STA_TESTING;
        }
    }

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 932,
        .page_count = 1,
        .quick_jump_perc = 80,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    func_cb.flag_animation = false;
    f_heartrate->heart_state = HEART_STA_IDLE;
#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 810,
        .page_count = 1,
        .quick_jump_perc = 80,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    func_cb.flag_animation = false;
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 2,
        .quick_jump_perc = GUI_SCREEN_HEIGHT/2,
        .jump_perc = 20,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    f_heartrate->no_wear_flag = true;
#else
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
#endif
#elif (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT)
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
#if !GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    uteModuleHeartStartSingleTesting(TYPE_HEART);
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
#endif
}

//退出心率功能
static void func_heartrate_exit(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if (f_heartrate->ptm)
    {
        func_free(f_heartrate->ptm);
    }
    if(uteModuleHeartIsSingleTesting())
    {
        uteModuleHeartStopSingleTesting(TYPE_HEART);
    }
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
