#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_module_heart.h"

#define TRACE_EN        0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    COMPO_ID_NUM_SPORT_TIME = 1,    //运动时间
    COMPO_ID_NUM_SPORT_HEARTRATE,   //心率
    COMPO_ID_NUM_SPORT_KCAL,        //卡路里
    COMPO_ID_NUM_SPORT_STEP,        //计步
    COMPO_ID_NUM_SPORT_KM,          //距离
    COMPO_ID_NUM_SPORT_COUNT,       //计次

    COMPO_ID_UINT_SPORT_TIME,        //运动时间
    COMPO_ID_UINT_SPORT_HEARTRATE,   //心率
    COMPO_ID_UINT_SPORT_KCAL,        //卡路里
    COMPO_ID_UINT_SPORT_STEP,        //计步
    COMPO_ID_UINT_SPORT_KM,          //距离
    COMPO_ID_UINT_SPORT_COUNT,       //计次

    COMPO_ID_PIC_SPORT_HEARTRATE,    //心率图片
    COMPO_ID_TILTE_TIME,             // 标题栏时间

    COMPO_ID_BTN_SPORT_STOP,         //暂停
    COMPO_ID_BTN_SPORT_EXIT,         //退出

    COMPO_ID_TXT_SPORT_STOP,         //暂停

};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

typedef struct f_sport_sub_run_t_
{
    u8 heart_pic_size_perc;
    bool heart_pic_size_add_flag;
    u32 updata_tick;

    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
    bool        sport_run_state;
    bool        sport_run_state_updata_flag;
    bool        sport_run_km_uint_updata_flag;
} f_sport_sub_run_t;

enum
{
    TOUCH_FINISH_STATE=0,
    AUTO_STATE,

};
enum
{
    PAGE_1=0,
    PAGE_2,
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
    TOTCH_MOVE,
};
enum
{
    SPORT_RUN_STOP=false,
    SPORT_RUN_START=true,
};
static bool sport_start_flag = false;
extern u32 func_sport_get_disp_mode(void);//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);
//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_sub_run_form_create(void)
{

    char txt_buf[50];
    ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
    uteModuleSportGetMoreSportsDatas(data);

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    compo_textbox_t* time = compo_textbox_create(frm, 32);
    compo_textbox_set_align_center(time, false);
    compo_textbox_set_location(time, GUI_SCREEN_CENTER_X, 15, GUI_SCREEN_CENTER_X-10, 23);
    compo_setid(time, COMPO_ID_TILTE_TIME);
    compo_bonddata(time, COMPO_BOND_HOURMIN_TXT);
    compo_textbox_set_right_align(time,true);

    compo_picturebox_t* pic = compo_picturebox_create(frm, func_sport_get_ui(sys_cb.sport_idx));///运动类型图片
    compo_picturebox_set_size(pic,56,56);
    compo_picturebox_set_pos(pic,10+56/2,12+56/2);

    compo_textbox_t* txt = compo_textbox_create(frm, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt, 10,76,200,48);
    compo_textbox_set_align_center(txt,false);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",data->totalSportTime / 3600,((data->totalSportTime) % 3600) / 60,(data->totalSportTime) % 60);
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_forecolor(txt, make_color(0xa9,0xff,0x00));
    compo_setid(txt,COMPO_ID_NUM_SPORT_TIME);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN);///卡路里图片
    compo_picturebox_set_size(pic,24,24);
    compo_picturebox_set_pos(pic,22,140);

    txt = compo_textbox_create(frm, 8);///卡路里数据
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_pos(txt, 45,128);
    compo_textbox_set_align_center(txt,false);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", data->saveData.sportCaloire);
    compo_textbox_set(txt, txt_buf);
    compo_setid(txt,COMPO_ID_NUM_SPORT_KCAL);

    area_t txt_wid = widget_text_get_area(txt->txt);
    txt = compo_textbox_create(frm, strlen(i18n[STR_KCAL]));///卡路里单位
    compo_textbox_set_location(txt, txt_wid.wid+50,131,110,widget_text_get_max_height());
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set(txt, i18n[STR_KCAL]);
    compo_setid(txt,COMPO_ID_UINT_SPORT_KCAL);

    if(func_sport_get_disp_mode() == MULTIPLE_DATA)
    {
        u8 km_integer = data->saveData.sportDistanceInteger;                 //距离 整数
        u8 km_decimals = data->saveData.sportDistanceDecimals;               //距离 小数

        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            uint16_t distance = km_integer*1000+km_decimals*10;
            distance = distance*0.6213712;
            km_integer  = distance/1000;
            km_decimals = distance%1000/10;
        }

        pic = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_DIS_BIN);///公里图片
        compo_picturebox_set_size(pic,24,24);
        compo_picturebox_set_pos(pic,22,140+38);

        txt = compo_textbox_create(frm, 8);///公里数据
        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_pos(txt, 45,128+38);
        compo_textbox_set_align_center(txt,false);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
        compo_textbox_set(txt, txt_buf);
        compo_setid(txt,COMPO_ID_NUM_SPORT_KM);

        txt_wid = widget_text_get_area(txt->txt);
        txt = compo_textbox_create(frm, strlen(i18n[STR_KM])+strlen(i18n[STR_MILE]));///公里单位
        compo_textbox_set_location(txt, txt_wid.wid+50,131+38,110,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,false);
        compo_setid(txt,COMPO_ID_UINT_SPORT_KM);
        if(uteModuleSystemtimeGetDistanceMiType()) //英里
        {
            compo_textbox_set(txt, i18n[STR_MILE]);
        }
        else
        {
            compo_textbox_set(txt, i18n[STR_KM]);
        }

        pic = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_STEP_BIN);///步数图片
        compo_picturebox_set_size(pic,24,24);
        compo_picturebox_set_pos(pic,22,140+38*2);

        txt = compo_textbox_create(frm, 8);///步数数据
        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_pos(txt, 45,128+38*2);
        compo_textbox_set_align_center(txt,false);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%ld", data->saveData.sportStep);
        compo_textbox_set(txt, txt_buf);
        compo_setid(txt,COMPO_ID_NUM_SPORT_STEP);

        txt_wid = widget_text_get_area(txt->txt);
        txt = compo_textbox_create(frm, strlen(i18n[STR_STEP]));///步数单位
        compo_textbox_set_location(txt, txt_wid.wid+50,131+38*2,110,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,false);
        compo_setid(txt,COMPO_ID_UINT_SPORT_STEP);
        compo_textbox_set(txt, i18n[STR_STEP]);


    }
    else if(func_sport_get_disp_mode() == MID_DATA)
    {
        pic = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_TIMES_BIN);///次数图片
        compo_picturebox_set_size(pic,24,24);
        compo_picturebox_set_pos(pic,22,140+38);

        txt = compo_textbox_create(frm, 8);///次数数据
        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_pos(txt, 45,128+38);
        compo_textbox_set_align_center(txt,false);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",data->saveData.sportTimes);
        compo_textbox_set(txt, txt_buf);
        compo_setid(txt,COMPO_ID_NUM_SPORT_COUNT);

        txt_wid = widget_text_get_area(txt->txt);
        txt = compo_textbox_create(frm, strlen(i18n[STR_SPORT_ORDER]));///次数单位
        compo_textbox_set_location(txt, txt_wid.wid+50,131+38,110,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,false);
        compo_setid(txt,COMPO_ID_UINT_SPORT_COUNT);
        compo_textbox_set(txt, i18n[STR_SPORT_ORDER]);
    }
    pic = compo_picturebox_create(frm, UI_BUF_I330001_SPORT_ICON2_HR_BIN);///心率图片
    compo_picturebox_set_size(pic,24,24);
    compo_picturebox_set_pos(pic,22,140+38*3);
    compo_setid(pic,COMPO_ID_PIC_SPORT_HEARTRATE);

    txt = compo_textbox_create(frm, 8);///心率数据
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(txt, 45,123+38*3);
    compo_textbox_set_align_center(txt,false);
    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleHeartGetHeartValue() > 0 && uteModuleHeartGetHeartValue() < 255)
    {
        snprintf(txt_buf, sizeof(txt_buf), "%d", uteModuleHeartGetHeartValue());
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");
    }
    compo_textbox_set(txt, txt_buf);
    compo_setid(txt,COMPO_ID_NUM_SPORT_HEARTRATE);

    txt_wid = widget_text_get_area(txt->txt);
    txt = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));///心率单位
    compo_textbox_set_location(txt, txt_wid.wid+50,133+38*3,110,widget_text_get_max_height());
    compo_textbox_set_align_center(txt,false);
    compo_setid(txt,COMPO_ID_UINT_SPORT_HEARTRATE);
    compo_textbox_set(txt, i18n[STR_PER_MINUTE]);

    //右滑退出界面
    compo_button_t* btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_PUSED_BIN);
    compo_button_set_pos(btn, -(GUI_SCREEN_CENTER_X), 62+68/2);
    compo_button_set_visible(btn, true);
    compo_setid(btn, COMPO_ID_BTN_SPORT_STOP);

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_CLOSE_BIN);
    compo_button_set_pos(btn, -(GUI_SCREEN_CENTER_X), 176+68/2);
    compo_button_set_visible(btn, true);
    compo_setid(btn, COMPO_ID_BTN_SPORT_EXIT);

    compo_textbox_t* text = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
    compo_textbox_set_location(text, -(GUI_SCREEN_CENTER_X), 134+23/2,
                               GUI_SCREEN_WIDTH/1.1,
                               gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).hei);
    compo_textbox_set(text, i18n[STR_PAUSE]);
    compo_setid(text,COMPO_ID_TXT_SPORT_STOP);

    text = compo_textbox_create(frm, 32);
    compo_textbox_set_location(text, -(GUI_SCREEN_CENTER_X), 248+23/2,
                               GUI_SCREEN_WIDTH/1.1,
                               gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).hei);
    compo_textbox_set(text, i18n[STR_END]);


    ab_free(data);

    return frm;
}

