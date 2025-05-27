#include "include.h"
#include "func.h"
#include "ute_drv_motor.h"
#include "ute_module_heart.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_breathe_run_t_
{
    u32 tick;
    u8 run_state;
    u8  count_up;
    u16 run_tick_time;
    bool animation_state;
    u16  pic_size;
    u8   mode;
    u16  rotate_data;
    u8   count_finish;
} f_breathe_run_t;
enum
{
    PIC_MAX_STATE=0,
    PIC_MIN_STATE,
};
enum
{
    COUNT_UP_STATE=0,
    RUN_STATE,
    BREATHE_FINISH,
};
enum
{
    COUNT_UP_1=0,
    COUNT_UP_2,
    COUNT_UP_3,
};
enum
{
    COMPO_ID_PIC_BG=1,
    COMPO_ID_PIC_NUM,
    COMPO_ID_TXT_STATE,
    COMPO_ID_PIC_PROMPT,
};
//速度模式
enum
{
    BREATHE_MODE_SLOW,          //缓慢
    BREATHE_MODE_MEDIUM,        //舒缓
    BREATHE_MODE_FAST,          //稍快
};

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

const uint32_t countdown_pic [3]=
{
    UI_BUF_I335001_19_BREATHING_TRAINING_3_COUNTDOWN_ICON_PIC36X48_X102_Y118_02_BIN,
    UI_BUF_I335001_19_BREATHING_TRAINING_3_COUNTDOWN_ICON_PIC36X48_X102_Y118_01_BIN,
    UI_BUF_I335001_19_BREATHING_TRAINING_3_COUNTDOWN_ICON_PIC36X48_X102_Y118_00_BIN,
};

#define PIC_BG_MAX_SIZE   144
#define PIC_BG_MIN_SIZE   80

#define BREATHE_DAST      6  // 次数快
#define BREATHE_DURING    7  // 次数中
#define BREATHE_SLOW      8  // 次数慢

//创建呼吸训练运行窗体
compo_form_t *func_breathe_run_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_19_BREATHING_TRAINING_3_COUNTDOWN_ICON_144X144_X48_Y70_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    picbox = compo_picturebox_create(frm, countdown_pic[0]);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    compo_setid(picbox,COMPO_ID_PIC_NUM);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_EXHALE])+strlen(i18n[STR_BREATHE_INHALE]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,250,230,widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_BREATHE_INHALE]);
    compo_textbox_set_visible(textbox,false);
    compo_setid(textbox,COMPO_ID_TXT_STATE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_EXPLAIN]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,250,230,60);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set_multiline_drag(textbox,true);
    compo_textbox_set(textbox,i18n[STR_BREATHE_EXPLAIN]);
    compo_setid(textbox,COMPO_ID_PIC_PROMPT);

    if(func_cb.sta == FUNC_BREATHE_RUN)
    {
        f_breathe_run_t* f_breathe_run = (f_breathe_run_t*)func_cb.f_cb;
        f_breathe_run->run_tick_time = 1000;
        f_breathe_run->count_up = COUNT_UP_1;
        f_breathe_run->run_state = COUNT_UP_STATE;
        f_breathe_run->tick = tick_get();
        f_breathe_run->animation_state = PIC_MAX_STATE;
        f_breathe_run->rotate_data = 0;
        switch (sys_cb.breathe_mode)
        {
            case BREATHE_MODE_SLOW:          //缓慢
                f_breathe_run->mode = BREATHE_SLOW;
                break;
            case BREATHE_MODE_MEDIUM:        //舒缓
                f_breathe_run->mode = BREATHE_DURING;
                break;
            case BREATHE_MODE_FAST:          //稍快
                f_breathe_run->mode = BREATHE_DAST;
                break;
            default:
                break;
        }
    }

    return frm;
}

