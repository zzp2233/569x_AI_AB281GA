#include "include.h"
#include "func.h"
#include "ute_drv_battery_common.h"
#include "ute_drv_motor.h"
#include "ute_module_systemtime.h"
#include "ute_application_common.h"
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
#include "ute_module_bedside_mode.h"
#endif

#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_charge_t_
{
    u8 percent_bkp;
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    u8 last_orientation;
#endif
    u8 light_level;                                 //亮度等级
} f_charge_t;
#define BAT_PERCENT_VALUE       uteDrvBatteryCommonGetLvl()  //电量百分比数值
#define BAT_PERCENT_FLAG        uteDrvBatteryCommonGetChargerStatus()   //充电状态0：未充电，1：充电中，2：充电完成
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    //数字
    COMPO_ID_NUM_TIME_HOUR = 1,
    COMPO_ID_NUM_TIME_MIN,
    COMPO_ID_PIC_TIME_COLON,

    COMPO_ID_NUM_BAT_VALUE,
    COMPO_ID_PIC_BAT_PERCENT,

    COMPO_ID_RECT_BAT_1,
    COMPO_ID_RECT_BAT_2,
    COMPO_ID_RECT_BAT_3,
    COMPO_ID_RECT_BAT_4,
    COMPO_ID_RECT_BAT_5,
};

typedef struct charge_ui_handle_t_
{
    struct ui_time_hour
    {
        u16 id_hour;
        s16 x_hour, y_hour;
        u16 w_hour, h_hour;
        u8 hour;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_hour;

    struct ui_time_min
    {
        u16 id_min;
        s16 x_min, y_min;
        u16 w_min, h_min;
        u8 min;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_min;

    struct ui_time_colon
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;
        u32 destroy_res;
        u8 rev;
    } time_colon;

    struct ui_bat_value
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } bat_value;

    struct ui_bat_percent
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;
        u8 rev;
    } bat_percent;

    struct ui_bat_rect
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;       //res不为0使用图库，为0使用组件实现
        u32 destroy_res;
        color_t bright_color;
        color_t destroy_color;
        u16 rev;              //矩形圆角
    } bat_rect[5];

} charge_ui_handle_t;

static const charge_ui_handle_t ui_handle =
{

    .time_hour = {
        .id_hour = COMPO_ID_NUM_TIME_HOUR,
        .x_hour = 122+78/2,
        .y_hour = 35+94/2,
        .w_hour = 0,
        .h_hour = 0,
        .hour   = 0,
        .res    = UI_BUF_I330001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_min = {
        .id_min = COMPO_ID_NUM_TIME_MIN,
        .x_min  = 122+78/2,
        .y_min  = 153+94/2,
        .w_min  = 0,
        .h_min  = 0,
        .min    = 0,
        .res    = UI_BUF_I330001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_colon = {
        .id     = COMPO_ID_PIC_TIME_COLON,
        .x      = 122+78/2,
        .y      = 128+25/2,
        .w      = 0,
        .h      = 0,
        .bright_res     = UI_BUF_I330001_CHARGE_COLON2_BIN,
        .destroy_res    = UI_BUF_I330001_CHARGE_COLON1_BIN,
        .rev    = 0,
    },

    .bat_value = {
        .id     = COMPO_ID_NUM_BAT_VALUE,
        .x      = 37+60/2,
        .y      = 30+87/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I330001_CHARGE_POWER_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 1,  //横向，竖向排列
        .num_align  = 0,   //对其方式
        .flag_zfill = false,
        .margin     = -5,
        .rev    = 3,
    },

    .bat_percent = {
        .id     = COMPO_ID_PIC_BAT_PERCENT,
        .x      = 37+60/2,
        .y      = 30+87+48/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I330001_CHARGE_PERCENT_BIN,
        .rev    = 0,
    },

    .bat_rect = {
        [0]     = {COMPO_ID_RECT_BAT_1,     37+60/2,    241+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [1]     = {COMPO_ID_RECT_BAT_2,     37+60/2,    223+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [2]     = {COMPO_ID_RECT_BAT_3,     37+60/2,    205+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [3]     = {COMPO_ID_RECT_BAT_4,     37+60/2,    187+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [4]     = {COMPO_ID_RECT_BAT_5,     37+60/2,    169+14/2,   60,     14,     0,  0,  {0,255,225}, {0,76,68}, 14/2},
    },


};

//电量更新
void func_charge_update(void)
{

    //更新bat Value
    compo_number_t* bat_value = compo_getobj_byid(ui_handle.bat_value.id);
    compo_number_set(bat_value, BAT_PERCENT_VALUE);

    //更新bat rect
    u8 bright_value = (sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])) * BAT_PERCENT_VALUE / 100;      //等级

    //熄灭的
    for(int i=0; i<(sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])); i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].destroy_color.r, ui_handle.bat_rect[i].destroy_color.g, ui_handle.bat_rect[i].destroy_color.b));
    }

    //点亮
    for(int i=0; i<bright_value; i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
    }

    //更新 :
    static u32 tick = 0;
    static bool bright_sta = 0;
    compo_picturebox_t* colon = compo_getobj_byid(ui_handle.time_colon.id);
    if (tick_check_expire(tick, 1000))
    {
        tick = tick_get();
        if (bright_sta)
        {
            compo_picturebox_set(colon, ui_handle.time_colon.bright_res);
        }
        else
        {
            compo_picturebox_set(colon, ui_handle.time_colon.destroy_res);
        }
        bright_sta = !bright_sta;
    }

}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建time hour
    compo_number_t* hour = compo_number_create(frm, ui_handle.time_hour.res, ui_handle.time_hour.rev);
    compo_number_set_align(hour, ui_handle.time_hour.num_align);
    compo_number_set_layout(hour, ui_handle.time_hour.num_layout);
    compo_number_set_zfill(hour, ui_handle.time_hour.flag_zfill);
    compo_number_set_margin(hour, ui_handle.time_hour.margin);
    compo_number_set_pos(hour, ui_handle.time_hour.x_hour, ui_handle.time_hour.y_hour);
    compo_number_set(hour, ui_handle.time_hour.hour);
    compo_setid(hour, ui_handle.time_hour.id_hour);
    compo_bonddata(hour, COMPO_BOND_HOUR);

    //创建timer min
    compo_number_t* min = compo_number_create(frm, ui_handle.time_min.res, ui_handle.time_min.rev);
    compo_number_set_align(min, ui_handle.time_min.num_align);
    compo_number_set_layout(min, ui_handle.time_min.num_layout);
    compo_number_set_zfill(min, ui_handle.time_min.flag_zfill);
    compo_number_set_margin(min, ui_handle.time_min.margin);
    compo_number_set_pos(min, ui_handle.time_min.x_min, ui_handle.time_min.y_min);
    compo_number_set(min, ui_handle.time_min.min);
    compo_setid(min, ui_handle.time_min.id_min);
    compo_bonddata(min, COMPO_BOND_MINUTE);

    //创建 :
    compo_picturebox_t* colon = compo_picturebox_create(frm, ui_handle.time_colon.bright_res);
    compo_picturebox_set_pos(colon, ui_handle.time_colon.x, ui_handle.time_colon.y);
    compo_setid(colon, ui_handle.time_colon.id);

    //创建BAT Vaule
    compo_number_t* bat_value = compo_number_create(frm, ui_handle.bat_value.res, ui_handle.bat_value.rev);
    compo_number_set_align(bat_value, ui_handle.bat_value.num_align);
    compo_number_set_layout(bat_value, ui_handle.bat_value.num_layout);
    compo_number_set_zfill(bat_value, ui_handle.bat_value.flag_zfill);
    compo_number_set_margin(bat_value, ui_handle.bat_value.margin);
    compo_number_set_pos(bat_value, ui_handle.bat_value.x, ui_handle.bat_value.y);
    compo_number_set(bat_value, 100);
    compo_setid(bat_value, ui_handle.bat_value.id);

    //创建 %
    compo_picturebox_t* bat_percent = compo_picturebox_create(frm, ui_handle.bat_percent.res);
    compo_picturebox_set_pos(bat_percent, ui_handle.bat_percent.x, ui_handle.bat_percent.y);
    compo_setid(bat_percent, ui_handle.bat_percent.id);

    //创建rect
    for (int i=0; i<sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0]); i++)
    {
        compo_shape_t* bat_rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
        compo_shape_set_location(bat_rect, ui_handle.bat_rect[i].x, ui_handle.bat_rect[i].y, ui_handle.bat_rect[i].w, ui_handle.bat_rect[i].h);
        compo_shape_set_radius(bat_rect, ui_handle.bat_rect[i].rev);
        compo_setid(bat_rect, ui_handle.bat_rect[i].id);
    }

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    //数字
    COMPO_ID_NUM_TIME_HOUR = 1,
    COMPO_ID_NUM_TIME_MIN,
    COMPO_ID_PIC_TIME_COLON,

    COMPO_ID_NUM_BAT_VALUE,
    COMPO_ID_PIC_BAT_PERCENT,

    COMPO_ID_RECT_BAT_1,
    COMPO_ID_RECT_BAT_2,
    COMPO_ID_RECT_BAT_3,
    COMPO_ID_RECT_BAT_4,
    COMPO_ID_RECT_BAT_5,
};


