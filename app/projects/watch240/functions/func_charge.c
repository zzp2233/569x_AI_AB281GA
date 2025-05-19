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
    COMPO_ID_NUM_TIME_HOUR = 1,
    COMPO_ID_NUM_TIME_MIN,
    COMPO_ID_PIC_TIME_COLON,

    COMPO_ID_NUM_BAT_VALUE,
    COMPO_ID_NUM_TIME_WEEK,
    COMPO_ID_NUM_DATE_VALUE,
    COMPO_ID_PIC_BAT_PERCENT,

    COMPO_ID_RECT_BAT_1,
    COMPO_ID_RECT_BAT_2,
    COMPO_ID_RECT_BAT_3,
    COMPO_ID_RECT_BAT_4,
    COMPO_ID_RECT_BAT_5,
};

typedef struct charge_ui_handle_t_
{
    struct ui_time_pic
    {
        u16 id_hour_s;
        u16 id_hour_g;
        u16 id_min_s;
        u16 id_min_g;
        u16 id_week_s;
        u16 id_week_g;
        u16 id_week_zh;
        u16 id_week_bat_pic;
    } time_pic;

    struct ui_pic_axis
    {
        s16 hour_s_x;
        s16 hour_s_y;
        s16 min_s_x;
        s16 min_s_y;
        s16 hour_m_x;
        s16 hour_m_y;
        s16 min_m_x;
        s16 min_m_y;
        s16 colon_x;
        s16 colon_y;
        u16 id_week_zh_x;
        u16 id_week_zh_y;
        u16 id_week_s_x;
        u16 id_week_s_y;
        u16 id_week_g_x;
        u16 id_week_g_y;
        u16 id_week_bat_pic_x;
        u16 id_week_bat_pic_y;
    } pic_axis;
} charge_ui_handle_t;

static const charge_ui_handle_t ui_handle =
{

};

//电量更新
void func_charge_update(void)
{
    // compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_NUM_BAT_VALUE);
    // compo_picturebox_t *week    = compo_getobj_byid(COMPO_ID_NUM_TIME_WEEK);
    // ute_module_systemtime_time_t time;
    // uteModuleSystemtimeGetTime(&time);
    // compo_picturebox_cut(week,time.week, 7);

    // compo_picturebox_cut(pic, uteDrvBatteryCommonGetBatteryIndex(4), 4);
}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建 :
    compo_picturebox_t*  pic =  compo_picturebox_create(frm, UI_BUF_I269032_CHARGE_XIE_21X14_X31_Y245_BIN);

    compo_picturebox_set_pos(pic, 42, 178);
    //compo_picturebox_cut(pic, 2, 7);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
    {
        pic =  compo_picturebox_create(frm, UI_BUF_I269032_CHARGE_CHAR_WEEK_ZH_30X50_X27_Y195_BIN);
    }
    else
    {
        pic = compo_picturebox_create(frm,UI_BUF_I269032_CHARGE_CHAR_WEEK_EN_30X50_X27_Y195_BIN);
    }
    compo_picturebox_set_pos(pic, 42, 100);
    compo_setid(pic, COMPO_ID_NUM_TIME_WEEK);
    compo_picturebox_cut(pic,time.week, 7);

