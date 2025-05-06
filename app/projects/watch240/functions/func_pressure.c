#include "include.h"
#include "func.h"
#include "ute_module_emotionPressure.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_PRESSURE_SUPPORT


#define TODAY_PRESSURE_TXT_MAX   2   //压力文本最大字数
#define CHART_NUM 28 //柱形图数量
#define CHART_100_LENGTH GUI_SCREEN_CENTER_Y*6.25/8  //柱形图框高度
#define CHART_100_VALUE 115 //柱形图对应底层图表100%数据


enum
{
    COMPO_TXT_VALUE_ID=1,
    COMPO_TXT_VALUE_MAX_ID,
    COMPO_TXT_VALUE_MIN_ID,
    COMPO_BTN_ABOUT_ID,
    COMPO_ID_AGAIN_BTN,
    COMPO_ID_PRESSURE_PIC,
    COMPO_CAHRT_1,
    COMPO_CAHRT_24 = COMPO_CAHRT_1+24,
};

typedef struct f_pressure_t_
{
    bool up_data_flag;
    page_tp_move_t *ptm;
} f_pressure_t;

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

#define Grade_1_color make_color(163,251,206)
#define Grade_2_color make_color(97,196,146)
#define Grade_3_color make_color(247,193,62)
#define Grade_4_color make_color(254,99,40)

#define Grade_1_val_min  0
#define Grade_2_val_min  30
#define Grade_3_val_min  59
#define Grade_4_val_min  80

#define mood_max   100
#define first_x    53
#define spacing_x  3
#define height_pic 69
#define first_y   (204-height_pic/2)
#define make_pic_hei(val)   (val*height_pic/mood_max)
#define make_pic_y(hei)     (height_pic-hei+first_y)

// 函数用于找出数组中除 0 和 255 之外的最大值和最小值
static void findMaxMin(uint8_t *arr, uint8_t *max, uint8_t *min)
{
    // 初始化最大值为一个极小值，最小值为一个极大值
    *max = -1;
    *min = 256;

    for (int i = 0; i < 24; i++)
    {
        if (arr[i] != 0 && arr[i] != 255)
        {
            if (arr[i] > *max)
            {
                *max = arr[i];
            }
            if (arr[i] < *min)
            {
                *min = arr[i];
            }
        }
    }
}
//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{
    char txt_buf[20];
    uint8_t test_date[24];
    uint8_t max_val=0;
    uint8_t min_val=0;

    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);
    findMaxMin(test_date,&max_val,&min_val);
    u8 Pressure_val = uteModuleEmotionPressureGetPressureValue();

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STRESS]);

    ///设置图片
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I335001_16_PRESSURE_1_1_PRESSURE_STRESS_ICON_PIC48X48_X10_Y62_00_BIN);
    compo_animation_set_pos(animation,14+52/2, 54+52/2);
    compo_animation_set_radix(animation, 11);
    compo_animation_set_interval(animation, 0);
    compo_setid(animation,COMPO_ID_PRESSURE_PIC);

    compo_textbox_t *textbox;
    if(Pressure_val!=0 && Pressure_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,74,65);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,14,114,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(max_val!=0 && max_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",max_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_setid(textbox,COMPO_TXT_VALUE_MAX_ID);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,20+txt_leng.wid,114);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,120,114,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(min_val!=0 && min_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",min_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_setid(textbox,COMPO_TXT_VALUE_MIN_ID);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,126+txt_leng.wid,114);
    compo_textbox_set_align_center(textbox, false);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I335001_7_SPO2_1_ICON_PLAY_44X44_X186_Y65_01_BIN);///重新测量按钮
    compo_button_set_pos(btn,22+184,22+58);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_6_HEART_1_1_ICON_ABOUT_18X18_X176_Y299_BIN);//////关于
    compo_picturebox_set_pos(picbox,158,5+66);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,158,5+66,30,30);///关于
    compo_setid(btn,COMPO_BTN_ABOUT_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_16_PRESSURE_1_1_PRESSURE_ICON_BG_DATA_224X110_X8_Y162_BIN);
    compo_picturebox_set_pos(picbox, 120,148+60);


    for (int i = 0; i < 24; i++)
    {
        compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE );
        widget_set_align_center(shape->rect, false);
        compo_shape_set_location(shape,first_x+i*spacing_x+i*4,make_pic_y(make_pic_hei(test_date[i])),4,had_date?make_pic_hei(test_date[i]):0);
        compo_setid(shape,COMPO_CAHRT_1+i);
        switch (test_date[i])
        {
            case Grade_1_val_min...Grade_2_val_min-1:
                compo_shape_set_color(shape,Grade_1_color);
                break;
            case Grade_2_val_min...Grade_3_val_min-1:
                compo_shape_set_color(shape,Grade_2_color);
                break;
            case Grade_3_val_min...Grade_4_val_min-1:
                compo_shape_set_color(shape,Grade_3_color);
                break;
            case Grade_4_val_min...100:
                compo_shape_set_color(shape,Grade_4_color);
                break;

            default:
                break;
        }
    }

    return frm;

}

