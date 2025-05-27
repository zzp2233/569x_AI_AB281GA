#include "include.h"
#include "func.h"
#include "ute_module_sport.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
enum
{
    ARC_ANIM_STATUS_START = 0,
    ARC_ANIM_STATUS_FW,
    ARC_ANIM_STATUS_BW,
    ARC_ANIM_STATUS_END,
};

enum
{
    ///文本框
    STEP_TXT_VALUE_ID=1,
    KCAL_TXT_VALUE_ID,
    KM_TXT_VALUE_ID,

    STEP_TXT_UNIT_ID,
    KCAL_TXT_UNIT_ID,
    KM_TXT_UNIT_ID,

    KCAL_ARC_ID,
    KM_ARC_ID,
    STEP_ARC_ID,

    STEP_DAY_TXT_VALUE_ID,
    STEP_DAY_TXT_VALUE_TARGET_ID,
    STEP_DAY_TXT2_VALUE_TARGET_ID,
    STEP_DAY_CAHRT_VALUE_ID,

    STEP_WEEK_TXT_VALUE_ID,
    STEP_WEEK_TXTT_VALUE_TARGET_ID,
    STEP_WEEK_CAHRT_VALUE_ID,
};

typedef struct f_activity_t_
{
    uint32_t tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
    page_tp_move_t *ptm;
    bool uint_km;
} f_activity_t;


#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

compo_form_t *func_activity_form_create(void)
{
    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    u8 pic_dis = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    if(totalStepCnt > uteModuleSportGetStepsTargetCnt())
    {
        pic_dis=10;
    }
    else
    {
        if(totalStepCnt!=0)
        {
            pic_dis = totalStepCnt*10/uteModuleSportGetStepsTargetCnt();
        }
    }
    u8 km_integer  = distance/100;                //距离 整数
    u8 km_decimals = distance%100;               //距离 小数

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_EVREY_DAY_ACTIVITY]);
    compo_picturebox_t * pic;
    compo_textbox_t *textbox;
    area_t txt_leng;
#define TXT_SPACING  87

    pic = compo_picturebox_create(frm,UI_BUF_I335001_5_ACTIVITY_TOP_ICON_PIC_ICON_KCAL_86X144_X28_Y64_00_BIN);
    compo_picturebox_set_pos(pic,86/2+28,144/2+64);
    compo_picturebox_cut(pic,pic_dis,11);
    compo_setid(pic,KCAL_ARC_ID);

    pic = compo_picturebox_create(frm,UI_BUF_I335001_5_ACTIVITY_TOP_ICON_PIC_ICON_STEPS_164X64_X38_Y178_00_BIN);
    compo_picturebox_set_pos(pic,164/2+38,64/2+178);
    compo_picturebox_cut(pic,pic_dis,11);
    compo_setid(pic,STEP_ARC_ID);

    pic = compo_picturebox_create(frm,UI_BUF_I335001_5_ACTIVITY_TOP_ICON_PIC_ICON_DIS_86X144_X126_Y64_00_BIN);
    compo_picturebox_set_pos(pic,86/2+126,144/2+64);
    compo_picturebox_cut(pic,pic_dis,11);
    compo_setid(pic,KM_ARC_ID);

    pic = compo_picturebox_create(frm,UI_BUF_I335001_5_ACTIVITY_TOP_ICON_PIC_ICON_30X36_X22_Y315_KCAL_30X30_X30_Y108_BIN);
    compo_picturebox_set_pos(pic,30/2+30,30/2+108);

    pic = compo_picturebox_create(frm,UI_BUF_I335001_5_ACTIVITY_TOP_ICON_PIC_ICON_30X36_X22_Y315_DIS_30X30_X180_Y108_BIN);
    compo_picturebox_set_pos(pic,30/2+180,30/2+108);

    pic = compo_picturebox_create(frm,UI_BUF_I335001_5_ACTIVITY_TOP_ICON_PIC_ICON_30X36_X22_Y315_STEP_30X30_X106_Y237_BIN);
    compo_picturebox_set_pos(pic,30/2+106,30/2+237);