static void func_sport_sub_run_updata(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t *)func_cb.f_cb;
    if(tick_check_expire(f_sport_sub_run->updata_tick, 10))
    {
        f_sport_sub_run->updata_tick = tick_get();

        char txt_buf[50];
        compo_textbox_t* txt_time       = compo_getobj_byid(COMPO_ID_NUM_SPORT_TIME);
        compo_textbox_t* txt_heart      = compo_getobj_byid(COMPO_ID_NUM_SPORT_HEARTRATE);
        compo_textbox_t* txt_kcal       = compo_getobj_byid(COMPO_ID_NUM_SPORT_KCAL);
        compo_textbox_t* txt_step       = compo_getobj_byid(COMPO_ID_NUM_SPORT_STEP);
        compo_textbox_t* txt_km         = compo_getobj_byid(COMPO_ID_NUM_SPORT_KM);
        compo_textbox_t* txt_count      = compo_getobj_byid(COMPO_ID_NUM_SPORT_COUNT);
        compo_textbox_t* uint_heart      = compo_getobj_byid(COMPO_ID_UINT_SPORT_HEARTRATE);
        compo_textbox_t* uint_kcal       = compo_getobj_byid(COMPO_ID_UINT_SPORT_KCAL);
        compo_textbox_t* uint_step       = compo_getobj_byid(COMPO_ID_UINT_SPORT_STEP);
        compo_textbox_t* uint_km         = compo_getobj_byid(COMPO_ID_UINT_SPORT_KM);
        compo_textbox_t* uint_count      = compo_getobj_byid(COMPO_ID_UINT_SPORT_COUNT);
        compo_picturebox_t* pic_heart    = compo_getobj_byid(COMPO_ID_PIC_SPORT_HEARTRATE);
        compo_button_t* btn_stop         = compo_getobj_byid(COMPO_ID_BTN_SPORT_STOP);
        compo_button_t* btn_exit         = compo_getobj_byid(COMPO_ID_BTN_SPORT_EXIT);
        compo_textbox_t* txt_stop        = compo_getobj_byid(COMPO_ID_TXT_SPORT_STOP);
        area_t txt_wid;

        ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
        uteModuleSportGetMoreSportsDatas(data);

        if(btn_stop != NULL && txt_stop != NULL)
        {

            switch (uteModuleSportMoreSportGetStatus())
            {
                case ALL_SPORT_STATUS_CLOSE:
                case ALL_SPORT_STATUS_PAUSE:
                    compo_button_set_bgimg(btn_stop,UI_BUF_I330001_SPORT_BTN_PLAY_BIN);
                    compo_textbox_set(txt_stop, i18n[STR_CONTINUE]);
                    f_sport_sub_run->sport_run_state = SPORT_RUN_START;
                    break;

                case ALL_SPORT_STATUS_OPEN:
                case ALL_SPORT_STATUS_CONTINUE:
                    compo_button_set_bgimg(btn_stop,UI_BUF_I330001_SPORT_BTN_PUSED_BIN);
                    compo_textbox_set(txt_stop, i18n[STR_PAUSE]);
                    f_sport_sub_run->sport_run_state = SPORT_RUN_STOP;;
                    break;
            }

            if(uteModuleSportMoreSportIsAppStart())
            {
                if (ble_is_connect())
                {
                    compo_button_set_visible(btn_stop,true);
                    compo_textbox_set_pos(txt_stop, -(GUI_SCREEN_CENTER_X), 134+23/2);
                }
                else
                {
                    compo_button_set_visible(btn_stop,false);
                    compo_textbox_set(txt_stop, i18n[STR_CONNECT_BLUETOOTH]);
                    compo_textbox_set_pos(txt_stop, -(GUI_SCREEN_CENTER_X), 62+68/2);
                }
            }

            if(f_sport_sub_run->sport_run_state != f_sport_sub_run->sport_run_state_updata_flag)
            {
                // printf("motor_on\n");
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                f_sport_sub_run->sport_run_state_updata_flag = f_sport_sub_run->sport_run_state;

                if(f_sport_sub_run->sport_run_state == SPORT_RUN_STOP)
                {
                    f_sport_sub_run->page_old_y = 0;
                    f_sport_sub_run->move_offset = 0;
                    f_sport_sub_run->page_num = PAGE_1;
                }
                else if(f_sport_sub_run->sport_run_state == SPORT_RUN_START)
                {
                    f_sport_sub_run->page_old_y = GUI_SCREEN_WIDTH;
                    f_sport_sub_run->move_offset = GUI_SCREEN_WIDTH;
                    f_sport_sub_run->page_num = PAGE_2;
                }

                widget_page_set_client(func_cb.frm_main->page_body,f_sport_sub_run->move_offset, 0);
            }

            // printf("sport_state:%d\n",uteModuleSportMoreSportGetStatus());
        }


        if(txt_time != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",data->totalSportTime / 3600,((data->totalSportTime) % 3600) / 60,(data->totalSportTime) % 60);
            compo_textbox_set(txt_time, txt_buf);
        }
        if(txt_kcal != NULL && uint_kcal != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf, sizeof(txt_buf), "%d", data->saveData.sportCaloire);
            compo_textbox_set(txt_kcal, txt_buf);

            txt_wid = widget_text_get_area(txt_kcal->txt);
            compo_textbox_set_pos(uint_kcal, txt_wid.wid+50,131);
        }
        if(txt_km != NULL && uint_km != NULL)
        {
            u8 km_integer = data->saveData.sportDistanceInteger;                 //距离 整数
            u8 km_decimals = data->saveData.sportDistanceDecimals;               //距离 小数
            if(uteModuleSystemtimeGetDistanceMiType())//英里
            {
                uint16_t distance = km_integer*1000+km_decimals*10;
                distance = distance*0.6213712;
                km_integer  = distance/1000;
                km_decimals = distance%1000/10;
            }
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
            compo_textbox_set(txt_km, txt_buf);

            txt_wid = widget_text_get_area(txt_km->txt);
            compo_textbox_set_pos(uint_km,txt_wid.wid+50,131+38);
            if(uteModuleSystemtimeGetDistanceMiType()!=f_sport_sub_run->sport_run_km_uint_updata_flag)
            {
                f_sport_sub_run->sport_run_km_uint_updata_flag = uteModuleSystemtimeGetDistanceMiType();
                if(f_sport_sub_run->sport_run_km_uint_updata_flag)
                {
                    compo_textbox_set(uint_km, i18n[STR_MILE]);
                }
                else
                {
                    compo_textbox_set(uint_km, i18n[STR_KM]);
                }
            }
        }
        if(txt_step != NULL && uint_step != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%ld", data->saveData.sportStep);
            compo_textbox_set(txt_step, txt_buf);

            txt_wid = widget_text_get_area(txt_step->txt);
            compo_textbox_set_pos(uint_step,txt_wid.wid+50,131+38*2);
        }
        if(txt_heart != NULL && uint_heart != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            if(uteModuleHeartGetHeartValue() > 0 && uteModuleHeartGetHeartValue() < 255)
            {
                snprintf(txt_buf, sizeof(txt_buf), "%d",uteModuleHeartGetHeartValue());
            }
            else
            {
                snprintf(txt_buf, sizeof(txt_buf), "--");
            }

            compo_textbox_set(txt_heart, txt_buf);
            txt_wid = widget_text_get_area(txt_heart->txt);
            compo_textbox_set_pos(uint_heart,txt_wid.wid+50,131+38*3);
        }
        if(txt_count != NULL && uint_count != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",data->saveData.sportTimes);
            compo_textbox_set(txt_count, txt_buf);

            txt_wid = widget_text_get_area(txt_count->txt);
            compo_textbox_set_pos(uint_count,txt_wid.wid+50,131+38);
        }
        if(uteModuleHeartIsWear() == true && pic_heart != NULL)   ///佩戴处理
        {
            if(f_sport_sub_run->heart_pic_size_add_flag == false)
            {
                if(--f_sport_sub_run->heart_pic_size_perc <= 50)
                {
                    f_sport_sub_run->heart_pic_size_add_flag = true;
                }
            }
            else
            {
                if(++f_sport_sub_run->heart_pic_size_perc >= 100)
                {
                    f_sport_sub_run->heart_pic_size_add_flag = false;
                }
            }
            area_t pic_size = gui_image_get_size(UI_BUF_I330001_SPORT_ICON2_HR_BIN);
            compo_picturebox_set_size(pic_heart,pic_size.wid*f_sport_sub_run->heart_pic_size_perc/100,pic_size.hei*f_sport_sub_run->heart_pic_size_perc/100);
        }

        ab_free(data);
    }
}

