#include "include.h"
#include "func.h"
#include "func_calculator_alg.h"

#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT

typedef struct f_calculator_t_
{

} f_calculator_t;
typedef struct calculator_disp_btn_item_t_
{
    u32 res_addr;
    u32 res_addr_dwon;
    u16 btn_id;
    s16 x;
    s16 y;
    u16 num_btn_old;
    u16 hold_btn_old;
} calculator_disp_btn_item_t;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

enum
{
    COMPO_ID_NUM_DISP = 128,
};

#define CALCULATOR_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_calculator_disp_btn_item) / sizeof(tbl_calculator_disp_btn_item[0])))
#define CALCULATOR_DISP_NUMBER_MAX      32                 //显示最大位数

//按钮item，创建时遍历一下
static const calculator_disp_btn_item_t tbl_calculator_disp_btn_item[] =
{
    {UI_BUF_I330001_CALCULATOR_DEFAULT_00_BIN,       UI_BUF_I330001_CALCULATOR_STATE_00_BIN,        BTN_0,              91,    257},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_01_BIN,       UI_BUF_I330001_CALCULATOR_STATE_01_BIN,        BTN_1,              32,     213},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_02_BIN,       UI_BUF_I330001_CALCULATOR_STATE_02_BIN,        BTN_2,              91,    213},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_03_BIN,       UI_BUF_I330001_CALCULATOR_STATE_03_BIN,        BTN_3,              149,    213},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_04_BIN,       UI_BUF_I330001_CALCULATOR_STATE_04_BIN,        BTN_4,              32,     169},//
    {UI_BUF_I330001_CALCULATOR_DEFAULT_05_BIN,       UI_BUF_I330001_CALCULATOR_STATE_05_BIN,        BTN_5,              91,    169},//
    {UI_BUF_I330001_CALCULATOR_DEFAULT_06_BIN,       UI_BUF_I330001_CALCULATOR_STATE_06_BIN,        BTN_6,              149,    169},//
    {UI_BUF_I330001_CALCULATOR_DEFAULT_07_BIN,       UI_BUF_I330001_CALCULATOR_STATE_07_BIN,        BTN_7,              32,     125},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_08_BIN,       UI_BUF_I330001_CALCULATOR_STATE_08_BIN,        BTN_8,              91,    125},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_09_BIN,       UI_BUF_I330001_CALCULATOR_STATE_09_BIN,        BTN_9,              149,    125},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_DIAN_BIN,     UI_BUF_I330001_CALCULATOR_STATE_DIAN_BIN,      BTN_POINT,          149,    257},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIAJIAN_BIN,  UI_BUF_I330001_CALCULATOR_STATE_JIAJIAN_BIN,   BTN_OPPOSITE,       32,     257},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_CE_BIN,       UI_BUF_I330001_CALCULATOR_STATE_CE_BIN,        BTN_CE,             32,     81},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_AC_BIN,       UI_BUF_I330001_CALCULATOR_STATE_AC_BIN,        BTN_C,              91,    81},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_DELETE_BIN,   UI_BUF_I330001_CALCULATOR_STATE_DELETE_BIN,    BTN_DEL,            149,    81},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_CHU_BIN,      UI_BUF_I330001_CALCULATOR_STATE_CHU_BIN,       BTN_DIV,            208,    81},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_CHENG_BIN,    UI_BUF_I330001_CALCULATOR_STATE_CHENG_BIN,     BTN_MUL,            208,    125},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIAN_BIN,     UI_BUF_I330001_CALCULATOR_STATE_JIAN_BIN,      BTN_SUB,            208,    213},
    {UI_BUF_I330001_CALCULATOR_DEFAULT_JIA_BIN,      UI_BUF_I330001_CALCULATOR_STATE_JIA_BIN,       BTN_ADD,            208,    169},//
    {UI_BUF_I330001_CALCULATOR_DEFAULT_DENG_BIN,     UI_BUF_I330001_CALCULATOR_STATE_DENG_BIN,      BTN_EQUAL,          208,    257},
};


