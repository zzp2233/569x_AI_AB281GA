#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if (USE_APP_TYPE == USE_AB_APP)
#define ALARM_IS_FREE(idx)              alarm_is_free(idx)
#define ALARM_EDIT(...)                 alarm_clock_edit(__VA_ARGS__)
#define ALARM_GET_HOUR(idx)             app_data.sector0.alarm[idx].hour
#define ALARM_GET_MIN(idx)              app_data.sector0.alarm[idx].minute
#define ALARM_GET_CYCLE(idx)            app_data.sector0.alarm[idx].cycle
#define ALARM_GET_SWITCH(idx)           app_data.sector0.alarm[idx].swi
#elif (USE_APP_TYPE == USE_UTE_APP)
#define ALARM_IS_FREE(idx)              uteModuleSystemtimeGetAlarmIsFree(idx)
#define ALARM_EDIT(...)                 uteModuleSystemtimeAlarmEdit(__VA_ARGS__)
#define ALARM_GET_HOUR(idx)             uteModuleSystemtimeGetAlarmHour(idx)
#define ALARM_GET_MIN(idx)              uteModuleSystemtimeGetAlarmMin(idx)
#define ALARM_GET_CYCLE(idx)            uteModuleSystemtimeGetAlarmCycle(idx)
#define ALARM_GET_SWITCH(idx)           uteModuleSystemtimeGetAlarmEnableState(idx)
#else
#define ALARM_IS_FREE(idx)              0
#define ALARM_EDIT(...)
#define ALARM_GET_HOUR(idx)             0
#define ALARM_GET_MIN(idx)              0
#define ALARM_GET_CYCLE(idx)            0
#define ALARM_GET_SWITCH(idx)           0
#endif

static void func_alarm_clock_sub_set_enter(void);
static void func_alarm_clock_sub_set_exit(void);

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_CONTINUE = 1,
    COMPO_ID_BTN_REPETAT_NO,
    COMPO_ID_BTN_REPETAT_YES,
    COMPO_ID_BTN_ACLOCK_HOUR,
    COMPO_ID_BTN_ACLOCK_MIN,

    //图像
    COMPO_ID_PIC_CONTINUE_CLICK,
    COMPO_ID_PIC_REPETAT_NO_CLICK,
    COMPO_ID_PIC_REPETAT_YES_CLICK,

    //数字
    COMPO_ID_NUM_ACLOCK_SET_HOUR_UU,
    COMPO_ID_NUM_ACLOCK_SET_HOUR_UP,
    COMPO_ID_NUM_ACLOCK_SET_HOUR,
    COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN,
    COMPO_ID_NUM_ACLOCK_SET_HOUR_DD,

    COMPO_ID_NUM_ACLOCK_SET_MIN_UU,
    COMPO_ID_NUM_ACLOCK_SET_MIN_UP,
    COMPO_ID_NUM_ACLOCK_SET_MIN,
    COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN,
    COMPO_ID_NUM_ACLOCK_SET_MIN_DD,

    //AM PM 文本
    COMPO_ID_TXT_AM_PM,
};

typedef struct aclock_set_num_item_t_
{
    int num_cnt;
    u16 num_id;
    s16 load_id;
    int val;
    s16 x;
    s16 y;
    bool visible_en;
    u8 alpha;
} aclock_set_num_item_t;

typedef struct f_alarm_clock_sub_set_t_
{
    aclock_set_num_item_t set_num_item[11];
    area_t size;
    u16 num_height;
    s16 hcnt;
    s16 mcnt;

    bool time_scale;
} f_alarm_clock_sub_set_t;

#define ACLOCK_SET_NUM_ITEM_CNT             ((int)(sizeof(tbl_aclock_set_num_item) / sizeof(tbl_aclock_set_num_item[0])))

#define ALARM_NUM_PIC_CENTER_X              120     //时间框的中心坐标
#define ALARM_NUM_PIC_CENTER_Y              126
#define ALARM_TXT_ITEM_H_X                  71
#define ALARM_TXT_ITEM_M_X                  171
#define ALARM_TXT_ITEM_Y                    48
#define ALARM_TXT_ITEM_Y_OFFSET             39

#define ALARM_CLOCK_SET_NUM_POS_Y_START                 ALARM_TXT_ITEM_Y
#define ALARM_CLOCK_SET_NUM_POS_Y(y_start,space,cnt)    (y_start)+(space)*(cnt)