static void func_sport_sub_run_move_handle(void)
{
#define   PAGE_TWO_SIZE  GUI_SCREEN_WIDTH  //最底x轴
#define   TOYCH_LAST_DY  40   //切换页滑动y
#define   TICK_TIME      8   //步进y像素点时间
#define   STEP_NUM       8    //步进y像素点
    f_sport_sub_run_t *f_sleep = (f_sport_sub_run_t *)func_cb.f_cb;

    if(f_sleep->page_num == PAGE_1)//第一页
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = dx;
            if(f_sleep->move_offset < 0)
            {
                f_sleep->move_offset = 0;
            }
            widget_page_set_client(func_cb.frm_main->page_body, f_sleep->move_offset,0);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset >= (TOYCH_LAST_DY))//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->tick = tick_get();
                        f_sleep->move_offset+=STEP_NUM;

                        if(f_sleep->move_offset >= GUI_SCREEN_WIDTH)
                        {
                            uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_PAUSE);
                            f_sleep->sport_run_state = SPORT_RUN_STOP;
                            f_sleep->move_offset = GUI_SCREEN_WIDTH;
                            f_sleep->page_num = PAGE_2;//第2页
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                            f_sleep->page_old_y = f_sleep->move_offset;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->move_offset-=STEP_NUM;

                        if(f_sleep->move_offset <= 0)
                        {
                            f_sleep->move_offset = 0;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                }
                widget_page_set_client(func_cb.frm_main->page_body,f_sleep->move_offset,0);
            }
        }

    }
    else if(f_sleep->page_num == PAGE_2)
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = f_sleep->page_old_y+dx;
            if(f_sleep->move_offset > PAGE_TWO_SIZE)
            {
                f_sleep->move_offset = PAGE_TWO_SIZE;
            }
            widget_page_set_client(func_cb.frm_main->page_body, f_sleep->move_offset,0);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset <= (GUI_SCREEN_WIDTH-TOYCH_LAST_DY))//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->move_offset-=STEP_NUM;

                        if(f_sleep->move_offset <= 0)
                        {
                            uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
                            f_sleep->sport_run_state = SPORT_RUN_START;
                            f_sleep->move_offset = 0;
                            f_sleep->page_num = PAGE_1;//第2页
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->move_offset+=STEP_NUM;

                        if(f_sleep->move_offset >= PAGE_TWO_SIZE)
                        {
                            f_sleep->move_offset = PAGE_TWO_SIZE;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                }
                f_sleep->page_old_y = f_sleep->move_offset;
                widget_page_set_client(func_cb.frm_main->page_body, f_sleep->move_offset,0);
            }
        }
    }
}
static void func_sport_sub_run_init(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    f_sport_sub_run->heart_pic_size_perc = 100;
    sys_cb.sport_idx = uteModuleSportMoreSportGetType();
    if(sport_start_flag == false)///是否正常进入运动
    {
        sport_start_flag = true;
    }
    f_sport_sub_run->sport_run_state = SPORT_RUN_STOP;
    f_sport_sub_run->sport_run_state_updata_flag = SPORT_RUN_STOP;
    f_sport_sub_run->sport_run_km_uint_updata_flag = uteModuleSystemtimeGetDistanceMiType();
    func_cb.frm_main = func_sport_sub_run_form_create();
}
static void func_sport_sub_run_exit_data(void)
{
    if(sys_cb.refresh_language_flag == false || sport_start_flag == true)//刷新语言时不清除数据
    {
        uteModuleGuiCommonDisplayOffAllowGoBack(true);
    }
}