static void func_breathe_run_disp_handle(void)
{
    f_breathe_run_t* f_breathe_run = (f_breathe_run_t*)func_cb.f_cb;

    if(tick_check_expire(f_breathe_run->tick, f_breathe_run->run_tick_time))
    {
        if (sys_cb.gui_sleep_sta)
        {
            sys_cb.gui_need_wakeup = 1;
        }
        reset_sleep_delay_all();
        f_breathe_run->tick = tick_get();
        compo_picturebox_t *picbox  = compo_getobj_byid(COMPO_ID_PIC_NUM);
        compo_textbox_t *txt_prompt = compo_getobj_byid(COMPO_ID_PIC_PROMPT);
        compo_textbox_t *txt_state  = compo_getobj_byid(COMPO_ID_TXT_STATE);

        if(f_breathe_run->run_state == COUNT_UP_STATE)
        {
            f_breathe_run->count_up ++ ;
            if(f_breathe_run->count_up <= COUNT_UP_3)
            {
                compo_picturebox_set(picbox,countdown_pic[f_breathe_run->count_up]);
            }
            else
            {
                f_breathe_run->run_tick_time = 60000/f_breathe_run->mode/2 /(PIC_BG_MAX_SIZE-PIC_BG_MIN_SIZE);
                f_breathe_run->pic_size = PIC_BG_MAX_SIZE;
                f_breathe_run->run_state = RUN_STATE;
                compo_textbox_set_visible(txt_prompt,false);
                compo_textbox_set_visible(txt_state,true);
                compo_picturebox_set_visible(picbox,false);
            }

        }
        else if(f_breathe_run->run_state == RUN_STATE)
        {
            if(f_breathe_run->animation_state == PIC_MAX_STATE)
            {
                if( --f_breathe_run->pic_size == PIC_BG_MIN_SIZE)
                {
                    f_breathe_run->animation_state = PIC_MIN_STATE;
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    compo_textbox_set(txt_state,i18n[STR_BREATHE_EXHALE]);
                    f_breathe_run->count_finish++;
                }
            }
            else if(f_breathe_run->animation_state == PIC_MIN_STATE)
            {
                if( ++f_breathe_run->pic_size == PIC_BG_MAX_SIZE)
                {
                    f_breathe_run->animation_state = PIC_MAX_STATE;
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    compo_textbox_set(txt_state,i18n[STR_BREATHE_INHALE]);
                }
            }

            f_breathe_run->rotate_data +=10;
            if(f_breathe_run->rotate_data >= 3600)
            {
                f_breathe_run->rotate_data = 0;
            }

            compo_picturebox_t *picbox  = compo_getobj_byid(COMPO_ID_PIC_BG);
            compo_picturebox_set_size(picbox,f_breathe_run->pic_size,f_breathe_run->pic_size);
            compo_picturebox_set_rotation(picbox,f_breathe_run->rotate_data);

            if(f_breathe_run->count_finish == f_breathe_run->mode)
            {
                f_breathe_run->run_state = BREATHE_FINISH;
            }

        }
        else if(f_breathe_run->run_state == BREATHE_FINISH)
        {
            uteTaskGuiStartScreen(FUNC_BREATHE_FINISH, 0, __func__);
        }
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

const uint32_t countdown_pic [3]=
{
    UI_BUF_I341001_19_BREATHING_TRAINING_3_BIN,
    UI_BUF_I341001_19_BREATHING_TRAINING_2_BIN,
    UI_BUF_I341001_19_BREATHING_TRAINING_1_BIN,
};

#define PIC_BG_MAX_SIZE   232
#define PIC_BG_MIN_SIZE   156

#define BREATHE_DAST      6  // 次数快
#define BREATHE_DURING    7  // 次数中
#define BREATHE_SLOW      8  // 次数慢

//创建呼吸训练运行窗体
compo_form_t *func_breathe_run_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_19_BREATHING_TRAINING_ICON_BREATHING_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,75+112);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    picbox = compo_picturebox_create(frm, countdown_pic[0]);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,75+112);
    compo_setid(picbox,COMPO_ID_PIC_NUM);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_EXHALE])+strlen(i18n[STR_BREATHE_INHALE]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,343+18,252,widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_BREATHE_INHALE]);
    compo_textbox_set_visible(textbox,false);
    compo_setid(textbox,COMPO_ID_TXT_STATE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_EXPLAIN]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,320+18,252,80);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set_multiline_drag(textbox,true);
    compo_textbox_set(textbox,i18n[STR_BREATHE_EXPLAIN]);
    compo_setid(textbox,COMPO_ID_PIC_PROMPT);

    if(func_cb.sta == FUNC_BREATHE_RUN)
    {
        f_breathe_run_t* f_breathe_run = (f_breathe_run_t*)func_cb.f_cb;
        f_breathe_run->run_tick_time = 1000;
        f_breathe_run->count_up = COUNT_UP_1;
        f_breathe_run->run_state = COUNT_UP_STATE;
        f_breathe_run->tick = tick_get();
        f_breathe_run->animation_state = PIC_MAX_STATE;
        f_breathe_run->rotate_data = 0;
        switch (sys_cb.breathe_mode)
        {
            case BREATHE_MODE_SLOW:          //缓慢
                f_breathe_run->mode = BREATHE_SLOW;
                break;
            case BREATHE_MODE_MEDIUM:        //舒缓
                f_breathe_run->mode = BREATHE_DURING;
                break;
            case BREATHE_MODE_FAST:          //稍快
                f_breathe_run->mode = BREATHE_DAST;
                break;
            default:
                break;
        }
    }

    return frm;
}

