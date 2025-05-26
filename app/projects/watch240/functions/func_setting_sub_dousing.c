#include "include.h"
#include "func.h"
#include "ute_module_charencode.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define DOUSING_LIST_CNT                       ((int)(sizeof(tbl_dousing_list) / sizeof(tbl_dousing_list[0])))

enum
{
    COMPO_ID_BTN_NUM0 = 0,
    COMPO_ID_BTN_NUM1,
    COMPO_ID_BTN_NUM2,
    COMPO_ID_BTN_NUM3,
    COMPO_ID_BTN_NUM4,
    COMPO_ID_BTN_NUM5,
    COMPO_ID_BTN_NUM6,
    COMPO_ID_BTN_NUM7,
    COMPO_ID_LISTBOX,
};

typedef struct f_dousing_list_t_
{
    char str[30];
    compo_listbox_t *listbox;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    bool touch_flag;
    u8 breathe_min[5];
    s32 move_dy;
    s32 move_dy_data;
    u8 min;
    s8 num_offset;
#endif
} f_dousing_list_t;

static const compo_listbox_item_t tbl_dousing_list[] =
{
    {     .vidx = COMPO_ID_BTN_NUM0},
    {     .vidx = COMPO_ID_BTN_NUM1},
    {     .vidx = COMPO_ID_BTN_NUM2},
    {     .vidx = COMPO_ID_BTN_NUM3},
    {     .vidx = COMPO_ID_BTN_NUM4},
    {     .vidx = COMPO_ID_BTN_NUM5},
};

u8 func_sel_dousing_bit(uint n)
{
    if(sys_cb.set_sleep_time_id == n)
    {
        return true;
    }

    return false;
}

static compo_listbox_custom_item_t tbl_list_dousing[6]= {0};


