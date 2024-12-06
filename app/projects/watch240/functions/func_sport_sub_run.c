#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"

#define TRACE_EN        0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


u32 func_sport_get_current_idx(void);
u32 func_sport_get_str(u8 sport_idx);
u32 func_sport_get_ui(u8 sport_idx);

enum
{
    COMPO_ID_NUM_SPORT_TIME = 1,        //运动时间
    COMPO_ID_NUM_SPORT_HEARTRATE,   //心率
    COMPO_ID_NUM_SPORT_KCAL,        //卡路里
    COMPO_ID_NUM_SPORT_STEP,        //计步
    COMPO_ID_NUM_SPORT_KM,          //距离

    COMPO_ID_TEXT_SPORT_KCAL,
    COMPO_ID_TEXT_SPORT_STEP,
    COMPO_ID_TEXT_SPORT_KM,
    COMPO_ID_TEXT_SPORT_HEARTRATE,

    COMPO_ID_PIC_SPORT_TYPE,        //运动图标
    COMPO_ID_PIC_SPORT_HEART,
    COMPO_ID_PIC_SPORT_TIME,
    COMPO_ID_PIC_SPORT_STEP,
    COMPO_ID_PIC_SPORT_KCAL,
    COMPO_ID_PIC_SPORT_KM,

    COMPO_ID_ARC_SPORT_KM,
    COMPO_ID_ARC_SPORT_STEP,
    COMPO_ID_ARC_SPORT_KCAL,

    COMPO_ID_BTN_SPORT_STOP,
    COMPO_ID_BTN_SPORT_EXIT,

    COMPO_ID_TXT_TIME,
};

typedef struct
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u32 res;
    bool wordwap;
    bool center;
    color_t color;
    u8 wordcnt;
    char* str;
} text_t;

#define TEXT_CNT    ((int)(sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0])))
static const text_t sport_sub_run_text[] =
{
    {COMPO_ID_NUM_SPORT_TIME,       10,     66,     0,      0,  UI_BUF_0FONT_FONT_NUM_46_BIN,   false,  false,  {.r=169, .g=255, .b=0},         20,    "02:38.58"},
    {COMPO_ID_NUM_SPORT_KM,         45,     163,    0,      0,  UI_BUF_0FONT_FONT_NUM_24_BIN,   false,  false,  {.r=255, .g=255, .b=255},       10,    "50 KM"},
    {COMPO_ID_NUM_SPORT_STEP,       45,     201,    0,      0,  UI_BUF_0FONT_FONT_NUM_24_BIN,   false,  false,  {.r=255, .g=255, .b=255},       20,    "20000 STEP"},
    {COMPO_ID_NUM_SPORT_KCAL,       42,     125,    0,      0,  UI_BUF_0FONT_FONT_NUM_24_BIN,   false,  false,  {.r=255, .g=255, .b=255},       20,     "5000 KCAL"},
    {COMPO_ID_NUM_SPORT_HEARTRATE,  44,     237,    0,      0,  UI_BUF_0FONT_FONT_NUM_48_BIN,   false,  false,  {.r=255, .g=255, .b=255},       3,      "108"},

    {COMPO_ID_TEXT_SPORT_KCAL,      0,      0,      0,      0,  UI_BUF_0FONT_FONT_BIN,          false,  false,  {.r=255, .g=255, .b=255},       20,     "KCAL"},
    {COMPO_ID_TEXT_SPORT_STEP,      0,      0,      0,      0,  UI_BUF_0FONT_FONT_BIN,          false,  false,  {.r=255, .g=255, .b=255},       20,     "STEP"},
    {COMPO_ID_TEXT_SPORT_KM,        0,      0,      0,      0,  UI_BUF_0FONT_FONT_BIN,          false,  false,  {.r=255, .g=255, .b=255},       20,     "KM"},
    {COMPO_ID_TEXT_SPORT_HEARTRATE, 0,      0,      0,      0,  UI_BUF_0FONT_FONT_BIN,          false,  false,  {.r=255, .g=255, .b=255},       20,     "MIN/Cnt"},
};

