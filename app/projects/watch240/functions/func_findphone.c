#include "include.h"
#include "func.h"
#include "ute_module_findphone.h"
#include "func_cover.h"
#include "ute_module_call.h"
#include "ute_drv_motor.h"
#include "ute_application_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_FIND_PHNOE_SUPPORT

#define FINDPHONE_POS_Y_MAX        170// 240
#define FINDPHONE_POS_Y_MIN        108// 140

typedef struct f_findphone_t_
{
    s16 angle;
} f_findphone_t;

enum
{
    COMPO_ID_TEXT_FIND = 1,
    COMPO_ID_BUTTON_FIND,
    COMPO_ID_PIC_FIND_BG,
    COMPO_ID_PIC_PHONE,
    COMPO_ID_TEXT_BT_UNCONNECT,
    COMPO_ID_BLE_NOT,
    COMPO_ID_TITLE,
    COMPO_ID_TITLE_T,
    COMPO_ID_PIC_GIF,

};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I330001_FINDPHONE_BG_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 129);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_THEME_1_FINDPHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 129);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 247);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 247);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            if(bt_a2dp_profile_completely_connected())
            {
                // bt_a2dp_profile_dis();
            }
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            // bt_a2dp_profile_en();
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I332001_FINDPHONE_BG_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 80+86);

    pic = compo_picturebox_create(frm, UI_BUF_I332001_THEME_ICON1_FINDPHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 80+86);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 64/2+266);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 64/2+266);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            if(bt_a2dp_profile_completely_connected())
            {
                bt_a2dp_profile_dis();
            }
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            bt_a2dp_profile_en();
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I340001_FINDPHONE_BG_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 80+86);

    pic = compo_picturebox_create(frm, UI_BUF_I340001_THEME_ICON1_FINDPHONE_BIN);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 80+86);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I340001_PUBLIC_RECTANGLE01_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 64/2+266);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 64/2+266);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I340001_PUBLIC_RECTANGLE01_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            if(bt_a2dp_profile_completely_connected())
            {
                bt_a2dp_profile_dis();
            }
            compo_button_set_bgimg(btn, UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            bt_a2dp_profile_en();
            compo_button_set_bgimg(btn, UI_BUF_I340001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}

#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_1_FIND_MY_PHONE_ICON_PIC58X100_X92_Y83_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 83+100/2);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_1_FIND_MY_PHONE_ICON_PIC58X100_X92_Y83_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 83+100/2);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 247);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_2_FIND_MY_PHONE_ICON_PIC140X100_X50_Y83_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_1_FIND_MY_PHONE_ICON_PIC58X100_X92_Y83_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_1_FIND_MY_PHONE_ICON_PIC58X100_X92_Y83_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_2_FIND_MY_PHONE_ICON_PIC140X100_X50_Y83_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT


//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I342001_27_MORE_FIND_PHONE_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 69+88/2);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I342001_27_MORE_FIND_PHONE_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 69+88/2);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 182+28/2);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I342001_27_MORE_RING_PHONE_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I342001_27_MORE_FIND_PHONE_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            compo_button_set_bgimg(btn, UI_BUF_I342001_27_MORE_FIND_PHONE_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I342001_27_MORE_RING_PHONE_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);
    compo_textbox_set_visible(frm->time, false);
    compo_textbox_set_visible(frm->title, false);
    compo_setid(frm->title, COMPO_ID_TITLE);
    compo_setid(frm->time, COMPO_ID_TITLE_T);

    //按钮上的文本
    compo_textbox_t *txt = compo_textbox_create(frm, MAX(strlen(i18n[STR_RING_STOP]),strlen(i18n[STR_RING_START])));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 50+324);
    compo_textbox_set(txt, i18n[STR_RING_STOP]);
    compo_textbox_set_visible(txt, false);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    //按钮
    compo_button_t *btn;
    btn = compo_button_create(frm);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, 132+154/2, 240, 240);

    compo_picturebox_t *picGif = compo_picturebox_create(frm, UI_BUF_I341001_27_MORE_GIF_BIN);
    compo_picturebox_set_location(picGif, GUI_SCREEN_CENTER_X, 132+154/2, 240, 240);
    compo_picturebox_cut(picGif, 0, 12);
    compo_setid(picGif, COMPO_ID_PIC_GIF);
    compo_picturebox_set_visible(picGif, false);

    compo_picturebox_t *picPhone = compo_picturebox_create(frm, UI_BUF_I341001_27_MORE_RING_PHONE_BIN);
    compo_picturebox_set_pos(picPhone, GUI_SCREEN_CENTER_X, 132+154/2);
    compo_setid(picPhone, COMPO_ID_PIC_PHONE);
    compo_picturebox_set_visible(picPhone, false);

    compo_picturebox_t *picBt = compo_picturebox_create(frm, UI_BUF_I341001_23_SOS_BLUETOOTH_BIN);
    compo_picturebox_set_pos(picBt, GUI_SCREEN_CENTER_X, 120+116/2);
    compo_setid(picBt, COMPO_ID_BLE_NOT);
    compo_picturebox_set_visible(picBt, false);

    compo_textbox_t *txtBt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
    compo_textbox_set_location(txtBt, GUI_SCREEN_CENTER_X, 282+24, GUI_SCREEN_WIDTH / 1.2, widget_text_get_max_height());
    compo_textbox_set(txtBt, i18n[STR_CONNECT_BLUETOOTH]);
    compo_setid(txtBt, COMPO_ID_TEXT_BT_UNCONNECT);
    compo_textbox_set_visible(txtBt, false);

    ute_ble_connect_state_t connectStatus;
    uteApplicationCommonGetBleConnectionState(&connectStatus);
    if (connectStatus.isConnected)
    {
        compo_textbox_set_visible(frm->time, true);
        compo_textbox_set_visible(frm->title, true);
        compo_picturebox_set_visible(picGif, true);
        compo_picturebox_set_visible(picPhone, true);
    }
    else
    {
        compo_picturebox_set_visible(picBt, true);
        compo_textbox_set_visible(txtBt, true);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{
    ute_ble_connect_state_t connectStatus;
    uteApplicationCommonGetBleConnectionState(&connectStatus);
    if (connectStatus.isConnected)
    {
        int id = compo_get_button_id();
        if (COMPO_ID_BUTTON_FIND == id)
        {
            //compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
            compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC_GIF);

            //compo_textbox_set_visible(txt, true);
            if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
            {
                if(bt_a2dp_profile_completely_connected())
                {
                    // bt_a2dp_profile_dis();
                }
                compo_picturebox_cut(picbox, 0, 12);
                uteModuleFindPhoneStartRing();
                //compo_textbox_set(txt, i18n[STR_RING_START]);
            }
            else
            {
                // bt_a2dp_profile_en();
                compo_picturebox_cut(picbox,0,12);
                uteModuleFindPhoneStopRing();
                //compo_textbox_set(txt, i18n[STR_RING_STOP]);
            }
        }
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I341001_27_MORE_RING_PHONE_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 132+154/2);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I341001_27_MORE_RING_PHONE_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 132+154/2);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 42/2+324);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I341001_27_MORE_RING_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I341001_27_MORE_RING_PHONE_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            if(bt_a2dp_profile_completely_connected())
            {
                // bt_a2dp_profile_dis();
            }
            compo_button_set_bgimg(btn, UI_BUF_I341001_27_MORE_RING_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            // bt_a2dp_profile_en();
            compo_button_set_bgimg(btn, UI_BUF_I341001_27_MORE_RING_PHONE_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I343001_27_MORE_RING_PHONE_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 134+114/2);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I343001_27_MORE_RING_PHONE_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 134+114/2);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 42/2+284);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I343001_27_MORE_RING_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I343001_27_MORE_RING_PHONE_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            if(bt_a2dp_profile_completely_connected())
            {
                // bt_a2dp_profile_dis();
            }
            compo_button_set_bgimg(btn, UI_BUF_I343001_27_MORE_RING_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            // bt_a2dp_profile_en();
            compo_button_set_bgimg(btn, UI_BUF_I343001_27_MORE_RING_PHONE_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

//创建查找手机窗体
compo_form_t *func_findphone_form_create(void)
{
    // component_t *compo;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_I338001_27_MORE_FIND_PHONE_BIN);
    compo_setid(pic, COMPO_ID_PIC_FIND_BG);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, 104+134/2);

    //图像按钮
    compo_button_t * btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I338001_27_MORE_FIND_PHONE_BIN);
    compo_setid(btn, COMPO_ID_BUTTON_FIND);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 104+134/2);

    //按钮上的文本
    compo_textbox_t * txt = compo_textbox_create(frm, strlen(i18n[STR_START]));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, 42/2+274);
    compo_textbox_set(txt, i18n[STR_START]);
    compo_setid(txt, COMPO_ID_TEXT_FIND);

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        compo_button_set_bgimg(btn, UI_BUF_I338001_27_MORE_RING_BIN);
        compo_textbox_set(txt, i18n[STR_STOP]);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I338001_27_MORE_FIND_PHONE_BIN);
        compo_textbox_set(txt, i18n[STR_START]);
    }

    return frm;
}
//查找手机按键事件处理
static void func_findphone_button_touch_handle(void)
{

    int id = compo_get_button_id();
    if (COMPO_ID_BUTTON_FIND == id)
    {
        compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
        compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);

        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_STOP)
        {
            if(bt_a2dp_profile_completely_connected())
            {
                // bt_a2dp_profile_dis();
            }
            compo_button_set_bgimg(btn, UI_BUF_I338001_27_MORE_RING_BIN);
            uteModuleFindPhoneStartRing();
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            // bt_a2dp_profile_en();
            compo_button_set_bgimg(btn, UI_BUF_I338001_27_MORE_FIND_PHONE_BIN);
            uteModuleFindPhoneStopRing();
            compo_textbox_set(txt, i18n[STR_START]);
        }

    }

}
#else
compo_form_t *func_findphone_form_create(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//查找手机功能事件处理
static void func_findphone_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE00_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE00_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            compo_button_set_bgimg(btn, UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I340001_PUBLIC_RECTANGLE00_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I340001_PUBLIC_RECTANGLE01_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            compo_button_set_bgimg(btn, UI_BUF_I340001_PUBLIC_RECTANGLE01_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_2_FIND_MY_PHONE_ICON_PIC140X100_X50_Y83_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_1_FIND_MY_PHONE_ICON_PIC58X100_X92_Y83_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            // compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_3_BLUETOOTH_NOCONNECTED_ICON_BLE_70X74_X86_Y76_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I342001_27_MORE_RING_PHONE_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I342001_27_MORE_FIND_PHONE_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            // compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_3_BLUETOOTH_NOCONNECTED_ICON_BLE_70X74_X86_Y76_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC_GIF);
    compo_picturebox_t *picpho = compo_getobj_byid(COMPO_ID_PIC_PHONE);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    compo_textbox_t *txtTitle = compo_getobj_byid(COMPO_ID_TITLE);
    compo_textbox_t *txtTime = compo_getobj_byid(COMPO_ID_TITLE_T);
    compo_textbox_t *txtBt = compo_getobj_byid(COMPO_ID_TEXT_BT_UNCONNECT);
    compo_picturebox_t *picBt = compo_getobj_byid(COMPO_ID_BLE_NOT);
    static u32 ticks1 = 0;
    static u8 index = 0;
    ute_ble_connect_state_t connectStatus;
    uteApplicationCommonGetBleConnectionState(&connectStatus);
    if (connectStatus.isConnected)
    {
        compo_textbox_set_visible(txt, true);
        compo_textbox_set_visible(txtTime, true);
        compo_textbox_set_visible(txtTitle, true);
        compo_picturebox_set_visible(picbox, true);
        compo_picturebox_set_visible(picpho, true);
        compo_picturebox_set_visible(picBt, false);
        compo_textbox_set_visible(txtBt, false);
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();

            if (tick_check_expire(ticks1, 100))
            {
                ticks1 = tick_get();
                index++;
                index = index > 11 ? 0 : index;
                compo_picturebox_cut(picbox,index,12);
            }

            compo_textbox_set(txt, i18n[STR_RING_START]);
        }
        else
        {
            index = 0;
            compo_picturebox_cut(picbox,index,12);

            compo_textbox_set(txt, i18n[STR_RING_STOP]);
        }
    }
    else
    {
        compo_textbox_set_visible(txt, false);
        compo_textbox_set_visible(txtTime, false);
        compo_textbox_set_visible(txtTitle, false);
        compo_picturebox_set_visible(picbox, false);
        compo_picturebox_set_visible(picpho, false);
        compo_picturebox_set_visible(picBt, true);
        compo_textbox_set_visible(txtBt, true);
//        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
//        {
//            index = 0;
//            compo_picturebox_cut(picbox,index,12);
//            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
//            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
//            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
//            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
//            compo_textbox_set_visible(txt, false);
//        }
    }
#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I341001_27_MORE_RING_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I341001_27_MORE_RING_PHONE_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            // compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_3_BLUETOOTH_NOCONNECTED_ICON_BLE_70X74_X86_Y76_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I338001_27_MORE_RING_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I338001_27_MORE_FIND_PHONE_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            // compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_3_BLUETOOTH_NOCONNECTED_ICON_BLE_70X74_X86_Y76_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

    compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC_FIND_BG);
    compo_button_t * btn = compo_getobj_byid(COMPO_ID_BUTTON_FIND);
    compo_textbox_t *txt = compo_getobj_byid(COMPO_ID_TEXT_FIND);
    f_findphone_t *f_findphone = (f_findphone_t *)func_cb.f_cb;
    static u32 ticks = 0;

    if (ble_is_connect())
    {
        if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            if (sys_cb.gui_sleep_sta)
            {
                sys_cb.gui_need_wakeup = 1;
            }
            reset_sleep_delay_all();
            if (tick_check_expire(ticks, 50))
            {
                ticks = tick_get();

                compo_picturebox_set_rotation(pic,f_findphone->angle*10);
                if(++f_findphone->angle == 360) f_findphone->angle = 0;
            }
            compo_button_set_bgimg(btn, UI_BUF_I343001_27_MORE_RING_BIN);
            compo_textbox_set(txt, i18n[STR_STOP]);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I343001_27_MORE_RING_PHONE_BIN);
            compo_textbox_set(txt, i18n[STR_START]);
        }
    }
    else
    {
        if (uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
        {
            // compo_button_set_bgimg(btn, UI_BUF_I335001_27_MORE_4_FIND_MY_PHONE_3_BLUETOOTH_NOCONNECTED_ICON_BLE_70X74_X86_Y76_BIN);
            uteModuleFindPhoneSetStatus(FIND_PHONE_STOP);
            compo_textbox_set(txt, i18n[STR_START]);
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}

//查找手机功能消息处理
static void func_findphone_message(size_msg_t msg)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            if (!ble_is_connect())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
            else
            {
                func_findphone_button_touch_handle();
            }
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT ||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            if (!ble_is_connect())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
            else
            {
                func_findphone_button_touch_handle();
            }
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            if (!ble_is_connect())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
            else
            {
                func_findphone_button_touch_handle();
            }
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            func_findphone_button_touch_handle();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            if (!ble_is_connect())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
            else
            {
                func_findphone_button_touch_handle();
            }
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
            if (!ble_is_connect())
            {
                uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
                msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
            }
            else
            {
                func_findphone_button_touch_handle();
            }
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//进入查找手机功能
static void func_findphone_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_findphone_t));
    func_cb.frm_main = func_findphone_form_create();
    uteModuleFindPhoneStartRing();
}

//退出查找手机功能
static void func_findphone_exit(void)
{
    func_cb.last = FUNC_FINDPHONE;

    if(uteModuleFindPhoneGetStatus() == FIND_PHONE_RING)
    {
        uteModuleFindPhoneStopRing();
    }
}

//查找手机功能
void func_findphone(void)
{
    printf("%s\n", __func__);
    func_findphone_enter();
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
    bool EntertranmentVoiceOn = uteModuleCallIsEntertranmentVoiceOn();
    if(bt_a2dp_profile_completely_connected() && EntertranmentVoiceOn)
    {
        uteModuleCallChangeEntertranmentVoiceSwitchStatus();
    }
#endif
    while (func_cb.sta == FUNC_FINDPHONE)
    {
        func_findphone_process();
        func_findphone_message(msg_dequeue());
    }
    func_findphone_exit();
#if UTE_MODULE_BT_ENTERTRANMENT_VOICE_SWITCH_SUPPORT
    if(EntertranmentVoiceOn && !uteModuleCallIsEntertranmentVoiceOn())
    {
        uteModuleCallChangeEntertranmentVoiceSwitchStatus();
    }
#endif
}

#endif
