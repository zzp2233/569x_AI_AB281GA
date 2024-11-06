#include "include.h"
#include "func.h"
#include "ute_module_sport.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define TEST_EN                 0      //假数据测试


#if 0

#define STEPS_DATA_LIMIT        "99999"
#define KCAL_DATA_LIMIT         "99999 千卡"
#define DISTANCE_DATA_LIMIT     "99999.9 千米"


enum
{
    ACTIVITY_STEPS,
    ACTIVITY_KCAL,
    ACTIVITY_DIST,
};

enum
{
    //文本框
    COMPO_ID_TXT_STEPS = 1,
    COMPO_ID_TXT_KCAL,
    COMPO_ID_TXT_DIATANCE,
};

typedef struct f_activity_t_
{
    uint32_t last_steps;
    uint32_t last_kcal;
    uint32_t last_distance;
} f_activity_t;

#if TEST_EN
static int cur_steps = 0, cur_kcal = 0, cur_distance = 0;
#endif


//创建活动记录窗体
compo_form_t *func_activity_form_create(void)
{
    component_t *compo;
    uint32_t res_bin[] =
    {
        UI_BUF_ACTIVITY_STEP_BIN,
        UI_BUF_ACTIVITY_KCAL_BIN,
        UI_BUF_ACTIVITY_KM_BIN,
    };

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ACTIVITY_RECORD]);

    //step_pic
    compo = (component_t *)compo_picturebox_create(frm, res_bin[ACTIVITY_STEPS]);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, GUI_SCREEN_CENTER_X, 102);

    //kcal_pic
    compo = (component_t *)compo_picturebox_create(frm, res_bin[ACTIVITY_KCAL]);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, 70, 260);

    //km_pic
    compo = (component_t *)compo_picturebox_create(frm, res_bin[ACTIVITY_DIST]);
    compo_picturebox_set_pos((compo_picturebox_t *)compo, 70, 330);

    //steps text
    compo = (component_t *)compo_textbox_create(frm, sizeof(STEPS_DATA_LIMIT));
    compo_setid(compo, COMPO_ID_TXT_STEPS);
    compo_textbox_set_pos((compo_textbox_t *)compo, GUI_SCREEN_CENTER_X, 171);

    //kcal text
    compo = (component_t *)compo_textbox_create(frm, sizeof(KCAL_DATA_LIMIT));
    compo_textbox_set_align_center((compo_textbox_t *)compo, false);
    compo_setid(compo, COMPO_ID_TXT_KCAL);
    compo_textbox_set_pos((compo_textbox_t *)compo, 117, 260);

    //distance text
    compo = (component_t *)compo_textbox_create(frm, sizeof(DISTANCE_DATA_LIMIT));
    compo_textbox_set_align_center((compo_textbox_t *)compo, false);
    compo_setid(compo, COMPO_ID_TXT_DIATANCE);
    compo_textbox_set_pos((compo_textbox_t *)compo, 117, 330);

    return frm;
}

//刷新活动数据
static void func_activity_data_refresh(void)
{
    compo_textbox_t *txt_compo;
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;
    char buf[16] = {0};

    //获取当前值(从存储接口或外设接口获取)
#if (!TEST_EN)
    int cur_steps = bsp_sensor_step_cur_steps_get();
    int cur_kcal = bsp_sensor_step_cur_kcal_get();
    int cur_distance = bsp_sensor_step_cur_distance_get();
#endif

    if (f_activity->last_steps != cur_steps)
    {
        txt_compo = compo_getobj_byid(COMPO_ID_TXT_STEPS);
        snprintf(buf, sizeof(buf), "%d", cur_steps);
        compo_textbox_set(txt_compo, buf);
        f_activity->last_steps = cur_steps;
    }

    if (f_activity->last_kcal != cur_kcal)
    {
        txt_compo = compo_getobj_byid(COMPO_ID_TXT_KCAL);
        snprintf(buf, sizeof(buf), "%d %s", cur_kcal / 10, i18n[STR_KCAL]);
        compo_textbox_set(txt_compo, buf);
        f_activity->last_kcal = cur_kcal;
    }

    if (f_activity->last_distance != cur_distance)
    {
        txt_compo = compo_getobj_byid(COMPO_ID_TXT_DIATANCE);
        snprintf(buf, sizeof(buf), "%d.%d %s", cur_distance / 10, cur_distance % 10, i18n[STR_KM]);
        compo_textbox_set(txt_compo, buf);
        f_activity->last_distance = cur_distance;
    }
}

//活动记录功能事件处理
static void func_activity_process(void)
{

#if TEST_EN
    static uint32_t tick = 0;
    if(tick_check_expire(tick, 100))
    {
        tick = tick_get();
        cur_steps += 1;
        cur_kcal += 2;
        cur_distance += 3;
    }
#endif
    func_activity_data_refresh();
    func_process();
}

