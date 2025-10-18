#include "include.h"
#include "func.h"
#include "ute_module_emotionPressure.h"
#include "ute_module_heart.h"
#include "ute_drv_motor.h"
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
    COMPO_ID_PIC_ABOUT,
    COMPO_ID_AGAIN_BTN,
    COMPO_ID_PRESSURE_PIC,
    COMPO_ID_TXT_TEST,
    COMPO_CAHRT_1,
    COMPO_CAHRT_24 = COMPO_CAHRT_1+24,

    COMPO_ID_PIC_BG,
    COMPO_ID_ABOUT_PIC,
    COMPO_ID_ABOUT_BTN,
    COMPO_ID_MAX_VLA,
    COMPO_ID_MIN_VLA,
    COMPO_ID_TXT_TMEE_TIP,
    COMPO_ID_TXT_TMEE,
    COMPO_ID_TXT_VALUE,
    COMPO_ID_TXT_START,
    COMPO_ID_BTN_START,
    COMPO_ID_PIC_MAX,
    COMPO_ID_PIC_MIN,
    COMPO_ID_PIC_CHART,
    COMPO_ID_TXT_NODATE,

    COMPO_ID_PIC_GIF,
    COMPO_ID_TXT_TIP,

    COMPO_ID_TITLE_TIME,
    COMPO_ID_TITLE,

    COMPO_ID_TXT_MES_FAIL,

    COMPO_ID_TXT_WEAR_FAIL,

    COMPO_ID_BTN_RSTART,
    COMPO_ID_CHART_VALUE,
};

typedef struct f_pressure_t_
{
    bool up_data_flag;
    page_tp_move_t *ptm;
    u32 tick;
    u8 press_state;
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

//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{
    char txt_buf[20];
    uint8_t test_date[24];
    uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
    uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();

    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);
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

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I335001_7_SPO2_1_ICON_PLAY_44X44_X186_Y65_00_BIN);///重新测量按钮
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
        // test_date[i] = 50;
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
        u8 msg_flag = msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
        if(msg_flag==MSGBOX_RES_OK)
        {
            uteModuleEmotionPressureStartSingleTesting(false);
        }
        else
        {
        }

    }

    if(f_pressure->up_data_flag != uteModuleEmotionPressureIsTesting())
    {
        f_pressure->up_data_flag = uteModuleEmotionPressureIsTesting();

        char txt_buf[20];
        uint8_t test_date[24];
        uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
        uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();
        compo_textbox_t *val = compo_getobj_byid(COMPO_TXT_VALUE_ID);
        compo_textbox_t *val_max = compo_getobj_byid(COMPO_TXT_VALUE_MAX_ID);
        compo_textbox_t *val_min = compo_getobj_byid(COMPO_TXT_VALUE_MIN_ID);

        bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date,24);
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
        compo_button_set_bgimg(btn, UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_ICON_PLAY_44X44_X180_Y62_01_BIN);
        compo_button_set_visible(btn,f_pressure->up_data_flag ? false : true);

        for (int i = 0; i < 24; i++)
        {
            // test_date[i] = 50;
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

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

#define Grade_1_color make_color(163,251,206)
#define Grade_2_color make_color(97,196,146)
#define Grade_3_color make_color(247,193,62)
#define Grade_4_color make_color(254,99,40)

#define Grade_1_val_min  0
#define Grade_2_val_min  30
#define Grade_3_val_min  59
#define Grade_4_val_min  80

#define mood_max   100
#define first_x    89
#define spacing_x  5
#define height_pic 106
#define first_y   (272)
#define make_pic_hei(val)   (val*height_pic/mood_max)
#define make_pic_y(hei)     (height_pic-hei+first_y)

enum
{
    PRESS_STA_IDLE,            //空闲
    PRESS_STA_TESTING,         //测量中
    PRESS_STA_SUCCSEE,         //测量成功
    PRESS_STA_FAIL,            //测量失败
    PRESS_STA_UNWEAR,          //未佩戴
};
#define DATE_DEBUG 0
extern bool start_testing;
//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{
    char txt_buf[100];
    uint8_t test_date[24];
    bool isWoking = (start_testing && !uteModuleEmotionPressureIsEmotionPressureAutoTesting());

    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);
    uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
    uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();
    ute_module_emotion_pressure_test_last_info_t lastInfo;
    uteModuleEmotionPressureGetLastTestInfo(&lastInfo);

    u8 Pressure_val = lastInfo.lastPressureValue;
    bool press_vaule_flage = (Pressure_val!=255 && Pressure_val) ? true :false;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STRESS]);
    compo_setid(frm->title, COMPO_ID_TITLE);
    compo_setid(frm->time, COMPO_ID_TITLE_TIME);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_16_PRESSURE_ICON_BIN);
    compo_picturebox_set_pos(picbox, 25 + 60 / 2, 79 + 60 / 2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X,154,GUI_SCREEN_WIDTH,70);
    widget_page_scale_to(widget_page,GUI_SCREEN_WIDTH * 0.8,70 * 0.8);

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(i18n[STR_LAST_UPDATED]) + 2);
    compo_textbox_set_location(textbox,0,30,230,46);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_visible(textbox,isWoking ? false : (press_vaule_flage ? true :false));
    compo_textbox_set_alpha(textbox,180);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s:",i18n[STR_LAST_UPDATED]);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_TMEE_TIP);
    area_t txt_leng = widget_text_get_area(textbox->txt);

    ute_module_systemtime_time_t sysTime;
    uteModuleSystemtimeGetTime(&sysTime);

    memset(txt_buf, 0, sizeof(txt_buf));
    if (lastInfo.lastTestTime.day == sysTime.day)
    {
        if (sysTime.is12HourTime)
        {
            uint8_t hour = lastInfo.lastTestTime.hour;
            hour = hour % 12;
            hour = hour ? hour : 12;
            if (lastInfo.lastTestTime.hour >= 12)
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_PM]);
            }
            else
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_AM]);
            }
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d", lastInfo.lastTestTime.hour, lastInfo.lastTestTime.min);
        }
    }
    else
    {
        if (lastInfo.lastTestTime.year == sysTime.year)
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d/%02d", lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%04d/%02d/%02d", lastInfo.lastTestTime.year, lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
    }

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(txt_buf));
    compo_textbox_set_visible(textbox,isWoking ? false : (press_vaule_flage ? true :false));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,10 + (txt_leng.wid > 230 ? 230 : txt_leng.wid),30,133,46);
    compo_textbox_set_alpha(textbox,180);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_TMEE);

    textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox, 99, 60 + 20);
    compo_textbox_set_align_center(textbox, false);
    if(press_vaule_flage)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);


    textbox = compo_textbox_create(frm, strlen(i18n[STR_START_MEAR]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,390 + 40 / 2,300,46);
    compo_textbox_set(textbox,i18n[STR_START_MEAR]);
    compo_textbox_set_forecolor(textbox,make_color(0xff,0x45,0x1f));
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_TXT_START);

    compo_button_t *btn = compo_button_create(frm);///重新/测量按钮
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X,390 + 40 / 2, 300, 80);
    compo_button_set_visible(btn, !isWoking);
    compo_setid(btn,COMPO_ID_BTN_START);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,180 + 30 / 2,75 + 30 / 2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_ABOUT_PIC);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,180 + 30 / 2,75 + 30 / 2,80,80);///关于
    compo_button_set_visible(btn, !isWoking);
    compo_setid(btn,COMPO_ID_ABOUT_BTN);