#define ALARM_CLOCK_FOCUS_ITEM_FONT         UI_BUF_0FONT_FONT_NUM_24_BIN
#define ALARM_CLOCK_ITEM_FONT               UI_BUF_0FONT_FONT_BIN

#define ALARM_OFS_X                         20

//搞个数字item，创建时遍历一下
static const aclock_set_num_item_t tbl_aclock_set_num_item[] =
{
    /*   num_cnt,              num_id,                           load_num_id                val,   x,                    y,                                             visible_en      alpha*/
    {2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_UU,      COMPO_ID_NUM_ACLOCK_SET_HOUR_UU,    22,   ALARM_TXT_ITEM_H_X,   ALARM_TXT_ITEM_Y,                              true,           100},
    {2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_UP,      COMPO_ID_NUM_ACLOCK_SET_HOUR_UP,    23,   ALARM_TXT_ITEM_H_X,   ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET,      true,           100},
    {2,        COMPO_ID_NUM_ACLOCK_SET_HOUR,         COMPO_ID_NUM_ACLOCK_SET_HOUR,       0,    ALARM_TXT_ITEM_H_X,   ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN,    COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN,  1,    ALARM_TXT_ITEM_H_X,   ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_DD,      COMPO_ID_NUM_ACLOCK_SET_HOUR_DD,    2,    ALARM_TXT_ITEM_H_X,   ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*4,    true,           100},

    {2,        COMPO_ID_NUM_ACLOCK_SET_MIN_UU,       COMPO_ID_NUM_ACLOCK_SET_MIN_UU,     58,   ALARM_TXT_ITEM_M_X,   ALARM_TXT_ITEM_Y,                              true,           100},
    {2,        COMPO_ID_NUM_ACLOCK_SET_MIN_UP,       COMPO_ID_NUM_ACLOCK_SET_MIN_UP,     59,   ALARM_TXT_ITEM_M_X,   ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET,      true,           100},
    {2,        COMPO_ID_NUM_ACLOCK_SET_MIN,          COMPO_ID_NUM_ACLOCK_SET_MIN,        0,    ALARM_TXT_ITEM_M_X,   ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {2,        COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN,     COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN,   1,    ALARM_TXT_ITEM_M_X,   ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {2,        COMPO_ID_NUM_ACLOCK_SET_MIN_DD,       COMPO_ID_NUM_ACLOCK_SET_MIN_DD,     2,    ALARM_TXT_ITEM_M_X,   ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*4,    true,           100},

};

static s16 func_alarm_get_ofs_x(void)
{
    if(uteModuleSystemtime12HOn())
    {
        return  ALARM_OFS_X;
    }
    return 0;
}

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

//获取闹钟 时分 上一个和下一个的数字
static u8 func_alarm_clock_get_time_cal(s8 num, bool hour_en, u8 mode/*, u8 scale*/)     //mode 0:返回当前hour/min，1：返回上一个hour/min，2：返回下一个hour/min scale:输入时间制 0:24小时; 1:AM,12小时; 2:PM,12小时
{

//    //时间制统一转换到24小时
//    if (hour_en) {
//        if (scale) {
//            if (scale == 2) {           //PM
//                if (num != 12) {
//                    num += 12;
//                }
//            } else if (scale == 1) {    //AM
//                if (num == 12) {
//                    num = 0;
//                }
//            }
//        }
//    }

    if (num < 0)
    {
        if (hour_en)
        {
            num += 24;
        }
        else
        {
            num += 60;
        }
    }

    if (mode == 1)
    {
        if (hour_en)
        {
            if (num == 0 || num > 23)
            {
                return 23;
            }
        }
        else
        {
            if (num == 0 || num > 59)
            {
                return 59;
            }
        }
        return num - 1;
    }
    else if (mode == 2)
    {
        if (hour_en)
        {
            if (num >= 23)
            {
                return 0;
            }
        }
        else
        {
            if (num >= 59)
            {
                return 0;
            }
        }
        return num + 1;
    }
    else
    {
        if (hour_en)
        {
            if (num > 23)
            {
                return 23;
            }
        }
        else
        {
            if (num > 59)
            {
                return 59;
            }
        }
        return num;
    }
}

//创建闹钟设置窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_alarm_clock_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_SET]);

    compo_picturebox_t *pic_bg = compo_picturebox_create(frm, UI_BUF_ALARM_CLOCK_NUM_BIN);
    compo_picturebox_set_pos(pic_bg, ALARM_NUM_PIC_CENTER_X + func_alarm_get_ofs_x(), ALARM_NUM_PIC_CENTER_Y);

    area_t size = gui_image_get_size(UI_BUF_ALARM_CLOCK_NUM_BIN);

    //创建数字
    compo_textbox_t *txt_num;
    u8 hour, min;
    if (!ALARM_IS_FREE(sys_cb.alarm_edit_idx))      //edit
    {
        hour = ALARM_GET_HOUR(sys_cb.alarm_edit_idx);
        min = ALARM_GET_MIN(sys_cb.alarm_edit_idx);
    }
    else        //add
    {
        hour = compo_cb.tm.hour;
        min = compo_cb.tm.min;
    }
    sys_cb.alarm_edit_hour = hour;
    sys_cb.alarm_edit_min = min;

    char buf[4];
    for (u8 idx = 0; idx < ACLOCK_SET_NUM_ITEM_CNT; idx++)
    {
        txt_num = compo_textbox_create(frm, tbl_aclock_set_num_item[idx].num_cnt);
        compo_textbox_set_font(txt_num, ALARM_CLOCK_ITEM_FONT);
        compo_setid(txt_num, tbl_aclock_set_num_item[idx].num_id);
        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_pos(txt_num, tbl_aclock_set_num_item[idx].x + func_alarm_get_ofs_x(), tbl_aclock_set_num_item[idx].y);
        compo_textbox_set_visible(txt_num, tbl_aclock_set_num_item[idx].visible_en);
        compo_textbox_set_alpha(txt_num, tbl_aclock_set_num_item[idx].alpha);

        memset(buf, 0, sizeof(buf));
        if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UU)        //上上次的时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(func_alarm_clock_get_time_cal(hour, 1, 1), 1, 1)).hour);
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP)     //上次的时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(hour, 1, 1)).hour);
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR)      //当前时间
        {
            compo_textbox_set_font(txt_num, ALARM_CLOCK_FOCUS_ITEM_FONT);
            snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(hour).hour);
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN)     //下次时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(hour, 1, 2)).hour);
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DD)       //下下次时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(func_alarm_clock_get_time_cal(hour, 1, 2), 1, 2)).hour);
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UU)
        {
            snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(func_alarm_clock_get_time_cal(min, 0, 1), 0, 1));
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UP)
        {
            snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(min, 0, 1));
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN)
        {
            compo_textbox_set_font(txt_num, ALARM_CLOCK_FOCUS_ITEM_FONT);
            snprintf(buf, sizeof(buf), "%02d", min);
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN)
        {
            snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(min, 0, 2));
        }
        else if (tbl_aclock_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DD)
        {
            snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(func_alarm_clock_get_time_cal(min, 0, 2), 0, 2));
        }
        compo_textbox_set(txt_num, buf);
    }

    //画两个矩形遮挡框框外面的数字
    compo_shape_t *shape_up = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(shape_up, COLOR_BLACK);
    compo_shape_set_location(shape_up, 120 + func_alarm_get_ofs_x(), 47, size.wid, 32);

    compo_shape_t *shape_down = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(shape_down, COLOR_BLACK);
    compo_shape_set_location(shape_down, 120 + func_alarm_get_ofs_x(), 236, size.wid, 96);


    //新建按钮
    compo_button_t *btn;
    if (ALARM_IS_FREE(sys_cb.alarm_edit_idx))   //add
    {
        btn = compo_button_create_by_image(frm, UI_BUF_ALARM_CLOCK_CONTINUE_BIN);
        compo_setid(btn, COMPO_ID_BTN_CONTINUE);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_CONTINUE_BIN).hei / 2 - 10);
    }
    else        //edit
    {
        btn = compo_button_create_by_image(frm, UI_BUF_COMMON_NO_BIN);
        compo_setid(btn, COMPO_ID_BTN_REPETAT_NO);
        compo_button_set_pos(btn, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_COMMON_NO_BIN).hei / 2 - 10);

        btn = compo_button_create_by_image(frm, UI_BUF_ALARM_CLOCK_YES_BIN);
        compo_setid(btn, COMPO_ID_BTN_REPETAT_YES);
        compo_button_set_pos(btn, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_YES_BIN).hei / 2 - 10);
    }

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_ACLOCK_HOUR);
    compo_button_set_location(btn, 71 + func_alarm_get_ofs_x(), 126, 75, 130);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_ACLOCK_MIN);
    compo_button_set_location(btn, 171 + func_alarm_get_ofs_x(), 126, 75, 130);

    //新建图像
    compo_picturebox_t *pic_click;
    if (ALARM_IS_FREE(sys_cb.alarm_edit_idx))   //add
    {
        pic_click = compo_picturebox_create(frm, UI_BUF_ALARM_CLOCK_CONTINUE_CLICK_BIN);
        compo_setid(pic_click, COMPO_ID_PIC_CONTINUE_CLICK);
        compo_picturebox_set_pos(pic_click, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_CONTINUE_CLICK_BIN).hei / 2 - 10);
        compo_picturebox_set_visible(pic_click, false);
    }
    else        //edit
    {
        pic_click = compo_picturebox_create(frm, UI_BUF_COMMON_NO_CLICK_BIN);
        compo_setid(pic_click, COMPO_ID_PIC_REPETAT_NO_CLICK);
        compo_picturebox_set_pos(pic_click, GUI_SCREEN_WIDTH/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_COMMON_NO_CLICK_BIN).hei / 2 - 10);
        compo_picturebox_set_visible(pic_click, false);

        pic_click = compo_picturebox_create(frm, UI_BUF_ALARM_CLOCK_YES_CLICK_BIN);
        compo_setid(pic_click, COMPO_ID_PIC_REPETAT_YES_CLICK);
        compo_picturebox_set_pos(pic_click, GUI_SCREEN_WIDTH*3/4, GUI_SCREEN_HEIGHT - gui_image_get_size(UI_BUF_ALARM_CLOCK_YES_CLICK_BIN).hei / 2 - 10);
        compo_picturebox_set_visible(pic_click, false);
    }

    //AM PM TXT
    compo_textbox_t* txt_am_pm = compo_textbox_create(frm, 50);
    compo_textbox_set_font(txt_am_pm, 0);
    compo_setid(txt_am_pm, COMPO_ID_TXT_AM_PM);
    compo_textbox_set_location(txt_am_pm, ALARM_TXT_ITEM_H_X-40, ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*2, GUI_SCREEN_WIDTH / 6, widget_text_get_height());
    compo_textbox_set_visible(txt_am_pm, true);