//创建计算器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_calculator_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALCULATOR_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_calculator_disp_btn_item[idx_btn].res_addr);
        compo_button_set_pos(btn, tbl_calculator_disp_btn_item[idx_btn].x, tbl_calculator_disp_btn_item[idx_btn].y-6);
        compo_setid(btn,tbl_calculator_disp_btn_item[idx_btn].btn_id);
    }

    //创建文本
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 11);
    compo_setid(txt, COMPO_ID_NUM_DISP);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
//    compo_textbox_set_autosize(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 208-widget_text_get_area(txt->txt).wid,14);

    return frm;
}

//按钮触摸效果
static void func_calculator_button_Refresh_disp(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;

//    printf("num_btn_old = %d\n", calculator_disp_btn_item->num_btn_old);
    if(calculator_disp_btn_item->num_btn_old)
    {
        compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
    }

}

//按钮触摸效果
static void func_calculator_button_press_handle(void)
{
//    printf("%s\n", __func__);
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    if(id && id!=BTN_ADD && id!=BTN_SUB && id!=BTN_MUL && id!=BTN_DIV)
    {
        compo_button_t *btn = NULL;
        if(calculator_disp_btn_item->num_btn_old != id && calculator_disp_btn_item->num_btn_old)
        {
            btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->num_btn_old = id;
        btn = compo_getobj_byid(id);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[id-1].res_addr_dwon);
    }

}

//符号按钮释放效果
static void func_calculator_button_release_handle(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    u16 hold_id = gcal_get_holding_operator();
    component_t *compo = (component_t *)compo_pool_get_top();

//    printf("btn:%d  btn:%d\n",calculator_disp_btn_item->hold_btn_old,hold_id );
    if(calculator_disp_btn_item->hold_btn_old!=hold_id)
    {
        if(calculator_disp_btn_item->hold_btn_old != 0)
        {
            compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->hold_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->hold_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->hold_btn_old = hold_id;
    }

    while (compo != NULL)
    {
        if (compo->type == COMPO_TYPE_BUTTON && compo->id == hold_id)
        {
            compo_button_t *btn = (compo_button_t *)compo;
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[compo->id-1].res_addr_dwon);
        }
        compo = compo_get_next(compo);
    }
}

//单击按钮
static void func_calculator_button_click_handler(void)
{
    int id = compo_get_button_id();
    gcalc_btn_click_handler(id);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_NUM_DISP);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 208-widget_text_get_area(txt->txt).wid,14);
    func_calculator_button_release_handle();
    func_calculator_button_Refresh_disp();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
enum
{
    COMPO_ID_NUM_DISP = 128,
};

#define CALCULATOR_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_calculator_disp_btn_item) / sizeof(tbl_calculator_disp_btn_item[0])))
#define CALCULATOR_DISP_NUMBER_MAX      32                 //显示最大位数

