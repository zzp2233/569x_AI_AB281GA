#include "include.h"
#include "func.h"
#include "ute_module_heart.h"
#include "ute_module_emotionPressure.h"
#include "ute_module_bloodoxygen.h"
#include "ute_drv_motor.h"
#include "ute_module_one_click_measure.h"

enum
{
    COMPO_ID_BTN_NO = 1,
    COMPO_ID_BTN_YES,
    COMPO_ID_MEASURING_ARC,
    COMPO_ID_NUM_TIME,
    COMPO_ID_BTN_RSTART,

};

enum
{
    READY_PAGE,         //  准备界面
    MEASURING_PAGE,     //测量中界面
    SUCCESS_PAGE,       //测量成功界面
    FAIL_PAGE,          //测量失败界面
};
static u8 page_type = 0;
typedef struct f_measure_t_
{
    u8 time;
    u32 tick;
} f_measure_t;

#if UTE_MODULE_SCREENS_MEASURE_SUPPORT
compo_form_t *func_measure_ready_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEAR_CHECK]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y - 102 / 2,300,180);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox, i18n[STR_WEAR_CHECK]);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I341001_20_ALARM_CLOCK_CENCEL_BIN);
    compo_setid(btn, COMPO_ID_BTN_NO);
    compo_button_set_pos(btn, 24+156/2, 347+80/2);

    btn = compo_button_create_by_image(frm, UI_BUF_I341001_28_SET_CONFIRM_2_BIN);
    compo_setid(btn, COMPO_ID_BTN_YES);
    compo_button_set_pos(btn, 188 + 156 / 2, 347+80/2);
    page_type = READY_PAGE;
    return frm;
}

compo_form_t *func_measuring_form_create(void)
{
    char txt_buf[20];
    compo_form_t *frm = compo_form_create(true);
    compo_picturebox_t* pic = compo_picturebox_create(frm,UI_BUF_I341001_21_KEY_MEASURE_ICON_GIF_BIN);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,188/2+57);
    compo_picturebox_cut(pic,0,8);
    compo_setid(pic,COMPO_ID_MEASURING_ARC);

    pic = compo_picturebox_create(frm,UI_BUF_I341001_21_KEY_MEASURE_MEASUREMENT_BIN);
    compo_picturebox_set_pos(pic,GUI_SCREEN_CENTER_X,188/2+57);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",60);
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X - 55,256 + 20,100,90);
    compo_textbox_set(textbox, txt_buf);
    compo_setid(textbox,COMPO_ID_NUM_TIME);
    area_t txt_leng = widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SEC]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X - 50 + txt_leng.wid,300,100,widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_SEC]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_MESSAGE1]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,350 + 40 / 2,300,widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_MESSAGE1]);
    page_type = MEASURING_PAGE;
    return frm;
}

compo_form_t *func_succeess_form_create(void)
{
    char txt_buf[20];
#define MOVE_Y 104
    uint8_t oxygen_val = 0;
    uint8_t Pressure_val = 0;
    uint8_t heart_value = 0;
    uteModuleOneClickMeasureGetTestValue(&heart_value, &oxygen_val, &Pressure_val);
    compo_form_t *frm = compo_form_create(true);
    compo_picturebox_t* pic = compo_picturebox_create(frm,UI_BUF_I341001_6_HEART_00_BIN);
    compo_picturebox_set_pos(pic,38 + 60 / 2,44 + 60 / 2);

    pic = compo_picturebox_create(frm,UI_BUF_I341001_7_SPO2_SPO2_BIN);
    compo_picturebox_set_pos(pic,38 + 60 / 2,44 + 60 / 2 + MOVE_Y);

    pic = compo_picturebox_create(frm,UI_BUF_I341001_16_PRESSURE_ICON_BIN);
    compo_picturebox_set_pos(pic,38 + 60 / 2,44 + 60 / 2 + MOVE_Y * 2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_HEART_RATE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox,make_color(0xff,0x45,0x15));
    compo_textbox_set_location(textbox,117,30,160,widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_HEART_RATE]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox,make_color(0xff,0x45,0x15));
    compo_textbox_set_location(textbox,117,30 + MOVE_Y,160,widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_BLOOD_OXYGEN]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STRESS]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_forecolor(textbox,make_color(0x00,0xd6,0xde));
    compo_textbox_set_location(textbox,117,30 + MOVE_Y * 2,160,widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_STRESS]);

    textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,117,70);
    compo_textbox_set_align_center(textbox, false);
    snprintf(txt_buf,sizeof(txt_buf),"%d",heart_value);
    compo_textbox_set(textbox,txt_buf);
    area_t txt_css = widget_text_get_area(textbox->txt);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,127 + txt_css.wid,70,100,widget_text_get_max_height());
    compo_textbox_set(textbox, i18n[STR_PER_MINUTE]);


    textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,117,70 + MOVE_Y);
    compo_textbox_set_align_center(textbox, false);
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_val);
    compo_textbox_set(textbox,txt_buf);

    textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,117,70 + MOVE_Y * 2);
    compo_textbox_set_align_center(textbox, false);
    snprintf(txt_buf,sizeof(txt_buf),"%d",Pressure_val);
    compo_textbox_set(textbox,txt_buf);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I341001_21_KEY_MEASURE_AGAIN00_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X,346 + 80 / 2);
    compo_setid(btn,COMPO_ID_BTN_RSTART);

    page_type = SUCCESS_PAGE;
    return frm;
}

