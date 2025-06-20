#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_drv_motor.h"

#define TRACE_EN        0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
    COMPO_ID_KM_VALUE,
    COMPO_ID_KM_UINT,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;
} f_sport_finish_t;
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);


//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    u16 str_id = func_sport_get_str(sys_cb.sport_idx);
    u32 icon_addr = func_sport_get_ui(sys_cb.sport_idx);

    if (func_cb.sta == FUNC_SPORT_FINISH)
    {
        f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;

        ute_module_more_sports_data_t sport_data;
        uteModuleSportGetMoreSportsDatas(&sport_data);

        if(uteModuleSystemtime12HOn())
        {
            sport_data.saveData.startSportTime.hour %=12;
            if(sport_data.saveData.startSportTime.hour == 0)
            {
                sport_data.saveData.startSportTime.hour = 12;
            }
        }
        char txt_buf[50];
        memset(txt_buf,0,sizeof(txt_buf));
        switch(f_sport_finish->sport_finish_state)
        {
            case COMPO_WALK_STATE://户外跑步类别
            {
                //运动类型图片
                compo_picturebox_t *picbox = compo_picturebox_create(frm, icon_addr);
                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);

                //运动类型名称
                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[str_id]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+73);
                compo_textbox_set(textbox, i18n[str_id]);

                //时间日期
                snprintf(txt_buf,sizeof(txt_buf),"%04d/%02d/%02d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+105);
                compo_textbox_set(textbox, txt_buf);
                compo_textbox_set_forecolor(textbox, COLOR_GREEN);

                //运动数据图标
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_DIS_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_STEP_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*3);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*4);


                /*运动数据*/
                memset(txt_buf,0,sizeof(txt_buf));//运动时长
                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142);
                compo_textbox_set(textbox, txt_buf);

                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40);
                compo_textbox_set(textbox, txt_buf);
                area_t txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
                compo_textbox_set(textbox, i18n[STR_KCAL]);

                u8 km_integer=sport_data.saveData.sportDistanceInteger;                 //距离 整数
                u8 km_decimals=sport_data.saveData.sportDistanceDecimals;                     // 小数
                if(uteModuleSystemtimeGetDistanceMiType())//英里
                {
                    uint16_t distance = km_integer*1000+km_decimals*10;
                    distance = distance*0.6213712;
                    km_integer  = distance/1000;
                    km_decimals = distance%1000/10;
                }
                memset(txt_buf,0,sizeof(txt_buf)); //运动公里
                snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*2);
                compo_textbox_set(textbox, txt_buf);
                compo_setid(textbox,COMPO_ID_KM_VALUE);

                txt_leng = widget_text_get_area(textbox->txt);
                if(uteModuleSystemtimeGetDistanceMiType())//英里
                {
                    textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE]));
                    compo_textbox_set_align_center(textbox, false);
                    compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40);
                    compo_textbox_set(textbox, i18n[STR_MILE]);
                }
                else
                {
                    textbox = compo_textbox_create(frm, strlen(i18n[STR_KM]));
                    compo_textbox_set_align_center(textbox, false);
                    compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40);
                    compo_textbox_set(textbox, i18n[STR_KM]);
                }
                compo_setid(textbox,COMPO_ID_KM_UINT);

                memset(txt_buf,0,sizeof(txt_buf));//运动步数
                snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*3);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_location(textbox, 50+txt_leng.wid, 5+184+40*2,100,widget_text_get_max_height());
                compo_textbox_set(textbox, i18n[STR_STEP]);

                memset(txt_buf,0,sizeof(txt_buf));//运动心率
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*4);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*3);
                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
                /*运动数据*/

                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*5);
                compo_setid(btn,COMPO_BTN_SURE);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*5,GUI_SCREEN_CENTER_X,28);
                compo_textbox_set(textbox, i18n[STR_OK]);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
                compo_textbox_set_align_center(textbox, true);
                compo_textbox_set_multiline(textbox, true);
                // compo_textbox_set_multiline_drag(textbox,true);
                widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*7,GUI_SCREEN_CENTER_X*1.7,28*2);
                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
            }
            break;
            case COMPO_SWIMING_STATE://游泳 跳绳
            {
                //运动类型图片
//                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
                compo_picturebox_t *picbox = compo_picturebox_create(frm, icon_addr);
                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);

                //运动类型名称
//                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(func_sport_get_current_idx())]));
                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[str_id]));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_24_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+73);
//                compo_textbox_set(textbox, i18n[func_sport_get_str(func_sport_get_current_idx())]);
                compo_textbox_set(textbox, i18n[str_id]);

                //时间日期
                snprintf(txt_buf,sizeof(txt_buf),"%04d/%02d/%02d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+105);
                compo_textbox_set(textbox, txt_buf);
                compo_textbox_set_forecolor(textbox, COLOR_GREEN);

                //运动数据图标
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIMES_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*3);


                /*运动数据*/
                memset(txt_buf,0,sizeof(txt_buf));//运动时长
                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142);
                compo_textbox_set(textbox, txt_buf);

                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40);
                compo_textbox_set(textbox, txt_buf);
                area_t txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
                compo_textbox_set(textbox, i18n[STR_KCAL]);

                memset(txt_buf,0,sizeof(txt_buf));//运动次数
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*2);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*1);
                compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

                memset(txt_buf,0,sizeof(txt_buf));//运动心率
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*3);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*2);
                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
                /*运动数据*/

                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*4);
                compo_setid(btn,COMPO_BTN_SURE);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*4,GUI_SCREEN_CENTER_X,28);
                compo_textbox_set(textbox, i18n[STR_OK]);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
                compo_textbox_set_align_center(textbox, true);
                compo_textbox_set_multiline(textbox, true);
                // compo_textbox_set_multiline_drag(textbox,true);
                widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*6,GUI_SCREEN_CENTER_X*1.7,28*2);
                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
            }
            break;
            case COMPO_OTHER_STATE://其他
            {
                //运动类型图片
//                compo_picturebox_t *picbox = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
                compo_picturebox_t *picbox = compo_picturebox_create(frm, icon_addr);
                compo_picturebox_set_pos(picbox, 10+56/2, 12+56/2);

                //运动类型名称
//                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[func_sport_get_str(func_sport_get_current_idx())]));
                compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[str_id]));
