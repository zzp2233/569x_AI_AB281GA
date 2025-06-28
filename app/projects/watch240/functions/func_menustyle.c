#include "include.h"
#include "func.h"
#include "func_menu.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_ROTARY_MENUSTYLE_SUPPORT

#define TBL_ROTARY_ITEM_CNT                     ((int)(sizeof(tbl_rotary_item_en) / sizeof(tbl_rotary_item_en[0])))

enum
{
    COMPO_ID_ROTARY = 1,
};

typedef struct f_menustyle_t_
{
    compo_rotary_t *rotary;
} f_menustyle_t;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
static const compo_rotary_item_t tbl_rotary_item_en[] =
{
    {UI_BUF_I330001_THEME_PREVIEW_EN_00_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I330001_THEME_PREVIEW_EN_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I330001_THEME_PREVIEW_EN_02_BIN,           STR_HALO},
    {UI_BUF_I330001_THEME_PREVIEW_EN_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I330001_THEME_PREVIEW_EN_04_BIN,           STR_GONG_GE},
    {UI_BUF_I330001_THEME_PREVIEW_EN_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I330001_THEME_PREVIEW_EN_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I330001_THEME_PREVIEW_EN_08_BIN,           STR_STYLE_WATERFALL},
    {UI_BUF_I330001_THEME_PREVIEW_EN_09_BIN,           STR_SPHERE},
};

static const compo_rotary_item_t tbl_rotary_item_zh[] =
{
    {UI_BUF_I330001_THEME_PREVIEW_00_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I330001_THEME_PREVIEW_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I330001_THEME_PREVIEW_02_BIN,           STR_HALO},
    {UI_BUF_I330001_THEME_PREVIEW_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I330001_THEME_PREVIEW_04_BIN,           STR_GONG_GE},
    {UI_BUF_I330001_THEME_PREVIEW_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I330001_THEME_PREVIEW_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I330001_THEME_PREVIEW_08_BIN,           STR_STYLE_WATERFALL},
    {UI_BUF_I330001_THEME_PREVIEW_09_BIN,           STR_SPHERE},
};

static const compo_menu_Mapping_Table[]=  //菜单映射表
{
    MENU_STYLE_LIST,
    MENU_STYLE_GRID,
    MENU_STYLE_KALE,
    MENU_STYLE_SKYRER,
    MENU_STYLE_CUM_SUDOKU,
    MENU_STYLE_CUM_GRID,
    MENU_STYLE_HONEYCOMB,
    MENU_STYLE_WATERFALL,
    MENU_STYLE_FOOTBALL,
};

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
static const compo_rotary_item_t tbl_rotary_item_en[] =
{
    {UI_BUF_I332001_THEME_PREVIEW_EN_00_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I332001_THEME_PREVIEW_EN_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I332001_THEME_PREVIEW_EN_02_BIN,           STR_HALO},
    {UI_BUF_I332001_THEME_PREVIEW_EN_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I332001_THEME_PREVIEW_EN_04_BIN,           STR_GONG_GE},
    {UI_BUF_I332001_THEME_PREVIEW_EN_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I332001_THEME_PREVIEW_EN_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I332001_THEME_PREVIEW_EN_08_BIN,           STR_STYLE_WATERFALL},
    {UI_BUF_I332001_THEME_PREVIEW_EN_09_BIN,           STR_SPHERE},
};
static const compo_rotary_item_t tbl_rotary_item_zh[] =
{
    {UI_BUF_I332001_THEME_PREVIEW_ZH_00_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I332001_THEME_PREVIEW_ZH_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I332001_THEME_PREVIEW_ZH_02_BIN,           STR_HALO},
    {UI_BUF_I332001_THEME_PREVIEW_ZH_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I332001_THEME_PREVIEW_ZH_04_BIN,           STR_GONG_GE},
    {UI_BUF_I332001_THEME_PREVIEW_ZH_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I332001_THEME_PREVIEW_ZH_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I332001_THEME_PREVIEW_ZH_08_BIN,           STR_STYLE_WATERFALL},
    {UI_BUF_I332001_THEME_PREVIEW_ZH_09_BIN,           STR_SPHERE},
};

