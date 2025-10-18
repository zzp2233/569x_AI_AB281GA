#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_drv_battery_common.h"
#include "func_cover.h"
#include "ute_drv_motor.h"
#include "ute_module_sportTarget.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SPORT_SUPPORT

enum
{
    COMPO_ID_LISTBOX = 1,
    COMPO_ID_SHAPE_MORE,
    COMPO_ID_PIC_MORE,
    COMPO_ID_TXT_MORE,
};

typedef struct f_sport_list_t_
{
    compo_listbox_t *listbox;
    u8 page;
    u8 count;

} f_sport_list_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
    [SPORT_TYPE_RUNNING]      = {STR_SPORT_RUN,           UI_BUF_I330001_SPORT_ICON_00_PAOBU_BIN,          },
    [SPORT_TYPE_RIDE_BIKE]    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I330001_SPORT_ICON_01_QIXING_BIN,         },
    [SPORT_TYPE_JUMP_ROPE]    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I330001_SPORT_ICON_03_TIAOSHEN_BIN,       },
    [SPORT_TYPE_SWIMMING]     = {STR_SPORT_SWIMMING,      UI_BUF_I330001_SPORT_ICON_04_YOUYONG_BIN,        },
    [SPORT_TYPE_BADMINTON]    = {STR_SPORT_BADMINTON,     UI_BUF_I330001_SPORT_ICON_05_YUMAOQIU_BIN,       },
    [SPORT_TYPE_TABLE_TENNIS] = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I330001_SPORT_ICON_06_PINGPANGQIU_BIN,    },
    [SPORT_TYPE_TENNIS]       = {STR_SPORT_TENNIS,        UI_BUF_I330001_SPORT_ICON_07_WANGQIU_BIN,        },
    [SPORT_TYPE_CLIMBING]     = {STR_SPORT_CLIMBING,      UI_BUF_I330001_SPORT_ICON_08_DENGSHAN_BIN,       },
    [SPORT_TYPE_WALKING]      = {STR_SPORT_WALKING,       UI_BUF_I330001_SPORT_ICON_02_JIANZOU_BIN,        },
    [SPORT_TYPE_BASKETBALL]   = {STR_SPORT_BASKETBALL,    UI_BUF_I330001_SPORT_ICON_09_LANQIU_BIN,         },
    [SPORT_TYPE_FOOTBALL]     = {STR_SPORT_FOOTBALL,      UI_BUF_I330001_SPORT_ICON_10_ZUQIU_BIN,          },
    [SPORT_TYPE_BASEBALL]     = {STR_SPORT_BASEBALL,      UI_BUF_I330001_SPORT_ICON_11_BANGQIU_BIN,        },
    [SPORT_TYPE_VOLLEYBALL]   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I330001_SPORT_ICON_12_PAIQIU_BIN,         },
    [SPORT_TYPE_CRICKET]      = {STR_SPORT_CRICKET,       UI_BUF_I330001_SPORT_ICON_13_BANQIU_BIN,         },
    [SPORT_TYPE_RUGBY]        = {STR_SPORT_RUGBY,         UI_BUF_I330001_SPORT_ICON_14_GANLANQIU_BIN,      },
    [SPORT_TYPE_HOCKEY]       = {STR_SPORT_HOCKEY,        UI_BUF_I330001_SPORT_ICON_15_QUGUNQIU_BIN,       },
    [SPORT_TYPE_DANCE]        = {STR_SPORT_DANCE,         UI_BUF_I330001_SPORT_ICON_16_TIAOWU_BIN,         },
    [SPORT_TYPE_SPINNING]     = {STR_SPORT_SPINNING,      UI_BUF_I330001_SPORT_ICON_17_DONGGANDANCHE_BIN,  },
    [SPORT_TYPE_YOGA]         = {STR_SPORT_YOGA,          UI_BUF_I330001_SPORT_ICON_18_YUJIA_BIN,          },
    [SPORT_TYPE_SIT_UP]       = {STR_SPORT_SIT_UP,        UI_BUF_I330001_SPORT_ICON_19_YANGWOQIZUO_BIN,    },
    [SPORT_TYPE_TREADMILL]    = {STR_SPORT_TREADMILL,     UI_BUF_I330001_SPORT_ICON_20_SHINEIPAO_BIN,      },
    [SPORT_TYPE_GYMNASTICS]   = {STR_SPORT_GYMNASTICS,    UI_BUF_I330001_SPORT_ICON_21_TICAO_BIN,          },
    [SPORT_TYPE_BOATING]      = {STR_SPORT_BOATING,       UI_BUF_I330001_SPORT_ICON_22_HUACHUAN_BIN,       },
    [SPORT_TYPE_JUMPING_JACK] = {STR_SPORT_JUMPING_JACK,  UI_BUF_I330001_SPORT_ICON_23_KAIHETIAO_BIN,      },
    [SPORT_TYPE_FREE_TRAINING]= {STR_SPORT_FREE_TRAINING, UI_BUF_I330001_SPORT_ICON_24_ZIYOUXUNLIAN_BIN,   },
    [SPORT_TYPE_MAX]          = {STR_MORE,                UI_BUF_I330001_SPORT_ICON_25_CAIDAN_BIN,         },
};

enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

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

    compo_listbox_set_focus_byidx(listbox, 0);
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
#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
    [SPORT_TYPE_RUNNING]      = {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_00_PAOBU_BIN,          },
    [SPORT_TYPE_RIDE_BIKE]    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I332001_SPORT_ICON_01_QIXING_BIN,         },
    [SPORT_TYPE_JUMP_ROPE]    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I332001_SPORT_ICON_03_TIAOSHEN_BIN,       },
    [SPORT_TYPE_SWIMMING]     = {STR_SPORT_SWIMMING,      UI_BUF_I332001_SPORT_ICON_04_YOUYONG_BIN,        },
    [SPORT_TYPE_BADMINTON]    = {STR_SPORT_BADMINTON,     UI_BUF_I332001_SPORT_ICON_05_YUMAOQIU_BIN,       },
    [SPORT_TYPE_TABLE_TENNIS] = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I332001_SPORT_ICON_06_PINGPANGQIU_BIN,    },
    [SPORT_TYPE_TENNIS]       = {STR_SPORT_TENNIS,        UI_BUF_I332001_SPORT_ICON_07_WANGQIU_BIN,        },
    [SPORT_TYPE_CLIMBING]     = {STR_SPORT_CLIMBING,      UI_BUF_I332001_SPORT_ICON_08_DENGSHAN_BIN,       },
    [SPORT_TYPE_WALKING]      = {STR_SPORT_WALKING,       UI_BUF_I332001_SPORT_ICON_02_JIANZOU_BIN,        },
    [SPORT_TYPE_BASKETBALL]   = {STR_SPORT_BASKETBALL,    UI_BUF_I332001_SPORT_ICON_09_LANQIU_BIN,         },
    [SPORT_TYPE_FOOTBALL]     = {STR_SPORT_FOOTBALL,      UI_BUF_I332001_SPORT_ICON_10_ZUQIU_BIN,          },
    [SPORT_TYPE_BASEBALL]     = {STR_SPORT_BASEBALL,      UI_BUF_I332001_SPORT_ICON_11_BANGQIU_BIN,        },
    [SPORT_TYPE_VOLLEYBALL]   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I332001_SPORT_ICON_12_PAIQIU_BIN,         },
    [SPORT_TYPE_CRICKET]      = {STR_SPORT_CRICKET,       UI_BUF_I332001_SPORT_ICON_13_BANQIU_BIN,         },
    [SPORT_TYPE_RUGBY]        = {STR_SPORT_RUGBY,         UI_BUF_I332001_SPORT_ICON_14_GANLANQIU_BIN,      },
    [SPORT_TYPE_HOCKEY]       = {STR_SPORT_HOCKEY,        UI_BUF_I332001_SPORT_ICON_15_QUGUNQIU_BIN,       },
    [SPORT_TYPE_DANCE]        = {STR_SPORT_DANCE,         UI_BUF_I332001_SPORT_ICON_16_TIAOWU_BIN,         },
    [SPORT_TYPE_SPINNING]     = {STR_SPORT_SPINNING,      UI_BUF_I332001_SPORT_ICON_17_DONGGANDANCHE_BIN,  },
    [SPORT_TYPE_YOGA]         = {STR_SPORT_YOGA,          UI_BUF_I332001_SPORT_ICON_18_YUJIA_BIN,          },
    [SPORT_TYPE_SIT_UP]       = {STR_SPORT_SIT_UP,        UI_BUF_I332001_SPORT_ICON_19_YANGWOQIZUO_BIN,    },
    [SPORT_TYPE_TREADMILL]    = {STR_SPORT_TREADMILL,     UI_BUF_I332001_SPORT_ICON_20_SHINEIPAO_BIN,      },
    [SPORT_TYPE_GYMNASTICS]   = {STR_SPORT_GYMNASTICS,    UI_BUF_I332001_SPORT_ICON_21_TICAO_BIN,          },
    [SPORT_TYPE_BOATING]      = {STR_SPORT_BOATING,       UI_BUF_I332001_SPORT_ICON_22_HUACHUAN_BIN,       },
    [SPORT_TYPE_JUMPING_JACK] = {STR_SPORT_JUMPING_JACK,  UI_BUF_I332001_SPORT_ICON_23_KAIHETIAO_BIN,      },
    [SPORT_TYPE_FREE_TRAINING]= {STR_SPORT_FREE_TRAINING, UI_BUF_I332001_SPORT_ICON_24_ZIYOUXUNLIAN_BIN,   },
    [SPORT_TYPE_MAX]          = {STR_MORE,                UI_BUF_I332001_SPORT_ICON_25_CAIDAN_BIN,         },
};

enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {STR_SPORT_RUN,           UI_BUF_I332001_SPORT_ICON_00_PAOBU_BIN,             .vidx = SPORT_TYPE_RUNNING       },
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {STR_SPORT_RIDE_BIKE,     UI_BUF_I332001_SPORT_ICON_01_QIXING_BIN,            .vidx = SPORT_TYPE_RIDE_BIKE     },
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {STR_SPORT_JUMP_ROPE,     UI_BUF_I332001_SPORT_ICON_03_TIAOSHEN_BIN,          .vidx = SPORT_TYPE_JUMP_ROPE     },
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {STR_SPORT_SWIMMING,      UI_BUF_I332001_SPORT_ICON_04_YOUYONG_BIN,           .vidx = SPORT_TYPE_SWIMMING      },
#endif
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

    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_SPORT_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 0);
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
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    [SPORT_TYPE_RUNNING]      = {STR_SPORT_RUN,           UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_001_OUTDOOR_RUNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    [SPORT_TYPE_RIDE_BIKE]    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_002_OUTDOOR_CYCLE_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    [SPORT_TYPE_JUMP_ROPE]    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_003_JUMP_ROPE_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    [SPORT_TYPE_SWIMMING]     = {STR_SPORT_JUMP_ROPE,     UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_004_SWIM_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    [SPORT_TYPE_BADMINTON]    = {STR_SPORT_BADMINTON,     UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_005_BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    [SPORT_TYPE_TABLE_TENNIS] = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_006_TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    [SPORT_TYPE_TENNIS]       = {STR_SPORT_TENNIS,        UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_007_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    [SPORT_TYPE_CLIMBING]     = {STR_SPORT_CLIMBING,      UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_008_CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    [SPORT_TYPE_WALKING]      = {STR_SPORT_WALKING,       UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_009_OUTDOORRUN_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    [SPORT_TYPE_BASKETBALL]   = {STR_SPORT_BASKETBALL,    UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_010_BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    [SPORT_TYPE_FOOTBALL]     = {STR_SPORT_FOOTBALL,      UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_011_FOOTBALL_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    [SPORT_TYPE_BASEBALL]     = {STR_SPORT_BASEBALL,      UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_012_BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    [SPORT_TYPE_VOLLEYBALL]   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_013_VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    [SPORT_TYPE_CRICKET]      = {STR_SPORT_CRICKET,       UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_014_CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    [SPORT_TYPE_RUGBY]        = {STR_SPORT_RUGBY,         UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_015_RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    [SPORT_TYPE_HOCKEY]       = {STR_SPORT_HOCKEY,        UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_016_HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    [SPORT_TYPE_DANCE]        = {STR_SPORT_DANCE,         UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_017_DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    [SPORT_TYPE_SPINNING]     = {STR_SPORT_SPINNING,      UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_018_INDOOR_CYCLE_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    [SPORT_TYPE_YOGA]         = {STR_SPORT_YOGA,          UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_019_YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    [SPORT_TYPE_SIT_UP]       = {STR_SPORT_SIT_UP,        UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_020_SITUP_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    [SPORT_TYPE_TREADMILL]    = {STR_SPORT_TREADMILL,     UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_021_TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    [SPORT_TYPE_GYMNASTICS]   = {STR_SPORT_GYMNASTICS,    UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_022_GYMNASTIC_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    [SPORT_TYPE_BOATING]      = {STR_SPORT_BOATING,       UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_023_BOATING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    [SPORT_TYPE_JUMPING_JACK] = {STR_SPORT_JUMPING_JACK,  UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_024_JUMPING_JACK_BIN},
#endif
};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM]  =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {.vidx = SPORT_TYPE_RUNNING,         .str_idx = STR_SPORT_RUN,            .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_001_OUTDOOR_RUNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {.vidx = SPORT_TYPE_RIDE_BIKE,       .str_idx = STR_SPORT_RIDE_BIKE,     .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_002_OUTDOOR_CYCLE_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    {.vidx = SPORT_TYPE_JUMP_ROPE,       .str_idx = STR_SPORT_JUMP_ROPE,     .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_003_JUMP_ROPE_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {.vidx = SPORT_TYPE_SWIMMING,        .str_idx = STR_SPORT_SWIMMING,     .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_004_SWIM_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    {.vidx = SPORT_TYPE_BADMINTON,       .str_idx = STR_SPORT_BADMINTON,     .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_005_BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TABLE_TENNIS,    .str_idx = STR_SPORT_TABLE_TENNIS,  .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_006_TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TENNIS,          .str_idx = STR_SPORT_TENNIS,        .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_007_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    {.vidx = SPORT_TYPE_CLIMBING,        .str_idx = STR_SPORT_CLIMBING,      .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_008_CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    {.vidx = SPORT_TYPE_WALKING,         .str_idx = STR_SPORT_WALKING,       .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_009_OUTDOORRUN_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASKETBALL,      .str_idx = STR_SPORT_BASKETBALL,    .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_010_BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    {.vidx = SPORT_TYPE_FOOTBALL,        .str_idx = STR_SPORT_FOOTBALL,      .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_011_FOOTBALL_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASEBALL,        .str_idx = STR_SPORT_BASEBALL,      .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_012_BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    {.vidx = SPORT_TYPE_VOLLEYBALL,      .str_idx = STR_SPORT_VOLLEYBALL,    .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_013_VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    {.vidx = SPORT_TYPE_CRICKET,         .str_idx = STR_SPORT_CRICKET,       .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_014_CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    {.vidx = SPORT_TYPE_RUGBY,           .str_idx = STR_SPORT_RUGBY,         .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_015_RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    {.vidx = SPORT_TYPE_HOCKEY,          .str_idx = STR_SPORT_HOCKEY,        .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_016_HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    {.vidx = SPORT_TYPE_DANCE,           .str_idx = STR_SPORT_DANCE,         .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_017_DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    {.vidx = SPORT_TYPE_SPINNING,        .str_idx = STR_SPORT_SPINNING,      .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_018_INDOOR_CYCLE_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    {.vidx = SPORT_TYPE_YOGA,            .str_idx = STR_SPORT_YOGA,          .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_019_YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    {.vidx = SPORT_TYPE_SIT_UP,          .str_idx = STR_SPORT_SIT_UP,        .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_020_SITUP_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    {.vidx = SPORT_TYPE_TREADMILL,       .str_idx = STR_SPORT_TREADMILL,     .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_021_TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    {.vidx = SPORT_TYPE_GYMNASTICS,      .str_idx = STR_SPORT_GYMNASTICS,    .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_022_GYMNASTIC_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    {.vidx = SPORT_TYPE_BOATING,         .str_idx = STR_SPORT_BOATING,       .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_023_BOATING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    {.vidx = SPORT_TYPE_JUMPING_JACK,    .str_idx = STR_SPORT_JUMPING_JACK,  .res_addr = UI_BUF_I335001_3_EXERCISE_1_LIST_ICON_SPORT_42X42_X24_Y63_X24_Y139_X24_Y215_024_JUMPING_JACK_BIN},
#endif
};

static compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_MAX_SPORT_NUM];

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    compo_listbox_set(listbox, tbl_sport_list_sort, uteModuleSportGetHundredSportValidNumber());
#else
    compo_listbox_set(listbox, tbl_sport_list_sort, UTE_MODULE_SPORT_MAX_SPORT_NUM);
#endif

    compo_listbox_set_bgimg(listbox, UI_BUF_I335001_3_EXERCISE_1_LIST_BG_ICON_224X68_X8_Y50_Y126_Y202_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus(listbox, 102);
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

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
enum
{
    SPORT_MAIN_PAGE,
    SPORT_MORE_PAGE,
};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM]  =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    [SPORT_TYPE_RUNNING] = {.vidx = SPORT_TYPE_RUNNING,         .str_idx = STR_OUTDOOR_RUN,            .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_OUTDOOR_WALK_SUPPORT
    [SPORT_TYPE_OUTDOOR_WALK] = {.vidx = SPORT_TYPE_OUTDOOR_WALK,         .str_idx = STR_OUTDOOR_WALK,       .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_026OUTDOOR_WALKING_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    [SPORT_TYPE_TREADMILL] = {.vidx = SPORT_TYPE_TREADMILL,       .str_idx = STR_SPORT_TREADMILL,     .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_TRAIL_RUNNING_SUPPORT
    [SPORT_TYPE_TRAIL_RUNNING] = {.vidx = SPORT_TYPE_TRAIL_RUNNING,       .str_idx = STR_CROSS_COUNTRY_RUNNING,     .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_036CROSS_COUNTRY_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    [SPORT_TYPE_CLIMBING] = {.vidx = SPORT_TYPE_CLIMBING,        .str_idx = STR_SPORT_CLIMBING,      .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    [SPORT_TYPE_RIDE_BIKE] = {.vidx = SPORT_TYPE_RIDE_BIKE,       .str_idx = STR_OUTDOOR_CYCLING,     .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    [SPORT_TYPE_SPINNING] = {.vidx = SPORT_TYPE_SPINNING,           .str_idx = STR_INDOOR_CYCLING,   .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_FREE_TRAINING_SUPPORT
    [SPORT_TYPE_FREE_TRAINING] = {.vidx = SPORT_TYPE_FREE_TRAINING,        .str_idx = STR_SPORT_FREE_TRAINING,      .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_025FREE_TRAINING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    [SPORT_TYPE_BASKETBALL] = {.vidx = SPORT_TYPE_BASKETBALL,             .str_idx = STR_SPORT_BASKETBALL,        .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    [SPORT_TYPE_FOOTBALL] = {.vidx = SPORT_TYPE_FOOTBALL,               .str_idx = STR_SPORT_FOOTBALL,          .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    [SPORT_TYPE_TABLE_TENNIS] = {.vidx = SPORT_TYPE_TABLE_TENNIS,           .str_idx = STR_SPORT_TABLE_TENNIS,       .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    [SPORT_TYPE_BADMINTON] = {.vidx = SPORT_TYPE_BADMINTON,              .str_idx = STR_SPORT_BADMINTON,           .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CORE_TRAINING_SUPPORT
    [SPORT_TYPE_CORE_TRAINING] = {.vidx = SPORT_TYPE_CORE_TRAINING,           .str_idx = STR_CORE_TRAINING,                        .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_080FITNESS_GAMES_BIN},
#endif
#if UTE_MODULE_SPORT_MIXED_CARDIO_SUPPORT
    [SPORT_TYPE_MIXED_CARDIO] = {.vidx = SPORT_TYPE_MIXED_CARDIO,           .str_idx = STR_MIXED_AEROBIC,                          .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_055LATIN_DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_STRENGTH_TRAINING_SUPPORT
    [SPORT_TYPE_STRENGTH_TRAINING] = {.vidx = SPORT_TYPE_STRENGTH_TRAINING,      .str_idx = STR_STRENGTH_TRAINING,                          .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_028STRENGTH_TRAINING_BIN},
#endif
#if UTE_MODULE_SPORT_PILATES_SUPPORT
    [SPORT_TYPE_PILATES] = {.vidx = SPORT_TYPE_PILATES,                 .str_idx = STR_PILATES,                          .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_049CROSS_FIT_BIN},
#endif
#if UTE_MODULE_SPORT_ELLIPTICAL_TRAINER_SUPPORT
    [SPORT_TYPE_ELLIPTICAL_TRAINER] = {.vidx = SPORT_TYPE_ELLIPTICAL_TRAINER,     .str_idx = STR_ELLIPTICAL_MACHINE,                            .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_031ELLIPTICAL_TRAINER_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    [SPORT_TYPE_YOGA] = {.vidx = SPORT_TYPE_YOGA,                   .str_idx = STR_YOGA,                           .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    [SPORT_TYPE_CRICKET] = {.vidx = SPORT_TYPE_CRICKET,                .str_idx = STR_CRICKET,                            .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    [SPORT_TYPE_JUMP_ROPE] = {.vidx = SPORT_TYPE_JUMP_ROPE,              .str_idx = STR_SPORT_JUMP_ROPE,             .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    [SPORT_TYPE_SWIMMING] = {.vidx = SPORT_TYPE_SWIMMING,               .str_idx = STR_SPORT_SWIMMING,              .res_addr =   UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN },
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    [SPORT_TYPE_TENNIS] = {.vidx = SPORT_TYPE_TENNIS,                 .str_idx = STR_SPORT_TENNIS,                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    [SPORT_TYPE_WALKING] = {.vidx = SPORT_TYPE_WALKING,                .str_idx = STR_HIKING,                 .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    [SPORT_TYPE_BASEBALL] = {.vidx = SPORT_TYPE_BASEBALL,               .str_idx = STR_BASEBALL,                             .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    [SPORT_TYPE_VOLLEYBALL] = {.vidx = SPORT_TYPE_VOLLEYBALL,             .str_idx = STR_VOLLEYBALL,                             .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    [SPORT_TYPE_RUGBY] = {.vidx = SPORT_TYPE_RUGBY,                  .str_idx = STR_RUGBY,                             .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    [SPORT_TYPE_HOCKEY] = {.vidx = SPORT_TYPE_HOCKEY,                 .str_idx = STR_SPORT_HOCKEY,                              .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    [SPORT_TYPE_DANCE] = {.vidx = SPORT_TYPE_DANCE,                  .str_idx = STR_SPORT_DANCE,                              .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    [SPORT_TYPE_SIT_UP] = {.vidx = SPORT_TYPE_SIT_UP,                 .str_idx = STR_SPORT_SIT_UP,                              .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    [SPORT_TYPE_GYMNASTICS] = {.vidx = SPORT_TYPE_GYMNASTICS,             .str_idx = STR_SPORT_GYMNASTICS,                             .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    [SPORT_TYPE_BOATING] = {.vidx = SPORT_TYPE_BOATING,                .str_idx = STR_SPORT_BOATING,                             .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    [SPORT_TYPE_JUMPING_JACK] = {.vidx = SPORT_TYPE_JUMPING_JACK,           .str_idx = STR_SPORT_JUMPING_JACK,                             .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
#if UTE_MODULE_SPORT_INDOOR_WALK_SUPPORT
    [SPORT_TYPE_INDOOR_WALK] = {.vidx = SPORT_TYPE_INDOOR_WALK,            .str_idx = STR_INDOOR_WALKING,                              .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_026OUTDOOR_WALKING_BIN},
#endif
#if UTE_MODULE_SPORT_INDOOR_RUN_SUPPORT
    [SPORT_TYPE_INDOOR_RUN] = {.vidx = SPORT_TYPE_INDOOR_RUN,             .str_idx = STR_INDOOR_RUNNING,                               .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_027INDOOR_RUN_BIN},
#endif
#if UTE_MODULE_SPORT_STEP_TRAINING_SUPPORT
    [SPORT_TYPE_STEP_TRAINING] = {.vidx = SPORT_TYPE_STEP_TRAINING,          .str_idx = STR_STEPER,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_029STEPPING_BIN},
#endif
#if UTE_MODULE_SPORT_HORSE_RIDING_SUPPORT
    [SPORT_TYPE_HORSE_RIDING] = {.vidx = SPORT_TYPE_HORSE_RIDING,           .str_idx = STR_EQUESTRIAN,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_030HORSE_RIDING_BIN },
#endif
#if UTE_MODULE_SPORT_TAI_CHI_SUPPORT
    [SPORT_TYPE_TAI_CHI] = {.vidx = SPORT_TYPE_TAI_CHI,                .str_idx = STR_TAI_CHI,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_032TAI_CHI_BIN },
#endif
#if UTE_MODULE_SPORT_SHUTTLECOCK_SUPPORT
    [SPORT_TYPE_SHUTTLECOCK] = {.vidx = SPORT_TYPE_SHUTTLECOCK,            .str_idx = STR_SHUTTLECOCK,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_033SHUTTLECOCK_BIN },
#endif
#if UTE_MODULE_SPORT_BOXING_SUPPORT
    [SPORT_TYPE_BOXING] = {.vidx = SPORT_TYPE_BOXING,                 .str_idx = STR_BOXING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_034BOXING_BIN },
#endif
#if UTE_MODULE_SPORT_SKIING_SUPPORT
    [SPORT_TYPE_SKIING] = {.vidx = SPORT_TYPE_SKIING,                 .str_idx = STR_SKI,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_037SKIING_BIN },
#endif
#if UTE_MODULE_SPORT_ICE_HOCKEY_SUPPORT
    [SPORT_TYPE_ICE_HOCKEY] = {.vidx = SPORT_TYPE_ICE_HOCKEY,             .str_idx = STR_PUCK,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_038ICE_HOCKEY_BIN },
#endif
#if UTE_MODULE_SPORT_TAEKWONDO_SUPPORT
    [SPORT_TYPE_TAEKWONDO] = {.vidx = SPORT_TYPE_TAEKWONDO,              .str_idx = STR_TAEKWONDO,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_039TAEKWONDO_BIN },
#endif
#if UTE_MODULE_SPORT_VO2MAX_TEST_SUPPORT
    [SPORT_TYPE_VO2MAX_TEST] = {.vidx = SPORT_TYPE_VO2MAX_TEST,            .str_idx = STR_MAX_O2,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_040VO2MAX_TEST_BIN },
#endif
#if UTE_MODULE_SPORT_ROWING_MACHING_SUPPORT
    [SPORT_TYPE_ROWING_MACHING] = {.vidx = SPORT_TYPE_ROWING_MACHING,         .str_idx = STR_ROWING_MACHINE,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_041ROWING_MACHING_BIN },
#endif
#if UTE_MODULE_SPORT_AIR_WALKER_SUPPORT
    [SPORT_TYPE_AIR_WALKER] = {.vidx = SPORT_TYPE_AIR_WALKER,             .str_idx = STR_AIR_WALKER,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_042STROLLER_BIN },
#endif
#if UTE_MODULE_SPORT_HIKING_SUPPORT
    [SPORT_TYPE_HIKING] = {.vidx = SPORT_TYPE_HIKING,             .str_idx = STR_HIKING,                               .res_addr =  NULL},
#endif
#if UTE_MODULE_SPORT_ATHLETICS_SUPPORT
    [SPORT_TYPE_ATHLETICS] = {.vidx = SPORT_TYPE_ATHLETICS,             .str_idx = STR_TRACK_AND_FIELD,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_043ATHLETICS_BIN },
#endif
#if UTE_MODULE_SPORT_WAIST_TRAINING_SUPPORT
    [SPORT_TYPE_WAIST_TRAINING] = {.vidx = SPORT_TYPE_WAIST_TRAINING,             .str_idx = STR_WAIST_AND_ABDOMEN_TRAINING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_044WAIST_AND_ABDOMINAL_TRAINING_BIN },
#endif
#if UTE_MODULE_SPORT_KARATE_SUPPORT
    [SPORT_TYPE_KARATE] = {.vidx = SPORT_TYPE_KARATE,             .str_idx = STR_KARATE,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_045KARATE_BIN },
#endif
#if UTE_MODULE_SPORT_COOLDOWN_SUPPORT
    [SPORT_TYPE_COOLDOWN] = {.vidx = SPORT_TYPE_COOLDOWN,             .str_idx = STR_RELAXATION,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_046RELAXATION_BIN },
#endif
#if UTE_MODULE_SPORT_CROSS_TRAINING_SUPPORT
    [SPORT_TYPE_CROSS_TRAINING] = {.vidx = SPORT_TYPE_CROSS_TRAINING,             .str_idx = STR_CROSSFIT,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_047CROSS_TRAINING_BIN },
#endif
#if UTE_MODULE_SPORT_CROSS_FIT_SUPPORT
    [SPORT_TYPE_CROSS_FIT] = {.vidx = SPORT_TYPE_CROSS_FIT,             .str_idx = STR_CROSS_COORDINATION,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_048PILATES_BIN },
#endif
#if UTE_MODULE_SPORT_FUNCTIONAL_TRAINING_SUPPORT
    [SPORT_TYPE_FUNCTIONAL_TRAINING] = {.vidx = SPORT_TYPE_FUNCTIONAL_TRAINING,             .str_idx = STR_FUNCTIONAL_TRAINING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_050FUNCTIONAL_TRAINING_BIN },
#endif
#if UTE_MODULE_SPORT_PHYSICAL_TRAINING_SUPPORT
    [SPORT_TYPE_PHYSICAL_TRAINING] = {.vidx = SPORT_TYPE_PHYSICAL_TRAINING,             .str_idx = STR_PHYSICAL_TRAINING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_051PHYSICAL_TRAINING_BIN },
#endif
#if UTE_MODULE_SPORT_ARCHERY_SUPPORT
    [SPORT_TYPE_ARCHERY] = {.vidx = SPORT_TYPE_ARCHERY,             .str_idx = STR_ARCHERY,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_052ARCHERY_BIN },
#endif
#if UTE_MODULE_SPORT_FLEXIBILITY_SUPPORT
    [SPORT_TYPE_FLEXIBILITY] = {.vidx = SPORT_TYPE_FLEXIBILITY,             .str_idx = STR_FLEXIBILITY,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_053FLEXIBILITY_BIN },
#endif
#if UTE_MODULE_SPORT_LATIN_DANCE_SUPPORT
    [SPORT_TYPE_LATIN_DANCE] = {.vidx = SPORT_TYPE_LATIN_DANCE,             .str_idx = STR_LATIN_DANCE,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_054MIXED_AEROBIC_BIN },
#endif
#if UTE_MODULE_SPORT_STREET_DANCE_SUPPORT
    [SPORT_TYPE_STREET_DANCE] = {.vidx = SPORT_TYPE_STREET_DANCE,             .str_idx = STR_STREET_DANCE,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_056STREET_DANCE_BIN },
#endif
#if UTE_MODULE_SPORT_KICKBOXING_SUPPORT
    [SPORT_TYPE_KICKBOXING] = {.vidx = SPORT_TYPE_KICKBOXING,             .str_idx = STR_FREE_SPARRING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_057FREE_BOXING_BIN },
#endif
#if UTE_MODULE_SPORT_BARRE_SUPPORT
    [SPORT_TYPE_BARRE] = {.vidx = SPORT_TYPE_BARRE,             .str_idx = STR_BALLET,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_058BALLET_BIN },
#endif
#if UTE_MODULE_SPORT_AUSTRALIAN_FOOTBALL_SUPPORT
    [SPORT_TYPE_AUSTRALIAN_FOOTBALL] = {.vidx = SPORT_TYPE_AUSTRALIAN_FOOTBALL,             .str_idx = STR_AUSTRALIAN_RULES_FOOTBALL,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_059AUSTRALIAN_SOCCER_BIN },
#endif
#if UTE_MODULE_SPORT_MARTIAL_ARTS_SUPPORT
    [SPORT_TYPE_MARTIAL_ARTS] = {.vidx = SPORT_TYPE_MARTIAL_ARTS,              .str_idx = STR_MARTIAL_ARTS,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_060MARTIAL_ARTS_BIN },
#endif
#if UTE_MODULE_SPORT_STAIRS_SUPPORT
    [SPORT_TYPE_STAIRS] = {.vidx = SPORT_TYPE_STAIRS,             .str_idx = STR_CLIMBING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_061STAIR_CLIMBING_BIN },
#endif
#if UTE_MODULE_SPORT_HANDBALL_SUPPORT
    [SPORT_TYPE_HANDBALL] = {.vidx = SPORT_TYPE_HANDBALL,             .str_idx = STR_HANDBALL,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_062HANDBALL_BIN },
#endif
#if UTE_MODULE_SPORT_BOWLING_SUPPORT
    [SPORT_TYPE_BOWLING] = {.vidx = SPORT_TYPE_BOWLING,             .str_idx = STR_BOWING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_063BOWLING_BIN   },
#endif
#if UTE_MODULE_SPORT_RACQUETBALL_SUPPORT
    [SPORT_TYPE_RACQUETBALL] = {.vidx = SPORT_TYPE_RACQUETBALL,             .str_idx = STR_SQUASH,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_064SQUASH_BIN  },
#endif
#if UTE_MODULE_SPORT_CURLING_SUPPORT
    [SPORT_TYPE_CURLING] = {.vidx = SPORT_TYPE_CURLING,             .str_idx = STR_CURLING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_065CURLING_BIN  },
#endif
#if UTE_MODULE_SPORT_HUNTING_SUPPORT
    [SPORT_TYPE_HUNTING] = {.vidx = SPORT_TYPE_HUNTING,              .str_idx = STR_HUNT,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_066HUNTING_BIN  },
#endif
#if UTE_MODULE_SPORT_SNOWBOARDING_SUPPORT
    [SPORT_TYPE_SNOWBOARDING] = {.vidx = SPORT_TYPE_SNOWBOARDING,              .str_idx = STR_SNOWBOARDING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_067SNOWBOARDING_BIN  },
#endif
#if UTE_MODULE_SPORT_PLAY_SUPPORT
    [SPORT_TYPE_PLAY] = {.vidx = SPORT_TYPE_PLAY,              .str_idx = STR_LEISURE_SPORT,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_068LEISURE_SPORTS_BIN },
#endif
#if UTE_MODULE_SPORT_AMERICAN_FOOTBALL_SUPPORT
    [SPORT_TYPE_AMERICAN_FOOTBALL] = {.vidx = SPORT_TYPE_AMERICAN_FOOTBALL,              .str_idx = STR_AMERICAN_FOOTBALL,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_069AMERICAN_FOOTBALL_BIN },
#endif
#if UTE_MODULE_SPORT_HAND_CYCLING_SUPPORT
    [SPORT_TYPE_HAND_CYCLING] = {.vidx = SPORT_TYPE_HAND_CYCLING,              .str_idx = STR_HANDCAR,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_070HAND_CYCLING_BIN  },
#endif
#if UTE_MODULE_SPORT_FISHING_SUPPORT
    [SPORT_TYPE_FISHING] = {.vidx = SPORT_TYPE_FISHING,             .str_idx = STR_FISHING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_071FISHING_BIN },
#endif
#if UTE_MODULE_SPORT_DISC_SPORTS_SUPPORT
    [SPORT_TYPE_DISC_SPORTS] = {.vidx = SPORT_TYPE_DISC_SPORTS,             .str_idx = STR_FRISBEE,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_072FRISBEE_BIN },
#endif
#if UTE_MODULE_SPORT_GOLF_SUPPORT
    [SPORT_TYPE_GOLF] = {.vidx = SPORT_TYPE_GOLF,             .str_idx = STR_GOLF,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_073GOLF_BIN },
#endif
#if UTE_MODULE_SPORT_FOLK_DANCE_SUPPORT
    [SPORT_TYPE_FOLK_DANCE] = {.vidx = SPORT_TYPE_FOLK_DANCE,             .str_idx = STR_NATIONAL_DANCE,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_074FOLK_DANCE_BIN },
#endif
#if UTE_MODULE_SPORT_DOWNHILL_SKIING_SUPPORT
    [SPORT_TYPE_DOWNHILL_SKIING] = {.vidx = SPORT_TYPE_DOWNHILL_SKIING,             .str_idx = STR_ALPINE_SKIING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_075DOWNHILL_SKIING_BIN },
#endif
#if UTE_MODULE_SPORT_SNOW_SPORTS_SUPPORT
    [SPORT_TYPE_SNOW_SPORTS] = {.vidx = SPORT_TYPE_SNOW_SPORTS,             .str_idx = STR_SNOW_SPORTS,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_076SNOW_SPORTS_BIN },
#endif
#if UTE_MODULE_SPORT_MIND_BODY_SUPPORT
    [SPORT_TYPE_MIND_BODY] = {.vidx = SPORT_TYPE_MIND_BODY,             .str_idx = STR_MEDITATION,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_078CORE_TRAINING_BIN },
#endif
#if UTE_MODULE_SPORT_SKATING_SUPPORT
    [SPORT_TYPE_SKATING] = {.vidx = SPORT_TYPE_SKATING,              .str_idx = STR_SKATING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_079ICE_SKATING_BIN  },
#endif
#if UTE_MODULE_SPORT_FITNESS_GAMING_SUPPORT
    [SPORT_TYPE_FITNESS_GAMING] = {.vidx = SPORT_TYPE_FITNESS_GAMING,             .str_idx = STR_FITNESS_GAMES,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_080FITNESS_GAMES_BIN },
#endif
#if UTE_MODULE_SPORT_AEROBICS_SUPPORT
    [SPORT_TYPE_AEROBICS] = {.vidx = SPORT_TYPE_AEROBICS,             .str_idx = STR_AERBOICS,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_081GYMNASTICS_BIN  },
#endif
#if UTE_MODULE_SPORT_GROUP_TRAINING_SUPPORT
    [SPORT_TYPE_GROUP_TRAINING] = {.vidx = SPORT_TYPE_GROUP_TRAINING,             .str_idx = STR_GROUP_CALLISHENICS,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_082GROUP_TRAINING_BIN  },
#endif
#if UTE_MODULE_SPORT_KENDO_SUPPORT
    [SPORT_TYPE_KENDO] = {.vidx = SPORT_TYPE_KENDO,             .str_idx = STR_KICK_BOXING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_083KICKBOXING_BIN  },
#endif
#if UTE_MODULE_SPORT_LACROSSE_SUPPORT
    [SPORT_TYPE_LACROSSE] = {.vidx = SPORT_TYPE_LACROSSE,             .str_idx = STR_LACROSSE,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_084LACROSSE_BIN  },
#endif
#if UTE_MODULE_SPORT_ROLLING_SUPPORT
    [SPORT_TYPE_ROLLING] = {.vidx = SPORT_TYPE_ROLLING,             .str_idx = STR_FOAM_ROLLER_STRECHING,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_085FOAM_ROLLING_BIN  },
#endif
#if UTE_MODULE_SPORT_WRESTLING_SUPPORT
    [SPORT_TYPE_WRESTLING] = {.vidx = SPORT_TYPE_WRESTLING,             .str_idx = STR_WRESTING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_086WRESTLING_BIN },
#endif
#if UTE_MODULE_SPORT_FENCING_SUPPORT
    [SPORT_TYPE_FENCING] = {.vidx = SPORT_TYPE_FENCING,             .str_idx = STR_FENCING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_087FENCING_BIN },
#endif
#if UTE_MODULE_SPORT_SOFTBALL_SUPPORT
    [SPORT_TYPE_SOFTBALL] = {.vidx = SPORT_TYPE_SOFTBALL,              .str_idx = STR_SOFTBALL,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_088SOFTBALL_BIN },
#endif
#if UTE_MODULE_SPORT_SINGLE_BAR_SUPPORT
    [SPORT_TYPE_SINGLE_BAR] = {.vidx = SPORT_TYPE_SINGLE_BAR,             .str_idx = STR_HORIZONTAL_BAR,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_089SINGLE_BAR_BIN   },
#endif
#if UTE_MODULE_SPORT_PARALLEL_BARS_SUPPORT
    [SPORT_TYPE_PARALLEL_BARS] = {.vidx = SPORT_TYPE_PARALLEL_BARS,             .str_idx = STR_PARALLEL_BARS,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_090PARALLEL_BARS_BIN  },
#endif
#if UTE_MODULE_SPORT_ROLLER_SKATING_SUPPORT
    [SPORT_TYPE_ROLLER_SKATING] = {.vidx = SPORT_TYPE_ROLLER_SKATING,             .str_idx = STR_INDOOR_SKATING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_091ROLLER_SKATING_BIN },
#endif
#if UTE_MODULE_SPORT_HULA_HOOP_SUPPORT
    [SPORT_TYPE_HULA_HOOP] = {.vidx = SPORT_TYPE_HULA_HOOP,             .str_idx = STR_HULA_HOOP,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_092HULA_HOOP_BIN  },
#endif
#if UTE_MODULE_SPORT_DARTS_SUPPORT
    [SPORT_TYPE_DARTS] = {.vidx = SPORT_TYPE_DARTS,              .str_idx = STR_DARTS,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_093DARTS_BIN  },
#endif
#if UTE_MODULE_SPORT_PICKLEBALL_SUPPORT
    [SPORT_TYPE_PICKLEBALL] = {.vidx = SPORT_TYPE_PICKLEBALL,              .str_idx = STR_PICKLEBALL,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_094PICKLEBALL_BIN },
#endif
#if UTE_MODULE_SPORT_HIIT_SUPPORT
    [SPORT_TYPE_HIIT] = {.vidx = SPORT_TYPE_HIIT,              .str_idx = STR_HIIT,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_095HIIT_BIN },
#endif
#if UTE_MODULE_SPORT_SHOOTING_SUPPORT
    [SPORT_TYPE_SHOOTING] = {.vidx = SPORT_TYPE_SHOOTING,              .str_idx = STR_SHOOTING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_096SHOOTING_BIN  },
#endif
#if UTE_MODULE_SPORT_JUDO_SUPPORT
    [SPORT_TYPE_JUDO] = {.vidx = SPORT_TYPE_JUDO,             .str_idx = STR_JUDO,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_97JUDO_BIN  },
#endif
#if UTE_MODULE_SPORT_TRAMPOLINE_SUPPORT
    [SPORT_TYPE_TRAMPOLINE] = {.vidx = SPORT_TYPE_TRAMPOLINE,              .str_idx = STR_TRAMPOLINING,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_098TRAMPOLINE_BIN },
#endif
#if UTE_MODULE_SPORT_SKATEBOARDING_SUPPORT
    [SPORT_TYPE_SKATEBOARDING] = {.vidx = SPORT_TYPE_SKATEBOARDING,             .str_idx = STR_SKATEBOARD,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_099SKATEBOARDING_BIN  },
#endif
#if UTE_MODULE_SPORT_HOVERBOARD_SUPPORT
    [SPORT_TYPE_HOVERBOARD] = {.vidx = SPORT_TYPE_HOVERBOARD,             .str_idx = STR_HOVER_BOARD,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_100HOVERBOARD_BIN   },
#endif
#if UTE_MODULE_SPORT_BLADING_SUPPORT
    [SPORT_TYPE_BLADING] = {.vidx = SPORT_TYPE_BLADING,             .str_idx = STR_ROLLER_SKATING,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_101BLADING_BIN  },
#endif
#if UTE_MODULE_SPORT_PARKOUR_SUPPORT
    [SPORT_TYPE_PARKOUR] = {.vidx = SPORT_TYPE_PARKOUR,              .str_idx = STR_PARKOUR,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_102PARKOUR_BIN },
#endif
#if UTE_MODULE_SPORT_DIVING_SUPPORT
    [SPORT_TYPE_DIVING] = {.vidx = SPORT_TYPE_DIVING,             .str_idx = STR_DIVING,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_103DIVING_BIN  },
#endif
#if UTE_MODULE_SPORT_SURFING_SUPPORT
    [SPORT_TYPE_SURFING] = {.vidx = SPORT_TYPE_SURFING,              .str_idx = STR_SURFING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_104SURFING_BIN  },
#endif
#if UTE_MODULE_SPORT_SNORKELING_SUPPORT
    [SPORT_TYPE_SNORKELING] = {.vidx = SPORT_TYPE_SNORKELING,             .str_idx = STR_SNORKELING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_105SNORKELING_BIN  },
#endif
#if UTE_MODULE_SPORT_PULL_UP_SUPPORT
    [SPORT_TYPE_PULL_UP] = {.vidx = SPORT_TYPE_PULL_UP,              .str_idx = STR_PULL_UPS,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_106PULL_UPS_BIN },
#endif
#if UTE_MODULE_SPORT_PUSH_UP_SUPPORT
    [SPORT_TYPE_PUSH_UP] = {.vidx = SPORT_TYPE_PUSH_UP,              .str_idx = STR_PUSH_UPS,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_107PUSH_UPS_BIN },
#endif
#if UTE_MODULE_SPORT_PLANKING_SUPPORT
    [SPORT_TYPE_PLANKING] = {.vidx = SPORT_TYPE_PLANKING,              .str_idx = STR_PLANKING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_108PLANKING_BIN  },
#endif
#if UTE_MODULE_SPORT_ROCK_CLIMBING_SUPPORT
    [SPORT_TYPE_ROCK_CLIMBING] = {.vidx = SPORT_TYPE_ROCK_CLIMBING,               .str_idx = STR_ROCK_CLIMBING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_109ROCK_CLIMBING_BIN },
#endif
#if UTE_MODULE_SPORT_HIGHTJUMP_SUPPORT
    [SPORT_TYPE_HIGHTJUMP] = {.vidx = SPORT_TYPE_HIGHTJUMP,             .str_idx = STR_HIG_JUMPING,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_110HIGH_JUMP_BIN  },
#endif
#if UTE_MODULE_SPORT_BUNGEE_JUMPING_SUPPORT
    [SPORT_TYPE_BUNGEE_JUMPING] = {.vidx = SPORT_TYPE_BUNGEE_JUMPING,              .str_idx = STR_BUNGEE_JUMPING,                                .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_111BUNGEE_JUMP_BIN },
#endif
#if UTE_MODULE_SPORT_LONGJUMP_SUPPORT
    [SPORT_TYPE_LONGJUMP] = {.vidx = SPORT_TYPE_LONGJUMP,              .str_idx = STR_LONG_JUMPING,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_112LONG_JUMP_BIN  },
#endif
#if UTE_MODULE_SPORT_MARATHON_SUPPORT
    [SPORT_TYPE_MARATHON] = {.vidx = SPORT_TYPE_MARATHON,              .str_idx = STR_MATATHON,                                .res_addr =  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_113MARATHON_BIN },
#endif
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(uteModuleSportTargetGetShowIndex())
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
        case SPORT_TYPE_INDOOR_WALK:
        case SPORT_TYPE_TRAIL_RUNNING:
        case SPORT_TYPE_OUTDOOR_WALK:
        case SPORT_TYPE_HIKING:
        case SPORT_TYPE_MARATHON:
#endif
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

#define SPORT_MAIN_NUM 8
compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM];

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx <= UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM)
    {
        return tbl_sport_list_sort[sport_idx - 1].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx <= UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM)
    {
        return tbl_sport_list_sort[sport_idx - 1].res_addr;
    }
    return 0;
}

void get_sport_taget_txt(compo_listbox_t *list,u8 i,char *txt)
{
    char txt_buf[30];
    uteModuleSportTargetMoreGetMoreFromIndex(i);
    u8 state = uteModuleSportTargetGetIsNoTarget();
    memset(txt_buf,0,sizeof(txt_buf));
    switch(state)
    {
        case SPORT_TARGET_DISTANCE_SET:
        {
            int dans_t = uteModuleSportTargetGetDistanceGoal() / 1000;
            int dans_s = (uteModuleSportTargetGetDistanceGoal() % 1000 / 100);
            snprintf(txt_buf,sizeof(txt_buf),"%d.%d%s",dans_t,dans_s,i18n[STR_KM]);
        }
        break;
        case SPORT_TARGET_TIME_SET:
        {
            int time = uteModuleSportTargetGetTimeGoal() / 60;
            snprintf(txt_buf,sizeof(txt_buf),"%d%s",time,i18n[STR_MINUTE]);
        }
        break;
        case SPORT_TARGET_KCAL_SET:
        {
            int kcal = uteModuleSportTargetGetKcalGoal();
            snprintf(txt_buf,sizeof(txt_buf),"%d%s",kcal,i18n[STR_KCAL]);
        }
        break;
        default:
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s",i18n[STR_NO_GOAL]);
        }
        break;
    }
    memcpy(txt,txt_buf,sizeof(txt_buf));
}

static compo_form_t* __this = NULL;
compo_listbox_item_t tbl_sport_main_list[SPORT_MAIN_NUM];
//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    for(u8 i =0; i < SPORT_MAIN_NUM; i++)
    {
        tbl_sport_main_list[i] = tbl_sport_list_sort[i];
    }
    //memcpy(tbl_sport_main_list,tbl_sport_list_sort,(int)sizeof(tbl_sport_main_list)/sizeof(tbl_sport_main_list[0]));
    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SPORT_LIST_BY_CHUANYIN);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    compo_listbox_set(listbox, tbl_sport_main_list, SPORT_MAIN_NUM);
    printf("line %d listbox->icon_area = %d\n",__LINE__,listbox->icon_area.wid);
    //listbox->icon_area.wid = 84;
    area_t area = {84,84};
    compo_listbox_set_icon_area(listbox,area);
    for(u8 i =0; i < SPORT_MAIN_NUM; i++)
    {
        widget_set_size(listbox->item_icon[i],84, 84);
    }
    printf("line %d listbox->icon_area = %d\n",__LINE__,listbox->icon_area.wid);
#else
    compo_listbox_set(listbox, tbl_sport_list_sort, UTE_MODULE_SPORT_MAX_SPORT_NUM);
#endif
    compo_listbox_set_bgimg_by_shape(listbox,GUI_SCREEN_CENTER_X,212 / 2, 344, 212,25,0XFF,\
                                     make_color(0x25,0x25,0x25));
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 0);

    compo_listbox_update(listbox);
    int move_y = compo_listbox_gety_byidx(listbox, 1);

    compo_shape_t * shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X,move_y,310,84,25,0xff,make_color(0x25,0x25,0x25));
    ute_compo_setid_for_btn(shape,COMPO_ID_SHAPE_MORE);

    compo_picturebox_t * pic = compo_picturebox_create(frm,UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_ICON_MORE_BIN);
    compo_setid(pic,COMPO_ID_PIC_MORE);

    compo_textbox_t* textbox = compo_textbox_create(frm,strlen(i18n[STR_MORE_SPORT]));
    compo_textbox_set(textbox,i18n[STR_MORE_SPORT]);
    compo_setid(textbox,COMPO_ID_TXT_MORE);
    if(compo_listbox_gety_byidx(listbox,SPORT_MAIN_NUM - 1) > GUI_SCREEN_HEIGHT)
    {
        compo_picturebox_set_visible(pic,false);
        compo_shape_set_visible(shape,false);
        compo_textbox_set_visible(textbox,false);
    }
    __this = frm;
    return frm;
}
compo_listbox_item_t tbl_sport_more_list[12];
compo_form_t *func_sport_more_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE & 0xFD);
    compo_form_set_title(frm, i18n[STR_MORE_SPORT]);
    int count = (uteModuleSportGetHundredSportValidNumber() - SPORT_MAIN_NUM) > 12 ? 12 :(uteModuleSportGetHundredSportValidNumber() - SPORT_MAIN_NUM);
    for(u8 i =0; i < count; i++)
    {
        tbl_sport_more_list[i] = tbl_sport_list_sort[i + SPORT_MAIN_NUM];
        tbl_sport_more_list[i].res_addr = NULL;
    }
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
    compo_listbox_set(listbox, tbl_sport_more_list,count);
    compo_listbox_set_bgimg(listbox, UI_BUF_I341001_20_ALARM_CLOCK_LINE_BIN);
    compo_listbox_set_item_height(listbox,107);
    compo_listbox_set_focus_byidx(listbox, 1);
    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_update(listbox);
    __this = frm;
    return frm;

}

//点进图标进入应用
static void func_sport_list_icon_click(void)
{
    int icon_idx;
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;

    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_SHAPE_MORE:
            func_free(listbox->mcb);
            compo_form_destroy(func_cb.frm_main);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
            func_sport_list_reorder(tbl_sport_list_sort);
#endif
            func_cb.frm_main = func_sport_more_form_create();
            f_sport->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
            listbox = f_sport->listbox;
            f_sport->page = SPORT_MORE_PAGE;
            listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));
            compo_listbox_move_init(listbox);
            f_sport->count = uteModuleSportGetHundredSportValidNumber();
            int count = (f_sport->count - SPORT_MAIN_NUM) > 12 ? 12 :(f_sport->count - SPORT_MAIN_NUM);
            compo_listbox_move_init_modify(listbox, compo_listbox_gety_byidx(listbox,1), compo_listbox_gety_byidx(listbox, count-1) -180);
            break;

        default:
            break;
    }

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
    if(f_sport->page == SPORT_MORE_PAGE)
    {
        icon_idx = icon_idx + SPORT_MAIN_NUM;
    }

    //uteModuleSportStartMoreSports(tbl_sport_list_sort[icon_idx].vidx, 1, 0);
    uteModuleSportTargetSetShowIndex(uteModuleSportFindHundredSportIndexByMode(tbl_sport_list_sort[icon_idx].vidx));
    uteModuleSportTargetMoreGetMoreFromIndex(uteModuleSportTargetGetShowIndex() - 1);
    extern u8 sport_target_page;
    sport_target_page = 0;
    uteTaskGuiStartScreen(FUNC_SPORT_SUB_SET, 0, __func__);
    printf("line %d,id = %d\n",__LINE__,uteModuleSportTargetGetShowIndex());
    sys_cb.sport_idx = tbl_sport_list_sort[icon_idx].vidx;
    uteModuleSportMoreSportSetType(tbl_sport_list_sort[icon_idx].vidx);

}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    [SPORT_TYPE_RUNNING]      = {STR_SPORT_RUN,           UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    [SPORT_TYPE_RIDE_BIKE]    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    [SPORT_TYPE_JUMP_ROPE]    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    [SPORT_TYPE_SWIMMING]     = {STR_SPORT_JUMP_ROPE,     UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    [SPORT_TYPE_BADMINTON]    = {STR_SPORT_BADMINTON,     UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_005BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    [SPORT_TYPE_TABLE_TENNIS] = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    [SPORT_TYPE_TENNIS]       = {STR_SPORT_TENNIS,        UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    [SPORT_TYPE_CLIMBING]     = {STR_SPORT_CLIMBING,      UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    [SPORT_TYPE_WALKING]      = {STR_SPORT_WALKING,       UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    [SPORT_TYPE_BASKETBALL]   = {STR_SPORT_BASKETBALL,    UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    [SPORT_TYPE_FOOTBALL]     = {STR_SPORT_FOOTBALL,      UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    [SPORT_TYPE_BASEBALL]     = {STR_SPORT_BASEBALL,      UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    [SPORT_TYPE_VOLLEYBALL]   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    [SPORT_TYPE_CRICKET]      = {STR_SPORT_CRICKET,       UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    [SPORT_TYPE_RUGBY]        = {STR_SPORT_RUGBY,         UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    [SPORT_TYPE_HOCKEY]       = {STR_SPORT_HOCKEY,        UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    [SPORT_TYPE_DANCE]        = {STR_SPORT_DANCE,         UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    [SPORT_TYPE_SPINNING]     = {STR_SPORT_SPINNING,      UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    [SPORT_TYPE_YOGA]         = {STR_SPORT_YOGA,          UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    [SPORT_TYPE_SIT_UP]       = {STR_SPORT_SIT_UP,        UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    [SPORT_TYPE_TREADMILL]    = {STR_SPORT_TREADMILL,     UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    [SPORT_TYPE_GYMNASTICS]   = {STR_SPORT_GYMNASTICS,    UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    [SPORT_TYPE_BOATING]      = {STR_SPORT_BOATING,       UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    [SPORT_TYPE_JUMPING_JACK] = {STR_SPORT_JUMPING_JACK,  UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM]  =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {.vidx = SPORT_TYPE_RUNNING,         .str_idx = STR_SPORT_RUN,            .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {.vidx = SPORT_TYPE_RIDE_BIKE,       .str_idx = STR_SPORT_RIDE_BIKE,     .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    {.vidx = SPORT_TYPE_JUMP_ROPE,       .str_idx = STR_SPORT_JUMP_ROPE,     .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {.vidx = SPORT_TYPE_SWIMMING,        .str_idx = STR_SPORT_SWIMMING,     .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    {.vidx = SPORT_TYPE_BADMINTON,       .str_idx = STR_SPORT_BADMINTON,     .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_005BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TABLE_TENNIS,    .str_idx = STR_SPORT_TABLE_TENNIS,  .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TENNIS,          .str_idx = STR_SPORT_TENNIS,        .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    {.vidx = SPORT_TYPE_CLIMBING,        .str_idx = STR_SPORT_CLIMBING,      .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    {.vidx = SPORT_TYPE_WALKING,         .str_idx = STR_SPORT_WALKING,       .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASKETBALL,      .str_idx = STR_SPORT_BASKETBALL,    .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    {.vidx = SPORT_TYPE_FOOTBALL,        .str_idx = STR_SPORT_FOOTBALL,      .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASEBALL,        .str_idx = STR_SPORT_BASEBALL,      .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    {.vidx = SPORT_TYPE_VOLLEYBALL,      .str_idx = STR_SPORT_VOLLEYBALL,    .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    {.vidx = SPORT_TYPE_CRICKET,         .str_idx = STR_SPORT_CRICKET,       .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    {.vidx = SPORT_TYPE_RUGBY,           .str_idx = STR_SPORT_RUGBY,         .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    {.vidx = SPORT_TYPE_HOCKEY,          .str_idx = STR_SPORT_HOCKEY,        .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    {.vidx = SPORT_TYPE_DANCE,           .str_idx = STR_SPORT_DANCE,         .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    {.vidx = SPORT_TYPE_SPINNING,        .str_idx = STR_SPORT_SPINNING,      .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    {.vidx = SPORT_TYPE_YOGA,            .str_idx = STR_SPORT_YOGA,          .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    {.vidx = SPORT_TYPE_SIT_UP,          .str_idx = STR_SPORT_SIT_UP,        .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    {.vidx = SPORT_TYPE_TREADMILL,       .str_idx = STR_SPORT_TREADMILL,     .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    {.vidx = SPORT_TYPE_GYMNASTICS,      .str_idx = STR_SPORT_GYMNASTICS,    .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    {.vidx = SPORT_TYPE_BOATING,         .str_idx = STR_SPORT_BOATING,       .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    {.vidx = SPORT_TYPE_JUMPING_JACK,    .str_idx = STR_SPORT_JUMPING_JACK,  .res_addr = UI_BUF_I341001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
};

static compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_MAX_SPORT_NUM];

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    compo_listbox_set(listbox, tbl_sport_list_sort, uteModuleSportGetHundredSportValidNumber());
#else
    compo_listbox_set(listbox, tbl_sport_list_sort, UTE_MODULE_SPORT_MAX_SPORT_NUM);
#endif
    compo_listbox_set_bgimg(listbox, UI_BUF_I341001_19_BREATHING_TRAINING_BG_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
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

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    [SPORT_TYPE_RUNNING]      = {STR_SPORT_RUN,           UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    [SPORT_TYPE_RIDE_BIKE]    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    [SPORT_TYPE_JUMP_ROPE]    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    [SPORT_TYPE_SWIMMING]     = {STR_SPORT_JUMP_ROPE,     UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    [SPORT_TYPE_BADMINTON]    = {STR_SPORT_BADMINTON,     UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_005BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    [SPORT_TYPE_TABLE_TENNIS] = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    [SPORT_TYPE_TENNIS]       = {STR_SPORT_TENNIS,        UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    [SPORT_TYPE_CLIMBING]     = {STR_SPORT_CLIMBING,      UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    [SPORT_TYPE_WALKING]      = {STR_SPORT_WALKING,       UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    [SPORT_TYPE_BASKETBALL]   = {STR_SPORT_BASKETBALL,    UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    [SPORT_TYPE_FOOTBALL]     = {STR_SPORT_FOOTBALL,      UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    [SPORT_TYPE_BASEBALL]     = {STR_SPORT_BASEBALL,      UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    [SPORT_TYPE_VOLLEYBALL]   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    [SPORT_TYPE_CRICKET]      = {STR_SPORT_CRICKET,       UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    [SPORT_TYPE_RUGBY]        = {STR_SPORT_RUGBY,         UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    [SPORT_TYPE_HOCKEY]       = {STR_SPORT_HOCKEY,        UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    [SPORT_TYPE_DANCE]        = {STR_SPORT_DANCE,         UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    [SPORT_TYPE_SPINNING]     = {STR_SPORT_SPINNING,      UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    [SPORT_TYPE_YOGA]         = {STR_SPORT_YOGA,          UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    [SPORT_TYPE_SIT_UP]       = {STR_SPORT_SIT_UP,        UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    [SPORT_TYPE_TREADMILL]    = {STR_SPORT_TREADMILL,     UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    [SPORT_TYPE_GYMNASTICS]   = {STR_SPORT_GYMNASTICS,    UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    [SPORT_TYPE_BOATING]      = {STR_SPORT_BOATING,       UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    [SPORT_TYPE_JUMPING_JACK] = {STR_SPORT_JUMPING_JACK,  UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM]  =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {.vidx = SPORT_TYPE_RUNNING,         .str_idx = STR_SPORT_RUN,            .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {.vidx = SPORT_TYPE_RIDE_BIKE,       .str_idx = STR_SPORT_RIDE_BIKE,     .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    {.vidx = SPORT_TYPE_JUMP_ROPE,       .str_idx = STR_SPORT_JUMP_ROPE,     .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {.vidx = SPORT_TYPE_SWIMMING,        .str_idx = STR_SPORT_SWIMMING,     .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    {.vidx = SPORT_TYPE_BADMINTON,       .str_idx = STR_SPORT_BADMINTON,     .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_005BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TABLE_TENNIS,    .str_idx = STR_SPORT_TABLE_TENNIS,  .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TENNIS,          .str_idx = STR_SPORT_TENNIS,        .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    {.vidx = SPORT_TYPE_CLIMBING,        .str_idx = STR_SPORT_CLIMBING,      .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    {.vidx = SPORT_TYPE_WALKING,         .str_idx = STR_SPORT_WALKING,       .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASKETBALL,      .str_idx = STR_SPORT_BASKETBALL,    .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    {.vidx = SPORT_TYPE_FOOTBALL,        .str_idx = STR_SPORT_FOOTBALL,      .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASEBALL,        .str_idx = STR_SPORT_BASEBALL,      .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    {.vidx = SPORT_TYPE_VOLLEYBALL,      .str_idx = STR_SPORT_VOLLEYBALL,    .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    {.vidx = SPORT_TYPE_CRICKET,         .str_idx = STR_SPORT_CRICKET,       .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    {.vidx = SPORT_TYPE_RUGBY,           .str_idx = STR_SPORT_RUGBY,         .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    {.vidx = SPORT_TYPE_HOCKEY,          .str_idx = STR_SPORT_HOCKEY,        .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    {.vidx = SPORT_TYPE_DANCE,           .str_idx = STR_SPORT_DANCE,         .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    {.vidx = SPORT_TYPE_SPINNING,        .str_idx = STR_SPORT_SPINNING,      .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    {.vidx = SPORT_TYPE_YOGA,            .str_idx = STR_SPORT_YOGA,          .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    {.vidx = SPORT_TYPE_SIT_UP,          .str_idx = STR_SPORT_SIT_UP,        .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    {.vidx = SPORT_TYPE_TREADMILL,       .str_idx = STR_SPORT_TREADMILL,     .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    {.vidx = SPORT_TYPE_GYMNASTICS,      .str_idx = STR_SPORT_GYMNASTICS,    .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    {.vidx = SPORT_TYPE_BOATING,         .str_idx = STR_SPORT_BOATING,       .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    {.vidx = SPORT_TYPE_JUMPING_JACK,    .str_idx = STR_SPORT_JUMPING_JACK,  .res_addr = UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
};

static compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_MAX_SPORT_NUM];

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_TITLE_NORMAL);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    compo_listbox_set(listbox, tbl_sport_list_sort, uteModuleSportGetHundredSportValidNumber());
#else
    compo_listbox_set(listbox, tbl_sport_list_sort, UTE_MODULE_SPORT_MAX_SPORT_NUM);
#endif
    compo_listbox_set_bgimg(listbox, UI_BUF_I343001_3_EXERCISE_1_LIST_ICON_ICON_BG_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
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


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    [SPORT_TYPE_RUNNING]      = {STR_SPORT_RUN,           UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    [SPORT_TYPE_RIDE_BIKE]    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    [SPORT_TYPE_JUMP_ROPE]    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    [SPORT_TYPE_SWIMMING]     = {STR_SPORT_JUMP_ROPE,     UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    [SPORT_TYPE_BADMINTON]    = {STR_SPORT_BADMINTON,     UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_005BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    [SPORT_TYPE_TABLE_TENNIS] = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    [SPORT_TYPE_TENNIS]       = {STR_SPORT_TENNIS,        UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    [SPORT_TYPE_CLIMBING]     = {STR_SPORT_CLIMBING,      UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    [SPORT_TYPE_WALKING]      = {STR_SPORT_WALKING,       UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    [SPORT_TYPE_BASKETBALL]   = {STR_SPORT_BASKETBALL,    UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    [SPORT_TYPE_FOOTBALL]     = {STR_SPORT_FOOTBALL,      UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    [SPORT_TYPE_BASEBALL]     = {STR_SPORT_BASEBALL,      UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    [SPORT_TYPE_VOLLEYBALL]   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    [SPORT_TYPE_CRICKET]      = {STR_SPORT_CRICKET,       UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    [SPORT_TYPE_RUGBY]        = {STR_SPORT_RUGBY,         UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    [SPORT_TYPE_HOCKEY]       = {STR_SPORT_HOCKEY,        UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    [SPORT_TYPE_DANCE]        = {STR_SPORT_DANCE,         UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    [SPORT_TYPE_SPINNING]     = {STR_SPORT_SPINNING,      UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    [SPORT_TYPE_YOGA]         = {STR_SPORT_YOGA,          UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    [SPORT_TYPE_SIT_UP]       = {STR_SPORT_SIT_UP,        UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    [SPORT_TYPE_TREADMILL]    = {STR_SPORT_TREADMILL,     UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    [SPORT_TYPE_GYMNASTICS]   = {STR_SPORT_GYMNASTICS,    UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    [SPORT_TYPE_BOATING]      = {STR_SPORT_BOATING,       UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    [SPORT_TYPE_JUMPING_JACK] = {STR_SPORT_JUMPING_JACK,  UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM]  =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {.vidx = SPORT_TYPE_RUNNING,         .str_idx = STR_SPORT_RUN,           .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {.vidx = SPORT_TYPE_RIDE_BIKE,       .str_idx = STR_SPORT_RIDE_BIKE,     .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    {.vidx = SPORT_TYPE_JUMP_ROPE,       .str_idx = STR_SPORT_JUMP_ROPE,     .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {.vidx = SPORT_TYPE_SWIMMING,        .str_idx = STR_SPORT_SWIMMING,      .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    {.vidx = SPORT_TYPE_BADMINTON,       .str_idx = STR_SPORT_BADMINTON,     .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_005BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TABLE_TENNIS,    .str_idx = STR_SPORT_TABLE_TENNIS,  .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TENNIS,          .str_idx = STR_SPORT_TENNIS,        .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    {.vidx = SPORT_TYPE_CLIMBING,        .str_idx = STR_SPORT_CLIMBING,      .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    {.vidx = SPORT_TYPE_WALKING,         .str_idx = STR_SPORT_WALKING,       .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASKETBALL,      .str_idx = STR_SPORT_BASKETBALL,    .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    {.vidx = SPORT_TYPE_FOOTBALL,        .str_idx = STR_SPORT_FOOTBALL,      .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASEBALL,        .str_idx = STR_SPORT_BASEBALL,      .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    {.vidx = SPORT_TYPE_VOLLEYBALL,      .str_idx = STR_SPORT_VOLLEYBALL,    .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    {.vidx = SPORT_TYPE_CRICKET,         .str_idx = STR_SPORT_CRICKET,       .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    {.vidx = SPORT_TYPE_RUGBY,           .str_idx = STR_SPORT_RUGBY,         .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    {.vidx = SPORT_TYPE_HOCKEY,          .str_idx = STR_SPORT_HOCKEY,        .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    {.vidx = SPORT_TYPE_DANCE,           .str_idx = STR_SPORT_DANCE,         .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    {.vidx = SPORT_TYPE_SPINNING,        .str_idx = STR_SPORT_SPINNING,      .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    {.vidx = SPORT_TYPE_YOGA,            .str_idx = STR_SPORT_YOGA,          .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    {.vidx = SPORT_TYPE_SIT_UP,          .str_idx = STR_SPORT_SIT_UP,        .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    {.vidx = SPORT_TYPE_TREADMILL,       .str_idx = STR_SPORT_TREADMILL,     .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    {.vidx = SPORT_TYPE_GYMNASTICS,      .str_idx = STR_SPORT_GYMNASTICS,    .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    {.vidx = SPORT_TYPE_BOATING,         .str_idx = STR_SPORT_BOATING,       .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    {.vidx = SPORT_TYPE_JUMPING_JACK,    .str_idx = STR_SPORT_JUMPING_JACK,  .res_addr = UI_BUF_I338001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
};

static compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_MAX_SPORT_NUM];

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SPORTS]);
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_MENU_CIRCLE);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    compo_listbox_set(listbox, tbl_sport_list_sort, uteModuleSportGetHundredSportValidNumber());
#else
    compo_listbox_set(listbox, tbl_sport_list_sort, UTE_MODULE_SPORT_MAX_SPORT_NUM);
#endif
    compo_listbox_set_bgimg(listbox, UI_BUF_I338001_11_CALL_BG_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
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

#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
    [SPORT_TYPE_RUNNING]      = {STR_SPORT_RUN,           UI_BUF_I340001_SPORT_ICON_00_PAOBU_BIN,          },
    [SPORT_TYPE_RIDE_BIKE]    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I340001_SPORT_ICON_01_QIXING_BIN,         },
    [SPORT_TYPE_JUMP_ROPE]    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I340001_SPORT_ICON_03_TIAOSHEN_BIN,       },
    [SPORT_TYPE_SWIMMING]     = {STR_SPORT_SWIMMING,      UI_BUF_I340001_SPORT_ICON_04_YOUYONG_BIN,        },
    [SPORT_TYPE_BADMINTON]    = {STR_SPORT_BADMINTON,     UI_BUF_I340001_SPORT_ICON_05_YUMAOQIU_BIN,       },
    [SPORT_TYPE_TABLE_TENNIS] = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I340001_SPORT_ICON_06_PINGPANGQIU_BIN,    },
    [SPORT_TYPE_TENNIS]       = {STR_SPORT_TENNIS,        UI_BUF_I340001_SPORT_ICON_07_WANGQIU_BIN,        },
    [SPORT_TYPE_CLIMBING]     = {STR_SPORT_CLIMBING,      UI_BUF_I340001_SPORT_ICON_08_DENGSHAN_BIN,       },
    [SPORT_TYPE_WALKING]      = {STR_SPORT_WALKING,       UI_BUF_I340001_SPORT_ICON_02_JIANZOU_BIN,        },
    [SPORT_TYPE_BASKETBALL]   = {STR_SPORT_BASKETBALL,    UI_BUF_I340001_SPORT_ICON_09_LANQIU_BIN,         },
    [SPORT_TYPE_FOOTBALL]     = {STR_SPORT_FOOTBALL,      UI_BUF_I340001_SPORT_ICON_10_ZUQIU_BIN,          },
    [SPORT_TYPE_BASEBALL]     = {STR_SPORT_BASEBALL,      UI_BUF_I340001_SPORT_ICON_11_BANGQIU_BIN,        },
    [SPORT_TYPE_VOLLEYBALL]   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I340001_SPORT_ICON_12_PAIQIU_BIN,         },
    [SPORT_TYPE_CRICKET]      = {STR_SPORT_CRICKET,       UI_BUF_I340001_SPORT_ICON_13_BANQIU_BIN,         },
    [SPORT_TYPE_RUGBY]        = {STR_SPORT_RUGBY,         UI_BUF_I340001_SPORT_ICON_14_GANLANQIU_BIN,      },
    [SPORT_TYPE_HOCKEY]       = {STR_SPORT_HOCKEY,        UI_BUF_I340001_SPORT_ICON_15_QUGUNQIU_BIN,       },
    [SPORT_TYPE_DANCE]        = {STR_SPORT_DANCE,         UI_BUF_I340001_SPORT_ICON_16_TIAOWU_BIN,         },
    [SPORT_TYPE_SPINNING]     = {STR_SPORT_SPINNING,      UI_BUF_I340001_SPORT_ICON_17_DONGGANDANCHE_BIN,  },
    [SPORT_TYPE_YOGA]         = {STR_SPORT_YOGA,          UI_BUF_I340001_SPORT_ICON_18_YUJIA_BIN,          },
    [SPORT_TYPE_SIT_UP]       = {STR_SPORT_SIT_UP,        UI_BUF_I340001_SPORT_ICON_19_YANGWOQIZUO_BIN,    },
    [SPORT_TYPE_TREADMILL]    = {STR_SPORT_TREADMILL,     UI_BUF_I340001_SPORT_ICON_20_SHINEIPAO_BIN,      },
    [SPORT_TYPE_GYMNASTICS]   = {STR_SPORT_GYMNASTICS,    UI_BUF_I340001_SPORT_ICON_21_TICAO_BIN,          },
    [SPORT_TYPE_BOATING]      = {STR_SPORT_BOATING,       UI_BUF_I340001_SPORT_ICON_22_HUACHUAN_BIN,       },
    [SPORT_TYPE_JUMPING_JACK] = {STR_SPORT_JUMPING_JACK,  UI_BUF_I340001_SPORT_ICON_23_KAIHETIAO_BIN,      },
    [SPORT_TYPE_FREE_TRAINING]= {STR_SPORT_FREE_TRAINING, UI_BUF_I340001_SPORT_ICON_24_ZIYOUXUNLIAN_BIN,   },
    [SPORT_TYPE_MAX]          = {STR_MORE,                UI_BUF_I340001_SPORT_ICON_25_CAIDAN_BIN,         },
};

enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {STR_SPORT_RUN,           UI_BUF_I340001_SPORT_ICON_00_PAOBU_BIN,             .vidx = SPORT_TYPE_RUNNING       },
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {STR_SPORT_RIDE_BIKE,     UI_BUF_I340001_SPORT_ICON_01_QIXING_BIN,            .vidx = SPORT_TYPE_RIDE_BIKE     },
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {STR_SPORT_JUMP_ROPE,     UI_BUF_I340001_SPORT_ICON_03_TIAOSHEN_BIN,          .vidx = SPORT_TYPE_JUMP_ROPE     },
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {STR_SPORT_SWIMMING,      UI_BUF_I340001_SPORT_ICON_04_YOUYONG_BIN,           .vidx = SPORT_TYPE_SWIMMING      },
#endif
    {STR_SPORT_BADMINTON,     UI_BUF_I340001_SPORT_ICON_05_YUMAOQIU_BIN,          .vidx = SPORT_TYPE_BADMINTON     },
    {STR_SPORT_TABLE_TENNIS,  UI_BUF_I340001_SPORT_ICON_06_PINGPANGQIU_BIN,       .vidx = SPORT_TYPE_TABLE_TENNIS  },
    {STR_SPORT_TENNIS,        UI_BUF_I340001_SPORT_ICON_07_WANGQIU_BIN,           .vidx = SPORT_TYPE_TENNIS        },
    {STR_SPORT_CLIMBING,      UI_BUF_I340001_SPORT_ICON_08_DENGSHAN_BIN,          .vidx = SPORT_TYPE_CLIMBING      },
    {STR_SPORT_WALKING,       UI_BUF_I340001_SPORT_ICON_02_JIANZOU_BIN,           .vidx = SPORT_TYPE_WALKING       },
    {STR_SPORT_BASKETBALL,    UI_BUF_I340001_SPORT_ICON_09_LANQIU_BIN,            .vidx = SPORT_TYPE_BASKETBALL    },
    {STR_SPORT_FOOTBALL,      UI_BUF_I340001_SPORT_ICON_10_ZUQIU_BIN,             .vidx = SPORT_TYPE_FOOTBALL      },
    {STR_SPORT_BASEBALL,      UI_BUF_I340001_SPORT_ICON_11_BANGQIU_BIN,           .vidx = SPORT_TYPE_BASEBALL      },
    {STR_SPORT_VOLLEYBALL,    UI_BUF_I340001_SPORT_ICON_12_PAIQIU_BIN,            .vidx = SPORT_TYPE_VOLLEYBALL    },
    {STR_SPORT_CRICKET,       UI_BUF_I340001_SPORT_ICON_13_BANQIU_BIN,            .vidx = SPORT_TYPE_CRICKET       },
    {STR_SPORT_RUGBY,         UI_BUF_I340001_SPORT_ICON_14_GANLANQIU_BIN,         .vidx = SPORT_TYPE_RUGBY         },
    {STR_SPORT_HOCKEY,        UI_BUF_I340001_SPORT_ICON_15_QUGUNQIU_BIN,          .vidx = SPORT_TYPE_HOCKEY        },
    {STR_SPORT_DANCE,         UI_BUF_I340001_SPORT_ICON_16_TIAOWU_BIN,            .vidx = SPORT_TYPE_DANCE         },
    {STR_SPORT_SPINNING,      UI_BUF_I340001_SPORT_ICON_17_DONGGANDANCHE_BIN,     .vidx = SPORT_TYPE_SPINNING      },
    {STR_SPORT_YOGA,          UI_BUF_I340001_SPORT_ICON_18_YUJIA_BIN,             .vidx = SPORT_TYPE_YOGA          },
    {STR_SPORT_SIT_UP,        UI_BUF_I340001_SPORT_ICON_19_YANGWOQIZUO_BIN,       .vidx = SPORT_TYPE_SIT_UP        },
    {STR_SPORT_TREADMILL,     UI_BUF_I340001_SPORT_ICON_20_SHINEIPAO_BIN,         .vidx = SPORT_TYPE_TREADMILL     },
    {STR_SPORT_GYMNASTICS,    UI_BUF_I340001_SPORT_ICON_21_TICAO_BIN,             .vidx = SPORT_TYPE_GYMNASTICS    },
    {STR_SPORT_BOATING,       UI_BUF_I340001_SPORT_ICON_22_HUACHUAN_BIN,          .vidx = SPORT_TYPE_BOATING       },
    {STR_SPORT_JUMPING_JACK,  UI_BUF_I340001_SPORT_ICON_23_KAIHETIAO_BIN,         .vidx = SPORT_TYPE_JUMPING_JACK  },
    {STR_SPORT_FREE_TRAINING, UI_BUF_I340001_SPORT_ICON_24_ZIYOUXUNLIAN_BIN,      .vidx = SPORT_TYPE_FREE_TRAINING },
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

    compo_listbox_set_icon_area(listbox, gui_image_get_size(UI_BUF_I340001_SPORT_ICON_19_YANGWOQIZUO_BIN));
    compo_listbox_set_bgimg(listbox, UI_BUF_I340001_SPORT_CARD_BIN);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 0);
    //运动列表，当图标的高度大于背景的2/3时，在图标的顶部会有一部分显示不出来
    compo_listbox_set_item_icon_pos(listbox,120*2/3,120/2+2);
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

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    [SPORT_TYPE_RUNNING]      = {STR_SPORT_RUN,           UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    [SPORT_TYPE_RIDE_BIKE]    = {STR_SPORT_RIDE_BIKE,     UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    [SPORT_TYPE_JUMP_ROPE]    = {STR_SPORT_JUMP_ROPE,     UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    [SPORT_TYPE_SWIMMING]     = {STR_SPORT_JUMP_ROPE,     UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    [SPORT_TYPE_BADMINTON]    = {STR_SPORT_BADMINTON,     UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_005BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    [SPORT_TYPE_TABLE_TENNIS] = {STR_SPORT_TABLE_TENNIS,  UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    [SPORT_TYPE_TENNIS]       = {STR_SPORT_TENNIS,        UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    [SPORT_TYPE_CLIMBING]     = {STR_SPORT_CLIMBING,      UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    [SPORT_TYPE_WALKING]      = {STR_SPORT_WALKING,       UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    [SPORT_TYPE_BASKETBALL]   = {STR_SPORT_BASKETBALL,    UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    [SPORT_TYPE_FOOTBALL]     = {STR_SPORT_FOOTBALL,      UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    [SPORT_TYPE_BASEBALL]     = {STR_SPORT_BASEBALL,      UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    [SPORT_TYPE_VOLLEYBALL]   = {STR_SPORT_VOLLEYBALL,    UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    [SPORT_TYPE_CRICKET]      = {STR_SPORT_CRICKET,       UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    [SPORT_TYPE_RUGBY]        = {STR_SPORT_RUGBY,         UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    [SPORT_TYPE_HOCKEY]       = {STR_SPORT_HOCKEY,        UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    [SPORT_TYPE_DANCE]        = {STR_SPORT_DANCE,         UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    [SPORT_TYPE_SPINNING]     = {STR_SPORT_SPINNING,      UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    [SPORT_TYPE_YOGA]         = {STR_SPORT_YOGA,          UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    [SPORT_TYPE_SIT_UP]       = {STR_SPORT_SIT_UP,        UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    [SPORT_TYPE_TREADMILL]    = {STR_SPORT_TREADMILL,     UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    [SPORT_TYPE_GYMNASTICS]   = {STR_SPORT_GYMNASTICS,    UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    [SPORT_TYPE_BOATING]      = {STR_SPORT_BOATING,       UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    [SPORT_TYPE_JUMPING_JACK] = {STR_SPORT_JUMPING_JACK,  UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
};
enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM]  =
{
#if UTE_MODULE_SPORT_RUNNING_SUPPORT
    {.vidx = SPORT_TYPE_RUNNING,         .str_idx = STR_SPORT_RUN,            .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_001OUTDOOR_RUNNING_BIN},
#endif
#if UTE_MODULE_SPORT_BIKE_SUPPORT
    {.vidx = SPORT_TYPE_RIDE_BIKE,       .str_idx = STR_SPORT_RIDE_BIKE,     .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_002OUTDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMP_ROPE_SUPPORT
    {.vidx = SPORT_TYPE_JUMP_ROPE,       .str_idx = STR_SPORT_JUMP_ROPE,     .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_003SKIPPING_BIN},
#endif
#if UTE_MODULE_SPORT_SWIMMING_SUPPORT
    {.vidx = SPORT_TYPE_SWIMMING,        .str_idx = STR_SPORT_SWIMMING,     .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_004SWIMMING_BIN},
#endif
#if UTE_MODULE_SPORT_BADMINTION_SUPPORT
    {.vidx = SPORT_TYPE_BADMINTON,       .str_idx = STR_SPORT_BADMINTON,     .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_005BADMINTON_BIN},
#endif
#if UTE_MODULE_SPORT_TABLE_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TABLE_TENNIS,    .str_idx = STR_SPORT_TABLE_TENNIS,  .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_006TABLE_TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_TENNIS_SUPPORT
    {.vidx = SPORT_TYPE_TENNIS,          .str_idx = STR_SPORT_TENNIS,        .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_007TENNIS_BIN},
#endif
#if UTE_MODULE_SPORT_CLIMBING_SUPPORT
    {.vidx = SPORT_TYPE_CLIMBING,        .str_idx = STR_SPORT_CLIMBING,      .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_008CLIMBING_BIN},
#endif
#if UTE_MODULE_SPORT_WALKING_SUPPORT
    {.vidx = SPORT_TYPE_WALKING,         .str_idx = STR_SPORT_WALKING,       .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_009HIKING_BIN},
#endif
#if UTE_MODULE_SPORT_BASKETBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASKETBALL,      .str_idx = STR_SPORT_BASKETBALL,    .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_010BASKETBALL_BIN},
#endif
#if UTE_MODULE_SPORT_FOOTBALL_SUPPORT
    {.vidx = SPORT_TYPE_FOOTBALL,        .str_idx = STR_SPORT_FOOTBALL,      .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_011SOCCER_BIN},
#endif
#if UTE_MODULE_SPORT_BASEBALL_SUPPORT
    {.vidx = SPORT_TYPE_BASEBALL,        .str_idx = STR_SPORT_BASEBALL,      .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_012BASEBALL_BIN},
#endif
#if UTE_MODULE_SPORT_VOLLEYBALL_SUPPORT
    {.vidx = SPORT_TYPE_VOLLEYBALL,      .str_idx = STR_SPORT_VOLLEYBALL,    .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_013VOLLEYBALL_BIN},
#endif
#if UTE_MODULE_SPORT_CRICKET_SUPPORT
    {.vidx = SPORT_TYPE_CRICKET,         .str_idx = STR_SPORT_CRICKET,       .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_014CRICKET_BIN},
#endif
#if UTE_MODULE_SPORT_RUGBY_SUPPORT
    {.vidx = SPORT_TYPE_RUGBY,           .str_idx = STR_SPORT_RUGBY,         .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_015RUGBY_BIN},
#endif
#if UTE_MODULE_SPORT_HOCKEY_SUPPORT
    {.vidx = SPORT_TYPE_HOCKEY,          .str_idx = STR_SPORT_HOCKEY,        .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_016HOCKEY_BIN},
#endif
#if UTE_MODULE_SPORT_DANCE_SUPPORT
    {.vidx = SPORT_TYPE_DANCE,           .str_idx = STR_SPORT_DANCE,         .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_017DANCE_BIN},
#endif
#if UTE_MODULE_SPORT_SPINNING_SUPPORT
    {.vidx = SPORT_TYPE_SPINNING,        .str_idx = STR_SPORT_SPINNING,      .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_018INDOOR_CYCLING_BIN},
#endif
#if UTE_MODULE_SPORT_YOGA_SUPPORT
    {.vidx = SPORT_TYPE_YOGA,            .str_idx = STR_SPORT_YOGA,          .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_019YOGA_BIN},
#endif
#if UTE_MODULE_SPORT_SIT_UP_SUPPORT
    {.vidx = SPORT_TYPE_SIT_UP,          .str_idx = STR_SPORT_SIT_UP,        .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_020SIT_UPS_BIN},
#endif
#if UTE_MODULE_SPORT_TREADMILL_SUPPORT
    {.vidx = SPORT_TYPE_TREADMILL,       .str_idx = STR_SPORT_TREADMILL,     .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_021TREADMILL_BIN},
#endif
#if UTE_MODULE_SPORT_GYMNASTICS_SUPPORT
    {.vidx = SPORT_TYPE_GYMNASTICS,      .str_idx = STR_SPORT_GYMNASTICS,    .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_022GYMNASTICS_BIN},
#endif
#if UTE_MODULE_SPORT_BOATING_SUPPORT
    {.vidx = SPORT_TYPE_BOATING,         .str_idx = STR_SPORT_BOATING,       .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_023ROWING_BIN},
#endif
#if UTE_MODULE_SPORT_JUMPING_JACK_SUPPORT
    {.vidx = SPORT_TYPE_JUMPING_JACK,    .str_idx = STR_SPORT_JUMPING_JACK,  .res_addr = UI_BUF_I342001_3_EXERCISE_1_LIST_ICON_024JUMPING_JACK_BIN},
#endif
};

static compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_MAX_SPORT_NUM];

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_SPORTS]);
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_MENU_CIRCLE);
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    compo_listbox_set(listbox, tbl_sport_list_sort, uteModuleSportGetHundredSportValidNumber());
#else
    compo_listbox_set(listbox, tbl_sport_list_sort, UTE_MODULE_SPORT_MAX_SPORT_NUM);
#endif
//    compo_listbox_set_bgimg(listbox, UI_BUF_I335001_3_EXERCISE_1_LIST_BG_ICON_224X68_X8_Y50_Y126_Y202_BIN);
    compo_listbox_set_item_height(listbox, 55);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 1);
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


#else
#define MENU_LIST_CNT                       ((int)(sizeof(tbl_sport_list_data) / sizeof(tbl_sport_list_data[0])))
static const compo_listbox_item_t tbl_sport_list_data[] =
{
    {0},
};

enum//对应运动中显示运动数据种类->不同项目可自行添加
{
    MULTIPLE_DATA=0,//多数据
    MID_DATA,       //中数据
    LESS_DATA,      //少数据
};

u32 func_sport_get_disp_mode(void)//对应运动中显示运动数据种类->不同项目可自行添加->用于运动中与运动结束
{
    switch(sys_cb.sport_idx)
    {
        case SPORT_TYPE_RUNNING:
        case SPORT_TYPE_CLIMBING:
        case SPORT_TYPE_WALKING:
        case SPORT_TYPE_TREADMILL:
            return MULTIPLE_DATA;
        case SPORT_TYPE_JUMP_ROPE://跳绳
        case SPORT_TYPE_SWIMMING://游泳
            return MID_DATA;
        default:
            return LESS_DATA;
    }
}

u32 func_sport_get_str(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].str_idx;
    }
    return 0;
}

u32 func_sport_get_ui(u8 sport_idx)
{
    if (sport_idx < MENU_LIST_CNT)
    {
        return tbl_sport_list_data[sport_idx].res_addr;
    }
    return 0;
}

static const compo_listbox_item_t tbl_sport_list[UTE_MODULE_SPORT_MAX_SPORT_NUM] =
{
    {0},
};

static compo_listbox_item_t tbl_sport_list_sort[UTE_MODULE_SPORT_MAX_SPORT_NUM];

//创建运动窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sport_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //新建菜单列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_CUM_SPORT_LIST);
    compo_listbox_set(listbox, tbl_sport_list_sort, 2);
    compo_listbox_set_bgimg(listbox, 0);
    compo_setid(listbox, COMPO_ID_LISTBOX);

    compo_listbox_set_focus_byidx(listbox, 0);
    compo_listbox_update(listbox);
    return frm;
}

//点进图标进入应用
static void func_sport_list_icon_click(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
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
    memset(sportList, 0, sizeof(compo_listbox_item_t) * UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM);

    // 根据sportOrder重新排序
    for (uint8_t i = 0; i < UTE_MODULE_SPORT_MAX_SPORT_NUM; i++)
    {
        uint8_t sportOrder = uteModuleSportFindHundredSportIndexByMode(tbl_sport_list[i].vidx);
        if (sportOrder > 0 && sportOrder <= UTE_MODULE_SPORT_DISPLAY_MAX_SPORT_NUM)
        {
            sportList[sportOrder - 1] = tbl_sport_list[i];
            //printf("real sportList[%d] = %d\n",sportOrder - 1,sportList[sportOrder - 1].vidx);
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

//获取列表是否需要重新排序
bool func_sport_list_get_need_reorder_flag(void)
{
    return (uteModuleSportGetAppUpdateTrainingListFlag() || !func_sport_list_init_flag);
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
    printf( "%s,success, sport_order %d: str_idx=%d, icon_addr=0x%x", __func__, sport_order, *str_idx, *icon_addr);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,success, sport_order %d: str_idx=%d, icon_addr=0x%x", __func__, sport_order, *str_idx, *icon_addr);
    return true;
}

#endif
#else
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
    printf( "%s,success, sport_order %d: str_idx=%d, icon_addr=0x%x", __func__, sport_order, *str_idx, *icon_addr);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,success, sport_order %d: str_idx=%d, icon_addr=0x%x", __func__, sport_order, *str_idx, *icon_addr);
    return true;
}

#endif
#endif //GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
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
    if(f_sport->page == SPORT_MAIN_PAGE)
    {
        for(u8 i =0; i < SPORT_MAIN_NUM; i++)
        {
            widget_set_size(f_sport->listbox->item_icon[i],84, 84);
        }
        compo_listbox_move(f_sport->listbox);
        compo_shape_t* shape  = ute_compo_getobj_byid_for_btn(COMPO_ID_SHAPE_MORE);
        compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_PIC_MORE);
        compo_textbox_t* textbox =  compo_getobj_byid(COMPO_ID_TXT_MORE);
        area_t textbox_css = widget_text_get_area(textbox->txt);

        if(compo_listbox_gety_byidx(f_sport->listbox,SPORT_MAIN_NUM - 1) - 100  < f_sport->listbox->ofs_y)
        {
            int move_y = compo_listbox_gety_byidx(f_sport->listbox, SPORT_MAIN_NUM - 1);
            move_y = move_y - f_sport->listbox->ofs_y - 50;
            compo_shape_set_pos(shape, GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT + move_y);
            compo_shape_set_visible(shape,true);
            int x = (GUI_SCREEN_CENTER_X - 10 - textbox_css.wid / 2 > 60) ? (GUI_SCREEN_CENTER_X - 10 - textbox_css.wid / 2) : 60;
            compo_picturebox_set_pos(pic,x,GUI_SCREEN_HEIGHT + move_y);
            compo_picturebox_set_visible(pic,true);
            compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X + 20,GUI_SCREEN_HEIGHT + move_y,200,46);
            compo_textbox_set_visible(textbox,true);
        }
        else
        {
            compo_shape_set_visible(shape,false);
            compo_picturebox_set_visible(pic,false);
            compo_textbox_set_visible(textbox,false);
        }
    }
    else
    {
        if(f_sport->count > 11)
        {
            compo_listbox_move(f_sport->listbox);
        }
        if(uteModuleSportGetAppUpdateTrainingListFlag())
        {
            func_sport_list_reorder(tbl_sport_list_sort);
        }

    }
    func_process();
}

//运动功能消息处理
static void func_sport_message(size_msg_t msg)
{
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_sport->listbox;

    if(f_sport->count > 11 || (f_sport->page == SPORT_MAIN_PAGE))
    {
        if (compo_listbox_message(listbox, msg))
        {
            return;                                         //处理列表框信息
        }
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_sport_list_icon_click();                //单击图标
            break;

        case MSG_CTP_LONG:
            break;
//        case MSG_CTP_SHORT_LEFT:
//            break;
        case MSG_CTP_SHORT_RIGHT:
            if(f_sport->page == SPORT_MORE_PAGE)
            {
                func_free(listbox->mcb);
                func_sport_list_reorder(tbl_sport_list_sort);
                compo_form_destroy(func_cb.frm_main);
                func_cb.frm_main = func_sport_form_create();
                f_sport->page = SPORT_MAIN_PAGE;
                f_sport->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
                listbox = f_sport->listbox;
                listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
                compo_listbox_move_init(listbox);
                compo_listbox_move_init_modify(listbox, 80, compo_listbox_gety_byidx(listbox, 8-1) + 100);
                return;
            }
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
        case KU_BACK:
            if(f_sport->page == SPORT_MORE_PAGE)
            {
                func_free(listbox->mcb);
                func_sport_list_reorder(tbl_sport_list_sort);
                compo_form_destroy(func_cb.frm_main);
                func_cb.frm_main = func_sport_form_create();
                f_sport->page = SPORT_MAIN_PAGE;
                f_sport->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
                listbox = f_sport->listbox;
                listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
                compo_listbox_move_init(listbox);
                compo_listbox_move_init_modify(listbox, 80, compo_listbox_gety_byidx(listbox, 8-1) + 100);
                return;
            }
            else
            {
                func_message(msg);
                return;
            }
            break;

        default:
            func_message(msg);
            break;
    }
}
#else
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
//        case MSG_CTP_SHORT_LEFT:
//            break;
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
#endif
//进入运动功能
static void func_sport_enter(void)
{
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT
    func_sport_list_reorder(tbl_sport_list_sort);
#endif
    func_cb.f_cb = func_zalloc(sizeof(f_sport_list_t));
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    if(uteModuleSportTargetGetShowIndex()> SPORT_MAIN_NUM && func_cb.last == FUNC_SPORT_SUB_SET )
    {
        func_cb.frm_main = func_sport_more_form_create();
    }
    else
#endif
    {
        func_cb.frm_main = func_sport_form_create();
    }
    f_sport_list_t *f_sport = (f_sport_list_t *)func_cb.f_cb;
    f_sport->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_sport->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
    compo_listbox_move_init(listbox);
    f_sport->count = uteModuleSportGetHundredSportValidNumber();
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    if(uteModuleSportFindHundredSportIndexByMode(sys_cb.sport_idx) > SPORT_MAIN_NUM && func_cb.last == FUNC_SPORT_SUB_SET)
    {
        f_sport->page = SPORT_MORE_PAGE;
        int count = (f_sport->count - SPORT_MAIN_NUM) > 12 ? 12 :(f_sport->count - SPORT_MAIN_NUM);
        compo_listbox_move_init_modify(listbox, compo_listbox_gety_byidx(listbox,1), compo_listbox_gety_byidx(listbox, count-1) - 180);

    }
    else
    {
        f_sport->page = SPORT_MAIN_PAGE;
        compo_listbox_move_init_modify(listbox, 80, compo_listbox_gety_byidx(listbox, 8-1) + 100);
    }
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, uteModuleSportGetHundredSportValidNumber()-2));
#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    compo_listbox_move_init_modify(listbox, 120, compo_listbox_gety_byidx(listbox, uteModuleSportGetHundredSportValidNumber()-2));
#endif
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
