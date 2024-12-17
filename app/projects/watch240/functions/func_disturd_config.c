#include "include.h"
#include "func.h"
#include "ute_module_notdisturb.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//组件ID
enum
{
//    //按键
//    COMPO_ID_BTN_CONTINUE = 1,
//    COMPO_ID_BTN_ACLOCK_HOUR,
//    COMPO_ID_BTN_ACLOCK_MIN,
//    COMPO_ID_BTN_YES_CLICK,
//    COMPO_ID_BTN_NO_CLICK,
////
////    //图像
////  COMPO_ID_PIC_CONTINUE_CLICK,
//
//    //数字
//    COMPO_ID_NUM_ACLOCK_SET_HOUR_UU,
//    COMPO_ID_NUM_ACLOCK_SET_HOUR_UP,
//    COMPO_ID_NUM_ACLOCK_SET_HOUR,
//    COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN,
//    COMPO_ID_NUM_ACLOCK_SET_HOUR_DD,
//
//    COMPO_ID_NUM_ACLOCK_SET_MIN_UU,
//    COMPO_ID_NUM_ACLOCK_SET_MIN_UP,
//    COMPO_ID_NUM_ACLOCK_SET_MIN,
//    COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN,
//    COMPO_ID_NUM_ACLOCK_SET_MIN_DD,

    //按键
    COMPO_ID_BTN_CONTINUE = 1,              //闹钟添加设置模式确定按钮（未按下状态）
    COMPO_ID_PIC_CONTINUE_CLICK,            //闹钟添加设置模式确定按钮（按下状态）
//    COMPO_ID_BTN_REPETAT_YES,               //闹钟编辑设置模式确定按钮（未按下状态）
//    COMPO_ID_PIC_REPETAT_YES_CLICK,         //闹钟编辑设置模式确定按钮（按下状态）


    COMPO_ID_BTN_ACLOCK_HOUR,               //设置模式时用于检测时间设置滑动检测
    COMPO_ID_BTN_ACLOCK_MIN,
    COMPO_ID_BTN_ACLOCK_AM_PM,

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

    COMPO_ID_NUM_ACLOCK_SET_SCALE_UU,
    COMPO_ID_NUM_ACLOCK_SET_SCALE_UP,
    COMPO_ID_NUM_ACLOCK_SET_SCALE_CENTER,
    COMPO_ID_NUM_ACLOCK_SET_SCALE_DOWN,
    COMPO_ID_NUM_ACLOCK_SET_SCALE_DD,