//触摸单击按钮
static void func_pressure_button_click(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_BTN_ABOUT_ID:
            uteTaskGuiStartScreen(FUNC_PRESSURE_EXPLAIN, 0, __func__);
            break;
        case COMPO_ID_AGAIN_BTN:
            if(f_pressure->up_data_flag)
            {
                uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_INITIATIVE_QUIT);
            }
            else
            {
                uteModuleEmotionPressureStartSingleTesting(false);
            }
            break;
    }

}

//刷新压力数据
static void func_pressure_refresh(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;

    if(f_pressure->up_data_flag && !uteModuleEmotionPressureIsWear() && !uteModuleEmotionPressureIsTesting())
    {
        u8 res = msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
        if (res == MSGBOX_RES_OK)
        {
            // uteModuleEmotionPressureStartSingleTesting(false);
        }
    }

    if(f_pressure->up_data_flag != uteModuleEmotionPressureIsTesting())
    {
        f_pressure->up_data_flag = uteModuleEmotionPressureIsTesting();

        char txt_buf[20];
        uint8_t test_date[24];
        uint8_t max_val=0;
        uint8_t min_val=0;
        compo_textbox_t *val = compo_getobj_byid(COMPO_TXT_VALUE_ID);
        compo_textbox_t *val_max = compo_getobj_byid(COMPO_TXT_VALUE_MAX_ID);
        compo_textbox_t *val_min = compo_getobj_byid(COMPO_TXT_VALUE_MIN_ID);

        bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date,24);
        findMaxMin(test_date,&max_val,&min_val);
        u8 Pressure_val = uteModuleEmotionPressureGetPressureValue();

        memset(txt_buf,0,sizeof(txt_buf));
        if(Pressure_val!=0 && Pressure_val!=255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--");
        }
        compo_textbox_set(val,txt_buf);

        memset(txt_buf,0,sizeof(txt_buf));
        if(max_val!=0 && max_val!=255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",max_val);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--");
        }
        compo_textbox_set(val_max,txt_buf);

        memset(txt_buf,0,sizeof(txt_buf));
        if(min_val!=0 && min_val!=255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",min_val);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--");
        }
        compo_textbox_set(val_min,txt_buf);


        compo_animation_t *animation = compo_getobj_byid(COMPO_ID_PRESSURE_PIC);
        compo_animation_set_interval(animation, f_pressure->up_data_flag?20:0);

        compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
        compo_button_set_bgimg(btn, f_pressure->up_data_flag ? UI_BUF_I335001_7_SPO2_1_ICON_PLAY_44X44_X186_Y65_00_BIN:UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_ICON_PLAY_44X44_X180_Y62_01_BIN);

        for (int i = 0; i < 24; i++)
        {
            u8 mood_mode = test_date[i];
            compo_shape_t *shape = compo_getobj_byid(COMPO_CAHRT_1+i);
            compo_shape_set_location(shape,first_x+i*spacing_x+i*4,make_pic_y(make_pic_hei(test_date[i])),4,had_date?make_pic_hei(test_date[i]):0);
            switch (test_date[i])
            {
                case Grade_1_val_min...Grade_2_val_min-1:
                    compo_shape_set_color(shape,Grade_1_color);
                    break;
                case Grade_2_val_min...Grade_3_val_min-1:
                    compo_shape_set_color(shape,Grade_2_color);
                    break;
                case Grade_3_val_min...Grade_4_val_min-1:
                    compo_shape_set_color(shape,Grade_3_color);
                    break;
                case Grade_4_val_min...100:
                    compo_shape_set_color(shape,Grade_4_color);
                    break;

                default:
                    break;
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

#define Grade_1_color make_color(163,251,206)
#define Grade_2_color make_color(97,196,146)
#define Grade_3_color make_color(247,193,62)
#define Grade_4_color make_color(254,99,40)

#define Grade_1_val_min  0
#define Grade_2_val_min  30
#define Grade_3_val_min  59
#define Grade_4_val_min  80

#define mood_max   100
#define first_x    95
#define spacing_x  4
#define height_pic 93
#define first_y   (233)
#define make_pic_hei(val)   (val*height_pic/mood_max)
#define make_pic_y(hei)     (height_pic-hei+first_y)

// 函数用于找出数组中除 0 和 255 之外的最大值和最小值
static void findMaxMin(uint8_t *arr, uint8_t *max, uint8_t *min)
{
    // 初始化最大值为一个极小值，最小值为一个极大值
    *max = -1;
    *min = 256;

    for (int i = 0; i < 24; i++)
    {
        if (arr[i] != 0 && arr[i] != 255)
        {
            if (arr[i] > *max)
            {
                *max = arr[i];
            }
            if (arr[i] < *min)
            {
                *min = arr[i];
            }
        }
    }
}
//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{
    char txt_buf[20];
    uint8_t test_date[24];
    uint8_t max_val=0;
    uint8_t min_val=0;

    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);
    findMaxMin(test_date,&max_val,&min_val);
    u8 Pressure_val = uteModuleEmotionPressureGetPressureValue();

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STRESS]);

    ///设置图片
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I338001_16_PRESSURE_GIF_BIN);
    compo_animation_set_pos(animation,37+64/2, 82+64/2);
    compo_animation_set_radix(animation, 16);
    compo_animation_set_interval(animation, 0);
    compo_setid(animation,COMPO_ID_PRESSURE_PIC);

    compo_textbox_t *textbox;
    if(Pressure_val!=0 && Pressure_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3 );/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,112,89);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,40,180,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(max_val!=0 && max_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",max_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_setid(textbox,COMPO_TXT_VALUE_MAX_ID);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,50+txt_leng.wid,180);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,186,180,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(min_val!=0 && min_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",min_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_setid(textbox,COMPO_TXT_VALUE_MIN_ID);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,196+txt_leng.wid,180);
    compo_textbox_set_align_center(textbox, false);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I338001_7_SPO2_PAUSE_BIN);///重新测量按钮
    compo_button_set_pos(btn,58/2+256,58/2+79);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338001_7_SPO2_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,190,89);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,190,89,30,30);///关于
    compo_setid(btn,COMPO_BTN_ABOUT_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I338001_16_PRESSURE_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,150/2+208);


    for (int i = 0; i < 24; i++)
    {
        // had_date = true;
        // test_date[i] = 100;
        compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE );
        widget_set_align_center(shape->rect, false);
        compo_shape_set_location(shape,first_x+i*spacing_x+i*5,make_pic_y(make_pic_hei(test_date[i])),5,had_date?make_pic_hei(test_date[i]):0);
        compo_setid(shape,COMPO_CAHRT_1+i);
        switch (test_date[i])
        {
            case Grade_1_val_min...Grade_2_val_min-1:
                compo_shape_set_color(shape,Grade_1_color);
                break;
            case Grade_2_val_min...Grade_3_val_min-1:
                compo_shape_set_color(shape,Grade_2_color);
                break;
            case Grade_3_val_min...Grade_4_val_min-1:
                compo_shape_set_color(shape,Grade_3_color);
                break;
            case Grade_4_val_min...100:
                compo_shape_set_color(shape,Grade_4_color);
                break;

            default:
                break;
        }
    }

    return frm;

}