//按钮item，创建时遍历一下
static const calculator_disp_btn_item_t tbl_calculator_disp_btn_item[] =
{
    {UI_BUF_I332001_CALCULATOR_DEFAULT_00_BIN,       UI_BUF_I332001_CALCULATOR_STATE_00_BIN,        BTN_0,              31+83,     272+22},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_01_BIN,       UI_BUF_I332001_CALCULATOR_STATE_01_BIN,        BTN_1,              31+17,     22+220},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_02_BIN,       UI_BUF_I332001_CALCULATOR_STATE_02_BIN,        BTN_2,              31+83,     22+220},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_03_BIN,       UI_BUF_I332001_CALCULATOR_STATE_03_BIN,        BTN_3,              31+149,    22+220},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_04_BIN,       UI_BUF_I332001_CALCULATOR_STATE_04_BIN,        BTN_4,              31+17,     22+168},//
    {UI_BUF_I332001_CALCULATOR_DEFAULT_05_BIN,       UI_BUF_I332001_CALCULATOR_STATE_05_BIN,        BTN_5,              31+83,     22+168},//
    {UI_BUF_I332001_CALCULATOR_DEFAULT_06_BIN,       UI_BUF_I332001_CALCULATOR_STATE_06_BIN,        BTN_6,              31+149,    22+168},//
    {UI_BUF_I332001_CALCULATOR_DEFAULT_07_BIN,       UI_BUF_I332001_CALCULATOR_STATE_07_BIN,        BTN_7,              31+17,     22+116},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_08_BIN,       UI_BUF_I332001_CALCULATOR_STATE_08_BIN,        BTN_8,              31+83,     22+116},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_09_BIN,       UI_BUF_I332001_CALCULATOR_STATE_09_BIN,        BTN_9,              31+149,    22+116},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_DIAN_BIN,     UI_BUF_I332001_CALCULATOR_STATE_DIAN_BIN,      BTN_POINT,          31+149,    272+22},
    {0,                                              0,                                             BTN_OPPOSITE,       0,          0},
    {0,                                              0,                                             BTN_CE,             0,          0},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_AC_BIN,       UI_BUF_I332001_CALCULATOR_STATE_AC_BIN,        BTN_C,              31+215,    22+116},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_DELETE_BIN,   UI_BUF_I332001_CALCULATOR_STATE_DELETE_BIN,    BTN_DEL,            31+281,    22+116},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_CHU_BIN,      UI_BUF_I332001_CALCULATOR_STATE_CHU_BIN,       BTN_DIV,            31+281,    22+168},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_CHENG_BIN,    UI_BUF_I332001_CALCULATOR_STATE_CHENG_BIN,     BTN_MUL,            31+215,    22+168},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_JIAN_BIN,     UI_BUF_I332001_CALCULATOR_STATE_JIAN_BIN,      BTN_SUB,            31+281,    22+220},
    {UI_BUF_I332001_CALCULATOR_DEFAULT_JIA_BIN,      UI_BUF_I332001_CALCULATOR_STATE_JIA_BIN,       BTN_ADD,            31+215,    22+220},//
    {UI_BUF_I332001_CALCULATOR_DEFAULT_DENG_BIN,     UI_BUF_I332001_CALCULATOR_STATE_DENG_BIN,      BTN_EQUAL,          31+215,    272+22},
};

//创建计算器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_calculator_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALCULATOR_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_calculator_disp_btn_item[idx_btn].res_addr);
        compo_button_set_pos(btn, tbl_calculator_disp_btn_item[idx_btn].x, tbl_calculator_disp_btn_item[idx_btn].y-6);
        compo_setid(btn,tbl_calculator_disp_btn_item[idx_btn].btn_id);
    }

    //创建文本
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 11);
    compo_setid(txt, COMPO_ID_NUM_DISP);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,60);

    return frm;
}

//按钮触摸效果
static void func_calculator_button_Refresh_disp(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;

//    printf("num_btn_old = %d\n", calculator_disp_btn_item->num_btn_old);
    if(calculator_disp_btn_item->num_btn_old)
    {
        compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
    }

}

//按钮触摸效果
static void func_calculator_button_press_handle(void)
{
//    printf("%s\n", __func__);
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    if(id && id!=BTN_ADD && id!=BTN_SUB && id!=BTN_MUL && id!=BTN_DIV)
    {
        compo_button_t *btn = NULL;
        if(calculator_disp_btn_item->num_btn_old != id && calculator_disp_btn_item->num_btn_old)
        {
            btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->num_btn_old = id;
        btn = compo_getobj_byid(id);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[id-1].res_addr_dwon);
    }

}