    //图片
    COMPO_ID_PIC_BG_HOUR,
    COMPO_ID_PIC_BG_MIN,
    COMPO_ID_PIC_BG_AM_PM,
    COMPO_ID_PIC_COLON,


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

typedef struct aclock_set_ampm_item_t_
{
    int num_cnt;
    u16 num_id;
    s16 load_id;
    //int val;
    s16 x;
    s16 y;
    bool visible_en;
    u8 alpha;
} disturd_set_ampm_item_t;

typedef struct f_disturd_sub_set_t_
{
    disturd_set_num_item_t set_num_item[11];
    disturd_set_ampm_item_t set_ampm_item[5];

    area_t size;
    u16 num_height;
    s16 hcnt;
    s16 mcnt;
    s16 am_pm_cnt;

    bool time_scale;
    u8 time_am_pm;
} f_disturd_sub_set_t;


typedef struct disturd_set_pic_bg_t_
{

    s16 ofs_24to12_bg_x_hour;
    s16 ofs_24to12_bg_x_min;
    s16 ofs_24to12_bg_x_colon;

    struct ui_disturd_set_bg
    {
        u16 id;
        s16 x, y;
        u32 res_24_bright;
        u32 res_24_destory;
        u32 res_12_bright;
        u32 res_12_destory;
    } bg_hour, bg_min, bg_am_pm;

    struct ui_disturd_set_bg_colon
    {
        u16 id;
        s16 x, y;
        u32 res;
    } bg_colon;


    struct ui_disturd_set_btn
    {
        u16 id;
    } btn_hour, btn_min, btn_am_pm;

    struct ui_disturd_set_btn_ok
    {
        u16 id1;
        u16 id1_click;
        u16 id2;
        u16 id2_click;
        s16 x,y;
        u16 w,h;
        u32 res1;
        u32 res2;
        u16 str_id;
        u16 font_w, font_h;
    } btn_ok;

} disturd_set_pic_bg_t;


//时间设置界面整体资源和坐标设置
static const disturd_set_pic_bg_t tbl_disturd_set_pic_bg =
{
    .ofs_24to12_bg_x_hour = 66,
    .ofs_24to12_bg_x_min = 21,
    .ofs_24to12_bg_x_colon = 44,

    .bg_hour = {
        .id = COMPO_ID_PIC_BG_HOUR,
        .x  = 4+100/2, //91+58/2  54,120,66
        .y  = 62+130/2,
        .res_24_bright  = UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIG_BIN,
        .res_24_destory = UI_BUF_I330001_PUBLIC_KUANG_GREY_BIG_BIN,
        .res_12_bright  = UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN,
        .res_12_destory = UI_BUF_I330001_PUBLIC_KUANG_GREY_SMALL_BIN,
    },

    .bg_min = {
        .id = COMPO_ID_PIC_BG_MIN,
        .x  = 136+100/2,    //178+58/2  186,207,21
        .y  = 62+130/2,
        .res_24_bright  = UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIG_BIN,
        .res_24_destory = UI_BUF_I330001_PUBLIC_KUANG_GREY_BIG_BIN,
        .res_12_bright  = UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN,
        .res_12_destory = UI_BUF_I330001_PUBLIC_KUANG_GREY_SMALL_BIN,
    },

    .bg_am_pm = {
        .id = COMPO_ID_PIC_BG_AM_PM,
        .x  = 4+58/2,
        .y  = 62+130/2,//127
        .res_24_bright  = 0,
        .res_24_destory = 0,
        .res_12_bright  = UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN,
        .res_12_destory = UI_BUF_I330001_PUBLIC_KUANG_GREY_SMALL_BIN,
    },

    .bg_colon = {
        .id = COMPO_ID_PIC_COLON,
        .x  = 116+6/2,  //160+6/2  119 163, 44
        .y  = 113+24/2,//125
        .res    = UI_BUF_I330001_PUBLIC_KUANG_COLON_BIN,
    },

    .btn_hour = {
        .id = COMPO_ID_BTN_ACLOCK_HOUR,
    },

    .btn_min = {
        .id = COMPO_ID_BTN_ACLOCK_MIN,
    },

    .btn_am_pm = {
        .id = COMPO_ID_BTN_ACLOCK_AM_PM,
    },

    .btn_ok = {
        .id1 = COMPO_ID_BTN_CONTINUE,
        .id1_click = COMPO_ID_PIC_CONTINUE_CLICK,
        .id2 = 0,
        .id2_click = 0,
        .x  = 13+214/2,
        .y  = 216+62/2,
        .w  = 0,
        .h  = 0,
        .res1 = UI_BUF_I330001_PUBLIC_RECTANGLE02_BIN,
        .res2 = UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN,
        .str_id = STR_OK,
        .font_w = 180,
        .font_h = 20,
    },

};

///时间 HOUR 与 MIN 坐标设置信息  用于实现 HOUR 与 MIN 滚动显示
#define DISTURD_FOCUS_ITEM_FONT         UI_BUF_0FONT_FONT_NUM_24_BIN
#define DISTURD_ITEM_FONT               UI_BUF_0FONT_FONT_NUM_24_BIN
#define DISTURD_FONT_HEIGHT             24

#define DISTURD_SET_NUM_ITEM_CNT            ((int)(sizeof(tbl_disturd_set_num_item) / sizeof(tbl_disturd_set_num_item[0])))

#define DISTURD_TXT_ITEM_H_X                  51
#define DISTURD_TXT_ITEM_M_X                  181
#define DISTURD_TXT_ITEM_Y                    -25
#define DISTURD_TXT_ITEM_Y_OFFSET             45

#define DISTURD_SET_NUM_POS_Y_START                 DISTURD_TXT_ITEM_Y
#define DISTURD_SET_NUM_POS_Y(y_start,space,cnt)    (y_start)+(space)*(cnt)

//搞个数字item，创建时遍历一下
static const disturd_set_num_item_t tbl_disturd_set_num_item[] =
{
    /*   res_addr,                   num_cnt,                 num_id,                           load_num_id                val,   x,                     y,                                             visible_en      alpha*/
    {DISTURD_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_UU,      COMPO_ID_NUM_ACLOCK_SET_HOUR_UU,    22,   DISTURD_TXT_ITEM_H_X,    DISTURD_TXT_ITEM_Y,                              true,           100},
    {DISTURD_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_UP,      COMPO_ID_NUM_ACLOCK_SET_HOUR_UP,    23,   DISTURD_TXT_ITEM_H_X,    DISTURD_TXT_ITEM_Y+DISTURD_TXT_ITEM_Y_OFFSET,      true,           100},
    {DISTURD_FOCUS_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR,         COMPO_ID_NUM_ACLOCK_SET_HOUR,       0,    DISTURD_TXT_ITEM_H_X,    DISTURD_TXT_ITEM_Y+DISTURD_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {DISTURD_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN,    COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN,  1,    DISTURD_TXT_ITEM_H_X,    DISTURD_TXT_ITEM_Y+DISTURD_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {DISTURD_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_HOUR_DD,      COMPO_ID_NUM_ACLOCK_SET_HOUR_DD,    2,    DISTURD_TXT_ITEM_H_X,    DISTURD_TXT_ITEM_Y+DISTURD_TXT_ITEM_Y_OFFSET*4,    true,           100},

    {DISTURD_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN_UU,       COMPO_ID_NUM_ACLOCK_SET_MIN_UU,     58,   DISTURD_TXT_ITEM_M_X,    DISTURD_TXT_ITEM_Y,                              true,           100},
    {DISTURD_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN_UP,       COMPO_ID_NUM_ACLOCK_SET_MIN_UP,     59,   DISTURD_TXT_ITEM_M_X,    DISTURD_TXT_ITEM_Y+DISTURD_TXT_ITEM_Y_OFFSET,      true,           100},
    {DISTURD_FOCUS_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN,          COMPO_ID_NUM_ACLOCK_SET_MIN,        0,    DISTURD_TXT_ITEM_M_X,    DISTURD_TXT_ITEM_Y+DISTURD_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {DISTURD_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN,     COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN,   1,    DISTURD_TXT_ITEM_M_X,    DISTURD_TXT_ITEM_Y+DISTURD_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {DISTURD_ITEM_FONT,      2,        COMPO_ID_NUM_ACLOCK_SET_MIN_DD,       COMPO_ID_NUM_ACLOCK_SET_MIN_DD,     2,    DISTURD_TXT_ITEM_M_X,    DISTURD_TXT_ITEM_Y+DISTURD_TXT_ITEM_Y_OFFSET*4,    true,           100},

};

///时间 AM PM 坐标设置信息  用于实现AM PM 滚动显示
#define DISTURD_FONT_AMPM_HEIGHT        20

#define DISTURD_SET_AMPM_ITEM_CNT             ((int)(sizeof(tbl_disturd_set_ampm_item) / sizeof(tbl_disturd_set_ampm_item[0])))

#define DISTURD_SCALE_TXT_ITEM_M_X                  4+58/2
#define DISTURD_SCALE_TXT_ITEM_Y                    -25
#define DISTURD_SCALE_TXT_ITEM_Y_OFFSET             45

#define DISTURD_SCALE_CLOCK_SET_NUM_POS_Y_START                 DISTURD_SCALE_TXT_ITEM_Y
#define DISTURD_SCALE_CLOCK_SET_NUM_POS_Y(y_start,space,cnt)    ((y_start)+(space)*(cnt))

//搞个数字item，创建时遍历一下
static const disturd_set_ampm_item_t tbl_disturd_set_ampm_item[] =
{
    /*   num_cnt,      num_id,             load_num_id              val,                 x,                       y,                                                         visible_en      alpha*/
    {10,        COMPO_ID_NUM_ACLOCK_SET_SCALE_UU,       COMPO_ID_NUM_ACLOCK_SET_SCALE_UU,          /*2,*/          DISTURD_SCALE_TXT_ITEM_M_X,         DISTURD_SCALE_TXT_ITEM_Y,                                    true,           100},
    {10,        COMPO_ID_NUM_ACLOCK_SET_SCALE_UP,       COMPO_ID_NUM_ACLOCK_SET_SCALE_UP,          /*1,*/          DISTURD_SCALE_TXT_ITEM_M_X,         DISTURD_SCALE_TXT_ITEM_Y+DISTURD_SCALE_TXT_ITEM_Y_OFFSET,      true,           100},
    {10,        COMPO_ID_NUM_ACLOCK_SET_SCALE_CENTER,   COMPO_ID_NUM_ACLOCK_SET_SCALE_CENTER,      /*0,*/          DISTURD_SCALE_TXT_ITEM_M_X,         DISTURD_SCALE_TXT_ITEM_Y+DISTURD_SCALE_TXT_ITEM_Y_OFFSET*2,    true,           255},
    {10,        COMPO_ID_NUM_ACLOCK_SET_SCALE_DOWN,     COMPO_ID_NUM_ACLOCK_SET_SCALE_DOWN,        /*1,*/          DISTURD_SCALE_TXT_ITEM_M_X,         DISTURD_SCALE_TXT_ITEM_Y+DISTURD_SCALE_TXT_ITEM_Y_OFFSET*3,    true,           100},
    {10,        COMPO_ID_NUM_ACLOCK_SET_SCALE_DD,       COMPO_ID_NUM_ACLOCK_SET_SCALE_DD,          /*2,*/          DISTURD_SCALE_TXT_ITEM_M_X,         DISTURD_SCALE_TXT_ITEM_Y+DISTURD_SCALE_TXT_ITEM_Y_OFFSET*4,    true,           100},

};

//模式字符映射表  AM PM 用于实现AM PM 滚动显示
typedef struct aclock_str_ampm_tbl_t_
{
    u16 str_id;
    u16 res;
} aclock_str_ampm_tbl_t;

#define ACLOCK_STR_AMPM_TBL_CNT                ((int)(sizeof(aclock_str_ampm_tbl) / sizeof(aclock_str_ampm_tbl[0])))
static const aclock_str_ampm_tbl_t aclock_str_ampm_tbl[] =
{
    {STR_AM, 0},          //AM0
    {STR_PM, 1},      //PM1
};

//获取映射表 上一个和下一个的对应的下标
static u8 func_disturd_ampm_get_str_cal(s8 num, u8 mode)
{
    if (num < 0)
    {
        num += ACLOCK_STR_AMPM_TBL_CNT;
    }

    if (mode == 1)
    {
        if (num == 0 || num > ACLOCK_STR_AMPM_TBL_CNT - 1)
        {
            return ACLOCK_STR_AMPM_TBL_CNT - 1;
        }
        return num - 1;
    }
    else if (mode == 2)
    {
        if (num >= ACLOCK_STR_AMPM_TBL_CNT - 1)
        {
            return 0;
        }
        return num + 1;
    }
    else
    {
        if (num > ACLOCK_STR_AMPM_TBL_CNT - 1)
        {
            return ACLOCK_STR_AMPM_TBL_CNT - 1;
        }
        return num;
    }
}

//12小时制时，界面对应的背景图X偏移大小
static s16 func_disturd_get_ofs_x(u8 ofs_state)
{
    if(uteModuleSystemtime12HOn())
    {
        if (ofs_state == 0)
        {
            return  tbl_disturd_set_pic_bg.ofs_24to12_bg_x_colon;
        }
        else if (ofs_state == 1)
        {
            return  tbl_disturd_set_pic_bg.ofs_24to12_bg_x_hour;
        }
        else if (ofs_state == 2)
        {
            return  tbl_disturd_set_pic_bg.ofs_24to12_bg_x_min;
        }
    }
    return 0;
}

typedef struct func_disturd_hour_format_t_
{
    u8 hour;
    u8 am_pm;
} func_disturd_hour_format_t;

static func_disturd_hour_format_t func_disturd_convert_to_12hour(s8 hour24)
{
    u8 am_pm = (hour24 >= 12) ? 2 : 1;    //2 PM, 1 AM
    func_disturd_hour_format_t hour12;
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

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, tbl_disturd_set_pic_bg.bg_hour.y, GUI_SCREEN_WIDTH, gui_image_get_size(tbl_disturd_set_pic_bg.bg_hour.res_12_bright).hei);

    char buf[20];

    //创建数字 hour min
    u8 hour, min;
    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec / 3600;
        min = (sys_cb.disturd_end_time_sec % 3600) / 60;
    }
    else
    {
        hour = sys_cb.disturd_start_time_sec / 3600;
        min = (sys_cb.disturd_start_time_sec % 3600) / 60;
    }

    for (u8 idx = 0; idx < DISTURD_SET_NUM_ITEM_CNT; idx++)
    {
        compo_textbox_t *txt_num = compo_textbox_create_for_page(frm, page, tbl_disturd_set_num_item[idx].num_cnt);
        compo_textbox_set_font(txt_num, DISTURD_ITEM_FONT);
        compo_setid(txt_num, tbl_disturd_set_num_item[idx].num_id);
        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_pos(txt_num, tbl_disturd_set_num_item[idx].x +
                              func_disturd_get_ofs_x(tbl_disturd_set_num_item[idx].num_id>=COMPO_ID_NUM_ACLOCK_SET_MIN_UU ? 2:1), tbl_disturd_set_num_item[idx].y);
        compo_textbox_set_visible(txt_num, tbl_disturd_set_num_item[idx].visible_en);
        compo_textbox_set_alpha(txt_num, tbl_disturd_set_num_item[idx].alpha);

//        char buf[4];
        memset(buf, 0, sizeof(buf));
        if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UU)        //上上次的时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(func_disturd_get_time_cal(hour, 1, 1), 1, 1)).hour);
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP)     //上次的时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(hour, 1, 1)).hour);
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR)      //当前时间
        {
            compo_textbox_set_font(txt_num, DISTURD_FOCUS_ITEM_FONT);
            snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(hour).hour);
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN)     //下次时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(hour, 1, 2)).hour);
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DD)       //下下次时间
        {
            snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(func_disturd_get_time_cal(hour, 1, 2), 1, 2)).hour);
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UU)
        {
            snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(min, 0, 1), 0, 1));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UP)
        {
            snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(min, 0, 1));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN)
        {
            compo_textbox_set_font(txt_num, DISTURD_FOCUS_ITEM_FONT);
            snprintf(buf, sizeof(buf), "%02d", min);
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN)
        {
            snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(min, 0, 2));
        }
        else if (tbl_disturd_set_num_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DD)
        {
            snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(min, 0, 2), 0, 2));
        }
        compo_textbox_set(txt_num, buf);
    }


    //创建数字 ampm
    //创建数字 -> 使用数字进行映射: 0 -> 上午, 1 -> 下午；
    s8 am_pm = func_disturd_convert_to_12hour(hour).am_pm;
    s8 am_pm_conv = am_pm - 1;
    if (func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
        f_disturd_set->time_am_pm = am_pm_conv;
    }
    if (am_pm_conv >= 0)
    {
        for (u8 idx = 0; idx < DISTURD_SET_AMPM_ITEM_CNT; idx++)
        {
            compo_textbox_t *txt_str = compo_textbox_create_for_page(frm, page, tbl_disturd_set_ampm_item[idx].num_cnt);
            compo_textbox_set_font(txt_str, UI_BUF_0FONT_FONT_BIN);
            compo_setid(txt_str, tbl_disturd_set_ampm_item[idx].num_id);
            compo_textbox_set_align_center(txt_str, true);
            compo_textbox_set_location(txt_str, tbl_disturd_set_ampm_item[idx].x, tbl_disturd_set_ampm_item[idx].y,
                                       gui_image_get_size(tbl_disturd_set_pic_bg.bg_hour.res_12_bright).wid, DISTURD_FONT_AMPM_HEIGHT);
            if (uteModuleSystemtime12HOn())
            {
                compo_textbox_set_visible(txt_str, tbl_disturd_set_ampm_item[idx].visible_en);
            }
            else
            {
                compo_textbox_set_visible(txt_str, false);
            }
            compo_textbox_set_alpha(txt_str, tbl_disturd_set_ampm_item[idx].alpha);
            memset(buf, 0, sizeof(buf));
            if (tbl_disturd_set_ampm_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_UU)
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(func_disturd_ampm_get_str_cal(am_pm_conv, 1), 1)].str_id]);
            }
            else if (tbl_disturd_set_ampm_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_UP)
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(am_pm_conv, 1)].str_id]);
            }
            else if (tbl_disturd_set_ampm_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_CENTER)
            {
                //compo_textbox_set_font(txt_str, UI_BUF_0FONT_FONT_NUM_24_BIN);
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[am_pm_conv].str_id]);
            }
            else if (tbl_disturd_set_ampm_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_DOWN)
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(am_pm_conv, 2)].str_id]);
            }
            else if (tbl_disturd_set_ampm_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_DD)
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(func_disturd_ampm_get_str_cal(am_pm_conv, 2), 2)].str_id]);
            }
            compo_textbox_set(txt_str, buf);
        }
    }

    //创建时间设置背景框
    compo_picturebox_t *bg_hour = compo_picturebox_create(frm, 0);
    if(uteModuleSystemtime12HOn())
    {
        compo_picturebox_set(bg_hour, tbl_disturd_set_pic_bg.bg_hour.res_12_destory);
    }
    else
    {
        compo_picturebox_set(bg_hour, tbl_disturd_set_pic_bg.bg_hour.res_24_destory);
    }
    compo_picturebox_set_pos(bg_hour, tbl_disturd_set_pic_bg.bg_hour.x + func_disturd_get_ofs_x(1), tbl_disturd_set_pic_bg.bg_hour.y);
