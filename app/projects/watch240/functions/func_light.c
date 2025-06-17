#include "include.h"
#include "func.h"
#include "ute_module_gui_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_LIGHT_SUPPORT

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//组件ID
enum
{
    //按键
    COMPO_ID_BTN_REDUCE = 1,
    COMPO_ID_BTN_INCREASE,

    //图像
    COMPO_ID_PIC_LEVEL1,            //亮度等级 1~5
    COMPO_ID_PIC_LEVEL2,
    COMPO_ID_PIC_LEVEL3,
    COMPO_ID_PIC_LEVEL4,
    COMPO_ID_PIC_LEVEL5,
    COMPO_ID_PIC_REDUCE_CLICK,      //按钮触摸效果图
    COMPO_ID_PIC_INCREASE_CLICK,
};

typedef struct f_light_t_
{
    int last_y_move;
    u8 light_level;
} f_light_t;

typedef struct light_pic_item_t_
{
    u32 res_addr;
    u16 pic_id;
    s16 x;
    s16 y;
} light_pic_item_t;

#define LIGHT_PIC_ITEM_CNT    ((int)(sizeof(tbl_light_pic_item) / sizeof(tbl_light_pic_item[0])))

//图片item，创建时遍历一下
static const light_pic_item_t tbl_light_pic_item[] =
{
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN,     COMPO_ID_PIC_LEVEL1,         120,     82   },
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL2,         120,     123    },
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL3,         120,     164    },
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL4,         120,     205    },
    {UI_BUF_I330001_SETTINGS_BRIGHTNESS_00_BIN,     COMPO_ID_PIC_LEVEL5,         120,     246    },
};

//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn, 39, 164);

    btn = compo_button_create_by_image(frm, UI_BUF_I330001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn, 201, 164);

    //新建图像
    compo_picturebox_t *pic_level[LIGHT_PIC_ITEM_CNT];
    for (u8 idx = 0; idx < LIGHT_PIC_ITEM_CNT; idx++)
    {
        pic_level[idx] = compo_picturebox_create(frm, tbl_light_pic_item[idx].res_addr);
        compo_setid(pic_level[idx], tbl_light_pic_item[idx].pic_id);
        compo_picturebox_set_pos(pic_level[idx], tbl_light_pic_item[idx].x, tbl_light_pic_item[idx].y);
    }
    return frm;
}

static void func_light_disp_Refresh(void)
{
    compo_picturebox_t *pic_level[5];
    pic_level[0] = compo_getobj_byid(COMPO_ID_PIC_LEVEL1);
    pic_level[1] = compo_getobj_byid(COMPO_ID_PIC_LEVEL2);
    pic_level[2] = compo_getobj_byid(COMPO_ID_PIC_LEVEL3);
    pic_level[3] = compo_getobj_byid(COMPO_ID_PIC_LEVEL4);
    pic_level[4] = compo_getobj_byid(COMPO_ID_PIC_LEVEL5);

    switch(sys_cb.light_level)
    {
        case 1:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
        case 2:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
        case 3:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
        case 4:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
        case 5:
            compo_picturebox_set(pic_level[0], UI_BUF_I330001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I330001_SETTINGS_BRIGHTNESS_05_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I330001_SETTINGS_BRIGHTNESS_02_BIN);
            break;
    }
}

//单击按钮
static void func_light_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REDUCE:
            if (sys_cb.light_level > DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level <= DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level--;
            }
            break;

        case COMPO_ID_BTN_INCREASE:
            if (sys_cb.light_level >= DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level < DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level++;
            }
            break;

        default:
            break;
    }
    func_light_disp_Refresh();
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//组件ID
enum
{
    //按键
    COMPO_ID_BTN_REDUCE = 1,
    COMPO_ID_BTN_INCREASE,

    //图像
    COMPO_ID_PIC_LEVEL1,            //亮度等级 1~5
    COMPO_ID_PIC_LEVEL2,
    COMPO_ID_PIC_LEVEL3,
    COMPO_ID_PIC_LEVEL4,
    COMPO_ID_PIC_LEVEL5,
    COMPO_ID_PIC_REDUCE_CLICK,      //按钮触摸效果图
    COMPO_ID_PIC_INCREASE_CLICK,
};