typedef struct
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u32 res;
} picture_t;
#define PICTURE_CNT     ((int)(sizeof(sport_sub_run_picture)/sizeof(sport_sub_run_picture[0])))
static const picture_t sport_sub_run_picture[] =
{
    {COMPO_ID_PIC_SPORT_TYPE,   10+56/2,  12+56/2,    56,   56,       0},
    {COMPO_ID_PIC_SPORT_KM,     10+24/2,  169+24/2,   24,  24,      UI_BUF_I330001_SPORT_ICON2_DIS_BIN},
    {COMPO_ID_PIC_SPORT_STEP,   10+24/2,  207+24/2,   24,  24,      UI_BUF_I330001_SPORT_ICON2_STEP_BIN},
    {COMPO_ID_PIC_SPORT_KCAL,   10+24/2,  131+24/2,   24,  24,      UI_BUF_I330001_SPORT_ICON2_CALORIES_BIN},
    {COMPO_ID_PIC_SPORT_HEART,  10+24/2,  251+24/2,   24,  24,      UI_BUF_I330001_SPORT_ICON2_HR_BIN},
};

#if USE_GOAL_ARC
typedef struct
{
    u8 id;
    s16 x;
    s16 y;
    u16 w;
    u16 h;
    u16 width;
    color_t content_color;
    color_t bg_color;
} arc_t;
#define ARC_CNT     ((int)(sizeof(sport_sub_run_arc)/sizeof(sport_sub_run_arc[0])))
static const arc_t sport_sub_run_arc[] =
{
    {COMPO_ID_ARC_SPORT_KM, 260, 260, 40, 40, 10, {.r=0, .g=202, .b=222}, {.r=0, .g=40, .b=44}},
    {COMPO_ID_ARC_SPORT_STEP, 260, 260, 70, 70, 10, {.r=74, .g=223, .b=12}, {.r=15, .g=45, .b=2}},
    {COMPO_ID_ARC_SPORT_KCAL, 260,260, 100, 100, 10, {.r=253, .g=46, .b=129}, {.r=51, .g=9, .b=26}},
};
#endif // USE_GOAL_ARC

typedef struct f_sport_sub_run_t_
{
    u8 sta;
    u8 hour;                //时
    u8 min;                 //分
    u8 sec;                 //秒
    u16 msec;               //毫秒
    u32 total_msec;         //总毫秒

    u8 heartrate;           //心率
    u16 kcal;               //卡路里
    u16 step;               //计步
    u8 km_integer;                 //距离 整数
    u8 km_decimals;                     // 小数

    page_tp_move_t *ptm;
    bool flag_drag;
    bool flag_auto_move;
    s16 last_dx;
    s16 focus_x;
    point_t moveto;
    u32 tick;

    bool sport_run_state;
} f_sport_sub_run_t;

