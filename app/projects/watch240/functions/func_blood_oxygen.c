#include "include.h"
#include "func.h"
#include "ute_module_bloodoxygen.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//血氧外设接口
static void sensor_blood_oxygen_start(void);
static void sensor_blood_oxygen_stop(void);
static uint8_t sensor_blood_oxygen_data_get(void);
static bool sensor_blood_oxygen_wear_sta_get(void);

//血氧检测状态
enum
{
    BO_STA_IDLE,            //空闲
    BO_STA_WORKING,         //检测中
    BO_STA_UNWEAR,          //未佩戴
};

//组件ID
enum
{
    //按键
    COMPO_ID_BTN = 1,
    COMPO_ID_PIC_BG,
    COMPO_ID_PIC_CLICK,
    COMPO_ID_TXT_TIPS,
    COMPO_ID_TXT_RESULT,
    COMPO_ID_TXT_UNWEAR,
    COMPO_ID_PIC_UNWEAR,
};

typedef struct f_blood_oxygen_t_
{
    uint8_t det_sta;
    uint8_t value;
    bool touch;
} f_blood_oxygen_t;


//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    component_t *compo;

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_BLOOD_OXYGEN_BLOOD_OXYGEN_BIN);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, 124);
    compo_setid(compo, COMPO_ID_PIC_BG);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    //图像按钮
    compo = (component_t *)compo_button_create_by_image(frm, UI_BUF_COMMON_BUTTON_BIN);
    compo_setid(compo, COMPO_ID_BTN);
    compo_button_set_pos((compo_button_t *)compo, GUI_SCREEN_CENTER_X, 258);
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_COMMON_BUTTON_CLICK_BIN);
    compo_setid(compo, COMPO_ID_PIC_CLICK);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, 258);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);                   //触摸按下的效果图先设置不可见

    //按钮上的文本
    compo = (component_t *)compo_textbox_create(frm, 20);
    compo_setid(compo, COMPO_ID_TXT_TIPS);
    compo_textbox_set_location((compo_textbox_t *)compo, GUI_SCREEN_CENTER_X, 258, gui_image_get_size(UI_BUF_COMMON_BUTTON_CLICK_BIN).wid, widget_text_get_height());
    compo_textbox_set((compo_textbox_t *)compo, i18n[STR_START]);

    //测量结果
    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 4);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_setid(textbox, COMPO_ID_TXT_RESULT);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, 176+20);
    compo_textbox_set_visible(textbox, false);
    widget_text_set_color(textbox->txt, make_color(233, 16, 75));

    //未佩戴提示
    compo = (component_t *)compo_picturebox_create(frm, UI_BUF_BLOOD_OXYGEN_EXPLAIN_BIN);
    compo_setid(compo, COMPO_ID_PIC_UNWEAR);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2);
    compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

    //未佩戴提示文本
    compo = (component_t *)compo_textbox_create(frm, 20);
    compo_setid(compo, COMPO_ID_TXT_UNWEAR);
    compo_textbox_set_location((compo_textbox_t *)compo, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, gui_image_get_size(UI_BUF_COMMON_BUTTON_CLICK_BIN).wid, widget_text_get_height());
    compo_textbox_set((compo_textbox_t *)compo, i18n[STR_WEAR_CHECK]);
    compo_textbox_set_visible((compo_textbox_t *)compo, false);

    return frm;
}

//触摸按钮效果处理
static void func_blood_oxygen_button_touch_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    if (COMPO_ID_BTN != id)
    {
        return ;
    }

    //触摸效果图设置可见
    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_CLICK);
    compo_picturebox_set_visible(pic_click, true);
    f_bo->touch = true;
}

//释放按钮效果处理
static void func_blood_oxygen_button_release_handle(void)
{
    //触摸效果图设置不可见
    compo_picturebox_t *pic_click = compo_getobj_byid(COMPO_ID_PIC_CLICK);
    compo_picturebox_set_visible(pic_click, false);
}

//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    component_t *compo;

    int id = compo_get_button_id();
    if (COMPO_ID_BTN != id)
    {
        return ;
    }

    //未佩戴下退出
    if (BO_STA_UNWEAR == f_bo->det_sta)
    {
        compo = compo_getobj_byid(COMPO_ID_PIC_UNWEAR);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);
        compo = compo_getobj_byid(COMPO_ID_TXT_UNWEAR);
        compo_textbox_set_visible((compo_textbox_t *)compo, false);
        compo = compo_getobj_byid(COMPO_ID_PIC_BG);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, true);
    }

    switch (f_bo->det_sta)
    {
        case BO_STA_IDLE:
        {
            sensor_blood_oxygen_stop();
            sensor_blood_oxygen_start();
            compo = compo_getobj_byid(COMPO_ID_TXT_TIPS);
            compo_textbox_set((compo_textbox_t *)compo, i18n[STR_MEASURING]);
            f_bo->det_sta = BO_STA_WORKING;
        }
        break;

        case BO_STA_WORKING:
        case BO_STA_UNWEAR:
        {
            sensor_blood_oxygen_stop();
            compo = compo_getobj_byid(COMPO_ID_TXT_TIPS);
            compo_textbox_set((compo_textbox_t *)compo, i18n[STR_START]);
            f_bo->det_sta = BO_STA_IDLE;
            compo_textbox_t *compo_txt = compo_getobj_byid(COMPO_ID_TXT_RESULT);
            compo_textbox_set_visible(compo_txt, false);
        }
        break;
    }
}