//                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_24_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+73);
//                compo_textbox_set(textbox, i18n[func_sport_get_str(func_sport_get_current_idx())]);
                compo_textbox_set(textbox, i18n[str_id]);

                //时间日期
                snprintf(txt_buf,sizeof(txt_buf),"%04d/%02d/%02d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 10, 5+105);
                compo_textbox_set(textbox, txt_buf);
                compo_textbox_set_forecolor(textbox, COLOR_GREEN);

                //运动数据图标
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIME_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40);
                picbox = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);
                compo_picturebox_set_pos(picbox, 12+21/2, 5+145+21/2+40*2);

                /*运动数据*/
                memset(txt_buf,0,sizeof(txt_buf));//运动时长
                snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142);
                compo_textbox_set(textbox, txt_buf);

                memset(txt_buf,0,sizeof(txt_buf));//运动卡路里
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40);
                compo_textbox_set(textbox, txt_buf);
                area_t txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184);
                compo_textbox_set(textbox, i18n[STR_KCAL]);

                memset(txt_buf,0,sizeof(txt_buf));//运动心率
                snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.avgHeartRate);
                textbox = compo_textbox_create(frm, strlen(txt_buf));
                compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 44,5+142+40*2);
                compo_textbox_set(textbox, txt_buf);
                txt_leng = widget_text_get_area(textbox->txt);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set_pos(textbox, 50+txt_leng.wid, 5+184+40*1);
                compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
                /*运动数据*/

                compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_BIN);
                compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 5+184+40*3);
                compo_setid(btn,COMPO_BTN_SURE);
                textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
                compo_textbox_set_forecolor(textbox, COLOR_BLACK);
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*3,GUI_SCREEN_CENTER_X,28);
                compo_textbox_set(textbox, i18n[STR_OK]);

                textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
                compo_textbox_set_align_center(textbox, true);
                compo_textbox_set_multiline(textbox, true);
                // compo_textbox_set_multiline_drag(textbox,true);
                widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
                compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 5+184+40*5,GUI_SCREEN_CENTER_X*1.7,28*2);
                compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);
            }
            break;
        }
    }
    return frm;
}

enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
extern u32 func_sport_get_disp_mode(void);//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
static void func_sport_finish_init(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }

    int page_length=0;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
            f_sport_finish->sport_finish_state = COMPO_WALK_STATE;
            page_length = 524;
            break;
        case MID_DATA:
            f_sport_finish->sport_finish_state = COMPO_SWIMING_STATE;
            page_length = 524-24*2;
            break;
        case LESS_DATA:
            f_sport_finish->sport_finish_state = COMPO_OTHER_STATE;
            page_length = 524-24*4;
            break;
    }

    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .page_size = page_length+5,
        .page_count = 1,
        .up_over_perc = 5,
        .down_over_perc = 5,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;

} f_sport_finish_t;

extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    u16 str_id = func_sport_get_str(sys_cb.sport_idx);
    u32 icon_addr = func_sport_get_ui(sys_cb.sport_idx);
    char txt_buf[50];
    ute_module_more_sports_data_t sport_data;
    uteModuleSportGetMoreSportsDatas(&sport_data);

    if(uteModuleSystemtime12HOn())
    {
        sport_data.saveData.startSportTime.hour %=12;
        if(sport_data.saveData.startSportTime.hour == 0)
        {
            sport_data.saveData.startSportTime.hour = 12;
        }
    }
    compo_textbox_t *textbox;
    compo_picturebox_t *picbox;
    area_t txt_leng;

    //设置标题栏名字//
    textbox = compo_textbox_create(frm,strlen(i18n[str_id]));
    compo_textbox_set_location(textbox,GUI_SCREEN_WIDTH/2,38/2,120,38);
    compo_textbox_set(textbox, i18n[str_id]);

    //第一页
    //运动类型图片
    picbox = compo_picturebox_create(frm, icon_addr);
    compo_picturebox_set_size(picbox, 120, 120);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //时间日期
    snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %d:%d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, 260+34/2);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);

    //第二页
    /*运动时长->图片*/
    picbox = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_TIME_BIN);
    compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40);
    /*运动时长->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[STR_ALL_TIMER]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48);
    compo_textbox_set(textbox, i18n[STR_ALL_TIMER]);
    compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43);
    compo_textbox_set(textbox, txt_buf);

    /*卡路里->图片*/
    picbox = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_CALORIES_BIN);
    compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40+92);
    /*卡路里->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIES]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48+92);
    compo_textbox_set(textbox, i18n[STR_CALORIES]);
    compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
    /*卡路里->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.sportCaloire);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43+92);
    compo_textbox_set(textbox, txt_buf);
    /*卡路里->单位*/
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
    // printf("sport_num:%d\n",sport_finish_mode);
    if(func_sport_get_disp_mode() == MULTIPLE_DATA)
    {
        u8 km_integer=sport_data.saveData.sportDistanceInteger;                 //距离 整数
        u8 km_decimals=sport_data.saveData.sportDistanceDecimals;                     // 小数
        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            uint16_t distance = km_integer*1000+km_decimals*10;
            distance = distance*0.6213712;
            km_integer  = distance/1000;
            km_decimals = distance%1000/10;
        }

        /*公里->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_DIS_BIN);
        compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40+92*2);
        /*公里->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_TOTAL_DISTANCE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48+92*2);
        compo_textbox_set(textbox, i18n[STR_TOTAL_DISTANCE]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*公里->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, txt_buf);
        /*公里->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE]));
            compo_textbox_set_align_center(textbox, false);
            compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92*2);
            compo_textbox_set(textbox, i18n[STR_MILE]);
        }
        else
        {
            textbox = compo_textbox_create(frm, strlen(i18n[STR_KM]));
            compo_textbox_set_align_center(textbox, false);
            compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92*2);
            compo_textbox_set(textbox, i18n[STR_KM]);
        }

        /*步数->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_STEP_BIN);
        compo_picturebox_set_pos(picbox, 60+19, 2*GUI_SCREEN_HEIGHT+48+40);
        /*步数->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_ALL_STEP]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, 2*GUI_SCREEN_HEIGHT+48);
        compo_textbox_set(textbox, i18n[STR_ALL_STEP]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*步数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,2*GUI_SCREEN_HEIGHT+48+43);
        compo_textbox_set(textbox, txt_buf);
        /*步数->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,2*GUI_SCREEN_HEIGHT+48+43);
        compo_textbox_set(textbox, i18n[STR_STEP]);

        /*心率->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_HR_BIN);
        compo_picturebox_set_pos(picbox, 60+19, 2*GUI_SCREEN_HEIGHT+48+40+92);
        /*心率->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_AVERAGE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, 2*GUI_SCREEN_HEIGHT+48+92);
        compo_textbox_set(textbox, i18n[STR_AVERAGE]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*心率->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,2*GUI_SCREEN_HEIGHT+48+43+92);
        compo_textbox_set(textbox, txt_buf);
        /*心率->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,2*GUI_SCREEN_HEIGHT+48+43+92);
        compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);

    }
    else if(func_sport_get_disp_mode() == MID_DATA)
    {
        /*次数->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_TIMES_BIN);
        compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40+92*2);
        /*次数->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_COUNT_NUM]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48+92*2);
        compo_textbox_set(textbox, i18n[STR_COUNT_NUM]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*次数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, txt_buf);
        /*次数->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        /*心率->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_HR_BIN);
        compo_picturebox_set_pos(picbox, 60+19, 2*GUI_SCREEN_HEIGHT+48+40);
        /*心率->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_AVERAGE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, 2*GUI_SCREEN_HEIGHT+48);
        compo_textbox_set(textbox, i18n[STR_AVERAGE]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*心率->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,2*GUI_SCREEN_HEIGHT+48+43);
        compo_textbox_set(textbox, txt_buf);
        /*心率->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,2*GUI_SCREEN_HEIGHT+48+43);
        compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
    }
    else if(func_sport_get_disp_mode() == LESS_DATA)
    {
        /*心率->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_HR_BIN);
        compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40+92*2);
        /*心率->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_AVERAGE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48+92*2);
        compo_textbox_set(textbox, i18n[STR_AVERAGE]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*心率->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, txt_buf);
        /*心率->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
    }
    u16 y;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
        case MID_DATA:
            y = GUI_SCREEN_HEIGHT*3;
            break;
        case LESS_DATA:
            y = GUI_SCREEN_HEIGHT*2;
            break;
    }
    /*心率->图片*/
    picbox = compo_picturebox_create(frm, UI_BUF_I332001_PERIOD_PHONE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, y+88/2+50);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, y+191,320,widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I332001_SPORT_BTN_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, y+34+266);
    compo_setid(btn,COMPO_BTN_SURE);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
    compo_textbox_set_forecolor(textbox, COLOR_BLACK);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, y+34+266,GUI_SCREEN_CENTER_X,64);
    compo_textbox_set(textbox, i18n[STR_OK]);

    return frm;
}