typedef struct charge_ui_handle_t_
{
    struct ui_time_hour
    {
        u16 id_hour;
        s16 x_hour, y_hour;
        u16 w_hour, h_hour;
        u8 hour;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_hour;

    struct ui_time_min
    {
        u16 id_min;
        s16 x_min, y_min;
        u16 w_min, h_min;
        u8 min;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_min;

    struct ui_time_colon
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;
        u32 destroy_res;
        u8 rev;
    } time_colon;

    struct ui_bat_value
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } bat_value;

    struct ui_bat_percent
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;
        u8 rev;
    } bat_percent;

    struct ui_bat_rect
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;       //res不为0使用图库，为0使用组件实现
        u32 destroy_res;
        color_t bright_color;
        color_t destroy_color;
        u16 rev;              //矩形圆角
    } bat_rect[5];

} charge_ui_handle_t;

static const charge_ui_handle_t ui_handle =
{

    .time_hour = {
        .id_hour = COMPO_ID_NUM_TIME_HOUR,
        .x_hour = GUI_SCREEN_CENTER_X-76,
        .y_hour = 84+96/2,
        .w_hour = 0,
        .h_hour = 0,
        .hour   = 0,
        .res    = UI_BUF_I332001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 0,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_min = {
        .id_min = COMPO_ID_NUM_TIME_MIN,
        .x_min  = GUI_SCREEN_CENTER_X+76,
        .y_min  = 84+96/2,
        .w_min  = 0,
        .h_min  = 0,
        .min    = 0,
        .res    = UI_BUF_I332001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 0,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_colon = {
        .id     = COMPO_ID_PIC_TIME_COLON,
        .x      = GUI_SCREEN_CENTER_X,
        .y      = 84+96/2,
        .w      = 0,
        .h      = 0,
        .bright_res     = UI_BUF_I332001_CHARGE_COLON2_BIN,
        .destroy_res    = UI_BUF_I332001_CHARGE_COLON_BIN,
        .rev    = 0,
    },

    .bat_value = {
        .id     = COMPO_ID_NUM_BAT_VALUE,
        .x      = 83,
        .y      = 231+60/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I332001_CHARGE_POWER_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 0,  //横向，竖向排列
        .num_align  = 0,   //对其方式
        .flag_zfill = false,
        .margin     = -5,
        .rev    = 3,
    },

    .bat_percent = {
        .id     = COMPO_ID_PIC_BAT_PERCENT,
        .x      = 83+74,
        .y      = 231+60/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I332001_CHARGE_PERCENT_BIN,
        .rev    = 0,
    },

    .bat_rect = {
        [0]     = {COMPO_ID_RECT_BAT_1,         108+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [1]     = {COMPO_ID_RECT_BAT_2,         126+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [2]     = {COMPO_ID_RECT_BAT_3,         144+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [3]     = {COMPO_ID_RECT_BAT_4,         162+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [4]     = {COMPO_ID_RECT_BAT_5,         180+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
    },


};

//电量更新
void func_charge_update(void)
{

    //更新bat Value
    compo_number_t* bat_value = compo_getobj_byid(ui_handle.bat_value.id);
    compo_number_set(bat_value, BAT_PERCENT_VALUE);

    //更新bat rect
    u8 bright_value = (sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])) * BAT_PERCENT_VALUE / 100;      //等级

    //熄灭的
    for(int i=0; i<(sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])); i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].destroy_color.r, ui_handle.bat_rect[i].destroy_color.g, ui_handle.bat_rect[i].destroy_color.b));
    }

    //点亮
    for(int i=0; i<bright_value; i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
    }

    //更新 :
    static u32 tick = 0;
    static bool bright_sta = 0;
    compo_picturebox_t* colon = compo_getobj_byid(ui_handle.time_colon.id);
    if (tick_check_expire(tick, 1000))
    {
        tick = tick_get();
        if (bright_sta)
        {
            compo_picturebox_set(colon, ui_handle.time_colon.bright_res);
        }
        else
        {
            compo_picturebox_set(colon, ui_handle.time_colon.destroy_res);
        }
        bright_sta = !bright_sta;
    }

}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建time hour
    compo_number_t* hour = compo_number_create(frm, ui_handle.time_hour.res, ui_handle.time_hour.rev);
    compo_number_set_align(hour, ui_handle.time_hour.num_align);
    compo_number_set_layout(hour, ui_handle.time_hour.num_layout);
    compo_number_set_zfill(hour, ui_handle.time_hour.flag_zfill);
    compo_number_set_margin(hour, ui_handle.time_hour.margin);
    compo_number_set_pos(hour, ui_handle.time_hour.x_hour, ui_handle.time_hour.y_hour);
    compo_number_set(hour, ui_handle.time_hour.hour);
    compo_setid(hour, ui_handle.time_hour.id_hour);
    compo_bonddata(hour, COMPO_BOND_HOUR);

    //创建timer min
    compo_number_t* min = compo_number_create(frm, ui_handle.time_min.res, ui_handle.time_min.rev);
    compo_number_set_align(min, ui_handle.time_min.num_align);
    compo_number_set_layout(min, ui_handle.time_min.num_layout);
    compo_number_set_zfill(min, ui_handle.time_min.flag_zfill);
    compo_number_set_margin(min, ui_handle.time_min.margin);
    compo_number_set_pos(min, ui_handle.time_min.x_min, ui_handle.time_min.y_min);
    compo_number_set(min, ui_handle.time_min.min);
    compo_setid(min, ui_handle.time_min.id_min);
    compo_bonddata(min, COMPO_BOND_MINUTE);

    //创建 :
    compo_picturebox_t* colon = compo_picturebox_create(frm, ui_handle.time_colon.bright_res);
    compo_picturebox_set_pos(colon, ui_handle.time_colon.x, ui_handle.time_colon.y);
    compo_setid(colon, ui_handle.time_colon.id);

    //创建BAT Vaule
    compo_number_t* bat_value = compo_number_create(frm, ui_handle.bat_value.res, ui_handle.bat_value.rev);
    compo_number_set_align(bat_value, ui_handle.bat_value.num_align);
    compo_number_set_layout(bat_value, ui_handle.bat_value.num_layout);
    compo_number_set_zfill(bat_value, ui_handle.bat_value.flag_zfill);
    compo_number_set_margin(bat_value, ui_handle.bat_value.margin);
    compo_number_set_pos(bat_value, ui_handle.bat_value.x, ui_handle.bat_value.y);
    compo_number_set(bat_value, 100);
    compo_setid(bat_value, ui_handle.bat_value.id);

    //创建 %
    compo_picturebox_t* bat_percent = compo_picturebox_create(frm, ui_handle.bat_percent.res);
    compo_picturebox_set_pos(bat_percent, ui_handle.bat_percent.x, ui_handle.bat_percent.y);
    compo_setid(bat_percent, ui_handle.bat_percent.id);

    //创建rect
    for (int i=0; i<sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0]); i++)
    {
        compo_shape_t* bat_rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
        compo_shape_set_location(bat_rect, ui_handle.bat_rect[i].x, ui_handle.bat_rect[i].y, ui_handle.bat_rect[i].w, ui_handle.bat_rect[i].h);
        compo_shape_set_radius(bat_rect, ui_handle.bat_rect[i].rev);
        compo_setid(bat_rect, ui_handle.bat_rect[i].id);
    }

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I269032_SUPPORT
enum
{
    //数字
    COMPO_ID_NUM_TIME_HOUR_S = 1,
    COMPO_ID_NUM_TIME_HOUR_G,
    COMPO_ID_NUM_TIME_MIN_S,
    COMPO_ID_NUM_CLOLON,
    COMPO_ID_NUM_TIME_MIN_G,
    COMPO_ID_PIC_TIME_COLON,

