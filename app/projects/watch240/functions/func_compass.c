#include "include.h"
#include "func.h"
#if UTE_MODULE_MAGNETIC_SUPPORT
#include "ute_module_compass.h"
#endif

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_COMPASS_SUPPORT

//组件ID
enum
{
    COMPO_ID_BG_PIC = 1, // 背景图片
    COMPO_ID_ANGLE_TEXT, // 角度
    COMPO_ID_ORIENTATION_TEXT, // 方位
    COMPO_ID_CALIBRATION_TEXT, // 校准提示
    COMPO_ID_CALIBRATION_PIC, // 校准图片
};

typedef struct f_compass_t_
{
    bool calibration_finish_flag;
} f_compass_t;

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

static const u16 angle_lang_id[] =
{
    STR_COMPASS_N,  // 北
    STR_COMPASS_NE, // 东北
    STR_COMPASS_E,  // 东
    STR_COMPASS_SE, // 东南
    STR_COMPASS_S,  // 南
    STR_COMPASS_SW, // 西南
    STR_COMPASS_W,  // 西
    STR_COMPASS_NW, // 西北
};

void func_compass_recreate(void);

// 创建指南针窗体
compo_form_t *func_compass_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    if (uteModuleCompassIsCalibrationFinish())
    {
        // 背景
        compo_picturebox_t *picbox = NULL;
        if(uteModuleSystemtimeReadLanguage() == CHINESE_LANGUAGE_ID)
        {
            picbox = compo_picturebox_create(frm, UI_BUF_I280002_33COMPASS_BG_ZH_BIN);
        }
        else
        {
            picbox = compo_picturebox_create(frm, UI_BUF_I280002_33COMPASS_BG_EN_BIN);
        }
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
        compo_setid(picbox, COMPO_ID_BG_PIC);

        //指针
        picbox = compo_picturebox_create(frm, UI_BUF_I280002_33COMPASS_DIRECTION_BIN);
        compo_picturebox_set_pos(picbox, 112 + 16 / 2, 58);

        //度数
        compo_textbox_t *textbox = NULL;
        textbox = compo_textbox_create(frm, 5);
        compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 140 + 50 / 2, 220, 50);
        compo_textbox_set_align_center(textbox, true);
        compo_textbox_set(textbox, "0°");
        compo_setid(textbox, COMPO_ID_ANGLE_TEXT);

        //方位
        textbox = compo_textbox_create(frm, 40);
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 116, 220, 50);
        compo_textbox_set_align_center(textbox, true);
        compo_textbox_set(textbox, i18n[angle_lang_id[0]]);
        compo_setid(textbox, COMPO_ID_ORIENTATION_TEXT);
    }
    else
    {
        compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I280002_33COMPASS_CALIBRATE_BIN);
        compo_picturebox_set_pos(picbox, 40 + 152 / 2, 60 + 78 / 2);

        compo_textbox_t *textbox;
        textbox = compo_textbox_create(frm, strlen(i18n[STR_COMPASS_CALIBRATION]));
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, 130 + 80, 220, 160);
        compo_textbox_set_multiline(textbox, true);
        compo_textbox_set_align_center(textbox, true);
        compo_textbox_set(textbox, i18n[STR_COMPASS_CALIBRATION]);
    }

    return frm;
}

// 指南针功能事件处理
static void func_compass_sub_process(void)
{
    f_compass_t *f_compass = (f_compass_t *)func_cb.f_cb;

    if (f_compass->calibration_finish_flag == uteModuleCompassIsCalibrationFinish())
    {
        s16 azimuth = uteModuleCompassGetAzimuth();

        compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_BG_PIC);
        if (picbox)
        {
            compo_picturebox_set_rotation(picbox, 3600 - azimuth*10);
        }

        compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_ANGLE_TEXT);
        if (textbox)
        {
            char str_buf[8];
            memset(str_buf, 0, sizeof(str_buf));
            snprintf(str_buf, sizeof(str_buf), "%d°", azimuth);
            compo_textbox_set(textbox, str_buf);
        }

        textbox = compo_getobj_byid(COMPO_ID_ORIENTATION_TEXT);
        if (textbox)
        {
            u16 lang_id = angle_lang_id[0];
            if (azimuth < 360)
            {
                // 使用45°间隔进行方向判断
                int index = ((azimuth + 22) / 45) % 8; // 0到7之间的索引

                if (index >= 0 && index < (int)(sizeof(angle_lang_id)/sizeof(angle_lang_id[0])))
                {
                    lang_id = angle_lang_id[index];
                }
            }
            compo_textbox_set(textbox, i18n[lang_id]);
        }
    }
    else
    {
        f_compass->calibration_finish_flag = uteModuleCompassIsCalibrationFinish();
        func_compass_recreate();
    }
}

void func_compass_recreate(void)
{
    if(func_cb.sta == FUNC_COMPASS)
    {
        compo_form_destroy(func_cb.frm_main);
        func_cb.frm_main = func_compass_form_create();
    }
}

#else
// 创建指南针窗体
compo_form_t *func_compass_form_create(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    // 设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_COMPASS]);

    printf("%s\n", __func__);

    return frm;
}

// 指南针功能事件处理
static void func_compass_sub_process(void)
{
    return;
}
#endif

static void func_compass_process(void)
{
    func_compass_sub_process();
    func_process();
}

// 指南针功能消息处理
static void func_compass_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_SYS_500MS:
            if(uteModulecompassOnoff())
            {
                reset_guioff_delay();
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

// 进入指南针功能
static void func_compass_enter(void)
{
    uteModuleCompassStart();

    func_cb.f_cb = func_zalloc(sizeof(f_compass_t));
    func_cb.frm_main = func_compass_form_create();

    f_compass_t *f_compass = (f_compass_t *)func_cb.f_cb;
    memset(f_compass, 0, sizeof(f_compass_t));
    f_compass->calibration_finish_flag = uteModuleCompassIsCalibrationFinish();
}

// 退出指南针功能
static void func_compass_exit(void)
{
    func_cb.last = FUNC_COMPASS;
    uteModuleCompassStop();
}

// 指南针功能
void func_compass(void)
{
    printf("%s\n", __func__);
    func_compass_enter();
    while (func_cb.sta == FUNC_COMPASS)
    {
        func_compass_process();
        func_compass_message(msg_dequeue());
    }
    func_compass_exit();
}
#endif
