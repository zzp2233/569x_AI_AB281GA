
#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

typedef struct f_ota_err_t_
{

} f_ota_err_t;

//创建地图窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}

//地图功能事件处理
static void func_ota_err_process(void)
{
    func_process();
}
#include "func_cover.h"
//地图功能消息处理
static void func_ota_err_message(size_msg_t msg)
{
    evt_message(msg);
}

//进入地图功能
static void func_ota_err_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_ota_err_t));
    func_cb.frm_main = func_ota_err_form_create();
}

//退出地图功能
static void func_ota_err_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_IDLE;
}

//地图功能
void func_ota_err(void)
{
    printf("%s\n", __func__);
    func_ota_err_enter();
    while (func_cb.sta == FUNC_OTA_ERROR)
    {
        func_ota_err_process();
        func_ota_err_message(msg_dequeue());
    }
    func_ota_err_exit();
}