    COMPO_ID_NUM_BAT_VALUE,
    COMPO_ID_NUM_TIME_WEEK,
    COMPO_ID_NUM_DATE_S,
    COMPO_ID_NUM_DATE_G,
    COMPO_ID_PIC_BAT_PERCENT,
    COMPO_ID_BAT_PIC,
};

typedef struct charge_ui_handle_t_
{
    u16 id;
    s16 pic_x;
    s16 pic_y;

} charge_ui_handle_t;

static const charge_ui_handle_t ui_handle_1 [] = //正面
{
    {.pic_x=35,    .pic_y=160, .id=COMPO_ID_NUM_TIME_HOUR_S},//小时 十位
    {.pic_x=35+46, .pic_y=160, .id=COMPO_ID_NUM_TIME_HOUR_G},//小时 个位
    {.pic_x=35+46*2, .pic_y=160, .id=COMPO_ID_NUM_CLOLON},//冒号
    {.pic_x=35+46*3, .pic_y=160, .id=COMPO_ID_NUM_TIME_MIN_S},//分钟 十位
    {.pic_x=35+46*4, .pic_y=160, .id=COMPO_ID_NUM_TIME_MIN_G},//分钟 个位
    {.pic_x=155, .pic_y=230, .id=COMPO_ID_NUM_TIME_WEEK},//星期
    {.pic_x=207, .pic_y=230, .id=COMPO_ID_NUM_DATE_S},//日期 十位
    {.pic_x=225, .pic_y=230, .id=COMPO_ID_NUM_DATE_G},//日期 个位
    {.pic_x=200, .pic_y=65, .id=COMPO_ID_PIC_BAT_PERCENT},//电量
};

static const charge_ui_handle_t ui_handle_2 [] = //90反转
{
    {.pic_x=120, .pic_y=55, .id=COMPO_ID_NUM_TIME_HOUR_S},//小时 十位
    {.pic_x=120, .pic_y=55+46, .id=COMPO_ID_NUM_TIME_HOUR_G},//小时 个位
    {.pic_x=120, .pic_y=55+46*2, .id=COMPO_ID_NUM_CLOLON},//冒号
    {.pic_x=120, .pic_y=55+46*3, .id=COMPO_ID_NUM_TIME_MIN_S},//分钟 十位
    {.pic_x=120, .pic_y=55+46*4, .id=COMPO_ID_NUM_TIME_MIN_G},//分钟 个位
    {.pic_x=50,  .pic_y=180, .id=COMPO_ID_NUM_TIME_WEEK},//星期
    {.pic_x=50,  .pic_y=180+52, .id=COMPO_ID_NUM_DATE_S},//日期 十位
    {.pic_x=50,  .pic_y=180+70, .id=COMPO_ID_NUM_DATE_G},//日期 个位
    {.pic_x=195, .pic_y=240, .id=COMPO_ID_PIC_BAT_PERCENT},//电量
};

#define num_big_0    UI_BUF_I269032_CHARGE_NUM_BIG_03_BIN
#define num_big_1    UI_BUF_I269032_CHARGE_NUM_BIG_04_BIN
#define num_big_2    UI_BUF_I269032_CHARGE_NUM_BIG_05_BIN
#define num_big_3    UI_BUF_I269032_CHARGE_NUM_BIG_06_BIN
#define num_big_4    UI_BUF_I269032_CHARGE_NUM_BIG_07_BIN
#define num_big_5    UI_BUF_I269032_CHARGE_NUM_BIG_08_BIN
#define num_big_6    UI_BUF_I269032_CHARGE_NUM_BIG_09_BIN
#define num_big_7    UI_BUF_I269032_CHARGE_NUM_BIG_10_BIN
#define num_big_8    UI_BUF_I269032_CHARGE_NUM_BIG_11_BIN
#define num_big_9    UI_BUF_I269032_CHARGE_NUM_BIG_12_BIN
#define colon_big    UI_BUF_I269032_CHARGE_NUM_BIG_13_BIN

#define num_mini_0    UI_BUF_I269032_CHARGE_NUM_MINI_03_BIN
#define num_mini_1    UI_BUF_I269032_CHARGE_NUM_MINI_04_BIN
#define num_mini_2    UI_BUF_I269032_CHARGE_NUM_MINI_05_BIN
#define num_mini_3    UI_BUF_I269032_CHARGE_NUM_MINI_06_BIN
#define num_mini_4    UI_BUF_I269032_CHARGE_NUM_MINI_07_BIN
#define num_mini_5    UI_BUF_I269032_CHARGE_NUM_MINI_08_BIN
#define num_mini_6    UI_BUF_I269032_CHARGE_NUM_MINI_09_BIN
#define num_mini_7    UI_BUF_I269032_CHARGE_NUM_MINI_10_BIN
#define num_mini_8    UI_BUF_I269032_CHARGE_NUM_MINI_11_BIN
#define num_mini_9    UI_BUF_I269032_CHARGE_NUM_MINI_12_BIN
#define colon_mini    UI_BUF_I269032_CHARGE_NUM_MINI_13_BIN

#define bat_pic_0     UI_BUF_I269032_CHARGE_BAT_00_BIN
#define bat_pic_1     UI_BUF_I269032_CHARGE_BAT_01_BIN
#define bat_pic_2     UI_BUF_I269032_CHARGE_BAT_02_BIN
#define bat_pic_3     UI_BUF_I269032_CHARGE_BAT_03_BIN
#define bat_pic_4     UI_BUF_I269032_CHARGE_BAT_04_BIN

#define week_zh_1     UI_BUF_I269032_CHARGE_WEEK_01_BIN
#define week_zh_2     UI_BUF_I269032_CHARGE_WEEK_02_BIN
#define week_zh_3     UI_BUF_I269032_CHARGE_WEEK_03_BIN
#define week_zh_4     UI_BUF_I269032_CHARGE_WEEK_04_BIN
#define week_zh_5     UI_BUF_I269032_CHARGE_WEEK_05_BIN
#define week_zh_6     UI_BUF_I269032_CHARGE_WEEK_06_BIN
#define week_zh_7     UI_BUF_I269032_CHARGE_WEEK_00_BIN

#define week_en_1     UI_BUF_I269032_CHARGE_WEEK_08_BIN
#define week_en_2     UI_BUF_I269032_CHARGE_WEEK_09_BIN
#define week_en_3     UI_BUF_I269032_CHARGE_WEEK_10_BIN
#define week_en_4     UI_BUF_I269032_CHARGE_WEEK_11_BIN
#define week_en_5     UI_BUF_I269032_CHARGE_WEEK_12_BIN
#define week_en_6     UI_BUF_I269032_CHARGE_WEEK_13_BIN
#define week_en_7     UI_BUF_I269032_CHARGE_WEEK_07_BIN

