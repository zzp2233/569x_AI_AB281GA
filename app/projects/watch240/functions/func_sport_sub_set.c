#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_drv_battery_common.h"
#include "func_cover.h"
#include "ute_drv_motor.h"
#include "ute_module_heart.h"
#include "ute_module_sportTarget.h"
#include "ute_application_common.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SPORT_SET_SUPPORT
enum
{
    COMPO_ID_BTN_TAG = 1,
    COMPO_ID_BTN_STAR,
    COMPO_ID_TXT_VAL,
    COMPO_ID_TXT_UNT,

    COMPO_ID_BTN_TIME,
    COMPO_ID_BTN_KCAL,
    COMPO_ID_BTN_DANS,
    COMPO_ID_BTN_NOGOL,

    COMPO_ID_BTN_ENTER,
    COMPO_ID_BTN_EXIT,

    COMPO_ID_BTN_NO,
    COMPO_ID_BTN_YES,
};

enum
{
    MAIN_PAGE,
    SET_PAGE,
    FAIL_WEAR,
    TIP_PAGE,
};

typedef enum SET_MODE_T
{
    SET_TIME_MODE = 1,
    SET_KCAL_MODE,
    SET_DANS_MODE,
} SET_MODE;

typedef enum SPORT_TIP_T
{
    BLE_NO_BACK = 1,  //蓝牙无连接直接返回
    BLE_NO_ENTER,      //蓝牙无连接可进入
    GPS_WORING,        //GPS获取定位
    GPS_FAIL_BACK,     //GPS获取位置失败退出
    GPS_FAIL_ENTER,    //GPS获取位置失败可进入
    RECODE_FULL,       //已达最大条数

} SPORT_TIP;

static SET_MODE list_mode;
static void sport_set_mode(SET_MODE mode)
{
    list_mode = mode;
}

SET_MODE sport_get_mode(void)
{
    return list_mode;
}

typedef struct f_sport_sub_set_list_t_
{
    bool no_wear_flag;
    u8 sport_tip;
    u8 sport_gps_time;
    bool sport_start;
    u32 tick;
} f_sport_sub_set_list_t;