typedef struct f_light_t_
{
    int last_y_move;
    u8 light_level;
} f_light_t;

typedef struct light_pic_item_t_
{
    u32 res_addr;
    u16 pic_id;
    s16 x;
    s16 y;
} light_pic_item_t;

#define LIGHT_PIC_ITEM_CNT    ((int)(sizeof(tbl_light_pic_item) / sizeof(tbl_light_pic_item[0])))

//图片item，创建时遍历一下
static const light_pic_item_t tbl_light_pic_item[] =
{
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN,     COMPO_ID_PIC_LEVEL1,         GUI_SCREEN_CENTER_X,     88+48/2   },
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL2,         GUI_SCREEN_CENTER_X,     137+48/2     },
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL3,         GUI_SCREEN_CENTER_X,     186+48/2     },
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL4,         GUI_SCREEN_CENTER_X,     235+48/2     },
    {UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN,     COMPO_ID_PIC_LEVEL5,         GUI_SCREEN_CENTER_X,     284+48/2     },
};
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn, 62/2+41,62/2+159);

    btn = compo_button_create_by_image(frm, UI_BUF_I332001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn,  62/2+257,62/2+159);

    //新建图像
    compo_picturebox_t *pic_level[LIGHT_PIC_ITEM_CNT];
    for (u8 idx = 0; idx < LIGHT_PIC_ITEM_CNT; idx++)
    {
        pic_level[idx] = compo_picturebox_create(frm, tbl_light_pic_item[idx].res_addr);
        compo_setid(pic_level[idx], tbl_light_pic_item[idx].pic_id);
        compo_picturebox_set_pos(pic_level[idx], tbl_light_pic_item[idx].x, tbl_light_pic_item[idx].y);
    }
    return frm;
}

static void func_light_disp_Refresh(void)
{
    compo_picturebox_t *pic_level[5];
    pic_level[0] = compo_getobj_byid(COMPO_ID_PIC_LEVEL1);
    pic_level[1] = compo_getobj_byid(COMPO_ID_PIC_LEVEL2);
    pic_level[2] = compo_getobj_byid(COMPO_ID_PIC_LEVEL3);
    pic_level[3] = compo_getobj_byid(COMPO_ID_PIC_LEVEL4);
    pic_level[4] = compo_getobj_byid(COMPO_ID_PIC_LEVEL5);

    switch(sys_cb.light_level)
    {
        case 1:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 2:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 3:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 4:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 5:
            compo_picturebox_set(pic_level[0], UI_BUF_I332001_SETTINGS_BRIGHTNESS_02_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I332001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I332001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
    }
}

//单击按钮
static void func_light_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REDUCE:
            if (sys_cb.light_level > DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level <= DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level--;
            }
            break;

        case COMPO_ID_BTN_INCREASE:
            if (sys_cb.light_level >= DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level < DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level++;
            }
            break;

        default:
            break;
    }
    func_light_disp_Refresh();
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//组件ID
enum
{
    //按键
    COMPO_ID_LIGHT_PIC = 1,
    COMPO_ID_LIGHT_BTN,
};