//触摸单击按钮
static void func_pressure_button_click(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_BTN_ABOUT_ID:
            uteTaskGuiStartScreen(FUNC_PRESSURE_EXPLAIN, 0, __func__);
            break;
        case COMPO_ID_AGAIN_BTN:
            if(f_pressure->up_data_flag)
            {
                uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_INITIATIVE_QUIT);
            }
            else
            {
                uteModuleEmotionPressureStartSingleTesting(false);
            }
            break;
    }

}

//刷新压力数据
static void func_pressure_refresh(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;

    if(f_pressure->up_data_flag && !uteModuleEmotionPressureIsWear() && !uteModuleEmotionPressureIsTesting())
    {
        u8 res = msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
        if (res == MSGBOX_RES_OK)
        {
            // uteModuleEmotionPressureStartSingleTesting(false);
        }
    }

    if(f_pressure->up_data_flag != uteModuleEmotionPressureIsTesting())
    {
        f_pressure->up_data_flag = uteModuleEmotionPressureIsTesting();

        char txt_buf[20];
        uint8_t test_date[24];
        uint8_t max_val=0;
        uint8_t min_val=0;
        compo_textbox_t *val = compo_getobj_byid(COMPO_TXT_VALUE_ID);
        compo_textbox_t *val_max = compo_getobj_byid(COMPO_TXT_VALUE_MAX_ID);
        compo_textbox_t *val_min = compo_getobj_byid(COMPO_TXT_VALUE_MIN_ID);

        bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date,24);
        findMaxMin(test_date,&max_val,&min_val);
        u8 Pressure_val = uteModuleEmotionPressureGetPressureValue();

        memset(txt_buf,0,sizeof(txt_buf));
        if(Pressure_val!=0 && Pressure_val!=255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--");
        }
        compo_textbox_set(val,txt_buf);

        memset(txt_buf,0,sizeof(txt_buf));
        if(max_val!=0 && max_val!=255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",max_val);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--");
        }
        compo_textbox_set(val_max,txt_buf);

        memset(txt_buf,0,sizeof(txt_buf));
        if(min_val!=0 && min_val!=255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",min_val);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--");
        }
        compo_textbox_set(val_min,txt_buf);


        compo_animation_t *animation = compo_getobj_byid(COMPO_ID_PRESSURE_PIC);
        compo_animation_set_radix(animation, 16);
        compo_animation_set_interval(animation, f_pressure->up_data_flag?20:0);

        compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
        compo_button_set_bgimg(btn, f_pressure->up_data_flag ? UI_BUF_I338001_16_PRESSURE_END_BIN:UI_BUF_I338001_16_PRESSURE_RETRY_BIN);

        for (int i = 0; i < 24; i++)
        {
            // had_date = true;
            // test_date[i] = 100;
            compo_shape_t *shape = compo_getobj_byid(COMPO_CAHRT_1+i);
            compo_shape_set_location(shape,first_x+i*spacing_x+i*5,make_pic_y(make_pic_hei(test_date[i])),5,had_date?make_pic_hei(test_date[i]):0);
            switch (test_date[i])
            {
                case Grade_1_val_min...Grade_2_val_min-1:
                    compo_shape_set_color(shape,Grade_1_color);
                    break;
                case Grade_2_val_min...Grade_3_val_min-1:
                    compo_shape_set_color(shape,Grade_2_color);
                    break;
                case Grade_3_val_min...Grade_4_val_min-1:
                    compo_shape_set_color(shape,Grade_3_color);
                    break;
                case Grade_4_val_min...100:
                    compo_shape_set_color(shape,Grade_4_color);
                    break;

                default:
                    break;
            }
        }
    }
}
#else
//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{


}