//符号按钮释放效果
static void func_calculator_button_release_handle(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    u16 hold_id = gcal_get_holding_operator();
    component_t *compo = (component_t *)compo_pool_get_top();

//    printf("btn:%d  btn:%d\n",calculator_disp_btn_item->hold_btn_old,hold_id );
    if(calculator_disp_btn_item->hold_btn_old!=hold_id)
    {
        if(calculator_disp_btn_item->hold_btn_old != 0)
        {
            compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->hold_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->hold_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->hold_btn_old = hold_id;
    }

    while (compo != NULL)
    {
        if (compo->type == COMPO_TYPE_BUTTON && compo->id == hold_id)
        {
            compo_button_t *btn = (compo_button_t *)compo;
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[compo->id-1].res_addr_dwon);
        }
        compo = compo_get_next(compo);
    }
}

//单击按钮
static void func_calculator_button_click_handler(void)
{
    int id = compo_get_button_id();
    gcalc_btn_click_handler(id);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_NUM_DISP);
    compo_textbox_set(txt, gcal_get_show_str());
    func_calculator_button_release_handle();
    func_calculator_button_Refresh_disp();
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

enum
{
    COMPO_ID_NUM_DISP = 128,
};

#define CALCULATOR_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_calculator_disp_btn_item) / sizeof(tbl_calculator_disp_btn_item[0])))
#define CALCULATOR_DISP_NUMBER_MAX      32                 //显示最大位数

//按钮item，创建时遍历一下
static const calculator_disp_btn_item_t tbl_calculator_disp_btn_item[] =
{
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_0_ICON_PIC112X40_X6_Y232_00_BIN,       UI_BUF_I335001_27_MORE_6_CALCULATOR_0_ICON_PIC112X40_X6_Y232_01_BIN,       BTN_0,              6+112/2, 232+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_1_ICON_PIC54X40_X6_Y188_00_BIN,        UI_BUF_I335001_27_MORE_6_CALCULATOR_1_ICON_PIC54X40_X6_Y188_01_BIN,        BTN_1,              6+54/2,  188+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_2_ICON_PIC54X40_X64_Y188_00_BIN,       UI_BUF_I335001_27_MORE_6_CALCULATOR_2_ICON_PIC54X40_X64_Y188_01_BIN,       BTN_2,              64+54/2, 188+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_3_ICON_PIC54X40_X122_Y188_00_BIN,      UI_BUF_I335001_27_MORE_6_CALCULATOR_3_ICON_PIC54X40_X122_Y188_01_BIN,      BTN_3,              122+54/2,188+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_4_ICON_PIC54X40_X6_Y144_00_BIN,        UI_BUF_I335001_27_MORE_6_CALCULATOR_4_ICON_PIC54X40_X6_Y144_01_BIN,        BTN_4,              6+54/2,  144+40/2},//
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_5_ICON_PIC54X40_X64_Y144_00_BIN,       UI_BUF_I335001_27_MORE_6_CALCULATOR_5_ICON_PIC54X40_X64_Y144_01_BIN,       BTN_5,              64+54/2, 144+40/2},//
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_6_ICON_PIC54X40_X122_Y144_00_BIN,      UI_BUF_I335001_27_MORE_6_CALCULATOR_6_ICON_PIC54X40_X122_Y144_01_BIN,      BTN_6,              122+54/2,144+40/2},//
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_7_ICON_PIC54X40_X6_Y100_00_BIN,        UI_BUF_I335001_27_MORE_6_CALCULATOR_7_ICON_PIC54X40_X6_Y100_01_BIN,        BTN_7,              6+54/2,  100+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_8_ICON_PIC54X40_X64_Y100_00_BIN,       UI_BUF_I335001_27_MORE_6_CALCULATOR_8_ICON_PIC54X40_X64_Y100_01_BIN,       BTN_8,              64+54/2, 100+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_9_ICON_PIC54X40_X122_Y100_00_BIN,      UI_BUF_I335001_27_MORE_6_CALCULATOR_9_ICON_PIC54X40_X122_Y100_01_BIN,      BTN_9,              122+54/2,100+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_DIAN_ICON_PIC54X40_X122_Y232_00_BIN,   UI_BUF_I335001_27_MORE_6_CALCULATOR_DIAN_ICON_PIC54X40_X122_Y232_01_BIN,   BTN_POINT,          122+54/2,232+40/2},
    {0,                                              0,                                             BTN_OPPOSITE,       0,          0},
    {0,                                              0,                                             BTN_CE,             0,          0},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_AC_ICON_PIC112X40_X6_Y58_00_BIN,       UI_BUF_I335001_27_MORE_6_CALCULATOR_AC_ICON_PIC112X40_X6_Y58_01_BIN,       BTN_C,              6+112/2, 56+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_DELETE_ICON_PIC54X40_X122_Y56_00_BIN,  UI_BUF_I335001_27_MORE_6_CALCULATOR_DELETE_ICON_PIC54X40_X122_Y56_01_BIN,  BTN_DEL,            122+54/2,56+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_CHU_ICON_PIC54X40_X180_Y56_00_BIN,     UI_BUF_I335001_27_MORE_6_CALCULATOR_CHU_ICON_PIC54X40_X180_Y56_01_BIN,     BTN_DIV,            180+54/2,56+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_CHENG_ICON_PIC54X40_X180_Y100_00_BIN,  UI_BUF_I335001_27_MORE_6_CALCULATOR_CHENG_ICON_PIC54X40_X180_Y100_01_BIN,  BTN_MUL,            180+54/2,100+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_JIAN_ICON_PIC54X40_X180_Y188_00_BIN,   UI_BUF_I335001_27_MORE_6_CALCULATOR_JIAN_ICON_PIC54X40_X180_Y188_01_BIN,   BTN_SUB,            180+54/2,188+40/2},
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_JIA_ICON_PIC54X40_X180_Y144_00_BIN,    UI_BUF_I335001_27_MORE_6_CALCULATOR_JIA_ICON_PIC54X40_X180_Y144_01_BIN,    BTN_ADD,            180+54/2,144+40/2},//
    {UI_BUF_I335001_27_MORE_6_CALCULATOR_DENG_ICON_PIC54X40_X180_Y232_00_BIN,   UI_BUF_I335001_27_MORE_6_CALCULATOR_DENG_ICON_PIC54X40_X180_Y232_01_BIN,   BTN_EQUAL,          180+54/2,232+40/2},
};


