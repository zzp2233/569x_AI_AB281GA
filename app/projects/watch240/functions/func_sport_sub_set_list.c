#include "include.h"
#include "func.h"
#include "ute_all_sports_int_algorithms.h"
#include "ute_module_sport.h"
#include "ute_drv_battery_common.h"
#include "func_cover.h"
#include "ute_drv_motor.h"
#include "ute_module_heart.h"
#include "ute_module_sportTarget.h"
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SPORT_SET_SUPPORT

#define TXT_SPACING    (92)
#define CENTER_TXT_Y   (170 - TXT_SPACING + 66/2)
#define KACL_NUM 20
static const int kcal_num[KACL_NUM] = {50,100,150,200,250, 300, 350, 400, 450,500,550,600,650,700,750,800,850,900,950,1000}; // 固定数组
#define DANS_NUM 22
static const uint16_t SPORT_TIME_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

enum
{
    COMPO_ID_BTN_SURE=1,
    COMPO_ID_TXT_1,
    COMPO_ID_TXT_2,
    COMPO_ID_TXT_3,
    COMPO_ID_TXT_4,
    COMPO_ID_TXT_5,
    COMPO_ID_TXT_6,
    COMPO_ID_BTN_MIN_BG,
    COMPO_ID_TXT_UINT_KM,
};
typedef struct f_sport_sub_set_list_t_
{
    bool touch_flag;
    int breathe_min[5];
    s32 move_dy;
    s32 move_dy_data;
    int min;
    int min_old;
    int dec;
    s8 num_offset;
} f_sport_sub_set_list_t;

typedef enum SET_MODE_T
{
    SET_TIME_MODE = 1,
    SET_KCAL_MODE,
    SET_DANS_MODE,
} SET_MODE;

enum
{
    TIME_PAGE,
    KCAL_PAGE,
    DANS_PAGE,
};

// 函数功能：根据timer和num偏移量更新timer，并生成环形排列的timer_data数组
static void func_sport_taget_get_timer(int timer, int *timer_data)
{
    s8 num=-10;
    for(int i=0; i<5; i++) ///前后数据处理
    {
        timer_data[i]  = (timer + 355 + (num)) % 355 + 10;
        timer_data[i] = (timer_data[i] <= 10) ? 10 : timer_data[i];
        num+=5;
    }
}

// 函数功能：根据timer和num偏移量更新timer，并生成环形排列的timer_data数组
static void func_sport_taget_get_kcal(int *kcal, int *kcal_data, s8 num)
{
    // 1. 计算偏移后的索引
    int new_index = (*kcal + num) % KACL_NUM;
    if (new_index < 0)
    {
        new_index += KACL_NUM;
    }
    // 2. 更新timer指向的值
    *kcal = new_index;
    // 3. 生成环形排列的timer_data数组
    for (int i = 0; i < 5; i++)
    {
        int idx = (new_index - 2 + i) % KACL_NUM;
        if (idx < 0)
        {
            idx += KACL_NUM;
        }
        kcal_data[i] = kcal_num[idx];
    }
}

// 函数功能：根据timer和num偏移量更新timer，并生成环形排列的timer_data数组
static void func_sport_taget_get_dans_t(int dans, int *dans_data)
{
    s8 num=-10;
    for(int i=0; i<5; i++) ///前后数据处理
    {
        dans_data[i]  = (dans + 205 + (num)) % 205 + 10;
        dans_data[i] = (dans_data[i] <= 10) ? 10 : dans_data[i];
        num+=5;
    }
}

static u8 page_type = 0;
static compo_form_t* set_time_list_creat(void)
{
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[20];
    int txt_data[5];
    int set_time = uteModuleSportTargetGetTimeGoal() / 60 - 10;
    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CUSTOM_TIME]);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X, 156,320, 1,0,0xff,make_color(47,47,47));

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X, 156 + TXT_SPACING,320, 1,0,0xff,make_color(47,47,47));

    func_sport_taget_get_timer(set_time,txt_data);///获取时间
    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 75+252/2, GUI_SCREEN_WIDTH,252);
    for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
    {
        memset(txt_buf,0,sizeof(txt_buf));
        compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,3);
        compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_54_BIN);
        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
        snprintf(txt_buf,sizeof(txt_buf),"%d",txt_data[idx-COMPO_ID_TXT_1]);
        compo_setid(txt,idx);
        compo_textbox_set(txt,txt_buf);
        compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
    }

    compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_MINUTE]));
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X + 60,SPORT_TIME_TXT_Y[2]);
    compo_textbox_set(txt,i18n[STR_MINUTE]);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮
    compo_button_set_location(btn_min,GUI_SCREEN_CENTER_X,75+252/2,200,252);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_28_SET_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I341001_28_SET_CONFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);


    if(func_cb.sta == FUNC_SPORT_SUB_SET_LIST)
    {
        f_sport_sub_set_list_t*f_sport_sub_set_list = (f_sport_sub_set_list_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_sport_sub_set_list->breathe_min[i]  = txt_data[i];
        }
        f_sport_sub_set_list->min = f_sport_sub_set_list->breathe_min[2];
        f_sport_sub_set_list->min_old = f_sport_sub_set_list->breathe_min[0];
    }
    page_type = TIME_PAGE;
    return frm;
}