//创建室内跑步窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_sub_run_form_create(void)
{
    bool sport_flag[4]= {true,true,true,true};
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
//    u32 str_id = STR_NULL;
//    if (uteModuleSportMoreSportIsAppStart())                        //多运动是手机端开启的
//    {
//        str_id = func_sport_get_str(uteModuleSportMoreSportGetType()-1);
//        TRACE("【APP连接】运动的类型:%d\n", str_id);
//    }
//    else                                                            //本地点击
//    {
//        str_id = func_sport_get_str(func_sport_get_current_idx());
//        TRACE("【本地】运动的类型:%d\n", str_id);
//    }
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[str_id]);

    compo_textbox_t* time = compo_textbox_create(frm, 32);
    compo_textbox_set_align_center(time, false);
    compo_textbox_set_location(time, 168, 15, 52, 23);
    compo_setid(time, COMPO_ID_TXT_TIME);
    compo_bonddata(time, COMPO_BOND_HOURMIN_TXT);


    switch(func_sport_get_current_idx())
    {
        case 0://跑步
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 1://骑行
            sport_flag[0] = true;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 2://跳绳
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 3://游泳
            sport_flag[0] = true;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 4://羽毛球
            sport_flag[0] = false;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 5://乒乓球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 6://网球
            sport_flag[0] = false;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 7://爬山
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 8://徒步
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 9://篮球
            sport_flag[0] = false;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 10://足球
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 11://棒球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 12://排球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 13://板球
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 14://橄榄球
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 15://曲棍球
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 16://跳舞
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 17://动感单车
            sport_flag[0] = true;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 18://瑜伽
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 19://仰卧起坐
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 20://跑步机
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 21://体操
            sport_flag[0] = false;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 22://划船
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 23://开合跳
            sport_flag[0] = false;//公里
            sport_flag[1] = false;//步数
            sport_flag[2] = true;//卡路里
            break;
        case 24://自由训练
            sport_flag[0] = true;//公里
            sport_flag[1] = true;//步数
            sport_flag[2] = true;//卡路里
            break;
    }

    sport_flag[3] = true;   //心率

    //不同运行界面不同数据,用户自己添加 --> todo

    //否则采用默认运行界面

    //创建数字
    for (int i=0; i<TEXT_CNT; i++)
    {

        compo_textbox_t* txt = compo_textbox_create(frm, sport_sub_run_text[i].wordcnt);
        compo_textbox_set_font(txt, sport_sub_run_text[i].res);
        widget_text_set_color(txt->txt, make_color(sport_sub_run_text[i].color.r, sport_sub_run_text[i].color.g, sport_sub_run_text[i].color.b));
        compo_textbox_set_align_center(txt, sport_sub_run_text[i].center);
        compo_textbox_set_multiline(txt, sport_sub_run_text[i].wordwap);
        if (sport_sub_run_text[i].w == 0 || sport_sub_run_text[i].h == 0)
        {
            compo_textbox_set_autosize(txt, true);
        }
        compo_textbox_set_location(txt, sport_sub_run_text[i].x, sport_sub_run_text[i].y, sport_sub_run_text[i].w, sport_sub_run_text[i].h);
        compo_setid(txt, sport_sub_run_text[i].id);

        if(i>=1 && i<5)
        {
            compo_textbox_set_visible(txt, sport_flag[i-1]);///不同模式屏蔽不同功能
        }
        else if(i>=5)
        {
            compo_textbox_set_visible(txt, sport_flag[i-5]);///不同模式屏蔽不同功能
        }
    }

#if USE_GOAL_ARC
    //创建圆弧
    for (int i=0; i<ARC_CNT; i++)
    {
        compo_arc_t *arc = compo_arc_create(frm);
        compo_arc_set_alpha(arc, 0xff, 0xff);
        compo_arc_set_location(arc, sport_sub_run_arc[i].x, sport_sub_run_arc[i].y, sport_sub_run_arc[i].w, sport_sub_run_arc[i].h);
        compo_arc_set_width(arc, sport_sub_run_arc[i].width);
        compo_arc_set_rotation(arc, 0);
        compo_arc_set_angles(arc, 0, 3600);
        compo_arc_set_color(arc, make_color(sport_sub_run_arc[i].content_color.r, sport_sub_run_arc[i].content_color.g, sport_sub_run_arc[i].content_color.b),
                            make_color(sport_sub_run_arc[i].bg_color.r, sport_sub_run_arc[i].bg_color.g, sport_sub_run_arc[i].bg_color.b));
        widget_arc_set_edge_circle(arc->arc, true, true);
        compo_setid(arc, sport_sub_run_arc[i].id);
    }