//按键处理
static void func_sport_sub_run_click_handler(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    if(f_sport_sub_run->touch_state == AUTO_STATE)
    {
        return;
    }
    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_SPORT_STOP:
            f_sport_sub_run->page_num = PAGE_1;
            f_sport_sub_run->page_old_y = 0;
            f_sport_sub_run->move_offset = 0;

            widget_page_set_client(func_cb.frm_main->page_body,f_sport_sub_run->move_offset, 0);
            f_sport_sub_run->sport_run_state = SPORT_RUN_START;
            uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
            break;
        case COMPO_ID_BTN_SPORT_EXIT:
        {
            int res=0;
            bool sport_flag = uteModuleSportMoreSportsIsLessData();
            // sport_flag ^= 1;
            if (sport_flag)
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG2], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_SPORT);
            }
            else
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG1], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_NONE);
            }
            if (res == MSGBOX_RES_OK)
            {
                uteModuleSportStopMoreSports();                             //通知APP退出运动
            }
        }
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};
static bool sport_start_flag = false;
extern u32 func_sport_get_disp_mode(void);//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);
enum
{
    COMPO_ID_NUM_SPORT_TIME = 1,    //运动时间
    COMPO_ID_NUM_SPORT_HEARTRATE,   //心率数据
    COMPO_ID_NUM_SPORT_KCAL,        //卡路里数据
    COMPO_ID_NUM_SPORT_TIME_DOWN,   //运动时间
    COMPO_ID_NUM_SPORT_STEP,        //计步数据
    COMPO_ID_NUM_SPORT_KM,          //距离数据
    COMPO_ID_PIC_SPORT_STEP,        //计步图片
    COMPO_ID_PIC_SPORT_KM,          //距离图片
    COMPO_ID_UINT_SPORT_KM,         //距离单位
    COMPO_ID_NUM_SPORT_COUNT,       //计次数据
    COMPO_ID_PIC_SPORT_COUNT,       //计次图片
    COMPO_ID_NUM_SPORT_TIME_RIGHT,  //运动时间
    COMPO_ID_BTN_SPORT_PALY,        //暂停开始
    COMPO_ID_BTN_SPORT_EXIT,        //结束退出
    COMPO_ID_PIC_SPORT_TITLE,       //运动类型图片
    COMPO_ID_SPOT_SPORT_SHAPE1,      //页码图片
    COMPO_ID_SPOT_SPORT_SHAPE2,      //页码图片
};

typedef struct f_sport_sub_run_t_
{
    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    s32         move_offset_x;
    s32         page_old_x;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
    u8          direction;
    uint32_t    updata_tick;
    bool page2_move_flag;
} f_sport_sub_run_t;
enum
{
    TOUCH_NULL=0,
    UP_DOWM_DIR,
    LEFT_RIGHT_DIR,
};

