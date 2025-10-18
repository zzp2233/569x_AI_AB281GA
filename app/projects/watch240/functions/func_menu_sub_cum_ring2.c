#include "include.h"
#include "func.h"
#include "func_menu.h"
#include "func_menu_ui_data.h"
#include "ute_module_systemtime.h"
#include "ute_module_menstrualcycle.h"

#if UTE_MENU_STYLE_CUM_RING2_FUNCTION
///> feat: menu ring2 code update, shadowX, 20250704, start

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


#if UTE_MODULE_SCREENS_MENU_DATA_BIND
#define MENU_SKYRER_CNT                             f_menu_ui_data_get_app_num()                           //图标总数
#else
#define MENU_SKYRER_CNT                             ((int)(sizeof(tbl_menu_skyrer) / sizeof(tbl_menu_skyrer[0])))                           //图标总数
#endif
#define SKYRER_LOOP_ICON_NUM                        12                                                                                      //环形个数
#define SKYRER_OTHER_ICON_NUM                       0                                                                                       //方形个数
#define SKYRER_EDGE_SPACE                           2                                                                                       //边缘距离
#define SKYRER_ICON_SIZE_ORG                        (gui_image_get_size(tbl_menu_skyrer[0].res_addr).wid)                                   //原始图标大小
#define SKYRER_ICON_SIZE_MAX                        ((SKYRER_ICON_SIZE_ORG >> 4) * 15)                                                      //图标最大尺寸
#define SKYRER_ICON_SIZE_MIN                        ((SKYRER_ICON_SIZE_MAX >> 4) * 15)                                                      //图标最小尺寸
#define SKYRER_CENX                                 GUI_SCREEN_CENTER_X                                                                     //中心点
#define SKYRER_CENY                                 GUI_SCREEN_CENTER_Y                                                                     //中心点
#define SKYRER_SHORT_SIDE                           ((GUI_SCREEN_HEIGHT >= GUI_SCREEN_WIDTH) ? GUI_SCREEN_WIDTH : GUI_SCREEN_HEIGHT)        //取短边
#define SKYRER_ROTATE_RADIUS                        ((SKYRER_SHORT_SIDE >> 1) - (SKYRER_ICON_SIZE_MAX >> 1) - SKYRER_EDGE_SPACE)            //旋转半径
#define SKYRER_ANGLE_DIV                            300                                                                                     //间隔角度
#define SKYRER_ANGLE_INITIAL                        900                                                                                     //初始角度
#define SKYRER_ANIMATION_DIFF                       (SKYRER_ANGLE_DIV / 6)                                                                  //入场动画差量角度必须为 SKYRER_ANGLE_DIV 的约数
#define SKYRER_ANIMATION_KICK                       12                                                                                      //入场动画刷新间隔 越大越慢
#define SKYRER_MOVE_AUTO_KICK                       16                                                                                      //自动移动刷新间隔 越大越慢

enum
{
    COMPO_ID_SKYRER = 1,
    COMPO_ID_APP_TXT,
    COMPO_ID_SHAPE_POINT,
};

typedef struct f_menu_skyrer_t_
{
    compo_rings_t *rings;
    bool flag_drag;                 //开始拖动
    bool flag_move_auto;            //自动移到坐标
    bool flag_move_inertia;         //惯性运动
    u8   spawn_id;                  //出生点
    u8   spawn_idx;                 //目标编号
    uint32_t ticks;                 //滑动时间
    int32_t  diff_angle;            //角度差量
    int32_t  align_angle;           //对齐角度
    int32_t  accelera;              //惯性加速度
    int8_t   rotate_dir;            //旋转方向 0:无方向;1:顺时针;-1:逆时针
    s32 sx;
    s32 sy;
    s32 x;
    s32 y;
} f_menu_skyrer_t;

typedef struct f_menu_skyrer_party_t
{
    s8 sgnx;
    s8 sgny;
    u8 idx;
} f_menu_skyrer_party;

typedef struct f_menu_skyrer_back_t
{
    u8 idx;
} f_menu_skyrer_back;

f_menu_skyrer_back ring2_back_idx[SKYRER_LOOP_ICON_NUM];

