#include "include.h"
#include "func.h"
#include "ute_application_common.h"
#include "ute_module_appbinding.h"
#include "ute_module_gui_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
typedef struct f_power_on_scan_t_
{
    bool screenOn;
    u32 tick;
} f_power_on_scan_t;
enum
{
    COMPO_ID_BTN_SKIP = 1,
    COMPO_ID_BTN_ABOUT,
    COMPO_ID_BTN_EXPAND,
};
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_QRCODE]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_PUBLIC_MORE_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, 24/4+278-FORM_TITLE_HEIGHT);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,235,GUI_SCREEN_WIDTH,28);
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, GUI_SCREEN_CENTER_X*0.7);
    uteModulePlatformMemoryFree(qr_str);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT


//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_QRCODE]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I332001_PUBLIC_MORE_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, 360-32/2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,302,230,64);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set_pos(qrbox,GUI_SCREEN_CENTER_X,90+82);
    compo_qrcodebox_set_bitwid(qrbox, 100);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 120);
    compo_qrcodebox_set(qrbox, qr_str);
    uteModulePlatformMemoryFree(qr_str);

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I335001_1_START_01_BIN);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 10/2+260,33,30);
    compo_setid(btn,COMPO_ID_BTN_SKIP);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,220,164,58);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 110);
    compo_qrcodebox_set_bitwid(qrbox, 110);
    uteModulePlatformMemoryFree(qr_str);
    compo_qrcodebox_set_pos(qrbox,GUI_SCREEN_CENTER_X,110);

    return frm;
}
static void func_power_on_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SKIP:
            uteTaskGuiStartScreen(FUNC_CLOCK, 0, __func__);
            break;

        default:
            break;
    }
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I343001_1_START_NEXT_BIN);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 14/2+356,24,14);
    compo_setid(btn,COMPO_ID_BTN_SKIP);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,278+68/2,164,68);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 120);
    compo_qrcodebox_set_bitwid(qrbox, 120);
    uteModulePlatformMemoryFree(qr_str);
    compo_qrcodebox_set_pos(qrbox,GUI_SCREEN_CENTER_X,59+202/2);

    return frm;
}
static void func_power_on_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SKIP:
            uteTaskGuiStartScreen(FUNC_CLOCK, 0, __func__);
            break;

        default:
            break;
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
// 获取传音二维码链接（含随机码）
bool oraimoWatchAppBindingGetQRcodeData(char *getQRcodeLink, uint8_t *maxSizeQrCodeLink)
{
    if (maxSizeQrCodeLink == NULL || getQRcodeLink == NULL || *maxSizeQrCodeLink == 0)
    {
        return false;
    }
    uint8_t qrCodeLinkSize = *maxSizeQrCodeLink;

    uint8_t macTmpChar[18];
    uint8_t macTmp[6];
    uteModulePlatformGetBleMacAddress(macTmp);
    memset(macTmpChar, 0, sizeof(macTmpChar));

    for (int i = 0; i < 6; ++i)
    {
        macTmpChar[i * 3] = (macTmp[i] / 16) + ((macTmp[i] / 16) > 9 ? ('A' - 10) : '0');
        macTmpChar[i * 3 + 1] = (macTmp[i] % 16) + ((macTmp[i] % 16) > 9 ? ('A' - 10) : '0');
        if (i < 5)
        {
            macTmpChar[i * 3 + 2] = ':';
        }
    }
    macTmpChar[17] = '\0';

    // 生成随机码
    uint32_t random = uteModuleAppBindingGetQRcodeRandom();
    if (random < 10000000)
    {
        random = 10000000 + get_random(0xffff); // 确保至少是 8 位数字
        uteModuleAppBindingSetQRcodeRandom(random);
    }

    int written = snprintf(getQRcodeLink, qrCodeLinkSize, "%s?%s&%s&%lu",
                           UTE_BINDING_QRENCODE_LINK, macTmpChar,
                           UTE_CUSTOM_DEVICE_MODEL, random);
    *maxSizeQrCodeLink = written;

    // 检查是否写入成功且未被截断
    if (written < 0 || written >= qrCodeLinkSize)
    {
        getQRcodeLink[0] = '\0'; // 清空无效内容
        return false;
    }

    return true;
}

