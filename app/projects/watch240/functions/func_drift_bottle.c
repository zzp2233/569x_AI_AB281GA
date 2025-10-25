#include "include.h"
#include "func.h"
#include "func_calculator_alg.h"
#include "ute_module_smoke.h"

#include "chatbot.h"
#include <string.h>


#define TRACE_EN 1

#if TRACE_EN
#define TRACE(...) printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define DRIFT_BOTTLE_COUNT 8    // 漂流瓶总数量  
#define DRIFT_BOTTLE_COLS 4     // 每行显示4个瓶子(2行x4列布局)  
#define DRIFT_MESSAGE_COUNT 6   // 每个瓶子最多6条消息
#define DRIFT_MESSAGE_VISIBLE_COUNT 4   // messages visible at once
#define DRIFT_MESSAGE_TEXT_OFFSET_Y 6   // 垂直方向偏移,用于对齐角色/编号文本
#define DRIFT_BOTTLE_INVALID 0xFF  // Sentinel used when no bottle is selected
#define DRIFT_TEXT_Y 15
// 视图状态枚举:瓶子列表视图
typedef enum
{
    DRIFT_VIEW_BOTTLES = 0,    // 显示8个漂流瓶的网格视图
    DRIFT_VIEW_MESSAGES,       // 显示选中瓶子的消息列表
} drift_bottle_view_t;

static bool drift_bottle_network = false;
static u16 drift_bottle_num = 0;
static bool drift_bottle_vol = false;
static u8 saved_volume = 0; // 保存进入机器人界面前的音量

extern uint8_t  power_ecig;
extern uint8_t  power_idx;
extern uint16_t tick_200ms;

enum
{
    COMPO_ID_TEXT_STATUS = 0xff,        // 状态文本框ID(值255):显示"Connected"/"Listening"等状态
    COMPO_ID_WARNING_TEXT,              // 警告文本框ID(值256):显示"! please clean the bottle"等警告
    COMPO_ID_BTN_CANCEL,                // 取消按钮ID(值257):左下角取消/返回按钮
    COMPO_ID_BTN_SEND,                  // 发送按钮ID(值258):中下发送语音消息按钮
    COMPO_ID_BTN_CONFIRM,               // 确认按钮ID(值259):右下角确认/清空按钮
    COMPO_ID_BTN_BACK,                  // 返回按钮ID(值260):消息视图右上角返回按钮

    /* 漂流瓶相关组件ID范围 */
    COMPO_ID_BOTTLE_BASE,                                                                 // 瓶子图片ID起始值(值261)
    COMPO_ID_BOTTLE_END = COMPO_ID_BOTTLE_BASE + DRIFT_BOTTLE_COUNT - 1,                  // 瓶子图片ID结束值(值268)

    COMPO_ID_BOTTLE_BUTTON_BASE,                                                          // 瓶子按钮ID起始值(值269):透明按钮覆盖在瓶子图片上
    COMPO_ID_BOTTLE_BUTTON_END = COMPO_ID_BOTTLE_BUTTON_BASE + DRIFT_BOTTLE_COUNT - 1,    // 瓶子按钮ID结束值(值276)

    COMPO_ID_BOTTLE_LABEL_BASE,                                                           // 瓶子标签ID起始值(值277):显示瓶子编号1-8
    COMPO_ID_BOTTLE_LABEL_END = COMPO_ID_BOTTLE_LABEL_BASE + DRIFT_BOTTLE_COUNT - 1,      // 瓶子标签ID结束值(值284)

    /* 消息相关组件ID范围 */
    COMPO_ID_MSG_BUTTON_BASE,                                                             // 消息按钮ID起始值(值285):显示消息的已读/未读状态
    COMPO_ID_MSG_BUTTON_END = COMPO_ID_MSG_BUTTON_BASE + DRIFT_MESSAGE_COUNT - 1,         // 消息按钮ID结束值(值288)

    COMPO_ID_MSG_ROLE_TEXT_BASE,                                                          // 消息角色文本ID起始值(值289):显示"Me"或"1"表示发送者
    COMPO_ID_MSG_ROLE_TEXT_END = COMPO_ID_MSG_ROLE_TEXT_BASE + DRIFT_MESSAGE_COUNT - 1,   // 消息角色文本ID结束值(值292)

    COMPO_ID_MSG_INDEX_TEXT_BASE,                                                         // 消息索引文本ID起始值(值293):显示消息编号1-4
    COMPO_ID_MSG_INDEX_TEXT_END = COMPO_ID_MSG_INDEX_TEXT_BASE + DRIFT_MESSAGE_COUNT - 1, // 消息索引文本ID结束值(值296)
};

// 辅助宏:根据索引计算组件ID
/* 组件ID计算宏 ------------------------------------------------------------------*/
/**
  * @brief  根据索引计算瓶子图片的组件ID
  * @param  idx: 瓶子索引(0-7)
  * @retval 对应的组件ID
  */
#define DRIFT_BOTTLE_PIC_ID(idx)        (COMPO_ID_BOTTLE_BASE + (idx))

/**
  * @brief  根据索引计算瓶子按钮的组件ID
  * @param  idx: 瓶子索引(0-7)
  * @retval 对应的组件ID
  */
#define DRIFT_BOTTLE_BUTTON_ID(idx)     (COMPO_ID_BOTTLE_BUTTON_BASE + (idx))

/**
  * @brief  根据索引计算瓶子标签的组件ID
  * @param  idx: 瓶子索引(0-7)
  * @retval 对应的组件ID
  */
#define DRIFT_BOTTLE_LABEL_ID(idx)      (COMPO_ID_BOTTLE_LABEL_BASE + (idx))

/**
  * @brief  根据索引计算消息按钮的组件ID
  * @param  idx: 消息索引(0-3)
  * @retval 对应的组件ID
  */
#define DRIFT_MESSAGE_BUTTON_ID(idx)    (COMPO_ID_MSG_BUTTON_BASE + (idx))

/**
  * @brief  根据索引计算消息角色文本的组件ID
  * @param  idx: 消息索引(0-3)
  * @retval 对应的组件ID
  */
#define DRIFT_MESSAGE_ROLE_ID(idx)      (COMPO_ID_MSG_ROLE_TEXT_BASE + (idx))

/**
  * @brief  根据索引计算消息索引文本的组件ID
  * @param  idx: 消息索引(0-3)
  * @retval 对应的组件ID
  */
#define DRIFT_MESSAGE_INDEX_ID(idx)     (COMPO_ID_MSG_INDEX_TEXT_BASE + (idx))

/* 功能状态结构体定义 ------------------------------------------------------------*/
/**
  * @brief  漂流瓶功能状态结构体
  *         存储漂流瓶功能运行时的所有状态信息
  */
typedef struct f_drift_bottle_t_
{
    char status[32];                    // 当前状态描述字符串:最大31个字符+'\0'结束符
    bool is_conn;                       // chatbot连接状态:true=已连接到服务器,false=未连接
    bool is_listen;                     // 语音监听状态:true=正在监听用户语音,false=未监听
    bool need_reconn;                   // 重连请求标志:true=需要重新连接,false=不需要
    bool last_net_state;                // 上一次网络状态:用于检测网络状态变化(从断开到连接)
    bool exiting;                       // 退出标志:true=正在退出功能,false=正常运行
    bool back_to;                       // 返回标志:true=需要返回上一界面,false=继续当前界面
    bool wait_disconn;                  // 等待断开标志:true=等待断开连接完成,false=不等待
    u32 wait_disconn_tick;              // 断开等待开始时间:记录开始等待的系统tick值
    u8 animation_type;                  // 动画类型:1=机器人,2=黑衣女人,3=中东女人(当前未使用)
    chatbot_info_t info;                // chatbot配置信息:包含API密钥、客户端ID等

    drift_bottle_view_t active_view;    // 当前活动视图:DRIFT_VIEW_BOTTLES或DRIFT_VIEW_MESSAGES
    u8 selected_bottle;                 // 当前选中的瓶子索引:0-7,指示用户选中了哪个瓶子
    u8 message_unread_count[DRIFT_BOTTLE_COUNT];  // 每个瓶子的未读消息数:数组索引对应瓶子编号
    bool bottle_has_message[DRIFT_BOTTLE_COUNT];  // 每个瓶子是否有消息:true=有消息,false=空瓶
    s16 message_start_y;               // 消息列表起始中心Y坐标
    s16 message_spacing;               // 消息项之间的垂直间距
    s16 message_half_height;           // 消息按钮的一半高度
    s16 message_view_bottom;           // 可见区域底部中心Y坐标
    s16 message_scroll_offset;         // 当前消息滚动偏移
    s16 message_scroll_max;            // 消息滚动的最大偏移
    s16 message_base_y[DRIFT_MESSAGE_COUNT]; // 记录每条消息的原始中心Y坐标
    bool pending_voice_ready;          // 是否存在待确认的语音消息
    bool pending_target_empty;        // 长按发送后是否准备投递到空瓶
    u8 pending_target_index;            // 长按准备发送的目标瓶子索引
    /**
      * @brief  消息状态结构体数组
      *         存储当前选中瓶子的6条消息的状态信息
      */
    struct
    {
        bool from_me;                   // 消息发送者:true=我发送的,false=对方发送的
        bool unread;                    // 消息已读状态:true=未读,false=已读
    } messages[DRIFT_MESSAGE_COUNT];    // 6条消息的状态数组
} f_drift_bottle_t;