enum
{
    TOUCH_FINISH_STATE=0,
    AUTO_STATE,

};
enum
{
    PAGE_1=0,
    PAGE_2,
    PAGE_3,
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
};
//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_sub_run_form_create(void)
{
    char txt_buf[50];
    ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
    uteModuleSportGetMoreSportsDatas(data);
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t* pic = compo_picturebox_create(frm, func_sport_get_ui(sys_cb.sport_idx));///运动类型图片
    compo_picturebox_set_size(pic,70,70);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,70/2+20);

    compo_textbox_t* txt = compo_textbox_create(frm, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 58/2+100, 240, 60);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(uint16_t)data->totalSportTime / 3600,(uint16_t)((data->totalSportTime) % 3600) / 60,(uint16_t)(data->totalSportTime) % 60);
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_forecolor(txt, make_color(0xa9,0xff,0x00));
    compo_setid(txt,COMPO_ID_NUM_SPORT_TIME);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_HEART_RATE],i18n[STR_PER_MINUTE]);
    txt = compo_textbox_create(frm, strlen(txt_buf));///心率文本
    compo_textbox_set_location(txt, 154/2+32, 22/2+184, 154, 30);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
    compo_textbox_set(txt, txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_CALORIE],i18n[STR_KCAL]);
    txt = compo_textbox_create(frm, strlen(txt_buf));///卡路里文本
    compo_textbox_set_location(txt, 132/2+186, 22/2+184, 148, 30);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
    compo_textbox_set(txt, txt_buf);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///灰色矩形
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+222, 296, 48);
    compo_shape_set_radius(shape, 48/2);
    compo_shape_set_color(shape, make_color(0x29,0x29,0x29));

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///白色矩形
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+222, 2, 48);
    compo_shape_set_color(shape, make_color(0x54,0x54,0x54));

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_HR_BIN);///心率图片
    compo_picturebox_set_pos(pic,30/2+56,48/2+222);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_CALORIES_BIN);///卡路里图片
    compo_picturebox_set_pos(pic,30/2+205,48/2+222);

    txt = compo_textbox_create(frm, 3);///心率数据文本
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(txt, 53/2+92-16, 232, 60, 43);
    compo_textbox_set_align_center(txt, false);
    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", uteModuleHeartGetHeartValue());
    compo_textbox_set(txt, txt_buf);
    compo_setid(txt,COMPO_ID_NUM_SPORT_HEARTRATE);

    txt = compo_textbox_create(frm, 3);///卡路里数据文本
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(txt, 53/2+245-16, 232, 60, 43);
    compo_textbox_set_align_center(txt, false);
    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", data->saveData.sportCaloire);
    compo_textbox_set(txt, txt_buf);
    compo_setid(txt,COMPO_ID_NUM_SPORT_KCAL);

    ///*右页*/
    pic = compo_picturebox_create_for_page(frm,frm->page,func_sport_get_ui(sys_cb.sport_idx));///运动类型图片
    compo_picturebox_set_size(pic,70,70);
    compo_picturebox_set_pos(pic,-GUI_SCREEN_CENTER_X,70/2+20);
    compo_setid(pic,COMPO_ID_PIC_SPORT_TITLE);

    txt = compo_textbox_create_for_page(frm,frm->page, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt, -GUI_SCREEN_CENTER_X, 58/2+126, 240, 60);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(u16)(data->totalSportTime / 3600),(u16)(((data->totalSportTime) % 3600) / 60),(u16)((data->totalSportTime) % 60));
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_forecolor(txt, make_color(0xa9,0xff,0x00));
    compo_setid(txt,COMPO_ID_NUM_SPORT_TIME_RIGHT);

    compo_button_t * btn = compo_button_create_page_by_image(frm,frm->page,UI_BUF_I332001_SPORT_BTN_PUSED_BIN);
    compo_setid(btn,COMPO_ID_BTN_SPORT_PALY);
    compo_button_set_pos(btn,80/2+68-GUI_SCREEN_WIDTH,80/2+234);

    btn =compo_button_create_page_by_image(frm,frm->page,UI_BUF_I332001_SPORT_BTN_CLOSE_BIN);
    compo_setid(btn,COMPO_ID_BTN_SPORT_EXIT);
    compo_button_set_pos(btn,80/2+212-GUI_SCREEN_WIDTH,80/2+234);

    ///*下页*/
    if(func_sport_get_disp_mode()==LESS_DATA)
    {
        ab_free(data);
        return frm;
    }

    if(func_cb.sta == FUNC_SPORT_SUB_RUN)
    {
        f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;

        f_sport_sub_run->page2_move_flag = true;
    }
    widget_page_t *page = widget_page_create(frm->page);///创建页码页面
    widget_page_set_client(page, 0, 0);
    widget_set_location(page,12/2+340, 30/2+165,12,30);

    shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, 6, 6, 12, 12);
    compo_setid(shape,COMPO_ID_SPOT_SPORT_SHAPE1);
    compo_shape_set_color(shape, COLOR_WHITE);
    compo_shape_set_radius(shape, 6);

    shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, 6, 30-6, 12, 12);
    compo_setid(shape,COMPO_ID_SPOT_SPORT_SHAPE2);
    compo_shape_set_color(shape, make_color(0x29,0x29,0x29));
    compo_shape_set_radius(shape, 6);

    txt = compo_textbox_create(frm, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 58/2+26+360, 180, 60);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(uint16_t)data->totalSportTime / 3600,(uint16_t)((data->totalSportTime) % 3600) / 60,(uint16_t)(data->totalSportTime) % 60);
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_forecolor(txt, make_color(0xa9,0xff,0x00));
    compo_setid(txt,COMPO_ID_NUM_SPORT_TIME_DOWN);

    if(func_sport_get_disp_mode()==MULTIPLE_DATA)
    {
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_SET_DISTANCE],uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_SET_DISTANCE] : i18n[STR_KM]);
        txt = compo_textbox_create(frm, strlen(txt_buf));///公里文本
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 24/2+96+360, 240, 30);
        compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
        compo_textbox_set(txt, txt_buf);
        compo_setid(txt,COMPO_ID_UINT_SPORT_KM);

        shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///灰色矩形
        compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+130+360, 296, 48);
        compo_shape_set_radius(shape, 48/2);
        compo_shape_set_color(shape, make_color(0x29,0x29,0x29));

        txt = compo_textbox_create(frm, 6);///公里数据文本
        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_pos(txt, 43/2+167, 48/2+130+360);
        u8 km_integer = data->saveData.sportDistanceInteger;                 //距离 整数
        u8 km_decimals = data->saveData.sportDistanceDecimals;               //距离 小数
        if(uteModuleSystemtimeGetDistanceMiType())//英里
        {
            uint16_t distance = km_integer*1000+km_decimals*10;
            distance = distance*0.6213712;
            km_integer  = distance/1000;
            km_decimals = distance%1000/10;
        }
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
        compo_textbox_set(txt, txt_buf);
        compo_textbox_set(txt, txt_buf);
        compo_setid(txt,COMPO_ID_NUM_SPORT_KM);

        pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_DIS_BIN);///公里图片
        compo_picturebox_set_pos(pic,43/2+167-30-widget_text_get_area(txt->txt).wid/2,48/2+130+360);
        compo_setid(pic,COMPO_ID_PIC_SPORT_KM);

        txt = compo_textbox_create(frm, strlen(i18n[STR_STEPS]));///步数文本
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 40/2+190+360, 240, 30);
        compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
        compo_textbox_set(txt, i18n[STR_STEPS]);

        shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///灰色矩形
        compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+232+360, 296, 48);
        compo_shape_set_radius(shape, 48/2);
        compo_shape_set_color(shape, make_color(0x29,0x29,0x29));

        txt = compo_textbox_create(frm, 6);///步数数据文本
        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_pos(txt, 43/2+167, 48/2+232+360);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d", data->saveData.sportStep);
        compo_textbox_set(txt, txt_buf);
        compo_setid(txt,COMPO_ID_NUM_SPORT_STEP);

        pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_STEP_BIN);///步数图片
        compo_picturebox_set_pos(pic,43/2+167-30-widget_text_get_area(txt->txt).wid/2,48/2+232+360);
        compo_setid(pic,COMPO_ID_PIC_SPORT_STEP);
    }
    else if(func_sport_get_disp_mode()==MID_DATA)
    {
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_COUNT_NUM],i18n[STR_SPORT_ORDER]);
        txt = compo_textbox_create(frm, strlen(txt_buf));///计数文本
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 24/2+96+360, 240, 30);
        compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
        compo_textbox_set(txt, txt_buf);

        shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);///灰色矩形
        compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 48/2+130+360, 296, 48);
        compo_shape_set_radius(shape, 48/2);
        compo_shape_set_color(shape, make_color(0x29,0x29,0x29));

        txt = compo_textbox_create(frm, 6);///计数数据文本
        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_pos(txt, 43/2+167, 48/2+130+360);
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",  data->saveData.sportTimes);
        compo_textbox_set(txt, txt_buf);
        compo_setid(txt,COMPO_ID_NUM_SPORT_COUNT);

        pic = compo_picturebox_create(frm, UI_BUF_I332001_SPORT_ICON2_TIMES_BIN);///计数图片
        compo_picturebox_set_pos(pic,43/2+167-30-widget_text_get_area(txt->txt).wid/2,48/2+130+360);
        compo_setid(pic,COMPO_ID_PIC_SPORT_COUNT);
    }
    ab_free(data);
    return frm;
}
static void func_soprt_run_move(void)
{
#define   PAGE_TWO_SIZE  360  //最底y轴
#define   TOYCH_LAST_DY  40   //切换页滑动y
#define   TICK_TIME      8   //步进y像素点时间
#define   STEP_NUM       8    //步进y像素点
    f_sport_sub_run_t *f_sleep = (f_sport_sub_run_t *)func_cb.f_cb;


    if(f_sleep->page_num == PAGE_1)//第一页
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            if(f_sleep->direction == UP_DOWM_DIR)
            {
                f_sleep->move_offset   = dy;
            }
            else if(f_sleep->direction == LEFT_RIGHT_DIR)
            {
                f_sleep->move_offset_x = dx;
            }

            if(f_sleep->move_offset > 0)
            {
                f_sleep->move_offset = 0;
            }
            else if(f_sleep->move_offset_x < 0)
            {
                f_sleep->move_offset_x = 0;
            }

            if(f_sleep->direction == UP_DOWM_DIR)
            {
                widget_page_set_client(func_cb.frm_main->page_body, f_sleep->move_offset_x, f_sleep->move_offset);
            }
            else if(f_sleep->direction == LEFT_RIGHT_DIR)
            {
                widget_page_set_client(func_cb.frm_main->page, f_sleep->move_offset_x, f_sleep->move_offset);
            }


            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset <= (-TOYCH_LAST_DY) || f_sleep->move_offset_x >= TOYCH_LAST_DY)//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->direction == UP_DOWM_DIR)
                    {
                        if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                        {
                            f_sleep->move_offset-=STEP_NUM;

                            if(f_sleep->move_offset <= -GUI_SCREEN_HEIGHT)
                            {
                                f_sleep->move_offset = -GUI_SCREEN_HEIGHT;
                                f_sleep->page_num = PAGE_2;//第2页
                                f_sleep->touch_state = TOUCH_FINISH_STATE;
                            }
                        }
                        else if(f_sleep->switch_page_state == SWITCH_NO)
                        {
                            f_sleep->move_offset+=STEP_NUM;

                            if(f_sleep->move_offset >= 0)
                            {
                                f_sleep->move_offset = 0;
                                f_sleep->touch_state = TOUCH_FINISH_STATE;
                            }
                        }
                    }
                    else if(f_sleep->direction == LEFT_RIGHT_DIR)
                    {
                        if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                        {
                            f_sleep->move_offset_x +=STEP_NUM;

                            if(f_sleep->move_offset_x >= GUI_SCREEN_HEIGHT)
                            {
                                f_sleep->move_offset_x = GUI_SCREEN_HEIGHT;
                                f_sleep->page_num = PAGE_3;//第2页
                                f_sleep->touch_state = TOUCH_FINISH_STATE;
                            }
                        }
                        else if(f_sleep->switch_page_state == SWITCH_NO)
                        {
                            f_sleep->move_offset_x-=STEP_NUM;

                            if(f_sleep->move_offset_x <= 0)
                            {
                                f_sleep->move_offset_x = 0;
                                f_sleep->touch_state = TOUCH_FINISH_STATE;
                            }
                        }
                    }
                    f_sleep->page_old_x = f_sleep->move_offset_x;
                    f_sleep->page_old_y = f_sleep->move_offset;
                }
                if(f_sleep->direction == UP_DOWM_DIR)
                {
                    widget_page_set_client(func_cb.frm_main->page_body, f_sleep->move_offset_x, f_sleep->move_offset);
                }
                else if(f_sleep->direction == LEFT_RIGHT_DIR)
                {
                    widget_page_set_client(func_cb.frm_main->page, f_sleep->move_offset_x, f_sleep->move_offset);
                }
                f_sleep->page_old_x = f_sleep->move_offset_x;
                if(f_sleep->touch_state == TOUCH_FINISH_STATE)
                {
                    f_sleep->direction = TOUCH_NULL;
                }
            }
        }

    }
    else if(f_sleep->page_num == PAGE_2)
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = f_sleep->page_old_y+dy;
            if(f_sleep->move_offset < -PAGE_TWO_SIZE)
            {
                f_sleep->move_offset = -PAGE_TWO_SIZE;
            }
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset >= -(GUI_SCREEN_HEIGHT-TOYCH_LAST_DY))//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->move_offset+=STEP_NUM;

                        if(f_sleep->move_offset >= 0)
                        {
                            f_sleep->move_offset = 0;
                            f_sleep->page_num = PAGE_1;//第2页
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->move_offset-=STEP_NUM;

                        if(f_sleep->move_offset <= -(GUI_SCREEN_HEIGHT))
                        {
                            f_sleep->move_offset = -GUI_SCREEN_HEIGHT;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                }
                f_sleep->page_old_y = f_sleep->move_offset;
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);
                if(f_sleep->touch_state == TOUCH_FINISH_STATE)
                {
                    f_sleep->direction = TOUCH_NULL;
                }
            }
        }
    }
    else if(f_sleep->page_num == PAGE_3)//第三页
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            if(f_sleep->direction == LEFT_RIGHT_DIR)
            {
                f_sleep->move_offset_x = f_sleep->page_old_x+dx;
            }

            if(f_sleep->move_offset_x > 360)
            {
                f_sleep->move_offset_x = 360;
            }

            if(f_sleep->direction == LEFT_RIGHT_DIR)
            {
                widget_page_set_client(func_cb.frm_main->page, f_sleep->move_offset_x, 0);
            }


            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset_x <= 360-TOYCH_LAST_DY)//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->move_offset_x -=STEP_NUM;

                        if(f_sleep->move_offset_x <= 0)
                        {
                            f_sleep->page_num = PAGE_1;//第1页
                            f_sleep->move_offset_x = 0;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->move_offset_x+=STEP_NUM;

                        if(f_sleep->move_offset_x >= 360)
                        {
                            f_sleep->move_offset_x = 360;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                }

                widget_page_set_client(func_cb.frm_main->page, f_sleep->move_offset_x, 0);
                f_sleep->page_old_x = f_sleep->move_offset_x;
                if(f_sleep->touch_state == TOUCH_FINISH_STATE)
                {
                    f_sleep->direction = TOUCH_NULL;
                }
            }
        }
        // printf("px:%d py:%d\n",f_sleep->move_offset_x,f_sleep->move_offset);
    }


}

