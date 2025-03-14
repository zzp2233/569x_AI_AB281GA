#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_drv_battery_common.h"
#include "func_cover.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SPORT_SUPPORT

#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list) / sizeof(tbl_sport_list[0])))

enum
{
    COMPO_ID_LISTBOX = 1,
};

typedef struct f_sport_list_t_
{
    compo_listbox_t *listbox;

} f_sport_list_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {STR_SPORT_RUN,           UI_BUF_I330001_SPORT_ICON_00_PAOBU_BIN,             .vidx = SPORT_TYPE_RUNNING       },
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {STR_SPORT_RIDE_BIKE,     UI_BUF_I330001_SPORT_ICON_01_QIXING_BIN,            .vidx = SPORT_TYPE_RIDE_BIKE     },
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {STR_SPORT_JUMP_ROPE,     UI_BUF_I330001_SPORT_ICON_03_TIAOSHEN_BIN,          .vidx = SPORT_TYPE_JUMP_ROPE     },
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {STR_SPORT_SWIMMING,      UI_BUF_I330001_SPORT_ICON_04_YOUYONG_BIN,           .vidx = SPORT_TYPE_SWIMMING      },
#endif
    {STR_SPORT_BADMINTON,     UI_BUF_I330001_SPORT_ICON_05_YUMAOQIU_BIN,          .vidx = SPORT_TYPE_BADMINTON     },
    {STR_SPORT_TABLE_TENNIS,  UI_BUF_I330001_SPORT_ICON_06_PINGPANGQIU_BIN,       .vidx = SPORT_TYPE_TABLE_TENNIS  },
    {STR_SPORT_TENNIS,        UI_BUF_I330001_SPORT_ICON_07_WANGQIU_BIN,           .vidx = SPORT_TYPE_TENNIS        },
    {STR_SPORT_CLIMBING,      UI_BUF_I330001_SPORT_ICON_08_DENGSHAN_BIN,          .vidx = SPORT_TYPE_CLIMBING      },
    {STR_SPORT_WALKING,       UI_BUF_I330001_SPORT_ICON_02_JIANZOU_BIN,           .vidx = SPORT_TYPE_WALKING       },
    {STR_SPORT_BASKETBALL,    UI_BUF_I330001_SPORT_ICON_09_LANQIU_BIN,            .vidx = SPORT_TYPE_BASKETBALL    },
    {STR_SPORT_FOOTBALL,      UI_BUF_I330001_SPORT_ICON_10_ZUQIU_BIN,             .vidx = SPORT_TYPE_FOOTBALL      },
    {STR_SPORT_BASEBALL,      UI_BUF_I330001_SPORT_ICON_11_BANGQIU_BIN,           .vidx = SPORT_TYPE_BASEBALL      },
    {STR_SPORT_VOLLEYBALL,    UI_BUF_I330001_SPORT_ICON_12_PAIQIU_BIN,            .vidx = SPORT_TYPE_VOLLEYBALL    },
    {STR_SPORT_CRICKET,       UI_BUF_I330001_SPORT_ICON_13_BANQIU_BIN,            .vidx = SPORT_TYPE_CRICKET       },
    {STR_SPORT_RUGBY,         UI_BUF_I330001_SPORT_ICON_14_GANLANQIU_BIN,         .vidx = SPORT_TYPE_RUGBY         },
    {STR_SPORT_HOCKEY,        UI_BUF_I330001_SPORT_ICON_15_QUGUNQIU_BIN,          .vidx = SPORT_TYPE_HOCKEY        },
    {STR_SPORT_DANCE,         UI_BUF_I330001_SPORT_ICON_16_TIAOWU_BIN,            .vidx = SPORT_TYPE_DANCE         },
    {STR_SPORT_SPINNING,      UI_BUF_I330001_SPORT_ICON_17_DONGGANDANCHE_BIN,     .vidx = SPORT_TYPE_SPINNING      },
    {STR_SPORT_YOGA,          UI_BUF_I330001_SPORT_ICON_18_YUJIA_BIN,             .vidx = SPORT_TYPE_YOGA          },
    {STR_SPORT_SIT_UP,        UI_BUF_I330001_SPORT_ICON_19_YANGWOQIZUO_BIN,       .vidx = SPORT_TYPE_SIT_UP        },
    {STR_SPORT_TREADMILL,     UI_BUF_I330001_SPORT_ICON_20_SHINEIPAO_BIN,         .vidx = SPORT_TYPE_TREADMILL     },
    {STR_SPORT_GYMNASTICS,    UI_BUF_I330001_SPORT_ICON_21_TICAO_BIN,             .vidx = SPORT_TYPE_GYMNASTICS    },
    {STR_SPORT_BOATING,       UI_BUF_I330001_SPORT_ICON_22_HUACHUAN_BIN,          .vidx = SPORT_TYPE_BOATING       },
    {STR_SPORT_JUMPING_JACK,  UI_BUF_I330001_SPORT_ICON_23_KAIHETIAO_BIN,         .vidx = SPORT_TYPE_JUMPING_JACK  },
    {STR_SPORT_FREE_TRAINING, UI_BUF_I330001_SPORT_ICON_24_ZIYOUXUNLIAN_BIN,      .vidx = SPORT_TYPE_FREE_TRAINING },
};

static compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_MAX_SPORT_NUM];

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);
    compo_form_set_title_txt_color(frm, make_color(169,255,0));

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_CUM_SPORT_LIST);

#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    compo_listbox_set(listbox, tbl_sport_list_sort, uteModuleSportGetHundredSportValidNumber());
#else
    compo_listbox_set(listbox, tbl_sport_list_sort, UTE_MODULE_SPORT_MAX_SPORT_NUM);
#endif

    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_SPORT_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    u8 sport_idx = sys_cb.sport_idx;
    compo_listbox_set_focus_byidx(listbox, sport_idx);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_sport_list_icon_click(void)
{
    int icon_idx;
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= UTE_MODULE_SPORT_MAX_SPORT_NUM)
    {
        return;
    }

    uint8_t batLvl = uteDrvBatteryCommonGetLvl();
    if(batLvl < UTE_DRV_BATTERY_LOW_POWER_PERECNT)
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_COVER_LOW_BATTERY;
        sys_cb.remind_tag = true;
        return;
    }

    uteTaskGuiStartScreen(FUNC_SPORT_SWITCH, 0, __func__);
    uteModuleSportStartMoreSports(tbl_sport_list_sort[icon_idx].vidx, 1, 0);
    sys_cb.sport_idx = tbl_sport_list_sort[icon_idx].vidx;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define UTE_SPORT_TYPE(type)        [type-1]