static compo_form_t* __this = NULL;
static u8 page_type = 0;
extern u32 func_sport_get_str(u8 sport_idx);
compo_form_t *func_sport_sub_set_main_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    int heart_value = uteModuleHeartGetHeartValue();
    uint8_t sportType = uteModuleSportGetAPPSendGPSType();
    char txt_buf[20];

    if(sportType)
    {
        if (uteModuleSportGetGPSStatus() != GPS_LACATION_OK)
        {
            uteModuleSportSetGPSStatus(GPS_LACATION_ING);
            uteModuleSportSendMoreSportsGetGPSStatus(0x01);
        }
    }

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[func_sport_get_str(uteModuleSportTargetGetShowIndex())]);

    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_4_COUNTDOWN_HEART_MAX_BIN);
    compo_picturebox_set_pos(pic, 88 + 50 / 2, 126 + 44 / 2);
    if(func_cb.sta == FUNC_SPORT_SUB_SET)
    {
        f_sport_sub_set_list_t* f_sport_sub_set = (f_sport_sub_set_list_t *)func_cb.f_cb;
    }
    printf("sportType  = %d\n",sportType);
    compo_button_t *button = compo_button_create_by_image(frm,sportType ? UI_BUF_I341001_3_EXERCISE_4_COUNTDOWN_GO00_BIN : UI_BUF_I341001_3_EXERCISE_4_COUNTDOWN_GO01_BIN);
    compo_button_set_pos(button,GUI_SCREEN_CENTER_X,252 + 158 / 2);
    compo_setid(button,COMPO_ID_BTN_STAR);

    compo_textbox_t *textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,146,98 + 20);
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
    compo_setid(textbox,COMPO_ID_TXT_VAL);
    area_t txt_leng = widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm, 6);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,156+txt_leng.wid,136,100, widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_PER_MINUTE]);
    compo_setid(textbox,COMPO_ID_TXT_UNT);

    u8 state = uteModuleSportTargetGetIsNoTarget();
    memset(txt_buf,0,sizeof(txt_buf));
    switch(state)
    {
        case SPORT_TARGET_DISTANCE_SET:
        {
            int dans_t = uteModuleSportTargetGetDistanceGoal() / 1000;
            int dans_s = (uteModuleSportTargetGetDistanceGoal() % 1000 / 100);
            snprintf(txt_buf,sizeof(txt_buf),"%d.%d%s",dans_t,dans_s,i18n[STR_KM]);
        }
        break;
        case SPORT_TARGET_TIME_SET:
        {
            int time = uteModuleSportTargetGetTimeGoal() / 60;
            snprintf(txt_buf,sizeof(txt_buf),"%d%s",time,i18n[STR_MINUTE]);
        }
        break;
        case SPORT_TARGET_KCAL_SET:
        {
            int kcal = uteModuleSportTargetGetKcalGoal();
            snprintf(txt_buf,sizeof(txt_buf),"%d%s",kcal,i18n[STR_KCAL]);
        }
        break;
        default:
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s",i18n[STR_SET_GOAL]);
        }
        break;
    }

    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,120,190,150, widget_text_get_max_height());
    compo_textbox_set(textbox,txt_buf);
    txt_leng = widget_text_get_area(textbox->txt);

    pic = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_2_MOTION_TARGET_NEXT_BIN);
    compo_picturebox_set_pos(pic, 134 + txt_leng.wid, 208);


    button = compo_button_create(frm);
    compo_button_set_location(button,GUI_SCREEN_CENTER_X,190 + 34 / 2,150,80);
    compo_setid(button,COMPO_ID_BTN_TAG);
    __this = frm;
    page_type = MAIN_PAGE;
    return frm;
}
compo_form_t *func_sport_sub_target_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    ute_module_more_sports_data_t sportsData;
    uteModuleSportGetMoreSportsDatas(&sportsData);
    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SET_GOAL]);
    compo_button_t *button = compo_button_create_by_image(frm,UI_BUF_I341001_3_EXERCISE_2_MOTION_TARGET_SETING_TIMES_BIN);
    compo_button_set_pos(button,19 + 150 / 2,98 + 90 / 2);
    compo_setid(button,COMPO_ID_BTN_TIME);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_CUSTOM_TIME]));/// 数据
    compo_textbox_set_pos(textbox,19 + 150 / 2,194 + 36 / 2);
    compo_textbox_set(textbox,i18n[STR_CUSTOM_TIME]);

    button = compo_button_create_by_image(frm,UI_BUF_I341001_3_EXERCISE_2_MOTION_TARGET_SETING_KCAL_BIN);
    compo_button_set_pos(button,198 + 150 / 2,98 + 90 / 2);
    compo_setid(button,COMPO_ID_BTN_KCAL);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_CALORIE]));/// 数据
    compo_textbox_set_pos(textbox,198 + 150 / 2,194 + 36 / 2);
    compo_textbox_set(textbox,i18n[STR_CALORIE]);

    printf("czh line %d,type %d\n",__LINE__,sportsData.saveData.sportsType);
    u8 sport_type = uteModuleSportGetCurrSportShowDataType(uteModuleSportMoreSportGetType());
    if(sport_type)
    {
        button = compo_button_create_by_image(frm,UI_BUF_I341001_3_EXERCISE_2_MOTION_TARGET_SETING_DIS_BIN);
        compo_button_set_pos(button,19 + 150 / 2,264 + 90 / 2);
        compo_setid(button,COMPO_ID_BTN_DANS);

        textbox = compo_textbox_create(frm, strlen(i18n[STR_DISTANCE]));/// 数据
        compo_textbox_set_pos(textbox,19 + 150 / 2,360 + 36 / 2);
        compo_textbox_set(textbox,i18n[STR_DISTANCE]);

        button = compo_button_create_by_image(frm,UI_BUF_I341001_3_EXERCISE_2_MOTION_TARGET_NO_GOAL01_BIN);
        compo_button_set_pos(button,198 + 150 / 2,264 + 90 / 2);
        compo_setid(button,COMPO_ID_BTN_NOGOL);

        textbox = compo_textbox_create(frm, 10);/// 数据
        compo_textbox_set_pos(textbox,198 + 150 / 2,360 + 36 / 2);
        compo_textbox_set(textbox,i18n[STR_NO_GOAL]);
    }
    else
    {
        button = compo_button_create_by_image(frm,UI_BUF_I341001_3_EXERCISE_2_MOTION_TARGET_NO_GOAL01_BIN);
        compo_button_set_pos(button,19 + 150 / 2,264 + 90 / 2);
        compo_setid(button,COMPO_ID_BTN_NOGOL);

        textbox = compo_textbox_create(frm, 10);/// 数据
        compo_textbox_set_pos(textbox,19 + 150 / 2,360 + 36 / 2);
        compo_textbox_set(textbox,i18n[STR_NO_GOAL]);
    }

    __this = frm;
    page_type = SET_PAGE;
    return frm;
}

