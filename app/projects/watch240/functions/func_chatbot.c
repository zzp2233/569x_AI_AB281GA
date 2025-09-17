#include "include.h"
#include "func.h"
#include "func_calculator_alg.h"
#include "ute_module_smoke.h"

#include "chatbot.h"

#define TRACE_EN 1

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

static bool chatbot_network = false;
static u16 chatbot_num = 0;
static bool chatbot_VOL = false;
static u8 saved_volume = 0; // 保存进入机器人界面前的音量


enum
{
    COMPO_ID_TEXT_STATUS = 0xff, // 状态文本框的唯一标识符，值为255
    COMPO_ID_BTN_SPEAKING,       // AI正在说话动画的标识符，值为256
    COMPO_ID_BTN_SPEAKED,        // AI已说话完成动画的标识符，值为257
    COMPO_ID_BTN_CONNECTED,      // 网络已连接图标的标识符，值为258
    COMPO_ID_BTN_DISCONNECTED,   // 网络未连接图标的标识符，值为259
    COMPO_ID_CHATBOT_REMIND,
    COMPO_ID_CHATBOT_CHINA,
    COMPO_ID_TXT_CHINA,
};

typedef struct f_chatbot_t_
{
    char status[32];     // 当前状态描述字符串，最大31个字符+结束符
    bool is_conn;        // 是否已连接到聊天服务器的标志位
    bool is_listen;      // 是否正在监听语音输入的标志位
    bool need_reconn;    // 是否需要重新连接的标志位
    bool last_net_state; // 记录上一次网络连接状态，用于检测网络状态变化
    bool exiting;        // 是否正在退出聊天功能的标志位
    bool back_to;        // 是否需要返回上一界面的标志位
    bool wait_disconn;
    u32 wait_disconn_tick;
    u8 animation_type; // 1机器人2黑衣女人3中东女人
} f_chatbot_t;

typedef struct chatbot_disp_btn_item_t_
{
    u32 res_addr; // 图片资源的内存地址
    u16 btn_id;   // 按钮的唯一标识符
    s16 x;        // 按钮在屏幕上的X坐标位置
    s16 y;        // 按钮在屏幕上的Y坐标位置
} chatbot_disp_btn_item_t;

