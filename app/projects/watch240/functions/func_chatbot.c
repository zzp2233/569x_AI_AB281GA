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

static bool chatbot_network = false;
static u16 chatbot_num = 0;

enum
{

    COMPO_ID_TEXT_STATUS = 0xff,
    COMPO_ID_BTN_SPEAKING,
    COMPO_ID_BTN_SPEAKED,
    COMPO_ID_BTN_CONNECTED,
    COMPO_ID_BTN_DISCONNECTED,
};

typedef struct f_chatbot_t_
{
    char status[32];
    bool is_conn;
    bool is_listen;
    bool need_reconn;
    bool last_net_state;  //记录上一次网络状态
    bool exiting;
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
    compo_picturebox_cut(picbox, 0, 7);
    compo_picturebox_set_pos(picbox, 120, 148);



    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I330001_AI_SPEAKING_BIN);
    compo_animation_set_pos(animation, 120, 148);
    compo_animation_set_radix(animation, 15);//张数
    compo_animation_set_interval(animation, 15);
    compo_setid(animation, COMPO_ID_BTN_SPEAKING);
    compo_animation_set_visible(animation, false);

    animation = compo_animation_create(frm, UI_BUF_I330001_AI_SPEAKED_BIN);
    compo_animation_set_pos(animation, 120, 148);
    compo_animation_set_radix(animation, 7);
    compo_animation_set_interval(animation, 15);
    compo_setid(animation, COMPO_ID_BTN_SPEAKED);
    compo_animation_set_visible(animation, false);

    // 已连接图片（默认隐藏）
    compo_picturebox_t *pic_connected = compo_picturebox_create(frm, UI_BUF_I330001_AI_CONNECTED_BIN);
    compo_picturebox_set_pos(pic_connected, 40, 40);
    compo_setid(pic_connected, COMPO_ID_BTN_CONNECTED);
    compo_picturebox_set_visible(pic_connected, false);


    compo_picturebox_t *pic_disconnected = compo_picturebox_create(frm, UI_BUF_I330001_AI_NOT_CONNECTED_BIN);
    compo_picturebox_set_pos(pic_disconnected, 40, 40);
    compo_setid(pic_disconnected, COMPO_ID_BTN_DISCONNECTED);
    compo_picturebox_set_visible(pic_disconnected, true);

    // 创建文本
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm, 32);
    compo_setid(txt, COMPO_ID_TEXT_STATUS);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 240);
    compo_textbox_set_autosize(txt, true);
    compo_textbox_set(txt, "Connecting...");
    compo_textbox_set_forecolor(txt, COLOR_WHITE);
    chatbot_network = false;
    return frm;
}