#if UTE_MODULE_SCREENS_MENU_DATA_BIND
static compo_rings_item_t tbl_menu_skyrer[MENU_APP_MAX_CNT];
#else
//天圆地方图标列表及顺序
static compo_rings_item_t tbl_menu_skyrer[] =
{
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    {.func_sta=FUNC_CALL,                       .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_CALL_BIN,              .str_id=STR_PHONE},                 //电话
#endif
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    {.func_sta=FUNC_ACTIVITY,                   .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_ACTIVITY_BIN,          .str_id=STR_EVREY_DAY_ACTIVITY},    //每日活动
#endif
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    {.func_sta=FUNC_HEARTRATE,                  .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_HEART_BIN,             .str_id=STR_HEART_RATE},            //心率
#endif
#if UTE_MODULE_SCREENS_PRESSURE_SUPPORT
    {.func_sta=FUNC_PRESSURE,                   .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_STRESS_BIN,            .str_id=STR_STRESS},               //压力
#endif
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    {.func_sta=FUNC_SPORT,                      .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SOPRT_BIN,             .str_id=STR_SPORTS},                //运动
#endif
#if UTE_MODULE_SPORTS_HSTORY_RECORD_SUPPORT
    {.func_sta=FUNC_SPORT_HISTORY,              .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SPORTS_RECORD_BIN,     .str_id=STR_EXERCISE_RECODE},                //运动记录
#endif
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    {.func_sta=FUNC_BLOOD_OXYGEN,               .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SPO2_BIN,              .str_id=STR_BLOOD_OXYGEN},          //血氧
#endif
#if UTE_MODULE_SCREENS_MEASURE_SUPPORT
    {.func_sta = FUNC_MEASURE,                  .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_KEY_MEASURE_BIN,         .str_id=STR_MEASURE},       //一键测量
#endif
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    {.func_sta=FUNC_SLEEP,                      .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SLEEP_BIN,             .str_id=STR_SLEEP},                 //睡眠
#endif
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    {.func_sta=FUNC_BT,                         .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_MUSIC_BIN,             .str_id=STR_MUSIC},                 //音乐
#endif
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    {.func_sta=FUNC_WEATHER,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_WEATHER_BIN,           .str_id=STR_WEATHER},               //天气
#endif
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    {.func_sta=FUNC_ALARM_CLOCK,                .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_ALARM_BIN,             .str_id=STR_ALARM_CLOCK},           //闹钟
#endif
#if UTE_MODULE_SCREENS_EMOTION_SUPPORT
    {.func_sta=FUNC_MOOD,                       .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_MOOD_BIN,              .str_id=STR_EMOTION},              //情绪
#endif
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    {.func_sta=FUNC_BREATHE,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_BREATHING_BIN,         .str_id=STR_BREATHE_TRAIN},         //呼吸
#endif
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    {.func_sta=FUNC_MESSAGE,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_MESSAGE_BIN,           .str_id=STR_MESSAGE},               //消息
#endif
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    {.func_sta=FUNC_VOICE,                      .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_VOICE_ASSISTANT_BIN,   .str_id=STR_VOICE},                 //语音助手
#endif
//#if UTE_MODULE_SCREENS_SUB_SOS_SUPPORT
//    {.func_sta=FUNC_SUB_SOS,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SOS_BIN,               .str_id=STR_SOS},                  //SOS
//#endif
#if UTE_MODULE_SCREENS_TOOLBOX_SUPPORT
    {.func_sta=FUNC_TOOLBOX,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_TOOLBOX_BIN,           .str_id=STR_TOOL_BOX},             //工具箱
#endif
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    {.func_sta=FUNC_SETTING,                    .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_SETTINGS_BIN,          .str_id=STR_SETTING},               //设置
#endif
#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT
    {.func_sta=FUNC_WOMEN_HEALTH,               .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PERIOD_BIN,            .str_id=STR_WOMEN_HEALTH},          //女性健康
#endif
#if UTE_MODULE_SCREENS_WORLD_CLOCK_SUPPORT
    {.func_sta = FUNC_WORLD_CLOCK,              .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_WORLD_TIME_BIN,        .str_id=STR_WORLD_CLOCK},          //世界时钟
#endif
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    {.func_sta = FUNC_GAME,                     .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_GAME_BIN,              .str_id=STR_GAME},          //游戏
#endif
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    { .func_sta = FUNC_STOPWATCH,            .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_STOPWATCH_BIN,       .str_id=STR_STOP_WATCH},  //秒表
#endif
#if UTE_MODULE_SCREENS_TIMER_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    { .func_sta = FUNC_TIMER,                 .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_TIMER_BIN,          .str_id= STR_TIMER},     //计时器
#endif
#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    { .func_sta = FUNC_FINDPHONE,            .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_FINDPHONE_BIN,       .str_id=STR_FIND_PHONE},  //找手机
#endif
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    { .func_sta = FUNC_FLASHLIGHT,            .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_FLASHLIGHT_BIN,     .str_id= STR_FLASHLIGHT}, //手电筒
#endif
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    { .func_sta = FUNC_CAMERA,                .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PHOTO_BIN,          .str_id=STR_CAMERA},     //遥控拍照
#endif
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    { .func_sta = FUNC_CALCULATOR,            .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_CALCULATOR_BIN,     .str_id= STR_CALCULATOR}, //计算器
#endif
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT && (!UTE_MODULE_SCREENS_TOOLBOX_SUPPORT)
    { .func_sta = FUNC_CALENDAER,             .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PHOTO_BIN,          .str_id= STR_CALENDAR}, //日历
#endif
#if APP_CUSTOM_REMIND_SETTING_SUPPORT
    {.func_sta = FUNC_REMIND_PRAYER,          .res_addr=UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PRAY_BIN,           .str_id=STR_PRAYER_REMIND},         //祈祷
#endif
};
#endif