// 创建聊天机器人窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_chatbot_form_create(void)
{
    //  f_chatbot_t *f_cb = func_cb.f_cb;
    u8 animation_type = 2;                    // 1机器人2黑衣女人3中东女人
    // 新建窗体和背景 - 创建一个新的窗体容器作为所有UI组件的父容器
    compo_form_t *frm = compo_form_create(true); // 参数true表示将窗体放在界面顶层
    compo_picturebox_t *picbox;                  // 声明图片框指针变量

    // 创建AI已说话状态的静态图片组件
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_AI_SPEAKED_BIN);
    if (animation_type == 1)
    {
        compo_picturebox_cut(picbox, 0, 7);
    }
    else if (animation_type == 2)
    {
        compo_picturebox_cut(picbox, 0, 38);
    }
    else if (animation_type == 3)
    {
        compo_picturebox_cut(picbox, 0, 31);
    }
    // compo_picturebox_cut(picbox, 0, 7);          // 将图片切割为31帧动画序列的第0帧（静态显示第一帧）
    compo_picturebox_set_pos(picbox, 120, 148);

    // 创建AI正在说话时的动画组件
    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I330001_AI_SPEAKING_BIN);
    compo_animation_set_pos(animation, 120, 148);
    if (animation_type == 1)
    {
        compo_animation_set_radix(animation, 16);
        compo_animation_set_interval(animation, 25);
    }
    else if (animation_type == 2)
    {
        compo_animation_set_radix(animation, 38);
        compo_animation_set_interval(animation, 18);
    }
    else if (animation_type == 3)
    {
        compo_animation_set_radix(animation, 28);
        compo_animation_set_interval(animation, 18);
    }
    // compo_animation_set_radix(animation, 16);             // 设置动画总帧数为28帧
    // compo_animation_set_interval(animation, 35);
    compo_setid(animation, COMPO_ID_BTN_SPEAKING);
    compo_animation_set_visible(animation, false);

    // 创建AI已说话完成时的动画组件
    animation = compo_animation_create(frm, UI_BUF_I330001_AI_SPEAKED_BIN);
    compo_animation_set_pos(animation, 120, 148);
    if (animation_type == 1)
    {
        compo_animation_set_radix(animation, 7); // 设置动画总帧数为31帧
        compo_animation_set_interval(animation, 60);
    }
    else if (animation_type == 2)
    {
        compo_animation_set_radix(animation, 38);
        compo_animation_set_interval(animation, 18);
    }
    else if (animation_type == 3)
    {
        compo_animation_set_radix(animation, 31);
        compo_animation_set_interval(animation, 18);
    }
    // compo_animation_set_radix(animation, 7);             // 设置动画总帧数为31帧
    // compo_animation_set_interval(animation, 35);
    compo_setid(animation, COMPO_ID_BTN_SPEAKED);
    compo_animation_set_visible(animation, false);

    // 创建网络已连接状态的图标（默认隐藏）
    compo_picturebox_t *pic_connected = compo_picturebox_create(frm, UI_BUF_I330001_AI_CONNECTED_BIN); // 创建已连接状态图标
    compo_picturebox_set_pos(pic_connected, 40, 40);                                                   // 设置图标位置为屏幕左上角(40, 40)
    compo_setid(pic_connected, COMPO_ID_BTN_CONNECTED);                                                // 分配唯一ID为COMPO_ID_BTN_CONNECTED
    compo_picturebox_set_visible(pic_connected, false);                                                // 设置图标默认为隐藏状态

    // 创建网络未连接状态的图标（默认显示）
    compo_picturebox_t *pic_disconnected = compo_picturebox_create(frm, UI_BUF_I330001_AI_NOT_CONNECTED_BIN); // 创建未连接状态图标
    compo_picturebox_set_pos(pic_disconnected, 40, 40);                                                       // 设置图标位置为(40, 40)，与已连接图标位置相同（通过可见性切换显示）
    compo_setid(pic_disconnected, COMPO_ID_BTN_DISCONNECTED);                                                 // 分配唯一ID为COMPO_ID_BTN_DISCONNECTED
    compo_picturebox_set_visible(pic_disconnected, true);                                                     // 设置图标默认为显示状态（初始显示未连接状态）

    // 创建状态显示文本框
    compo_textbox_t *txt;                                 // 声明文本框指针变量
    txt = compo_textbox_create(frm, 32);                  // 在窗体中创建文本框，最大字符数为32
    compo_setid(txt, COMPO_ID_TEXT_STATUS);               // 分配唯一ID为COMPO_ID_TEXT_STATUS
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 240); // 设置文本框位置：X坐标为屏幕中心，Y坐标为240
    compo_textbox_set_autosize(txt, true);                // 启用文本框自动调整大小功能（根据文本内容自动调整尺寸）
    compo_textbox_set(txt, "Connecting...");              // 设置文本框显示内容为"Connecting..."（连接中提示）
    compo_textbox_set_forecolor(txt, COLOR_WHITE);        // 设置文本颜色为白色

    compo_button_t *btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_CHATBOT_REMIND);
    compo_button_set_location(btn, 40, 40, 50, 50);

    compo_button_t *btn2 = compo_button_create(frm);
    compo_button_set_location(btn2, 35, 60, 60, 60);
    compo_setid(btn2, COMPO_ID_CHATBOT_CHINA);

    txt = compo_textbox_create(frm, 10);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, (uteModuleSmokeData.is_out_chaina == 0) ? "C" : "O");
    compo_textbox_set_pos(txt, 35, 60);
    compo_setid(txt, COMPO_ID_TXT_CHINA);

    // 设置全局网络状态变量并返回窗体
    chatbot_network = false; // 设置全局网络连接状态为false（未连接状态）
    return frm;              // 返回创建完成的窗体指针，供调用者使用
}