compo_form_t *func_sport_sub_set_form_create(void)
{
    compo_form_t *frm = NULL;
    extern u8 sport_target_page;
    switch(sport_target_page)
    {
        case 0:
            frm = func_sport_sub_set_main_form_create();
            break;
        case 1:
            frm = func_sport_sub_target_form_create();
            break;
        default:
            func_sport_sub_set_main_form_create();
            break;
    }
    return frm;
}

static compo_form_t *func_sport_sub_fail_wear_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_2_MOTION_TARGET_ICON_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 70 + 180 / 2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_WEAR_DETECTION]));/// 数据
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,250 + 150 / 2,330,150);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox,i18n[STR_WEAR_DETECTION]);
    page_type = FAIL_WEAR;
    __this = frm;
    return frm;
}
static compo_form_t *func_sport_sub_tip_form_create(SPORT_TIP type)
{
    f_sport_sub_set_list_t* f_sport_sub_set = (f_sport_sub_set_list_t *)func_cb.f_cb;
    compo_form_t *frm = compo_form_create(true);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_28_SET_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,346 + 80 / 2);
    compo_button_set_visible(btn_ok,true);
    compo_setid(btn_ok,COMPO_ID_BTN_ENTER);

    compo_textbox_t *txt_remind = compo_textbox_create(frm, 100);
    compo_textbox_set_location(txt_remind,GUI_SCREEN_CENTER_X,200,320,180);
    compo_textbox_set_forecolor(txt_remind,COLOR_WHITE);
    compo_textbox_set_multiline(txt_remind,true);

    switch(type)
    {
        case BLE_NO_BACK:
            f_sport_sub_set->sport_start = false;
            compo_textbox_set(txt_remind,i18n[STR_SPORT_TIP1]);
            break;
        case BLE_NO_ENTER:
            f_sport_sub_set->sport_start = true;
            compo_textbox_set(txt_remind,i18n[STR_SPORT_TIP2]);
            break;
        case GPS_WORING:
        {
            compo_textbox_t * textbox = compo_textbox_create(frm, strlen(i18n[STR_LOCATING]));
            compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,132,320,46);
            compo_textbox_set(textbox,i18n[STR_LOCATING]);

            compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_NO);
            compo_button_set_pos(btn, 24+156/2, 347+80/2);

            btn = compo_button_create_by_image(frm, UI_BUF_I341001_28_SET_CONFIRM_2_BIN);
            compo_setid(btn, COMPO_ID_BTN_YES);
            compo_button_set_pos(btn, 188 + 156 / 2, 347+80/2);

            f_sport_sub_set->sport_start = false;
            compo_textbox_set(txt_remind,i18n[STR_SPORT_TIP3]);
            compo_button_set_visible(btn_ok,false);
            compo_textbox_set_forecolor(txt_remind,make_color(0x66,0x66,0x66));

        }
        break;
        case GPS_FAIL_BACK:
            f_sport_sub_set->sport_start = false;
            compo_textbox_set(txt_remind,i18n[STR_SPORT_TIP4]);
            break;
        case GPS_FAIL_ENTER:
            f_sport_sub_set->sport_start = true;
            compo_textbox_set(txt_remind,i18n[STR_SPORT_TIP5]);
            break;
        case RECODE_FULL:
        {
            f_sport_sub_set->sport_start = true;
            compo_textbox_set(txt_remind,i18n[STR_STORAGE_FULL]);
            compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
            compo_setid(btn, COMPO_ID_BTN_NO);
            compo_button_set_pos(btn, 24+156/2, 347+80/2);

            btn = compo_button_create_by_image(frm, UI_BUF_I341001_28_SET_CONFIRM_2_BIN);
            compo_setid(btn, COMPO_ID_BTN_YES);
            compo_button_set_pos(btn, 188 + 156 / 2, 347+80/2);
            compo_button_set_visible(btn_ok,false);
        }
        break;
    }
    f_sport_sub_set->sport_tip = type;
    __this = frm;
    page_type = TIP_PAGE;
    return frm;

}
static u8 time_count = 0;
static void func_sport_sub_set_process(void)
{
    char txt_buf[20];
    int heart_value = uteModuleHeartGetHeartValue();
    f_sport_sub_set_list_t* f_sport_sub_set = (f_sport_sub_set_list_t *)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VAL);
    compo_textbox_t *textbox_unt = compo_getobj_byid(COMPO_ID_TXT_UNT);
    compo_button_t *start_btn = compo_getobj_byid(COMPO_ID_BTN_STAR);
    uint8_t sportType = uteModuleSportGetAPPSendGPSType();
    switch(page_type)
    {
        case MAIN_PAGE:
            if(tick_check_expire(f_sport_sub_set->tick, 500))
            {
                time_count++;
                f_sport_sub_set->tick = tick_get();
                if(sportType)
                {
                    if (uteModuleSportGetGPSStatus() == GPS_LACATION_OK)
                    {
                        compo_button_set_bgimg(start_btn,UI_BUF_I341001_3_EXERCISE_4_COUNTDOWN_GO01_BIN);
                    }
                    else if (uteModuleSportGetGPSStatus() == GPS_LACATION_FAIL)
                    {
                        compo_button_set_bgimg(start_btn,UI_BUF_I341001_3_EXERCISE_4_COUNTDOWN_GO00_BIN);
                    }
                }

                if(!uteModuleHeartIsWear() && time_count > 2)
                {
                    time_count = 0;
                    compo_form_destroy(__this);
                    func_cb.frm_main = func_sport_sub_fail_wear_form_create();
                    return;
                }
                if(heart_value)
                {
                    snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
                }
                else
                {
                    snprintf(txt_buf,sizeof(txt_buf),"--");
                }
                compo_textbox_set(textbox,txt_buf);

                area_t txt_leng = widget_text_get_area(textbox->txt);
                compo_textbox_set_location(textbox_unt,156+txt_leng.wid,136,100, widget_text_get_max_height());
            }
            break;
        case FAIL_WEAR:
            if(tick_check_expire(f_sport_sub_set->tick, 500))
            {
                f_sport_sub_set->tick = tick_get();
                reset_guioff_delay();
                f_sport_sub_set->sport_gps_time++;
                if(f_sport_sub_set->sport_gps_time > 10)
                {
                    f_sport_sub_set->sport_gps_time = 0;
                    compo_form_destroy(__this);
                    func_cb.frm_main = func_sport_sub_set_main_form_create();
                }
            }
            break;
        case SET_PAGE:
            break;
        case TIP_PAGE:
            if(tick_check_expire(f_sport_sub_set->tick, 500))
            {
                reset_guioff_delay();
                f_sport_sub_set->tick = tick_get();
                switch(f_sport_sub_set->sport_tip)
                {
                    case BLE_NO_BACK:
                        f_sport_sub_set->sport_gps_time++;
                        if(f_sport_sub_set->sport_gps_time > 10)
                        {
                            f_sport_sub_set->sport_gps_time = 0;
                            compo_form_destroy(__this);
                            func_cb.frm_main = func_sport_sub_set_main_form_create();

                        }
                        break;
                    case GPS_FAIL_ENTER:
                    case BLE_NO_ENTER:
                        f_sport_sub_set->sport_gps_time++;
                        if(f_sport_sub_set->sport_gps_time > 10)
                        {
                            f_sport_sub_set->sport_start = true;
                            f_sport_sub_set->sport_gps_time = 0;
                            uteModuleSportTargetSetEditStatus(SPORT_TARGET_START);
                            uteModuleSportSetGPSStatus(GPS_LACATION_IDLE);
                            uteModuleSportSendMoreSportsGetGPSStatus(0x00);//ask APP no need send GPS
                            uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 0x01, false);
                        }
                        break;
                    case GPS_WORING:
                    {
                        f_sport_sub_set->sport_start = false;
                        f_sport_sub_set->sport_gps_time++;
                        if(f_sport_sub_set->sport_gps_time > 60)
                        {
                            f_sport_sub_set->sport_gps_time = 0;
                            if(sportType == 2)
                            {
                                compo_form_destroy(__this);
                                func_sport_sub_tip_form_create(GPS_FAIL_BACK);
                                return;
                            }
                            else
                            {
                                compo_form_destroy(__this);
                                func_sport_sub_tip_form_create(GPS_FAIL_ENTER);
                                return;
                            }
                        }
                        if (uteModuleSportGetGPSStatus() == GPS_LACATION_OK)
                        {
                            f_sport_sub_set->sport_start = true;
                            f_sport_sub_set->sport_gps_time = 0;
                            uteModuleSportTargetSetEditStatus(SPORT_TARGET_START);
                            uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 0x01, false);
                            //可点击进入
                            return;
                        }
                    }
                    break;
                    case GPS_FAIL_BACK:
                        uteModuleSportSetGPSStatus(GPS_LACATION_FAIL);
                        uteModuleSportSendMoreSportsGetGPSStatus(0x00);
                        f_sport_sub_set->sport_gps_time++;
                        if(f_sport_sub_set->sport_gps_time > 10)
                        {
                            f_sport_sub_set->sport_gps_time = 0;
                            compo_form_destroy(__this);
                            func_cb.frm_main = func_sport_sub_set_main_form_create();

                        }
                        break;
                }
            }
    }
    func_process();
}