static const compo_menu_Mapping_Table[]=    //菜单映射表
{
    MENU_STYLE_LIST,
    MENU_STYLE_GRID,
    MENU_STYLE_KALE,
    MENU_STYLE_SKYRER,
    MENU_STYLE_CUM_SUDOKU,
    MENU_STYLE_CUM_GRID,
    MENU_STYLE_HONEYCOMB,
    MENU_STYLE_WATERFALL,
    MENU_STYLE_FOOTBALL,
};
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
static const compo_rotary_item_t tbl_rotary_item_en[] =
{
    {UI_BUF_I340001_THEME_PREVIEW_EN_00_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I340001_THEME_PREVIEW_EN_04_BIN,           STR_GONG_GE},
    {UI_BUF_I340001_THEME_PREVIEW_EN_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I340001_THEME_PREVIEW_EN_09_BIN,           STR_SPHERE},
    {UI_BUF_I340001_THEME_PREVIEW_EN_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I340001_THEME_PREVIEW_EN_02_BIN,           STR_HALO},
    {UI_BUF_I340001_THEME_PREVIEW_EN_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I340001_THEME_PREVIEW_EN_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I340001_THEME_PREVIEW_EN_08_BIN,           STR_STYLE_WATERFALL},
};
static const compo_rotary_item_t tbl_rotary_item_zh[] =
{
    {UI_BUF_I340001_THEME_PREVIEW_ZH_00_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I340001_THEME_PREVIEW_ZH_04_BIN,           STR_GONG_GE},
    {UI_BUF_I340001_THEME_PREVIEW_ZH_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I340001_THEME_PREVIEW_ZH_09_BIN,           STR_SPHERE},
    {UI_BUF_I340001_THEME_PREVIEW_ZH_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I340001_THEME_PREVIEW_ZH_02_BIN,           STR_HALO},
    {UI_BUF_I340001_THEME_PREVIEW_ZH_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I340001_THEME_PREVIEW_ZH_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I340001_THEME_PREVIEW_ZH_08_BIN,           STR_STYLE_WATERFALL},
};

static const compo_menu_Mapping_Table[]=    //菜单映射表
{
    MENU_STYLE_LIST,
    MENU_STYLE_CUM_SUDOKU,
    MENU_STYLE_HONEYCOMB,
    MENU_STYLE_FOOTBALL,
    MENU_STYLE_GRID,
    MENU_STYLE_KALE,
    MENU_STYLE_SKYRER,
    MENU_STYLE_CUM_GRID,
    MENU_STYLE_WATERFALL,
};
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
static const compo_rotary_item_t tbl_rotary_item_en[] =
{
    {UI_BUF_I335001_THEME_PREVIEW_00_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I335001_THEME_PREVIEW_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I335001_THEME_PREVIEW_02_BIN,           STR_HALO},
    {UI_BUF_I335001_THEME_PREVIEW_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I335001_THEME_PREVIEW_04_BIN,           STR_GONG_GE},
    {UI_BUF_I335001_THEME_PREVIEW_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I335001_THEME_PREVIEW_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I335001_THEME_PREVIEW_08_BIN,           STR_STYLE_WATERFALL},
    {UI_BUF_I335001_THEME_PREVIEW_09_BIN,           STR_SPHERE},
};
static const compo_rotary_item_t tbl_rotary_item_zh[] =
{
    {UI_BUF_I335001_THEME_PREVIEW_10_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I335001_THEME_PREVIEW_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I335001_THEME_PREVIEW_02_BIN,           STR_HALO},
    {UI_BUF_I335001_THEME_PREVIEW_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I335001_THEME_PREVIEW_04_BIN,           STR_GONG_GE},
    {UI_BUF_I335001_THEME_PREVIEW_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I335001_THEME_PREVIEW_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I335001_THEME_PREVIEW_08_BIN,           STR_STYLE_WATERFALL},
    {UI_BUF_I335001_THEME_PREVIEW_09_BIN,           STR_SPHERE},
};

static const compo_menu_Mapping_Table[]=    //菜单映射表
{
    MENU_STYLE_LIST,
    MENU_STYLE_GRID,
    MENU_STYLE_KALE,
    MENU_STYLE_SKYRER,
    MENU_STYLE_SUDOKU,
    MENU_STYLE_CUM_GRID,
    MENU_STYLE_HONEYCOMB,
    MENU_STYLE_WATERFALL,
    MENU_STYLE_FOOTBALL,
};

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
static const compo_rotary_item_t tbl_rotary_item_en[] =
{
    {UI_BUF_I335001_THEME_PREVIEW_00_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I335001_THEME_PREVIEW_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I335001_THEME_PREVIEW_02_BIN,           STR_HALO},
    {UI_BUF_I335001_THEME_PREVIEW_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I335001_THEME_PREVIEW_04_BIN,           STR_GONG_GE},
    {UI_BUF_I335001_THEME_PREVIEW_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I335001_THEME_PREVIEW_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I335001_THEME_PREVIEW_08_BIN,           STR_STYLE_WATERFALL},
    {UI_BUF_I335001_THEME_PREVIEW_09_BIN,           STR_SPHERE},
};
static const compo_rotary_item_t tbl_rotary_item_zh[] =
{
    {UI_BUF_I335001_THEME_PREVIEW_10_BIN,           STR_STYLE_LIST_1},
    {UI_BUF_I335001_THEME_PREVIEW_01_BIN,           STR_CHECKERBOARD},
    {UI_BUF_I335001_THEME_PREVIEW_02_BIN,           STR_HALO},
    {UI_BUF_I335001_THEME_PREVIEW_03_BIN,           STR_STYLE_SKYRER},
    {UI_BUF_I335001_THEME_PREVIEW_04_BIN,           STR_GONG_GE},
    {UI_BUF_I335001_THEME_PREVIEW_05_BIN,           STR_STYLE_GRID_1},
    {UI_BUF_I335001_THEME_PREVIEW_07_BIN,           STR_STYLE_HONEYCOMB},
    {UI_BUF_I335001_THEME_PREVIEW_08_BIN,           STR_STYLE_WATERFALL},
    {UI_BUF_I335001_THEME_PREVIEW_09_BIN,           STR_SPHERE},
};