static void drift_bottle_set_active_view(f_drift_bottle_t *f_cb, drift_bottle_view_t view);
static void drift_bottle_update_bottle_ui(f_drift_bottle_t *f_cb);
static void drift_bottle_update_messages(f_drift_bottle_t *f_cb);
static bool drift_bottle_scroll_by(f_drift_bottle_t *f_cb, s16 delta);
static bool drift_bottle_add_voice_to_bottle(f_drift_bottle_t *f_cb, u8 bottle_index);
static bool drift_bottle_send_voice_to_empty(f_drift_bottle_t *f_cb, compo_textbox_t *status);
static void drift_bottle_prepare_voice_decision(f_drift_bottle_t *f_cb, compo_textbox_t *status);
static void drift_bottle_abort_voice_capture(f_drift_bottle_t *f_cb, compo_textbox_t *status, const char *message);

static bool drift_bottle_scroll_by(f_drift_bottle_t *f_cb, s16 delta)
{
    if (!f_cb || f_cb->message_scroll_max <= 0)
    {
        return false;
    }

    s16 target = f_cb->message_scroll_offset + delta;

    if (target < 0)
    {
        target = 0;
    }
    else if (target > f_cb->message_scroll_max)
    {
        target = f_cb->message_scroll_max;
    }

    if (target == f_cb->message_scroll_offset)
    {
        return false;
    }

    f_cb->message_scroll_offset = target;
    return true;
}

static void drift_bottle_sync_warning(f_drift_bottle_t *f_cb);
static void drift_bottle_init_demo_data(f_drift_bottle_t *f_cb);
static void drift_bottle_handle_bottle_select(f_drift_bottle_t *f_cb, u8 index);
static void drift_bottle_set_send_button_pressed(bool pressed);
static void drift_bottle_set_decision_buttons_visible(bool visible);

/**
  * @brief  创建漂流瓶功能的主窗体
  * @note   此函数在进入漂流瓶界面时被调用,创建所有UI组件
  *         包括:状态文本框、警告文本框、底部按钮、8个漂流瓶、6条消息显示
  * @param  None
  * @retval compo_form_t* 返回创建的窗体指针
  */
compo_form_t *func_drift_bottle_form_create(void)
{
    // 创建顶层窗体容器,参数true表示将窗体放在界面顶层
    compo_form_t *frm = compo_form_create(true);
    f_drift_bottle_t *f_cb = func_cb.f_cb;

    if (f_cb)
    {
        f_cb->pending_voice_ready = false;
        f_cb->pending_target_empty = false;
    }


    // ========== 创建状态文本框 ==========
    // 用于显示"Connected"/"Listening"/"Please connect phone"等状态信息
    compo_textbox_t *txt = compo_textbox_create(frm, 48);                          // 创建文本框,最大容纳48个字符
    compo_setid(txt, COMPO_ID_TEXT_STATUS);                                        // 设置组件ID为COMPO_ID_TEXT_STATUS(值255)
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_20_BIN);                     // 设置字体为20号字体资源
    compo_textbox_set_align_center(txt, true);                                     // 设置文本居中对齐
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, DRIFT_TEXT_Y);                           // 设置位置:X=屏幕中心,Y=32像素
    compo_textbox_set_forecolor(txt, COLOR_WHITE);                                 // 设置文本颜色为白色

    // ========== 创建警告文本框 ==========
    // 用于显示"! please clean the bottle"等警告信息
    compo_textbox_t *warning = compo_textbox_create(frm, 64);                      // 创建文本框,最大容纳64个字符
    compo_setid(warning, COMPO_ID_WARNING_TEXT);                                   // 设置组件ID为COMPO_ID_WARNING_TEXT(值256)
    compo_textbox_set_font(warning, UI_BUF_0FONT_FONT_NUM_20_BIN);                 // 设置字体为20号字体资源
    compo_textbox_set_align_center(warning, true);                                 // 设置文本居中对齐
    compo_textbox_set_pos(warning, GUI_SCREEN_CENTER_X, 223);                      // 设置位置:X=屏幕中心,Y=223像素
    compo_textbox_set_forecolor(warning, COLOR_YELLOW);                            // 设置文本颜色为黄色(警告色)
    compo_textbox_set(warning, "");                                                // 初始文本内容为空字符串

    // ========== 创建底部三个功能按钮 ==========
    // 取消按钮(左下角)
    compo_button_t *btn_cancel = compo_button_create_by_image(frm,                 // 通过图片资源创建按钮
                                 UI_BUF_I330001_DRIFT_BOTTLE_BLACK_CANCEL_BIN);                             // 使用取消按钮的图片资源
    compo_setid(btn_cancel, COMPO_ID_BTN_CANCEL);                                  // 设置组件ID为COMPO_ID_BTN_CANCEL(值257)
    compo_button_set_pos(btn_cancel, 48, GUI_SCREEN_HEIGHT - 40);                  // 设置位置:X=48像素(左侧),Y=屏幕高度-40像素(底部)
    compo_button_set_visible(btn_cancel, false);                                   // 初始状态设置为隐藏

    // 发送语音消息按钮(中下)
    compo_button_t *btn_send = compo_button_create_by_image(frm,                   // 通过图片资源创建按钮
                               UI_BUF_I330001_DRIFT_BOTTLE_BLACK_SEND_VOICE_MESSAGE_BIN);                 // 使用发送语音消息按钮的图片资源
    compo_setid(btn_send, COMPO_ID_BTN_SEND);                                      // 设置组件ID为COMPO_ID_BTN_SEND(值258)
    compo_button_set_pos(btn_send, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT - 40);   // 设置位置:X=屏幕中心,Y=屏幕高度-40像素(底部中央)

    //发送语音消息按钮(中下,橙色，隐藏，按下的时候才显示)black_send_in_progress_message  UI_BUF_I330001_DRIFT_BOTTLE_BLACK_SEND_IN_PROGRESS_MESSAGE_BIN

    // 确认/清空按钮(右下角)
    compo_button_t *btn_confirm = compo_button_create_by_image(frm,                // 通过图片资源创建按钮
                                  UI_BUF_I330001_DRIFT_BOTTLE_BLACK_NOTARIZE_BIN);                           // 使用确认按钮的图片资源
    compo_setid(btn_confirm, COMPO_ID_BTN_CONFIRM);                                // 设置组件ID为COMPO_ID_BTN_CONFIRM(值259)
    compo_button_set_pos(btn_confirm, GUI_SCREEN_WIDTH - 48,                       // 设置位置:X=屏幕宽度-48像素(右侧)
                         GUI_SCREEN_HEIGHT - 40);                                  // Y=屏幕高度-40像素(底部)
    compo_button_set_visible(btn_confirm, false);                                   // 初始状态设置为隐藏

    // ========== 创建返回按钮(仅在消息视图显示) ==========
    compo_button_t *btn_back = compo_button_create_by_image(frm,                   // 通过图片资源创建按钮
                               UI_BUF_I330001_DRIFT_BOTTLE_BLACK_GO_BACK_BIN);                            // 使用返回按钮的图片资源
    compo_setid(btn_back, COMPO_ID_BTN_BACK);                                      // 设置组件ID为COMPO_ID_BTN_BACK(值260)
    compo_button_set_pos(btn_back, GUI_SCREEN_WIDTH - 32, 48);                     // 设置位置:X=屏幕宽度-32像素(右上角),Y=48像素
    compo_button_set_visible(btn_back, false);                                     // 初始状态设置为隐藏(只在消息视图显示)

    // ========== 计算漂流瓶布局参数 ==========
    area_t bottle_size = gui_image_get_size(                                       // 获取瓶子图片的尺寸信息
                             UI_BUF_I330001_DRIFT_BOTTLE_BLACK_BOTTLE_EMPTY_BIN);                       // 使用空瓶子图片资源
    const s16 bottle_spacing_x = bottle_size.wid + 14;                             // 计算水平间距:瓶子宽度+14像素间隔
    const s16 bottle_spacing_y = bottle_size.hei + 10;                             // 计算垂直间距:瓶子高度+18像素间隔
    const s16 first_row_y = 90;                                                   // 第一行瓶子的Y坐标起始位置
    const s16 origin_x = GUI_SCREEN_CENTER_X -                                     // 计算第一个瓶子的X坐标起始位置
                         ((DRIFT_BOTTLE_COLS - 1) * bottle_spacing_x) / 2;         // 使整行瓶子居中显示:(列数-1)*间距/2

    // ========== 创建8个漂流瓶(2行×4列网格布局) ==========
    for (u8 idx = 0; idx < DRIFT_BOTTLE_COUNT; idx++)                              // 循环创建8个漂流瓶(DRIFT_BOTTLE_COUNT=8)
    {
        u8 col = idx % DRIFT_BOTTLE_COLS;                                          // 计算当前瓶子的列索引(0-3):索引对4取余
        u8 row = idx / DRIFT_BOTTLE_COLS;                                          // 计算当前瓶子的行索引(0-1):索引除以4取整
        s16 center_x = origin_x + col * bottle_spacing_x;                          // 计算当前瓶子的X坐标:起始X + 列索引*水平间距
        s16 center_y = first_row_y + row * bottle_spacing_y;                       // 计算当前瓶子的Y坐标:起始Y + 行索引*垂直间距

        // --- 创建瓶子图片组件 ---
        compo_picturebox_t *pic = compo_picturebox_create(frm,                     // 创建图片框组件
                                  UI_BUF_I330001_DRIFT_BOTTLE_BLACK_BOTTLE_EMPTY_BIN);                   // 初始使用空瓶子图片(后续会根据状态切换为满瓶)
        compo_setid(pic, DRIFT_BOTTLE_PIC_ID(idx));                                // 设置组件ID:COMPO_ID_BOTTLE_BASE + idx(261-268)
        compo_picturebox_set_pos(pic, center_x, center_y);                         // 设置图片位置为计算出的中心坐标

        // --- 创建透明按钮覆盖在瓶子上(用于点击检测) ---
        compo_button_t *btn = compo_button_create(frm);                            // 创建透明按钮(无背景图片)
        compo_setid(btn, DRIFT_BOTTLE_BUTTON_ID(idx));                             // 设置组件ID:COMPO_ID_BOTTLE_BUTTON_BASE + idx(269-276)
        compo_button_set_location(btn,                                             // 设置按钮的位置和尺寸
                                  center_x - bottle_size.wid / 2 + 25,                                   // 左上角X坐标:中心X - 宽度/2
                                  center_y - bottle_size.hei / 2 + 30,                                   // 左上角Y坐标:中心Y - 高度/2
                                  bottle_size.wid,                                                       // 按钮宽度:与瓶子图片宽度相同
                                  bottle_size.hei);                                                      // 按钮高度:与瓶子图片高度相同

        // // 创建带背景的按钮用于调试
        // compo_button_t *btn = compo_button_create_by_image(frm,
        //     UI_BUF_I330001_FIRSTORDER_00_BIN);  // 使用任意黑色背景图片资源
        // compo_setid(btn, DRIFT_BOTTLE_BUTTON_ID(idx));
        // compo_button_set_location(btn,
        //     center_x - bottle_size.wid / 2 +20,
        //     center_y - bottle_size.hei / 2 +30,
        //     bottle_size.wid,
        //     bottle_size.hei);



        // --- 创建瓶子编号标签(显示1-8) ---
        char label[3];                                                             // 声明字符数组,用于存储编号字符串(最多2位数字+结束符)
        snprintf(label, sizeof(label), "%u", idx + 1);                             // 将索引+1转换为字符串(0->1, 1->2, ..., 7->8)
        compo_textbox_t *num_txt = compo_textbox_create(frm, 3);                   // 创建文本框,最大容纳3个字符
        compo_setid(num_txt, DRIFT_BOTTLE_LABEL_ID(idx));                          // 设置组件ID:COMPO_ID_BOTTLE_LABEL_BASE + idx(277-284)
        compo_textbox_set_font(num_txt, UI_BUF_0FONT_FONT_NUM_28_BIN);             // 设置字体为28号字体资源
        compo_textbox_set_align_center(num_txt, true);                             // 设置文本居中对齐
        compo_textbox_set_forecolor(num_txt, COLOR_WHITE);                         // 设置文本颜色为白色
        compo_textbox_set(num_txt, label);                                         // 设置文本内容为编号字符串
        compo_textbox_set_pos(num_txt, center_x, center_y + 4);                    // 设置位置:X=瓶子中心X,Y=瓶子中心Y+4像素(略微下移)
    }

    // ========== 计算消息列表布局参数 ==========
    area_t message_size = gui_image_get_size(                                      // 获取消息按钮图片的尺寸信息
                              UI_BUF_I330001_DRIFT_BOTTLE_BLACK_READ_MESSAGE_BIN);                       // 使用已读消息按钮图片资源
    const s16 message_spacing = message_size.hei + 12;                             // 计算消息按钮垂直间距:按钮高度+12像素间隔
    const s16 message_start_y = 86;                                                // 第一条消息的Y坐标起始位置

    if (f_cb)
    {
        f_cb->message_spacing = message_spacing;
        f_cb->message_start_y = message_start_y;
        f_cb->message_half_height = message_size.hei / 2;
        f_cb->message_view_bottom = message_start_y + message_spacing * (DRIFT_MESSAGE_VISIBLE_COUNT - 1);
        f_cb->message_scroll_max = (DRIFT_MESSAGE_COUNT > DRIFT_MESSAGE_VISIBLE_COUNT)
                                   ? (s16)(message_spacing * (DRIFT_MESSAGE_COUNT - DRIFT_MESSAGE_VISIBLE_COUNT))
                                   : 0;
        f_cb->message_scroll_offset = 0;
    }

    // ========== 创建6条消息的UI组件(初始隐藏,切换到消息视图时显示) ==========
    for (u8 idx = 0; idx < DRIFT_MESSAGE_COUNT; idx++)                             // 循环创建6条消息(DRIFT_MESSAGE_COUNT=6)
    {
        s16 center_y = message_start_y + idx * message_spacing;                    // 计算当前消息的Y坐标:起始Y + 索引*垂直间距

        if (f_cb)
        {
            f_cb->message_base_y[idx] = center_y;
        }

        // --- 创建消息按钮(显示已读/未读状态) ---
        compo_button_t *msg_btn = compo_button_create_by_image(frm,                // 通过图片资源创建按钮
                                  UI_BUF_I330001_DRIFT_BOTTLE_BLACK_READ_MESSAGE_BIN);                   // 初始使用已读消息图片(后续会根据状态切换)
        compo_setid(msg_btn, DRIFT_MESSAGE_BUTTON_ID(idx));                        // 设置组件ID:COMPO_ID_MSG_BUTTON_BASE + idx(285-290)
        compo_button_set_pos(msg_btn, GUI_SCREEN_CENTER_X + 12, center_y);         // 设置位置:X=屏幕中心,Y=消息中心
        compo_button_set_visible(msg_btn, false);                                  // 初始状态设置为隐藏(只在消息视图显示)

        // --- 创建消息角色文本(显示"Me"或"1"表示发送者) ---
        compo_textbox_t *role_txt = compo_textbox_create(frm, 8);                  // 创建文本框,最大容纳8个字符
        compo_setid(role_txt, DRIFT_MESSAGE_ROLE_ID(idx));                         // 设置组件ID:COMPO_ID_MSG_ROLE_TEXT_BASE + idx(289-294)
        compo_textbox_set_font(role_txt, UI_BUF_0FONT_FONT_NUM_20_BIN);            // 设置信字体为20号字体资源
        compo_textbox_set_align_center(role_txt, false);                           // 设置文本左对齐(不居中)
        compo_textbox_set_forecolor(role_txt, COLOR_WHITE);                        // 设置文本颜色为白色
        compo_textbox_set(role_txt, "");                                           // 初始文本内容为空字符串
        compo_textbox_set_pos(role_txt, 36,
                              center_y - message_size.hei / 2 + DRIFT_MESSAGE_TEXT_OFFSET_Y);                // Y=消息中心Y - 消息高度/2 + 控制偏移
        compo_textbox_set_visible(role_txt, false);                                // 初始状态设置为隐藏

        // --- 创建消息索引文本(显示消息编号1-6) ---
        compo_textbox_t *index_txt = compo_textbox_create(frm, 8);                 // 创建文本框,最大容纳8个字符
        compo_setid(index_txt, DRIFT_MESSAGE_INDEX_ID(idx));                       // 设置组件ID:COMPO_ID_MSG_INDEX_TEXT_BASE + idx(293-298)
        compo_textbox_set_font(index_txt, UI_BUF_0FONT_FONT_NUM_20_BIN);           // 设置信字体为20号字体资源
        compo_textbox_set_align_center(index_txt, false);                          // 设置文本左对齐(不居中)
        compo_textbox_set_forecolor(index_txt, COLOR_WHITE);                       // 设置文本颜色为白色
        compo_textbox_set(index_txt, "");                                          // 初始文本内容为空字符串
        compo_textbox_set_pos(index_txt, GUI_SCREEN_WIDTH - 36,
                              center_y - message_size.hei / 2 + DRIFT_MESSAGE_TEXT_OFFSET_Y-4);                // Y=消息中心Y - 消息高度/2 + 控制偏移
        compo_textbox_set_visible(index_txt, false);                               // 初始状态设置为隐藏
    }

    drift_bottle_network = false;                                                  // 初始化全局网络状态为未连接
    return frm;                                                                    // 返回创建完成的窗体指针

}