static const compo_listbox_item_t tbl_sport_list[] =
{
    UTE_SPORT_TYPE(SPORT_TYPE_RUNNING)      = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_00_PAOBU_BIN,             .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_RIDE_BIKE)    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I332001_SPORT_ICON_01_QIXING_BIN,            .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_JUMP_ROPE)    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I332001_SPORT_ICON_03_TIAOSHEN_BIN,          .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SWIMMING)     = {STR_SPORT_SWIMMING,      UI_BUF_I332001_SPORT_ICON_04_YOUYONG_BIN,           .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BADMINTON)    = {STR_SPORT_BADMINTON,     UI_BUF_I332001_SPORT_ICON_05_YUMAOQIU_BIN,          .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TABLE_TENNIS) = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I332001_SPORT_ICON_06_PINGPANGQIU_BIN,       .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TENNIS)       = {STR_SPORT_TENNIS,        UI_BUF_I332001_SPORT_ICON_07_WANGQIU_BIN,           .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CLIMBING)     = {STR_SPORT_CLIMBING,      UI_BUF_I332001_SPORT_ICON_08_DENGSHAN_BIN,          .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_WALKING)      = {STR_SPORT_WALKING,       UI_BUF_I332001_SPORT_ICON_02_JIANZOU_BIN,           .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BASKETBALL)   = {STR_SPORT_BASKETBALL,    UI_BUF_I332001_SPORT_ICON_09_LANQIU_BIN,            .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FOOTBALL)     = {STR_SPORT_FOOTBALL,      UI_BUF_I332001_SPORT_ICON_10_ZUQIU_BIN,             .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BASEBALL)     = {STR_SPORT_BASEBALL,      UI_BUF_I332001_SPORT_ICON_11_BANGQIU_BIN,           .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_VOLLEYBALL)   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I332001_SPORT_ICON_12_PAIQIU_BIN,            .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CRICKET)      = {STR_SPORT_CRICKET,       UI_BUF_I332001_SPORT_ICON_13_BANQIU_BIN,            .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_RUGBY)        = {STR_SPORT_RUGBY,         UI_BUF_I332001_SPORT_ICON_14_GANLANQIU_BIN,         .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HOCKEY)       = {STR_SPORT_HOCKEY,        UI_BUF_I332001_SPORT_ICON_15_QUGUNQIU_BIN,          .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DANCE)        = {STR_SPORT_DANCE,         UI_BUF_I332001_SPORT_ICON_16_TIAOWU_BIN,            .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SPINNING)     = {STR_SPORT_SPINNING,      UI_BUF_I332001_SPORT_ICON_17_DONGGANDANCHE_BIN,     .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_YOGA)         = {STR_SPORT_YOGA,          UI_BUF_I332001_SPORT_ICON_18_YUJIA_BIN,             .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SIT_UP)       = {STR_SPORT_SIT_UP,        UI_BUF_I332001_SPORT_ICON_19_YANGWOQIZUO_BIN,       .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TREADMILL)    = {STR_SPORT_TREADMILL,     UI_BUF_I332001_SPORT_ICON_20_SHINEIPAO_BIN,         .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_GYMNASTICS)   = {STR_SPORT_GYMNASTICS,    UI_BUF_I332001_SPORT_ICON_21_TICAO_BIN,             .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BOATING)      = {STR_SPORT_BOATING,       UI_BUF_I332001_SPORT_ICON_22_HUACHUAN_BIN,          .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_JUMPING_JACK) = {STR_SPORT_JUMPING_JACK,  UI_BUF_I332001_SPORT_ICON_23_KAIHETIAO_BIN,         .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FREE_TRAINING)= {STR_SPORT_FREE_TRAINING, UI_BUF_I332001_SPORT_ICON_24_ZIYOUXUNLIAN_BIN,      .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MAX)          = {STR_MORE,                UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,            .func_sta = FUNC_SPORT_SWITCH},
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    UTE_SPORT_TYPE(SPORT_TYPE_INDOOR_WALK)          = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_INDOOR_RUN)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_STRENGTH_TRAINING)    = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_STEP_TRAINING)        = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HORSE_RIDING)         = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ELLIPTICAL_TRAINER)   = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TAI_CHI)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SHUTTLECOCK)          = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BOXING)               = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_OUTDOOR_WALK)         = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TRAIL_RUNNING)        = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SKIING)               = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ICE_HOCKEY)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TAEKWONDO)            = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_VO2MAX_TEST)          = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ROWING_MACHING)       = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_AIR_WALKER)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HIKING)               = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ATHLETICS)            = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_WAIST_TRAINING)       = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_KARATE)               = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_COOLDOWN)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CROSS_TRAINING)       = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PILATES)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CROSS_FIT)            = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FUNCTIONAL_TRAINING)  = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PHYSICAL_TRAINING)    = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ARCHERY)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FLEXIBILITY)          = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MIXED_CARDIO)         = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_LATIN_DANCE)          = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_STREET_DANCE)         = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_KICKBOXING)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BARRE)                = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_AUSTRALIAN_FOOTBALL)  = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MARTIAL_ARTS)         = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_STAIRS)               = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HANDBALL)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BOWLING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_RACQUETBALL)          = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CURLING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HUNTING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SNOWBOARDING)         = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PLAY)                 = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_AMERICAN_FOOTBALL)    = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HAND_CYCLING)         = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FISHING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DISC_SPORTS)          = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_RUGGER)               = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_GOLF)                 = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FOLK_DANCE)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DOWNHILL_SKIING)      = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SNOW_SPORTS)          = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MIND_BODY)            = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CORE_TRAINING)        = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SKATING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FITNESS_GAMING)       = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_AEROBICS)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_GROUP_TRAINING)       = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_KENDO)                = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_LACROSSE)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ROLLING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_WRESTLING)            = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_FENCING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SOFTBALL)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SINGLE_BAR)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PARALLEL_BARS)        = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ROLLER_SKATING)       = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HULA_HOOP)            = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DARTS)                = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PICKLEBALL)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HIIT)                 = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SHOOTING)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_JUDO)                 = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_TRAMPOLINE)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SKATEBOARDING)        = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HOVERBOARD)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BLADING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PARKOUR)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_DIVING)               = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SURFING)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_SNORKELING)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PULL_UP)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PUSH_UP)              = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_PLANKING)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_ROCK_CLIMBING)        = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_HIGHTJUMP)            = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_BUNGEE_JUMPING)       = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_LONGJUMP)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_MARATHON)             = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_OTHER)                = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CYWEE_SWIM_GNSS)      = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
    UTE_SPORT_TYPE(SPORT_TYPE_CYWEE_SWIM)           = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,              .func_sta = FUNC_SPORT_SWITCH},
