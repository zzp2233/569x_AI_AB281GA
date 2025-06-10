#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"

#if UTE_MODULE_SCREENS_ALARM_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if (USE_APP_TYPE == USE_AB_APP)
#define ALARM_ENABLE_CNT()              alarm_num_get()
#define ALARM_ENABLE(idx, en)           alarm_clock_enable(idx, en)
#define ALARM_GET_HOUR(idx)             app_data.sector0.alarm[idx].hour
#define ALARM_GET_MIN(idx)              app_data.sector0.alarm[idx].minute
#define ALARM_GET_CYCLE(idx)            app_data.sector0.alarm[idx].cycle
#define ALARM_GET_SWITCH(idx)           app_data.sector0.alarm[idx].swi
#define ALARM_CLOCK_NUM_MAX             ALARM_NUM_MAX
#elif (USE_APP_TYPE == USE_UTE_APP)
#define ALARM_ENABLE_CNT()              uteModuleSystemtimeGetAlarmTotalCnt()
#define ALARM_ENABLE(idx, en)           uteModuleSystemtimeEnableAlarm(idx, en)
#define ALARM_GET_HOUR(idx)             uteModuleSystemtimeGetAlarmHour(idx)
#define ALARM_GET_MIN(idx)              uteModuleSystemtimeGetAlarmMin(idx)
#define ALARM_GET_CYCLE(idx)            uteModuleSystemtimeGetAlarmCycle(idx)
#define ALARM_GET_SWITCH(idx)           uteModuleSystemtimeGetAlarmEnableState(idx)
#define ALARM_CLOCK_NUM_MAX             uteModuleSystemtimeGetAlarmMaxCnt()
#else
#define ALARM_ENABLE_CNT()              0
#define ALARM_ENABLE(idx, en)
#define ALARM_GET_HOUR(idx)             0
#define ALARM_GET_MIN(idx)              0
#define ALARM_GET_CYCLE(idx)            0
#define ALARM_GET_SWITCH(idx)           0
#define ALARM_CLOCK_NUM_MAX             0
#endif

#define MAKE_GRAY(lv)                   make_color(lv, lv, lv)

#define ACLOCK_LIST_CNT                 ((int)(sizeof(tbl_aclock_list) / sizeof(tbl_aclock_list[0])))

static void func_alarm_clock_exit(void);
static void func_alarm_clock_enter(void);

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_ADD = 1,

    //图像
    COMPO_ID_PIC_ADD_CLICK,

    //卡片
    COMPO_ID_CARD_0,
};

typedef struct f_alarm_clock_t_
{
    page_tp_move_t *ptm;
    bool time_scale;        //时间制 0:24小时; 1:12小时
} f_alarm_clock_t;

static widget_icon_t *icon_add;

typedef struct func_alarm_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_alarm_hour_format_t;

static func_alarm_hour_format_t func_alarm_convert_to_12hour(s8 hour24)
{
    u8 am_pm = (hour24 >= 12) ? 2 : 1;    //2 PM, 1 AM
    func_alarm_hour_format_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (hour24 == 0)
        {
            hour12.hour = 12;
        }
        else if (hour24 > 12)
        {
            hour12.hour = hour24 - 12;
        }
        else
        {
            hour12.hour = hour24;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = hour24;
    hour12.am_pm = 0;
    return hour12;
}

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

#define WEEK_LOCATION_X  (-80)
#define WEEK_LOCATION_Y  (15)
#define WEEK_SPACING_X   (23)

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_form_create(void)
{
    char str_buff[50];
    const uint32_t clock_off_day[7]=
    {
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_00_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_01_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_02_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_03_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_04_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_05_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_06_BIN,
    };
    const uint32_t clock_on_day[7]=
    {
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_07_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_08_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_09_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_10_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_11_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_12_BIN,
        UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_CHAR_WEEK_22X22_X20_Y97_X46_Y97_X72_Y97_X98_Y97_X124_Y97_X150_Y97_X176_Y97_13_BIN,
    };

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK]);

    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {
            compo_cardbox_t *cardbox = compo_cardbox_create(frm,1,1,1,228,80);
            compo_cardbox_set_pos(cardbox,GUI_SCREEN_CENTER_X,84+(i*86));
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_rect_set_location(cardbox,0,0,0,228,80,10);
            compo_cardbox_rect_set_color(cardbox, 0,make_color(36,36,36));

            compo_cardbox_icon_set_pos(cardbox,0,80,-15);
            compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_01_OPEN_BIN:UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_00_CLOSE_BIN);

            memset(str_buff,0,sizeof(str_buff));
            func_alarm_hour_format_t clock_time = func_alarm_convert_to_12hour(ALARM_GET_HOUR(i));
            snprintf(str_buff, sizeof(str_buff), "%02d:%02d %s", clock_time.hour, ALARM_GET_MIN(i),clock_time.am_pm ? (clock_time.am_pm ==1 ? i18n[STR_AM] : i18n[STR_PM]) : " ");
            compo_cardbox_text_set_location(cardbox,0,WEEK_LOCATION_X-10,-30,110,33);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set_font(0,0, UI_BUF_0FONT_FONT_NUM_32_BIN);
            compo_cardbox_text_set_align_center(cardbox,0, false);
            compo_cardbox_text_set(cardbox,0,str_buff);


            if (ALARM_GET_CYCLE(i) & BIT(7))
            {
                compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(i18n[STR_ONCE]));
                compo_textbox_set_pos(textbox, WEEK_LOCATION_X-8, WEEK_LOCATION_Y-15);
                compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set(textbox,i18n[STR_ONCE]);
            }
            else if (ALARM_GET_CYCLE(i) == 0x7f)
            {
                compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(i18n[STR_EVERY_DAY]));
                compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
                compo_textbox_set_pos(textbox, WEEK_LOCATION_X-8, WEEK_LOCATION_Y-15);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set(textbox,i18n[STR_EVERY_DAY]);
            }
            else
            {
                for (u8 j = 0; j < 7; j++)
                {

                    compo_picturebox_t *picbox = compo_picturebox_create_for_page(frm,cardbox->page,0);
                    compo_picturebox_set_pos(picbox, WEEK_LOCATION_X+(WEEK_SPACING_X*j), WEEK_LOCATION_Y);
                    compo_picturebox_set(picbox,(ALARM_GET_CYCLE(i) & BIT(j)) ? clock_on_day[j] : clock_off_day[j]);

                }
            }
        }

        // widget_page_t *page = widget_page_create(frm->page);
        // widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-40, GUI_SCREEN_WIDTH, 80);

        // compo_shape_t * shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);
        // compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 45, GUI_SCREEN_WIDTH, 80);
        // compo_shape_set_color(shape,COLOR_BLACK);

        compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ADD_208X52_X16_Y222_BIN);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,84+(ALARM_ENABLE_CNT()*86)-5);
        compo_setid(btn,COMPO_ID_BTN_ADD);

    }
    else
    {
        compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I335001_20_ALARM_CLOCK_1_ADD_ALARM_ICON_ADD_ALARM_100X100_X70_Y76_BIN);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,50+76);
        compo_setid(btn,COMPO_ID_BTN_ADD);

        compo_textbox_t *textbox = compo_textbox_create(frm,strlen(i18n[STR_ADD_CLOCK]));
        compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,213,230,25);
        compo_textbox_set(textbox,i18n[STR_ADD_CLOCK]);
    }



    return frm;
}
//实时刷新，解决再当前界面时单次闹钟响铃后，闹钟开关状态不更新问题
static void func_alarm_clock_disp_handle(void)
{
    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        compo_cardbox_t *cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        {
            //刷新
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_01_OPEN_BIN:UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_00_CLOSE_BIN);
        }
    }
}
//单击按钮
static void func_alarm_clock_button_click(void)
{
    rect_t rect;
    point_t pt = ctp_get_sxy();
    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        compo_cardbox_t *cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        if (compo_cardbox_btn_is(cardbox, pt))
        {
            rect_t rect = compo_cardbox_get_icon_absolute(cardbox, 0); //上一首
            if (abs_s(pt.x - rect.x) <= rect.wid && abs_s(pt.y - rect.y)  <= rect.hei*2)
                //if (pt.x > (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_01_OPEN_BIN).wid))   //开关
            {
                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
                //刷新
                compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
                compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_01_OPEN_BIN:UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_00_CLOSE_BIN);
            }
            else        //编辑
            {
                sys_cb.alarm_edit_idx = i;
                uteTaskGuiStartScreen(FUNC_ALARM_CLOCK_SUB_EDIT, 0, __func__);
            }
        }
    }

    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_ADD:
            if(SYSTEM_TIME_ALARMS_MAX_CNT == ALARM_ENABLE_CNT())
            {
                msgbox(i18n[STR_ADD_CLOCK_NO_MORE], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
                return;
            }
            uteTaskGuiStartScreen(FUNC_ALARM_CLOCK_SUB_SET, 0, __func__);
            break;

        default:
            break;
    }

}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