//    compo_picturebox_set_pos(bg_hour, tbl_disturd_set_pic_bg.bg_hour.x, tbl_disturd_set_pic_bg.bg_hour.y);
    compo_setid(bg_hour, tbl_disturd_set_pic_bg.bg_hour.id);

    compo_picturebox_t *bg_min = compo_picturebox_create(frm, 0);
    if(uteModuleSystemtime12HOn())
    {
        compo_picturebox_set(bg_min, tbl_disturd_set_pic_bg.bg_min.res_12_destory);
    }
    else
    {
        compo_picturebox_set(bg_min, tbl_disturd_set_pic_bg.bg_min.res_24_destory);
    }
    compo_picturebox_set_pos(bg_min, tbl_disturd_set_pic_bg.bg_min.x + func_disturd_get_ofs_x(2), tbl_disturd_set_pic_bg.bg_min.y);
    compo_setid(bg_min, tbl_disturd_set_pic_bg.bg_min.id);

    compo_picturebox_t *bg_am_pm = compo_picturebox_create(frm, 0);
    if(uteModuleSystemtime12HOn())
    {
        compo_picturebox_set(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.res_12_destory);
        compo_picturebox_set_visible(bg_am_pm, true);
    }
    else
    {
        compo_picturebox_set(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.res_24_destory);
        compo_picturebox_set_visible(bg_am_pm, false);
    }
    compo_picturebox_set_pos(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.x, tbl_disturd_set_pic_bg.bg_am_pm.y);
    compo_setid(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.id);

    ///创建滑动检测按钮
    //hour
    compo_button_t* btn_hour = compo_button_create(frm);
    if(uteModuleSystemtime12HOn())
    {
        compo_button_set_location(btn_hour, tbl_disturd_set_pic_bg.bg_hour.x + func_disturd_get_ofs_x(1), tbl_disturd_set_pic_bg.bg_hour.y, gui_image_get_size(tbl_disturd_set_pic_bg.bg_hour.res_12_destory).wid,
                                  gui_image_get_size(tbl_disturd_set_pic_bg.bg_hour.res_12_destory).hei);
    }
    else
    {
        compo_button_set_location(btn_hour, tbl_disturd_set_pic_bg.bg_hour.x + func_disturd_get_ofs_x(1), tbl_disturd_set_pic_bg.bg_hour.y, gui_image_get_size(tbl_disturd_set_pic_bg.bg_hour.res_24_destory).wid,
                                  gui_image_get_size(tbl_disturd_set_pic_bg.bg_hour.res_24_destory).hei);
    }
    compo_setid(btn_hour, tbl_disturd_set_pic_bg.btn_hour.id);
    //min
    compo_button_t* btn_min = compo_button_create(frm);
    if(uteModuleSystemtime12HOn())
    {
        compo_button_set_location(btn_min, tbl_disturd_set_pic_bg.bg_min.x + func_disturd_get_ofs_x(2), tbl_disturd_set_pic_bg.bg_min.y, gui_image_get_size(tbl_disturd_set_pic_bg.bg_min.res_12_destory).wid,
                                  gui_image_get_size(tbl_disturd_set_pic_bg.bg_min.res_12_destory).hei);
    }
    else
    {
        compo_button_set_location(btn_min, tbl_disturd_set_pic_bg.bg_min.x + func_disturd_get_ofs_x(2), tbl_disturd_set_pic_bg.bg_min.y, gui_image_get_size(tbl_disturd_set_pic_bg.bg_min.res_24_destory).wid,
                                  gui_image_get_size(tbl_disturd_set_pic_bg.bg_min.res_24_destory).hei);
    }
    compo_setid(btn_min, tbl_disturd_set_pic_bg.btn_min.id);
    //ampm
    compo_button_t* btn_am_pm = compo_button_create(frm);
    if(uteModuleSystemtime12HOn())
    {
        compo_button_set_location(btn_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.x, tbl_disturd_set_pic_bg.bg_am_pm.y, gui_image_get_size(tbl_disturd_set_pic_bg.bg_am_pm.res_12_destory).wid,
                                  gui_image_get_size(tbl_disturd_set_pic_bg.bg_am_pm.res_12_destory).hei);
        compo_button_set_visible(btn_am_pm, true);
    }
    else
    {
        compo_button_set_location(btn_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.x, tbl_disturd_set_pic_bg.bg_am_pm.y, 0, 0);
        compo_button_set_visible(btn_am_pm, false);
    }
    compo_setid(btn_am_pm, tbl_disturd_set_pic_bg.btn_am_pm.id);

        //建立确定按钮 - 未按下图片
    compo_button_t* add_btn = compo_button_create_by_image(frm, tbl_disturd_set_pic_bg.btn_ok.res1);
    compo_button_set_pos(add_btn, tbl_disturd_set_pic_bg.btn_ok.x, tbl_disturd_set_pic_bg.btn_ok.y);
    compo_setid(add_btn, tbl_disturd_set_pic_bg.btn_ok.id1);

    //建立确定按钮图像 - 按下图片
    compo_picturebox_t *pic_click = compo_picturebox_create(frm, tbl_disturd_set_pic_bg.btn_ok.res2);
    compo_picturebox_set_pos(pic_click, tbl_disturd_set_pic_bg.btn_ok.x, tbl_disturd_set_pic_bg.btn_ok.y);
    compo_picturebox_set_visible(pic_click, false);
    compo_setid(pic_click, tbl_disturd_set_pic_bg.btn_ok.id1_click);

    //建立 :
    compo_picturebox_t *bg_colon = compo_picturebox_create(frm, tbl_disturd_set_pic_bg.bg_colon.res);
    compo_picturebox_set_pos(bg_colon, tbl_disturd_set_pic_bg.bg_colon.x + func_disturd_get_ofs_x(0), tbl_disturd_set_pic_bg.bg_colon.y);
    compo_setid(bg_colon, tbl_disturd_set_pic_bg.bg_colon.id);

    //建立确定按钮文本
    compo_textbox_t* btn_str = compo_textbox_create(frm, strlen(i18n[tbl_disturd_set_pic_bg.btn_ok.str_id]));
    compo_textbox_set_location(btn_str, tbl_disturd_set_pic_bg.btn_ok.x, tbl_disturd_set_pic_bg.btn_ok.y,
                               tbl_disturd_set_pic_bg.btn_ok.font_w, tbl_disturd_set_pic_bg.btn_ok.font_h);
    compo_textbox_set(btn_str, i18n[tbl_disturd_set_pic_bg.btn_ok.str_id]);

    return frm;
}

