#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_sport.h"
#include "ute_module_sleep.h"
#include "ute_module_heart.h"
#include "func_bt.h"
#include "ute_module_call.h"
#include "ute_module_music.h"

#if UTE_MODULE_SCREENS_UP_MENU_SUPPORT

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if 0

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define CARD_SCALE_START_POS            190//265                                                                     //卡片开始缩小位置
#define CARD_SCALE_STOP_POS             230//320                                                                     //卡片停止缩小位置
#define CARD_SCALE_STOP_Y               (CARD_SCALE_STOP_POS + 156 * 2)                                         //卡片停止缩小实际位置（越大缩小越慢，可见叠加越多）
#define CARD_WIDTH_MIN                  150//200                                                                     //卡片缩小的最小宽度
#define CARD_UNVISIBLE_POS_TOP          (-100)//(-160)                                                                  //卡片超出屏幕顶部某位置后设为不可见
#define CARD_UNVISIBLE_POS_BOTTOM       (CARD_SCALE_STOP_Y + 156)                                               //卡片缩到最小后实际位置大于某值设为不可见
#define CARD_BG_GRAY_START              41                                                                      //卡片开始缩小位置处背景的灰度
#define CARD_BG_GRAY_STOP               10                                                                      //卡片停止缩小位置处背景的灰度
#define GRAY_LV(x)                      make_color(x, x, x)                                                     //生成灰度颜色值（0:黑 255:白）

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
//#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - card_y_info[CARD_COUNT - 1] + 50)                     //回弹到底部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_END-1) + 50)                     //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + 30)                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - 78)                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 5   //最近任务不足5个时默认值补上
// const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT,FUNC_ADDRESS_BOOK, FUNC_HEARTRATE,FUNC_WEATHER};
///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  232//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_timer;
        struct text_t text_time;
    } card4;

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_stopwatch;
        struct text_t text_time;
    } card5;

    struct card6_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_comm_use;
        struct pic_click_t pic_click[3];
    } card6;

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[4];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,
    COMPO_ID_CLOCK_H,
    COMPO_ID_CLOCK_M,
    COMPO_ID_DATE,
    COMPO_ID_WEEKDAY,

    COMPO_CARD_START,
    COMPO_ID_CARD_1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    COMPO_ID_CARD_5,
    COMPO_ID_CARD_6,
    COMPO_ID_CARD_7,
    COMPO_CARD_END,

    COMPO_ID_ARC_KCAL,
    COMPO_ID_ARC_KM,
    COMPO_ID_ARC_STEP,

    COMPO_MUSIC_BTN_PREV,
    COMPO_MUSIC_BTN_NEXT,
    COMPO_MUSIC_BTN_PLAY,

    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
    COMPO_ID_APP4,
};

static const ui_handle_t ui_handle =
{
    .card_clock_day = {
        .id = COMPO_CARD_START,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 24+100/2,
        .w  = 0,
        .h  = 0,

        .date = {
            .id = COMPO_ID_DATE,
            .x  = 12-232/2,
            .y  = 41-136/2,
            .w  = 80+20,
            .h  = 40,
            .center = false,
            .bonddata = COMPO_BOND_DATE,
            .color = {255,255,255},
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = 12-232/2,
            .y  = 79-136/2,
            .w  = 80,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 130+100/2-232/2,
            .bg_y   = 0,
            .h_x    = 130+96/2-232/2,
            .h_y    = 0,
            .m_x    = 130+96/2-232/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 18,
            .m_w    = 65,
            .m_h    = 18,
            .bg_res = UI_BUF_I330001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I330001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I330001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    .card1 = {
        .id = COMPO_ID_CARD_1,
        .x  = 4+232/2,
        .y  = 190,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },
        .pic_kcal = {
            .idx    = 0,
            .x      = 10+18/2-232/2,
            .y      = 21+18/2-108/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_CALORIES_BIN,
        },
        .pic_km = {
            .idx    = 1,
            .x      = 10+18/2-232/2,
            .y      = 46+18/2-108/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_DIS_BIN,
        },
        .pic_step = {
            .idx    = 2,
            .x      = 10+18/2-232/2,
            .y      = 71+18/2-108/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_STEPT_BIN,
        },

        .arc_kcal = {
            .id     = COMPO_ID_ARC_KCAL,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100,
            .color  = {252,55,40},
            .bg_color = {0x7f,0x6e,0x19},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_km = {
            .id     = COMPO_ID_ARC_KM,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-12,
            .color  = {255,212,0},
            .bg_color = {0x19,0x7a,0x6f},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_step = {
            .id     = COMPO_ID_ARC_STEP,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-24,
            .color  = {0,242,214},
            .bg_color = {0x7e,0x2f,0x29},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .text_kcal = {
            .idx    = 0,
            .x      = 34-232/2,
            .y      = 21+18/2-108/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 0,
            .center = false,
            .color  = {252,55,40},
        },
        .text_km = {
            .idx    = 1,
            .x      = 34-232/2,
            .y      = 46+18/2-108/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 1,
            .center = false,
            .color  = {255,212,0},
        },
        .text_step  = {
            .idx    = 2,
            .x      = 34-232/2,
            .y      = 71+18/2-108/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 2,
            .center = false,
            .color  = {0,242,214},
        },
    },

    .card2 = {
        .id = COMPO_ID_CARD_2,
        .x  = 4+232/2,
        .y  = 190+(108+6)*1,
        .w  = 232,
        .h  = 108,
        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_sleep = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_SLEEP,
            .center = false,
            .color  = {100,95,255},
        },
        .text_hour = {
            .idx    = 1,
            .x      = 12-232/2,
            .y      = 38-108/2+6,
            .w      = 41+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_hour_unit = {
            .idx    = 2,
            .x      = 56-232/2,
            .y      = 53-108/2,
            .w      = 46,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_HOUR,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min = {
            .idx    = 3,
            .x      = 97-232/2+14,
            .y      = 38-108/2+6,
            .w      = 40+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min_unit = {
            .idx    = 4,
            .x      = 140-232/2+12,
            .y      = 53-108/2,
            .w      = 50,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MIN,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card3 = {
        .id = COMPO_ID_CARD_3,
        .x  = 4+232/2,
        .y  = 190+(108+6)*2,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_music = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MUSIC,
            .center = false,
            .color  = {255,48,96},
        },

        .pic_click_prev = {
            .idx    = COMPO_MUSIC_BTN_PREV,
            .x      = 26+28/2-232/2,
            .y      = 52+28/2-108/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_UP00_BIN,
            .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_UP01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_next = {
            .idx    = COMPO_MUSIC_BTN_NEXT,
            .x      = 178+28/2-232/2,
            .y      = 52+28/2-108/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_NEXT00_BIN,
            .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_NEXT01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_play = {
            .idx    = COMPO_MUSIC_BTN_PLAY,
            .x      = 88+56/2-232/2,
            .y      = 38+56/2-108/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY00_BIN,
            .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY01_BIN,
            .res_switch = UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN,
            .func_sta = 0,
        },
    },

    .card4 = {
        .id = COMPO_ID_CARD_4,
        .x  = 4+232/2,
        .y  = 190+(108+6)*3,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_timer = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_TIMER,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-232/2,
            .y      = 37-108/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card5 = {
        .id = COMPO_ID_CARD_5,
        .x  = 4+232/2,
        .y  = 190+(108+6)*4,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_stopwatch = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_STOP_WATCH,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-232/2,
            .y      = 37-108/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card6 = {
        .id = COMPO_ID_CARD_6,
        .x  = 4+232/2,
        .y  = 190+(108+6)*5,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_comm_use = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_COMMON_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = 0,
                .x      = 27+48/2-232/2,
                .y      = 41+48/2-108/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_SPORT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },

            [1] = {
                .idx    = 1,
                .x      = 91+48/2-232/2,
                .y      = 41+48/2-108/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_MSM_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
                .func_sta = FUNC_MESSAGE,
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            },

            [2] = {
                .idx    = 2,
                .x      = 155+48/2-232/2,
                .y      = 41+48/2-108/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I330001_FIRSTORDER_FIXED_CALL_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },

    .card7 = {
        .id = COMPO_ID_CARD_7,
        .x  = 4+232/2,
        .y  = 190+(108+6)*6+7,
        .w  = 232,
        .h  = 108,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 232,
            .h      = 108,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_last_use = {
            .idx    = 0,
            .x      = 12-232/2,
            .y      = 10-108/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_LATEST_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = COMPO_ID_APP1,
                .x      = 12+44/2-232/2,
                .y      = 43+44/2-108/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I330001_THEME_1_SOPRT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },
            [1] = {
                .idx    = COMPO_ID_APP2,
                .x      = 66+44/2-232/2,
                .y      = 43+44/2-108/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I330001_THEME_1_BRIGHTNESS_BIN,
                .res_click = 1,
                .res_switch = 1,
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
                .func_sta = FUNC_LIGHT,
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            },
            [2] = {
                .idx    = COMPO_ID_APP3,
                .x      = 120+44/2-232/2,
                .y      = 43+44/2-108/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I330001_THEME_1_TIMER_BIN,
                .res_click = 2,
                .res_switch = 2,
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
                .func_sta = FUNC_TIMER,
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            },
            [3] = {
                .idx    = COMPO_ID_APP4,
                .x      = 174+44/2-232/2,
                .y      = 43+44/2-108/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I330001_THEME_1_CALL_BIN,
                .res_click = 3,
                .res_switch = 3,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },
};

//获取卡片原始位置
static int func_clock_sub_card_get_y_info(int card_compo_id)
{
    int ret = 0;

    if (card_compo_id == ui_handle.card_clock_day.id)
    {
        ret = ui_handle.card_clock_day.y;
    }

    if (card_compo_id == ui_handle.card1.id)
    {
        ret = ui_handle.card1.y;
    }
    if (card_compo_id == ui_handle.card2.id)
    {
        ret = ui_handle.card2.y;
    }
    if (card_compo_id == ui_handle.card3.id)
    {
        ret = ui_handle.card3.y;
    }
    if (card_compo_id == ui_handle.card4.id)
    {
        ret = ui_handle.card4.y;
    }
    if (card_compo_id == ui_handle.card5.id)
    {
        ret = ui_handle.card5.y;
    }
    if (card_compo_id == ui_handle.card6.id)
    {
        ret = ui_handle.card6.y;
    }
    if (card_compo_id == ui_handle.card7.id)
    {
        ret = ui_handle.card7.y;
    }
    return ret;
}

//功能结构体
typedef struct f_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s16 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 activity_tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
//    co_timer_t music_refresh;
} f_card_t;

static void card1_updata_disp(void)
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;

//    if(tick_check_expire(f_activity->activity_tick, ))
//    {
//        f_activity->activity_tick = tick_get();
    char txt_buf[20];
    uint32_t totalStepCnt = 0;
    uint32_t Step_value = 0;
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
#endif
    Step_value = totalStepCnt;
#if UTE_MODULE_SPORT_SUPPORT
    if(Step_value>uteModuleSportGetStepsTargetCnt())
    {
        Step_value = uteModuleSportGetStepsTargetCnt();
    }
#endif
    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL);
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP);
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);

    compo_picturebox_set_visible(arc_kcal, true);
    compo_picturebox_set_visible(arc_km, true);
    compo_picturebox_set_visible(arc_step, true);


    if(f_activity->activity_state == 0)
    {
        f_activity->arc_step_value+=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value >=1800)
        {
            f_activity->activity_state =1;
        }
    }
    else if(f_activity->activity_state == 1)
    {

        f_activity->arc_step_value-=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );
#if UTE_MODULE_SPORT_SUPPORT
        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt())
        {
            f_activity->activity_state = 2;
        }
#endif

    }
    else
    {
#if UTE_MODULE_SPORT_SUPPORT
        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());
#else
        f_activity->arc_kcal_value =0;
        f_activity->arc_km_value   =0;
        f_activity->arc_step_value =0;

#endif

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
        compo_cardbox_text_set(cardbox, 2, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
        compo_cardbox_text_set(cardbox, 1, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
        compo_cardbox_text_set(cardbox, 0, txt_buf);


        if(f_activity->arc_step_value)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);
        }
        else
        {
            compo_picturebox_set_visible(arc_kcal,false);
            compo_picturebox_set_visible(arc_km,false);
            compo_picturebox_set_visible(arc_step,false);
        }
    }

//    }
}
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 card_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 card_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(card_get_latest_func(idx));
}

//创建组件
static void func_clock_sub_card_compo_create(compo_form_t *frm)
{
    char txt_buf[20];
    compo_button_t *btn=NULL;

    ///卡片7
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, ui_handle.card7.w, ui_handle.card7.h);
    compo_setid(card7, ui_handle.card7.id);

    compo_cardbox_rect_set_location(card7, ui_handle.card7.rect.idx, ui_handle.card7.rect.x, ui_handle.card7.rect.y, ui_handle.card7.rect.w, ui_handle.card7.rect.h, ui_handle.card7.rect.r);
    compo_cardbox_rect_set_color(card7, ui_handle.card7.rect.idx, make_color(ui_handle.card7.rect.color.r, ui_handle.card7.rect.color.g, ui_handle.card7.rect.color.b));

    compo_cardbox_text_set_font(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.res);
    compo_cardbox_text_set_align_center(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.center);
    widget_text_set_color(card7->text[ui_handle.card7.text_last_use.idx], make_color(ui_handle.card7.text_last_use.color.r, ui_handle.card7.text_last_use.color.g, ui_handle.card7.text_last_use.color.b));
    compo_cardbox_text_set_location(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.x, ui_handle.card7.text_last_use.y, ui_handle.card7.text_last_use.w, ui_handle.card7.text_last_use.h);
    compo_cardbox_text_set(card7, ui_handle.card7.text_last_use.idx, i18n[ui_handle.card7.text_last_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card7.pic_click)/sizeof(ui_handle.card7.pic_click[0]); i++)
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i));
        compo_button_set_location(btn,ui_handle.card7.pic_click[i].x,ui_handle.card7.pic_click[i].y, ui_handle.card7.pic_click[i].w, ui_handle.card7.pic_click[i].h);
        compo_setid(btn,ui_handle.card7.pic_click[i].idx);
    }

    ///卡片6
    compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card6.w, ui_handle.card6.h);
    compo_setid(card6, ui_handle.card6.id);

    compo_cardbox_rect_set_location(card6, ui_handle.card6.rect.idx, ui_handle.card6.rect.x, ui_handle.card6.rect.y, ui_handle.card6.rect.w, ui_handle.card6.rect.h, ui_handle.card6.rect.r);
    compo_cardbox_rect_set_color(card6, ui_handle.card6.rect.idx, make_color(ui_handle.card6.rect.color.r, ui_handle.card6.rect.color.g, ui_handle.card6.rect.color.b));

    compo_cardbox_text_set_font(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.res);
    compo_cardbox_text_set_align_center(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.center);
    widget_text_set_color(card6->text[ui_handle.card6.text_comm_use.idx], make_color(ui_handle.card6.text_comm_use.color.r, ui_handle.card6.text_comm_use.color.g, ui_handle.card6.text_comm_use.color.b));
    compo_cardbox_text_set_location(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.x, ui_handle.card6.text_comm_use.y, ui_handle.card6.text_comm_use.w, ui_handle.card6.text_comm_use.h);
    compo_cardbox_text_set(card6, ui_handle.card6.text_comm_use.idx, i18n[ui_handle.card6.text_comm_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
    {
        compo_cardbox_icon_set_location(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].x, ui_handle.card6.pic_click[i].y, ui_handle.card6.pic_click[i].w, ui_handle.card6.pic_click[i].h);
        compo_cardbox_icon_set(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].res);
    }


    ///卡片5
    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card5.w, ui_handle.card5.h);
    compo_setid(card5, ui_handle.card5.id);

    compo_cardbox_rect_set_location(card5, ui_handle.card5.rect.idx, ui_handle.card5.rect.x, ui_handle.card5.rect.y, ui_handle.card5.rect.w, ui_handle.card5.rect.h, ui_handle.card5.rect.r);
    compo_cardbox_rect_set_color(card5, ui_handle.card5.rect.idx, make_color(ui_handle.card5.rect.color.r, ui_handle.card5.rect.color.g, ui_handle.card5.rect.color.b));

//    compo_cardbox_text_set_font(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_stopwatch.idx], make_color(ui_handle.card5.text_stopwatch.color.r, ui_handle.card5.text_stopwatch.color.g, ui_handle.card5.text_stopwatch.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.x, ui_handle.card5.text_stopwatch.y, ui_handle.card5.text_stopwatch.w, ui_handle.card5.text_stopwatch.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_stopwatch.idx, i18n[ui_handle.card5.text_stopwatch.str_id]);

    compo_cardbox_text_set_font(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_time.idx], make_color(ui_handle.card5.text_time.color.r, ui_handle.card5.text_time.color.g, ui_handle.card5.text_time.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.x, ui_handle.card5.text_time.y, ui_handle.card5.text_time.w, ui_handle.card5.text_time.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_time.idx, "00:00.00");

    ///卡片4
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card4.w, ui_handle.card4.h);
    compo_setid(card4, ui_handle.card4.id);

    compo_cardbox_rect_set_location(card4, ui_handle.card4.rect.idx, ui_handle.card4.rect.x, ui_handle.card4.rect.y, ui_handle.card4.rect.w, ui_handle.card4.rect.h, ui_handle.card4.rect.r);
    compo_cardbox_rect_set_color(card4, ui_handle.card4.rect.idx, make_color(ui_handle.card4.rect.color.r, ui_handle.card4.rect.color.g, ui_handle.card4.rect.color.b));

    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_timer.idx], make_color(ui_handle.card4.text_timer.color.r, ui_handle.card4.text_timer.color.g, ui_handle.card4.text_timer.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.x, ui_handle.card4.text_timer.y, ui_handle.card4.text_timer.w, ui_handle.card4.text_timer.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_timer.idx, i18n[ui_handle.card4.text_timer.str_id]);

    compo_cardbox_text_set_font(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.res);
    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_time.idx], make_color(ui_handle.card4.text_time.color.r, ui_handle.card4.text_time.color.g, ui_handle.card4.text_time.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.x, ui_handle.card4.text_time.y, ui_handle.card4.text_time.w, ui_handle.card4.text_time.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_time.idx, "00:00:00");


    ///卡片3
    compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card3.w, ui_handle.card3.h);
    compo_setid(card3, ui_handle.card3.id);

    compo_cardbox_rect_set_location(card3, ui_handle.card3.rect.idx, ui_handle.card3.rect.x, ui_handle.card3.rect.y, ui_handle.card3.rect.w, ui_handle.card3.rect.h, ui_handle.card3.rect.r);
    compo_cardbox_rect_set_color(card3, ui_handle.card3.rect.idx, make_color(ui_handle.card3.rect.color.r, ui_handle.card3.rect.color.g, ui_handle.card3.rect.color.b));

    compo_cardbox_text_set_font(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.res);
    compo_cardbox_text_set_align_center(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.center);
    widget_text_set_color(card3->text[ui_handle.card3.text_music.idx], make_color(ui_handle.card3.text_music.color.r, ui_handle.card3.text_music.color.g, ui_handle.card3.text_music.color.b));
    compo_cardbox_text_set_location(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.x, ui_handle.card3.text_music.y, ui_handle.card3.text_music.w, ui_handle.card3.text_music.h);
    compo_cardbox_text_set(card3, ui_handle.card3.text_music.idx, i18n[ui_handle.card3.text_music.str_id]);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_prev.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_prev.x, ui_handle.card3.pic_click_prev.y, ui_handle.card3.pic_click_prev.w, ui_handle.card3.pic_click_prev.h);
    compo_setid(btn,ui_handle.card3.pic_click_prev.idx);
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res_click);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res);
    }

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_next.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_next.x, ui_handle.card3.pic_click_next.y, ui_handle.card3.pic_click_next.w, ui_handle.card3.pic_click_next.h);
    compo_setid(btn,ui_handle.card3.pic_click_next.idx);
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_next.res_click);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_next.res);
    }

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_play.res_click);
    compo_button_set_pos(btn,ui_handle.card3.pic_click_play.x, ui_handle.card3.pic_click_play.y);
    compo_setid(btn,ui_handle.card3.pic_click_play.idx);
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN : ui_handle.card3.pic_click_play.res);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_play.res_click);
    }

    ///卡片2
    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 0, 5, ui_handle.card2.w, ui_handle.card2.h);
    compo_setid(card2, ui_handle.card2.id);

    compo_cardbox_rect_set_location(card2, ui_handle.card2.rect.idx, ui_handle.card2.rect.x, ui_handle.card2.rect.y, ui_handle.card2.rect.w, ui_handle.card2.rect.h, ui_handle.card2.rect.r);
    compo_cardbox_rect_set_color(card2, ui_handle.card2.rect.idx, make_color(ui_handle.card2.rect.color.r, ui_handle.card2.rect.color.g, ui_handle.card2.rect.color.b));

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_sleep.idx], make_color(ui_handle.card2.text_sleep.color.r, ui_handle.card2.text_sleep.color.g, ui_handle.card2.text_sleep.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.x, ui_handle.card2.text_sleep.y, ui_handle.card2.text_sleep.w, ui_handle.card2.text_sleep.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_sleep.idx, i18n[ui_handle.card2.text_sleep.str_id]);

//    sleep_data->totalSleepMin =  9;
    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour.idx], make_color(ui_handle.card2.text_hour.color.r, ui_handle.card2.text_hour.color.g, ui_handle.card2.text_hour.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.x, ui_handle.card2.text_hour.y, ui_handle.card2.text_hour.w, ui_handle.card2.text_hour.h);
    memset(txt_buf,0,sizeof(txt_buf));
    if(sleep_data->totalSleepMin)   ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠小时*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour_unit.idx], make_color(ui_handle.card2.text_hour_unit.color.r, ui_handle.card2.text_hour_unit.color.g, ui_handle.card2.text_hour_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.x+6, ui_handle.card2.text_hour_unit.y, ui_handle.card2.text_hour_unit.w, ui_handle.card2.text_hour_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour_unit.idx, i18n[ui_handle.card2.text_hour_unit.str_id]);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min.idx], make_color(ui_handle.card2.text_min.color.r, ui_handle.card2.text_min.color.g, ui_handle.card2.text_min.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.x, ui_handle.card2.text_min.y, ui_handle.card2.text_min.w, ui_handle.card2.text_min.h);
    if(sleep_data->totalSleepMin)  ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠分钟*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_min.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min_unit.idx], make_color(ui_handle.card2.text_min_unit.color.r, ui_handle.card2.text_min_unit.color.g, ui_handle.card2.text_min_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.x+6, ui_handle.card2.text_min_unit.y, ui_handle.card2.text_min_unit.w, ui_handle.card2.text_min_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_min_unit.idx, i18n[ui_handle.card2.text_min_unit.str_id]);

    ///卡片1
    compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, ui_handle.card1.w, ui_handle.card1.h);
    compo_setid(card1, ui_handle.card1.id);

    compo_cardbox_rect_set_location(card1, ui_handle.card1.rect.idx, ui_handle.card1.rect.x, ui_handle.card1.rect.y, ui_handle.card1.rect.w, ui_handle.card1.rect.h, ui_handle.card1.rect.r);
    compo_cardbox_rect_set_color(card1, ui_handle.card1.rect.idx, make_color(ui_handle.card1.rect.color.r, ui_handle.card1.rect.color.g, ui_handle.card1.rect.color.b));

    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.x, ui_handle.card1.pic_kcal.y, ui_handle.card1.pic_kcal.w, ui_handle.card1.pic_kcal.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.x, ui_handle.card1.pic_km.y, ui_handle.card1.pic_km.w, ui_handle.card1.pic_km.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.x, ui_handle.card1.pic_step.y, ui_handle.card1.pic_step.w, ui_handle.card1.pic_step.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.res);

    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_kcal.idx], make_color(ui_handle.card1.text_kcal.color.r, ui_handle.card1.text_kcal.color.g, ui_handle.card1.text_kcal.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.x, ui_handle.card1.text_kcal.y, ui_handle.card1.text_kcal.w, ui_handle.card1.text_kcal.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
    compo_cardbox_text_set(card1, ui_handle.card1.text_kcal.idx, txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_km.idx], make_color(ui_handle.card1.text_km.color.r, ui_handle.card1.text_km.color.g, ui_handle.card1.text_km.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.x, ui_handle.card1.text_km.y, ui_handle.card1.text_km.w, ui_handle.card1.text_km.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d.%02d",KM/100, KM%100);///公里数据
    compo_cardbox_text_set(card1, ui_handle.card1.text_km.idx,txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_step.idx], make_color(ui_handle.card1.text_step.color.r, ui_handle.card1.text_step.color.g, ui_handle.card1.text_step.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.x, ui_handle.card1.text_step.y, ui_handle.card1.text_step.w, ui_handle.card1.text_step.h);
    snprintf(txt_buf,sizeof(txt_buf),"%ld",totalStepCnt);
    compo_cardbox_text_set(card1, ui_handle.card1.text_step.idx, txt_buf);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I330001_FIRSTORDER_ACTIVITY_BG_BIN);
    widget_page_t * widget_page = widget_page_create(card1->page);///创建按键页面
    widget_set_location(widget_page,52,0,pic_bg_area.wid,pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_BG_BIN);//彩虹背景
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_RED_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,50,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KCAL);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_YELLOW_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,50,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KM);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_BLUE_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,50,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_STEP);


    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I330001_FIRSTORDER_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,52,pic_bg_area.hei-gui_image_get_size(UI_BUF_I330001_FIRSTORDER_ACTIVITY_YUANJIAO_BIN).hei );

    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res);
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg);

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res);
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2);
    compo_datetime_set_start_angle(clock_h, 900);
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata);

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res);
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min);
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2);
    compo_datetime_set_start_angle(clock_m, 900);
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata);

    compo_textbox_t* date = compo_textbox_create(frm, 5);                                 //日期
    compo_setid(date, ui_handle.card_clock_day.date.id);
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res);
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center);
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b));
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata);

    compo_textbox_t* week = compo_textbox_create(frm, 20);                                 //日期
    compo_setid(week, ui_handle.card_clock_day.week.id);
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res);
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center);
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b));
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata);

//    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 6, 36, 6);
//    compo_shape_set_color(shape, make_color(0X4C,0X4C,0X4C));
//    compo_shape_set_radius(shape, 35);

    ab_free(sleep_data);

}

//更新组件位置和大小
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL)
    {
        //获取初始值
        switch (compo->id)
        {
            case COMPO_ID_CLOCK_BG:
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG)
                {
                    printf("COMPO_ID_CLOCK_BG ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.bg_w;
                h = ui_handle.card_clock_day.clock.bg_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_H:
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H)
                {
                    printf("COMPO_ID_CLOCK_H ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.h_w;
                h = ui_handle.card_clock_day.clock.h_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_M:
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M)
                {
                    printf("COMPO_ID_CLOCK_M ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.m_w;
                h = ui_handle.card_clock_day.clock.m_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_DATE:
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE)
                {
                    printf("COMPO_ID_DATE ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.date.w;
                h = ui_handle.card_clock_day.date.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_WEEKDAY:
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY)
                {
                    printf("COMPO_ID_WEEKDAY ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.week.w;
                h = ui_handle.card_clock_day.week.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CARD_1:
                if (ui_handle.card1.id != COMPO_ID_CARD_1)
                {
                    printf("COMPO_ID_CARD_1 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card1.x;
                y = card_y = ui_handle.card1.y;
                w = ui_handle.card1.w;
                h = ui_handle.card1.h;
                card_id = ui_handle.card1.id;
                break;

            case COMPO_ID_CARD_2:
                if (ui_handle.card2.id != COMPO_ID_CARD_2)
                {
                    printf("COMPO_ID_CARD_2 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card2.x;
                y = card_y = ui_handle.card2.y;
                w = ui_handle.card2.w;
                h = ui_handle.card2.h;
                card_id = ui_handle.card2.id;
                break;

            case COMPO_ID_CARD_3:
                if (ui_handle.card3.id != COMPO_ID_CARD_3)
                {
                    printf("COMPO_ID_CARD_3 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card3.x;
                y = card_y = ui_handle.card3.y;
                w = ui_handle.card3.w;
                h = ui_handle.card3.h;
                card_id = ui_handle.card3.id;
                break;

            case COMPO_ID_CARD_4:
                if (ui_handle.card4.id != COMPO_ID_CARD_4)
                {
                    printf("COMPO_ID_CARD_4 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card4.x;
                y = card_y = ui_handle.card4.y;
                w = ui_handle.card4.w;
                h = ui_handle.card4.h;
                card_id = ui_handle.card4.id;
                break;

            case COMPO_ID_CARD_5:
                if (ui_handle.card5.id != COMPO_ID_CARD_5)
                {
                    printf("COMPO_ID_CARD_5 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card5.x;
                y = card_y = ui_handle.card5.y;
                w = ui_handle.card5.w;
                h = ui_handle.card5.h;
                card_id = ui_handle.card5.id;
                break;


            case COMPO_ID_CARD_6:
                if (ui_handle.card6.id != COMPO_ID_CARD_6)
                {
                    printf("COMPO_ID_CARD_6 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card6.x;
                y = card_y = ui_handle.card6.y;
                w = ui_handle.card6.w;
                h = ui_handle.card6.h;
                card_id = ui_handle.card6.id;
                break;


            case COMPO_ID_CARD_7:
                if (ui_handle.card7.id != COMPO_ID_CARD_7)
                {
                    printf("COMPO_ID_CARD_7 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card7.x;
                y = card_y = ui_handle.card7.y;
                w = ui_handle.card7.w;
                h = ui_handle.card7.h;
                card_id = ui_handle.card7.id;
                break;

            default:
                compo = compo_get_next(compo);
                continue;
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;
            f_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }

//            if (card_id > COMPO_CARD_END-1)      //被覆盖的卡片不可见，降低element溢出概率
//            {
//                rect_t rect = compo_cardbox_get_location((compo_cardbox_t *)compo_getobj_byid(COMPO_ID_CARD_SPORT_COMPASS + (card_id - CARD_ID_SPORT_COMPASS) - 1));
//                if (y <= rect.y)
//                {
//                    compo_cardbox_set_visible(cardbox, false);
//                }
//                else
//                {
//                    compo_cardbox_set_visible(cardbox, true);
//                }
//            }

        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_pos(pic, x, y);
            // compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set_location(txt, x, y, w, h);
            if (unvisible)
            {
                compo_textbox_set_visible(txt, false);
            }
            else
            {
                compo_textbox_set_visible(txt, true);
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;
            compo_datetime_set_pos(dt, x, y);
        }

        compo = compo_get_next(compo);          //遍历组件
    }
}

//创建上拉卡片菜单（文件内调用）
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
//    compo_shape_set_alpha(masklayer, 160);
    //创建所有组件
    func_clock_sub_card_compo_create(frm);
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);

    return frm;
}

//创建上拉卡片菜单（供外部调用）
compo_form_t *func_clock_sub_card_form_create(void)
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX);
}

//获取点击的卡片组件id
static u16 func_clock_sub_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_BTN_COUNT; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_CARD_START + 1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//表盘上拉卡片菜单点击处理
static void func_clock_sub_card_click_handler(void)
{
    compo_cardbox_t *cardbox = NULL;
//    rect_t rect;
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);
    printf("click compo_id:%d\n", compo_id);
    int id = compo_get_button_id();

    switch(compo_id)
    {
        case COMPO_ID_CARD_1 :
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            func_jump = FUNC_ACTIVITY;
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            break;
        case COMPO_ID_CARD_2 :
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
            func_jump = FUNC_SLEEP;
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
            break;
        case COMPO_ID_CARD_3:
            switch(id)
            {
                case COMPO_MUSIC_BTN_PREV:
                {
                    uteModuleMusicCtrl(false,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
                    compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res);
                }
                break;
                case COMPO_MUSIC_BTN_PLAY:
                {
                    uteModuleMusicCtrlPaused(false);
                    if(ble_is_connect())
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
                    }
//                    if(bt_cb.music_playing)
//                    {
//                        compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY);
//                        compo_button_set_bgimg(btn, ui_handle.card3.pic_click_play.res_click);
//                    }
                }
                break;
                case COMPO_MUSIC_BTN_NEXT:
                {
                    uteModuleMusicCtrl(true,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
                    compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res);
                }
                break;
                default:
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    func_jump = FUNC_BT;
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    break;
            }
            break;
        case COMPO_ID_CARD_4 :
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
            func_jump = FUNC_TIMER;
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            break;
        case COMPO_ID_CARD_5 :
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            func_jump = FUNC_STOPWATCH;
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            break;
        case COMPO_ID_CARD_6 :
            cardbox = compo_getobj_byid(COMPO_ID_CARD_6);
            for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
            {
                rect_t rect = compo_cardbox_get_icon_absolute(cardbox, ui_handle.card6.pic_click[i].idx); //上一首
                if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
                {
//                    printf("rect = [%d,%d,%d,%d]\n", rect.x, rect.y, rect.wid, rect.hei);
                    func_cb.sta = ui_handle.card6.pic_click[i].func_sta;
                }
            }
            break;
        case COMPO_ID_CARD_7 :
            printf("id : %d\n", id);
            if (id >= COMPO_ID_APP1)
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1);
            }
            break;
        default:
            break;
    }
    if(func_jump != FUNC_NULL)
    {
        printf("func_jump=%d\n", func_jump);
        func_cb.sta = func_jump;  //直接跳转
    }
}

//根据偏移位置把最近的卡片对齐到屏幕中央
static s16 func_clock_sub_card_align_y(s16 ofs_y)
{
    u8 i, nearest_id = 0;
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    u16 dc;
    if (ofs_y >= DRAG_Y_MAX)
    {
        return DRAG_Y_MAX;  //回弹效果
    }
    else if (ofs_y <= DRAG_Y_MIN)
    {
        return DRAG_Y_MIN;
    }
//    for (i=0; i<CARD_COUNT; i++)
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y);
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y);
        if (dc < dc_min)
        {
            nearest_id = i;
            dc_min = dc;
        }
    }
//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id));
}

//根据卡片序号对齐页面偏移位置到屏幕中央
static s16 func_clock_sub_card_align_by_idx(s8 card_id)
{
//    if (card_id <= CARD_ID_CLOCK)
    if (card_id <= COMPO_CARD_START)
    {
        return DRAG_Y_MAX;
    }
//    else if (card_id > (CARD_COUNT - 1))
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1))
    {
        return DRAG_Y_MIN;
    }
    else
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id));
    }
}

//限制滑动偏移位置
static s16 func_clock_sub_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//设置页面偏移位置
static void func_clock_sub_card_set_offs(s16 ofs_y)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    f_card->ofs_y = ofs_y;
    func_clock_sub_card_compo_update(f_card->ofs_y, false);
}

//时钟表盘上拉菜单数据更新----------------------------------------->todo
static void func_clock_sub_card_data_update(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    compo_cardbox_t *cardbox = NULL;
    char txt_buf[20];

    if(f_card->flag_drag == 0)
    {
        card1_updata_disp();
    }

//    if(f_activity->activity_state != 2) return;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec));
    cardbox = compo_getobj_byid(ui_handle.card4.id);
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf);

    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    u16 msec = sys_cb.stopwatch_total_msec % 1000;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min  = 99;
        sec  = 59;
        msec = 999;
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d:%02d.%02d",min,sec,msec/10);
    cardbox = compo_getobj_byid(ui_handle.card5.id);
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf);
}

//下拉返回表盘
static void func_clock_sub_card_switch_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y);
    func_cb.frm_main = frm;
    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
}