//    compo_textbox_set_alpha(txt_am_pm, tbl_aclock_set_num_item[idx].alpha);
    u8 am_pm = func_alarm_convert_to_12hour(hour).am_pm;
    if (am_pm == 1)                //AM
    {
        compo_textbox_set(txt_am_pm, i18n[STR_AM]);
    }
    else if (am_pm == 2)           //PM
    {
        compo_textbox_set(txt_am_pm, i18n[STR_PM]);
    }
    else if (am_pm == 0)
    {
        compo_textbox_set_visible(txt_am_pm, false);
    }

    return frm;
}

static void alarm_clock_set_num_pos_cal(s32 dy, u8 id, bool press)
{
    f_alarm_clock_sub_set_t *f_alarm_set = (f_alarm_clock_sub_set_t*)func_cb.f_cb;

    area_t size = f_alarm_set->size;
    u16 num_height = f_alarm_set->num_height;

    static s32 last_dy = 0;
    s8 dir = 0;
    s32 distance = 0;

    aclock_set_num_item_t *time_item;
    u8 ofs = 0;

    if (id == COMPO_ID_BTN_ACLOCK_HOUR)
    {
        ofs = 0;
        time_item = &f_alarm_set->set_num_item[0];
    }
    else if (id == COMPO_ID_BTN_ACLOCK_MIN)
    {
        ofs = 5;
        time_item = &f_alarm_set->set_num_item[5];
    }

    if (press)
    {
        distance = dy - last_dy;
    }
    else
    {
        dy = 0;
        last_dy = 0;
        distance = 0;
    }

    time_item[0].y += distance;
    time_item[1].y += distance;
    time_item[2].y += distance;
    time_item[3].y += distance;
    time_item[4].y += distance;

    for (int i=0; i<5; i++)
    {
        if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP+ofs)
        {
            if ((time_item[i].y - (ALARM_NUM_PIC_CENTER_Y - size.hei/2)) <= -num_height)
            {
                dir = -1;
                break;
            }
            else if (press == false && ((time_item[i].y - (ALARM_NUM_PIC_CENTER_Y - size.hei/2)) <= -num_height/2))
            {
                dir = -1;
                break;
            }
        }
        else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN+ofs)
        {
            if (time_item[i].y >= (ALARM_NUM_PIC_CENTER_Y + size.hei/2 + num_height))
            {
                dir = 1;
                break;
            }
            else if (press == false && (time_item[i].y >= (ALARM_NUM_PIC_CENTER_Y + size.hei/2 + num_height/2)))
            {
                dir = 1;
                break;
            }
        }
    }

    if (dir)
    {
        for(int i=0; i<5; i++)
        {
            time_item[i].load_id += dir;
            if (time_item[i].load_id < COMPO_ID_NUM_ACLOCK_SET_HOUR_UU+ofs)
            {
                time_item[i].load_id = COMPO_ID_NUM_ACLOCK_SET_HOUR_DD+ofs;
            }
            else if (time_item[i].load_id > COMPO_ID_NUM_ACLOCK_SET_HOUR_DD+ofs)
            {
                time_item[i].load_id = COMPO_ID_NUM_ACLOCK_SET_HOUR_UU+ofs;
            }
        }

        if (id == COMPO_ID_BTN_ACLOCK_HOUR)
        {
            f_alarm_set->hcnt += dir;
        }
        else if (id == COMPO_ID_BTN_ACLOCK_MIN)
        {
            f_alarm_set->mcnt += dir;
        }
    }

    if (press == false || dir)
    {
        for(int i=0; i<5; i++)
        {
            if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UU+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ALARM_CLOCK_SET_NUM_POS_Y(ALARM_CLOCK_SET_NUM_POS_Y_START,ALARM_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR_UU-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DD+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ALARM_CLOCK_SET_NUM_POS_Y(ALARM_CLOCK_SET_NUM_POS_Y_START,ALARM_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR_DD-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ALARM_CLOCK_SET_NUM_POS_Y(ALARM_CLOCK_SET_NUM_POS_Y_START,ALARM_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR_UP-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ALARM_CLOCK_SET_NUM_POS_Y(ALARM_CLOCK_SET_NUM_POS_Y_START,ALARM_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 255;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = ALARM_CLOCK_SET_NUM_POS_Y(ALARM_CLOCK_SET_NUM_POS_Y_START,ALARM_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 100;
            }
        }
    }

    for (int i=0; i<5; i++)
    {
        compo_textbox_t *txt_num = compo_getobj_byid(time_item[i].num_id);
        if (time_item[i].alpha == 255)
        {
            compo_textbox_set_font(txt_num, ALARM_CLOCK_FOCUS_ITEM_FONT);
        }
        else
        {
            compo_textbox_set_font(txt_num, ALARM_CLOCK_ITEM_FONT);
        }
        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_pos(txt_num, time_item[i].x + func_alarm_get_ofs_x(), time_item[i].y);
        compo_textbox_set_visible(txt_num, time_item[i].visible_en);
        compo_textbox_set_alpha(txt_num, time_item[i].alpha);
    }

    last_dy = dy;

    //printf("===============================\n");
    //for(int i=0; i<5; i++) {
    //    printf("[%d] id:[%d], vis:[%d], xy:[%d,%d], ret:[%d], dir[%d], press:[%d], hcnt:[%d], mcnt[%d]\n", time_item[i].num_id, time_item[i].load_id,
    //            time_item[i].visible_en, time_item[i].x, time_item[i].y, ret, dir, press, f_alarm_set->hcnt, f_alarm_set->mcnt);
    //}
    //printf("===============================\n");
}

static void func_alarm_clock_set_move_handle(u8 id)
{
    f_alarm_clock_sub_set_t *f_alarm_set = (f_alarm_clock_sub_set_t*)func_cb.f_cb;
    aclock_set_num_item_t *time_item;
    bool flag_press = 0;
//    u32 tick = tick_get();
    s32 dx = 0, dy = 0;     //坐标差量
    char buf[4];

    static u8 am_pm_last = 0;

    if (id == COMPO_ID_BTN_ACLOCK_HOUR)
    {
        time_item = &f_alarm_set->set_num_item[0];
    }
    else if (id == COMPO_ID_BTN_ACLOCK_MIN)
    {
        time_item = &f_alarm_set->set_num_item[5];
    }

    u8 hour = sys_cb.alarm_edit_hour;
    u8 min = sys_cb.alarm_edit_min;

    s8 hour_disp = hour, min_disp = min;
    for (;;)
    {
        flag_press = ctp_get_dxy(&dx, &dy);
        //printf("x:%d, y:%d\n",dx, dy);

        alarm_clock_set_num_pos_cal(dy, id, flag_press);
        hour_disp = (hour - f_alarm_set->hcnt) % 24;
        min_disp = (min - f_alarm_set->mcnt) % 60;

        if (1) //(tick_check_expire(tick, 15)) {      //15ms刷新一下数字框
        {
//            tick = tick_get();
            if (id == COMPO_ID_BTN_ACLOCK_HOUR)
            {
                for (int idx = 0; idx < 5; idx++)
                {
                    memset(buf, 0, sizeof(buf));
                    compo_textbox_t *txt_num = compo_getobj_byid(time_item[idx].num_id);
//                    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_BIN);
                    if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UU)        //上上次的时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(func_alarm_clock_get_time_cal(hour_disp, 1, 1), 1, 1)).hour);
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP)     //上次的时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(hour_disp, 1, 1)).hour);
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR)      //当前时间
                    {
//                        compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(hour_disp, 1, 0)).hour);
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN)     //下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(hour_disp, 1, 2)).hour);
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DD)       //下下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(func_alarm_clock_get_time_cal(hour_disp, 1, 2), 1, 2)).hour);
                    }
                    compo_textbox_set(txt_num, buf);
                }
            }
            else if (id == COMPO_ID_BTN_ACLOCK_MIN)
            {
                for (int idx = 0; idx < 5; idx++)
                {
                    memset(buf, 0, sizeof(buf));
                    compo_textbox_t *txt_num = compo_getobj_byid(time_item[idx].num_id);
//                    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_BIN);
                    if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UU)        //上上次的时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(func_alarm_clock_get_time_cal(min_disp, 0, 1), 0, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UP)     //上次的时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(min_disp, 0, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN)      //当前时间
                    {
//                        compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(min_disp, 0, 0));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN)     //下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(min_disp, 0, 2));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DD)       //下下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_alarm_clock_get_time_cal(func_alarm_clock_get_time_cal(min_disp, 0, 2), 0, 2));
                    }
                    compo_textbox_set(txt_num, buf);
                }
            }
        }

        compo_textbox_t* am_pm_txt = compo_getobj_byid(COMPO_ID_TXT_AM_PM);
        u8 am_pm = func_alarm_convert_to_12hour(func_alarm_clock_get_time_cal(hour_disp, 1, 0)).am_pm;
        if (am_pm != 0 && am_pm_last != am_pm)
        {
            am_pm_last = am_pm;
            if (am_pm_txt != NULL)
            {
                if (am_pm_last == 1)                //AM
                {
                    compo_textbox_set(am_pm_txt, i18n[STR_AM]);
                }
                else if (am_pm_last == 2)           //PM
                {
                    compo_textbox_set(am_pm_txt, i18n[STR_PM]);
                }
            }
        }

        if (!flag_press)
        {
            f_alarm_set->hcnt = 0;
            f_alarm_set->mcnt = 0;
            hour = func_alarm_clock_get_time_cal(hour_disp, 1, 0);
            min = func_alarm_clock_get_time_cal(min_disp, 0, 0);

            sys_cb.alarm_edit_hour = hour;
            sys_cb.alarm_edit_min = min;
            printf("alarm%d_edit[%02d:%02d]\n", sys_cb.alarm_edit_idx, sys_cb.alarm_edit_hour, sys_cb.alarm_edit_min);
            break;
        }
        evt_message(msg_dequeue());
        func_process();
    }

}