static void disturd_set_num_pos_cal(s32 dy, u8 id, bool press)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*)func_cb.f_cb;

    area_t size = f_disturd_set->size;
    u16 num_height = f_disturd_set->num_height;

    static s32 last_dy = 0;
    s8 dir = 0;
    s32 distance = 0;

    disturd_set_num_item_t *time_item;
    u8 ofs = 0;

    if (id == COMPO_ID_BTN_ACLOCK_HOUR)
    {
        ofs = 0;
        time_item = &f_disturd_set->set_num_item[0];
    }
    else if (id == COMPO_ID_BTN_ACLOCK_MIN)
    {
        ofs = 5;
        time_item = &f_disturd_set->set_num_item[5];
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
//            if ((time_item[i].y - (DISTURD_NUM_PIC_CENTER_Y - size.hei/2)) <= -num_height)
            if ((time_item[i].y) <= -num_height)
            {
                dir = -1;
                break;
            }
//            else if (press == false && ((time_item[i].y - (DISTURD_NUM_PIC_CENTER_Y - size.hei/2)) <= -num_height/2))
            else if (press == false && ((time_item[i].y) <= -num_height/2))
            {
                dir = -1;
                break;
            }
        }
        else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN+ofs)
        {
//            if (time_item[i].y >= (DISTURD_NUM_PIC_CENTER_Y + size.hei/2 + num_height))
            if (time_item[i].y >= (size.hei + num_height))
            {
                dir = 1;
                break;
            }
//            else if (press == false && (time_item[i].y >= (DISTURD_NUM_PIC_CENTER_Y + size.hei/2 + num_height/2)))
            else if (press == false && (time_item[i].y >= (size.hei + num_height/2)))
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
            f_disturd_set->hcnt += dir;
        }
        else if (id == COMPO_ID_BTN_ACLOCK_MIN)
        {
            f_disturd_set->mcnt += dir;
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
                time_item[i].y = DISTURD_SET_NUM_POS_Y(DISTURD_SET_NUM_POS_Y_START,DISTURD_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR_UU-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DD+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SET_NUM_POS_Y(DISTURD_SET_NUM_POS_Y_START,DISTURD_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR_DD-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SET_NUM_POS_Y(DISTURD_SET_NUM_POS_Y_START,DISTURD_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR_UP-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SET_NUM_POS_Y(DISTURD_SET_NUM_POS_Y_START,DISTURD_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 255;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN+ofs)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SET_NUM_POS_Y(DISTURD_SET_NUM_POS_Y_START,DISTURD_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN-COMPO_ID_NUM_ACLOCK_SET_HOUR_UU);
                time_item[i].alpha = 100;
            }
        }
    }

    for (int i=0; i<5; i++)
    {
        compo_textbox_t *txt_num = compo_getobj_byid(time_item[i].num_id);
        if (time_item[i].alpha == 255)
        {
            compo_textbox_set_font(txt_num, DISTURD_FOCUS_ITEM_FONT);
        }
        else
        {
            compo_textbox_set_font(txt_num, DISTURD_ITEM_FONT);
        }
        compo_textbox_set_align_center(txt_num, true);
        if (id == COMPO_ID_BTN_ACLOCK_HOUR)
        {
            compo_textbox_set_pos(txt_num, time_item[i].x + func_disturd_get_ofs_x(1), time_item[i].y);
        }
        else if (id == COMPO_ID_BTN_ACLOCK_MIN)
        {
            compo_textbox_set_pos(txt_num, time_item[i].x + func_disturd_get_ofs_x(2), time_item[i].y);
        }
        compo_textbox_set_visible(txt_num, time_item[i].visible_en);
        compo_textbox_set_alpha(txt_num, time_item[i].alpha);
    }

    last_dy = dy;

//    printf("===============================\n");
//    for(int i=0; i<5; i++) {
//        printf("[%d] id:[%d], vis:[%d], xy:[%d,%d], ret:[%d], dir[%d], press:[%d], hcnt:[%d], mcnt[%d]\n", time_item[i].num_id, time_item[i].load_id,
//                time_item[i].visible_en, time_item[i].x, time_item[i].y, 0, dir, press, f_disturd_set->hcnt, f_disturd_set->mcnt);
//    }
//    printf("===============================\n");
}