#define SPACING_COUNT  (-40)
typedef struct f_light_t_
{
    bool touch_flag;
    s32  move_offset;
    s32  move_offset_old;
    s8   level_old;
    s8   level_value;
} f_light_t;
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{
    u8 level_data = sys_cb.light_level-1;

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);
    //亮度
    compo_picturebox_t *light_pic = compo_picturebox_create(frm, UI_BUF_I335001_27_MORE_28_SET_3_BRIGHTNESS_2_BRIGHTNESS_SETTING_ICON_68X200_X86_Y58_00_BIN);
    compo_setid(light_pic, COMPO_ID_LIGHT_PIC);
    compo_picturebox_set_pos(light_pic, GUI_SCREEN_CENTER_X, 58+200/2);
    compo_picturebox_cut(light_pic,level_data,5);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 58+200/2,68,200);
    compo_setid(btn,COMPO_ID_LIGHT_BTN);

    if(func_cb.sta == FUNC_LIGHT)
    {
        f_light_t *f_light = (f_light_t *)func_cb.f_cb;
        f_light->level_old = (s8)sys_cb.light_level;
    }

    return frm;
}
static void func_light_disp_move_handle(void)
{
    f_light_t *f_light = (f_light_t *)func_cb.f_cb;
    if(f_light->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_light->touch_flag = ctp_get_dxy(&dx, &dy);

        compo_picturebox_t *light_pic = compo_getobj_byid(COMPO_ID_LIGHT_PIC);
        if(f_light->touch_flag)//触摸状态
        {
            s8 level_data= (dy/SPACING_COUNT)+f_light->level_old;

            if(level_data<1)
                level_data=1;
            else if(level_data>5)
                level_data=5;

            if(level_data != f_light->level_value)
            {
                // printf("level_data:%d\n",level_data);
                f_light->level_value = level_data;
                compo_picturebox_cut(light_pic,(uint8_t)level_data-1,5);
                tft_bglight_set_level((uint8_t)level_data,false);
            }
        }
        else
        {
            f_light->level_old = f_light->level_value;
        }
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
//组件ID
enum
{
    //按键
    COMPO_ID_LIGHT_PIC = 1,
    COMPO_ID_LIGHT_BTN,
};

#define SPACING_COUNT  (-40)
typedef struct f_light_t_
{
    bool touch_flag;
    s32  move_offset;
    s32  move_offset_old;
    s8   level_old;
    s8   level_value;
} f_light_t;

const static u32 light_arr[5] =
{
    UI_BUF_I341001_28_SET_BRIGHTNESS_00_BIN,
    UI_BUF_I341001_28_SET_BRIGHTNESS_01_BIN,
    UI_BUF_I341001_28_SET_BRIGHTNESS_02_BIN,
    UI_BUF_I341001_28_SET_BRIGHTNESS_03_BIN,
    UI_BUF_I341001_28_SET_BRIGHTNESS_04_BIN,
};
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);
    //亮度
    compo_picturebox_t *light_pic = compo_picturebox_create(frm, light_arr[sys_cb.light_level-1]);
    compo_setid(light_pic, COMPO_ID_LIGHT_PIC);
    compo_picturebox_set_pos(light_pic, GUI_SCREEN_CENTER_X, 97+306/2);
    //compo_picturebox_cut(light_pic,sys_cb.light_level-1,5);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 97+306/2,106,306);
    compo_setid(btn,COMPO_ID_LIGHT_BTN);

    if(func_cb.sta == FUNC_LIGHT)
    {
        f_light_t *f_light = (f_light_t *)func_cb.f_cb;
        f_light->level_old = (s8)sys_cb.light_level;
    }

    return frm;
}
static void func_light_disp_move_handle(void)
{
    f_light_t *f_light = (f_light_t *)func_cb.f_cb;
    if(f_light->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_light->touch_flag = ctp_get_dxy(&dx, &dy);

        compo_picturebox_t *light_pic = compo_getobj_byid(COMPO_ID_LIGHT_PIC);
        if(f_light->touch_flag)//触摸状态
        {
            s8 level_data= (dy/SPACING_COUNT)+f_light->level_old;

            if(level_data<1)
                level_data=1;
            else if(level_data>5)
                level_data=5;

            if(level_data != f_light->level_value)
            {
                // printf("level_data:%d\n",level_data);
                f_light->level_value = level_data;
                compo_picturebox_set(light_pic, light_arr[level_data-1]);
                tft_bglight_set_level((uint8_t)level_data,false);
            }
        }
        else
        {
            f_light->level_old = f_light->level_value;
        }
    }
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//组件ID
enum
{
    //按键
    COMPO_ID_LIGHT_PIC = 1,
    COMPO_ID_LIGHT_BTN,
};

#define SPACING_COUNT  (-40)
typedef struct f_light_t_
{
    bool touch_flag;
    s32  move_offset;
    s32  move_offset_old;
    s8   level_old;
    s8   level_value;
} f_light_t;

//const static u32 light_arr[5] =
//{
//    UI_BUF_I341001_28_SET_BRIGHTNESS_00_BIN,
//    UI_BUF_I341001_28_SET_BRIGHTNESS_01_BIN,
//    UI_BUF_I341001_28_SET_BRIGHTNESS_02_BIN,
//    UI_BUF_I341001_28_SET_BRIGHTNESS_03_BIN,
//    UI_BUF_I341001_28_SET_BRIGHTNESS_04_BIN,
//};
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);
    //亮度
    compo_picturebox_t *light_pic = compo_picturebox_create(frm, UI_BUF_I343001_28_SET_BRIGHTNESS_00_BIN);
    compo_setid(light_pic, COMPO_ID_LIGHT_PIC);
    compo_picturebox_set_pos(light_pic, GUI_SCREEN_CENTER_X, 79+266/2);
    compo_picturebox_cut(light_pic,sys_cb.light_level-1,5);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 79+226/2,106,266);
    compo_setid(btn,COMPO_ID_LIGHT_BTN);

    if(func_cb.sta == FUNC_LIGHT)
    {
        f_light_t *f_light = (f_light_t *)func_cb.f_cb;
        f_light->level_old = (s8)sys_cb.light_level;
    }

    return frm;
}
static void func_light_disp_move_handle(void)
{
    f_light_t *f_light = (f_light_t *)func_cb.f_cb;
    if(f_light->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_light->touch_flag = ctp_get_dxy(&dx, &dy);

        compo_picturebox_t *light_pic = compo_getobj_byid(COMPO_ID_LIGHT_PIC);
        if(f_light->touch_flag)//触摸状态
        {
            s8 level_data= (dy/SPACING_COUNT)+f_light->level_old;

            if(level_data<1)
                level_data=1;
            else if(level_data>5)
                level_data=5;

            if(level_data != f_light->level_value)
            {
//                 printf("level_data:%d\n",level_data);
                f_light->level_value = level_data;
                compo_picturebox_cut(light_pic,f_light->level_value-1,5);
                tft_bglight_set_level((uint8_t)level_data,false);
            }
        }
        else
        {
            f_light->level_old = f_light->level_value;
        }
    }
}


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//组件ID
enum
{
    //按键
    COMPO_ID_LIGHT_PIC = 1,
    COMPO_ID_LIGHT_BTN,
};

