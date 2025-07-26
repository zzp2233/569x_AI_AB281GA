#include "include.h"
#include "func.h"
#include "func_calculator_alg.h"

#include "chatbot.h"

#define TRACE_EN 1

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

enum
{

    COMPO_ID_TEXT_STATUS = 0xff,
    COMPO_ID_BTN_SPEAKING,
    COMPO_ID_BTN_SPEAKED,
};

typedef struct f_chatbot_t_
{
    char status[32];
    bool is_conn;
    bool is_listen;
    bool need_reconn;
} f_chatbot_t;

typedef struct chatbot_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    s16 x;
    s16 y;
} chatbot_disp_btn_item_t;

// 创建聊天机器人窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_chatbot_form_create(void)
{
    // 新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    compo_picturebox_t *picbox;

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_AI_SPEAKED_BIN);
    compo_picturebox_cut(picbox, 0, 38);
    compo_picturebox_set_pos(picbox, 120, 148);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I330001_AI_SPEAKING_BIN);
    compo_animation_set_pos(animation, 120, 148);
    compo_animation_set_radix(animation, 30);
    compo_animation_set_interval(animation, 15);
    compo_setid(animation, COMPO_ID_BTN_SPEAKING);
    compo_animation_set_visible(animation, false);

    animation = compo_animation_create(frm, UI_BUF_I330001_AI_SPEAKED_BIN);
    compo_animation_set_pos(animation, 120, 148);
    compo_animation_set_radix(animation, 30);
    compo_animation_set_interval(animation, 15);
    compo_setid(animation, COMPO_ID_BTN_SPEAKED);
    compo_animation_set_visible(animation, false);

    // 创建文本
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 32);
    compo_setid(txt, COMPO_ID_TEXT_STATUS);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 240);
    compo_textbox_set_autosize(txt, true);
    compo_textbox_set(txt, "Connecting...");
    compo_textbox_set_forecolor(txt, COLOR_WHITE);

    return frm;
}

static void event_cb(chatbot_event_t event)
{
    f_chatbot_t *f_cb = func_cb.f_cb;
    printf("\nchatbot event: %d\n", event);
    compo_animation_t *animation1 = compo_getobj_byid(COMPO_ID_BTN_SPEAKING);
    compo_animation_t *animation2 = compo_getobj_byid(COMPO_ID_BTN_SPEAKED);
    switch (event)
    {
        case CHATEVT_IS_CONN:
        {
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Listening");//监听中
            // compo_textbox_set_forecolor(txt, COLOR_GREEN);
            compo_animation_set_visible(animation2, true);
            compo_animation_set_visible(animation1, false);

            f_cb->is_listen = true;
            f_cb->is_conn = true;
            break;
        }
        case CHATEVT_EXIT:
            func_back_to();
            break;
        case CHATEVT_TTS_PLAYING:
        {
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Speaking");//说话中
            //compo_textbox_set_forecolor(txt, COLOR_RED);
            compo_animation_set_visible(animation1, true);
            compo_animation_set_visible(animation2, false);

            f_cb->is_listen = false;
            break;
        }
        case CHATEVT_LISTENING:
        {
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Listening");//监听中
            //compo_textbox_set_forecolor(txt, COLOR_GREEN);
            compo_animation_set_visible(animation2, true);
            compo_animation_set_visible(animation1, false);

            f_cb->is_listen = true;
            break;
        }
        case CHATEVT_ERROR_RECONN:
        {

            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Reconnect");//重新连接中
            //compo_textbox_set_forecolor(txt, COLOR_WHITE);
            compo_animation_set_visible(animation1, false);
            compo_animation_set_visible(animation2, false);

            f_cb->need_reconn = true;
            f_cb->is_conn = false;
            break;
        }
        case CHATEVT_THINKING:
        {
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Thinking...");//思考中
            // compo_textbox_set_forecolor(txt, COLOR_YELLOW);
            compo_animation_set_visible(animation2, true);
            compo_animation_set_visible(animation1, false);
            f_cb->is_listen = false;
            break;
        }
        case CHATEVT_ERROR_BROKEN:
            func_back_to();
            break;
        default:
            break;
    }
}

// 聊天机器人功能事件处理
static void func_chatbot_process(void)
{
    f_chatbot_t *f_cb = func_cb.f_cb;
    if (f_cb->need_reconn)
    {
        printf("f_cb->need_reconn = false\r\n");
        f_cb->need_reconn = false;
        f_cb->is_conn = false;
        chatbot_deinit();
        if (chatbot_init())
        {
            chatbot_set_event_callback(event_cb);
            chatbot_start();
            f_cb->is_conn = true;
        }
    }
    reset_sleep_delay_all(); // 不休眠
    func_process();
}