static void disturd_ampm_set_num_pos_cal(s32 dy, bool press)
{
    f_disturd_sub_set_t *f_set = (f_disturd_sub_set_t*)func_cb.f_cb;

    //area_t size = f_set->size;
    area_t size = f_set->size;
    u16 size_hei = size.hei;//DISTURD_SCALE_TXT_ITEM_Y_OFFSET*3;
    u16 num_height = f_set->num_height;

    static s32 last_dy = 0;
    s8 dir = 0;
    s32 distance = 0;

    disturd_set_ampm_item_t *time_item;

    time_item = &f_set->set_ampm_item[0];

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
        if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_UP)
        {
            if ((time_item[i].y /*- (BREATHE_NUM_PIC_CENTER_Y - size.hei/2)*/) <= -num_height)
            {
                dir = -1;
                break;
            }
            else if (press == false && ((time_item[i].y /*- (BREATHE_NUM_PIC_CENTER_Y - size.hei/2)*/) <= -num_height/2))
            {
                dir = -1;
                break;
            }
        }
        else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_DOWN)
        {
            if (time_item[i].y >= (/*BREATHE_NUM_PIC_CENTER_Y + size.hei/2*/size_hei + num_height))
            {
                dir = 1;
                break;
            }
            else if (press == false && (time_item[i].y >= (/*BREATHE_NUM_PIC_CENTER_Y + size.hei/2*/size_hei + num_height/2)))
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
            if (time_item[i].load_id < COMPO_ID_NUM_ACLOCK_SET_SCALE_UU)
            {
                time_item[i].load_id = COMPO_ID_NUM_ACLOCK_SET_SCALE_DD;
            }
            else if (time_item[i].load_id > COMPO_ID_NUM_ACLOCK_SET_SCALE_DD)
            {
                time_item[i].load_id = COMPO_ID_NUM_ACLOCK_SET_SCALE_UU;
            }
        }

        f_set->am_pm_cnt += dir;
    }

    if (press == false || dir)
    {
        for(int i=0; i<5; i++)
        {
            if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_UU)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SCALE_CLOCK_SET_NUM_POS_Y(DISTURD_SCALE_CLOCK_SET_NUM_POS_Y_START,DISTURD_SCALE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_SCALE_UU-COMPO_ID_NUM_ACLOCK_SET_SCALE_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_DD)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SCALE_CLOCK_SET_NUM_POS_Y(DISTURD_SCALE_CLOCK_SET_NUM_POS_Y_START,DISTURD_SCALE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_SCALE_DD-COMPO_ID_NUM_ACLOCK_SET_SCALE_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_UP)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SCALE_CLOCK_SET_NUM_POS_Y(DISTURD_SCALE_CLOCK_SET_NUM_POS_Y_START,DISTURD_SCALE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_SCALE_UP-COMPO_ID_NUM_ACLOCK_SET_SCALE_UU);
                time_item[i].alpha = 100;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_CENTER)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SCALE_CLOCK_SET_NUM_POS_Y(DISTURD_SCALE_CLOCK_SET_NUM_POS_Y_START,DISTURD_SCALE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_SCALE_CENTER-COMPO_ID_NUM_ACLOCK_SET_SCALE_UU);
                time_item[i].alpha = 255;
            }
            else if (time_item[i].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_DOWN)
            {
                time_item[i].visible_en = true;
                time_item[i].num_cnt = 2;
                time_item[i].y = DISTURD_SCALE_CLOCK_SET_NUM_POS_Y(DISTURD_SCALE_CLOCK_SET_NUM_POS_Y_START,DISTURD_SCALE_TXT_ITEM_Y_OFFSET,COMPO_ID_NUM_ACLOCK_SET_SCALE_DOWN-COMPO_ID_NUM_ACLOCK_SET_SCALE_UU);
                time_item[i].alpha = 100;
            }
        }
    }

    for (int i=0; i<5; i++)
    {
        compo_textbox_t *txt_num = compo_getobj_byid(time_item[i].num_id);

        //if (time_item[i].load_id == COMPO_ID_NUM_CENTER) {
        //    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
        //} else {
        //    compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_BIN);
        //}

        compo_textbox_set_align_center(txt_num, true);
        compo_textbox_set_visible(txt_num, time_item[i].visible_en);
        compo_textbox_set_alpha(txt_num, time_item[i].alpha);
        compo_textbox_set_pos(txt_num, time_item[i].x, time_item[i].y);


    }

    last_dy = dy;

    //printf("===============================\n");
    //for(int i=0; i<5; i++) {
    //    printf("[%d] id:[%d], vis:[%d], xy:[%d,%d], ret:[%d], dir[%d], press:[%d], hcnt:[%d], am_pm_cnt[%d]\n", time_item[i].num_id, time_item[i].load_id,
    //            time_item[i].visible_en, time_item[i].x, time_item[i].y, ret, dir, press, f_set->hcnt, f_set->am_pm_cnt);
    //}
    //printf("===============================\n");
}