#endif // UTE_MODULE_SPORT_HUNDRED_SUPPORT
};

#define SPORT_LIST_CNT                       ((int)(sizeof(tbl_sport_list_sort) / sizeof(tbl_sport_list_sort[0])))
static const compo_listbox_item_t tbl_sport_list_sort[] =
{
    {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_00_PAOBU_BIN,             .vidx = SPORT_TYPE_RUNNING       },
    {STR_SPORT_RIDE_BIKE,     UI_BUF_I332001_SPORT_ICON_01_QIXING_BIN,            .vidx = SPORT_TYPE_RIDE_BIKE     },
    {STR_SPORT_JUMP_ROPE,     UI_BUF_I332001_SPORT_ICON_03_TIAOSHEN_BIN,          .vidx = SPORT_TYPE_JUMP_ROPE     },
    {STR_SPORT_BADMINTON,     UI_BUF_I332001_SPORT_ICON_05_YUMAOQIU_BIN,          .vidx = SPORT_TYPE_BADMINTON     },
    {STR_SPORT_TABLE_TENNIS,  UI_BUF_I332001_SPORT_ICON_06_PINGPANGQIU_BIN,       .vidx = SPORT_TYPE_TABLE_TENNIS  },
    {STR_SPORT_TENNIS,        UI_BUF_I332001_SPORT_ICON_07_WANGQIU_BIN,           .vidx = SPORT_TYPE_TENNIS        },
    {STR_SPORT_CLIMBING,      UI_BUF_I332001_SPORT_ICON_08_DENGSHAN_BIN,          .vidx = SPORT_TYPE_CLIMBING      },
    {STR_SPORT_WALKING,       UI_BUF_I332001_SPORT_ICON_02_JIANZOU_BIN,           .vidx = SPORT_TYPE_WALKING       },
    {STR_SPORT_BASKETBALL,    UI_BUF_I332001_SPORT_ICON_09_LANQIU_BIN,            .vidx = SPORT_TYPE_BASKETBALL    },
    {STR_SPORT_FOOTBALL,      UI_BUF_I332001_SPORT_ICON_10_ZUQIU_BIN,             .vidx = SPORT_TYPE_FOOTBALL      },
    {STR_SPORT_BASEBALL,      UI_BUF_I332001_SPORT_ICON_11_BANGQIU_BIN,           .vidx = SPORT_TYPE_BASEBALL      },
    {STR_SPORT_VOLLEYBALL,    UI_BUF_I332001_SPORT_ICON_12_PAIQIU_BIN,            .vidx = SPORT_TYPE_VOLLEYBALL    },
    {STR_SPORT_CRICKET,       UI_BUF_I332001_SPORT_ICON_13_BANQIU_BIN,            .vidx = SPORT_TYPE_CRICKET       },
    {STR_SPORT_RUGBY,         UI_BUF_I332001_SPORT_ICON_14_GANLANQIU_BIN,         .vidx = SPORT_TYPE_RUGBY         },
    {STR_SPORT_HOCKEY,        UI_BUF_I332001_SPORT_ICON_15_QUGUNQIU_BIN,          .vidx = SPORT_TYPE_HOCKEY        },
    {STR_SPORT_DANCE,         UI_BUF_I332001_SPORT_ICON_16_TIAOWU_BIN,            .vidx = SPORT_TYPE_DANCE         },
    {STR_SPORT_SPINNING,      UI_BUF_I332001_SPORT_ICON_17_DONGGANDANCHE_BIN,     .vidx = SPORT_TYPE_SPINNING      },
    {STR_SPORT_YOGA,          UI_BUF_I332001_SPORT_ICON_18_YUJIA_BIN,             .vidx = SPORT_TYPE_YOGA          },
    {STR_SPORT_SIT_UP,        UI_BUF_I332001_SPORT_ICON_19_YANGWOQIZUO_BIN,       .vidx = SPORT_TYPE_SIT_UP        },
    {STR_SPORT_TREADMILL,     UI_BUF_I332001_SPORT_ICON_20_SHINEIPAO_BIN,         .vidx = SPORT_TYPE_TREADMILL     },
    {STR_SPORT_GYMNASTICS,    UI_BUF_I332001_SPORT_ICON_21_TICAO_BIN,             .vidx = SPORT_TYPE_GYMNASTICS    },
    {STR_SPORT_BOATING,       UI_BUF_I332001_SPORT_ICON_22_HUACHUAN_BIN,          .vidx = SPORT_TYPE_BOATING       },
    {STR_SPORT_JUMPING_JACK,  UI_BUF_I332001_SPORT_ICON_23_KAIHETIAO_BIN,         .vidx = SPORT_TYPE_JUMPING_JACK  },
    {STR_SPORT_FREE_TRAINING, UI_BUF_I332001_SPORT_ICON_24_ZIYOUXUNLIAN_BIN,      .vidx = SPORT_TYPE_FREE_TRAINING },
};
u32 func_sport_get_current_idx(void)
{
    return sys_cb.sport_idx;
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list[sport_idx].str_idx;
    }
    if (sport_idx == SPORT_TYPE_MAX - 1)
    {
        return STR_MORE;
    }
    return STR_NULL;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list[sport_idx].res_addr;
    }
    if (sport_idx == SPORT_TYPE_MAX - 1)
    {
//        return UI_BUF_I330001_SPORT_ICON_25_CAIDAN_BIN;
    }
    return 0;
}

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);
    compo_form_set_title_txt_color(frm, make_color(169,255,0));

    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_CUM_SPORT_LIST);
    compo_listbox_set(listbox, tbl_sport_list_sort, SPORT_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_SPORT_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    u8 sport_idx = sys_cb.sport_idx;
    compo_listbox_set_focus_byidx(listbox, sport_idx);
    compo_listbox_update(listbox);

    return frm;
}

