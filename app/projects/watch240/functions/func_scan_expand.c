#include "include.h"
#include "func.h"

#if UTE_MODULE_SCREENS_SCAN_EXPAND_SUPPORT

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
typedef struct f_scan_expand_t_
{
    u32 tick;
} f_scan_expand_t;

enum
{
    COMPO_ID_BTN_SKIP = 1,
};

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

extern bool oraimoWatchAppBindingGetQRcodeData(char *getQRcodeLink, uint8_t *maxSizeQrCodeLink);

compo_form_t *func_scan_expand_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_button_t *btn = compo_button_create(frm);//compo_button_create_by_image(frm, UI_BUF_I341001_1_START_QR_CODE_BIN);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 286, 286);
    compo_setid(btn,COMPO_ID_BTN_SKIP);

    uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    oraimoWatchAppBindingGetQRcodeData(qr_str,&maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 240);
    uteModulePlatformMemoryFree(qr_str);
    compo_qrcodebox_set_pos(qrbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    widget_rect_set_radius(qrbox->bg, 20);

    return frm;
}
#endif

void func_scan_expand_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SKIP:
            uteTaskGuiStartScreen(FUNC_POWER_ON_SCAN, 0, __func__);
            //func_switch_to(FUNC_POWER_ON_SCAN, FUNC_SWITCH_LR_RIGHT | FUNC_SWITCH_AUTO);
            break;

        default:
            break;
    }
}

static void func_scan_expand_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_scan_expand_click_handler();
            break;

        case KU_BACK:
        case KU_DELAY_BACK:
        case MSG_CTP_SHORT_RIGHT:
            //func_switch_to(FUNC_POWER_ON_SCAN, FUNC_SWITCH_LR_RIGHT | FUNC_SWITCH_AUTO);
            uteTaskGuiStartScreen(FUNC_POWER_ON_SCAN, 0, __func__);
            break;
    }
}

//下滑事件流程处理
static void func_scan_expand_process(void)
{
    func_process();
}

//创建界面
static void func_scan_expand_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_scan_expand_t));
    func_cb.frm_main = func_scan_expand_form_create();
}
//退出任务
static void func_scan_expand_exit(void)
{
    func_cb.last = FUNC_SCAN_EXPAND;
}

//说明功能
void func_scan_expand(void)
{
    printf("%s\n", __func__);
    func_scan_expand_enter();
    while (func_cb.sta == FUNC_SCAN_EXPAND)
    {
        func_scan_expand_process();
        func_scan_expand_message(msg_dequeue());
    }
    func_scan_expand_exit();
}

#endif