////////////////////////////////////////////////////////////////////

    memset(txt_buf,0,sizeof(txt_buf));
    if(max_val > 0 && max_val != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",max_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 10);///最高数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,90,344,100,56);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_MAX_VLA);


    memset(txt_buf,0,sizeof(txt_buf));
    if(min_val > 0 && min_val != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",min_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 10);///最低数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,257,344,100,56);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_MIN_VLA);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_16_PRESSURE_ICON_MAX_1_BIN);
    compo_picturebox_set_pos(picbox, 59 + 26 / 2, 358+21/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_MAX);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_16_PRESSURE_ICON_MINI_1_BIN);
    compo_picturebox_set_pos(picbox, 222 + 26 / 2, 358+21/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_MIN);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_16_PRESSURE_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,171 + 170/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_CHART);

#if DATE_DEBUG
    for (int i=0; i<24; i++)
    {
        test_date[i] = get_random(101);
    }
    test_date[1] = 0;
    test_date[23] = 100;
#endif
    int crc = 0;
    for (int i=0; i<24; i++)
    {
        crc = crc + test_date[i];
    }
    textbox = compo_textbox_create(frm, strlen(i18n[STR_NO_DATA]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,230 + 40 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking ? false : (crc ? false :true));
    compo_textbox_set_alpha(textbox,180);
    compo_textbox_set(textbox,i18n[STR_NO_DATA]);
    compo_setid(textbox,COMPO_ID_TXT_NODATE);

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC_U, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,193 + 125 / 2,312,125);
    compo_chartbox_set_pixel(chart, 1);
    widget_set_visible(chart->chart,!isWoking);
    compo_setid(chart,COMPO_ID_CHART_VALUE);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*7 + 4;
        chart_info.height = test_date[i]*1.25;///心率数据转换为柱形条显示数据
        if(test_date[i] < 30)
        {
            compo_chartbox_set_value(chart, i, chart_info, make_color(0x9b,0xfe,0xcb));
        }
        else if(test_date[i] < 60 && test_date[i] > 29)
        {
            compo_chartbox_set_value(chart, i, chart_info, make_color(0x34,0xe0,0x84));
        }
        else if(test_date[i] < 80 && test_date[i] > 59)
        {
            compo_chartbox_set_value(chart, i, chart_info, make_color(0xfe,0xc0,0x30));
        }
        else if(test_date[i] <= 100 && test_date[i] > 79)
        {
            compo_chartbox_set_value(chart, i, chart_info, make_color(0xfd,0x64,0x30));
        }


    }

    //测量中界面
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_16_PRESSURE_ICON_GIF_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 137 + 124 / 2);
    compo_picturebox_cut(picbox, 0, 16);
    compo_picturebox_set_visible(picbox,isWoking);
    compo_setid(picbox,COMPO_ID_PIC_GIF);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_MESSAGE1]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,300 + 46 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking);
    compo_textbox_set(textbox,i18n[STR_MESSAGE1]);
    compo_setid(textbox,COMPO_ID_TXT_TEST);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SOS_QBCSWJZ]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,358 + 46 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking);
    compo_textbox_set(textbox,i18n[STR_SOS_QBCSWJZ]);
    compo_setid(textbox,COMPO_ID_TXT_TIP);


    //测量失败
    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEAR_CHECK]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,300,150);
    compo_textbox_set_visible(textbox,false);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox,i18n[STR_WEAR_CHECK]);
    compo_setid(textbox,COMPO_ID_TXT_MES_FAIL);

    btn = compo_button_create_by_image(frm,UI_BUF_I341001_7_SPO2_AGAIN01_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X,346 + 80 / 2);
    compo_button_set_visible(btn, false);
    compo_setid(btn,COMPO_ID_BTN_RSTART);

    //佩戴检测失败
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PLEASE_WEAR]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,300,100);
    compo_textbox_set_visible(textbox,false);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox,i18n[STR_PLEASE_WEAR]);
    compo_setid(textbox,COMPO_ID_TXT_WEAR_FAIL);

    return frm;

}
static void func_press_date_refresh(void)
{
    ute_module_emotion_pressure_test_last_info_t lastInfo;
    uteModuleEmotionPressureGetLastTestInfo(&lastInfo);

    u8 Pressure_val = lastInfo.lastPressureValue;
    bool press_vaule_flage = (Pressure_val!=255 && Pressure_val) ? true :false;

    uint8_t test_date[24];
    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);

    uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
    uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();

    compo_textbox_t *text_value = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    compo_textbox_t *txt_nodate = compo_getobj_byid(COMPO_ID_TXT_NODATE);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);
    compo_textbox_t *text_tip = compo_getobj_byid(COMPO_ID_TXT_TMEE_TIP);
    compo_textbox_t *text_time = compo_getobj_byid(COMPO_ID_TXT_TMEE);

    char txt_buf[100];

    if(press_vaule_flage)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(text_value,txt_buf);
    compo_textbox_set_visible(text_tip,press_vaule_flage ? true : false);

    ute_module_systemtime_time_t sysTime;
    uteModuleSystemtimeGetTime(&sysTime);

    memset(txt_buf, 0, sizeof(txt_buf));
    if (lastInfo.lastTestTime.day == sysTime.day)
    {
        if (sysTime.is12HourTime)
        {
            uint8_t hour = lastInfo.lastTestTime.hour;
            hour = hour % 12;
            hour = hour ? hour : 12;
            if (lastInfo.lastTestTime.hour >= 12)
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_PM]);
            }
            else
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_AM]);
            }
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d", lastInfo.lastTestTime.hour, lastInfo.lastTestTime.min);
        }
    }
    else
    {
        if (lastInfo.lastTestTime.year == sysTime.year)
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d/%02d", lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%04d/%02d/%02d", lastInfo.lastTestTime.year, lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
    }
    compo_textbox_set(text_time, txt_buf);

    compo_textbox_set_visible(text_time,press_vaule_flage ? true : false);

    memset(txt_buf,0,sizeof(txt_buf));
    if(max_val > 0 && max_val != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",max_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(max_value,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    if(min_val > 0 && min_val != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",min_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(min_value,txt_buf);

#if DATE_DEBUG
    for (int i=0; i<24; i++)
    {
        test_date[i] = get_random(101);
    }
    test_date[1] = 0;
    test_date[23] = 100;
#endif
    int crc = 0;
    for (int i=0; i<24; i++)
    {
        crc = crc + test_date[i];
    }
    compo_textbox_set_visible(txt_nodate,crc ? false : true);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*7 + 4;
        chart_info.height = test_date[i]*1.25;///心率数据转换为柱形条显示数据
        if(test_date[i] < 30)
        {
            compo_chartbox_set_value(chart, i, chart_info, make_color(0x9b,0xfe,0xcb));
        }
        else if(test_date[i] < 60 && test_date[i] > 29)
        {
            compo_chartbox_set_value(chart, i, chart_info, make_color(0x34,0xe0,0x84));
        }
        else if(test_date[i] < 80 && test_date[i] > 59)
        {
            compo_chartbox_set_value(chart, i, chart_info, make_color(0xfe,0xc0,0x30));
        }
        else if(test_date[i] <= 100 && test_date[i] > 79)
        {
            compo_chartbox_set_value(chart, i, chart_info, make_color(0xfd,0x64,0x30));
        }


    }

}