static bool drift_bottle_add_voice_to_bottle(f_drift_bottle_t *f_cb, u8 bottle_index)
{
    if (!f_cb || bottle_index >= DRIFT_BOTTLE_COUNT)
    {
        return false;
    }

    if (f_cb->message_unread_count[bottle_index] < DRIFT_MESSAGE_COUNT)
    {
        f_cb->message_unread_count[bottle_index]++;
    }

    f_cb->bottle_has_message[bottle_index] = true;

    if (bottle_index == f_cb->selected_bottle)
    {
        u8 slot = 0;
        bool found_free = false;

        for (u8 i = 0; i < DRIFT_MESSAGE_COUNT; i++)
        {
            if (!f_cb->messages[i].unread)
            {
                slot = i;
                found_free = true;
                break;
            }
        }

        if (!found_free)
        {
            slot = (u8)((slot + DRIFT_MESSAGE_COUNT - 1) % DRIFT_MESSAGE_COUNT);
        }

        f_cb->messages[slot].from_me = true;
        f_cb->messages[slot].unread = true;
        drift_bottle_update_messages(f_cb);
    }

    drift_bottle_update_bottle_ui(f_cb);
    drift_bottle_sync_warning(f_cb);

    return true;
}

static bool drift_bottle_send_voice_to_empty(f_drift_bottle_t *f_cb, compo_textbox_t *status)
{
    if (!f_cb)
    {
        return false;
    }

    u8 target = DRIFT_BOTTLE_INVALID;

    for (u8 idx = 0; idx < DRIFT_BOTTLE_COUNT; idx++)
    {
        if (!f_cb->bottle_has_message[idx])
        {
            target = idx;
            break;
        }
    }

    if (target == DRIFT_BOTTLE_INVALID)
    {
        if (status)
        {
            compo_textbox_set(status, "No empty bottle");//没有空瓶
        }
        return false;
    }

    if (!drift_bottle_add_voice_to_bottle(f_cb, target))
    {
        return false;
    }

    if (status)
    {
        compo_textbox_set(status, "Voice sent");//语音发送完毕
    }

    printf("voice sent to empty bottle %u\n", (unsigned int)target);
    return true;
}

static void drift_bottle_prepare_voice_decision(f_drift_bottle_t *f_cb, compo_textbox_t *status)
{
    if (!f_cb)
    {
        return;
    }

    f_cb->is_listen = false;
    f_cb->pending_voice_ready = true;
    drift_bottle_set_decision_buttons_visible(true);

    if (status)
    {
        compo_textbox_set(status, f_cb->pending_target_empty ? "Confirm or cancel (empty bottle)" : "Confirm or cancel");
    }

    printf("voice capture finished, awaiting decision\n");
}