static void func_sport_sub_run_updata(void)
{

    f_sport_sub_run_t *f_sleep = (f_sport_sub_run_t *)func_cb.f_cb;
    if(tick_check_expire(f_sleep->updata_tick, 200))
    {
        f_sleep->updata_tick = tick_get();
        char txt_buf[50];
        compo_textbox_t* txt_time       = compo_getobj_byid(COMPO_ID_NUM_SPORT_TIME);
        compo_textbox_t* txt_heart      = compo_getobj_byid(COMPO_ID_NUM_SPORT_HEARTRATE);
        compo_textbox_t* txt_kcal       = compo_getobj_byid(COMPO_ID_NUM_SPORT_KCAL);
        compo_textbox_t* txt_step       = compo_getobj_byid(COMPO_ID_NUM_SPORT_STEP);
        compo_textbox_t* txt_km         = compo_getobj_byid(COMPO_ID_NUM_SPORT_KM);
        compo_textbox_t* uint_km        = compo_getobj_byid(COMPO_ID_UINT_SPORT_KM);
        compo_textbox_t* txt_count      = compo_getobj_byid(COMPO_ID_NUM_SPORT_COUNT);
        compo_picturebox_t* pic_km      = compo_getobj_byid(COMPO_ID_PIC_SPORT_KM);
        compo_picturebox_t* pic_step    = compo_getobj_byid(COMPO_ID_PIC_SPORT_STEP);
        compo_picturebox_t* pic_count   = compo_getobj_byid(COMPO_ID_PIC_SPORT_COUNT);
        compo_textbox_t* txt_time_down  = compo_getobj_byid(COMPO_ID_NUM_SPORT_TIME_DOWN);
        compo_textbox_t* txt_time_right = compo_getobj_byid(COMPO_ID_NUM_SPORT_TIME_RIGHT);
        compo_shape_t * page_point1     = compo_getobj_byid(COMPO_ID_SPOT_SPORT_SHAPE1);
        compo_shape_t * page_point2     = compo_getobj_byid(COMPO_ID_SPOT_SPORT_SHAPE2);
        compo_button_t * btn_play       = compo_getobj_byid(COMPO_ID_BTN_SPORT_PALY);

        ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
        uteModuleSportGetMoreSportsDatas(data);

        if(btn_play != NULL)
        {
            if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_OPEN)
            {
                compo_button_set_bgimg(btn_play,UI_BUF_I332001_SPORT_BTN_PUSED_BIN);
            }
            else if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_PAUSE)
            {
                compo_button_set_bgimg(btn_play,UI_BUF_I332001_SPORT_BTN_PLAY_BIN);
            }
        }


        if(txt_time != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(uint16_t)data->totalSportTime / 3600,(uint16_t)((data->totalSportTime) % 3600) / 60,(uint16_t)(data->totalSportTime) % 60);
            compo_textbox_set(txt_time, txt_buf);
        }

        if(txt_heart != NULL)
        {
            memset(txt_buf, 0, sizeof(txt_buf));
            snprintf(txt_buf, sizeof(txt_buf), "%d", uteModuleHeartGetHeartValue());
            compo_textbox_set(txt_heart, txt_buf);
        }

        if(txt_kcal != NULL)
        {
            memset(txt_buf, 0, sizeof(txt_buf));
            snprintf(txt_buf, sizeof(txt_buf), "%d", data->saveData.sportCaloire);
            compo_textbox_set(txt_kcal, txt_buf);
        }

        if(txt_step != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d", data->saveData.sportStep);
            compo_textbox_set(txt_step, txt_buf);
        }

        if(txt_km != NULL)
        {
            u8 km_integer = data->saveData.sportDistanceInteger;                 //距离 整数
            u8 km_decimals = data->saveData.sportDistanceDecimals;               //距离 小数
            if(uteModuleSystemtimeGetDistanceMiType())//英里
            {
                uint16_t distance = km_integer*1000+km_decimals*10;
                distance = distance*0.6213712;
                km_integer  = distance/1000;
                km_decimals = distance%1000/10;
            }
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",km_integer,km_decimals);
            compo_textbox_set(txt_km, txt_buf);
        }
        if(uint_km != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_SET_DISTANCE],uteModuleSystemtimeGetDistanceMiType() ? i18n[STR_SET_DISTANCE] : i18n[STR_KM]);
            compo_textbox_set(uint_km, txt_buf);
        }

        if(txt_count != NULL)
        {
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",  data->saveData.sportTimes);
            compo_textbox_set(txt_count, txt_buf);
        }

        if(pic_km != NULL)
        {
            compo_picturebox_set_pos(pic_km,43/2+167-30-widget_text_get_area(txt_km->txt).wid/2,48/2+130+360);
        }

        if(pic_step != NULL)
        {
            compo_picturebox_set_pos(pic_step,43/2+167-30-widget_text_get_area(txt_step->txt).wid/2,48/2+232+360);
        }

        if(pic_count != NULL)
        {
            compo_picturebox_set_pos(pic_count,43/2+167-30-widget_text_get_area(txt_count->txt).wid/2,48/2+130+360);
        }

        if(txt_time_down != NULL)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(uint16_t)data->totalSportTime / 3600,(uint16_t)((data->totalSportTime) % 3600) / 60,(uint16_t)(data->totalSportTime) % 60);
            compo_textbox_set(txt_time_down, txt_buf);
        }

        if(txt_time_right != NULL)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(uint16_t)data->totalSportTime / 3600,(uint16_t)((data->totalSportTime) % 3600) / 60,(uint16_t)(data->totalSportTime) % 60);
            compo_textbox_set(txt_time_right, txt_buf);
        }

        if(page_point1 != NULL)
        {
            if(f_sleep->page_num == PAGE_1)
            {
                compo_shape_set_color(page_point1,COLOR_WHITE);
            }
            else
            {
                compo_shape_set_color(page_point1, make_color(0x29,0x29,0x29));
            }

        }

        if(page_point2 != NULL)
        {
            if(f_sleep->page_num == PAGE_2)
            {
                compo_shape_set_color(page_point2,COLOR_WHITE);
            }
            else
            {
                compo_shape_set_color(page_point2, make_color(0x29,0x29,0x29));
            }
        }

        ab_free(data);
    }
}
//按键处理
static void func_sport_sub_run_click_handler(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SPORT_PALY:
            if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_OPEN)
            {
                uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_PAUSE);
            }
            else if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_PAUSE)
            {
                uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
            }
            break;
        case COMPO_ID_BTN_SPORT_EXIT:
        {
            int res=0;
            bool sport_flag = uteModuleSportMoreSportsIsLessData();
            // sport_flag ^= 1;
            if (sport_flag)
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG2], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_SPORT);
            }
            else
            {
                res = msgbox(i18n[STR_SPORT_EXIT_MSG1], NULL, NULL, MSGBOX_MODE_BTN_YESNO, MSGBOX_MSG_TYPE_NONE);
            }

            if (res == MSGBOX_RES_OK)
            {
                uteModuleSportStopMoreSports();                             //通知APP退出运动
                if (!sport_flag)
                {
                    func_cb.sta = FUNC_SPORT_FINISH;
                }
                sport_start_flag = false;
                if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
                {
                    task_stack_pop();
                }
            }
        }
        break;
        default:
            break;
    }
}
static void func_sport_sub_run_init(void)
{
    if(sport_start_flag == false)///是否正常进入运动
    {
        sport_start_flag = true;
        uteModuleSportSetCountZeroIndex(0);
        uteModuleHeartStartSingleTesting(TYPE_HEART);
    }

    func_cb.frm_main = func_sport_sub_run_form_create();
}
static void func_sport_sub_run_exit_data(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;

    if(sys_cb.refresh_language_flag == false || sport_start_flag == true)//刷新语言时不清除数据
    {
        uteModuleHeartStopSingleTesting(TYPE_HEART);
        uteModuleGuiCommonDisplayOffAllowGoBack(true);
        if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
        {
            task_stack_pop();
        }
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    }

}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
enum
{
    COMPO_ID_NUM_SPORT_TIME = 1,    //运动时间
    COMPO_ID_NUM_SPORT_HEARTRATE,   //心率
    COMPO_ID_NUM_SPORT_KCAL,        //卡路里
    COMPO_ID_NUM_SPORT_STEP,        //计步
    COMPO_ID_NUM_SPORT_KM,          //距离
    COMPO_ID_NUM_SPORT_COUNT,       //计次