const static u32 num_pic_big[10]=
{
    num_big_0,
    num_big_1,
    num_big_2,
    num_big_3,
    num_big_4,
    num_big_5,
    num_big_6,
    num_big_7,
    num_big_8,
    num_big_9,
};
const static u32 num_pic_mini[10]=
{
    num_mini_0,
    num_mini_1,
    num_mini_2,
    num_mini_3,
    num_mini_4,
    num_mini_5,
    num_mini_6,
    num_mini_7,
    num_mini_8,
    num_mini_9,
};
const static u32 bat_pic[5]=
{
    bat_pic_0,
    bat_pic_1,
    bat_pic_2,
    bat_pic_3,
    bat_pic_4,
};
const static u32 week_pic_zh[10]=
{
    week_zh_7,
    week_zh_1,
    week_zh_2,
    week_zh_3,
    week_zh_4,
    week_zh_5,
    week_zh_6,
};
const static u32 week_pic_en[10]=
{
    week_en_7,
    week_en_1,
    week_en_2,
    week_en_3,
    week_en_4,
    week_en_5,
    week_en_6,
};
void func_charge_update(void)
{
    if(uteModuleBedsideModeIsOpen())
    {
        u32 img_3d_res_addr[9]= {0};
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        if(uteModuleSystemtime12HOn())
        {
            time.hour%=12;
            if(time.hour==0)
            {
                time.hour=12;
            }
        }
        img_3d_res_addr[0] = num_pic_big[time.hour/10];
        img_3d_res_addr[1] = num_pic_big[time.hour%10];
        img_3d_res_addr[2] = colon_big;
        img_3d_res_addr[3] = num_pic_big[time.min/10];
        img_3d_res_addr[4] = num_pic_big[time.min%10];
        if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
        {
            img_3d_res_addr[5] = week_pic_zh[time.week];
        }
        else
        {
            img_3d_res_addr[5] = week_pic_en[time.week];
        }
        img_3d_res_addr[6] = num_pic_mini[time.day/10];
        img_3d_res_addr[7] = num_pic_mini[time.day%10];
        img_3d_res_addr[8] = bat_pic[uteDrvBatteryCommonGetBatteryIndex(sizeof(bat_pic)/sizeof(bat_pic[0]))];

        f_charge_t *f_charge = (f_charge_t *)func_cb.f_cb;

        u8 direction = uteModuleSportGetDeviceOrientation();
        if(direction > 0 && direction < 5)
        {
            f_charge->last_orientation = direction;
        }

        for (u8 i = 0; i < 9; i++)
        {
            compo_picturebox_t *pic = compo_getobj_byid(ui_handle_1[i].id);
            compo_picturebox_set(pic, img_3d_res_addr[i]);
            switch (f_charge->last_orientation)
            {
                case 3:
                    compo_picturebox_set_pos(pic, ui_handle_1[i].pic_x, ui_handle_1[i].pic_y); // 0°
                    compo_picturebox_set_rotation(pic, 0);
                    break;
                case 1:
                    compo_picturebox_set_pos(pic, ui_handle_2[i].pic_x, ui_handle_2[i].pic_y); // 90°
                    compo_picturebox_set_rotation(pic, 900);
                    break;
                case 4:
                    compo_picturebox_set_pos(pic, GUI_SCREEN_WIDTH - ui_handle_1[i].pic_x, GUI_SCREEN_HEIGHT - ui_handle_1[i].pic_y); // 180°
                    compo_picturebox_set_rotation(pic, 1800);
                    break;
                case 2:
                    compo_picturebox_set_pos(pic, GUI_SCREEN_WIDTH - ui_handle_2[i].pic_x, GUI_SCREEN_HEIGHT - ui_handle_2[i].pic_y); // 270°
                    compo_picturebox_set_rotation(pic, 2700);
                    break;
                default:
                    break;
            }
        }
    }
    else
    {
        compo_textbox_t *txt_val   = compo_getobj_byid(COMPO_ID_NUM_BAT_VALUE);
        compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_BAT_PIC);

        char txt_buf[30];
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
        compo_textbox_set_location(txt_val,GUI_SCREEN_CENTER_X,BAT_PERCENT_VALUE<=20 ? GUI_SCREEN_CENTER_Y-20 : GUI_SCREEN_CENTER_Y-10,150,50);
        compo_textbox_set(txt_val,txt_buf);
        compo_picturebox_set_visible(pic,BAT_PERCENT_VALUE<100);
    }
}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    if(uteModuleBedsideModeIsOpen())
    {
        //新建窗体和背景
        compo_form_t *frm = compo_form_create(true);

        u32 img_3d_res_addr[9]= {0};

        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);

        if(uteModuleSystemtime12HOn())
        {
            time.hour%=12;
            if(time.hour==0)
            {
                time.hour=12;
            }
        }

        img_3d_res_addr[0] = num_pic_big[time.hour/10];
        img_3d_res_addr[1] = num_pic_big[time.hour%10];
        img_3d_res_addr[2] = colon_big;
        img_3d_res_addr[3] = num_pic_big[time.min/10];
        img_3d_res_addr[4] = num_pic_big[time.min%10];
        if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
        {
            img_3d_res_addr[5] = week_pic_zh[time.week];
        }
        else
        {
            img_3d_res_addr[5] = week_pic_en[time.week];
        }
        img_3d_res_addr[6] = num_pic_mini[time.day/10];
        img_3d_res_addr[7] = num_pic_mini[time.day%10];
        img_3d_res_addr[8] = bat_pic[uteDrvBatteryCommonGetBatteryIndex(sizeof(bat_pic)/sizeof(bat_pic[0]))];

        u8 direction = uteModuleSportGetDeviceOrientation();

        for (u8 i = 0; i < 9; i++)
        {
            compo_picturebox_t *pic = compo_picturebox_create(frm, img_3d_res_addr[i]);
            compo_setid(pic, ui_handle_1[i].id);
            switch (direction)
            {
                case 1:
                    compo_picturebox_set_pos(pic, ui_handle_2[i].pic_x, ui_handle_2[i].pic_y); // 90°
                    compo_picturebox_set_rotation(pic, 900);
                    break;
                case 4:
                    compo_picturebox_set_pos(pic, GUI_SCREEN_WIDTH - ui_handle_1[i].pic_x, GUI_SCREEN_HEIGHT - ui_handle_1[i].pic_y); // 180°
                    compo_picturebox_set_rotation(pic, 1800);
                    break;
                case 2:
                    compo_picturebox_set_pos(pic, GUI_SCREEN_WIDTH - ui_handle_2[i].pic_x, GUI_SCREEN_HEIGHT - ui_handle_2[i].pic_y); // 270°
                    compo_picturebox_set_rotation(pic, 2700);
                    break;
                default:
                    compo_picturebox_set_pos(pic, ui_handle_1[i].pic_x, ui_handle_1[i].pic_y); // 0°
                    compo_picturebox_set_rotation(pic, 0);
                    break;
            }
        }
        return frm;
    }
    else
#endif
    {
        char txt_buf[30];
        //新建窗体和背景
        compo_form_t *frm = compo_form_create(true);

        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
        compo_form_set_title(frm, i18n[STR_NULL]);

        compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_CHARGE_ICON_GIF_168X231_X36_Y53_BG_BIN);
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20);

        compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I335001_CHARGE_ICON_GIF_168X231_X36_Y53_GIF_BIN);
        compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+156/2+75/2-20);
        compo_animation_set_radix(animation, 16);
        compo_animation_set_interval(animation, 15);

        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
        compo_textbox_t *textbox = compo_textbox_create(frm, 5);
        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
        compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-20,150,50);
        compo_textbox_set(textbox,txt_buf);
        compo_setid(textbox,COMPO_ID_NUM_BAT_VALUE);

        picbox = compo_picturebox_create(frm, UI_BUF_I335001_CHARGE_BATT_14X18_X114_Y171_BIN);
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+30);
        compo_setid(picbox,COMPO_ID_BAT_PIC);
        compo_picturebox_set_visible(picbox,BAT_PERCENT_VALUE<100);


        return frm;
    }
}

#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

enum
{
    //数字
    COMPO_ID_NUM_BAT_VALUE=1,
    COMPO_ID_TTXT,
    COMPO_ID_GIF_PIC,
    COMPO_ID_BAT_PIC,
};