//创建计算器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_calculator_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALCULATOR_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_calculator_disp_btn_item[idx_btn].res_addr);
        compo_button_set_pos(btn, tbl_calculator_disp_btn_item[idx_btn].x, tbl_calculator_disp_btn_item[idx_btn].y-6);
        compo_setid(btn,tbl_calculator_disp_btn_item[idx_btn].btn_id);
    }

    //创建文本
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 11);
    compo_setid(txt, COMPO_ID_NUM_DISP);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_32_BIN);
//    compo_textbox_set_autosize(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 208-widget_text_get_area(txt->txt).wid,14);

    return frm;
}

//按钮触摸效果
static void func_calculator_button_Refresh_disp(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;

//    printf("num_btn_old = %d\n", calculator_disp_btn_item->num_btn_old);
    if(calculator_disp_btn_item->num_btn_old)
    {
        compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
    }

}

//按钮触摸效果
static void func_calculator_button_press_handle(void)
{
//    printf("%s\n", __func__);
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    if(id && id!=BTN_ADD && id!=BTN_SUB && id!=BTN_MUL && id!=BTN_DIV)
    {
        compo_button_t *btn = NULL;
        if(calculator_disp_btn_item->num_btn_old != id && calculator_disp_btn_item->num_btn_old)
        {
            btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->num_btn_old = id;
        btn = compo_getobj_byid(id);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[id-1].res_addr_dwon);
    }

}