static void drift_bottle_abort_voice_capture(f_drift_bottle_t *f_cb, compo_textbox_t *status, const char *message)
{
    if (!f_cb)
    {
        return;
    }

    f_cb->is_listen = false;
    f_cb->pending_voice_ready = false;
    f_cb->pending_target_empty = false;
    f_cb->pending_target_index = DRIFT_BOTTLE_INVALID;
    drift_bottle_set_decision_buttons_visible(false);

    if (status && message)
    {
        compo_textbox_set(status, message);
    }
}



/**
  * @brief  更新漂流瓶UI显示状态
  * @note   根据当前视图状态和瓶子数据更新所有瓶子的显示
  *         包括:瓶子图片(空瓶/满瓶)、标签颜色、按钮可见性
  * @param  f_cb: 指向漂流瓶功能状态结构体的指针
  * @retval None
  */
static void drift_bottle_update_bottle_ui(f_drift_bottle_t *f_cb)
{
    bool show = (f_cb->active_view == DRIFT_VIEW_BOTTLES);                        // 判断当前是否为瓶子视图(true=瓶子视图,false=消息视图)

    for (u8 idx = 0; idx < DRIFT_BOTTLE_COUNT; idx++)                              // 遍历所有8个漂流瓶
    {
        // --- 更新瓶子图片(空瓶或满瓶) ---
        compo_picturebox_t *pic = compo_getobj_byid(DRIFT_BOTTLE_PIC_ID(idx));     // 通过ID获取瓶子图片组件

        if (pic)                                                                   // 如果组件存在(安全检查)
        {
            // 根据未读消息数量选择显示空瓶或满瓶图片
            u32 res = (f_cb->message_unread_count[idx] > 0)                        // 如果该瓶子有未读消息
                      ? UI_BUF_I330001_DRIFT_BOTTLE_BLACK_BOTTLE_FULL_BIN                // 使用满瓶图片资源
                      : UI_BUF_I330001_DRIFT_BOTTLE_BLACK_BOTTLE_EMPTY_BIN;              // 否则使用空瓶图片资源
            compo_picturebox_set(pic, res);                                        // 设置图片资源
            compo_picturebox_set_visible(pic, show);                               // 设置可见性(只在瓶子视图显示)
        }

        // --- 更新瓶子编号标签颜色 ---
        compo_textbox_t *label = compo_getobj_byid(DRIFT_BOTTLE_LABEL_ID(idx));    // 通过ID获取瓶子标签组件
        if (label)                                                                 // 如果组件存在(安全检查)
        {
            // 根据瓶子状态设置标签颜色
            u16 color = (f_cb->message_unread_count[idx] > 0)                      // 如果有未读消息
                        ? COLOR_GREEN                                                      // 使用绿色(表示有新消息)
                        : COLOR_WHITE;                                                     // 否则使用白色(默认颜色)
            if (idx == f_cb->selected_bottle)                                      // 如果是当前选中的瓶子
            {
                color = COLOR_CYAN;                                                // 使用青色高亮显示
            }
            compo_textbox_set_forecolor(label, color);                             // 设置文本颜色
            compo_textbox_set_visible(label, show);                                // 设置可见性(只在瓶子视图显示)
        }

        // --- 更新瓶子按钮可见性 ---
        compo_button_t *btn = compo_getobj_byid(DRIFT_BOTTLE_BUTTON_ID(idx));      // 通过ID获取瓶子按钮组件
        if (btn)                                                                   // 如果组件存在(安全检查)
        {
            compo_button_set_visible(btn, show);                                   // 设置可见性(只在瓶子视图显示)
        }
    }
}

/**
  * @brief  更新消息列表UI显示状态
  * @note   根据消息数据更新所有消息按钮的显示
  *         包括:消息按钮背景(已读/未读)、角色文本、索引文本、可见性
  * @param  f_cb: 指向漂流瓶功能状态结构体的指针
  * @retval None
  */
static void drift_bottle_update_messages(f_drift_bottle_t *f_cb)
{
    if (!f_cb)
    {
        return;
    }

    bool show_view = (f_cb->active_view == DRIFT_VIEW_MESSAGES);
    s16 visible_top = f_cb->message_start_y - f_cb->message_half_height;
    s16 visible_bottom = f_cb->message_view_bottom + f_cb->message_half_height;

    for (u8 idx = 0; idx < DRIFT_MESSAGE_COUNT; idx++)
    {
        s16 center_y = f_cb->message_base_y[idx] - f_cb->message_scroll_offset;
        bool in_view = show_view &&
                       (center_y + f_cb->message_half_height > visible_top) &&
                       (center_y - f_cb->message_half_height < visible_bottom);

        compo_button_t *btn = compo_getobj_byid(DRIFT_MESSAGE_BUTTON_ID(idx));
        if (btn)
        {
            u32 res = f_cb->messages[idx].unread
                      ? UI_BUF_I330001_DRIFT_BOTTLE_BLACK_UN_MESSAGE_BIN
                      : UI_BUF_I330001_DRIFT_BOTTLE_BLACK_READ_MESSAGE_BIN;
            compo_button_set_bgimg(btn, res);
            compo_button_set_pos(btn, GUI_SCREEN_CENTER_X + 12, center_y);
            compo_button_set_visible(btn, in_view);
        }

        compo_textbox_t *role = compo_getobj_byid(DRIFT_MESSAGE_ROLE_ID(idx));
        if (role)
        {
            compo_textbox_set(role, f_cb->messages[idx].from_me ? "Me" : "1");
            compo_textbox_set_pos(role, 36,
                                  center_y - f_cb->message_half_height + DRIFT_MESSAGE_TEXT_OFFSET_Y);
            compo_textbox_set_visible(role, in_view);
        }

        compo_textbox_t *index_txt = compo_getobj_byid(DRIFT_MESSAGE_INDEX_ID(idx));
        if (index_txt)
        {
            char buf[8];
            snprintf(buf, sizeof(buf), "%u", idx + 1);
            compo_textbox_set(index_txt, buf);
            compo_textbox_set_pos(index_txt, GUI_SCREEN_WIDTH - 36,
                                  center_y - f_cb->message_half_height + DRIFT_MESSAGE_TEXT_OFFSET_Y-4);
            compo_textbox_set_visible(index_txt, in_view);
        }
    }
}

/**
  * @brief  同步警告提示显示状态
  * @note   检查所有瓶子的消息状态,决定是否显示警告提示
  *         修改后:只有当所有8个瓶子都有未读消息时才显示警告
  * @param  f_cb: 指向漂流瓶功能状态结构体的指针
  * @retval None
  */
static void drift_bottle_sync_warning(f_drift_bottle_t *f_cb)
{
    compo_textbox_t *warning = compo_getobj_byid(COMPO_ID_WARNING_TEXT);           // 通过ID获取警告文本框组件

    if (!warning)                                                                  // 如果组件不存在(安全检查)
    {
        return;                                                                    // 直接返回,不执行后续操作
    }

    // 只在瓶子视图显示警告(消息视图不显示)
    if (f_cb->active_view != DRIFT_VIEW_BOTTLES)                                   // 如果当前不是瓶子视图
    {
        compo_textbox_set_visible(warning, false);                                 // 隐藏警告文本框
        return;                                                                    // 直接返回
    }

    // 检查是否所有8个瓶子都有未读消息
    bool all_bottles_have_messages = true;                                         // 假设所有瓶子都有消息
    for (u8 idx = 0; idx < DRIFT_BOTTLE_COUNT; idx++)                              // 遍历所有8个瓶子
    {
        if (f_cb->message_unread_count[idx] == 0)                                  // 如果发现任何一个瓶子没有未读消息
        {
            all_bottles_have_messages = false;                                     // 标记为不是所有瓶子都有消息
            break;                                                                 // 跳出循环,无需继续检查
        }
    }

    if (all_bottles_have_messages)                                                 // 如果所有8个瓶子都有未读消息
    {
        compo_textbox_set(warning, "! please clean the bottle");                   // 设置警告文本内容
        compo_textbox_set_forecolor(warning, COLOR_YELLOW);                        // 设置文本颜色为黄色(警告色)
        compo_textbox_set_visible(warning, true);                                  // 显示警告文本框
    }
    else                                                                           // 否则(至少有一个瓶子为空)
    {
        compo_textbox_set(warning, "");                                            // 清空警告文本内容
        compo_textbox_set_visible(warning, false);                                 // 隐藏警告文本框
    }
}

/**
  * @brief  切换漂流瓶界面的显示视图
  * @note   在瓶子列表视图和消息列表视图之间切换
  *         同时控制所有相关UI组件的可见性
  * @param  f_cb: 指向漂流瓶功能状态结构体的指针
  * @param  view: 目标视图类型(DRIFT_VIEW_BOTTLES或DRIFT_VIEW_MESSAGES)
  * @retval None
  */
