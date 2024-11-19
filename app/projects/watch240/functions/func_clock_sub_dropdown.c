#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_notdisturb.h"
#include "ute_module_liftwrist.h"
#include "ute_application_common.h"

#define MENU_DROPDOWN_ALPHA             200

static uint8_t dropdown_disturb_sw;     //功能未做，加个开关先放着

enum
{
    COMPO_ID_BTN_START = FUNC_MAX_NUM, //从任务最大枚举开始，避免和表盘跳转et的d冲突
    //按键
    COMPO_ID_BTN_CONNECT,       //蓝牙连接开关
    COMPO_ID_BTN_DISCURD,       //勿扰模式开关
    COMPO_ID_BTN_MUTE,          //静音模式开关
    COMPO_ID_BTN_FLASHLIGHT,    //手电筒
    COMPO_ID_BTN_SCAN,          //二维码
    COMPO_ID_BTN_LIGHT,         //亮度调节

    //电池百分比文本
    COMPO_ID_TXT_BATTERY_PERCENT,
    //电池百分比图片
    COMPO_ID_TXT_BATTERY_PIC,
    //蓝牙状态图片
    COMPO_ID_TXT_BLUETOOTH_STA_PIC,
    COMPO_ID_TXT_BTETOOTH_STA_PIC,
};


#define DROPDOWN_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_dropdown_disp_btn_item) / sizeof(tbl_dropdown_disp_btn_item[0])))

typedef struct dropdown_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    s16 x;
    s16 y;
    u8 light_flag;

} dropdown_disp_btn_item_t;

//类型有显示信息，控制开关，点击跳转任务
//按钮item，创建时遍历一下
static  dropdown_disp_btn_item_t tbl_dropdown_disp_btn_item[] =
{
//    {UI_BUF_DROPDOWN_BLUETOOTH_OFF_BIN,          COMPO_ID_BTN_CONNECT,          60,  80},     //蓝牙扫描开关
//    {UI_BUF_DROPDOWN_DISTURB_OFF_BIN,            COMPO_ID_BTN_DISCURD,          180, 80},
//    {UI_BUF_DROPDOWN_FLASHLIGHT_ON_BIN,          COMPO_ID_BTN_FLASHLIGHT,       60,  155},
//    {UI_BUF_DROPDOWN_MUTE_ON_BIN,                COMPO_ID_BTN_MUTE,             180, 155},
//    {UI_BUF_DROPDOWN_SCAN_2_BIN,                 COMPO_ID_BTN_SCAN,             60,  230},
//    {UI_BUF_DROPDOWN_LIGHT_1_BIN,                COMPO_ID_BTN_LIGHT,            180, 230},

    {UI_BUF_CLOCK_DOWN_MENU_BT_OFF_BIN,          COMPO_ID_BTN_CONNECT,          60,  80},     //蓝牙扫描开关
    {UI_BUF_CLOCK_DOWN_MENU_DND_OFF_BIN,         COMPO_ID_BTN_DISCURD,          180, 80},
    {UI_BUF_CLOCK_DOWN_MENU_TOUCH_BIN,           COMPO_ID_BTN_FLASHLIGHT,       60,  155},
    {UI_BUF_CLOCK_DOWN_MENU_MUTE_ON_BIN,         COMPO_ID_BTN_MUTE,             180, 155},
    {UI_BUF_CLOCK_DOWN_MENU_SETTING_BIN,         COMPO_ID_BTN_SCAN,             60,  230},
    {UI_BUF_CLOCK_DOWN_MENU_LIGHT_1_BIN,         COMPO_ID_BTN_LIGHT,            180, 230},
};