//符号按钮释放效果
static void func_calculator_button_release_handle(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    u16 hold_id = gcal_get_holding_operator();
    component_t *compo = (component_t *)compo_pool_get_top();

//    printf("btn:%d  btn:%d\n",calculator_disp_btn_item->hold_btn_old,hold_id );
    if(calculator_disp_btn_item->hold_btn_old!=hold_id)
    {
        if(calculator_disp_btn_item->hold_btn_old != 0)
        {
            compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->hold_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->hold_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->hold_btn_old = hold_id;
    }

    while (compo != NULL)
    {
        if (compo->type == COMPO_TYPE_BUTTON && compo->id == hold_id)
        {
            compo_button_t *btn = (compo_button_t *)compo;
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[compo->id-1].res_addr_dwon);
        }
        compo = compo_get_next(compo);
    }
}

//单击按钮
static void func_calculator_button_click_handler(void)
{
    int id = compo_get_button_id();
    gcalc_btn_click_handler(id);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_NUM_DISP);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 208-widget_text_get_area(txt->txt).wid,14);
    func_calculator_button_release_handle();
    func_calculator_button_Refresh_disp();
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

enum
{
    COMPO_ID_NUM_DISP = 128,
};

#define CALCULATOR_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_calculator_disp_btn_item) / sizeof(tbl_calculator_disp_btn_item[0])))
#define CALCULATOR_DISP_NUMBER_MAX      32                 //显示最大位数

//按钮item，创建时遍历一下
static const calculator_disp_btn_item_t tbl_calculator_disp_btn_item[] =
{
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_00_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_00_BIN,    BTN_0,              84+60/2, 283+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_01_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_01_BIN,    BTN_1,              18+60/2, 224+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_02_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_02_BIN,    BTN_2,              84+60/2, 224+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_03_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_03_BIN,    BTN_3,              150+60/2,224+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_04_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_04_BIN,    BTN_4,              18+60/2, 165+50/2},//
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_05_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_05_BIN,    BTN_5,              84+60/2, 165+50/2},//
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_06_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_06_BIN,    BTN_6,              150+60/2,165+50/2},//
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_07_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_07_BIN,    BTN_7,              18+60/2, 106+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_08_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_08_BIN,    BTN_8,              84+60/2, 106+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_09_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_09_BIN,    BTN_9,              150+60/2,106+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_10_BIN,      UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_10_BIN,    BTN_POINT,          150+60/2,283+50/2},
    {0,                                              0,                                             BTN_OPPOSITE,       0,          0},
    {0,                                              0,                                             BTN_CE,             0,          0},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_11_BIN,     UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_11_BIN,     BTN_C,              216+60/2,106+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_12_BIN,     UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_12_BIN,     BTN_DEL,            282+60/2,106+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_13_BIN,     UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_13_BIN,     BTN_DIV,            216+60/2,165+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_14_BIN,     UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_14_BIN,     BTN_MUL,            282+60/2,165+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_16_BIN,     UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_16_BIN,     BTN_SUB,            282+60/2,224+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_15_BIN,     UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_15_BIN,     BTN_ADD,            216+60/2,224+50/2},
    {UI_BUF_I338001_27_MORE_6_CALCULATOR_STATE_17_BIN,     UI_BUF_I338001_27_MORE_6_CALCULATOR_DEFAULT_17_BIN,     BTN_EQUAL,          216+60/2,283+50/2},
};


//创建计算器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_calculator_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < CALCULATOR_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_calculator_disp_btn_item[idx_btn].res_addr);
        compo_button_set_pos(btn, tbl_calculator_disp_btn_item[idx_btn].x, tbl_calculator_disp_btn_item[idx_btn].y-6);
        compo_setid(btn,tbl_calculator_disp_btn_item[idx_btn].btn_id);
    }

    //创建文本
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 11);
    compo_setid(txt, COMPO_ID_NUM_DISP);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 216+60-widget_text_get_area(txt->txt).wid,63);

    return frm;
}

//按钮触摸效果
static void func_calculator_button_Refresh_disp(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;

//    printf("num_btn_old = %d\n", calculator_disp_btn_item->num_btn_old);
    if(calculator_disp_btn_item->num_btn_old)
    {
        compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
    }

}