#endif // USE_GOAL_ARC

    //创建运动类型图片
    for (int i=0; i<PICTURE_CNT; i++)
    {
        compo_picturebox_t* pic;
        if (sport_sub_run_picture[i].res)
        {
            pic = compo_picturebox_create(frm, sport_sub_run_picture[i].res);
        }
        else
        {
            pic = compo_picturebox_create(frm, func_sport_get_ui(func_sport_get_current_idx()));
        }
        compo_picturebox_set_pos(pic, sport_sub_run_picture[i].x, sport_sub_run_picture[i].y);
        if (sport_sub_run_picture[i].w !=0 && sport_sub_run_picture[i].h != 0)
        {
            compo_picturebox_set_size(pic, sport_sub_run_picture[i].w, sport_sub_run_picture[i].h);
        }
        compo_setid(pic, sport_sub_run_picture[i].id);

        if(i>=1)
        {
            compo_picturebox_set_visible(pic, sport_flag[i-1]);///不同模式屏蔽不同功能
        }
    }


    //右滑退出界面
    compo_button_t* btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_PUSED_BIN);
    compo_button_set_pos(btn, -140-120/2, 62+68/2);
    compo_button_set_visible(btn, true);
    compo_setid(btn, COMPO_ID_BTN_SPORT_STOP);

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_SPORT_BTN_CLOSE_BIN);
    compo_button_set_pos(btn, -140-120/2, 176+68/2);
    compo_button_set_visible(btn, true);
    compo_setid(btn, COMPO_ID_BTN_SPORT_EXIT);

    compo_textbox_t* text = compo_textbox_create(frm, 32);
    compo_textbox_set_location(text, -140-120/2, 134+23/2,
                               gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).wid - gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).hei,
                               gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).hei);
    compo_textbox_set(text, "暂停");

    text = compo_textbox_create(frm, 32);
    compo_textbox_set_location(text, -140-120/2, 248+23/2, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).wid - gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).hei,
                               gui_image_get_size(UI_BUF_I330001_SPORT_BTN_CLOSE_BIN).hei);
    compo_textbox_set(text, "关闭");

    return frm;
}

static void func_sport_sub_run_updata(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t *)func_cb.f_cb;
    u32 str_id = func_sport_get_str(func_sport_get_current_idx());

    ute_module_more_sports_data_t *data = ab_zalloc(sizeof(ute_module_more_sports_data_t));
    uteModuleSportGetMoreSportsDatas(data);

//    if (sys_cb.guioff_delay == 0 && !sys_cb.gui_sleep_sta)
//    {
//        gui_sleep();                //仅熄屏
//    }
//
//    reset_sleep_delay();
//    reset_pwroff_delay();

    if (f_sport_sub_run->sport_run_state == false)
    {
        goto __exit;
    }

    switch (str_id)
    {
        //不同运行界面不同数据,用户自己添加 --> todo
//    case STR_RIDE_ATVS:
//        break;
        //否则采用默认运行界面
        default:
        {
            //更新时间
            char buf[14];
            compo_textbox_t *txt_time = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_TIME) {
                    txt_time = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            f_sport_sub_run->hour = data->totalSportTime / 3600;
            f_sport_sub_run->min = ((data->totalSportTime) % 3600) / 60;
            f_sport_sub_run->sec = (data->totalSportTime) % 60;
            snprintf(buf, sizeof(buf), "%02d:%02d:%02d", f_sport_sub_run->hour, f_sport_sub_run->min, f_sport_sub_run->sec);
            compo_textbox_set(txt_time, buf);


            //更新心率
            f_sport_sub_run->heartrate = data->saveData.avgHeartRate;
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "%d", f_sport_sub_run->heartrate);
            compo_textbox_t* txt_heartrate = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_HEARTRATE) {
                    txt_heartrate = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            compo_textbox_set(txt_heartrate, buf);

            compo_textbox_t* txt_heartrate_unit = compo_getobj_byid(COMPO_ID_TEXT_SPORT_HEARTRATE);
            area_t rel_text_area = widget_text_get_area(txt_heartrate->txt);
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_HEARTRATE) {
                    compo_textbox_set_location(txt_heartrate_unit, sport_sub_run_text[i].x + rel_text_area.wid + 10, sport_sub_run_text[i].y+8, 0, 0);
                    break;
                }
            }
            compo_textbox_set(txt_heartrate_unit, "次/分");


            //更新卡路里
            f_sport_sub_run->kcal = data->saveData.sportCaloire;
            memset(buf, 0, sizeof(buf));