static void drift_bottle_set_active_view(f_drift_bottle_t *f_cb, drift_bottle_view_t view)
{
    if (!f_cb)
    {
        return;
    }

    f_cb->active_view = view;

    for (u8 idx = 0; idx < DRIFT_BOTTLE_COUNT; idx++)
    {
        compo_button_t *btn = compo_getobj_byid(DRIFT_BOTTLE_BUTTON_ID(idx));
        if (btn)
        {
            compo_button_set_visible(btn, view == DRIFT_VIEW_BOTTLES);
        }

        compo_picturebox_t *pic = compo_getobj_byid(DRIFT_BOTTLE_PIC_ID(idx));
        if (pic)
        {
            compo_picturebox_set_visible(pic, view == DRIFT_VIEW_BOTTLES);
        }

        compo_textbox_t *label = compo_getobj_byid(DRIFT_BOTTLE_LABEL_ID(idx));
        if (label)
        {
            compo_textbox_set_visible(label, view == DRIFT_VIEW_BOTTLES);
        }
    }

    for (u8 idx = 0; idx < DRIFT_MESSAGE_COUNT; idx++)
    {
        compo_button_t *btn = compo_getobj_byid(DRIFT_MESSAGE_BUTTON_ID(idx));
        if (btn)
        {
            compo_button_set_visible(btn, view == DRIFT_VIEW_MESSAGES);
        }

        compo_textbox_t *role = compo_getobj_byid(DRIFT_MESSAGE_ROLE_ID(idx));
        if (role)
        {
            compo_textbox_set_visible(role, view == DRIFT_VIEW_MESSAGES);
        }

        compo_textbox_t *index_txt = compo_getobj_byid(DRIFT_MESSAGE_INDEX_ID(idx));
        if (index_txt)
        {
            compo_textbox_set_visible(index_txt, view == DRIFT_VIEW_MESSAGES);
        }
    }

    compo_button_t *back_btn = compo_getobj_byid(COMPO_ID_BTN_BACK);
    if (back_btn)
    {
        compo_button_set_visible(back_btn, view == DRIFT_VIEW_MESSAGES);
    }

    drift_bottle_update_bottle_ui(f_cb);
    drift_bottle_update_messages(f_cb);
    drift_bottle_sync_warning(f_cb);
    drift_bottle_set_decision_buttons_visible(false);
}

/**
  * @brief  初始化漂流瓶演示数据
  * @note   设置初始状态,包括未读消息数量、瓶子状态和选中的瓶子
  *         这是演示数据,实际应用中应从服务器获取
  * @param  f_cb: 指向漂流瓶功能状态结构体的指针
  * @retval None
  */
static void drift_bottle_init_demo_data(f_drift_bottle_t *f_cb)
{
    // ========== 清空所有数据数组 ==========
    memset(f_cb->message_unread_count, 0, sizeof(f_cb->message_unread_count));    // 将所有瓶子的未读消息数量清零
    memset(f_cb->bottle_has_message, 0, sizeof(f_cb->bottle_has_message));        // 将所有瓶子的消息标志清零
    f_cb->pending_target_empty = false;
    f_cb->pending_target_index = DRIFT_BOTTLE_INVALID;

    // ========== 初始化消息数据 ==========
    for (u8 idx = 0; idx < DRIFT_MESSAGE_COUNT; idx++)                             // 遍历所有4条消息槽位
    {
        f_cb->messages[idx].from_me = (idx % 2 == 0);                              // 偶数索引(0,2)标记为"我"发送的,奇数索引(1,3)标记为对方发送的
        f_cb->messages[idx].unread = false;                                        // 初始状态所有消息标记为已读
    }

    // ========== 设置默认视图和选中瓶子 ==========
    f_cb->selected_bottle = 0;                                                     // 默认选中第一个瓶子(索引0)
    f_cb->active_view = DRIFT_VIEW_BOTTLES;                                        // 默认显示瓶子列表视图

    // ========== 设置演示数据:瓶子0有2条未读消息 ==========
    if (DRIFT_BOTTLE_COUNT > 0)                                                    // 如果至少有1个瓶子(DRIFT_BOTTLE_COUNT=8)
    {
        f_cb->message_unread_count[0] = 2;                                         // 设置瓶子0有2条未读消息
        f_cb->bottle_has_message[0] = true;                                        // 标记瓶子0有消息
    }

    // ========== 设置演示数据:瓶子1有1条未读消息 ==========
    if (DRIFT_BOTTLE_COUNT > 1)                                                    // 如果至少有2个瓶子
    {
        f_cb->message_unread_count[1] = 1;                                         // 设置瓶子1有1条未读消息
        f_cb->bottle_has_message[1] = true;                                        // 标记瓶子1有消息
    }

    // ========== 设置演示数据:瓶子6有1条未读消息并选中 ==========
    if (DRIFT_BOTTLE_COUNT > 6)                                                    // 如果至少有7个瓶子(索引0-6)
    {
        f_cb->message_unread_count[6] = 1;                                         // 设置瓶子6有1条未读消息
        f_cb->bottle_has_message[6] = true;                                        // 标记瓶子6有消息
        f_cb->selected_bottle = 6;                                                 // 将选中的瓶子改为瓶子6
    }

    // ========== 根据选中瓶子的未读数量初始化消息状态 ==========
    u8 unread = f_cb->message_unread_count[f_cb->selected_bottle];                 // 获取当前选中瓶子的未读消息数量
    for (u8 idx = 0; idx < DRIFT_MESSAGE_COUNT; idx++)                             // 遍历所有4条消息槽位
    {
        f_cb->messages[idx].unread = (idx < unread);                               // 前N条消息标记为未读(N=未读数量),其余标记为已读
        // 例如:如果unread=2,则消息0和1标记为未读,消息2和3标记为已读
    }
}

/**
  * @brief  处理瓶子选择事件
  * @note   当用户点击某个瓶子时调用此函数
  *         更新选中状态,初始化消息数据,并切换到消息视图
  * @param  f_cb: 指向漂流瓶功能状态结构体的指针
  * @param  index: 被选中的瓶子索引(0-7)
  * @retval None
  */
static void drift_bottle_handle_bottle_select(f_drift_bottle_t *f_cb, u8 index)
{
    // ========== 参数有效性检查 ==========
    if (index >= DRIFT_BOTTLE_COUNT)                                               // 检查索引是否超出范围(必须在0-7之间)
    {
        return;                                                                    // 索引无效,直接返回,不执行后续操作
    }

    // ========== 更新选中的瓶子索引 ==========
    f_cb->selected_bottle = index;                                                 // 将当前选中的瓶子索引更新为用户点击的瓶子

    // ========== 根据选中瓶子的未读数量初始化消息状态 ==========
    u8 unread = f_cb->message_unread_count[index];                                 // 获取该瓶子的未读消息数量
    for (u8 i = 0; i < DRIFT_MESSAGE_COUNT; i++)                                   // 遍历所有4条消息槽位
    {
        f_cb->messages[i].from_me = (i % 2 == 0);                                  // 偶数索引的消息标记为"我"发送的,奇数索引标记为对方发送的
        f_cb->messages[i].unread = (i < unread);                                   // 前N条消息标记为未读(N=未读数量),其余标记为已读
    }

    // ========== 切换到消息视图并更新UI ==========
    drift_bottle_set_active_view(f_cb, DRIFT_VIEW_MESSAGES);                       // 切换界面显示为消息列表视图
    drift_bottle_update_bottle_ui(f_cb);                                           // 更新瓶子UI状态(高亮选中的瓶子)
    drift_bottle_update_messages(f_cb);                                            // 更新消息UI状态(显示已读/未读)
    drift_bottle_sync_warning(f_cb);                                               // 同步警告提示显示
}

/**
  * @brief  标记选中瓶子的所有消息为已读
  * @note   清空瓶子中的未读消息计数,并更新UI显示
  *         通常在用户点击"确认/清空"按钮时调用
  * @param  f_cb: 指向漂流瓶功能状态结构体的指针
  * @param  index: 瓶子索引(0-7)
  * @retval None
  */
/**
  * @brief  Chatbot事件回调函数
  * @note   处理来自chatbot库的各种事件,更新UI状态和内部标志
  *         包括连接状态、监听状态、播放状态、错误处理等
  * @param  event: chatbot事件类型枚举值
  * @retval None
  */
static void event_cb(chatbot_event_t event)
{
    // ========== 获取功能状态结构体 ==========
    f_drift_bottle_t *f_cb = func_cb.f_cb;                                         // 获取漂流瓶功能状态结构体指针
    if (!f_cb)                                                                     // 安全检查:如果结构体未初始化
    {
        return;                                                                    // 直接返回,不执行后续操作(防止空指针访问)
    }

    // ========== 调试输出 ==========
    printf("drift_bottle event: %d", event);                                       // 调试输出:打印事件类型编号,用于开发调试
    compo_textbox_t *status = compo_getobj_byid(COMPO_ID_TEXT_STATUS);            // 通过ID获取状态文本框组件指针

    // ========== 根据事件类型进行分支处理 ==========
    switch (event)
    {
        // --- 事件:成功连接到chatbot服务器 ---
        case CHATEVT_IS_CONN:
        {
            drift_bottle_network = true;                                           // 更新全局网络状态标志为已连接
            if (status)                                                            // 如果状态文本框存在(安全检查)
            {
                compo_textbox_set(status, "Connected");                            // 更新状态文本为"已连接"
            }
//            f_cb->is_listen = true;                                                // 设置内部监听状态标志为true(准备接收语音)
            drift_bottle_set_send_button_pressed(false);
            f_cb->is_conn = true;                                                  // 设置内部连接状态标志为true
            drift_bottle_update_bottle_ui(f_cb);                                   // 更新瓶子UI显示(可能需要刷新网络相关状态)
            break;                                                                 // 跳出switch语句
        }

        // --- 事件:chatbot正常退出 ---
        case CHATEVT_EXIT:
            if (!f_cb->exiting)                                                    // 防止重复退出(如果未在退出状态)
            {
                f_cb->exiting = true;                                              // 标记正在退出标志为true
                f_cb->back_to = true;                                              // 标记需要返回上一界面标志为true
            }
            break;                                                                 // 跳出switch语句

        // --- 事件:AI正在播放语音回复(Text-To-Speech) ---
        case CHATEVT_TTS_PLAYING:
            drift_bottle_network = true;                                           // 确保网络状态标志为已连接
            drift_bottle_set_send_button_pressed(false);
            f_cb->is_listen = false;                                               // 停止监听状态(AI说话时不监听用户输入)
            if (status)                                                            // 如果状态文本框存在(安全检查)
            {
                compo_textbox_set(status, "Playing message...");                   // 更新状态文本为"播放消息中..."
            }
            break;                                                                 // 跳出switch语句

        // --- 事件:正在监听语音输入 ---
        case CHATEVT_LISTENING:
            drift_bottle_network = true;                                           // 确保网络状态标志为已连接
            f_cb->is_listen = true;                                                // 设置内部监听状态标志为true
            drift_bottle_set_send_button_pressed(true);
            if (status)                                                            // 如果状态文本框存在(安全检查)
            {
                compo_textbox_set(status, "Listening...");                         // 更新状态文本为"监听中..."
            }
            break;                                                                 // 跳出switch语句

        // --- 事件:网络错误,需要重新连接 ---
        case CHATEVT_ERROR_RECONN:
            drift_bottle_network = false;                                          // 更新全局网络状态标志为未连接
            if (status)                                                            //
            {
                compo_textbox_set(status, "Network error, retrying...");
            }
            f_cb->need_reconn = true;
            f_cb->is_conn = false;
            break;

        case CHATEVT_THINKING:
            if (status)
            {
                compo_textbox_set(status, "Thinking...");
            }
            drift_bottle_set_send_button_pressed(false);
            f_cb->is_listen = false;
            break;

        case CHATEVT_ERROR_BROKEN:
            if (!f_cb->exiting)
            {
                drift_bottle_network = false;
                f_cb->need_reconn = true;
            }
            break;

        default:
            break;
    }

    drift_bottle_sync_warning(f_cb);
}