//触摸按钮效果处理
static void func_alarm_clock_sub_set_button_touch_handle(void)
{

    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_CONTINUE:
            compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_CONTINUE_CLICK), true);
            break;

        case COMPO_ID_BTN_REPETAT_NO:
            compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_REPETAT_NO_CLICK), true);
            break;

        case COMPO_ID_BTN_REPETAT_YES:
            compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_REPETAT_YES_CLICK), true);
            break;

        case COMPO_ID_BTN_ACLOCK_HOUR:
        case COMPO_ID_BTN_ACLOCK_MIN:
            func_alarm_clock_set_move_handle(id);
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_alarm_clock_sub_set_button_release_handle(void)
{
    if (ALARM_IS_FREE(sys_cb.alarm_edit_idx))   //add
    {
        compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_CONTINUE_CLICK), false);
    }
    else        //edit
    {
        compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_REPETAT_NO_CLICK), false);
        compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_REPETAT_YES_CLICK), false);
    }
}

//单击按钮
static void func_alarm_clock_sub_set_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_CONTINUE:
            func_cb.sta = FUNC_ALARM_CLOCK_SUB_REPEAT;
            break;

        case COMPO_ID_BTN_REPETAT_NO:
            func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
            break;

        case COMPO_ID_BTN_REPETAT_YES:
            ALARM_EDIT(sys_cb.alarm_edit_idx,
                       ALARM_GET_SWITCH(sys_cb.alarm_edit_idx),
                       ALARM_GET_CYCLE(sys_cb.alarm_edit_idx),
                       sys_cb.alarm_edit_hour,
                       sys_cb.alarm_edit_min,
                       0,
                       0);
            func_cb.sta = FUNC_ALARM_CLOCK_SUB_EDIT;
            break;

        default:
            break;
    }

    func_alarm_clock_sub_set_button_release_handle();
}