static void event_cb(chatbot_event_t event)
{
    f_chatbot_t *f_cb = func_cb.f_cb;                                                    // 获取聊天机器人功能状态结构体指针
    printf("\nchatbot event: %d\n", event);                                              // 打印当前接收到的事件类型（用于调试）
    compo_animation_t *animation1 = compo_getobj_byid(COMPO_ID_BTN_SPEAKING);            // 获取"正在说话"动画组件
    compo_animation_t *animation2 = compo_getobj_byid(COMPO_ID_BTN_SPEAKED);             // 获取"已说话完成"动画组件
    compo_picturebox_t *pic_connected = compo_getobj_byid(COMPO_ID_BTN_CONNECTED);       // 获取"已连接"图标组件
    compo_picturebox_t *pic_disconnected = compo_getobj_byid(COMPO_ID_BTN_DISCONNECTED); // 获取"未连接"图标组件
    switch (event)
    {
        case CHATEVT_IS_CONN:
        {
            chatbot_network = true;                                         // 设置全局网络状态为已连接
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS); // 获取状态文本框
            compo_textbox_set(txt, "Listening");                            // 更新状态文本为"监听中"
            compo_animation_set_visible(animation2, true);                  // 显示"已说话完成"动画（监听状态）
            compo_animation_set_visible(animation1, false);                 // 隐藏"正在说话"动画
            compo_picturebox_set_visible(pic_connected, true);              // 显示"已连接"图标
            compo_picturebox_set_visible(pic_disconnected, false);          // 隐藏"未连接"图标

            f_cb->is_listen = true; // 设置内部状态为监听模式
            f_cb->is_conn = true;   // 设置内部状态为已连接
            break;
        }

        case CHATEVT_EXIT:
            if (!f_cb->exiting) // 检查是否已经在退出过程中（防止重复退出）
            {
                printf("zzp1\n");     // 调试输出
                f_cb->exiting = true; // 标记正在退出
                f_cb->back_to = true; // 标记需要返回上一界面
                // func_back_to();                                            // 返回函数调用
            }
            break;

        case CHATEVT_TTS_PLAYING:
        {
            chatbot_network = true;                                         // 确保网络状态为已连接
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS); // 获取状态文本框
            compo_textbox_set(txt, "Speaking");                             // 更新状态文本为"说话中"
            compo_animation_set_visible(animation1, true);                  // 显示"正在说话"动画
            compo_animation_set_visible(animation2, false);                 // 隐藏"已说话完成"动画
            compo_picturebox_set_visible(pic_connected, true);              // 显示"已连接"图标
            compo_picturebox_set_visible(pic_disconnected, false);          // 隐藏"未连接"图标

            f_cb->is_listen = false; // 设置内部状态为非监听模式（AI在说话时不监听用户）
            break;
        }

        case CHATEVT_LISTENING:
        {
            chatbot_network = true;                                         // 确保网络状态为已连接
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS); // 获取状态文本框
            compo_textbox_set(txt, "Listening");                            // 更新状态文本为"监听中"
            compo_animation_set_visible(animation2, true);                  // 显示"已说话完成"动画（监听状态）
            compo_animation_set_visible(animation1, false);                 // 隐藏"正在说话"动画

            f_cb->is_listen = true; // 设置内部状态为监听模式
            break;
        }

        case CHATEVT_ERROR_RECONN:
        {
            chatbot_network = false;                                        // 设置全局网络状态为未连接
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS); // 获取状态文本框
            compo_textbox_set(txt, "Reconnect");                            // 更新状态文本为"重新连接中"
            compo_animation_set_visible(animation1, false);                 // 隐藏"正在说话"动画
            compo_animation_set_visible(animation2, false);                 // 隐藏"已说话完成"动画
            compo_picturebox_set_visible(pic_connected, false);             // 隐藏"已连接"图标
            compo_picturebox_set_visible(pic_disconnected, true);           // 显示"未连接"图标

            f_cb->need_reconn = true; // 标记需要重新连接
            f_cb->is_conn = false;    // 设置内部状态为未连接
            break;
        }

        case CHATEVT_THINKING:
        {
            chatbot_network = true;                                         // 确保网络状态为已连接
            compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_STATUS); // 获取状态文本框
            compo_textbox_set(txt, "Thinking...");                          // 更新状态文本为"思考中"
            compo_animation_set_visible(animation2, true);                  // 显示"已说话完成"动画（思考状态）
            compo_animation_set_visible(animation1, false);                 // 隐藏"正在说话"动画
            f_cb->is_listen = false;                                      // 设置内部状态为非监听模式（AI思考时不监听）
            chatbot_send_vape_record(get_today_smoking_count(), get_this_week_smoking_count());
            break;
        }

        case CHATEVT_ERROR_BROKEN:
            printf("f_cb->exiting=%d\n", f_cb->exiting); // 调试输出当前退出状态
            if (!f_cb->exiting)                          // 检查是否已经在退出过程中
            {
                chatbot_network = false; // 设置全局网络状态为未连接
                f_cb->exiting = true;    // 标记正在退出
                f_cb->back_to = true;    // 标记需要返回上一界面
                // func_back_to();                                            // 掉的返回函数调用
            }
            break;
        default:
            break;
    }
}