/**
  * @brief  MCP(设备控制协议)回调函数
  * @note   处理来自chatbot的设备控制命令
  *         目前支持电子烟功率增加和降低命令
  * @param  mcp: MCP命令类型枚举值
  * @param  param: 命令参数指针(当前未使用)
  * @retval None
  */
static void mcp_cb(chatbot_mcp_t mcp, void *param)
{
    // 根据MCP命令类型进行分支处理
    switch (mcp)
    {
        case CHATMCP_POWERUP:                                                      // 命令:增加电子烟功率
            printf("CHATMCP_POWERUP\n");                                           // 调试输出:打印功率增加命令
            msg_enqueue(EVT_VAPE_POWERUP);                                         // 将功率增加事件加入系统消息队列
            break;                                                                 // 跳出switch语句

        case CHATMCP_POWERDOWN:                                                    // 命令:降低电子烟功率
            printf("CHATMCP_POWERDOWN\n");                                         // 调试输出:打印功率降低命令
            msg_enqueue(EVT_VAPE_POWERDOWN);                                       // 将功率降低事件加入系统消息队列
            break;                                                                 // 跳出switch语句

        default:                                                                   // 默认分支:未处理的命令类型
            break;                                                                 // 跳出switch语句(不做任何处理)
    }
}

/**
  * @brief  启动chatbot服务
  * @note   发送抽吸数据到服务器,注册回调函数,启动chatbot
  *         此函数在初始化成功后被调用
  * @param  None
  * @retval None
  */
static void drift_bottle_start_do(void)
{
    // 发送抽吸数据到chatbot服务器
    chatbot_send_vape_record(
        get_today_smoking_count(),                                                 // 参数1:今日抽吸次数
        get_this_week_smoking_count()                                              // 参数2:本周抽吸次数
    );

    // 注册回调函数
    chatbot_set_event_callback(event_cb);                                          // 设置chatbot事件回调函数为event_cb
    chatbot_set_mcp_callback(mcp_cb);                                              // 设置设备控制协议回调函数为mcp_cb

    // 启动chatbot服务
    chatbot_start();                                                               // 调用chatbot库的启动函数,开始语音交互服务
}


/**
  * @brief  漂流瓶功能主处理循环函数
  * @note   此函数在主循环中被周期性调用,负责:
  *         - 网络状态监控和自动重连
  *         - UI状态更新
  *         - 休眠延迟管理
  *         - 返回上一界面的处理
  * @param  None
  * @retval None
  */
static void func_drift_bottle_process(void)
{
    f_drift_bottle_t *f_cb = func_cb.f_cb;                                         // 获取漂流瓶功能状态结构体指针
    chatbot_info_t *info = &f_cb->info;                                            // 获取chatbot配置信息结构体指针

    // ========== 网络状态计数器管理 ==========
    if (!drift_bottle_network && ++drift_bottle_num > 500)                         // 如果网络未连接且计数器超过500
    {
        drift_bottle_num = 0;                                                      // 重置计数器为0(防止溢出)
    }

    // ========== 检测网络状态变化 ==========
    bool current_net_state = bnep_network_is_ok();                                 // 获取当前网络连接状态(true=已连接,false=未连接)

    // ========== 网络从断开变为连接时的处理 ==========
    if (current_net_state && !f_cb->last_net_state)                                // 如果当前网络已连接且上次网络未连接(检测到网络恢复)
    {
        if (!f_cb->is_conn && chatbot_init(uteModuleSmokeData.is_out_chaina, info)) // 如果chatbot未连接且初始化成功
        {
            f_cb->is_conn = true;                                                  // 更新内部连接状态标志为true
            drift_bottle_start_do();                                               // 启动chatbot服务(注册回调、发送数据)
        }
    }

    // ========== 网络未连接时的提示 ==========
    if (!current_net_state)                                                        // 如果当前网络未连接
    {
        compo_textbox_t *status = compo_getobj_byid(COMPO_ID_TEXT_STATUS);        // 通过ID获取状态文本框组件
        if (status)                                                                // 如果组件存在(安全检查)
        {
            compo_textbox_set(status, "Waiting for network...");                  // 更新状态文本为"等待网络连接..."
        }
    }

    // ========== 更新上次网络状态 ==========
    f_cb->last_net_state = current_net_state;                                      // 保存当前网络状态,用于下次循环检测状态变化

    // ========== 处理重连请求 ==========
    if (f_cb->need_reconn)                                                         // 如果需要重新连接标志为true
    {
        f_cb->need_reconn = false;                                                 // 清除重连请求标志
        f_cb->is_conn = false;                                                     // 设置内部连接状态为未连接
        f_cb->wait_disconn = true;                                                 // 设置等待断开标志为true
        f_cb->wait_disconn_tick = tick_get();                                      // 记录开始等待的系统tick值
        chatbot_deinit();                                                          // 反初始化chatbot(断开连接、释放资源)
        compo_textbox_t *status = compo_getobj_byid(COMPO_ID_TEXT_STATUS);        // 通过ID获取状态文本框组件
        if (status)                                                                // 如果组件存在(安全检查)
        {
            compo_textbox_set(status, "Reconnecting...");                         // 更新状态文本为"重新连接中..."
        }
    }

    // ========== 等待断开完成后重新初始化 ==========
    if (f_cb->wait_disconn)                                                        // 如果正在等待断开完成
    {
        if (chatbot_init(uteModuleSmokeData.is_out_chaina, info))                 // 尝试重新初始化chatbot
        {
            f_cb->wait_disconn = false;                                            // 清除等待断开标志(初始化成功)
            drift_bottle_start_do();                                               // 启动chatbot服务
        }
        if (tick_check_expire(f_cb->wait_disconn_tick, 4000))                      // 如果等待时间超过4000ms(4秒)
        {
            f_cb->wait_disconn = false;                                            // 清除等待断开标志(超时放弃)
            func_back_to();                                                        // 返回上一界面
            return;                                                                // 直接返回,不执行后续代码
        }
    }

    // ========== 更新UI显示 ==========
    drift_bottle_update_bottle_ui(f_cb);                                           // 更新所有漂流瓶的UI显示(空瓶/满瓶图片、标签颜色)
    drift_bottle_sync_warning(f_cb);                                               // 同步警告提示文本显示

    // ========== 重置休眠延迟 ==========
    reset_sleep_delay_all();                                                       // 重置所有休眠延迟计时器(保持屏幕常亮)

    // ========== 处理返回上一界面请求 ==========
    if (f_cb->back_to)                                                             // 如果需要返回上一界面标志为true
    {
        f_cb->back_to = false;                                                     // 清除返回标志
        func_back_to();                                                            // 执行返回操作(弹出任务栈)
        return;                                                                    // 直接返回,不执行后续代码
    }

    // ========== 通用功能处理 ==========
    func_process();                                                                // 调用通用功能处理函数(刷新UI、处理系统消息等)
}


#include "chatbot.h"
/**
  * @brief  设置发送按钮的按下/释放状态
  * @note   此函数用于控制发送语音消息按钮的UI状态变化
  *         按下时:切换到"发送中"图标,隐藏取消和确认按钮
  *         释放时:恢复到"发送语音"图标,显示取消和确认按钮
  * @param  pressed: 按钮状态标志
  *                  - true:  按钮被按下
  *                  - false: 按钮被释放
  * @retval None
  */
static void drift_bottle_set_decision_buttons_visible(bool visible)
{
    compo_button_t *btn_cancel = compo_getobj_byid(COMPO_ID_BTN_CANCEL);
    compo_button_t *btn_confirm = compo_getobj_byid(COMPO_ID_BTN_CONFIRM);

    if (btn_cancel)
    {
        compo_button_set_visible(btn_cancel, visible);
    }
    if (btn_confirm)
    {
        compo_button_set_visible(btn_confirm, visible);
    }
}

static void drift_bottle_set_send_button_pressed(bool pressed)
{
    compo_button_t *btn_send = compo_getobj_byid(COMPO_ID_BTN_SEND);

    if (!btn_send)
    {
        return;
    }

    f_drift_bottle_t *f_cb = func_cb.f_cb;

    if (pressed)
    {
        compo_button_set_bgimg(btn_send, UI_BUF_I330001_DRIFT_BOTTLE_BLACK_SEND_IN_PROGRESS_MESSAGE_BIN);
        drift_bottle_set_decision_buttons_visible(false);
    }
    else
    {
        compo_button_set_bgimg(btn_send, UI_BUF_I330001_DRIFT_BOTTLE_BLACK_SEND_VOICE_MESSAGE_BIN);
        bool show_decision = (f_cb && f_cb->pending_voice_ready);
        drift_bottle_set_decision_buttons_visible(show_decision);
    }
}