//        snprintf(buf, sizeof(buf), "%dKCAL", f_sport_sub_run->kcal);
            snprintf(buf, sizeof(buf), "%d", f_sport_sub_run->kcal);
            compo_textbox_t* txt_kcal = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_KCAL) {
                    txt_kcal = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            compo_textbox_set(txt_kcal, buf);

            compo_textbox_t* txt_kcal_unit = compo_getobj_byid(COMPO_ID_TEXT_SPORT_KCAL);
            rel_text_area = widget_text_get_area(txt_kcal->txt);
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_KCAL) {
                    compo_textbox_set_location(txt_kcal_unit, sport_sub_run_text[i].x + rel_text_area.wid + 10, sport_sub_run_text[i].y+8, 0, 0);
                    break;
                }
            }
            compo_textbox_set(txt_kcal_unit, i18n[STR_SET_CALORIE]);

            //更新计步
            f_sport_sub_run->step = data->saveData.sportStep;
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "%d", f_sport_sub_run->step);
            compo_textbox_t* txt_step = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_STEP) {
                    txt_step = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            compo_textbox_set(txt_step, buf);

            compo_textbox_t* txt_step_unit = compo_getobj_byid(COMPO_ID_TEXT_SPORT_STEP);
            rel_text_area = widget_text_get_area(txt_step->txt);
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_STEP) {
                    compo_textbox_set_location(txt_step_unit, sport_sub_run_text[i].x + rel_text_area.wid + 10, sport_sub_run_text[i].y+8, 0, 0);
                    break;
                }
            }
            compo_textbox_set(txt_step_unit, i18n[STR_SET_STEP_COUNT]);

            //更新距离
            f_sport_sub_run->km_integer = data->saveData.sportDistanceInteger;
            f_sport_sub_run->km_decimals = data->saveData.sportDistanceDecimals;
            memset(buf, 0, sizeof(buf));
            snprintf(buf, sizeof(buf), "%d.%02d", f_sport_sub_run->km_integer, f_sport_sub_run->km_decimals);
            compo_textbox_t* txt_km = NULL;
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_KM) {
                    txt_km = compo_getobj_byid(sport_sub_run_text[i].id);
                    break;
                }
            }
            compo_textbox_set(txt_km, buf);

            compo_textbox_t* txt_km_unit = compo_getobj_byid(COMPO_ID_TEXT_SPORT_KM);
            rel_text_area = widget_text_get_area(txt_km->txt);
            for (u8 i=0; i<sizeof(sport_sub_run_text)/sizeof(sport_sub_run_text[0]); i++) {
                if (sport_sub_run_text[i].id == COMPO_ID_NUM_SPORT_KM) {
                    compo_textbox_set_location(txt_km_unit, sport_sub_run_text[i].x + rel_text_area.wid + 10, sport_sub_run_text[i].y+8, 0, 0);
                    break;
                }
            }
            compo_textbox_set(txt_km_unit, i18n[STR_SET_DISTANCE]);

#if USE_GOAL_ARC
            //更新圆弧
            compo_arc_t* rc_km = compo_getobj_byid(sport_sub_run_arc[0].id);
            compo_arc_set_value(rc_km, f_sport_sub_run->km * (ARC_VALUE_MAX / 100));
            compo_arc_t* rc_step = compo_getobj_byid(sport_sub_run_arc[1].id);
            compo_arc_set_value(rc_step, f_sport_sub_run->step * ARC_VALUE_MAX / 30000);
            compo_arc_t* rc_kcal = compo_getobj_byid(sport_sub_run_arc[2].id);
            compo_arc_set_value(rc_kcal, f_sport_sub_run->kcal * ARC_VALUE_MAX / 6000);
#endif // USE_GOAL_ARC
        }

        break;

    }

__exit:
    ab_free(data);

}