///////////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_I335001_5_ACTIVITY_ICON_KCAL_30X36_X22_Y316_BIN);
    compo_picturebox_set_pos(pic, 30/2+22, 36/2+316);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,305,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_CALORIE]);

    /*千卡数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75, 331);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,336,88,25);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
    compo_setid(textbox,KCAL_TXT_UNIT_ID);
///////////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_I335001_5_ACTIVITY_ICON_DIS_30X36_X22_Y401_BIN);
    compo_picturebox_set_pos(pic, 30/2+22, 36/2+316+TXT_SPACING);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DISTANCE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,305+TXT_SPACING,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_DISTANCE]);

    /*公里数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d.%02d",km_integer, km_decimals);///公里数据
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75, 331+TXT_SPACING);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,336+TXT_SPACING,88,25);
    compo_textbox_set(textbox, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
    compo_setid(textbox,KM_TXT_UNIT_ID);

///////////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_I335001_5_ACTIVITY_ICON_STEPS_30X36_X22_Y485_BIN);
    compo_picturebox_set_pos(pic, 30/2+22, 36/2+316+TXT_SPACING*2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SET_STEP_COUNT]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,305+TXT_SPACING*2,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_SET_STEP_COUNT]);

    /*步数数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm, 7);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75, 331+TXT_SPACING*2);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,336+TXT_SPACING*2,88,25);
    compo_textbox_set(textbox, i18n[STR_STEP]);
    compo_setid(textbox,STEP_TXT_UNIT_ID);
///////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_I335001_5_ACTIVITY_STEP_DETAILS_DATE_BG_224X220_X8_Y555_BIN);
    compo_picturebox_set_pos(pic, 224/2+8, 220/2+555);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP_DETAILS]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,15,563,150,25);
    compo_textbox_set_forecolor(textbox, make_color(0x12,0xa9,0x94));
    compo_textbox_set(textbox, i18n[STR_STEP_DETAILS]);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据目标
    textbox = compo_textbox_create(frm, 7);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,10,590,GUI_SCREEN_CENTER_X-13,25);
    compo_textbox_set_right_align(textbox, true);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,STEP_DAY_TXT_VALUE_ID);

    uint32_t target_step = uteModuleSportGetStepsTargetCnt();
    // uint32_t target_step = 0;
    if(target_step == 0)target_step = 8000;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"/%ld",target_step);///步数数据目标
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,594,GUI_SCREEN_CENTER_X-13,25);
    compo_textbox_set_forecolor(textbox, make_color(153,153,153));
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,STEP_DAY_TXT_VALUE_TARGET_ID);

    uint32_t step_date[24];
    uteModuleSportLoadTodayEveryHourStepHistoryData(step_date);
    compo_chartbox_t*chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+12,698,212,94);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,STEP_DAY_CAHRT_VALUE_ID);


    // printf("mb:target_step:%ld\n",target_step);
    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<24; i++)
    {
        // printf("step:%ld\n",step_date[i]);
        // step_date[i] =8000;
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = step_date[i]*(94*1000/target_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0,236,203));
    }

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",target_step);///day步数 小字号
    textbox = compo_textbox_create(frm, 7);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_12_BIN);
    compo_textbox_set_location(textbox,20,700-15-94/2,230,30);
    compo_textbox_set_forecolor(textbox, make_color(128,128,128));
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,STEP_DAY_TXT2_VALUE_TARGET_ID);

//////////////////////////////////////////////////////////////////////////////////
    uint32_t week_step_data;
    uint32_t week_step_date[7];
    uint32_t target_week_step = uteModuleSportLoadWeekDayStepHistoryData(week_step_date, &week_step_data);
    // target_week_step =18000;

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,15,800,150,25);
    compo_textbox_set_forecolor(textbox, make_color(0x12,0xa9,0x94));
    compo_textbox_set(textbox, i18n[STR_WEEK_STEP]);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",week_step_data);///week步数数据
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,850,230,30);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,STEP_WEEK_TXT_VALUE_ID);

    pic = compo_picturebox_create(frm,uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID ? UI_BUF_I335001_5_ACTIVITY_WEEKLY_DATA_01_ZH_218X112_X12_Y888_BIN : UI_BUF_I335001_5_ACTIVITY_WEEKLY_DATA_00_EN_218X112_X12_Y888_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 930);

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+12,926,GUI_SCREEN_WIDTH,77);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,STEP_WEEK_CAHRT_VALUE_ID);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",target_week_step);///week步数数据 最大值
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_12_BIN);
    compo_textbox_set_location(textbox,15,912-78/2,230,30);
    compo_textbox_set_forecolor(textbox, make_color(128,128,128));
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,STEP_WEEK_TXTT_VALUE_TARGET_ID);
    chart_info;
    chart_info.y = 0;
    chart_info.width = 12;   ///像素点
    for (int i=0; i<7; i++)
    {
        // week_step_date[i] =18000;
        chart_info.x = i*chart_info.width + i*22;
        chart_info.height = week_step_date[i]*(77*1000/target_week_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0,236,203));
    }

    compo_shape_t *shape = compo_shape_create_for_page(frm, frm->page_body, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,926-78/2,218,1);
    compo_shape_set_color(shape,make_color(51,51,51));

    return frm;
}
static void func_activity_disp_handle(void)
{
    compo_picturebox_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
    compo_picturebox_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
    compo_picturebox_t *arc_step = compo_getobj_byid(STEP_ARC_ID);

    compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
    compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
    compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);

    compo_textbox_t *textbox_kcal_unit = compo_getobj_byid(KCAL_TXT_UNIT_ID);
    compo_textbox_t *textbox_step_unit = compo_getobj_byid(STEP_TXT_UNIT_ID);
    compo_textbox_t *textbox_km_unit = compo_getobj_byid(KM_TXT_UNIT_ID);
///////////////////////////////////////////////////////////////////////////////
    compo_textbox_t *textbox_day_step = compo_getobj_byid(STEP_DAY_TXT_VALUE_ID);
    compo_textbox_t *textbox_day_step_target = compo_getobj_byid(STEP_DAY_TXT_VALUE_TARGET_ID);
    compo_textbox_t *textbox2_day_step_target = compo_getobj_byid(STEP_DAY_TXT2_VALUE_TARGET_ID);
    compo_chartbox_t*chart_day = compo_getobj_byid(STEP_DAY_CAHRT_VALUE_ID);
//////////////////////////////////////////////////////////////////////////////////
    compo_textbox_t *textbox_week_step = compo_getobj_byid(STEP_WEEK_TXT_VALUE_ID);
    compo_textbox_t *textbox_week_step_target = compo_getobj_byid(STEP_WEEK_TXTT_VALUE_TARGET_ID);
    compo_chartbox_t*chart_week = compo_getobj_byid(STEP_WEEK_CAHRT_VALUE_ID);
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    u8 pic_dis=0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    if(totalStepCnt > uteModuleSportGetStepsTargetCnt())
    {
        pic_dis=10;
    }
    else
    {
        if(totalStepCnt!=0)
        {
            pic_dis = totalStepCnt*10/uteModuleSportGetStepsTargetCnt();
        }
    }
    u8 km_integer  = distance/100;                //距离 整数
    u8 km_decimals = distance%100;               //距离 小数

    compo_picturebox_cut(arc_kcal,pic_dis,11);//进度图
    compo_picturebox_cut(arc_km,pic_dis,11);
    compo_picturebox_cut(arc_step,pic_dis,11);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);//步数
    compo_textbox_set(textbox_step, txt_buf);
    area_t txt_leng = widget_text_get_area(textbox_step->txt);
    compo_textbox_set_location(textbox_step_unit,75+8+txt_leng.wid,336+TXT_SPACING*2,88,25);//单位文本

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d.%02d",km_integer, km_decimals);///公里数据
    compo_textbox_set(textbox_km, txt_buf);
    txt_leng = widget_text_get_area(textbox_km->txt);
    compo_textbox_set_location(textbox_km_unit,75+8+txt_leng.wid,336+TXT_SPACING,88,25);//单位文本

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    compo_textbox_set(textbox_kcal, txt_buf);
    txt_leng = widget_text_get_area(textbox_kcal->txt);
    compo_textbox_set_location(textbox_kcal_unit,75+8+txt_leng.wid,336,88,25);//单位文本


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);//一天步数
    compo_textbox_set(textbox_day_step, txt_buf);

    uint32_t target_step = uteModuleSportGetStepsTargetCnt();
    if(target_step == 0)target_step = 8000;
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"/%ld",target_step);
    compo_textbox_set(textbox_day_step_target, txt_buf);///一天步数目标
    compo_textbox_set(textbox2_day_step_target, txt_buf);///一天步数目标 柱形图最大值

    uint32_t step_date[24];
    uteModuleSportLoadTodayEveryHourStepHistoryData(step_date);
    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = step_date[i]*(94*1000/target_step)/1000;
        compo_chartbox_set_value(chart_day, i, chart_info, make_color(0,236,203));//一天步数柱形图
    }
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    uint32_t week_step_data;
    uint32_t week_step_date[7];
    uint32_t target_week_step = uteModuleSportLoadWeekDayStepHistoryData(week_step_date, &week_step_data);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",week_step_data);///week步数数据
    compo_textbox_set(textbox_week_step, txt_buf);///一周步数

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",target_week_step);///week步数数据 最大值
    compo_textbox_set(textbox_week_step_target, txt_buf);///一周步数目标 柱形图最大值

    chart_info;
    chart_info.y = 0;
    chart_info.width = 12;   ///像素点
    for (int i=0; i<7; i++)
    {
        chart_info.x = i*chart_info.width + i*22;
        chart_info.height = week_step_date[i]*(77*1000/target_week_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart_week, i, chart_info, make_color(0,236,203));
    }


}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

compo_form_t *func_activity_form_create(void)
{
    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT

    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    uint32_t arc_step = 0;
    if(uteModuleSportGetStepsTargetCnt() != 0)
    {
        arc_step = (totalStepCnt*1000 / uteModuleSportGetStepsTargetCnt());
        if(arc_step>1000)arc_step=1000;
    }


    widget_page_t *page = widget_page_create(frm->page_body);///创建按键页面
    widget_set_location(page,GUI_SCREEN_WIDTH/2,100,GUI_SCREEN_WIDTH,316/2+28/2);

    compo_arc_t *arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,316/2,316,316);
    compo_arc_set_width(arc,28);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(76,16,12),0);
    compo_arc_set_value(arc,1000);
    compo_arc_set_edge_circle(arc,true,true);

    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,316/2,244,244);
    compo_arc_set_width(arc,28);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(76,64,0),0);
    compo_arc_set_value(arc,1000);
    compo_arc_set_edge_circle(arc,true,true);

    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,316/2,172,172);
    compo_arc_set_width(arc,28);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(0,73,64),0);
    compo_arc_set_value(arc,1000);
    compo_arc_set_edge_circle(arc,true,true);
///////////////////////////////////////////////////////////
    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,316/2,316,316);
    compo_arc_set_width(arc,28);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(252,55,40),make_color(76,16,12));
    compo_arc_set_value(arc,arc_step);
    compo_arc_set_edge_circle(arc,true,true);
    compo_setid(arc,KCAL_ARC_ID);

    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,316/2,244,244);
    compo_arc_set_width(arc,28);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(255,212,0),make_color(76,64,0));
    compo_arc_set_value(arc,arc_step);
    compo_arc_set_edge_circle(arc,true,true);
    compo_setid(arc,KM_ARC_ID);

    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,316/2,172,172);
    compo_arc_set_width(arc,28);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(0,242,214),make_color(0,73,64));
    compo_arc_set_value(arc,arc_step);
    compo_arc_set_edge_circle(arc,true,true);
    compo_setid(arc,STEP_ARC_ID);
////////////////////////////////////////////////////////////////////////////////
    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();

    compo_textbox_t *textbox;
    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,208);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338002_5_ACTIVITY_ICON_KCAL_BIN);
    compo_picturebox_set_size(picbox,32,32);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,218+37);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",distance/100, distance%100);///公里数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,208);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_5_ACTIVITY_ICON_DIS_BIN);
    compo_picturebox_set_size(picbox,32,32);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,218+37);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,249);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_5_ACTIVITY_ICON_STEPS_BIN);
    compo_picturebox_set_size(picbox,32,32);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,279+17);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_5_ACTIVITY_NEXT_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,328+24/2);
/////////////////////////////////////////////////////////////////////////////////////////////////
    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP_DETAILS]));
    compo_textbox_set(textbox, i18n[STR_STEP_DETAILS]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+42);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_5_ACTIVITY_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+108+144/2);

    /*步数数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+275+45/2);
    compo_setid(textbox,STEP_DAY_TXT_VALUE_ID);

    area_t txt_leng= widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));
    compo_textbox_set(textbox, i18n[STR_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+txt_leng.wid/2+16,GUI_SCREEN_HEIGHT+300);
    compo_textbox_set_forecolor(textbox,make_color(128,128,128));
    compo_setid(textbox,STEP_DAY_TXT_UNIT_ID);

    uint32_t target_step = uteModuleSportGetStepsTargetCnt();
    if(target_step == 0)target_step = 8000;
    uint32_t step_date[24];
    uteModuleSportLoadTodayEveryHourStepHistoryData(step_date);
    compo_chartbox_t*chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+170,306,104);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(textbox,STEP_DAY_CAHRT_VALUE_ID);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        // printf("step:%ld\n",step_date[i]);
        // step_date[i] =8000;
        chart_info.x = i*chart_info.width + i*7;
        chart_info.height = step_date[i]*(104*1000/target_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0,242,214));
    }

//////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t week_step_data;
    uint32_t week_step_date[7];
    uint32_t target_week_step = uteModuleSportLoadWeekDayStepHistoryData(week_step_date, &week_step_data);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_WEEK_STEP]));
    compo_textbox_set(textbox, i18n[STR_WEEK_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT*2+42);

    picbox = compo_picturebox_create(frm, uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID ? UI_BUF_I338002_5_ACTIVITY_WEEK_DATE_BG_ZH_BIN: UI_BUF_I338002_5_ACTIVITY_WEEK_DATE_BG_EN_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT*2+108+144/2);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",week_step_data);///week步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT*2+275+45/2);
    compo_setid(textbox,STEP_WEEK_TXT_VALUE_ID);

    txt_leng= widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));
    compo_textbox_set(textbox, i18n[STR_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+txt_leng.wid/2+16,GUI_SCREEN_HEIGHT*2+300);
    compo_textbox_set_forecolor(textbox,make_color(128,128,128));
    compo_setid(textbox,STEP_WEEK_TXT_UNIT_ID);

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+5,GUI_SCREEN_HEIGHT*2+170,312-13,100);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(textbox,STEP_WEEK_CAHRT_VALUE_ID);

    chart_info;
    chart_info.y = 0;
    chart_info.width = 20;   ///像素点
    for (int i=0; i<7; i++)
    {
        // week_step_date[i] =18000;
        // target_week_step = 18000;
        chart_info.x = i*chart_info.width + i*25;
        if(target_week_step != 0)
        {
            chart_info.height = week_step_date[i]*(100*1000/target_week_step)/1000;///心率数据转换为柱形条显示数据
        }
        else
        {
            chart_info.height = 0;///心率数据转换为柱形条显示数据
        }
        compo_chartbox_set_value(chart, i, chart_info,  make_color(0,242,214));
    }

//////////////////////////////////////////////////////////////////////////////////////////////////

#else
    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uint32_t totalStepCnt_handle = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    if(totalStepCnt > uteModuleSportGetStepsTargetCnt())
    {
        totalStepCnt_handle = uteModuleSportGetStepsTargetCnt();
    }
    else
    {
        if(totalStepCnt!=0)
        {
            totalStepCnt_handle = uteModuleSportGetStepsTargetCnt()/totalStepCnt;
        }
        else
        {
            totalStepCnt_handle = 0;
        }

    }
    u8 pic_dis = totalStepCnt_handle!=0 ? (100/(totalStepCnt_handle)/10 ) : 0;
    if(pic_dis>10)pic_dis=10;
    u8 km_integer  = distance/100;                //距离 整数
    u8 km_decimals = distance%100;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_EVREY_DAY_ACTIVITY]);


    compo_picturebox_t * pic;
    compo_textbox_t *textbox;
    area_t txt_leng;
#define TXT_SPACING  87

    pic = compo_picturebox_create(frm,UI_BUF_I338001_5_ACTIVITY_KCAL_BIN);
    compo_picturebox_set_pos(pic,114/2+58,20+189/2+96-TITLE_BAR_HIGH);
    compo_picturebox_cut(pic,pic_dis,11);
    compo_setid(pic,KCAL_ARC_ID);

    pic = compo_picturebox_create(frm,UI_BUF_I338001_5_ACTIVITY_STEPS_BIN);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,20+115/2+249-TITLE_BAR_HIGH);
    compo_picturebox_cut(pic,pic_dis,11);
    compo_setid(pic,STEP_ARC_ID);

    pic = compo_picturebox_create(frm,UI_BUF_I338001_5_ACTIVITY_DIS_BIN);
    compo_picturebox_set_pos(pic,114/2+188,20+189/2+96-TITLE_BAR_HIGH);
    compo_picturebox_cut(pic,pic_dis,11);
    compo_setid(pic,KM_ARC_ID);

///////////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_I338001_5_ACTIVITY_ICON_KCAL_BIN);
    compo_picturebox_set_pos(pic, 46/2+48, 50/2+429);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,118,410,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_CALORIE]);

    /*千卡数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,118, 443);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,118+5+txt_leng.wid,449,88,25);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
///////////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_I338001_5_ACTIVITY_ICON_DIS_BIN);
    compo_picturebox_set_pos(pic, 46/2+48, 50/2+429+TXT_SPACING);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DISTANCE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,118,410+TXT_SPACING,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_DISTANCE]);

    /*公里数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d.%02d",km_integer, distance);///公里数据
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,118, 443+TXT_SPACING);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,118+5+txt_leng.wid,449+TXT_SPACING,88,25);
    compo_textbox_set(textbox, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
    compo_setid(textbox,KM_TXT_UNIT_ID);

///////////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_I338001_5_ACTIVITY_ICON_STEP_BIN);
    compo_picturebox_set_pos(pic, 46/2+48, 50/2+429+TXT_SPACING*2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SET_STEP_COUNT]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,118,410+TXT_SPACING*2,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_SET_STEP_COUNT]);

    /*步数数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,118, 443+TXT_SPACING*2);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,118+5+txt_leng.wid,449+TXT_SPACING*2,88,25);
    compo_textbox_set(textbox, i18n[STR_STEP]);
///////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_I338001_5_ACTIVITY_DATE_BG_BIN);
    compo_picturebox_set_pos(pic, 298/2+31, 294/2+749);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP_DETAILS]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,49,769,150,30);
    compo_textbox_set_forecolor(textbox, make_color(0x12,0xa9,0x94));
    compo_textbox_set(textbox, i18n[STR_STEP_DETAILS]);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,826+4/2,GUI_SCREEN_CENTER_X,30);
    compo_textbox_set(textbox, txt_buf);

    uint32_t target_step = uteModuleSportGetStepsTargetCnt();
    // uint32_t target_step = 0;
    if(target_step == 0)target_step = 8000;

    // memset(txt_buf,0,sizeof(txt_buf));
    // snprintf((char *)txt_buf, sizeof(txt_buf),"/%ld",target_step);///步数数据目标
    // textbox = compo_textbox_create(frm, strlen(txt_buf));
    // compo_textbox_set_align_center(textbox, false);
    // compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,594,GUI_SCREEN_CENTER_X-13,25);
    // compo_textbox_set_forecolor(textbox, make_color(153,153,153));
    // compo_textbox_set(textbox, txt_buf);

    uint32_t step_date[24];

    uteModuleSportLoadTodayEveryHourStepHistoryData(step_date);
    compo_chartbox_t*chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,877+124/2,235,124);
    compo_chartbox_set_pixel(chart, 1);

    // printf("mb:target_step:%ld\n",target_step);
    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 5;   ///像素点
    for (int i=0; i<24; i++)
    {
        // printf("step:%ld\n",step_date[i]);
        // step_date[i] =8000;
        chart_info.x = i*chart_info.width + i*5;
        chart_info.height = step_date[i]*(123*1000/target_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(85,238,151));
    }

//////////////////////////////////////////////////////////////////////////////////
    uint32_t week_step_data;
    uint32_t week_step_date[7];
    uint32_t target_week_step = uteModuleSportLoadWeekDayStepHistoryData(week_step_date, &week_step_data);
    // target_week_step =18000;

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEEK_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,44,1077,150,30);
    compo_textbox_set_forecolor(textbox, make_color(0x12,0xa9,0x94));
    compo_textbox_set(textbox, i18n[STR_WEEK_STEP]);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",week_step_data);///week步数数据
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,1128+45/2,230,30);
    compo_textbox_set(textbox, txt_buf);

    pic = compo_picturebox_create(frm,uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID ? UI_BUF_I338001_5_ACTIVITY_WEEK_DATE_BG_ZH_BIN: UI_BUF_I338001_5_ACTIVITY_WEEK_DATE_BG_EN_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 1193+155/2);

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+36,1200+110/2,GUI_SCREEN_WIDTH,126);
    compo_chartbox_set_pixel(chart, 1);

    chart_info;
    chart_info.y = 0;
    chart_info.width = 16;   ///像素点
    for (int i=0; i<7; i++)
    {
        // week_step_date[i] =18000;
        chart_info.x = i*chart_info.width + i*29;
        chart_info.height = week_step_date[i]*(126*1000/target_week_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(85,238,151));
    }
#endif

    return frm;
}
static void func_activity_disp_handle(void)
{
#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT
    compo_arc_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
    compo_arc_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
    compo_arc_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
    compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
    compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
    compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);
    compo_textbox_t *textbox_step_day = compo_getobj_byid(STEP_DAY_TXT_VALUE_ID);
    compo_textbox_t *textbox_step_week = compo_getobj_byid(STEP_WEEK_TXT_VALUE_ID);
    compo_textbox_t *textbox_step_day_uint = compo_getobj_byid(STEP_DAY_TXT_UNIT_ID);
    compo_textbox_t *textbox_step_week_uint = compo_getobj_byid(STEP_WEEK_TXT_UNIT_ID);
    compo_chartbox_t*day_chart = compo_getobj_byid(STEP_DAY_CAHRT_VALUE_ID);
    compo_chartbox_t*week_chart = compo_getobj_byid(STEP_WEEK_CAHRT_VALUE_ID);

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    uint32_t arc_step_value = 0;
    if(uteModuleSportGetStepsTargetCnt() != 0)
    {
        arc_step_value = (totalStepCnt*1000 / uteModuleSportGetStepsTargetCnt());
        if(arc_step_value>1000)arc_step_value=1000;
    }

    u8 km_integer  = distance/100;                //距离 整数
    u8 km_decimals = distance%100;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }
//////////////////////////////////////////////////////////////////////////
    compo_arc_set_value(arc_kcal,arc_step_value);
    compo_arc_set_value(arc_km,arc_step_value);
    compo_arc_set_value(arc_step,arc_step_value);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
    compo_textbox_set(textbox_step, txt_buf);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d.%02d",km_integer, distance);///公里数据
    compo_textbox_set(textbox_km, txt_buf);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    compo_textbox_set(textbox_kcal, txt_buf);
////////////////////////////////////////////////////////////////
    uint32_t target_step = uteModuleSportGetStepsTargetCnt();
    if(target_step == 0)target_step = 8000;
    uint32_t step_date[24];
    uteModuleSportLoadTodayEveryHourStepHistoryData(step_date);
    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*7;
        chart_info.height = step_date[i]*(104*1000/target_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(day_chart, i, chart_info, make_color(0,242,214));
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///今日步数数据
    compo_textbox_set(textbox_step_day, txt_buf);
//////////////////////////////////////////////////////////////////////
    uint32_t week_step_data;
    uint32_t week_step_date[7];
    uint32_t target_week_step = uteModuleSportLoadWeekDayStepHistoryData(week_step_date, &week_step_data);
    chart_info;
    chart_info.y = 0;
    chart_info.width = 20;   ///像素点
    for (int i=0; i<7; i++)
    {
        chart_info.x = i*chart_info.width + i*25;
        if(target_week_step != 0)
        {
            chart_info.height = week_step_date[i]*(100*1000/target_week_step)/1000;///心率数据转换为柱形条显示数据
        }
        else
        {
            chart_info.height = 0;///心率数据转换为柱形条显示数据
        }
        compo_chartbox_set_value(week_chart, i, chart_info,  make_color(0,242,214));
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",week_step_data);///week步数数据
    compo_textbox_set(textbox_step_week, txt_buf);

#else
    compo_picturebox_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
    compo_picturebox_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
    compo_picturebox_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
    compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
    compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
    compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);
    compo_textbox_t *textbox_km_unit = compo_getobj_byid(KM_TXT_UNIT_ID);
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    u8 pic_dis =(totalStepCnt / uteModuleSportGetStepsTargetCnt());
    if(pic_dis>10)pic_dis=10;
    u8 km_integer  = distance/100;                //距离 整数
    u8 km_decimals = distance%100;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }

    compo_picturebox_cut(arc_kcal,pic_dis,11);
    compo_picturebox_cut(arc_km,pic_dis,11);
    compo_picturebox_cut(arc_step,pic_dis,11);

    if(f_activity->uint_km != uteModuleSystemtimeGetDistanceMiType())
    {
        compo_textbox_set(textbox_km_unit, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
    }

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
    compo_textbox_set(textbox_step, txt_buf);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d.%02d",km_integer, distance);///公里数据
    compo_textbox_set(textbox_km, txt_buf);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    compo_textbox_set(textbox_kcal, txt_buf);
#endif
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#define KCAL_ARC_COLOR                61861
#define KM_ARC_COLOR                  65188
#define STEP_ARC_COLOR                1946

#define TXT_SPACING_Y                 widget_text_get_height()+8
///创建活动记录窗体
compo_form_t *func_activity_form_create(void)
{
    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I332001_ACTIVITY_BG_BIN);
    ///创建圆弧
    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X, 114, pic_bg_area.wid, pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_BG_BIN);
    compo_picturebox_set_pos(arc_pic, pic_bg_area.wid/2, pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_CALORIES_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KCAL_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_DISTANCE_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KM_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_STEPS_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,STEP_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I332001_ACTIVITY_YUANJIAO_BIN).hei/2);

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_textbox_t *textbox;

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,218);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KCAL]));///千卡
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,218+47,80,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,KCAL_ARC_COLOR);
    compo_textbox_set(textbox, i18n[STR_KCAL]);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",distance/100, distance%100);///公里数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,218);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_MILE])+strlen(i18n[STR_KM]));//英里
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,218+47,80,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,KM_ARC_COLOR);
    compo_setid(textbox,KM_TXT_UNIT_ID);
    if(uteModuleSystemtimeGetDistanceMiType())
    {
        compo_textbox_set(textbox, i18n[STR_MILE]);
    }
    else
    {
        compo_textbox_set(textbox, i18n[STR_KM]);
    }

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,279);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));///步数
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,279+47,80,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,STEP_ARC_COLOR);
    compo_textbox_set(textbox, i18n[STR_STEP]);

    return frm;
}


//活动记录功能事件处理
static void func_activity_disp_handle(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    if(tick_check_expire(f_activity->tick, 5))
    {
        f_activity->tick = tick_get();
        char txt_buf[20];
        uint32_t totalStepCnt = 0;
        uint32_t Step_value = 0;
        uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
        Step_value = totalStepCnt;
        if(Step_value>uteModuleSportGetStepsTargetCnt())
        {
            Step_value = uteModuleSportGetStepsTargetCnt();
        }

        compo_picturebox_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
        compo_picturebox_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
        compo_picturebox_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
        compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
        compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
        compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);
        compo_textbox_t *textbox_km_unit = compo_getobj_byid(KM_TXT_UNIT_ID);

        // if(f_activity->uint_km != uteModuleSystemtimeGetDistanceMiType())
        {
            compo_textbox_set(textbox_km_unit, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KM]);
        }

        if(f_activity->activity_state == 0)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);

            f_activity->arc_step_value+=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value >=1800)
            {
                f_activity->activity_state = 1;
            }
        }
        else if(f_activity->activity_state == 1)
        {

            f_activity->arc_step_value-=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value <= Step_value*1800 /uteModuleSportGetStepsTargetCnt())
            {
                f_activity->activity_state = 2;
            }

        }
        else
        {
            f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
            f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
            f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

            if(f_activity->arc_step_value)
            {
                compo_picturebox_set_visible(arc_kcal,true);
                compo_picturebox_set_visible(arc_km,true);
                compo_picturebox_set_visible(arc_step,true);
            }
            else
            {
                compo_picturebox_set_visible(arc_kcal,false);
                compo_picturebox_set_visible(arc_km,false);
                compo_picturebox_set_visible(arc_step,false);
            }


            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
            compo_textbox_set(textbox_step, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
            compo_textbox_set(textbox_km, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
            compo_textbox_set(textbox_kcal, txt_buf);
        }

    }
}

#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define KCAL_ARC_COLOR                61861
#define KM_ARC_COLOR                  65188
#define STEP_ARC_COLOR                1946

#define TXT_SPACING_Y                 widget_text_get_height()+8

///创建活动记录窗体
compo_form_t *func_activity_form_create(void)
{
    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_EVREY_DAY_ACTIVITY]);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I330001_ACTIVITY_BG_BIN);
    ///创建圆弧
    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/4.5, pic_bg_area.wid, pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_BG_BIN);
    compo_picturebox_set_pos(arc_pic, pic_bg_area.wid/2, pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_CALORIES_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KCAL_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_DISTANCE_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KM_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_STEPS_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,STEP_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I330001_ACTIVITY_YUANJIAO_BIN).hei/2);

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_textbox_t *textbox;

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KCAL]));///千卡
    compo_textbox_set(textbox, i18n[STR_KCAL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,KCAL_ARC_COLOR);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",distance/100, distance%100);///公里数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    if(uteModuleSystemtimeGetDistanceMiType())
    {
        textbox = compo_textbox_create(frm,strlen(i18n[STR_MILE]));//英里
        compo_textbox_set(textbox, i18n[STR_MILE]);
    }
    else
    {
        textbox = compo_textbox_create(frm,strlen(i18n[STR_KM]));//公里
        compo_textbox_set(textbox, i18n[STR_KM]);
    }
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,KM_ARC_COLOR);
    compo_setid(textbox,KM_TXT_UNIT_ID);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/1.28);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));///步数
    compo_textbox_set(textbox, i18n[STR_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/1.28+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,STEP_ARC_COLOR);

    return frm;
}


//活动记录功能事件处理
static void func_activity_disp_handle(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    if(tick_check_expire(f_activity->tick, 5))
    {
        f_activity->tick = tick_get();
        char txt_buf[20];
        uint32_t totalStepCnt = 0;
        uint32_t Step_value = 0;
        uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
        Step_value = totalStepCnt;
        if(Step_value>uteModuleSportGetStepsTargetCnt())
        {
            Step_value = uteModuleSportGetStepsTargetCnt();
        }

        compo_picturebox_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
        compo_picturebox_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
        compo_picturebox_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
        compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
        compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
        compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);
        compo_textbox_t *textbox_km_unit = compo_getobj_byid(KM_TXT_UNIT_ID);

        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            compo_textbox_set(textbox_km_unit, i18n[STR_MILE]);
        }
        else
        {
            compo_textbox_set(textbox_km_unit, i18n[STR_KM]);
        }

        if(f_activity->activity_state == 0)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);

            f_activity->arc_step_value+=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value >=1800)
            {
                f_activity->activity_state = 1;
            }
        }
        else if(f_activity->activity_state == 1)
        {

            f_activity->arc_step_value-=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value <= Step_value*1800 /uteModuleSportGetStepsTargetCnt())
            {
                f_activity->activity_state = 2;
            }

        }
        else
        {
            f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
            f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
            f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

            if(f_activity->arc_step_value)
            {
                compo_picturebox_set_visible(arc_kcal,true);
                compo_picturebox_set_visible(arc_km,true);
                compo_picturebox_set_visible(arc_step,true);
            }
            else
            {
                compo_picturebox_set_visible(arc_kcal,false);
                compo_picturebox_set_visible(arc_km,false);
                compo_picturebox_set_visible(arc_step,false);
            }


            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
            compo_textbox_set(textbox_step, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
            compo_textbox_set(textbox_km, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
            compo_textbox_set(textbox_kcal, txt_buf);
        }

    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#define KCAL_ARC_COLOR                61861
#define KM_ARC_COLOR                  65188
#define STEP_ARC_COLOR                1946
#define TXT_SPACING_Y                 widget_text_get_height()+8
///创建活动记录窗体
compo_form_t *func_activity_form_create(void)
{
    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I340001_ACTIVITY_BG_BIN);
    ///创建圆弧
    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X, 114, pic_bg_area.wid, pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_ACTIVITY_BG_BIN);
    compo_picturebox_set_pos(arc_pic, pic_bg_area.wid/2, pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_ACTIVITY_CALORIES_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KCAL_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_ACTIVITY_DISTANCE_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,KM_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_ACTIVITY_STEPS_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_setid(arc_pic,STEP_ARC_ID);
    compo_picturebox_set_visible(arc_pic,false);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I340001_ACTIVITY_YUANJIAO_BIN).hei/2);

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_textbox_t *textbox;

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,218);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KCAL]));///千卡
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,218+47,80,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,KCAL_ARC_COLOR);
    compo_textbox_set(textbox, i18n[STR_KCAL]);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",distance/100, distance%100);///公里数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,218);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_MILE])+strlen(i18n[STR_KM]));//英里
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,218+47,80,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,KM_ARC_COLOR);
    compo_setid(textbox,KM_TXT_UNIT_ID);
    if(uteModuleSystemtimeGetDistanceMiType())
    {
        compo_textbox_set(textbox, i18n[STR_MILE]);
    }
    else
    {
        compo_textbox_set(textbox, i18n[STR_KM]);
    }

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,279);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));///步数
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,279+47,80,widget_text_get_max_height());
    compo_textbox_set_forecolor(textbox,STEP_ARC_COLOR);
    compo_textbox_set(textbox, i18n[STR_STEP]);

    return frm;
}
//活动记录功能事件处理
static void func_activity_disp_handle(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    if(tick_check_expire(f_activity->tick, 5))
    {
        f_activity->tick = tick_get();
        char txt_buf[20];
        uint32_t totalStepCnt = 0;
        uint32_t Step_value = 0;
        uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
        Step_value = totalStepCnt;
        if(Step_value>uteModuleSportGetStepsTargetCnt())
        {
            Step_value = uteModuleSportGetStepsTargetCnt();
        }

        compo_picturebox_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
        compo_picturebox_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
        compo_picturebox_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
        compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
        compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
        compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);
        compo_textbox_t *textbox_km_unit = compo_getobj_byid(KM_TXT_UNIT_ID);

        if(f_activity->uint_km != uteModuleSystemtimeGetDistanceMiType())
        {
            compo_textbox_set(textbox_km_unit, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KM]);
        }

        if(f_activity->activity_state == 0)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);

            f_activity->arc_step_value+=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value >=1800)
            {
                f_activity->activity_state = 1;
            }
        }
        else if(f_activity->activity_state == 1)
        {

            f_activity->arc_step_value-=18;

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

            if(f_activity->arc_step_value <= Step_value*1800 /uteModuleSportGetStepsTargetCnt())
            {
                f_activity->activity_state = 2;
            }

        }
        else
        {
            f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
            f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
            f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

            compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
            compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

            if(f_activity->arc_step_value)
            {
                compo_picturebox_set_visible(arc_kcal,true);
                compo_picturebox_set_visible(arc_km,true);
                compo_picturebox_set_visible(arc_step,true);
            }
            else
            {
                compo_picturebox_set_visible(arc_kcal,false);
                compo_picturebox_set_visible(arc_km,false);
                compo_picturebox_set_visible(arc_step,false);
            }


            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
            compo_textbox_set(textbox_step, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
            compo_textbox_set(textbox_km, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
            compo_textbox_set(textbox_kcal, txt_buf);
        }

    }
}
#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

compo_form_t *func_activity_form_create(void)
{
    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    uint32_t arc_step = 0;
    if(uteModuleSportGetStepsTargetCnt() != 0)
    {
        arc_step = (totalStepCnt*1000 / uteModuleSportGetStepsTargetCnt());
        if(arc_step>1000)arc_step=1000;
    }


    widget_page_t *page = widget_page_create(frm->page_body);///创建按键页面
    widget_set_location(page,GUI_SCREEN_WIDTH/2,96/2+24,GUI_SCREEN_WIDTH,212/2+20/2);

    compo_arc_t *arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,212/2,192,192);
    compo_arc_set_width(arc,20);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(76,16,12),0);
    compo_arc_set_value(arc,1000);
    compo_arc_set_edge_circle(arc,true,true);

    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,212/2,144,144);
    compo_arc_set_width(arc,20);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(76,64,0),0);
    compo_arc_set_value(arc,1000);
    compo_arc_set_edge_circle(arc,true,true);

    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,212/2,96,96);
    compo_arc_set_width(arc,20);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(0,73,64),0);
    compo_arc_set_value(arc,1000);
    compo_arc_set_edge_circle(arc,true,true);
///////////////////////////////////////////////////////////
    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,212/2,192,192);
    compo_arc_set_width(arc,20);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(252,55,40),make_color(76,16,12));
    compo_arc_set_value(arc,arc_step);
    compo_arc_set_edge_circle(arc,true,true);
    compo_setid(arc,KCAL_ARC_ID);

    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,212/2,144,144);
    compo_arc_set_width(arc,20);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(255,212,0),make_color(76,64,0));
    compo_arc_set_value(arc,arc_step);
    compo_arc_set_edge_circle(arc,true,true);
    compo_setid(arc,KM_ARC_ID);

    arc = compo_arc_create_page(frm,page);
    compo_arc_set_alpha(arc,255,0);
    compo_arc_set_location(arc,GUI_SCREEN_CENTER_X,212/2,96,96);
    compo_arc_set_width(arc,20);
    compo_arc_set_rotation(arc,2700);
    compo_arc_set_angles(arc,0,1800);
    compo_arc_set_color(arc,make_color(0,242,214),make_color(0,73,64));
    compo_arc_set_value(arc,arc_step);
    compo_arc_set_edge_circle(arc,true,true);
    compo_setid(arc,STEP_ARC_ID);
////////////////////////////////////////////////////////////////////////////////
    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();

    compo_textbox_t *textbox;
    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,144);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I342001_5_ACTIVITY_ICON_KCAL_BIN);
    compo_picturebox_set_size(picbox,32,32);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,144+25);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",distance/100, distance%100);///公里数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,144);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_5_ACTIVITY_ICON_DIS_BIN);
    compo_picturebox_set_size(picbox,32,32);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,144+25);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,178);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_5_ACTIVITY_ICON_STEPS_BIN);
    compo_picturebox_set_size(picbox,32,32);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,178+25);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_5_ACTIVITY_NEXT_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,218+16/2);
/////////////////////////////////////////////////////////////////////////////////////////////////
    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP_DETAILS]));
    compo_textbox_set(textbox, i18n[STR_STEP_DETAILS]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+42);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_5_ACTIVITY_NO_DATA_BG_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+72+96/2);

    /*步数数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+186+30/2);
    compo_setid(textbox,STEP_DAY_TXT_VALUE_ID);

    area_t txt_leng= widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));
    compo_textbox_set(textbox, i18n[STR_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+txt_leng.wid/2+16,GUI_SCREEN_HEIGHT+186+30/2);
    compo_textbox_set_forecolor(textbox,make_color(128,128,128));
//    compo_setid(textbox,STEP_DAY_TXT_UNIT_ID);

    uint32_t target_step = uteModuleSportGetStepsTargetCnt();
    if(target_step == 0)target_step = 8000;
    uint32_t step_date[24];
    uteModuleSportLoadTodayEveryHourStepHistoryData(step_date);
    compo_chartbox_t*chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT+72+96/2-10,206,80);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(textbox,STEP_DAY_CAHRT_VALUE_ID);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<24; i++)
    {
//         printf("step:%ld\n",step_date[i]);
//         step_date[i] =8000;
        chart_info.x = i*chart_info.width + i*5;
        chart_info.height = step_date[i]*(104*1000/target_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0,242,214));
    }

//////////////////////////////////////////////////////////////////////////////////////////////////
    uint32_t week_step_data;
    uint32_t week_step_date[7];
    uint32_t target_week_step = uteModuleSportLoadWeekDayStepHistoryData(week_step_date, &week_step_data);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_WEEK_STEP]));
    compo_textbox_set(textbox, i18n[STR_WEEK_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT*2+42);

    picbox = compo_picturebox_create(frm, uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID ? UI_BUF_I342001_5_ACTIVITY_WEEK_DATE_BG_ZH_BIN: UI_BUF_I342001_5_ACTIVITY_WEEK_DATE_BG_EN_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT*2+72+96/2);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",week_step_data);///week步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT*2+186+30/2);
    compo_setid(textbox,STEP_WEEK_TXT_VALUE_ID);

    txt_leng= widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEP]));
    compo_textbox_set(textbox, i18n[STR_STEP]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+txt_leng.wid/2+16,GUI_SCREEN_HEIGHT*2+186+30/2);
    compo_textbox_set_forecolor(textbox,make_color(128,128,128));
//    compo_setid(textbox,STEP_WEEK_TXT_UNIT_ID);

    chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 7);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+5,GUI_SCREEN_HEIGHT*2+72+96/2-10,206,78);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(textbox,STEP_WEEK_CAHRT_VALUE_ID);

    chart_info;
    chart_info.y = 0;
    chart_info.width = 14;   ///像素点
    for (int i=0; i<7; i++)
    {
//         week_step_date[i] =18000;
//         target_week_step = 18000;
        chart_info.x = i*chart_info.width + i*16;
        if(target_week_step != 0)
        {
            chart_info.height = week_step_date[i]*(100*1000/target_week_step)/1000;///心率数据转换为柱形条显示数据
        }
        else
        {
            chart_info.height = 0;///心率数据转换为柱形条显示数据
        }
        compo_chartbox_set_value(chart, i, chart_info,  make_color(0,242,214));
    }


    return frm;
}
static void func_activity_disp_handle(void)
{
    compo_arc_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
    compo_arc_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
    compo_arc_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
    compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
    compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
    compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);
    compo_textbox_t *textbox_step_day = compo_getobj_byid(STEP_DAY_TXT_VALUE_ID);
    compo_textbox_t *textbox_step_week = compo_getobj_byid(STEP_WEEK_TXT_VALUE_ID);
//    compo_textbox_t *textbox_step_day_uint = compo_getobj_byid(STEP_DAY_TXT_UNIT_ID);
//    compo_textbox_t *textbox_step_week_uint = compo_getobj_byid(STEP_WEEK_TXT_UNIT_ID);
    compo_chartbox_t*day_chart = compo_getobj_byid(STEP_DAY_CAHRT_VALUE_ID);
    compo_chartbox_t*week_chart = compo_getobj_byid(STEP_WEEK_CAHRT_VALUE_ID);

    char txt_buf[20];
    uint16_t distance = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    uint32_t arc_step_value = 0;
    if(uteModuleSportGetStepsTargetCnt() != 0)
    {
        arc_step_value = (totalStepCnt*1000 / uteModuleSportGetStepsTargetCnt());
        if(arc_step_value>1000)arc_step_value=1000;
    }

    u8 km_integer  = distance/100;                //距离 整数
    u8 km_decimals = distance%100;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }
//////////////////////////////////////////////////////////////////////////
    compo_arc_set_value(arc_kcal,arc_step_value);
    compo_arc_set_value(arc_km,arc_step_value);
    compo_arc_set_value(arc_step,arc_step_value);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
    compo_textbox_set(textbox_step, txt_buf);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d.%02d",km_integer, distance);///公里数据
    compo_textbox_set(textbox_km, txt_buf);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    compo_textbox_set(textbox_kcal, txt_buf);
////////////////////////////////////////////////////////////////
    uint32_t target_step = uteModuleSportGetStepsTargetCnt();
    if(target_step == 0)target_step = 8000;
    uint32_t step_date[24];
    uteModuleSportLoadTodayEveryHourStepHistoryData(step_date);
    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*5;
        chart_info.height = step_date[i]*(104*1000/target_step)/1000;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(day_chart, i, chart_info, make_color(0,242,214));
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///今日步数数据
    compo_textbox_set(textbox_step_day, txt_buf);
//////////////////////////////////////////////////////////////////////
    uint32_t week_step_data;
    uint32_t week_step_date[7];
    uint32_t target_week_step = uteModuleSportLoadWeekDayStepHistoryData(week_step_date, &week_step_data);
    chart_info;
    chart_info.y = 0;
    chart_info.width = 14;   ///像素点
    for (int i=0; i<7; i++)
    {
        chart_info.x = i*chart_info.width + i*16;
        if(target_week_step != 0)
        {
            chart_info.height = week_step_date[i]*(100*1000/target_week_step)/1000;///心率数据转换为柱形条显示数据
        }
        else
        {
            chart_info.height = 0;///心率数据转换为柱形条显示数据
        }
        compo_chartbox_set_value(week_chart, i, chart_info,  make_color(0,242,214));
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",week_step_data);///week步数数据
    compo_textbox_set(textbox_step_week, txt_buf);
}

#else
compo_form_t *func_activity_form_create(void)
{
}
static void func_activity_disp_handle(void)
{
}
#endif

// 显示活动记录界面刷新
static void func_activity_process(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;
    if (f_activity->ptm)
    {
        compo_page_move_process(f_activity->ptm);
    }
    func_activity_disp_handle();
    func_process();
}
#include "func_cover.h"
//活动记录功能消息处理
static void func_activity_message(size_msg_t msg)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if (f_activity->ptm)
            {
                compo_page_move_touch_handler(f_activity->ptm);
            }
            break;
        case MSG_CTP_CLICK:
            // uteModuleCallChangeEntertranmentVoiceSwitchStatus();
            // func_cb.sta = FUNC_HEART_WARNING;
            // sys_cb.cover_index = REMIND_COVER_LOW_BATTERY;
            // msgbox(NULL, NULL, NULL, NULL, MSGBOX_MSG_TYPE_REMIND_COVER);
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入活动记录控制功能
static void func_activity_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_activity_t));
    func_cb.frm_main = func_activity_form_create();
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;
    f_activity->activity_state = 0;

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
    f_activity->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 990,
        .page_count = 1,
        .quick_jump_perc = 40,
    };
    compo_page_move_init(f_activity->ptm, func_cb.frm_main->page_body, &info);
    f_activity->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    f_activity->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 3,
        .quick_jump_perc = GUI_SCREEN_HEIGHT,
        .jump_perc = 20,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_activity->ptm, func_cb.frm_main->page_body, &info);
    f_activity->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
#else
    f_activity->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 1420,
        .page_count = 1,
        .quick_jump_perc = 40,
    };
    compo_page_move_init(f_activity->ptm, func_cb.frm_main->page_body, &info);
    f_activity->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
#endif
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    f_activity->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
#endif
}

//退出活动记录功能
static void func_activity_exit(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;
    func_cb.last = FUNC_ACTIVITY;
    if (f_activity->ptm)
    {
        func_free(f_activity->ptm);
    }
}

//活动记录功能
void func_activity(void)
{
    printf("%s\n", __func__);
    func_activity_enter();
    while (func_cb.sta == FUNC_ACTIVITY)
    {
        func_activity_process();
        func_activity_message(msg_dequeue());
    }
    func_activity_exit();
}
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