static void func_disturd_set_move_handle(u8 id)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*)func_cb.f_cb;
    disturd_set_num_item_t *time_item;
    bool flag_press = 0;
//    u32 tick = tick_get();
    s32 dx = 0, dy = 0;     //坐标差量
    char buf[4];

//    static u8 am_pm_last = 0;

    compo_picturebox_t *bg_hour = compo_getobj_byid(COMPO_ID_PIC_BG_HOUR);
    compo_picturebox_t *bg_min = compo_getobj_byid(COMPO_ID_PIC_BG_MIN);
    compo_picturebox_t *bg_am_pm = compo_getobj_byid(COMPO_ID_PIC_BG_AM_PM);
    if (id == COMPO_ID_BTN_ACLOCK_HOUR)
    {
        if(uteModuleSystemtime12HOn())
        {
            compo_picturebox_set(bg_hour, tbl_disturd_set_pic_bg.bg_hour.res_12_bright);
            compo_picturebox_set(bg_min, tbl_disturd_set_pic_bg.bg_min.res_12_destory);
            compo_picturebox_set(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.res_12_destory);
        }
        else
        {
            compo_picturebox_set(bg_hour, tbl_disturd_set_pic_bg.bg_hour.res_24_bright);
            compo_picturebox_set(bg_min, tbl_disturd_set_pic_bg.bg_min.res_24_destory);
            compo_picturebox_set(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.res_24_destory);
        }
        time_item = &f_disturd_set->set_num_item[0];
    }
    else if (id == COMPO_ID_BTN_ACLOCK_MIN)
    {
        if(uteModuleSystemtime12HOn())
        {
            compo_picturebox_set(bg_hour, tbl_disturd_set_pic_bg.bg_hour.res_12_destory);
            compo_picturebox_set(bg_min, tbl_disturd_set_pic_bg.bg_min.res_12_bright);
            compo_picturebox_set(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.res_12_destory);
        }
        else
        {
            compo_picturebox_set(bg_hour, tbl_disturd_set_pic_bg.bg_hour.res_24_destory);
            compo_picturebox_set(bg_min, tbl_disturd_set_pic_bg.bg_min.res_24_bright);
            compo_picturebox_set(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.res_24_destory);
        }
        time_item = &f_disturd_set->set_num_item[5];
    }

    u8 hour, min;

    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec / 3600;
        min = (sys_cb.disturd_end_time_sec % 3600) / 60;
    }
    else
    {
        hour = sys_cb.disturd_start_time_sec / 3600;
        min = (sys_cb.disturd_start_time_sec % 3600) / 60;
    }

    s8 hour_disp = hour, min_disp = min;
    for (;;)
    {
        flag_press = ctp_get_dxy(&dx, &dy);
        //printf("x:%d, y:%d\n",dx, dy);

        disturd_set_num_pos_cal(dy, id, flag_press);
        hour_disp = (hour - f_disturd_set->hcnt) % 24;
        min_disp = (min - f_disturd_set->mcnt) % 60;

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
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(func_disturd_get_time_cal(hour_disp, 1, 1), 1, 1)).hour);
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_UP)     //上次的时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(hour_disp, 1, 1)).hour);
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR)      //当前时间
                    {
//                        compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(hour_disp, 1, 0)).hour);
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DOWN)     //下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(hour_disp, 1, 2)).hour);
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_HOUR_DD)       //下下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_convert_to_12hour(func_disturd_get_time_cal(func_disturd_get_time_cal(hour_disp, 1, 2), 1, 2)).hour);
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
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(min_disp, 0, 1), 0, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_UP)     //上次的时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(min_disp, 0, 1));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN)      //当前时间
                    {
//                        compo_textbox_set_font(txt_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(min_disp, 0, 0));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DOWN)     //下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(min_disp, 0, 2));
                    }
                    else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_MIN_DD)       //下下次时间
                    {
                        snprintf(buf, sizeof(buf), "%02d", func_disturd_get_time_cal(func_disturd_get_time_cal(min_disp, 0, 2), 0, 2));
                    }
                    compo_textbox_set(txt_num, buf);
                }
            }
        }

        if (!flag_press)
        {
            f_disturd_set->hcnt = 0;
            f_disturd_set->mcnt = 0;
            hour = func_disturd_get_time_cal(hour_disp, 1, 0);
            min = func_disturd_get_time_cal(min_disp, 0, 0);

            if (f_disturd_set->time_scale)
            {
                hour = func_disturd_convert_to_12hour(func_disturd_get_time_cal(hour_disp, 1, 0)).hour;

                if (f_disturd_set->time_am_pm == 1)   //PM
                {
                    if (hour != 12)
                    {
                        hour += 12;
                    }
                }
                else if (f_disturd_set->time_am_pm == 0 && hour == 12)        //AM
                {
                    hour = 0;
                }
            }


            if (sys_cb.disturd_sel)
            {
                sys_cb.disturd_end_time_sec = hour * 3600 + min * 60;
                if(uteModuleNotDisturbGetTimeStatus() != NOT_DISTURB_END_TIME)
                {
                    uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                }
                uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
            }
            else
            {
                sys_cb.disturd_start_time_sec = hour * 3600 + min * 60;
                if(uteModuleNotDisturbGetTimeStatus() != NOT_DISTURB_START_TIME)
                {
                    uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                }
                uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
            }
            printf("total_sec:%d, hour:%d, min:%d\n",hour * 3600 + min * 60, hour, min);
            break;
        }
        evt_message(msg_dequeue());
        func_process();
    }

}

