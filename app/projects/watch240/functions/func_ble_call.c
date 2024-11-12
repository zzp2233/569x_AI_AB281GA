#include "include.h"
#include "func.h"

enum
{
    COMPO_ID_TXT_NUMBER = 1,     //避免id被覆盖
    COMPO_ID_BTN_REJECT,
};

typedef struct f_ble_call_t_
{

} f_ble_call_t;


//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_ble_call_form_create(void)
{
    //printf("%s\n", __func__);

    char txt_buf[20]="121353461";///假数据

    //新建窗体, 通话页面
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;
    compo_textbox_t *txt;

    snprintf((char*)txt_buf,sizeof(txt_buf),"121353461");
    txt = compo_textbox_create(frm, strlen(txt_buf));
    compo_setid(txt, COMPO_ID_TXT_NUMBER);
    compo_textbox_set(txt, txt_buf);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5);

    //挂断按钮
    btn = compo_button_create_by_image(frm, UI_BUF_CALL_REJECT_BIN);
    compo_setid(btn, COMPO_ID_BTN_REJECT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-GUI_SCREEN_HEIGHT/4);

    return frm;
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
    f_ble_call_t *f_ble_call = (f_ble_call_t *)func_cb.f_cb;
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