//闹钟设置功能事件处理
static void func_alarm_clock_sub_set_process(void)
{
    compo_textbox_t* txt_am_pm = compo_getobj_byid(COMPO_ID_TXT_AM_PM);
    if (uteModuleSystemtime12HOn())
    {
        compo_textbox_set_visible(txt_am_pm, true);
    }
    else
    {
        compo_textbox_set_visible(txt_am_pm, false);
    }

    func_process();
}

//闹钟设置功能消息处理
static void func_alarm_clock_sub_set_message(size_msg_t msg)
{
    f_alarm_clock_sub_set_t *f_alarm_set = (f_alarm_clock_sub_set_t*) func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_alarm_clock_sub_set_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_alarm_clock_sub_set_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_alarm_clock_sub_set_button_release_handle();
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_alarm_clock_sub_set_button_release_handle();
            func_message(msg);
            break;

        case MSG_QDEC_BACKWARD:
        case MSG_QDEC_FORWARD:
            break;

        case MSG_SYS_500MS:
            if (f_alarm_set->time_scale != uteModuleSystemtime12HOn())
            {
                if (func_cb.frm_main != NULL)
                {
                    compo_form_destroy(func_cb.frm_main);
                }
                func_alarm_clock_sub_set_exit();
                if (func_cb.f_cb != NULL)
                {
                    func_free(func_cb.f_cb);
                    func_cb.f_cb = NULL;
                }
                func_alarm_clock_sub_set_enter();
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入闹钟设置功能
static void func_alarm_clock_sub_set_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_alarm_clock_sub_set_t));
    func_cb.frm_main = func_alarm_clock_sub_set_form_create();

    f_alarm_clock_sub_set_t *f_alarm_set = (f_alarm_clock_sub_set_t*) func_cb.f_cb;
    memcpy(f_alarm_set->set_num_item, tbl_aclock_set_num_item, sizeof(tbl_aclock_set_num_item));
    f_alarm_set->size = gui_image_get_size(UI_BUF_ALARM_CLOCK_NUM_BIN);
    f_alarm_set->num_height = widget_text_get_height();

    func_cb.enter_tick = tick_get();

    f_alarm_set->time_scale = uteModuleSystemtime12HOn();
}

//退出闹钟设置功能
static void func_alarm_clock_sub_set_exit(void)
{
}

//闹钟设置功能
void func_alarm_clock_sub_set(void)
{
    printf("%s\n", __func__);
    func_alarm_clock_sub_set_enter();
    while (func_cb.sta == FUNC_ALARM_CLOCK_SUB_SET)
    {
        func_alarm_clock_sub_set_process();
        func_alarm_clock_sub_set_message(msg_dequeue());
    }
    func_alarm_clock_sub_set_exit();
}