static compo_form_t* set_kcal_list_creat(void)
{
    char txt_buf[20];
    int txt_data[5];
    int taget_kcal = (uteModuleSportTargetGetKcalGoal() - 50) / 50;
    compo_form_t *frm = compo_form_create(true);
    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CALORIE]);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X, 156,320, 1,0,0xff,make_color(47,47,47));

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X, 156 + TXT_SPACING,320, 1,0,0xff,make_color(47,47,47));

    func_sport_taget_get_kcal(&taget_kcal,txt_data,0);///获取时间
    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 75+252/2, GUI_SCREEN_WIDTH,252);
    for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
    {
        memset(txt_buf,0,sizeof(txt_buf));
        compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,4);
        compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_54_BIN);
        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
        snprintf(txt_buf,sizeof(txt_buf),"%d",txt_data[idx-COMPO_ID_TXT_1]);
        compo_setid(txt,idx);
        compo_textbox_set(txt,txt_buf);
        compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
    }

    compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_KCAL]));
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X+70,SPORT_TIME_TXT_Y[2]);
    compo_textbox_set(txt,i18n[STR_KCAL]);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮
    compo_button_set_location(btn_min,GUI_SCREEN_CENTER_X,75+252/2,200,252);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_28_SET_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I341001_28_SET_CONFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);


    if(func_cb.sta == FUNC_SPORT_SUB_SET_LIST)
    {
        f_sport_sub_set_list_t*f_sport_sub_set_list = (f_sport_sub_set_list_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_sport_sub_set_list->breathe_min[i]  = txt_data[i];
        }
        f_sport_sub_set_list->min = (f_sport_sub_set_list->breathe_min[2] - 50 )/ 50;
        f_sport_sub_set_list->min_old = f_sport_sub_set_list->breathe_min[0];
    }

    page_type = KCAL_PAGE;
    return frm;
}
static compo_form_t* set_dans_list_creat(void)
{
    char txt_buf[20];
    int txt_data[5];
    int dans_t = (uteModuleSportTargetGetDistanceGoal() / 100 - 10) < 0 ? 0 : ( uteModuleSportTargetGetDistanceGoal() / 100 - 10);
    if(dans_t > 190 && dans_t < 300)
    {
        dans_t = 195;
    }
    else if(dans_t > 380)
    {
        dans_t = 200;
    }
    compo_form_t *frm = compo_form_create(true);
    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_DISTANCE]);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X, 156,320, 1,0,0xff,make_color(47,47,47));

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    ute_compo_shape_set(shape,GUI_SCREEN_CENTER_X, 156 + TXT_SPACING,320, 1,0,0xff,make_color(47,47,47));

    func_sport_taget_get_dans_t(dans_t,txt_data);

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 75+252/2, GUI_SCREEN_WIDTH,252);
    for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
    {
        memset(txt_buf,0,sizeof(txt_buf));
        compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,20);
        if(txt_data[idx-COMPO_ID_TXT_1] == 205)
        {
            compo_textbox_set_font(txt,0);
            compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1] + 14);
            snprintf(txt_buf,sizeof(txt_buf),"%s",i18n[STR_MARATHON]);
        }
        else if(txt_data[idx-COMPO_ID_TXT_1] == 210)
        {
            compo_textbox_set_font(txt,0);
            compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1] + 14);
            snprintf(txt_buf,sizeof(txt_buf),"%s",i18n[STR_HALF_MARATHON]);
        }
        else
        {
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_54_BIN);
            compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
            snprintf(txt_buf,sizeof(txt_buf),"%d.%d",txt_data[idx-COMPO_ID_TXT_1]/10,txt_data[idx-COMPO_ID_TXT_1]%10);
        }

        compo_setid(txt,idx);
        compo_textbox_set(txt,txt_buf);
        compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
    }
    bool unit_vis = (txt_data[2] > 200) ? false : true;
    compo_textbox_t *txt = compo_textbox_create(frm,strlen(i18n[STR_KM]));
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X + 50,SPORT_TIME_TXT_Y[2] + 75);
    compo_textbox_set(txt,i18n[STR_KM]);
    compo_textbox_set_visible(txt,unit_vis);
    compo_setid(txt,COMPO_ID_TXT_UINT_KM);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮
    compo_button_set_location(btn_min,GUI_SCREEN_CENTER_X,75+252/2,200,252);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_28_SET_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I341001_28_SET_CONFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    if(func_cb.sta == FUNC_SPORT_SUB_SET_LIST)
    {
        f_sport_sub_set_list_t*f_sport_sub_set_list = (f_sport_sub_set_list_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_sport_sub_set_list->breathe_min[i]  = txt_data[i];
        }
        f_sport_sub_set_list->min = f_sport_sub_set_list->breathe_min[2];
        f_sport_sub_set_list->min_old = f_sport_sub_set_list->breathe_min[0];
    }

    page_type = DANS_PAGE;
    return frm;
}