compo_form_t *func_fail_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PLEASE_WEAR]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y - 102 / 2,328,180);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox, i18n[STR_PLEASE_WEAR]);
    if (widget_text_get_area(textbox->txt).wid <= 328 && widget_text_get_area(textbox->txt).hei < widget_text_get_height())
    {
        compo_textbox_set_multiline(textbox, false);
    }

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I341001_7_SPO2_AGAIN01_BIN);
    compo_button_set_pos(btn,GUI_SCREEN_CENTER_X,346 + 80 / 2);
    compo_setid(btn,COMPO_ID_BTN_RSTART);
    page_type = FAIL_PAGE;
    return frm;
}

compo_form_t *func_measure_form_create(void)
{
    compo_form_t *frm = func_measure_ready_form_create();

    return frm;
}

static void func_measure_start()
{
    if(!uteModuleOneClickMeasureIsTesting())
    {
        uteModuleOneClickMeasureStartSingleTesting();
    }
}

static void func_measure_stop()
{
    if(uteModuleOneClickMeasureIsTesting())
    {
        uteModuleOneClickMeasureStopSingleTesting();
    }
}

static void func_measure_updata()
{
    uint8_t oxygen_val = uteModuleBloodoxygenGetValue();
    int heart_value = uteModuleHeartGetHeartValue();
    char txt_buf[20];
    f_measure_t *f_measure = (f_measure_t *)func_cb.f_cb;
    compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_MEASURING_ARC);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_NUM_TIME);
    static u8 index = 0;
    switch(page_type)
    {
        case READY_PAGE:
            break;
        case MEASURING_PAGE:
            reset_guioff_delay();
            if (!uteModuleOneClickMeasureIsTesting())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
                if (uteModuleOneClickMeasureGetTestResult() > MEASURE_RESULT_SUCCESS)
                {

                    compo_form_destroy(func_cb.frm_main);
                    func_cb.frm_main = func_fail_form_create();
                    return;
                }
                else
                {
                    compo_form_destroy(func_cb.frm_main);
                    func_cb.frm_main = func_succeess_form_create();
                    return;
                }
            }
            else
            {
                f_measure->time = (uteModuleOneClickMeasureGetTestingSecond() <= UTE_MODULE_ONE_CLICK_MEASURE_TEST_SECOND) ? UTE_MODULE_ONE_CLICK_MEASURE_TEST_SECOND - uteModuleOneClickMeasureGetTestingSecond() : 0;
                memset(txt_buf, 0, sizeof(txt_buf));
                snprintf((char *)txt_buf, sizeof(txt_buf), "%ld", f_measure->time);
                compo_textbox_set(textbox, txt_buf);
            }
            break;
        case SUCCESS_PAGE:
            break;
        case FAIL_PAGE:
            break;
    }
}


static void func_measure_process()
{
    f_measure_t *f_measure = (f_measure_t *)func_cb.f_cb;
    compo_picturebox_t* pic = compo_getobj_byid(COMPO_ID_MEASURING_ARC);
    static u8 temp = 0;
    switch(page_type)
    {
        case MEASURING_PAGE:
            if(tick_check_expire(f_measure->tick, 200))
            {
                f_measure->tick = tick_get();
                temp = temp + 1 > 7 ? 0 : temp + 1;
                compo_picturebox_cut(pic,temp,8);
            }
            break;
    }
    func_process();
}

//单击按钮
static void func_measure_button_click(void)
{
    //f_breathe_t *f_breathe = (f_breathe_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch(page_type)
    {
        case READY_PAGE:
            switch(id)
            {
                case COMPO_ID_BTN_NO:
                    func_back_to();
                    break;
                case COMPO_ID_BTN_YES:
                    func_measure_start();
                    compo_form_destroy(func_cb.frm_main);
                    func_cb.frm_main = func_measuring_form_create();
                    return;
                    break;
            }
        case SUCCESS_PAGE:
        case FAIL_PAGE:
            switch(id)
            {
                case COMPO_ID_BTN_RSTART:
                    func_measure_start();
                    compo_form_destroy(func_cb.frm_main);
                    func_cb.frm_main = func_measuring_form_create();
                    break;
            }
            break;
    }
}

static void func_measure_message(size_msg_t msg)
{

    switch (msg)
    {
        case MSG_CTP_TOUCH:

            break;
        case MSG_CTP_CLICK:
            func_measure_button_click();
            break;
        case MSG_SYS_500MS:
            func_measure_updata();
        default:
            func_message(msg);
            break;
    }

}


//进入一键测量功能
static void func_measure_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_measure_t));
    f_measure_t *f_measure = (f_measure_t *)func_cb.f_cb;
    func_cb.frm_main = func_measure_form_create();
    f_measure->time = UTE_MODULE_ONE_CLICK_MEASURE_TEST_SECOND;
}

static void func_measure_exit(void)
{
    func_measure_stop();
    func_cb.last = FUNC_MEASURE;
}

//一键测量功能
void func_measure(void)
{
    printf("%s\n", __func__);
    func_measure_enter();
    while (func_cb.sta == FUNC_MEASURE)
    {
        func_measure_process();
        func_measure_message(msg_dequeue());
    }
    func_measure_exit();
}
#endif //UTE_MODULE_SCREENS_MEASURE_SUPPORT