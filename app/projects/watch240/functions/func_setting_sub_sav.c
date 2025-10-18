#include "include.h"
#include "func.h"
#include "ute_module_call.h"
#include "ute_module_localRingtone.h"
#include "ute_module_settingremind.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

enum
{
    COMPO_ID_SAV = 1,
    COMPO_ID_MOTOR_GRADE,
    COMPO_ID_MUTE,
    COMPO_ID_VOL,
    COMPO_ID_MAX,
};
typedef struct f_sav_t_
{
    u8 vol_value;
    u8 shk_value;
    page_tp_move_t *ptm;
} f_sav_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级

#define  ON_PIC     UI_BUF_I330001_PUBLIC_SWITCH02_BIN
#define  OFF_PIC    UI_BUF_I330001_PUBLIC_SWITCH00_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOL]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_SAV);
    compo_cardbox_rect_set_location(cardbox,0,0,0,232,62,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2+62+6,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_MUTE);
    compo_cardbox_rect_set_location(cardbox,0,0,0,232,62,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MUTE]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set_location(cardbox_sav, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级

#define  ON_PIC     UI_BUF_I332001_PUBLIC_SWITCH02_BIN
#define  OFF_PIC    UI_BUF_I332001_PUBLIC_SWITCH00_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOL]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,80/2+102,324, 80);
    compo_setid(cardbox, COMPO_ID_SAV);
    compo_cardbox_rect_set_location(cardbox,0,0,0,324, 80,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, 10-324/2,  -(34/2), 200, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0,  116, 0, 50, 30);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,80/2+192,324, 80);
    compo_setid(cardbox, COMPO_ID_MUTE);
    compo_cardbox_rect_set_location(cardbox,0,0,0,324, 80,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, 10-324/2,  -(34/2), 200, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MUTE]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 116, 0, 50, 30);
    compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus() ? ON_PIC : OFF_PIC);
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级


#define  ON_PIC     UI_BUF_I335001_27_MORE_28_SET_4_SOUND_AND_VIBRATION_1_SOUND_AND_VIBRATION_SWITCH_ICON_PIC40X20_X184_Y204_268_OPEN_BIN
#define  OFF_PIC    UI_BUF_I335001_27_MORE_28_SET_4_SOUND_AND_VIBRATION_1_SOUND_AND_VIBRATION_SWITCH_ICON_PIC40X20_X184_Y204_268_CLOSE_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SOUND_AND_VIBRATION]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_MOTOR_GRADE);
    compo_cardbox_icon_set_pos(cardbox, 0, 0,0);
    compo_cardbox_icon_set(cardbox,0,UI_BUF_I335001_3_EXERCISE_LIST_BIN);
    compo_cardbox_rect_set_location(cardbox,0,0,30,232,1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_scroll_process(cardbox, true);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_VBRATION]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);

    compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,54+62/2+30,GUI_SCREEN_WIDTH,1);
    compo_shape_set_color(shape,make_color(0x29,0x29,0x29));

    cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2+68,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_SAV);
    compo_cardbox_rect_set_location(cardbox,0,0,30,232,1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,54+62/2+68*2,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_MUTE);
    compo_cardbox_rect_set_location(cardbox,0,0,30,232,1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+15, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MUTE]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_SILENT_MODE_PROMAT]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,240+68,235,68);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_SILENT_MODE_PROMAT]);
    compo_textbox_set_forecolor(textbox,make_color(102,102,102));

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级


#define  ON_PIC     UI_BUF_I341001_28_SET_SWITCH_OPEN_BUTTON_BIN
#define  OFF_PIC    UI_BUF_I341001_28_SET_SWITCH_CLOSE_BUTTON_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SOUND_AND_VIBRATION]);

    //声音
    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, 320, 116);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,60+116/2,320,116);
    compo_setid(cardbox, COMPO_ID_VOL);
    compo_cardbox_rect_set_location(cardbox, 0, 0, 114/2, 320, 1, 16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29, 0x29, 0x29));
    compo_cardbox_icon_set_pos(cardbox, 0, 320-20/2-160, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 280, 48);
    compo_cardbox_text_set(cardbox, 0, i18n[STR_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_text_scroll_process(cardbox, true);

    //震动
    cardbox = compo_cardbox_create(frm, 1, 1, 1, 320, 116);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60+116/2+116, 320, 116);
    compo_setid(cardbox, COMPO_ID_MOTOR_GRADE);
    compo_cardbox_rect_set_location(cardbox, 0, 0, 114/2, 320, 1, 16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29, 0x29, 0x29));
    compo_cardbox_icon_set_pos(cardbox, 0, 320-20/2-160, 0);
    compo_cardbox_icon_set(cardbox, 0, UI_BUF_I341001_20_ALARM_CLOCK_MORE_BIN);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 280, 48);
    compo_cardbox_text_set(cardbox, 0, i18n[STR_VBRATION]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_text_scroll_process(cardbox, true);


    //静音
    cardbox = compo_cardbox_create(frm, 0, 1, 1, 320, 116);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 60+116/2+116*2, 320, 116);
    compo_setid(cardbox, COMPO_ID_MUTE);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-27, 240, 48);
    compo_cardbox_text_set(cardbox, 0, i18n[STR_MUTE]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 320-62/2-160, 0-10, 62, 32);
    compo_cardbox_icon_set(cardbox, 0, uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_SILENT_MODE_PROMAT]));
    compo_textbox_set_location(textbox, 24, 386, 320, 110);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set_align_center(textbox, false);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox, i18n[STR_SILENT_MODE_PROMAT]);
    compo_textbox_set_forecolor(textbox, make_color(102, 102, 102));

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, 388+60+110/2-3, 320, 1);
    compo_shape_set_color(shape, make_color(0x29, 0x29, 0x29));


    //媒体音频
    cardbox = compo_cardbox_create(frm, 0, 1, 1, 320, 116);
    compo_cardbox_set_location(cardbox, GUI_SCREEN_CENTER_X, 388+60+110/2+116/2, 320, 116);
    compo_setid(cardbox, COMPO_ID_SAV);

//    compo_cardbox_rect_set_location(cardbox,0, 0, 114/2, 320, 1, 16);    //画线
//    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29, 0x29, 0x29));

    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 240, 48);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 320-62/2-160, 0, 62, 32);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_MEDIA_SWITCH]));
    compo_textbox_set_location(textbox, 24, 610, 320, 110);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_multiline(textbox, true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox, i18n[STR_MEDIA_SWITCH]);
    compo_textbox_set_forecolor(textbox,make_color(102, 102, 102));

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级


#define  ON_PIC     UI_BUF_I343001_28_SET_OPEN_BIN
#define  OFF_PIC    UI_BUF_I343001_28_SET_CLOSE_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SOUND_AND_VIBRATION]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, 278, 99);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,55+99/2,278,99);
    compo_setid(cardbox, COMPO_ID_MOTOR_GRADE);

    compo_cardbox_rect_set_location(cardbox,0,0,114/2,278,1,16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));

    compo_cardbox_icon_set_pos(cardbox, 0, 278-20/2-278/2,0);
    compo_cardbox_icon_set(cardbox,0,UI_BUF_I343001_20_ALARM_CLOCK_NEXT_BIN);
    compo_cardbox_text_scroll_process(cardbox, true);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 200, 48);
    compo_cardbox_text_set(cardbox,0,i18n[STR_VBRATION]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);


    cardbox = compo_cardbox_create(frm, 1, 1, 1, 278, 99);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,55+99/2+99,278,99);
    compo_setid(cardbox, COMPO_ID_SAV);

    compo_cardbox_rect_set_location(cardbox,0,0,114/2,278,1,16);    //画线
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));

    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 200, 48);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 278-53/2-278/2, 0, 53, 28);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);


    cardbox = compo_cardbox_create(frm, 1, 1, 1, 278, 99);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,55+99/2+99*2,278,99);
    compo_setid(cardbox, COMPO_ID_MUTE);
    //compo_cardbox_rect_set_location(cardbox,0,0,30,232,1,16);
    //compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+24, 0-17, 200, 48);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MUTE]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 278-53/2-278/2, 0, 53, 28);
    compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_SILENT_MODE_PROMAT]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,338+100/2,278,100);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_SILENT_MODE_PROMAT]);
    compo_textbox_set_forecolor(textbox,make_color(102,102,102));

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级

#define  ON_PIC     UI_BUF_I338001_28_SET_OPEN_BIN
#define  OFF_PIC    UI_BUF_I338001_28_SET_CLOSE_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SOUND_AND_VIBRATION]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1,324, 80);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,80/2+102-30,324, 80);
    compo_cardbox_rect_set_location(cardbox,0,0,39,278, 1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_setid(cardbox, COMPO_ID_MOTOR_GRADE);
    compo_cardbox_icon_set_pos(cardbox, 0, 130,0);
    compo_cardbox_icon_set(cardbox,0,UI_BUF_I338001_28_SET_MORE_BIN);
    compo_cardbox_text_scroll_process(cardbox, true);
    compo_cardbox_text_set_location(cardbox, 0, 25-324/2,  -(34/2), 200, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_VBRATION]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,80/2+102+90-30,324, 80);
    compo_setid(cardbox, COMPO_ID_SAV);
    compo_cardbox_rect_set_location(cardbox,0,0,39,278, 1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, 25-324/2,  -(34/2), 200, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 116, 0, 50, 30);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,80/2+192+90-30,324, 80);
    compo_setid(cardbox, COMPO_ID_MUTE);
    compo_cardbox_rect_set_location(cardbox,0,0,39,278, 1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, 25-324/2,  -(34/2), 200, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MUTE]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 116, 0, 50, 30);
    compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_SILENT_MODE_PROMAT]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,310+95-30,230,85);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_SILENT_MODE_PROMAT]);
    compo_textbox_set_forecolor(textbox,make_color(102,102,102));

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级

#define  ON_PIC     UI_BUF_I340001_PUBLIC_SWITCH02_BIN
#define  OFF_PIC    UI_BUF_I340001_PUBLIC_SWITCH00_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOL]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,80/2+102,324, 80);
    compo_setid(cardbox, COMPO_ID_SAV);
    compo_cardbox_rect_set_location(cardbox,0,0,0,324, 80,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, 10-324/2,  -(34/2), 200, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0,  116, 0, 50, 30);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, 324, 80);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,80/2+192,324, 80);
    compo_setid(cardbox, COMPO_ID_MUTE);
    compo_cardbox_rect_set_location(cardbox,0,0,0,324, 80,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, 10-324/2,  -(34/2), 200, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MUTE]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 116, 0, 50, 30);
    compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus() ? ON_PIC : OFF_PIC);
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级

#define  ON_PIC     UI_BUF_I342001_28_SET_OPEN_BIN
#define  OFF_PIC    UI_BUF_I342001_28_SET_CLOSE_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SOUND_AND_VIBRATION]);

    compo_cardbox_t *cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,30+62/2,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_MOTOR_GRADE);
    compo_cardbox_rect_set_location(cardbox,0,0,30,192,1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_icon_set_pos(cardbox, 0, 80,0);
    compo_cardbox_icon_set(cardbox,0,UI_BUF_I342001_28_SET_MORE_BIN);
    compo_cardbox_text_scroll_process(cardbox, true);
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+25, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_VBRATION]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,30+62/2+68,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_SAV);
    compo_cardbox_rect_set_location(cardbox,0,0,30,192,1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+25, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MEDIA_VOL]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    cardbox = compo_cardbox_create(frm, 1, 1, 1, GUI_SCREEN_WIDTH, 62);
    compo_cardbox_set_location(cardbox,GUI_SCREEN_CENTER_X,30+62/2+68*2,GUI_SCREEN_WIDTH,62);
    compo_setid(cardbox, COMPO_ID_MUTE);
    compo_cardbox_rect_set_location(cardbox,0,0,30,192,1,16);
    compo_cardbox_rect_set_color(cardbox,0,make_color(0x29,0x29,0x29));
    compo_cardbox_text_set_location(cardbox, 0, -GUI_SCREEN_CENTER_X+25, -11, 145, 30);
    compo_cardbox_text_set(cardbox,0,i18n[STR_MUTE]);
    compo_cardbox_text_set_align_center(cardbox, 0, false);
    compo_cardbox_icon_set_location(cardbox, 0, 194-GUI_SCREEN_CENTER_X, 0, 40, 24);
    compo_cardbox_icon_set(cardbox,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
    compo_cardbox_text_scroll_process(cardbox, true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_SILENT_MODE_PROMAT]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,240+30,200,75);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_SILENT_MODE_PROMAT]);
    compo_textbox_set_forecolor(textbox,make_color(102,102,102));

    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
    compo_cardbox_t *cardbox_sav  = compo_getobj_byid(COMPO_ID_SAV);
    compo_cardbox_t *cardbox_mute = compo_getobj_byid(COMPO_ID_MUTE);

    compo_cardbox_icon_set(cardbox_sav,0,uteModuleCallIsEntertranmentVoiceOn() ? ON_PIC : OFF_PIC);
    compo_cardbox_icon_set(cardbox_mute,0,uteModuleLocalRingtoneGetMuteStatus()==false ? OFF_PIC : ON_PIC);
}

#else
#define VOL_CHANGE          4  //音量等级每次增加或者减少4
#define MOTOR_MAX_LEVEL     4  //马达最大等级


#define  ON_PIC     UI_BUF_I332001_PUBLIC_SWITCH02_BIN
#define  OFF_PIC    UI_BUF_I332001_PUBLIC_SWITCH00_BIN

//声音与振动页面
compo_form_t *func_set_sub_sav_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}

//更新显示界面
static void func_set_sub_sav_disp(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//声音与振动事件处理
static void func_set_sub_sav_process(void)
{
    f_sav_t *f_sav = (f_sav_t *)func_cb.f_cb;
    if(f_sav->ptm)
    {
        compo_page_move_process(f_sav->ptm);
    }
    func_set_sub_sav_disp();
    func_process();
}

//获取点击的卡片组件id
static u16 func_setting_sav_card_get_btn_id(point_t pt)
{
    u16 i, id;
    u16 ret = 0;
    rect_t rect;
    compo_cardbox_t *cardbox;
    for(i=0; i<COMPO_ID_MAX; i++)
    {
        id = COMPO_ID_SAV + i;
        cardbox = compo_getobj_byid(id);
        if(cardbox == NULL)//如果没有这个组件就不需要处理
        {
            continue;
        }
        rect = compo_cardbox_get_absolute(cardbox);
        if (abs_s(pt.x - rect.x) * 2 <= rect.wid && abs_s(pt.y - rect.y) * 2 <= rect.hei)
        {
            ret = id;
            break;
        }
    }
    return ret;
}
//单击按钮
static void func_sav_button_click(void)
{
    point_t pt = ctp_get_sxy();
    u16 compo_id = func_setting_sav_card_get_btn_id(pt);
    if (compo_id <= 0 || compo_id > COMPO_ID_MAX)
    {
        return;
    }

    switch(compo_id)
    {
        case COMPO_ID_SAV:
        {
            compo_cardbox_t* card = compo_getobj_byid(compo_id);
            rect_t play_rect = compo_cardbox_get_icon_absolute(card, 0);
            if (abs_s(pt.x - play_rect.x) * 2 <= (play_rect.wid + 20) && abs_s(pt.y - play_rect.y) * 2 <= (play_rect.hei + 20))
            {
                uteModuleCallChangeEntertranmentVoiceSwitchStatus();
            }
        }
        break;
        case COMPO_ID_MUTE:
        {
            compo_cardbox_t* card = compo_getobj_byid(compo_id);
            rect_t play_rect = compo_cardbox_get_icon_absolute(card, 0);
            if (abs_s(pt.x - play_rect.x) * 2 <= (play_rect.wid + 20) && abs_s(pt.y - play_rect.y) * 2 <= (play_rect.hei + 20))
            {
                if(uteModuleLocalRingtoneGetMuteStatus())
                {
                    uteModuleLocalRingtoneSetMuteStatus(false,true);
                }
                else
                {
                    uteModuleLocalRingtoneSetMuteStatus(true,true);
                }
#if 0//UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT
                if (uteModuleSettingRemindGetWhetherShowId(SETTING_REMIND_ID_MUTE))
                {
                    uteModuleSettingRemindSetRemindId(SETTING_REMIND_ID_MUTE);
                    uteTaskGuiStartScreen(FUNC_SETTING_REMIND, 0, __func__);
                }
#endif
            }
        }
        break;
#if UTE_MODULE_SCREENS_MOTOR_GRADE_SUPPORT
        case COMPO_ID_MOTOR_GRADE:
            uteTaskGuiStartScreen(FUNC_MOTOR_GRADE, 0, __func__);
            break;
#endif
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case COMPO_ID_VOL:
            uteTaskGuiStartScreen(FUNC_VOLUME, 0, __func__);
            break;
#endif
        default:
            break;
    }
    func_set_sub_sav_disp();
}

//声音与振动功能消息处理
static void func_set_sub_sav_message(size_msg_t msg)
{
    f_sav_t *f_sav = (f_sav_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_sav->ptm)
            {
                compo_page_move_touch_handler(f_sav->ptm);
            }
            break;
        case MSG_CTP_CLICK:
            func_sav_button_click();
            break;

        default:
            func_message(msg);
            break;
    }

}

//进入声音与振动功能
static void func_set_sub_sav_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sav_t));
    func_cb.frm_main = func_set_sub_sav_form_create();
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    f_sav_t *f_sav = (f_sav_t *)func_cb.f_cb;
    f_sav->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  340,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_sav->ptm, func_cb.frm_main->page_body, &info);
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    f_sav_t *f_sav = (f_sav_t *)func_cb.f_cb;
    f_sav->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  470,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_sav->ptm, func_cb.frm_main->page_body, &info);
#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_sav_t *f_sav = (f_sav_t *)func_cb.f_cb;
    f_sav->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  702,
        .page_count = 1,
        .quick_jump_perc = 10,
    };
    compo_page_move_init(f_sav->ptm, func_cb.frm_main->page_body, &info);
#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    f_sav_t *f_sav = (f_sav_t *)func_cb.f_cb;
    f_sav->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  440,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_sav->ptm, func_cb.frm_main->page_body, &info);
#endif
}

//退出声音与振动功能
static void func_set_sub_sav_exit(void)
{
    f_sav_t *f_sav = (f_sav_t *)func_cb.f_cb;
    if (f_sav->ptm)
    {
        func_free(f_sav->ptm);
    }
    func_cb.last = FUNC_SET_SUB_SAV;
}

//声音与振动功能
void func_set_sub_sav(void)
{
    printf("%s\n", __func__);
    func_set_sub_sav_enter();
    while (func_cb.sta == FUNC_SET_SUB_SAV)
    {
        func_set_sub_sav_process();
        func_set_sub_sav_message(msg_dequeue());
    }
    func_set_sub_sav_exit();
}