static void func_press_layout_init(u8 state)
{
    //f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_textbox_t *text_tip = compo_getobj_byid(COMPO_ID_TXT_TMEE_TIP);
    compo_textbox_t *text_time = compo_getobj_byid(COMPO_ID_TXT_TMEE);
    compo_textbox_t *text_value = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_textbox_t *text_start = compo_getobj_byid(COMPO_ID_TXT_START);

    compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START);

    compo_picturebox_t * picbox_about = compo_getobj_byid(COMPO_ID_ABOUT_PIC);
    compo_button_t *btn_about = compo_getobj_byid(COMPO_ID_ABOUT_BTN);

    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    compo_picturebox_t * picbox_max = compo_getobj_byid(COMPO_ID_PIC_MAX);
    compo_picturebox_t * picbox_min = compo_getobj_byid(COMPO_ID_PIC_MIN);
    compo_picturebox_t * picbox_chart = compo_getobj_byid(COMPO_ID_PIC_CHART);
    compo_textbox_t *txt_nodate = compo_getobj_byid(COMPO_ID_TXT_NODATE);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);

    compo_picturebox_t * picbox_gif = compo_getobj_byid(COMPO_ID_PIC_GIF);
    compo_textbox_t *txt_test = compo_getobj_byid(COMPO_ID_TXT_TEST);
    compo_textbox_t *txt_tip = compo_getobj_byid(COMPO_ID_TXT_TIP);

    compo_textbox_t *txt_fail = compo_getobj_byid(COMPO_ID_TXT_MES_FAIL);
    compo_textbox_t *txt_w_fail = compo_getobj_byid(COMPO_ID_TXT_WEAR_FAIL);
    compo_button_t *btn_rstart = compo_getobj_byid(COMPO_ID_BTN_RSTART);

    compo_textbox_t *txt_title = compo_getobj_byid(COMPO_ID_TITLE);
    compo_textbox_t *txt_time = compo_getobj_byid(COMPO_ID_TITLE_TIME);

    switch (state)
    {
        case PRESS_STA_IDLE:
            compo_picturebox_set_visible(picbox,true);
            compo_textbox_set_visible(text_tip,true);
            compo_textbox_set_visible(text_time,true);
            compo_textbox_set_visible(text_value,true);
            compo_textbox_set(text_start,i18n[STR_START_MEAR]);
            compo_textbox_set_visible(text_start,true);
            compo_button_set_visible(btn_start, true);
            compo_picturebox_set_visible(picbox_about,true);
            compo_button_set_visible(btn_about, true);
            compo_textbox_set_visible(max_value,true);
            compo_textbox_set_visible(min_value,true);
            compo_picturebox_set_visible(picbox_max,true);
            compo_picturebox_set_visible(picbox_min,true);
            compo_picturebox_set_visible(picbox_chart,true);
            compo_textbox_set_visible(txt_nodate,true);
            widget_set_visible(chart->chart, true);
            compo_textbox_set_visible(txt_title,true);
            compo_textbox_set_visible(txt_time,true);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_button_set_visible(btn_rstart, false);
            func_press_date_refresh();
            break;
        case PRESS_STA_TESTING:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(text_start,false);
            compo_button_set_visible(btn_start, false);
            compo_picturebox_set_visible(picbox_about,false);
            compo_button_set_visible(btn_about, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);

            compo_textbox_set_visible(txt_title,true);
            compo_textbox_set_visible(txt_time,true);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_button_set_visible(btn_rstart, false);

            compo_picturebox_set_visible(picbox_gif,true);
            compo_textbox_set_visible(txt_test,true);
            compo_textbox_set_visible(txt_tip,true);
            break;
        case PRESS_STA_SUCCSEE:
            compo_picturebox_set_visible(picbox,true);
            compo_textbox_set_visible(text_tip,true);
            compo_textbox_set_visible(text_time,true);
            compo_textbox_set_visible(text_value,true);
            compo_textbox_set(text_start,i18n[STR_AGAIN]);
            compo_textbox_set_visible(text_start,true);
            compo_button_set_visible(btn_start, true);
            compo_picturebox_set_visible(picbox_about,true);
            compo_button_set_visible(btn_about, true);
            compo_textbox_set_visible(max_value,true);
            compo_textbox_set_visible(min_value,true);
            compo_picturebox_set_visible(picbox_max,true);
            compo_picturebox_set_visible(picbox_min,true);
            compo_picturebox_set_visible(picbox_chart,true);
            compo_textbox_set_visible(txt_nodate,true);
            widget_set_visible(chart->chart, true);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_button_set_visible(btn_rstart, false);

            compo_textbox_set_visible(txt_title,true);
            compo_textbox_set_visible(txt_time,true);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            func_press_date_refresh();
            break;
        case PRESS_STA_FAIL:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(text_start,false);
            compo_button_set_visible(btn_start, false);
            compo_picturebox_set_visible(picbox_about,false);
            compo_button_set_visible(btn_about, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);

            compo_textbox_set_visible(txt_fail,true);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_button_set_visible(btn_rstart, true);

            compo_textbox_set_visible(txt_title,false);
            compo_textbox_set_visible(txt_time, false);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            break;
        case PRESS_STA_UNWEAR:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(text_start,false);
            compo_button_set_visible(btn_start, false);
            compo_picturebox_set_visible(picbox_about,false);
            compo_button_set_visible(btn_about, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);

            compo_textbox_set_visible(txt_title,false);
            compo_textbox_set_visible(txt_time, false);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,true);
            compo_button_set_visible(btn_rstart, true);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            break;
        default :
            break;
    }
}