static void func_sport_finish_init(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }
    int page_num=0;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
            page_num = 4;
            break;
        case MID_DATA:
            page_num = 4;
            break;
        case LESS_DATA:
            page_num = 3;
            break;
    }

    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used=false,
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = page_num,
        .jump_perc = 20,
        .quick_jump_perc = 0,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;

} f_sport_finish_t;

extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

#define TXT_SPACING  87
//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    u16 str_id = func_sport_get_str(sys_cb.sport_idx);
    u32 icon_addr = func_sport_get_ui(sys_cb.sport_idx);
    char txt_buf[50];
    ute_module_more_sports_data_t sport_data;
    uteModuleSportGetMoreSportsDatas(&sport_data);

    if(uteModuleSystemtime12HOn())
    {
        sport_data.saveData.startSportTime.hour %=12;
        if(sport_data.saveData.startSportTime.hour == 0)
        {
            sport_data.saveData.startSportTime.hour = 12;
        }
    }
    compo_textbox_t *textbox;
    compo_picturebox_t *picbox;

    //第一页
    //运动类型图片
    picbox = compo_picturebox_create(frm, icon_addr);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 42/2+32);
    /*运动类型->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[str_id]));

    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, 26/2+88);
    compo_textbox_set(textbox, i18n[str_id]);

    //时间日期
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, 121+22/2);
    compo_textbox_set(textbox, txt_buf);
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_3_EXERCISE_9_FINISH_ICON_44X44_X16_Y174_Y261_Y348_Y435_Y522_Y609_00_TIME_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CUSTOM_TIME]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set_location(textbox,75,170,130,25);
    compo_textbox_set(textbox, i18n[STR_CUSTOM_TIME]);

    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195);
    compo_textbox_set(textbox, txt_buf);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
    u16 accrual_y = 0;
///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_3_EXERCISE_9_FINISH_ICON_44X44_X16_Y174_Y261_Y348_Y435_Y522_Y609_01_HEART_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_HEART_RATE]);

    /*运动心率->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+accrual_y,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_3_EXERCISE_9_FINISH_ICON_44X44_X16_Y174_Y261_Y348_Y435_Y522_Y609_02_KCAL_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_CALORIE]);

    /*运动卡路里->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
//////////////////////////////////////////////////////////////////////////////////////////////////////

    u8 sport_type = 0;

    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA://多数据
#if UTE_MODULE_SCREENS_SPORT_KM_OFF
            sport_type = 1;
#else
            sport_type = 0;
#endif
            break;
        case MID_DATA:     //中数据
            sport_type = 2;
            break;
        case LESS_DATA:    //少数据
            sport_type = 3;
            break;
    }

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I335001_3_EXERCISE_9_FINISH_BUTTON_ICON_PIC208X52_X16_Y1014_00_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 195+TXT_SPACING*6-(sport_type*TXT_SPACING));
    compo_setid(btn,COMPO_BTN_SURE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 180+TXT_SPACING*7-(sport_type*TXT_SPACING),GUI_SCREEN_CENTER_X*1.7,28);
    compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);

///               判断运动类型                       /
    if(func_sport_get_disp_mode() == LESS_DATA)
    {
        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+TXT_SPACING,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
    if(func_sport_get_disp_mode() == MID_DATA)
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I335001_3_EXERCISE_9_FINISH_ICON_44X44_X16_Y174_Y261_Y348_Y435_Y522_Y609_06_FREQUENCY_BIN);
        compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
        compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        /*次数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox,75,195+accrual_y);
        compo_textbox_set(textbox, txt_buf);

        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+accrual_y,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_3_EXERCISE_9_FINISH_ICON_44X44_X16_Y174_Y261_Y348_Y435_Y522_Y609_03_STEP_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEPS]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_STEPS]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, i18n[STR_STEP]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+accrual_y,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
    u8 km_integer  = sport_data.saveData.sportDistanceInteger;                 //距离 整数
    u8 km_decimals = sport_data.saveData.sportDistanceDecimals;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_3_EXERCISE_9_FINISH_ICON_44X44_X16_Y174_Y261_Y348_Y435_Y522_Y609_04_PACE_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PACE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_PACE]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    if(sport_data.saveData.avgTimeSecond)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d'%d%c", sport_data.saveData.avgTimeMinute, sport_data.saveData.avgTimeSecond,'"');
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d'%d%c", 0,0,'"');
    }
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"/%s",uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, txt_buf);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+accrual_y,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));

///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
#if (UTE_MODULE_SCREENS_SPORT_KM_OFF==0)
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_3_EXERCISE_9_FINISH_ICON_44X44_X16_Y174_Y261_Y348_Y435_Y522_Y609_05_DIS_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DISTANCE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_DISTANCE]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE])+strlen(i18n[STR_KILOMETRE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
#endif
    return frm;
}

static void func_sport_finish_init(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }
    int page_size=0;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
            page_size = 836;
#if UTE_MODULE_SCREENS_SPORT_KM_OFF
            page_size -= TXT_SPACING;
#endif
            break;
        case MID_DATA:
            page_size = 665;
            break;
        case LESS_DATA:
            page_size = 577;
            break;
    }
    page_size+=TXT_SPACING;
    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used=false,
        .page_size = page_size-65,
        .page_count = 1,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;

} f_sport_finish_t;

extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

#define TXT_SPACING  133

    u16 str_id = func_sport_get_str(sys_cb.sport_idx);
    u32 icon_addr = func_sport_get_ui(sys_cb.sport_idx);
    char txt_buf[50];
    ute_module_more_sports_data_t sport_data;
    uteModuleSportGetMoreSportsDatas(&sport_data);

    if(uteModuleSystemtime12HOn())
    {
        sport_data.saveData.startSportTime.hour %=12;
        if(sport_data.saveData.startSportTime.hour == 0)
        {
            sport_data.saveData.startSportTime.hour = 12;
        }
    }
    compo_textbox_t *textbox;
    compo_picturebox_t *picbox;

    //第一页
    //运动类型图片
    picbox = compo_picturebox_create(frm, icon_addr);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 66/2+44);
    /*运动类型->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[str_id]));

    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, 20+125);
    compo_textbox_set(textbox, i18n[str_id]);

    //时间日期
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, 174+20);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_5_EXERCISING_ICON_TIME_BIN);
    compo_picturebox_set_pos(picbox, 66/2+24, 66/2+265);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CUSTOM_TIME]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set_location(textbox,113,257,130,40);
    compo_textbox_set(textbox, i18n[STR_CUSTOM_TIME]);

    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,113,296);
    compo_textbox_set(textbox, txt_buf);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,366,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_5_EXERCISING_ICON_HEART_BIN);
    compo_picturebox_set_pos(picbox, 66/2+24, 66/2+265+TXT_SPACING);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113,257+TXT_SPACING,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_HEART_RATE]);

    /*运动心率->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,113,296+TXT_SPACING);
    compo_textbox_set(textbox, txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113+8+txt_leng.wid,303+TXT_SPACING,88,40);
    compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,366+TXT_SPACING,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_5_EXERCISING_ICON_KCAL_BIN);
    compo_picturebox_set_pos(picbox, 66/2+24, 66/2+265+TXT_SPACING*2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113,257+TXT_SPACING*2,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_CALORIE]);

    /*运动卡路里->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,113,296+TXT_SPACING*2);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113+8+txt_leng.wid,303+TXT_SPACING*2,88,40);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
//////////////////////////////////////////////////////////////////////////////////////////////////////

    u8 sport_type = 0;

    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA://多数据
            sport_type = 0;
            break;
        case MID_DATA:     //中数据
            sport_type = 2;
            break;
        case LESS_DATA:    //少数据
            sport_type = 3;
            break;
    }

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I341001_28_SET_CONFIRM_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 296+TXT_SPACING*6-(sport_type*TXT_SPACING));
    compo_setid(btn,COMPO_BTN_SURE);

    // textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
    // compo_textbox_set_align_center(textbox, true);
    // compo_textbox_set_multiline(textbox, true);
    // widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    // compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 180+TXT_SPACING*7-(sport_type*TXT_SPACING),GUI_SCREEN_CENTER_X*1.7,28*2);
    // compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);

///               判断运动类型                       /
    if(func_sport_get_disp_mode() == LESS_DATA)
    {
        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,366+TXT_SPACING*2,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    if(func_sport_get_disp_mode() == MID_DATA)
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_5_EXERCISING_ICON_TIMES_BIN);
        compo_picturebox_set_pos(picbox, 66/2+24, 66/2+265+TXT_SPACING*3);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,113,257+TXT_SPACING*3,130,40);
        compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        /*次数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox,113,296+TXT_SPACING*3);
        compo_textbox_set(textbox, txt_buf);

        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,133+8+txt_leng.wid,303+TXT_SPACING*3,88,40);
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,366+TXT_SPACING*3,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_5_EXERCISING_ICON_STEP_BIN);
    compo_picturebox_set_pos(picbox, 66/2+24, 66/2+265+TXT_SPACING*3);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEPS]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113,257+TXT_SPACING*3,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_STEPS]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,113,296+TXT_SPACING*3);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113+8+txt_leng.wid,303+TXT_SPACING*3,88,40);
    compo_textbox_set(textbox, i18n[STR_STEP]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,366+TXT_SPACING*3,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    u8 km_integer  = sport_data.saveData.sportDistanceInteger;                 //距离 整数
    u8 km_decimals = sport_data.saveData.sportDistanceDecimals;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_5_EXERCISING_ICON_PACE_BIN);
    compo_picturebox_set_pos(picbox, 66/2+24, 66/2+265+TXT_SPACING*4);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PACE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113,257+TXT_SPACING*4,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_PACE]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    if(sport_data.saveData.avgTimeSecond)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d'%d%c", sport_data.saveData.avgTimeMinute, sport_data.saveData.avgTimeSecond,'"');
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d'%d%c", 0,0,'"');
    }
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,113,296+TXT_SPACING*4);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"/%s",uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113+8+txt_leng.wid,303+TXT_SPACING*4,88,25);
    compo_textbox_set(textbox, txt_buf);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,366+TXT_SPACING*4,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));

///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_5_EXERCISING_ICON_DIS_BIN);
    compo_picturebox_set_pos(picbox, 66/2+24, 66/2+265+TXT_SPACING*5);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DISTANCE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113,257+TXT_SPACING*5,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_DISTANCE]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,113,296+TXT_SPACING*5);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE])+strlen(i18n[STR_KILOMETRE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113+8+txt_leng.wid,303+TXT_SPACING*5,88,40);
    compo_textbox_set(textbox, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);

    return frm;
}

static void func_sport_finish_init(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }
    int page_size=0;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
            page_size = 1288;
            break;
        case MID_DATA:
            page_size = 1014;
            break;
        case LESS_DATA:
            page_size = 886;
            break;
    }

    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used=false,
        .page_size = page_size-106,
        .page_count = 1,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;

} f_sport_finish_t;

extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

#define TXT_SPACING  100

    u16 str_id = func_sport_get_str(sys_cb.sport_idx);
    u32 icon_addr = func_sport_get_ui(sys_cb.sport_idx);
    char txt_buf[50];
    ute_module_more_sports_data_t sport_data;
    uteModuleSportGetMoreSportsDatas(&sport_data);

    if(uteModuleSystemtime12HOn())
    {
        sport_data.saveData.startSportTime.hour %=12;
        if(sport_data.saveData.startSportTime.hour == 0)
        {
            sport_data.saveData.startSportTime.hour = 12;
        }
    }
    compo_textbox_t *textbox;
    compo_picturebox_t *picbox;

    //第一页
    //运动类型图片
    picbox = compo_picturebox_create(frm, icon_addr);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 58/2+38);
    /*运动类型->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[str_id]));

    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, 20+109);
    compo_textbox_set(textbox, i18n[str_id]);

    //时间日期
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, 152+20);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I343001_3_EXERCISE_5_EXERCISING_ICON1_TIME_BIN);
    compo_picturebox_set_pos(picbox, 58/2+21, 58/2+231);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CUSTOM_TIME]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set_location(textbox,98,223,130,40);
    compo_textbox_set(textbox, i18n[STR_CUSTOM_TIME]);

    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,98,257);
    compo_textbox_set(textbox, txt_buf);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,223+95,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I343001_3_EXERCISE_5_EXERCISING_ICON1_HEART_BIN);
    compo_picturebox_set_pos(picbox, 58/2+21, 58/2+231+TXT_SPACING);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98,223+TXT_SPACING,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_HEART_RATE]);

    /*运动心率->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,98,257+TXT_SPACING);
    compo_textbox_set(textbox, txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98+8+txt_leng.wid,257+TXT_SPACING,88,40);
    compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,223+95+TXT_SPACING,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I343001_3_EXERCISE_5_EXERCISING_ICON1_KCAL_BIN);
    compo_picturebox_set_pos(picbox, 58/2+21, 58/2+231+TXT_SPACING*2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98,223+TXT_SPACING*2,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_CALORIE]);

    /*运动卡路里->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,98,257+TXT_SPACING*2);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98+8+txt_leng.wid,257+TXT_SPACING*2,88,40);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
//////////////////////////////////////////////////////////////////////////////////////////////////////

    u8 sport_type = 0;

    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA://多数据
            sport_type = 0;
            break;
        case MID_DATA:     //中数据
            sport_type = 2;
            break;
        case LESS_DATA:    //少数据
            sport_type = 3;
            break;
    }

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I343001_COMM_OK_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 257+TXT_SPACING*6-(sport_type*TXT_SPACING));
    compo_setid(btn,COMPO_BTN_SURE);

    // textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
    // compo_textbox_set_align_center(textbox, true);
    // compo_textbox_set_multiline(textbox, true);
    // widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    // compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 180+TXT_SPACING*7-(sport_type*TXT_SPACING),GUI_SCREEN_CENTER_X*1.7,28*2);
    // compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);

///               判断运动类型                       /
    if(func_sport_get_disp_mode() == LESS_DATA)
    {
        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,223+95+TXT_SPACING*2,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    if(func_sport_get_disp_mode() == MID_DATA)
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I343001_3_EXERCISE_5_EXERCISING_ICON1_TIMES_BIN);
        compo_picturebox_set_pos(picbox, 58/2+21, 58/2+231+TXT_SPACING*3);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,98,223+TXT_SPACING*3,130,40);
        compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        /*次数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox,98,257+TXT_SPACING*3);
        compo_textbox_set(textbox, txt_buf);

        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,98+8+txt_leng.wid,257+TXT_SPACING*3,88,40);
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,223+95+TXT_SPACING*3,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I343001_3_EXERCISE_5_EXERCISING_ICON1_STEP_BIN);
    compo_picturebox_set_pos(picbox, 58/2+21, 58/2+231+TXT_SPACING*3);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEPS]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98,223+TXT_SPACING*3,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_STEPS]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,98,257+TXT_SPACING*3);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98+8+txt_leng.wid,257+TXT_SPACING*3,88,40);
    compo_textbox_set(textbox, i18n[STR_STEP]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,223+95+TXT_SPACING*3,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    u8 km_integer  = sport_data.saveData.sportDistanceInteger;                 //距离 整数
    u8 km_decimals = sport_data.saveData.sportDistanceDecimals;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I343001_3_EXERCISE_5_EXERCISING_ICON1_PACE_BIN);
    compo_picturebox_set_pos(picbox, 58/2+21, 58/2+231+TXT_SPACING*4);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PACE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98,223+TXT_SPACING*4,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_PACE]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    if(sport_data.saveData.avgTimeSecond)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d'%d%c", sport_data.saveData.avgTimeMinute, sport_data.saveData.avgTimeSecond,'"');
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d'%d%c", 0,0,'"');
    }
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,98,257+TXT_SPACING*4);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"/%s",uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98+8+txt_leng.wid,257+TXT_SPACING*4,88,25);
    compo_textbox_set(textbox, txt_buf);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,223+95+TXT_SPACING*4,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));

///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I343001_3_EXERCISE_5_EXERCISING_ICON1_DIS_BIN);
    compo_picturebox_set_pos(picbox, 58/2+21, 58/2+231+TXT_SPACING*5);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DISTANCE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98,223+TXT_SPACING*5,130,40);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_DISTANCE]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,98,257+TXT_SPACING*5);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE])+strlen(i18n[STR_KILOMETRE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,98+8+txt_leng.wid,257+TXT_SPACING*5,88,40);
    compo_textbox_set(textbox, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);

    return frm;
}

static void func_sport_finish_init(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }
    int page_size=0;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
            page_size = 1288-68;
            break;
        case MID_DATA:
            page_size = 1014-68;
            break;
        case LESS_DATA:
            page_size = 886-68;
            break;
    }

    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used=false,
        .page_size = page_size-106,
        .page_count = 1,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;

} f_sport_finish_t;

extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

#define TXT_SPACING  117

    u16 str_id = func_sport_get_str(sys_cb.sport_idx);
    u32 icon_addr = func_sport_get_ui(sys_cb.sport_idx);
    char txt_buf[50];
    ute_module_more_sports_data_t sport_data;
    uteModuleSportGetMoreSportsDatas(&sport_data);

    if(uteModuleSystemtime12HOn())
    {
        sport_data.saveData.startSportTime.hour %=12;
        if(sport_data.saveData.startSportTime.hour == 0)
        {
            sport_data.saveData.startSportTime.hour = 12;
        }
    }
    compo_textbox_t *textbox;
    compo_picturebox_t *picbox;

    //第一页
    //运动类型图片
    picbox = compo_picturebox_create(frm, icon_addr);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 58/2+38);
    compo_picturebox_set_size(picbox, 58, 58);

    /*运动类型->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[str_id]));

    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, 42/2+109);
    compo_textbox_set(textbox, i18n[str_id]);

    //时间日期
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, 152+38/2);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I338001_3_EXERCISE_ICON1_TIME_BIN);
    compo_picturebox_set_pos(picbox, 58/2+45, 58/2+231);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CUSTOM_TIME]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set_location(textbox,126,217,130,30);
    compo_textbox_set(textbox, i18n[STR_CUSTOM_TIME]);

    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,126,251);
    compo_textbox_set(textbox, txt_buf);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,317,278,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I338001_3_EXERCISE_ICON1_HEART_BIN);
    compo_picturebox_set_pos(picbox, 58/2+45, 58/2+231+TXT_SPACING);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,126,217+TXT_SPACING,130,30);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_HEART_RATE]);

    /*运动心率->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,126,251+TXT_SPACING);
    compo_textbox_set(textbox, txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,130+txt_leng.wid,253+TXT_SPACING,88,30);
    compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,317+TXT_SPACING,278,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I338001_3_EXERCISE_ICON1_KCAL_BIN);
    compo_picturebox_set_pos(picbox, 58/2+45, 58/2+231+TXT_SPACING*2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,126,217+TXT_SPACING*2,130,30);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_CALORIE]);

    /*运动卡路里->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,126,251+TXT_SPACING*2);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,130+txt_leng.wid,253+TXT_SPACING*2,88,30);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
//////////////////////////////////////////////////////////////////////////////////////////////////////

    u8 sport_type = 0;

    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA://多数据
            sport_type = 0;
            break;
        case MID_DATA:     //中数据
            sport_type = 2;
            break;
        case LESS_DATA:    //少数据
            sport_type = 3;
            break;
    }

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I338001_3_EXERCISE_5_EXERCISING_OK_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 251+TXT_SPACING*6-(sport_type*TXT_SPACING));
    compo_setid(btn,COMPO_BTN_SURE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
    compo_textbox_set_align_center(textbox, true);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 225+TXT_SPACING*7-(sport_type*TXT_SPACING),GUI_SCREEN_CENTER_X*1.7,30*2);
    compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);

///               判断运动类型                       /
    if(func_sport_get_disp_mode() == LESS_DATA)
    {
        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,317+TXT_SPACING*2,278,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    if(func_sport_get_disp_mode() == MID_DATA)
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I338001_3_EXERCISE_ICON1_TIMES_BIN);
        compo_picturebox_set_pos(picbox, 58/2+45, 58/2+231+TXT_SPACING*3);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,126,217+TXT_SPACING*3,130,30);
        compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        /*次数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox,126,251+TXT_SPACING*3);
        compo_textbox_set(textbox, txt_buf);

        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,130+txt_leng.wid,253+TXT_SPACING*3,88,30);
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,317+TXT_SPACING*3,278,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I338001_3_EXERCISE_ICON1_STEP_BIN);
    compo_picturebox_set_pos(picbox, 58/2+45, 58/2+231+TXT_SPACING*3);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEPS]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,126,217+TXT_SPACING*3,130,30);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_STEPS]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.sportStep);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,126,251+TXT_SPACING*3);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,130+txt_leng.wid,253+TXT_SPACING*3,88,30);
    compo_textbox_set(textbox, i18n[STR_STEP]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,317+TXT_SPACING*4,278,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    u8 km_integer  = sport_data.saveData.sportDistanceInteger;                 //距离 整数
    u8 km_decimals = sport_data.saveData.sportDistanceDecimals;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I338001_3_EXERCISE_ICON1_PACE_BIN);
    compo_picturebox_set_pos(picbox, 58/2+45, 58/2+231+TXT_SPACING*4);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PACE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,126,217+TXT_SPACING*4,130,30);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_PACE]);

    /*运动配速->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d'%02d%c",sport_data.saveData.avgTimeMinute,sport_data.saveData.avgTimeSecond,'"');
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,126,251+TXT_SPACING*4);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"/%s",uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,130+txt_leng.wid,253+TXT_SPACING*4,88,30);
    compo_textbox_set(textbox, txt_buf);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,317+TXT_SPACING*5,278,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));

///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I338001_3_EXERCISE_ICON1_DIS_BIN);
    compo_picturebox_set_pos(picbox, 58/2+45, 58/2+231+TXT_SPACING*5);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DISTANCE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,126,217+TXT_SPACING*5,130,30);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_DISTANCE]);

    /*运动距离->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,126,251+TXT_SPACING*5);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE])+strlen(i18n[STR_KILOMETRE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,130+txt_leng.wid,253+TXT_SPACING*5,88,30);
    compo_textbox_set(textbox, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);

    return frm;
}

static void func_sport_finish_init(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }
    int page_size=0;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
            page_size = 1134;
            break;
        case MID_DATA:
            page_size = 900;
            break;
        case LESS_DATA:
            page_size = 798;
            break;
    }

    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used=false,
        .page_size = page_size,
        .page_count = 1,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;

} f_sport_finish_t;

extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    u16 str_id = func_sport_get_str(sys_cb.sport_idx);
    u32 icon_addr = func_sport_get_ui(sys_cb.sport_idx);
    char txt_buf[50];
    ute_module_more_sports_data_t sport_data;
    uteModuleSportGetMoreSportsDatas(&sport_data);

    if(uteModuleSystemtime12HOn())
    {
        sport_data.saveData.startSportTime.hour %=12;
        if(sport_data.saveData.startSportTime.hour == 0)
        {
            sport_data.saveData.startSportTime.hour = 12;
        }
    }
    compo_textbox_t *textbox;
    compo_picturebox_t *picbox;
    area_t txt_leng;

    //设置标题栏名字//
    textbox = compo_textbox_create(frm,strlen(i18n[str_id]));
    compo_textbox_set_location(textbox,GUI_SCREEN_WIDTH/2,38/2,120,38);
    compo_textbox_set(textbox, i18n[str_id]);

    //第一页
    //运动类型图片
    picbox = compo_picturebox_create(frm, icon_addr);
    compo_picturebox_set_size(picbox, 120, 120);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    //时间日期
    snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %d:%d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_20_BIN);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, 260+34/2);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);

    //第二页
    /*运动时长->图片*/
    picbox = compo_picturebox_create(frm, UI_BUF_I340001_SPORT_ICON2_TIME_BIN);
    compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40);
    /*运动时长->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[STR_ALL_TIMER]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48);
    compo_textbox_set(textbox, i18n[STR_ALL_TIMER]);
    compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43);
    compo_textbox_set(textbox, txt_buf);

    /*卡路里->图片*/
    picbox = compo_picturebox_create(frm, UI_BUF_I340001_SPORT_ICON2_CALORIES_BIN);
    compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40+92);
    /*卡路里->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIES]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48+92);
    compo_textbox_set(textbox, i18n[STR_CALORIES]);
    compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
    /*卡路里->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.sportCaloire);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43+92);
    compo_textbox_set(textbox, txt_buf);
    /*卡路里->单位*/
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
    // printf("sport_num:%d\n",sport_finish_mode);
    if(func_sport_get_disp_mode() == MULTIPLE_DATA)
    {
        u8 km_integer=sport_data.saveData.sportDistanceInteger;                 //距离 整数
        u8 km_decimals=sport_data.saveData.sportDistanceDecimals;                     // 小数
        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            uint16_t distance = km_integer*1000+km_decimals*10;
            distance = distance*0.6213712;
            km_integer  = distance/1000;
            km_decimals = distance%1000/10;
        }

        /*公里->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I340001_SPORT_ICON2_DIS_BIN);
        compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40+92*2);
        /*公里->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_TOTAL_DISTANCE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48+92*2);
        compo_textbox_set(textbox, i18n[STR_TOTAL_DISTANCE]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*公里->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, txt_buf);
        /*公里->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE]));
            compo_textbox_set_align_center(textbox, false);
            compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92*2);
            compo_textbox_set(textbox, i18n[STR_MILE]);
        }
        else
        {
            textbox = compo_textbox_create(frm, strlen(i18n[STR_KM]));
            compo_textbox_set_align_center(textbox, false);
            compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92*2);
            compo_textbox_set(textbox, i18n[STR_KM]);
        }

        /*步数->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I340001_SPORT_ICON2_STEP_BIN);
        compo_picturebox_set_pos(picbox, 60+19, 2*GUI_SCREEN_HEIGHT+48+40);
        /*步数->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_ALL_STEP]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, 2*GUI_SCREEN_HEIGHT+48);
        compo_textbox_set(textbox, i18n[STR_ALL_STEP]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*步数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,2*GUI_SCREEN_HEIGHT+48+43);
        compo_textbox_set(textbox, txt_buf);
        /*步数->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,2*GUI_SCREEN_HEIGHT+48+43);
        compo_textbox_set(textbox, i18n[STR_STEP]);

        /*心率->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I340001_SPORT_ICON2_HR_BIN);
        compo_picturebox_set_pos(picbox, 60+19, 2*GUI_SCREEN_HEIGHT+48+40+92);
        /*心率->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_AVERAGE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, 2*GUI_SCREEN_HEIGHT+48+92);
        compo_textbox_set(textbox, i18n[STR_AVERAGE]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*心率->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,2*GUI_SCREEN_HEIGHT+48+43+92);
        compo_textbox_set(textbox, txt_buf);
        /*心率->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,2*GUI_SCREEN_HEIGHT+48+43+92);
        compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);

    }
    else if(func_sport_get_disp_mode() == MID_DATA)
    {
        /*次数->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I340001_SPORT_ICON2_TIMES_BIN);
        compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40+92*2);
        /*次数->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_COUNT_NUM]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48+92*2);
        compo_textbox_set(textbox, i18n[STR_COUNT_NUM]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*次数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, txt_buf);
        /*次数->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        /*心率->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I340001_SPORT_ICON2_HR_BIN);
        compo_picturebox_set_pos(picbox, 60+19, 2*GUI_SCREEN_HEIGHT+48+40);
        /*心率->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_AVERAGE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, 2*GUI_SCREEN_HEIGHT+48);
        compo_textbox_set(textbox, i18n[STR_AVERAGE]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*心率->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,2*GUI_SCREEN_HEIGHT+48+43);
        compo_textbox_set(textbox, txt_buf);
        /*心率->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,2*GUI_SCREEN_HEIGHT+48+43);
        compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
    }
    else if(func_sport_get_disp_mode() == LESS_DATA)
    {
        /*心率->图片*/
        picbox = compo_picturebox_create(frm, UI_BUF_I340001_SPORT_ICON2_HR_BIN);
        compo_picturebox_set_pos(picbox, 60+19, GUI_SCREEN_HEIGHT+48+40+92*2);
        /*心率->标题*/
        textbox = compo_textbox_create(frm, strlen(i18n[STR_AVERAGE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54, GUI_SCREEN_HEIGHT+48+92*2);
        compo_textbox_set(textbox, i18n[STR_AVERAGE]);
        compo_textbox_set_forecolor(textbox,make_color(0x80,0x80,0x80));
        /*心率->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+54,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, txt_buf);
        /*心率->单位*/
        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox, 60+64+txt_leng.wid,GUI_SCREEN_HEIGHT+48+43+92*2);
        compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);
    }
    u16 y;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
        case MID_DATA:
            y = GUI_SCREEN_HEIGHT*3;
            break;
        case LESS_DATA:
            y = GUI_SCREEN_HEIGHT*2;
            break;
    }
    /*心率->图片*/
    picbox = compo_picturebox_create(frm, UI_BUF_I340001_PERIOD_PHONE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, y+88/2+50);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, y+191,320,widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I340001_SPORT_BTN_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, y+34+266);
    compo_setid(btn,COMPO_BTN_SURE);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_OK]));
    compo_textbox_set_forecolor(textbox, COLOR_BLACK);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, y+34+266,GUI_SCREEN_CENTER_X,64);
    compo_textbox_set(textbox, i18n[STR_OK]);

    return frm;
}