//电量更新
void func_charge_update(void)
{
    compo_textbox_t *txt_val   = compo_getobj_byid(COMPO_ID_NUM_BAT_VALUE);
    compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_BAT_PIC);

    char txt_buf[30];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_set_location(txt_val,GUI_SCREEN_CENTER_X,BAT_PERCENT_VALUE<=20 ? GUI_SCREEN_CENTER_Y-20 : GUI_SCREEN_CENTER_Y-10,150,50);
    compo_textbox_set(txt_val,txt_buf);
    compo_picturebox_set_visible(pic,BAT_PERCENT_VALUE<100);

}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    char txt_buf[30];
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_NULL]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_CHARGE_ICON_GIF_168X231_X36_Y53_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I335001_CHARGE_ICON_GIF_168X231_X36_Y53_GIF_BIN);
    compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+156/2+75/2-20);
    compo_animation_set_radix(animation, 16);
    compo_animation_set_interval(animation, 15);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_t *textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-20,150,50);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_NUM_BAT_VALUE);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_CHARGE_BATT_14X18_X114_Y171_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+30);
    compo_setid(picbox,COMPO_ID_BAT_PIC);
    compo_picturebox_set_visible(picbox,BAT_PERCENT_VALUE<100);


    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

enum
{
    //数字
    COMPO_ID_NUM_BAT_VALUE=1,
    COMPO_ID_TTXT,
    COMPO_ID_GIF_PIC,
    COMPO_ID_BAT_PIC,
};

//电量更新
void func_charge_update(void)
{
    compo_textbox_t *txt_val   = compo_getobj_byid(COMPO_ID_NUM_BAT_VALUE);
    compo_textbox_t *txt_title = compo_getobj_byid(COMPO_ID_TTXT);
    compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_BAT_PIC);
    compo_animation_t *animation = compo_getobj_byid(COMPO_ID_GIF_PIC);

    char txt_buf[30];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_set_location(txt_val,GUI_SCREEN_CENTER_X,BAT_PERCENT_VALUE<=20 ? GUI_SCREEN_CENTER_Y-40 : GUI_SCREEN_CENTER_Y-15,150,50);
    compo_textbox_set(txt_val,txt_buf);
    compo_textbox_set_visible(txt_title,BAT_PERCENT_VALUE<=20);
    compo_picturebox_set_visible(pic,bsp_charge_sta_get()==1);
    if (bsp_charge_sta_get() == 2)
    {
        compo_animation_set_interval(animation, 0);
    }
}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    char txt_buf[30];
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_NULL]);

    // ///设置动图
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I341001_CHARGE_00_BIN);
    compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, 365/2+75);
    compo_animation_set_radix(animation, 15);
    compo_animation_set_interval(animation, 15);
    compo_setid(animation,COMPO_ID_GIF_PIC);
    /*compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_CHARGE_LOW_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 365/2+75);*/

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_t *textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-15,150,50);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_NUM_BAT_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOW_BATTERY]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+6,150,50);
    compo_textbox_set_forecolor(textbox,COLOR_RED);
    compo_textbox_set(textbox,i18n[STR_LOW_BATTERY]);
    compo_setid(textbox,COMPO_ID_TTXT);
    compo_textbox_set_visible(textbox,false);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_CHARGE_BATT_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+52);
    compo_setid(picbox,COMPO_ID_BAT_PIC);
    compo_picturebox_set_visible(picbox,false);

    return frm;
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

enum
{
    //数字
    COMPO_ID_NUM_BAT_VALUE=1,
    COMPO_ID_TTXT,
    COMPO_ID_GIF_PIC,
    COMPO_ID_BAT_PIC,
};

//电量更新
void func_charge_update(void)
{
    compo_textbox_t *txt_val   = compo_getobj_byid(COMPO_ID_NUM_BAT_VALUE);
    compo_textbox_t *txt_title = compo_getobj_byid(COMPO_ID_TTXT);
    compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_BAT_PIC);
    compo_animation_t *animation = compo_getobj_byid(COMPO_ID_GIF_PIC);

    char txt_buf[30];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_set_location(txt_val,GUI_SCREEN_CENTER_X,BAT_PERCENT_VALUE<=20 ? GUI_SCREEN_CENTER_Y-40 : GUI_SCREEN_CENTER_Y-15,150,50);
    compo_textbox_set(txt_val,txt_buf);
    compo_textbox_set_visible(txt_title,BAT_PERCENT_VALUE<=20);
    compo_picturebox_set_visible(pic,bsp_charge_sta_get()==1);
    if (bsp_charge_sta_get() == 2)
    {
        compo_animation_set_interval(animation, 0);
    }
}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    char txt_buf[30];
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_NULL]);

    // ///设置动图
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I343001_CHARGE_ICON_BATT_BIN);
    compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, 296/2+74);
    compo_animation_set_radix(animation, 8);
    compo_animation_set_interval(animation, 8);
    compo_setid(animation,COMPO_ID_GIF_PIC);
    /*compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_CHARGE_LOW_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 365/2+75);*/

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_t *textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-15,150,50);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_NUM_BAT_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOW_BATTERY]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+6,150,50);
    compo_textbox_set_forecolor(textbox,COLOR_RED);
    compo_textbox_set(textbox,i18n[STR_LOW_BATTERY]);
    compo_setid(textbox,COMPO_ID_TTXT);
    compo_textbox_set_visible(textbox,false);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I343001_CHARGE_LOW_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 296/2+74);
    compo_setid(picbox,COMPO_ID_BAT_PIC);
    compo_picturebox_set_visible(picbox,false);

    return frm;
}


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

enum
{
    //数字
    COMPO_ID_NUM_TIME_HOUR_S = 1,
    COMPO_ID_NUM_TIME_HOUR_G,
    COMPO_ID_NUM_TIME_MIN_S,
    COMPO_ID_NUM_CLOLON,
    COMPO_ID_NUM_TIME_MIN_G,
    COMPO_ID_PIC_TIME_COLON,

    COMPO_ID_NUM_BAT_VALUE,
    COMPO_ID_NUM_TIME_WEEK,
    COMPO_ID_NUM_DATE_S,
    COMPO_ID_NUM_DATE_G,
    COMPO_ID_PIC_BAT_PERCENT,
    COMPO_ID_BAT_PIC,
};

typedef struct charge_ui_handle_t_
{
    u16 id;
    s16 pic_x;
    s16 pic_y;

} charge_ui_handle_t;

static const charge_ui_handle_t ui_handle_1 [] = //正面
{
    {.pic_x=79,    .pic_y=190, .id=COMPO_ID_NUM_TIME_HOUR_S},//小时 十位
    {.pic_x=79+54, .pic_y=190, .id=COMPO_ID_NUM_TIME_HOUR_G},//小时 个位
    {.pic_x=79+54*2, .pic_y=190, .id=COMPO_ID_NUM_CLOLON},//冒号
    {.pic_x=79+54*3, .pic_y=190, .id=COMPO_ID_NUM_TIME_MIN_S},//分钟 十位
    {.pic_x=79+54*4, .pic_y=190, .id=COMPO_ID_NUM_TIME_MIN_G},//分钟 个位
    {.pic_x=206, .pic_y=272, .id=COMPO_ID_NUM_TIME_WEEK},//星期
    {.pic_x=264, .pic_y=272, .id=COMPO_ID_NUM_DATE_S},//日期 十位
    {.pic_x=284, .pic_y=272, .id=COMPO_ID_NUM_DATE_G},//日期 个位
    {.pic_x=268, .pic_y=93, .id=COMPO_ID_PIC_BAT_PERCENT},//电量
};

#define num_big_0    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_03_BIN
#define num_big_1    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_04_BIN
#define num_big_2    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_05_BIN
#define num_big_3    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_06_BIN
#define num_big_4    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_07_BIN
#define num_big_5    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_08_BIN
#define num_big_6    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_08_BIN
#define num_big_7    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_10_BIN
#define num_big_8    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_11_BIN
#define num_big_9    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_12_BIN
#define colon_big    UI_BUF_I338003_CHAGE_NUM_HOUR_MIN_54X76_X52_Y152_X186_Y152_13_BIN