static void mcp_cb(chatbot_mcp_t mcp, void *param)
{

    switch (mcp)
    {

        case CHATMCP_POWERUP:
            printf("CHATMCP_POWERUP\n");
            msg_enqueue(EVT_VAPE_POWERUP);
            break;
        case CHATMCP_POWERDOWN:
            printf("CHATMCP_POWERDOWN\n");
            msg_enqueue(EVT_VAPE_POWERDOWN);
            break;

        default:
            break;
    }
}

static void chatbot_start_do(void)
{

    bd_addr_t addr;
    bt_get_local_bd_addr((u8 *)&addr);
    chatbot_set_mac(&addr);
    chatbot_send_vape_record(get_today_smoking_count(), get_this_week_smoking_count()); //修改数据，发送给机器人

    chatbot_set_event_callback(event_cb);
    chatbot_set_mcp_callback(mcp_cb);
    chatbot_start();
}

// 聊天机器人功能事件处理
static void func_chatbot_process(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_CHINA);
    if (uteModuleSmokeData.is_out_chaina == 0)
    {
        compo_textbox_set(textbox, "C");
    }
    else
    {
        compo_textbox_set(textbox, "O");
    }
    if (!chatbot_network && ++chatbot_num > 500)
    {
        chatbot_num = 0;
        // printf("zzp_chatbot_network\n");
        // bt_panu_network_connect();

        // extern void zzp_btnum(void);
        // zzp_btnum();
    }

    f_chatbot_t *f_cb = func_cb.f_cb;
    bool current_net_state = bnep_network_is_ok(); // 获取当前网络状态

    // 检测网络从失败变为成功
    if (current_net_state && !f_cb->last_net_state)
    {
        printf("111111uteModuleSmokeData.is_out_chaina=%d\n", uteModuleSmokeData.is_out_chaina);
        // 重新初始化（如果之前未初始化成功）
        if (!f_cb->is_conn && chatbot_init(uteModuleSmokeData.is_out_chaina))
        {
            f_cb->is_conn = true;
            chatbot_start_do(); // 启动后会触发CHATEVT_IS_CONN事件，进一步更新界面
        }
        else
        {
            // func_back_to();
        }
    }

    // 更新上一次网络状态
    f_cb->last_net_state = current_net_state;
    if (f_cb->need_reconn)
    {
        printf("f_cb->need_reconn = false\r\n");
        f_cb->need_reconn = false;
        f_cb->is_conn = false;
        f_cb->wait_disconn = true;
        f_cb->wait_disconn_tick = tick_get();
        chatbot_deinit();
    }

    if (f_cb->wait_disconn)
    {
        // printf("222222uteModuleSmokeData.is_out_chaina=%d\n", uteModuleSmokeData.is_out_chaina);
        if (chatbot_init(uteModuleSmokeData.is_out_chaina))
        {
            f_cb->wait_disconn = false;
            chatbot_start_do();
        }
        if (tick_check_expire(f_cb->wait_disconn_tick, 4000))
        {
            f_cb->wait_disconn = false;
            printf("zzp2\n");
            func_back_to();
        }
    }
    reset_sleep_delay_all(); // 不休眠

    if (f_cb->back_to)
    {
        f_cb->back_to = false;
        printf("zzp3\n");
        func_back_to();
    }
    func_process();
}