static void func_sport_main_page_click()
{
    f_sport_sub_set_list_t* f_sport_sub_set = (f_sport_sub_set_list_t *)func_cb.f_cb;
    uint8_t sportType = uteModuleSportGetAPPSendGPSType();
    int id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BTN_TAG:
            compo_form_destroy(__this);
            func_cb.frm_main = func_sport_sub_target_form_create();

            break;
        case COMPO_ID_BTN_STAR:
#if APP_MODULE_SPORT_GPS_TRANSLATE_SUPPORT
            if (uteModuleSportGetMemoryFullType() == FULL_TYPE_IDLE)
            {
                if(ble_is_connect())
                {
                    if (uteModuleSportGetGPSStatus() == GPS_LACATION_OK)
                    {
                        uteModuleSportTargetSetEditStatus(SPORT_TARGET_START);
                        uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 0x01, false);
                        return;
                    }
                    if(sportType == 2)
                    {
                        compo_form_destroy(__this);
                        func_cb.frm_main = func_sport_sub_tip_form_create(GPS_WORING);
                    }
                    else if(sportType == 1)
                    {
                        compo_form_destroy(__this);
                        func_cb.frm_main = func_sport_sub_tip_form_create(GPS_WORING);
                    }
                    else
                    {
                        uteModuleSportTargetSetEditStatus(SPORT_TARGET_START);
                        uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 0x01, false);
                    }
                }
                else
                {
                    if(sportType == 2)
                    {
                        compo_form_destroy(__this);
                        func_cb.frm_main = func_sport_sub_tip_form_create(BLE_NO_BACK);
                    }
                    else if(sportType == 1)
                    {
                        compo_form_destroy(__this);
                        func_cb.frm_main = func_sport_sub_tip_form_create(BLE_NO_ENTER);
                    }
                    else
                    {
                        uteModuleSportTargetSetEditStatus(SPORT_TARGET_START);
                        uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 0x01, false);
                    }
                }
            }
            else
            {
                compo_form_destroy(__this);
                func_cb.frm_main = func_sport_sub_tip_form_create(RECODE_FULL);
            }

