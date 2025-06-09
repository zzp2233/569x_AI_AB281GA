#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_notdisturb.h"
#include "ute_module_liftwrist.h"
#include "ute_application_common.h"
#include "ute_drv_battery_common.h"
#include "ute_module_sport.h"
#include "func_menu.h"
#include "ute_module_call.h"
#include "ute_module_localRingtone.h"
#include "ute_module_bedside_mode.h"

#if 1

typedef struct f_clock_dropdwon_menu_t_
{
#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE
    page_tp_move_t ptm;
#endif
    u8 page_num;
} f_clock_dropdwon_menu_t;

#ifdef UTE_CLOCK_DWON_PAGE_NUM //页面数量
#define CLOCK_DWON_PAGE_NUM   UTE_CLOCK_DWON_PAGE_NUM
#else
#define CLOCK_DWON_PAGE_NUM   2
#endif

enum
{
    //按键
    COMPO_ID_BTN_CONNECT=1,     //蓝牙连接开关
    COMPO_ID_BTN_MENU,          //菜单
    COMPO_ID_BTN_FLASHLIGHT,    //手电筒
    COMPO_ID_BTN_WIRST,         //抬婉亮屏
    COMPO_ID_BTN_DISCURD,       //勿扰模式开关
    COMPO_ID_BTN_LIGHT,         //亮度调节
    COMPO_ID_BTN_MUTE,          //静音模式开关
    COMPO_ID_BTN_PHONE,         //查找手机
    COMPO_ID_BTN_SETTING,       //设置
    COMPO_ID_BTN_ABOUT,         //关于
    COMPO_ID_BTN_BEDSIDE_MODE,  //床头钟模式开关

    COMPO_ID_PAGE_FIRST,      //底部页码点->初始
    COMPO_ID_PAGE_LAST=COMPO_ID_PAGE_FIRST+CLOCK_DWON_PAGE_NUM-1,       //底部页码点->最后
};

typedef struct dropdown_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    s16 x;
    s16 y;
} dropdown_disp_btn_item_t;

#define DROPDOWN_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_dropdown_disp_btn_item) / sizeof(tbl_dropdown_disp_btn_item[0])))
#define MENU_CNT    ((int)(sizeof(dwon_tbl_style_list) / sizeof(dwon_tbl_style_list[0])))

/*!* 更新函数/*/
static void switch_set_sub_wrist(void);
static void func_clock_sub_dropdown_mute_pic_update(compo_button_t *btn);
static void func_clock_sub_dropdown_bedside_mode_pic_update(compo_button_t *btn);
static void func_clock_sub_dropdown_bluetooth_btn_pic_update(compo_button_t *btn);
static void func_clock_sub_dropdown_wrist_pic_update(compo_button_t *btn);
static void func_clock_sub_dropdown_disturb_pic_update(compo_button_t *btn);
static void func_clock_sub_dropdown_menu_pic_update(compo_button_t *btn);

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
/*! 功能事件图片地址定义 */
#define IMG_ADDR_BT_OFF               0   ///蓝牙Bt->开
#define IMG_ADDR_BT_ON                0   ///蓝牙Bt->关
#define IMG_ADDR_DISTURD_ON           0   ///勿扰->开
#define IMG_ADDR_DISTURD_OFF          0   ///勿扰->关
#define IMG_ADDR_MUTE_ON              0   ///静音->开
#define IMG_ADDR_MUTE_OFF             0   ///静音->关
#define IMG_ADDR_LIGHT_FUN            0   ///亮度->功能
#define IMG_ADDR_WRIST_ON             0   ///抬腕亮屏->开
#define IMG_ADDR_WRIST_OFF            0   ///抬腕亮屏->关
#define IMG_ADDR_FIND_PHONE_FUN       0   ///找手机->功能
#define IMG_ADDR_FLASHLIGHT_FUN       0   ///手电筒->功能
#define IMG_ADDR_SETTING_FUN          0   ///设置->功能
#define IMG_ADDR_ABOUT_FUN            0   ///关于->功能
#define IMG_ADDR_BED_LAMP_ON          0   ///床头灯->开
#define IMG_ADDR_BED_LAMP_OFF         0   ///床头灯->关