#include "chatbot.h"

// 聊天机器人功能消息处理
static void func_chatbot_message(size_msg_t msg)
{
    f_chatbot_t *f_cb = func_cb.f_cb;
    compo_animation_t *animation1 = compo_getobj_byid(COMPO_ID_BTN_SPEAKING);
    compo_animation_t *animation2 = compo_getobj_byid(COMPO_ID_BTN_SPEAKED);
    switch (msg)
    {
        // case MSG_CTP_TOUCH:
        //     func_calculator_button_press_handle();
        //     break;

        // case MSG_CTP_SHORT_UP:
        // case MSG_CTP_SHORT_DOWN:
        // case MSG_CTP_SHORT_LEFT:
        // case MSG_CTP_LONG:
        //     func_calculator_button_release_handle();
        //     if (func_cb.flag_sort) {
        //         func_message(msg);
        //     }
        //     break;

        // case MSG_CTP_SHORT_RIGHT:
        //     func_calculator_button_release_handle();
        //     func_message(msg);
        //     break;

        case MSG_QDEC_FORWARD:
            printf("MSG_QDEC_FORWARD: %d\n", f_cb->is_listen);
            if (f_cb->is_conn && !f_cb->is_listen)
            {
                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
                compo_textbox_set(txt, "Listening");//监听中
                //compo_textbox_set_forecolor(txt, COLOR_GREEN);
                compo_animation_set_visible(animation2, true);
                compo_animation_set_visible(animation1, false);

                f_cb->is_listen = true;
                chatbot_start_mic();
            }
            break;
        case MSG_QDEC_BACKWARD:
            printf("MSG_QDEC_BACKWARD: %d\n", f_cb->is_listen);
            if (f_cb->is_conn && f_cb->is_listen)
            {
                compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
                compo_textbox_set(txt, "Ready (Idle), Click to Chat");
                // compo_textbox_set_forecolor(txt, COLOR_WHITE);
                compo_animation_set_visible(animation2, true);
                compo_animation_set_visible(animation1, false);

                f_cb->is_listen = false;
                chatbot_stop_mic();
            }
            break;

        case MSG_CTP_CLICK:
            // printf("MSG_CTP_CLICK: %d\n", f_cb->is_listen);
            // if (f_cb->is_conn)
            // {
            //     if (!f_cb->is_listen)
            //     {
            //         compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            //         compo_textbox_set(txt, "Listening");//监听中
            //         //compo_textbox_set_forecolor(txt, COLOR_GREEN);
            //         compo_animation_set_visible(animation2, true);
            //         compo_animation_set_visible(animation1, false);

            //         f_cb->is_listen = true;
            //         chatbot_start_mic();
            //     }
            //     else
            //     {
            //         compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            //         compo_textbox_set(txt, "Ready (Idle), Click to Chat");
            //         //compo_textbox_set_forecolor(txt, COLOR_WHITE);
            //         compo_animation_set_visible(animation2, true);
            //         compo_animation_set_visible(animation1, false);

            //         f_cb->is_listen = false;
            //         chatbot_stop_mic();
            //     }
            // }
            //     func_calculator_button_click_handler();
            break;

        default:
            func_message(msg);
            break;
    }
}

// 进入聊天机器人功能
static void func_chatbot_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_chatbot_t));
    func_cb.frm_main = func_chatbot_form_create();

    sco_dump_init();

    mem_monitor_run();
    // 在 func_chatbot_enter() 中
    // if (!bnep_network_is_ok()) {
    //     compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
    //     compo_textbox_set(txt, "网络连接失败,请检查WiFi");
    //     delay_ms(2000);
    //     //func_back_to();
    //     return;
    // }
    if (chatbot_init())
    {
        // printf("11111111111111133\r\n");
        chatbot_set_event_callback(event_cb);
        chatbot_start();
    }
    else
    {
        extern u8 bt_onoff_chatbot;
        if(bt_onoff_chatbot)
        {
            func_back_to();
        }

    }

    if (gcal_cb_init() == false)
    {
        halt(HALT_MALLOC);
    }
}

// 退出聊天机器人功能
static void func_chatbot_exit(void)
{
    f_chatbot_t *f_cb = func_cb.f_cb;
    gcal_cb_destroy();

    f_cb->is_conn = false;
    chatbot_deinit();
    mem_monitor_run();
}

// 聊天机器人功能
void func_chatbot(void)
{
    printf("%s\n", __func__);
    func_chatbot_enter();
    while (func_cb.sta == FUNC_CHATBOT)
    {
        func_chatbot_process();
        func_chatbot_message(msg_dequeue());
    }
    func_chatbot_exit();
}