static void event_cb(chatbot_event_t event)
{
    f_chatbot_t *f_cb = func_cb.f_cb;
    printf("\nchatbot event: %d\n", event);
    compo_animation_t *animation1 = compo_getobj_byid(COMPO_ID_BTN_SPEAKING);
    compo_animation_t *animation2 = compo_getobj_byid(COMPO_ID_BTN_SPEAKED);
    compo_picturebox_t *pic_connected = compo_getobj_byid(COMPO_ID_BTN_CONNECTED);
    compo_picturebox_t *pic_disconnected = compo_getobj_byid(COMPO_ID_BTN_DISCONNECTED);


    switch (event)
    {
        case CHATEVT_IS_CONN:
        {
            chatbot_network = true;
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Listening");//监听中
            // compo_textbox_set_forecolor(txt, COLOR_GREEN);
            compo_animation_set_visible(animation2, true);
            compo_animation_set_visible(animation1, false);
            compo_picturebox_set_visible(pic_connected, true);
            compo_picturebox_set_visible(pic_disconnected, false);

            f_cb->is_listen = true;
            f_cb->is_conn = true;
            break;
        }
        case CHATEVT_EXIT:
            if(!f_cb->exiting)
            {
                printf("zzp1\n");
                f_cb->exiting = true;
                func_back_to();
            }

            break;
        case CHATEVT_TTS_PLAYING:
        {
            chatbot_network = true;
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Speaking");//说话中
            //compo_textbox_set_forecolor(txt, COLOR_RED);
            compo_animation_set_visible(animation1, true);
            compo_animation_set_visible(animation2, false);
            compo_picturebox_set_visible(pic_connected, true);
            compo_picturebox_set_visible(pic_disconnected, false);

            f_cb->is_listen = false;
            break;
        }
        case CHATEVT_LISTENING:
        {
            chatbot_network = true;
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
            chatbot_network = false;
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Reconnect");//重新连接中
            //compo_textbox_set_forecolor(txt, COLOR_WHITE);
            compo_animation_set_visible(animation1, false);
            compo_animation_set_visible(animation2, false);
            compo_picturebox_set_visible(pic_connected, false);
            compo_picturebox_set_visible(pic_disconnected,true );

            f_cb->need_reconn = true;
            f_cb->is_conn = false;
            break;
        }
        case CHATEVT_THINKING:
        {
            chatbot_network = true;
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
            compo_textbox_set(txt, "Thinking...");//思考中
            // compo_textbox_set_forecolor(txt, COLOR_YELLOW);
            compo_animation_set_visible(animation2, true);
            compo_animation_set_visible(animation1, false);
            f_cb->is_listen = false;
            break;
        }
        case CHATEVT_ERROR_BROKEN:
            if(!f_cb->exiting)
            {
                chatbot_network = false;
                f_cb->exiting = true;
                func_back_to();
            }
            break;
        default:
            break;
    }
}

// 聊天机器人功能事件处理
static void func_chatbot_process(void)
{
    if(!chatbot_network && ++chatbot_num > 500)
    {
        chatbot_num = 0;
        //printf("zzp_chatbot_network\n");
        bt_panu_network_connect();

        // extern void zzp_btnum(void);
        // zzp_btnum();
    }

    f_chatbot_t *f_cb = func_cb.f_cb;
    bool current_net_state = bnep_network_is_ok();  // 获取当前网络状态


    //检测网络从失败变为成功
    if (current_net_state && !f_cb->last_net_state)
    {
        //重新初始化（如果之前未初始化成功）
        if (!f_cb->is_conn && chatbot_init())
        {
            f_cb->is_conn = true;
            chatbot_set_event_callback(event_cb);
            chatbot_start();  // 启动后会触发CHATEVT_IS_CONN事件，进一步更新界面
        }
        else
        {
            //func_back_to();
        }
    }

    // 更新上一次网络状态
    f_cb->last_net_state = current_net_state;
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
        else
        {
            //func_back_to();
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

    f_chatbot_t *f_cb = func_cb.f_cb;
    f_cb->last_net_state = bnep_network_is_ok();  // 记录初始网络状态

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
        chatbot_set_event_callback(event_cb);
        chatbot_start();
    }
    else
    {
        //func_back_to();

    }

    // if (gcal_cb_init() == false)
    // {
    //     halt(HALT_MALLOC);
    // }
}

// 退出聊天机器人功能
static void func_chatbot_exit(void)
{
    f_chatbot_t *f_cb = func_cb.f_cb;
    //gcal_cb_destroy();

    //f_cb->exiting = true;
    // f_cb->is_conn = false;
    // chatbot_deinit();
    // mem_monitor_run();


    compo_form_t *frm = func_create_form(func_cb.sta);
    f_cb->exiting = true;
    f_cb->is_conn = false;
    printf("zzp1\n");
    chatbot_deinit();
    printf("zzp2\n");
    compo_form_destroy(frm);


}

// 聊天机器人功能
void func_chatbot(void)
{
//    bsp_set_volume(VOL_MAX);
    printf("%s\n", __func__);
    func_chatbot_enter();
    while (func_cb.sta == FUNC_CHATBOT)
    {
        func_chatbot_process();
        func_chatbot_message(msg_dequeue());
    }
    func_chatbot_exit();
}