static void func_breathe_run_disp_handle(void)
{
    f_breathe_run_t* f_breathe_run = (f_breathe_run_t*)func_cb.f_cb;

    if(tick_check_expire(f_breathe_run->tick, f_breathe_run->run_tick_time))
    {
        if (sys_cb.gui_sleep_sta)
        {
            sys_cb.gui_need_wakeup = 1;
        }
        reset_sleep_delay_all();
        f_breathe_run->tick = tick_get();
        compo_picturebox_t *picbox  = compo_getobj_byid(COMPO_ID_PIC_NUM);
        compo_textbox_t *txt_prompt = compo_getobj_byid(COMPO_ID_PIC_PROMPT);
        compo_textbox_t *txt_state  = compo_getobj_byid(COMPO_ID_TXT_STATE);

        if(f_breathe_run->run_state == COUNT_UP_STATE)
        {
            f_breathe_run->count_up ++ ;
            if(f_breathe_run->count_up <= COUNT_UP_3)
            {
                compo_picturebox_set(picbox,countdown_pic[f_breathe_run->count_up]);
            }
            else
            {
                f_breathe_run->run_tick_time = 60000/f_breathe_run->mode/2 /(PIC_BG_MAX_SIZE-PIC_BG_MIN_SIZE);
                f_breathe_run->pic_size = PIC_BG_MAX_SIZE;
                f_breathe_run->run_state = RUN_STATE;
                compo_textbox_set_visible(txt_prompt,false);
                compo_textbox_set_visible(txt_state,true);
                compo_picturebox_set_visible(picbox,false);
            }

        }
        else if(f_breathe_run->run_state == RUN_STATE)
        {
            if(f_breathe_run->animation_state == PIC_MAX_STATE)
            {
                if( --f_breathe_run->pic_size == PIC_BG_MIN_SIZE)
                {
                    f_breathe_run->animation_state = PIC_MIN_STATE;
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    compo_textbox_set(txt_state,i18n[STR_BREATHE_EXHALE]);
                    f_breathe_run->count_finish++;
                }
            }
            else if(f_breathe_run->animation_state == PIC_MIN_STATE)
            {
                if( ++f_breathe_run->pic_size == PIC_BG_MAX_SIZE)
                {
                    f_breathe_run->animation_state = PIC_MAX_STATE;
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    compo_textbox_set(txt_state,i18n[STR_BREATHE_INHALE]);
                }
            }

            f_breathe_run->rotate_data +=10;
            if(f_breathe_run->rotate_data >= 3600)
            {
                f_breathe_run->rotate_data = 0;
            }

            compo_picturebox_t *picbox  = compo_getobj_byid(COMPO_ID_PIC_BG);
            compo_picturebox_set_size(picbox,f_breathe_run->pic_size,f_breathe_run->pic_size);
            compo_picturebox_set_rotation(picbox,f_breathe_run->rotate_data);

            if(f_breathe_run->count_finish == f_breathe_run->mode)
            {
                f_breathe_run->run_state = BREATHE_FINISH;
            }

        }
        else if(f_breathe_run->run_state == BREATHE_FINISH)
        {
            uteTaskGuiStartScreen(FUNC_BREATHE_FINISH, 0, __func__);
        }
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

#define PIC_BG_MAX_SIZE   193
#define PIC_BG_MIN_SIZE   135

#define BREATHE_DAST      6  // 次数快
#define BREATHE_DURING    7  // 次数中
#define BREATHE_SLOW      8  // 次数慢

//创建呼吸训练运行窗体
compo_form_t *func_breathe_run_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I338001_19_BREATHING_TRAINING_BG_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *txt_num = compo_textbox_create(frm, 1);
    compo_textbox_set_font(txt_num,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(txt_num,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    compo_setid(txt_num,COMPO_ID_PIC_NUM);
    compo_textbox_set(txt_num,"3");

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_EXHALE])+strlen(i18n[STR_BREATHE_INHALE]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,44/2+277,230,widget_text_get_max_height());
    compo_textbox_set(textbox,i18n[STR_BREATHE_INHALE]);
    compo_textbox_set_visible(textbox,false);
    compo_setid(textbox,COMPO_ID_TXT_STATE);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BREATHE_EXHALE]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,250,230,widget_text_get_max_height()*2);
    compo_textbox_set(textbox,i18n[STR_BREATHE_EXHALE]);
    compo_textbox_set_multiline(textbox,true);
    compo_setid(textbox,COMPO_ID_PIC_PROMPT);
    compo_textbox_set_visible(textbox,false);

    if(func_cb.sta == FUNC_BREATHE_RUN)
    {
        f_breathe_run_t* f_breathe_run = (f_breathe_run_t*)func_cb.f_cb;
        f_breathe_run->run_tick_time = 1000;
        f_breathe_run->count_up = COUNT_UP_1;
        f_breathe_run->run_state = COUNT_UP_STATE;
        f_breathe_run->tick = tick_get();
        f_breathe_run->animation_state = PIC_MAX_STATE;
        f_breathe_run->rotate_data = 0;
        switch (sys_cb.breathe_mode)
        {
            case BREATHE_MODE_SLOW:          //缓慢
                f_breathe_run->mode = BREATHE_SLOW;
                break;
            case BREATHE_MODE_MEDIUM:        //舒缓
                f_breathe_run->mode = BREATHE_DURING;
                break;
            case BREATHE_MODE_FAST:          //稍快
                f_breathe_run->mode = BREATHE_DAST;
                break;
            default:
                break;
        }
    }

    return frm;
}

