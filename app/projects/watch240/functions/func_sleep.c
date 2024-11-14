#include "include.h"
#include "func.h"
#include "ute_module_sleep.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define SIDEBAR_PAGE_HEIGHT GUI_SCREEN_HEIGHT*2.8 //长图总高度

#define FINT_HEIGHT 24

#define FUNC_SLEEP_SIMULATE_DATA_EN     1                           //假数据
#define TODAY_SLEEP_TIME_TXT_MAX        2//16

enum
{
    COMPO_ID_TEXT_DEEP_TIME_HOUR = 1,
    COMPO_ID_TEXT_DEEP_TIME_MIN,
    COMPO_ID_TEXT_LIGHT_TIME_HOUR,
    COMPO_ID_TEXT_LIGHT_TIME_MIN,

    COMPO_ID_CHART_SHALLOW,
    COMPO_ID_CHART_DEEP,
};

enum
{
    CHART_ANIM_STA_IDLE,
    CHART_ANIM_STA_START,
    CHART_ANIM_STA_END,

};

typedef struct f_sleep_t_
{
    page_tp_move_t *ptm;

    u8 anim_sta;
    u16 data[7];
} f_sleep_t;


#if FUNC_SLEEP_SIMULATE_DATA_EN
//static const uint16_t total_sleep_time[] = {8*60+30, 9*60+40, 7*60+20, 10*60+30, 5*60+10, 0, 8*60+30}; //min
#endif

//创建睡眠窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sleep_form_create(void)
{
    compo_textbox_t * txt;
    compo_picturebox_t *pic;
    char buf[50];

    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_title(frm, i18n[STR_SLEEP]);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);

    pic = compo_picturebox_create(frm, UI_BUF_SLEEP_SLEEP_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y*0.6);
    compo_picturebox_set_size(pic, 70, 70);

    snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/3- GUI_SCREEN_CENTER_X/10, GUI_SCREEN_CENTER_Y);
    compo_textbox_set(txt, buf);
//    compo_textbox_set_forecolor(txt, make_color(128, 0, 128));

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));
    //compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/3+GUI_SCREEN_CENTER_X/4- GUI_SCREEN_CENTER_X/10, GUI_SCREEN_CENTER_Y);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/3+GUI_SCREEN_CENTER_X/4- GUI_SCREEN_CENTER_X/10, GUI_SCREEN_CENTER_Y,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_HOUR]);

//    compo_textbox_set_forecolor(txt, make_color(128, 0, 128));

    snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3- GUI_SCREEN_CENTER_X/10, GUI_SCREEN_CENTER_Y);
    compo_textbox_set(txt, buf);
//    compo_textbox_set_forecolor(txt, make_color(128, 0, 128));

    txt = compo_textbox_create(frm,strlen(i18n[STR_MIN]));
//    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3+GUI_SCREEN_CENTER_X/4 - GUI_SCREEN_CENTER_X/10, GUI_SCREEN_CENTER_Y);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3+GUI_SCREEN_CENTER_X/4 - GUI_SCREEN_CENTER_X/10, GUI_SCREEN_CENTER_Y,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_HOUR]);
    compo_textbox_set(txt, i18n[STR_MIN]);
//    compo_textbox_set_forecolor(txt, make_color(128, 0, 128));


    snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->fallAsSleepTime.hour,sleep_data->fallAsSleepTime.min);///* 睡眠点*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2);
    compo_textbox_set(txt, buf);
//    compo_textbox_set_forecolor(txt, make_color(128, 0, 128));

    snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->getUpSleepTime.hour,sleep_data->getUpSleepTime.min);///* 起床点*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2);
    compo_textbox_set(txt, buf);
//    compo_textbox_set_forecolor(txt, make_color(128, 0, 128));

    pic = compo_picturebox_create(frm, UI_BUF_SLEEP_SLEEP_UP_BIN);///* 睡眠图标*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.3);
//    compo_picturebox_set_size(pic, 35, 35);

    pic = compo_picturebox_create(frm, UI_BUF_SLEEP_SLEEP_DOWN_BIN);///* 起床图标*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.3);
//    compo_picturebox_set_size(pic, 35, 35);


//    //睡眠日总时长详情
//    compo_form_add_image(frm, UI_BUF_SLEEP_CHART_BG_BIN, 120, 460);
#if (!FUNC_SLEEP_SIMULATE_DATA_EN)
    uint16_t total_sleep_time[2] = {0};