void music_data_refresh(void)
{
    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); //播放/暂停
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN : ui_handle.card3.pic_click_play.res);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_play.res_click);
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define CARD_SCALE_START_POS            250//265                                                                     //卡片开始缩小位置
#define CARD_SCALE_STOP_POS             360//320                                                                     //卡片停止缩小位置
#define CARD_SCALE_STOP_Y               (CARD_SCALE_STOP_POS + 200+90 )                                         //卡片停止缩小实际位置（越大缩小越慢，可见叠加越多）
#define CARD_WIDTH_MIN                  100//200                                                                     //卡片缩小的最小宽度
#define CARD_UNVISIBLE_POS_TOP          (-100)//(-160)                                                                  //卡片超出屏幕顶部某位置后设为不可见
#define CARD_UNVISIBLE_POS_BOTTOM       (CARD_SCALE_STOP_Y + 200)                                               //卡片缩到最小后实际位置大于某值设为不可见
#define CARD_BG_GRAY_START              41                                                                      //卡片开始缩小位置处背景的灰度
#define CARD_BG_GRAY_STOP               10                                                                      //卡片停止缩小位置处背景的灰度
#define GRAY_LV(x)                      make_color(x, x, x)                                                     //生成灰度颜色值（0:黑 255:白）

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
//#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - card_y_info[CARD_COUNT - 1] + 50)                     //回弹到底部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_END-1) + 50)                     //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + 30)                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - 78)                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 5   //最近任务不足5个时默认值补上
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};

///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  232//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_timer;
        struct text_t text_time;
    } card4;

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_stopwatch;
        struct text_t text_time;
    } card5;

    struct card6_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_comm_use;
        struct pic_click_t pic_click[3];
    } card6;

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[4];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,
    COMPO_ID_CLOCK_H,
    COMPO_ID_CLOCK_M,
    COMPO_ID_DATE,
    COMPO_ID_WEEKDAY,

    COMPO_CARD_START,
    COMPO_ID_CARD_1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    COMPO_ID_CARD_5,
    COMPO_ID_CARD_6,
    COMPO_ID_CARD_7,
    COMPO_CARD_END,

    COMPO_ID_ARC_KCAL,
    COMPO_ID_ARC_KM,
    COMPO_ID_ARC_STEP,

    COMPO_MUSIC_BTN_PREV,
    COMPO_MUSIC_BTN_NEXT,
    COMPO_MUSIC_BTN_PLAY,

    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
    COMPO_ID_APP4,
};

static const ui_handle_t ui_handle =
{
    .card_clock_day = {
        .id = COMPO_CARD_START,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 24+100/2,
        .w  = 0,
        .h  = 0,

        .date = {
            .id = COMPO_ID_DATE,
            .x  = 12-232/2,
            .y  = 41-136/2,
            .w  = 80+20,
            .h  = 40,
            .center = false,
            .bonddata = COMPO_BOND_DATE,
            .color = {255,255,255},
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = 12-232/2+20,
            .y  = 69-136/2,
            .w  = 60,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 130+100/2-232/2,
            .bg_y   = 0,
            .h_x    = 130+96/2-232/2,
            .h_y    = 0,
            .m_x    = 130+96/2-232/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 18,
            .m_w    = 65,
            .m_h    = 18,
            .bg_res = UI_BUF_I332001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I332001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I332001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    .card1 = {
        .id = COMPO_ID_CARD_1,
        .x  = 180,
        .y  = 228,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },
        .pic_kcal = {
            .idx    = 0,
            .x      = -123,
            .y      = -33,
            .w      = 26,
            .h      = 26,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_CALORIES_BIN,
        },
        .pic_km = {
            .idx    = 1,
            .x      = -123,
            .y      = 0,
            .w      = 26,
            .h      = 26,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_DIS_BIN,
        },
        .pic_step = {
            .idx    = 2,
            .x      = -123,
            .y      = 33,
            .w      = 26,
            .h      = 26,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_STEPT_BIN,
        },

        .arc_kcal = {
            .id     = COMPO_ID_ARC_KCAL,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100,
            .color  = {252,55,40},
            .bg_color = {0x7f,0x6e,0x19},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_km = {
            .id     = COMPO_ID_ARC_KM,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-12,
            .color  = {255,212,0},
            .bg_color = {0x19,0x7a,0x6f},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_step = {
            .id     = COMPO_ID_ARC_STEP,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-24,
            .color  = {0,242,214},
            .bg_color = {0x7e,0x2f,0x29},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .text_kcal = {
            .idx    = 0,
            .x      = -123+21,
            .y      = -(33+15),
            .w      = 65,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_24_BIN,
            .str_id = 0,
            .center = false,
            .color  = {252,55,40},
        },
        .text_km = {
            .idx    = 1,
            .x      = -123+21,
            .y      = -15,
            .w      = 65,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_24_BIN,
            .str_id = 1,
            .center = false,
            .color  = {255,212,0},
        },
        .text_step  = {
            .idx    = 2,
            .x      = -123+21,
            .y      = (33-15),
            .w      = 65,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_24_BIN,
            .str_id = 2,
            .center = false,
            .color  = {0,242,214},
        },
    },

    .card2 = {
        .id = COMPO_ID_CARD_2,
        .x  = 180,
        .y  = 76+316,
        .w  = 324,
        .h  = 152,
        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_sleep = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_SLEEP,
            .center = false,
            .color  = {100,95,255},
        },
        .text_hour = {
            .idx    = 1,
            .x      = -120,
            .y      = -15,
            .w      = 60,
            .h      = 58,
            .res    = UI_BUF_0FONT_FONT_NUM_48_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_hour_unit = {
            .idx    = 2,
            .x      = -65,
            .y      = 15,
            .w      = 35,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_HOUR,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min = {
            .idx    = 3,
            .x      = -20,
            .y      = -15,
            .w      = 60,
            .h      = 58,
            .res    = UI_BUF_0FONT_FONT_NUM_48_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min_unit = {
            .idx    = 4,
            .x      = 35,
            .y      = 15,
            .w      = 35,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MIN,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card3 = {
        .id = COMPO_ID_CARD_3,
        .x  = 180,
        .y  = 480+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_music = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MUSIC,
            .center = false,
            .color  = {255,48,96},
        },

        .pic_click_prev = {
            .idx    = COMPO_MUSIC_BTN_PREV,
            .x      = -95,
            .y      = 18,
            .w      = 36,
            .h      = 36,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_UP00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_UP01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_next = {
            .idx    = COMPO_MUSIC_BTN_NEXT,
            .x      = 90,
            .y      = 18,
            .w      = 36,
            .h      = 36,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_NEXT00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_NEXT01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_play = {
            .idx    = COMPO_MUSIC_BTN_PLAY,
            .x      = -5,
            .y      = 18,
            .w      = 70,
            .h      = 70,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_PLAY00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_PLAY01_BIN,
            .res_switch = UI_BUF_I332001_FIRSTORDER_MUSIC_PAUSED_BIN,
            .func_sta = 0,
        },
    },

    .card4 = {
        .id = COMPO_ID_CARD_4,
        .x  = 180,
        .y  = 644+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_timer = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_TIMER,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 23-324/2,
            .y      = -15,
            .w      = 180,
            .h      = 58,
            .res    = UI_BUF_0FONT_FONT_NUM_48_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card5 = {
        .id = COMPO_ID_CARD_5,
        .x  = 180,
        .y  = 808+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_stopwatch = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_STOP_WATCH,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 23-324/2,
            .y      = -15,
            .w      = 180,
            .h      = 58,
            .res    = UI_BUF_0FONT_FONT_NUM_48_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card6 = {
        .id = COMPO_ID_CARD_6,
        .x  = 180,
        .y  = 972+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_comm_use = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_COMMON_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = 0,
                .x      = -104,
                .y      = 18,
                .w      = 70,
                .h      = 70,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_SPORT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },

            [1] = {
                .idx    = 1,
                .x      = 0,
                .y      = 18,
                .w      = 70,
                .h      = 70,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_MSM_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
                .func_sta = FUNC_MESSAGE,
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            },

            [2] = {
                .idx    = 2,
                .x      = 104,
                .y      = 18,
                .w      = 70,
                .h      = 70,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_CALL_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },

    .card7 = {
        .id = COMPO_ID_CARD_7,
        .x  = 180,
        .y  = 1136+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_last_use = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_LATEST_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = COMPO_ID_APP1,
                .x      = -115,
                .y      = 18,
                .w      = 64,
                .h      = 64,
                .res    = UI_BUF_I332001_THEME_ICON1_SOPRT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },
            [1] = {
                .idx    = COMPO_ID_APP2,
                .x      = -115+74,
                .y      = 18,
                .w      = 64,
                .h      = 64,
                .res    = UI_BUF_I332001_THEME_ICON1_BRIGHTNESS_BIN,
                .res_click = 1,
                .res_switch = 1,
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
                .func_sta = FUNC_LIGHT,
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            },
            [2] = {
                .idx    = COMPO_ID_APP3,
                .x      = -115+74*2,
                .y      = 18,
                .w      = 64,
                .h      = 64,
                .res    = UI_BUF_I332001_THEME_ICON1_TIMER_BIN,
                .res_click = 2,
                .res_switch = 2,
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
                .func_sta = FUNC_TIMER,
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            },
            [3] = {
                .idx    = COMPO_ID_APP4,
                .x      = -115+74*3,
                .y      = 18,
                .w      = 64,
                .h      = 64,
                .res    = UI_BUF_I332001_THEME_ICON1_CALL_BIN,
                .res_click = 3,
                .res_switch = 3,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },
};

//获取卡片原始位置
static int func_clock_sub_card_get_y_info(int card_compo_id)
{
    int ret = 0;

    if (card_compo_id == ui_handle.card_clock_day.id)
    {
        ret = ui_handle.card_clock_day.y;
    }

    if (card_compo_id == ui_handle.card1.id)
    {
        ret = ui_handle.card1.y;
    }
    if (card_compo_id == ui_handle.card2.id)
    {
        ret = ui_handle.card2.y;
    }
    if (card_compo_id == ui_handle.card3.id)
    {
        ret = ui_handle.card3.y;
    }
    if (card_compo_id == ui_handle.card4.id)
    {
        ret = ui_handle.card4.y;
    }
    if (card_compo_id == ui_handle.card5.id)
    {
        ret = ui_handle.card5.y;
    }
    if (card_compo_id == ui_handle.card6.id)
    {
        ret = ui_handle.card6.y;
    }
    if (card_compo_id == ui_handle.card7.id)
    {
        ret = ui_handle.card7.y;
    }
    return ret;
}

//功能结构体
typedef struct f_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s16 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 activity_tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
//    co_timer_t music_refresh;
} f_card_t;

static void card1_updata_disp(void)
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;

//    if(tick_check_expire(f_activity->activity_tick, ))
//    {
//        f_activity->activity_tick = tick_get();
    char txt_buf[20];
    uint32_t totalStepCnt = 0;
    uint32_t Step_value = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    Step_value = totalStepCnt;
    if(Step_value>uteModuleSportGetStepsTargetCnt())
    {
        Step_value = uteModuleSportGetStepsTargetCnt();
    }

    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL);
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP);
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);

    compo_picturebox_set_visible(arc_kcal, true);
    compo_picturebox_set_visible(arc_km, true);
    compo_picturebox_set_visible(arc_step, true);


    if(f_activity->activity_state == 0)
    {
        f_activity->arc_step_value+=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value >=1800)
        {
            f_activity->activity_state =1;
        }
    }
    else if(f_activity->activity_state == 1)
    {

        f_activity->arc_step_value-=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt())
        {
            f_activity->activity_state = 2;
        }

    }
    else
    {

        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
        compo_cardbox_text_set(cardbox, 2, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
        compo_cardbox_text_set(cardbox, 1, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
        compo_cardbox_text_set(cardbox, 0, txt_buf);


        if(f_activity->arc_step_value)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);
        }
        else
        {
            compo_picturebox_set_visible(arc_kcal,false);
            compo_picturebox_set_visible(arc_km,false);
            compo_picturebox_set_visible(arc_step,false);
        }
    }

//    }
}
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 card_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 card_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(card_get_latest_func(idx));
}

//创建组件
static void func_clock_sub_card_compo_create(compo_form_t *frm)
{
    char txt_buf[20];
    compo_button_t *btn=NULL;

    ///卡片7
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, ui_handle.card7.w, ui_handle.card7.h);
    compo_setid(card7, ui_handle.card7.id);

    compo_cardbox_rect_set_location(card7, ui_handle.card7.rect.idx, ui_handle.card7.rect.x, ui_handle.card7.rect.y, ui_handle.card7.rect.w, ui_handle.card7.rect.h, ui_handle.card7.rect.r);
    compo_cardbox_rect_set_color(card7, ui_handle.card7.rect.idx, make_color(ui_handle.card7.rect.color.r, ui_handle.card7.rect.color.g, ui_handle.card7.rect.color.b));

    compo_cardbox_text_set_font(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.res);
    compo_cardbox_text_set_align_center(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.center);
    widget_text_set_color(card7->text[ui_handle.card7.text_last_use.idx], make_color(ui_handle.card7.text_last_use.color.r, ui_handle.card7.text_last_use.color.g, ui_handle.card7.text_last_use.color.b));
    compo_cardbox_text_set_location(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.x, ui_handle.card7.text_last_use.y, ui_handle.card7.text_last_use.w, ui_handle.card7.text_last_use.h);
    compo_cardbox_text_set(card7, ui_handle.card7.text_last_use.idx, i18n[ui_handle.card7.text_last_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card7.pic_click)/sizeof(ui_handle.card7.pic_click[0]); i++)
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i));
        compo_button_set_location(btn,ui_handle.card7.pic_click[i].x,ui_handle.card7.pic_click[i].y, ui_handle.card7.pic_click[i].w, ui_handle.card7.pic_click[i].h);
        compo_setid(btn,ui_handle.card7.pic_click[i].idx);
    }

    ///卡片6
    compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card6.w, ui_handle.card6.h);
    compo_setid(card6, ui_handle.card6.id);

    compo_cardbox_rect_set_location(card6, ui_handle.card6.rect.idx, ui_handle.card6.rect.x, ui_handle.card6.rect.y, ui_handle.card6.rect.w, ui_handle.card6.rect.h, ui_handle.card6.rect.r);
    compo_cardbox_rect_set_color(card6, ui_handle.card6.rect.idx, make_color(ui_handle.card6.rect.color.r, ui_handle.card6.rect.color.g, ui_handle.card6.rect.color.b));

    compo_cardbox_text_set_font(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.res);
    compo_cardbox_text_set_align_center(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.center);
    widget_text_set_color(card6->text[ui_handle.card6.text_comm_use.idx], make_color(ui_handle.card6.text_comm_use.color.r, ui_handle.card6.text_comm_use.color.g, ui_handle.card6.text_comm_use.color.b));
    compo_cardbox_text_set_location(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.x, ui_handle.card6.text_comm_use.y, ui_handle.card6.text_comm_use.w, ui_handle.card6.text_comm_use.h);
    compo_cardbox_text_set(card6, ui_handle.card6.text_comm_use.idx, i18n[ui_handle.card6.text_comm_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
    {
        compo_cardbox_icon_set_location(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].x, ui_handle.card6.pic_click[i].y, ui_handle.card6.pic_click[i].w, ui_handle.card6.pic_click[i].h);
        compo_cardbox_icon_set(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].res);
    }


    ///卡片5
    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card5.w, ui_handle.card5.h);
    compo_setid(card5, ui_handle.card5.id);

    compo_cardbox_rect_set_location(card5, ui_handle.card5.rect.idx, ui_handle.card5.rect.x, ui_handle.card5.rect.y, ui_handle.card5.rect.w, ui_handle.card5.rect.h, ui_handle.card5.rect.r);
    compo_cardbox_rect_set_color(card5, ui_handle.card5.rect.idx, make_color(ui_handle.card5.rect.color.r, ui_handle.card5.rect.color.g, ui_handle.card5.rect.color.b));

//    compo_cardbox_text_set_font(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_stopwatch.idx], make_color(ui_handle.card5.text_stopwatch.color.r, ui_handle.card5.text_stopwatch.color.g, ui_handle.card5.text_stopwatch.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.x, ui_handle.card5.text_stopwatch.y, ui_handle.card5.text_stopwatch.w, ui_handle.card5.text_stopwatch.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_stopwatch.idx, i18n[ui_handle.card5.text_stopwatch.str_id]);

    compo_cardbox_text_set_font(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_time.idx], make_color(ui_handle.card5.text_time.color.r, ui_handle.card5.text_time.color.g, ui_handle.card5.text_time.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.x, ui_handle.card5.text_time.y, ui_handle.card5.text_time.w, ui_handle.card5.text_time.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_time.idx, "00:00.00");

    ///卡片4
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card4.w, ui_handle.card4.h);
    compo_setid(card4, ui_handle.card4.id);

    compo_cardbox_rect_set_location(card4, ui_handle.card4.rect.idx, ui_handle.card4.rect.x, ui_handle.card4.rect.y, ui_handle.card4.rect.w, ui_handle.card4.rect.h, ui_handle.card4.rect.r);
    compo_cardbox_rect_set_color(card4, ui_handle.card4.rect.idx, make_color(ui_handle.card4.rect.color.r, ui_handle.card4.rect.color.g, ui_handle.card4.rect.color.b));

    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_timer.idx], make_color(ui_handle.card4.text_timer.color.r, ui_handle.card4.text_timer.color.g, ui_handle.card4.text_timer.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.x, ui_handle.card4.text_timer.y, ui_handle.card4.text_timer.w, ui_handle.card4.text_timer.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_timer.idx, i18n[ui_handle.card4.text_timer.str_id]);

    compo_cardbox_text_set_font(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.res);
    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_time.idx], make_color(ui_handle.card4.text_time.color.r, ui_handle.card4.text_time.color.g, ui_handle.card4.text_time.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.x, ui_handle.card4.text_time.y, ui_handle.card4.text_time.w, ui_handle.card4.text_time.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_time.idx, "00:00:00");


    ///卡片3
    compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card3.w, ui_handle.card3.h);
    compo_setid(card3, ui_handle.card3.id);

    compo_cardbox_rect_set_location(card3, ui_handle.card3.rect.idx, ui_handle.card3.rect.x, ui_handle.card3.rect.y, ui_handle.card3.rect.w, ui_handle.card3.rect.h, ui_handle.card3.rect.r);
    compo_cardbox_rect_set_color(card3, ui_handle.card3.rect.idx, make_color(ui_handle.card3.rect.color.r, ui_handle.card3.rect.color.g, ui_handle.card3.rect.color.b));

    compo_cardbox_text_set_font(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.res);
    compo_cardbox_text_set_align_center(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.center);
    widget_text_set_color(card3->text[ui_handle.card3.text_music.idx], make_color(ui_handle.card3.text_music.color.r, ui_handle.card3.text_music.color.g, ui_handle.card3.text_music.color.b));
    compo_cardbox_text_set_location(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.x, ui_handle.card3.text_music.y, ui_handle.card3.text_music.w, ui_handle.card3.text_music.h);
    compo_cardbox_text_set(card3, ui_handle.card3.text_music.idx, i18n[ui_handle.card3.text_music.str_id]);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_prev.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_prev.x, ui_handle.card3.pic_click_prev.y, ui_handle.card3.pic_click_prev.w, ui_handle.card3.pic_click_prev.h);
    compo_setid(btn,ui_handle.card3.pic_click_prev.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_next.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_next.x, ui_handle.card3.pic_click_next.y, ui_handle.card3.pic_click_next.w, ui_handle.card3.pic_click_next.h);
    compo_setid(btn,ui_handle.card3.pic_click_next.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_play.res_click);
    compo_button_set_pos(btn,ui_handle.card3.pic_click_play.x, ui_handle.card3.pic_click_play.y);
    compo_setid(btn,ui_handle.card3.pic_click_play.idx);

    ///卡片2
    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 0, 5, ui_handle.card2.w, ui_handle.card2.h);
    compo_setid(card2, ui_handle.card2.id);

    compo_cardbox_rect_set_location(card2, ui_handle.card2.rect.idx, ui_handle.card2.rect.x, ui_handle.card2.rect.y, ui_handle.card2.rect.w, ui_handle.card2.rect.h, ui_handle.card2.rect.r);
    compo_cardbox_rect_set_color(card2, ui_handle.card2.rect.idx, make_color(ui_handle.card2.rect.color.r, ui_handle.card2.rect.color.g, ui_handle.card2.rect.color.b));

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_sleep.idx], make_color(ui_handle.card2.text_sleep.color.r, ui_handle.card2.text_sleep.color.g, ui_handle.card2.text_sleep.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.x, ui_handle.card2.text_sleep.y, ui_handle.card2.text_sleep.w, ui_handle.card2.text_sleep.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_sleep.idx, i18n[ui_handle.card2.text_sleep.str_id]);

    // sleep_data->totalSleepMin =  412;
    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour.idx], make_color(ui_handle.card2.text_hour.color.r, ui_handle.card2.text_hour.color.g, ui_handle.card2.text_hour.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.x, ui_handle.card2.text_hour.y, ui_handle.card2.text_hour.w, ui_handle.card2.text_hour.h);
    memset(txt_buf,0,sizeof(txt_buf));
    if(sleep_data->totalSleepMin)   ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠小时*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour_unit.idx], make_color(ui_handle.card2.text_hour_unit.color.r, ui_handle.card2.text_hour_unit.color.g, ui_handle.card2.text_hour_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.x+6, ui_handle.card2.text_hour_unit.y, ui_handle.card2.text_hour_unit.w, ui_handle.card2.text_hour_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour_unit.idx, i18n[ui_handle.card2.text_hour_unit.str_id]);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min.idx], make_color(ui_handle.card2.text_min.color.r, ui_handle.card2.text_min.color.g, ui_handle.card2.text_min.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.x, ui_handle.card2.text_min.y, ui_handle.card2.text_min.w, ui_handle.card2.text_min.h);
    if(sleep_data->totalSleepMin)  ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠分钟*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_min.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min_unit.idx], make_color(ui_handle.card2.text_min_unit.color.r, ui_handle.card2.text_min_unit.color.g, ui_handle.card2.text_min_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.x+6, ui_handle.card2.text_min_unit.y, ui_handle.card2.text_min_unit.w, ui_handle.card2.text_min_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_min_unit.idx, i18n[ui_handle.card2.text_min_unit.str_id]);

    ///卡片1
    compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, ui_handle.card1.w, ui_handle.card1.h);
    compo_setid(card1, ui_handle.card1.id);

    compo_cardbox_rect_set_location(card1, ui_handle.card1.rect.idx, ui_handle.card1.rect.x, ui_handle.card1.rect.y, ui_handle.card1.rect.w, ui_handle.card1.rect.h, ui_handle.card1.rect.r);
    compo_cardbox_rect_set_color(card1, ui_handle.card1.rect.idx, make_color(ui_handle.card1.rect.color.r, ui_handle.card1.rect.color.g, ui_handle.card1.rect.color.b));

    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.x, ui_handle.card1.pic_kcal.y, ui_handle.card1.pic_kcal.w, ui_handle.card1.pic_kcal.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.x, ui_handle.card1.pic_km.y, ui_handle.card1.pic_km.w, ui_handle.card1.pic_km.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.x, ui_handle.card1.pic_step.y, ui_handle.card1.pic_step.w, ui_handle.card1.pic_step.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.res);

#if UTE_MODULE_SPORT_SUPPORT
    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
#else
    uint16_t KM = 0;
#endif
    uint32_t totalStepCnt = 0;
#if UTE_MODULE_SPORT_SUPPORT
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
#endif
    compo_cardbox_text_set_font(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_kcal.idx], make_color(ui_handle.card1.text_kcal.color.r, ui_handle.card1.text_kcal.color.g, ui_handle.card1.text_kcal.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.x, ui_handle.card1.text_kcal.y, ui_handle.card1.text_kcal.w, ui_handle.card1.text_kcal.h);
    memset(txt_buf,0,sizeof(txt_buf));
#if UTE_MODULE_SPORT_SUPPORT
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
#else
    snprintf(txt_buf,sizeof(txt_buf),"%d",0);
#endif
    compo_cardbox_text_set(card1, ui_handle.card1.text_kcal.idx, txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_km.idx], make_color(ui_handle.card1.text_km.color.r, ui_handle.card1.text_km.color.g, ui_handle.card1.text_km.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.x, ui_handle.card1.text_km.y, ui_handle.card1.text_km.w, ui_handle.card1.text_km.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",KM/100, KM%100);///公里数据
    compo_cardbox_text_set(card1, ui_handle.card1.text_km.idx,txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_step.idx], make_color(ui_handle.card1.text_step.color.r, ui_handle.card1.text_step.color.g, ui_handle.card1.text_step.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.x, ui_handle.card1.text_step.y, ui_handle.card1.text_step.w, ui_handle.card1.text_step.h);
    snprintf(txt_buf,sizeof(txt_buf),"%ld",totalStepCnt);
    compo_cardbox_text_set(card1, ui_handle.card1.text_step.idx, txt_buf);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I332001_FIRSTORDER_ACTIVITY_BG_BIN);
    widget_page_t * widget_page = widget_page_create(card1->page);///创建按键页面
    widget_set_location(widget_page,52,0,pic_bg_area.wid,pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_BG_BIN);//彩虹背景
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_RED_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KCAL);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_YELLOW_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KM);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_BLUE_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_STEP);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I332001_FIRSTORDER_ACTIVITY_YUANJIAO_BIN).hei+1 );

    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res);
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg);

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res);
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2);
    compo_datetime_set_start_angle(clock_h, 900);
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata);

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res);
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min);
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2);
    compo_datetime_set_start_angle(clock_m, 900);
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata);

    compo_textbox_t* date = compo_textbox_create(frm, 5);                                 //日期
    compo_setid(date, ui_handle.card_clock_day.date.id);
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res);
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center);
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b));
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata);

    compo_textbox_t* week = compo_textbox_create(frm, 20);                                 //日期
    compo_setid(week, ui_handle.card_clock_day.week.id);
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res);
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center);
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b));
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 13, 54, 6);
    compo_shape_set_color(shape, make_color(76,76,76));
    compo_shape_set_radius(shape, 35);

    ab_free(sleep_data);

}

//更新组件位置和大小
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL)
    {
        //获取初始值
        switch (compo->id)
        {
            case COMPO_ID_CLOCK_BG:
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG)
                {
                    printf("COMPO_ID_CLOCK_BG ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.bg_w;
                h = ui_handle.card_clock_day.clock.bg_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_H:
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H)
                {
                    printf("COMPO_ID_CLOCK_H ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.h_w;
                h = ui_handle.card_clock_day.clock.h_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_M:
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M)
                {
                    printf("COMPO_ID_CLOCK_M ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.m_w;
                h = ui_handle.card_clock_day.clock.m_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_DATE:
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE)
                {
                    printf("COMPO_ID_DATE ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.date.w;
                h = ui_handle.card_clock_day.date.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_WEEKDAY:
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY)
                {
                    printf("COMPO_ID_WEEKDAY ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.week.w;
                h = ui_handle.card_clock_day.week.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CARD_1:
                if (ui_handle.card1.id != COMPO_ID_CARD_1)
                {
                    printf("COMPO_ID_CARD_1 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card1.x;
                y = card_y = ui_handle.card1.y;
                w = ui_handle.card1.w;
                h = ui_handle.card1.h;
                card_id = ui_handle.card1.id;
                break;

            case COMPO_ID_CARD_2:
                if (ui_handle.card2.id != COMPO_ID_CARD_2)
                {
                    printf("COMPO_ID_CARD_2 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card2.x;
                y = card_y = ui_handle.card2.y;
                w = ui_handle.card2.w;
                h = ui_handle.card2.h;
                card_id = ui_handle.card2.id;
                break;

            case COMPO_ID_CARD_3:
                if (ui_handle.card3.id != COMPO_ID_CARD_3)
                {
                    printf("COMPO_ID_CARD_3 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card3.x;
                y = card_y = ui_handle.card3.y;
                w = ui_handle.card3.w;
                h = ui_handle.card3.h;
                card_id = ui_handle.card3.id;
                break;

            case COMPO_ID_CARD_4:
                if (ui_handle.card4.id != COMPO_ID_CARD_4)
                {
                    printf("COMPO_ID_CARD_4 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card4.x;
                y = card_y = ui_handle.card4.y;
                w = ui_handle.card4.w;
                h = ui_handle.card4.h;
                card_id = ui_handle.card4.id;
                break;

            case COMPO_ID_CARD_5:
                if (ui_handle.card5.id != COMPO_ID_CARD_5)
                {
                    printf("COMPO_ID_CARD_5 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card5.x;
                y = card_y = ui_handle.card5.y;
                w = ui_handle.card5.w;
                h = ui_handle.card5.h;
                card_id = ui_handle.card5.id;
                break;


            case COMPO_ID_CARD_6:
                if (ui_handle.card6.id != COMPO_ID_CARD_6)
                {
                    printf("COMPO_ID_CARD_6 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card6.x;
                y = card_y = ui_handle.card6.y;
                w = ui_handle.card6.w;
                h = ui_handle.card6.h;
                card_id = ui_handle.card6.id;
                break;


            case COMPO_ID_CARD_7:
                if (ui_handle.card7.id != COMPO_ID_CARD_7)
                {
                    printf("COMPO_ID_CARD_7 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card7.x;
                y = card_y = ui_handle.card7.y;
                w = ui_handle.card7.w;
                h = ui_handle.card7.h;
                card_id = ui_handle.card7.id;
                break;

            default:
                compo = compo_get_next(compo);
                continue;
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;
            f_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }
        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_pos(pic, x, y);
            compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set_location(txt, x, y, w, h);
            if (unvisible)
            {
                compo_textbox_set_visible(txt, false);
            }
            else
            {
                compo_textbox_set_visible(txt, true);
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;
            compo_datetime_set_pos(dt, x, y);
        }

        compo = compo_get_next(compo);          //遍历组件
    }
}

//创建上拉卡片菜单（文件内调用）
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
//    compo_shape_set_alpha(masklayer, 160);
    //创建所有组件
    func_clock_sub_card_compo_create(frm);
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);

    return frm;
}

//创建上拉卡片菜单（供外部调用）
compo_form_t *func_clock_sub_card_form_create(void)
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX);
}

//获取点击的卡片组件id
static u16 func_clock_sub_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_BTN_COUNT; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_CARD_START + 1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//表盘上拉卡片菜单点击处理
static void func_clock_sub_card_click_handler(void)
{
    compo_cardbox_t *cardbox = NULL;
//    rect_t rect;
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);
    printf("click compo_id:%d\n", compo_id);
    int id = compo_get_button_id();

    switch(compo_id)
    {
        case COMPO_ID_CARD_1 :
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            func_jump = FUNC_ACTIVITY;
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            break;
        case COMPO_ID_CARD_2 :
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
            func_jump = FUNC_SLEEP;
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
            break;
        case COMPO_ID_CARD_3:
            switch(id)
            {
                case COMPO_MUSIC_BTN_PREV:
                {
                    uteModuleMusicCtrl(false,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
                    compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res);
                }
                break;
                case COMPO_MUSIC_BTN_PLAY:
                {
                    uteModuleMusicCtrlPaused(false);
                    if(ble_is_connect())
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
                    }
//                    if(bt_cb.music_playing)
//                    {
//                        compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY);
//                        compo_button_set_bgimg(btn, ui_handle.card3.pic_click_play.res_click);
//                    }
                }
                break;
                case COMPO_MUSIC_BTN_NEXT:
                {
                    uteModuleMusicCtrl(true,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
                    compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res);
                }
                break;
                default:
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    func_jump = FUNC_BT;
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    break;
            }
            break;
        case COMPO_ID_CARD_4 :
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
            func_jump = FUNC_TIMER;
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            break;
        case COMPO_ID_CARD_5 :
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            func_jump = FUNC_STOPWATCH;
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            break;
        case COMPO_ID_CARD_6 :
            cardbox = compo_getobj_byid(COMPO_ID_CARD_6);
            for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
            {
                rect_t rect = compo_cardbox_get_icon_absolute(cardbox, ui_handle.card6.pic_click[i].idx); //上一首
                if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
                {
//                    printf("rect = [%d,%d,%d,%d]\n", rect.x, rect.y, rect.wid, rect.hei);
                    func_cb.sta = ui_handle.card6.pic_click[i].func_sta;
                }
            }
            break;
        case COMPO_ID_CARD_7 :
            printf("id : %d\n", id);
            if (id >= COMPO_ID_APP1)
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1);
            }
            break;
        default:
            break;
    }
    if(func_jump != FUNC_NULL)
    {
        printf("func_jump=%d\n", func_jump);
        func_cb.sta = func_jump;  //直接跳转
    }
}

//根据偏移位置把最近的卡片对齐到屏幕中央
static s16 func_clock_sub_card_align_y(s16 ofs_y)
{
    u8 i, nearest_id = 0;
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    u16 dc;
    if (ofs_y >= DRAG_Y_MAX)
    {
        return DRAG_Y_MAX;  //回弹效果
    }
    else if (ofs_y <= DRAG_Y_MIN)
    {
        return DRAG_Y_MIN;
    }
//    for (i=0; i<CARD_COUNT; i++)
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y);
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y);
        if (dc < dc_min)
        {
            nearest_id = i;
            dc_min = dc;
        }
    }
//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id));
}

//根据卡片序号对齐页面偏移位置到屏幕中央
static s16 func_clock_sub_card_align_by_idx(s8 card_id)
{
//    if (card_id <= CARD_ID_CLOCK)
    if (card_id <= COMPO_CARD_START)
    {
        return DRAG_Y_MAX;
    }
//    else if (card_id > (CARD_COUNT - 1))
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1))
    {
        return DRAG_Y_MIN;
    }
    else
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id));
    }
}

//限制滑动偏移位置
static s16 func_clock_sub_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//设置页面偏移位置
static void func_clock_sub_card_set_offs(s16 ofs_y)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    f_card->ofs_y = ofs_y;
    func_clock_sub_card_compo_update(f_card->ofs_y, false);
}

//时钟表盘上拉菜单数据更新----------------------------------------->todo
static void func_clock_sub_card_data_update(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    compo_cardbox_t *cardbox = NULL;
    char txt_buf[20];

    if(f_card->flag_drag == 0)
    {
        card1_updata_disp();
    }

//    if(f_activity->activity_state != 2) return;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec));
    cardbox = compo_getobj_byid(ui_handle.card4.id);
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf);

    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    u16 msec = sys_cb.stopwatch_total_msec % 1000;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min  = 99;
        sec  = 59;
        msec = 999;
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld.%02ld",min,sec,msec/10);
    cardbox = compo_getobj_byid(ui_handle.card5.id);
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf);
}

//下拉返回表盘
static void func_clock_sub_card_switch_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y);
    func_cb.frm_main = frm;
    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
}

void music_data_refresh(void)
{
    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); //播放/暂停
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I332001_FIRSTORDER_MUSIC_PAUSED_BIN : ui_handle.card3.pic_click_play.res);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_play.res_click);
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#define CARD_SCALE_START_POS            250//265                                                                     //卡片开始缩小位置
#define CARD_SCALE_STOP_POS             360//320                                                                     //卡片停止缩小位置
#define CARD_SCALE_STOP_Y               (CARD_SCALE_STOP_POS + 200+90 )                                         //卡片停止缩小实际位置（越大缩小越慢，可见叠加越多）
#define CARD_WIDTH_MIN                  100//200                                                                     //卡片缩小的最小宽度
#define CARD_UNVISIBLE_POS_TOP          (-100)//(-160)                                                                  //卡片超出屏幕顶部某位置后设为不可见
#define CARD_UNVISIBLE_POS_BOTTOM       (CARD_SCALE_STOP_Y + 200)                                               //卡片缩到最小后实际位置大于某值设为不可见
#define CARD_BG_GRAY_START              41                                                                      //卡片开始缩小位置处背景的灰度
#define CARD_BG_GRAY_STOP               10                                                                      //卡片停止缩小位置处背景的灰度
#define GRAY_LV(x)                      make_color(x, x, x)                                                     //生成灰度颜色值（0:黑 255:白）

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
//#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - card_y_info[CARD_COUNT - 1] + 50)                     //回弹到底部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_END-1) + 50)                     //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + 30)                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - 78)                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 5   //最近任务不足5个时默认值补上
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};