typedef struct f_light_t_
{
    bool touch_flag;
    s32  move_offset;
    s32  move_offset_old;
    s8   level_old;
    s8   level_value;
} f_light_t;

#define val_shape_y  60
#define val_shepe_x  135
#define val_shape_h  260
#define val_shepe_w  90
#define make_pic_hei(val)   (val*val_shape_h/5)
#define make_pic_y(hei)     (val_shape_h-hei+val_shape_y)
#define SPACING_COUNT  (-val_shape_h/5)
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);
    //亮度
    // compo_picturebox_t *light_pic = compo_picturebox_create(frm, UI_BUF_I338001_28_SET_BRIGHTNESS_00_BIN);
    // compo_setid(light_pic, COMPO_ID_LIGHT_PIC);
    // compo_picturebox_set_pos(light_pic, GUI_SCREEN_CENTER_X, 79+266/2);
    // compo_picturebox_cut(light_pic,sys_cb.light_level-1,5);

    // compo_button_t *btn = compo_button_create(frm);
    // compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 79+266/2,90,266);
    // compo_setid(btn,COMPO_ID_LIGHT_BTN);

    s16 shape_y = make_pic_y(make_pic_hei(sys_cb.light_level));
    s16 shape_h = make_pic_hei(sys_cb.light_level);

    compo_shape_t * val = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(val,make_color(37,37,41));
    compo_shape_set_location(val,val_shepe_x,make_pic_y(make_pic_hei(5))+5,val_shepe_w,make_pic_hei(5));
    compo_shape_set_radius(val, 20);
    widget_set_align_center(val->rect, false);

    val = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(val,val_shepe_x,shape_y+5,val_shepe_w,shape_h);
    compo_shape_set_radius(val, 20);
    widget_set_align_center(val->rect, false);
    compo_setid(val, COMPO_ID_LIGHT_PIC);

    compo_button_t *btn = compo_button_create_by_image(frm, 0);
    compo_setid(btn, COMPO_ID_LIGHT_BTN);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+5,val_shepe_w,val_shape_h);

    btn = compo_button_create_by_image(frm, UI_BUF_I338001_11_CALL_LIGHT_BIN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,32/2+262);

    if(func_cb.sta == FUNC_LIGHT)
    {
        f_light_t *f_light = (f_light_t *)func_cb.f_cb;
        f_light->level_old = (s8)sys_cb.light_level;
    }

    return frm;
}
static void func_light_disp_move_handle(void)
{
    f_light_t *f_light = (f_light_t *)func_cb.f_cb;
    if(f_light->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_light->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_light->touch_flag)//触摸状态
        {
            s8 level_data= (dy/SPACING_COUNT)+f_light->level_old;

            if(level_data<1)
                level_data=1;
            else if(level_data>5)
                level_data=5;

            if(level_data != f_light->level_value)
            {
                // printf("level_data:%d\n",level_data);
                f_light->level_value = level_data;
                // compo_picturebox_cut(light_pic,(uint8_t)level_data-1,5);
                s16 shape_y = make_pic_y(make_pic_hei(level_data));
                s16 shape_h = make_pic_hei(level_data);
                compo_shape_t * val = compo_getobj_byid(COMPO_ID_LIGHT_PIC);
                if(val != NULL)
                {
                    compo_shape_set_location(val,val_shepe_x,shape_y+5,val_shepe_w,shape_h);
                }
                // printf("y:%d h:%d lever:%d\n",shape_y,shape_h,level_data);
                tft_bglight_set_level((uint8_t)level_data,false);
            }
        }
        else
        {
            f_light->level_old = f_light->level_value;
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//组件ID
enum
{
    //按键
    COMPO_ID_BTN_REDUCE = 1,
    COMPO_ID_BTN_INCREASE,

    //图像
    COMPO_ID_PIC_LEVEL1,            //亮度等级 1~5
    COMPO_ID_PIC_LEVEL2,
    COMPO_ID_PIC_LEVEL3,
    COMPO_ID_PIC_LEVEL4,
    COMPO_ID_PIC_LEVEL5,
    COMPO_ID_PIC_REDUCE_CLICK,      //按钮触摸效果图
    COMPO_ID_PIC_INCREASE_CLICK,
};

typedef struct f_light_t_
{
    int last_y_move;
    u8 light_level;
} f_light_t;

typedef struct light_pic_item_t_
{
    u32 res_addr;
    u16 pic_id;
    s16 x;
    s16 y;
} light_pic_item_t;

#define LIGHT_PIC_ITEM_CNT    ((int)(sizeof(tbl_light_pic_item) / sizeof(tbl_light_pic_item[0])))

//图片item，创建时遍历一下
static const light_pic_item_t tbl_light_pic_item[] =
{
    {UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN,     COMPO_ID_PIC_LEVEL1,         GUI_SCREEN_CENTER_X,     88+48/2   },
    {UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL2,         GUI_SCREEN_CENTER_X,     137+48/2     },
    {UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL3,         GUI_SCREEN_CENTER_X,     186+48/2     },
    {UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN,     COMPO_ID_PIC_LEVEL4,         GUI_SCREEN_CENTER_X,     235+48/2     },
    {UI_BUF_I340001_SETTINGS_BRIGHTNESS_00_BIN,     COMPO_ID_PIC_LEVEL5,         GUI_SCREEN_CENTER_X,     284+48/2     },
};
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn, 62/2+41,62/2+159);

    btn = compo_button_create_by_image(frm, UI_BUF_I340001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn,  62/2+257,62/2+159);

    //新建图像
    compo_picturebox_t *pic_level[LIGHT_PIC_ITEM_CNT];
    for (u8 idx = 0; idx < LIGHT_PIC_ITEM_CNT; idx++)
    {
        pic_level[idx] = compo_picturebox_create(frm, tbl_light_pic_item[idx].res_addr);
        compo_setid(pic_level[idx], tbl_light_pic_item[idx].pic_id);
        compo_picturebox_set_pos(pic_level[idx], tbl_light_pic_item[idx].x, tbl_light_pic_item[idx].y);
    }
    return frm;
}

