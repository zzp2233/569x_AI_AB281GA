#include "include.h"
#include "func.h"
#include "func_cover.h"
#include "ute_module_call.h"
#include "ute_drv_motor.h"

#include "ute_module_gui_common.h"
#include "ute_module_log.h"
#include "ute_module_systemtime.h"
#include "ute_language_common.h"
#include "ute_module_timezonesetting.h"

#if 1
#define WCLOCK_DEBUG printf
#else
#define WCLOCK_DEBUG //
#endif
#if UTE_MODULE_SCREENS_WORLD_CLOCK_SUPPORT
enum
{
    COMPO_ID_CARD_START = 1,
};

typedef struct f_world_time_t_
{
    page_tp_move_t *ptm;
} f_world_time_t;

static uint8_t worldClockListTotalNumeber = 0;

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

static compo_form_t* __this = NULL;

#define CARD_W (368)
#define CARD_H (124)
#define CARD_X (0)
#define CARD_Y (85)
#define CARD_SPACE (0)
#define PRO_X(a)    ((a) - CARD_X - CARD_W / 2)
#define PRO_Y(a)    ((a) - CARD_Y - CARD_H / 2)

void func_get_world_time(ute_module_systemtime_time_t *buffer,ute_module_timezonesetting_data_t worldClockData,uint8_t index)
{

    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    static uint8_t worldClockMonthBuff[13] = {0,31,28,31,30,31,30,31,31,30,31,30,31};
    static uint8_t worldClockLeapMonthBuff[13] = {0,31,29,31,30,31,30,31,31,30,31,30,31};
    static uint8_t *monday;
    buffer->zone = (worldClockData.info[index].timezone_dec != 0xff) ? worldClockData.info[index].timezone_dec : 0;
    int hour_temp = 0;
    if(worldClockData.info[index].timezone > 0)//东
    {
        if((worldClockData.info[index].timezone*10 + buffer->zone - time.zone *10) >= 0)//设置的时区大于当前时区
        {
            hour_temp = time.hour + (worldClockData.info[index].timezone - time.zone);
            buffer->min = time.min + buffer->zone*60/10;
            if(buffer->min >= 60)
            {
                hour_temp = hour_temp + 1;
                buffer->min = buffer->min - 60;
            }
            if(hour_temp >= 24)  //判断是否大于24，看日期是否加1
            {
                hour_temp = hour_temp - 24;
                buffer->day = time.day + 1;
                if(uteModuleTimeZoneSettingIsLeapYear(time.year)) //判段平年还是闰年
                {
                    monday = &worldClockLeapMonthBuff[0];
                }
                else
                {
                    monday = &worldClockMonthBuff[0];
                }
                if(buffer->day  > monday[time.month])
                {
                    buffer->day = 1;
                    buffer->month = time.month + 1;
                    if(buffer->month > 12)
                    {
                        buffer->month = 1;
                        buffer->year = time.year + 1;
                    }
                    else
                    {
                        buffer->year = time.year;
                    }
                }
                else
                {
                    buffer->year = time.year;
                    buffer->month = time.month;
                }
            }
            else
            {
                buffer->day = time.day;
                buffer->month = time.month;
                buffer->year = time.year;
            }
        }
        else//设置的时区小于当前时区
        {
            hour_temp = time.hour - (time.zone - worldClockData.info[index].timezone);
            buffer->min = time.min + buffer->zone*60/100;
            if(buffer->min >= 60)
            {
                hour_temp = hour_temp + 1;
                buffer->min = buffer->min - 60;
            }
            if(hour_temp<0)  //判断是否小于0，看日期是否减1
            {
                hour_temp = hour_temp + 24;
                buffer->day = time.day - 1;
                if(uteModuleTimeZoneSettingIsLeapYear(time.year)) //判段平年还是闰年
                {
                    monday = &worldClockLeapMonthBuff[0];
                }
                else
                {
                    monday = &worldClockMonthBuff[0];
                }
                if(buffer->day  <= 0)
                {
                    buffer->month = time.month - 1;
                    buffer->day  = monday[buffer->month];
                    if(buffer->month <= 0)
                    {
                        buffer->month = 12;
                        buffer->year =time.year - 1;
                    }
                    else
                    {
                        buffer->year = time.year;
                    }
                }
                else
                {
                    buffer->year = time.year;
                    buffer->month = time.month;
                }
            }
            else
            {
                buffer->day  = time.day;
                buffer->month = time.month;
                buffer->year = time.year;
            }
        }
    }
    else//西
    {
        hour_temp = time.hour - (time.zone-worldClockData.info[index].timezone);
        buffer->min = time.min - buffer->zone*60/100;
        if(buffer->min < 0)
        {
            hour_temp = hour_temp - 1;
            buffer->min = buffer->min + 60;
        }
        if(hour_temp<0)  //判断是否小于0，看日期是否减1
        {
            hour_temp = hour_temp + 24;
            buffer->day = time.day - 1;
            if(uteModuleTimeZoneSettingIsLeapYear(time.year)) //判段平年还是闰年
            {
                monday = &worldClockLeapMonthBuff[0];
            }
            else
            {
                monday = &worldClockMonthBuff[0];
            }
            if(buffer->day <= 0)
            {
                buffer->month = time.month - 1;
                buffer->day = monday[buffer->month];
                if(buffer->month <= 0)
                {
                    buffer->month = 12;
                    buffer->year =time.year - 1;
                }
                else
                {
                    buffer->year = time.year;
                }
            }
            else
            {
                buffer->year = time.year;
                buffer->month = time.month;
            }
        }
        else
        {
            buffer->day = time.day;
            buffer->month = time.month;
            buffer->year = time.year;
        }
    }
    buffer->hour = hour_temp;

}