static void func_sport_finish_init(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }
    int page_num=0;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
            page_num = 4;
            break;
        case MID_DATA:
            page_num = 4;
            break;
        case LESS_DATA:
            page_num = 3;
            break;
    }

    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used=false,
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = page_num,
        .jump_perc = 20,
        .quick_jump_perc = 0,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;

} f_sport_finish_t;

extern u32 func_sport_get_current_idx(void);
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);

#define TXT_SPACING  87
//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    u16 str_id = func_sport_get_str(sys_cb.sport_idx);
    u32 icon_addr = func_sport_get_ui(sys_cb.sport_idx);
    char txt_buf[50];
    ute_module_more_sports_data_t sport_data;
    uteModuleSportGetMoreSportsDatas(&sport_data);

    if(uteModuleSystemtime12HOn())
    {
        sport_data.saveData.startSportTime.hour %=12;
        if(sport_data.saveData.startSportTime.hour == 0)
        {
            sport_data.saveData.startSportTime.hour = 12;
        }
    }
    compo_textbox_t *textbox;
    compo_picturebox_t *picbox;

    //第一页
    //运动类型图片
    picbox = compo_picturebox_create(frm, icon_addr);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 42/2+32);
    /*运动类型->标题*/
    textbox = compo_textbox_create(frm, strlen(i18n[str_id]));

    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, 26/2+88);
    compo_textbox_set(textbox, i18n[str_id]);

    //时间日期
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d/%d/%d %02d:%02d",sport_data.saveData.startSportTime.year,sport_data.saveData.startSportTime.month,
             sport_data.saveData.startSportTime.day,sport_data.saveData.startSportTime.hour,sport_data.saveData.startSportTime.min);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, 121+22/2);
    compo_textbox_set(textbox, txt_buf);
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I342001_3_EXERCISE_ICON1_TIME_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CUSTOM_TIME]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set_location(textbox,75,170,130,25);
    compo_textbox_set(textbox, i18n[STR_CUSTOM_TIME]);

    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",sport_data.totalSportTime/60/60,sport_data.totalSportTime/60%60,sport_data.totalSportTime%60);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195);
    compo_textbox_set(textbox, txt_buf);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
    u16 accrual_y = 0;
