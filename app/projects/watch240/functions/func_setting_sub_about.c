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
    u32 tick;
    u8 click_cnt;
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

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
#define SHAPE_HEIGTH  GUI_SCREEN_HEIGHT/4.5

#define UTE_MODULE_SETTING_SUB_ABOUT_LONG_PAGE_SUPPORT 1

enum
{
    COMPO_ID_QR = 1,
};

bool oraimoWatchAppBindingGetQRcodeData1(char *getQRcodeLink, uint8_t *maxSizeQrCodeLink)
{
    if (maxSizeQrCodeLink == NULL || getQRcodeLink == NULL || *maxSizeQrCodeLink == 0)
    {
        return false;
    }
    uint8_t qrCodeLinkSize = *maxSizeQrCodeLink;

    int written = snprintf(getQRcodeLink, qrCodeLinkSize, "%s",
                           UTE_BINDING_QRENCODE_LINK);
    *maxSizeQrCodeLink = written;

    // 检查是否写入成功且未被截断
    if (written < 0 || written >= qrCodeLinkSize)
    {
        getQRcodeLink[0] = '\0'; // 清空无效内容
        return false;
    }
    return true;
}

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
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2-GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm, strlen(davName));
    compo_textbox_set(txt, davName);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 - GUI_SCREEN_CENTER_Y/1.8);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙名称
    memset(davName,'\0',sizeof(davName));
    davNameLength = sizeof(davName);
    uteModulePlatformGetDevCompleteName((uint8_t*)davName,&davNameLength);//获取蓝牙名称
    printf("%s\n", davName);
    txt = compo_textbox_create(frm, strlen(i18n[STR_BT_NAME]));
    compo_textbox_set(txt, i18n[STR_BT_NAME]);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2);

    txt = compo_textbox_create(frm, strlen(davName));
    compo_textbox_set(txt, davName);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_autoroll_mode(txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1, 320, 40);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //设备型号
    txt = compo_textbox_create(frm, strlen(i18n[STR_SOS_SBXH]));
    compo_textbox_set(txt, i18n[STR_SOS_SBXH]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2 + GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm,strlen(UTE_CUSTOM_DEVICE_MODEL));
    compo_textbox_set(txt, UTE_CUSTOM_DEVICE_MODEL);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 + GUI_SCREEN_CENTER_Y/1.8);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //设备版本
    txt = compo_textbox_create(frm,strlen(i18n[STR_DEV_VERSION]));
    compo_textbox_set(txt, i18n[STR_DEV_VERSION]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2 + GUI_SCREEN_CENTER_Y/1.8*2);

    txt = compo_textbox_create(frm,strlen(UTE_SW_CUSTOM_VERSION));
    compo_textbox_set(txt, UTE_SW_CUSTOM_VERSION);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 + GUI_SCREEN_CENTER_Y/1.8*2);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    //蓝牙地址
    txt = compo_textbox_create(frm,strlen(i18n[STR_BT_ADDR]));
    compo_textbox_set(txt, i18n[STR_BT_ADDR]);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 - SHAPE_HEIGTH/2.2 + GUI_SCREEN_CENTER_Y/1.8*3);

    txt = compo_textbox_create(frm,strlen(Ble_Address_str_buf));
    compo_textbox_set(txt, (char*)Ble_Address_str_buf);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/8, GUI_SCREEN_CENTER_Y*1.1 + GUI_SCREEN_CENTER_Y/1.8*3);
    compo_textbox_set_forecolor(txt, make_color(0x80,0x80,0x80));

    ///创建页码点
    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I341001_1_START_POINT_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 425 + GUI_SCREEN_CENTER_Y/1.8*2);
    compo_picturebox_cut(picbox, 1, 2);
    compo_picturebox_set_visible(picbox, (func_cb.last == FUNC_POWER_ON_SCAN) ? true : false);

    uint8_t maxSizeQrCodeLink = 140;
    char *qr_str = (char *)uteModulePlatformMemoryAlloc(maxSizeQrCodeLink);
    oraimoWatchAppBindingGetQRcodeData1(qr_str,&maxSizeQrCodeLink);
    compo_qrcodebox_t *qrbox = compo_qrcodebox_create(frm, QRCODE_TYPE_2D, maxSizeQrCodeLink);
    compo_qrcodebox_set(qrbox, qr_str);
    compo_qrcodebox_set_bitwid_by_qrwid(qrbox, 290);
    uteModulePlatformMemoryFree(qr_str);
    compo_qrcodebox_set_pos(qrbox, GUI_SCREEN_CENTER_X, 554 + 170 + GUI_SCREEN_CENTER_Y/1.8);
    widget_rect_set_radius(qrbox->bg, 22);
    compo_setid(qrbox, COMPO_ID_QR);
    if (func_cb.last == FUNC_POWER_ON_SCAN)
    {
        widget_set_visible(qrbox->bg, false);
        widget_set_visible(qrbox->qrcode, false);
    }

    return frm;
}

void func_set_sub_about_dis(void)
{
    f_about_t *f_about = (f_about_t *)func_cb.f_cb;
    compo_qrcodebox_t *qrbox = compo_getobj_byid(COMPO_ID_QR);
    compo_qrcodebox_set_pos(qrbox, GUI_SCREEN_CENTER_X, 554 + 170 + GUI_SCREEN_CENTER_Y/1.8 + f_about->ptm->move_offset);
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

    if((func_cb.last == FUNC_POWER_ON_SCAN))
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
#endif

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            case MSG_CTP_CLICK:
                f_about->tick = tick_get();
                if (++f_about->click_cnt == 5)
                {
                    task_stack_init();  //任务堆栈
                    latest_task_init(); //最近任务
                    func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
                    //sys_cb.power_on_state = true;
                }
                break;
            case MSG_CTP_SHORT_RIGHT:
                func_switch_to(FUNC_POWER_ON_SCAN, FUNC_SWITCH_LR_ZOOM_RIGHT | FUNC_SWITCH_AUTO);
                break;
#else
            case MSG_CTP_SHORT_DOWN:
                uteTaskGuiStartScreen(FUNC_POWER_ON_SCAN, FUNC_SWITCH_UD_ZOOM_DOWN | FUNC_SWITCH_AUTO,__func__);
                break;
#endif
            case KL_BACK:
                uteTaskGuiStartScreen(FUNC_LONG_PRESS, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO,__func__);
                break;
            case MSG_CTP_LONG:
                if (!uteApplicationCommonIsHasConnectOurApp())
                {
                    uteTaskGuiStartScreen(FUNC_TEST_MODE, 0, __func__);
                }
                break;
            case MSG_SYS_500MS:
                if (uteApplicationCommonIsHasConnectOurApp())
                {
                    task_stack_init();  // 任务堆栈
                    latest_task_init(); // 最近任务
                    func_switch_to(FUNC_CLOCK, FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
                    // sys_cb.power_on_state = true;
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
#endif
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

    if ((func_cb.last == FUNC_POWER_ON_SCAN) && tick_check_expire(f_about->tick, 5000))
    {
        f_about->click_cnt = 0;
    }

    if (f_about->ptm)
    {
        compo_page_move_process(f_about->ptm);
    }
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    func_set_sub_about_dis();
#endif
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
        .page_size =  (func_cb.last == FUNC_POWER_ON_SCAN) ? 638 : 992,
        .page_count = 1,
        .quick_jump_perc = 10,
    };
    compo_page_move_init(f_about->ptm, func_cb.frm_main->page_body, &info);
    f_about->click_cnt = 0;
#endif
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
#endif
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
