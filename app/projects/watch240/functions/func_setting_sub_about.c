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

typedef struct f_about_t_
{

} f_about_t;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#define SHAPE_HEIGTH  GUI_SCREEN_HEIGHT/4.5

//关于页面
compo_form_t *func_set_sub_about_form_create(void)
{
    compo_textbox_t * txt;

    char davName[40];
    memset(davName,'\0',sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
    uteModulePlatformGetDevName((uint8_t*)davName,&davNameLength);//获取设备名称

    uint8_t Ble_Address[6];//获取蓝牙地址数组
    char Ble_Address_str_buf[20];//蓝牙地址文本数组
    memset(Ble_Address_str_buf,'\0',sizeof(Ble_Address_str_buf));//初始化数组
    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址

    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02X:%02X:%02X:%02X:%02X:%02X",\
             Ble_Address[0],Ble_Address[1],Ble_Address[2],Ble_Address[3],Ble_Address[4],Ble_Address[5]); //信息

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_ABOUT]);

    //设备名称
    txt = compo_textbox_create(frm,strlen(i18n[STR_DEV_NEME]));
    compo_textbox_set(txt, i18n[STR_DEV_NEME]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2-GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm,strlen(davName));
    compo_textbox_set(txt, davName);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 -GUI_SCREEN_CENTER_Y/1.8);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //系统版本
    txt = compo_textbox_create(frm,strlen(i18n[STR_SYS_VERSION]));
    compo_textbox_set(txt, i18n[STR_SYS_VERSION]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1  - SHAPE_HEIGTH/2.2);

    txt = compo_textbox_create(frm,strlen(UTE_SW_VERSION));
    compo_textbox_set(txt, UTE_SW_VERSION);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 );
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙地址
    txt = compo_textbox_create(frm,strlen(i18n[STR_BLE_MAC]));
    compo_textbox_set(txt, i18n[STR_BLE_MAC]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2+GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
    compo_textbox_set(txt, (char*)Ble_Address_str_buf);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 +GUI_SCREEN_CENTER_Y/1.8);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

//关于页面
compo_form_t *func_set_sub_about_form_create(void)
{
#define SPACING  (164-82)
    compo_textbox_t * txt;
    char davName[40];
    memset(davName,'\0',sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
    uteModulePlatformGetDevName((uint8_t*)davName,&davNameLength);//获取设备名称

    uint8_t Ble_Address[6];//获取蓝牙地址数组
    char Ble_Address_str_buf[20];//蓝牙地址文本数组
    memset(Ble_Address_str_buf,'\0',sizeof(Ble_Address_str_buf));//初始化数组
    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址

    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02X:%02X:%02X:%02X:%02X:%02X",\
             Ble_Address[0],Ble_Address[1],Ble_Address[2],Ble_Address[3],Ble_Address[4],Ble_Address[5]); //信息

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_ABOUT]);

    //设备名称
    txt = compo_textbox_create(frm,strlen(i18n[STR_DEV_NEME]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98,220,34);
    compo_textbox_set(txt, i18n[STR_DEV_NEME]);

    txt = compo_textbox_create(frm,strlen(davName));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134,220,34);
    compo_textbox_set(txt, davName);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //系统版本
    txt = compo_textbox_create(frm,strlen(i18n[STR_SYS_VERSION]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING,220,34);
    compo_textbox_set(txt, i18n[STR_SYS_VERSION]);

    txt = compo_textbox_create(frm,strlen(UTE_SW_VERSION));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING,220,34);
    compo_textbox_set(txt, UTE_SW_VERSION);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙地址
    txt = compo_textbox_create(frm,strlen(i18n[STR_BLE_MAC]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING*2,220,34);
    compo_textbox_set(txt, i18n[STR_BLE_MAC]);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING*2,220,34);
    compo_textbox_set(txt, (char*)Ble_Address_str_buf);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