#endif
    ///* 第二页*/
    area_t pic_size = gui_image_get_size(UI_BUF_SLEEP_SLEEP_DATE_BG_BIN);

    compo_form_add_image(frm, UI_BUF_SLEEP_SLEEP_DATE_BG_BIN, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4);///* 背景图*/

    snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->fallAsSleepTime.hour,sleep_data->fallAsSleepTime.min);///* 睡眠初始点*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-pic_size.wid/2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+pic_size.hei/2-FINT_HEIGHT);
    compo_textbox_set(txt, buf);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);

    snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->getUpSleepTime.hour,sleep_data->getUpSleepTime.min);///* 睡眠结束点*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X+pic_size.wid/2-FINT_HEIGHT/2*strlen(buf), GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+pic_size.hei/2-FINT_HEIGHT);
    compo_textbox_set(txt, buf);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);

    u16 length,y1,y2;

    pic = compo_picturebox_create(frm, UI_BUF_SLEEP_SLEEP_STATE1_BIN);///* 深度睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5+GUI_SCREEN_HEIGHT/3);
    y1 = compo_picturebox_get_location(pic).y;

    pic = compo_picturebox_create(frm, UI_BUF_SLEEP_SLEEP_STATE2_BIN);///* 浅度睡眠图*/
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5+GUI_SCREEN_HEIGHT/3*2);
    y2 = compo_picturebox_get_location(pic).y;
    length = y2 - y1;

    pic = compo_picturebox_create(frm, UI_BUF_SLEEP_SLEEP_STATE3_BIN);///* 清醒睡眠图*/
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5+GUI_SCREEN_HEIGHT/3*3);

    pic = compo_picturebox_create(frm, UI_BUF_SLEEP_SLEEP_STATE4_BIN);///* 快速眼动睡眠图*/
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5+GUI_SCREEN_HEIGHT/3*4);

    txt = compo_textbox_create(frm,strlen(i18n[STR_DEEP_SLEEP]));///* 深睡*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+GUI_SCREEN_HEIGHT/3);
    compo_textbox_set(txt, i18n[STR_DEEP_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x78, 0x35, 0xf7));

    txt = compo_textbox_create(frm,strlen(i18n[STR_LIGHT_SLEEP]));///* 浅睡*
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+GUI_SCREEN_HEIGHT/3*2);
    compo_textbox_set(txt, i18n[STR_LIGHT_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0xad, 0x86, 0xfc));

    txt = compo_textbox_create(frm,strlen(i18n[STR_ALWAKE_SLEEP]));///* 清醒*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+GUI_SCREEN_HEIGHT/3*3);
    compo_textbox_set(txt, i18n[STR_ALWAKE_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0xff, 0x87, 0x0f));

    txt = compo_textbox_create(frm,strlen(i18n[STR_EYE_MOVE_SLEEP]));///* 快速眼动*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+GUI_SCREEN_HEIGHT/3*4);
    compo_textbox_set(txt, i18n[STR_EYE_MOVE_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x00, 0xf7, 0xd6));
//////////////////////////////////////////////////////////////////////////////////
    snprintf(buf, sizeof(buf), "%02d", sleep_data->deepSleepMin/60);///* 深睡小时数据*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.7);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));///* 小时*/
//    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_HOUR]);

    snprintf(buf, sizeof(buf), "%02d", sleep_data->deepSleepMin%60);///* 深睡分钟数据*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*2.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.7);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_MIN]));///* 分钟*/
//    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*4, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*4, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_MIN]);
//////////////////////////////////////////////////////////////////////////////////////////////////////

    snprintf(buf, sizeof(buf), "%02d", sleep_data->lightSleepMin/60);///* 浅睡小时数据*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.7+length);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));///* 小时*/
//    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_HOUR]);

    snprintf(buf, sizeof(buf), "%02d", sleep_data->lightSleepMin%60);///* 浅睡分钟数据*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*2.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.7+length);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_MIN]));///* 分钟*/
//    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*4, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*4, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_MIN]);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    snprintf(buf, sizeof(buf), "%02d", sleep_data->wakeSleepMin/60);///* 清醒小时数据*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.7+length*2);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));///* 小时*/
//    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length*2);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length*2,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_HOUR]);

    snprintf(buf, sizeof(buf), "%02d", sleep_data->wakeSleepMin%60);///* 清醒分钟数据*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*2.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.7+length*2);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_MIN]));///* 分钟*/