static u8 func_disturd_sub_set_ampm_move_handle(u8 am_pm)
{
    f_disturd_sub_set_t *f_set = (f_disturd_sub_set_t*)func_cb.f_cb;
    disturd_set_ampm_item_t *time_item;
    bool flag_press = 0;
    s32 dx = 0, dy = 0;     //坐标差量
    char buf[10];
    time_item = &f_set->set_ampm_item[0];

    u8 ampm = am_pm;
    s8 ampm_disp = ampm;

    compo_picturebox_t *bg_hour = compo_getobj_byid(COMPO_ID_PIC_BG_HOUR);
    compo_picturebox_t *bg_min = compo_getobj_byid(COMPO_ID_PIC_BG_MIN);
    compo_picturebox_t *bg_am_pm = compo_getobj_byid(COMPO_ID_PIC_BG_AM_PM);

    if(uteModuleSystemtime12HOn())
    {
        compo_picturebox_set(bg_hour, tbl_disturd_set_pic_bg.bg_hour.res_12_destory);
        compo_picturebox_set(bg_min, tbl_disturd_set_pic_bg.bg_min.res_12_destory);
        compo_picturebox_set(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.res_12_bright);
    }
    else
    {
        compo_picturebox_set(bg_hour, tbl_disturd_set_pic_bg.bg_hour.res_24_destory);
        compo_picturebox_set(bg_min, tbl_disturd_set_pic_bg.bg_min.res_24_destory);
        compo_picturebox_set(bg_am_pm, tbl_disturd_set_pic_bg.bg_am_pm.res_24_bright);
    }

//
    printf("%s1\n", __func__);
    for (;;)
    {
        flag_press = ctp_get_dxy(&dx, &dy);
        //printf("x:%d, y:%d\n",dx, dy);

        disturd_ampm_set_num_pos_cal(dy, flag_press);
        ampm_disp = (ampm - f_set->am_pm_cnt) % ACLOCK_STR_AMPM_TBL_CNT;

        for (int idx = 0; idx < 5; idx++)
        {
            memset(buf, '\0', sizeof(buf));
            compo_textbox_t *txt_num = compo_getobj_byid(time_item[idx].num_id);
            if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_UU)        //上上次的时间
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(func_disturd_ampm_get_str_cal(ampm_disp, 1), 1)].str_id]);
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_UP)     //上次的时间
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(ampm_disp, 1)].str_id]);
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_CENTER)      //当前时间
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(ampm_disp, 0)].str_id]);
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_DOWN)     //下次时间
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(ampm_disp, 2)].str_id]);
            }
            else if (time_item[idx].load_id == COMPO_ID_NUM_ACLOCK_SET_SCALE_DD)       //下下次时间
            {
                snprintf(buf, sizeof(buf), "%s", i18n[aclock_str_ampm_tbl[func_disturd_ampm_get_str_cal(func_disturd_ampm_get_str_cal(ampm_disp, 2), 2)].str_id]);
            }
            compo_textbox_set(txt_num, buf);
        }


        if (!flag_press)
        {
            f_set->am_pm_cnt = 0;
            ampm = func_disturd_ampm_get_str_cal(ampm_disp, 0);
            f_set->time_am_pm = aclock_str_ampm_tbl[ampm].res;

            u8 hour, min;
            if(sys_cb.disturd_sel)
            {
                hour = sys_cb.disturd_end_time_sec / 3600;
                min = (sys_cb.disturd_end_time_sec % 3600) / 60;
            }
            else
            {
                hour = sys_cb.disturd_start_time_sec / 3600;
                min = (sys_cb.disturd_start_time_sec % 3600) / 60;
            }

            if (f_set->time_scale)
            {
                hour = func_disturd_convert_to_12hour(hour).hour;

                if (f_set->time_am_pm == 1)   //PM
                {
                    if (hour != 12)
                    {
                        hour += 12;
                    }
                }
                else if (f_set->time_am_pm == 0 && hour == 12)        //AM
                {
                    hour = 0;
                }
            }

//            sys_cb.disturd_edit_hour = hour;

            if (sys_cb.disturd_sel)
            {
                sys_cb.disturd_end_time_sec = hour * 3600 + min * 60;
                if(uteModuleNotDisturbGetTimeStatus() != NOT_DISTURB_END_TIME)
                {
                    uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                }
                uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
            }
            else
            {
                sys_cb.disturd_start_time_sec = hour * 3600 + min * 60;
                if(uteModuleNotDisturbGetTimeStatus() != NOT_DISTURB_START_TIME)
                {
                    uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                }
                uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
            }

            printf(">>>ampm:%d\n", aclock_str_ampm_tbl[ampm].res);
            printf("total_sec:%d, hour:%d, min:%d\n",hour * 3600 + min * 60, hour, min);

            break;
        }
        evt_message(msg_dequeue());
        func_process();
    }

    func_process();

    return ampm;

}

