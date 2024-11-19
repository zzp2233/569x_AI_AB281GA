#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//组件ID
enum
{
    //图像
    COMPO_ID_PIC_VOICE = 1,

    //TEXT
    COMPO_ID_TXT_VOICE,
};

typedef struct f_voice_t_
{
    bool siri_en;
    u8 siri_status;
    u32 voice_over_tick;
} f_voice_t;

enum
{
    FUNC_SIRI_STATUS_NONE = 0,
    FUNC_SIRI_STATUS_CLICK,
    FUNC_SIRI_STATUS_CONNBT,
    FUNC_SIRI_STATUS_SPEAKER,
};

//创建语音助手窗体
compo_form_t *func_voice_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOICE]);

    //创建动画
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_VOICE_320_86_BIN);
    compo_animation_set_pos(animation, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_animation_set_radix(animation, 2);
    compo_animation_set_interval(animation, 1);
    compo_setid(animation, COMPO_ID_PIC_VOICE);
    compo_animation_set_roll(animation, UI_BUF_VOICE_320_86_BIN);

    //创建TEXT
    compo_textbox_t* txt = compo_textbox_create(frm, 50);
    compo_textbox_set_font(txt, 0);
    compo_textbox_set_align_center(txt, true);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*4/5, GUI_SCREEN_WIDTH/2+5, widget_text_get_height());
    compo_textbox_set_visible(txt, true);
//    compo_textbox_set_autoroll_mode(txt, 0);
    //compo_textbox_set(txt, "点击屏幕开启SIRI");
    compo_setid(txt, COMPO_ID_TXT_VOICE);

    return frm;
}

//动画滚动(开启语音助手)
static void func_voice_animation_roll(bool en)
{
    compo_animation_t *animation = compo_getobj_byid(COMPO_ID_PIC_VOICE);

    if (animation->interval == 0)
    {
        return;
    }

    static s32 dx;
    static u32 tick;
    if (en)
    {
        if (tick_check_expire(tick, animation->interval))
        {
            tick = tick_get();
            dx--;
            if (dx < -(animation->wid * animation->radix))
            {
                dx = 0;
            }
            widget_page_set_client(animation->page, dx, 0);
        }
    }
    else
    {
        widget_page_set_client(animation->page, 0, 0);
    }
}

static void func_voice_start_siri(void)
{
    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    if (bt_is_connected())
    {
        bt_hfp_siri_switch();
        f_voice->siri_en = true;
        f_voice->voice_over_tick = tick_get();
    }
    else
    {
        f_voice->siri_en = false;
    }
}

//语音助手功能事件处理
static void func_voice_process(void)
{
    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    static u8 siri_cnt = 0;

    compo_textbox_t* txt = compo_getobj_byid(COMPO_ID_TXT_VOICE);

    if (!bt_is_connected())
    {
        f_voice->siri_en = false;
    }
    else if (!bt_get_siri_status())
    {
        if (bt_is_ios_device())     //IOS设备可以正常获取siri的状态
        {
            if (f_voice->siri_en)
            {
                siri_cnt++;
                if (siri_cnt > 50)
                {
                    f_voice->siri_en = false;
                }
            }
        }
        else      //部分安卓设备无法获取得到siri的状态,先这样处理(5s后界面显示自动关闭)
        {
            if (f_voice->siri_en)
            {
                if (tick_check_expire(f_voice->voice_over_tick, 5000))
                {
                    f_voice->siri_en = false;
                }
            }
        }
    }
    else
    {
        f_voice->siri_en = true;
        siri_cnt = 0;
    }


    if (f_voice->siri_en)
    {
        func_voice_animation_roll(true);
        if (f_voice->siri_status != FUNC_SIRI_STATUS_SPEAKER)
        {
            f_voice->siri_status = FUNC_SIRI_STATUS_SPEAKER;
            compo_textbox_set(txt, i18n[STR_VOICE_SPEAKER]);
        }
    }
    else
    {

        if (!bt_is_connected())
        {
            if (f_voice->siri_status != FUNC_SIRI_STATUS_CONNBT)
            {
                //func_voice_animation_roll(false);
                f_voice->siri_status = FUNC_SIRI_STATUS_CONNBT;
                compo_textbox_set(txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
            }
        }
        else
        {
            if (f_voice->siri_status != FUNC_SIRI_STATUS_CLICK)
            {
                //func_voice_animation_roll(false);
                f_voice->siri_status = FUNC_SIRI_STATUS_CLICK;
                compo_textbox_set(txt, i18n[STR_VOICE_SIRI_START]);
            }
        }
    }

    func_process();
}

//语音助手功能消息处理
static void func_voice_message(size_msg_t msg)
{
    //f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_voice_start_siri();
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

//进入语音助手功能
static void func_voice_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_voice_t));
    func_cb.frm_main = func_voice_form_create();

    f_voice_t *f_voice = (f_voice_t*)func_cb.f_cb;
    f_voice->siri_en = false;

}

//退出语音助手功能
static void func_voice_exit(void)
{
    func_cb.last = FUNC_VOICE;
}

//语音助手功能
void func_voice(void)
{
    printf("%s\n", __func__);
    func_voice_enter();
    while (func_cb.sta == FUNC_VOICE)
    {
        func_voice_process();
        func_voice_message(msg_dequeue());
    }
    func_voice_exit();
}
