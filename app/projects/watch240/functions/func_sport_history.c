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
#include "ute_module_systemtime.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
static ute_module_sports_record_data_t sportsHistoryRecordData;
bool sport_history_page = false;
typedef struct sport_history_t_
{
    compo_listbox_t *listbox;
} sport_history_t;

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
static u8 page_type = 0;
enum
{
    PAGE_NO_DATA,
    PAGE_DATA,
    PAGE_SUMMARY,
};

enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    NO_DATA = 0,        //无数据 //心率、消耗、时长
    MULTIPLE_DATA,  //多数据 //支持实时配速、步频、心率、实时速度、步数、消耗、平均配速、时长、距离
    MID_DATA,       //中数据 //支持实时配速、步频、心率、步数、消耗、平均配速、时长、距离
    LESS_DATA,      //少数据 //实时速度、距离、心率、实时配速、时长
};

//组件ID
enum
{
    //卡片
    COMPO_ID_BTN_SHAPE = 1,
    COMPO_ID_TXT,
    COMPO_ID_PIC_MORE,
    COMPO_ID_LISTBOX,
};

static compo_listbox_item_t tbl_sport_history_list[20] =
{

};
extern const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM];
extern compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM];
extern void func_sport_list_reorder(compo_listbox_item_t *sportList);

compo_form_t *func_sport_history_nodata_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_EXERCISE_RECODE]);

    compo_picturebox_t * pic;
    compo_textbox_t *textbox;

    pic = compo_picturebox_create(frm,UI_BUF_I341001_4_SPORTS_RECORD_NODATE_BIN);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,142 + 118 / 2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_NO_RECORDS]));///公里文本
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 297 + 48 / 2, 300,45);
    compo_textbox_set(textbox, i18n[STR_NO_RECORDS]);
    page_type = PAGE_NO_DATA;
    return frm;
}
#define CARD_W (342)
#define CARD_H (104)
#define CARD_X (26)
#define CARD_Y (147)
#define CARD_SPACE (118)
#define PRO_X(a)    ((a) - CARD_X - CARD_W / 2)
#define PRO_Y(a)    ((a) - CARD_Y - CARD_H / 2)
//菜单项定义

static void sport_history_init(compo_listbox_item_t *list)
{
    for(u8 i = 0; i < uteModuleSportsHistoryGetRecordTotalCnt(); i++)
    {
        u8 mode = sportsHistoryRecordData.oneHistoryRecordData[i].sportsType;
        list[i].res_addr = tbl_sport_list[mode].res_addr;
    }
}

void get_sport_history_txt(u8 i,char *txt)
{
    char txt_buf[20];
    u8 mode = sportsHistoryRecordData.oneHistoryRecordData[i].sportsType;
    u8 index = uteModuleSportFindHundredSportIndexByMode(mode);
    if(uteModuleSportGetCurrSportShowDataType(mode)!= NO_DATA)
    {
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d.%02d",sportsHistoryRecordData.oneHistoryRecordData[i].sportDistanceInteger,\
                 sportsHistoryRecordData.oneHistoryRecordData[i].sportDistanceDecimals);
    }
    else
    {
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d",sportsHistoryRecordData.oneHistoryRecordData[i].sportCaloire);

    }
    memcpy(txt,txt_buf,sizeof(txt_buf));
}

void get_sport_history_time(u8 i,char *txt)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    char txt_buf[20];
    if(sportsHistoryRecordData.oneHistoryRecordData[i].startSportTime.day == time.day)
    {
        u8 sportsDateHour = sportsHistoryRecordData.oneHistoryRecordData[i].startSportTime.hour;
        if (time.is12HourTime)
        {
            if (sportsDateHour > 12)
            {
                sportsDateHour = sportsDateHour - 12;
            }
            if (sportsDateHour == 0)
            {
                sportsDateHour = 12;
            }
        }
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf), "%02d:%02d", sportsDateHour, sportsHistoryRecordData.oneHistoryRecordData[i].startSportTime.min);
    }
    else
    {
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf), "%02d/%02d", sportsHistoryRecordData.oneHistoryRecordData[i].startSportTime.month, sportsHistoryRecordData.oneHistoryRecordData[i].startSportTime.day);
    }
    memcpy(txt,txt_buf,sizeof(txt_buf));
}


void get_sport_history_uint(u8 i,char *txt)
{
    char txt_buf[30];
    u8 mode = sportsHistoryRecordData.oneHistoryRecordData[i].sportsType;
    int txt_index = (uteModuleSportGetCurrSportShowDataType(mode)!= NO_DATA) ? STR_KM :STR_KCAL;
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf), "%s", i18n[txt_index]);
    memcpy(txt,i18n[txt_index],strlen(txt_buf));
}

