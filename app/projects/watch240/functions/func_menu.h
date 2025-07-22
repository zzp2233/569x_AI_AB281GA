#ifndef _FUNC_MENU_H
#define _FUNC_MENU_H
#include "ute_module_gui_common.h"

enum
{
    MENU_CLARM,         //心率
    MENU_CLOCK,         //时钟表盘
    MENU_ACTIVITY,      //运动

};

//创建不同菜单窗体
compo_form_t *func_menu_sub_football_form_create(void);
compo_form_t *func_menu_sub_honeycomb_form_create(void);
compo_form_t *func_menu_sub_waterfall_form_create(void);
compo_form_t *func_menu_sub_list_form_create(void);
compo_form_t *func_menu_sub_sudoku_form_create(void);
compo_form_t* func_menu_sub_grid_form_create(void);
compo_form_t *func_menu_sub_disk_form_create(void);
compo_form_t *func_menu_sub_ring_form_create(void);
compo_form_t *func_menu_sub_kale_form_create(void);
compo_form_t *func_menu_sub_skyrer_form_create(void);
#if UTE_MENU_STYLE_CUM_RING1_FUNCTION
compo_form_t *func_menu_sub_cum_ring1_form_create();
#endif
#if UTE_MENU_STYLE_CUM_RING2_FUNCTION
compo_form_t *func_menu_sub_cum_ring2_form_create();
#endif
compo_form_t *func_menu_sub_cum_sudoku_form_create(void);      //客户新加菜单风格
compo_form_t *func_menu_sub_hexagon_form_create(void);

//不同样式菜单入口
void func_menu_sub_football(void);
void func_menu_sub_honeycomb(void);
void func_menu_sub_waterfall(void);
void func_menu_sub_list(void);
void func_menu_sub_sudoku(void);
void func_menu_sub_grid(void);
void func_menu_sub_disk(void);
void func_menu_sub_ring(void);
void func_menu_sub_kale(void);
void func_menu_sub_skyrer(void);
#if UTE_MENU_STYLE_CUM_RING1_FUNCTION
void func_menu_sub_cum_ring1(void);
#endif
#if UTE_MENU_STYLE_CUM_RING2_FUNCTION
void func_menu_sub_cum_ring2(void);
#endif
void func_menu_sub_cum_sudoku(void);   //客户新加菜单风格
void func_menu_sub_hexagon(void);

//公用函数
void func_menu_sub_message(size_msg_t msg);
void func_menu_sub_exit(void);

#endif