#define num_mini_0    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_03_BIN
#define num_mini_1    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_04_BIN
#define num_mini_2    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_05_BIN
#define num_mini_3    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_06_BIN
#define num_mini_4    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_07_BIN
#define num_mini_5    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_08_BIN
#define num_mini_6    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_08_BIN
#define num_mini_7    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_10_BIN
#define num_mini_8    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_11_BIN
#define num_mini_9    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_12_BIN
#define colon_mini    UI_BUF_I338003_CHAGE_NUM_DATE_20X28_X254_Y258_13_BIN

#define bat_pic_0     UI_BUF_I338003_CHAGE_ICON_BATT_64X64_X236_Y61_00_BIN
#define bat_pic_1     UI_BUF_I338003_CHAGE_ICON_BATT_64X64_X236_Y61_01_BIN
#define bat_pic_2     UI_BUF_I338003_CHAGE_ICON_BATT_64X64_X236_Y61_02_BIN
#define bat_pic_3     UI_BUF_I338003_CHAGE_ICON_BATT_64X64_X236_Y61_03_BIN
#define bat_pic_4     UI_BUF_I338003_CHAGE_ICON_BATT_64X64_X236_Y61_04_BIN

#define week_zh_1     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_01_BIN
#define week_zh_2     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_02_BIN
#define week_zh_3     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_03_BIN
#define week_zh_4     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_04_BIN
#define week_zh_5     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_05_BIN
#define week_zh_6     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_06_BIN
#define week_zh_7     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_00_BIN

#define week_en_1     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_08_BIN
#define week_en_2     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_09_BIN
#define week_en_3     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_10_BIN
#define week_en_4     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_11_BIN
#define week_en_5     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_12_BIN
#define week_en_6     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_13_BIN
#define week_en_7     UI_BUF_I338003_CHAGE_CHAR_WEEK_84X34_X164_Y255_07_BIN

const static u32 num_pic_big[10]=
{
    num_big_0,
    num_big_1,
    num_big_2,
    num_big_3,
    num_big_4,
    num_big_5,
    num_big_6,
    num_big_7,
    num_big_8,
    num_big_9,
};
const static u32 num_pic_mini[10]=
{
    num_mini_0,
    num_mini_1,
    num_mini_2,
    num_mini_3,
    num_mini_4,
    num_mini_5,
    num_mini_6,
    num_mini_7,
    num_mini_8,
    num_mini_9,
};
const static u32 bat_pic[5]=
{
    bat_pic_0,
    bat_pic_1,
    bat_pic_2,
    bat_pic_3,
    bat_pic_4,
};
const static u32 week_pic_zh[10]=
{
    week_zh_7,
    week_zh_1,
    week_zh_2,
    week_zh_3,
    week_zh_4,
    week_zh_5,
    week_zh_6,
};
const static u32 week_pic_en[10]=
{
    week_en_7,
    week_en_1,
    week_en_2,
    week_en_3,
    week_en_4,
    week_en_5,
    week_en_6,
};

//电量更新
void func_charge_update(void)
{
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    if(uteModuleBedsideModeIsOpen())
    {
        u32 img_3d_res_addr[9]= {0};
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        img_3d_res_addr[0] = num_pic_big[time.hour/10];
        img_3d_res_addr[1] = num_pic_big[time.hour%10];
        img_3d_res_addr[2] = colon_big;
        img_3d_res_addr[3] = num_pic_big[time.min/10];
        img_3d_res_addr[4] = num_pic_big[time.min%10];
        if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
        {
            img_3d_res_addr[5] = week_pic_zh[time.week];
        }
        else
        {
            img_3d_res_addr[5] = week_pic_en[time.week];
        }
        img_3d_res_addr[6] = num_pic_mini[time.day/10];
        img_3d_res_addr[7] = num_pic_mini[time.day%10];
        img_3d_res_addr[8] = bat_pic[uteDrvBatteryCommonGetBatteryIndex(sizeof(bat_pic)/sizeof(bat_pic[0]))];

        f_charge_t *f_charge = (f_charge_t *)func_cb.f_cb;

        u8 direction = uteModuleSportGetDeviceOrientation();
        if(direction > 0 && direction < 5)
        {
            f_charge->last_orientation = direction;
        }

        for (u8 i = 0; i < 9; i++)
        {
            compo_picturebox_t *pic = compo_getobj_byid(ui_handle_1[i].id);
            compo_picturebox_set(pic, img_3d_res_addr[i]);
            // printf("mode:%d\n",f_charge->last_orientation );
            switch (f_charge->last_orientation)
            {
                case 3:
                    compo_picturebox_set_pos(pic, ui_handle_1[i].pic_x, ui_handle_1[i].pic_y); // 0°
                    compo_picturebox_set_rotation(pic, 0);
                    break;
                case 1:
                    compo_picturebox_set_pos(pic, GUI_SCREEN_HEIGHT-ui_handle_1[i].pic_y,ui_handle_1[i].pic_x); // 90°
                    compo_picturebox_set_rotation(pic, 900);
                    break;
                case 4:
                    compo_picturebox_set_pos(pic, GUI_SCREEN_WIDTH - ui_handle_1[i].pic_x, GUI_SCREEN_HEIGHT - ui_handle_1[i].pic_y); // 180°
                    compo_picturebox_set_rotation(pic, 1800);
                    break;
                case 2:
                    compo_picturebox_set_pos(pic, ui_handle_1[i].pic_y, GUI_SCREEN_WIDTH - ui_handle_1[i].pic_x); // 270°
                    compo_picturebox_set_rotation(pic, 2700);
                    break;
                default:
                    break;
            }
        }
    }
    else
#endif
    {
        compo_textbox_t *txt_val   = compo_getobj_byid(COMPO_ID_NUM_BAT_VALUE);
        compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_BAT_PIC);

        char txt_buf[30];
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
        compo_textbox_set_location(txt_val,GUI_SCREEN_CENTER_X,BAT_PERCENT_VALUE<=20 ? GUI_SCREEN_CENTER_Y-20 : GUI_SCREEN_CENTER_Y-10,150,50);
        compo_textbox_set(txt_val,txt_buf);
        compo_picturebox_set_visible(pic,BAT_PERCENT_VALUE<100);
    }
}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    if(uteModuleBedsideModeIsOpen())
    {
        //新建窗体和背景
        compo_form_t *frm = compo_form_create(true);

        u32 img_3d_res_addr[9]= {0};

        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);

        img_3d_res_addr[0] = num_pic_big[time.hour/10];
        img_3d_res_addr[1] = num_pic_big[time.hour%10];
        img_3d_res_addr[2] = colon_big;
        img_3d_res_addr[3] = num_pic_big[time.min/10];
        img_3d_res_addr[4] = num_pic_big[time.min%10];
        if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
        {
            img_3d_res_addr[5] = week_pic_zh[time.week];
        }
        else
        {
            img_3d_res_addr[5] = week_pic_en[time.week];
        }
        img_3d_res_addr[6] = num_pic_mini[time.day/10];
        img_3d_res_addr[7] = num_pic_mini[time.day%10];
        img_3d_res_addr[8] = bat_pic[uteDrvBatteryCommonGetBatteryIndex(sizeof(bat_pic)/sizeof(bat_pic[0]))];

        u8 direction = uteModuleSportGetDeviceOrientation();

        for (u8 i = 0; i < 9; i++)
        {
            compo_picturebox_t *pic = compo_picturebox_create(frm, img_3d_res_addr[i]);
            compo_setid(pic, ui_handle_1[i].id);
            switch (direction)
            {
                case 1:
                    compo_picturebox_set_pos(pic, GUI_SCREEN_HEIGHT-ui_handle_1[i].pic_y,ui_handle_1[i].pic_x); // 90°
                    compo_picturebox_set_rotation(pic, 900);
                    break;
                case 4:
                    compo_picturebox_set_pos(pic, GUI_SCREEN_WIDTH - ui_handle_1[i].pic_x, GUI_SCREEN_HEIGHT - ui_handle_1[i].pic_y); // 180°
                    compo_picturebox_set_rotation(pic, 1800);
                    break;
                case 2:
                    compo_picturebox_set_pos(pic, ui_handle_1[i].pic_y, GUI_SCREEN_WIDTH - ui_handle_1[i].pic_x); // 270°
                    compo_picturebox_set_rotation(pic, 2700);
                    break;
                default:
                    compo_picturebox_set_pos(pic, ui_handle_1[i].pic_x, ui_handle_1[i].pic_y); // 0°
                    compo_picturebox_set_rotation(pic, 0);
                    break;
            }
        }
        return frm;
    }
    else