    COMPO_ID_UINT_SPORT_TIME,        //运动时间
    COMPO_ID_UINT_SPORT_HEARTRATE,   //心率
    COMPO_ID_UINT_SPORT_KCAL,        //卡路里
    COMPO_ID_UINT_SPORT_STEP,        //计步
    COMPO_ID_UINT_SPORT_KM,          //距离
    COMPO_ID_UINT_SPORT_COUNT,       //计次

    COMPO_ID_PIC_SPORT_HEARTRATE,                        //心率图片
    COMPO_ID_PIC_SPORT_HEARTRATE_PERCENTAGE,             //心率指针百分比

    COMPO_ID_BTN_SPORT_STOP,         //暂停
    COMPO_ID_BTN_SPORT_EXIT,         //退出

    COMPO_ID_TXT_SPORT_STOP,         //暂停

};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

typedef struct f_sport_sub_run_t_
{
    u8 heart_pic_size_perc;
    bool heart_pic_size_add_flag;
    u32 updata_tick;

    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
    bool        sport_run_state;
    bool        sport_run_state_updata_flag;
    bool        sport_run_km_uint_updata_flag;
} f_sport_sub_run_t;

enum
{
    TOUCH_FINISH_STATE=0,
    AUTO_STATE,

};
enum
{
    PAGE_1=0,
    PAGE_2,
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
    TOTCH_MOVE,
};
enum
{
    SPORT_RUN_STOP=false,
    SPORT_RUN_START=true,
};
static bool sport_start_flag = false;
extern u32 func_sport_get_disp_mode(void);//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);
//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_sub_run_form_create(void)
{
    char txt_buf[50];
    ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
    uteModuleSportGetMoreSportsDatas(data);
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_title(frm, i18n[func_sport_get_str(sys_cb.sport_idx)]);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);

    compo_textbox_t* txt = compo_textbox_create(frm, 8);///运动时长
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X,78, 240, 60);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d:%02d",(uint16_t)data->totalSportTime / 3600,(uint16_t)((data->totalSportTime) % 3600) / 60,(uint16_t)(data->totalSportTime) % 60);
    compo_textbox_set(txt, txt_buf);
    compo_setid(txt,COMPO_ID_NUM_SPORT_TIME);

    compo_picturebox_t* pic = compo_picturebox_create(frm, UI_BUF_I335001_5_EXERCISING_HEART_ICON_PIC26X22_X16_Y134_00_BIN);///心率图片
    compo_picturebox_set_pos(pic,26/2+16,22/2+134);
    compo_setid(pic,COMPO_ID_PIC_SPORT_HEARTRATE);

    txt = compo_textbox_create(frm, 3);///心率数据文本
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_location(txt, 26+16+8,136, 60, 43);
    compo_textbox_set_align_center(txt, false);
    memset(txt_buf, 0, sizeof(txt_buf));
    snprintf(txt_buf, sizeof(txt_buf), "%d", uteModuleHeartGetHeartValue());
    compo_textbox_set(txt, txt_buf);
    compo_setid(txt,COMPO_ID_NUM_SPORT_HEARTRATE);

    area_t leng_size = widget_text_get_area(txt->txt);
    txt = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));///心率文本
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, 32+16+8+leng_size.wid,136, 130, 43);
    compo_textbox_set(txt, i18n[STR_HEART_RATE]);
    compo_setid(pic,COMPO_ID_UINT_SPORT_HEARTRATE);

    pic = compo_picturebox_create(frm, UI_BUF_I335001_5_EXERCISING_HEART_PROGRESS_ICON_PIC220X16_X10_Y168_00_BIN);///心率进度
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,176);

    pic = compo_picturebox_create(frm, UI_BUF_I335001_5_EXERCISING_ARROW_14X11_X26_X70_X114_X158_X200_Y183_BIN);///心率进度指针
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X+(44*(1-3)),176+15);
    compo_setid(pic,COMPO_ID_PIC_SPORT_HEARTRATE);



    ab_free(data);
    return frm;
}
static void func_sport_sub_run_click_handler(void)
{
}
static void func_sport_sub_run_updata(void)
{
}
static void func_sport_sub_run_init(void)
{
    if(sport_start_flag == false)///是否正常进入运动
    {
        sport_start_flag = true;
        uteModuleSportSetCountZeroIndex(0);
        uteModuleHeartStartSingleTesting(TYPE_HEART);
    }

    func_cb.frm_main = func_sport_sub_run_form_create();
}
static void func_sport_sub_run_exit_data(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;

    if(sys_cb.refresh_language_flag == false || sport_start_flag == true)//刷新语言时不清除数据
    {
        uteModuleHeartStopSingleTesting(TYPE_HEART);
        uteModuleGuiCommonDisplayOffAllowGoBack(true);
        if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
        {
            task_stack_pop();
        }
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
    }

}
#else
enum
{
    COMPO_ID_NUM_SPORT_TIME = 1,    //运动时间
    COMPO_ID_NUM_SPORT_HEARTRATE,   //心率
    COMPO_ID_NUM_SPORT_KCAL,        //卡路里
    COMPO_ID_NUM_SPORT_STEP,        //计步
    COMPO_ID_NUM_SPORT_KM,          //距离
    COMPO_ID_NUM_SPORT_COUNT,       //计次

