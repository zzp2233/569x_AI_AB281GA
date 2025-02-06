#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_CONTINUE = 1,
    COMPO_ID_BTN_ACLOCK_HOUR,
    COMPO_ID_BTN_ACLOCK_MIN,
    COMPO_ID_BTN_YES_CLICK,
    COMPO_ID_BTN_NO_CLICK,
//
//    //图像
//  COMPO_ID_PIC_CONTINUE_CLICK,

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
};

typedef struct disturd_set_num_item_t_
{
    u32 res_addr;
    int num_cnt;
    u16 num_id;
    s16 load_id;
    int val;
    s16 x;
    s16 y;
    bool visible_en;
    u8 alpha;
} disturd_set_num_item_t;

typedef struct f_disturd_sub_set_t_
{
    disturd_set_num_item_t set_num_item[11];
    area_t size;
    u16 num_height;
    s16 hcnt;
    s16 mcnt;
} f_disturd_sub_set_t;

#define DISTURD_SET_NUM_ITEM_CNT            ((int)(sizeof(tbl_disturd_set_num_item) / sizeof(tbl_disturd_set_num_item[0])))

#define ALARM_NUM_PIC_CENTER_X              160     //时间框的中心坐标
#define ALARM_NUM_PIC_CENTER_Y              164
#define ALARM_TXT_ITEM_H_X                  95
#define ALARM_TXT_ITEM_M_X                  225
#define ALARM_TXT_ITEM_Y                    65
#define ALARM_TXT_ITEM_Y_OFFSET             51

#define ALARM_CLOCK_SET_NUM_POS_Y_START                 ALARM_TXT_ITEM_Y
#define ALARM_CLOCK_SET_NUM_POS_Y(y_start,space,cnt)    (y_start)+(space)*(cnt)

//搞个数字item，创建时遍历一下
static const disturd_set_num_item_t tbl_disturd_set_num_item[] =
{
    /*   res_addr,                   num_cnt,                 num_id,                           load_num_id                val,   x,                     y,                                             visible_en      alpha*/
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_UU,      COMPO_ID_NUM_ACLOCK_SET_HOUR_UU,    22,   ALARM_TXT_ITEM_H_X,    ALARM_TXT_ITEM_Y,                              true,           100},
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_UP,      COMPO_ID_NUM_ACLOCK_SET_HOUR_UP,    23,   ALARM_TXT_ITEM_H_X,    ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET,      true,           100},
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR,         COMPO_ID_NUM_ACLOCK_SET_HOUR,       0,    ALARM_TXT_ITEM_H_X,    ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN,    COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN,  1,    ALARM_TXT_ITEM_H_X,    ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_DD,      COMPO_ID_NUM_ACLOCK_SET_HOUR_DD,    2,    ALARM_TXT_ITEM_H_X,    ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*4,    true,           100},

    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN_UU,       COMPO_ID_NUM_ACLOCK_SET_MIN_UU,     58,   ALARM_TXT_ITEM_M_X,    ALARM_TXT_ITEM_Y,                              true,           100},
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN_UP,       COMPO_ID_NUM_ACLOCK_SET_MIN_UP,     59,   ALARM_TXT_ITEM_M_X,    ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET,      true,           100},
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN,          COMPO_ID_NUM_ACLOCK_SET_MIN,        0,    ALARM_TXT_ITEM_M_X,    ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN,     COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN,   1,    ALARM_TXT_ITEM_M_X,    ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {UI_BUF_0FONT_FONT_NUM_24_BIN,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN_DD,       COMPO_ID_NUM_ACLOCK_SET_MIN_DD,     2,    ALARM_TXT_ITEM_M_X,    ALARM_TXT_ITEM_Y+ALARM_TXT_ITEM_Y_OFFSET*4,    true,           100},

};

