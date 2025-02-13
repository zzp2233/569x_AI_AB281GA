#include "include.h"
#include "func.h"


typedef struct f_calendar_t_
{
    u16 today_year;
    u8 today_mon;
    u8 today_day;
    u16 update_year;
    u8 update_mon;
} f_calendar_t;

enum
{
    CALE_SUNDAY = 0,
    CALE_MONDAY,
    CALE_TUESDAY,
    CALE_WEDNESDAY,
    CALE_THURSDAY,
    CALE_FRIDAY,
    CALE_SATURDAY,
} e_cale_week;


#define CALE_CONTEXT_X_START_GAP    10  //x方向边界间隙
#define CALE_CONTEXT_X_GAP          15  //间隔间隙
#define CALE_CONTEXT_WIDTH          ((GUI_SCREEN_WIDTH - 6*CALE_CONTEXT_X_GAP - 2*CALE_CONTEXT_X_START_GAP) / 7) //宽度

#define CALE_CONTEXT_y_START_GAP    170 //y方向上边界间隙
#define CALE_CONTEXT_y_GAP          14  //间隔间隙
#define CALE_CONTEXT_HEIGHT         21  //字高

#define CALE_CONTEXT_MAX            35
#define CALE_CONTEXT_NUM_COUNT_MAX  2


//判断闰年
#define IS_LEAP_YEAR(year)                          (!((year) % 400) || (((year) % 100) && !((year) % 4)))

//基姆拉尔森计算公式，求某天的星期(当月份为1月或2月时，当作上一年的13月和14月)
#define CAL_DAY_OF_WEEK(year, month, day)     (uint16_t)(((day) + 1 + 2 * (month) + 3 * (month + 1) / 5 + \
                                                                (year) + (year) / 4 - (year) / 100 + (year) / 400) % 7)

enum
{
    //文本
    COMPO_ID_YEAR_TEXT = 1,
    COMPO_ID_MON_TEXT,
    COMPO_ID_DATE_TEXT_START,
    COMPO_ID_DATE_TEXT_END = 44,

    //按钮
    COMPO_ID_LAST_BTN,
    COMPO_ID_NEXT_BTN,
};

//根据当前月获取上个月
static uint8_t cal_last_month(uint8_t cur_month)
{
    return (uint8_t)(cur_month > 1 ? (--cur_month) : 12);
}

//获取每个月最大天数
static uint8_t cal_max_of_days_per_month(uint16_t year, uint8_t month)
{
    uint8_t day_max;

    switch(month)
    {
        case 1:
        case 3:
        case 5:
        case 7:
        case 8:
        case 10:
        case 12:
            day_max = 31;
            break;

        case 4:
        case 6:
        case 9:
        case 11:
            day_max = 30;
            break;

        case 2:
            day_max = IS_LEAP_YEAR(year) ? 29 : 28;
            break;

        default:
            day_max = 30;
            break;
    }

    return day_max;
}