//下拉电量图标更新
static void func_clock_sub_dropdown_battery_pic_update(void)
{
    compo_picturebox_t *battery_pic = compo_getobj_byid(COMPO_ID_TXT_BATTERY_PIC);
    switch(sys_cb.vbat_percent)
    {
        case 0 ... 14:
            compo_picturebox_set(battery_pic, UI_BUF_DROPDOWN_BATTERY_0_BIN);
            break;
        case 15 ... 29:
            compo_picturebox_set(battery_pic, UI_BUF_DROPDOWN_BATTERY_1_BIN);
            break;
        case 30 ... 44:
            compo_picturebox_set(battery_pic, UI_BUF_DROPDOWN_BATTERY_2_BIN);
            break;
        case 45 ... 59:
            compo_picturebox_set(battery_pic, UI_BUF_DROPDOWN_BATTERY_3_BIN);
            break;
        case 60 ... 74:
            compo_picturebox_set(battery_pic, UI_BUF_DROPDOWN_BATTERY_4_BIN);
            break;
        case 75 ... 84:
            compo_picturebox_set(battery_pic, UI_BUF_DROPDOWN_BATTERY_5_BIN);
            break;
        case 85 ... 100:
            compo_picturebox_set(battery_pic, UI_BUF_DROPDOWN_BATTERY_6_BIN);
            break;
        default:
            compo_picturebox_set(battery_pic, UI_BUF_DROPDOWN_POWER6_BIN);
            break;
    }
}

//下拉蓝牙连接标志更新
static void func_clock_sub_dropdown_bluetooth_pic_update(void)
{
    compo_picturebox_t *bluetooth_pic = compo_getobj_byid(COMPO_ID_TXT_BLUETOOTH_STA_PIC);
    compo_picturebox_t *btooth_pic = compo_getobj_byid(COMPO_ID_TXT_BTETOOTH_STA_PIC);
//    compo_picturebox_set_visible(bluetooth_pic, true);
    if (ble_is_connect())
    {
        compo_picturebox_set(bluetooth_pic, UI_BUF_CLOCK_DOWN_MENU_BLE_ICON_ON_BIN);
//        printf("w:%d h:%d\n",gui_image_get_size(UI_BUF_DROPDOWN_BLUETOOTH_CONNECT_ON_BIN).wid,gui_image_get_size(UI_BUF_DROPDOWN_BLUETOOTH_CONNECT_ON_BIN).hei);
    }
    else
    {
        compo_picturebox_set(bluetooth_pic, UI_BUF_CLOCK_DOWN_MENU_BLE_ICON_OFF_BIN);
    }

    if(bt_is_connected())
    {
        compo_picturebox_set(btooth_pic, UI_BUF_CLOCK_DOWN_MENU_BT_ICON_ON_BIN);
    }
    else
    {
        compo_picturebox_set(btooth_pic, UI_BUF_CLOCK_DOWN_MENU_BT_ICON_OFF_BIN);
    }

}

//下拉静音图标更新
static void func_clock_sub_dropdown_mute_pic_update(void)
{
    compo_button_t *mute_pic = compo_getobj_byid(COMPO_ID_BTN_MUTE);
    if(sys_cb.mute)
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_CLOCK_DOWN_MENU_MUTE_ON_BIN);
    }
    else
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_CLOCK_DOWN_MENU_MUTE_OFF_BIN);
    }
}

//下拉蓝牙按钮更新
static void func_clock_sub_dropdown_bluetooth_btn_pic_update(void)
{
    compo_button_t *bluetooth_pic = compo_getobj_byid(COMPO_ID_BTN_CONNECT);
    compo_picturebox_t *bluetooth = compo_getobj_byid(COMPO_ID_TXT_BTETOOTH_STA_PIC);
    printf("bt_get_scan: 0x%x\n", bt_get_scan());
    if(bt_get_scan())
    {
        compo_button_set_bgimg(bluetooth_pic, UI_BUF_CLOCK_DOWN_MENU_BT_ON_BIN);
        compo_picturebox_set_visible(bluetooth, true);
    }
    else
    {
        compo_button_set_bgimg(bluetooth_pic, UI_BUF_CLOCK_DOWN_MENU_BT_OFF_BIN);
        compo_picturebox_set_visible(bluetooth, false);

    }
}