///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  232//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_timer;
        struct text_t text_time;
    } card4;

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_stopwatch;
        struct text_t text_time;
    } card5;

    struct card6_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_comm_use;
        struct pic_click_t pic_click[3];
    } card6;

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[4];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,
    COMPO_ID_CLOCK_H,
    COMPO_ID_CLOCK_M,
    COMPO_ID_DATE,
    COMPO_ID_WEEKDAY,

    COMPO_CARD_START,
    COMPO_ID_CARD_1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    COMPO_ID_CARD_5,
    COMPO_ID_CARD_6,
    COMPO_ID_CARD_7,
    COMPO_CARD_END,

    COMPO_ID_ARC_KCAL,
    COMPO_ID_ARC_KM,
    COMPO_ID_ARC_STEP,

    COMPO_MUSIC_BTN_PREV,
    COMPO_MUSIC_BTN_NEXT,
    COMPO_MUSIC_BTN_PLAY,

    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
    COMPO_ID_APP4,
};

static const ui_handle_t ui_handle =
{
    .card_clock_day = {
        .id = COMPO_CARD_START,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 24+100/2,
        .w  = 0,
        .h  = 0,

        .date = {
            .id = COMPO_ID_DATE,
            .x  = 12-232/2,
            .y  = 41-136/2,
            .w  = 80+20,
            .h  = 40,
            .center = false,
            .bonddata = COMPO_BOND_DATE,
            .color = {255,255,255},
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = 12-232/2+20,
            .y  = 69-136/2,
            .w  = 60,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 130+100/2-232/2,
            .bg_y   = 0,
            .h_x    = 130+96/2-232/2,
            .h_y    = 0,
            .m_x    = 130+96/2-232/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 17,
            .m_w    = 66,
            .m_h    = 17,
            .bg_res = UI_BUF_I340001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I340001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I340001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    .card1 = {
        .id = COMPO_ID_CARD_1,
        .x  = 180,
        .y  = 228,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },
        .pic_kcal = {
            .idx    = 0,
            .x      = -123,
            .y      = -33,
            .w      = 26,
            .h      = 26,
            .res    = UI_BUF_I340001_FIRSTORDER_ACTIVITY_CALORIES_BIN,
        },
        .pic_km = {
            .idx    = 1,
            .x      = -123,
            .y      = 0,
            .w      = 26,
            .h      = 26,
            .res    = UI_BUF_I340001_FIRSTORDER_ACTIVITY_DIS_BIN,
        },
        .pic_step = {
            .idx    = 2,
            .x      = -123,
            .y      = 33,
            .w      = 26,
            .h      = 26,
            .res    = UI_BUF_I340001_FIRSTORDER_ACTIVITY_STEPT_BIN,
        },

        .arc_kcal = {
            .id     = COMPO_ID_ARC_KCAL,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100,
            .color  = {252,55,40},
            .bg_color = {0x7f,0x6e,0x19},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_km = {
            .id     = COMPO_ID_ARC_KM,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-12,
            .color  = {255,212,0},
            .bg_color = {0x19,0x7a,0x6f},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_step = {
            .id     = COMPO_ID_ARC_STEP,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-24,
            .color  = {0,242,214},
            .bg_color = {0x7e,0x2f,0x29},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .text_kcal = {
            .idx    = 0,
            .x      = -123+21,
            .y      = -(33+15),
            .w      = 65,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_24_BIN,
            .str_id = 0,
            .center = false,
            .color  = {252,55,40},
        },
        .text_km = {
            .idx    = 1,
            .x      = -123+21,
            .y      = -15,
            .w      = 65,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_24_BIN,
            .str_id = 1,
            .center = false,
            .color  = {255,212,0},
        },
        .text_step  = {
            .idx    = 2,
            .x      = -123+21,
            .y      = (33-15),
            .w      = 65,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_24_BIN,
            .str_id = 2,
            .center = false,
            .color  = {0,242,214},
        },
    },

    .card2 = {
        .id = COMPO_ID_CARD_2,
        .x  = 180,
        .y  = 76+316,
        .w  = 324,
        .h  = 152,
        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_sleep = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_SLEEP,
            .center = false,
            .color  = {100,95,255},
        },
        .text_hour = {
            .idx    = 1,
            .x      = -120,
            .y      = -15,
            .w      = 60,
            .h      = 58,
            .res    = UI_BUF_0FONT_FONT_NUM_48_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_hour_unit = {
            .idx    = 2,
            .x      = -65,
            .y      = 15,
            .w      = 35,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_HOUR,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min = {
            .idx    = 3,
            .x      = -20,
            .y      = -15,
            .w      = 60,
            .h      = 58,
            .res    = UI_BUF_0FONT_FONT_NUM_48_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min_unit = {
            .idx    = 4,
            .x      = 35,
            .y      = 15,
            .w      = 35,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MIN,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card3 = {
        .id = COMPO_ID_CARD_3,
        .x  = 180,
        .y  = 480+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_music = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MUSIC,
            .center = false,
            .color  = {255,48,96},
        },

        .pic_click_prev = {
            .idx    = COMPO_MUSIC_BTN_PREV,
            .x      = -95,
            .y      = 18,
            .w      = 36,
            .h      = 36,
            .res    = UI_BUF_I340001_FIRSTORDER_MUSIC_UP00_BIN,
            .res_click = UI_BUF_I340001_FIRSTORDER_MUSIC_UP01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_next = {
            .idx    = COMPO_MUSIC_BTN_NEXT,
            .x      = 90,
            .y      = 18,
            .w      = 36,
            .h      = 36,
            .res    = UI_BUF_I340001_FIRSTORDER_MUSIC_NEXT00_BIN,
            .res_click = UI_BUF_I340001_FIRSTORDER_MUSIC_NEXT01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_play = {
            .idx    = COMPO_MUSIC_BTN_PLAY,
            .x      = -5,
            .y      = 18,
            .w      = 70,
            .h      = 70,
            .res    = UI_BUF_I340001_FIRSTORDER_MUSIC_PLAY00_BIN,
            .res_click = UI_BUF_I340001_FIRSTORDER_MUSIC_PLAY01_BIN,
            .res_switch = UI_BUF_I340001_FIRSTORDER_MUSIC_PAUSED_BIN,
            .func_sta = 0,
        },
    },

    .card4 = {
        .id = COMPO_ID_CARD_4,
        .x  = 180,
        .y  = 644+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_timer = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_TIMER,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 23-324/2,
            .y      = -15,
            .w      = 180,
            .h      = 58,
            .res    = UI_BUF_0FONT_FONT_NUM_48_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card5 = {
        .id = COMPO_ID_CARD_5,
        .x  = 180,
        .y  = 808+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_stopwatch = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_STOP_WATCH,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 23-324/2,
            .y      = -15,
            .w      = 180,
            .h      = 58,
            .res    = UI_BUF_0FONT_FONT_NUM_48_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card6 = {
        .id = COMPO_ID_CARD_6,
        .x  = 180,
        .y  = 972+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_comm_use = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_COMMON_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = 0,
                .x      = -104,
                .y      = 18,
                .w      = 70,
                .h      = 70,
                .res    = UI_BUF_I340001_FIRSTORDER_FIXED_SPORT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },

            [1] = {
                .idx    = 1,
                .x      = 0,
                .y      = 18,
                .w      = 70,
                .h      = 70,
                .res    = UI_BUF_I340001_FIRSTORDER_FIXED_MSM_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
                .func_sta = FUNC_MESSAGE,
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            },

            [2] = {
                .idx    = 2,
                .x      = 104,
                .y      = 18,
                .w      = 70,
                .h      = 70,
                .res    = UI_BUF_I340001_FIRSTORDER_FIXED_CALL_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },

    .card7 = {
        .id = COMPO_ID_CARD_7,
        .x  = 180,
        .y  = 1136+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_last_use = {
            .idx    = 0,
            .x      = 23-324/2,
            .y      = 15-152/2,
            .w      = 200,
            .h      = 35,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_LATEST_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = COMPO_ID_APP1,
                .x      = -115,
                .y      = 18,
                .w      = 64,
                .h      = 64,
                .res    = UI_BUF_I340001_THEME_ICON1_SOPRT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },
            [1] = {
                .idx    = COMPO_ID_APP2,
                .x      = -115+74,
                .y      = 18,
                .w      = 64,
                .h      = 64,
                .res    = UI_BUF_I340001_THEME_ICON1_BRIGHTNESS_BIN,
                .res_click = 1,
                .res_switch = 1,
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
                .func_sta = FUNC_LIGHT,
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            },
            [2] = {
                .idx    = COMPO_ID_APP3,
                .x      = -115+74*2,
                .y      = 18,
                .w      = 64,
                .h      = 64,
                .res    = UI_BUF_I340001_THEME_ICON1_TIMER_BIN,
                .res_click = 2,
                .res_switch = 2,
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
                .func_sta = FUNC_TIMER,
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            },
            [3] = {
                .idx    = COMPO_ID_APP4,
                .x      = -115+74*3,
                .y      = 18,
                .w      = 64,
                .h      = 64,
                .res    = UI_BUF_I340001_THEME_ICON1_CALL_BIN,
                .res_click = 3,
                .res_switch = 3,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },
};

//获取卡片原始位置
static int func_clock_sub_card_get_y_info(int card_compo_id)
{
    int ret = 0;

    if (card_compo_id == ui_handle.card_clock_day.id)
    {
        ret = ui_handle.card_clock_day.y;
    }

    if (card_compo_id == ui_handle.card1.id)
    {
        ret = ui_handle.card1.y;
    }
    if (card_compo_id == ui_handle.card2.id)
    {
        ret = ui_handle.card2.y;
    }
    if (card_compo_id == ui_handle.card3.id)
    {
        ret = ui_handle.card3.y;
    }
    if (card_compo_id == ui_handle.card4.id)
    {
        ret = ui_handle.card4.y;
    }
    if (card_compo_id == ui_handle.card5.id)
    {
        ret = ui_handle.card5.y;
    }
    if (card_compo_id == ui_handle.card6.id)
    {
        ret = ui_handle.card6.y;
    }
    if (card_compo_id == ui_handle.card7.id)
    {
        ret = ui_handle.card7.y;
    }
    return ret;
}

//功能结构体
typedef struct f_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s16 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 activity_tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
//    co_timer_t music_refresh;
} f_card_t;

static void card1_updata_disp(void)
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;

//    if(tick_check_expire(f_activity->activity_tick, ))
//    {
//        f_activity->activity_tick = tick_get();
    char txt_buf[20];
    uint32_t totalStepCnt = 0;
    uint32_t Step_value = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    Step_value = totalStepCnt;
    if(Step_value>uteModuleSportGetStepsTargetCnt())
    {
        Step_value = uteModuleSportGetStepsTargetCnt();
    }

    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL);
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP);
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);

    compo_picturebox_set_visible(arc_kcal, true);
    compo_picturebox_set_visible(arc_km, true);
    compo_picturebox_set_visible(arc_step, true);


    if(f_activity->activity_state == 0)
    {
        f_activity->arc_step_value+=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value >=1800)
        {
            f_activity->activity_state =1;
        }
    }
    else if(f_activity->activity_state == 1)
    {

        f_activity->arc_step_value-=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt())
        {
            f_activity->activity_state = 2;
        }

    }
    else
    {

        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
        compo_cardbox_text_set(cardbox, 2, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
        compo_cardbox_text_set(cardbox, 1, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
        compo_cardbox_text_set(cardbox, 0, txt_buf);


        if(f_activity->arc_step_value)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);
        }
        else
        {
            compo_picturebox_set_visible(arc_kcal,false);
            compo_picturebox_set_visible(arc_km,false);
            compo_picturebox_set_visible(arc_step,false);
        }
    }

//    }
}
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 card_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 card_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(card_get_latest_func(idx));
}

//创建组件
static void func_clock_sub_card_compo_create(compo_form_t *frm)
{
    char txt_buf[20];
    compo_button_t *btn=NULL;

    ///卡片7
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, ui_handle.card7.w, ui_handle.card7.h);
    compo_setid(card7, ui_handle.card7.id);

    compo_cardbox_rect_set_location(card7, ui_handle.card7.rect.idx, ui_handle.card7.rect.x, ui_handle.card7.rect.y, ui_handle.card7.rect.w, ui_handle.card7.rect.h, ui_handle.card7.rect.r);
    compo_cardbox_rect_set_color(card7, ui_handle.card7.rect.idx, make_color(ui_handle.card7.rect.color.r, ui_handle.card7.rect.color.g, ui_handle.card7.rect.color.b));

    compo_cardbox_text_set_font(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.res);
    compo_cardbox_text_set_align_center(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.center);
    widget_text_set_color(card7->text[ui_handle.card7.text_last_use.idx], make_color(ui_handle.card7.text_last_use.color.r, ui_handle.card7.text_last_use.color.g, ui_handle.card7.text_last_use.color.b));
    compo_cardbox_text_set_location(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.x, ui_handle.card7.text_last_use.y, ui_handle.card7.text_last_use.w, ui_handle.card7.text_last_use.h);
    compo_cardbox_text_set(card7, ui_handle.card7.text_last_use.idx, i18n[ui_handle.card7.text_last_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card7.pic_click)/sizeof(ui_handle.card7.pic_click[0]); i++)
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i));
        compo_button_set_location(btn,ui_handle.card7.pic_click[i].x,ui_handle.card7.pic_click[i].y, ui_handle.card7.pic_click[i].w, ui_handle.card7.pic_click[i].h);
        compo_setid(btn,ui_handle.card7.pic_click[i].idx);
    }

    ///卡片6
    compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card6.w, ui_handle.card6.h);
    compo_setid(card6, ui_handle.card6.id);

    compo_cardbox_rect_set_location(card6, ui_handle.card6.rect.idx, ui_handle.card6.rect.x, ui_handle.card6.rect.y, ui_handle.card6.rect.w, ui_handle.card6.rect.h, ui_handle.card6.rect.r);
    compo_cardbox_rect_set_color(card6, ui_handle.card6.rect.idx, make_color(ui_handle.card6.rect.color.r, ui_handle.card6.rect.color.g, ui_handle.card6.rect.color.b));

    compo_cardbox_text_set_font(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.res);
    compo_cardbox_text_set_align_center(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.center);
    widget_text_set_color(card6->text[ui_handle.card6.text_comm_use.idx], make_color(ui_handle.card6.text_comm_use.color.r, ui_handle.card6.text_comm_use.color.g, ui_handle.card6.text_comm_use.color.b));
    compo_cardbox_text_set_location(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.x, ui_handle.card6.text_comm_use.y, ui_handle.card6.text_comm_use.w, ui_handle.card6.text_comm_use.h);
    compo_cardbox_text_set(card6, ui_handle.card6.text_comm_use.idx, i18n[ui_handle.card6.text_comm_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
    {
        compo_cardbox_icon_set_location(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].x, ui_handle.card6.pic_click[i].y, ui_handle.card6.pic_click[i].w, ui_handle.card6.pic_click[i].h);
        compo_cardbox_icon_set(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].res);
    }


    ///卡片5
    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card5.w, ui_handle.card5.h);
    compo_setid(card5, ui_handle.card5.id);

    compo_cardbox_rect_set_location(card5, ui_handle.card5.rect.idx, ui_handle.card5.rect.x, ui_handle.card5.rect.y, ui_handle.card5.rect.w, ui_handle.card5.rect.h, ui_handle.card5.rect.r);
    compo_cardbox_rect_set_color(card5, ui_handle.card5.rect.idx, make_color(ui_handle.card5.rect.color.r, ui_handle.card5.rect.color.g, ui_handle.card5.rect.color.b));

//    compo_cardbox_text_set_font(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_stopwatch.idx], make_color(ui_handle.card5.text_stopwatch.color.r, ui_handle.card5.text_stopwatch.color.g, ui_handle.card5.text_stopwatch.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.x, ui_handle.card5.text_stopwatch.y, ui_handle.card5.text_stopwatch.w, ui_handle.card5.text_stopwatch.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_stopwatch.idx, i18n[ui_handle.card5.text_stopwatch.str_id]);

    compo_cardbox_text_set_font(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_time.idx], make_color(ui_handle.card5.text_time.color.r, ui_handle.card5.text_time.color.g, ui_handle.card5.text_time.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.x, ui_handle.card5.text_time.y, ui_handle.card5.text_time.w, ui_handle.card5.text_time.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_time.idx, "00:00.00");

    ///卡片4
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card4.w, ui_handle.card4.h);
    compo_setid(card4, ui_handle.card4.id);

    compo_cardbox_rect_set_location(card4, ui_handle.card4.rect.idx, ui_handle.card4.rect.x, ui_handle.card4.rect.y, ui_handle.card4.rect.w, ui_handle.card4.rect.h, ui_handle.card4.rect.r);
    compo_cardbox_rect_set_color(card4, ui_handle.card4.rect.idx, make_color(ui_handle.card4.rect.color.r, ui_handle.card4.rect.color.g, ui_handle.card4.rect.color.b));

    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_timer.idx], make_color(ui_handle.card4.text_timer.color.r, ui_handle.card4.text_timer.color.g, ui_handle.card4.text_timer.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.x, ui_handle.card4.text_timer.y, ui_handle.card4.text_timer.w, ui_handle.card4.text_timer.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_timer.idx, i18n[ui_handle.card4.text_timer.str_id]);

    compo_cardbox_text_set_font(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.res);
    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_time.idx], make_color(ui_handle.card4.text_time.color.r, ui_handle.card4.text_time.color.g, ui_handle.card4.text_time.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.x, ui_handle.card4.text_time.y, ui_handle.card4.text_time.w, ui_handle.card4.text_time.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_time.idx, "00:00:00");


    ///卡片3
    compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card3.w, ui_handle.card3.h);
    compo_setid(card3, ui_handle.card3.id);

    compo_cardbox_rect_set_location(card3, ui_handle.card3.rect.idx, ui_handle.card3.rect.x, ui_handle.card3.rect.y, ui_handle.card3.rect.w, ui_handle.card3.rect.h, ui_handle.card3.rect.r);
    compo_cardbox_rect_set_color(card3, ui_handle.card3.rect.idx, make_color(ui_handle.card3.rect.color.r, ui_handle.card3.rect.color.g, ui_handle.card3.rect.color.b));

    compo_cardbox_text_set_font(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.res);
    compo_cardbox_text_set_align_center(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.center);
    widget_text_set_color(card3->text[ui_handle.card3.text_music.idx], make_color(ui_handle.card3.text_music.color.r, ui_handle.card3.text_music.color.g, ui_handle.card3.text_music.color.b));
    compo_cardbox_text_set_location(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.x, ui_handle.card3.text_music.y, ui_handle.card3.text_music.w, ui_handle.card3.text_music.h);
    compo_cardbox_text_set(card3, ui_handle.card3.text_music.idx, i18n[ui_handle.card3.text_music.str_id]);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_prev.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_prev.x, ui_handle.card3.pic_click_prev.y, ui_handle.card3.pic_click_prev.w, ui_handle.card3.pic_click_prev.h);
    compo_setid(btn,ui_handle.card3.pic_click_prev.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_next.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_next.x, ui_handle.card3.pic_click_next.y, ui_handle.card3.pic_click_next.w, ui_handle.card3.pic_click_next.h);
    compo_setid(btn,ui_handle.card3.pic_click_next.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_play.res_click);
    compo_button_set_pos(btn,ui_handle.card3.pic_click_play.x, ui_handle.card3.pic_click_play.y);
    compo_setid(btn,ui_handle.card3.pic_click_play.idx);

    ///卡片2
    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 0, 5, ui_handle.card2.w, ui_handle.card2.h);
    compo_setid(card2, ui_handle.card2.id);

    compo_cardbox_rect_set_location(card2, ui_handle.card2.rect.idx, ui_handle.card2.rect.x, ui_handle.card2.rect.y, ui_handle.card2.rect.w, ui_handle.card2.rect.h, ui_handle.card2.rect.r);
    compo_cardbox_rect_set_color(card2, ui_handle.card2.rect.idx, make_color(ui_handle.card2.rect.color.r, ui_handle.card2.rect.color.g, ui_handle.card2.rect.color.b));

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_sleep.idx], make_color(ui_handle.card2.text_sleep.color.r, ui_handle.card2.text_sleep.color.g, ui_handle.card2.text_sleep.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.x, ui_handle.card2.text_sleep.y, ui_handle.card2.text_sleep.w, ui_handle.card2.text_sleep.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_sleep.idx, i18n[ui_handle.card2.text_sleep.str_id]);

    // sleep_data->totalSleepMin =  412;
    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour.idx], make_color(ui_handle.card2.text_hour.color.r, ui_handle.card2.text_hour.color.g, ui_handle.card2.text_hour.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.x, ui_handle.card2.text_hour.y, ui_handle.card2.text_hour.w, ui_handle.card2.text_hour.h);
    memset(txt_buf,0,sizeof(txt_buf));
    if(sleep_data->totalSleepMin)   ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠小时*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour_unit.idx], make_color(ui_handle.card2.text_hour_unit.color.r, ui_handle.card2.text_hour_unit.color.g, ui_handle.card2.text_hour_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.x+6, ui_handle.card2.text_hour_unit.y, ui_handle.card2.text_hour_unit.w, ui_handle.card2.text_hour_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour_unit.idx, i18n[ui_handle.card2.text_hour_unit.str_id]);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min.idx], make_color(ui_handle.card2.text_min.color.r, ui_handle.card2.text_min.color.g, ui_handle.card2.text_min.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.x, ui_handle.card2.text_min.y, ui_handle.card2.text_min.w, ui_handle.card2.text_min.h);
    if(sleep_data->totalSleepMin)  ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠分钟*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_min.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min_unit.idx], make_color(ui_handle.card2.text_min_unit.color.r, ui_handle.card2.text_min_unit.color.g, ui_handle.card2.text_min_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.x+6, ui_handle.card2.text_min_unit.y, ui_handle.card2.text_min_unit.w, ui_handle.card2.text_min_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_min_unit.idx, i18n[ui_handle.card2.text_min_unit.str_id]);

    ///卡片1
    compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, ui_handle.card1.w, ui_handle.card1.h);
    compo_setid(card1, ui_handle.card1.id);

    compo_cardbox_rect_set_location(card1, ui_handle.card1.rect.idx, ui_handle.card1.rect.x, ui_handle.card1.rect.y, ui_handle.card1.rect.w, ui_handle.card1.rect.h, ui_handle.card1.rect.r);
    compo_cardbox_rect_set_color(card1, ui_handle.card1.rect.idx, make_color(ui_handle.card1.rect.color.r, ui_handle.card1.rect.color.g, ui_handle.card1.rect.color.b));

    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.x, ui_handle.card1.pic_kcal.y, ui_handle.card1.pic_kcal.w, ui_handle.card1.pic_kcal.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.x, ui_handle.card1.pic_km.y, ui_handle.card1.pic_km.w, ui_handle.card1.pic_km.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.x, ui_handle.card1.pic_step.y, ui_handle.card1.pic_step.w, ui_handle.card1.pic_step.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.res);

    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_kcal.idx], make_color(ui_handle.card1.text_kcal.color.r, ui_handle.card1.text_kcal.color.g, ui_handle.card1.text_kcal.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.x, ui_handle.card1.text_kcal.y, ui_handle.card1.text_kcal.w, ui_handle.card1.text_kcal.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
    compo_cardbox_text_set(card1, ui_handle.card1.text_kcal.idx, txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_km.idx], make_color(ui_handle.card1.text_km.color.r, ui_handle.card1.text_km.color.g, ui_handle.card1.text_km.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.x, ui_handle.card1.text_km.y, ui_handle.card1.text_km.w, ui_handle.card1.text_km.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",KM/100, KM%100);///公里数据
    compo_cardbox_text_set(card1, ui_handle.card1.text_km.idx,txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_step.idx], make_color(ui_handle.card1.text_step.color.r, ui_handle.card1.text_step.color.g, ui_handle.card1.text_step.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.x, ui_handle.card1.text_step.y, ui_handle.card1.text_step.w, ui_handle.card1.text_step.h);
    snprintf(txt_buf,sizeof(txt_buf),"%ld",totalStepCnt);
    compo_cardbox_text_set(card1, ui_handle.card1.text_step.idx, txt_buf);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I340001_FIRSTORDER_ACTIVITY_BG_BIN);
    widget_page_t * widget_page = widget_page_create(card1->page);///创建按键页面
    widget_set_location(widget_page,52,0,pic_bg_area.wid,pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_FIRSTORDER_ACTIVITY_BG_BIN);//彩虹背景
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_FIRSTORDER_ACTIVITY_RED_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KCAL);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_FIRSTORDER_ACTIVITY_YELLOW_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KM);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_FIRSTORDER_ACTIVITY_BLUE_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_STEP);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I340001_FIRSTORDER_ACTIVITY_YUANJIAO_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I340001_FIRSTORDER_ACTIVITY_YUANJIAO_BIN).hei+1 );

    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res);
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg);

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res);
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2);
    compo_datetime_set_start_angle(clock_h, 900);
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata);

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res);
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min);
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2);
    compo_datetime_set_start_angle(clock_m, 900);
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata);

    compo_textbox_t* date = compo_textbox_create(frm, 5);                                 //日期
    compo_setid(date, ui_handle.card_clock_day.date.id);
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res);
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center);
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b));
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata);

    compo_textbox_t* week = compo_textbox_create(frm, 20);                                 //日期
    compo_setid(week, ui_handle.card_clock_day.week.id);
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res);
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center);
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b));
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 13, 54, 6);
    compo_shape_set_color(shape, make_color(76,76,76));
    compo_shape_set_radius(shape, 35);

    ab_free(sleep_data);

}

//更新组件位置和大小
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL)
    {
        //获取初始值
        switch (compo->id)
        {
            case COMPO_ID_CLOCK_BG:
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG)
                {
                    printf("COMPO_ID_CLOCK_BG ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.bg_w;
                h = ui_handle.card_clock_day.clock.bg_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_H:
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H)
                {
                    printf("COMPO_ID_CLOCK_H ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.h_w;
                h = ui_handle.card_clock_day.clock.h_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_M:
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M)
                {
                    printf("COMPO_ID_CLOCK_M ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.m_w;
                h = ui_handle.card_clock_day.clock.m_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_DATE:
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE)
                {
                    printf("COMPO_ID_DATE ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.date.w;
                h = ui_handle.card_clock_day.date.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_WEEKDAY:
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY)
                {
                    printf("COMPO_ID_WEEKDAY ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.week.w;
                h = ui_handle.card_clock_day.week.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CARD_1:
                if (ui_handle.card1.id != COMPO_ID_CARD_1)
                {
                    printf("COMPO_ID_CARD_1 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card1.x;
                y = card_y = ui_handle.card1.y;
                w = ui_handle.card1.w;
                h = ui_handle.card1.h;
                card_id = ui_handle.card1.id;
                break;

            case COMPO_ID_CARD_2:
                if (ui_handle.card2.id != COMPO_ID_CARD_2)
                {
                    printf("COMPO_ID_CARD_2 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card2.x;
                y = card_y = ui_handle.card2.y;
                w = ui_handle.card2.w;
                h = ui_handle.card2.h;
                card_id = ui_handle.card2.id;
                break;

            case COMPO_ID_CARD_3:
                if (ui_handle.card3.id != COMPO_ID_CARD_3)
                {
                    printf("COMPO_ID_CARD_3 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card3.x;
                y = card_y = ui_handle.card3.y;
                w = ui_handle.card3.w;
                h = ui_handle.card3.h;
                card_id = ui_handle.card3.id;
                break;

            case COMPO_ID_CARD_4:
                if (ui_handle.card4.id != COMPO_ID_CARD_4)
                {
                    printf("COMPO_ID_CARD_4 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card4.x;
                y = card_y = ui_handle.card4.y;
                w = ui_handle.card4.w;
                h = ui_handle.card4.h;
                card_id = ui_handle.card4.id;
                break;

            case COMPO_ID_CARD_5:
                if (ui_handle.card5.id != COMPO_ID_CARD_5)
                {
                    printf("COMPO_ID_CARD_5 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card5.x;
                y = card_y = ui_handle.card5.y;
                w = ui_handle.card5.w;
                h = ui_handle.card5.h;
                card_id = ui_handle.card5.id;
                break;


            case COMPO_ID_CARD_6:
                if (ui_handle.card6.id != COMPO_ID_CARD_6)
                {
                    printf("COMPO_ID_CARD_6 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card6.x;
                y = card_y = ui_handle.card6.y;
                w = ui_handle.card6.w;
                h = ui_handle.card6.h;
                card_id = ui_handle.card6.id;
                break;


            case COMPO_ID_CARD_7:
                if (ui_handle.card7.id != COMPO_ID_CARD_7)
                {
                    printf("COMPO_ID_CARD_7 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card7.x;
                y = card_y = ui_handle.card7.y;
                w = ui_handle.card7.w;
                h = ui_handle.card7.h;
                card_id = ui_handle.card7.id;
                break;

            default:
                compo = compo_get_next(compo);
                continue;
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;
            f_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }
        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_pos(pic, x, y);
            compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set_location(txt, x, y, w, h);
            if (unvisible)
            {
                compo_textbox_set_visible(txt, false);
            }
            else
            {
                compo_textbox_set_visible(txt, true);
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;
            compo_datetime_set_pos(dt, x, y);
        }

        compo = compo_get_next(compo);          //遍历组件
    }
}

//创建上拉卡片菜单（文件内调用）
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
//    compo_shape_set_alpha(masklayer, 160);
    //创建所有组件
    func_clock_sub_card_compo_create(frm);
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);

    return frm;
}

//创建上拉卡片菜单（供外部调用）
compo_form_t *func_clock_sub_card_form_create(void)
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX);
}

//获取点击的卡片组件id
static u16 func_clock_sub_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_BTN_COUNT; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_CARD_START + 1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//表盘上拉卡片菜单点击处理
static void func_clock_sub_card_click_handler(void)
{
    compo_cardbox_t *cardbox = NULL;
//    rect_t rect;
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);
    printf("click compo_id:%d\n", compo_id);
    int id = compo_get_button_id();

    switch(compo_id)
    {
        case COMPO_ID_CARD_1 :
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            func_jump = FUNC_ACTIVITY;
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            break;
        case COMPO_ID_CARD_2 :
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
            func_jump = FUNC_SLEEP;
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
            break;
        case COMPO_ID_CARD_3:
            switch(id)
            {
                case COMPO_MUSIC_BTN_PREV:
                {
                    uteModuleMusicCtrl(false,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
                    compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res);
                }
                break;
                case COMPO_MUSIC_BTN_PLAY:
                {
                    uteModuleMusicCtrlPaused(false);
                    if(ble_is_connect())
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
                    }
//                    if(bt_cb.music_playing)
//                    {
//                        compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY);
//                        compo_button_set_bgimg(btn, ui_handle.card3.pic_click_play.res_click);
//                    }
                }
                break;
                case COMPO_MUSIC_BTN_NEXT:
                {
                    uteModuleMusicCtrl(true,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
                    compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res);
                }
                break;
                default:
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    func_jump = FUNC_BT;
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    break;
            }
            break;
        case COMPO_ID_CARD_4 :
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
            func_jump = FUNC_TIMER;
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            break;
        case COMPO_ID_CARD_5 :
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            func_jump = FUNC_STOPWATCH;
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            break;
        case COMPO_ID_CARD_6 :
            cardbox = compo_getobj_byid(COMPO_ID_CARD_6);
            for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
            {
                rect_t rect = compo_cardbox_get_icon_absolute(cardbox, ui_handle.card6.pic_click[i].idx); //上一首
                if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
                {
//                    printf("rect = [%d,%d,%d,%d]\n", rect.x, rect.y, rect.wid, rect.hei);
                    func_cb.sta = ui_handle.card6.pic_click[i].func_sta;
                }
            }
            break;
        case COMPO_ID_CARD_7 :
            printf("id : %d\n", id);
            if (id >= COMPO_ID_APP1)
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1);
            }
            break;
        default:
            break;
    }
    if(func_jump != FUNC_NULL)
    {
        printf("func_jump=%d\n", func_jump);
        func_cb.sta = func_jump;  //直接跳转
    }
}

//根据偏移位置把最近的卡片对齐到屏幕中央
static s16 func_clock_sub_card_align_y(s16 ofs_y)
{
    u8 i, nearest_id = 0;
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    u16 dc;
    if (ofs_y >= DRAG_Y_MAX)
    {
        return DRAG_Y_MAX;  //回弹效果
    }
    else if (ofs_y <= DRAG_Y_MIN)
    {
        return DRAG_Y_MIN;
    }
//    for (i=0; i<CARD_COUNT; i++)
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y);
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y);
        if (dc < dc_min)
        {
            nearest_id = i;
            dc_min = dc;
        }
    }
//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id));
}

//根据卡片序号对齐页面偏移位置到屏幕中央
static s16 func_clock_sub_card_align_by_idx(s8 card_id)
{
//    if (card_id <= CARD_ID_CLOCK)
    if (card_id <= COMPO_CARD_START)
    {
        return DRAG_Y_MAX;
    }
//    else if (card_id > (CARD_COUNT - 1))
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1))
    {
        return DRAG_Y_MIN;
    }
    else
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id));
    }
}

//限制滑动偏移位置
static s16 func_clock_sub_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//设置页面偏移位置
static void func_clock_sub_card_set_offs(s16 ofs_y)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    f_card->ofs_y = ofs_y;
    func_clock_sub_card_compo_update(f_card->ofs_y, false);
}

//时钟表盘上拉菜单数据更新----------------------------------------->todo
static void func_clock_sub_card_data_update(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    compo_cardbox_t *cardbox = NULL;
    char txt_buf[20];

    if(f_card->flag_drag == 0)
    {
        card1_updata_disp();
    }

//    if(f_activity->activity_state != 2) return;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec));
    cardbox = compo_getobj_byid(ui_handle.card4.id);
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf);

    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    u16 msec = sys_cb.stopwatch_total_msec % 1000;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min  = 99;
        sec  = 59;
        msec = 999;
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld.%02ld",min,sec,msec/10);
    cardbox = compo_getobj_byid(ui_handle.card5.id);
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf);
}

//下拉返回表盘
static void func_clock_sub_card_switch_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y);
    func_cb.frm_main = frm;
    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
}

void music_data_refresh(void)
{
    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); //播放/暂停
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I340001_FIRSTORDER_MUSIC_PAUSED_BIN : ui_handle.card3.pic_click_play.res);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_play.res_click);
    }
}
#else
typedef struct f_card_t_
{
} f_card_t;

