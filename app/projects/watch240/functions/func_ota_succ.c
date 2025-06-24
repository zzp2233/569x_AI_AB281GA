#include "include.h"
#include "func.h"
#include "ute_application_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
enum
{
    COMPO_ID_TXT_OTA_SUCC1 = 1,
    COMPO_ID_TXT_OTA_SUCC2,
};

typedef struct f_ota_succ_t_
{

} f_ota_succ_t;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_succ_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_01_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 35+112/2);

    //TXT1 升级成功
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_SUCC1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATED]);

    //TXT2 手表即将进入重启
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_SUCC2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 215+26/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt, i18n[STR_WATCH_RESTART]);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_succ_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_OTA_01_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 60+150/2);

    //TXT1 升级成功
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_SUCC1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 250+32/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATED]);

    //TXT2 手表即将进入重启
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_SUCC2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 293+32/2, 224, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt, i18n[STR_WATCH_RESTART]);

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_succ_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I335001_UPGRADE_UPDATE_SUCCESSED_ICON_SUCCEEDED_92X92_X74_Y65_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-35);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+55,150,50);
    compo_textbox_set(textbox,i18n[STR_UPDATED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WATCH_RESTART]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+95,150,30);
    compo_textbox_set(textbox,i18n[STR_WATCH_RESTART]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_succ_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_1_START_SUCCEED_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 104+72);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+80,320,50);
    compo_textbox_set(textbox,i18n[STR_UPDATED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WATCH_RESTART]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+136,320,50);
    compo_textbox_set(textbox,i18n[STR_WATCH_RESTART]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_succ_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I343001_UPGRADE_SUCCEED_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 91+124/2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,247+50/2,300,50);
    compo_textbox_set(textbox,i18n[STR_UPDATED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WATCH_RESTART]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,300+50/2,300,50);
    compo_textbox_set(textbox,i18n[STR_WATCH_RESTART]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//创建地图窗体
compo_form_t *func_ota_succ_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I338001_UPGRADE_00_SUCCEED_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 76+62);

    //TXT1 升级成功
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_SUCC1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 228+15, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATED]);

    //TXT2 手表即将进入重启
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_SUCC2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 274+15, 224, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt, i18n[STR_WATCH_RESTART]);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_succ_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I340001_OTA_01_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 60+150/2);

    //TXT1 升级成功
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_SUCC1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 250+32/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATED]);

    //TXT2 手表即将进入重启
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_SUCC2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 293+32/2, 224, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt, i18n[STR_WATCH_RESTART]);

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT


//创建地图窗体
compo_form_t *func_ota_succ_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I342001_UPGRADE_SUCCEED_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 50+82/2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,152+30/2,150,50);
    compo_textbox_set(textbox,i18n[STR_UPDATED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WATCH_RESTART]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,182+30/2,150,30);
    compo_textbox_set(textbox,i18n[STR_WATCH_RESTART]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}

#else
compo_form_t *func_ota_succ_form_create(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//地图功能事件处理
static void func_ota_succ_process(void)
{

    if (tick_check_expire(func_cb.enter_tick, 2000))
    {
        // WDT_RST();
        uteApplicationCommonRestart();
    }

    func_process();
}
//地图功能消息处理
static void func_ota_succ_message(size_msg_t msg)
{
    evt_message(msg);
}

//进入地图功能
static void func_ota_succ_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_ota_succ_t));
    func_cb.frm_main = func_ota_succ_form_create();
    func_cb.enter_tick = tick_get();
}

//退出地图功能
static void func_ota_succ_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_OTA_SUCC;
}

//地图功能
void func_ota_succ(void)
{
    printf("%s\n", __func__);
    func_ota_succ_enter();
    while (func_cb.sta == FUNC_OTA_SUCC)
    {
        func_ota_succ_process();
        func_ota_succ_message(msg_dequeue());
    }
    func_ota_succ_exit();
}
