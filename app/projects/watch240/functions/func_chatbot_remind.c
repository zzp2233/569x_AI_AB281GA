#include "include.h"
#include "func.h"
#include "ute_module_log.h"
#include "string.h"
#include "ute_application_common.h"
#include "ute_module_systemtime.h"


#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif



typedef struct f_chatbot_remind_t_
{
    //bool flag_drag;                 //开始拖动
    s32 x_pos;
    s32 y_pos;
    s32 x;
    s32 y;

} f_chatbot_remind_t;
//关于页面
compo_form_t *func_chatbot_remind_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_picturebox_t *picbox;
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_WEATHER_ICON_WEATHER_14_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    return frm;
}


//关于功能消息处理
static void func_chatbot_remind_message(size_msg_t msg)
{

    switch (msg)
    {

        //  case KL_BACK:
        //  printf("12121212121212\r\n");
        //  func_cb.sta = FUNC_CHATBOT;
        //    break;
        default:
            func_message(msg);
            break;
    }


}

//进入关于功能
static void func_chatbot_remind_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_chatbot_remind_t));
    func_cb.frm_main = func_chatbot_remind_form_create();
}

//退出关于功能
static void func_chatbot_remind_exit(void)
{
    func_cb.last = FUNC_CHATBOT_REMIND;
}

//关于功能
void func_chatbot_remind(void)
{
    printf("%s\n", __func__);
    func_chatbot_remind_enter();
    while (func_cb.sta == FUNC_CHATBOT_REMIND)
    {
        func_process();
        func_chatbot_remind_message(msg_dequeue());
    }
    func_chatbot_remind_exit();
}