static void func_light_disp_Refresh(void)
{
    compo_picturebox_t *pic_level[5];
    pic_level[0] = compo_getobj_byid(COMPO_ID_PIC_LEVEL1);
    pic_level[1] = compo_getobj_byid(COMPO_ID_PIC_LEVEL2);
    pic_level[2] = compo_getobj_byid(COMPO_ID_PIC_LEVEL3);
    pic_level[3] = compo_getobj_byid(COMPO_ID_PIC_LEVEL4);
    pic_level[4] = compo_getobj_byid(COMPO_ID_PIC_LEVEL5);

    switch(sys_cb.light_level)
    {
        case 1:
            compo_picturebox_set(pic_level[0], UI_BUF_I340001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I340001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 2:
            compo_picturebox_set(pic_level[0], UI_BUF_I340001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I340001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 3:
            compo_picturebox_set(pic_level[0], UI_BUF_I340001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I340001_SETTINGS_BRIGHTNESS_04_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I340001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 4:
            compo_picturebox_set(pic_level[0], UI_BUF_I340001_SETTINGS_BRIGHTNESS_03_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I340001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
        case 5:
            compo_picturebox_set(pic_level[0], UI_BUF_I340001_SETTINGS_BRIGHTNESS_02_BIN);
            compo_picturebox_set(pic_level[1], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[2], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[3], UI_BUF_I340001_SETTINGS_BRIGHTNESS_01_BIN);
            compo_picturebox_set(pic_level[4], UI_BUF_I340001_SETTINGS_BRIGHTNESS_00_BIN);
            break;
    }
}

//单击按钮
static void func_light_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REDUCE:
            if (sys_cb.light_level > DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level <= DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level--;
            }
            break;

        case COMPO_ID_BTN_INCREASE:
            if (sys_cb.light_level >= DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE && sys_cb.light_level < DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                sys_cb.light_level++;
            }
            break;

        default:
            break;
    }
    func_light_disp_Refresh();
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

//组件ID
enum
{
    //按键
    COMPO_ID_LIGHT_PIC = 1,
    COMPO_ID_LIGHT_BTN,
};

typedef struct f_light_t_
{
    bool touch_flag;
    s32  move_offset;
    s32  move_offset_old;
    s8   level_old;
    s8   level_value;
} f_light_t;

#define val_shape_y  51
#define val_shepe_x  90
#define val_shape_h  178
#define val_shepe_w  60
#define make_pic_hei(val)   (val*val_shape_h/5)
#define make_pic_y(hei)     (val_shape_h-hei+val_shape_y)
#define SPACING_COUNT  (-val_shape_h/5)
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_LIGHT]);
    //亮度
    // compo_picturebox_t *light_pic = compo_picturebox_create(frm, UI_BUF_I338001_28_SET_BRIGHTNESS_00_BIN);
    // compo_setid(light_pic, COMPO_ID_LIGHT_PIC);
    // compo_picturebox_set_pos(light_pic, GUI_SCREEN_CENTER_X, 79+266/2);
    // compo_picturebox_cut(light_pic,sys_cb.light_level-1,5);

    // compo_button_t *btn = compo_button_create(frm);
    // compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 79+266/2,90,266);
    // compo_setid(btn,COMPO_ID_LIGHT_BTN);

    s16 shape_y = make_pic_y(make_pic_hei(sys_cb.light_level));
    s16 shape_h = make_pic_hei(sys_cb.light_level);
    compo_shape_t * val = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(val,val_shepe_x,shape_y+5,val_shepe_w,shape_h);
    compo_shape_set_radius(val, 20);
    widget_set_align_center(val->rect, false);
    compo_setid(val, COMPO_ID_LIGHT_PIC);

    compo_button_t *btn = compo_button_create_by_image(frm, 0);
    compo_setid(btn, COMPO_ID_LIGHT_BTN);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+5,val_shepe_w,val_shape_h);