//刷新日历内容
static void func_calender_refresh(uint16_t year, uint8_t month, uint8_t today_day)
{
    component_t *comop;
    uint16_t frist_day_week;
    uint8_t day;
    uint8_t day_max;
    uint8_t last_day_max;
    uint8_t i;
    char buf[3] = {0};

    day = 1;
    day_max = cal_max_of_days_per_month(year, month);
    last_day_max = cal_max_of_days_per_month(year, cal_last_month(month));
    if(1 == month || 2 == month)
    {
        month += 12;
        year --;
    }
    frist_day_week = CAL_DAY_OF_WEEK(year, month, 1);

    for(i = 0; i < CALE_CONTEXT_MAX; i++)
    {
        comop = compo_getobj_byid(COMPO_ID_DATE_TEXT_START + i);
        if(comop)
        {
            if(i < frist_day_week)
            {
                buf[0] = 48 + (last_day_max - frist_day_week + i + 1) / 10;
                buf[1] = 48 + (last_day_max - frist_day_week + i + 1) % 10;
                compo_label_set_forecolor((compo_label_t *)comop, COLOR_GRAY);
                compo_label_set((compo_label_t *)comop, buf);
            }
            else
            {
                if(day <= day_max)
                {
                    if(day < 10)
                    {
                        buf[0] = 48 + day;
                        buf[1] = 0;
                    }
                    else
                    {
                        buf[0] = 48 + day / 10;
                        buf[1] = 48 + day % 10;
                    }
                    if(today_day == day)
                    {
                        compo_label_set_forecolor((compo_label_t *)comop, COLOR_RED);
                    }
                    else
                    {
                        compo_label_set_forecolor((compo_label_t *)comop, COLOR_WHITE);
                    }
                    compo_label_set((compo_label_t *)comop, buf);
                }
                else
                {
                    if((day - day_max) < 10)
                    {
                        buf[0] = 48 + (day - day_max);
                        buf[1] = 0;
                    }
                    else
                    {
                        buf[0] = 48 + (day - day_max) / 10;
                        buf[1] = 48 + (day - day_max) % 10;
                    }
                    compo_label_set_forecolor((compo_label_t *)comop, COLOR_GRAY);
                    compo_label_set((compo_label_t *)comop, buf);
                }

                day ++;
            }
        }
    }
}

//创建日历主界面
compo_form_t *func_calender_form_create(void)
{
    uint8_t i;
    compo_label_t *cale_label;
    s16 x_pos = CALE_CONTEXT_X_START_GAP + CALE_CONTEXT_WIDTH / 2;
    s16 y_pos = CALE_CONTEXT_y_START_GAP + CALE_CONTEXT_HEIGHT / 2;
    char week_text[7][4] = {"日", "一", "二", "三", "四", "五", "六"};

    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE);
    compo_form_set_title(frm, i18n[STR_SETTING_CALENDAR]);

    //新建日历文本内容
    for(i = 0; i < CALE_CONTEXT_MAX; i++)
    {
        if(!(i % 7))
        {
            x_pos = CALE_CONTEXT_X_START_GAP + CALE_CONTEXT_WIDTH / 2;
            y_pos = CALE_CONTEXT_y_START_GAP + CALE_CONTEXT_HEIGHT / 2 + (i / 7) * (CALE_CONTEXT_HEIGHT + CALE_CONTEXT_y_GAP);
        }
        cale_label = compo_label_create(frm, CALE_CONTEXT_NUM_COUNT_MAX);
        compo_label_set_font(cale_label, UI_BUF_0FONT_FONT_ASC_BIN);
        compo_setid(cale_label, COMPO_ID_DATE_TEXT_START + i);
        compo_label_set_pos(cale_label, x_pos, y_pos);
        x_pos += (CALE_CONTEXT_WIDTH + CALE_CONTEXT_X_GAP);
    }

    //新建（日 一 二 三 四 五 六）文本
    x_pos = CALE_CONTEXT_X_START_GAP + CALE_CONTEXT_WIDTH / 2;
    y_pos = CALE_CONTEXT_y_START_GAP - 40;
    for(i = 0; i < 7; i++)
    {
        cale_label = compo_label_create(frm, sizeof(week_text[i]));
        compo_label_set_pos(cale_label, x_pos, y_pos);
        compo_label_set(cale_label, week_text[i]);
        x_pos += (CALE_CONTEXT_WIDTH + CALE_CONTEXT_X_GAP);
    }

    compo_shape_t *line_shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(line_shape, GUI_SCREEN_CENTER_X, y_pos + CALE_CONTEXT_HEIGHT, GUI_SCREEN_WIDTH, 2);
    compo_shape_set_color(line_shape, COLOR_GRAY);

    //year_text
    cale_label = compo_label_create(frm, 4);
    compo_label_set_font(cale_label, UI_BUF_0FONT_FONT_ASC_BIN);
    compo_label_set_pos(cale_label, GUI_SCREEN_CENTER_X - 40, CALE_CONTEXT_y_START_GAP - 90);
    compo_label_set(cale_label, "2023");
    compo_setid(cale_label, COMPO_ID_YEAR_TEXT);

    //mon_text
    cale_label = compo_label_create(frm, 2);
    compo_label_set_font(cale_label, UI_BUF_0FONT_FONT_ASC_BIN);
    compo_label_set_pos(cale_label, GUI_SCREEN_CENTER_X + 40, CALE_CONTEXT_y_START_GAP - 90);
    compo_label_set(cale_label, "11");
    compo_setid(cale_label, COMPO_ID_MON_TEXT);

    //last_btn
    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_COMMON_REDUCE_CLICK_BIN);
    compo_setid(btn, COMPO_ID_LAST_BTN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X - 110, CALE_CONTEXT_y_START_GAP - 90);

    //next_btn
    btn = compo_button_create_by_image(frm, UI_BUF_COMMON_INCREASE_CLICK_BIN);
    compo_setid(btn, COMPO_ID_NEXT_BTN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X + 110, CALE_CONTEXT_y_START_GAP - 90);

    return frm;
}