#define WEEK_LOCATION_X  (-133)
#define WEEK_LOCATION_Y  (25)
#define WEEK_SPACING_X   (38)

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_form_create(void)
{
    char str_buff[50];
    const uint32_t clock_off_day[7]=
    {
        UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_00_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_01_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_02_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_03_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_04_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_05_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_DEFAULT_06_BIN,
    };
    const uint32_t clock_on_day[7]=
    {
        UI_BUF_I341001_20_ALARM_CLOCK_STATE_00_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_STATE_01_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_STATE_02_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_STATE_03_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_STATE_04_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_STATE_05_BIN,
        UI_BUF_I341001_20_ALARM_CLOCK_STATE_06_BIN,
    };

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK]);

    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {
            compo_cardbox_t *cardbox = compo_cardbox_create(frm,1,1,1,346,120);
            compo_cardbox_set_pos(cardbox,GUI_SCREEN_CENTER_X,80+60+(i*132));
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_rect_set_location(cardbox,0,0,0,346,120,10);
            compo_cardbox_rect_set_color(cardbox, 0,make_color(36,36,36));

            compo_cardbox_icon_set_pos(cardbox,0,121,-35);
            compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I341001_20_ALARM_CLOCK_OPEN_BIN:UI_BUF_I341001_20_ALARM_CLOCK_GRAY_BUTTON_BIN);

            memset(str_buff,0,sizeof(str_buff));
            func_alarm_hour_format_t clock_time = func_alarm_convert_to_12hour(ALARM_GET_HOUR(i));
            snprintf(str_buff, sizeof(str_buff), "%02d:%02d %s", clock_time.hour, ALARM_GET_MIN(i),clock_time.am_pm ? (clock_time.am_pm ==1 ? i18n[STR_AM] : i18n[STR_PM]) : " ");
            compo_cardbox_text_set_location(cardbox,0,WEEK_LOCATION_X-20,-50,180,35);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            //compo_cardbox_text_set_font(cardbox,0, UI_BUF_0FONT_FONT_NUM_38_BIN);
            compo_cardbox_text_set_align_center(cardbox,0, false);
            compo_cardbox_text_set(cardbox,0,str_buff);


            if (ALARM_GET_CYCLE(i) & BIT(7))
            {
                compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(i18n[STR_ONCE]));
                compo_textbox_set_pos(textbox, WEEK_LOCATION_X-8, WEEK_LOCATION_Y-15);
                compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set(textbox,i18n[STR_ONCE]);
            }
            else if (ALARM_GET_CYCLE(i) == 0x7f)
            {
                compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(i18n[STR_EVERY_DAY]));
                compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
                compo_textbox_set_pos(textbox, WEEK_LOCATION_X-8, WEEK_LOCATION_Y-15);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set(textbox,i18n[STR_EVERY_DAY]);
            }
            else
            {
                for (u8 j = 0; j < 7; j++)
                {

                    compo_picturebox_t *picbox = compo_picturebox_create_for_page(frm,cardbox->page,0);
                    compo_picturebox_set_pos(picbox, WEEK_LOCATION_X+(WEEK_SPACING_X*j), WEEK_LOCATION_Y);
                    compo_picturebox_set(picbox,(ALARM_GET_CYCLE(i) & BIT(j)) ? clock_on_day[j] : clock_off_day[j]);

                }
            }
        }

        // widget_page_t *page = widget_page_create(frm->page);
        // widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-40, GUI_SCREEN_WIDTH, 80);

        // compo_shape_t * shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);
        // compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 45, GUI_SCREEN_WIDTH, 80);
        // compo_shape_set_color(shape,COLOR_BLACK);

        compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I341001_20_ALARM_CLOCK_ADD2_BIN);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,84 + 40 +(ALARM_ENABLE_CNT()*132));
        compo_setid(btn,COMPO_ID_BTN_ADD);

    }
    else
    {
        compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_ADD_BIN);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,122+77);
        compo_setid(btn,COMPO_ID_BTN_ADD);

        compo_textbox_t *textbox = compo_textbox_create(frm,strlen(i18n[STR_ADD_CLOCK]));
        compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,309+26,330,34);
        compo_textbox_set(textbox,i18n[STR_ADD_CLOCK]);
    }



    return frm;
}

//单击按钮
static void func_alarm_clock_button_click(void)
{
    rect_t rect;
    point_t pt = ctp_get_sxy();
    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        compo_cardbox_t *cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        if (compo_cardbox_btn_is(cardbox, pt))
        {
            rect_t rect = compo_cardbox_get_icon_absolute(cardbox, 0); //上一首
            if (abs_s(pt.x - rect.x) <= rect.wid && abs_s(pt.y - rect.y)  <= rect.hei*2)
                //if (pt.x > (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_01_OPEN_BIN).wid))   //开关
            {
                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
                //刷新
                compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
                compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I341001_20_ALARM_CLOCK_OPEN_BIN:UI_BUF_I341001_20_ALARM_CLOCK_GRAY_BUTTON_BIN);
            }
            else        //编辑
            {
                sys_cb.alarm_edit_idx = i;
                uteTaskGuiStartScreen(FUNC_ALARM_CLOCK_SUB_EDIT, 0, __func__);
            }
        }
    }

    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_ADD:
            if(SYSTEM_TIME_ALARMS_MAX_CNT == ALARM_ENABLE_CNT())
            {
                msgbox(i18n[STR_ADD_CLOCK_NO_MORE], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
                return;
            }
            uteTaskGuiStartScreen(FUNC_ALARM_CLOCK_SUB_SET, 0, __func__);
            break;

        default:
            break;
    }

}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