static void func_breathe_run_disp_handle(void)
{
    f_breathe_run_t* f_breathe_run = (f_breathe_run_t*)func_cb.f_cb;

    if(tick_check_expire(f_breathe_run->tick, f_breathe_run->run_tick_time))
    {
        if (sys_cb.gui_sleep_sta)
        {
            sys_cb.gui_need_wakeup = 1;
        }
        reset_sleep_delay_all();
        f_breathe_run->tick = tick_get();
        compo_textbox_t *txt_num    = compo_getobj_byid(COMPO_ID_PIC_NUM);
        compo_textbox_t *txt_prompt = compo_getobj_byid(COMPO_ID_PIC_PROMPT);
        compo_textbox_t *txt_state  = compo_getobj_byid(COMPO_ID_TXT_STATE);

        if(f_breathe_run->run_state == COUNT_UP_STATE)
        {
            f_breathe_run->count_up ++ ;
            if(f_breathe_run->count_up <= COUNT_UP_3)
            {
                if(f_breathe_run->count_up == 1)
                    compo_textbox_set(txt_num,"2");
                else if(f_breathe_run->count_up == 2)
                    compo_textbox_set(txt_num,"1");
            }
            else
            {
                f_breathe_run->run_tick_time = 60000/f_breathe_run->mode/2 /(PIC_BG_MAX_SIZE-PIC_BG_MIN_SIZE);
                f_breathe_run->pic_size = PIC_BG_MAX_SIZE;
                f_breathe_run->run_state = RUN_STATE;
                compo_textbox_set_visible(txt_prompt,false);
                compo_textbox_set_visible(txt_state,true);
                compo_textbox_set_visible(txt_num,false);
            }

        }
        else if(f_breathe_run->run_state == RUN_STATE)
        {
            if(f_breathe_run->animation_state == PIC_MAX_STATE)
            {
                if( --f_breathe_run->pic_size == PIC_BG_MIN_SIZE)
                {
                    f_breathe_run->animation_state = PIC_MIN_STATE;
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    compo_textbox_set(txt_state,i18n[STR_BREATHE_EXHALE]);
                    f_breathe_run->count_finish++;
                }
            }
            else if(f_breathe_run->animation_state == PIC_MIN_STATE)
            {
                if( ++f_breathe_run->pic_size == PIC_BG_MAX_SIZE)
                {
                    f_breathe_run->animation_state = PIC_MAX_STATE;
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    compo_textbox_set(txt_state,i18n[STR_BREATHE_INHALE]);
                }
            }

            f_breathe_run->rotate_data +=10;
            if(f_breathe_run->rotate_data >= 3600)
            {
                f_breathe_run->rotate_data = 0;
            }

            compo_picturebox_t *picbox  = compo_getobj_byid(COMPO_ID_PIC_BG);
            compo_picturebox_set_size(picbox,f_breathe_run->pic_size,f_breathe_run->pic_size);
            compo_picturebox_set_rotation(picbox,f_breathe_run->rotate_data);

            if(f_breathe_run->count_finish == f_breathe_run->mode)
            {
                f_breathe_run->run_state = BREATHE_FINISH;
            }

        }
        else if(f_breathe_run->run_state == BREATHE_FINISH)
        {
            uteTaskGuiStartScreen(FUNC_BREATHE_FINISH, 0, __func__);
        }
    }
}
#else