//获取 时分 上一个和下一个的数字
static u8 func_disturd_get_time_cal(s8 num, bool hour_en, u8 mode)     //mode 0:返回当前hour/min，1：返回上一个hour/min，2：返回下一个hour/min
{
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

//创建定时勿扰设置窗体
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    if(sys_cb.disturd_sel)
    {
        compo_form_set_title(frm, i18n[STR_DISTURD_TIM_END]);
    }
    else
    {
        compo_form_set_title(frm, i18n[STR_DISTURD_TIM_START]);
    }

    compo_picturebox_t *pic_bg = compo_picturebox_create(frm, UI_BUF_ALARM_CLOCK_NUM_BIN);
    compo_picturebox_set_pos(pic_bg, ALARM_NUM_PIC_CENTER_X, ALARM_NUM_PIC_CENTER_Y);

    area_t size = gui_image_get_size(UI_BUF_ALARM_CLOCK_NUM_BIN);

    //创建数字
    compo_textbox_t *num;
    char str_buff[4];
    u8 hour, min;
    if(sys_cb.disturd_sel)
    {
        if (func_cb.last == FUNC_ALARM_CLOCK_SUB_EDIT)
        {
            hour = sys_cb.disturd_end_time_sec / 3600;
            min = (sys_cb.disturd_end_time_sec % 3600) / 60;
        }
        else
        {
            hour = sys_cb.disturd_end_time_sec / 3600;
            min = (sys_cb.disturd_end_time_sec % 3600) / 60;
        }
    }
    else
    {
        if (func_cb.last == FUNC_ALARM_CLOCK_SUB_EDIT)
        {
            hour = sys_cb.disturd_start_time_sec / 3600;
            min = (sys_cb.disturd_start_time_sec % 3600) / 60;
        }
        else
        {
            hour = sys_cb.disturd_start_time_sec / 3600;
            min = (sys_cb.disturd_start_time_sec % 3600) / 60;
        }
    }
    for (u8 idx = 0; idx < DISTURD_SET_NUM_ITEM_CNT; idx++)
    {
        num = compo_textbox_create(frm, tbl_disturd_set_num_item[idx].num_cnt);
        compo_textbox_set_font(num, tbl_disturd_set_num_item[idx].res_addr);
        compo_setid(num, tbl_disturd_set_num_item[idx].num_id);
        compo_textbox_set_pos(num, tbl_disturd_set_num_item[idx].x, tbl_disturd_set_num_item[idx].y);
        compo_textbox_set_visible(num, tbl_disturd_set_num_item[idx].visible_en);
        compo_textbox_set_alpha(num, tbl_disturd_set_num_item[idx].alpha);

        if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UU)        //上上次的时间
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(hour, 1, 1), 1, 1));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP)     //上次的时间
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(hour, 1, 1));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR)      //当前时间
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", hour);
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN)     //下次时间
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(hour, 1, 2));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DD)       //下下次时间
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(hour, 1, 2), 1, 2));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UU)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(min, 0, 1), 0, 1));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UP)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(min, 0, 1));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", min);
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(min, 0, 2));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DD)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(min, 0, 2), 0, 2));
        }
        compo_textbox_set(num, str_buff);
    }

    //画两个矩形遮挡框框外面的数字
    compo_shape_t *shape_up = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(shape_up, COLOR_BLACK);
    compo_shape_set_location(shape_up, 160, 60, size.wid, 42);

    compo_shape_t *shape_down = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(shape_down, COLOR_BLACK);
    compo_shape_set_location(shape_down, 160, 310, size.wid, 124);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_ACLOCK_HOUR);
    compo_button_set_location(btn, 94, 160, 100, 170);

    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BTN_ACLOCK_MIN);
    compo_button_set_location(btn, 225, 160, 100, 170);

    btn = compo_button_create_by_image(frm, UI_BUF_ALARM_CLOCK_YES_BIN);
    compo_setid(btn, COMPO_ID_BTN_YES_CLICK);
    compo_button_set_pos(btn, 160, 321);

    return frm;
}

static void disturd_set_num_pos_cal(s32 dy, u8 id, bool press)
{
    f_disturd_sub_set_t *f_alarm_set = (f_disturd_sub_set_t*)func_cb.f_cb;

    area_t size = f_alarm_set->size;
    u16 num_height = f_alarm_set->num_height;

    static s32 last_dy = 0;
    s8 dir = 0;
    s32 distance = 0;

    disturd_set_num_item_t *time_item;
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
        compo_textbox_t *num = compo_getobj_byid(time_item[i].num_id);
        compo_textbox_set_pos(num, time_item[i].x, time_item[i].y);
        compo_textbox_set_visible(num, time_item[i].visible_en);
        compo_textbox_set_alpha(num, time_item[i].alpha);
    }

    last_dy = dy;
}