//触摸单击按钮
static void func_pressure_button_click(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_ABOUT_BTN:
            uteTaskGuiStartScreen(FUNC_PRESSURE_EXPLAIN, 0, __func__);
            break;
        case COMPO_ID_BTN_RSTART:
        case COMPO_ID_BTN_START:
            uteModuleEmotionPressureStartSingleTesting(false);
            f_pressure->press_state = PRESS_STA_TESTING;
            func_press_layout_init(f_pressure->press_state);
            f_pressure->tick = tick_get();
            break;
    }

}
#define TIME_OUT (30)
//刷新压力数据
static void func_pressure_refresh(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
    static int time_count = 0;
    compo_picturebox_t *picbox_gif = compo_getobj_byid(COMPO_ID_PIC_GIF);
    if (tick_check_expire(f_pressure->tick, 100))
    {
        f_pressure->tick = tick_get();
        if (f_pressure->press_state == PRESS_STA_TESTING)
        {
            time_count++;
            compo_picturebox_cut(picbox_gif, time_count % 16, 16);
            if (!uteModuleEmotionPressureIsWear() && !uteModuleEmotionPressureIsTesting())
            {
                time_count = 0;
                f_pressure->press_state = PRESS_STA_UNWEAR;
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
                func_press_layout_init(f_pressure->press_state);
            }
            else if (!uteModuleEmotionPressureIsTesting() && uteModuleEmotionPressureGetStopReasion() == EP_STOP_REASION_TIMEOUT)
            {
                time_count = 0;
                f_pressure->press_state = PRESS_STA_FAIL;
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
                func_press_layout_init(f_pressure->press_state);
            }
            else if (!uteModuleEmotionPressureIsTesting() && uteModuleEmotionPressureGetStopReasion() == EP_STOP_REASION_SUCCESS)
            {
                time_count = 0;
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
                f_pressure->press_state = PRESS_STA_SUCCSEE;
                func_press_layout_init(f_pressure->press_state);
            }
            else if (!uteModuleEmotionPressureIsTesting())
            {
                time_count = 0;
                f_pressure->press_state = PRESS_STA_SUCCSEE;
                func_press_layout_init(f_pressure->press_state);
            }
        }
        else
        {
            if (uteModuleEmotionPressureIsTesting() && !uteModuleEmotionPressureIsEmotionPressureAutoTesting())
            {
                f_pressure->press_state = PRESS_STA_TESTING;
                func_press_layout_init(f_pressure->press_state);
            }
        }
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

#define Grade_1_color make_color(163,251,206)
#define Grade_2_color make_color(97,196,146)
#define Grade_3_color make_color(247,193,62)
#define Grade_4_color make_color(254,99,40)

#define Grade_1_val_min  0
#define Grade_2_val_min  30
#define Grade_3_val_min  59
#define Grade_4_val_min  80

#define mood_max   100
#define first_x    89
#define spacing_x  5
#define height_pic 106
#define first_y   (272)
#define make_pic_hei(val)   (val*height_pic/mood_max)
#define make_pic_y(hei)     (height_pic-hei+first_y)

//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{
    char txt_buf[20];
    uint8_t test_date[24];
    uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
    uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();

    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);
    u8 Pressure_val = uteModuleEmotionPressureGetPressureValue();

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STRESS]);

    ///设置图片
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I341001_16_PRESSURE_GIF_BIN);
    compo_animation_set_pos(animation,23+74/2, 84+74/2);
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
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,109,73);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,26,195+14,100, widget_text_get_max_height());
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
    compo_textbox_set_pos(textbox,31+txt_leng.wid,195+14);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,194,195+14,100, widget_text_get_max_height());
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
    compo_textbox_set_pos(textbox,198+txt_leng.wid,195+14);
    compo_textbox_set_align_center(textbox, false);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I341001_16_PRESSURE_RETRY_BIN);///重新测量按钮
    compo_button_set_pos(btn,26+282,26+98);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,12+186,12+91);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,12+186,12+91,30,30);///关于
    compo_setid(btn,COMPO_BTN_ABOUT_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_16_PRESSURE_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 247+85);


    for (int i = 0; i < 24; i++)
    {
        compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
        widget_set_align_center(shape->rect, false);
        compo_shape_set_location(shape,first_x+i*spacing_x+i*5,make_pic_y(make_pic_hei(test_date[i])),4,had_date?make_pic_hei(test_date[i]):0);
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
            uteModuleEmotionPressureStartSingleTesting(false);
        }
    }

    if(f_pressure->up_data_flag != uteModuleEmotionPressureIsTesting())
    {
        f_pressure->up_data_flag = uteModuleEmotionPressureIsTesting();

        char txt_buf[20];
        uint8_t test_date[24];
        uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
        uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();
        compo_textbox_t *val = compo_getobj_byid(COMPO_TXT_VALUE_ID);
        compo_textbox_t *val_max = compo_getobj_byid(COMPO_TXT_VALUE_MAX_ID);
        compo_textbox_t *val_min = compo_getobj_byid(COMPO_TXT_VALUE_MIN_ID);

        bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date,24);
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
        compo_button_set_bgimg(btn, f_pressure->up_data_flag ? UI_BUF_I341001_7_SPO2_CONTINUE_BIN : UI_BUF_I341001_16_PRESSURE_PAUSE_BIN);

        for (int i = 0; i < 24; i++)
        {
            u8 mood_mode = test_date[i];
            compo_shape_t *shape = compo_getobj_byid(COMPO_CAHRT_1+i);
            compo_shape_set_location(shape,first_x+i*spacing_x+i*5,make_pic_y(make_pic_hei(test_date[i])),4,had_date?make_pic_hei(test_date[i]):0);
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

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

#define Grade_1_color make_color(163,251,206)
#define Grade_2_color make_color(97,196,146)
#define Grade_3_color make_color(247,193,62)
#define Grade_4_color make_color(254,99,40)

#define Grade_1_val_min  0
#define Grade_2_val_min  30
#define Grade_3_val_min  59
#define Grade_4_val_min  80

#define mood_max   100
#define first_x    77
#define spacing_x  5
#define height_pic 83
#define first_y   (247)
#define make_pic_hei(val)   (val*height_pic/mood_max)
#define make_pic_y(hei)     (height_pic-hei+first_y)

//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{
    char txt_buf[20];
    uint8_t test_date[24];
    uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
    uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();

    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);
    u8 Pressure_val = uteModuleEmotionPressureGetPressureValue();

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_STRESS]);

    ///设置图片
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I343001_16_PRESSURE_GIF_BIN);
    compo_animation_set_pos(animation,20+64/2, 82+64/2);
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
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,95,81);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,23,179,100, widget_text_get_max_height());
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
    compo_textbox_set_pos(textbox,23+txt_leng.wid,178);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,169,179,100, widget_text_get_max_height());
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
    compo_textbox_set_pos(textbox,169+txt_leng.wid,179);
    compo_textbox_set_align_center(textbox, false);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I343001_16_PRESSURE_RETRY_BIN);///重新测量按钮
    compo_button_set_pos(btn,239+58/2,79+58/2);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I343001_6_HEART_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,163+20/2,79+20/2);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,163+20/2,79+20/2,30,30);///关于
    compo_setid(btn,COMPO_BTN_ABOUT_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I343001_16_PRESSURE_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 214+149/2);


    for (int i = 0; i < 24; i++)
    {
        compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
        widget_set_align_center(shape->rect, false);
        compo_shape_set_location(shape,first_x+i*spacing_x+i*5,make_pic_y(make_pic_hei(test_date[i])),4,had_date?make_pic_hei(test_date[i]):0);
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
            uteModuleEmotionPressureStartSingleTesting(false);
        }
    }

    if(f_pressure->up_data_flag != uteModuleEmotionPressureIsTesting())
    {
        f_pressure->up_data_flag = uteModuleEmotionPressureIsTesting();

        char txt_buf[20];
        uint8_t test_date[24];
        uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
        uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();
        compo_textbox_t *val = compo_getobj_byid(COMPO_TXT_VALUE_ID);
        compo_textbox_t *val_max = compo_getobj_byid(COMPO_TXT_VALUE_MAX_ID);
        compo_textbox_t *val_min = compo_getobj_byid(COMPO_TXT_VALUE_MIN_ID);

        bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date,24);
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
        compo_button_set_bgimg(btn, f_pressure->up_data_flag ? UI_BUF_I343001_7_SPO2_CONTINUE_BIN : UI_BUF_I343001_16_PRESSURE_PAUSE_BIN);

        for (int i = 0; i < 24; i++)
        {
            u8 mood_mode = test_date[i];
            compo_shape_t *shape = compo_getobj_byid(COMPO_CAHRT_1+i);
            compo_shape_set_location(shape,first_x+i*spacing_x+i*5,make_pic_y(make_pic_hei(test_date[i])),4,had_date?make_pic_hei(test_date[i]):0);
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

//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{
    char txt_buf[20];
    uint8_t test_date[24];
    uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
    uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();

    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);
    u8 Pressure_val = uteModuleEmotionPressureGetPressureValue();

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT
    ///设置背景图片
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338002_16_PRESSURE_NO_DATA_BIN);
    compo_picturebox_set_pos(picbox, 180, 180);


    picbox = compo_picturebox_create(frm, UI_BUF_I338002_16_PRESSURE_PRESSURE_BIN);
    compo_picturebox_set_pos(picbox, 150, 60);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 5);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,180,40);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_7_SPO2_EXPLAIN_BIN);
    compo_picturebox_set_pos(picbox, 280, 70);
    compo_setid(picbox,COMPO_ID_AGAIN_BTN);

    memset(txt_buf, 0, sizeof(txt_buf));
    if(Pressure_val!=0 && Pressure_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
        compo_picturebox_set_visible(picbox, true);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
        compo_picturebox_set_visible(picbox, false);
    }
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_TXT_VALUE_ID);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,180,34/2+307,200,60);///点击测量区域
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_7_SPO2_EXPLAIN_BIN);
    compo_picturebox_set_pos(picbox, 280, 70);
    compo_setid(picbox,COMPO_ID_PIC_ABOUT);
    compo_picturebox_set_visible(picbox,(Pressure_val!=0 && Pressure_val!=255) ? true : false);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, 280, 70, 90,90);///关于
    compo_setid(btn,COMPO_BTN_ABOUT_ID);
    compo_button_set_visible(btn,(Pressure_val!=0 && Pressure_val!=255) ? true : false);

    ////////////////////////////////////////////////////////////////////

    memset(txt_buf,0,sizeof(txt_buf));
    if(max_val!=0 && max_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",max_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,109,280);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_TXT_VALUE_MAX_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_7_SPO2_HIGH_BIN);//////最高标志
    compo_picturebox_set_pos(picbox,89,295);

    memset(txt_buf,0,sizeof(txt_buf));
    if(min_val!=0 && min_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",min_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,230,280);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_TXT_VALUE_MIN_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_7_SPO2_LOW_BIN);//////最低标志
    compo_picturebox_set_pos(picbox,210,295);

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,166,312,100);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_CAHRT_1);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 5;   ///像素点
    for (int i=0; i<24; i++)
    {
        // test_date[i] =Grade_4_val_min;
        chart_info.x = i*chart_info.width + i*7.8 + 5;
        chart_info.height = test_date[i]*0.89;///压力数据转换为柱形条显示数据
        switch (test_date[i])
        {
            case Grade_1_val_min...Grade_2_val_min-1:
                compo_chartbox_set_value(chart, i, chart_info, Grade_1_color);
                break;
            case Grade_2_val_min...Grade_3_val_min-1:
                compo_chartbox_set_value(chart, i, chart_info, Grade_2_color);
                break;
            case Grade_3_val_min...Grade_4_val_min-1:
                compo_chartbox_set_value(chart, i, chart_info, Grade_3_color);
                break;
            case Grade_4_val_min...100:
                compo_chartbox_set_value(chart, i, chart_info, Grade_4_color);
                break;

            default:
                break;
        }
    }

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STATE_TEST]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,34/2+307,100,34);
    compo_setid(textbox,COMPO_ID_TXT_TEST);
    compo_textbox_set_forecolor(textbox, make_color(6,235,149) );
    compo_textbox_set(textbox,i18n[STR_STATE_TEST]);