//功能事件处理
static void func_sport_sub_run_process(void)
{
    static u32 sport_ticks = 0;

    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    compo_button_t* btn = compo_getobj_byid(COMPO_ID_BTN_SPORT_STOP);
    if (btn != NULL)
    {
        if (uteModuleSportMoreSportIsAppStart())                        //多运动是手机端开启的
        {
            u8 ute_sport_status = uteModuleSportMoreSportGetStatus();

            switch (ute_sport_status)
            {
                case ALL_SPORT_STATUS_CLOSE:
                case ALL_SPORT_STATUS_PAUSE:
                    compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PLAY_BIN);
                    f_sport_sub_run->sport_run_state = false;
                    TRACE("【APP连接】运动停止/退出\n");
                    break;

                case ALL_SPORT_STATUS_OPEN:
                case ALL_SPORT_STATUS_CONTINUE:
                    compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PUSED_BIN);
                    f_sport_sub_run->sport_run_state = true;
                    TRACE("【APP连接】运动开始/继续\n");
                    break;

                default:
                    break;
            }
        }
    }

    if (f_sport_sub_run->flag_drag)
    {
        s32 dy=0,dx=0;
        f_sport_sub_run->flag_drag = ctp_get_dxy(&dx, &dy);
        if (f_sport_sub_run->flag_drag)
        {
            f_sport_sub_run->focus_x = f_sport_sub_run->last_dx + dx;
            if (f_sport_sub_run->focus_x > 320)
            {
                f_sport_sub_run->focus_x = 320;
            }
            else if (f_sport_sub_run->focus_x < 0)
            {
                f_sport_sub_run->focus_x = 0;
            }
            widget_page_set_client(func_cb.frm_main->page_body, f_sport_sub_run->focus_x, 0);
        }
        else     //松手
        {
            s32 dx = f_sport_sub_run->focus_x - f_sport_sub_run->last_dx;
            f_sport_sub_run->last_dx = f_sport_sub_run->focus_x;
            //printf("dx=%d, last_dx = %d\n", dx, f_sport_sub_run->last_dx);
            if (abs_s(dx) > GUI_SCREEN_WIDTH / 5)
            {
                if (dx > 0)
                {
                    f_sport_sub_run->moveto.x = 320;
                    f_sport_sub_run->moveto.y = 0;
                }
                else if (dx < 0)
                {
                    f_sport_sub_run->moveto.x = 0;
                    f_sport_sub_run->moveto.y = 0;
                }
            }
            f_sport_sub_run->flag_auto_move = true;
        }
    }
    else if (f_sport_sub_run->flag_auto_move)
    {
        if (f_sport_sub_run->last_dx == f_sport_sub_run->moveto.x)
        {
            f_sport_sub_run->flag_auto_move = false;
        }
        else if (tick_check_expire(f_sport_sub_run->tick, 16))
        {
            f_sport_sub_run->tick = tick_get();
            if (f_sport_sub_run->last_dx < f_sport_sub_run->moveto.x)
            {
                f_sport_sub_run->last_dx += 16;
                f_sport_sub_run->last_dx = (f_sport_sub_run->last_dx > f_sport_sub_run->moveto.x) ? f_sport_sub_run->moveto.x : f_sport_sub_run->last_dx;
            }
            else if (f_sport_sub_run->last_dx > f_sport_sub_run->moveto.x)
            {
                f_sport_sub_run->last_dx -= 16;
                f_sport_sub_run->last_dx = (f_sport_sub_run->last_dx < f_sport_sub_run->moveto.x) ? f_sport_sub_run->moveto.x : f_sport_sub_run->last_dx;
            }
            widget_page_set_client(func_cb.frm_main->page_body, f_sport_sub_run->last_dx, 0);
        }
    }

    if (tick_check_expire(sport_ticks, 10))
    {
        sport_ticks = tick_get();
        //界面数据更新
        func_sport_sub_run_updata();
    }


    func_process();
}