//    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*4, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length*2);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*4, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length*2,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_MIN]);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin%60);///* 快速眼动小时数据*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.7+length*3);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));///* 小时*/
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*1.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length*3);
    compo_textbox_set(txt, i18n[STR_HOUR]);
    compo_textbox_set_align_center(txt, false);

    snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin%60);///* 快速眼动分钟数据*/
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*2.5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.7+length*3);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));///* 分钟*/
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.3+FINT_HEIGHT*4, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3+GUI_SCREEN_HEIGHT/2.5+length*3);
    compo_textbox_set(txt, i18n[STR_HOUR]);
    compo_textbox_set_align_center(txt, false);
////////////////////////////////////////////////////////////////////////////////////////////////////////////
//    compo_shape_t *shape;
//    u8 test_value[8] = {120,30,30,60,60,60,60,60};
//    u8 test_value_state[8] = {3,1,3,2,4,1,2,4};
//    u16 disp_test = 23600/(8*60);
//    s16 location_x;
//
//    for(int i=0;i<8;i++)
//    {
//       if(!i){
//            location_x = test_value[i]*disp_test/100/2;
//       }
//       else {
//            location_x += test_value[i]*disp_test/100/2+1;
//       }
//
//       shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
//
//       compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1-(21*(test_value_state[i]-1)),test_value[i]*disp_test/100, 16);
//
//       switch(test_value_state[i])
//       {
//        case 1:
//            compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1-21*2,test_value[i]*disp_test/100, 16);
//            compo_shape_set_color(shape, make_color(0x78, 0x35, 0xf7));
//            break;
//        case 2:
//            compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1-21,test_value[i]*disp_test/100, 16);
//            compo_shape_set_color(shape, make_color(0xad, 0x86, 0xfc));
//            break;
//        case 3:
//            compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1,test_value[i]*disp_test/100, 16);
//            compo_shape_set_color(shape, make_color(0xff, 0x87, 0x0f));
//            break;
//        case 4:
//            compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1+21,test_value[i]*disp_test/100, 16);
//            compo_shape_set_color(shape, make_color(0x00, 0xf7, 0xd6));
//            break;
//       }
//
//       if(!i){
//            location_x = compo_shape_get_location(shape).wid;
//       }
//       else{
//            location_x += compo_shape_get_location(shape).wid/2;
//       }
//
//    }
////////////////////////////////////////////////////////////////////////////////////
//    u8 test_value[8] = {60,60,60,60,60,60,60,60};
//    u8 test_value_state[8] = {0,1,0,2,1,0,2,1};
//    sleep_data->totalSleepMin=8;
//
//    for(int i=0;i<sleep_data->totalSleepMin;i++){
//
//        sleep_data->sleep_record[i].startTime.hour = test_value[i]/60;
//        sleep_data->sleep_record[i].startTime.min  = test_value[i]%60;
//        sleep_data->sleep_record[i].sleepFlag = test_value_state[i];
//    }
/////////////////////////////////////////////////////////////////////////////////////

    if(sleep_data->totalSleepMin)///是否有睡眠时长
    {
        compo_shape_t *shape;
        u16 disp_test = 23600/sleep_data->totalSleepMin;///放大图片像素/总睡眠分钟，获取每分钟像素精度
        s16 location_x;

        for(int i=0; i<sleep_data->recordCnt; i++)/// 总睡眠中睡眠状态次数 <-sleep_data->recordCn->
        {
            if(!i) ///设置第一个显示块的X坐标
            {
                location_x = (sleep_data->sleep_record[i].startTime.hour*60 + sleep_data->sleep_record[i].startTime.min)*disp_test/100/2;
            }
            else ///设置累加显示块的X坐标
            {
                location_x += (sleep_data->sleep_record[i].startTime.hour*60 + sleep_data->sleep_record[i].startTime.min)*disp_test/100/2+1;
            }

            shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///创建显示块矩形

            switch(sleep_data->sleep_record[i].sleepFlag)///设置不同睡眠状态显示块Y坐标及颜色
            {
                case DEEP_SLEEP:
                    compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1-21*2,///设置显示块X坐标与宽度
                                             (sleep_data->sleep_record[i].startTime.hour*60 + sleep_data->sleep_record[i].startTime.min)*disp_test/100, 16);
                    compo_shape_set_color(shape, make_color(0x78, 0x35, 0xf7));
                    break;
                case LIGHT_SLEEP:
                    compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1-21,///设置显示块X坐标与宽度
                                             (sleep_data->sleep_record[i].startTime.hour*60 + sleep_data->sleep_record[i].startTime.min)*disp_test/100, 16);
                    compo_shape_set_color(shape, make_color(0xad, 0x86, 0xfc));
                    break;
                case AWAKE_SLEEP:
                    compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1,///设置显示块X坐标与宽度
                                             (sleep_data->sleep_record[i].startTime.hour*60 + sleep_data->sleep_record[i].startTime.min)*disp_test/100, 16);
                    compo_shape_set_color(shape, make_color(0xff, 0x87, 0x0f));
                    break;
                case REM_SLEEP:
                    compo_shape_set_location(shape,location_x+3, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4.1+21,///设置显示块X坐标与宽度
                                             (sleep_data->sleep_record[i].startTime.hour*60 + sleep_data->sleep_record[i].startTime.min)*disp_test/100, 16);
                    compo_shape_set_color(shape, make_color(0x00, 0xf7, 0xd6));
                    break;
            }

            if(!i) ///设置第一个显示块的X坐标处理
            {
                location_x = compo_shape_get_location(shape).wid;
            }
            else ///设置累加显示块的X坐标处理
            {
                location_x += compo_shape_get_location(shape).wid/2;
            }

        }
    }

    ab_free(sleep_data);
    return frm;
}