#else
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
#endif
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
        uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
        uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();
        compo_textbox_t *val = compo_getobj_byid(COMPO_TXT_VALUE_ID);
        compo_textbox_t *val_max = compo_getobj_byid(COMPO_TXT_VALUE_MAX_ID);
        compo_textbox_t *val_min = compo_getobj_byid(COMPO_TXT_VALUE_MIN_ID);
        compo_textbox_t *state_test = compo_getobj_byid(COMPO_ID_TXT_TEST);
        compo_picturebox_t *pic_about = compo_getobj_byid(COMPO_ID_PIC_ABOUT);
        compo_button_t *btn_about = compo_getobj_byid(COMPO_BTN_ABOUT_ID);

        bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date,24);
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


#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT

        chart_t chart_info;
        chart_info.y = 0;
        chart_info.width = 5;   ///像素点
        compo_chartbox_t *chart = compo_getobj_byid(COMPO_CAHRT_1);
        for (int i=0; i<24; i++)
        {
            // test_date[i] =Grade_4_val_min;
            chart_info.x = i*chart_info.width + i*7.8 + 5;
            chart_info.height = test_date[i]*0.89;///压力数据转换为柱形条显示数据

            switch (test_date[i])
            {
                case Grade_1_val_min...Grade_2_val_min-1:
                    compo_chartbox_set_value(chart, i, chart_info, Grade_1_color);
                    break;
                case Grade_2_val_min...Grade_3_val_min-1:
                    compo_chartbox_set_value(chart, i, chart_info, Grade_2_color);
                    break;
                case Grade_3_val_min...Grade_4_val_min-1:
                    compo_chartbox_set_value(chart, i, chart_info, Grade_3_color);
                    break;
                case Grade_4_val_min...100:
                    compo_chartbox_set_value(chart, i, chart_info, Grade_4_color);
                    break;

                default:
                    break;
            }
        }
        compo_textbox_set(state_test,f_pressure->up_data_flag ? i18n[STR_TESTING] : i18n[STR_STATE_TEST]);
        compo_textbox_set_forecolor(state_test,f_pressure->up_data_flag ? COLOR_WHITE : make_color(6,235,149));
        compo_picturebox_set_visible(pic_about,(Pressure_val!=0 && Pressure_val!=255) ? true : false);
        compo_button_set_visible(btn_about,(Pressure_val!=0 && Pressure_val!=255) ? true : false);