//触摸单击按钮
static void func_pressure_button_click(void)
{

}

//刷新压力数据
static void func_pressure_refresh(void)
{

}
#endif
//压力功能消息处理
static void func_pressure_process(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
    if(f_pressure->ptm)
    {
        compo_page_move_process(f_pressure->ptm);
    }
    if(uteModuleEmotionPressureIsTesting())
    {
        reset_guioff_delay();
    }
    func_pressure_refresh();
    func_process();
}

static void func_pressure_message(size_msg_t msg)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_pressure->ptm)
            {
                compo_page_move_touch_handler(f_pressure->ptm);
            }
            break;
        case MSG_CTP_CLICK:
            func_pressure_button_click();
            break;

        default:
            func_message(msg);
            break;
    }
}
//进入压力功能
static void func_pressure_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_pressure_t));
    func_cb.frm_main = func_pressure_form_create();

    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_pressure->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  430,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_pressure->ptm, func_cb.frm_main->page_body, &info);
#endif
}

//退出压力功能
static void func_pressure_exit(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
    if (f_pressure->ptm)
    {
        func_free(f_pressure->ptm);
    }
    if(uteModuleEmotionPressureIsTesting())
    {
        uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_INITIATIVE_QUIT);
    }
    func_cb.last = FUNC_PRESSURE;
}
//压力功能
void func_pressure(void)
{
    printf("%s\n", __func__);
    func_pressure_enter();
    while (func_cb.sta == FUNC_PRESSURE)
    {
        func_pressure_process();
        func_pressure_message(msg_dequeue());
    }
    func_pressure_exit();
}


#endif
