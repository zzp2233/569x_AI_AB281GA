#ifndef _FUNC_MENU_UI_DATA_H
#define _FUNC_MENU_UI_DATA_H

#include "include.h"
#if UTE_MODULE_SCREENS_MENU_DATA_BIND

typedef struct f_menu_ui_data_
{
    u8 func_sta;                            //普通菜单模式
    u32 str_idx;                                //文字
    u32 res_addr;                               //图标
    u32 fb_res_addr;                            //图标->球形菜单
} f_menu_ui_data;

#ifndef UTE_MENU_APP_MAX_CNT
#define MENU_APP_MAX_CNT     30
#else
#define MENU_APP_MAX_CNT     UTE_MENU_APP_MAX_CNT
#endif

extern f_menu_ui_data f_menu_data[MENU_APP_MAX_CNT];
#define MENU_APP_CNT                       ((int)(sizeof(f_menu_ui_data_all) / sizeof(f_menu_ui_data_all[0])))

#ifdef UTE_MENU_STYLE_LIST_APP_MAX_NUM/*!  列表式菜单 显示应用最多数量*/
#define MENU_STYLE_LIST_APP_MAX_NUM UTE_MENU_STYLE_LIST_APP_MAX_NUM
#else
#define MENU_STYLE_LIST_APP_MAX_NUM MENU_APP_CNT
#endif
#ifdef UTE_MENU_STYLE_CUM_SUDOKU_APP_MAX_NUM /*! 宫格式菜单 显示应用最多数量  */
#define MENU_STYLE_CUM_SUDOKU_APP_MAX_NUM UTE_MENU_STYLE_CUM_SUDOKU_APP_MAX_NUM
#else
#define MENU_STYLE_CUM_SUDOKU_APP_MAX_NUM MENU_APP_CNT
#endif
#ifdef UTE_MENU_STYLE_HONEYCOMB_APP_MAX_NUM /*! 蜂窝状菜单 显示应用最多数量  */
#define MENU_STYLE_HONEYCOMB_APP_MAX_NUM UTE_MENU_STYLE_HONEYCOMB_APP_MAX_NUM
#else
#define MENU_STYLE_HONEYCOMB_APP_MAX_NUM MENU_APP_CNT
#endif
#ifdef UTE_MENU_STYLE_FOOTBALL_APP_MAX_NUM /*! 蜂窝状菜单 显示应用最多数量  */
#define MENU_STYLE_FOOTBALL_APP_MAX_NUM UTE_MENU_STYLE_FOOTBALL_APP_MAX_NUM
#else
#define MENU_STYLE_FOOTBALL_APP_MAX_NUM MENU_APP_CNT
#endif
#ifdef UTE_MENU_STYLE_GRID_APP_MAX_NUM /*! 棋盘式网格菜单 显示应用最多数量  */
#define MENU_STYLE_GRID_APP_MAX_NUM UTE_MENU_STYLE_GRID_APP_MAX_NUM
#else
#define MENU_STYLE_GRID_APP_MAX_NUM MENU_APP_CNT
#endif
#ifdef UTE_MENU_STYLE_KALE_APP_MAX_NUM /*! 环形光晕菜单 显示应用最多数量  */
#define MENU_STYLE_KALE_APP_MAX_NUM UTE_MENU_STYLE_KALE_APP_MAX_NUM
#else
#define MENU_STYLE_KALE_APP_MAX_NUM MENU_APP_CNT
#endif
#ifdef UTE_MENU_STYLE_SKYRER_APP_MAX_NUM /*! 天圆地方布局菜单 显示应用最多数量  */
#define MENU_STYLE_SKYRER_APP_MAX_NUM UTE_MENU_STYLE_SKYRER_APP_MAX_NUM
#else
#define MENU_STYLE_SKYRER_APP_MAX_NUM MENU_APP_CNT
#endif
#ifdef UTE_MENU_STYLE_CUM_GRID_APP_MAX_NUM /*! 累积式网格菜单 显示应用最多数量  */
#define MENU_STYLE_CUM_GRID_APP_MAX_NUM UTE_MENU_STYLE_CUM_GRID_APP_MAX_NUM
#else
#define MENU_STYLE_CUM_GRID_APP_MAX_NUM MENU_APP_CNT
#endif
#ifdef UTE_MENU_STYLE_WATERFALL_APP_MAX_NUM /*! 瀑布流式菜单 显示应用最多数量  */
#define MENU_STYLE_WATERFALL_APP_MAX_NUM UTE_MENU_STYLE_WATERFALL_APP_MAX_NUM
#else
#define MENU_STYLE_WATERFALL_APP_MAX_NUM MENU_APP_CNT
#endif

extern void f_menu_ui_data_init(void);
extern u8 f_menu_ui_data_get_app_num();
#endif
#endif