static const int16_t tbl_party_angle[1] =
{
    SKYRER_ANGLE_DIV * 9,
};

static const f_menu_skyrer_party tal_party[1] =
{
    {3, 5, 1},
};

static uint16_t func_menu_sub_skyrer_icon_size_update(compo_rings_t *rings, int32_t angle);
static void func_menu_sub_skyrer_icon_party_update(compo_rings_t *rings);
static void func_menu_sub_skyrer_update(compo_rings_t *rings, int32_t diff_angle, bool swp);
static void func_menu_sub_skyrer_animation_set(compo_rings_t *rings);
static void func_menu_sub_skyrer_animation_update(void);
static void func_menu_sub_skyrer_switch_backup(compo_rings_t *rings, u8 idx);
static void func_menu_sub_skyrer_set_focus(compo_rings_t *rings);

u8 func_menu_sub_cum_ring2_get_first_idx(void)
{
    return ring2_back_idx[0].idx;
}

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_sub_cum_ring2_form_create(void)
{
#if UTE_MODULE_SCREENS_MENU_DATA_BIND
    memset(tbl_menu_skyrer,0,sizeof(tbl_menu_skyrer));
    for (int i = 0; i < MENU_SKYRER_CNT ; i++)
    {
        tbl_menu_skyrer[i].func_sta= f_menu_data[i].func_sta;
        tbl_menu_skyrer[i].res_addr= f_menu_data[i].res_addr;
        tbl_menu_skyrer[i].str_id  = f_menu_data[i].str_idx;
    }
#endif

    //新建窗体
    compo_form_t *frm = compo_form_create(false);       //菜单一般创建在底层

    //新建菜单
    compo_rings_t *rings = compo_rings_create(frm);
    compo_rings_ele_t ele =
    {
        .x              = SKYRER_CENX,
        .y              = SKYRER_CENY,
        .r              = SKYRER_ROTATE_RADIUS,
        .div            = -SKYRER_ANGLE_DIV,            //逆时针
        .angle          = SKYRER_ANGLE_INITIAL,
        .loop_icon_num  = SKYRER_LOOP_ICON_NUM,
        .other_icon_num = SKYRER_OTHER_ICON_NUM,
        .item_num       = MENU_SKYRER_CNT,
        .item           = tbl_menu_skyrer,
    };

    int j, k;
    for (j=0, k=0; j<MENU_SKYRER_CNT; j++,k++)
    {
//        ute_personal_info_t personal;
//        uteApplicationCommonGetPersonalInfo(&personal);
        if ((!uteModuleMenstrualCycleIsOpen() && (coo_menu_list[j].func_sta == FUNC_WOMEN_HEALTH)) || \
            (!uteModuleSystemtimeGetSupportRemindSetting() && (coo_menu_list[j].func_sta == FUNC_REMIND_PRAYER)))
        {
            k--;
            continue;
        }
        tbl_menu_skyrer[k].func_sta = coo_menu_list[j].func_sta;
        tbl_menu_skyrer[k].res_addr = coo_menu_list[j].res_addr;
        tbl_menu_skyrer[k].str_id   = coo_menu_list[j].str_id;
    }
    menuCnt = k;
//    for (int i=0; i<MENU_SKYRER_CNT; i++)
//    {
//#if APP_CUSTOM_REMIND_SETTING_SUPPORT
//        if(!uteModuleSystemtimeGetSupportRemindSetting())
//        {
//            if(tbl_menu_skyrer[i].func_sta == FUNC_REMIND_PRAYER)
//            {
//                tbl_menu_skyrer[i].func_sta = FUNC_FLASHLIGHT;
//                tbl_menu_skyrer[i].res_addr = UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_FLASHLIGHT_BIN;
//                tbl_menu_skyrer[i].str_id   = STR_FLASHLIGHT;
//                break;
//            }
//        }
//        else
//        {
//            if(tbl_menu_skyrer[i].func_sta == FUNC_FLASHLIGHT)
//            {
//                tbl_menu_skyrer[i].func_sta = FUNC_REMIND_PRAYER;
//                tbl_menu_skyrer[i].res_addr = UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PRAY_BIN;
//                tbl_menu_skyrer[i].str_id   = STR_PRAYER_REMIND;
//                break;
//            }
//        }
//#endif
//#if UTE_MODULE_SCREENS_WOMEN_HEALTH_SUPPORT
//        if(uteModuleMenstrualCycleIsOpen())
//        {
//            if(tbl_menu_skyrer[i].func_sta == FUNC_TOOLBOX)
//            {
//                tbl_menu_skyrer[i].func_sta = FUNC_WOMEN_HEALTH;
//                tbl_menu_skyrer[i].res_addr = UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_PERIOD_BIN;
//                tbl_menu_skyrer[i].str_id   = STR_WOMEN_HEALTH;
//                break;
//            }
//        }
//        else
//        {
//            if(tbl_menu_skyrer[i].func_sta == FUNC_WOMEN_HEALTH)
//            {
//                tbl_menu_skyrer[i].func_sta = FUNC_TOOLBOX;
//                tbl_menu_skyrer[i].res_addr = UI_BUF_I341001_2_HONEYCOMB_CIRCLE_ICON_TOOLBOX_BIN;
//                tbl_menu_skyrer[i].str_id   = STR_TOOL_BOX;
//                break;
//            }
//        }
//#endif
//    }
    compo_rings_set(rings, &ele);
    //环形图标
    for (int i = 0; i < SKYRER_LOOP_ICON_NUM; i++)
    {
        compo_rings_icon_t *icon = compo_rings_icon_add(rings, tbl_menu_skyrer[i].res_addr, i, true);
        if (icon)
        {
            int32_t angle = compo_rings_loop_icon_find_angle(rings, i);
            uint16_t size = func_menu_sub_skyrer_icon_size_update(rings, angle);
            compo_rings_set_size(rings, size, i);
        }
    }

    compo_textbox_t* text = compo_textbox_create(frm, 50);
    compo_setid(text, COMPO_ID_APP_TXT);
#if 1//UTE_MENU_STYLE_CUM_RING2_TRIANGLE_PIC
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_27_MORE_POINT_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_WIDTH * 0.25 + 6, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(picbox, false);
    compo_setid(picbox, COMPO_ID_SHAPE_POINT);
#else
    compo_shape_t* point = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(point, GUI_SCREEN_WIDTH * 0.25, GUI_SCREEN_CENTER_Y, 12, 12);
    compo_shape_set_radius(point, 12);
    compo_shape_set_visible(point, false);
    compo_setid(point, COMPO_ID_SHAPE_POINT);
#endif

#if UTE_MENU_CUM_RING_CLOCK_APP_DISP
    //时钟指针
    compo_rings_add_time(rings, COMPO_RINGS_TIME_TYPE_HOUR, UI_BUF_ICON_CLOCK_H_BIN, 1, 2, SKYRER_ICON_SIZE_ORG);
    compo_rings_add_time(rings, COMPO_RINGS_TIME_TYPE_MIN, UI_BUF_ICON_CLOCK_M_BIN, 1, 2, SKYRER_ICON_SIZE_ORG);
    compo_rings_add_time(rings, COMPO_RINGS_TIME_TYPE_SEC, UI_BUF_ICON_CLOCK_S_BIN, 9, 2, SKYRER_ICON_SIZE_ORG);
    compo_rings_set_start_angle(rings, 900);
    compo_rings_idx_time_set(rings, 0);
#endif

    compo_setid(rings, COMPO_ID_SKYRER);

    if (func_cb.flag_animation)
    {
        func_menu_sub_skyrer_animation_set(rings);
    }
    else
    {
        func_menu_sub_skyrer_set_focus(rings);
        func_menu_sub_skyrer_update(rings, 0, false);
    }

    return frm;
}