//点进图标进入应用
static void func_sport_list_icon_click(void)
{
    int icon_idx;
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;
    u8 func_sta;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= MENU_LIST_CNT)
    {
        return;
    }
    uint8_t batLvl = uteDrvBatteryCommonGetLvl();
    if(batLvl < UTE_DRV_BATTERY_LOW_POWER_PERECNT)
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        sys_cb.cover_index = REMIND_COVER_LOW_BATTERY;
        sys_cb.remind_tag = true;
        return;
    }
    //根据图标索引获取应用ID
    func_sta = FUNC_SPORT_SWITCH;

    //切入应用
    if (func_sta > 0)
    {
        func_cb.sta = func_sta;
        sys_cb.sport_idx = tbl_sport_list_sort[icon_idx].vidx-1;                //记住当前编号
    }

}

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
static bool  func_sport_list_init_flag = false; // 列表是否初始化完成，true表示列表已经初始化完成，false表示列表未初始化完成

//重新排序列表
void func_sport_list_reorder(compo_listbox_item_t *sportList)
{
    if (sportList == NULL)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,ERROR, sportList is NULL!", __func__);
        return;
    }
    memset(sportList, 0, sizeof(compo_listbox_item_t) * UTE_MODULE_SPORT_MAX_SPORT_NUM);

    // 根据sportOrder重新排序
    for (uint8_t i = 0; i < UTE_MODULE_SPORT_MAX_SPORT_NUM; i++)
    {
        uint8_t sportOrder = uteModuleSportGetSportOrder(tbl_sport_list[i].vidx);
        if (sportOrder > 0 && sportOrder <= UTE_MODULE_SPORT_MAX_SPORT_NUM)
        {
            sportList[sportOrder - 1] = tbl_sport_list[i];
        }
        else
        {
            UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,WARNING, sportOrder %d is invalid for sportMode %d!", __func__, sportOrder, tbl_sport_list[i].vidx);
        }
    }
    func_sport_list_init_flag = true;
    uteModuleSportSetAppUpdateTrainingListFlag(false);

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,success, array sorted by sportOrder!", __func__);
}