#define WEEK_LOCATION_X  (-115)
#define WEEK_LOCATION_Y  (78-55)
#define WEEK_SPACING_X   (32)

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_form_create(void)
{
    char str_buff[50];
    const uint32_t clock_off_day[7]=
    {
        UI_BUF_I338001_20_ALARM_CLOCK_DEFAULT_00_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_DEFAULT_01_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_DEFAULT_02_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_DEFAULT_03_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_DEFAULT_04_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_DEFAULT_05_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_DEFAULT_06_BIN,
    };
    const uint32_t clock_on_day[7]=
    {
        UI_BUF_I338001_20_ALARM_CLOCK_STATE_00_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_STATE_01_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_STATE_02_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_STATE_03_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_STATE_04_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_STATE_05_BIN,
        UI_BUF_I338001_20_ALARM_CLOCK_STATE_06_BIN,
    };

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK]);

    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {
            compo_cardbox_t *cardbox = compo_cardbox_create(frm,0,1,1,260,78);
            compo_cardbox_set_pos(cardbox,GUI_SCREEN_CENTER_X,83+(i*105));
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_icon_set_pos(cardbox,0,103,-22);
            compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I338001_20_ALARM_CLOCK_OPEN_BIN:UI_BUF_I338001_20_ALARM_CLOCK_GRAY_BUTTON_BIN);

            memset(str_buff,0,sizeof(str_buff));
            func_alarm_hour_format_t clock_time = func_alarm_convert_to_12hour(ALARM_GET_HOUR(i));
            snprintf(str_buff, sizeof(str_buff), "%02d:%02d %s", clock_time.hour, ALARM_GET_MIN(i),clock_time.am_pm ? (clock_time.am_pm ==1 ? i18n[STR_AM] : i18n[STR_PM]) : " ");
            compo_cardbox_text_set_location(cardbox,0,WEEK_LOCATION_X-10,-30,110,33);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set_font(0,0, UI_BUF_0FONT_FONT_NUM_32_BIN);
            compo_cardbox_text_set_align_center(cardbox,0, false);
            compo_cardbox_text_set(cardbox,0,str_buff);


            if (ALARM_GET_CYCLE(i) & BIT(7))
            {
                compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(i18n[STR_ONCE]));
                compo_textbox_set_pos(textbox, WEEK_LOCATION_X-14, WEEK_LOCATION_Y-14);
                compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set(textbox,i18n[STR_ONCE]);
            }
            else if (ALARM_GET_CYCLE(i) == 0x7f)
            {
                compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(i18n[STR_EVERY_DAY]));
                compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
                compo_textbox_set_pos(textbox, WEEK_LOCATION_X-14, WEEK_LOCATION_Y-14);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set(textbox,i18n[STR_EVERY_DAY]);
            }
            else
            {
                for (u8 j = 0; j < 7; j++)
                {

                    compo_picturebox_t *picbox = compo_picturebox_create_for_page(frm,cardbox->page,0);
                    compo_picturebox_set_pos(picbox, WEEK_LOCATION_X+(WEEK_SPACING_X*j), WEEK_LOCATION_Y);
                    compo_picturebox_set(picbox,(ALARM_GET_CYCLE(i) & BIT(j)) ? clock_on_day[j] : clock_off_day[j]);

                }
            }
        }

        compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I338001_20_ALARM_CLOCK_ADD_BIN);
        compo_button_set_location(btn, GUI_SCREEN_CENTER_X,84+(ALARM_ENABLE_CNT()*105)-5,80,80);
        compo_setid(btn,COMPO_ID_BTN_ADD);

    }
    else
    {
        compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I338001_20_ALARM_CLOCK_ADD_BIN);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,108+60);
        compo_setid(btn,COMPO_ID_BTN_ADD);

        compo_textbox_t *textbox = compo_textbox_create(frm,strlen(i18n[STR_ADD_CLOCK]));
        compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,21+274,230,25);
        compo_textbox_set(textbox,i18n[STR_ADD_CLOCK]);
    }



    return frm;
}