#include "chatbot.h"
// 单击按钮
static void func_chatbot_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_CHATBOT_REMIND:
            // func_cb.sta = FUNC_CHATBOT_REMIND;//FUNC_SET_SUB_ABOUT;//FUNC_CHATBOT_REMIND;

            break;
        case COMPO_ID_CHATBOT_CHINA:
            uteModuleSmokeData.is_out_chaina = !uteModuleSmokeData.is_out_chaina;
            uteModuleSmokeDataSaveConfig();
            // chatbot_start_do();
            break;
        default:
            break;
    }
}
// 聊天机器人功能消息处理
static void func_chatbot_message(size_msg_t msg)
{
    f_chatbot_t *f_cb = func_cb.f_cb;
    compo_animation_t *animation1 = compo_getobj_byid(COMPO_ID_BTN_SPEAKING);
    compo_animation_t *animation2 = compo_getobj_byid(COMPO_ID_BTN_SPEAKED);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_CHATBOT_REMIND);
    int id = compo_get_button_id();
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
                compo_textbox_set(txt, "Listening"); // 监听中
                // compo_textbox_set_forecolor(txt, COLOR_GREEN);
                compo_animation_set_visible(animation2, true);
                compo_animation_set_visible(animation1, false);

                f_cb->is_listen = true;
                // chatbot_start_mic();
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
                // chatbot_stop_mic();
            }
            break;

        case MSG_CTP_CLICK:
            func_chatbot_button_click();
            break;
        // case EVT_VAPE_POWERUP:
        //     printf("EVT_VAPE_POWERUP\n");
        //     break;
        // case EVT_VAPE_POWERDOWN:
        //     printf("EVT_VAPE_POWERDOWN\n");
        //     break;

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

    f_cb->last_net_state = bnep_network_is_ok(); // 记录初始网络状态

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
    printf("33333uteModuleSmokeData.is_out_chaina=%d\n", uteModuleSmokeData.is_out_chaina);
    if (chatbot_init(uteModuleSmokeData.is_out_chaina))
    {
        chatbot_start_do();
    }
    else
    {
        // func_back_to();
    }

    // if (gcal_cb_init() == false)
    // {
    //     halt(HALT_MALLOC);
    // }
}

// 退出聊天机器人功能
static void func_chatbot_exit(void)
{
    // 恢复保存的音量
    if (saved_volume > 0)
    {
        bsp_set_volume(saved_volume);
        printf("chatbot exit - restored volume: %d\\n", saved_volume);
        saved_volume = 0; // 清除保存的音量
    }

    f_chatbot_t *f_cb = func_cb.f_cb;
    // gcal_cb_destroy();

    // f_cb->exiting = true;
    //  f_cb->is_conn = false;
    //  chatbot_deinit();
    //  mem_monitor_run();

    compo_form_t *frm = func_create_form(func_cb.sta);
    f_cb->exiting = true;
    f_cb->is_conn = false;
    chatbot_deinit();
    compo_form_destroy(frm);
}

// 聊天机器人功能
void func_chatbot(void)
{
    // 保存当前音量
    saved_volume = sys_cb.vol;
    printf("chatbot enter - saved volume: %d\n", saved_volume);

    // 设置为最大音量
    bsp_set_volume(VOL_MAX);
    printf("chatbot enter - set volume to max: %d\n", VOL_MAX);

    // watch_point_set(&func_cb.sta);
    printf("%s\n", __func__);
    bt_panu_network_connect(); // 进入机器人界面默认打开网络连接
    func_chatbot_enter();
    while (func_cb.sta == FUNC_CHATBOT)
    {
        func_chatbot_process();
        func_chatbot_message(msg_dequeue());
    }
    // netlib_run_test();
    func_chatbot_exit();
}