//    btn = compo_button_create_by_image(frm, UI_BUF_I342001_28_SET_ICON_DISPLAY_AND_BRIGHTNESS_BIN);
//    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,22/2+192);

    compo_picturebox_t *pic = compo_picturebox_create(frm, 0);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X,22/2+192);

    if(func_cb.sta == FUNC_LIGHT)
    {
        f_light_t *f_light = (f_light_t *)func_cb.f_cb;
        f_light->level_old = (s8)sys_cb.light_level;
    }

    return frm;
}
static void func_light_disp_move_handle(void)
{
    f_light_t *f_light = (f_light_t *)func_cb.f_cb;
    if(f_light->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_light->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_light->touch_flag)//触摸状态
        {
            s8 level_data= (dy/SPACING_COUNT)+f_light->level_old;

            if(level_data<1)
                level_data=1;
            else if(level_data>5)
                level_data=5;

            if(level_data != f_light->level_value)
            {
                // printf("level_data:%d\n",level_data);
                f_light->level_value = level_data;
                // compo_picturebox_cut(light_pic,(uint8_t)level_data-1,5);
                s16 shape_y = make_pic_y(make_pic_hei(level_data));
                s16 shape_h = make_pic_hei(level_data);
                compo_shape_t * val = compo_getobj_byid(COMPO_ID_LIGHT_PIC);
                if(val != NULL)
                {
                    compo_shape_set_location(val,val_shepe_x,shape_y+5,val_shepe_w,shape_h);
                }
                // printf("y:%d h:%d lever:%d\n",shape_y,shape_h,level_data);
                tft_bglight_set_level((uint8_t)level_data,false);
            }
        }
        else
        {
            f_light->level_old = f_light->level_value;
        }
    }
}