//关于页面
compo_form_t *func_set_sub_about_form_create(void)
{
#define SPACING  (164-82)
    compo_textbox_t * txt;
    char davName[40];
    memset(davName,'\0',sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
    uteModulePlatformGetDevName((uint8_t*)davName,&davNameLength);//获取设备名称

    uint8_t Ble_Address[6];//获取蓝牙地址数组
    char Ble_Address_str_buf[20];//蓝牙地址文本数组
    memset(Ble_Address_str_buf,'\0',sizeof(Ble_Address_str_buf));//初始化数组
    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址

    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02X:%02X:%02X:%02X:%02X:%02X",\
             Ble_Address[0],Ble_Address[1],Ble_Address[2],Ble_Address[3],Ble_Address[4],Ble_Address[5]); //信息

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_ABOUT]);

    //设备名称
    txt = compo_textbox_create(frm,strlen(i18n[STR_DEV_NEME]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98,220,34);
    compo_textbox_set(txt, i18n[STR_DEV_NEME]);

    txt = compo_textbox_create(frm,strlen(davName));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134,220,34);
    compo_textbox_set(txt, davName);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //系统版本
    txt = compo_textbox_create(frm,strlen(i18n[STR_SYS_VERSION]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING,220,34);
    compo_textbox_set(txt, i18n[STR_SYS_VERSION]);

    txt = compo_textbox_create(frm,strlen(UTE_SW_VERSION));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING,220,34);
    compo_textbox_set(txt, UTE_SW_VERSION);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙地址
    txt = compo_textbox_create(frm,strlen(i18n[STR_BLE_MAC]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING*2,220,34);
    compo_textbox_set(txt, i18n[STR_BLE_MAC]);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING*2,220,34);
    compo_textbox_set(txt, (char*)Ble_Address_str_buf);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    return frm;
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
#define SHAPE_HEIGTH  GUI_SCREEN_HEIGHT/4.5

//关于页面
compo_form_t *func_set_sub_about_form_create(void)
{
    compo_textbox_t * txt;

    char davName[40];
    memset(davName,'\0',sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
    uteModulePlatformGetDevName((uint8_t*)davName,&davNameLength);//获取设备名称

    uint8_t Ble_Address[6];//获取蓝牙地址数组
    char Ble_Address_str_buf[20];//蓝牙地址文本数组
    memset(Ble_Address_str_buf,'\0',sizeof(Ble_Address_str_buf));//初始化数组
    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址

    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02X:%02X:%02X:%02X:%02X:%02X",\
             Ble_Address[0],Ble_Address[1],Ble_Address[2],Ble_Address[3],Ble_Address[4],Ble_Address[5]); //信息

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_SETTING_ABOUT]);

    //设备名称
    txt = compo_textbox_create(frm,strlen(i18n[STR_DEV_NEME]));
    compo_textbox_set(txt, i18n[STR_DEV_NEME]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2-GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm,strlen(davName));
    compo_textbox_set(txt, davName);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 -GUI_SCREEN_CENTER_Y/1.8);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //系统版本
    txt = compo_textbox_create(frm,strlen(i18n[STR_SYS_VERSION]));
    compo_textbox_set(txt, i18n[STR_SYS_VERSION]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1  - SHAPE_HEIGTH/2.2);

    txt = compo_textbox_create(frm,strlen(UTE_SW_VERSION));
    compo_textbox_set(txt, UTE_SW_VERSION);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 );
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙地址
    txt = compo_textbox_create(frm,strlen(i18n[STR_BLE_MAC]));
    compo_textbox_set(txt, i18n[STR_BLE_MAC]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2+GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
    compo_textbox_set(txt, (char*)Ble_Address_str_buf);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y*1.1 +GUI_SCREEN_CENTER_Y/1.8);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    return frm;
}
#else
compo_form_t *func_set_sub_about_form_create(void)
{
}
#endif


//关于功能消息处理
static void func_set_sub_about_message(size_msg_t msg)
{

    if(sys_cb.power_on_state==false)
    {
        switch (msg)
        {
            case MSG_CTP_SHORT_DOWN:
                uteTaskGuiStartScreen(FUNC_POWER_ON_SCAN, FUNC_SWITCH_UD_ZOOM_DOWN | FUNC_SWITCH_AUTO,__func__);
                break;
            case KL_BACK:
                uteTaskGuiStartScreen(FUNC_LONG_PRESS, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO,__func__);
                break;
            case MSG_CTP_LONG:
                if (!uteApplicationCommonIsHasConnectOurApp())
                {
                    uteTaskGuiStartScreen(FUNC_TEST_MODE, 0, __func__);
                }
                break;
        }
    }
    else
    {
        switch (msg)
        {
            case MSG_CTP_LONG:
                if (!uteApplicationCommonIsHasConnectOurApp())
                {
                    uteTaskGuiStartScreen(FUNC_TEST_MODE, 0, __func__);
                }
                break;
            default:
                func_message(msg);
                break;
        }
    }

}

//进入关于功能
static void func_set_sub_about_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_about_t));
    func_cb.frm_main = func_set_sub_about_form_create();
}

//退出关于功能
static void func_set_sub_about_exit(void)
{
    func_cb.last = FUNC_SET_SUB_ABOUT;
}

//关于功能
void func_set_sub_about(void)
{
    printf("%s\n", __func__);
    func_set_sub_about_enter();
    while (func_cb.sta == FUNC_SET_SUB_ABOUT)
    {
        func_process();
        func_set_sub_about_message(msg_dequeue());
    }
    func_set_sub_about_exit();
}