compo_form_t *func_clock_sub_card_form_create(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//时钟表盘上拉菜单主要事件流程处理
static void func_clock_sub_card_process(void)
{
#if (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT)
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    if (f_card->flag_drag)
    {
        s32 dx, dy;
        f_card->flag_drag = ctp_get_dxy(&dx, &dy);
        if (f_card->flag_drag)
        {
            //拖动页面
            f_card->ofs_y_drag = func_clock_sub_card_limit_y(f_card->ofs_y + dy);
            func_clock_sub_card_compo_update(f_card->ofs_y_drag, false);
        }
        else
        {
            //抬手
            f_card->ofs_y = f_card->ofs_y_drag;
            s16 last_dy = ctp_get_last_dxy().y;
            s16 to_y = f_card->ofs_y + (last_dy * DRAG_AUTO_SPEED);
            f_card->moveto_y = func_clock_sub_card_align_y(to_y);
            f_card->flag_move_auto = true;
            f_card->tick = tick_get();
        }
        for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
        {
            u16 id = COMPO_CARD_START + 1 + i;
            compo_cardbox_text_scroll_reset((compo_cardbox_t *)compo_getobj_byid(id));   //scroll test
            compo_cardbox_text_scroll_pause((compo_cardbox_t *)compo_getobj_byid(id), true);   //scroll test
        }

    }
    else if (f_card->flag_move_auto)
    {
        for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
        {
            u16 id = COMPO_CARD_START + 1 + i;
            compo_cardbox_text_scroll_pause((compo_cardbox_t *)compo_getobj_byid(id), false);   //scroll test
        }

        //自动移动
        if (f_card->ofs_y == f_card->moveto_y)
        {
            if (f_card->ofs_y < SPRING_Y_MIN)
            {
                f_card->moveto_y = SPRING_Y_MIN;
            }
            else if (f_card->ofs_y > SPRING_Y_MAX)
            {
                f_card->moveto_y = SPRING_Y_MAX;
            }
            else
            {
                f_card->flag_move_auto = false;            //移动完成
                f_card->moveto_card = f_card->focus_card;
            }
        }
        else if (tick_check_expire(f_card->tick, FOCUS_AUTO_TICK_EXPIRE))
        {
            TRACE("[%d]--move_to->[%d]\n", f_card->ofs_y, f_card->moveto_y);
            s16 dy;
            f_card->tick = tick_get();
            dy = f_card->moveto_y - f_card->ofs_y;
            if (dy > 0)
            {
                if (dy > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy > FOCUS_AUTO_STEP)
                {
                    dy = FOCUS_AUTO_STEP;
                }
            }
            else if (dy < 0)
            {
                if (dy < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV)
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;
                }
                else if (dy < -FOCUS_AUTO_STEP)
                {
                    dy = -FOCUS_AUTO_STEP;
                }
            }
            func_clock_sub_card_set_offs(f_card->ofs_y + dy);
        }
    }

    func_clock_sub_card_data_update();
    func_process();

    for(u8 i=0; i<CARD_BTN_COUNT; i++)      //文本滚动
    {
        u16 id = COMPO_CARD_START + 1 + i;
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true);
    }
#endif
}

//时钟表盘上拉菜单功能消息处理
static void func_clock_sub_card_message(size_msg_t msg)
{
#if (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT)
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 5   //最近任务不足5个时默认值补上
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};

///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  232//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

            music_data_refresh();
        }
        default:
            func_message(msg);
            break;
    }
#endif
}

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

//时钟表盘上拉菜单进入处理
static void func_clock_sub_card_enter(void)
{
    tft_set_temode(3);
    func_cb.f_cb = func_zalloc(sizeof(f_card_t));
    func_cb.frm_main = func_clock_sub_card_form_create();
#if (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT)
//    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    func_clock_sub_card_set_offs(SPRING_Y_MAX);
#endif
}

//时钟表盘上拉菜单退出处理
static void func_clock_sub_card_exit(void)
{
//    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    func_cb.last = FUNC_CARD;
    tft_set_temode(DEFAULT_TE_MODE);
}

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[4];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,
    COMPO_ID_CLOCK_H,
    COMPO_ID_CLOCK_M,
    COMPO_ID_DATE,
    COMPO_ID_WEEKDAY,

    COMPO_CARD_START,
    COMPO_ID_CARD_1,
    COMPO_ID_CARD_2,
    COMPO_ID_CARD_3,
    COMPO_ID_CARD_4,
    COMPO_ID_CARD_5,
    COMPO_ID_CARD_6,
    COMPO_ID_CARD_7,
    COMPO_CARD_END,

    COMPO_ID_ARC_KCAL,
    COMPO_ID_ARC_KM,
    COMPO_ID_ARC_STEP,

    COMPO_MUSIC_BTN_PREV,
    COMPO_MUSIC_BTN_NEXT,
    COMPO_MUSIC_BTN_PLAY,

    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
    COMPO_ID_APP4,
};

static const ui_handle_t ui_handle =
{
    .card_clock_day = {
        .id = COMPO_CARD_START,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 24+100/2,
        .w  = 0,
        .h  = 0,

        .date = {
            .id = COMPO_ID_DATE,
            .x  = 12-232/2,
            .y  = 41-136/2,
            .w  = 80+20,
            .h  = 40,
            .center = false,
            .bonddata = COMPO_BOND_DATE,
            .color = {255,255,255},
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = 12-232/2,
            .y  = 79-136/2,
            .w  = 45,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 130+100/2-232/2,
            .bg_y   = 0,
            .h_x    = 130+96/2-232/2,
            .h_y    = 0,
            .m_x    = 130+96/2-232/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 18,
            .m_w    = 65,
            .m_h    = 18,
            .bg_res = UI_BUF_I332001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I332001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I332001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    .card1 = {
        .id = COMPO_ID_CARD_1,
        .x  = 180,
        .y  = 228,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },
        .pic_kcal = {
            .idx    = 0,
            .x      = 10+18/2-324/2,
            .y      = 21+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_CALORIES_BIN,
        },
        .pic_km = {
            .idx    = 1,
            .x      = 10+18/2-324/2,
            .y      = 46+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_DIS_BIN,
        },
        .pic_step = {
            .idx    = 2,
            .x      = 10+18/2-324/2,
            .y      = 71+18/2-152/2,
            .w      = 18,
            .h      = 18,
            .res    = UI_BUF_I332001_FIRSTORDER_ACTIVITY_STEPT_BIN,
        },

        .arc_kcal = {
            .id     = COMPO_ID_ARC_KCAL,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100,
            .color  = {252,55,40},
            .bg_color = {0x7f,0x6e,0x19},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_km = {
            .id     = COMPO_ID_ARC_KM,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-12,
            .color  = {255,212,0},
            .bg_color = {0x19,0x7a,0x6f},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .arc_step = {
            .id     = COMPO_ID_ARC_STEP,
            .x      = 25,
            .y      = 50,
            .w      = 10,
            .r      = 100-24,
            .color  = {0,242,214},
            .bg_color = {0x7e,0x2f,0x29},
            .zero_angle = 2700,
            .angle_min  = 0,
            .angle_max  = 1800,
        },
        .text_kcal = {
            .idx    = 0,
            .x      = 34-324/2,
            .y      = 21+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 0,
            .center = false,
            .color  = {252,55,40},
        },
        .text_km = {
            .idx    = 1,
            .x      = 34-324/2,
            .y      = 46+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 1,
            .center = false,
            .color  = {255,212,0},
        },
        .text_step  = {
            .idx    = 2,
            .x      = 34-324/2,
            .y      = 71+18/2-152/2-10,
            .w      = 80,
            .h      = 24,
            .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,
            .str_id = 2,
            .center = false,
            .color  = {0,242,214},
        },
    },

    .card2 = {
        .id = COMPO_ID_CARD_2,
        .x  = 180,
        .y  = 76+316,
        .w  = 324,
        .h  = 152,
        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_sleep = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_SLEEP,
            .center = false,
            .color  = {100,95,255},
        },
        .text_hour = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 38-152/2+6,
            .w      = 41+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_hour_unit = {
            .idx    = 2,
            .x      = 56-324/2,
            .y      = 53-152/2,
            .w      = 46,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_HOUR,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min = {
            .idx    = 3,
            .x      = 97-324/2+14,
            .y      = 38-152/2+6,
            .w      = 40+23,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
        .text_min_unit = {
            .idx    = 4,
            .x      = 140-324/2+12,
            .y      = 53-152/2,
            .w      = 50,
            .h      = 28,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MIN,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card3 = {
        .id = COMPO_ID_CARD_3,
        .x  = 180,
        .y  = 480+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_music = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_MUSIC,
            .center = false,
            .color  = {255,48,96},
        },

        .pic_click_prev = {
            .idx    = COMPO_MUSIC_BTN_PREV,
            .x      = 26+28/2-324/2,
            .y      = 52+28/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_UP00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_UP01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_next = {
            .idx    = COMPO_MUSIC_BTN_NEXT,
            .x      = 178+28/2-324/2,
            .y      = 52+28/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_NEXT00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_NEXT01_BIN,
            .res_switch = 0,
            .func_sta = 0,
        },
        .pic_click_play = {
            .idx    = COMPO_MUSIC_BTN_PLAY,
            .x      = 88+56/2-324/2,
            .y      = 38+56/2-152/2,
            .w      = 28,
            .h      = 28,
            .res    = UI_BUF_I332001_FIRSTORDER_MUSIC_PLAY00_BIN,
            .res_click = UI_BUF_I332001_FIRSTORDER_MUSIC_PLAY01_BIN,
            .res_switch = UI_BUF_I332001_FIRSTORDER_MUSIC_PAUSED_BIN,
            .func_sta = 0,
        },
    },

    .card4 = {
        .id = COMPO_ID_CARD_4,
        .x  = 180,
        .y  = 644+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_timer = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_TIMER,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 37-152/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card5 = {
        .id = COMPO_ID_CARD_5,
        .x  = 180,
        .y  = 808+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_stopwatch = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_STOP_WATCH,
            .center = false,
            .color  = {248,132,10},
        },
        .text_time = {
            .idx    = 1,
            .x      = 12-324/2,
            .y      = 37-152/2,
            .w      = 200,
            .h      = 46,
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,
            .str_id = STR_NULL,
            .center = false,
            .color  = {255,255,255},
        },
    },

    .card6 = {
        .id = COMPO_ID_CARD_6,
        .x  = 180,
        .y  = 972+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_comm_use = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_COMMON_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = 0,
                .x      = 27+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_SPORT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },

            [1] = {
                .idx    = 1,
                .x      = 91+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_MSM_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
                .func_sta = FUNC_MESSAGE,
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
            },

            [2] = {
                .idx    = 2,
                .x      = 155+48/2-324/2,
                .y      = 41+48/2-152/2,
                .w      = 48,
                .h      = 48,
                .res    = UI_BUF_I332001_FIRSTORDER_FIXED_CALL_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },

    .card7 = {
        .id = COMPO_ID_CARD_7,
        .x  = 180,
        .y  = 1136+76,
        .w  = 324,
        .h  = 152,

        .rect = {
            .idx    = 0,
            .x      = 0,
            .y      = 0,
            .w      = 324,
            .h      = 152,
            .r      = 16,
            .color  = {41,41,41},
        },

        .text_last_use = {
            .idx    = 0,
            .x      = 12-324/2,
            .y      = 10-152/2,
            .w      = 200,
            .h      = 30,
            .res    = UI_BUF_0FONT_FONT_BIN,
            .str_id = STR_LATEST_APP,
            .center = false,
            .color  = {255,255,255},
        },
        .pic_click = {
            [0] = {
                .idx    = COMPO_ID_APP1,
                .x      = 12+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I332001_THEME_ICON1_SOPRT_BIN,
                .res_click = 0,
                .res_switch = 0,
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
                .func_sta = FUNC_SPORT,
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
            },
            [1] = {
                .idx    = COMPO_ID_APP2,
                .x      = 66+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 45,
                .h      = 45,
                .res    = UI_BUF_I332001_THEME_ICON1_BRIGHTNESS_BIN,
                .res_click = 1,
                .res_switch = 1,
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
                .func_sta = FUNC_LIGHT,
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT
            },
            [2] = {
                .idx    = COMPO_ID_APP3,
                .x      = 120+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I332001_THEME_ICON1_TIMER_BIN,
                .res_click = 2,
                .res_switch = 2,
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
                .func_sta = FUNC_TIMER,
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            },
            [3] = {
                .idx    = COMPO_ID_APP4,
                .x      = 174+44/2-324/2,
                .y      = 43+44/2-152/2,
                .w      = 46,
                .h      = 46,
                .res    = UI_BUF_I332001_THEME_ICON1_CALL_BIN,
                .res_click = 3,
                .res_switch = 3,
#if UTE_MODULE_SCREENS_CALL_SUPPORT
                .func_sta = FUNC_CALL,
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
            },
        },
    },
};

//获取卡片原始位置
static int func_clock_sub_card_get_y_info(int card_compo_id)
{
    int ret = 0;

    if (card_compo_id == ui_handle.card_clock_day.id)
    {
        ret = ui_handle.card_clock_day.y;
    }

    if (card_compo_id == ui_handle.card1.id)
    {
        ret = ui_handle.card1.y;
    }
    if (card_compo_id == ui_handle.card2.id)
    {
        ret = ui_handle.card2.y;
    }
    if (card_compo_id == ui_handle.card3.id)
    {
        ret = ui_handle.card3.y;
    }
    if (card_compo_id == ui_handle.card4.id)
    {
        ret = ui_handle.card4.y;
    }
    if (card_compo_id == ui_handle.card5.id)
    {
        ret = ui_handle.card5.y;
    }
    if (card_compo_id == ui_handle.card6.id)
    {
        ret = ui_handle.card6.y;
    }
    if (card_compo_id == ui_handle.card7.id)
    {
        ret = ui_handle.card7.y;
    }
    return ret;
}

//功能结构体
typedef struct f_card_t_
{
    s32 ofs_y;
    s32 ofs_y_drag;
    s16 focus_card;                  //靠近屏幕中央的卡片序号
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    s32 moveto_y;                   //设定自动移到的坐标
    s16 moveto_card;                //设定自动移到的卡片序号
    u32 tick;
    u32 activity_tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
//    co_timer_t music_refresh;
} f_card_t;

static void card1_updata_disp(void)
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;

//    if(tick_check_expire(f_activity->activity_tick, ))
//    {
//        f_activity->activity_tick = tick_get();
    char txt_buf[20];
    uint32_t totalStepCnt = 0;
    uint32_t Step_value = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);
    Step_value = totalStepCnt;
    if(Step_value>uteModuleSportGetStepsTargetCnt())
    {
        Step_value = uteModuleSportGetStepsTargetCnt();
    }

    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL);
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP);
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);

    compo_picturebox_set_visible(arc_kcal, true);
    compo_picturebox_set_visible(arc_km, true);
    compo_picturebox_set_visible(arc_step, true);


    if(f_activity->activity_state == 0)
    {
        f_activity->arc_step_value+=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value >=1800)
        {
            f_activity->activity_state =1;
        }
    }
    else if(f_activity->activity_state == 1)
    {

        f_activity->arc_step_value-=18;

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value );

        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt())
        {
            f_activity->activity_state = 2;
        }

    }
    else
    {

        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt());

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 );
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 );

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
        compo_cardbox_text_set(cardbox, 2, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100);
        compo_cardbox_text_set(cardbox, 1, txt_buf);

        memset(txt_buf,'\0',sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
        compo_cardbox_text_set(cardbox, 0, txt_buf);


        if(f_activity->arc_step_value)
        {
            compo_picturebox_set_visible(arc_kcal,true);
            compo_picturebox_set_visible(arc_km,true);
            compo_picturebox_set_visible(arc_step,true);
        }
        else
        {
            compo_picturebox_set_visible(arc_kcal,false);
            compo_picturebox_set_visible(arc_km,false);
            compo_picturebox_set_visible(arc_step,false);
        }
    }

//    }
}
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 card_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 card_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(card_get_latest_func(idx));
}

//创建组件
static void func_clock_sub_card_compo_create(compo_form_t *frm)
{
    char txt_buf[20];
    compo_button_t *btn=NULL;

    ///卡片7
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, ui_handle.card7.w, ui_handle.card7.h);
    compo_setid(card7, ui_handle.card7.id);

    compo_cardbox_rect_set_location(card7, ui_handle.card7.rect.idx, ui_handle.card7.rect.x, ui_handle.card7.rect.y, ui_handle.card7.rect.w, ui_handle.card7.rect.h, ui_handle.card7.rect.r);
    compo_cardbox_rect_set_color(card7, ui_handle.card7.rect.idx, make_color(ui_handle.card7.rect.color.r, ui_handle.card7.rect.color.g, ui_handle.card7.rect.color.b));

    compo_cardbox_text_set_font(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.res);
    compo_cardbox_text_set_align_center(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.center);
    widget_text_set_color(card7->text[ui_handle.card7.text_last_use.idx], make_color(ui_handle.card7.text_last_use.color.r, ui_handle.card7.text_last_use.color.g, ui_handle.card7.text_last_use.color.b));
    compo_cardbox_text_set_location(card7, ui_handle.card7.text_last_use.idx, ui_handle.card7.text_last_use.x, ui_handle.card7.text_last_use.y, ui_handle.card7.text_last_use.w, ui_handle.card7.text_last_use.h);
    compo_cardbox_text_set(card7, ui_handle.card7.text_last_use.idx, i18n[ui_handle.card7.text_last_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card7.pic_click)/sizeof(ui_handle.card7.pic_click[0]); i++)
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i));
        compo_button_set_location(btn,ui_handle.card7.pic_click[i].x,ui_handle.card7.pic_click[i].y, ui_handle.card7.pic_click[i].w, ui_handle.card7.pic_click[i].h);
        compo_setid(btn,ui_handle.card7.pic_click[i].idx);
    }

    ///卡片6
    compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card6.w, ui_handle.card6.h);
    compo_setid(card6, ui_handle.card6.id);

    compo_cardbox_rect_set_location(card6, ui_handle.card6.rect.idx, ui_handle.card6.rect.x, ui_handle.card6.rect.y, ui_handle.card6.rect.w, ui_handle.card6.rect.h, ui_handle.card6.rect.r);
    compo_cardbox_rect_set_color(card6, ui_handle.card6.rect.idx, make_color(ui_handle.card6.rect.color.r, ui_handle.card6.rect.color.g, ui_handle.card6.rect.color.b));

    compo_cardbox_text_set_font(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.res);
    compo_cardbox_text_set_align_center(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.center);
    widget_text_set_color(card6->text[ui_handle.card6.text_comm_use.idx], make_color(ui_handle.card6.text_comm_use.color.r, ui_handle.card6.text_comm_use.color.g, ui_handle.card6.text_comm_use.color.b));
    compo_cardbox_text_set_location(card6, ui_handle.card6.text_comm_use.idx, ui_handle.card6.text_comm_use.x, ui_handle.card6.text_comm_use.y, ui_handle.card6.text_comm_use.w, ui_handle.card6.text_comm_use.h);
    compo_cardbox_text_set(card6, ui_handle.card6.text_comm_use.idx, i18n[ui_handle.card6.text_comm_use.str_id]);

    for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
    {
        compo_cardbox_icon_set_location(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].x, ui_handle.card6.pic_click[i].y, ui_handle.card6.pic_click[i].w, ui_handle.card6.pic_click[i].h);
        compo_cardbox_icon_set(card6, ui_handle.card6.pic_click[i].idx, ui_handle.card6.pic_click[i].res);
    }


    ///卡片5
    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card5.w, ui_handle.card5.h);
    compo_setid(card5, ui_handle.card5.id);

    compo_cardbox_rect_set_location(card5, ui_handle.card5.rect.idx, ui_handle.card5.rect.x, ui_handle.card5.rect.y, ui_handle.card5.rect.w, ui_handle.card5.rect.h, ui_handle.card5.rect.r);
    compo_cardbox_rect_set_color(card5, ui_handle.card5.rect.idx, make_color(ui_handle.card5.rect.color.r, ui_handle.card5.rect.color.g, ui_handle.card5.rect.color.b));

//    compo_cardbox_text_set_font(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_stopwatch.idx], make_color(ui_handle.card5.text_stopwatch.color.r, ui_handle.card5.text_stopwatch.color.g, ui_handle.card5.text_stopwatch.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_stopwatch.idx, ui_handle.card5.text_stopwatch.x, ui_handle.card5.text_stopwatch.y, ui_handle.card5.text_stopwatch.w, ui_handle.card5.text_stopwatch.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_stopwatch.idx, i18n[ui_handle.card5.text_stopwatch.str_id]);

    compo_cardbox_text_set_font(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.res);
    compo_cardbox_text_set_align_center(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.center);
    widget_text_set_color(card5->text[ui_handle.card5.text_time.idx], make_color(ui_handle.card5.text_time.color.r, ui_handle.card5.text_time.color.g, ui_handle.card5.text_time.color.b));
    compo_cardbox_text_set_location(card5, ui_handle.card5.text_time.idx, ui_handle.card5.text_time.x, ui_handle.card5.text_time.y, ui_handle.card5.text_time.w, ui_handle.card5.text_time.h);
    compo_cardbox_text_set(card5, ui_handle.card5.text_time.idx, "00:00.00");

    ///卡片4
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, ui_handle.card4.w, ui_handle.card4.h);
    compo_setid(card4, ui_handle.card4.id);

    compo_cardbox_rect_set_location(card4, ui_handle.card4.rect.idx, ui_handle.card4.rect.x, ui_handle.card4.rect.y, ui_handle.card4.rect.w, ui_handle.card4.rect.h, ui_handle.card4.rect.r);
    compo_cardbox_rect_set_color(card4, ui_handle.card4.rect.idx, make_color(ui_handle.card4.rect.color.r, ui_handle.card4.rect.color.g, ui_handle.card4.rect.color.b));

    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_timer.idx], make_color(ui_handle.card4.text_timer.color.r, ui_handle.card4.text_timer.color.g, ui_handle.card4.text_timer.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_timer.idx, ui_handle.card4.text_timer.x, ui_handle.card4.text_timer.y, ui_handle.card4.text_timer.w, ui_handle.card4.text_timer.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_timer.idx, i18n[ui_handle.card4.text_timer.str_id]);

    compo_cardbox_text_set_font(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.res);
    compo_cardbox_text_set_align_center(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.center);
    widget_text_set_color(card4->text[ui_handle.card4.text_time.idx], make_color(ui_handle.card4.text_time.color.r, ui_handle.card4.text_time.color.g, ui_handle.card4.text_time.color.b));
    compo_cardbox_text_set_location(card4, ui_handle.card4.text_time.idx, ui_handle.card4.text_time.x, ui_handle.card4.text_time.y, ui_handle.card4.text_time.w, ui_handle.card4.text_time.h);
    compo_cardbox_text_set(card4, ui_handle.card4.text_time.idx, "00:00:00");


    ///卡片3
    compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 3, 1, ui_handle.card3.w, ui_handle.card3.h);
    compo_setid(card3, ui_handle.card3.id);

    compo_cardbox_rect_set_location(card3, ui_handle.card3.rect.idx, ui_handle.card3.rect.x, ui_handle.card3.rect.y, ui_handle.card3.rect.w, ui_handle.card3.rect.h, ui_handle.card3.rect.r);
    compo_cardbox_rect_set_color(card3, ui_handle.card3.rect.idx, make_color(ui_handle.card3.rect.color.r, ui_handle.card3.rect.color.g, ui_handle.card3.rect.color.b));

    compo_cardbox_text_set_font(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.res);
    compo_cardbox_text_set_align_center(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.center);
    widget_text_set_color(card3->text[ui_handle.card3.text_music.idx], make_color(ui_handle.card3.text_music.color.r, ui_handle.card3.text_music.color.g, ui_handle.card3.text_music.color.b));
    compo_cardbox_text_set_location(card3, ui_handle.card3.text_music.idx, ui_handle.card3.text_music.x, ui_handle.card3.text_music.y, ui_handle.card3.text_music.w, ui_handle.card3.text_music.h);
    compo_cardbox_text_set(card3, ui_handle.card3.text_music.idx, i18n[ui_handle.card3.text_music.str_id]);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_prev.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_prev.x, ui_handle.card3.pic_click_prev.y, ui_handle.card3.pic_click_prev.w, ui_handle.card3.pic_click_prev.h);
    compo_setid(btn,ui_handle.card3.pic_click_prev.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_next.res_click);
    compo_button_set_location(btn,ui_handle.card3.pic_click_next.x, ui_handle.card3.pic_click_next.y, ui_handle.card3.pic_click_next.w, ui_handle.card3.pic_click_next.h);
    compo_setid(btn,ui_handle.card3.pic_click_next.idx);

    btn = compo_button_create_page_by_image(frm,card3->page,ui_handle.card3.pic_click_play.res_click);
    compo_button_set_pos(btn,ui_handle.card3.pic_click_play.x, ui_handle.card3.pic_click_play.y);
    compo_setid(btn,ui_handle.card3.pic_click_play.idx);

    ///卡片2
    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 0, 5, ui_handle.card2.w, ui_handle.card2.h);
    compo_setid(card2, ui_handle.card2.id);

    compo_cardbox_rect_set_location(card2, ui_handle.card2.rect.idx, ui_handle.card2.rect.x, ui_handle.card2.rect.y, ui_handle.card2.rect.w, ui_handle.card2.rect.h, ui_handle.card2.rect.r);
    compo_cardbox_rect_set_color(card2, ui_handle.card2.rect.idx, make_color(ui_handle.card2.rect.color.r, ui_handle.card2.rect.color.g, ui_handle.card2.rect.color.b));

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_sleep.idx], make_color(ui_handle.card2.text_sleep.color.r, ui_handle.card2.text_sleep.color.g, ui_handle.card2.text_sleep.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_sleep.idx, ui_handle.card2.text_sleep.x, ui_handle.card2.text_sleep.y, ui_handle.card2.text_sleep.w, ui_handle.card2.text_sleep.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_sleep.idx, i18n[ui_handle.card2.text_sleep.str_id]);

//    sleep_data->totalSleepMin =  9;
    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour.idx], make_color(ui_handle.card2.text_hour.color.r, ui_handle.card2.text_hour.color.g, ui_handle.card2.text_hour.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour.idx, ui_handle.card2.text_hour.x, ui_handle.card2.text_hour.y, ui_handle.card2.text_hour.w, ui_handle.card2.text_hour.h);
    memset(txt_buf,0,sizeof(txt_buf));
    if(sleep_data->totalSleepMin)   ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠小时*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_hour_unit.idx], make_color(ui_handle.card2.text_hour_unit.color.r, ui_handle.card2.text_hour_unit.color.g, ui_handle.card2.text_hour_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_hour_unit.idx, ui_handle.card2.text_hour_unit.x+6, ui_handle.card2.text_hour_unit.y, ui_handle.card2.text_hour_unit.w, ui_handle.card2.text_hour_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_hour_unit.idx, i18n[ui_handle.card2.text_hour_unit.str_id]);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min.idx], make_color(ui_handle.card2.text_min.color.r, ui_handle.card2.text_min.color.g, ui_handle.card2.text_min.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min.idx, ui_handle.card2.text_min.x, ui_handle.card2.text_min.y, ui_handle.card2.text_min.w, ui_handle.card2.text_min.h);
    if(sleep_data->totalSleepMin)  ///是否有睡眠时长
    {
        snprintf(txt_buf, sizeof(txt_buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(txt_buf, sizeof(txt_buf), "--");///* 总睡眠分钟*/
    }
    compo_cardbox_text_set(card2, ui_handle.card2.text_min.idx, txt_buf);

    compo_cardbox_text_set_font(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.res);
    compo_cardbox_text_set_align_center(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.center);
    widget_text_set_color(card2->text[ui_handle.card2.text_min_unit.idx], make_color(ui_handle.card2.text_min_unit.color.r, ui_handle.card2.text_min_unit.color.g, ui_handle.card2.text_min_unit.color.b));
    compo_cardbox_text_set_location(card2, ui_handle.card2.text_min_unit.idx, ui_handle.card2.text_min_unit.x+6, ui_handle.card2.text_min_unit.y, ui_handle.card2.text_min_unit.w, ui_handle.card2.text_min_unit.h);
    compo_cardbox_text_set(card2, ui_handle.card2.text_min_unit.idx, i18n[ui_handle.card2.text_min_unit.str_id]);

    ///卡片1
    compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, ui_handle.card1.w, ui_handle.card1.h);
    compo_setid(card1, ui_handle.card1.id);

    compo_cardbox_rect_set_location(card1, ui_handle.card1.rect.idx, ui_handle.card1.rect.x, ui_handle.card1.rect.y, ui_handle.card1.rect.w, ui_handle.card1.rect.h, ui_handle.card1.rect.r);
    compo_cardbox_rect_set_color(card1, ui_handle.card1.rect.idx, make_color(ui_handle.card1.rect.color.r, ui_handle.card1.rect.color.g, ui_handle.card1.rect.color.b));

    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.x, ui_handle.card1.pic_kcal.y, ui_handle.card1.pic_kcal.w, ui_handle.card1.pic_kcal.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_kcal.idx, ui_handle.card1.pic_kcal.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.x, ui_handle.card1.pic_km.y, ui_handle.card1.pic_km.w, ui_handle.card1.pic_km.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_km.idx, ui_handle.card1.pic_km.res);
    compo_cardbox_icon_set_location(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.x, ui_handle.card1.pic_step.y, ui_handle.card1.pic_step.w, ui_handle.card1.pic_step.h);
    compo_cardbox_icon_set(card1, ui_handle.card1.pic_step.idx, ui_handle.card1.pic_step.res);

    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_kcal.idx], make_color(ui_handle.card1.text_kcal.color.r, ui_handle.card1.text_kcal.color.g, ui_handle.card1.text_kcal.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_kcal.idx, ui_handle.card1.text_kcal.x, ui_handle.card1.text_kcal.y, ui_handle.card1.text_kcal.w, ui_handle.card1.text_kcal.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
    compo_cardbox_text_set(card1, ui_handle.card1.text_kcal.idx, txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_km.idx], make_color(ui_handle.card1.text_km.color.r, ui_handle.card1.text_km.color.g, ui_handle.card1.text_km.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_km.idx, ui_handle.card1.text_km.x, ui_handle.card1.text_km.y, ui_handle.card1.text_km.w, ui_handle.card1.text_km.h);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",KM/100, KM%100);///公里数据
    compo_cardbox_text_set(card1, ui_handle.card1.text_km.idx,txt_buf);

    compo_cardbox_text_set_font(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.res);
    compo_cardbox_text_set_align_center(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.center);
    widget_text_set_color(card1->text[ui_handle.card1.text_step.idx], make_color(ui_handle.card1.text_step.color.r, ui_handle.card1.text_step.color.g, ui_handle.card1.text_step.color.b));
    compo_cardbox_text_set_location(card1, ui_handle.card1.text_step.idx, ui_handle.card1.text_step.x, ui_handle.card1.text_step.y, ui_handle.card1.text_step.w, ui_handle.card1.text_step.h);
    snprintf(txt_buf,sizeof(txt_buf),"%ld",totalStepCnt);
    compo_cardbox_text_set(card1, ui_handle.card1.text_step.idx, txt_buf);

    area_t pic_bg_area = gui_image_get_size(UI_BUF_I332001_FIRSTORDER_ACTIVITY_BG_BIN);
    widget_page_t * widget_page = widget_page_create(card1->page);///创建按键页面
    widget_set_location(widget_page,52,0,pic_bg_area.wid,pic_bg_area.hei);

    compo_picturebox_t *arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_BG_BIN);//彩虹背景
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei/2);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_RED_BIN);//圆弧红/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KCAL);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_YELLOW_BIN);//圆弧黄/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_KM);
    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_BLUE_BIN);//圆弧绿/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei);
    compo_picturebox_set_rotation(arc_pic, 1800 );
    compo_picturebox_set_visible(arc_pic, false);
    compo_setid(arc_pic,COMPO_ID_ARC_STEP);

    arc_pic = compo_picturebox_create_for_page(frm,widget_page,UI_BUF_I332001_FIRSTORDER_ACTIVITY_YUANJIAO1_BIN);//圆弧角背景/
    compo_picturebox_set_pos(arc_pic,pic_bg_area.wid/2,pic_bg_area.hei-gui_image_get_size(UI_BUF_I332001_FIRSTORDER_ACTIVITY_YUANJIAO1_BIN).hei );

    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res);
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg);

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res);
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2);
    compo_datetime_set_start_angle(clock_h, 900);
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata);

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res);
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min);
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2);
    compo_datetime_set_start_angle(clock_m, 900);
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata);

    compo_textbox_t* date = compo_textbox_create(frm, 5);                                 //日期
    compo_setid(date, ui_handle.card_clock_day.date.id);
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res);
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center);
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b));
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata);

    compo_textbox_t* week = compo_textbox_create(frm, 20);                                 //日期
    compo_setid(week, ui_handle.card_clock_day.week.id);
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res);
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center);
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b));
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata);

//    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 6, 36, 6);
//    compo_shape_set_color(shape, make_color(0X4C,0X4C,0X4C));
//    compo_shape_set_radius(shape, 35);

    ab_free(sleep_data);

}