//活动记录功能消息处理
static void func_activity_message(size_msg_t msg)
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

        default:
            func_message(msg);
            break;
    }
}

//进入活动记录控制功能
static void func_activity_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_activity_t));
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;
    f_activity->last_steps = 0xffffffff;
    f_activity->last_kcal = 0xffffffff;
    f_activity->last_distance = 0xffffffff;

    func_cb.frm_main = func_activity_form_create();
}

//退出活动记录功能
static void func_activity_exit(void)
{
    func_cb.last = FUNC_ACTIVITY;
}

//活动记录功能
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


#else

enum
{
    ARC_ANIM_STATUS_START = 0,
    ARC_ANIM_STATUS_FW,
    ARC_ANIM_STATUS_BW,
    ARC_ANIM_STATUS_END,
};

enum
{
    //文本框
    COMPO_ID_ARC_START = 1,
    COMPO_ID_ARC_END = 3,
};

typedef struct f_activity_t_
{
    uint32_t tick;
    uint32_t last_steps;
    uint32_t last_kcal;
    uint32_t last_distance;

    u16 value[3];
    u8 anim_status;
    page_tp_move_t *ptm;
} f_activity_t;

// size, arc_w, rotation, s_angle, e_angle, content_color, bg_color
static const int16_t activity_arc_info[][7] =
{
    {226, 24, 0,  0,    3600,   COLOR_BLUE, COLOR_BLUE},      //kcal
    {166, 24, 0,  3600, 0,    0x9772,     0x9772},      //steps
    {106, 24, 0,  0,    3600,   0xfbea,     0xfbea},      //exervise
};
//static const u16 activity_arc_value_goal[3] = {750, 750, 1000};
uint32_t activity_arc_value_goal[3];

//uteModuleSportGetStepsTargetCnt()//步数目标

//创建活动记录窗体
compo_form_t *func_activity_form_create(void)
{
    char txt_buf[20];
    uint16_t KM = uteModuleSportGetCurrDayDistanceData();
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);

    activity_arc_value_goal[0] = totalStepCnt*ARC_VALUE_MAX / uteModuleSportGetStepsTargetCnt();
    activity_arc_value_goal[1] = totalStepCnt*ARC_VALUE_MAX / uteModuleSportGetStepsTargetCnt();
    activity_arc_value_goal[2] = totalStepCnt*ARC_VALUE_MAX / uteModuleSportGetStepsTargetCnt();

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ACTIVITY_RECORD]);

    //创建圆弧
    for (u8 i = 0; i < sizeof(activity_arc_info) / sizeof(activity_arc_info[0]); i++)
    {
        compo_arc_t *arc = compo_arc_create(frm);
        compo_setid(arc, COMPO_ID_ARC_START + i);
        compo_arc_set_alpha(arc, 255, 80);
        compo_arc_set_location(arc, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y +30, activity_arc_info[i][0], activity_arc_info[i][0]);
        compo_arc_set_edge_circle(arc,true, true);
        compo_arc_set_width(arc, activity_arc_info[i][1]);
        compo_arc_set_rotation(arc, activity_arc_info[i][2]);
        compo_arc_set_angles(arc, activity_arc_info[i][3], activity_arc_info[i][4]);
        compo_arc_set_color(arc, activity_arc_info[i][5], activity_arc_info[i][6]);
        //compo_arc_set_value(arc, 500);
    }

    compo_textbox_t *textbox;
    compo_picturebox_t* pic;

    pic = compo_picturebox_create(frm, UI_BUF_SPORT_EXERCISING_STEP_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5-GUI_SCREEN_HEIGHT/3.5);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%ld",totalStepCnt);
    textbox = compo_textbox_create(frm,strlen(txt_buf));//步数数据
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/2,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5-GUI_SCREEN_HEIGHT/3.5-12);
    widget_text_set_color(textbox->txt, make_color(26, 137, 255));

    textbox = compo_textbox_create(frm,strlen(i18n[STR_STEPS]));//步数
    //compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, i18n[STR_STEPS]);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/2+strlen(txt_buf)*16+8,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5-GUI_SCREEN_HEIGHT/3.5-8);
    widget_text_set_color(textbox->txt, make_color(26, 137, 255));
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_SPORT_EXERCISING_KM_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d.%d%d",KM/1000,KM/100,KM/10);
    textbox = compo_textbox_create(frm,strlen(txt_buf));//距离数据
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/2,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5-12);
    widget_text_set_color(textbox->txt, make_color(57, 255, 0));

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KM]));//千米
    //compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, i18n[STR_KM]);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/2+strlen(txt_buf)*16+8,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5-8);
    widget_text_set_color(textbox->txt, make_color(57, 255, 0));
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    pic = compo_picturebox_create(frm, UI_BUF_SPORT_EXERCISING_KCAL_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5+GUI_SCREEN_HEIGHT/3.5);

    memset(txt_buf,'\0',sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf),"%d",uteModuleSportGetCurrDayKcalData());
    textbox = compo_textbox_create(frm,strlen(txt_buf));//卡路里数据
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/2,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5+GUI_SCREEN_HEIGHT/3.5-12);
    widget_text_set_color(textbox->txt, make_color(233, 16, 75));

    textbox = compo_textbox_create(frm,strlen(i18n[STR_KCAL]));//千卡
    //compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set(textbox, i18n[STR_KCAL]);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/2+strlen(txt_buf)*16+8,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/2.5+GUI_SCREEN_HEIGHT/3.5-8);
    widget_text_set_color(textbox->txt, make_color(233, 16, 75));

    return frm;
}