static void func_disturd_set_move_handle(u8 id)
{
    f_disturd_sub_set_t *f_alarm_set = (f_disturd_sub_set_t*)func_cb.f_cb;
    disturd_set_num_item_t *time_item;
    bool flag_press = 0;
    u32 tick = tick_get();
    s32 dx = 0, dy = 0;     //坐标差量
    char str_buff[4];

    if (id == COMPO_ID_BTN_ACLOCK_HOUR)
    {
        time_item = &f_alarm_set->set_num_item[0];
    }
    else if (id == COMPO_ID_BTN_ACLOCK_MIN)
    {
        time_item = &f_alarm_set->set_num_item[5];
    }

    u8 hour, min;

    if(sys_cb.disturd_sel)
    {
        if (func_cb.last == FUNC_ALARM_CLOCK_SUB_EDIT)
        {
            hour = sys_cb.disturd_end_time_sec / 3600;
            min = (sys_cb.disturd_end_time_sec % 3600) / 60;
        }
        else
        {
            hour = sys_cb.disturd_end_time_sec / 3600;
            min = (sys_cb.disturd_end_time_sec % 3600) / 60;
        }
    }
    else
    {
        if (func_cb.last == FUNC_ALARM_CLOCK_SUB_EDIT)
        {
            hour = sys_cb.disturd_start_time_sec / 3600;
            min = (sys_cb.disturd_start_time_sec % 3600) / 60;
        }
        else
        {
            hour = sys_cb.disturd_start_time_sec / 3600;
            min = (sys_cb.disturd_start_time_sec % 3600) / 60;
        }
    }

    s8 hour_disp = hour, min_disp = min;
    for (;;)
    {
        flag_press = ctp_get_dxy(&dx, &dy);
        //printf("x:%d, y:%d\n",dx, dy);

        disturd_set_num_pos_cal(dy, id, flag_press);
        hour_disp = (hour - f_alarm_set->hcnt) % 24;
        min_disp = (min - f_alarm_set->mcnt) % 60;

        if (tick_check_expire(tick, 15))        //15ms刷新一下数字框
        {
            tick = tick_get();
            if (id == COMPO_ID_BTN_ACLOCK_HOUR)
            {
                for (int idx = 0; idx < 5; idx++)
                {
                    compo_textbox_t *num = compo_getobj_byid(time_item[idx].num_id);
                    if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UU)        //上上次的时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(hour_disp, 1, 1), 1, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP)     //上次的时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(hour_disp, 1, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR)      //当前时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(hour_disp, 1, 0));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN)     //下次时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(hour_disp, 1, 2));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DD)       //下下次时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(hour_disp, 1, 2), 1, 2));
                    }
                    compo_textbox_set(num, str_buff);
                }
            }
            else if (id == COMPO_ID_BTN_ACLOCK_MIN)
            {
                for (int idx = 0; idx < 5; idx++)
                {
                    compo_textbox_t *num = compo_getobj_byid(time_item[idx].num_id);
                    if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UU)        //上上次的时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(min_disp, 0, 1), 0, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UP)     //上次的时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(min_disp, 0, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN)      //当前时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(min_disp, 0, 0));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN)     //下次时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(min_disp, 0, 2));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DD)       //下下次时间
                    {
                        snprintf(str_buff, sizeof(str_buff), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(min_disp, 0, 2), 0, 2));
                    }
                    compo_textbox_set(num, str_buff);
                }
            }
        }

        if (!flag_press)
        {
            f_alarm_set->hcnt = 0;
            f_alarm_set->mcnt = 0;
            hour = func_disturd_get_time_cal(hour_disp, 1, 0);
            min = func_disturd_get_time_cal(min_disp, 0, 0);
            if (sys_cb.disturd_sel)
            {
                sys_cb.disturd_end_time_sec = hour * 3600 + min * 60;
            }
            else
            {
                sys_cb.disturd_start_time_sec = hour * 3600 + min * 60;
            }
            printf("total_sec:%d, hour:%d, min:%d\n",sys_cb.disturd_start_time_sec, hour, min);
            break;
        }
        evt_message(msg_dequeue());
        func_process();
    }

}

//触摸按钮效果处理
static void func_disturd_sub_set_button_touch_handle(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_ACLOCK_HOUR:
        case COMPO_ID_BTN_ACLOCK_MIN:
            func_disturd_set_move_handle(id);
            break;

        default:
            break;
    }

}

//释放按钮效果处理
static void func_disturd_sub_set_button_release_handle(void)
{
    return;
}

//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_YES_CLICK:
            if (func_cb.last == FUNC_SET_SUB_DISTURD)
            {
                func_cb.sta = FUNC_SET_SUB_DISTURD;
                task_stack_pop();
            }

            break;

        case COMPO_ID_BTN_NO_CLICK:
            if (func_cb.last == FUNC_SET_SUB_DISTURD)
            {
                func_cb.sta = FUNC_SET_SUB_DISTURD;
                task_stack_pop();
            }

            break;

        default:
            break;
    }

    func_disturd_sub_set_button_release_handle();
}

//定时勿扰设置功能事件处理
static void func_disturd_sub_set_process(void)
{
    func_process();
}

//定时勿扰设置功能消息处理
static void func_disturd_sub_set_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_disturd_sub_set_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_disturd_sub_set_button_click();
            break;

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_disturd_sub_set_button_release_handle();
            break;

        case MSG_CTP_SHORT_RIGHT:
            break;

        case MSG_QDEC_BACKWARD:
        case MSG_QDEC_FORWARD:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入定时勿扰设置功能
static void func_disturd_sub_set_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_disturd_sub_set_t));
    func_cb.frm_main = func_disturd_sub_set_form_create();

    f_disturd_sub_set_t *f_alarm_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    memcpy(f_alarm_set->set_num_item, tbl_disturd_set_num_item, sizeof(tbl_disturd_set_num_item));
    f_alarm_set->size = gui_image_get_size(UI_BUF_ALARM_CLOCK_NUM_BIN);
    f_alarm_set->num_height = widget_text_get_height();

    func_cb.enter_tick = tick_get();
}

//退出定时勿扰设置功能
static void func_disturd_sub_set_exit(void)
{
    printf("%s\n",__func__);
    func_cb.last = FUNC_DISTURD_SUB_SET;

    compo_form_destroy(func_cb.frm_main);
    func_free(func_cb.f_cb);
    func_cb.frm_main = NULL;
    func_cb.f_cb = NULL;
}

//定时勿扰设置功能
void func_disturd_sub_set(void)
{
    printf("%s\n", __func__);
    func_disturd_sub_set_enter();
    while (func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        func_disturd_sub_set_process();
        func_disturd_sub_set_message(msg_dequeue());
    }
    func_disturd_sub_set_exit();
}