//更新组件位置和大小
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating)
{
    s16 x, y, w, h, card_y, card_w;
    u8 card_id = 0;
    u16 dc;  //当前卡片中心距屏幕中心距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    component_t *compo = (component_t *)compo_pool_get_top();
    while (compo != NULL)
    {
        //获取初始值
        switch (compo->id)
        {
            case COMPO_ID_CLOCK_BG:
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG)
                {
                    printf("COMPO_ID_CLOCK_BG ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.bg_w;
                h = ui_handle.card_clock_day.clock.bg_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_H:
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H)
                {
                    printf("COMPO_ID_CLOCK_H ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.h_w;
                h = ui_handle.card_clock_day.clock.h_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CLOCK_M:
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M)
                {
                    printf("COMPO_ID_CLOCK_M ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.clock.m_w;
                h = ui_handle.card_clock_day.clock.m_h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_DATE:
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE)
                {
                    printf("COMPO_ID_DATE ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.date.w;
                h = ui_handle.card_clock_day.date.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_WEEKDAY:
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY)
                {
                    printf("COMPO_ID_WEEKDAY ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x;
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y;
                w = ui_handle.card_clock_day.week.w;
                h = ui_handle.card_clock_day.week.h;
                card_y = ui_handle.card_clock_day.y;
//                card_id = CARD_ID_CLOCK;
                card_id = ui_handle.card_clock_day.id;
                break;

            case COMPO_ID_CARD_1:
                if (ui_handle.card1.id != COMPO_ID_CARD_1)
                {
                    printf("COMPO_ID_CARD_1 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card1.x;
                y = card_y = ui_handle.card1.y;
                w = ui_handle.card1.w;
                h = ui_handle.card1.h;
                card_id = ui_handle.card1.id;
                break;

            case COMPO_ID_CARD_2:
                if (ui_handle.card2.id != COMPO_ID_CARD_2)
                {
                    printf("COMPO_ID_CARD_2 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card2.x;
                y = card_y = ui_handle.card2.y;
                w = ui_handle.card2.w;
                h = ui_handle.card2.h;
                card_id = ui_handle.card2.id;
                break;

            case COMPO_ID_CARD_3:
                if (ui_handle.card3.id != COMPO_ID_CARD_3)
                {
                    printf("COMPO_ID_CARD_3 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card3.x;
                y = card_y = ui_handle.card3.y;
                w = ui_handle.card3.w;
                h = ui_handle.card3.h;
                card_id = ui_handle.card3.id;
                break;

            case COMPO_ID_CARD_4:
                if (ui_handle.card4.id != COMPO_ID_CARD_4)
                {
                    printf("COMPO_ID_CARD_4 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card4.x;
                y = card_y = ui_handle.card4.y;
                w = ui_handle.card4.w;
                h = ui_handle.card4.h;
                card_id = ui_handle.card4.id;
                break;

            case COMPO_ID_CARD_5:
                if (ui_handle.card5.id != COMPO_ID_CARD_5)
                {
                    printf("COMPO_ID_CARD_5 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card5.x;
                y = card_y = ui_handle.card5.y;
                w = ui_handle.card5.w;
                h = ui_handle.card5.h;
                card_id = ui_handle.card5.id;
                break;


            case COMPO_ID_CARD_6:
                if (ui_handle.card6.id != COMPO_ID_CARD_6)
                {
                    printf("COMPO_ID_CARD_6 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card6.x;
                y = card_y = ui_handle.card6.y;
                w = ui_handle.card6.w;
                h = ui_handle.card6.h;
                card_id = ui_handle.card6.id;
                break;


            case COMPO_ID_CARD_7:
                if (ui_handle.card7.id != COMPO_ID_CARD_7)
                {
                    printf("COMPO_ID_CARD_7 ERR\n");
                    halt(HALT_GUI);
                }
                x = ui_handle.card7.x;
                y = card_y = ui_handle.card7.y;
                w = ui_handle.card7.w;
                h = ui_handle.card7.h;
                card_id = ui_handle.card7.id;
                break;

            default:
                compo = compo_get_next(compo);
                continue;
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;
        card_y += ofs_y;
        y += ofs_y;
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);
        if (creating == false && dc < dc_min)    //焦点，创建时不使用f_cb
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;
            f_card->focus_card = card_id;
            dc_min = dc;
//            TRACE("focus[%d]\n", card_id);
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM)
        {
            if (card_y > CARD_SCALE_STOP_Y)    //缩到最小后保持一段时间
            {
                y -= card_y - CARD_SCALE_STOP_Y;
                card_y = CARD_SCALE_STOP_Y;
            }
            if (card_w > CARD_WIDTH_MIN)
            {
                //缩放
                s16 card_w_org = card_w;
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                w = w * card_w / card_w_org;
                h = h * card_w / card_w_org;
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org;
                y += (card_y - y) * (card_w_org - card_w) / card_w_org;
                //偏移
                s16 card_y_org = card_y;
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS);
                y += card_y - card_y_org;
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM;
        if (compo->type == COMPO_TYPE_CARDBOX)
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level));   //背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level));
            if (unvisible)
            {
                compo_cardbox_set_visible(cardbox, false);
            }
            else
            {
                compo_cardbox_set_visible(cardbox, true);
            }
        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo;
            compo_picturebox_set_pos(pic, x, y);
            compo_picturebox_set_size(pic, w, h);
            if (unvisible)
            {
                compo_picturebox_set_visible(pic, false);
            }
            else
            {
                compo_picturebox_set_visible(pic, true);
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;
            compo_textbox_set_location(txt, x, y, w, h);
            if (unvisible)
            {
                compo_textbox_set_visible(txt, false);
            }
            else
            {
                compo_textbox_set_visible(txt, true);
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;
            compo_datetime_set_pos(dt, x, y);
        }

        compo = compo_get_next(compo);          //遍历组件
    }
}

//创建上拉卡片菜单（文件内调用）
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y)
{
    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
//    compo_shape_set_alpha(masklayer, 160);
    //创建所有组件
    func_clock_sub_card_compo_create(frm);
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);

    return frm;
}

//创建上拉卡片菜单（供外部调用）
compo_form_t *func_clock_sub_card_form_create(void)
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX);
}

//获取点击的卡片组件id
static u16 func_clock_sub_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<CARD_BTN_COUNT; i++)
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;
        id = COMPO_CARD_START + 1 + i;
        cardbox = compo_getobj_byid(id);
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}

//表盘上拉卡片菜单点击处理
static void func_clock_sub_card_click_handler(void)
{
    compo_cardbox_t *cardbox = NULL;
//    rect_t rect;
    u8 func_jump = FUNC_NULL;
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);
    printf("click compo_id:%d\n", compo_id);
    int id = compo_get_button_id();

    switch(compo_id)
    {
        case COMPO_ID_CARD_1 :
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            func_jump = FUNC_ACTIVITY;
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
            break;
        case COMPO_ID_CARD_2 :
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
            func_jump = FUNC_SLEEP;
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
            break;
        case COMPO_ID_CARD_3:
            switch(id)
            {
                case COMPO_MUSIC_BTN_PREV:
                {
                    uteModuleMusicCtrl(false,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV);
                    compo_button_set_bgimg(btn,ui_handle.card3.pic_click_prev.res);
                }
                break;
                case COMPO_MUSIC_BTN_PLAY:
                {
                    uteModuleMusicCtrlPaused(false);
                    if(ble_is_connect())
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
                    }
//                    if(bt_cb.music_playing)
//                    {
//                        compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY);
//                        compo_button_set_bgimg(btn, ui_handle.card3.pic_click_play.res_click);
//                    }
                }
                break;
                case COMPO_MUSIC_BTN_NEXT:
                {
                    uteModuleMusicCtrl(true,true,false);
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);
                    compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res);
                }
                break;
                default:
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    func_jump = FUNC_BT;
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
                    break;
            }
            break;
        case COMPO_ID_CARD_4 :
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
            func_jump = FUNC_TIMER;
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
            break;
        case COMPO_ID_CARD_5 :
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            func_jump = FUNC_STOPWATCH;
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
            break;
        case COMPO_ID_CARD_6 :
            cardbox = compo_getobj_byid(COMPO_ID_CARD_6);
            for (u8 i=0; i<sizeof(ui_handle.card6.pic_click)/sizeof(ui_handle.card6.pic_click[0]); i++)
            {
                rect_t rect = compo_cardbox_get_icon_absolute(cardbox, ui_handle.card6.pic_click[i].idx); //上一首
                if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
                {
//                    printf("rect = [%d,%d,%d,%d]\n", rect.x, rect.y, rect.wid, rect.hei);
                    func_cb.sta = ui_handle.card6.pic_click[i].func_sta;
                }
            }
            break;
        case COMPO_ID_CARD_7 :
            printf("id : %d\n", id);
            if (id >= COMPO_ID_APP1)
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1);
            }
            break;
        default:
            break;
    }
    if(func_jump != FUNC_NULL)
    {
        printf("func_jump=%d\n", func_jump);
        func_cb.sta = func_jump;  //直接跳转
    }
}

//根据偏移位置把最近的卡片对齐到屏幕中央
static s16 func_clock_sub_card_align_y(s16 ofs_y)
{
    u8 i, nearest_id = 0;
    u16 dc_min = GUI_SCREEN_CENTER_Y;
    u16 dc;
    if (ofs_y >= DRAG_Y_MAX)
    {
        return DRAG_Y_MAX;  //回弹效果
    }
    else if (ofs_y <= DRAG_Y_MIN)
    {
        return DRAG_Y_MIN;
    }
//    for (i=0; i<CARD_COUNT; i++)
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y);
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y);
        if (dc < dc_min)
        {
            nearest_id = i;
            dc_min = dc;
        }
    }
//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id));
}

//根据卡片序号对齐页面偏移位置到屏幕中央
static s16 func_clock_sub_card_align_by_idx(s8 card_id)
{
//    if (card_id <= CARD_ID_CLOCK)
    if (card_id <= COMPO_CARD_START)
    {
        return DRAG_Y_MAX;
    }
//    else if (card_id > (CARD_COUNT - 1))
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1))
    {
        return DRAG_Y_MIN;
    }
    else
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id));
    }
}

//限制滑动偏移位置
static s16 func_clock_sub_card_limit_y(s16 ofs_y)
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));
    return ofs_y;
}

//设置页面偏移位置
static void func_clock_sub_card_set_offs(s16 ofs_y)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    f_card->ofs_y = ofs_y;
    func_clock_sub_card_compo_update(f_card->ofs_y, false);
}

//时钟表盘上拉菜单数据更新----------------------------------------->todo
static void func_clock_sub_card_data_update(void)
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;
    compo_cardbox_t *cardbox = NULL;
    char txt_buf[20];

    if(f_card->flag_drag == 0)
    {
        card1_updata_disp();
    }

//    if(f_activity->activity_state != 2) return;

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec));
    cardbox = compo_getobj_byid(ui_handle.card4.id);
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf);

    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100;
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;
    u16 msec = sys_cb.stopwatch_total_msec % 1000;
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)
    {
        min  = 99;
        sec  = 59;
        msec = 999;
    }
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld.%02ld",min,sec,msec/10);
    cardbox = compo_getobj_byid(ui_handle.card5.id);
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf);
}

//下拉返回表盘
static void func_clock_sub_card_switch_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y);
    func_cb.frm_main = frm;
    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
}

void music_data_refresh(void)
{
    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); //播放/暂停
    if(ble_is_connect() || uteModuleCallBtIsConnected())
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I332001_FIRSTORDER_MUSIC_PAUSED_BIN : ui_handle.card3.pic_click_play.res);
    }
    else
    {
        compo_button_set_bgimg(btn,ui_handle.card3.pic_click_play.res_click);
    }
}

#endif


#if 0

#elif 1
#define CARD_SCALE_START_POS            190//265                                                                     //卡片开始缩小位置
#define CARD_SCALE_STOP_POS             230//320                                                                     //卡片停止缩小位置
#define CARD_SCALE_STOP_Y               (CARD_SCALE_STOP_POS + 156 * 2)                                         //卡片停止缩小实际位置（越大缩小越慢，可见叠加越多）
#define CARD_WIDTH_MIN                  150//200                                                                     //卡片缩小的最小宽度
#define CARD_UNVISIBLE_POS_TOP          (-100)//(-160)                                                                  //卡片超出屏幕顶部某位置后设为不可见
#define CARD_UNVISIBLE_POS_BOTTOM       (CARD_SCALE_STOP_Y + 156)                                               //卡片缩到最小后实际位置大于某值设为不可见
#define CARD_BG_GRAY_START              41                                                                      //卡片开始缩小位置处背景的灰度
#define CARD_BG_GRAY_STOP               10                                                                      //卡片停止缩小位置处背景的灰度
#define GRAY_LV(x)                      make_color(x, x, x)                                                     //生成灰度颜色值（0:黑 255:白）

#define SPRING_Y_MAX                    0                                                                       //回弹到顶部时的ofs_y
//#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - card_y_info[CARD_COUNT - 1] + 50)                     //回弹到底部时的ofs_y
#define SPRING_Y_MIN                    (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_END-1) + 50)                     //回弹到底部时的ofs_y
#define DRAG_Y_MAX                      (SPRING_Y_MAX + 30)                                                     //拖动到顶部时的ofs_y
#define DRAG_Y_MIN                      (SPRING_Y_MIN - 78)                                                     //拖动到底部时的ofs_y

#define FOCUS_AUTO_TICK_EXPIRE          18                                                                      //松手后自动对齐焦点单位时间(ms)
#define FOCUS_AUTO_STEP                 16                                                                      //松手后自动对齐焦点单位时间步进
#define FOCUS_AUTO_STEP_DIV             6
#define DRAG_AUTO_SPEED                 16                                                                      //拖动松手后的速度(惯性)

#define EXIT_Y_MIN                      0
#define EXIT_Y_MAX                      50//60                                                                      //下滑退出触摸点离屏幕顶部小于某值
#define TXT_CNT_MAX                     8                                                                       //文本框最大字符数
#define CARD_BTN_COUNT                  (COMPO_CARD_END - COMPO_CARD_START - 1)    //卡片（按钮）数量

#define SEC_TO_HOUR(s)  (s / 3600)          //总秒数转换为时分秒（时）
#define SEC_TO_MIN(s)   ((s % 3600) / 60)   //总秒数转换为时分秒（分）
#define SEC_TO_SEC(s)   (s % 60)            //总秒数转换为时分秒（秒）

#define DEFAULT_LATEST_TASK_NUM 4   //最近任务不足4个时默认值补上
// const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY,FUNC_WEATHER};
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_CHATBOT,FUNC_SETTING,FUNC_SET_SUB_ABOUT,FUNC_SET_SUB_LANGUAGE,FUNC_SET_PUFFS};
///卡片、组件原始位置（相对于卡片中心点）
#define CARD_WIDTH_ORG                  208//308

typedef struct ui_handle_t_
{
    struct card_clock_day_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct date_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            u8 bonddata;
            bool center;
            color_t color;
            u32 res;
        } date;

        struct week_t
        {
            u16 id;
            s16 x,y;
            u16 w,h;
            bool center;
            u8 bonddata;
            color_t color;
            u32 res;
        } week;

        struct clock_t
        {
            u16 id_bg;
            u16 id_h;
            u16 id_min;
            s16 bg_x,bg_y,h_x,h_y,m_x,m_y;
            u16 bg_w,bg_h,h_w,h_h,m_w,m_h;
            u32 bg_res, h_res, m_res;
            u8 h_bonddata, m_bonddata;
        } clock;
    } card_clock_day;

    struct card1_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u16 r;
            color_t color;
        } rect;

        struct pic_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
        } pic_kcal;
        struct pic_t pic_km;
        struct pic_t pic_step;

        struct sport_arc_t
        {
            u16 id;
            s16 x,y;
            u16 w,r;
            color_t color;
            color_t bg_color;
            u16 zero_angle;
            u16 angle_min;
            u16 angle_max;
        } arc_kcal;
        struct sport_arc_t arc_km;
        struct sport_arc_t arc_step;

        struct text_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u16 str_id;
            bool center;
            color_t color;
        } text_kcal;
        struct text_t text_km;
        struct text_t text_step;
    } card1;

    struct card2_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_sleep;
        struct text_t text_hour;
        struct text_t text_hour_unit;
        struct text_t text_min;
        struct text_t text_min_unit;
    } card2;

    struct card3_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_music;

        struct pic_click_t
        {
            u16 idx;
            s16 x,y;
            u16 w,h;
            u32 res;
            u32 res_click;
            u32 res_switch;
            u16 func_sta;
        } pic_click_prev;
        struct pic_click_t pic_click_next;
        struct pic_click_t pic_click_play;
    } card3;

    struct card4_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_timer;
        struct text_t text_time;
    } card4;

    struct card5_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_stopwatch;
        struct text_t text_time;
    } card5;

    struct card6_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_comm_use;
        struct pic_click_t pic_click[3];
    } card6;

    struct card7_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;

        struct rect_t rect;

        struct text_t text_last_use;
        struct pic_click_t pic_click[3];
    } card7;

} ui_handle_t;
//组件ID
enum
{
    COMPO_ID_CLOCK_BG = 256,    // 时钟背景图片
    COMPO_ID_CLOCK_H,           // 时钟时针 (257)
    COMPO_ID_CLOCK_M,           // 时钟分针 (258)
    COMPO_ID_DATE,              // 日期显示 (259)
    COMPO_ID_WEEKDAY,           // 星期显示 (260)

    COMPO_CARD_START,           // 卡片开始标记 (261)
//   COMPO_ID_CARD_1,            // 第1个卡片 - 计算器 (262)
    COMPO_ID_CARD_2,            // 第2个卡片 - 音乐 (263)
    // COMPO_ID_CARD_3,         // 第3个卡片 - 已注释
    COMPO_ID_CARD_4,            // 第4个卡片 - 定时器 (264)
    COMPO_ID_CARD_5,            // 第5个卡片 - 秒表 (265)
    //COMPO_ID_CARD_6,            // 第6个卡片 - 日历 (266)
    COMPO_ID_CARD_7,            // 第7个卡片 - 最近应用 (267)
    COMPO_CARD_END,             // 卡片结束标记 (268)

    COMPO_ID_ARC_KCAL,          // 卡路里弧形进度条 (269)
    COMPO_ID_ARC_KM,            // 距离弧形进度条 (270)
    COMPO_ID_ARC_STEP,          // 步数弧形进度条 (271)

    COMPO_MUSIC_BTN_PREV,       // 上一首按钮 (272)
    COMPO_MUSIC_BTN_NEXT,       // 下一首按钮 (273)
    COMPO_MUSIC_BTN_PLAY,       // 播放/暂停按钮 (274)

    COMPO_ID_APP1,              // 应用1快捷方式 (275)
    COMPO_ID_APP2,              // 应用2快捷方式 (276)
    COMPO_ID_APP3,              // 应用3快捷方式 (277)
    COMPO_ID_APP4,              // 应用4快捷方式 (278)
};