//按钮item，创建时遍历一下
static const  dropdown_disp_btn_item_t tbl_dropdown_disp_btn_item[] =
{
    ///*第一页*/
    {IMG_ADDR_BT_OFF,               COMPO_ID_BTN_CONNECT,          GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  84-56},///蓝牙连接开关
    {0,                             COMPO_ID_BTN_MENU,             GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  84-56},///菜单
    {IMG_ADDR_DISTURD_OFF,          COMPO_ID_BTN_DISCURD,          GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  148-56},///勿扰模式开关
    {IMG_ADDR_MUTE_OFF,             COMPO_ID_BTN_MUTE,             GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  148-56},///静音模式开关
    {IMG_ADDR_LIGHT_FUN,            COMPO_ID_BTN_LIGHT,            GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  212-56},///亮度调节
    {IMG_ADDR_WRIST_OFF,            COMPO_ID_BTN_WIRST,            GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  212-56},///抬婉亮屏
    ///*第二页*/
    {IMG_ADDR_FIND_PHONE_FUN,       COMPO_ID_BTN_PHONE,             GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  84-56},///查找手机
    {IMG_ADDR_FLASHLIGHT_FUN,       COMPO_ID_BTN_FLASHLIGHT,        GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  84-56},///手电筒
    {IMG_ADDR_SETTING_FUN,          COMPO_ID_BTN_SETTING,           GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  148-56},///设置
    {IMG_ADDR_ABOUT_FUN,            COMPO_ID_BTN_ABOUT,             GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2,  148-56},///关于
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    {IMG_ADDR_BED_LAMP_OFF,         COMPO_ID_BTN_BEDSIDE_MODE,     GUI_SCREEN_WIDTH + GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,  212-56},// 床头钟模式开关
#endif
};

//风格列表tbl
static const compo_listbox_item_t dwon_tbl_style_list[] =
{
    {STR_STYLE_LIST_1,          UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_06_BIN,           .menu_style = MENU_STYLE_LIST},             //列表
    {STR_GONG_GE,               UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_09_BIN,           .menu_style = MENU_STYLE_SUDOKU},           //宫格
    {STR_STYLE_HONEYCOMB,       UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_08_BIN,           .menu_style = MENU_STYLE_HONEYCOMB},        //蜂窝
    {STR_SPHERE,                UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_02_BIN,           .menu_style = MENU_STYLE_FOOTBALL},         //球体
    {STR_CHECKERBOARD,          UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_03_BIN,           .menu_style = MENU_STYLE_GRID},             //棋盘
    {STR_HALO,                  UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_07_BIN,           .menu_style = MENU_STYLE_KALE},             //光环
    {STR_STYLE_SKYRER,          UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_01_BIN,           .menu_style = MENU_STYLE_SKYRER},           //天圆地方
    {STR_STYLE_GRID_1,          UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_00_BIN,           .menu_style = MENU_STYLE_CUM_GRID},         //网格
    {STR_STYLE_WATERFALL,       UI_BUF_I335001_STYLE_CLOCK_DOWN_GRAY_04_BIN,           .menu_style = MENU_STYLE_WATERFALL},        //瀑布
};