#endif
            break;
    }
}

static void func_sport_set_page_click()
{
    int id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BTN_TIME:
            sport_set_mode(SET_TIME_MODE);
            func_cb.sta = FUNC_SPORT_SUB_SET_LIST;
            break;
        case COMPO_ID_BTN_KCAL:
            sport_set_mode(SET_KCAL_MODE);
            func_cb.sta = FUNC_SPORT_SUB_SET_LIST;
            break;
        case COMPO_ID_BTN_DANS:
            sport_set_mode(SET_DANS_MODE);
            func_cb.sta = FUNC_SPORT_SUB_SET_LIST;
            break;
        case COMPO_ID_BTN_NOGOL:
            uteModuleSportTargetSetIsNoTarget(false);
            compo_form_destroy(func_cb.frm_main);
            func_cb.frm_main = func_sport_sub_set_main_form_create();
            break;
    }
}

static void func_sport_tip_page_click()
{
    f_sport_sub_set_list_t* f_sport_sub_set = (f_sport_sub_set_list_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    uint8_t sportType = uteModuleSportGetAPPSendGPSType();
    switch(id)
    {
        case COMPO_ID_BTN_YES:
        case COMPO_ID_BTN_ENTER:
        {
            f_sport_sub_set->sport_gps_time = 0;
            if(f_sport_sub_set->sport_start)
            {
                if(f_sport_sub_set->sport_tip != RECODE_FULL)
                {
                    uteModuleSportTargetSetEditStatus(SPORT_TARGET_START);
                    uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 0x01, false);
                }
                else
                {
                    if(ble_is_connect())
                    {
                        if(sportType == 2)
                        {
                            compo_form_destroy(__this);
                            func_cb.frm_main = func_sport_sub_tip_form_create(GPS_WORING);
                        }
                        else if(sportType == 1)
                        {
                            compo_form_destroy(__this);
                            func_cb.frm_main = func_sport_sub_tip_form_create(GPS_WORING);
                        }
                        else
                        {
                            uteModuleSportTargetSetEditStatus(SPORT_TARGET_START);
                            uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 0x01, false);
                        }
                    }
                    else
                    {
                        if(sportType == 2)
                        {
                            compo_form_destroy(__this);
                            func_cb.frm_main = func_sport_sub_tip_form_create(BLE_NO_BACK);
                        }
                        else if(sportType == 1)
                        {
                            compo_form_destroy(__this);
                            func_cb.frm_main = func_sport_sub_tip_form_create(BLE_NO_ENTER);
                        }
                        else
                        {
                            uteModuleSportTargetSetEditStatus(SPORT_TARGET_START);
                            uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 0x01, false);
                        }
                    }
                    return;
                }
            }
            else
            {
                if(uteModuleSportGetGPSStatus() == GPS_LACATION_ING)
                {
                    if(sportType == 2)
                    {
                        compo_form_destroy(__this);
                        func_sport_sub_tip_form_create(GPS_FAIL_BACK);
                        return;
                    }
                    else
                    {
                        compo_form_destroy(__this);
                        func_sport_sub_tip_form_create(GPS_FAIL_ENTER);
                        return;
                    }
                }
                else
                {
                    compo_form_destroy(__this);
                    func_cb.frm_main = func_sport_sub_set_main_form_create();
                    return;
                }
            }
        }
        case COMPO_ID_BTN_NO:
        {
            compo_form_destroy(__this);
            func_cb.frm_main = func_sport_sub_set_main_form_create();
            return;
        }
        break;
    }
}
static void func_sport_set_icon_click()
{
    switch(page_type)
    {
        case MAIN_PAGE:
            func_sport_main_page_click();
            break;
        case SET_PAGE:
            func_sport_set_page_click();
            break;
        case TIP_PAGE:
            func_sport_tip_page_click();
            break;
    }
}