#define SUB_CARD_W 208    // 子卡片宽度：208像素  
#define SUB_CARD_H 94     // 子卡片高度：94像素    
#define SUB_CARD_X 16     // 子卡片X坐标偏移：16像素  
// #define SUB_CARD_Y
static const ui_handle_t ui_handle =
{
    //卡片时钟日期容器配置
    .card_clock_day = {
        .id = COMPO_CARD_START,           // 组件ID，对应枚举中的卡片开始标记
        .x  = GUI_SCREEN_CENTER_X,        // X坐标：屏幕中心X位置
        .y  = 24+100/2,                   // Y坐标：24 + 50 = 74像素
        .w  = 0,                          // 宽度
        .h  = 0,                          // 高度

        //日期显示组件配置
        .date = {
            .id = COMPO_ID_DATE,                       // 组件ID，对应日期显示组件
            .x  = SUB_CARD_X-SUB_CARD_W/2,             // X坐标：16 - 208/2 = -88像素（相对于卡片中心）
            .y  = 41-136/2,                            // Y坐标：41 - 68 = -27像素（相对于卡片中心）
            .w  = 80+20,                               // 宽度：100像素
            .h  = 40,                                  // 高度：40像素
            .center = false,                           // 文本不居中对齐
            .bonddata = COMPO_BOND_DATE,               // 数据绑定类型：日期数据
            .color = {255,255,255},                    // 文本颜色：白色(RGB)
            .res = UI_BUF_0FONT_FONT_NUM_32_BIN,       // 字体资源：32号数字字体
        },

        .week = {
            .id = COMPO_ID_WEEKDAY,
            .x  = SUB_CARD_X-SUB_CARD_W/2,
            .y  = 79-136/2,
            .w  = 45,
            .h  = 32,
            .center = false,
            .bonddata = COMPO_BOND_WEEKDAY,
            .color = {153,153,153},
            .res = UI_BUF_0FONT_FONT_BIN,
        },

        .clock = {
            .id_bg  = COMPO_ID_CLOCK_BG,
            .id_h   = COMPO_ID_CLOCK_H,
            .id_min = COMPO_ID_CLOCK_M,
            .bg_x   = 100+100/2-SUB_CARD_W/2,
            .bg_y   = 0,
            .h_x    = 100+96/2-SUB_CARD_W/2,
            .h_y    = 0,
            .m_x    = 100+96/2-SUB_CARD_W/2,
            .m_y    = 0,
            .bg_w   = 100,
            .bg_h   = 100,
            .h_w    = 53,
            .h_h    = 18,
            .m_w    = 65,
            .m_h    = 18,
            .bg_res = UI_BUF_I330001_FIRSTORDER_TIME_BG_BIN,
            .h_res  = UI_BUF_I330001_FIRSTORDER_TIME_H_BIN,
            .m_res  = UI_BUF_I330001_FIRSTORDER_TIME_M_BIN,
            .h_bonddata = COMPO_BOND_HOUR,
            .m_bonddata = COMPO_BOND_MINUTE,
        },
    },

    // .card1 = {                                                      // 定义第一个卡片（计算器卡片）的配置结构体
    //     .id = COMPO_ID_CARD_1,                                      // 设置卡片的组件ID为COMPO_ID_CARD_1（值为262），用于在系统中唯一标识这个卡片
    //     .x  = SUB_CARD_X+SUB_CARD_W/2,                              // 设置卡片的X坐标：16+208/2=120像素，这是卡片中心点的X位置
    //     .y  = 190,                                                  // 设置卡片的Y坐标为190像素，这是卡片中心点的Y位置
    //     .w  = SUB_CARD_W,                                           // 设置卡片宽度为SUB_CARD_W（208像素）
    //     .h  = SUB_CARD_H,                                           // 设置卡片高度为SUB_CARD_H（94像素）

    //     .rect = {                                                   // 定义卡片背景矩形的配置
    //         .idx    = 0,                                            // 矩形索引号为0，用于标识这是第一个矩形组件
    //         .x      = 0,                                            // 矩形相对于卡片中心的X偏移为0（居中）
    //         .y      = 0,                                            // 矩形相对于卡片中心的Y偏移为0（居中）
    //         .w      = SUB_CARD_W,                                   // 矩形宽度为208像素，与卡片宽度相同
    //         .h      = SUB_CARD_H,                                   // 矩形高度为94像素，与卡片高度相同
    //         .r      = 16,                                           // 矩形圆角半径为16像素，创建圆角效果
    //         .color  = {41,41,41},                                   // 矩形背景颜色为深灰色，RGB值为(41,41,41)
    //     },
    //     .pic_kcal = {                                               // 定义卡路里图标的配置
    //         .idx    = 0,                                            // 图标索引号为0，标识这是第一个图标
    //         .x      = 10+18/2-SUB_CARD_W/2,                         // 图标X坐标：10+9-104=-85像素，相对于卡片中心向左偏移85像素
    //         .y      = 21+18/2-SUB_CARD_H/2,                         // 图标Y坐标：21+9-47=-17像素，相对于卡片中心向上偏移17像素
    //         .w      = 18,                                           // 图标宽度为18像素
    //         .h      = 18,                                           // 图标高度为18像素
    //         .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_CALORIES_BIN, // 图标资源文件，显示卡路里图标
    //     },
    //     .pic_km = {                                                 // 定义距离图标的配置
    //         .idx    = 1,                                            // 图标索引号为1，标识这是第二个图标
    //         .x      = 10+18/2-SUB_CARD_W/2,                         // 图标X坐标：10+9-104=-85像素，与卡路里图标X坐标相同（垂直对齐）
    //         .y      = 46+18/2-SUB_CARD_H/2,                         // 图标Y坐标：46+9-47=8像素，相对于卡片中心向下偏移8像素
    //         .w      = 18,                                           // 图标宽度为18像素
    //         .h      = 18,                                           // 图标高度为18像素
    //         .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_DIS_BIN,   // 图标资源文件，显示距离图标
    //     },
    //     .pic_step = {                                               // 定义步数图标的配置
    //         .idx    = 2,                                            // 图标索引号为2，标识这是第三个图标
    //         .x      = 10+18/2-SUB_CARD_W/2,                         // 图标X坐标：10+9-104=-85像素，与前两个图标X坐标相同（垂直对齐）
    //         .y      = 71+18/2-SUB_CARD_H/2,                         // 图标Y坐标：71+9-47=33像素，相对于卡片中心向下偏移33像素
    //         .w      = 18,                                           // 图标宽度为18像素
    //         .h      = 18,                                           // 图标高度为18像素
    //         .res    = UI_BUF_I330001_FIRSTORDER_ACTIVITY_STEPT_BIN, // 图标资源文件，显示步数图标
    //     },

    //     .arc_kcal = {                                               // 定义卡路里弧形进度条的配置
    //         .id     = COMPO_ID_ARC_KCAL,                            // 设置弧形进度条的组件ID为COMPO_ID_ARC_KCAL（值为269）
    //         .x      = 25,                                           // 弧形进度条中心点X坐标为25像素（相对于卡片）
    //         .y      = 50,                                           // 弧形进度条中心点Y坐标为50像素（相对于卡片）
    //         .w      = 10,                                           // 弧形进度条线条宽度为10像素
    //         .r      = 100,                                          // 弧形进度条半径为100像素（最外层弧形）
    //         .color  = {252,55,40},                                  // 弧形进度条颜色为红色，RGB值为(252,55,40)
    //         .bg_color = {0x7f,0x6e,0x19},                           // 弧形进度条背景颜色，十六进制RGB值为(127,110,25)
    //         .zero_angle = 2700,                                     // 弧形起始角度为270度（6点钟方向），以十分之一度为单位
    //         .angle_min  = 0,                                        // 弧形最小角度为0度
    //         .angle_max  = 1800,                                     // 弧形最大角度为180度，以十分之一度为单位（实际18度）
    //     },
    //     .arc_km = {                                                 // 定义距离弧形进度条的配置
    //         .id     = COMPO_ID_ARC_KM,                              // 设置弧形进度条的组件ID为COMPO_ID_ARC_KM（值为270）
    //         .x      = 25,                                           // 弧形进度条中心点X坐标为25像素，与卡路里弧形同心
    //         .y      = 50,                                           // 弧形进度条中心点Y坐标为50像素，与卡路里弧形同心
    //         .w      = 10,                                           // 弧形进度条线条宽度为10像素
    //         .r      = 100-12,                                       // 弧形进度条半径为88像素（比卡路里弧形小12像素，形成同心圆）
    //         .color  = {255,212,0},                                  // 弧形进度条颜色为黄色，RGB值为(255,212,0)
    //         .bg_color = {0x19,0x7a,0x6f},                           // 弧形进度条背景颜色，十六进制RGB值为(25,122,111)
    //         .zero_angle = 2700,                                     // 弧形起始角度为270度（6点钟方向）
    //         .angle_min  = 0,                                        // 弧形最小角度为0度
    //         .angle_max  = 1800,                                     // 弧形最大角度为180度，以十分之一度为单位
    //     },
    //     .arc_step = {                                               // 定义步数弧形进度条的配置
    //         .id     = COMPO_ID_ARC_STEP,                            // 设置弧形进度条的组件ID为COMPO_ID_ARC_STEP（值为271）
    //         .x      = 25,                                           // 弧形进度条中心点X坐标为25像素，与前两个弧形同心
    //         .y      = 50,                                           // 弧形进度条中心点Y坐标为50像素，与前两个弧形同心
    //         .w      = 10,                                           // 弧形进度条线条宽度为10像素
    //         .r      = 100-24,                                       // 弧形进度条半径为76像素（比卡路里弧形小24像素，最内层弧形）
    //         .color  = {0,242,214},                                  // 弧形进度条颜色为青色，RGB值为(0,242,214)
    //         .bg_color = {0x7e,0x2f,0x29},                           // 弧形进度条背景颜色，十六进制RGB值为(126,47,41)
    //         .zero_angle = 2700,                                     // 弧形起始角度为270度（6点钟方向）
    //         .angle_min  = 0,                                        // 弧形最小角度为0度
    //         .angle_max  = 1800,                                     // 弧形最大角度为180度，以十分之一度为单位
    //     },
    //     .text_kcal = {                                              // 定义卡路里数值文本的配置
    //         .idx    = 0,                                            // 文本索引号为0，标识这是第一个文本组件
    //         .x      = 34-SUB_CARD_W/2,                              // 文本X坐标：34-104=-70像素，相对于卡片中心向左偏移70像素
    //         .y      = 21+18/2-SUB_CARD_H/2-10,                      // 文本Y坐标：21+9-47-10=-27像素，相对于卡片中心向上偏移27像素
    //         .w      = 80,                                           // 文本框宽度为80像素
    //         .h      = 24,                                           // 文本框高度为24像素
    //         .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,                 // 文本字体资源，使用20号数字字体
    //         .str_id = 0,                                            // 字符串ID为0，用于标识显示的数据类型
    //         .center = false,                                        // 文本不居中对齐，使用左对齐
    //         .color  = {252,55,40},                                  // 文本颜色为红色，RGB值为(252,55,40)，与卡路里弧形颜色一致
    //     },
    //     .text_km = {                                                // 定义距离数值文本的配置
    //         .idx    = 1,                                            // 文本索引号为1，标识这是第二个文本组件
    //         .x      = 34-SUB_CARD_W/2,                              // 文本X坐标：34-104=-70像素，与卡路里文本X坐标相同（垂直对齐）
    //         .y      = 46+18/2-SUB_CARD_H/2-10,                      // 文本Y坐标：46+9-47-10=-2像素，相对于卡片中心向上偏移2像素
    //         .w      = 80,                                           // 文本框宽度为80像素
    //         .h      = 24,                                           // 文本框高度为24像素
    //         .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,                 // 文本字体资源，使用20号数字字体
    //         .str_id = 1,                                            // 字符串ID为1，用于标识显示的数据类型
    //         .center = false,                                        // 文本不居中对齐，使用左对齐
    //         .color  = {255,212,0},                                  // 文本颜色为黄色，RGB值为(255,212,0)，与距离弧形颜色一致
    //     },
    //     .text_step  = {                                             // 定义步数数值文本的配置
    //         .idx    = 2,                                            // 文本索引号为2，标识这是第三个文本组件
    //         .x      = 34-SUB_CARD_W/2,                              // 文本X坐标：34-104=-70像素，与前两个文本X坐标相同（垂直对齐）
    //         .y      = 71+18/2-SUB_CARD_H/2-10,                      // 文本Y坐标：71+9-47-10=23像素，相对于卡片中心向下偏移23像素
    //         .w      = 80,                                           // 文本框宽度为80像素
    //         .h      = 24,                                           // 文本框高度为24像素
    //         .res    = UI_BUF_0FONT_FONT_NUM_20_BIN,                 // 文本字体资源，使用20号数字字体
    //         .str_id = 2,                                            // 字符串ID为2，用于标识显示的数据类型
    //         .center = false,                                        // 文本不居中对齐，使用左对齐
    //         .color  = {0,242,214},                                  // 文本颜色为青色，RGB值为(0,242,214)，与步数弧形颜色一致
    //     },
    // },

    .card2 = {                                                      // 定义第二个卡片（睡眠数据卡片）的配置结构体
        .id = COMPO_ID_CARD_2,                                      // 设置卡片的组件ID为COMPO_ID_CARD_2（值为263），用于在系统中唯一标识这个卡片
        .x  = SUB_CARD_X+SUB_CARD_W/2,                              // 设置卡片的X坐标：16+208/2=120像素，这是卡片中心点的X位置
        .y  = 190,                                 // 设置卡片的Y坐标：190+(94+6)*1=290像素，相对于第一个卡片向下偏移100像素
        .w  = SUB_CARD_W,                                           // 设置卡片宽度为SUB_CARD_W（208像素）
        .h  = SUB_CARD_H,                                           // 设置卡片高度为SUB_CARD_H（94像素）
        .rect = {                                                   // 定义卡片背景矩形的配置
            .idx    = 0,                                            // 矩形索引号为0，用于标识这是第一个矩形组件
            .x      = 0,                                            // 矩形相对于卡片中心的X偏移为0（居中）
            .y      = 0,                                            // 矩形相对于卡片中心的Y偏移为0（居中）
            .w      = SUB_CARD_W,                                   // 矩形宽度为208像素，与卡片宽度相同
            .h      = SUB_CARD_H,                                   // 矩形高度为94像素，与卡片高度相同
            .r      = 16,                                           // 矩形圆角半径为16像素，创建圆角效果
            .color  = {41,41,41},                                   // 矩形背景颜色为深灰色，RGB值为(41,41,41)
        },

        .text_sleep = {                                             // 定义睡眠标题文本的配置
            .idx    = 0,                                            // 文本索引号为0，标识这是第一个文本组件
            .x      = 12-SUB_CARD_W/2,                              // 文本X坐标：12-104=-92像素，相对于卡片中心向左偏移92像素
            .y      = 10-SUB_CARD_H/2,                              // 文本Y坐标：10-47=-37像素，相对于卡片中心向上偏移37像素
            .w      = 200,                                          // 文本框宽度为200像素
            .h      = 30,                                           // 文本框高度为30像素
            .res    = UI_BUF_0FONT_FONT_BIN,                        // 文本字体资源，使用标准字体
            .str_id = STR_SLEEP,                                    // 字符串ID为STR_SLEEP，显示"睡眠"文字
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {100,95,255},                                 // 文本颜色为紫色，RGB值为(100,95,255)
        },
        .text_hour = {                                              // 定义睡眠小时数值文本的配置
            .idx    = 1,                                            // 文本索引号为1，标识这是第二个文本组件
            .x      = 12-SUB_CARD_W/2,                              // 文本X坐标：12-104=-92像素，与睡眠标题X坐标相同（垂直对齐）
            .y      = 38-SUB_CARD_H/2+6,                            // 文本Y坐标：38-47+6=-3像素，相对于卡片中心向上偏移3像素
            .w      = 41+23,                                        // 文本框宽度为64像素（41+23），用于显示小时数字
            .h      = 46,                                           // 文本框高度为46像素
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,                 // 文本字体资源，使用38号数字字体
            .str_id = STR_NULL,                                     // 字符串ID为STR_NULL，表示动态内容
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {255,255,255},                                // 文本颜色为白色，RGB值为(255,255,255)
        },
        .text_hour_unit = {                                         // 定义小时单位文本的配置
            .idx    = 2,                                            // 文本索引号为2，标识这是第三个文本组件
            .x      = 56-SUB_CARD_W/2,                              // 文本X坐标：56-104=-48像素，相对于卡片中心向左偏移48像素
            .y      = 53-SUB_CARD_H/2,                              // 文本Y坐标：53-47=6像素，相对于卡片中心向下偏移6像素
            .w      = 46,                                           // 文本框宽度为46像素
            .h      = 28,                                           // 文本框高度为28像素
            .res    = UI_BUF_0FONT_FONT_BIN,                        // 文本字体资源，使用标准字体
            .str_id = STR_HOUR,                                     // 字符串ID为STR_HOUR，显示"小时"单位文字
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {255,255,255},                                // 文本颜色为白色，RGB值为(255,255,255)
        },
        .text_min = {                                               // 定义睡眠分钟数值文本的配置
            .idx    = 3,                                            // 文本索引号为3，标识这是第四个文本组件
            .x      = 97-SUB_CARD_W/2+14,                           // 文本X坐标：97-104+14=7像素，相对于卡片中心向右偏移7像素
            .y      = 38-SUB_CARD_H/2+6,                            // 文本Y坐标：38-47+6=-3像素，与小时数值Y坐标相同（水平对齐）
            .w      = 40+23,                                        // 文本框宽度为63像素（40+23），用于显示分钟数字
            .h      = 46,                                           // 文本框高度为46像素
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,                 // 文本字体资源，使用38号数字字体
            .str_id = STR_NULL,                                     // 字符串ID为STR_NULL，表示动态内容
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {255,255,255},                                // 文本颜色为白色，RGB值为(255,255,255)
        },
        .text_min_unit = {                                          // 定义分钟单位文本的配置
            .idx    = 4,                                            // 文本索引号为4，标识这是第五个文本组件
            .x      = 140-SUB_CARD_W/2+12,                          // 文本X坐标：140-104+12=48像素，相对于卡片中心向右偏移48像素
            .y      = 53-SUB_CARD_H/2,                              // 文本Y坐标：53-47=6像素，与小时单位Y坐标相同（水平对齐）
            .w      = 50,                                           // 文本框宽度为50像素
            .h      = 28,                                           // 文本框高度为28像素
            .res    = UI_BUF_0FONT_FONT_BIN,                        // 文本字体资源，使用标准字体
            .str_id = STR_MIN,                                      // 字符串ID为STR_MIN，显示"分钟"单位文字
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {255,255,255},                                // 文本颜色为白色，RGB值为(255,255,255)
        },
    },

    // .card3 = {
    //     .id = COMPO_ID_CARD_3,
    //     .x  = SUB_CARD_X+SUB_CARD_W/2,
    //     .y  = 190+(SUB_CARD_H+6)*2,
    //     .w  = SUB_CARD_W,
    //     .h  = SUB_CARD_H,

    //     .rect = {
    //         .idx    = 0,
    //         .x      = 0,
    //         .y      = 0,
    //         .w      = SUB_CARD_W,
    //         .h      = SUB_CARD_H,
    //         .r      = 16,
    //         .color  = {41,41,41},
    //     },

    //     .text_music = {
    //         .idx    = 0,
    //         .x      = 12-SUB_CARD_W/2,
    //         .y      = 10-SUB_CARD_H/2,
    //         .w      = 200,
    //         .h      = 30,
    //         .res    = UI_BUF_0FONT_FONT_BIN,
    //         .str_id = STR_MUSIC,
    //         .center = false,
    //         .color  = {255,48,96},
    //     },

    //     .pic_click_prev = {
    //         .idx    = COMPO_MUSIC_BTN_PREV,
    //         .x      = 26+28/2-SUB_CARD_W/2,
    //         .y      = 52+28/2-SUB_CARD_H/2,
    //         .w      = 28,
    //         .h      = 28,
    //         .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_UP00_BIN,
    //         .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_UP01_BIN,
    //         .res_switch = 0,
    //         .func_sta = 0,
    //     },
    //     .pic_click_next = {
    //         .idx    = COMPO_MUSIC_BTN_NEXT,
    //         .x      = 178+28/2-SUB_CARD_W/2,
    //         .y      = 52+28/2-SUB_CARD_H/2,
    //         .w      = 28,
    //         .h      = 28,
    //         .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_NEXT00_BIN,
    //         .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_NEXT01_BIN,
    //         .res_switch = 0,
    //         .func_sta = 0,
    //     },
    //     .pic_click_play = {
    //         .idx    = COMPO_MUSIC_BTN_PLAY,
    //         .x      = 88+56/2-SUB_CARD_W/2,
    //         .y      = 38+56/2-SUB_CARD_H/2,
    //         .w      = 28,
    //         .h      = 28,
    //         .res    = UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY00_BIN,
    //         .res_click = UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY01_BIN,
    //         .res_switch = UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN,
    //         .func_sta = 0,
    //     },
    // },

    .card4 = {                                                      // 定义第四个卡片（定时器卡片）的配置结构体
        .id = COMPO_ID_CARD_4,                                      // 设置卡片的组件ID为COMPO_ID_CARD_4（值为264），用于在系统中唯一标识这个卡片
        .x  = SUB_CARD_X+SUB_CARD_W/2,                              // 设置卡片的X坐标：16+208/2=120像素，这是卡片中心点的X位置
        .y  = 190+(SUB_CARD_H+6)*1,                                 // 设置卡片的Y坐标：190+(94+6)*2=390像素，相对于第一个卡片向下偏移200像素
        .w  = SUB_CARD_W,                                           // 设置卡片宽度为SUB_CARD_W（208像素）
        .h  = SUB_CARD_H,                                           // 设置卡片高度为SUB_CARD_H（94像素）

        .rect = {                                                   // 定义卡片背景矩形的配置
            .idx    = 0,                                            // 矩形索引号为0，用于标识这是第一个矩形组件
            .x      = 0,                                            // 矩形相对于卡片中心的X偏移为0（居中）
            .y      = 0,                                            // 矩形相对于卡片中心的Y偏移为0（居中）
            .w      = SUB_CARD_W,                                   // 矩形宽度为208像素，与卡片宽度相同
            .h      = SUB_CARD_H,                                   // 矩形高度为94像素，与卡片高度相同
            .r      = 16,                                           // 矩形圆角半径为16像素，创建圆角效果
            .color  = {41,41,41},                                   // 矩形背景颜色为深灰色，RGB值为(41,41,41)
        },

        .text_timer = {                                             // 定义定时器标题文本的配置
            .idx    = 0,                                            // 文本索引号为0，标识这是第一个文本组件
            .x      = 12-SUB_CARD_W/2,                              // 文本X坐标：12-104=-92像素，相对于卡片中心向左偏移92像素
            .y      = 10-SUB_CARD_H/2,                              // 文本Y坐标：10-47=-37像素，相对于卡片中心向上偏移37像素
            .w      = 200,                                          // 文本框宽度为200像素
            .h      = 30,                                           // 文本框高度为30像素
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,                 // 文本字体资源，使用38号数字字体
            .str_id = STR_TIMER,                                    // 字符串ID为STR_TIMER，显示"定时器"文字
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {248,132,10},                                 // 文本颜色为橙色，RGB值为(248,132,10)
        },
        .text_time = {                                              // 定义定时器时间显示文本的配置
            .idx    = 1,                                            // 文本索引号为1，标识这是第二个文本组件
            .x      = 12-SUB_CARD_W/2,                              // 文本X坐标：12-104=-92像素，与定时器标题X坐标相同（垂直对齐）
            .y      = 37-SUB_CARD_H/2,                              // 文本Y坐标：37-47=-10像素，相对于卡片中心向上偏移10像素
            .w      = 200,                                          // 文本框宽度为200像素
            .h      = 46,                                           // 文本框高度为46像素
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,                 // 文本字体资源，使用38号数字字体
            .str_id = STR_NULL,                                     // 字符串ID为STR_NULL，表示动态内容
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {255,255,255},                                // 文本颜色为白色，RGB值为(255,255,255)
        },
    },

    .card5 = {                                                      // 定义第五个卡片（秒表卡片）的配置结构体
        .id = COMPO_ID_CARD_5,                                      // 设置卡片的组件ID为COMPO_ID_CARD_5（值为265），用于在系统中唯一标识这个卡片
        .x  = SUB_CARD_X+SUB_CARD_W/2,                              // 设置卡片的X坐标：16+208/2=120像素，这是卡片中心点的X位置
        .y  = 190+(SUB_CARD_H+6)*2,                                 // 设置卡片的Y坐标：190+(94+6)*3=490像素，相对于第一个卡片向下偏移300像素
        .w  = SUB_CARD_W,                                           // 设置卡片宽度为SUB_CARD_W（208像素）
        .h  = SUB_CARD_H,                                           // 设置卡片高度为SUB_CARD_H（94像素）

        .rect = {                                                   // 定义卡片背景矩形的配置
            .idx    = 0,                                            // 矩形索引号为0，用于标识这是第一个矩形组件
            .x      = 0,                                            // 矩形相对于卡片中心的X偏移为0（居中）
            .y      = 0,                                            // 矩形相对于卡片中心的Y偏移为0（居中）
            .w      = SUB_CARD_W,                                   // 矩形宽度为208像素，与卡片宽度相同
            .h      = SUB_CARD_H,                                   // 矩形高度为94像素，与卡片高度相同
            .r      = 16,                                           // 矩形圆角半径为16像素，创建圆角效果
            .color  = {41,41,41},                                   // 矩形背景颜色为深灰色，RGB值为(41,41,41)
        },

        .text_stopwatch = {                                         // 定义秒表标题文本的配置
            .idx    = 0,                                            // 文本索引号为0，标识这是第一个文本组件
            .x      = 12-SUB_CARD_W/2,                              // 文本X坐标：12-104=-92像素，相对于卡片中心向左偏移92像素
            .y      = 10-SUB_CARD_H/2,                              // 文本Y坐标：10-47=-37像素，相对于卡片中心向上偏移37像素
            .w      = 200,                                          // 文本框宽度为200像素
            .h      = 30,                                           // 文本框高度为30像素
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,                 // 文本字体资源，使用38号数字字体
            .str_id = STR_STOP_WATCH,                               // 字符串ID为STR_STOP_WATCH，显示"秒表"文字
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {248,132,10},                                 // 文本颜色为橙色，RGB值为(248,132,10)
        },
        .text_time = {                                              // 定义秒表时间显示文本的配置
            .idx    = 1,                                            // 文本索引号为1，标识这是第二个文本组件
            .x      = 12-SUB_CARD_W/2,                              // 文本X坐标：12-104=-92像素，与秒表标题X坐标相同（垂直对齐）
            .y      = 37-SUB_CARD_H/2,                              // 文本Y坐标：37-47=-10像素，相对于卡片中心向上偏移10像素
            .w      = 200,                                          // 文本框宽度为200像素
            .h      = 46,                                           // 文本框高度为46像素
            .res    = UI_BUF_0FONT_FONT_NUM_38_BIN,                 // 文本字体资源，使用38号数字字体
            .str_id = STR_NULL,                                     // 字符串ID为STR_NULL，表示动态内容
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {255,255,255},                                // 文本颜色为白色，RGB值为(255,255,255)
        },
    },

//     .card6 = {                                                      // 定义第六个卡片（常用应用卡片）的配置结构体
//         .id = COMPO_ID_CARD_6,                                      // 设置卡片的组件ID为COMPO_ID_CARD_6（值为266），用于在系统中唯一标识这个卡片
//         .x  = SUB_CARD_X+SUB_CARD_W/2,                              // 设置卡片的X坐标：16+208/2=120像素，这是卡片中心点的X位置
//         .y  = 190+(SUB_CARD_H+6)*4,                                 // 设置卡片的Y坐标：190+(94+6)*4=590像素，相对于第一个卡片向下偏移400像素
//         .w  = SUB_CARD_W,                                           // 设置卡片宽度为SUB_CARD_W（208像素）
//         .h  = SUB_CARD_H,                                           // 设置卡片高度为SUB_CARD_H（94像素）

//         .rect = {                                                   // 定义卡片背景矩形的配置
//             .idx    = 0,                                            // 矩形索引号为0，用于标识这是第一个矩形组件
//             .x      = 0,                                            // 矩形相对于卡片中心的X偏移为0（居中）
//             .y      = 0,                                            // 矩形相对于卡片中心的Y偏移为0（居中）
//             .w      = SUB_CARD_W,                                   // 矩形宽度为208像素，与卡片宽度相同
//             .h      = SUB_CARD_H,                                   // 矩形高度为94像素，与卡片高度相同
//             .r      = 16,                                           // 矩形圆角半径为16像素，创建圆角效果
//             .color  = {41,41,41},                                   // 矩形背景颜色为深灰色，RGB值为(41,41,41)
//         },

//         .text_comm_use = {                                          // 定义常用应用标题文本的配置
//             .idx    = 0,                                            // 文本索引号为0，标识这是第一个文本组件
//             .x      = 12-SUB_CARD_W/2,                              // 文本X坐标：12-104=-92像素，相对于卡片中心向左偏移92像素
//             .y      = 10-SUB_CARD_H/2,                              // 文本Y坐标：10-47=-37像素，相对于卡片中心向上偏移37像素
//             .w      = 200,                                          // 文本框宽度为200像素
//             .h      = 30,                                           // 文本框高度为30像素
//             .res    = UI_BUF_0FONT_FONT_BIN,                        // 文本字体资源，使用标准字体
//             .str_id = STR_COMMON_APP,                               // 字符串ID为STR_COMMON_APP，显示"常用应用"文字
//             .center = false,                                        // 文本不居中对齐，使用左对齐
//             .color  = {255,255,255},                                // 文本颜色为白色，RGB值为(255,255,255)
//         },
//         .pic_click = {                                              // 定义可点击图标数组的配置
//             [0] = {                                                 // 定义第一个可点击图标（运动应用）的配置
//                 .idx    = 0,                                        // 图标索引号为0，标识这是第一个图标
//                 .x      = 27+48/2-SUB_CARD_W/2,                     // 图标X坐标：27+24-104=-53像素，相对于卡片中心向左偏移53像素
//                 .y      = 41+48/2-SUB_CARD_H/2,                     // 图标Y坐标：41+24-47=18像素，相对于卡片中心向下偏移18像素
//                 .w      = 48,                                       // 图标宽度为48像素
//                 .h      = 48,                                       // 图标高度为48像素
//                 .res    = UI_BUF_I330001_FIRSTORDER_FIXED_SPORT_BIN,// 图标资源文件，显示运动图标
//                 .res_click = 0,                                     // 点击时的图标资源（0表示不变）
//                 .res_switch = 0,                                    // 切换状态的图标资源（0表示不变）
// #if UTE_MODULE_SCREENS_SPORT_SUPPORT                            // 如果支持运动模块
//                 .func_sta = FUNC_SPORT,                             // 点击后跳转到运动功能界面
// #endif // UTE_MODULE_SCREENS_SPORT_SUPPORT                      // 运动模块支持条件编译结束
//             },

//             [1] = {                                                 // 定义第二个可点击图标（消息应用）的配置
//                 .idx    = 1,                                        // 图标索引号为1，标识这是第二个图标
//                 .x      = 91+48/2-SUB_CARD_W/2,                     // 图标X坐标：91+24-104=11像素，相对于卡片中心向右偏移11像素
//                 .y      = 41+48/2-SUB_CARD_H/2,                     // 图标Y坐标：41+24-47=18像素，相对于卡片中心向下偏移18像素
//                 .w      = 48,                                       // 图标宽度为48像素
//                 .h      = 48,                                       // 图标高度为48像素
//                 .res    = UI_BUF_I330001_FIRSTORDER_FIXED_MSM_BIN,  // 图标资源文件，显示消息图标
//                 .res_click = 0,                                     // 点击时的图标资源（0表示不变）
//                 .res_switch = 0,                                    // 切换状态的图标资源（0表示不变）
// #if UTE_MODULE_SCREENS_MESSAGE_SUPPORT                          // 如果支持消息模块
//                 .func_sta = FUNC_MESSAGE,                           // 点击后跳转到消息功能界面
// #endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT                   // 消息模块支持条件编译结束
//             },

//             [2] = {                                                 // 定义第三个可点击图标（通话应用）的配置
//                 .idx    = 2,                                        // 图标索引号为2，标识这是第三个图标
//                 .x      = 155+48/2-SUB_CARD_W/2,                    // 图标X坐标：155+24-104=75像素，相对于卡片中心向右偏移75像素
//                 .y      = 41+48/2-SUB_CARD_H/2,                     // 图标Y坐标：41+24-47=18像素，相对于卡片中心向下偏移18像素
//                 .w      = 48,                                       // 图标宽度为48像素
//                 .h      = 48,                                       // 图标高度为48像素
//                 .res    = UI_BUF_I330001_FIRSTORDER_FIXED_CALL_BIN, // 图标资源文件，显示通话图标
//                 .res_click = 0,                                     // 点击时的图标资源（0表示不变）
//                 .res_switch = 0,                                    // 切换状态的图标资源（0表示不变）
// #if UTE_MODULE_SCREENS_CALL_SUPPORT                             // 如果支持通话模块
//                 .func_sta = FUNC_CALL,                              // 点击后跳转到通话功能界面
// #endif // UTE_MODULE_SCREENS_CALL_SUPPORT                       // 通话模块支持条件编译结束
//             },
//         },
//     },

    .card7 = {                                                      // 定义第七个卡片（最近应用卡片）的配置结构体
        .id = COMPO_ID_CARD_7,                                      // 设置卡片的组件ID为COMPO_ID_CARD_7（值为267），用于在系统中唯一标识这个卡片
        .x  = SUB_CARD_X+SUB_CARD_W/2,                              // 设置卡片的X坐标：16+208/2=120像素，这是卡片中心点的X位置
        .y  = 190+(SUB_CARD_H+6)*3+7,                               // 设置卡片的Y坐标：190+(94+6)*5+7=697像素，相对于第一个卡片向下偏移507像素
        .w  = SUB_CARD_W,                                           // 设置卡片宽度为SUB_CARD_W（208像素）
        .h  = SUB_CARD_H,                                           // 设置卡片高度为SUB_CARD_H（94像素）

        .rect = {                                                   // 定义卡片背景矩形的配置
            .idx    = 0,                                            // 矩形索引号为0，用于标识这是第一个矩形组件
            .x      = 0,                                            // 矩形相对于卡片中心的X偏移为0（居中）
            .y      = 0,                                            // 矩形相对于卡片中心的Y偏移为0（居中）
            .w      = SUB_CARD_W,                                   // 矩形宽度为208像素，与卡片宽度相同
            .h      = SUB_CARD_H,                                   // 矩形高度为94像素，与卡片高度相同
            .r      = 16,                                           // 矩形圆角半径为16像素，创建圆角效果
            .color  = {41,41,41},                                   // 矩形背景颜色为深灰色，RGB值为(41,41,41)
        },

        .text_last_use = {                                          // 定义最近应用标题文本的配置
            .idx    = 0,                                            // 文本索引号为0，标识这是第一个文本组件
            .x      = 12-SUB_CARD_W/2,                              // 文本X坐标：12-104=-92像素，相对于卡片中心向左偏移92像素
            .y      = 10-SUB_CARD_H/2,                              // 文本Y坐标：10-47=-37像素，相对于卡片中心向上偏移37像素
            .w      = 200,                                          // 文本框宽度为200像素
            .h      = 30,                                           // 文本框高度为30像素
            .res    = UI_BUF_0FONT_FONT_BIN,                        // 文本字体资源，使用标准字体
            .str_id = STR_LATEST_APP,                               // 字符串ID为STR_LATEST_APP，显示"最近应用"文字
            .center = false,                                        // 文本不居中对齐，使用左对齐
            .color  = {255,255,255},                                // 文本颜色为白色，RGB值为(255,255,255)
        },
        .pic_click = {                                              // 定义可点击图标数组的配置
            [0] = {                                                 // 定义第一个可点击图标（运动应用）的配置
                .idx    = COMPO_ID_APP1,                            // 图标组件ID为COMPO_ID_APP1（值为275），用于标识第一个应用图标
                .x      = 12+44/2-SUB_CARD_W/2,                     // 图标X坐标：12+22-104=-70像素，相对于卡片中心向左偏移70像素
                .y      = 43+44/2-SUB_CARD_H/2,                     // 图标Y坐标：43+22-47=18像素，相对于卡片中心向下偏移18像素
                .w      = 50,                                       // 图标宽度为50像素
                .h      = 50,                                       // 图标高度为50像素
                .res    = UI_BUF_I330001_THEME_1_SOPRT_BIN,         // 图标资源文件，显示运动主题图标
                .res_click = 0,                                     // 点击时的图标资源（0表示不变）
                .res_switch = 0,                                    // 切换状态的图标资源（0表示不变）
#if UTE_MODULE_SCREENS_SPORT_SUPPORT                            // 如果支持运动模块  
                .func_sta = FUNC_SPORT,                             // 点击后跳转到运动功能界面
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT                      // 运动模块支持条件编译结束  
            },
            [1] = {                                                 // 定义第二个可点击图标（亮度应用）的配置
                .idx    = COMPO_ID_APP2,                            // 图标组件ID为COMPO_ID_APP2（值为276），用于标识第二个应用图标
                .x      = 66+44/2-SUB_CARD_W/2,                     // 图标X坐标：66+22-104=-16像素，相对于卡片中心向左偏移16像素
                .y      = 43+44/2-SUB_CARD_H/2,                     // 图标Y坐标：43+22-47=18像素，相对于卡片中心向下偏移18像素
                .w      = 50,                                       // 图标宽度为50像素
                .h      = 50,                                       // 图标高度为50像素
                .res    = UI_BUF_I330001_THEME_1_BRIGHTNESS_BIN,    // 图标资源文件，显示亮度主题图标
                .res_click = 1,                                     // 点击时的图标资源索引为1
                .res_switch = 1,                                    // 切换状态的图标资源索引为1
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT                            // 如果支持亮度模块  
                .func_sta = FUNC_LIGHT,                             // 点击后跳转到亮度功能界面
#endif // UTE_MODULE_SCREENS_LIGHT_SUPPORT                      // 亮度模块支持条件编译结束  
            },
            [2] = {                                                 // 定义第三个可点击图标（定时器应用）的配置
                .idx    = COMPO_ID_APP3,                            // 图标组件ID为COMPO_ID_APP3（值为277），用于标识第三个应用图标
                .x      = 120+44/2-SUB_CARD_W/2,                    // 图标X坐标：120+22-104=38像素，相对于卡片中心向右偏移38像素
                .y      = 43+44/2-SUB_CARD_H/2,                     // 图标Y坐标：43+22-47=18像素，相对于卡片中心向下偏移18像素
                .w      = 50,                                       // 图标宽度为50像素
                .h      = 50,                                       // 图标高度为50像素
                .res    = UI_BUF_I330001_THEME_1_TIMER_BIN,         // 图标资源文件，显示定时器主题图标
                .res_click = 2,                                     // 点击时的图标资源索引为2
                .res_switch = 2,                                    // 切换状态的图标资源索引为2
#if UTE_MODULE_SCREENS_TIMER_SUPPORT                            // 如果支持定时器模块  
                .func_sta = FUNC_TIMER,                             // 点击后跳转到定时器功能界面
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT                      // 定时器模块支持条件编译结束  
            },
            //             [3] = {                                         // 第四个图标配置（已注释掉）
            //                 .idx    = COMPO_ID_APP4,                    // 图标组件ID为COMPO_ID_APP4（值为278）
            //                 .x      = 174+44/2-SUB_CARD_W/2,            // 图标X坐标：174+22-104=92像素
            //                 .y      = 43+44/2-SUB_CARD_H/2,             // 图标Y坐标：43+22-47=18像素
            //                 .w      = 46,                               // 图标宽度为46像素
            //                 .h      = 46,                               // 图标高度为46像素
            //                 .res    = UI_BUF_I330001_THEME_1_CALL_BIN,  // 图标资源文件，显示通话主题图标
            //                 .res_click = 3,                             // 点击时的图标资源索引为3
            //                 .res_switch = 3,                            // 切换状态的图标资源索引为3
            // #if UTE_MODULE_SCREENS_CALL_SUPPORT                         // 如果支持通话模块
            //                 .func_sta = FUNC_CALL,                      // 点击后跳转到通话功能界面
            // #endif // UTE_MODULE_SCREENS_CALL_SUPPORT                   // 通话模块支持条件编译结束
            // },                                              // 第四个图标配置结束（已注释）
        },
    },

};

//获取卡片原始位置
/**
  * @brief  获取卡片原始Y坐标位置
  * @param  card_compo_id: 卡片组件ID
  * @retval int: 返回对应卡片的Y坐标值，如果未找到匹配的ID则返回0
  * @note   该函数用于根据卡片组件ID查找对应卡片在UI配置中的Y坐标
  *         主要用于卡片滚动、对齐和位置计算等功能
  */
static int func_clock_sub_card_get_y_info(int card_compo_id)    // 定义静态函数，根据卡片组件ID获取对应的Y坐标信息
{
    int ret = 0;                                                // 初始化返回值为0，如果没有找到匹配的ID将返回此默认值

    if (card_compo_id == ui_handle.card_clock_day.id)           // 检查传入的ID是否匹配时钟日期卡片的组件ID
    {
        ret = ui_handle.card_clock_day.y;                       // 如果匹配，返回时钟日期卡片的Y坐标（通常为74像素）
    }

    // if (card_compo_id == ui_handle.card1.id)                    // 检查传入的ID是否匹配第一个卡片（计算器卡片）的组件ID
    // {
    //     ret = ui_handle.card1.y;                                // 如果匹配，返回第一个卡片的Y坐标（190像素）
    // }

    if (card_compo_id == ui_handle.card2.id)                    // 检查传入的ID是否匹配第二个卡片（睡眠数据卡片）的组件ID
    {
        ret = ui_handle.card2.y;                                // 如果匹配，返回第二个卡片的Y坐标（290像素）
    }

    // if (card_compo_id == ui_handle.card3.id)                // 第三个卡片的检查逻辑（已注释掉）
    // {
    //     ret = ui_handle.card3.y;                            // 如果匹配，返回第三个卡片的Y坐标（已注释掉）
    // }

    if (card_compo_id == ui_handle.card4.id)                    // 检查传入的ID是否匹配第四个卡片（定时器卡片）的组件ID
    {
        ret = ui_handle.card4.y;                                // 如果匹配，返回第四个卡片的Y坐标（390像素）
    }

    if (card_compo_id == ui_handle.card5.id)                    // 检查传入的ID是否匹配第五个卡片（秒表卡片）的组件ID
    {
        ret = ui_handle.card5.y;                                // 如果匹配，返回第五个卡片的Y坐标（490像素）
    }

    // if (card_compo_id == ui_handle.card6.id)                    // 检查传入的ID是否匹配第六个卡片（日历卡片）的组件ID
    // {
    //     ret = ui_handle.card6.y;                                // 如果匹配，返回第六个卡片的Y坐标（590像素）
    // }

    if (card_compo_id == ui_handle.card7.id)                    // 检查传入的ID是否匹配第七个卡片（最近应用卡片）的组件ID
    {
        ret = ui_handle.card7.y;                                // 如果匹配，返回第七个卡片的Y坐标（697像素）
    }

    return ret;                                                 // 返回找到的Y坐标值，如果没有匹配的ID则返回0
}

//功能结构体
/**
  * @brief  卡片滚动界面功能控制结构体
  * @note   该结构体用于管理时钟表盘上拉菜单中所有卡片的状态、位置、动画和数据
  *         包含滚动控制、拖拽处理、自动对齐、计算器显示等功能
  */
typedef struct f_card_t_                                        // 定义卡片功能控制结构体类型
{
    s32 ofs_y;                                                 // 当前页面Y轴偏移量（像素），用于控制所有卡片的垂直位置
    s32 ofs_y_drag;                                            // 拖拽过程中的临时Y轴偏移量（像素），实时跟随用户手指移动
    s16 focus_card;                                            // 当前焦点卡片的组件ID，表示最靠近屏幕中央的卡片序号
    bool flag_drag;                                            // 拖拽状态标志，true表示用户正在拖动屏幕，false表示未拖动
    bool flag_move_auto;                                       // 自动移动标志，true表示系统正在自动对齐卡片到目标位置
    s32 moveto_y;                                              // 自动移动的目标Y坐标（像素），系统会平滑移动到此位置
    s16 moveto_card;                                           // 自动移动的目标卡片序号，用于记录要对齐到哪个卡片
    u32 tick;                                                  // 时间戳计数器，用于控制自动移动的时间间隔和动画帧率
    u32 activity_tick;                                         // 计算器更新时间戳，用于控制运动卡片数据刷新频率
    uint32_t arc_kcal_value;                                   // 卡路里弧形进度条的当前数值，用于显示用户消耗的卡路里
    uint32_t arc_km_value;                                     // 距离弧形进度条的当前数值，用于显示用户行走的距离（单位：厘米）
    uint32_t arc_step_value;                                   // 步数弧形进度条的当前数值，用于显示用户的步数或动画进度
    u8 activity_state;                                         // 计算器动画状态，0=向上动画，1=向下动画，2=显示实际数据
//    co_timer_t music_refresh;                                // 音乐刷新定时器（已注释），原用于定期更新音乐播放状态
} f_card_t;



/**
  * @brief  更新第一个卡片（计算器卡片）的显示内容
  * @param  None
  * @retval None
  * @note   该函数实现计算器卡片的动画效果和数据显示更新
  *         包含三个状态：向上动画(0)、向下动画(1)、显示实际数据(2)
  *         管理卡路里、距离、步数三个弧形进度条的显示
  */
static void card1_updata_disp(void)                             // 定义计算器卡片显示更新函数
{
    f_card_t *f_activity = (f_card_t *)func_cb.f_cb;           // 获取卡片功能控制结构体指针，用于访问动画状态和数据

//    if(tick_check_expire(f_activity->activity_tick, ))       // 时间检查逻辑（已注释），原用于控制更新频率
//    {
//        f_activity->activity_tick = tick_get();               // 更新时间戳（已注释）
    char txt_buf[20];                                           // 定义文本缓冲区，用于格式化显示的数字文本
    uint32_t totalStepCnt = 0;                                  // 定义总步数变量，存储当天的实际步数
    uint32_t Step_value = 0;                                    // 定义步数值变量，用于计算进度条显示
#if UTE_MODULE_SPORT_SUPPORT                                    // 如果支持运动模块  
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);   // 获取当天的步数统计数据
#endif
    Step_value = totalStepCnt;                                  // 将总步数赋值给步数值变量
#if UTE_MODULE_SPORT_SUPPORT                                    // 如果支持运动模块  
    if(Step_value>uteModuleSportGetStepsTargetCnt())            // 检查实际步数是否超过目标步数
    {
        Step_value = uteModuleSportGetStepsTargetCnt();         // 如果超过目标，则限制为目标步数（防止进度条溢出）
    }
#endif
    compo_picturebox_t *arc_kcal = compo_getobj_byid(COMPO_ID_ARC_KCAL); // 获取卡路里弧形进度条组件指针
    compo_picturebox_t *arc_km   = compo_getobj_byid(COMPO_ID_ARC_KM);   // 获取距离弧形进度条组件指针
    compo_picturebox_t *arc_step = compo_getobj_byid(COMPO_ID_ARC_STEP); // 获取步数弧形进度条组件指针
    compo_cardbox_t *cardbox = compo_getobj_byid(ui_handle.card1.id);    // 获取计算器卡片容器组件指针

    compo_picturebox_set_visible(arc_kcal, true);              // 设置卡路里弧形进度条为可见状态
    compo_picturebox_set_visible(arc_km, true);                // 设置距离弧形进度条为可见状态
    compo_picturebox_set_visible(arc_step, true);              // 设置步数弧形进度条为可见状态


    if(f_activity->activity_state == 0)                        // 检查动画状态是否为0（向上旋转动画阶段）
    {
        f_activity->arc_step_value+=18;                         // 每次调用增加18度（0.1度单位），实现顺时针旋转动画

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value ); // 设置卡路里弧形旋转角度：180度+动画值
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value ); // 设置距离弧形旋转角度：180度+动画值
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value ); // 设置步数弧形旋转角度：180度+动画值

        if(f_activity->arc_step_value >=1800)                  // 检查动画值是否达到180度（完整半圆）
        {
            f_activity->activity_state =1;                     // 切换到状态1（向下旋转动画阶段）
        }
    }
    else if(f_activity->activity_state == 1)                   // 检查动画状态是否为1（向下旋转动画阶段）
    {

        f_activity->arc_step_value-=18;                         // 每次调用减少18度，实现逆时针旋转动画

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value ); // 设置卡路里弧形旋转角度：180度+动画值
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value ); // 设置距离弧形旋转角度：180度+动画值
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value ); // 设置步数弧形旋转角度：180度+动画值
#if UTE_MODULE_SPORT_SUPPORT                                   // 如果支持运动模块  
        if(f_activity->arc_step_value <= Step_value*1800 / uteModuleSportGetStepsTargetCnt()) // 检查动画值是否达到实际步数对应的角度
        {
            f_activity->activity_state = 2;                    // 切换到状态2（显示实际数据阶段）
        }
#endif

    }
    else                                                        // 动画状态为2或其他值（显示实际数据阶段）
    {
#if UTE_MODULE_SPORT_SUPPORT                                   // 如果支持运动模块  
        f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData(); // 获取当天卡路里消耗数据
        f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData(); // 获取当天行走距离数据
        f_activity->arc_step_value =(uint32_t) (Step_value*1000 / uteModuleSportGetStepsTargetCnt()); // 计算步数完成百分比（千分比）
#else                                                           // 如果不支持运动模块  
        f_activity->arc_kcal_value =0;                          // 设置卡路里数据为0
        f_activity->arc_km_value   =0;                          // 设置距离数据为0
        f_activity->arc_step_value =0;                          // 设置步数数据为0

#endif

        compo_picturebox_set_rotation(arc_kcal, 1800+f_activity->arc_step_value*1.8 ); // 设置卡路里弧形角度：180度+步数百分比*1.8
        compo_picturebox_set_rotation(arc_km,   1800+f_activity->arc_step_value*1.8 ); // 设置距离弧形角度：180度+步数百分比*1.8
        compo_picturebox_set_rotation(arc_step, 1800+f_activity->arc_step_value*1.8 ); // 设置步数弧形角度：180度+步数百分比*1.8

        memset(txt_buf,'\\0',sizeof(txt_buf));                  // 清空文本缓冲区，准备格式化步数文本
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt); // 格式化步数为字符串
        compo_cardbox_text_set(cardbox, 2, txt_buf);           // 设置卡片中索引为2的文本组件显示步数

        memset(txt_buf,'\\0',sizeof(txt_buf));                  // 清空文本缓冲区，准备格式化距离文本
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld.%02ld",f_activity->arc_km_value/100, f_activity->arc_km_value%100); // 格式化距离为"XX.XX"格式
        compo_cardbox_text_set(cardbox, 1, txt_buf);           // 设置卡片中索引为1的文本组件显示距离

        memset(txt_buf,'\\0',sizeof(txt_buf));                  // 清空文本缓冲区，准备格式化卡路里文本
        snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value); // 格式化卡路里为字符串
        compo_cardbox_text_set(cardbox, 0, txt_buf);           // 设置卡片中索引为0的文本组件显示卡路里


        if(f_activity->arc_step_value)                          // 检查步数进度值是否大于0
        {
            compo_picturebox_set_visible(arc_kcal,true);        // 如果有步数数据，显示卡路里弧形进度条
            compo_picturebox_set_visible(arc_km,true);          // 如果有步数数据，显示距离弧形进度条
            compo_picturebox_set_visible(arc_step,true);        // 如果有步数数据，显示步数弧形进度条
        }
        else                                                    // 如果步数进度值为0
        {
            compo_picturebox_set_visible(arc_kcal,false);       // 隐藏卡路里弧形进度条
            compo_picturebox_set_visible(arc_km,false);         // 隐藏距离弧形进度条
            compo_picturebox_set_visible(arc_step,false);       // 隐藏步数弧形进度条
        }
    }

//    }                                                         // 时间检查逻辑结束（已注释）
}

/**
  * @brief  根据序号获取最近任务功能ID
  * @param  idx: 任务索引（0为最近使用的任务，最大值为3）
  * @retval u8: 返回对应的功能ID，如果无任务则返回0
  * @note   该函数实现最近任务的获取逻辑，当实际最近任务不足时会用默认任务补充
  */
static u8 card_get_latest_func(u8 idx)                          // 定义静态函数，根据索引获取最近使用的任务功能ID
{
    u8 i, j;                                                    // 定义循环变量i和临时数组索引j
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};       // 创建临时数组存储过滤后的默认任务，初始化为0
    u8 latest_cnt = latest_task_count();                        // 获取当前实际存储的最近任务数量

    if (latest_cnt > idx)                                       // 检查实际最近任务数量是否大于请求的索引
    {
        return latest_task_get(idx);                            // 如果有足够的最近任务，直接返回对应索引的任务ID
    }
    else                                                        // 如果实际最近任务数量不足
    {
        j = 0;                                                  // 初始化临时数组索引为0
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)          // 遍历默认任务数组中的所有任务
        {
            if (latest_task_find(last_default[i]) == -1)       // 检查默认任务是否已经在最近任务列表中（-1表示未找到）
            {
                latest_default_tmp[j] = last_default[i];       // 如果默认任务不在最近任务中，添加到临时数组
                j++;                                            // 临时数组索引递增
            }
        }
        return latest_default_tmp[idx - latest_cnt];            // 返回补充任务：索引为(请求索引-实际最近任务数量)
    }
}

/**
  * @brief  根据序号获取最近任务的图标资源地址
  * @param  idx: 任务索引
  * @retval u32: 返回对应任务的图标资源地址
  * @note   该函数是card_get_latest_func的包装函数，用于获取图标资源
  */
static u32 card_get_latest_icon(u8 idx)                         // 定义静态函数，根据索引获取最近任务的图标资源地址
{
    return func_get_icon_addr(card_get_latest_func(idx));       // 调用card_get_latest_func获取功能ID，然后获取对应的图标地址
}

#define CARDBOX_RADIUS 16                                       // 定义卡片圆角半径常量为16像素  

/**
  * @brief  设置卡片矩形背景的位置、尺寸和颜色
  * @param  cardbox: 卡片组件指针
  * @param  card_index: 矩形索引（在卡片中的序号）
  * @param  x,y: 矩形位置坐标
  * @param  w,h: 矩形宽度和高度
  * @param  color: 矩形背景颜色
  * @retval None
  * @note   该函数封装了卡片矩形的位置和颜色设置，统一使用16像素圆角
  */
void set_cardbox_rect_location(compo_cardbox_t *cardbox,int card_index, int x, int y, int w, int h,u16 color) // 定义卡片矩形设置函数
{
    if (cardbox == NULL || card_index>=CARD_RECT_MAX)           // 检查输入参数的有效性：卡片指针不为空且索引在有效范围内
    {
        TRACE("[%s]-- erro\\n",__func__);                       // 如果参数无效，输出错误信息（函数名）
        return;                                                 // 直接返回，不执行后续操作
    }
    compo_cardbox_rect_set_location(cardbox, card_index, x, y, w, h, CARDBOX_RADIUS); // 设置矩形的位置和尺寸，使用预定义的圆角半径
    compo_cardbox_rect_set_color(cardbox, card_index, color);   // 设置矩形的背景颜色
}