//创建呼吸训练运行窗体
compo_form_t *func_breathe_run_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
static void func_breathe_run_disp_handle(void)
{}
#endif


//呼吸训练运行功能事件处理
static void func_breathe_run_process(void)
{
    func_breathe_run_disp_handle();
    func_process();
}

//呼吸训练运行功能消息处理
static void func_breathe_run_message(size_msg_t msg)
{
    switch (msg)
    {
        case KU_BACK:
            uteTaskGuiStartScreen(FUNC_BREATHE, 0, __func__);
            break;

        default:
            evt_message(msg);
            break;
    }
}

//进入呼吸训练运行功能
static void func_breathe_run_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_breathe_run_t));
    func_cb.frm_main = func_breathe_run_form_create();
    uteModuleHeartStartSingleTesting(TYPE_HEART);  //开启心率
}

//退出呼吸训练运行功能
static void func_breathe_run_exit(void)
{
    func_cb.last = FUNC_BREATHE_RUN;
    if (!sys_cb.refresh_language_flag)
    {
        uteTaskGuiStackRemoveScreenId(FUNC_BREATHE_RUN);
        uteModuleHeartStopSingleTesting(TYPE_HEART);  //关闭心率
    }
}

//呼吸训练运行功能
void func_breathe_run(void)
{
    printf("%s\n", __func__);
    func_breathe_run_enter();
    while (func_cb.sta == FUNC_BREATHE_RUN)
    {
        func_breathe_run_process();
        func_breathe_run_message(msg_dequeue());
    }
    func_breathe_run_exit();
}