//切换日期并刷新
static void func_calendar_date_update(bool next)
{
    f_calendar_t *time = (f_calendar_t *)func_cb.f_cb;
    component_t *comop;
    uint8_t today_day;
    char buf[8];

    if(next)
    {
        time->update_mon ++;
        if(time->update_mon > 12)
        {
            time->update_mon = 1;
            time->update_year ++;
        }
    }
    else
    {
        time->update_mon --;
        if(0 == time->update_mon)
        {
            time->update_mon = 12;
            time->update_year --;
        }
    }

    comop = compo_getobj_byid(COMPO_ID_YEAR_TEXT);
    snprintf(buf, sizeof(buf), "%d", time->update_year);
    compo_label_set((compo_label_t *)comop, buf);

    comop = compo_getobj_byid(COMPO_ID_MON_TEXT);
    snprintf(buf, sizeof(buf), "%d", time->update_mon);
    compo_label_set((compo_label_t *)comop, buf);

    //刷新当前日期
    today_day = 0;
    if(time->today_year == time->update_year && time->today_mon == time->update_mon)
    {
        today_day = time->today_day;
    }

    //刷新日历内容
    func_calender_refresh(time->update_year, time->update_mon, today_day);
}

//单击按钮
static void func_calendar_button_click(void)
{
    int id = compo_get_button_id();

    if(COMPO_ID_LAST_BTN == id)
    {
        func_calendar_date_update(false);
    }
    else if(COMPO_ID_NEXT_BTN == id)
    {
        func_calendar_date_update(true);
    }
}

//公共事件处理
static void func_calendar_comm_process(void)
{
    func_process();
}

//消息处理
static void func_calendar_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_calendar_button_click();
            break;

        case MSG_QDEC_FORWARD:
            func_calendar_date_update(true);
            break;

        case MSG_QDEC_BACKWARD:
            func_calendar_date_update(false);
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入日历功能
static void func_calendar_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_calendar_t));
    f_calendar_t *f_calendar = (f_calendar_t *)func_cb.f_cb;
    f_calendar->today_year = 2023;
    f_calendar->today_mon = 11;
    f_calendar->today_day = 22;

    f_calendar->update_year = f_calendar->today_year;
    f_calendar->update_mon = f_calendar->today_mon;

    func_cb.frm_main = func_calender_form_create();
    //刷新文本内容
    func_calender_refresh(2023, 11, 22);
}

//退出日历功能
static void func_calendar_exit(void)
{
    func_cb.last = FUNC_CALENDAER;
}

//日历功能
void func_calendar(void)
{
    printf("%s\n", __func__);
    func_calendar_enter();
    while (func_cb.sta == FUNC_CALENDAER)
    {
        func_calendar_comm_process();
        func_calendar_message(msg_dequeue());
    }
    func_calendar_exit();
}