//创建下拉菜单
compo_form_t *func_clock_dropdown_menu_form_create(void)
{

    compo_form_t *frm = compo_form_create(true);

    ///创建遮罩层
    compo_shape_t *masklayer = compo_shape_create_for_page(frm,frm->page, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);

    ///创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < DROPDOWN_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn =compo_button_create_by_image(frm, tbl_dropdown_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn,  tbl_dropdown_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_dropdown_disp_btn_item[idx_btn].x, tbl_dropdown_disp_btn_item[idx_btn].y);

        switch (tbl_dropdown_disp_btn_item[idx_btn].btn_id)
        {
            case COMPO_ID_BTN_CONNECT:
                func_clock_sub_dropdown_bluetooth_btn_pic_update(btn);////蓝牙按钮更新
                break;
            case COMPO_ID_BTN_MUTE:
                func_clock_sub_dropdown_mute_pic_update(btn);          //静音更新
                break;
            case COMPO_ID_BTN_DISCURD:
                func_clock_sub_dropdown_disturb_pic_update(btn);       //勿扰更新
                break;
            case COMPO_ID_BTN_WIRST:
                func_clock_sub_dropdown_wrist_pic_update(btn);//抬婉亮屏更新
                break;
            case COMPO_ID_BTN_MENU:
                func_clock_sub_dropdown_menu_pic_update(btn);//菜单更新
                break;
            case COMPO_ID_BTN_BEDSIDE_MODE:
                func_clock_sub_dropdown_bedside_mode_pic_update(btn); //床头钟更新
                break;
        }
    }
    //电池
    compo_picturebox_t *battery_pic = compo_picturebox_create_for_page(frm, UI_BUF_I335001_DROP_DOWN_MENU_ICON_BATTERY_34X18_X190_Y11_00_BIN,frm->page);
    compo_picturebox_set_pos(battery_pic, 207, 20);
    battery_pic->bond_data = COMPO_BOND_VBAT_PROGRESS;
    battery_pic->radix  = 5;
    compo_set_bonddata((component_t *)battery_pic, time_to_tm(compo_cb.rtc_cnt));

    //蓝牙状态 Bt
    compo_picturebox_t *bt_pic = compo_picturebox_create_for_page(frm, UI_BUF_I335001_DROP_DOWN_MENU_PHONE_ICON_PIC26X11_X54_Y15_01_BIN,frm->page);
    compo_picturebox_set_pos(bt_pic, 67, 20);
    bt_pic->bond_data = COMPO_BOND_BT_STA;
    bt_pic->radix  = 2;
    compo_set_bonddata((component_t *)bt_pic, time_to_tm(compo_cb.rtc_cnt));

    //蓝牙状态 Ble
    compo_picturebox_t *ble_pic  = compo_picturebox_create_for_page(frm, UI_BUF_I335001_DROP_DOWN_MENU_PHONE_ICON_PIC26X11_X54_Y15_01_BIN,frm->page);
    compo_picturebox_set_pos(ble_pic, 28, 20);
    ble_pic->bond_data = COMPO_BOND_BLE_STA;
    ble_pic->radix  = 2;
    compo_set_bonddata((component_t *)ble_pic, time_to_tm(compo_cb.rtc_cnt));

    char txt_buf[50];
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    const u16 week_sort[7]= {STR_SUNDAY,     // 周日
                             STR_MONDAY,     // 周一
                             STR_TUESDAY,    // 周二
                             STR_WEDNESDAY,  // 周三
                             STR_THURSDAY,   // 周四
                             STR_FRIDAY,     // 周五
                             STR_SATURDAY,   // 周六
                            };

    snprintf(txt_buf,sizeof(txt_buf),"%02d/%02d %s",time.month,time.day,i18n[week_sort[time.week]]);
    compo_textbox_t *textbox = compo_textbox_create_for_page(frm,strlen(txt_buf),frm->page);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/5.5,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/10.5);
    compo_textbox_set(textbox,txt_buf );

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",uteDrvBatteryCommonGetLvl() );//电量百分比数值
    textbox = compo_textbox_create_for_page(frm,4,frm->page);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_20_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 10,65,30);
    compo_textbox_set_right_align(textbox,true);
    compo_textbox_set(textbox,txt_buf );

    ///创建页码点
    for (int idx = COMPO_ID_PAGE_FIRST; idx <= COMPO_ID_PAGE_LAST; idx++)
    {
        compo_shape_t *page_num = compo_shape_create_for_page(frm,frm->page, COMPO_SHAPE_TYPE_RECTANGLE);
        if(idx!=COMPO_ID_PAGE_FIRST)
        {
            compo_shape_set_color(page_num, COLOR_BLACK);
        }
        compo_shape_set_location(page_num, GUI_SCREEN_CENTER_X+6*idx-(COMPO_ID_PAGE_LAST-idx)*6, 278, 8, 8);
    }

    return frm;
}
//更新显示
static void func_clock_dropdown_update(void)
{
    f_clock_dropdwon_menu_t *f_clock_dropdwon_menu = (f_clock_dropdwon_menu_t *)func_cb.f_cb;

    ///更新页码点
    for (int idx = 0; idx < CLOCK_DWON_PAGE_NUM; idx++)
    {
        compo_shape_t *page_num = compo_getobj_byid(idx);
        if(page_num!=NULL)
        {
            compo_shape_set_color(page_num, (idx == f_clock_dropdwon_menu->page_num) ? COLOR_WHITE:COLOR_BLACK);
        }
    }
}

#endif