//下拉勿扰按钮更新
static void func_clock_sub_dropdown_disturb_pic_update(void)
{
    compo_button_t *disturb_pic = compo_getobj_byid(COMPO_ID_BTN_DISCURD);

    if(dropdown_disturb_sw)
    {
        sys_cb.disturd_adl = true;
        compo_button_set_bgimg(disturb_pic, UI_BUF_CLOCK_DOWN_MENU_DND_ON_BIN);
        uteModuleNotDisturbSetOpenStatus(sys_cb.disturd_adl);
    }
    else
    {
        sys_cb.disturd_adl = false;
        compo_button_set_bgimg(disturb_pic, UI_BUF_CLOCK_DOWN_MENU_DND_OFF_BIN);
        uteModuleNotDisturbSetOpenStatus(sys_cb.disturd_adl);
    }

}


//创建下拉菜单
static void func_clock_sub_dropdown_form_create(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if(quick.isNotDisturb)
    {
        dropdown_disturb_sw = 1;
    }
    else
    {
        dropdown_disturb_sw = 0;
    }
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
    if(uteModuleNotDisturbIsOpenScheduled())
    {
        if(uteModuleNotDisturbIsTimeBucket())
        {
            dropdown_disturb_sw = 1;
        }
        else
        {
            dropdown_disturb_sw = 0;
        }
    }
#endif

    compo_form_t *frm = compo_form_create(true);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    //创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < DROPDOWN_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn = compo_button_create_by_image(frm, tbl_dropdown_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn, tbl_dropdown_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_dropdown_disp_btn_item[idx_btn].x, tbl_dropdown_disp_btn_item[idx_btn].y);
        compo_button_set_alpha(btn, MENU_DROPDOWN_ALPHA);
        if(idx_btn == DROPDOWN_DISP_BTN_ITEM_CNT-1)
        {
            switch(sys_cb.light_level)
            {
                case 1:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_1_BIN);
                    break;
                case 2:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_2_BIN);
                    break;
                case 3:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_3_BIN);
                    break;
                case 4:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_4_BIN);
                    break;
                case 5:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_5_BIN);
                    break;
            }
        }
    }

    //电池
    compo_picturebox_t *battery_pic = compo_picturebox_create(frm, UI_BUF_DROPDOWN_BATTERY_0_BIN);
    compo_setid(battery_pic, COMPO_ID_TXT_BATTERY_PIC);
    compo_picturebox_set_pos(battery_pic, GUI_SCREEN_CENTER_X*1.6, GUI_SCREEN_CENTER_Y/8);
    compo_picturebox_set_visible(battery_pic, true);

    //蓝牙状态
    compo_picturebox_t *bluetooth_pic = compo_picturebox_create(frm, UI_BUF_DROPDOWN_BLUETOOTH_CONNECT_OFF_BIN);
    compo_setid(bluetooth_pic, COMPO_ID_TXT_BLUETOOTH_STA_PIC);
    compo_picturebox_set_pos(bluetooth_pic, GUI_SCREEN_CENTER_X/3.2, GUI_SCREEN_CENTER_Y/8);
//    compo_picturebox_set(bluetooth_pic, UI_BUF_DROPDOWN_BLUETOOTH_CONNECT_ON_BIN);
    compo_picturebox_set(bluetooth_pic, UI_BUF_CLOCK_DOWN_MENU_BT_ICON_OFF_BIN);
//    compo_picturebox_set_visible(bluetooth_pic, true);

    bluetooth_pic = compo_picturebox_create(frm, UI_BUF_DROPDOWN_BLUETOOTH_CONNECT_OFF_BIN);
    compo_setid(bluetooth_pic, COMPO_ID_TXT_BTETOOTH_STA_PIC);
    compo_picturebox_set_pos(bluetooth_pic, GUI_SCREEN_CENTER_X/1.7, GUI_SCREEN_CENTER_Y/8);
    compo_picturebox_set(bluetooth_pic, UI_BUF_CLOCK_DOWN_MENU_BLE_ICON_OFF_BIN);
//    compo_picturebox_set_visible(bluetooth_pic, true);


    //电池电量