/**
  * @brief  漂流瓶功能按钮点击处理函数
  * @note   处理所有UI按钮的点击事件,包括:
  *         - 8个漂流瓶按钮的点击(切换到消息视图)
  *         - 4条消息按钮的点击(切换已读/未读状态)
  *         - 底部功能按钮(取消、发送、确认、返回)
  * @param  None
  * @retval None
  */
static void func_drift_bottle_button_click(void)
{
    f_drift_bottle_t *f_cb = func_cb.f_cb;                                         // 获取漂流瓶功能状态结构体指针
    if (!f_cb)                                                                     // 安全检查:如果结构体未初始化
    {
        return;                                                                    // 直接返回,不执行后续操作
    }

    int id = compo_get_button_id();                                                // 获取被点击按钮的ID
    compo_textbox_t *status = compo_getobj_byid(COMPO_ID_TEXT_STATUS);            // 通过ID获取状态文本框组件
    printf("Button ID: %d\n", id);

    // ========== 处理漂流瓶按钮点击 ==========
    if (id >= COMPO_ID_BOTTLE_BUTTON_BASE && id <= COMPO_ID_BOTTLE_BUTTON_END)    // 如果点击的是8个漂流瓶按钮之一(ID范围:269-276)
    {
        u8 index = (u8)(id - COMPO_ID_BOTTLE_BUTTON_BASE);                         // 计算瓶子索引(0-7)
        printf("click bottle %d\n", index);
        drift_bottle_handle_bottle_select(f_cb, index);                            // 调用瓶子选择处理函数(切换到消息视图)
        return;                                                                    // 直接返回,不执行后续代码
    }

    printf("ID not in bottle button range (269-276)\n");

    // ========== 处理消息按钮点击 ==========
    if (id >= COMPO_ID_MSG_BUTTON_BASE && id <= COMPO_ID_MSG_BUTTON_END)          // 如果点击的是4条消息按钮之一(ID范围:285-288)
    {
        u8 index = (u8)(id - COMPO_ID_MSG_BUTTON_BASE);                            // 计算消息索引(0-3)
        bool was_unread = f_cb->messages[index].unread;                            // 记录原始未读状态
        f_cb->messages[index].unread = !f_cb->messages[index].unread;              // 切换消息的已读/未读状态

        // --- 更新瓶子的未读计数 ---
        if (f_cb->messages[index].unread && !was_unread)                           // 如果从已读变为未读
        {
            if (f_cb->message_unread_count[f_cb->selected_bottle] < 255)           // 如果未读计数未达到上限(255)
            {
                f_cb->message_unread_count[f_cb->selected_bottle]++;               // 增加选中瓶子的未读消息计数
            }
        }
        else if (!f_cb->messages[index].unread && was_unread)                      // 如果从未读变为已读
        {
            if (f_cb->message_unread_count[f_cb->selected_bottle] > 0)             // 如果未读计数大于0
            {
                f_cb->message_unread_count[f_cb->selected_bottle]--;               // 减少选中瓶子的未读消息计数
            }
        }

        // --- 更新UI显示 ---
        drift_bottle_update_messages(f_cb);                                        // 更新消息列表UI(已读/未读背景)
        drift_bottle_update_bottle_ui(f_cb);                                       // 更新瓶子UI(空瓶/满瓶图片)
        drift_bottle_sync_warning(f_cb);                                           // 同步警告提示文本
        return;                                                                    // 直接返回,不执行后续代码
    }

    // ========== 处理功能按钮点击 ==========
    switch (id)                                                                    // 根据按钮ID进行分支处理
    {
        // --- 取消/返回按钮 ---
        case COMPO_ID_BTN_CANCEL:
        {
            if (f_cb->pending_voice_ready || f_cb->is_listen)
            {
                drift_bottle_abort_voice_capture(f_cb, status, "Voice canceled");
                printf("voice input canceled\n");
            }
            break;
        }

        case COMPO_ID_BTN_SEND:
        {
            if (f_cb->pending_voice_ready)
            {
                break;
            }

            if (f_cb->is_listen)
            {
                drift_bottle_abort_voice_capture(f_cb, status, "Press and hold to talk");
                printf("voice input canceled (short press)\n");
            }
            else if (!f_cb->is_conn)
            {
                drift_bottle_abort_voice_capture(f_cb, status, "No connection");
                printf("voice input blocked: no connection\n");
            }
            else if (status)
            {
                compo_textbox_set(status, "Press and hold to talk");
            }
            break;
        }

        case COMPO_ID_BTN_CONFIRM:
        {
            if (!f_cb->pending_voice_ready)
            {
                printf("confirm pressed without pending voice\n");
                drift_bottle_set_decision_buttons_visible(false);
                break;
            }

            bool sent = false;

            // ✅ 新逻辑: 使用 pending_target_index
            if (f_cb->pending_target_index != DRIFT_BOTTLE_INVALID)
            {
                if (drift_bottle_add_voice_to_bottle(f_cb, f_cb->pending_target_index))
                {
                    if (status)
                    {
                        compo_textbox_set(status, "Voice sent");
                    }
                    sent = true;
                    printf("voice sent to bottle %u\n", (unsigned int)f_cb->pending_target_index);
                }
            }
            else if (!f_cb->pending_target_empty && f_cb->selected_bottle < DRIFT_BOTTLE_COUNT)
            {
                // 短按情况: 发送到当前选中瓶子
                if (drift_bottle_add_voice_to_bottle(f_cb, f_cb->selected_bottle))
                {
                    if (status)
                    {
                        compo_textbox_set(status, "Voice sent");
                    }
                    sent = true;
                }
            }
            else
            {
                printf("confirm pressed but no valid target\n");
            }

            if (sent)
            {
                drift_bottle_abort_voice_capture(f_cb, status, NULL);
            }
            break;
        }

        case COMPO_ID_BTN_BACK:
            drift_bottle_set_active_view(f_cb, DRIFT_VIEW_BOTTLES);                // 切换视图为瓶子列表视图
            break;

        // --- 默认分支:未处理的按钮ID ---
        default:
            break;
    }
}




/**
  * @brief  漂流瓶功能消息处理函数
  * @note   此函数在主循环中被调用,处理来自系统消息队列的各种消息
  *         包括触摸屏事件、按钮点击、滑动手势等用户交互消息
  *         以及系统定时器、蓝牙事件等系统消息
  * @param  msg: 系统消息枚举值(size_msg_t类型)
  *              常见消息类型包括:
  *              - MSG_CTP_CLICK: 触摸屏单击事件
  *              - MSG_CTP_SHORT_UP/DOWN/LEFT/RIGHT: 滑动手势
  *              - MSG_CTP_LONG: 长按事件
  *              - MSG_SYS_1S: 系统1秒定时器
  *              - 其他系统消息
  * @retval None
  */
static void func_drift_bottle_message(size_msg_t msg)
{
    switch (msg)
    {
        // ========== 处理触摸按下事件 ==========
        case MSG_CTP_TOUCH:
        {
            int id = compo_get_button_id();
            if (id == COMPO_ID_BTN_SEND)
            {
                drift_bottle_set_send_button_pressed(true);  // 按下时更新UI
                f_drift_bottle_t *f_cb = func_cb.f_cb;
                if (f_cb)
                {
                    compo_textbox_t *status = compo_getobj_byid(COMPO_ID_TEXT_STATUS);
                    if (!f_cb->is_conn)
                    {
                        drift_bottle_abort_voice_capture(f_cb, status, "No connection");
                        drift_bottle_set_send_button_pressed(false);
                        printf("voice input blocked: no connection\n");
                    }
                    else
                    {
                        f_cb->is_listen = true;
                        f_cb->pending_voice_ready = false;
                        f_cb->pending_target_empty = false;
                        drift_bottle_set_decision_buttons_visible(false);
                        if (status)
                        {
                            compo_textbox_set(status, "Listening...");
                        }
                        printf("voice capture started\n");
                    }
                }
            }
            func_message(msg);
            break;
        }

        // ========== 处理触摸屏单击事件 ==========
        case MSG_CTP_CLICK:
        {
            int id = compo_get_button_id();
            func_drift_bottle_button_click();
            if (id == COMPO_ID_BTN_SEND)
            {
                drift_bottle_set_send_button_pressed(false);  // 点击发送按钮后恢复UI
            }
            break;
        }

        // ========== 处理触摸释放事件(滑动、长按等) ==========
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        {
            func_message(msg);
            f_drift_bottle_t *f_cb = func_cb.f_cb;
            if (f_cb && f_cb->active_view == DRIFT_VIEW_MESSAGES)
            {
                s16 step = (msg == MSG_CTP_SHORT_UP) ? f_cb->message_spacing : (s16)(-f_cb->message_spacing);
                if (drift_bottle_scroll_by(f_cb, step))
                {
                    drift_bottle_update_messages(f_cb);
                }
            }
            break;
        }

        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
            func_message(msg);
            break;

        case MSG_CTP_LONG:
        {
            int id = compo_get_button_id();
            if (id == COMPO_ID_BTN_SEND)
            {
                f_drift_bottle_t *f_cb = func_cb.f_cb;
                if (f_cb && f_cb->is_listen)
                {
                    compo_textbox_t *status = compo_getobj_byid(COMPO_ID_TEXT_STATUS);

                    // 先选定目标瓶子
                    u8 target = DRIFT_BOTTLE_INVALID;

                    // 1. 如果当前选中瓶子没有消息,就用它
                    if (f_cb->selected_bottle < DRIFT_BOTTLE_COUNT &&
                        f_cb->message_unread_count[f_cb->selected_bottle] == 0)
                    {
                        target = f_cb->selected_bottle;
                    }
                    else
                    {
                        // 2. 否则找第一个空瓶
                        for (u8 idx = 0; idx < DRIFT_BOTTLE_COUNT; idx++)
                        {
                            if (f_cb->message_unread_count[idx] == 0)
                            {
                                target = idx;
                                break;
                            }
                        }
                    }

                    if (target != DRIFT_BOTTLE_INVALID)
                    {
                        f_cb->pending_target_index = target;
                        f_cb->pending_target_empty = true;
                        if (status)
                        {
                            compo_textbox_set(status, "Release to send to empty bottle");
                        }
                        printf("voice capture marked for bottle %u\n", (unsigned int)target);
                    }
                    else
                    {
                        // 3. 没有空瓶,直接中止
                        drift_bottle_abort_voice_capture(f_cb, status, "No empty bottle");
                        drift_bottle_set_send_button_pressed(false);
                        printf("no empty bottle available\n");
                    }
                }
            }
            func_message(msg);
            break;
        }

        case MSG_CTP_LONG_LIFT:
        case MSG_CTP_SLIDE_LIFT:
        {
            int id = compo_get_button_id();
            if (id == COMPO_ID_BTN_SEND)
            {
                f_drift_bottle_t *f_cb = func_cb.f_cb;
                if (f_cb && f_cb->is_listen)
                {
                    compo_textbox_t *status = compo_getobj_byid(COMPO_ID_TEXT_STATUS);

                    // 只有在有合适目标时才进入决策状态
                    if (f_cb->pending_target_index != DRIFT_BOTTLE_INVALID)
                    {
                        drift_bottle_prepare_voice_decision(f_cb, status);
                    }
                    else
                    {
                        // 没有合适目标,直接 reset
                        drift_bottle_abort_voice_capture(f_cb, status, "No empty bottle");
                        printf("no valid target on lift\n");
                    }
                }
                drift_bottle_set_send_button_pressed(false);
            }
            func_message(msg);
            break;
        }

        // ========== 处理其他所有消息 ==========
        default:
            func_message(msg);
            break;
    }
}