//点进图标进入应用
static void func_menu_sub_skyrer_icon_click(void)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    compo_rings_t *rings = f_menu->rings;

    if (func_cb.flag_animation) return;

    u8 func_sta;

    compo_rings_sel_t *sel = compo_rings_select(rings, ctp_get_sxy().x, ctp_get_sxy().y);

    if ((sel->idx % menuCnt) < 0 || (sel->idx % menuCnt) >= menuCnt)
    {
        return;
    }

    //根据图标索引获取应用ID
    func_sta = tbl_menu_skyrer[sel->idx % menuCnt].func_sta;

    // //切入应用
    if (func_sta > 0)
    {
        func_menu_sub_skyrer_switch_backup(rings, sel->idx);
        compo_form_t *frm = func_create_form(func_sta);
        func_switching(FUNC_SWITCH_ZOOM_ENTER | FUNC_SWITCH_AUTO, sel->icon);
        compo_form_destroy(frm);
        func_cb.sta = func_sta;
        func_cb.menu_idx = sel->idx;                //记住当前编号
        if (sel->idx % menuCnt == 0)        //时钟则开启入场动画
        {
            func_cb.flag_animation = true;
        }
        else
        {
            func_cb.flag_animation = false;
        }
    }
}

//切换到时钟
static void func_menu_sub_skyrer_switch_to_clock(void)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    compo_rings_t *rings = f_menu->rings;
    widget_icon_t *icon = compo_rings_select_byidx(rings, 0);
    u8 func_sta = FUNC_CLOCK;
    compo_form_t *frm = func_create_form(func_sta);
    func_switching(FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO, icon);
    compo_form_destroy(frm);
    func_cb.sta = func_sta;
    func_cb.menu_idx = 0;
    func_cb.menu_idx_angle = -1;
    func_cb.flag_animation = true;
}