//单击按钮
static void func_alarm_clock_button_click(void)
{
    rect_t rect;
    point_t pt = ctp_get_sxy();
    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        compo_cardbox_t *cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        if (compo_cardbox_btn_is(cardbox, pt))
        {
            rect_t rect = compo_cardbox_get_icon_absolute(cardbox, 0); //上一首
            if (abs_s(pt.x - rect.x) <= rect.wid && abs_s(pt.y - rect.y)  <= rect.hei*2)
                //if (pt.x > (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_I335001_20_ALARM_CLOCK_2_1_ALARM_CLOCK_ICON_ALARM_40X20_X176_Y65_X176_Y151_01_OPEN_BIN).wid))   //开关
            {
                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
                //刷新
                compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
                compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I338001_20_ALARM_CLOCK_OPEN_BIN:UI_BUF_I338001_20_ALARM_CLOCK_GRAY_BUTTON_BIN);
            }
            else        //编辑
            {
                sys_cb.alarm_edit_idx = i;
                uteTaskGuiStartScreen(FUNC_ALARM_CLOCK_SUB_EDIT, 0, __func__);
            }
        }
    }

    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_ADD:
            if(SYSTEM_TIME_ALARMS_MAX_CNT == ALARM_ENABLE_CNT())
            {
                msgbox(i18n[STR_ADD_CLOCK_NO_MORE], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
                return;
            }
            uteTaskGuiStartScreen(FUNC_ALARM_CLOCK_SUB_SET, 0, __func__);
            break;

        default:
            break;
    }

}
#elif  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK]);

    //闹钟选项卡
    compo_cardbox_t *cardbox;
    int buf_num=0;
    int str_week_buf[7]=
    {
        STR_MONDAY, // 周一
        STR_TUESDAY, // 周二
        STR_WEDNESDAY, // 周三
        STR_THURSDAY, // 周四
        STR_FRIDAY, // 周五
        STR_SATURDAY, // 周六
        STR_SUNDAY, // 周日
    };

    char *str_buff = NULL;
    uint16_t str_buff_size = 0;

    for (uint8_t i = 0; i < 7; i++)
    {
        str_buff_size += strlen(i18n[str_week_buf[i]]) + 2;
    }

    if(str_buff_size < MAX(strlen(i18n[STR_ONCE]) + 2, strlen(i18n[STR_EVERY_DAY]) + 2))
    {
        str_buff_size = MAX(strlen(i18n[STR_ONCE]) + 2, strlen(i18n[STR_EVERY_DAY]) + 2);
    }

    str_buff = (char *)uteModulePlatformMemoryAlloc(str_buff_size);

    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {
            memset(str_buff,0,str_buff_size);
            buf_num=0;
            cardbox = compo_cardbox_create(frm, 1, 1, 2, GUI_SCREEN_WIDTH - 10, GUI_SCREEN_HEIGHT/4);
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT/4 + (GUI_SCREEN_HEIGHT/4 + 4) * i);
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I330001_PUBLIC_SWITCH01_BIN : UI_BUF_I330001_PUBLIC_SWITCH00_BIN);
            compo_cardbox_icon_set_pos(cardbox, 0,
                                       (GUI_SCREEN_WIDTH - 10) / 2 - gui_image_get_size(UI_BUF_I330001_PUBLIC_SWITCH01_BIN).wid / 2 - 2, 0);

            snprintf(str_buff, str_buff_size, "%02d:%02d", func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).hour, ALARM_GET_MIN(i));
            compo_cardbox_text_set_font(cardbox, 0, UI_BUF_0FONT_FONT_NUM_32_BIN);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set(cardbox, 0, str_buff);
            compo_cardbox_text_set_align_center(cardbox, 0, false);
            compo_cardbox_text_set_location(cardbox, 0, -100, -30, 180, 50);

            // compo_cardbox_text_set_font(cardbox, 1, UI_BUF_0FONT_FONT_BIN);
            // compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            // compo_cardbox_text_set_align_center(cardbox, 1, false);
            // compo_cardbox_text_set_location(cardbox, 1, -100 + widget_text_get_area(cardbox->text[0]).wid + 10, -25, 80, 50);
            // compo_cardbox_text_set_visible(cardbox, 1, true);
            if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 0)
            {
                compo_cardbox_text_set_visible(cardbox, 1, false);
            }
            else if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 1)           //AM
            {
                compo_cardbox_text_set(cardbox, 1, i18n[STR_AM]);
            }
            else if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 2)           //PM
            {
                compo_cardbox_text_set(cardbox, 1, i18n[STR_PM]);
            }


            memset(str_buff,0,str_buff_size);

            if (ALARM_GET_CYCLE(i) & BIT(7))
            {
                snprintf(str_buff, str_buff_size, i18n[STR_ONCE]);
            }
            else if (ALARM_GET_CYCLE(i) == 0x7f)
            {
                snprintf(str_buff, str_buff_size, i18n[STR_EVERY_DAY]);
            }
            else
            {
                for (u8 j = 0; j < 7; j++)
                {
                    if (ALARM_GET_CYCLE(i) & BIT(j))
                    {
                        const char *week_str = i18n[str_week_buf[j]];
                        uint8_t week_str_len = strlen(week_str);
                        if (buf_num + week_str_len + 1 <= str_buff_size)
                        {
                            memcpy(&str_buff[buf_num], week_str, week_str_len);
                            buf_num += week_str_len;
                            str_buff[buf_num++] = ' ';
                        }
                    }
                }
            }

            compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(str_buff));
            compo_textbox_set_align_center(textbox, false);
            compo_textbox_set_location(textbox, -100, 10, 160, 40);
            compo_textbox_set(textbox,str_buff);
            compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));

            // compo_cardbox_text_set_forecolor(cardbox, 1, MAKE_GRAY(128));
            // compo_cardbox_text_set(cardbox, 1, str_buff);
            // compo_cardbox_text_set_align_center(cardbox, 1, false);
            // compo_cardbox_text_set_location(cardbox, 1, -100, 10, 160, 40);

            compo_cardbox_rect_set_color(cardbox, 0, MAKE_GRAY(26));
            compo_cardbox_rect_set_location(cardbox, 0, 0, 0, GUI_SCREEN_WIDTH - 10, GUI_SCREEN_HEIGHT/4, 20);
        }
    }
    else
    {
        compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_NO_CLOCK]));
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 100, GUI_SCREEN_WIDTH/1.1,28);
        compo_textbox_set(textbox, i18n[STR_NO_CLOCK]);
    }
    // printf("week:%s\n",str_buff);
    uteModulePlatformMemoryFree(str_buff);

    //添加闹钟按钮图标
    if (ALARM_ENABLE_CNT() < ALARM_CLOCK_NUM_MAX)
    {
        icon_add = widget_icon_create(frm->page, UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN);
        widget_set_pos(icon_add, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei / 2 - 20);

        //page_body结合compo_page_move实现列表滑动（先绘制所有组件，再将page平均分段）
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 42 / 100+15, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT * 6 / 10);//208);
    }
    else
    {
        icon_add = NULL;
    }

    //添加闹钟按钮文字
//    compo_textbox_t* icon_add_txt = compo_textbox_create(frm, strlen(i18n[STR_ADD_CLOCK]));
    if (icon_add)
    {
        compo_textbox_t* icon_add_txt = compo_textbox_create_for_page(frm, frm->page, 50);
        compo_textbox_set_location(icon_add_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei / 2 - 20,
                                   gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).wid - gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei,
                                   gui_image_get_size(UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN).hei);
        compo_textbox_set(icon_add_txt, i18n[STR_ADD_CLOCK]);
    }

    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_button_touch_handle(void)
{
    if (icon_add)
    {
        point_t pt = ctp_get_sxy();
        rect_t rect = widget_get_absolute(icon_add);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)   //添加闹钟
        {
            widget_icon_set(icon_add, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
        }
    }
}

//释放按钮效果处理
static void func_alarm_clock_button_release_handle(void)
{
    compo_cardbox_t *cardbox;
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I330001_PUBLIC_SWITCH01_BIN : UI_BUF_I330001_PUBLIC_SWITCH00_BIN);
        compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
        // compo_cardbox_text_set_forecolor(cardbox, 2, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
        compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
    }

    if (icon_add)
    {
        widget_icon_set(icon_add, UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN);
    }
}

//单击按钮
static void func_alarm_clock_button_click(void)
{
    rect_t rect;
    point_t pt = ctp_get_sxy();

    if (icon_add)
    {
        rect = widget_get_absolute(icon_add);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)   //添加闹钟
        {
            if (ALARM_ENABLE_CNT() < ALARM_CLOCK_NUM_MAX)
            {
                sys_cb.alarm_edit_idx = ALARM_ENABLE_CNT();
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
                return;
            }
        }
    }

    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        compo_cardbox_t *cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        if(cardbox == NULL)
        {
            return;
        }
        if (compo_cardbox_btn_is(cardbox, pt))
        {
            if (pt.x > (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_I330001_PUBLIC_SWITCH01_BIN).wid))   //开关
            {
                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
                //刷新
                compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
                compo_cardbox_text_set_forecolor(cardbox, 2, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
                compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I330001_PUBLIC_SWITCH01_BIN : UI_BUF_I330001_PUBLIC_SWITCH00_BIN);
            }
            else        //编辑
            {
                sys_cb.alarm_edit_idx = i;
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
            }
        }
    }