enum
{
    NODATA_PAGE,
    DATA_PAGE,
};
static u8 page_type = 0;
compo_form_t *func_world_clock_default_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_18_WORLD_CLOCK_PHONE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 96 + 144 / 2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_ADD]));
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 289 + 88 / 2, 300, 120);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox,i18n[STR_APP_ADD]);
    page_type = NODATA_PAGE;
    return frm;
}

compo_form_t *func_world_clock_date_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    char str_buff[50] = {};
    ute_module_timezonesetting_data_t worldClockData;
    uteModuleTimeZoneSettingGetData(&worldClockData);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_WORLD_CLOCK]);
    for(u8 i = 0; i < worldClockListTotalNumeber; i++)
    {
        compo_cardbox_t *cardbox = compo_cardbox_create(frm,0,1,4,318,112);
        compo_cardbox_set_pos(cardbox,GUI_SCREEN_CENTER_X,CARD_Y + CARD_H / 2 + (i*CARD_H));
        uint8_t txt[100] = {};
        int txt_len = 100;
        compo_cardbox_icon_set(cardbox,0,UI_BUF_I341001_20_ALARM_CLOCK_LINE_BIN);
        compo_cardbox_icon_set_pos(cardbox,0,0,112 / 2 - 10);

        compo_cardbox_text_set_location(cardbox,0,PRO_X(24),PRO_Y(90),200,46);
        compo_cardbox_text_set_align_center(cardbox,0, false);
        compo_cardbox_text_set(cardbox,0,worldClockData.info[i].countryNameU8);

        memset(str_buff,0,sizeof(str_buff));
        ute_module_systemtime_time_t get_time;
        func_get_world_time(&get_time,worldClockData,i);
        snprintf(str_buff, sizeof(str_buff), "%02d:%02d", get_time.hour, get_time.min);
        compo_cardbox_text_set_location(cardbox,1,PRO_X(24),PRO_Y(136),150,50);
        compo_cardbox_text_set_font(cardbox,1, UI_BUF_0FONT_FONT_NUM_48_BIN);
        compo_cardbox_text_set_align_center(cardbox,1, false);
        compo_cardbox_text_set(cardbox,1,str_buff);

        compo_cardbox_text_set_location(cardbox,2,PRO_X(240),PRO_Y(90),100,46);
        widget_text_set_right_align(cardbox->text[2],true);
        compo_cardbox_text_set_align_center(cardbox,2, false);
        int txt_index = (get_time.day == time.day ) ? STR_TO_DAY : ((get_time.day > time.day) ? STR_TOMMOROW :STR_YESTERDAY);
        compo_cardbox_text_set(cardbox,2,i18n[txt_index]);
        compo_cardbox_text_set_forecolor(cardbox,2, COLOR_GRAY);

        memset(str_buff,0,sizeof(str_buff));
        int8_t hour_temp = 0;
        int8_t time_temp = 0;
        int temp = (get_time.day - time.day) > 1 ? (-1) :((get_time.day - time.day) < -1 ? 1 : get_time.day - time.day);
        hour_temp = ((temp * 24 * 60 + get_time.hour * 60 + get_time.min) - (time.hour *60 + time.min)) / 60;
        time_temp = ((temp * 24 * 60 + get_time.hour * 60 + get_time.min) - (time.hour *60 + time.min)) % 60;
        WCLOCK_DEBUG("line %d ,hour_temp = %d\n",__LINE__,hour_temp);
        compo_cardbox_text_set_location(cardbox,3,PRO_X(150),PRO_Y(142),190,46);
        compo_cardbox_text_set_align_center(cardbox,3, false);
        memset(str_buff,0,sizeof(str_buff));
        if(hour_temp >= 0 || (hour_temp == 0 && time_temp > 0))
        {
            snprintf(str_buff,sizeof(str_buff),"+%d%s%d%s",hour_temp,i18n[STR_HOUR],time_temp,i18n[STR_MINUTE]);
        }
        else
        {
            snprintf(str_buff,sizeof(str_buff),"%d%s%d%s", hour_temp,i18n[STR_HOUR],time_temp,i18n[STR_MINUTE]);
        }
        compo_cardbox_text_set(cardbox,3,str_buff);
        widget_text_set_right_align(cardbox->text[3],true);
        compo_cardbox_text_set_forecolor(cardbox,3, COLOR_GRAY);

        compo_setid(cardbox, COMPO_ID_CARD_START+i);
    }

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_ADD]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, CARD_Y + CARD_H * worldClockListTotalNumeber + 90 / 2,300,88);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox, i18n[STR_APP_ADD]);
    page_type = DATA_PAGE;
    return frm;
}