//按键处理
static void func_sport_sub_run_click_handler(void)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SPORT_STOP:
        {
            //停止按钮
            compo_button_t* btn = compo_getobj_byid(COMPO_ID_BTN_SPORT_STOP);
            if (btn != NULL)
            {
                if (f_sport_sub_run->sport_run_state == true)
                {
                    compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PLAY_BIN);
                    widget_set_size(btn->widget, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).wid, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).hei);
                    f_sport_sub_run->sport_run_state = false;
                    if (uteModuleSportMoreSportIsAppStart())                        //多运动是手机端开启的
                    {
                        if (sys_cb.sport_app_disconnect)                            //判断app是否断链，断链停止运动
                        {
                            compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PLAY_BIN);
                            widget_set_size(btn->widget, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).wid, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).hei);
                            f_sport_sub_run->sport_run_state = false;
                            uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_PAUSE);   //通知APP暂停运动
                            TRACE("【APP连接】本地通知APP运动停止\n");
                        }
                        else
                        {
                            uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_PAUSE);   //通知APP暂停运动
                        }
                    }
                }
                else if (f_sport_sub_run->sport_run_state == false)
                {
                    compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PUSED_BIN);
                    f_sport_sub_run->sport_run_state = true;
                    if (uteModuleSportMoreSportIsAppStart())                        //多运动是手机端开启的
                    {
                        if (sys_cb.sport_app_disconnect)                            //判断app是否断链，断链停止运动
                        {
                            compo_button_set_bgimg(btn, UI_BUF_I330001_SPORT_BTN_PLAY_BIN);
                            widget_set_size(btn->widget, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).wid, gui_image_get_size(UI_BUF_I330001_SPORT_BTN_PUSED_BIN).hei);
                            f_sport_sub_run->sport_run_state = false;
                            uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_PAUSE);   //通知APP暂停运动
                            TRACE("【APP连接】本地通知APP运动继续\n");
                        }
                        else
                        {
                            uteModuleSportSyncAppSportStatus(ALL_SPORT_STATUS_CONTINUE);   //通知APP继续运动
                        }
                    }
                }
            }
        }
        break;
        case COMPO_ID_BTN_SPORT_EXIT:
        {
            const char* msg = NULL;
            //退出按钮
//            if (uteModuleSportMoreSportIsAppStart())   //多运动是手机端开启的
//            {
            if (uteModuleSportMoreSportsIsLessData())
            {
                msg = i18n[STR_SPORT_EXIT_MSG2];
            }
            else
            {
                msg = i18n[STR_SPORT_EXIT_MSG1];
            }
//            }
//            else
//            {
//                msg = i18n[STR_SPORT_EXIT_MSG3];
//            }
            int res = msgbox((char*)msg, NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, 0);
            if (res == MSGBOX_RES_OK)
            {
//                if (uteModuleSportMoreSportIsAppStart()) {                      //多运动是手机端开启的
                uteModuleSportStopMoreSports();                             //通知APP退出运动
                TRACE("【APP连接】本地通知APP运动退出\n");
//                } else {
//                    task_stack_pop();
//                    func_switch_to(FUNC_SPORT, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
//                }
                if (uteModuleSportMoreSportsIsLessData())
                {

                }
            }
            else if (res == MSGBOX_RES_CANCEL)
            {

            }
        }   break;
        default:
            break;
    }
}

//室内跑步功能消息处理
static void func_sport_sub_run_message(size_msg_t msg)
{
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sport_sub_run_click_handler();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
            f_sport_sub_run->flag_drag = true;
            f_sport_sub_run->flag_auto_move = false;
            //func_switch_to(FUNC_SPORT_SUB_RUN_EIXT, FUNC_SWITCH_LR_ZOOM_RIGHT);
            break;

        case KU_BACK:
            f_sport_sub_run->flag_auto_move = true;
            f_sport_sub_run->moveto.x = 320;
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入室内跑步功能
static void func_sport_sub_run_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_sport_sub_run_t));
    func_cb.frm_main = func_sport_sub_run_form_create();
    f_sport_sub_run_t *f_sport_sub_run = (f_sport_sub_run_t*)func_cb.f_cb;
    f_sport_sub_run->sport_run_state = true;
    if (uteModuleSportMoreSportIsAppStart())
    {
        //uteModuleSportStartMoreSports(uteModuleSportMoreSportGetType(), 1, 1);
        if (func_cb.last != FUNC_SPORT_SWITCH)
        {
            func_cb.sta = FUNC_SPORT_SWITCH;
        }
        else
        {
            uteModuleSportSetCountZeroIndex(0);
        }
        TRACE("【APP】开始运动\n");
    }
    else
    {
        uteModuleSportStartMoreSports(func_sport_get_current_idx()+1, 1, 0);
        uteModuleSportSetCountZeroIndex(0);
        TRACE("【本地】开始运动:%d\n",func_sport_get_current_idx()+1);
    }

}

//退出室内跑步功能
static void func_sport_sub_run_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    if (task_stack_get_top() == FUNC_SPORT_SUB_RUN)
    {
        task_stack_pop();
    }
    func_cb.last = FUNC_SPORT_SUB_RUN;
}

//室内跑步功能
void func_sport_sub_run(void)
{
    printf("%s\n", __func__);
    func_sport_sub_run_enter();
    while (func_cb.sta == FUNC_SPORT_SUB_RUN)
    {
        func_sport_sub_run_process();
        func_sport_sub_run_message(msg_dequeue());
    }
    func_sport_sub_run_exit();
}


