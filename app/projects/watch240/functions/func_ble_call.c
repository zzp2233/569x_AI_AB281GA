#include "include.h"
#include "func.h"
#include "ute_module_call.h"

enum
{
    COMPO_ID_TXT_NUMBER = 0x01,     //避免id被覆盖
    COMPO_ID_TXT_NAME,
    COMPO_ID_TXT_TIME,
    COMPO_ID_BTN_REJECT,
};

typedef struct f_ble_call_t_
{
    u16 times;                  //通话秒数
    char call_time_str[10];     //通话计时字符串
} f_ble_call_t;


//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_ble_call_form_create(void)
{
    //printf("%s\n", __func__);

    char txt_buf[20]="121353461";///假数据

    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

//    ute_bt_call_data_t callData;
//    uteModuleCallGetData(&callData);

    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5-GUI_SCREEN_CENTER_Y/6, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autosize(name_txt, true);
//    compo_textbox_set(name_txt, callData.name);
//    compo_textbox_set(name_txt, "中国移动");
    compo_setid(name_txt, COMPO_ID_TXT_NAME);

    compo_textbox_t *number_txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(number_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autosize(number_txt, true);
    compo_setid(number_txt, COMPO_ID_TXT_NUMBER);
//    compo_textbox_set(number_txt,callData.number);

    compo_textbox_t *time_txt = compo_textbox_create(frm, 10);
    compo_textbox_set_location(time_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5+GUI_SCREEN_CENTER_Y/6, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autosize(time_txt, true);
    compo_setid(time_txt, COMPO_ID_TXT_TIME);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_CALL_CALLING_END_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 240);


    return frm;
}

void bt_incall_time_update(void)
{
    f_ble_call_t *f_ble_call = (f_ble_call_t *)func_cb.f_cb;
    char *call_time_str = f_ble_call->call_time_str;

#if !CALL_MGR_EN
    u16 call_times = f_ble_call->times;
#else
    u16 call_times = bt_cb.times;
#endif

    uteModuleCallUpdateCallingTimeSecond(call_times);

    u8 hours   = call_times / 3600;
    u8 minutes = (call_times % 3600) / 60;
    u8 seconds = call_times % 60;
    sprintf(call_time_str, "%02d:%02d:%02d", hours, minutes, seconds);
//    printf("call_time_str: %s\n", call_time_str);

    compo_textbox_t *time_txt = compo_getobj_byid(COMPO_ID_TXT_TIME);
    compo_textbox_set(time_txt, call_time_str);
    compo_textbox_set_visible(time_txt, true);
}


void func_ble_call_process(void)
{
    func_process();
}

static void func_ble_call_click(void)
{
//     f_ble_call_t *f_ble_call = (f_ble_call_t *)func_cb.f_cb;

    int id = compo_get_button_id();
    switch (id)
    {

        case COMPO_ID_BTN_REJECT:

            break;

        default:
            break;
    }

}

//蓝牙音乐消息处理
static void func_ble_call_message(size_msg_t msg)
{
//    f_ble_call_t *f_ble_call = (f_ble_call_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_ble_call_click();
            break;
        case MSG_SYS_1S:
            bt_incall_time_update();
            break;
        default:
            func_message(msg);
            break;
    }
}

void func_ble_call_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_ble_call_t));
    func_cb.frm_main = func_ble_call_form_create();

}

void func_ble_call_exit(void)
{
//    f_ble_call_t *f_ble_call = (f_ble_call_t *)func_cb.f_cb;
    func_cb.last = FUNC_BLE_CALL;
}

void func_ble_call(void)
{
    printf("%s\n", __func__);
    func_ble_call_enter();
    while (func_cb.sta == FUNC_BLE_CALL)
    {
        func_ble_call_process();
        func_ble_call_message(msg_dequeue());
    }
    func_ble_call_exit();
}