    COMPO_ID_UINT_SPORT_TIME,        //运动时间
    COMPO_ID_UINT_SPORT_HEARTRATE,   //心率
    COMPO_ID_UINT_SPORT_KCAL,        //卡路里
    COMPO_ID_UINT_SPORT_STEP,        //计步
    COMPO_ID_UINT_SPORT_KM,          //距离
    COMPO_ID_UINT_SPORT_COUNT,       //计次

    COMPO_ID_PIC_SPORT_HEARTRATE,    //心率图片
    COMPO_ID_TILTE_TIME,             //标题栏时间

    COMPO_ID_BTN_SPORT_STOP,         //暂停
    COMPO_ID_BTN_SPORT_EXIT,         //退出

    COMPO_ID_TXT_SPORT_STOP,         //暂停

};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

typedef struct f_sport_sub_run_t_
{
    u8 heart_pic_size_perc;
    bool heart_pic_size_add_flag;
    u32 updata_tick;

    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
    bool        sport_run_state;
    bool        sport_run_state_updata_flag;
    bool        sport_run_km_uint_updata_flag;
} f_sport_sub_run_t;

enum
{
    TOUCH_FINISH_STATE=0,
    AUTO_STATE,

};
enum
{
    PAGE_1=0,
    PAGE_2,
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
    TOTCH_MOVE,
};
enum
{
    SPORT_RUN_STOP=false,
    SPORT_RUN_START=true,
};
static bool sport_start_flag = false;
extern u32 func_sport_get_disp_mode(void);//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
extern u32 func_sport_get_str(u8 sport_idx);
extern u32 func_sport_get_ui(u8 sport_idx);
//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_sub_run_form_create(void)
{
}
static void func_sport_sub_run_click_handler(void)
{
}
static void func_sport_sub_run_updata(void)
{
}
static void func_sport_sub_run_init(void)
{
}
static void func_sport_sub_run_exit_data(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//功能事件处理
static void func_sport_sub_run_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_sport_sub_run_updata();
    func_sport_sub_run_move_handle();
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_soprt_run_move();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}

//室内跑步功能消息处理
static void func_sport_sub_run_message(size_msg_t msg)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    switch (msg)
    {
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
            break;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            if(f_sport_sub_run->touch_state != AUTO_STATE)
            {
                f_sport_sub_run->touch_flag = true;
            }
#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            if(f_sport_sub_run->touch_state == TOUCH_FINISH_STATE)
            {
                f_sport_sub_run->touch_flag = true;
            }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;
        case MSG_SYS_500MS:
            func_sport_sub_run_updata();
            break;
        case MSG_CTP_CLICK:
            func_sport_sub_run_click_handler();
            break;
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_LONG_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_LONG_DOWN:
            if(f_sport_sub_run->page2_move_flag && f_sport_sub_run->direction==TOUCH_NULL)
            {
                f_sport_sub_run->direction = UP_DOWM_DIR;
            }
            break;
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG_LEFT:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG_RIGHT:
            if(f_sport_sub_run->direction==TOUCH_NULL)
            {
                f_sport_sub_run->direction = LEFT_RIGHT_DIR;
            }
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
            switch(f_sport_sub_run->page_num)
            {
                case PAGE_1:
                    widget_page_set_client(func_cb.frm_main->page, 360, 0);
                    break;
                case PAGE_2:
                    widget_page_set_client(func_cb.frm_main->page_body,0, 0);
                    widget_page_set_client(func_cb.frm_main->page, 360, 0);
                    break;
            }
            f_sport_sub_run->page_old_y = 0;
            f_sport_sub_run->move_offset = 0;
            f_sport_sub_run->move_offset_x = 360;
            f_sport_sub_run->page_old_x = 360;
            f_sport_sub_run->page_num = PAGE_3;

            break;
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
        case KU_BACK:


            if(f_sport_sub_run->page_num == PAGE_1)f_sport_sub_run->page_num = PAGE_2;
            else if(f_sport_sub_run->page_num == PAGE_2)f_sport_sub_run->page_num = PAGE_1;

            switch(f_sport_sub_run->page_num)
            {
                case PAGE_1:

                    uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);
                    f_sport_sub_run->page_old_y = 0;
                    f_sport_sub_run->move_offset = 0;
                    break;
                case PAGE_2:
                    uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_PAUSE);
                    f_sport_sub_run->page_old_y = GUI_SCREEN_WIDTH;
                    f_sport_sub_run->move_offset = GUI_SCREEN_WIDTH;
                    break;
            }
            widget_page_set_client(func_cb.frm_main->page_body,f_sport_sub_run->move_offset, 0);
            break;
        case KU_DELAY_BACK:
            break;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

        case KU_LEFT:
            if (UTE_KEY_LEFT_SWITCH_SCREEN == FUNC_SPORT)
            {
                break;
            }

        default:
            func_message(msg);
            break;
    }
}

//进入室内跑步功能
static void func_sport_sub_run_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_sport_sub_run_t));
    func_sport_sub_run_init();
}

//退出室内跑步功能
static void func_sport_sub_run_exit(void)
{
    func_sport_sub_run_exit_data();
}

//室内跑步功能
void func_sport_sub_run(void)
{
    printf("%s\n", __func__);
    func_sport_sub_run_enter();
    while (func_cb.sta == FUNC_SPORT_SUB_RUN)
    {
        func_sport_sub_run_message(msg_dequeue());
        func_sport_sub_run_process();
    }
    func_sport_sub_run_exit();
}