char *back_string(char *num,char*txt)
{
    if(func_cb.sta == FUNC_SET_SUB_DOUSING)
    {
        f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
        memset(f_set->str,0,sizeof(f_set->str));
        uteModuleCharencodeReplaceSubString(txt, f_set->str,"##",num);
        return f_set->str;
    }
    return NULL;
}

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//熄屏设置页面
compo_form_t *func_set_sub_dousing_form_create(void)
{

    switch (uteModuleGuiCommonGetDisplayOffTime())
    {
        case 5:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM0;
            break;

        case 10:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM1;
            break;

        case 20:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM2;
            break;

        case 30:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM3;
            break;

        case 60:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM4;
            break;

        case 0:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM5;
            break;
    }

    snprintf(tbl_list_dousing[0].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("5",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[1].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("10",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[2].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("20",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[3].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("30",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[4].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("1",i18n[STR_MIN_JOINT]));
    snprintf(tbl_list_dousing[5].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",i18n[STR_LIGHT_ALWAYS]);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_DOUSING]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_dousing_list, DOUSING_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I330001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_text_modify(listbox, tbl_list_dousing);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I330001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_dousing_bit);

    compo_setid(listbox, COMPO_ID_LISTBOX);
    uint8_t set_idx = 0;
    if (set_idx < 1)
    {
        set_idx = 1;
    }

    compo_listbox_set_focus_byidx(listbox, set_idx);
    compo_listbox_update(listbox);


    return frm;
}

//点进图标进入应用
void func_set_sub_dousing_list_icon_click(void)
{
    int icon_idx;
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= DOUSING_LIST_CNT)
    {
        return;
    }
    sys_cb.set_sleep_time_id = icon_idx;
    //切入应用
    switch(icon_idx)
    {
        case  COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM6:
            if (icon_idx==COMPO_ID_BTN_NUM0)
            {
                uteModuleGuiCommonSetDisplayOffTime(5);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM1)
            {
                uteModuleGuiCommonSetDisplayOffTime(10);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM2)
            {
                uteModuleGuiCommonSetDisplayOffTime(20);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM3)
            {
                uteModuleGuiCommonSetDisplayOffTime(30);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM4)
            {
                uteModuleGuiCommonSetDisplayOffTime(60);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM5)
            {
                uteModuleGuiCommonSetDisplayOffTime(0);
            }
            compo_listbox_update(listbox);
            break;

        default:
            break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//熄屏设置页面
compo_form_t *func_set_sub_dousing_form_create(void)
{

    switch (uteModuleGuiCommonGetDisplayOffTime())
    {
        case 5:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM0;
            break;

        case 10:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM1;
            break;

        case 20:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM2;
            break;

        case 30:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM3;
            break;

        case 60:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM4;
            break;

        case 0:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM5;
            break;
    }

    snprintf(tbl_list_dousing[0].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("5",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[1].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("10",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[2].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("20",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[3].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("30",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[4].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("1",i18n[STR_MIN_JOINT]));
    snprintf(tbl_list_dousing[5].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",i18n[STR_LIGHT_ALWAYS]);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_DOUSING]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_dousing_list, DOUSING_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I332001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_text_modify(listbox, tbl_list_dousing);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I332001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_dousing_bit);

    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_focus(listbox, 80);
    compo_listbox_update(listbox);


    return frm;
}

//点进图标进入应用
void func_set_sub_dousing_list_icon_click(void)
{
    int icon_idx;
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= DOUSING_LIST_CNT)
    {
        return;
    }
    sys_cb.set_sleep_time_id = icon_idx;
    //切入应用
    switch(icon_idx)
    {
        case  COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM6:
            if (icon_idx==COMPO_ID_BTN_NUM0)
            {
                uteModuleGuiCommonSetDisplayOffTime(5);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM1)
            {
                uteModuleGuiCommonSetDisplayOffTime(10);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM2)
            {
                uteModuleGuiCommonSetDisplayOffTime(20);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM3)
            {
                uteModuleGuiCommonSetDisplayOffTime(30);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM4)
            {
                uteModuleGuiCommonSetDisplayOffTime(60);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM5)
            {
                uteModuleGuiCommonSetDisplayOffTime(0);
            }
            compo_listbox_update(listbox);
            break;

        default:
            break;
    }
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

enum
{
    COMPO_ID_BTN_SURE=1,
    COMPO_ID_TXT_1,
    COMPO_ID_TXT_2,
    COMPO_ID_TXT_3,
    COMPO_ID_TXT_4,
    COMPO_ID_TXT_5,
    COMPO_ID_BTN_MIN_BG,
};

#define TXT_SPACING    (105-62)
#define CENTER_TXT_Y   (130/2)
const static uint8_t txt_num[5] = {5, 10, 15, 30, 60}; // 固定数组
const static uint16_t DOUSING_TIME_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴
/**
 * @brief 根据当前时间和偏移量更新时间值，并生成环形排列的时间选项数组
 * @param[in,out] timer      : 当前选中的时间值（应为txt_num中的元素）
 * @param[out]    timer_data : 生成的环形时间选项数组（5个元素）
 * @param[in]     num        : 偏移量（正数为增加，负数为减少）
 */
static void func_set_sub_dousing_get_timer(uint8_t *timer, uint8_t *timer_data, s8 num)
{
    // 1. 查找当前timer在txt_num中的索引
    unsigned char current_index = 0;
    for (unsigned char i = 0; i < 5; i++)
    {
        if (txt_num[i] == *timer)
        {
            current_index = i;
            break;
        }
    }

    // 2. 应用偏移量并计算新索引（处理环形逻辑）
    int new_index = (current_index + num) % 5;
    if (new_index < 0) new_index += 5; // 处理负数索引

    // 3. 更新timer为新值
    *timer = txt_num[new_index];

    // 4. 生成环形排列的timer_data数组（以新索引为中心）
    for (signed char i = -2; i <= 2; i++)
    {
        // 计算相对于新索引的环形偏移
        unsigned char offset = (new_index + i + 5) % 5;
        // 将结果放入以索引2为中心的位置
        timer_data[i + 2] = txt_num[offset];
    }
}
//创建设置窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_set_sub_dousing_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[20];
    uint8_t txt_data[5];
    uint8_t set_time = uteModuleGuiCommonGetDisplayOffTime() ;

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_SET]);
    // printf("create_time:%d\n",set_time);
    func_set_sub_dousing_get_timer(&set_time,txt_data,0);///获取时间
    // printf("create_time:%d\n",set_time);
    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 62+130/2, GUI_SCREEN_WIDTH,130);
    for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
    {
        memset(txt_buf,0,sizeof(txt_buf));
        compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
        compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_28_BIN);
        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,DOUSING_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
        snprintf(txt_buf,sizeof(txt_buf),"%02d",txt_data[idx-COMPO_ID_TXT_1]);
        compo_setid(txt,idx);
        compo_textbox_set(txt,txt_buf);
        compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
    }

    compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_SEC]));
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X+25,DOUSING_TIME_TXT_Y[2]-8);
    compo_textbox_set(txt,i18n[STR_SEC]);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮
    compo_button_set_location(btn_min,GUI_SCREEN_CENTER_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I335001_19_BREATHING_TRAINING_2_1_SET_TIME_ICON_YES_208X52_X16_Y222_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I335001_19_BREATHING_TRAINING_2_1_SET_TIME_ICON_YES_208X52_X16_Y222_BIN).hei/2-10);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 105, 220, 1);
    compo_shape_set_color(shape,make_color(47,47,47));

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 150, 220, 1);
    compo_shape_set_color(shape,make_color(47,47,47));


    if(func_cb.sta == FUNC_SET_SUB_DOUSING)
    {
        f_dousing_list_t*f_alarm_clock_sub_set = (f_dousing_list_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_alarm_clock_sub_set->breathe_min[i]  = txt_data[i];
        }
        f_alarm_clock_sub_set->min  = set_time;
    }

    return frm;
}
/*函数功能：滑动处理*/
static void func_set_sub_dousing_sub_move(void)
{
    f_dousing_list_t *f_disturd_set = (f_dousing_list_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy = (dy%TXT_SPACING);

            if( dy != 0 && f_disturd_set->num_offset != dy/TXT_SPACING)
            {
                s8 set_offset = -(dy/TXT_SPACING)+f_disturd_set->num_offset;//获取偏移
                func_set_sub_dousing_get_timer(&f_disturd_set->min,f_disturd_set->breathe_min,set_offset);///获取时间
                f_disturd_set->num_offset = dy/TXT_SPACING;

            }
            for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
            {
                compo_textbox_t *txt = compo_getobj_byid(idx);
                memset(txt_buf,0,sizeof(txt_buf));
                compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,DOUSING_TIME_TXT_Y[idx-COMPO_ID_TXT_1]+f_disturd_set->move_dy);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->breathe_min[idx-COMPO_ID_TXT_1]);
                compo_textbox_set(txt,txt_buf);
                compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
            }
        }
        else   //松手状态
        {
            f_disturd_set->num_offset = 0;
            for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
            {
                compo_textbox_t *txt = compo_getobj_byid(idx);
                compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,DOUSING_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
            }
        }
    }
}
//单击按钮
static void func_set_sub_dousing_list_icon_click(void)
{
    f_dousing_list_t *f_disturd_set = (f_dousing_list_t*) func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
            // printf("create_time:%d\n",f_disturd_set->min);
            uteModuleGuiCommonSetDisplayOffTime(f_disturd_set->min);
            sys_cb.set_sleep_time_id = f_disturd_set->min;
            func_backing_to();
            break;
        default:
            break;
    }
}
static void func_set_sub_dousing_button_touch_handle(void)
{
    f_dousing_list_t *f_breathe_sub_time = (f_dousing_list_t*) func_cb.f_cb;
    if(f_breathe_sub_time->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_MIN_BG:
                f_breathe_sub_time->touch_flag = true;
                break;
        }
    }

}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