#endif
    {
        char txt_buf[30];
        //新建窗体和背景
        compo_form_t *frm = compo_form_create(true);

        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
        compo_form_set_title(frm, i18n[STR_NULL]);

        //低电量背景
        compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338001_CHARGE_BATT_GIF_BG_BIN);
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);

        //充电中动画
        compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I338001_CHARGE_BATT_GIF_BIN);
        compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+206/2+90/2);//(360-296)/2
        compo_animation_set_radix(animation, 8);
        compo_animation_set_interval(animation, 15);

        //--低电量跟充满电是同一个图标
        picbox = compo_picturebox_create(frm, UI_BUF_I338001_CHARGE_BATT_BATT_BIN);
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+60);//360-290/2
        compo_setid(picbox,COMPO_ID_BAT_PIC);
        compo_picturebox_set_visible(picbox,BAT_PERCENT_VALUE!=100);

        //电量
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
        compo_textbox_t *textbox = compo_textbox_create(frm, 5);
        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
        compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,191,150,50);
        compo_textbox_set(textbox,txt_buf);
        compo_setid(textbox,COMPO_ID_NUM_BAT_VALUE);
        return frm;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
enum
{
    //数字
    COMPO_ID_NUM_TIME_HOUR = 1,
    COMPO_ID_NUM_TIME_MIN,
    COMPO_ID_PIC_TIME_COLON,

    COMPO_ID_NUM_BAT_VALUE,
    COMPO_ID_PIC_BAT_PERCENT,

    COMPO_ID_RECT_BAT_1,
    COMPO_ID_RECT_BAT_2,
    COMPO_ID_RECT_BAT_3,
    COMPO_ID_RECT_BAT_4,
    COMPO_ID_RECT_BAT_5,
};


typedef struct charge_ui_handle_t_
{
    struct ui_time_hour
    {
        u16 id_hour;
        s16 x_hour, y_hour;
        u16 w_hour, h_hour;
        u8 hour;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_hour;

    struct ui_time_min
    {
        u16 id_min;
        s16 x_min, y_min;
        u16 w_min, h_min;
        u8 min;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } time_min;

    struct ui_time_colon
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;
        u32 destroy_res;
        u8 rev;
    } time_colon;

    struct ui_bat_value
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;        //res不为0使用图库，为0使用字库
        u8 num_layout;  //横向，竖向排列
        u8 num_align;   //对其方式
        bool flag_zfill;
        int  margin;
        u8 rev;
    } bat_value;

    struct ui_bat_percent
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 res;
        u8 rev;
    } bat_percent;

    struct ui_bat_rect
    {
        u16 id;
        s16 x, y;
        u16 w, h;
        u32 bright_res;       //res不为0使用图库，为0使用组件实现
        u32 destroy_res;
        color_t bright_color;
        color_t destroy_color;
        u16 rev;              //矩形圆角
    } bat_rect[5];

} charge_ui_handle_t;

static const charge_ui_handle_t ui_handle =
{

    .time_hour = {
        .id_hour = COMPO_ID_NUM_TIME_HOUR,
        .x_hour = GUI_SCREEN_CENTER_X-76,
        .y_hour = 84+96/2,
        .w_hour = 0,
        .h_hour = 0,
        .hour   = 0,
        .res    = UI_BUF_I340001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 0,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_min = {
        .id_min = COMPO_ID_NUM_TIME_MIN,
        .x_min  = GUI_SCREEN_CENTER_X+76,
        .y_min  = 84+96/2,
        .w_min  = 0,
        .h_min  = 0,
        .min    = 0,
        .res    = UI_BUF_I340001_CHARGE_TIME_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 0,
        .num_align  = 0,
        .flag_zfill = true,
        .margin     = -5,
        .rev    = 2,
    },

    .time_colon = {
        .id     = COMPO_ID_PIC_TIME_COLON,
        .x      = GUI_SCREEN_CENTER_X,
        .y      = 84+96/2,
        .w      = 0,
        .h      = 0,
        .bright_res     = UI_BUF_I340001_CHARGE_COLON2_BIN,
        .destroy_res    = UI_BUF_I340001_CHARGE_COLON_BIN,
        .rev    = 0,
    },

    .bat_value = {
        .id     = COMPO_ID_NUM_BAT_VALUE,
        .x      = 83,
        .y      = 231+60/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I340001_CHARGE_POWER_BIN,        //res不为0使用图库，为0使用字库
        .num_layout = 0,  //横向，竖向排列
        .num_align  = 0,   //对其方式
        .flag_zfill = false,
        .margin     = -5,
        .rev    = 3,
    },

    .bat_percent = {
        .id     = COMPO_ID_PIC_BAT_PERCENT,
        .x      = 83+74,
        .y      = 231+60/2,
        .w      = 0,
        .h      = 0,
        .res    = UI_BUF_I340001_CHARGE_PERCENT_BIN,
        .rev    = 0,
    },

    .bat_rect = {
        [0]     = {COMPO_ID_RECT_BAT_1,         108+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [1]     = {COMPO_ID_RECT_BAT_2,         126+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [2]     = {COMPO_ID_RECT_BAT_3,         144+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [3]     = {COMPO_ID_RECT_BAT_4,         162+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
        [4]     = {COMPO_ID_RECT_BAT_5,         180+14/2+78,   231+60/2,     14,  60,   0,  0,  {0,255,225}, {0,76,68}, 14/2},
    },


};

//电量更新
void func_charge_update(void)
{

    //更新bat Value
    compo_number_t* bat_value = compo_getobj_byid(ui_handle.bat_value.id);
    compo_number_set(bat_value, BAT_PERCENT_VALUE);

    //更新bat rect
    u8 bright_value = (sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])) * BAT_PERCENT_VALUE / 100;      //等级

    //熄灭的
    for(int i=0; i<(sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0])); i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].destroy_color.r, ui_handle.bat_rect[i].destroy_color.g, ui_handle.bat_rect[i].destroy_color.b));
    }

    //点亮
    for(int i=0; i<bright_value; i++)
    {
        compo_shape_t* bat_rect = compo_getobj_byid(ui_handle.bat_rect[i].id);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
    }

    //更新 :
    static u32 tick = 0;
    static bool bright_sta = 0;
    compo_picturebox_t* colon = compo_getobj_byid(ui_handle.time_colon.id);
    if (tick_check_expire(tick, 1000))
    {
        tick = tick_get();
        if (bright_sta)
        {
            compo_picturebox_set(colon, ui_handle.time_colon.bright_res);
        }
        else
        {
            compo_picturebox_set(colon, ui_handle.time_colon.destroy_res);
        }
        bright_sta = !bright_sta;
    }

}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建time hour
    compo_number_t* hour = compo_number_create(frm, ui_handle.time_hour.res, ui_handle.time_hour.rev);
    compo_number_set_align(hour, ui_handle.time_hour.num_align);
    compo_number_set_layout(hour, ui_handle.time_hour.num_layout);
    compo_number_set_zfill(hour, ui_handle.time_hour.flag_zfill);
    compo_number_set_margin(hour, ui_handle.time_hour.margin);
    compo_number_set_pos(hour, ui_handle.time_hour.x_hour, ui_handle.time_hour.y_hour);
    compo_number_set(hour, ui_handle.time_hour.hour);
    compo_setid(hour, ui_handle.time_hour.id_hour);
    compo_bonddata(hour, COMPO_BOND_HOUR);

    //创建timer min
    compo_number_t* min = compo_number_create(frm, ui_handle.time_min.res, ui_handle.time_min.rev);
    compo_number_set_align(min, ui_handle.time_min.num_align);
    compo_number_set_layout(min, ui_handle.time_min.num_layout);
    compo_number_set_zfill(min, ui_handle.time_min.flag_zfill);
    compo_number_set_margin(min, ui_handle.time_min.margin);
    compo_number_set_pos(min, ui_handle.time_min.x_min, ui_handle.time_min.y_min);
    compo_number_set(min, ui_handle.time_min.min);
    compo_setid(min, ui_handle.time_min.id_min);
    compo_bonddata(min, COMPO_BOND_MINUTE);

    //创建 :
    compo_picturebox_t* colon = compo_picturebox_create(frm, ui_handle.time_colon.bright_res);
    compo_picturebox_set_pos(colon, ui_handle.time_colon.x, ui_handle.time_colon.y);
    compo_setid(colon, ui_handle.time_colon.id);

    //创建BAT Vaule
    compo_number_t* bat_value = compo_number_create(frm, ui_handle.bat_value.res, ui_handle.bat_value.rev);
    compo_number_set_align(bat_value, ui_handle.bat_value.num_align);
    compo_number_set_layout(bat_value, ui_handle.bat_value.num_layout);
    compo_number_set_zfill(bat_value, ui_handle.bat_value.flag_zfill);
    compo_number_set_margin(bat_value, ui_handle.bat_value.margin);
    compo_number_set_pos(bat_value, ui_handle.bat_value.x, ui_handle.bat_value.y);
    compo_number_set(bat_value, 100);
    compo_setid(bat_value, ui_handle.bat_value.id);

    //创建 %
    compo_picturebox_t* bat_percent = compo_picturebox_create(frm, ui_handle.bat_percent.res);
    compo_picturebox_set_pos(bat_percent, ui_handle.bat_percent.x, ui_handle.bat_percent.y);
    compo_setid(bat_percent, ui_handle.bat_percent.id);

    //创建rect
    for (int i=0; i<sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0]); i++)
    {
        compo_shape_t* bat_rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
        compo_shape_set_location(bat_rect, ui_handle.bat_rect[i].x, ui_handle.bat_rect[i].y, ui_handle.bat_rect[i].w, ui_handle.bat_rect[i].h);
        compo_shape_set_radius(bat_rect, ui_handle.bat_rect[i].rev);
        compo_setid(bat_rect, ui_handle.bat_rect[i].id);
    }

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
enum
{
    //数字
    COMPO_ID_NUM_BAT_VALUE=1,
    COMPO_ID_TTXT,
    COMPO_ID_GIF_PIC,
    COMPO_ID_BAT_PIC,
};

