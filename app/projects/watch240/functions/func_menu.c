#include "include.h"
#include "func.h"
#include "func_menu.h"
#include "func_menu_ui_data.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//创建主菜单窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_menu_form_create(void)
{

#if UTE_MODULE_SCREENS_MENU_DATA_BIND
    f_menu_ui_data_init();
#endif
    switch (func_cb.menu_style)
    {
        case MENU_STYLE_CUM_HONEYGRID:
        case MENU_STYLE_HONEYCOMB:
            return func_menu_sub_honeycomb_form_create();

        case MENU_STYLE_WATERFALL:
            return func_menu_sub_waterfall_form_create();

        case MENU_STYLE_FOOTBALL:
            return func_menu_sub_football_form_create();

        case MENU_STYLE_LIST:
        case MENU_STYLE_CUM_FOURGRID:
            return func_menu_sub_list_form_create();

        case MENU_STYLE_SUDOKU:
        case MENU_STYLE_SUDOKU_HRZ:
            return func_menu_sub_sudoku_form_create();

        case MENU_STYLE_GRID:
        case MENU_STYLE_CUM_GRID:
            return func_menu_sub_grid_form_create();

        case MENU_STYLE_DISK:
            return func_menu_sub_disk_form_create();

        case MENU_STYLE_RING:
            return func_menu_sub_ring_form_create();

        case MENU_STYLE_KALE:
            return func_menu_sub_kale_form_create();

        case MENU_STYLE_SKYRER:
            return func_menu_sub_skyrer_form_create();

        case MENU_STYLE_CUM_SUDOKU:
            return func_menu_sub_cum_sudoku_form_create();

        case MENU_STYLE_CUM_HEXAGON:
            return func_menu_sub_hexagon_form_create();
#if UTE_MENU_STYLE_CUM_RING1_FUNCTION
        case MENU_STYLE_CUM_RING1:
            return func_menu_sub_cum_ring1_form_create();
#endif
#if UTE_MENU_STYLE_CUM_RING2_FUNCTION
        case MENU_STYLE_CUM_RING2:
            return func_menu_sub_cum_ring2_form_create();
#endif
        default:
            halt(HALT_FUNC_MENU_SUBSTA);
            return NULL;
    }
}

#if UTE_MENU_STYLE_DOUBLE_NEXT_ENABLE
#define MENU_CNT                       sizeof(SWITCH_NEXT_MENU) / sizeof(SWITCH_NEXT_MENU[0])
const u8 SWITCH_NEXT_MENU[]=UTE_CUI_SCREEN_MENU_STYLE;
static void func_menu_sub_switch_next(void)
{
    int idx;
    for(int idx=0; idx<MENU_CNT; idx++)
    {
        if(SWITCH_NEXT_MENU[idx] == func_cb.menu_style)
        {
            idx++;
            if(idx == MENU_CNT)
            {
                idx = 0;
            }
            func_cb.menu_style = SWITCH_NEXT_MENU[idx];
            if (func_cb.menu_style == MENU_STYLE_SKYRER)
            {
                u8 func_menu_sub_skyrer_get_first_idx(void);
                func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
            }
            else
            {
                func_cb.menu_idx = 0;           //切换风格后进入回中心位置
            }
            uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
            return;
        }
    }
}
#endif

//菜单样式公用消息
void func_menu_sub_message(size_msg_t msg)
{
    switch (msg)
    {
        case KU_BACK:
            //不响应
            break;

        case KD_BACK:
#if UTE_MODULE_SCREENS_ROTARY_MENUSTYLE_SUPPORT
            if (tick_check_expire(func_cb.enter_tick, TICK_IGNORE_KEY))
            {
                func_cb.sta = FUNC_MENUSTYLE;
            }
#elif UTE_MENU_STYLE_DOUBLE_NEXT_ENABLE
            func_menu_sub_switch_next();
#endif
            break;

        default:
            func_message(msg);
            break;
    }
}

//退出菜单样式
void func_menu_sub_exit(void)
{
    compo_form_destroy(func_cb.frm_main);
    func_free(func_cb.f_cb);
    func_cb.frm_main = NULL;
    func_cb.f_cb = NULL;
    tft_set_temode(DEFAULT_TE_MODE);
}


//进入主菜单功能
static void func_menu_enter(void)
{
}

//退出主菜单功能
static void func_menu_exit(void)
{
    func_cb.last = FUNC_MENU;
}

//主菜单功能
void func_menu(void)
{
    printf("%s\n", __func__);
    func_menu_enter();
    while (func_cb.sta == FUNC_MENU)
    {
        printf("func_cb.menu_style:[%d]\n",func_cb.menu_style);
        switch (func_cb.menu_style)
        {
            case MENU_STYLE_CUM_HONEYGRID:
            case MENU_STYLE_HONEYCOMB:
                func_menu_sub_honeycomb();
                break;

            case MENU_STYLE_WATERFALL:
                func_menu_sub_waterfall();
                break;

            case MENU_STYLE_FOOTBALL:
                func_menu_sub_football();
                break;

            case MENU_STYLE_LIST:
            case MENU_STYLE_CUM_FOURGRID:
                func_menu_sub_list();
                break;

            case MENU_STYLE_SUDOKU:
            case MENU_STYLE_SUDOKU_HRZ:
                func_menu_sub_sudoku();
                break;

            case MENU_STYLE_GRID:
            case MENU_STYLE_CUM_GRID:
                func_menu_sub_grid();
                break;

            case MENU_STYLE_DISK:
                func_menu_sub_disk();
                break;

            case MENU_STYLE_RING:
                func_menu_sub_ring();
                break;

            case MENU_STYLE_KALE:
                func_menu_sub_kale();
                break;

            case MENU_STYLE_SKYRER:
                func_menu_sub_skyrer();
                break;

            case MENU_STYLE_CUM_SUDOKU:
                func_menu_sub_cum_sudoku();
                break;

            case MENU_STYLE_CUM_HEXAGON:
                func_menu_sub_hexagon();
                break;
#if UTE_MENU_STYLE_CUM_RING1_FUNCTION
            case MENU_STYLE_CUM_RING1:
                func_menu_sub_cum_ring1();
                break;
#endif
#if UTE_MENU_STYLE_CUM_RING2_FUNCTION
            case MENU_STYLE_CUM_RING2:
                func_menu_sub_cum_ring2();
                break;
#endif
            default:
                halt(HALT_FUNC_MENU_SUBSTA);
                break;
        }
    }
    func_menu_exit();
}