static void func_clock_sub_dropdown_click_handler(void)
{
    dropdown_disp_btn_item_t *dropdown_disp_btn_item = (dropdown_disp_btn_item_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch(id)
    {
        case COMPO_ID_BTN_CONNECT:
            if(uteModuleCallBtIsPowerOn())
            {
                uteModuleCallBtPowerOff(UTE_BT_POWER_OFF_BUTTON);
            }
            else
            {
                uteModuleCallBtPowerOn(UTE_BT_POWER_ON_NORMAL);
            }
            func_clock_sub_dropdown_bluetooth_btn_pic_update(compo_getobj_byid(COMPO_ID_BTN_CONNECT)); //Bt更新
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
            func_clock_sub_dropdown_disturb_pic_update(compo_getobj_byid(COMPO_ID_BTN_DISCURD));//勿扰更新
        }
        break;
        case COMPO_ID_BTN_MUTE:
            if(uteModuleLocalRingtoneGetMuteStatus())
            {
                uteModuleLocalRingtoneSetMuteStatus(false,true);
            }
            else
            {
                uteModuleLocalRingtoneSetMuteStatus(true,true);
            }
            func_clock_sub_dropdown_mute_pic_update(compo_getobj_byid(COMPO_ID_BTN_MUTE));         //静音更新
            break;
        case COMPO_ID_BTN_FLASHLIGHT:
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
            func_cb.sta = FUNC_FLASHLIGHT;
#endif
            break;
        case COMPO_ID_BTN_LIGHT:
#if UTE_MODULE_SCREENS_LIGHT_SUPPORT
            func_cb.sta = FUNC_LIGHT;
#endif
            break;
        case COMPO_ID_BTN_SETTING:
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
            func_cb.sta = FUNC_SETTING;
#endif
            break;
        case COMPO_ID_BTN_ABOUT:
            func_cb.sta = FUNC_SET_SUB_ABOUT;
            break;
        case COMPO_ID_BTN_PHONE:
            func_cb.sta = FUNC_FINDPHONE;
            break;
        case COMPO_ID_BTN_WIRST:
            switch_set_sub_wrist();
            break;
        case COMPO_ID_BTN_MENU:
            for(u8 idx=0; idx<MENU_CNT; idx++)
            {
                if(dwon_tbl_style_list[idx].menu_style == func_cb.menu_style)
                {
                    idx = (idx+1)%MENU_CNT;
                    func_cb.menu_style = dwon_tbl_style_list[idx].menu_style;
                    if (func_cb.menu_style == MENU_STYLE_SKYRER)
                    {
                        u8 func_menu_sub_skyrer_get_first_idx(void);
                        func_cb.menu_idx = func_menu_sub_skyrer_get_first_idx();
                    }
                    else
                    {
                        func_cb.menu_idx = 0;
                    }
                    uteModuleGuiCommonSetThemeTypeId(func_cb.menu_style);
                    break;
                }
            }
            func_clock_sub_dropdown_menu_pic_update(compo_getobj_byid(COMPO_ID_BTN_MENU));//菜单样式按钮更新
            break;
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
        case COMPO_ID_BTN_BEDSIDE_MODE:
            uteModuleBedsideModeSwitch();
            func_clock_sub_dropdown_bedside_mode_pic_update(compo_getobj_byid(COMPO_ID_BTN_BEDSIDE_MODE));
            break;
#endif
        default:
            break;
    }
}

static void func_clock_sub_dropdown_process(void)
{
    func_clock_dropdown_update();
    func_clock_sub_process();
}

//时钟表盘下拉菜单功能消息处理
static void func_clock_sub_dropdown_message(size_msg_t msg)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clock_dropdwon_menu_t *f_clock_dropdwon_menu =(f_clock_dropdwon_menu_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_clk->ptm)
            {
                compo_page_move_touch_handler(f_clk->ptm);
            }
            break;
        case MSG_CTP_CLICK:
            func_clock_sub_dropdown_click_handler();
            break;
        case MSG_CTP_SHORT_LEFT:
#if !UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE
            if(f_clock_dropdwon_menu->page_num < CLOCK_DWON_PAGE_NUM-1)
            {
                f_clock_dropdwon_menu->page_num++;
            }
#endif
            break;
        case MSG_CTP_SHORT_RIGHT:
#if !UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE
            if(f_clock_dropdwon_menu->page_num != 0)
            {
                f_clock_dropdwon_menu->page_num--;
            }
#endif
            break;
        default:
            func_message(msg);
            break;
    }
}

//时钟表盘下拉菜单进入处理
static void func_clock_sub_dropdown_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_clock_dropdwon_menu_t));
    func_cb.frm_main = func_clock_dropdown_menu_form_create();
    f_clock_dropdwon_menu_t *f_clock_dropdwon_menu = (f_clock_dropdwon_menu_t *)func_cb.f_cb;
#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE//是否使用滑动效果
    f_clock_dropdwon_menu->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .dir = 1,
        .page_size =  GUI_SCREEN_WIDTH,
        .page_count = CLOCK_DWON_PAGE_NUM,//项目config定义页面数量
        .jump_perc  = GUI_SCREEN_WIDTH/18,
    };
    compo_page_move_init(f_clock_dropdwon_menu->ptm,func_cb.frm_main->page_body,&info);
#endif
}

//时钟表盘下拉菜单退出处理
static void func_clock_sub_dropdown_exit(void)
{

}

//时钟表盘下拉菜单
void func_clock_dropdown_menu(void)
{
    func_clock_sub_dropdown_enter();
    while (func_cb.sta == FUNC_CLOCK_DROPDOWN_MENU)
    {
        func_clock_sub_dropdown_process();
        func_clock_sub_dropdown_message(msg_dequeue());
    }
    func_clock_sub_dropdown_exit();
}

///               /                 *更新*                    /                 ///
//下拉抬婉亮屏按钮状态切换
static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    uteApplicationCommonSendQuickSwitchStatus();
}

//下拉静音按钮更新
static void func_clock_sub_dropdown_mute_pic_update(compo_button_t *btn)
{
    if(uteModuleLocalRingtoneGetMuteStatus())
    {
        compo_button_set_bgimg(btn, IMG_ADDR_MUTE_ON);
    }
    else
    {
        compo_button_set_bgimg(btn, IMG_ADDR_MUTE_OFF);
    }
}

// 下拉床头钟按钮更新
static void func_clock_sub_dropdown_bedside_mode_pic_update(compo_button_t *btn)
{
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    if(uteModuleBedsideModeIsOpen())
    {
        compo_button_set_bgimg(btn, IMG_ADDR_BED_LAMP_ON);
    }
    else
    {
        compo_button_set_bgimg(btn, IMG_ADDR_BED_LAMP_OFF);
    }
#endif
}

////蓝牙按钮更新
static void func_clock_sub_dropdown_bluetooth_btn_pic_update(compo_button_t *btn)
{
    if(uteModuleCallBtIsPowerOn())
    {
        compo_button_set_bgimg(btn, IMG_ADDR_BT_ON);
    }
    else
    {
        compo_button_set_bgimg(btn,IMG_ADDR_BT_OFF );
    }
}

//抬婉亮屏按钮更新
static void func_clock_sub_dropdown_wrist_pic_update(compo_button_t *btn)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);

    if(quick.isTurnTheWrist == true)
    {
        compo_button_set_bgimg(btn, IMG_ADDR_WRIST_ON);
    }
    else
    {
        compo_button_set_bgimg(btn, IMG_ADDR_WRIST_OFF);
    }
}
//勿扰按钮更新
static void func_clock_sub_dropdown_disturb_pic_update(compo_button_t *btn)
{
    if(uteModuleNotDisturbGetOpenStatus())
    {
        compo_button_set_bgimg(btn, IMG_ADDR_DISTURD_ON);
    }
    else
    {
        compo_button_set_bgimg(btn, IMG_ADDR_DISTURD_OFF);
    }

}
//菜单样式按钮更新
static void func_clock_sub_dropdown_menu_pic_update(compo_button_t *btn)
{
    if(btn == NULL)
    {
        return;
    }
    for(u8 idx=0; idx<MENU_CNT; idx++)
    {
        if(dwon_tbl_style_list[idx].menu_style == func_cb.menu_style)
        {
            compo_button_set_bgimg(btn, dwon_tbl_style_list[idx].res_addr);
            return;
        }
    }
}

///               /                 *更新*                    /                 ///

#endif // UTE_MODULE_SCREENS_DWON_MENU_SUPPORT