//    compo_textbox_t *battery_txt = compo_textbox_create(frm, 4);
//    compo_textbox_set_location(battery_txt, 200, 28, 0, 0);
//    compo_textbox_set_autosize(battery_txt, true);
//    compo_bonddata(battery_txt, COMPO_BOND_BATTERY);
//    compo_textbox_set_font(battery_txt, UI_BUF_0FONT_FONT_BIN);
//    compo_textbox_set_forecolor(battery_txt, make_color(0xae, 0xb3, 0xbc));     //和电池底图颜色一致

    func_clock_sub_dropdown_battery_pic_update();
    func_clock_sub_dropdown_bluetooth_pic_update();     //蓝牙更新
    func_clock_sub_dropdown_bluetooth_btn_pic_update();
    func_clock_sub_dropdown_mute_pic_update();          //静音更新
    func_clock_sub_dropdown_disturb_pic_update();       //勿扰

    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sub_frm = frm;
    f_clk->masklayer = masklayer;
}

//时钟表盘主要事件流程处理
static void func_clock_sub_dropdown_process(void)
{
    func_clock_sub_process();
}

static void func_clock_sub_dropdown_click_handler(void)
{
    int id = compo_get_button_id();
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_LIGHT);
    printf("id: %d\n", id);
    switch(id)
    {
        case COMPO_ID_BTN_CONNECT:
            if(bt_get_scan())
            {
                if (bt_is_connected())
                {
                    bt_disconnect(0);
                }
                bt_scan_disable();
            }
            else
            {
                bt_scan_enable();
                bt_connect();
            }
            printf("bt_get_scan: %d\n", bt_get_scan());
            func_clock_sub_dropdown_bluetooth_btn_pic_update();
            break;
        case COMPO_ID_BTN_DISCURD:
        {
#if UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
            ute_quick_switch_t quick;
            uteApplicationCommonGetQuickSwitchStatus(&quick);
            quick.isNotDisturb = !quick.isNotDisturb;
            if(quick.isNotDisturb)
            {
                uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_ALLDAY_OPEN);
                uteModuleLiftWristRecordOpenStatus();
                if(uteModuleLiftWristGetOldOpenStatus())
                {
                    uteModuleLiftWristSetOpenStatus(false);
                }
                else if(uteModuleLiftWristGetOldScheduledOpenStatus())
                {
                    uteModuleLiftWristSetScheduled(false);
                }
                if(uteModuleNotDisturbIsOpenScheduled())
                {
                    uteModuleNotDisturbSetScheduled(false);
                }
            }
            else
            {
                uteModuleNotDisturbSetOpenStatus(NOT_DISTURB_CLOSE);
                if(uteModuleLiftWristGetOldOpenStatus())
                {
                    uteModuleLiftWristSetOpenStatus(true);
                }
                else if(uteModuleLiftWristGetOldScheduledOpenStatus())
                {
                    uteModuleLiftWristSetScheduled(true);
                }
            }
            uteApplicationCommonSetQuickSwitchStatus(&quick);
#else
            ute_module_not_disturb_data_t param;
            uteModuleNotDisturbGetParam(&param);
            param.isOpen = !param.isOpen;
            uteModuleNotDisturbSaveParam(param);

            ute_quick_switch_t quick;
            uteApplicationCommonGetQuickSwitchStatus(&quick);
            quick.isNotDisturb = !quick.isNotDisturb;
            uteApplicationCommonSetQuickSwitchStatus(&quick);
#endif
            uteApplicationCommonSendQuickSwitchStatus();
            dropdown_disturb_sw = quick.isNotDisturb;
            func_clock_sub_dropdown_disturb_pic_update();
        }
        break;
        case COMPO_ID_BTN_MUTE:
            if(sys_cb.mute)
            {
                bsp_sys_unmute();
            }
            else
            {
                bsp_sys_mute();
            }
            func_clock_sub_dropdown_mute_pic_update();          //静音更新
            break;
        //点击任务跳转
        case COMPO_ID_BTN_FLASHLIGHT:
            func_cb.sta = FUNC_FLASHLIGHT;
            break;
        case COMPO_ID_BTN_SCAN:
            //func_cb.sta = FUNC_SCAN;
            func_cb.sta = FUNC_SETTING;
            break;
        case COMPO_ID_BTN_LIGHT:
//            func_cb.sta = FUNC_LIGHT;
            if(++sys_cb.light_level>5)sys_cb.light_level=1;
            switch(sys_cb.light_level)
            {
                case 1:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_1_BIN);
                    break;
                case 2:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_2_BIN);
                    break;
                case 3:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_3_BIN);
                    break;
                case 4:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_4_BIN);
                    break;
                case 5:
                    compo_button_set_bgimg(btn, UI_BUF_CLOCK_DOWN_MENU_LIGHT_5_BIN);
                    break;
            }
            tft_bglight_set_level(sys_cb.light_level,false);
            break;

        default:
            break;
    }
}

//时钟表盘下拉菜单功能消息处理
static void func_clock_sub_dropdown_message(size_msg_t msg)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_clock_sub_dropdown_click_handler();
            break;
        case MSG_CTP_SHORT_LEFT:
            break;
        case MSG_CTP_SHORT_RIGHT:
            break;
        case MSG_CTP_SHORT_UP:
            if (func_switching(FUNC_SWITCH_MENU_DROPDOWN_UP, NULL))
            {
                f_clk->sta = FUNC_CLOCK_MAIN;                   //上滑返回到时钟主界面
            }
            break;

        case KU_BACK:
            func_switching(FUNC_SWITCH_MENU_DROPDOWN_UP | FUNC_SWITCH_AUTO, NULL);
            f_clk->sta = FUNC_CLOCK_MAIN;                       //单击BACK键返回到时钟主界面
            break;
        case MSG_SYS_1S:
            func_clock_sub_dropdown_battery_pic_update();       //电量更新
            func_clock_sub_dropdown_bluetooth_pic_update();     //蓝牙更新
            func_clock_sub_dropdown_mute_pic_update();          //静音更新
            break;
        case MSG_QDEC_BACKWARD:
            printf("MSG_QDEC_BACKWARD\n");
            break;

        case MSG_QDEC_FORWARD:
            printf("MSG_QDEC_FORWARD\n");
            break;

        case EVT_CLOCK_DROPDOWN_EXIT:
            f_clk->sta = FUNC_CLOCK_MAIN;                       //返回到时钟主界面
            break;

        default:
            func_clock_sub_message(msg);
            break;
    }
}

//时钟表盘下拉菜单进入处理
static void func_clock_sub_dropdown_enter(void)
{
    if(uteModuleNotDisturbGetOpenStatus() == true)
    {
        dropdown_disturb_sw = 1;
    }
    else
    {
        dropdown_disturb_sw = 0;
    }

    func_clock_butterfly_set_light_visible(false);
    func_clock_sub_dropdown_form_create();
    if (!func_switching(FUNC_SWITCH_MENU_DROPDOWN_DOWN, NULL))
    {
        return;                                             //下拉到一半取消
    }
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sta = FUNC_CLOCK_SUB_DROPDOWN;                   //进入到下拉菜单
}

//时钟表盘下拉菜单退出处理
static void func_clock_sub_dropdown_exit(void)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    compo_form_destroy(f_clk->sub_frm);
    func_clock_butterfly_set_light_visible(true);
    f_clk->sub_frm = NULL;
}

//时钟表盘下拉菜单
void func_clock_sub_dropdown(void)
{
    func_clock_sub_dropdown_enter();
    while (func_cb.sta == FUNC_CLOCK && ((f_clock_t *)func_cb.f_cb)->sta == FUNC_CLOCK_SUB_DROPDOWN)
    {
        func_clock_sub_dropdown_process();
        func_clock_sub_dropdown_message(msg_dequeue());
    }
    func_clock_sub_dropdown_exit();
}