compo_form_t *func_sport_sub_set_list_form_create(void)
{
    compo_form_t *frm;
    uteModuleSportTargetMoreGetMoreFromIndex(uteModuleSportTargetGetShowIndex() - 1);
    switch(sport_get_mode())
    {
        case SET_TIME_MODE:
            frm = set_time_list_creat();
            break;
        case SET_KCAL_MODE:
            frm = set_kcal_list_creat();
            break;
        case SET_DANS_MODE:
            frm = set_dans_list_creat();
            break;
    }
    return frm;
}

/*函数功能：滑动处理*/
static void func_sport_set_time_move(void)
{
    f_sport_sub_set_list_t*f_sport_sub_set_list = (f_sport_sub_set_list_t*)func_cb.f_cb;

    if(f_sport_sub_set_list->touch_flag == true)
    {
        s16 txt_buf[20];
        s32 dx, dy;
        f_sport_sub_set_list->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_sport_sub_set_list->touch_flag == true)//触摸状态
        {
            f_sport_sub_set_list->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_sport_sub_set_list->move_dy = dy-f_sport_sub_set_list->move_dy_data;

            if( dy != 0 && f_sport_sub_set_list->num_offset != dy/TXT_SPACING)
            {
                s8 set_offset = -(dy/TXT_SPACING)+f_sport_sub_set_list->num_offset;//获取偏移
                if(page_type == TIME_PAGE)
                {
                    f_sport_sub_set_list->min = (355 + f_sport_sub_set_list->min_old-(f_sport_sub_set_list->move_dy_data/TXT_SPACING) * 5)%355;
                    func_sport_taget_get_timer(f_sport_sub_set_list->min,f_sport_sub_set_list->breathe_min);///获取时间
                }
                else if (page_type == KCAL_PAGE)
                {
                    func_sport_taget_get_kcal(&f_sport_sub_set_list->min,f_sport_sub_set_list->breathe_min,set_offset);///获取时间
                    f_sport_sub_set_list->min = (f_sport_sub_set_list->breathe_min[2] - 50 )/ 50;
                }
                else
                {
                    f_sport_sub_set_list->min = (205 + f_sport_sub_set_list->min_old-(f_sport_sub_set_list->move_dy_data/TXT_SPACING) * 5)%205;
                    func_sport_taget_get_dans_t(f_sport_sub_set_list->min,f_sport_sub_set_list->breathe_min);
                }
                f_sport_sub_set_list->num_offset = dy/TXT_SPACING;
            }
            for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
            {
                compo_textbox_t *txt = compo_getobj_byid(idx);
                compo_textbox_t *uint_txt = compo_getobj_byid(COMPO_ID_TXT_UINT_KM);
                memset(txt_buf,0,sizeof(txt_buf));
                if (page_type == DANS_PAGE)
                {
                    if(f_sport_sub_set_list->breathe_min[idx-COMPO_ID_TXT_1] == 205)
                    {
                        compo_textbox_set_font(txt,0);
                        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1] + 14+f_sport_sub_set_list->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%s",i18n[STR_MARATHON]);
                    }
                    else if(f_sport_sub_set_list->breathe_min[idx-COMPO_ID_TXT_1] == 210)
                    {
                        compo_textbox_set_font(txt,0);
                        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1] + 14+f_sport_sub_set_list->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%s",i18n[STR_HALF_MARATHON]);
                    }
                    else
                    {
                        compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_54_BIN);
                        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1] + f_sport_sub_set_list->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%d.%d",f_sport_sub_set_list->breathe_min[idx-COMPO_ID_TXT_1] / 10,f_sport_sub_set_list->breathe_min[idx-COMPO_ID_TXT_1] % 10);
                    }
                    bool unit_vis = (f_sport_sub_set_list->breathe_min[2] > 200) ? false : true;
                    compo_textbox_set_visible(uint_txt,unit_vis);
                }
                else
                {
                    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1]+f_sport_sub_set_list->move_dy);
                    snprintf(txt_buf,sizeof(txt_buf),"%d",f_sport_sub_set_list->breathe_min[idx-COMPO_ID_TXT_1]);
                }
                compo_textbox_set(txt,txt_buf);
                compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
            }
        }
        else   //松手状态
        {
            f_sport_sub_set_list->num_offset = 0;
            f_sport_sub_set_list->min_old = f_sport_sub_set_list->min;
            for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
            {
                compo_textbox_t *txt = compo_getobj_byid(idx);
                if (page_type == DANS_PAGE)
                {
                    if(f_sport_sub_set_list->breathe_min[idx-COMPO_ID_TXT_1] == 205 || f_sport_sub_set_list->breathe_min[idx-COMPO_ID_TXT_1] == 210)
                    {
                        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1] + 14);
                    }
                    else
                    {
                        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
                    }
                }
                else
                {
                    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,SPORT_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
                }
            }
        }
    }
}
static void func_sport_sub_set_list_process(void)
{
    switch(page_type)
    {
        case TIME_PAGE:
        case KCAL_PAGE:
        case DANS_PAGE:
            func_sport_set_time_move();
            break;

    }
    func_process();
}
static void func_sport_set_time_touch_handle(void)
{
    f_sport_sub_set_list_t*f_sport_sub_set_list = (f_sport_sub_set_list_t*)func_cb.f_cb;
    if(f_sport_sub_set_list->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_MIN_BG:
                f_sport_sub_set_list->touch_flag = true;
                break;
        }
    }

}