//活动记录功能事件处理
static void func_activity_process(void)
{
    int i = 0;
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    compo_page_move_process(f_activity->ptm);
    func_process();

    if (ARC_ANIM_STATUS_END == f_activity->anim_status)//界面刷图动画
    {
        for (i = 0; i < sizeof(activity_arc_info) / sizeof(activity_arc_info[0]); i++)
        {
            compo_arc_t *arc = compo_getobj_byid(COMPO_ID_ARC_START + i);
            compo_arc_set_value(arc, activity_arc_value_goal[i]);
        }
        return ;
    }

    if(tick_check_expire(f_activity->tick, 15))
    {
        f_activity->tick = tick_get();
        u8 count = sizeof(f_activity->value) / sizeof(f_activity->value[0]);

        switch (f_activity->anim_status)
        {
            case ARC_ANIM_STATUS_START:
            {
                for (i = 0; i < count; i++)
                {
                    f_activity->value[i] = 0;
                }
                f_activity->anim_status = ARC_ANIM_STATUS_FW;
            }
            break;

            case ARC_ANIM_STATUS_FW:
            {
                for (i = 0; i < count; i++)
                {
                    f_activity->value[i] += 10;
                    if (f_activity->value[i] > ARC_VALUE_MAX)
                    {
                        f_activity->value[i] = ARC_VALUE_MAX;
                        f_activity->anim_status = ARC_ANIM_STATUS_BW;
                    }
                }
            }
            break;

            case ARC_ANIM_STATUS_BW:
            {
//                for (i = 0; i < sizeof(f_activity->value) / sizeof(f_activity->value[0]); i++)
//                {
//                    u16 offset = (ARC_VALUE_MAX - activity_arc_value_goal[i]) / 20;
//                    if (f_activity->value[i] < activity_arc_value_goal[i])
//                    {
//                        f_activity->value[i] = activity_arc_value_goal[i];
//
//                    }
//                    else
//                    {
//                        f_activity->value[i] -= offset;
//                        //f_activity->anim_status = ARC_ANIM_STATUS_END;
//                    }
//                }
                for (i = 0; i < count; i++)
                {
                    f_activity->value[i] -= 10;
                    if (f_activity->value[i] ==   activity_arc_value_goal[i])
                    {
                        //f_activity->value[i] = ARC_VALUE_MAX;
                        f_activity->anim_status = ARC_ANIM_STATUS_END;
                    }
                }
            }
            break;
        }
    }

    for (i = 0; i < sizeof(activity_arc_info) / sizeof(activity_arc_info[0]); i++)
    {
        compo_arc_t *arc = compo_getobj_byid(COMPO_ID_ARC_START + i);
        compo_arc_set_value(arc, f_activity->value[i]);
    }
}

//活动记录功能消息处理
static void func_activity_message(size_msg_t msg)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_activity->ptm);
            break;
        case MSG_CTP_CLICK:
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入活动记录控制功能
static void func_activity_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_activity_t));
    func_cb.frm_main = func_activity_form_create();
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;
    f_activity->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = GUI_SCREEN_HEIGHT - (GUI_SCREEN_HEIGHT / 8),
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = 100,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_activity->ptm, func_cb.frm_main->page_body, &info);

    f_activity->last_steps = 0xffffffff;
    f_activity->last_kcal = 0xffffffff;
    f_activity->last_distance = 0xffffffff;
    f_activity->tick = tick_get();
}

//退出活动记录功能
static void func_activity_exit(void)
{
    f_activity_t *f_activity = (f_activity_t *)func_cb.f_cb;
    if (f_activity->ptm)
    {
        func_free(f_activity->ptm);
    }
    func_cb.last = FUNC_ACTIVITY;

}

//活动记录功能
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


#endif