/**
  * @brief  进入漂流瓶功能
  * @note   此函数在进入漂流瓶界面时被调用,负责:
  *         - 分配功能状态结构体内存
  *         - 创建UI窗体
  *         - 初始化演示数据
  *         - 检查蓝牙和网络连接状态
  *         - 初始化chatbot服务
  * @param  None
  * @retval None
  */
static void func_drift_bottle_enter(void)
{
    // ========== 分配内存和创建窗体 ==========
    func_cb.f_cb = func_zalloc(sizeof(f_drift_bottle_t));                          // 分配漂流瓶功能状态结构体内存并清零
    func_cb.frm_main = func_drift_bottle_form_create();                            // 调用窗体创建函数,创建所有UI组件

    f_drift_bottle_t *f_cb = func_cb.f_cb;                                         // 获取功能状态结构体指针(简化后续代码)

    // ========== 初始化状态变量 ==========
    f_cb->last_net_state = bnep_network_is_ok();                                   // 记录当前网络连接状态,用于后续检测网络状态变化
    f_cb->pending_target_index = DRIFT_BOTTLE_INVALID;
    // ========== 初始化演示数据 ==========
    drift_bottle_init_demo_data(f_cb);                                             // 调用函数初始化演示数据(设置瓶子0、1、6有未读消息)

    // ========== 初始化音频和内存监控 ==========
    sco_dump_init();                                                               // 初始化SCO音频转储功能(用于调试)
    mem_monitor_run();                                                             // 运行内存监控(检查内存使用情况)

    // ========== 获取状态文本框组件 ==========
    compo_textbox_t *status = compo_getobj_byid(COMPO_ID_TEXT_STATUS);             // 通过ID获取状态文本框组件指针

    // ========== 检查蓝牙连接状态 ==========
    if (!bt_is_connected())                                                        // 如果蓝牙未连接
    {
        if (status)                                                                // 如果状态文本框存在(安全检查)
        {
            compo_textbox_set(status, "Please connect the phone");                 // 显示"请连接手机"提示
        }
        return;                                                                    // 直接返回,不执行后续的chatbot初始化
    }

    // ========== 配置chatbot信息 ==========
    chatbot_info_t *info = &f_cb->info;                                            // 获取chatbot配置信息结构体指针
    info->api_key = "ny8Z4xIOzWQ0D5C3xO4SfT50ravHk7th";                            // 设置API密钥(用于服务器认证)
    info->client_id = "Voopoo123";                                                 // 设置客户端ID
    info->device_id = "web_test_device";                                           // 设置设备ID
    bt_get_local_bd_addr((u8 *)&info->mac);                                        // 获取本地蓝牙MAC地址并保存到info结构体

    // ========== 初始化chatbot服务 ==========
    if (chatbot_init(uteModuleSmokeData.is_out_chaina, info))                      // 调用chatbot初始化函数(参数:是否国外,配置信息)
    {
        drift_bottle_start_do();                                                   // 如果初始化成功,启动chatbot服务(注册回调、发送数据)
        if (status)                                                                // 如果状态文本框存在(安全检查)
        {
            compo_textbox_set(status, "Connecting...");                            // 显示"连接中..."提示
        }
    }
    else                                                                           // 如果初始化失败(网络未连接)
    {
        if (status)                                                                // 如果状态文本框存在(安全检查)
        {
            compo_textbox_set_location(status, GUI_SCREEN_CENTER_X, DRIFT_TEXT_Y, 220, 15); // 设置文本框位置和尺寸(支持多行显示)
            compo_textbox_set_multiline(status, true);                             // 启用多行模式
            compo_textbox_set_align_center(status, true);                          // 设置文本居中对齐
            compo_textbox_set(status, "Please make sure your phone is on WiFi or 5G"); // 显示网络错误提示
        }
    }
}

/**
  * @brief  退出漂流瓶功能
  * @note   此函数在退出漂流瓶界面时被调用,负责:
  *         - 恢复保存的音量设置
  *         - 标记退出状态
  *         - 断开chatbot连接
  *         - 销毁UI窗体
  * @param  None
  * @retval None
  */
static void func_drift_bottle_exit(void)
{
    // ========== 恢复音量设置 ==========
    if (saved_volume > 0)                                                          // 如果有保存的音量值(大于0)
    {
        bsp_set_volume(saved_volume);                                              // 恢复到进入前的音量
        printf("drift_bottle exit - restored volume: %d\n", saved_volume);         // 调试输出:打印恢复的音量值
        saved_volume = 0;                                                          // 清除保存的音量值
    }

    f_drift_bottle_t *f_cb = func_cb.f_cb;                                         // 获取功能状态结构体指针

    // ========== 创建临时窗体(用于销毁) ==========
    compo_form_t *frm = func_create_form(func_cb.sta);                             // 创建当前功能状态对应的窗体

    // ========== 标记退出状态 ==========
    f_cb->exiting = true;                                                          // 设置退出标志为true
    f_cb->is_conn = false;                                                         // 设置连接状态为false

    // ========== 断开chatbot连接 ==========
    chatbot_deinit();                                                              // 调用chatbot反初始化函数,断开连接并释放资源

    // ========== 销毁窗体 ==========
    compo_form_destroy(frm);                                                       // 销毁临时创建的窗体,释放UI资源
}


// 漂流瓶功能
/**
  * @brief  漂流瓶功能主函数
  * @note   此函数是漂流瓶功能的入口点,负责:
  *         - 打开蓝牙网络连接
  *         - 调用入口函数初始化
  *         - 运行主循环处理事件和消息
  *         - 处理电子烟功率调节
  *         - 调用出口函数清理资源
  * @param  None
  * @retval None
  */
void func_drift_bottle(void)
{
    // ========== 调试输出 ==========
    printf("%s\n", __func__);                                                      // 打印函数名(用于调试跟踪)

    // ========== 打开蓝牙网络连接 ==========
    bt_panu_network_connect();                                                     // 调用函数打开蓝牙PAN(Personal Area Network)网络连接

    // ========== 调用入口函数 ==========
    func_drift_bottle_enter();                                                     // 调用入口函数,初始化UI和chatbot服务

    // ========== 主循环 ==========
    while (func_cb.sta == FUNC_DRIFT_BOTTLE)                                       // 当功能状态为漂流瓶时持续循环
    {
        func_drift_bottle_process();                                               // 调用处理函数,处理网络状态、重连等逻辑
        func_drift_bottle_message(msg_dequeue());                                  // 从消息队列取出消息并处理(触摸、按钮等事件)

        // ========== 电子烟功率调节处理 ==========
        if (sys_cb.smoke_index == SMOKING)                                         // 如果当前正在抽吸(系统抽吸状态为SMOKING)
        {
            if(power_ecig)                                                         // 如果电子烟功率调节标志为true
            {
                power_ecig = 0;                                                    // 清除功率调节标志
                if (power_idx < BAR_COUNT)                                         // 如果功率档位索引未超过最大值
                {
                    uteModuleSmokeData.current_power = uteModuleSmokeData.power_seq[power_idx]; // 从功率序列中获取当前档位的功率值
                    ecig_set_power(uteModuleSmokeData.current_power);              // 调用函数设置电子烟功率
                    power_idx++;                                                   // 功率档位索引递增(下次抽吸时使用下一档位)
                }
                printf("zzp_reminder_%d\n",uteModuleSmokeData.current_power);      // 调试输出:打印当前设置的功率值
            }
        }
    }

    // ========== 调用出口函数 ==========
    func_drift_bottle_exit();                                                      // 调用出口函数,清理资源并恢复设置
}