/**
  * @brief  设置卡片中图标的资源和位置
  * @param  cardbox: 卡片组件指针
  * @param  card_index: 图标索引（在卡片中的序号）
  * @param  icon: 图标资源指针
  * @param  x,y: 图标期望位置坐标
  * @param  w,h: 图标宽度和高度
  * @param  card_w,card_h: 卡片的宽度和高度，用于居中计算
  * @retval None
  * @note   该函数会自动计算图标的居中位置，确保图标在指定区域内居中显示
  */
void set_cardbox_icon(compo_cardbox_t *cardbox,int card_index, void* icon, int x, int y, int w, int h,int card_w,int card_h) // 定义卡片图标设置函数
{

    if (cardbox == NULL || icon == NULL||card_index>=CARD_ICON_MAX) // 检查输入参数的有效性：卡片指针、图标指针不为空，索引在有效范围内
    {
        TRACE("[%s]-- erro\\n",__func__);                       // 如果参数无效，输出错误信息（函数名）
        return;                                                 // 直接返回，不执行后续操作
    }
    compo_cardbox_icon_set(cardbox, card_index, icon);         // 设置图标的资源文件
    compo_cardbox_icon_set_location(cardbox, card_index, x+(w/2-card_w/2), y+(h/2-card_h/2), w, h); // 设置图标位置，通过偏移计算实现居中对齐
}

/**
  * @brief  设置卡片中文本的内容、样式和位置
  * @param  cardbox: 卡片组件指针
  * @param  card_index: 文本索引（在卡片中的序号）
  * @param  txt: 文本组件指针
  * @param  text: 要显示的文本内容
  * @param  color: 文本颜色
  * @param  res: 字体资源
  * @param  x,y: 文本位置坐标
  * @param  w,h: 文本框宽度和高度
  * @param  card_w,card_h: 卡片的宽度和高度，用于位置计算
  * @param  align_center: 是否居中对齐
  * @retval None
  * @note   该函数会根据卡片尺寸调整文本位置，并设置所有文本属性
  */
void set_cardbox_text(compo_cardbox_t *cardbox,int card_index,widget_text_t *txt, const char* text,u16 color,u32 res, int x, int y, int w, int h,int card_w,int card_h,bool align_center) // 定义卡片文本设置函数
{

    if (cardbox == NULL || txt == NULL || text == NULL||card_index>=CARD_TEXT_MAX) // 检查输入参数的有效性：卡片指针、文本指针、文本内容不为空，索引在有效范围内
    {
        TRACE("[%s]-- erro\\n",__func__);                       // 如果参数无效，输出错误信息（函数名）
        return;                                                 // 直接返回，不执行后续操作
    }
    compo_cardbox_text_set_location(cardbox, card_index, x-(card_w/2), y-(card_h/2), w, h); // 设置文本位置，通过减去卡片中心偏移量来调整坐标
    compo_cardbox_text_set_font(cardbox, card_index, res);     // 设置文本的字体资源
    compo_cardbox_text_set_align_center(cardbox,card_index,align_center); // 设置文本的对齐方式（居中或左对齐）
    widget_text_set_color(txt,color);                          // 设置文本的颜色
    compo_cardbox_text_set(cardbox, card_index, text);         // 设置文本的显示内容
}



/**
  * @brief  设置卡片中图像按钮的位置和ID
  * @param  btn: 按钮组件指针
  * @param  id: 按钮的组件ID
  * @param  x,y: 按钮位置坐标
  * @param  w,h: 按钮宽度和高度
  * @param  card_w,card_h: 卡片的宽度和高度，用于位置计算
  * @retval None
  * @note   该函数会根据卡片尺寸调整按钮位置，并设置按钮ID用于事件处理
  */
void set_cardbox_imgbtn(compo_button_t *btn,u16 id, int x, int y, int w, int h,int card_w,int card_h) // 定义卡片图像按钮设置函数
{

    if (btn == NULL)                                            // 检查按钮指针是否为空
    {
        TRACE("[%s]-- erro\\n",__func__);                       // 如果按钮指针为空，输出错误信息（函数名）
        return;                                                 // 直接返回，不执行后续操作
    }
    compo_button_set_location(btn,x-card_w/2, y-card_h/2, w, h); // 设置按钮位置，通过减去卡片中心偏移量来调整坐标
    compo_setid(btn,id);                                        // 设置按钮的组件ID，用于后续的事件识别和处理
}


#define CARD_LAST_APP_MAX  3
const uint16_t Last_Icon_ID[]=
{
    COMPO_ID_APP1,
    COMPO_ID_APP2,
    COMPO_ID_APP3,
};
const uint16_t Week_ID[]=
{
    STR_SUNDAY, // 周日
    STR_MONDAY, // 周一
    STR_TUESDAY, // 周二
    STR_WEDNESDAY, // 周三
    STR_THURSDAY, // 周四
    STR_FRIDAY, // 周五
    STR_SATURDAY, // 周六
};

/**
  * @brief  创建卡片滚动界面的所有组件
  * @param  frm: 窗体指针，用于创建组件的容器
  * @retval None
  * @note   该函数按照从下到上的顺序创建所有卡片组件
  *         包括最近应用、日历、秒表、定时器、音乐、计算器和时钟日期组件
  */
static void func_clock_sub_card_compo_create(compo_form_t *frm)  // 定义卡片组件创建函数
{
    char txt_buf[20];                                           // 定义文本缓冲区，用于格式化时间显示文本
    compo_button_t *btn=NULL;                                   // 定义按钮组件指针，用于创建可点击的图像按钮
    ute_module_systemtime_time_t time;                          // 定义系统时间结构体变量
    uteModuleSystemtimeGetTime(&time);                          // 获取当前系统时间，用于日历卡片显示

    ///卡片7 最近3个应用
    compo_cardbox_t* card7 = compo_cardbox_create(frm, 1, 4, 1, SUB_CARD_W, SUB_CARD_H); // 创建最近应用卡片：1个矩形，4个图标，1个文本，尺寸为SUB_CARD_W×SUB_CARD_H
    compo_setid(card7, COMPO_ID_CARD_7);                       // 设置卡片7的组件ID为COMPO_ID_CARD_7

    //标题
    set_cardbox_rect_location(card7, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41)); // 设置卡片7的背景矩形：位置(0,0)，深灰色背景
    set_cardbox_text(card7,0,card7->text[0],i18n[STR_LATEST_APP],make_color(255,255,255),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false); // 设置标题文本："最近应用"，白色，位置(12,10)

    //图片按键
    for (u8 i=0; i<CARD_LAST_APP_MAX; i++)                     // 循环创建最近应用的图标按钮，CARD_LAST_APP_MAX为最大应用数量
    {
        btn = compo_button_create_page_by_image(frm,card7->page,card_get_latest_icon(i)); // 创建图像按钮，使用动态获取的最近应用图标
        set_cardbox_imgbtn(btn,Last_Icon_ID[i],37+(65*i),65,50,50,SUB_CARD_W,SUB_CARD_H); // 设置按钮位置：X坐标为37+65*i，Y坐标为65，尺寸50×50
    }

    // //卡片6 日历
    // compo_cardbox_t* card6 = compo_cardbox_create(frm, 1, 3, 3, SUB_CARD_W, SUB_CARD_H); // 创建日历卡片：1个矩形，3个图标，3个文本
    // compo_setid(card6, COMPO_ID_CARD_6);                       // 设置卡片6的组件ID为COMPO_ID_CARD_6
    // set_cardbox_rect_location(card6, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41)); // 设置日历卡片的背景矩形，深灰色
    // set_cardbox_icon(card6, 0,UI_BUF_I330001_FIRSTORDER_CALENDAR_BG_L_BIN,0,0,SUB_CARD_W, SUB_CARD_H,SUB_CARD_W,SUB_CARD_H); // 设置日历背景图标，覆盖整个卡片

    // set_cardbox_text(card6,0,card6->text[0],i18n[Week_ID[time.week]],make_color(255,255,255),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false); // 设置星期文本，根据当前系统时间显示对应星期
    // memset(txt_buf,0,sizeof(txt_buf));                          // 清空文本缓冲区，准备格式化月份
    // snprintf((char *)txt_buf, sizeof(txt_buf),"%02d",time.month); // 格式化月份为两位数字符串（如"01"、"12"）
    // set_cardbox_text(card6,1,card6->text[1],txt_buf,make_color(255,255,255),UI_BUF_0FONT_FONT_NUM_38_BIN,7,50,200,46,SUB_CARD_W,SUB_CARD_H,false); // 设置月份文本，使用38号数字字体，位置(7,50)

    // set_cardbox_icon(card6, 1,UI_BUF_I330001_FIRSTORDER_CALENDAR_MOTH_BIN,55,55,26, 26,SUB_CARD_W,SUB_CARD_H); // 设置月份装饰图标，位置(55,55)，尺寸26×26

    // memset(txt_buf,0,sizeof(txt_buf));                          // 清空文本缓冲区，准备格式化日期
    // snprintf((char *)txt_buf, sizeof(txt_buf),"%02d",time.day); // 格式化日期为两位数字符串（如"01"、"31"）
    // set_cardbox_text(card6,2,card6->text[1],txt_buf,make_color(255,255,255),UI_BUF_0FONT_FONT_NUM_38_BIN,80,50,200,46,SUB_CARD_W,SUB_CARD_H,false); // 设置日期文本，使用38号数字字体，位置(80,50)

    // set_cardbox_icon(card6, 2,UI_BUF_I330001_FIRSTORDER_CALENDAR_DATA_BIN,123,55,26, 26,SUB_CARD_W,SUB_CARD_H); // 设置日期装饰图标，位置(123,55)，尺寸26×26

    //卡片5 秒表
    compo_cardbox_t* card5 = compo_cardbox_create(frm, 1, 0, 2, SUB_CARD_W, SUB_CARD_H); // 创建秒表卡片：1个矩形，0个图标，2个文本
    compo_setid(card5, COMPO_ID_CARD_5);                       // 设置卡片5的组件ID为COMPO_ID_CARD_5
    set_cardbox_rect_location(card5, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41)); // 设置秒表卡片的背景矩形，深灰色
    //标题
    set_cardbox_text(card5,0,card5->text[0],i18n[STR_STOP_WATCH],make_color(248,132,10),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false); // 设置秒表标题文本，橙色(248,132,10)，位置(12,10)
    //计时器数字
    set_cardbox_text(card5,1,card5->text[1],"00:00:00",make_color(255,255,255),UI_BUF_0FONT_FONT_NUM_38_BIN,12,37,200,46,SUB_CARD_W,SUB_CARD_H,false); // 设置秒表时间显示，初始值"00:00:00"，白色，38号数字字体

    //卡片4 倒计时
    compo_cardbox_t* card4 = compo_cardbox_create(frm, 1, 0, 2, SUB_CARD_W, SUB_CARD_H); // 创建定时器卡片：1个矩形，0个图标，2个文本
    compo_setid(card4, COMPO_ID_CARD_4);                       // 设置卡片4的组件ID为COMPO_ID_CARD_4
    set_cardbox_rect_location(card4, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41)); // 设置定时器卡片的背景矩形，深灰色
    //标题
    set_cardbox_text(card4,0,card4->text[0],i18n[STR_TIMER],make_color(248,132,10),UI_BUF_0FONT_FONT_BIN,12,10,200,30,SUB_CARD_W,SUB_CARD_H,false); // 设置定时器标题文本，橙色(248,132,10)，位置(12,10)
    //倒计时数字文本
    set_cardbox_text(card4,1,card4->text[1],"00:00:00",make_color(255,255,255),UI_BUF_0FONT_FONT_NUM_38_BIN,12,37,200,46,SUB_CARD_W,SUB_CARD_H,false); // 设置定时器时间显示，初始值"00:00:00"，白色，38号数字字体

    // //卡片3 //闹钟（已注释掉）
    // compo_cardbox_t* card3 = compo_cardbox_create(frm, 1, 1, 5, SUB_CARD_W,SUB_CARD_H); // 创建闹钟卡片的代码已被注释
    // compo_setid(card3, COMPO_ID_CARD_3);                    // 设置卡片3的ID（已注释）
    // set_cardbox_rect_location(card3, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41)); // 设置背景（已注释）
    // set_cardbox_icon(card3, 0,UI_BUF_I330001_FIRSTORDER_VAPE_RECORD_BG_L_BIN,0,0,SUB_CARD_W, SUB_CARD_H,SUB_CARD_W,SUB_CARD_H); // 设置图标（已注释）

    //卡片2 //音乐
    compo_cardbox_t* card2 = compo_cardbox_create(frm, 1, 4, 1, SUB_CARD_W, SUB_CARD_H); // 创建音乐卡片：1个矩形，4个图标，1个文本
    compo_setid(card2,COMPO_ID_CARD_2);                        // 设置卡片2的组件ID为COMPO_ID_CARD_2
    set_cardbox_rect_location(card2, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41)); // 设置音乐卡片的背景矩形，深灰色
    set_cardbox_icon(card2, 0,UI_BUF_I330001_FIRSTORDER_E_MUSIC_BG_BIN,0,0,SUB_CARD_W, SUB_CARD_H,SUB_CARD_W,SUB_CARD_H); // 设置音乐背景图标，覆盖整个卡片
    set_cardbox_text(card2,0,card2->text[0],i18n[STR_MUSIC],make_color(255,255,255),UI_BUF_0FONT_FONT_BIN,12,10,200, 30,SUB_CARD_W,SUB_CARD_H,false); // 设置音乐标题文本，白色，位置(12,10)

    //下一曲
    btn = compo_button_create_page_by_image(frm,card2->page,UI_BUF_I330001_FIRSTORDER_E_MUSIC_NEXT_SONG_BIN); // 创建下一曲按钮，使用下一曲图标
    set_cardbox_imgbtn(btn,COMPO_MUSIC_BTN_NEXT,177,55,30,30,SUB_CARD_W,SUB_CARD_H); // 设置下一曲按钮位置(177,55)，尺寸30×30，ID为COMPO_MUSIC_BTN_NEXT

    //开始/暂停
    btn = compo_button_create_page_by_image(frm,card2->page, UI_BUF_I330001_FIRSTORDER_E_MUSIC_PAUSE_BIN); // 创建播放/暂停按钮，使用暂停图标
    set_cardbox_imgbtn(btn,COMPO_MUSIC_BTN_PLAY,104,55,50,50,SUB_CARD_W,SUB_CARD_H); // 设置播放按钮位置(104,55)，尺寸50×50，ID为COMPO_MUSIC_BTN_PLAY

    //上一曲
    btn = compo_button_create_page_by_image(frm,card2->page, UI_BUF_I330001_FIRSTORDER_E_MUSIC_PREVIOUS_SONG_BIN); // 创建上一曲按钮，使用上一曲图标
    set_cardbox_imgbtn(btn,COMPO_MUSIC_BTN_PREV,31,55,30,30,SUB_CARD_W,SUB_CARD_H); // 设置上一曲按钮位置(31,55)，尺寸

    //刷新音乐图标状态
    music_data_refresh();

    // ///卡片1  ///计算器
    // compo_cardbox_t* card1 = compo_cardbox_create(frm, 1, 3, 3, SUB_CARD_W, SUB_CARD_H); // 创建计算器卡片：1个矩形，3个图标，3个文本
    // compo_setid(card1, COMPO_ID_CARD_1);                       // 设置卡片1的组件ID为COMPO_ID_CARD_1

    // set_cardbox_rect_location(card1, 0,0,0, SUB_CARD_W, SUB_CARD_H,make_color(41,41,41)); // 设置计算器卡片的背景矩形，深灰色背景
    // set_cardbox_icon(card1, 0,UI_BUF_I330001_FIRSTORDER_CALCULATOR_L_BIN,0,0,SUB_CARD_W, SUB_CARD_H,SUB_CARD_W,SUB_CARD_H); // 设置计算器背景图标，覆盖整个卡片

    ///时钟 日期
    compo_picturebox_t* clock_bg = compo_picturebox_create(frm, ui_handle.card_clock_day.clock.bg_res); // 创建时钟背景图片组件，使用UI配置中的背景资源
    compo_setid(clock_bg, ui_handle.card_clock_day.clock.id_bg); // 设置时钟背景的组件ID

    compo_datetime_t* clock_h = compo_datetime_create(frm, ui_handle.card_clock_day.clock.h_res); // 创建时钟时针组件，使用UI配置中的时针资源
    compo_setid(clock_h, ui_handle.card_clock_day.clock.id_h);  // 设置时针的组件ID
    compo_datetime_set_center(clock_h, 13, ui_handle.card_clock_day.clock.h_h / 2); // 设置时针的旋转中心点：X=13，Y=时针高度的一半
    compo_datetime_set_start_angle(clock_h, 900);              // 设置时针的起始角度为900（90度，指向12点方向）
    compo_bonddata(clock_h, ui_handle.card_clock_day.clock.h_bonddata); // 绑定时针到小时数据，实现自动更新

    compo_datetime_t* clock_m = compo_datetime_create(frm, ui_handle.card_clock_day.clock.m_res); // 创建时钟分针组件，使用UI配置中的分针资源
    compo_setid(clock_m, ui_handle.card_clock_day.clock.id_min); // 设置分针的组件ID
    compo_datetime_set_center(clock_m, 13, ui_handle.card_clock_day.clock.m_h / 2); // 设置分针的旋转中心点：X=13，Y=分针高度的一半
    compo_datetime_set_start_angle(clock_m, 900);              // 设置分针的起始角度为900（90度，指向12点方向）
    compo_bonddata(clock_m, ui_handle.card_clock_day.clock.m_bonddata); // 绑定分针到分钟数据，实现自动更新

    compo_textbox_t* date = compo_textbox_create(frm, 5);      // 创建日期文本框组件，最大字符数为5
    compo_setid(date, ui_handle.card_clock_day.date.id);       // 设置日期文本框的组件ID
    compo_textbox_set_font(date, ui_handle.card_clock_day.date.res); // 设置日期文本的字体资源
    compo_textbox_set_align_center(date, ui_handle.card_clock_day.date.center); // 设置日期文本的对齐方式
    widget_text_set_color(date->txt, make_color(ui_handle.card_clock_day.date.color.r, ui_handle.card_clock_day.date.color.g, ui_handle.card_clock_day.date.color.b)); // 设置日期文本颜色为白色
    compo_bonddata(date, ui_handle.card_clock_day.date.bonddata); // 绑定日期文本到日期数据，实现自动更新

    compo_textbox_t* week = compo_textbox_create(frm, 20);     // 创建星期文本框组件，最大字符数为20
    compo_setid(week, ui_handle.card_clock_day.week.id);       // 设置星期文本框的组件ID
    compo_textbox_set_font(week, ui_handle.card_clock_day.week.res); // 设置星期文本的字体资源
    compo_textbox_set_align_center(week, ui_handle.card_clock_day.week.center); // 设置星期文本的对齐方式
    widget_text_set_color(week->txt, make_color(ui_handle.card_clock_day.week.color.r, ui_handle.card_clock_day.week.color.g, ui_handle.card_clock_day.week.color.b)); // 设置星期文本颜色为灰色
    compo_bonddata(week, ui_handle.card_clock_day.week.bonddata); // 绑定星期文本到星期数据，实现自动更新

}                                                               // 函数结束


/**
  * @brief  更新卡片滚动界面中所有组件的位置和大小
  * @param  ofs_y: Y轴偏移量，控制整个卡片列表的垂直滚动位置
  * @param  creating: 是否为创建阶段，true时不更新焦点卡片信息
  * @retval None
  * @note   该函数实现了卡片的动态缩放、位置偏移、可见性控制和焦点检测
  *         包含复杂的数学计算来实现平滑的滚动和缩放效果
  */
static void func_clock_sub_card_compo_update(s32 ofs_y, bool creating) // 定义组件更新函数
{
    s16 x, y, w, h, card_y, card_w;                             // 定义位置、尺寸相关变量
    u8 card_id = 0;                                             // 当前处理的卡片ID
    u16 dc;                                                     // 当前卡片中心距屏幕中心的距离
    u16 dc_min = GUI_SCREEN_CENTER_Y;                           // 记录最小距离，用于确定焦点卡片
    component_t *compo = (component_t *)compo_pool_get_top();   // 获取组件池中的第一个组件
    while (compo != NULL)                                       // 遍历所有组件
    {
        //获取初始值
        switch (compo->id)                                      // 根据组件ID获取对应的初始位置和尺寸信息
        {
            case COMPO_ID_CLOCK_BG:                             // 时钟背景组件
                if (ui_handle.card_clock_day.clock.id_bg != COMPO_ID_CLOCK_BG) // 验证ID一致性
                {
                    printf("COMPO_ID_CLOCK_BG ERR\\n");          // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card_clock_day.clock.bg_x + ui_handle.card_clock_day.x; // 计算绝对X坐标：相对坐标+卡片坐标
                y = ui_handle.card_clock_day.clock.bg_y + ui_handle.card_clock_day.y; // 计算绝对Y坐标：相对坐标+卡片坐标
                w = ui_handle.card_clock_day.clock.bg_w;        // 获取背景宽度
                h = ui_handle.card_clock_day.clock.bg_h;        // 获取背景高度
                card_y = ui_handle.card_clock_day.y;            // 获取所属卡片的Y坐标
                card_id = ui_handle.card_clock_day.id;          // 获取所属卡片的ID
                break;

            case COMPO_ID_CLOCK_H:                              // 时钟时针组件
                if (ui_handle.card_clock_day.clock.id_h != COMPO_ID_CLOCK_H) // 验证ID一致性
                {
                    printf("COMPO_ID_CLOCK_H ERR\\n");           // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card_clock_day.clock.h_x + ui_handle.card_clock_day.x; // 计算时针绝对X坐标
                y = ui_handle.card_clock_day.clock.h_y + ui_handle.card_clock_day.y; // 计算时针绝对Y坐标
                w = ui_handle.card_clock_day.clock.h_w;         // 获取时针宽度
                h = ui_handle.card_clock_day.clock.h_h;         // 获取时针高度
                card_y = ui_handle.card_clock_day.y;            // 获取所属卡片的Y坐标
                card_id = ui_handle.card_clock_day.id;          // 获取所属卡片的ID
                break;

            case COMPO_ID_CLOCK_M:                              // 时钟分针组件
                if (ui_handle.card_clock_day.clock.id_min != COMPO_ID_CLOCK_M) // 验证ID一致性
                {
                    printf("COMPO_ID_CLOCK_M ERR\\n");           // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card_clock_day.clock.m_x + ui_handle.card_clock_day.x; // 计算分针绝对X坐标
                y = ui_handle.card_clock_day.clock.m_y + ui_handle.card_clock_day.y; // 计算分针绝对Y坐标
                w = ui_handle.card_clock_day.clock.m_w;         // 获取分针宽度
                h = ui_handle.card_clock_day.clock.m_h;         // 获取分针高度
                card_y = ui_handle.card_clock_day.y;            // 获取所属卡片的Y坐标
                card_id = ui_handle.card_clock_day.id;          // 获取所属卡片的ID
                break;

            case COMPO_ID_DATE:                                 // 日期文本组件
                if (ui_handle.card_clock_day.date.id != COMPO_ID_DATE) // 验证ID一致性
                {
                    printf("COMPO_ID_DATE ERR\\n");              // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card_clock_day.date.x + ui_handle.card_clock_day.x; // 计算日期文本绝对X坐标
                y = ui_handle.card_clock_day.date.y + ui_handle.card_clock_day.y; // 计算日期文本绝对Y坐标
                w = ui_handle.card_clock_day.date.w;            // 获取日期文本宽度
                h = ui_handle.card_clock_day.date.h;            // 获取日期文本高度
                card_y = ui_handle.card_clock_day.y;            // 获取所属卡片的Y坐标
                card_id = ui_handle.card_clock_day.id;          // 获取所属卡片的ID
                break;

            case COMPO_ID_WEEKDAY:                              // 星期文本组件
                if (ui_handle.card_clock_day.week.id != COMPO_ID_WEEKDAY) // 验证ID一致性
                {
                    printf("COMPO_ID_WEEKDAY ERR\\n");           // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card_clock_day.week.x + ui_handle.card_clock_day.x; // 计算星期文本绝对X坐标
                y = ui_handle.card_clock_day.week.y + ui_handle.card_clock_day.y; // 计算星期文本绝对Y坐标
                w = ui_handle.card_clock_day.week.w;            // 获取星期文本宽度
                h = ui_handle.card_clock_day.week.h;            // 获取星期文本高度
                card_y = ui_handle.card_clock_day.y;            // 获取所属卡片的Y坐标
                card_id = ui_handle.card_clock_day.id;          // 获取所属卡片的ID
                break;

            // case COMPO_ID_CARD_1:                               // 第一个卡片（计算器卡片）
            //     if (ui_handle.card1.id != COMPO_ID_CARD_1)      // 验证ID一致性
            //     {
            //         printf("COMPO_ID_CARD_1 ERR\\n");            // 输出错误信息
            //         halt(HALT_GUI);                             // 停止GUI系统
            //     }
            //     x = ui_handle.card1.x;                          // 获取卡片X坐标
            //     y = card_y = ui_handle.card1.y;                 // 获取卡片Y坐标，同时赋值给card_y
            //     w = ui_handle.card1.w;                          // 获取卡片宽度
            //     h = ui_handle.card1.h;                          // 获取卡片高度
            //     card_id = ui_handle.card1.id;                   // 获取卡片ID
            //     break;

            case COMPO_ID_CARD_2:                               // 第二个卡片（音乐卡片）
                if (ui_handle.card2.id != COMPO_ID_CARD_2)      // 验证ID一致性
                {
                    printf("COMPO_ID_CARD_2 ERR\\n");            // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card2.x;                          // 获取卡片X坐标
                y = card_y = ui_handle.card2.y;                 // 获取卡片Y坐标，同时赋值给card_y
                w = ui_handle.card2.w;                          // 获取卡片宽度
                h = ui_handle.card2.h;                          // 获取卡片高度
                card_id = ui_handle.card2.id;                   // 获取卡片ID
                break;

            // case COMPO_ID_CARD_3:                            // 第三个卡片（已注释掉）
            //     if (ui_handle.card3.id != COMPO_ID_CARD_3)   // 验证ID一致性（已注释）
            //     {
            //         printf("COMPO_ID_CARD_3 ERR\\n");         // 输出错误信息（已注释）
            //         halt(HALT_GUI);                          // 停止GUI系统（已注释）
            //     }
            //     x = ui_handle.card3.x;                       // 获取卡片X坐标（已注释）
            //     y = card_y = ui_handle.card3.y;              // 获取卡片Y坐标（已注释）
            //     w = ui_handle.card3.w;                       // 获取卡片宽度（已注释）
            //     h = ui_handle.card3.h;                       // 获取卡片高度（已注释）
            //     card_id = ui_handle.card3.id;                // 获取卡片ID（已注释）
            //     break;

            case COMPO_ID_CARD_4:                               // 第四个卡片（定时器卡片）
                if (ui_handle.card4.id != COMPO_ID_CARD_4)      // 验证ID一致性
                {
                    printf("COMPO_ID_CARD_4 ERR\\n");            // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card4.x;                          // 获取卡片X坐标
                y = card_y = ui_handle.card4.y;                 // 获取卡片Y坐标，同时赋值给card_y
                w = ui_handle.card4.w;                          // 获取卡片宽度
                h = ui_handle.card4.h;                          // 获取卡片高度
                card_id = ui_handle.card4.id;                   // 获取卡片ID
                break;

            case COMPO_ID_CARD_5:                               // 第五个卡片（秒表卡片）
                if (ui_handle.card5.id != COMPO_ID_CARD_5)      // 验证ID一致性
                {
                    printf("COMPO_ID_CARD_5 ERR\\n");            // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card5.x;                          // 获取卡片X坐标
                y = card_y = ui_handle.card5.y;                 // 获取卡片Y坐标，同时赋值给card_y
                w = ui_handle.card5.w;                          // 获取卡片宽度
                h = ui_handle.card5.h;                          // 获取卡片高度
                card_id = ui_handle.card5.id;                   // 获取卡片ID
                break;

            // case COMPO_ID_CARD_6:                               // 第六个卡片（日历卡片）
            //     if (ui_handle.card6.id != COMPO_ID_CARD_6)      // 验证ID一致性
            //     {
            //         printf("COMPO_ID_CARD_6 ERR\\n");            // 输出错误信息
            //         halt(HALT_GUI);                             // 停止GUI系统
            //     }
            //     x = ui_handle.card6.x;                          // 获取卡片X坐标
            //     y = card_y = ui_handle.card6.y;                 // 获取卡片Y坐标，同时赋值给card_y
            //     w = ui_handle.card6.w;                          // 获取卡片宽度
            //     h = ui_handle.card6.h;                          // 获取卡片高度
            //     card_id = ui_handle.card6.id;                   // 获取卡片ID
            //     break;

            case COMPO_ID_CARD_7:                               // 第七个卡片（最近应用卡片）
                if (ui_handle.card7.id != COMPO_ID_CARD_7)      // 验证ID一致性
                {
                    printf("COMPO_ID_CARD_7 ERR\\n");            // 输出错误信息
                    halt(HALT_GUI);                             // 停止GUI系统
                }
                x = ui_handle.card7.x;                          // 获取卡片X坐标
                y = card_y = ui_handle.card7.y;                 // 获取卡片Y坐标，同时赋值给card_y
                w = ui_handle.card7.w;                          // 获取卡片宽度
                h = ui_handle.card7.h;                          // 获取卡片高度
                card_id = ui_handle.card7.id;                   // 获取卡片ID
                break;

            default:                                            // 默认情况：不匹配任何已知组件ID
                compo = compo_get_next(compo);                  // 获取下一个组件
                continue;                                       // 跳过当前循环，继续处理下一个组件
                break;
        }

        //计算偏移、缩放
        card_w = CARD_WIDTH_ORG;                                // 初始化卡片宽度为原始宽度
        card_y += ofs_y;                                        // 将Y轴偏移量加到卡片Y坐标上，实现滚动效果
        y += ofs_y;                                             // 将Y轴偏移量加到组件Y坐标上
        dc = abs_s(card_y - GUI_SCREEN_CENTER_Y);               // 计算卡片中心与屏幕中心的距离（绝对值）
        if (creating == false && dc < dc_min)                   // 如果不是创建阶段且当前距离小于最小距离
        {
            f_card_t *f_card = (f_card_t *)func_cb.f_cb;        // 获取卡片功能控制结构体指针
            f_card->focus_card = card_id;                       // 更新焦点卡片ID为当前卡片
            dc_min = dc;                                        // 更新最小距离记录
//            TRACE("focus[%d]\\n", card_id);                    // 调试输出焦点卡片ID（已注释）
        }
        if (card_y > CARD_SCALE_START_POS && card_y < CARD_UNVISIBLE_POS_BOTTOM) // 检查卡片是否在缩放区域内
        {
            if (card_y > CARD_SCALE_STOP_Y)                     // 如果卡片Y坐标超过缩放停止位置
            {
                y -= card_y - CARD_SCALE_STOP_Y;                // 调整组件Y坐标，保持在缩放停止位置
                card_y = CARD_SCALE_STOP_Y;                     // 将卡片Y坐标限制在缩放停止位置
            }
            if (card_w > CARD_WIDTH_MIN)                        // 如果卡片宽度大于最小宽度
            {
                //缩放
                s16 card_w_org = card_w;                        // 保存原始卡片宽度
                card_w -= (card_w - CARD_WIDTH_MIN) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS); // 根据位置计算缩放后的卡片宽度
                w = w * card_w / card_w_org;                    // 按比例缩放组件宽度
                h = h * card_w / card_w_org;                    // 按比例缩放组件高度（保持宽高比）
                x += (GUI_SCREEN_CENTER_X - x) * (card_w_org - card_w) / card_w_org; // 调整X坐标，使组件向屏幕中心收缩
                y += (card_y - y) * (card_w_org - card_w) / card_w_org; // 调整Y坐标，保持组件相对于卡片的位置关系
                //偏移
                s16 card_y_org = card_y;                        // 保存原始卡片Y坐标
                card_y = CARD_SCALE_START_POS + (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS) * (card_y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_Y - CARD_SCALE_START_POS); // 计算缩放后的卡片Y坐标
                y += card_y - card_y_org;                       // 根据卡片Y坐标变化调整组件Y坐标
            }
        }

        //设置位置和大小
        bool unvisible = card_y < CARD_UNVISIBLE_POS_TOP || card_y >= CARD_UNVISIBLE_POS_BOTTOM; // 判断卡片是否超出可见范围
        if (compo->type == COMPO_TYPE_CARDBOX)                  // 如果组件类型是卡片容器
        {
            compo_cardbox_t *cardbox = (compo_cardbox_t *)compo;
            compo_cardbox_set_location(cardbox, x, y, w, h);
            u8 level = y > CARD_SCALE_START_POS ? \
                       (y < CARD_SCALE_STOP_POS ? \
                        (CARD_BG_GRAY_START - (CARD_BG_GRAY_START - CARD_BG_GRAY_STOP) * (y - CARD_SCALE_START_POS) / (CARD_SCALE_STOP_POS - CARD_SCALE_START_POS)) : \
                        CARD_BG_GRAY_STOP
                       ) : \
                       CARD_BG_GRAY_START;
//            TRACE("gray[%d]\n", level);
            compo_cardbox_rect_set_color(cardbox, 0, GRAY_LV(level)); // 设置第一个矩形的背景颜色
            compo_cardbox_rect_set_color(cardbox, 1, GRAY_LV(level)); // 设置第二个矩形的背景颜色
            if (unvisible)                                      // 如果卡片不可见
            {
                compo_cardbox_set_visible(cardbox, false);      // 隐藏卡片容器
            }
            else                                                // 如果卡片可见
            {
                compo_cardbox_set_visible(cardbox, true);       // 显示卡片容器
            }

//            if (card_id > COMPO_CARD_END-1)                  // 被覆盖卡片处理逻辑（已注释）
//            {
//                rect_t rect = compo_cardbox_get_location((compo_cardbox_t *)compo_getobj_byid(COMPO_ID_CARD_SPORT_COMPASS + (card_id - CARD_ID_SPORT_COMPASS) - 1)); // 获取前一个卡片位置
//                if (y <= rect.y)                             // 如果当前卡片被前一个卡片覆盖
//                {
//                    compo_cardbox_set_visible(cardbox, false); // 隐藏被覆盖的卡片
//                }
//                else
//                {
//                    compo_cardbox_set_visible(cardbox, true); // 显示未被覆盖的卡片
//                }
//            }

        }
        else if (compo->type == COMPO_TYPE_PICTUREBOX)          // 如果组件类型是图片框
        {
            compo_picturebox_t *pic = (compo_picturebox_t *)compo; // 转换为图片框指针
            compo_picturebox_set_pos(pic, x, y);                // 设置图片框位置
            compo_picturebox_set_size(pic, w, h);               // 设置图片框大小
            if (unvisible)                                      // 如果不可见
            {
                compo_picturebox_set_visible(pic, false);       // 隐藏图片框
            }
            else                                                // 如果可见
            {
                compo_picturebox_set_visible(pic, true);        // 显示图片框
            }
        }
        else if (compo->type == COMPO_TYPE_TEXTBOX)             // 如果组件类型是文本框
        {
            compo_textbox_t *txt = (compo_textbox_t *)compo;    // 转换为文本框指针
            compo_textbox_set_location(txt, x, y, w, h);        // 设置文本框的位置和大小
            if (unvisible)                                      // 如果不可见
            {
                compo_textbox_set_visible(txt, false);          // 隐藏文本框
            }
            else                                                // 如果可见
            {
                compo_textbox_set_visible(txt, true);           // 显示文本框
            }
        }
        else if (compo->type == COMPO_TYPE_DATETIME)            // 如果组件类型是日期时间
        {
            compo_datetime_t *dt = (compo_datetime_t *)compo;   // 转换为日期时间指针
            compo_datetime_set_pos(dt, x, y);                   // 设置日期时间组件位置（时钟指针不需要缩放）
        }

        compo = compo_get_next(compo);                          // 获取下一个组件，继续遍历
    }
}                                                               // 函数结束

/**
  * @brief  创建上拉卡片菜单（文件内调用）
  * @param  ofs_y: Y轴偏移量，用于设置卡片的初始滚动位置
  * @retval compo_form_t*: 返回创建的表单指针
  * @note   该函数是内部实现函数，创建完整的卡片界面包括遮罩层和所有卡片组件
  */
static compo_form_t *func_clock_sub_card_form_create_by_ofs(s16 ofs_y) // 定义内部卡片表单创建函数
{
    compo_form_t *frm = compo_form_create(true);                // 创建一个新的表单容器，参数true表示启用透明背景

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); // 创建矩形形状组件作为遮罩层
    compo_shape_set_color(masklayer, COLOR_BLACK);              // 设置遮罩层颜色为黑色
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT); // 设置遮罩层覆盖整个屏幕
//    compo_shape_set_alpha(masklayer, 160);                    // 设置遮罩层透明度（已注释，可能用于半透明效果）
    //创建所有组件
    func_clock_sub_card_compo_create(frm);                      // 调用组件创建函数，创建所有卡片和时钟组件
    //更新组件位置和大小
    func_clock_sub_card_compo_update(ofs_y, true);             // 调用组件更新函数，根据偏移量设置初始位置，true表示创建阶段

    return frm;                                                 // 返回创建完成的表单指针
}

/**
  * @brief  创建上拉卡片菜单（供外部调用）
  * @param  None
  * @retval compo_form_t*: 返回创建的表单指针
  * @note   该函数是外部接口，使用默认的最大弹簧位置创建卡片菜单
  */
compo_form_t *func_clock_sub_card_form_create(void)            // 定义外部接口函数
{
    return func_clock_sub_card_form_create_by_ofs(SPRING_Y_MAX); // 调用内部函数，使用SPRING_Y_MAX作为默认偏移量
}

/**
  * @brief  获取点击的卡片组件ID
  * @param  pt: 触摸点坐标
  * @retval u16: 返回被点击的卡片组件ID，如果没有点击到卡片则返回0
  * @note   该函数通过遍历所有卡片，检测触摸点是否在卡片范围内
  */
static u16 func_clock_sub_card_get_btn_id(point_t pt)          // 定义点击检测函数
{
    u16 i, id;                                                  // 定义循环变量和组件ID变量
    u16 ret = 0;                                                // 初始化返回值为0（表示未点击到任何卡片）
    rect_t rect;                                                // 定义矩形结构体，用于存储卡片的位置信息
    compo_cardbox_t *cardbox;                                   // 定义卡片组件指针

    for(i=0; i<CARD_BTN_COUNT; i++)                             // 遍历所有卡片按钮，CARD_BTN_COUNT为卡片总数
    {
//        id = COMPO_ID_CARD_SPORT_COMPASS + i;                // 旧的ID计算方式（已注释）
        id = COMPO_CARD_START + 1 + i;                          // 计算当前卡片的组件ID：从COMPO_CARD_START+1开始递增
        cardbox = compo_getobj_byid(id);                        // 根据ID获取对应的卡片组件指针
        rect = compo_cardbox_get_absolute(cardbox);             // 获取卡片在屏幕上的绝对位置和尺寸
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei) // 检测触摸点是否在卡片范围内
        {
            ret = id;                                           // 如果点击在卡片内，记录卡片ID
            break;                                              // 找到匹配的卡片后立即退出循环
        }
    }
    return ret;                                                 // 返回被点击的卡片ID（0表示未点击到任何卡片）
}

/**
  * @brief  表盘上拉卡片菜单点击处理函数
  * @param  None
  * @retval None
  * @note   该函数处理用户在卡片界面上的点击事件，根据点击的卡片或按钮执行相应的功能跳转
  *         支持条件编译，只有在相应模块支持时才启用对应功能
  */
static void func_clock_sub_card_click_handler(void)            // 定义卡片点击处理函数
{
    compo_cardbox_t *cardbox = NULL;                            // 定义卡片组件指针，用于获取卡片对象
//    rect_t rect;                                              // 矩形结构体（已注释，可能用于位置检测）
    u8 func_jump = FUNC_NULL;                                   // 初始化功能跳转标识为空，表示不跳转
    point_t pt = ctp_get_sxy();                                 // 获取触摸屏点击的坐标位置
    u16 compo_id = func_clock_sub_card_get_btn_id(pt);          // 调用点击检测函数，获取被点击的卡片组件ID
    printf("click compo_id:%d\\n", compo_id);                   // 调试输出：打印被点击的组件ID
    int id = compo_get_button_id();                             // 获取被点击的按钮ID（用于卡片内部按钮识别）

    switch(compo_id)                                            // 根据被点击的卡片组件ID进行分支处理
    {
//         case COMPO_ID_CARD_1 :                                  // 处理第一个卡片（计算器/计算器卡片）的点击

// #if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT                        // 如果支持运动模块
//             func_jump = FUNC_ACTIVITY;                          // 设置跳转到运动功能界面
// #endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT                  // 运动模块支持条件编译结束

// #if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT                      // 如果支持计算器模块
//             func_jump =FUNC_CALCULATOR;                         // 设置跳转到计算器功能界面
// #endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT                // 计算器模块支持条件编译结束
//             break;                                              // 结束第一个卡片的处理

        case COMPO_ID_CARD_2 :                                  // 处理第二个卡片（音乐卡片）的点击
            switch(id)                                          // 根据卡片内部按钮ID进行二级分支处理
            {
                case COMPO_MUSIC_BTN_PREV:                      // 处理上一曲按钮点击
                {
                    uteModuleMusicCtrl(false,true,false);       // 调用音乐控制函数：上一曲（参数：非下一曲，执行操作，非暂停）
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV); // 获取上一曲按钮组件指针
                    compo_button_set_bgimg(btn,UI_BUF_I330001_FIRSTORDER_E_MUSIC_PREVIOUS_SONG_BIN); // 设置按钮背景图片为上一曲图标
                }
                break;                                          // 结束上一曲按钮处理

                case COMPO_MUSIC_BTN_PLAY:                      // 处理播放/暂停按钮点击
                {
                    uteModuleMusicCtrlPaused(false);            // 调用音乐暂停控制函数，切换播放/暂停状态
                    if(ble_is_connect())                        // 检查蓝牙是否连接
                    {
                        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused(); // 更新蓝牙音乐播放状态：取反当前暂停状态
                    }
                    music_data_refresh();                       // 刷新音乐数据显示
                }
                break;                                          // 结束播放按钮处理

                case COMPO_MUSIC_BTN_NEXT:                      // 处理下一曲按钮点击
                {
                    uteModuleMusicCtrl(true,true,false);        // 调用音乐控制函数：下一曲（参数：下一曲，执行操作，非暂停）
                    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT); // 获取下一曲按钮组件指针
                    compo_button_set_bgimg(btn, UI_BUF_I330001_FIRSTORDER_E_MUSIC_NEXT_SONG_BIN); // 设置按钮背景图片为下一曲图标
                }
                break;                                          // 结束下一曲按钮处理

                default:                                        // 处理音乐卡片的其他区域点击（非按钮区域）
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT                           // 如果支持音乐模块  
                    func_jump = FUNC_BT;                        // 设置跳转到蓝牙音乐功能界面
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT                     // 音乐模块支持条件编译结束  
                    break;                                      // 结束默认处理
            }

            break;                                              // 结束第二个卡片的处理

        // case COMPO_ID_CARD_3:                               // 第三个卡片处理（已注释掉）
        //     func_jump = FUNC_ECIG_VPAE_SUB;                 // 跳转到电子烟功能（已注释）
        //     break;                                          // 结束第三个卡片处理（已注释）

        case COMPO_ID_CARD_4 :                                  // 处理第四个卡片（定时器卡片）的点击
#if UTE_MODULE_SCREENS_TIMER_SUPPORT                           // 如果支持定时器模块  
            func_jump = FUNC_TIMER;                             // 设置跳转到定时器功能界面
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT                     // 定时器模块支持条件编译结束  
            break;                                              // 结束第四个卡片的处理

        case COMPO_ID_CARD_5 :                                  // 处理第五个卡片（秒表卡片）的点击
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT                       // 如果支持秒表模块  
            func_jump = FUNC_STOPWATCH;                         // 设置跳转到秒表功能界面
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT                 // 秒表模块支持条件编译结束  
            break;                                              // 结束第五个卡片的处理

//         case COMPO_ID_CARD_6 :                                  // 处理第六个卡片（日历卡片）的点击
// #if UTE_MODULE_SCREENS_CALENDAER_SUPPORT                       // 如果支持日历模块
//             func_jump =  FUNC_CALENDAER;                        // 设置跳转到日历功能界面
// #endif
//             break;                                              // 结束第六个卡片的处理

        case COMPO_ID_CARD_7 :                                  // 处理第七个卡片（最近应用卡片）的点击
            printf("id : %d\\n", id);                            // 调试输出：打印按钮ID
            if (id >= COMPO_ID_APP1)                            // 检查按钮ID是否为应用图标按钮
            {
                func_jump = card_get_latest_func(id-COMPO_ID_APP1); // 调用最近任务获取函数，计算应用索引并获取对应功能ID
            }
            break;                                              // 结束第七个卡片的处理
        default:                                                // 处理未匹配的组件ID（点击到空白区域或未知组件）
            break;                                              // 不执行任何操作
    }
    if(func_jump != FUNC_NULL)                                  // 检查是否需要执行功能跳转
    {
        printf("func_jump=%d\\n", func_jump);                    // 调试输出：打印要跳转的功能ID
        func_cb.sta = func_jump;                                // 设置功能状态为目标功能，触发界面跳转
    }
}

/**
  * @brief  根据偏移位置把最近的卡片对齐到屏幕中央
  * @param  ofs_y: 当前Y轴偏移量
  * @retval s16: 返回计算出的目标偏移量，用于将最近的卡片对齐到屏幕中心
  * @note   该函数通过遍历所有卡片，找到距离屏幕中心最近的卡片，然后计算对齐所需的偏移量
  *         包含边界检查，防止超出拖拽范围
  */
static s16 func_clock_sub_card_align_y(s16 ofs_y)              // 定义基于偏移位置的卡片对齐函数
{
    u8 i, nearest_id = 0;                                       // 定义循环变量和最近卡片的索引
    u16 dc_min = GUI_SCREEN_CENTER_Y;                           // 初始化最小距离为屏幕中心Y坐标，用于比较
    u16 dc;                                                     // 定义当前卡片到屏幕中心的距离变量
    if (ofs_y >= DRAG_Y_MAX)                                    // 检查偏移量是否超过最大拖拽范围
    {
        return DRAG_Y_MAX;                                      // 如果超过上边界，返回最大拖拽值（回弹效果）
    }
    else if (ofs_y <= DRAG_Y_MIN)                               // 检查偏移量是否低于最小拖拽范围
    {
        return DRAG_Y_MIN;                                      // 如果超过下边界，返回最小拖拽值
    }


//    for (i=0; i<CARD_COUNT; i++)                             // 旧的循环方式（已注释）
    for (i=0; i<COMPO_CARD_END-COMPO_CARD_START-1; i++)        // 遍历所有卡片，计算卡片数量为组件结束ID减去开始ID再减1
    {
//        dc = abs_s(card_y_info[i] + ofs_y - GUI_SCREEN_CENTER_Y); // 旧的距离计算方式（已注释）
        dc = abs_s(func_clock_sub_card_get_y_info(COMPO_CARD_START+1+i) + ofs_y - GUI_SCREEN_CENTER_Y); // 计算当前卡片加上偏移量后到屏幕中心的距离
        if (dc < dc_min)                                        // 如果当前距离小于记录的最小距离
        {
            nearest_id = i;                                     // 更新最近卡片的索引
            dc_min = dc;                                        // 更新最小距离记录
        }
    }

//    return (GUI_SCREEN_CENTER_Y - card_y_info[nearest_id]);   // 旧的返回值计算方式（已注释）
    return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+nearest_id)); // 返回将最近卡片对齐到屏幕中心所需的偏移量
}