//备份环形图标
static void func_menu_sub_skyrer_switch_backup(compo_rings_t *rings, u8 idx)
{
    for (int i = 0; i < SKYRER_LOOP_ICON_NUM; i++)
    {
        u8 idx0 = compo_rings_loop_icon_find_idx(rings, i);
        ring2_back_idx[i].idx = idx0;
        if (idx0 == idx % menuCnt)
        {
            func_cb.menu_idx_angle = compo_rings_loop_icon_find_angle(rings, i);
        }
    }
}

//设置图标集合焦点
static void func_menu_sub_skyrer_set_focus(compo_rings_t *rings)
{
    u8 foucs_angle_idx = 0;
    for (int i = 0; i < SKYRER_LOOP_ICON_NUM; i++)
    {
        if (ring2_back_idx[i].idx == 0)
        {
            foucs_angle_idx++;
        }
    }
    if (foucs_angle_idx > 1) return;
    for (int i = 0; i < SKYRER_LOOP_ICON_NUM; i++)
    {
        compo_rings_set_uidx(rings, ring2_back_idx[i].idx, i);                              //设置IDX
        compo_rings_set_res(rings, tbl_menu_skyrer[ring2_back_idx[i].idx].res_addr, i);     //设置图片
        if (ring2_back_idx[i].idx == func_cb.menu_idx % menuCnt)
        {
            foucs_angle_idx = i;
        }
    }

    //小于就加
    for (int i = foucs_angle_idx; i >= 0; i--)
    {
        int32_t angle = func_cb.menu_idx_angle + (foucs_angle_idx - i) * SKYRER_ANGLE_DIV;
        angle = RINGS_GET_ANGLE(angle);
        compo_rings_set_angle(rings, angle, i);
    }

    //大于就减
    for (int i = foucs_angle_idx; i < SKYRER_LOOP_ICON_NUM; i++)
    {
        int32_t angle = func_cb.menu_idx_angle - (i - foucs_angle_idx) * SKYRER_ANGLE_DIV;
        angle = RINGS_GET_ANGLE(angle);
        compo_rings_set_angle(rings, angle, i);
    }
}

//入场动画配置
static void func_menu_sub_skyrer_animation_set(compo_rings_t *rings)
{
    for (int i = 0; i < SKYRER_LOOP_ICON_NUM; i++)
    {
        compo_rings_set_angle(rings, SKYRER_ANGLE_INITIAL, i);
        uint16_t size = func_menu_sub_skyrer_icon_size_update(rings, SKYRER_ANGLE_INITIAL);
        compo_rings_set_size(rings, size, i);
    }
}

//入场动画刷新
static void func_menu_sub_skyrer_animation_update(void)
{
    if (!func_cb.flag_animation) return;
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    compo_rings_t *rings = f_menu->rings;

    if (tick_check_expire(f_menu->ticks, SKYRER_ANIMATION_KICK))        //动画刷新
    {
        f_menu->ticks = tick_get();
        int32_t end_angle = compo_rings_loop_icon_find_angle(rings, SKYRER_LOOP_ICON_NUM-1);

        if (end_angle == (SKYRER_ANGLE_INITIAL + SKYRER_ANGLE_DIV))     //动画结束
        {
            func_menu_sub_skyrer_icon_party_update(rings);
            func_cb.flag_animation = false;
        }
        else                                                            //动画开始
        {
            compo_rings_loop_icon_set_angle_diff(rings, SKYRER_ANIMATION_DIFF, 0);         //差量角度必须为 SKYRER_ANGLE_DIV 的约数
            int32_t angle0 = compo_rings_loop_icon_find_angle(rings, 0);
            uint16_t size0 = func_menu_sub_skyrer_icon_size_update(rings, angle0);
            compo_rings_set_size(rings, size0, 0);

            for (int i = 1; i < SKYRER_LOOP_ICON_NUM; i++)
            {
                int32_t pre_angle = compo_rings_loop_icon_find_angle(rings, i-1);
                int32_t cur_angle = compo_rings_loop_icon_find_angle(rings, i);
                if (abs(pre_angle - cur_angle) > SKYRER_ANGLE_DIV)
                {
                    compo_rings_loop_icon_set_angle_diff(rings, SKYRER_ANIMATION_DIFF, i); //差量角度必须为 SKYRER_ANGLE_DIV 的约数
                }
                int32_t angle = compo_rings_loop_icon_find_angle(rings, i);
                uint16_t size = func_menu_sub_skyrer_icon_size_update(rings, angle);
                compo_rings_set_size(rings, size, i);
            }
            compo_rings_update_loop(rings);
        }
    }
}