//未佩戴和佩戴处理
static void func_blood_oxygen_unwear_handle(bool wear)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    component_t *compo;

    if (wear)
    {
        compo = compo_getobj_byid(COMPO_ID_PIC_UNWEAR);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

        compo = compo_getobj_byid(COMPO_ID_TXT_UNWEAR);
        compo_textbox_set_visible((compo_textbox_t *)compo, false);

        compo = compo_getobj_byid(COMPO_ID_PIC_BG);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, true);

        compo = compo_getobj_byid(COMPO_ID_TXT_TIPS);
        compo_textbox_set((compo_textbox_t *)compo, i18n[STR_MEASURING]);
    }
    else
    {
        compo = compo_getobj_byid(COMPO_ID_PIC_UNWEAR);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, true);

        compo = compo_getobj_byid(COMPO_ID_TXT_UNWEAR);
        compo_textbox_set_visible((compo_textbox_t *)compo, true);

        compo = compo_getobj_byid(COMPO_ID_PIC_BG);
        compo_picturebox_set_visible((compo_picturebox_t *)compo, false);

        compo = compo_getobj_byid(COMPO_ID_TXT_TIPS);
        compo_textbox_set((compo_textbox_t *)compo, i18n[STR_RETRY]);

        f_bo->value = 0;
        compo = compo_getobj_byid(COMPO_ID_TXT_RESULT);
        compo_textbox_set_visible((compo_textbox_t *)compo, false);
    }
}

//血氧功能事件处理
static void func_blood_oxygen_process(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    s32 dx, dy;

    //按钮释放效果
    if (f_bo->touch && !ctp_get_dxy(&dx, &dy))
    {
        f_bo->touch = false;
        func_blood_oxygen_button_release_handle();
    }

    if (BO_STA_WORKING == f_bo->det_sta)
    {
        if (!sensor_blood_oxygen_wear_sta_get())   //未佩戴检测
        {
            func_blood_oxygen_unwear_handle(false);
            f_bo->det_sta = BO_STA_UNWEAR;
            printf("spo2 unwear\n");
        }
        else
        {
            //出值结果刷新
            uint8_t value = sensor_blood_oxygen_data_get();
            char buf[8] = {0};
            if (f_bo->value != value)
            {
                f_bo->value = value;
                compo_textbox_t *compo_txt = compo_getobj_byid(COMPO_ID_TXT_RESULT);
                compo_textbox_set_visible(compo_txt, true);
                snprintf(buf, sizeof(buf), "%d%%", value);
                compo_textbox_set(compo_txt, buf);
            }
            if(uteModuleBloodoxygenIsTesting())
            {
                reset_sleep_delay_all();
            }
        }
    }
    else if(BO_STA_UNWEAR == f_bo->det_sta)        //佩戴检测
    {
        if (sensor_blood_oxygen_wear_sta_get())
        {
            func_blood_oxygen_unwear_handle(true);
            f_bo->det_sta = BO_STA_WORKING;
            printf("spo2 wear111111\n");
        }
    }

    func_process();
}

//血氧功能消息处理
static void func_blood_oxygen_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_blood_oxygen_button_touch_handle();
            break;

        case MSG_CTP_CLICK:
            func_blood_oxygen_button_click();
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入血氧功能
static void func_blood_oxygen_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_blood_oxygen_t));
    func_cb.frm_main = func_blood_oxygen_form_create();

    f_blood_oxygen_t *f_blood_oxygen = (f_blood_oxygen_t *)func_cb.f_cb;
    f_blood_oxygen->det_sta = BO_STA_IDLE;
}

//退出血氧功能
static void func_blood_oxygen_exit(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    if (BO_STA_IDLE != f_bo->det_sta)
    {
        sensor_blood_oxygen_stop();
    }
    func_cb.last = FUNC_BLOOD_OXYGEN;
}

//血氧功能
void func_blood_oxygen(void)
{
    printf("%s\n", __func__);
    func_blood_oxygen_enter();
    while (func_cb.sta == FUNC_BLOOD_OXYGEN)
    {
        func_blood_oxygen_process();
        func_blood_oxygen_message(msg_dequeue());
    }
    func_blood_oxygen_exit();
}


//血氧外设接口
static void sensor_blood_oxygen_start(void)
{
    uteModuleBloodoxygenStartSingleTesting();

    return ;
}

static void sensor_blood_oxygen_stop(void)
{
    uteModuleBloodoxygenStopSingleTesting();

    return ;
}

static uint8_t sensor_blood_oxygen_data_get(void)
{
    return bsp_sensor_spo2_data_get();
}

static bool sensor_blood_oxygen_wear_sta_get(void)
{
    return bsp_sensor_hr_wear_sta_get();
}