enum
{
    COMPO_ID_BTN_SURE=1,
    COMPO_ID_TXT_1,
    COMPO_ID_TXT_2,
    COMPO_ID_TXT_3,
    COMPO_ID_TXT_4,
    COMPO_ID_TXT_5,
    COMPO_ID_BTN_MIN_BG,
};

#define TXT_SPACING    (135-82)
#define CENTER_TXT_Y   (140/2)
const static uint8_t txt_num[5] = {5, 10, 15, 30, 60}; // 固定数组
const static uint16_t DOUSING_TIME_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴
/**
 * @brief 根据当前时间和偏移量更新时间值，并生成环形排列的时间选项数组
 * @param[in,out] timer      : 当前选中的时间值（应为txt_num中的元素）
 * @param[out]    timer_data : 生成的环形时间选项数组（5个元素）
 * @param[in]     num        : 偏移量（正数为增加，负数为减少）
 */
static void func_set_sub_dousing_get_timer(uint8_t *timer, uint8_t *timer_data, s8 num)
{
    // 1. 查找当前timer在txt_num中的索引
    unsigned char current_index = 0;
    for (unsigned char i = 0; i < 5; i++)
    {
        if (txt_num[i] == *timer)
        {
            current_index = i;
            break;
        }
    }

    // 2. 应用偏移量并计算新索引（处理环形逻辑）
    int new_index = (current_index + num) % 5;
    if (new_index < 0) new_index += 5; // 处理负数索引

    // 3. 更新timer为新值
    *timer = txt_num[new_index];

    // 4. 生成环形排列的timer_data数组（以新索引为中心）
    for (signed char i = -2; i <= 2; i++)
    {
        // 计算相对于新索引的环形偏移
        unsigned char offset = (new_index + i + 5) % 5;
        // 将结果放入以索引2为中心的位置
        timer_data[i + 2] = txt_num[offset];
    }
}
//创建设置窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_set_sub_dousing_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[20];
    u8 txt_data[5];
    u8 set_time = sys_cb.set_sleep_time_id;

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_SET]);

    func_set_sub_dousing_get_timer(&set_time,txt_data,0);///获取时间
    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20, GUI_SCREEN_WIDTH,140);
    for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
    {
        memset(txt_buf,0,sizeof(txt_buf));
        compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
        compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_38_BIN);
        compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,DOUSING_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
        snprintf(txt_buf,sizeof(txt_buf),"%02d",txt_data[idx-COMPO_ID_TXT_1]);
        compo_setid(txt,idx);
        compo_textbox_set(txt,txt_buf);
        compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
    }

    compo_textbox_t *txt = compo_textbox_create(frm,strlen(i18n[STR_SEC]));
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_location(txt,220,151,100,35);
    compo_textbox_set(txt,i18n[STR_SEC]);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮
    compo_button_set_location(btn_min,GUI_SCREEN_CENTER_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I338001_28_SET_COMFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I338001_28_SET_COMFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);


    if(func_cb.sta == FUNC_SET_SUB_DOUSING)
    {
        f_dousing_list_t*f_alarm_clock_sub_set = (f_dousing_list_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_alarm_clock_sub_set->breathe_min[i]  = txt_data[i];
        }
        f_alarm_clock_sub_set->min  = txt_data[2];
    }

    return frm;
}
/*函数功能：滑动处理*/
static void func_set_sub_dousing_sub_move(void)
{
    f_dousing_list_t *f_disturd_set = (f_dousing_list_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy = (dy%TXT_SPACING);

            if( dy != 0 && f_disturd_set->num_offset != dy/TXT_SPACING)
            {
                s8 set_offset = -(dy/TXT_SPACING)+f_disturd_set->num_offset;//获取偏移
                func_set_sub_dousing_get_timer(&f_disturd_set->min,f_disturd_set->breathe_min,set_offset);///获取时间
                f_disturd_set->num_offset = dy/TXT_SPACING;

            }
            for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
            {
                compo_textbox_t *txt = compo_getobj_byid(idx);
                memset(txt_buf,0,sizeof(txt_buf));
                compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,DOUSING_TIME_TXT_Y[idx-COMPO_ID_TXT_1]+f_disturd_set->move_dy);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->breathe_min[idx-COMPO_ID_TXT_1]);
                compo_textbox_set(txt,txt_buf);
                compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_3 ? COLOR_WHITE : COLOR_GRAY);
            }
        }
        else   //松手状态
        {
            f_disturd_set->num_offset = 0;
            for(int idx=COMPO_ID_TXT_1; idx<=COMPO_ID_TXT_5; idx++) ///创建滑动文本
            {
                compo_textbox_t *txt = compo_getobj_byid(idx);
                compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,DOUSING_TIME_TXT_Y[idx-COMPO_ID_TXT_1]);
            }
        }
    }
}
//单击按钮
static void func_set_sub_dousing_list_icon_click(void)
{
    f_dousing_list_t *f_disturd_set = (f_dousing_list_t*) func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
            uteModuleGuiCommonSetDisplayOffTime(txt_num[f_disturd_set->min]);
            sys_cb.set_sleep_time_id = f_disturd_set->min;
            func_backing_to();
            break;
        default:
            break;
    }
}
static void func_set_sub_dousing_button_touch_handle(void)
{
    f_dousing_list_t *f_breathe_sub_time = (f_dousing_list_t*) func_cb.f_cb;
    if(f_breathe_sub_time->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_MIN_BG:
                f_breathe_sub_time->touch_flag = true;
                break;
        }
    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//熄屏设置页面
compo_form_t *func_set_sub_dousing_form_create(void)
{

    switch (uteModuleGuiCommonGetDisplayOffTime())
    {
        case 5:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM0;
            break;

        case 10:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM1;
            break;

        case 20:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM2;
            break;

        case 30:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM3;
            break;

        case 60:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM4;
            break;

        case 0:
            sys_cb.set_sleep_time_id = COMPO_ID_BTN_NUM5;
            break;
    }

    snprintf(tbl_list_dousing[0].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("5",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[1].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("10",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[2].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("20",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[3].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("30",i18n[STR_SECOND_JOINT]));
    snprintf(tbl_list_dousing[4].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",back_string("1",i18n[STR_MIN_JOINT]));
    snprintf(tbl_list_dousing[5].str_txt,sizeof(tbl_list_dousing[1].str_txt),"%s",i18n[STR_LIGHT_ALWAYS]);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_DOUSING]);

    //新建列表
    compo_listbox_t *listbox = compo_listbox_create(frm, COMPO_LISTBOX_STYLE_SELECT);
    compo_listbox_set(listbox, tbl_dousing_list, DOUSING_LIST_CNT);
    compo_listbox_set_bgimg(listbox, UI_BUF_I340001_FIRSTORDER_CARD_BIN);
    compo_listbox_set_text_modify(listbox, tbl_list_dousing);
    compo_listbox_set_sta_icon(listbox, UI_BUF_I340001_PUBLIC_GOU_BIN, /*UI_BUF_COMPO_SELECT_ADD_BIN*/0);
    compo_listbox_set_bithook(listbox, func_sel_dousing_bit);

    compo_setid(listbox, COMPO_ID_LISTBOX);
    compo_listbox_set_focus(listbox, 80);
    compo_listbox_update(listbox);


    return frm;
}

//点进图标进入应用
void func_set_sub_dousing_list_icon_click(void)
{
    int icon_idx;
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
    compo_listbox_t *listbox = f_set->listbox;

    icon_idx = compo_listbox_select(listbox, ctp_get_sxy());
    if (icon_idx < 0 || icon_idx >= DOUSING_LIST_CNT)
    {
        return;
    }
    sys_cb.set_sleep_time_id = icon_idx;
    //切入应用
    switch(icon_idx)
    {
        case  COMPO_ID_BTN_NUM0...COMPO_ID_BTN_NUM6:
            if (icon_idx==COMPO_ID_BTN_NUM0)
            {
                uteModuleGuiCommonSetDisplayOffTime(5);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM1)
            {
                uteModuleGuiCommonSetDisplayOffTime(10);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM2)
            {
                uteModuleGuiCommonSetDisplayOffTime(20);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM3)
            {
                uteModuleGuiCommonSetDisplayOffTime(30);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM4)
            {
                uteModuleGuiCommonSetDisplayOffTime(60);
            }
            else if (icon_idx==COMPO_ID_BTN_NUM5)
            {
                uteModuleGuiCommonSetDisplayOffTime(0);
            }
            compo_listbox_update(listbox);
            break;

        default:
            break;
    }
}

#else
compo_form_t *func_set_sub_dousing_form_create(void)
{
}
void func_set_sub_dousing_list_icon_click(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//熄屏时长功能事件处理
static void func_set_sub_dousing_list_process(void)
{
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
#if(GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT ||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT)
    compo_listbox_move(f_set->listbox);
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    func_set_sub_dousing_sub_move();
#endif
    func_process();
}

//熄屏时长功能消息处理
static void func_set_sub_dousing_list_message(size_msg_t msg)
{
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
#if(GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT)
    compo_listbox_t *listbox = f_set->listbox;
    if (compo_listbox_message(listbox, msg))
    {
        return;                                         //处理列表框信息
    }
#endif
    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
            func_set_sub_dousing_button_touch_handle();
#endif
            break;
        case MSG_CTP_CLICK:
            func_set_sub_dousing_list_icon_click();                //单击图标
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入设置功能
static void func_set_sub_dousing_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_dousing_list_t));
    func_cb.frm_main = func_set_sub_dousing_form_create();

    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
#if(GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT)
    f_set->listbox = compo_getobj_byid(COMPO_ID_LISTBOX);
    compo_listbox_t *listbox = f_set->listbox;
    if (listbox->type != COMPO_TYPE_LISTBOX)
    {
        halt(HALT_GUI_COMPO_LISTBOX_TYPE);
    }
    listbox->mcb = func_zalloc(sizeof(compo_listbox_move_cb_t));        //建立移动控制块，退出时需要释放
#endif

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    compo_listbox_move_init_modify(listbox, 100, compo_listbox_gety_byidx(listbox, DOUSING_LIST_CNT - 2));
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    compo_listbox_move_init_modify(listbox, 80, compo_listbox_gety_byidx(listbox, DOUSING_LIST_CNT - 2)+40);
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    compo_listbox_move_init_modify(listbox, 80, compo_listbox_gety_byidx(listbox, DOUSING_LIST_CNT - 2)+40);

#endif
    func_cb.enter_tick = tick_get();
}

//退出设置功能
static void func_set_sub_dousing_exit(void)
{
    f_dousing_list_t *f_set = (f_dousing_list_t *)func_cb.f_cb;
#if(GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT)
    compo_listbox_t *listbox = f_set->listbox;
    func_free(listbox->mcb);                                            //释放移动控制块
#endif
    func_cb.last = FUNC_SET_SUB_DOUSING;
}

//设置功能
void func_set_sub_dousing(void)
{
    printf("%s\n", __func__);
    func_set_sub_dousing_enter();
    while (func_cb.sta == FUNC_SET_SUB_DOUSING)
    {
        func_set_sub_dousing_list_process();
        func_set_sub_dousing_list_message(msg_dequeue());
    }
    func_set_sub_dousing_exit();
}