//    func_alarm_clock_button_release_handle();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK]);

    //闹钟选项卡
    compo_cardbox_t *cardbox;
    int buf_num=0;
    int str_week_buf[7]=
    {
        STR_MONDAY, // 周一
        STR_TUESDAY, // 周二
        STR_WEDNESDAY, // 周三
        STR_THURSDAY, // 周四
        STR_FRIDAY, // 周五
        STR_SATURDAY, // 周六
        STR_SUNDAY, // 周日
    };

    char *str_buff = NULL;
    uint16_t str_buff_size = 0;

    for (uint8_t i = 0; i < 7; i++)
    {
        str_buff_size += strlen(i18n[str_week_buf[i]]) + 2;
    }

    if(str_buff_size < MAX(strlen(i18n[STR_ONCE]) + 2, strlen(i18n[STR_EVERY_DAY]) + 2))
    {
        str_buff_size = MAX(strlen(i18n[STR_ONCE]) + 2, strlen(i18n[STR_EVERY_DAY]) + 2);
    }

    str_buff = (char *)uteModulePlatformMemoryAlloc(str_buff_size);

    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {
            memset(str_buff,0,str_buff_size);
            buf_num=0;
            cardbox = compo_cardbox_create(frm, 1, 1, 2, 360, 90);
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, 130 + 106*i);
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_rect_set_color(cardbox, 0, MAKE_GRAY(26));
            compo_cardbox_rect_set_location(cardbox, 0, 0, 0, 324, 90, 20);

            compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I332001_PUBLIC_SWITCH01_BIN : UI_BUF_I332001_PUBLIC_SWITCH00_BIN);
            compo_cardbox_icon_set_pos(cardbox, 0, (GUI_SCREEN_WIDTH - 10) / 2 - gui_image_get_size(UI_BUF_I332001_PUBLIC_SWITCH01_BIN).wid / 2 - 25, 0);
            snprintf(str_buff, str_buff_size, "%02d:%02d", func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).hour, ALARM_GET_MIN(i));
//            compo_cardbox_text_set_font(cardbox, 0, UI_BUF_0FONT_FONT_NUM_32_BIN);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set(cardbox, 0, str_buff);
            compo_cardbox_text_set_align_center(cardbox, 0, false);
            compo_cardbox_text_set_location(cardbox, 0, -130, -30, 180, 50);

            compo_cardbox_text_set_font(cardbox, 1, UI_BUF_0FONT_FONT_BIN);
            compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set_align_center(cardbox, 1, false);
            compo_cardbox_text_set_location(cardbox, 1, -130 + widget_text_get_area(cardbox->text[0]).wid + 10, -30, 80, 50);
            compo_cardbox_text_set_visible(cardbox, 1, true);
            if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 0)
            {
                compo_cardbox_text_set_visible(cardbox, 1, false);
            }
            else if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 1)           //AM
            {
                compo_cardbox_text_set(cardbox, 1, i18n[STR_AM]);
            }
            else if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 2)           //PM
            {
                compo_cardbox_text_set(cardbox, 1, i18n[STR_PM]);
            }


            memset(str_buff,0,str_buff_size);

            if (ALARM_GET_CYCLE(i) & BIT(7))
            {
                snprintf(str_buff, str_buff_size, i18n[STR_ONCE]);
            }
            else if (ALARM_GET_CYCLE(i) == 0x7f)
            {
                snprintf(str_buff, str_buff_size, i18n[STR_EVERY_DAY]);
            }
            else
            {
                for (u8 j = 0; j < 7; j++)
                {
                    if (ALARM_GET_CYCLE(i) & BIT(j))
                    {
                        const char *week_str = i18n[str_week_buf[j]];
                        uint8_t week_str_len = strlen(week_str);
                        if (buf_num + week_str_len + 1 <= str_buff_size)
                        {
                            memcpy(&str_buff[buf_num], week_str, week_str_len);
                            buf_num += week_str_len;
                            str_buff[buf_num++] = ' ';
                        }
                    }
                }
            }

            compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(str_buff));
            compo_textbox_set_align_center(textbox, false);
            compo_textbox_set_location(textbox, -130, 10, 160, 40);
            compo_textbox_set(textbox,str_buff);
            compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
        }
    }
    else
    {
        compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_NO_CLOCK]));
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20, GUI_SCREEN_WIDTH/1.1,28 );
        compo_textbox_set(textbox, i18n[STR_NO_CLOCK]);

    }

    //添加闹钟按钮图标
    if (ALARM_ENABLE_CNT() < ALARM_CLOCK_NUM_MAX)
    {
        icon_add = widget_icon_create(frm->page, UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN);
        widget_set_pos(icon_add, GUI_SCREEN_CENTER_X, 292);

        //page_body结合compo_page_move实现列表滑动（先绘制所有组件，再将page平均分段）
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 42 / 100+15, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT * 6 / 10);//208);
    }
    else
    {
        icon_add = NULL;
    }

    //添加闹钟按钮文字
    if (icon_add)
    {
        compo_textbox_t* icon_add_txt = compo_textbox_create_for_page(frm, frm->page, 50);
        compo_textbox_set_location(icon_add_txt, GUI_SCREEN_CENTER_X, 292,210,40);
        compo_textbox_set(icon_add_txt, i18n[STR_ADD_CLOCK]);
    }

    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_button_touch_handle(void)
{
    if (icon_add)
    {
        point_t pt = ctp_get_sxy();
        rect_t rect = widget_get_absolute(icon_add);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)   //添加闹钟
        {
            widget_icon_set(icon_add, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
        }
    }
}

//释放按钮效果处理
static void func_alarm_clock_button_release_handle(void)
{
    compo_cardbox_t *cardbox;
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I332001_PUBLIC_SWITCH01_BIN : UI_BUF_I332001_PUBLIC_SWITCH00_BIN);
        compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
        compo_cardbox_text_set_forecolor(cardbox, 2, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
    }

    if (icon_add)
    {
        widget_icon_set(icon_add, UI_BUF_I332001_PUBLIC_RECTANGLE02_BIN);
    }
}

//单击按钮
static void func_alarm_clock_button_click(void)
{
    rect_t rect;
    point_t pt = ctp_get_sxy();

    if (icon_add)
    {
        rect = widget_get_absolute(icon_add);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)   //添加闹钟
        {
            if (ALARM_ENABLE_CNT() < ALARM_CLOCK_NUM_MAX)
            {
                sys_cb.alarm_edit_idx = ALARM_ENABLE_CNT();
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
                return;
            }
        }
    }
    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        if (compo_cardbox_btn_is(compo_getobj_byid(COMPO_ID_CARD_0 + i), pt))
        {
            rect_t rect = compo_cardbox_get_icon_absolute(compo_getobj_byid(COMPO_ID_CARD_0 + i),0);
            if (abs_s(pt.x - rect.x) * 2 <= rect.wid*1.5 && abs_s(pt.y - rect.y) * 2 <= rect.hei*2)  //开关
            {
                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
                //刷新
                compo_cardbox_text_set_forecolor(compo_getobj_byid(COMPO_ID_CARD_0 + i), 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
                compo_cardbox_text_set_forecolor(compo_getobj_byid(COMPO_ID_CARD_0 + i), 2, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            }
            else        //编辑
            {
                sys_cb.alarm_edit_idx = i;
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
            }
        }
    }

    func_alarm_clock_button_release_handle();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK]);

    //闹钟选项卡
    compo_cardbox_t *cardbox;
    int buf_num=0;
    int str_week_buf[7]=
    {
        STR_MONDAY, // 周一
        STR_TUESDAY, // 周二
        STR_WEDNESDAY, // 周三
        STR_THURSDAY, // 周四
        STR_FRIDAY, // 周五
        STR_SATURDAY, // 周六
        STR_SUNDAY, // 周日
    };

    char *str_buff = NULL;
    uint16_t str_buff_size = 0;

    for (uint8_t i = 0; i < 7; i++)
    {
        str_buff_size += strlen(i18n[str_week_buf[i]]) + 2;
    }

    if(str_buff_size < MAX(strlen(i18n[STR_ONCE]) + 2, strlen(i18n[STR_EVERY_DAY]) + 2))
    {
        str_buff_size = MAX(strlen(i18n[STR_ONCE]) + 2, strlen(i18n[STR_EVERY_DAY]) + 2);
    }

    str_buff = (char *)uteModulePlatformMemoryAlloc(str_buff_size);

    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {
            memset(str_buff,0,str_buff_size);
            buf_num=0;
            cardbox = compo_cardbox_create(frm, 1, 1, 2, 360, 90);
            compo_cardbox_set_pos(cardbox, GUI_SCREEN_CENTER_X, 130 + 106*i);
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_rect_set_color(cardbox, 0, MAKE_GRAY(26));
            compo_cardbox_rect_set_location(cardbox, 0, 0, 0, 324, 90, 20);

            compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I340001_PUBLIC_SWITCH01_BIN : UI_BUF_I340001_PUBLIC_SWITCH00_BIN);
            compo_cardbox_icon_set_pos(cardbox, 0, (GUI_SCREEN_WIDTH - 10) / 2 - gui_image_get_size(UI_BUF_I340001_PUBLIC_SWITCH01_BIN).wid / 2 - 25, 0);
            snprintf(str_buff, str_buff_size, "%02d:%02d", func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).hour, ALARM_GET_MIN(i));