static void data_clean()
{
    f_sport_sub_set_list_t*f_sport_sub_set_list = (f_sport_sub_set_list_t*)func_cb.f_cb;
    memset(f_sport_sub_set_list,0, sizeof(f_sport_sub_set_list_t));
}

static void func_sport_set_icon_click()
{
    f_sport_sub_set_list_t*f_sport_sub_set_list = (f_sport_sub_set_list_t*)func_cb.f_cb;
    switch(page_type)
    {
        case TIME_PAGE:
            f_sport_sub_set_list->min = f_sport_sub_set_list->breathe_min[2];
            uteModuleSportTargetSetTimeGoal((f_sport_sub_set_list->min)*60);
            break;
        case KCAL_PAGE:
            uteModuleSportTargetSetKcalGoal(f_sport_sub_set_list->min * 50 + 50);
            break;
        case DANS_PAGE:
            f_sport_sub_set_list->min = f_sport_sub_set_list->breathe_min[2];
            if(f_sport_sub_set_list->min == 205)
            {
                uteModuleSportTargetSetDistanceGoal(21097);
            }
            else if(f_sport_sub_set_list->min == 210)
            {
                uteModuleSportTargetSetDistanceGoal(42195);
            }
            else
            {
                f_sport_sub_set_list->min = f_sport_sub_set_list->min * 100;
                uteModuleSportTargetSetDistanceGoal(f_sport_sub_set_list->min);
            }
            break;

    }
    data_clean();
    func_cb.sta = FUNC_SPORT_SUB_SET;
}
u8 sport_target_page = 0;
static void func_sport_sub_set_list_message(size_msg_t msg)
{
    f_sport_sub_set_list_t*f_sport_sub_set_list = (f_sport_sub_set_list_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_sport_set_time_touch_handle();
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
            sport_target_page = 1;
            func_switch_to(FUNC_SPORT_SUB_SET, FUNC_SWITCH_DIRECT);
            break;
        case MSG_CTP_SHORT_RIGHT:
            sport_target_page = 1;
            func_message(msg);
            break;
        case MSG_CTP_CLICK:
            if(id == COMPO_ID_BTN_SURE)
            {
                func_sport_set_icon_click();                //单击图标
            }
            break;
        default:
            func_message(msg);
            break;
    }
}


static void func_sport_sub_set_list_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sport_sub_set_list_t));
    func_cb.frm_main = func_sport_sub_set_list_form_create();

}

//退出运动功能
static void func_sport_sub_set_list_exit(void)
{
    f_sport_sub_set_list_t *f_sport = (f_sport_sub_set_list_t *)func_cb.f_cb;
    func_cb.last = FUNC_SPORT_SUB_SET_LIST;
}
//运动功能
void func_sport_sub_set_list(void)
{
    printf("%s\n", __func__);
    func_sport_sub_set_list_enter();
    while (func_cb.sta == FUNC_SPORT_SUB_SET_LIST)
    {
        func_sport_sub_set_list_process();
        func_sport_sub_set_list_message(msg_dequeue());
    }
    func_sport_sub_set_list_exit();
}
#endif //UTE_MODULE_SCREENS_SPORT_SET_SUPPORT