///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
    picbox = compo_picturebox_create(frm, UI_BUF_I342001_3_EXERCISE_ICON1_HEART_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_HEART_RATE]);

    /*运动心率->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d",sport_data.saveData.avgHeartRate);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+accrual_y,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
    picbox = compo_picturebox_create(frm, UI_BUF_I342001_3_EXERCISE_ICON1_KCAL_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_CALORIE]);

    /*运动卡路里->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportCaloire);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
//////////////////////////////////////////////////////////////////////////////////////////////////////

    u8 sport_type = 0;

    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA://多数据
#if UTE_MODULE_SCREENS_SPORT_KM_OFF
            sport_type = 1;
#else
            sport_type = 0;
#endif
            break;
        case MID_DATA:     //中数据
            sport_type = 2;
            break;
        case LESS_DATA:    //少数据
            sport_type = 3;
            break;
    }

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I342001_3_EXERCISE_5_EXERCISING_OK_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 195+TXT_SPACING*6-(sport_type*TXT_SPACING));
    compo_setid(btn,COMPO_BTN_SURE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_FINISH_APP]));//运动说明
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 180+TXT_SPACING*7-(sport_type*TXT_SPACING),GUI_SCREEN_CENTER_X*1.7,28);
    compo_textbox_set(textbox, i18n[STR_SPORT_FINISH_APP]);

///               判断运动类型                       /
    if(func_sport_get_disp_mode() == LESS_DATA)
    {
        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+TXT_SPACING,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
    if(func_sport_get_disp_mode() == MID_DATA)
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I342001_3_EXERCISE_ICON1_TIME_BIN);
        compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
        compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        /*次数->数据*/
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",sport_data.saveData.sportTimes);
        textbox = compo_textbox_create(frm, strlen(txt_buf));
        compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_pos(textbox,75,195+accrual_y);
        compo_textbox_set(textbox, txt_buf);

        txt_leng = widget_text_get_area(textbox->txt);
        textbox = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));
        compo_textbox_set_align_center(textbox, false);
        compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
        compo_textbox_set(textbox, i18n[STR_SPORT_ORDER]);

        return frm;
    }

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+accrual_y,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I342001_3_EXERCISE_ICON1_STEP_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEPS]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_STEPS]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%ld",sport_data.saveData.sportStep);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_STEP]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, i18n[STR_STEP]);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+accrual_y,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));
///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
    u8 km_integer  = sport_data.saveData.sportDistanceInteger;                 //距离 整数
    u8 km_decimals = sport_data.saveData.sportDistanceDecimals;               //距离 小数
    if(uteModuleSystemtimeGetDistanceMiType())//英里
    {
        uint16_t distance = km_integer*1000+km_decimals*10;
        distance = distance*0.6213712;
        km_integer  = distance/1000;
        km_decimals = distance%1000/10;
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_3_EXERCISE_ICON1_PACE_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PACE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_PACE]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    if(sport_data.saveData.avgTimeSecond)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d'%d%c", sport_data.saveData.avgTimeMinute, sport_data.saveData.avgTimeSecond,'"');
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d'%d%c", 0,0,'"');
    }
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"/%s",uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, txt_buf);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,243+accrual_y,208,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));