//            compo_cardbox_text_set_font(cardbox, 0, UI_BUF_0FONT_FONT_NUM_32_BIN);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set(cardbox, 0, str_buff);
            compo_cardbox_text_set_align_center(cardbox, 0, false);
            compo_cardbox_text_set_location(cardbox, 0, -130, -30, 180, 50);

            compo_cardbox_text_set_font(cardbox, 1, UI_BUF_0FONT_FONT_BIN);
            compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set_align_center(cardbox, 1, false);
            compo_cardbox_text_set_location(cardbox, 1, -130 + widget_text_get_area(cardbox->text[0]).wid + 10, -30, 80, 50);
            compo_cardbox_text_set_visible(cardbox, 1, true);
            if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 0)
            {
                compo_cardbox_text_set_visible(cardbox, 1, false);
            }
            else if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 1)           //AM
            {
                compo_cardbox_text_set(cardbox, 1, i18n[STR_AM]);
            }
            else if (func_alarm_convert_to_12hour(ALARM_GET_HOUR(i)).am_pm == 2)           //PM
            {
                compo_cardbox_text_set(cardbox, 1, i18n[STR_PM]);
            }


            memset(str_buff,0,str_buff_size);

            if (ALARM_GET_CYCLE(i) & BIT(7))
            {
                snprintf(str_buff, str_buff_size, i18n[STR_ONCE]);
            }
            else if (ALARM_GET_CYCLE(i) == 0x7f)
            {
                snprintf(str_buff, str_buff_size, i18n[STR_EVERY_DAY]);
            }
            else
            {
                for (u8 j = 0; j < 7; j++)
                {
                    if (ALARM_GET_CYCLE(i) & BIT(j))
                    {
                        const char *week_str = i18n[str_week_buf[j]];
                        uint8_t week_str_len = strlen(week_str);
                        if (buf_num + week_str_len + 1 <= str_buff_size)
                        {
                            memcpy(&str_buff[buf_num], week_str, week_str_len);
                            buf_num += week_str_len;
                            str_buff[buf_num++] = ' ';
                        }
                    }
                }
            }

            compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(str_buff));
            compo_textbox_set_align_center(textbox, false);
            compo_textbox_set_location(textbox, -130, 10, 160, 40);
            compo_textbox_set(textbox,str_buff);
            compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
        }
    }
    else
    {
        compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_NO_CLOCK]));
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20, GUI_SCREEN_WIDTH/1.1,28 );
        compo_textbox_set(textbox, i18n[STR_NO_CLOCK]);

    }

    //添加闹钟按钮图标
    if (ALARM_ENABLE_CNT() < ALARM_CLOCK_NUM_MAX)
    {
        icon_add = widget_icon_create(frm->page, UI_BUF_I340001_PUBLIC_RECTANGLE02_BIN);
        widget_set_pos(icon_add, GUI_SCREEN_CENTER_X, 292);

        //page_body结合compo_page_move实现列表滑动（先绘制所有组件，再将page平均分段）
        widget_set_location(frm->page_body, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 42 / 100+15, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT * 6 / 10);//208);
    }
    else
    {
        icon_add = NULL;
    }

    //添加闹钟按钮文字
    if (icon_add)
    {
        compo_textbox_t* icon_add_txt = compo_textbox_create_for_page(frm, frm->page, 50);
        compo_textbox_set_location(icon_add_txt, GUI_SCREEN_CENTER_X, 292,210,40);
        compo_textbox_set(icon_add_txt, i18n[STR_ADD_CLOCK]);
    }

    return frm;
}

//触摸按钮效果处理
static void func_alarm_clock_button_touch_handle(void)
{
    if (icon_add)
    {
        point_t pt = ctp_get_sxy();
        rect_t rect = widget_get_absolute(icon_add);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)   //添加闹钟
        {
            widget_icon_set(icon_add, UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN);
        }
    }
}

//释放按钮效果处理
static void func_alarm_clock_button_release_handle(void)
{
    compo_cardbox_t *cardbox;
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I340001_PUBLIC_SWITCH01_BIN : UI_BUF_I340001_PUBLIC_SWITCH00_BIN);
        compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
        compo_cardbox_text_set_forecolor(cardbox, 2, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
    }

    if (icon_add)
    {
        widget_icon_set(icon_add, UI_BUF_I340001_PUBLIC_RECTANGLE02_BIN);
    }
}

