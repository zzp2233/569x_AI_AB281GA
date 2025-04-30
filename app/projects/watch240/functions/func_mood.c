#include "include.h"
#include "func.h"
#include "ute_drv_motor.h"
#include "ute_module_emotionPressure.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
#if UTE_MODULE_SCREENS_EMOTION_SUPPORT

//组件ID
enum
{
    //按键
    COMPO_ID_AGAIN_BTN = 1,
    COMPO_ID_ABOUT_BTN,
    COMPO_ID_MOOD_PIC,
    COMPO_ID_MOOD_TXT,
    COMPO_ID_MOOD_GIF,
    COMPO_CAHRT_1,
    COMPO_CAHRT_24 = COMPO_CAHRT_1+24,
};

enum
{
    EMOTION_MODE_MEDIUM=0,//消极
    EMOTION_MODE_POOR,//平缓
    EMOTION_MODE_GOOD,//积极
};

typedef struct f_mood_t_
{
    bool up_data_flag;
} f_mood_t;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
typedef struct f_mood_ui_t_
{
    u32  res_addr;
    u16  str_id;
    u8 r;
    u8 g;
    u8 b;
} f_mood_ui_t;

static const f_mood_ui_t f_mood_ui[]=
{
    [EMOTION_MODE_GOOD]   = {.res_addr = UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_ICON_EMOTIONS_50X50_X12_Y62_00_BIN,.str_id=STR_POSITIVE,.r=243,.g=183,.b=45},
    [EMOTION_MODE_POOR]   = {.res_addr = UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_ICON_EMOTIONS_50X50_X12_Y62_01_BIN,.str_id=STR_CALM,.r=30,.g=236,.b=126},
    [EMOTION_MODE_MEDIUM] = {.res_addr = UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_ICON_EMOTIONS_50X50_X12_Y62_02_BIN,.str_id=STR_NEGATIVE,.r=67,.g=87,.b=250},
};
#define Grade_1_color make_color(241,159,45)
#define Grade_2_color make_color(60,227,161)
#define Grade_3_color make_color(66,90,230)

#define first_x    27
#define spacing_x  4
#define height_pic 69
#define first_y   (204-height_pic/2)
#define make_pic_hei(val)   ((val+1)*height_pic/3)
#define make_pic_y(hei)     (height_pic+first_y-hei)

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_mood_form_create(void)
{
    uint8_t test_date[24];
    uteModuleEmotionPressureGetTodayEmotionHistoryData(test_date,24);
    u8 mood_mode = uteModuleEmotionPressureGetEmotionValue();
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_EMOTION]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, mood_mode>2 ? f_mood_ui[0].res_addr : f_mood_ui[mood_mode].res_addr );
    compo_picturebox_set_pos(picbox, 14+52/2, 54+52/2);
    compo_setid(picbox,COMPO_ID_MOOD_PIC);

    ///设置图片
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_DATE_GIF_PROCESS_64X74_X8_Y42_BIN);
    compo_animation_set_pos(animation,14+52/2, 54+52/2);
    compo_animation_set_radix(animation, 11);
    compo_animation_set_interval(animation, 0);
    compo_setid(animation,COMPO_ID_MOOD_GIF);
    compo_animation_set_visible(animation,mood_mode==255? true : false);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_ICON_PLAY_44X44_X180_Y62_01_BIN);///重新测量按钮
    compo_button_set_pos(btn,22+184,22+58);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_ICON_ABOUT_18X18_X146_Y67_BIN);//////关于
    compo_picturebox_set_pos(picbox,125,124);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,125,124,30,30);///关于
    compo_setid(btn,COMPO_ID_ABOUT_BTN);
////////////////////////////////////////////////////////////////////
    compo_textbox_t*textbox = compo_textbox_create(frm, strlen(i18n[f_mood_ui[1].str_id])+30 );
    compo_textbox_set_location(textbox,20,114,100, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    if(mood_mode > 2)
    {
        compo_textbox_set(textbox,"--");
        compo_textbox_set_forecolor(textbox, COLOR_WHITE);
    }
    else
    {
        compo_textbox_set(textbox,i18n[f_mood_ui[mood_mode].str_id]);
        compo_textbox_set_forecolor(textbox, make_color(f_mood_ui[mood_mode].r,f_mood_ui[mood_mode].g,f_mood_ui[mood_mode].b));
    }
    compo_setid(textbox,COMPO_ID_MOOD_TXT);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_DATE_BG_224X110_X8_Y164_BIN);
    compo_picturebox_set_pos(picbox, 120,148+60);
    for (int i = 0; i < 24; i++)
    {
        u8 mood_mode = test_date[i];

        compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE );
        widget_set_align_center(shape->rect, false);
        if(mood_mode <= 2)
        {
            compo_shape_set_location(shape,first_x+i*spacing_x+i*4,make_pic_y(make_pic_hei(mood_mode)),4,make_pic_hei(mood_mode));
        }
        else
        {
            compo_shape_set_location(shape,0,0,0,0);
        }
        compo_setid(shape,COMPO_CAHRT_1+i);
        switch (mood_mode)
        {
            case EMOTION_MODE_MEDIUM:
                compo_shape_set_color(shape,Grade_3_color);
                break;
            case EMOTION_MODE_POOR:
                compo_shape_set_color(shape,Grade_2_color);
                break;
            case EMOTION_MODE_GOOD:
                compo_shape_set_color(shape,Grade_1_color);
                break;
            default:
                break;
        }

    }

    return frm;
}
//单击按钮
static void func_mood_button_click(void)
{
    f_mood_t *f_bo = (f_mood_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_ABOUT_BTN:
            uteTaskGuiStartScreen(FUNC_MOOD_ABOUT, 0, __func__);
            break;
        case COMPO_ID_AGAIN_BTN:
            if(f_bo->up_data_flag)
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
static void func_mood_refresh_update(void)
{
    f_mood_t *f_bo = (f_mood_t *)func_cb.f_cb;

    if(f_bo->up_data_flag && !uteModuleEmotionPressureIsWear() && !uteModuleEmotionPressureIsTesting())
    {
        u8 res = msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
        if (res == MSGBOX_RES_OK)
        {
            // uteModuleEmotionPressureStartSingleTesting(false);
        }
    }

    if(f_bo->up_data_flag != uteModuleEmotionPressureIsTesting() )
    {
        f_bo->up_data_flag = uteModuleEmotionPressureIsTesting();

        u8 mood_mode = uteModuleEmotionPressureGetEmotionValue();
        compo_animation_t *animation = compo_getobj_byid(COMPO_ID_MOOD_GIF);
        compo_picturebox_t * picbox  = compo_getobj_byid(COMPO_ID_MOOD_PIC);
        compo_textbox_t*textbox      = compo_getobj_byid(COMPO_ID_MOOD_TXT);

        if(f_bo->up_data_flag == true)
        {
            compo_animation_set_interval(animation, 15);
        }
        else
        {
            if(mood_mode<=2)
            {
                compo_picturebox_set_visible(picbox,true);
                compo_picturebox_set(picbox, f_mood_ui[mood_mode].res_addr);
                compo_animation_set_visible(animation,false);
                compo_textbox_set(textbox,i18n[f_mood_ui[mood_mode].str_id]);
                compo_textbox_set_forecolor(textbox, make_color(f_mood_ui[mood_mode].r,f_mood_ui[mood_mode].g,f_mood_ui[mood_mode].b));
            }
            else
            {
                compo_animation_set_interval(animation, 0);
                compo_animation_set_visible(animation,true);
                compo_picturebox_set_visible(picbox,false);
                compo_textbox_set(textbox,"--");
                compo_textbox_set_forecolor(textbox, COLOR_WHITE);
            }
        }

        compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
        compo_button_set_bgimg(btn, f_bo->up_data_flag ? UI_BUF_I335001_7_SPO2_1_ICON_PLAY_44X44_X186_Y65_00_BIN:UI_BUF_I335001_15_EMOTIONS_1_1_EMOTIONS_ICON_PLAY_44X44_X180_Y62_01_BIN);

        uint8_t test_date[24];
        uteModuleEmotionPressureGetTodayEmotionHistoryData(test_date,24);
        for (int i = 0; i < 24; i++)
        {
            u8 mood_mode = test_date[i];
            compo_shape_t *shape  = compo_getobj_byid(COMPO_CAHRT_1+i);
            if(mood_mode <= 2)
            {
                compo_shape_set_location(shape,first_x+i*spacing_x+i*4,make_pic_y(make_pic_hei(mood_mode)),4,make_pic_hei(mood_mode));
            }
            else
            {
                compo_shape_set_location(shape,0,0,0,0);
            }
            compo_setid(shape,COMPO_CAHRT_1+i);
            switch (mood_mode)
            {
                case EMOTION_MODE_MEDIUM:
                    compo_shape_set_color(shape,Grade_3_color);
                    break;
                case EMOTION_MODE_POOR:
                    compo_shape_set_color(shape,Grade_2_color);
                    break;
                case EMOTION_MODE_GOOD:
                    compo_shape_set_color(shape,Grade_1_color);
                    break;
                default:
                    break;
            }
        }
    }
}
#else
//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_mood_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
//单击按钮
static void func_mood_button_click(void)
{

}
static void func_mood_refresh_update(void)
{

}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//血氧界面刷新、事件处理
static void func_mood_process(void)
{
    if(uteModuleEmotionPressureIsTesting())
    {
        reset_guioff_delay();
    }
    func_mood_refresh_update();
    func_process();
}

//血氧功能消息处理
static void func_mood_message(size_msg_t msg)
{
    f_mood_t *f_bo = (f_mood_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            func_mood_button_click();
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入血氧功能
static void func_mood_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_mood_t));
    func_cb.frm_main = func_mood_form_create();
}

//退出血氧功能
static void func_mood_exit(void)
{
    if(uteModuleEmotionPressureIsTesting())
    {
        uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_INITIATIVE_QUIT);
    }
    func_cb.last = FUNC_MOOD;
}

//血氧功能
void func_mood(void)
{
    printf("%s\n", __func__);
    func_mood_enter();
    while (func_cb.sta == FUNC_MOOD)
    {
        func_mood_process();
        func_mood_message(msg_dequeue());
    }
    func_mood_exit();
}

#endif