#else
typedef struct f_light_t_
{
} f_light_t;
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_light_form_create(void)
{

}
//单击按钮
static void func_light_button_click(void)
{
}
static void func_light_disp_Refresh(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//亮度调节功能事件处理
static void func_light_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT \
    || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    func_light_disp_move_handle();
#endif
    func_process();
}

//亮度调节功能消息处理
static void func_light_message(size_msg_t msg)
{
    f_light_t *f_light = (f_light_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT \
    || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
            if(compo_get_button_id() == COMPO_ID_LIGHT_BTN && f_light->touch_flag == false)
            {
                f_light->touch_flag = true;
            }
#endif
            break;
        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
            func_light_button_click();
            uint8_t level_tmp = sys_cb.light_level;
            if(level_tmp < DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                level_tmp = DEFAULT_BACK_LIGHT_PERCENT_MIN / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
            }
            else if(level_tmp > DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE)
            {
                level_tmp = DEFAULT_BACK_LIGHT_PERCENT_MAX / BACK_LIGHT_PERCENT_INCREASE_OR_INCREASE;
            }
            tft_bglight_set_level(level_tmp,false);
#endif
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入亮度调节功能
static void func_light_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_light_t));
    func_cb.frm_main = func_light_form_create();
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    func_light_disp_Refresh();
#endif
}

//退出亮度调节功能
static void func_light_exit(void)
{
    f_light_t *f_light = (f_light_t *)func_cb.f_cb;
    uteModuleGuiCommonSetBackLightPercent(tft_cb.tft_bglight_duty);
    func_cb.last = FUNC_LIGHT;
}

//亮度调节功能
void func_light(void)
{
    printf("%s\n", __func__);
    func_light_enter();
    while (func_cb.sta == FUNC_LIGHT)
    {
        func_light_process();
        func_light_message(msg_dequeue());
    }
    func_light_exit();
}

#endif