//单击按钮
static void func_alarm_clock_button_click(void)
{
    rect_t rect;
    point_t pt = ctp_get_sxy();

    if (icon_add)
    {
        rect = widget_get_absolute(icon_add);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)   //添加闹钟
        {
            if (ALARM_ENABLE_CNT() < ALARM_CLOCK_NUM_MAX)
            {
                sys_cb.alarm_edit_idx = ALARM_ENABLE_CNT();
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_SET;
                return;
            }
        }
    }
    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        if (compo_cardbox_btn_is(compo_getobj_byid(COMPO_ID_CARD_0 + i), pt))
        {
            rect_t rect = compo_cardbox_get_icon_absolute(compo_getobj_byid(COMPO_ID_CARD_0 + i),0);
            if (abs_s(pt.x - rect.x) * 2 <= rect.wid*1.5 && abs_s(pt.y - rect.y) * 2 <= rect.hei*2)  //开关
            {
                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
                //刷新
                compo_cardbox_text_set_forecolor(compo_getobj_byid(COMPO_ID_CARD_0 + i), 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
                compo_cardbox_text_set_forecolor(compo_getobj_byid(COMPO_ID_CARD_0 + i), 2, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            }
            else        //编辑
            {
                sys_cb.alarm_edit_idx = i;
                func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
            }
        }
    }

    func_alarm_clock_button_release_handle();
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

#define WEEK_LOCATION_X  (-80)
#define WEEK_LOCATION_Y  (15)
#define WEEK_SPACING_X   (23)

//创建闹钟窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_form_create(void)
{
    char str_buff[50];
    const uint32_t clock_off_day[7]=
    {
        UI_BUF_I342001_20_ALARM_CLOCK_DEFAULT_00_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_DEFAULT_01_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_DEFAULT_02_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_DEFAULT_03_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_DEFAULT_04_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_DEFAULT_05_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_DEFAULT_06_BIN,
    };
    const uint32_t clock_on_day[7]=
    {
        UI_BUF_I342001_20_ALARM_CLOCK_STATE_00_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_STATE_01_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_STATE_02_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_STATE_03_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_STATE_04_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_STATE_05_BIN,
        UI_BUF_I342001_20_ALARM_CLOCK_STATE_06_BIN,
    };

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK]);

    if (ALARM_ENABLE_CNT())
    {
        for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
        {
            compo_cardbox_t *cardbox = compo_cardbox_create(frm,1,1,1,228,80);
            compo_cardbox_set_pos(cardbox,GUI_SCREEN_CENTER_X,84+(i*86));
            compo_setid(cardbox, COMPO_ID_CARD_0 + i);

            compo_cardbox_rect_set_location(cardbox,0,0,0,228,80,10);
            compo_cardbox_rect_set_color(cardbox, 0,make_color(36,36,36));

            compo_cardbox_icon_set_pos(cardbox,0,80,-15);
            compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I342001_20_ALARM_CLOCK_OPEN_BIN:UI_BUF_I342001_20_ALARM_CLOCK_CLOSE_BIN);

            memset(str_buff,0,sizeof(str_buff));
            func_alarm_hour_format_t clock_time = func_alarm_convert_to_12hour(ALARM_GET_HOUR(i));
            snprintf(str_buff, sizeof(str_buff), "%02d:%02d %s", clock_time.hour, ALARM_GET_MIN(i),clock_time.am_pm ? (clock_time.am_pm ==1 ? i18n[STR_AM] : i18n[STR_PM]) : " ");
            compo_cardbox_text_set_location(cardbox,0,WEEK_LOCATION_X-10,-30,110,33);
            compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
            compo_cardbox_text_set_font(0,0, UI_BUF_0FONT_FONT_NUM_32_BIN);
            compo_cardbox_text_set_align_center(cardbox,0, false);
            compo_cardbox_text_set(cardbox,0,str_buff);


            if (ALARM_GET_CYCLE(i) & BIT(7))
            {
                compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(i18n[STR_ONCE]));
                compo_textbox_set_pos(textbox, WEEK_LOCATION_X-8, WEEK_LOCATION_Y-15);
                compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set(textbox,i18n[STR_ONCE]);
            }
            else if (ALARM_GET_CYCLE(i) == 0x7f)
            {
                compo_textbox_t *textbox = compo_textbox_create_for_page(frm,cardbox->page,strlen(i18n[STR_EVERY_DAY]));
                compo_textbox_set_forecolor(textbox, MAKE_GRAY(128));
                compo_textbox_set_pos(textbox, WEEK_LOCATION_X-8, WEEK_LOCATION_Y-15);
                compo_textbox_set_align_center(textbox, false);
                compo_textbox_set(textbox,i18n[STR_EVERY_DAY]);
            }
            else
            {
                for (u8 j = 0; j < 7; j++)
                {

                    compo_picturebox_t *picbox = compo_picturebox_create_for_page(frm,cardbox->page,0);
                    compo_picturebox_set_pos(picbox, WEEK_LOCATION_X+(WEEK_SPACING_X*j), WEEK_LOCATION_Y);
                    compo_picturebox_set(picbox,(ALARM_GET_CYCLE(i) & BIT(j)) ? clock_on_day[j] : clock_off_day[j]);

                }
            }
        }

        // widget_page_t *page = widget_page_create(frm->page);
        // widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-40, GUI_SCREEN_WIDTH, 80);

        // compo_shape_t * shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);
        // compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 45, GUI_SCREEN_WIDTH, 80);
        // compo_shape_set_color(shape,COLOR_BLACK);

        compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I342001_20_ALARM_CLOCK_ADD2_BIN);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,84+(ALARM_ENABLE_CNT()*86)-5);
        compo_setid(btn,COMPO_ID_BTN_ADD);

    }
    else
    {
        compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I342001_20_ALARM_CLOCK_ADD_BIN);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,50+76);
        compo_setid(btn,COMPO_ID_BTN_ADD);

        compo_textbox_t *textbox = compo_textbox_create(frm,strlen(i18n[STR_ADD_CLOCK]));
        compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,213,230,25);
        compo_textbox_set(textbox,i18n[STR_ADD_CLOCK]);
    }



    return frm;
}

//单击按钮
static void func_alarm_clock_button_click(void)
{
    rect_t rect;
    point_t pt = ctp_get_sxy();
    //编辑/开关闹钟
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)
    {
        compo_cardbox_t *cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        if (compo_cardbox_btn_is(cardbox, pt))
        {
            rect_t rect = compo_cardbox_get_icon_absolute(cardbox, 0); //上一首
            if (abs_s(pt.x - rect.x) <= rect.wid && abs_s(pt.y - rect.y)  <= rect.hei*2)
                //if (pt.x > (GUI_SCREEN_WIDTH - gui_image_get_size(UI_BUF_I342001_20_ALARM_CLOCK_OPEN_BIN).wid))   //开关
            {
                ALARM_ENABLE(i, !ALARM_GET_SWITCH(i));
                //刷新
                compo_cardbox_text_set_forecolor(cardbox, 0, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
                compo_cardbox_icon_set(cardbox, 0,  ALARM_GET_SWITCH(i) ? UI_BUF_I342001_20_ALARM_CLOCK_OPEN_BIN:UI_BUF_I342001_20_ALARM_CLOCK_CLOSE_BIN);
            }
            else        //编辑
            {
                sys_cb.alarm_edit_idx = i;
                uteTaskGuiStartScreen(FUNC_ALARM_CLOCK_SUB_EDIT, 0, __func__);
            }
        }
    }

    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_ADD:
            if(SYSTEM_TIME_ALARMS_MAX_CNT == ALARM_ENABLE_CNT())
            {
                msgbox(i18n[STR_ADD_CLOCK_NO_MORE], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
                return;
            }
            uteTaskGuiStartScreen(FUNC_ALARM_CLOCK_SUB_SET, 0, __func__);
            break;

        default:
            break;
    }

}