//按钮触摸效果
static void func_calculator_button_press_handle(void)
{
//    printf("%s\n", __func__);
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    if(id && id!=BTN_ADD && id!=BTN_SUB && id!=BTN_MUL && id!=BTN_DIV)
    {
        compo_button_t *btn = NULL;
        if(calculator_disp_btn_item->num_btn_old != id && calculator_disp_btn_item->num_btn_old)
        {
            btn = compo_getobj_byid(calculator_disp_btn_item->num_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->num_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->num_btn_old = id;
        btn = compo_getobj_byid(id);
        compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[id-1].res_addr_dwon);
    }

}

//符号按钮释放效果
static void func_calculator_button_release_handle(void)
{
    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    u16 hold_id = gcal_get_holding_operator();
    component_t *compo = (component_t *)compo_pool_get_top();

//    printf("btn:%d  btn:%d\n",calculator_disp_btn_item->hold_btn_old,hold_id );
    if(calculator_disp_btn_item->hold_btn_old!=hold_id)
    {
        if(calculator_disp_btn_item->hold_btn_old != 0)
        {
            compo_button_t *btn = compo_getobj_byid(calculator_disp_btn_item->hold_btn_old);
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[calculator_disp_btn_item->hold_btn_old-1].res_addr);
        }
        calculator_disp_btn_item->hold_btn_old = hold_id;
    }

    while (compo != NULL)
    {
        if (compo->type == COMPO_TYPE_BUTTON && compo->id == hold_id)
        {
            compo_button_t *btn = (compo_button_t *)compo;
            compo_button_set_bgimg(btn, tbl_calculator_disp_btn_item[compo->id-1].res_addr_dwon);
        }
        compo = compo_get_next(compo);
    }
}

//单击按钮
static void func_calculator_button_click_handler(void)
{
    int id = compo_get_button_id();
    gcalc_btn_click_handler(id);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_NUM_DISP);
    compo_textbox_set(txt, gcal_get_show_str());
    compo_textbox_set_pos(txt, 216+60-widget_text_get_area(txt->txt).wid,63);
    func_calculator_button_release_handle();
    func_calculator_button_Refresh_disp();
}

#else

compo_form_t *func_calculator_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    return frm;
}
//按钮触摸效果
static void func_calculator_button_press_handle(void)
{

}

//符号按钮释放效果
static void func_calculator_button_release_handle(void)
{

}

//单击按钮
static void func_calculator_button_click_handler(void)
{

}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//计算器功能事件处理
static void func_calculator_process(void)
{
    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }
    reset_sleep_delay_all();
    func_process();
}

//计算器功能消息处理
static void func_calculator_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_calculator_button_press_handle();
            break;
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_LONG:
            func_calculator_button_Refresh_disp();
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            break;

        case MSG_CTP_SHORT_RIGHT:
            func_calculator_button_Refresh_disp();
            func_message(msg);
            break;

        case MSG_CTP_CLICK:
            func_calculator_button_click_handler();
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入计算器功能
static void func_calculator_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_calculator_t));
    func_cb.frm_main = func_calculator_form_create();

    calculator_disp_btn_item_t *calculator_disp_btn_item = (calculator_disp_btn_item_t *)func_cb.f_cb;
    calculator_disp_btn_item->num_btn_old = 0;
    calculator_disp_btn_item->hold_btn_old = 0;
    if (gcal_cb_init() == false)
    {
        halt(HALT_MALLOC);
    }
    if (sys_cb.gui_sleep_sta)
    {
        sys_cb.gui_need_wakeup = 1;
    }
    reset_sleep_delay_all();
}

//退出计算器功能
static void func_calculator_exit(void)
{
    gcal_cb_destroy();
}

//计算器功能
void func_calculator(void)
{
    printf("%s\n", __func__);
    func_calculator_enter();
    while (func_cb.sta == FUNC_CALCULATOR)
    {
        func_calculator_process();
        func_calculator_message(msg_dequeue());
    }
    func_calculator_exit();
}
#endif