//电量更新
void func_charge_update(void)
{
    compo_textbox_t *txt_val   = compo_getobj_byid(COMPO_ID_NUM_BAT_VALUE);
    compo_textbox_t *txt_title = compo_getobj_byid(COMPO_ID_TTXT);
    compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_BAT_PIC);

    char txt_buf[30];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_set_location(txt_val,GUI_SCREEN_CENTER_X,BAT_PERCENT_VALUE<=20 ? 95+35/2 : 109+35/2,150,50);
    compo_textbox_set(txt_val,txt_buf);
    compo_textbox_set_visible(txt_title,BAT_PERCENT_VALUE<=20 && BAT_PERCENT_FLAG == BAT_STATUS_NO_CHARGE);
    compo_picturebox_set_visible(pic,(BAT_PERCENT_VALUE<=20 && BAT_PERCENT_FLAG == BAT_STATUS_NO_CHARGE) || (BAT_PERCENT_VALUE==100 && BAT_PERCENT_FLAG == BAT_STATUS_CHARGED));

}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    char txt_buf[30];
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_NULL]);

    //充电中动画
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I342001_CHARGE_BATT_GIF_BIN);
    compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, 42+198/2);//(360-296)/2
    compo_animation_set_radix(animation, 8);
    compo_animation_set_interval(animation, 15);
    compo_setid(animation,COMPO_ID_GIF_PIC);
    compo_animation_set_visible(animation,BAT_PERCENT_FLAG == BAT_STATUS_CHARGING);

    //低电量背景--低电量跟充满电是同一个图标
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I342001_CHARGE_LOW_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,42+198/2);//360-290/2
    compo_setid(picbox,COMPO_ID_BAT_PIC);
    compo_picturebox_set_visible(picbox,(BAT_PERCENT_VALUE<=20 && BAT_PERCENT_FLAG == BAT_STATUS_NO_CHARGE) || (BAT_PERCENT_VALUE==100 && BAT_PERCENT_FLAG == BAT_STATUS_CHARGED));

    //电量
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_t *textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,109+35/2,150,50);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_NUM_BAT_VALUE);

    //低电量文本
    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOW_BATTERY]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,132+32/2,150,30);
    compo_textbox_set_forecolor(textbox,COLOR_RED);
    compo_textbox_set(textbox,i18n[STR_LOW_BATTERY]);
    compo_setid(textbox,COMPO_ID_TTXT);
    compo_textbox_set_visible(textbox,BAT_PERCENT_VALUE<=20 && BAT_PERCENT_FLAG == BAT_STATUS_NO_CHARGE);


    return frm;
}

#else
//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
void func_charge_update(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//充电功能事件处理
static void func_charge_process(void)
{
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    if (uteModuleBedsideModeIsOpen())
    {
        reset_guioff_delay();
    }
#endif
    func_charge_update();
    func_process();
}

//充电功能消息处理
static void func_charge_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            break;
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG:
            break;

        case KU_BACK:
        case KL_BACK:
        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        default:
            // func_message(msg);
            break;
    }
}

//进入充电功能
static void func_charge_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_charge_t));
    func_cb.frm_main = func_charge_form_create();

    f_charge_t *f_charge = (f_charge_t *)func_cb.f_cb;
    f_charge->percent_bkp = BAT_PERCENT_VALUE;
    func_cb.enter_tick = tick_get();
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    u8 direction = uteModuleSportGetDeviceOrientation(); // 5、6为平放，忽略
    if (direction > 0 && direction < 5)
    {
        f_charge->last_orientation = direction;
    }
    else
    {
        f_charge->last_orientation = 1;
    }
#endif

#if UTE_MODULE_BEDSIDE_MODE_LIGHT_LOW_SUPPORT
    if(uteModuleBedsideModeIsOpen())
    {
        f_charge->light_level = sys_cb.light_level;
        tft_bglight_set_level(1,false);
    }
#endif
}

//退出充电功能
static void func_charge_exit(void)
{
    printf("%s\n", __func__);
    f_charge_t *f_charge = (f_charge_t*)func_cb.f_cb;
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_CHARGE;

#if UTE_MODULE_BEDSIDE_MODE_LIGHT_LOW_SUPPORT
    if(uteModuleBedsideModeIsOpen())
    {
        tft_bglight_set_level(f_charge->light_level,false);
        uteModuleGuiCommonSetBackLightPercent(tft_cb.tft_bglight_duty);
    }
#endif

    if(sys_cb.power_on_flag == false)
    {
        sys_cb.power_on_state=true;
        if(!uteApplicationCommonIsHasConnectOurApp())
        {
            sys_cb.power_on_state=false;
            ute_module_systemtime_time_t time;
            uteModuleSystemtimeGetTime(&time);

            if(time.isWatchSetLangage == false)
            {
                func_cb.sta = FUNC_POWER_ON_LANGUAGE;
            }
            else
            {
                func_cb.sta = FUNC_POWER_ON_SCAN;
            }
        }
        sys_cb.power_on_flag = true;
    }
}

//充电功能
void func_charge(void)
{
    printf("%s\n", __func__);
    func_charge_enter();
    while (func_cb.sta == FUNC_CHARGE)
    {
        func_charge_process();
        func_charge_message(msg_dequeue());
        if (!bsp_charge_sta_get() && func_cb.sta == FUNC_CHARGE)
        {
            func_directly_back_to();
        }
    }
    func_charge_exit();
}