void get_sport_history_pm(u8 i,char *txt)
{
    char txt_buf[30];
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if(sportsHistoryRecordData.oneHistoryRecordData[i].startSportTime.day == time.day)
    {
        u8 sportsDateHour = sportsHistoryRecordData.oneHistoryRecordData[i].startSportTime.hour;
        memset(txt_buf,0,sizeof(txt_buf));
        int index_txt = 0;
        if(sportsDateHour,time.is12HourTime)
        {
            index_txt = (sportsDateHour > 11) ? STR_PM:STR_AM;
        }
        snprintf(txt_buf,sizeof(txt_buf), "%s",i18n[index_txt]);
        memcpy(txt,txt_buf,sizeof(txt_buf));
    }
}

compo_form_t *func_sport_history_data_form_create(void)
{
    char txt_buf[30];
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_EXERCISE_RECODE]);
    uteModuleSportsHistoryRecordSetIsHistoryRecordData(true);
    uteModuleSportsHistoryRecordGetData(&sportsHistoryRecordData);
    u8 COUNT = uteModuleSportsHistoryGetRecordTotalCnt();
    sport_history_init(tbl_sport_history_list);

    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SPORT_HISTORY_LIST);
    compo_listbox_set(listbox, tbl_sport_history_list, COUNT);

    compo_listbox_set_bgimg(listbox, UI_BUF_I341001_20_ALARM_CLOCK_LINE_BIN);
    compo_listbox_set_item_height(listbox,118);
    //comp_lisbox_set_icon_pos(listbox,0,24,listbox->line_center_y);
    compo_listbox_set_sta_icon(listbox,UI_BUF_I341001_4_SPORTS_RECORD_NEXT_BIN,0);

    //compo_listbox_set_focus_byidx(listbox, 0);
    int y = compo_listbox_gety_byidx(listbox, 0);
    compo_listbox_set_focus(listbox,y + 40);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_update(listbox);

    int move_y = 100;
    compo_shape_t * shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X,move_y,342,62,12,0XFF,make_color(0x33,0x33,0x33));
    ute_compo_setid_for_btn(shape,COMPO_ID_BTN_SHAPE);
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_LAST_SUMMARY]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,26,move_y - 18,290,46);
    compo_textbox_set(textbox, i18n[STR_LAST_SUMMARY]);
    compo_setid(textbox,COMPO_ID_TXT);
    compo_picturebox_t * pic = compo_picturebox_create(frm, UI_BUF_I341001_4_SPORTS_RECORD_NEXT_BIN);
    compo_picturebox_set_pos(pic, 330, move_y);
    compo_setid(pic,COMPO_ID_PIC_MORE);
    page_type = PAGE_DATA;
    return frm;
}

compo_form_t *func_sport_history_summary_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SUMMARY]);

    compo_picturebox_t *picbox = NULL;
    compo_textbox_t *textbox = NULL;
    char txt_buf[20];

    int total_time = 0;
    int total_kcal = 0;
    for(u8 i = 0; i < uteModuleSportsHistoryGetRecordTotalCnt(); i++)
    {
        total_time = total_time + sportsHistoryRecordData.oneHistoryRecordData[i].totalTime;
        total_kcal = total_kcal + sportsHistoryRecordData.oneHistoryRecordData[i].sportCaloire;
    }
#define SPACE_Y 118
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_4_COUNTDOWN_ICON_SPORTS_TIMES_BIN);
    compo_picturebox_set_pos(picbox, 20 + 66 / 2, 90 + 66 / 2);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_TOAL_DURATION]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113,86,240,40);
    compo_textbox_set(textbox, i18n[STR_TOAL_DURATION]);

    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",total_time/60/60,total_time/60%60,total_time%60);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,113,125);
    compo_textbox_set(textbox, txt_buf);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,182,320,1);
    compo_shape_set_color(shape, make_color(0x33,0x33,0x33));

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_3_EXERCISE_5_EXERCISING_ICON_KCAL_BIN);
    compo_picturebox_set_location(picbox, 20 + 66 / 2, 90 + 66 / 2 + SPACE_Y,66,66);

    textbox = compo_textbox_create(frm, 30);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,113,86 + SPACE_Y,240,40);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s(%s)",i18n[STR_TOAL_CALORIES],i18n[STR_KCAL]);
    compo_textbox_set(textbox, txt_buf);

    /*运动时长->数据*/
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",total_kcal);
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,113,125 + SPACE_Y);
    compo_textbox_set(textbox, txt_buf);

    page_type = PAGE_SUMMARY;
    return frm;
}


compo_form_t *func_sport_history_form_create(void)
{
    compo_form_t *frm = NULL;
    printf("czh line %d, number = %d\n",__LINE__,uteModuleSportsHistoryGetRecordTotalCnt());
    if (uteModuleSportsHistoryGetRecordTotalCnt() == 0)
    {
        frm = func_sport_history_nodata_form_create();
    }
    else
    {
        frm = func_sport_history_data_form_create();
    }
    return frm;
}