//设置出生点
static void func_menu_sub_skyrer_icon_spawn_idx(void)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    compo_rings_t *rings = f_menu->rings;
    u8 flag_swawn = 0;
    for (int i = 0; i < SKYRER_LOOP_ICON_NUM; i++)
    {
        int32_t angle = compo_rings_loop_icon_find_angle(rings, i);
        if (f_menu->rotate_dir == 1)                //顺时针
        {
            if (angle >= SKYRER_ANGLE_INITIAL && angle < (SKYRER_ANGLE_INITIAL + SKYRER_ANGLE_DIV))
            {
                f_menu->spawn_id = i;               //标记
                flag_swawn |= BIT(1);
            }
            if (angle >= (SKYRER_ANGLE_INITIAL + SKYRER_ANGLE_DIV) && angle < (SKYRER_ANGLE_INITIAL + SKYRER_ANGLE_DIV * 2))
            {
                u8 idx0 = compo_rings_loop_icon_find_idx(rings, i);
                cycle_plus(&idx0, 0, (menuCnt - 1));
                f_menu->spawn_idx = idx0;           //下一个
                flag_swawn |= BIT(2);
            }
        }
        else if (f_menu->rotate_dir == -1)          //逆时针
        {
            if (angle > SKYRER_ANGLE_INITIAL && angle <= (SKYRER_ANGLE_INITIAL + SKYRER_ANGLE_DIV))
            {
                f_menu->spawn_id = i;               //标记
                flag_swawn |= BIT(1);
            }
            if (angle <= SKYRER_ANGLE_INITIAL && angle > (SKYRER_ANGLE_INITIAL - SKYRER_ANGLE_DIV))
            {
                u8 idx0 = compo_rings_loop_icon_find_idx(rings, i);
                cycle_sub(&idx0, 0, (menuCnt - 1));
                f_menu->spawn_idx = idx0;           //上一个
                flag_swawn |= BIT(2);
            }
        }

        if (flag_swawn == (BIT(1) | BIT(2))) break;
    }
}

//更新出生点
static void func_menu_sub_skyrer_swawn_update(void)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    compo_rings_t *rings = f_menu->rings;

    int32_t angle = compo_rings_loop_icon_find_angle(rings, f_menu->spawn_id);
    if (f_menu->rotate_dir == 1)
    {
        if (angle >= (SKYRER_ANGLE_INITIAL + SKYRER_ANGLE_DIV))
        {
            u8 idx0 = compo_rings_loop_icon_find_idx(rings, f_menu->spawn_id);
            compo_rings_set_res_from_idx(rings, tbl_menu_skyrer[f_menu->spawn_idx].res_addr, idx0);
            compo_rings_set_uidx_from_idx(rings, f_menu->spawn_idx, idx0);
        }
    }
    else if (f_menu->rotate_dir == -1)
    {
        if (angle <= SKYRER_ANGLE_INITIAL)
        {
            u8 idx0 = compo_rings_loop_icon_find_idx(rings, f_menu->spawn_id);
            compo_rings_set_res_from_idx(rings, tbl_menu_skyrer[f_menu->spawn_idx].res_addr, idx0);
            compo_rings_set_uidx_from_idx(rings, f_menu->spawn_idx, idx0);
        }
    }
}

//方形图标刷新
static void func_menu_sub_skyrer_icon_party_update(compo_rings_t *rings)
{

    for (int i = 0; i < SKYRER_LOOP_ICON_NUM; i++)
    {
        int32_t angle = compo_rings_loop_icon_find_angle(rings, i);
        u8      idx0  = compo_rings_loop_icon_find_idx(rings, i);

        for (int k = 0; k < 1; k++)
        {
            if (abs(angle - tbl_party_angle[k]) <= 50)
            {
                s16 x = SKYRER_CENX * 2 * tal_party[0].sgnx / 10 + 8;
                s16 y = SKYRER_CENY * 2 * tal_party[0].sgny / 10;
                if (func_cb.sta == FUNC_MENU && func_cb.menu_style == MENU_STYLE_CUM_RING2)
                {
                    compo_textbox_t* text = compo_getobj_byid(COMPO_ID_APP_TXT);
                    compo_textbox_set_align_center(text, false);
                    compo_textbox_set_location(text, x, y - widget_text_get_height()/2, 150, 48);
                    compo_textbox_set(text, i18n[tbl_menu_skyrer[idx0].str_id]);
                }
            }
        }

    }
}

//环形图标size更新
static uint16_t func_menu_sub_skyrer_icon_size_update(compo_rings_t *rings, int32_t angle)
{
//    printf("%s -> angle:%d\n", __func__, angle);
    uint16_t size = SKYRER_ICON_SIZE_MIN;

    if (angle <= 2400 && angle >= 3000)
    {
        return size;
    }

    if (angle > 2400 && angle < 3000)
    {
        size = SKYRER_ICON_SIZE_MAX - muls(SKYRER_ICON_SIZE_MAX - SKYRER_ICON_SIZE_MIN, abs(2700 - angle)) / SKYRER_ANGLE_DIV;
        return size;
    }

    return size;

}