//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TIME | COMPO_FORM_MODE_SHOW_ICON);
    compo_form_set_title_icon(frm, UI_BUF_I341001_1_START_RETURN_BIN);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, 35, 33, 70, 66);
    compo_setid(btn, COMPO_ID_BTN_SKIP);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_1_START_ABOUT_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 314);
    btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, 316, 50, 50);
    compo_setid(btn, COMPO_ID_BTN_ABOUT);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_DOWNLOAD_APP]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,372,328,70);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_DOWNLOAD_APP]);

    ///创建页码点
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_1_START_POINT_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 419+6);
    compo_picturebox_cut(picbox, 0, 2);

    // btn = compo_button_create_by_image(frm, UI_BUF_I341001_1_START_QR_CODE_BIN);
    // compo_button_set_pos(btn,GUI_SCREEN_CENTER_X, 184);
    // compo_setid(btn, COMPO_ID_BTN_EXPAND);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 184, 135, 135);
    compo_setid(btn, COMPO_ID_BTN_EXPAND);

    uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    oraimoWatchAppBindingGetQRcodeData(qr_str,&maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 180);
    uteModulePlatformMemoryFree(qr_str);
    compo_qrcodebox_set_pos(qrbox, GUI_SCREEN_CENTER_X, 178);
    widget_rect_set_radius(qrbox->bg, 10);

    return frm;
}
static void func_power_on_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SKIP:
            func_switch_to(FUNC_POWER_ON_LANGUAGE, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
            //uteTaskGuiStartScreen(FUNC_CLOCK, 0, __func__);
            break;
        case COMPO_ID_BTN_ABOUT:
            uteTaskGuiStartScreen(FUNC_SCAN_ABOUT, 0, __func__);
            break;
        case COMPO_ID_BTN_EXPAND:
            uteTaskGuiStartScreen(FUNC_SCAN_EXPAND, 0, __func__);
            break;
        default:
            break;
    }
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    // compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    // compo_form_set_title(frm, i18n[STR_QRCODE]);

    compo_button_t *btn = compo_button_create_image_in_btn(frm, UI_BUF_I338001_28_SET_NEXT_BIN,GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT-30,60,60);
    compo_setid(btn,COMPO_ID_BTN_SKIP);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,290,230,35);
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set_pos(qrbox,GUI_SCREEN_CENTER_X,90+60);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, GUI_SCREEN_CENTER_X*0.7);
    widget_rect_set_radius(qrbox->bg, 20);
    uteModulePlatformMemoryFree(qr_str);

    return frm;
}
static void func_power_on_click_handler(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SKIP:
            uteTaskGuiStartScreen(FUNC_CLOCK, 0, __func__);
            break;

        default:
            break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT


//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_QRCODE]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I340001_PUBLIC_MORE_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, 360-32/2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,302,230,64);
    compo_textbox_set_multiline(textbox,true);
    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set_pos(qrbox,GUI_SCREEN_CENTER_X,90+82);
    compo_qrcodebox_set_bitwid(qrbox, 100);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 120);
    compo_qrcodebox_set(qrbox, qr_str);
    uteModulePlatformMemoryFree(qr_str);

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

//创建扫一扫窗体
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_QRCODE]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I342001_1_START_NEXT_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X, 240+8/2-14);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_DOWNLOAD]) );
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,186+22/2,144,64);
//    compo_textbox_set_multiline(textbox,true);
//    widget_text_set_ellipsis(textbox->txt, false);      //避免既有滚动又有省略号的情况
    compo_textbox_set(textbox,i18n[STR_APP_DOWNLOAD]);

    static const uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    uteApplicationCommonGetDeviceQrCodeLink(qr_str,maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set_pos(qrbox,GUI_SCREEN_CENTER_X,140/2+38);
    compo_qrcodebox_set_bitwid(qrbox, 80);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 80);
    compo_qrcodebox_set(qrbox, qr_str);
    uteModulePlatformMemoryFree(qr_str);

    return frm;
}
#else
compo_form_t *func_power_on_scan_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//扫一扫功能事件处理
static void func_power_on_scan_process(void)
{
    f_power_on_scan_t *f_power_on_scan = (f_power_on_scan_t *)func_cb.f_cb;
    if (f_power_on_scan->screenOn)
    {
        reset_sleep_delay_all();
    }

    if (tick_check_expire(f_power_on_scan->tick, (60 - uteModuleGuiCommonGetDisplayOffTime()) * 1000))
    {
        f_power_on_scan->screenOn = false;
    }

    func_process();
}

//扫一扫功能消息处理
static void func_power_on_scan_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
            func_power_on_click_handler();
#endif
            break;
        case MSG_CTP_SHORT_RIGHT:
            func_switch_to(FUNC_POWER_ON_LANGUAGE, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
            break;
#if !GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case MSG_CTP_SHORT_UP:
            func_switch_to(FUNC_SET_SUB_ABOUT, FUNC_SWITCH_UD_ZOOM_UP | FUNC_SWITCH_AUTO);
#else
        case MSG_CTP_SHORT_LEFT:
            func_switch_to(FUNC_SET_SUB_ABOUT, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
            // uteTaskGuiStartScreen(FUNC_SET_SUB_ABOUT, FUNC_SWITCH_LR_LEFT | FUNC_SWITCH_AUTO, __func__);
#endif
            break;
        case MSG_CTP_LONG:
            // task_stack_init();  // 任务堆栈
            // latest_task_init(); // 最近任务
            // func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
            // sys_cb.power_on_state = true;
            break;
        case KU_BACK:
        case KU_DELAY_BACK:
#if UTE_DRV_PWRKEY_KEY0_KU_BACK_IN_CLOCK
            task_stack_init();  // 任务堆栈
            latest_task_init(); // 最近任务
            func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
#endif
            break;
        case KL_BACK:
            func_switch_to(FUNC_LONG_PRESS, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
            break;
        case MSG_SYS_500MS:
            if (uteApplicationCommonIsHasConnectOurApp())
            {
                task_stack_init();  // 任务堆栈
                latest_task_init(); // 最近任务
                func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
            }
            break;

        case MSG_CTP_TOUCH:
        case KEY_BACK:
        {
            if (func_cb.sta == FUNC_POWER_ON_SCAN)
            {
                f_power_on_scan_t *f_power_on_scan = (f_power_on_scan_t *)func_cb.f_cb;
                f_power_on_scan->tick = tick_get();
            }
        }
        break;
    }
}

//进入扫一扫功能
static void func_power_on_scan_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_power_on_scan_t));
    func_cb.frm_main = func_power_on_scan_form_create();

    f_power_on_scan_t *f_power_on_scan = (f_power_on_scan_t *)func_cb.f_cb;
    f_power_on_scan->screenOn = true;
    f_power_on_scan->tick = tick_get();
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
}

//退出扫一扫功能
static void func_power_on_scan_exit(void)
{
    func_cb.last = FUNC_POWER_ON_SCAN;
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
}

//扫一扫功能
void func_power_on_scan(void)
{
    printf("%s\n", __func__);
    func_power_on_scan_enter();
    while (func_cb.sta == FUNC_POWER_ON_SCAN)
    {
        func_power_on_scan_process();
        func_power_on_scan_message(msg_dequeue());
    }
    func_power_on_scan_exit();
}