#else
compo_form_t *func_alarm_clock_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//闹钟功能事件处理
static void func_alarm_clock_process(void)
{
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    if(f_aclock->ptm)
    {
        compo_page_move_process(f_aclock->ptm);
    }
    //处理在当前界面时单次闹钟响铃后，闹钟开关状态不更新问题
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
    func_alarm_clock_disp_handle();
#endif // GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
    func_process();

#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)      //文本滚动
    {
        compo_cardbox_t* cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        compo_cardbox_text_scroll_process(cardbox, true);
    }
#elif  GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)      //文本滚动
    {
        compo_cardbox_t* cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        compo_cardbox_text_scroll_process(cardbox, true);
        compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
        compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I332001_PUBLIC_SWITCH01_BIN : UI_BUF_I332001_PUBLIC_SWITCH00_BIN);
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    for(u8 i=0; i<ALARM_ENABLE_CNT(); i++)      //文本滚动
    {
        compo_cardbox_t* cardbox = compo_getobj_byid(COMPO_ID_CARD_0 + i);
        compo_cardbox_text_scroll_process(cardbox, true);
        compo_cardbox_text_set_forecolor(cardbox, 1, ALARM_GET_SWITCH(i) ? MAKE_GRAY(255) : MAKE_GRAY(128));
        compo_cardbox_icon_set(cardbox, 0, ALARM_GET_SWITCH(i) ? UI_BUF_I340001_PUBLIC_SWITCH01_BIN : UI_BUF_I340001_PUBLIC_SWITCH00_BIN);
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}


//闹钟功能消息处理
static void func_alarm_clock_message(size_msg_t msg)
{
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

    switch (msg)
    {
        case MSG_CTP_TOUCH:
//            func_alarm_clock_button_touch_handle();
            compo_page_move_touch_handler(f_aclock->ptm);
            break;

        case MSG_CTP_CLICK:
            func_alarm_clock_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_alarm_clock_button_release_handle();
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_alarm_clock_button_release_handle();
            func_message(msg);
            break;

        case MSG_QDEC_BACKWARD:
            compo_page_move_set_by_pages(f_aclock->ptm, -1);
            break;

        case MSG_QDEC_FORWARD:
            compo_page_move_set_by_pages(f_aclock->ptm, 1);
            break;

        case MSG_SYS_500MS:
            //时间制检测
            if (f_aclock->time_scale != uteModuleSystemtime12HOn())
            {
                if (func_cb.frm_main != NULL)
                {
                    compo_form_destroy(func_cb.frm_main);
                }
                func_alarm_clock_exit();
                if (func_cb.f_cb != NULL)
                {
                    func_free(func_cb.f_cb);
                    func_cb.f_cb = NULL;
                }
                func_alarm_clock_enter();
            }
            break;

        default:
            func_message(msg);
            break;
    }
#elif  GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
//            func_alarm_clock_button_touch_handle();
            compo_page_move_touch_handler(f_aclock->ptm);
            break;

        case MSG_CTP_CLICK:
            func_alarm_clock_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_alarm_clock_button_release_handle();
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_alarm_clock_button_release_handle();
            func_message(msg);
            break;

        case MSG_QDEC_BACKWARD:
            compo_page_move_set_by_pages(f_aclock->ptm, -1);
            break;

        case MSG_QDEC_FORWARD:
            compo_page_move_set_by_pages(f_aclock->ptm, 1);
            break;

        case MSG_SYS_500MS:
            //时间制检测
            if (f_aclock->time_scale != uteModuleSystemtime12HOn())
            {
                if (func_cb.frm_main != NULL)
                {
                    compo_form_destroy(func_cb.frm_main);
                }
                func_alarm_clock_exit();
                if (func_cb.f_cb != NULL)
                {
                    func_free(func_cb.f_cb);
                    func_cb.f_cb = NULL;
                }
                func_alarm_clock_enter();
            }
            break;

        default:
            func_message(msg);
            break;
    }
#else
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_aclock->ptm)
            {
                compo_page_move_touch_handler(f_aclock->ptm);
            }
            break;
        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
            func_alarm_clock_button_click();
#endif
            break;
        default:
            func_message(msg);
            break;
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

}

//进入闹钟功能
static void func_alarm_clock_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_t));
    func_cb.frm_main = func_alarm_clock_form_create();
    sys_cb.alarm_edit_idx = ALARM_ENABLE_CNT();
#if  GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    f_aclock->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = ((ALARM_ENABLE_CNT()+1)*86)+10,
        .page_count = 1,
        .quick_jump_perc = 40,
        .up_over_perc = ALARM_ENABLE_CNT() ? 1 : 0,
        .down_over_perc = ALARM_ENABLE_CNT() ? 1 : 0,
    };
    compo_page_move_init(f_aclock->ptm, func_cb.frm_main->page_body, &info);

    //当前显示的时间制
    f_aclock->time_scale = uteModuleSystemtime12HOn();
#elif  GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    f_aclock->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = ((ALARM_ENABLE_CNT()+1)*132)+8,
        .page_count = 1,
        .quick_jump_perc = 40,
        .up_over_perc = ALARM_ENABLE_CNT() ? 1 : 0,
        .down_over_perc = ALARM_ENABLE_CNT() ? 1 : 0,
    };
    compo_page_move_init(f_aclock->ptm, func_cb.frm_main->page_body, &info);

    //当前显示的时间制
    f_aclock->time_scale = uteModuleSystemtime12HOn();
#elif  GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    f_aclock->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = ((ALARM_ENABLE_CNT()+1)*(78+25))+10,
        .page_count = 1,
        .quick_jump_perc = 40,
        .up_over_perc = ALARM_ENABLE_CNT() ? 1 : 0,
        .down_over_perc = ALARM_ENABLE_CNT() ? 1 : 0,
    };
    compo_page_move_init(f_aclock->ptm, func_cb.frm_main->page_body, &info);

    //当前显示的时间制
    f_aclock->time_scale = uteModuleSystemtime12HOn();
#elif  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    f_aclock->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 80,
        .page_count = ALARM_ENABLE_CNT(),
        .jump_perc = 20,
        .quick_jump_perc = 200,
        .up_over_perc = ALARM_ENABLE_CNT() ? 50 : 0,
        .down_over_perc = ALARM_ENABLE_CNT() ? 50 : 0,
        .down_spring_perc = icon_add ? 0 : 40,
    };
    compo_page_move_init(f_aclock->ptm, func_cb.frm_main->page_body, &info);

    //当前显示的时间制
    f_aclock->time_scale = uteModuleSystemtime12HOn();
#elif  GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    f_aclock->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 100,
        .page_count = ALARM_ENABLE_CNT()+1,
        .jump_perc = 20,
        .quick_jump_perc = 200,
        .up_over_perc = ALARM_ENABLE_CNT() ? 50 : 0,
        .down_over_perc = ALARM_ENABLE_CNT() ? 50 : 0,
        .down_spring_perc = icon_add ? 0 : 40,
    };
    compo_page_move_init(f_aclock->ptm, func_cb.frm_main->page_body, &info);

    //当前显示的时间制
    f_aclock->time_scale = uteModuleSystemtime12HOn();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//退出闹钟功能
static void func_alarm_clock_exit(void)
{
    f_alarm_clock_t *f_aclock = (f_alarm_clock_t *)func_cb.f_cb;
    if (f_aclock->ptm)
    {
        func_free(f_aclock->ptm);
    }
    func_cb.last = FUNC_ALARM_CLOCK;
}

//闹钟功能
void func_alarm_clock(void)
{
    printf("%s\n", __func__);
    func_alarm_clock_enter();
    while (func_cb.sta == FUNC_ALARM_CLOCK)
    {
        func_alarm_clock_process();
        func_alarm_clock_message(msg_dequeue());
    }
    func_alarm_clock_exit();
}
#endif