compo_form_t *func_world_clock_form_create(void)
{
    compo_form_t *frm = NULL;
    ute_module_timezonesetting_data_t worldClockData;
    uteModuleTimeZoneSettingGetData(&worldClockData);

    worldClockListTotalNumeber = worldClockData.number;
    WCLOCK_DEBUG("line %d ,worldClockListTotalNumeber = %d\n",__LINE__,worldClockListTotalNumeber);
    WCLOCK_DEBUG("line %d ,worldClockListTotalcrc = %d\n",__LINE__,worldClockData.receiveCrc);
    if(worldClockListTotalNumeber == 0)
    {
        frm = func_world_clock_default_create();
    }
    else
    {
        frm = func_world_clock_date_create();
    }
    __this = frm;
    return frm;
}

static void func_world_clock_process(void)
{
    f_world_time_t* f_world_time = (f_world_time_t *)func_cb.f_cb;
    ute_module_timezonesetting_data_t worldClockData;
    uteModuleTimeZoneSettingGetData(&worldClockData);
    static uint8_t crc = 0;
    if(worldClockData.number)
    {
        for (int i=0; i<worldClockData.number; i++)
        {
            compo_cardbox_t *card = compo_getobj_byid(COMPO_ID_CARD_START + i);
            compo_cardbox_text_scroll_process(card, true);
        }
    }
    if (f_world_time->ptm &&  page_type == DATA_PAGE)
    {
        compo_page_move_process(f_world_time->ptm);
    }
    if(crc != worldClockData.receiveCrc || (worldClockData.number == 0 && page_type == DATA_PAGE))
    {
        WCLOCK_DEBUG("line %d ,crc = %d,worldClockListTotalcrc = %d\n",__LINE__,crc,worldClockData.receiveCrc);
        crc = worldClockData.receiveCrc;
        compo_form_destroy(__this);
        func_cb.frm_main = func_world_clock_form_create();
        __this = func_cb.frm_main;
        f_world_time->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
        page_move_info_t info =
        {
            .title_used = true,
            .page_size =  worldClockListTotalNumeber * CARD_H + 120,
            .page_count = 1,
            .quick_jump_perc = 40,
        };
        compo_page_move_init(f_world_time->ptm, func_cb.frm_main->page_body, &info);
    }
    func_process();
}


//时钟表盘右滑菜单功能消息处理
static void func_world_clock_message(size_msg_t msg)
{
    f_world_time_t* f_world_time = (f_world_time_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if (f_world_time->ptm)
            {
                compo_page_move_touch_handler(f_world_time->ptm);
            }
            break;
        case MSG_CTP_CLICK:
            break;
        default:
            func_message(msg);
            break;
    }
}

//创建界面
static void func_world_clock_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_world_time_t));
    func_cb.frm_main = func_world_clock_form_create();
    f_world_time_t* f_world_time = (f_world_time_t *)func_cb.f_cb;

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_world_time->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = worldClockListTotalNumeber * CARD_H + 120,
        .page_count = 1,
        .quick_jump_perc = 40,
    };
    compo_page_move_init(f_world_time->ptm, func_cb.frm_main->page_body, &info);
#endif
}

static func_world_clock_exit(void)
{
    worldClockListTotalNumeber = 0;
    func_cb.last = FUNC_WORLD_CLOCK;

    f_world_time_t* f_world_time = (f_world_time_t *)func_cb.f_cb;
    if (f_world_time->ptm)
    {
        func_free(f_world_time->ptm);
    }
}
#else
compo_form_t *func_world_clock_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    return frm;
}

static void func_world_clock_process(void)
{
    func_process();
}


//时钟表盘右滑菜单功能消息处理
static void func_world_clock_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;

        case MSG_CTP_SHORT_LEFT:
            break;

        default:
            func_message(msg);
            break;
    }
}

//创建界面
static void func_world_clock_enter(void)
{
    func_cb.frm_main = func_world_clock_form_create();
}

static func_world_clock_exit(void)
{

}

#endif

//血氧功能
void func_world_clock(void)
{
    printf("%s\n", __func__);
    func_world_clock_enter();
    while (func_cb.sta == FUNC_WORLD_CLOCK)
    {
        func_world_clock_process();
        func_world_clock_message(msg_dequeue());
    }
    func_world_clock_exit();
}
#endif //UTE_MODULE_SCREENS_WORLD_CLOCK_SUPPORT