//天圆地方菜单刷新
static void func_menu_sub_skyrer_update(compo_rings_t *rings, int32_t diff_angle, bool swp)
{
    if (swp)                                                    //设置出生点
    {
        func_menu_sub_skyrer_icon_spawn_idx();
    }

    for (int i = 0; i < SKYRER_LOOP_ICON_NUM; i++)              //更新差量角度 环形图标size
    {
        compo_rings_loop_icon_set_angle_diff(rings, diff_angle, i);
        int32_t angle = compo_rings_loop_icon_find_angle(rings, i);
        uint16_t size = func_menu_sub_skyrer_icon_size_update(rings, angle);
        compo_rings_set_size(rings, size, i);
    }

    if (swp)
    {
        func_menu_sub_skyrer_swawn_update();                    //刷新出生点
    }

    compo_rings_page_time1_vis(rings, false);
    func_menu_sub_skyrer_icon_party_update(rings);              //设置方形图标
//    for (int i = 0; i < SKYRER_OTHER_ICON_NUM; i++) {           //刷新方形图标
//        int idx1 = compo_rings_loop_icon_find_idx(rings, SKYRER_LOOP_ICON_NUM + i);
//        s16 x = SKYRER_CENX + tal_party[i].sgnx * ((SKYRER_ICON_SIZE_MAX >> 1) + 5);
//        s16 y = SKYRER_CENY + tal_party[i].sgny * ((SKYRER_ICON_SIZE_MAX >> 1) + 5);
//        compo_rings_icon_t *icon = compo_rings_set_location(rings, x, y, SKYRER_ICON_SIZE_MAX, SKYRER_ICON_SIZE_MAX, idx1);
//        compo_rings_page_time1_bg(rings, icon->widget, idx1 % MENU_SKYRER_CNT);
//        compo_rings_page_time1_update(rings, x, y, SKYRER_ICON_SIZE_MAX, idx1 % MENU_SKYRER_CNT);
//    }
    compo_rings_update_loop(rings);                             //刷新环形图标
}

//主菜单功能事件处理
static void func_menu_sub_skyrer_process(void)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    compo_rings_t *rings = f_menu->rings;

    func_menu_sub_skyrer_animation_update();

    if (!func_cb.flag_animation)
    {
#if 1//UTE_MENU_STYLE_CUM_RING2_TRIANGLE_PIC
        compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_SHAPE_POINT);
        compo_picturebox_set_visible(picbox, true);
#else
        compo_shape_t* point = compo_getobj_byid(COMPO_ID_SHAPE_POINT);
        compo_shape_set_visible(point, true);
#endif
        if (f_menu->flag_drag)
        {
            f_menu->flag_drag = ctp_get_cur_point(&f_menu->sx, &f_menu->sy, &f_menu->x, &f_menu->y);
            if (f_menu->flag_drag)
            {
                f_menu->diff_angle = compo_rings_touch_angle_diff(rings, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, f_menu->x, f_menu->y);
                if (abs(f_menu->diff_angle) != 0)
                {
                    f_menu->diff_angle = (abs(f_menu->diff_angle) >= SKYRER_ANGLE_DIV) ? ((SKYRER_ANGLE_DIV - 1) * (f_menu->diff_angle / abs(f_menu->diff_angle))) : f_menu->diff_angle;
                }

                if (f_menu->diff_angle != 0)
                {
                    uint32_t ticks      = (uint32_t)(tick_get() - f_menu->ticks);
                    f_menu->accelera    = ticks > 0 ? (int32_t)((int32_t)f_menu->diff_angle / (int32_t)ticks) : f_menu->accelera;           //惯性加速度
                    f_menu->rotate_dir  = (abs(f_menu->diff_angle) > 0) ? (f_menu->diff_angle / abs(f_menu->diff_angle)) : 0;               //旋转方向
                }
                func_menu_sub_skyrer_update(rings, f_menu->diff_angle, true);
                f_menu->ticks = tick_get();
            }
            else
            {
                f_menu->flag_move_auto    = true;
                f_menu->ticks             = tick_get();
            }
        }
        else if (f_menu->flag_move_auto)
        {
            if (tick_check_expire(f_menu->ticks, SKYRER_MOVE_AUTO_KICK))    //自动刷新
            {
                f_menu->ticks = tick_get();
                if (f_menu->accelera != 0)
                {
                    f_menu->diff_angle = f_menu->accelera >> 0;             //差量角度
                    f_menu->accelera += -f_menu->rotate_dir * 20;           //惯性加速度归0
                    if ((f_menu->rotate_dir >= 0 && f_menu->accelera <= 0) || (f_menu->rotate_dir <= 0 && f_menu->accelera >= 0))
                    {
                        f_menu->accelera = 0;
                    }
                    func_menu_sub_skyrer_update(rings, f_menu->diff_angle, true);
                }
                else if (f_menu->accelera == 0)
                {
                    int32_t margin_angle = compo_rings_loop_icon_find_angle(rings, 0) % SKYRER_ANGLE_DIV;

                    if (margin_angle != 0)
                    {
                        //如果没有辨别到方向时，做最近点的方向补偿
                        f_menu->rotate_dir  = (f_menu->rotate_dir == 0) ? (((abs(margin_angle) - (SKYRER_ANGLE_DIV >> 1)) > 0) ? 1 : -1) : f_menu->rotate_dir;
                        f_menu->align_angle = (f_menu->rotate_dir > 0) ? (SKYRER_ANGLE_DIV - margin_angle) : (-margin_angle);
                        f_menu->diff_angle  = f_menu->rotate_dir * 20;

                        if (abs(f_menu->diff_angle) > abs(f_menu->align_angle))
                        {
                            f_menu->diff_angle = f_menu->align_angle;
                        }
                        func_menu_sub_skyrer_update(rings, f_menu->diff_angle, true);
                    }
                    else
                    {
                        f_menu->rotate_dir          = 0;
                        f_menu->align_angle         = 0;
                        f_menu->diff_angle          = 0;
                        f_menu->flag_move_auto      = false;
                    }
                }
            }
        }
    }

    func_process();
}