#else
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
#endif
    }
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
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

//创建压力窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_pressure_form_create(void)
{
    char txt_buf[20];
    uint8_t test_date[24];
    uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
    uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();

    bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date, 24);
    u8 Pressure_val = uteModuleEmotionPressureGetPressureValue();

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    ///设置背景图片
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I342001_16_PRESSURE_BG_BIN);
    compo_picturebox_set_pos(picbox, 16+208/2, 70+93/2);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_16_PRESSURE_PRESSURE_BIN);
    compo_picturebox_set_pos(picbox, 77+26/2, 25+30/2);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 5);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(textbox,116,20);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_16_PRESSURE_EXPLAIN_BIN);
    compo_picturebox_set_pos(picbox, 164+16/2, 38+16/2);
    compo_setid(picbox,COMPO_ID_AGAIN_BTN);

    memset(txt_buf, 0, sizeof(txt_buf));
    if(Pressure_val!=0 && Pressure_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
        compo_picturebox_set_visible(picbox, true);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
        compo_picturebox_set_visible(picbox, false);
    }
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_TXT_VALUE_ID);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X,34/2+204,200,60);///点击测量区域
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_16_PRESSURE_EXPLAIN_BIN);
    compo_picturebox_set_pos(picbox, 164+16/2, 38+16/2);
    compo_setid(picbox,COMPO_ID_PIC_ABOUT);
    compo_picturebox_set_visible(picbox,(Pressure_val!=0 && Pressure_val!=255) ? true : false);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, 64+16/2, 38+16/2, 90, 90);///关于
    compo_setid(btn,COMPO_BTN_ABOUT_ID);
    compo_button_set_visible(btn,(Pressure_val!=0 && Pressure_val!=255) ? true : false);

    ////////////////////////////////////////////////////////////////////

    memset(txt_buf,0,sizeof(txt_buf));
    if(max_val!=0 && max_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",max_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最高数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,72,180);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_TXT_VALUE_MAX_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_16_PRESSURE_HIGH_BIN);//////最高标志
    compo_picturebox_set_pos(picbox,53+12/2,186+12/2);

    memset(txt_buf,0,sizeof(txt_buf));
    if(min_val!=0 && min_val!=255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",min_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    textbox = compo_textbox_create(frm, 3);///最低数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,156,180);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_TXT_VALUE_MIN_ID);

    picbox = compo_picturebox_create(frm, UI_BUF_I342001_16_PRESSURE_LOW_BIN);//////最低标志
    compo_picturebox_set_pos(picbox,137+12/2,186+12/2);

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,73+93/2,206,92);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_CAHRT_1);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<24; i++)
    {
        // test_date[i] =Grade_4_val_min;
        chart_info.x = i*chart_info.width + i*5;
        chart_info.height = test_date[i]*0.89;///压力数据转换为柱形条显示数据
        switch (test_date[i])
        {
            case Grade_1_val_min...Grade_2_val_min-1:
                compo_chartbox_set_value(chart, i, chart_info, Grade_1_color);
                break;
            case Grade_2_val_min...Grade_3_val_min-1:
                compo_chartbox_set_value(chart, i, chart_info, Grade_2_color);
                break;
            case Grade_3_val_min...Grade_4_val_min-1:
                compo_chartbox_set_value(chart, i, chart_info, Grade_3_color);
                break;
            case Grade_4_val_min...100:
                compo_chartbox_set_value(chart, i, chart_info, Grade_4_color);
                break;

            default:
                break;
        }
    }

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STATE_TEST]));
    compo_textbox_set(textbox,i18n[STR_STATE_TEST]);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,34/2+204,100,34);
    compo_setid(textbox,COMPO_ID_TXT_TEST);
    compo_textbox_set_forecolor(textbox, make_color(6,235,149) );

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
        uint8_t max_val= uteModuleEmotionPressureGetEmotionPressureMaxValue();
        uint8_t min_val= uteModuleEmotionPressureGetEmotionPressureMinValue();
        compo_textbox_t *val = compo_getobj_byid(COMPO_TXT_VALUE_ID);
        compo_textbox_t *val_max = compo_getobj_byid(COMPO_TXT_VALUE_MAX_ID);
        compo_textbox_t *val_min = compo_getobj_byid(COMPO_TXT_VALUE_MIN_ID);
        compo_textbox_t *state_test = compo_getobj_byid(COMPO_ID_TXT_TEST);
        compo_picturebox_t *pic_about = compo_getobj_byid(COMPO_ID_PIC_ABOUT);
        compo_button_t *btn_about = compo_getobj_byid(COMPO_BTN_ABOUT_ID);

        bool had_date = uteModuleEmotionPressureGetTodayPressureHistoryData(test_date,24);
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

        chart_t chart_info;
        chart_info.y = 0;
        chart_info.width = 4;   ///像素点
        compo_chartbox_t *chart = compo_getobj_byid(COMPO_CAHRT_1);
        for (int i=0; i<24; i++)
        {
            // test_date[i] =Grade_4_val_min;
            chart_info.x = i*chart_info.width + i*5;
            chart_info.height = test_date[i]*0.89;///压力数据转换为柱形条显示数据

            switch (test_date[i])
            {
                case Grade_1_val_min...Grade_2_val_min-1:
                    compo_chartbox_set_value(chart, i, chart_info, Grade_1_color);
                    break;
                case Grade_2_val_min...Grade_3_val_min-1:
                    compo_chartbox_set_value(chart, i, chart_info, Grade_2_color);
                    break;
                case Grade_3_val_min...Grade_4_val_min-1:
                    compo_chartbox_set_value(chart, i, chart_info, Grade_3_color);
                    break;
                case Grade_4_val_min...100:
                    compo_chartbox_set_value(chart, i, chart_info, Grade_4_color);
                    break;

                default:
                    break;
            }
        }
        compo_textbox_set(state_test,f_pressure->up_data_flag ? i18n[STR_TESTING] : i18n[STR_STATE_TEST]);
        compo_textbox_set_forecolor(state_test,f_pressure->up_data_flag ? COLOR_WHITE : make_color(6,235,149));
        compo_picturebox_set_visible(pic_about,(Pressure_val!=0 && Pressure_val!=255) ? true : false);
        compo_button_set_visible(btn_about,(Pressure_val!=0 && Pressure_val!=255) ? true : false);
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

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
//压力功能消息处理
static void func_pressure_process(void)
{
    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;

    if(f_pressure->press_state == PRESS_STA_TESTING)
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
            break;
        case MSG_CTP_CLICK:
            func_pressure_button_click();
            break;
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case MSG_CTP_SHORT_RIGHT:
        case KU_BACK:
        case KU_DELAY_BACK:
        {
            if(f_pressure->press_state == PRESS_STA_TESTING || f_pressure->press_state == PRESS_STA_FAIL || f_pressure->press_state == PRESS_STA_UNWEAR)
            {
                f_pressure->press_state = PRESS_STA_IDLE;
                uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_INITIATIVE_QUIT);
                func_press_layout_init(f_pressure->press_state);
            }
            else
            {
                func_message(msg);
            }
        }
        break;
#endif
        default:
            func_message(msg);
            break;
    }
}
#else
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
#endif
//进入压力功能
static void func_pressure_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_pressure_t));
    func_cb.frm_main = func_pressure_form_create();

    f_pressure_t *f_pressure = (f_pressure_t *)func_cb.f_cb;
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_pressure->press_state = PRESS_STA_IDLE;
    if(start_testing)//app、小组件主动测量
    {
        start_testing = false;
        if(!uteModuleEmotionPressureIsEmotionPressureAutoTesting())
        {
            f_pressure->press_state = PRESS_STA_TESTING;
        }
    }
#endif
#if GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT


#else
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
    if(uteModuleEmotionPressureIsTesting() && !uteModuleEmotionPressureIsEmotionPressureAutoTesting())
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