/**
  * @brief  根据卡片序号对齐页面偏移位置到屏幕中央
  * @param  card_id: 卡片的组件ID或索引
  * @retval s16: 返回计算出的目标偏移量，用于将指定卡片对齐到屏幕中心
  * @note   该函数直接根据卡片ID计算对齐偏移量，包含边界检查
  */
static s16 func_clock_sub_card_align_by_idx(s8 card_id)        // 定义基于卡片索引的对齐函数
{
//    if (card_id <= CARD_ID_CLOCK)                            // 旧的边界检查方式（已注释）
    if (card_id <= COMPO_CARD_START)                           // 检查卡片ID是否小于等于卡片起始ID
    {
        return DRAG_Y_MAX;                                      // 如果是时钟或更早的组件，返回最大拖拽值（顶部位置）
    }
//    else if (card_id > (CARD_COUNT - 1))                     // 旧的上边界检查方式（已注释）
    else if (card_id > (COMPO_CARD_END-COMPO_CARD_START-1 - 1)) // 检查卡片ID是否超过最大卡片索引
    {
        return DRAG_Y_MIN;                                      // 如果超过最大索引，返回最小拖拽值（底部位置）
    }
    else                                                        // 如果卡片ID在有效范围内
    {
//        return (GUI_SCREEN_CENTER_Y - card_y_info[card_id]);  // 旧的偏移量计算方式（已注释）
        return (GUI_SCREEN_CENTER_Y - func_clock_sub_card_get_y_info(COMPO_CARD_START+1+card_id)); // 返回将指定卡片对齐到屏幕中心所需的偏移量
    }
}



/**
  * @brief  限制滑动偏移位置在有效范围内
  * @param  ofs_y: 输入的Y轴偏移量
  * @retval s16: 返回限制后的偏移量
  * @note   该函数确保偏移量在DRAG_Y_MIN和DRAG_Y_MAX之间，防止滚动超出边界
  */
static s16 func_clock_sub_card_limit_y(s16 ofs_y)              // 定义滑动位置限制函数
{
    ofs_y = MIN(DRAG_Y_MAX, MAX(DRAG_Y_MIN, ofs_y));           // 使用MIN和MAX宏将偏移量限制在[DRAG_Y_MIN, DRAG_Y_MAX]范围内
    return ofs_y;                                               // 返回限制后的偏移量
}


/**
  * @brief  设置页面偏移位置并更新所有组件
  * @param  ofs_y: 要设置的Y轴偏移量
  * @retval None
  * @note   该函数更新卡片结构体中的偏移量，并触发所有组件的位置更新
  */
static void func_clock_sub_card_set_offs(s16 ofs_y)           // 定义页面偏移位置设置函数
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;               // 获取卡片功能控制结构体指针

    f_card->ofs_y = ofs_y;                                     // 更新结构体中的Y轴偏移量
    func_clock_sub_card_compo_update(f_card->ofs_y, false);   // 调用组件更新函数，应用新的偏移量，false表示非创建阶段
}

/**
  * @brief  时钟表盘上拉菜单数据更新
  * @param  None
  * @retval None
  * @note   该函数负责更新定时器和秒表卡片的时间显示
  *         包含时间格式化和边界值处理
  */
static void func_clock_sub_card_data_update(void)             // 定义数据更新函数
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;               // 获取卡片功能控制结构体指针
    compo_cardbox_t *cardbox = NULL;                            // 定义卡片组件指针
    char txt_buf[20];                                           // 定义文本缓冲区，用于格式化时间字符串

    if(f_card->flag_drag == 0)                                 // 检查是否处于拖拽状态
    {
        // card1_updata_disp();                                // 计算器卡片更新（已注释）
    }

//    if(f_activity->activity_state != 2) return;              // 运动状态检查（已注释）

    // 更新定时器卡片显示
    memset(txt_buf,0,sizeof(txt_buf));                         // 清空文本缓冲区
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld:%02ld",SEC_TO_HOUR(sys_cb.timer_left_sec),SEC_TO_MIN(sys_cb.timer_left_sec),SEC_TO_SEC(sys_cb.timer_left_sec)); // 格式化定时器剩余时间为"HH:MM:SS"格式
    cardbox = compo_getobj_byid(ui_handle.card4.id);           // 获取定时器卡片组件指针
    compo_cardbox_text_set(cardbox, ui_handle.card4.text_time.idx,txt_buf); // 设置定时器卡片的时间文本显示

    // 计算秒表时间组件
    u8 min = ((sys_cb.stopwatch_total_msec / 1000) / 60) % 100; // 计算秒表分钟数：总毫秒转秒再转分钟，取模100防止溢出
    u8 sec = (sys_cb.stopwatch_total_msec / 1000) % 60;        // 计算秒表秒数：总毫秒转秒，取模60得到秒数部分
    u16 msec = sys_cb.stopwatch_total_msec % 1000;             // 计算秒表毫秒数：取模1000得到毫秒部分
    if(sys_cb.stopwatch_total_msec / 1000 / 60 >= 100)         // 检查秒表是否超过100分钟
    {
        min  = 99;                                              // 如果超过100分钟，限制分钟显示为99
        sec  = 59;                                              // 限制秒数显示为59
        msec = 999;                                             // 限制毫秒显示为999
    }
    memset(txt_buf,0,sizeof(txt_buf));                         // 清空文本缓冲区
    snprintf(txt_buf,sizeof(txt_buf),"%02ld:%02ld.%02ld",min,sec,msec/10); // 格式化秒表时间为"MM:SS.MS"格式，毫秒除以10显示为两位数
    cardbox = compo_getobj_byid(ui_handle.card5.id);           // 获取秒表卡片组件指针
    compo_cardbox_text_set(cardbox, ui_handle.card5.text_time.idx,txt_buf); // 设置秒表卡片的时间文本显示
}

/**
  * @brief  下拉返回表盘界面
  * @param  auto_switch: 是否为自动切换（true为自动，false为手动）
  * @retval None
  * @note   该函数实现从卡片菜单返回到时钟表盘的界面切换
  *         包含表单销毁、重建和切换动画处理
  */
static void func_clock_sub_card_switch_to_clock(bool auto_switch) // 定义界面切换函数
{
    printf("%s\\n", __func__);                                  // 调试输出：打印函数名
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;               // 获取卡片功能控制结构体指针

    u16 switch_mode = FUNC_SWITCH_MENU_PULLUP_DOWN | (auto_switch ? FUNC_SWITCH_AUTO : 0); // 设置切换模式：上拉下拉菜单模式，根据auto_switch参数决定是否添加自动切换标志

    compo_form_destroy(func_cb.frm_main);                      // 销毁当前主表单，释放卡片菜单界面资源
    compo_form_t * frm_clock = func_create_form(FUNC_CLOCK);   // 创建时钟表盘表单
    compo_form_t * frm = func_clock_sub_card_form_create_by_ofs(f_card->ofs_y); // 使用当前偏移量重新创建卡片菜单表单（用于切换动画）
    func_cb.frm_main = frm;                                    // 设置新创建的卡片表单为主表单
    if (func_switching(switch_mode, NULL))                     // 执行界面切换，传入切换模式
    {
        func_cb.sta = FUNC_CLOCK;                              // 如果切换成功，设置功能状态为时钟表盘
    }
    compo_form_destroy(frm_clock);                             // 销毁临时创建的时钟表盘表单
}

/**
  * @brief  音乐数据刷新函数
  * @param  None
  * @retval None
  * @note   该函数负责更新音乐播放按钮的状态和图标显示
  *         根据蓝牙连接状态和播放状态动态切换按钮图标
  */
void music_data_refresh(void)                                   // 定义音乐数据刷新函数
{
    if(ble_is_connect())                                        // 检查蓝牙是否连接
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused(); // 更新音乐播放状态：取反暂停状态得到播放状态
    }

    compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PLAY); // 获取播放/暂停按钮组件指针
    if(ble_is_connect() || uteModuleCallBtIsConnected())        // 检查蓝牙或通话蓝牙是否连接
    {
        compo_button_set_bgimg(btn, bt_cb.music_playing ? UI_BUF_I330001_FIRSTORDER_MUSIC_PAUSED_BIN : UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY00_BIN); // 根据播放状态设置按钮图标：播放中显示暂停图标，暂停时显示播放图标
    }
    else                                                        // 如果蓝牙未连接
    {
        compo_button_set_bgimg(btn,UI_BUF_I330001_FIRSTORDER_MUSIC_PLAY01_BIN); // 设置为未连接状态的播放图标
    }
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

/**
  * @brief  时钟表盘上拉菜单主要事件流程处理
  * @param  None
  * @retval None
  * @note   该函数是卡片滚动界面的核心处理函数，管理拖拽、自动对齐、数据更新等所有主要逻辑
  *         包含三个主要状态：拖拽状态、自动移动状态、静止状态
  */
static void func_clock_sub_card_process(void)                  // 定义卡片滚动主要事件处理函数
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;               // 获取卡片功能控制结构体指针

    if (f_card->flag_drag)                                     // 检查是否处于拖拽状态
    {
        s32 dx, dy;                                             // 定义触摸位移变量
        f_card->flag_drag = ctp_get_dxy(&dx, &dy);             // 获取触摸位移量，同时更新拖拽状态标志
        if (f_card->flag_drag)                                 // 如果仍在拖拽中
        {
            //拖动页面
            f_card->ofs_y_drag = func_clock_sub_card_limit_y(f_card->ofs_y + dy); // 计算拖拽偏移量并限制在有效范围内
            func_clock_sub_card_compo_update(f_card->ofs_y_drag, false); // 使用拖拽偏移量更新所有组件位置
        }
        else                                                    // 如果用户抬手（拖拽结束）
        {
            //抬手
            f_card->ofs_y = f_card->ofs_y_drag;                // 将拖拽偏移量设为当前偏移量
            s16 last_dy = ctp_get_last_dxy().y;                // 获取最后一次触摸的Y轴位移（用于计算惯性）
            s16 to_y = f_card->ofs_y + (last_dy * DRAG_AUTO_SPEED); // 根据惯性计算目标位置
            f_card->moveto_y = func_clock_sub_card_align_y(to_y); // 计算最近卡片的对齐位置作为最终目标
            f_card->flag_move_auto = true;                      // 启动自动移动标志
            f_card->tick = tick_get();                          // 记录当前时间戳，用于控制自动移动速度
        }
        for(u8 i=0; i<CARD_BTN_COUNT; i++)                     // 遍历所有卡片，处理文本滚动
        {
            u16 id = COMPO_CARD_START + 1 + i;                 // 计算卡片组件ID
            compo_cardbox_text_scroll_reset((compo_cardbox_t *)compo_getobj_byid(id)); // 重置卡片文本滚动状态
            compo_cardbox_text_scroll_pause((compo_cardbox_t *)compo_getobj_byid(id), true); // 暂停卡片文本滚动（拖拽时不滚动文本）
        }

    }
    else if (f_card->flag_move_auto)                           // 检查是否处于自动移动状态
    {
        for(u8 i=0; i<CARD_BTN_COUNT; i++)                     // 遍历所有卡片，处理文本滚动
        {
            u16 id = COMPO_CARD_START + 1 + i;                 // 计算卡片组件ID
            compo_cardbox_text_scroll_pause((compo_cardbox_t *)compo_getobj_byid(id), false); // 恢复卡片文本滚动（自动移动时允许文本滚动）
        }

        //自动移动
        if (f_card->ofs_y == f_card->moveto_y)                 // 检查是否已到达目标位置
        {
            if (f_card->ofs_y < SPRING_Y_MIN)                  // 检查是否超出下边界
            {
                f_card->moveto_y = SPRING_Y_MIN;               // 设置回弹到下边界位置
            }
            else if (f_card->ofs_y > SPRING_Y_MAX)             // 检查是否超出上边界
            {
                f_card->moveto_y = SPRING_Y_MAX;               // 设置回弹到上边界位置
            }
            else                                                // 如果在正常范围内
            {
                f_card->flag_move_auto = false;                // 关闭自动移动标志，移动完成
                f_card->moveto_card = f_card->focus_card;      // 记录移动完成后的焦点卡片
            }
        }
        else if (tick_check_expire(f_card->tick, FOCUS_AUTO_TICK_EXPIRE)) // 检查是否到了移动时间间隔
        {
            TRACE("[%d]--move_to->[%d]\\n", f_card->ofs_y, f_card->moveto_y); // 调试输出：当前位置到目标位置
            s16 dy;                                             // 定义移动步长变量
            f_card->tick = tick_get();                          // 更新时间戳
            dy = f_card->moveto_y - f_card->ofs_y;              // 计算到目标位置的距离
            if (dy > 0)                                         // 如果需要向下移动
            {
                if (dy > FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV) // 如果距离很大
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;              // 使用较大的步长（距离除以分割因子）
                }
                else if (dy > FOCUS_AUTO_STEP)                  // 如果距离中等
                {
                    dy = FOCUS_AUTO_STEP;                       // 使用标准步长
                }
                // 如果距离很小，保持原值（小步长移动）
            }
            else if (dy < 0)                                    // 如果需要向上移动
            {
                if (dy < -FOCUS_AUTO_STEP * FOCUS_AUTO_STEP_DIV) // 如果距离很大（负值）
                {
                    dy = dy / FOCUS_AUTO_STEP_DIV;              // 使用较大的步长（负值除以分割因子）
                }
                else if (dy < -FOCUS_AUTO_STEP)                 // 如果距离中等（负值）
                {
                    dy = -FOCUS_AUTO_STEP;                      // 使用标准步长（负值）
                }
                // 如果距离很小，保持原值（小步长移动）
            }
            func_clock_sub_card_set_offs(f_card->ofs_y + dy);  // 应用计算出的移动步长，更新位置
        }
    }

    func_clock_sub_card_data_update();                         // 更新卡片数据显示（定时器、秒表等）
    func_process();                                             // 调用通用功能处理函数

    for(u8 i=0; i<CARD_BTN_COUNT; i++)                         // 遍历所有卡片，处理文本滚动
    {
        u16 id = COMPO_CARD_START + 1 + i;                     // 计算卡片组件ID
        compo_cardbox_text_scroll_process((compo_cardbox_t *)compo_getobj_byid(id), true); // 处理卡片文本滚动动画
    }
}

/**
  * @brief  时钟表盘上拉菜单功能消息处理
  * @param  msg: 接收到的系统消息
  * @retval None
  * @note   该函数根据当前卡片状态（拖拽、自动移动、静止）分别处理不同的消息
  *         实现了状态机式的消息处理逻辑，确保在不同状态下响应合适的用户操作
  */
static void func_clock_sub_card_message(size_msg_t msg)        // 定义卡片菜单消息处理函数
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;               // 获取卡片功能控制结构体指针

    if (f_card->flag_drag)                                     // 检查是否处于拖拽状态
    {
        evt_message(msg);                                       // 拖动中，只处理部分消息，将消息传递给事件处理系统
        return;                                                 // 直接返回，不处理其他消息
    }
    else if (f_card->flag_move_auto)                           // 检查是否处于自动移动状态
    {
        switch (msg)                                            // 在自动移动状态下，只处理特定消息
        {
            case MSG_CTP_TOUCH:                                 // 处理触屏触摸消息
                f_card->flag_drag = true;                       // 设置拖拽标志为true，准备开始新的拖拽操作
                f_card->flag_move_auto = false;                // 停止自动移动，用户触摸优先级更高
                break;

            case MSG_QDEC_FORWARD:                              // 处理向前滚动消息（滚轮或按键向前）
                if (++f_card->moveto_card > f_card->focus_card) // 增加目标卡片索引，检查是否超过当前焦点卡片
                {
                    f_card->moveto_y = func_clock_sub_card_align_by_idx(f_card->moveto_card); // 计算新的目标Y坐标位置
                }
                break;

            case MSG_QDEC_BACKWARD:                             // 处理向后滚动消息（滚轮或按键向后）
                if (--f_card->moveto_card < f_card->focus_card) // 减少目标卡片索引，检查是否小于当前焦点卡片
                {
                    f_card->moveto_y = func_clock_sub_card_align_by_idx(f_card->moveto_card); // 计算新的目标Y坐标位置
                }
                break;
                break;                                          // 多余的break语句（可能是代码编辑遗留）
            default:                                            // 处理其他消息
                evt_message(msg);                               // 将未处理的消息传递给事件处理系统
                break;
        }
        return;                                                 // 自动移动状态处理完成，直接返回
    }

    switch (msg)                                                // 在静止状态下处理各种消息
    {
        case MSG_CTP_CLICK:                                     // 处理触屏点击消息
            func_clock_sub_card_click_handler();               // 调用点击处理函数，处理卡片和按钮点击
            break;
            break;                                              // 多余的break语句（可能是代码编辑遗留）

        case MSG_CTP_SHORT_UP:                                  // 处理短距离向上滑动消息
        case MSG_CTP_SHORT_DOWN:                                // 处理短距离向下滑动消息
            if (msg == MSG_CTP_SHORT_DOWN && ctp_get_sxy().y < EXIT_Y_MAX && ctp_get_sxy().y > EXIT_Y_MIN) // 检查是否为下滑且在退出区域内
            {
                func_clock_sub_card_switch_to_clock(false);    // 下滑返回到时钟主界面，false表示手动切换
            }
            else                                                // 其他滑动操作
            {
                f_card->flag_drag = true;                       // 设置拖拽标志，准备开始拖拽操作
                f_card->flag_move_auto = false;                // 停止自动移动
            }
            break;

        case MSG_CTP_SHORT_LEFT:                                // 处理短距离左滑消息
        case MSG_CTP_SHORT_RIGHT:                               // 处理短距离右滑消息
        case MSG_CTP_LONG:                                      // 处理长按消息
            break;                                              // 这些消息暂不处理，直接忽略

        case MSG_QDEC_FORWARD:                                  // 处理向前滚动消息（静止状态下）
            f_card->flag_move_auto = true;                      // 启动自动移动标志
            if (++f_card->moveto_card > f_card->focus_card)     // 增加目标卡片索引，检查是否超过当前焦点卡片
            {
                f_card->moveto_y = func_clock_sub_card_align_by_idx(f_card->moveto_card); // 计算新的目标Y坐标位置
            }
            TRACE("focus_card[%d] to_y[%d]\\n", f_card->focus_card, f_card->moveto_y); // 调试输出：当前焦点卡片和目标位置
            break;

        case MSG_QDEC_BACKWARD:                                 // 处理向后滚动消息（静止状态下）
            f_card->flag_move_auto = true;                      // 启动自动移动标志
            if (--f_card->moveto_card < f_card->focus_card)     // 减少目标卡片索引，检查是否小于当前焦点卡片
            {
                f_card->moveto_y = func_clock_sub_card_align_by_idx(f_card->moveto_card); // 计算新的目标Y坐标位置
            }
            TRACE("focus_card[%d] to_y[%d]\\n", f_card->focus_card, f_card->moveto_y); // 调试输出：当前焦点卡片和目标位置
            break;

        case KU_BACK:                                           // 处理返回键消息
            func_clock_sub_card_switch_to_clock(true);         // 单击BACK键返回到时钟主界面，true表示自动切换
            break;
        case MSG_SYS_500MS:                                     // 处理500毫秒系统定时消息
        {
// #if BT_ID3_TAG_EN                                          // 蓝牙ID3标签支持（已注释）
//             if (bt_is_connected())                          // 检查蓝牙连接状态（已注释）
//             {
//                 bt_music_paly_status_info();                // 获取音乐播放状态信息（已注释）
//             }
// #endif
//             compo_button_t *btn = compo_getobj_byid(COMPO_MUSIC_BTN_PREV); // 获取上一曲按钮（已注释）
//             if(uteModuleCallBtIsConnected() || ble_is_connect()) // 检查蓝牙连接（已注释）
//             {
//                 compo_button_set_bgimg(btn, ui_handle.card3.pic_click_prev.res_click); // 设置连接状态图标（已注释）
//             }
//             else
//             {
//                 compo_button_set_bgimg(btn, ui_handle.card3.pic_click_prev.res); // 设置未连接状态图标（已注释）
//             }

//             btn = compo_getobj_byid(COMPO_MUSIC_BTN_NEXT);  // 获取下一曲按钮（已注释）
//             if(uteModuleCallBtIsConnected() || ble_is_connect()) // 检查蓝牙连接（已注释）
//             {
//                 compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res_click); // 设置连接状态图标（已注释）
//             }
//             else
//             {
//                 compo_button_set_bgimg(btn, ui_handle.card3.pic_click_next.res); // 设置未连接状态图标（已注释）
//             }

            music_data_refresh();                              // 刷新音乐数据和按钮状态
        }
        default:                                                // 处理其他未匹配的消息
            func_message(msg);                                  // 将消息传递给通用消息处理函数
            break;
    }
}

/**
  * @brief  时钟表盘上拉菜单进入处理
  * @param  None
  * @retval None
  * @note   该函数负责初始化卡片菜单界面，包括内存分配、界面创建和初始位置设置
  */
static void func_clock_sub_card_enter(void)                    // 定义卡片菜单进入处理函数
{
    tft_set_temode(3);                                         // 设置TFT显示模式为3，可能用于优化卡片滚动时的显示性能
    func_cb.f_cb = func_zalloc(sizeof(f_card_t));              // 为卡片功能控制结构体分配内存并清零初始化
    func_cb.frm_main = func_clock_sub_card_form_create();      // 创建卡片菜单的主表单界面

    f_card_t *f_card = (f_card_t *)func_cb.f_cb;               // 获取已分配的卡片控制结构体指针
//    co_timer_set(&f_card->music_refresh, 50, TIMER_REPEAT, LEVEL_LOW_PRI, music_data_refresh, NULL); // 设置音乐刷新定时器（已注释，原用于50ms周期性刷新音乐状态）

    func_clock_sub_card_set_offs(SPRING_Y_MAX);                // 设置卡片菜单的初始Y轴偏移量为最大弹簧位置，即菜单完全收起状态
}

/**
  * @brief  时钟表盘上拉菜单退出处理
  * @param  None
  * @retval None
  * @note   该函数负责清理卡片菜单界面的资源，恢复显示模式并记录上一个功能状态
  */
static void func_clock_sub_card_exit(void)                     // 定义卡片菜单退出处理函数
{
    f_card_t *f_card = (f_card_t *)func_cb.f_cb;               // 获取卡片功能控制结构体指针
//    co_timer_del(&f_card->music_refresh);                    // 删除音乐刷新定时器（已注释，与进入函数中的定时器设置对应）
    func_cb.last = FUNC_CARD;                                  // 记录上一个功能状态为卡片菜单，用于功能切换历史记录
    tft_set_temode(DEFAULT_TE_MODE);                           // 恢复TFT显示模式为默认模式，退出卡片滚动优化模式
}


/**
  * @brief  时钟表盘上拉菜单主函数
  * @param  None
  * @retval None
  * @note   该函数是卡片菜单功能的入口点，实现完整的生命周期管理
  *         包含初始化、主循环处理和清理三个阶段
  */
void func_clock_sub_card(void)                                 // 定义卡片菜单主函数（非静态，可被外部调用）
{
    printf("%s\\n", __func__);                                  // 调试输出：打印当前函数名，用于跟踪函数调用
    func_clock_sub_card_enter();                               // 调用进入处理函数，初始化卡片菜单界面
    while (func_cb.sta == FUNC_CARD)                           // 主循环：当功能状态为FUNC_CARD时持续运行
    {
        func_clock_sub_card_process();                          // 调用事件处理函数，处理拖拽、自动移动、数据更新等核心逻辑
        func_clock_sub_card_message(msg_dequeue());            // 调用消息处理函数，处理从消息队列中取出的用户输入和系统消息
    }
    func_clock_sub_card_exit();                                // 循环结束后调用退出处理函数，清理资源
}
#endif // UTE_MODULE_SCREENS_UP_MENU_SUPPORT