static void func_sport_sub_set_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sport_set_icon_click();                //单击图标
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
        case MSG_CTP_SHORT_RIGHT:
            if(page_type == SET_PAGE || page_type == TIP_PAGE || page_type == FAIL_WEAR)
            {
                compo_form_destroy(__this);
                func_cb.frm_main = func_sport_sub_set_main_form_create();
                return;
            }
            else
            {
                func_cb.sta = FUNC_SPORT;                        //右滑缓慢退出任务
                if (func_cb.flag_sort_jump)
                {
                    func_cb.flag_sort = true;
                    func_cb.flag_sort_jump = false;
                }
            }
            break;
        default:
            func_message(msg);
            break;
    }
}

static void func_sport_sub_set_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sport_sub_set_list_t));
    func_cb.frm_main = func_sport_sub_set_form_create();
    uteModuleSportMoreSportsCalcSpaceSize();
    uteModuleHeartStartSingleTesting(TYPE_HEART);
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
}

//退出运动功能
static void func_sport_sub_set_exit(void)
{
    f_sport_sub_set_list_t *f_sport = (f_sport_sub_set_list_t *)func_cb.f_cb;
    func_cb.last = FUNC_SPORT_SUB_SET;
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    if(uteModuleSportMoreSportGetStatus() == ALL_SPORT_STATUS_CLOSE)
    {
        uteModuleHeartStopSingleTesting(TYPE_HEART);
        uteModuleSportSetGPSStatus(GPS_LACATION_IDLE);
        uteModuleSportSendMoreSportsStatusToAPPWhenChange(ALL_SPORT_STATUS_CLOSE);
    }
}

//运动功能
void func_sport_sub_set(void)
{
    printf("%s\n", __func__);
    func_sport_sub_set_enter();
    while (func_cb.sta == FUNC_SPORT_SUB_SET)
    {
        func_sport_sub_set_process();
        func_sport_sub_set_message(msg_dequeue());
    }
    func_sport_sub_set_exit();
}

#endif //UTE_MODULE_SCREENS_SPORT_SET_SUPPORT