static void func_sport_history_process(void)
{
    sport_history_t *sport_history = (sport_history_t *)func_cb.f_cb;
    compo_shape_t* shape  = ute_compo_getobj_byid_for_btn(COMPO_ID_BTN_SHAPE);
    compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_PIC_MORE);
    compo_textbox_t* textbox =  compo_getobj_byid(COMPO_ID_TXT);

    switch(page_type)
    {
        case PAGE_DATA:
        {
            int move_y = compo_listbox_gety_byidx(sport_history->listbox,0);
            move_y = move_y - sport_history->listbox->ofs_y + 140;
            compo_shape_set_pos(shape, GUI_SCREEN_CENTER_X, move_y);
            compo_picturebox_set_pos(pic,334,move_y);
            compo_textbox_set_location(textbox,26,move_y - 18,280,46);

            u8 COUNT = uteModuleSportsHistoryGetRecordTotalCnt();
            if(COUNT > 2)
            {
                compo_listbox_move(sport_history->listbox);
            }
        }
        break;
        case PAGE_NO_DATA:
        case PAGE_SUMMARY:
            break;
    }
    func_process();
}
//单击按钮
static void func_sport_history_button_click(void)
{
    sport_history_t *sport_history = (sport_history_t *)func_cb.f_cb;
    compo_listbox_t *listbox = sport_history->listbox;
    u32 id = compo_get_button_id();
    switch(id)
    {
        case COMPO_ID_BTN_SHAPE:
            if(listbox->mcb != NULL)
            {
                func_free(listbox->mcb);
            }
            compo_form_destroy(func_cb.frm_main);
            func_cb.frm_main = func_sport_history_summary_form_create();
            listbox->mcb = NULL;
            return;
    }

    u8 icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= UTE_MODULE_SPORT_MAX_SPORT_NUM)
    {
        return;
    }
    else
    {
        uteModuleSportsSetHistoryRecordDisplayIndex(icon_idx & 0xff);
        sport_history_page = true;
        func_cb.sta = FUNC_SPORT_FINISH;

    }
}

static void func_sport_history_message(size_msg_t msg)
{
    sport_history_t *sport_history = (sport_history_t *)func_cb.f_cb;
    compo_listbox_t *listbox = sport_history->listbox;
    u8 count = uteModuleSportsHistoryGetRecordTotalCnt();
    if((page_type == PAGE_DATA) && count > 2)
    {
        if (compo_listbox_message(listbox, msg))
        {
            return;                                         //处理列表框信息
        }

    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            if(page_type == PAGE_DATA)
            {
                func_sport_history_button_click();                //单击图标
            }
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
        case MSG_CTP_SHORT_RIGHT:
            switch(page_type)
            {
                case PAGE_DATA:
                case PAGE_NO_DATA:
                    func_message(msg);
                    break;
                case PAGE_SUMMARY:
                {
                    compo_form_destroy(func_cb.frm_main);
                    func_cb.frm_main = func_sport_history_data_form_create();
                    sport_history->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
                    listbox = sport_history->listbox;
                    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
                    compo_listbox_move_init(listbox);
                    compo_listbox_move_init_modify(listbox, compo_listbox_gety_byidx(listbox,0) + 40, compo_listbox_gety_byidx(listbox, count - 1));
                }
                break;
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

static void func_sport_history_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(sport_history_t));
    func_cb.frm_main = func_sport_history_form_create();
    sport_history_t *sport_history = (sport_history_t *)func_cb.f_cb;
    u8 count = uteModuleSportsHistoryGetRecordTotalCnt();
    sport_history_page = false;
    if(count)
    {
        sport_history->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
        compo_listbox_t *listbox = sport_history->listbox;
        listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
        compo_listbox_move_init(listbox);
        compo_listbox_move_init_modify(listbox, compo_listbox_gety_byidx(listbox,0) + 40, compo_listbox_gety_byidx(listbox, count - 1) - 180);
    }
}

//退出运动功能
static void func_sport_history_exit(void)
{
    sport_history_t *sport_history = (sport_history_t *)func_cb.f_cb;
    func_cb.last = FUNC_SPORT_HISTORY;
    uteModuleSportsHistoryRecordSetIsHistoryRecordData(false);
    if(sport_history->listbox->mcb != NULL)
    {
        func_free(sport_history->listbox->mcb);
        sport_history->listbox->mcb = NULL;
    }
}
#else

static void func_sport_history_process(void)
{
    func_process();
}


static void func_sport_history_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            break;
        default:
            func_message(msg);
            break;
    }
}

static void func_sport_history_enter(void)
{


}

//退出运动功能
static void func_sport_history_exit(void)
{
}
#endif
//运动功能
void func_sport_history(void)
{
    printf("%s\n", __func__);
    func_sport_history_enter();
    while (func_cb.sta == FUNC_SPORT_HISTORY)
    {
        func_sport_history_process();
        func_sport_history_message(msg_dequeue());
    }
    func_sport_history_exit();
}
#endif //UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT