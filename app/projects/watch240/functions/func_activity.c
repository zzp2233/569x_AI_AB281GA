#include "include.h"
#include "func.h"
#include "ute_module_sport.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define ARC_WID                        20
#define KCAL_ARC_ARDUIS                222-ARC_WID/2
#define KM_ARC_ARDUIS                  172-ARC_WID/2
#define STEP_ARC_ARDUIS                122-ARC_WID/2

#define KCAL_ARC_COLOR                61861
#define KM_ARC_COLOR                  65188
#define STEP_ARC_COLOR                1946

#define TXT_SPACING_Y                 widget_text_get_height()+4

#elif UTE_CUM_XXX
#else
#error "UTE_CUM_XXX ERR"
#endif // UTE_CUM_XXX

enum
{
    ARC_ANIM_STATUS_START = 0,
    ARC_ANIM_STATUS_FW,
    ARC_ANIM_STATUS_BW,
    ARC_ANIM_STATUS_END,
};

enum
{
    ///文本框
    STEP_TXT_VALUE_ID=1,
    KCAL_TXT_VALUE_ID,
    KM_TXT_VALUE_ID,

    KCAL_ARC_ID,
    KM_ARC_ID,
    STEP_ARC_ID,
};

typedef struct f_activity_t_
{
    uint32_t tick;
    uint32_t arc_kcal_value;
    uint32_t arc_km_value;
    uint32_t arc_step_value;
    u8 activity_state;
} f_activity_t;

/// size, arc_w, rotation, s_angle, e_angle, content_color, bg_color
static const int16_t activity_arc_info[][7] =
{
    {KCAL_ARC_ARDUIS, ARC_WID, 3600/4*3,  3600/4*3,    3600/4*1,   KCAL_ARC_COLOR, KCAL_ARC_COLOR},      //kcal
    {KM_ARC_ARDUIS,   ARC_WID, 3600/4*3,  3600/4*3,    3600/4*1,   KM_ARC_COLOR,   KM_ARC_COLOR},      //steps
    {STEP_ARC_ARDUIS, ARC_WID, 3600/4*3,  3600/4*3,    3600/4*1,   STEP_ARC_COLOR, STEP_ARC_COLOR},      //exervise
};

///创建活动记录窗体
compo_form_t *func_activity_form_create(void)
{

    ///新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ACTIVITY_RECORD]);

    ///创建圆弧
    for (u8 i = 0; i < sizeof(activity_arc_info) / sizeof(activity_arc_info[0]); i++)
    {
        compo_arc_t *arc = compo_arc_create(frm);
        compo_arc_set_alpha(arc, 80, 0);
        compo_arc_set_location(arc, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/10, activity_arc_info[i][0], activity_arc_info[i][0]);
        compo_arc_set_width(arc, activity_arc_info[i][1]);
        compo_arc_set_rotation(arc, 2700);
        compo_arc_set_angles(arc, 0, 1800);
        compo_arc_set_color(arc, activity_arc_info[i][5], activity_arc_info[i][6]);
        compo_arc_set_value(arc, 1000);

    }

    ///创建圆弧
    for (u8 i = 0; i < sizeof(activity_arc_info) / sizeof(activity_arc_info[0]); i++)
    {
        compo_arc_t *arc = compo_arc_create(frm);
        compo_setid(arc, KCAL_ARC_ID + i);
        compo_arc_set_alpha(arc, 255, 0);
        compo_arc_set_location(arc, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/10, activity_arc_info[i][0], activity_arc_info[i][0]);
        compo_arc_set_width(arc, activity_arc_info[i][1]);
        compo_arc_set_rotation(arc, 2700);
        compo_arc_set_angles(arc, 0, 1800);
        compo_arc_set_color(arc, activity_arc_info[i][5], activity_arc_info[i][6]);
        compo_arc_set_value(arc, 0);
    }

    char txt_buf[20];
    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    compo_textbox_t *textbox;

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());///千卡数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5);
    compo_setid(textbox,KCAL_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KCAL]));///千卡
    compo_textbox_set(textbox, i18n[STR_KCAL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,KCAL_ARC_COLOR);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",KM);///公里数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5);
    compo_setid(textbox,KM_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KM]));///公里
    compo_textbox_set(textbox, i18n[STR_KM]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.6,GUI_SCREEN_HEIGHT/1.5+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,KM_ARC_COLOR);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);///步数数据
    textbox = compo_textbox_create(frm,6);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/1.2);
    compo_setid(textbox,STEP_TXT_VALUE_ID);

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEPS]));///步数
    compo_textbox_set(textbox, i18n[STR_STEPS]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/1.2+TXT_SPACING_Y);
    compo_textbox_set_forecolor(textbox,STEP_ARC_COLOR);



    return frm;
}


///活动记录功能事件处理
static void func_activity_process(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    if(tick_check_expire(f_activity->tick, 10))
    {
        f_activity->tick = tick_get();
        char txt_buf[20];
        uint32_t totalStepCnt = 0;
        uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

        compo_arc_t *arc_kcal = compo_getobj_byid(KCAL_ARC_ID);
        compo_arc_t *arc_km   = compo_getobj_byid(KM_ARC_ID);
        compo_arc_t *arc_step = compo_getobj_byid(STEP_ARC_ID);
        compo_textbox_t *textbox_kcal = compo_getobj_byid(KCAL_TXT_VALUE_ID);
        compo_textbox_t *textbox_km = compo_getobj_byid(KM_TXT_VALUE_ID);
        compo_textbox_t *textbox_step = compo_getobj_byid(STEP_TXT_VALUE_ID);

        if(f_activity->activity_state == 0)
        {

            f_activity->arc_step_value+=10;

            compo_arc_set_value(arc_kcal, f_activity->arc_step_value);
            compo_arc_set_value(arc_km,   f_activity->arc_step_value);
            compo_arc_set_value(arc_step, f_activity->arc_step_value);

            if(f_activity->arc_step_value >=ARC_VALUE_MAX)
            {
                f_activity->activity_state = 1;
            }
        }
        else if(f_activity->activity_state == 1)
        {

            f_activity->arc_step_value-=10;

            compo_arc_set_value(arc_kcal, f_activity->arc_step_value);
            compo_arc_set_value(arc_km,   f_activity->arc_step_value);
            compo_arc_set_value(arc_step, f_activity->arc_step_value);

            if(f_activity->arc_step_value <= totalStepCnt*ARC_VALUE_MAX / uteModuleSportGetStepsTargetCnt())
            {
                f_activity->activity_state = 2;
            }

        }
        else
        {

            f_activity->arc_kcal_value =(uint32_t) uteModuleSportGetCurrDayKcalData();
            f_activity->arc_km_value   =(uint32_t) uteModuleSportGetCurrDayDistanceData();
            f_activity->arc_step_value =(uint32_t) (totalStepCnt*ARC_VALUE_MAX / uteModuleSportGetStepsTargetCnt());

            compo_arc_set_value(arc_kcal, f_activity->arc_step_value);
            compo_arc_set_value(arc_km,   f_activity->arc_step_value);
            compo_arc_set_value(arc_step, f_activity->arc_step_value);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
            compo_textbox_set(textbox_step, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_km_value);
            compo_textbox_set(textbox_km, txt_buf);

            memset(txt_buf,'\0',sizeof(txt_buf));
            snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",f_activity->arc_kcal_value);
            compo_textbox_set(textbox_kcal, txt_buf);
        }

    }
    func_process();
}
///活动记录功能消息处理
static void func_activity_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            break;
        default:
            func_message(msg);
            break;
    }
}

///进入活动记录控制功能
static void func_activity_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_activity_t));
    func_cb.frm_main = func_activity_form_create();
}

///退出活动记录功能
static void func_activity_exit(void)
{
    func_cb.last = FUNC_ACTIVITY;
}

///活动记录功能
void func_activity(void)
{
    printf("%s\n", __func__);
    func_activity_enter();
    while (func_cb.sta == FUNC_ACTIVITY)
    {
        func_activity_process();
        func_activity_message(msg_dequeue());
    }
    func_activity_exit();
}