//刷新睡眠数据
static void func_sleep_data_refresh(void)
{
#if (!FUNC_SLEEP_SIMULATE_DATA_EN)
    uint16_t total_sleep_time[2] = {0};
#endif

//    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;

}

//睡眠图表动画
//static void func_sleep_animation(void)
//{
//    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
//    if (f_sleep->anim_sta == CHART_ANIM_STA_END)
//    {
//        return;
//    }
//    compo_chartbox_t *chart = compo_getobj_byid(COMPO_ID_CHART_SHALLOW);
//    u8 max_num = compo_chartbox_get_max_num(chart);
//
//    chart_t chart_info;
//    chart_info.y = 0;
//    chart_info.width = 5;   //5 * pixel(2) = 10像素点
//
//    static u8 anim_cnt;
//    static u32 ticks;
//    if(tick_check_expire(ticks, 10))
//    {
//        ticks = tick_get();
//
//        switch (f_sleep->anim_sta)
//        {
//            case CHART_ANIM_STA_IDLE:
////                printf("sleep_get_offs:%d\n", compo_page_move_get_offset(f_sleep->ptm));
//                if (compo_page_move_get_offset(f_sleep->ptm) < -300)
//                {
//                    anim_cnt = 0;
//                    f_sleep->anim_sta = CHART_ANIM_STA_START;
//                }
//                break;
//
//            case CHART_ANIM_STA_START:
//                for (int i=0; i<max_num; i++)
//                {
//                    if (i == 0 && ++anim_cnt >= 255)            //图表数据最大时退出动画
//                    {
//                        f_sleep->anim_sta = CHART_ANIM_STA_END;
//                    }
//                    if (anim_cnt > f_sleep->data[i])
//                    {
//                        continue;
//                    }
//                    chart_info.x = i*17 + 1;
//                    chart_info.height = anim_cnt;
//                    compo_chartbox_set_value(chart, i, chart_info, COLOR_BLUE);
//                }
//                break;
//        }
//    }
//}

//睡眠功能消息处理
static void func_sleep_process(void)
{
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;

    compo_page_move_process(f_sleep->ptm);
    //func_sleep_animation();
    func_process();
}

static void func_sleep_message(size_msg_t msg)
{
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_sleep->ptm);
            break;

        case MSG_CTP_CLICK:
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_LONG:
            break;

        case MSG_QDEC_BACKWARD:
            if (func_cb.flag_sort)      //快捷应用状态下不滚动页面
            {
                func_message(msg);
            }
            else
            {
                compo_page_move_set_by_pages(f_sleep->ptm, -1);
            }
            break;

        case MSG_QDEC_FORWARD:
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            else
            {
                compo_page_move_set_by_pages(f_sleep->ptm, 1);
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入睡眠功能
static void func_sleep_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sleep_t));
    func_cb.frm_main = func_sleep_form_create();

    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
    f_sleep->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .quick_jump_perc = 30,
        .title_used = true,
        .page_size = SIDEBAR_PAGE_HEIGHT,
        .page_count = 1,
        .up_over_perc = 5,
        .down_over_perc = 5,
    };
    compo_page_move_init(f_sleep->ptm, func_cb.frm_main->page_body, &info);

    func_sleep_data_refresh();
}

//退出睡眠功能
static void func_sleep_exit(void)
{
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
    if (f_sleep->ptm)
    {
        func_free(f_sleep->ptm);
    }
}

//睡眠功能
void func_sleep(void)
{
    printf("%s\n", __func__);
    func_sleep_enter();
    while (func_cb.sta == FUNC_SLEEP)
    {
        func_sleep_process();
        func_sleep_message(msg_dequeue());
    }
    func_sleep_exit();
}
