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
    page_tp_move_t *ptm;
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
    char Ble_last_four[10];
    snprintf(Ble_last_four, sizeof(Ble_last_four), "%02X%02X", Ble_Address[4], Ble_Address[5]);

    // 组合蓝牙名称和后四位地址
    char combined_name[50];
    snprintf(combined_name, sizeof(combined_name), "%s(ID-%s)", davName, Ble_last_four);

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
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134,220,34);
    compo_textbox_set(txt, davName);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //系统版本
    txt = compo_textbox_create(frm,strlen(i18n[STR_SYS_VERSION]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING,220,34);
    compo_textbox_set(txt, i18n[STR_SYS_VERSION]);

#ifdef UTE_SW_VERSION_FIRST_NAME
    u8 str[100];
    snprintf(str,100,"%s%s",UTE_SW_VERSION_FIRST_NAME,UTE_SW_VERSION);
    txt = compo_textbox_create(frm,strlen(str));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING,250,34);
    compo_textbox_set(txt, str);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
#else
    txt = compo_textbox_create(frm,strlen(UTE_SW_VERSION));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING,250,34);
    compo_textbox_set(txt, UTE_SW_VERSION);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
#endif


    //蓝牙地址
    txt = compo_textbox_create(frm,strlen(i18n[STR_BLE_MAC]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING*2,220,34);
    compo_textbox_set(txt, i18n[STR_BLE_MAC]);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
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

    txt = compo_textbox_create(frm,strlen(combined_name));
    compo_textbox_set(txt, combined_name);
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

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
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

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

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


#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#if UTE_MODULE_MODEL_NUMBER_SUPPORT
#define UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT 1    //关于页面支持长页面
#else
#define UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT 0    //关于页面不支持长页面
#endif
//关于页面
compo_form_t *func_set_sub_about_form_create(void)
{
#define SPACING  (164-82)
#if UTE_MODULE_MODEL_NUMBER_SUPPORT
#define CONTENT_NUM  4 //内容数量
    char modelName[40];
    memset(modelName,'\0',sizeof(modelName));
    uint8_t modelNameLength = sizeof(modelName);
#else
#define CONTENT_NUM  3 //内容数量
#endif  //UTE_MODULE_MODEL_NUMBER_SUPPORT
    compo_textbox_t * txt;
    char davName[40];
    memset(davName,'\0',sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
#if UTE_MODULE_LOCAL_BLE_NAME_SUPPORT
    memcpy(davName,UTE_DEV_LOCAL_BLE_NAME,sizeof(UTE_DEV_LOCAL_BLE_NAME));
#else
    uteModulePlatformGetDevName((uint8_t*)davName,&davNameLength);//获取设备名称
#endif

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

#if UTE_MODULE_MODEL_NUMBER_SUPPORT
    memcpy(modelName,UTE_MODULE_MODEL_NUMBER,sizeof(UTE_MODULE_MODEL_NUMBER));
    //设备型号
    txt = compo_textbox_create(frm,strlen(i18n[STR_DEV_MODEL]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING*(CONTENT_NUM-4),220,34);
    compo_textbox_set(txt, i18n[STR_DEV_MODEL]);

    txt = compo_textbox_create(frm,strlen(modelName));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING*(CONTENT_NUM-4),220,34);
    compo_textbox_set(txt, modelName);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));
#endif // UTE_MODULE_MODEL_NUMBER_SUPPORT

    //设备名称
    txt = compo_textbox_create(frm,strlen(i18n[STR_DEV_NEME]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING*(CONTENT_NUM-3),220,34);
    compo_textbox_set(txt, i18n[STR_DEV_NEME]);

    txt = compo_textbox_create(frm,strlen(davName));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING*(CONTENT_NUM-3),220,34);
    compo_textbox_set(txt, davName);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //系统版本
    txt = compo_textbox_create(frm,strlen(i18n[STR_SYS_VERSION]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING*(CONTENT_NUM-2),220,34);
    compo_textbox_set(txt, i18n[STR_SYS_VERSION]);

    txt = compo_textbox_create(frm,strlen(UTE_SW_VERSION));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING*(CONTENT_NUM-2),220,34);
    compo_textbox_set(txt, UTE_SW_VERSION);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙地址
    txt = compo_textbox_create(frm,strlen(i18n[STR_BLE_MAC]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,98+SPACING*(CONTENT_NUM-1),220,34);
    compo_textbox_set(txt, i18n[STR_BLE_MAC]);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,134+SPACING*(CONTENT_NUM-1),220,34);
    compo_textbox_set(txt, (char*)Ble_Address_str_buf);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

//关于页面
compo_form_t *func_set_sub_about_form_create(void)
{
#define SPACING  (50)
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
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X, 28/2+50, 200, 34);
    compo_textbox_set(txt, i18n[STR_DEV_NEME]);

    txt = compo_textbox_create(frm,strlen(davName));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X, 22/2+80, 200, 34);
    compo_textbox_set(txt, davName);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //系统版本
    txt = compo_textbox_create(frm,strlen(i18n[STR_SYS_VERSION]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X, 28/2+50+SPACING, 200, 34);
    compo_textbox_set(txt, i18n[STR_SYS_VERSION]);

    txt = compo_textbox_create(frm,strlen(UTE_SW_VERSION));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X, 22/2+80+SPACING, 200, 34);
    compo_textbox_set(txt, UTE_SW_VERSION);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙地址
    txt = compo_textbox_create(frm,strlen(i18n[STR_BLE_MAC]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X, 28/2+50+SPACING*2, 200, 34);
    compo_textbox_set(txt, i18n[STR_BLE_MAC]);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X, 22/2+80+SPACING*2, 200, 34);
    compo_textbox_set(txt, (char*)Ble_Address_str_buf);
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
    f_about_t *f_about = (f_about_t *)func_cb.f_cb;

    if(sys_cb.power_on_state==false)
    {
        switch (msg)
        {
#if UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
            case MSG_CTP_TOUCH:
                if(f_about->ptm)
                {
                    compo_page_move_touch_handler(f_about->ptm);
                }
                break;
#endif  // UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
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
#if UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
            case MSG_CTP_TOUCH:
                if(f_about->ptm)
                {
                    compo_page_move_touch_handler(f_about->ptm);
                }
                break;
#endif  // UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
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

static void func_set_sub_about_process(void)
{
#if UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
    f_about_t *f_about = (f_about_t *)func_cb.f_cb;

    if (f_about->ptm)
    {
        compo_page_move_process(f_about->ptm);
    }
#endif  // UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
    func_process();
}
//进入关于功能
static void func_set_sub_about_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_about_t));
    func_cb.frm_main = func_set_sub_about_form_create();
#if UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
    f_about_t *f_about = (f_about_t *)func_cb.f_cb;
    f_about->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size =  406,
        .page_count = 1,
        .quick_jump_perc =10,
    };
    compo_page_move_init(f_about->ptm, func_cb.frm_main->page_body, &info);
#endif  // UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
}

//退出关于功能
static void func_set_sub_about_exit(void)
{
    f_about_t *f_about = (f_about_t *)func_cb.f_cb;
    func_cb.last = FUNC_SET_SUB_ABOUT;
#if UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
    if (f_about->ptm)
    {
        func_free(f_about->ptm);
    }
#endif  // UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT
}

//关于功能
void func_set_sub_about(void)
{
    printf("%s\n", __func__);
    func_set_sub_about_enter();
    while (func_cb.sta == FUNC_SET_SUB_ABOUT)
    {
        // func_process();
        func_set_sub_about_process();
        func_set_sub_about_message(msg_dequeue());
    }
    func_set_sub_about_exit();
}