//     //创建time hour
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

    pic = compo_picturebox_create(frm, UI_BUF_I269032_CHARGE_MAOHAO_88X49_X65_Y43_X65_Y175_BIN);
    compo_picturebox_set_pos(pic, 109, 135);
    compo_setid(pic, COMPO_ID_PIC_TIME_COLON);


    pic =  compo_picturebox_create(frm,UI_BUF_I269032_CHARGE_ICON_54X54_X161_Y219_BIN);
    compo_picturebox_set_pos(pic, 178,246);
    compo_setid(pic, COMPO_ID_NUM_BAT_VALUE);
    compo_picturebox_cut(pic, uteDrvBatteryCommonGetBatteryIndex(4), 4);

    //创建 Vaule
    compo_number_t* bat_value = compo_number_create(frm, ui_handle.bat_value.res, ui_handle.bat_value.rev);
    compo_number_set_align(bat_value, ui_handle.bat_value.num_align);
    compo_number_set_layout(bat_value, ui_handle.bat_value.num_layout);
    compo_number_set_zfill(bat_value, ui_handle.bat_value.flag_zfill);
    compo_number_set_margin(bat_value, ui_handle.bat_value.margin);
    compo_number_set_pos(bat_value, ui_handle.bat_value.x, ui_handle.bat_value.y);
    compo_number_set(bat_value,ui_handle.bat_value.val);
    compo_setid(bat_value, ui_handle.bat_value.id);
    compo_bonddata(bat_value,COMPO_BOND_MONTH);

    compo_number_t* bat_date = compo_number_create(frm, ui_handle.bat_date.res, ui_handle.bat_date.rev);
    compo_number_set_align(bat_date, ui_handle.bat_date.num_align);
    compo_number_set_layout(bat_date, ui_handle.bat_date.num_layout);
    compo_number_set_zfill(bat_date, ui_handle.bat_date.flag_zfill);
    compo_number_set_margin(bat_date, ui_handle.bat_date.margin);
    compo_number_set_pos(bat_date, ui_handle.bat_date.x, ui_handle.bat_date.y);
    compo_number_set(bat_date,ui_handle.bat_date.val);
    compo_setid(bat_date, ui_handle.bat_date.id);
    compo_bonddata(bat_date,COMPO_BOND_DAY);

    // //创建 %
    // compo_picturebox_t* bat_percent = compo_picturebox_create(frm, ui_handle.bat_percent.res);
    // compo_picturebox_set_pos(bat_percent, ui_handle.bat_percent.x, ui_handle.bat_percent.y);
    // compo_setid(bat_percent, ui_handle.bat_percent.id);

    //创建rect
    // for (int i=0; i<sizeof(ui_handle.bat_rect)/sizeof(ui_handle.bat_rect[0]); i++)
    // {
    //     compo_shape_t* bat_rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    //     compo_shape_set_color(bat_rect, make_color(ui_handle.bat_rect[i].bright_color.r, ui_handle.bat_rect[i].bright_color.g, ui_handle.bat_rect[i].bright_color.b));
    //     compo_shape_set_location(bat_rect, ui_handle.bat_rect[i].x, ui_handle.bat_rect[i].y, ui_handle.bat_rect[i].w, ui_handle.bat_rect[i].h);
    //     compo_shape_set_radius(bat_rect, ui_handle.bat_rect[i].rev);
    //     compo_setid(bat_rect, ui_handle.bat_rect[i].id);
    // }

    return frm;
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
    compo_textbox_t *txt_title = compo_getobj_byid(COMPO_ID_TTXT);
    compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_BAT_PIC);

    char txt_buf[30];
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_set_location(txt_val,GUI_SCREEN_CENTER_X,BAT_PERCENT_VALUE<=20 ? GUI_SCREEN_CENTER_Y-20 : GUI_SCREEN_CENTER_Y-10,150,50);
    compo_textbox_set(txt_val,txt_buf);
    compo_textbox_set_visible(txt_title,BAT_PERCENT_VALUE<=20);
    compo_picturebox_set_visible(pic,BAT_PERCENT_VALUE>20 && BAT_PERCENT_VALUE<100);

}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    char txt_buf[30];
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_NULL]);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I335001_CHARGE_ICON_GIF_168X231_X36_Y53_00_BIN);
    compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+20);
    compo_animation_set_radix(animation, 16);
    compo_animation_set_interval(animation, 15);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_t *textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y-20,150,50);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_NUM_BAT_VALUE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOW_BATTERY]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+20,150,30);
    compo_textbox_set_forecolor(textbox,COLOR_RED);
    compo_textbox_set(textbox,i18n[STR_LOW_BATTERY]);
    compo_setid(textbox,COMPO_ID_TTXT);
    compo_textbox_set_visible(textbox,BAT_PERCENT_VALUE<=20);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_CHARGE_BATT_14X18_X114_Y171_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+20);
    compo_setid(picbox,COMPO_ID_BAT_PIC);
    compo_picturebox_set_visible(picbox,BAT_PERCENT_VALUE>20);


    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
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
    compo_textbox_set_location(txt_val,GUI_SCREEN_CENTER_X,BAT_PERCENT_VALUE<=20 ? 169 : 191,150,50);
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
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I338001_CHARGE_GIF_BATT_238X2368_BIN);
    compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+32);//(360-296)/2
    compo_animation_set_radix(animation, 8);
    compo_animation_set_interval(animation, 15);
    compo_setid(animation,COMPO_ID_GIF_PIC);
    compo_animation_set_visible(animation,BAT_PERCENT_FLAG == BAT_STATUS_CHARGING);

    //低电量背景--低电量跟充满电是同一个图标
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338001_CHARGE_LOW_232X290_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,215);//360-290/2
    compo_setid(picbox,COMPO_ID_BAT_PIC);
    compo_picturebox_set_visible(picbox,(BAT_PERCENT_VALUE<=20 && BAT_PERCENT_FLAG == BAT_STATUS_NO_CHARGE) || (BAT_PERCENT_VALUE==100 && BAT_PERCENT_FLAG == BAT_STATUS_CHARGED));

    //电量
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
    compo_textbox_t *textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,191,150,50);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_NUM_BAT_VALUE);

    //低电量文本
    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOW_BATTERY]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,216,150,30);
    compo_textbox_set_forecolor(textbox,COLOR_RED);
    compo_textbox_set(textbox,i18n[STR_LOW_BATTERY]);
    compo_setid(textbox,COMPO_ID_TTXT);
    compo_textbox_set_visible(textbox,BAT_PERCENT_VALUE<=20 && BAT_PERCENT_FLAG == BAT_STATUS_NO_CHARGE);


    return frm;
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
}

//退出充电功能
static void func_charge_exit(void)
{
    printf("%s\n", __func__);
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_CHARGE;

    if(sys_cb.power_on_flag == true) return;

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
