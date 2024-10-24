#include "include.h"
#include "func.h"
#include "func_bt.h"

enum {
    COMPO_ID_TXT_NUMBER = 0xff,     //避免id被覆盖
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
    COMPO_ID_BTN_MIC,
};

typedef struct f_bt_call_t_ {
    bool sta;                   //0:呼出(outgoing); 1:通话(call);

    u16 times;                  //通话秒数
    char call_time_str[10];     //通话计时字符串
    u32 clcc_tick;              //主动查询号码计时
    u32 exit_tick;              //页面退出计时
} f_bt_call_t;

static void func_bt_call_back_to(void)
{
    u8 last_func = func_directly_back_to();
    if (last_func == FUNC_BT_RING) {
        func_directly_back_to();
    }
}

void bt_incall_time_update(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;
    char *call_time_str = f_bt_call->call_time_str;

#if !CALL_MGR_EN
    u16 call_times = f_bt_call->times;
#else
    u16 call_times = bt_cb.times;
#endif
    u8 hours   = call_times / 3600;
    u8 minutes = (call_times % 3600) / 60;
    u8 seconds = call_times % 60;
    sprintf(call_time_str, "%02d:%02d:%02d", hours, minutes, seconds);
//    printf("call_time_str: %s\n", call_time_str);

    compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_TIME);
    compo_textbox_set(time_txt, call_time_str);
    compo_textbox_set_visible(time_txt, true);
}

void func_bt_call_number_update(void)
{
    if (bt_cb.number_sta) {
        compo_textbox_t *number_txt = compo_getobj_byid(COMPO_ID_TXT_NUMBER);
        compo_textbox_set(number_txt, hfp_get_last_call_number(0));
    }
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_call_form_create(void)
{
    //printf("%s\n", __func__);
    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autosize(number_txt, true);
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    compo_textbox_t *time_txt = compo_textbox_create(frm, 10);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/4, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autosize(time_txt, true);
    compo_setid(time_txt, COMPO_ID_TXT_TIME);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_CALL_REJECT_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-GUI_SCREEN_HEIGHT/4);

    //mic
    btn = compo_button_create_by_image(frm, UI_BUF_CALL_MUTE_BIN);
    compo_setid(btn, COMPO_ID_BTN_MIC);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X*2/3, GUI_SCREEN_HEIGHT-GUI_SCREEN_HEIGHT/4);


    return frm;
}

//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_outgoing_form_create(void)
{
//    新建窗体, 呼出页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autosize(number_txt, true);
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
    msg_enqueue(EVT_CALL_NUMBER_UPDATE);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_CALL_REJECT_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-GUI_SCREEN_HEIGHT/4);


    return frm;
}

AT(.text.func.bt)
static void func_bt_call_interface(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (bt_cb.disp_status == BT_STA_INCALL && f_bt_call->sta == false) {
        //销毁窗体
        if (func_cb.frm_main != NULL) {
            compo_form_destroy(func_cb.frm_main);
        }

        func_cb.frm_main = func_bt_call_form_create();
        f_bt_call->sta = true;
    }
}

AT(.text.func.bt)
static void func_bt_call_exit_process(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    if (!bt_cb.number_sta) {                                                            //号码未通知, 主动查询
        if (tick_check_expire(f_bt_call->clcc_tick, 500)) {
            f_bt_call->clcc_tick = tick_get();
            bt_call_get_remote_phone_number();
        }
    }

#if !CALL_MGR_EN
    if(bt_cb.disp_status != BT_STA_OUTGOING && bt_cb.disp_status != BT_STA_INCALL) {    //退出通话页面
        func_bt_call_back_to();
    } else
#endif
    if (f_bt_call->exit_tick && tick_check_expire(f_bt_call->exit_tick, 500)) {        //强制退出, 防呆
        printf("call reject, force exit!\n");
        sys_cb.reject_tick = tick_get();
        f_bt_call->exit_tick = 0;
        func_bt_call_back_to();
    }
}

void func_bt_call_process(void)
{
    func_process();
    func_bt_sub_process();
    func_bt_call_exit_process();
    func_bt_call_interface();
}

static void func_bt_call_click(void)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id) {

    case COMPO_ID_BTN_REJECT:
        printf("COMPO_ID_BTN_REJECT\n");
        bt_call_terminate();
        f_bt_call->exit_tick = tick_get();
        break;

    default:
        break;
    }

}

//蓝牙音乐消息处理
static void func_bt_call_message(size_msg_t msg)
{
    f_bt_call_t *f_bt_call = (f_bt_call_t *)func_cb.f_cb;

    switch (msg) {
    case KU_BACK:
        if(bt_get_call_indicate() == BT_CALL_INCOMING) {
            bt_call_answer_incoming();                  //接听第2路通话
        } else {
            bt_call_terminate();                        //挂断当前通话
        }
        break;

    case KU_RIGHT:
        bt_volume_up();
        break;

    case KU_LEFT:
        bt_volume_down();
        break;

    case MSG_SYS_1S:
        func_message(MSG_SYS_1S);
        if (f_bt_call->sta == true) {
#if !CALL_MGR_EN
            f_bt_call->times++;
#endif
            bt_incall_time_update();
        }
        break;

    case MSG_CTP_CLICK:
        func_bt_call_click();                           //单击按钮
        break;

    case EVT_CALL_NUMBER_UPDATE:
        func_bt_call_number_update();
        break;

    case MSG_SYS_500MS:
        reset_sleep_delay_all();                           //来电不休眠
        break;

    default:
        func_message(msg);
        break;
    }
}

void func_bt_call_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_bt_call_t));
    func_cb.frm_main = func_bt_outgoing_form_create();

    func_cb.mp3_res_play = func_bt_mp3_res_play;

    bsp_bt_call_enter();
    if (sys_cb.gui_sleep_sta) {
        sys_cb.gui_need_wakeup = 1;
    }
}

void func_bt_call_exit(void)
{
    bsp_bt_call_exit();

    if (func_cb.sta == FUNC_SCAN) {
        while (sys_cb.sco_state == true) {        //等待SCO完全退出, 支付宝和扫一扫界面都使用二维码，二维码与通话复用
            WDT_CLR();
        }
    }
}

void func_bt_call(void)
{
    printf("%s\n", __func__);
    func_bt_call_enter();
    while (func_cb.sta == FUNC_BT_CALL) {
        func_bt_call_process();
        func_bt_call_message(msg_dequeue());
    }
    func_bt_call_exit();
}