//触摸按钮效果处理
static void func_disturd_sub_set_button_touch_handle(void)
{

//    static u8 am_pm_state = 0;  //0:AM 1:PM
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_CONTINUE:
            compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_CONTINUE_CLICK), true);
            break;

//        case COMPO_ID_BTN_REPETAT_NO:
//            compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_REPETAT_NO_CLICK), true);
//            break;

//        case COMPO_ID_BTN_REPETAT_YES:
//            compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_REPETAT_YES_CLICK), true);
//            break;

        case COMPO_ID_BTN_ACLOCK_HOUR:
        case COMPO_ID_BTN_ACLOCK_MIN:
            printf("COMPO_ID_BTN_ACLOCK_MIN\n");
            func_disturd_set_move_handle(id);
            break;

        case COMPO_ID_BTN_ACLOCK_AM_PM:
            printf("COMPO_ID_BTN_ACLOCK_AM_PM\n");
            f_disturd_set->time_am_pm = func_disturd_sub_set_ampm_move_handle(f_disturd_set->time_am_pm);
            break;

        default:
            break;
    }
}


//释放按钮效果处理
static void func_disturd_sub_set_button_release_handle(void)
{
        compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_CONTINUE_CLICK), false);
//        compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_REPETAT_NO_CLICK), false);
//        compo_picturebox_set_visible(compo_getobj_byid(COMPO_ID_PIC_REPETAT_YES_CLICK), false);
}


//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_CONTINUE:
            if (func_cb.last == FUNC_SET_SUB_DISTURD)
            {
                func_cb.sta = FUNC_SET_SUB_DISTURD;
                task_stack_pop();
            }

            break;

//        case COMPO_ID_BTN_NO_CLICK:
//            if (func_cb.last == FUNC_SET_SUB_DISTURD)
//            {
//                func_cb.sta = FUNC_SET_SUB_DISTURD;
//                task_stack_pop();
//            }
//
//            break;

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

static void func_disturd_sub_set_enter(void);
static void func_disturd_sub_set_exit(void);
//定时勿扰设置功能消息处理
static void func_disturd_sub_set_message(size_msg_t msg)
{

    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
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
            func_disturd_sub_set_button_release_handle();
            func_message(msg);
            break;

        case MSG_QDEC_BACKWARD:
        case MSG_QDEC_FORWARD:
            break;

        case MSG_SYS_500MS:
            if (f_disturd_set->time_scale != uteModuleSystemtime12HOn())
            {
                if (func_cb.frm_main != NULL)
                {
                    compo_form_destroy(func_cb.frm_main);
                }
                func_disturd_sub_set_exit();
                if (func_cb.f_cb != NULL)
                {
                    func_free(func_cb.f_cb);
                    func_cb.f_cb = NULL;
                }
                func_disturd_sub_set_enter();
            }
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

    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    memcpy(f_disturd_set->set_num_item, tbl_disturd_set_num_item, sizeof(tbl_disturd_set_num_item));
    memcpy(f_disturd_set->set_ampm_item, tbl_disturd_set_ampm_item, sizeof(tbl_disturd_set_ampm_item));
    f_disturd_set->size = gui_image_get_size(tbl_disturd_set_pic_bg.bg_hour.res_24_destory);
    f_disturd_set->num_height = DISTURD_FONT_HEIGHT;
    f_disturd_set->time_scale = uteModuleSystemtime12HOn();
    func_cb.enter_tick = tick_get();
}

//退出定时勿扰设置功能
static void func_disturd_sub_set_exit(void)
{
    printf("%s\n",__func__);
    func_cb.last = FUNC_DISTURD_SUB_SET;
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
