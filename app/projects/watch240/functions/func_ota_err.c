
#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
enum
{
    COMPO_ID_TXT_OTA_ERR1 = 1,
    COMPO_ID_TXT_OTA_ERR2,
};

typedef struct f_ota_err_t_
{

} f_ota_err_t;

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_02_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 35+112/2);

    //TXT1 升级失败
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATE_FAILED]);

    //TXT2 请连接App重试
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 215+26/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt,i18n[STR_APP_AGIAN]);

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_OTA_02_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 60+150/2);

    //TXT1 升级失败
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 250+32/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATE_FAILED]);

    //TXT2 请连接App重试
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 293+32/2, 224, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt,i18n[STR_APP_AGIAN]);

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I335001_UPGRADE_UPGRADE_UNSUCCESSFUL_ICON_SYNC_FAILED_92X92_X74_Y65_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-35);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATE_FAILED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+55,150,50);
    compo_textbox_set(textbox,i18n[STR_UPDATE_FAILED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_AGIAN]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+95,150,30);
    compo_textbox_set(textbox,i18n[STR_APP_AGIAN]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_1_START_FAIL_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 104+72);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATE_FAILED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+80,320,50);
    compo_textbox_set(textbox,i18n[STR_UPDATE_FAILED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_AGIAN]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+136,320,50);
    compo_textbox_set(textbox,i18n[STR_APP_AGIAN]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//创建地图窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I343001_UPGRADE_FAIL_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 91+124/2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATE_FAILED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,247+50/2,300,50);
    compo_textbox_set(textbox,i18n[STR_UPDATE_FAILED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_AGIAN]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,300+50/2,300,50);
    compo_textbox_set(textbox,i18n[STR_APP_AGIAN]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
//创建地图窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I338001_UPGRADE_01_FAIL_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 76+62);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATE_FAILED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,228+25,150,50);
    compo_textbox_set(textbox,i18n[STR_UPDATE_FAILED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_AGIAN]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,274+15,150,30);
    compo_textbox_set(textbox,i18n[STR_APP_AGIAN]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//创建窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I340001_OTA_02_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 60+150/2);

    //TXT1 升级失败
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR1);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 250+32/2, 196, 30);
    widget_text_set_color(txt->txt, make_color(255,255,255));
    compo_textbox_set(txt, i18n[STR_UPDATE_FAILED]);

    //TXT2 请连接App重试
    txt = compo_textbox_create(frm, 20);
    compo_setid(txt, COMPO_ID_TXT_OTA_ERR2);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 293+32/2, 224, 30);
    widget_text_set_color(txt->txt, make_color(128,128,128));
    compo_textbox_set(txt,i18n[STR_APP_AGIAN]);

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT


//创建地图窗体
compo_form_t *func_ota_err_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I342001_UPGRADE_UNSUCCEED_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 50+82/2);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATE_FAILED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,152+30/2,150,50);
    compo_textbox_set(textbox,i18n[STR_UPDATE_FAILED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_AGIAN]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,182+30/2,150,30);
    compo_textbox_set(textbox,i18n[STR_APP_AGIAN]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));

    return frm;
}

#else
compo_form_t *func_ota_err_form_create(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//地图功能事件处理
static void func_ota_err_process(void)
{
    func_process();
}
#include "func_cover.h"
//地图功能消息处理
static void func_ota_err_message(size_msg_t msg)
{

    switch (msg)
    {
        // case MSG_CTP_SHORT_LEFT:
        // case MSG_CTP_SHORT_UP:
        // case MSG_CTP_SHORT_RIGHT:
        // case MSG_CTP_SHORT_DOWN:

        //     break;

        case KU_BACK:
            func_switch_to(FUNC_CLOCK, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);
            break;

        default:
            func_message(msg);
            break;
    }


}

//进入地图功能
static void func_ota_err_enter(void)
{
    // uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_ota_err_t));
    func_cb.frm_main = func_ota_err_form_create();
}

//退出地图功能
static void func_ota_err_exit(void)
{
    // uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_OTA_ERROR;
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