///////////////////////////////////////////////////////////////////////////////////
    accrual_y+=TXT_SPACING;
#if (UTE_MODULE_SCREENS_SPORT_KM_OFF==0)
    picbox = compo_picturebox_create(frm, UI_BUF_I342001_3_EXERCISE_ICON1_DIS_BIN);
    compo_picturebox_set_pos(picbox, 44/2+16, 44/2+178+accrual_y);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DISTANCE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,170+accrual_y,130,25);
    compo_textbox_set_forecolor(textbox, make_color(0xa3,0xa3,0xa3));
    compo_textbox_set(textbox, i18n[STR_DISTANCE]);

    /*运动步数->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,75,195+accrual_y);
    compo_textbox_set(textbox, txt_buf);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, strlen(i18n[STR_MILE])+strlen(i18n[STR_KILOMETRE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75+8+txt_leng.wid,200+accrual_y,88,25);
    compo_textbox_set(textbox, uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_MILE] : i18n[STR_KILOMETRE]);
#endif
    return frm;
}

static void func_sport_finish_init(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    //防止跳回运动中界面
    if(task_stack_contains(FUNC_SPORT_SUB_RUN))
    {
        task_stack_remove(FUNC_SPORT_SUB_RUN);
    }
    int page_size=0;
    switch(func_sport_get_disp_mode())
    {
        case MULTIPLE_DATA:
            page_size = 836;
#if UTE_MODULE_SCREENS_SPORT_KM_OFF
            page_size -= TXT_SPACING;
#endif
            break;
        case MID_DATA:
            page_size = 665;
            break;
        case LESS_DATA:
            page_size = 577;
            break;
    }
    page_size+=TXT_SPACING;
    func_cb.frm_main = func_sport_finish_form_create();
    f_sport_finish->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used=false,
        .page_size = page_size-65,
        .page_count = 1,
    };
    compo_page_move_init(f_sport_finish->ptm, func_cb.frm_main->page_body, &info);

    f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
}

#else
enum
{
    COMPO_SWIMING_STATE=1,
    COMPO_WALK_STATE,
    COMPO_OTHER_STATE,
};
enum
{
    COMPO_BTN_SURE=1,
    COMPO_ID_KM_VALUE,
    COMPO_ID_KM_UINT,
};

typedef struct f_sport_finish_t_
{
    page_tp_move_t *ptm;
    u8 sport_finish_state;
    bool uint_km;
} f_sport_finish_t;
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);


//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_finish_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
static void func_sport_finish_init(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//功能事件处理
static void func_sport_finish_process(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    compo_page_move_process(f_sport_finish->ptm);
    // printf("move_offset:%d\n",f_sport_finish->ptm->move_offset);
    func_process();
}
//室内跑步功能消息处理
static void func_sport_finish_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case COMPO_BTN_SURE:
            func_back_to();
            break;
    }

}
//室内跑步功能消息处理
static void func_sport_finish_message(size_msg_t msg)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_sport_finish->ptm);
            break;
        case MSG_CTP_CLICK:
            func_sport_finish_click();
            break;
        case MSG_SYS_500MS:
            if(f_sport_finish->uint_km != uteModuleSystemtimeGetDistanceMiType())
            {
                f_sport_finish->uint_km = uteModuleSystemtimeGetDistanceMiType();//英里
                msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
            }
            break;
        default:
            func_message(msg);
            break;
    }
}


//进入室内跑步功能
static void func_sport_finish_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sport_finish_t));
    func_sport_finish_init();
}

//退出室内跑步功能
static void func_sport_finish_exit(void)
{
    f_sport_finish_t *f_sport_finish = (f_sport_finish_t*)func_cb.f_cb;
    func_cb.last = FUNC_SPORT_FINISH;
    if (f_sport_finish->ptm)
    {
        func_free(f_sport_finish->ptm);
    }
}

//室内跑步功能
void func_sport_finish(void)
{
    printf("%s\n", __func__);
    func_sport_finish_enter();
    while (func_cb.sta == FUNC_SPORT_FINISH)
    {
        func_sport_finish_process();
        func_sport_finish_message(msg_dequeue());
    }
    func_sport_finish_exit();
}