//旋钮滚动处理
static void func_menu_sub_knob_scroll(size_msg_t msg)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;

    if (func_cb.flag_animation) return;
    if (f_menu->flag_drag) return;

    f_menu->flag_move_auto  = true;
    f_menu->rotate_dir = (msg == MSG_QDEC_FORWARD) ? 1 : -1;
    int32_t diff_angle = (msg == MSG_QDEC_FORWARD) ? ((SKYRER_ANGLE_DIV >> 2)) : -((SKYRER_ANGLE_DIV >> 2));//带旋转路径
    func_menu_sub_skyrer_update(f_menu->rings, diff_angle, true);
}

//拖动处理
static void func_menu_sub_drag_move(void)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    compo_rings_t *rings = f_menu->rings;

    if (func_cb.flag_animation) return;

    ctp_get_cur_point(&f_menu->sx, &f_menu->sy, &f_menu->x, &f_menu->y);
    compo_rings_touch_angle_pre(rings, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, f_menu->sx, f_menu->sy);
    f_menu->ticks = tick_get();
    f_menu->flag_drag = true;                           //任何方向短划，开启拖动
}

//拖动过程中，只响应部分消息
static void func_menu_sub_skyrer_drag_message(size_msg_t msg)
{
    switch (msg)
    {

        default:
            evt_message(msg);
            break;
    }
}

//自动移动中，只响应部分消息
static void func_menu_sub_skyrer_move_auto_message(size_msg_t msg)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            f_menu->flag_drag = true;                       //移动过程中，触屏停止
            f_menu->flag_move_auto = false;
            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            func_menu_sub_knob_scroll(msg);
            break;
        case MSG_QDEC_BACKWARD:                             //向后滚动菜单
            func_menu_sub_knob_scroll(msg);
            break;

        default:
            evt_message(msg);
            break;
    }
}

//正常模式下
static void func_menu_sub_skyrer_normal_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_menu_sub_skyrer_icon_click();                //单击图标
            break;

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_SHORT_DOWN:
            func_menu_sub_drag_move();
            break;

        case MSG_QDEC_FORWARD:                              //向前滚动菜单
            func_menu_sub_knob_scroll(msg);
            break;
        case MSG_QDEC_BACKWARD:                             //向后滚动菜单
            func_menu_sub_knob_scroll(msg);
            break;

        case KU_DELAY_BACK:
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_menu_sub_skyrer_switch_to_clock();     //返回时钟表盘界面
            }
            break;

        default:
            func_menu_sub_message(msg);
            break;
    }
}

//主菜单功能消息处理
static void func_menu_sub_skyrer_message(size_msg_t msg)
{
    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    if (f_menu->flag_drag)
    {
        //拖动过程中，只响应部分消息
        func_menu_sub_skyrer_drag_message(msg);
    }
    else if (f_menu->flag_move_auto)
    {
        //自动移动中，只响应部分消息
        func_menu_sub_skyrer_move_auto_message(msg);
    }
    else
    {
        //正常模式下
        func_menu_sub_skyrer_normal_message(msg);
    }
}

//进入主菜单功能
static void func_menu_sub_skyrer_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_menu_skyrer_t));
    func_cb.frm_main = func_menu_sub_cum_ring2_form_create();

    f_menu_skyrer_t *f_menu = (f_menu_skyrer_t *)func_cb.f_cb;
    f_menu->rings = compo_getobj_byid(COMPO_ID_SKYRER);
    compo_rings_t *rings = f_menu->rings;

    if (rings->type != COMPO_TYPE_RINGS)
    {
        halt(HALT_GUI_COMPO_ICONLIST_TYPE);
    }

    if (func_cb.flag_animation)
    {
        f_menu->ticks = tick_get();
    }

    func_cb.enter_tick = tick_get();
}

//主菜单功能
void func_menu_sub_cum_ring2(void)
{
    printf("%s\n", __func__);
    func_menu_sub_skyrer_enter();
    while (func_cb.sta == FUNC_MENU && func_cb.menu_style == MENU_STYLE_CUM_RING2)
    {
        func_menu_sub_skyrer_process();
        func_menu_sub_skyrer_message(msg_dequeue());
    }

    func_menu_sub_exit();
}

///> feat: menu ring2 code update, shadowX, 20250704, end
#endif