static const compo_menu_Mapping_Table[]=    //菜单映射表
{
    MENU_STYLE_LIST,
    MENU_STYLE_GRID,
    MENU_STYLE_KALE,
    MENU_STYLE_SKYRER,
    MENU_STYLE_SUDOKU,
    MENU_STYLE_CUM_GRID,
    MENU_STYLE_HONEYCOMB,
    MENU_STYLE_WATERFALL,
    MENU_STYLE_FOOTBALL,
};

#else
static const compo_rotary_item_t tbl_rotary_item_en[] =
{
    [0]               = {0,           0},
};
static const compo_rotary_item_t tbl_rotary_item_zh[] =
{
    [0]               = {0,           0},
};

static const compo_menu_Mapping_Table[]=    //菜单映射表
{
    0,
};
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建转盘，创建窗体中不要使用功能结构体 func_cb.f_cb
static compo_form_t *func_menustyle_rotary_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    //创建背景
    compo_shape_t *rect = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect, make_color(24, 24, 32));
    compo_shape_set_location(rect, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);

    //创建转盘
    compo_rotary_t *rotary;
    if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
    {
        rotary = compo_rotary_create(frm, tbl_rotary_item_zh,TBL_ROTARY_ITEM_CNT);
    }
    else
    {
        rotary = compo_rotary_create(frm, tbl_rotary_item_en,TBL_ROTARY_ITEM_CNT);
    }
    compo_setid(rotary, COMPO_ID_ROTARY);
    compo_rotary_update(rotary);
    return frm;
}

//主菜单样式列表事件处理
static void func_menustyle_process(void)
{
    f_menustyle_t *f_menustyle = (f_menustyle_t *)func_cb.f_cb;
    compo_rotary_t *rotary = f_menustyle->rotary;
    compo_rotary_move(rotary);
    func_process();
    if (compo_rotary_get_sta(rotary) == COMPO_ROTARY_STA_EXIT)
    {
        func_cb.sta = FUNC_MENU;
    }
}

//主菜单样式列表消息处理
static void func_menustyle_message(size_msg_t msg)
{
    f_menustyle_t *f_menustyle = (f_menustyle_t *)func_cb.f_cb;
    compo_rotary_t *rotary = f_menustyle->rotary;

    switch (msg)
    {
        case KU_BACK:
            func_cb.menu_style = compo_menu_Mapping_Table[compo_rotary_get_idx(rotary)];
            uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
            break;

        default:
            break;
    }
    if (compo_rotary_message(rotary, msg))
    {
        return;                                         //处理列表框信息
    }
    switch (msg)
    {
        case MSG_CTP_CLICK:
            //单击屏幕
            func_cb.menu_style = compo_menu_Mapping_Table[compo_rotary_get_idx(rotary)];
            compo_rotary_move_control(rotary, COMPO_ROTARY_MOVE_CMD_EXITING);
            uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
            break;

        default:
            func_message(msg);
            break;
    }
}

static bool menustyle_refresh = true;
//进入主菜单样式列表
static void func_menustyle_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_menustyle_t));
    func_cb.frm_main = func_menustyle_rotary_form_create();

    tft_set_temode(2);
    f_menustyle_t *f_menustyle = (f_menustyle_t *)func_cb.f_cb;
    f_menustyle->rotary = compo_getobj_byid(COMPO_ID_ROTARY);
    compo_rotary_t *rotary = f_menustyle->rotary;
    if (rotary->type != COMPO_TYPE_ROTARY)
    {
        halt(HALT_GUI_COMPO_ROTARY_TYPE);
    }
    // printf("%s: menu_style = %d\n", __func__, func_cb.menu_style);
    // compo_rotary_set_rotation_byidx(rotary, func_cb.menu_style);
    for(int i = 0; i < TBL_ROTARY_ITEM_CNT; i++)
    {
        if (compo_menu_Mapping_Table[i] == func_cb.menu_style)
        {
            compo_rotary_set_rotation_byidx(rotary, i);//设置旋转角度，根据compo_menu_Mapping_Table内容的下标匹配
            break;
        }
    }
    compo_rotary_update(rotary);
    if(menustyle_refresh == true)
    {
        if (func_cb.last == FUNC_MENU)
        {
            compo_rotary_move_control(rotary, COMPO_ROTARY_MOVE_CMD_ENTERING);      //入场
        }
    }
}

//退出主菜单样式列表
static void func_menustyle_exit(void)
{
    menustyle_refresh = !sys_cb.refresh_language_flag;
    tft_set_temode(DEFAULT_TE_MODE);
    task_stack_pop();
}

//主菜单样式列表
void func_menustyle(void)
{
    printf("%s\n", __func__);
    func_menustyle_enter();
    while (func_cb.sta == FUNC_MENUSTYLE)
    {
        func_menustyle_process();
        func_menustyle_message(msg_dequeue());
    }
    func_menustyle_exit();
}

#endif