//根据运动模式获取对应的文本和图片
bool func_sport_get_sport_mode_info(uint8_t sport_mode, uint16_t *str_idx, uint32_t *icon_addr)
{
    if(str_idx == NULL || icon_addr == NULL || sport_mode > UTE_MODULE_SPORT_MAX_SPORT_NUM)
    {
        return false;
    }
    if(uteModuleSportGetAppUpdateTrainingListFlag() || !func_sport_list_init_flag)
    {
        func_sport_list_reorder(tbl_sport_list_sort);
    }
    uint8_t sport_order = uteModuleSportGetSportOrder(sport_mode);
    if(sport_order > 0)
    {
        *str_idx = tbl_sport_list_sort[sport_order - 1].str_idx;
        *icon_addr = tbl_sport_list_sort[sport_order - 1].res_addr;
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"sport_order = %d, sport_mode = %d, str_idx = %d, icon_addr = %d", sport_order, sport_mode, *str_idx, *icon_addr);
        return true;
    }
    return false;
}

//根据序号获取文本和图片
bool func_sport_get_by_order(uint8_t sport_order, uint16_t *str_idx, uint32_t *icon_addr)
{
    if (str_idx == NULL || icon_addr == NULL ||
        sport_order < 1 || sport_order > UTE_MODULE_SPORT_MAX_SPORT_NUM)
    {
        return false;
    }

    // 确保列表已排序
    if (uteModuleSportGetAppUpdateTrainingListFlag() || !func_sport_list_init_flag)
    {
        func_sport_list_reorder(tbl_sport_list_sort);
    }

    // 直接通过 sport_order 索引访问已排序的列表
    compo_listbox_item_t *item = &tbl_sport_list_sort[sport_order - 1];
    if (item == NULL)
    {
        UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,ERROR, invalid item for sport_order %d!", __func__, sport_order);
        return false;
    }

    *str_idx = item->str_idx;
    *icon_addr = item->res_addr;

    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,success, sport_order %d: str_idx=%d, icon_addr=0x%x", __func__, sport_order, *str_idx, *icon_addr);
    return true;
}

#endif

//运动功能事件处理
static void func_sport_process(void)
{
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    if(uteModuleSportGetAppUpdateTrainingListFlag() || !func_sport_list_init_flag)
    {
        func_sport_list_reorder(tbl_sport_list_sort);
        msg_enqueue(MSG_CHECK_LANGUAGE);
    }
#endif
    compo_listbox_move(f_sport->listbox);
    func_process();
}

//运动功能消息处理
static void func_sport_message(size_msg_t msg)
{
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;

    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sport_list_icon_click();                //单击图标
            break;

        case MSG_CTP_LONG:
            break;
        case MSG_CTP_SHORT_LEFT:
            break;
        case MSG_CTP_SHORT_RIGHT:
            if (func_cb.flag_sort)
            {
                func_backing_to();
            }
            else
            {
                func_message(msg);
            }
            sys_cb.sport_idx = 0;
            break;

        case KU_DELAY_BACK:
//        if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY)) {
//            func_menu_sub_list_switch_to_clock();       //返回时钟表盘界面
//        }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入运动功能
static void func_sport_enter(void)
{
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    func_sport_list_reorder(tbl_sport_list_sort);
#endif
    func_cb.f_cb = func_zalloc(sizeof(f_sport_list_t));
    func_cb.frm_main = func_sport_form_create();

    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    f_sport->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_sport->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init(listbox);
    // compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, MENU_LIST_CNT - 1));
    func_cb.enter_tick = tick_get();
}

//退出运动功能
static void func_sport_exit(void)
{
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;
    func_free(listbox->mcb);
    func_cb.last = FUNC_SPORT;
}

//运动功能
void func_sport(void)
{
    printf("%s\n", __func__);
    func_sport_enter();
    while (func_cb.sta == FUNC_SPORT)
    {
        func_sport_process();
        func_sport_message(msg_dequeue());
    }
    func_sport_exit();
}

#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
