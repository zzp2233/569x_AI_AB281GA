#include "include.h"
#include "func.h"
#include "ute_module_menstrualcycle.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_women_health_t_
{
    s16 pic_x;
    s16 pic_y;
    s16 txt_x;
    s16 txt_y;
    s16 txt_w;
    s16 txt_h;
    u16 str_id;
    u32 res_addr;
    u8  state;
    bool open_flag;
} f_women_health_t;

enum
{
    NO_DATA=0,      //请前往APP同步数据
    MENSTRUAL_CYCLE,//月经期
    PREGNANCY,      //易孕期
    SAFE_PERIOD,    //安全期
};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=48/2+96,  .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+175, .txt_w=224, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I330001_PERIOD_PHONE_BIN},
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=110/2+75, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+218, .txt_w=224, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I330001_PERIOD_PERIOD_MP_BIN},
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=110/2+75, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+218, .txt_w=224, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I330001_PERIOD_PERIOD_FP_BIN},
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=110/2+75, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+218, .txt_w=224, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I330001_PERIOD_SAFE_MP_BIN},
};



//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }
    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=92/2+64,  .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+188, .txt_w=224, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I335001_26_WOMEN_1_SYNCHRONOUS_DATA_ICON_DATA_56X92_X92_Y64_BIN},//前往app同步
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=96/2+50, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+178, .txt_w=224, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I335001_26_WOMEN_7_ESTIMATED_MENSTRUAL_PERIOD_ICON_96X96_X72_Y50_02_BIN},//月经
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=96/2+50, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+178, .txt_w=224, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I335001_26_WOMEN_7_ESTIMATED_MENSTRUAL_PERIOD_ICON_96X96_X72_Y50_01_BIN},//易孕
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=96/2+50, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+178, .txt_w=224, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I335001_26_WOMEN_7_ESTIMATED_MENSTRUAL_PERIOD_ICON_96X96_X72_Y50_00_BIN},//安全
};



//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }
    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=109+142/2, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+292, .txt_w=308, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I341001_23_SOS_PHONE_BIN},//前往app同步
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=112/2+118, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+317, .txt_w=308, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I341001_26_WOMEN_HEALTH_00_BIN},//月经
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=112/2+118, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+317, .txt_w=308, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I341001_26_WOMEN_HEALTH_02_BIN},//易孕
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=112/2+118, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=28/2+317, .txt_w=308, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I341001_26_WOMEN_HEALTH_01_BIN},//安全
};



//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }
    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=92/2+86,   .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+212, .txt_w=288, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I332001_PERIOD_PHONE_BIN},
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I332001_PERIOD_PERIOD_MP_BIN},
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I332001_PERIOD_PERIOD_FP_BIN},
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I332001_PERIOD_SAFE_MP_BIN},
};

//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }

    if(state)
    {
        //设置标题栏
        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
        compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);
    }

    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=288, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I338001_26_WOMEN_HEALTH_PHONE_BIN},
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I338001_26_WOMEN_HEALTH_00_BIN},
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I338001_26_WOMEN_HEALTH_01_BIN},
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I338001_26_WOMEN_HEALTH_02_BIN},
};

//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }


    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    u16 color = COLOR_WHITE;

    switch (state)
    {
        case MENSTRUAL_CYCLE:
            color = make_color(255,87,165);
            break;
        case PREGNANCY:
            color = make_color(72,144,255);
            break;
        case SAFE_PERIOD:
            color = make_color(161,72,255);
            break;
        default:
            break;
    }

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);
    compo_textbox_set_forecolor(textbox,color);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
static const f_women_health_t    f_women_health[]=
{
    [NO_DATA]          ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=92/2+86,   .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+212, .txt_w=288, .txt_h=40, .str_id=STR_PLEASE_APP_DATA, .res_addr=UI_BUF_I340001_PERIOD_PHONE_BIN},
    [MENSTRUAL_CYCLE]  ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_MENSTRUAL_CYCLE, .res_addr=UI_BUF_I340001_PERIOD_PERIOD_MP_BIN},
    [PREGNANCY]        ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_PREGNANCY,       .res_addr=UI_BUF_I340001_PERIOD_PERIOD_FP_BIN},
    [SAFE_PERIOD]      ={ .pic_x=GUI_SCREEN_CENTER_X, .pic_y=138/2+102, .txt_x=GUI_SCREEN_CENTER_X,  .txt_y=34/2+270, .txt_w=220, .txt_h=40, .str_id=STR_SAFE_PERIOD,     .res_addr=UI_BUF_I340001_PERIOD_SAFE_MP_BIN},
};

//创建女性健康窗体
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    u8 state=0;

    if(uteModuleMenstrualCycleIsOpen())
    {
        uteModuleMenstrualCycleGetStatus(&state);
    }

    if(state)
    {
        //设置标题栏
        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
        compo_form_set_title(frm, i18n[STR_WOMEN_HEALTH]);
    }

    compo_picturebox_t *picbox = compo_picturebox_create(frm,f_women_health[state].res_addr);
    compo_picturebox_set_pos(picbox,f_women_health[state].pic_x,f_women_health[state].pic_y);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[f_women_health[state].str_id]));
    compo_textbox_set_location(textbox,f_women_health[state].txt_x,f_women_health[state].txt_y,f_women_health[state].txt_w,f_women_health[state].txt_h);
    compo_textbox_set(textbox,i18n[f_women_health[state].str_id]);

    return frm;
}

#else
compo_form_t *func_women_health_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}

#endif

//女性健康功能事件处理
static void func_women_health_process(void)
{
    func_process();
}
//女性健康功能消息处理
static void func_women_health_message(size_msg_t msg)
{
    f_women_health_t *f_women_health = (f_women_health_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_SYS_1S:
        {
            u8 state=NO_DATA;
            uteModuleMenstrualCycleGetStatus(&state);
            if(state!= f_women_health->state || f_women_health->open_flag !=uteModuleMenstrualCycleIsOpen())
            {
                f_women_health->state = state;
                f_women_health->open_flag = uteModuleMenstrualCycleIsOpen();
                msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
            }
        }
        break;
        case MSG_CTP_CLICK:
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入女性健康功能
static void func_women_health_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_women_health_t));
    func_cb.frm_main = func_women_health_form_create();

    f_women_health_t *f_women_health = (f_women_health_t*)func_cb.f_cb;
    uteModuleMenstrualCycleGetStatus(&f_women_health->state);
    f_women_health->open_flag = uteModuleMenstrualCycleIsOpen();
}

//退出女性健康功能
static void func_women_health_exit(void)
{
    func_cb.last = FUNC_WOMEN_HEALTH;
}

//女性健康功能
void func_women_health(void)
{
    printf("%s\n", __func__);
    func_women_health_enter();
    while (func_cb.sta == FUNC_WOMEN_HEALTH)
    {
        func_women_health_process();
        func_women_health_message(msg_dequeue());
    }
    func_women_health_exit();
}
