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
#include "ute_module_settingremind.h"

#if UTE_MODULE_SCREENS_NEW_DWON_MENU_SUPPORT

typedef struct f_clock_dropdwon_menu_t_
{
#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE
    page_tp_move_t* ptm;
    bool touch_flag;
#endif
    u8 page_num;
    bool up_date_flag;
} f_clock_dropdwon_menu_t;

#ifdef UTE_CLOCK_DWON_PAGE_NUM //页面数量
#define CLOCK_DWON_PAGE_NUM   UTE_CLOCK_DWON_PAGE_NUM
#else
#define CLOCK_DWON_PAGE_NUM   2
#endif

/*!* 更新函数  后续添加功能用宏控制/*/
//static void switch_set_sub_wrist(void);
//static void func_clock_sub_dropdown_mute_pic_update(compo_button_t *btn);
//#if UTE_MODULE_BEDSIDE_MODE_SUPPORT// 床头钟更新
//static void func_clock_sub_dropdown_bedside_mode_pic_update(compo_button_t *btn);
//#endif
//static void func_clock_sub_dropdown_bluetooth_btn_pic_update(compo_button_t *btn);
//static void func_clock_sub_dropdown_wrist_pic_update(compo_button_t *btn);
//static void func_clock_sub_dropdown_disturb_pic_update(compo_button_t *btn);
//static void func_clock_sub_dropdown_menu_pic_update(compo_button_t *btn);

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
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

/*! 功能事件图片地址定义 */
#define IMG_ADDR_BT_OFF               UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_01_BIN   ///蓝牙Bt->开
#define IMG_ADDR_BT_ON                UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_00_BIN   ///蓝牙Bt->关
#define IMG_ADDR_DISTURD_ON           UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_04_BIN   ///勿扰->开
#define IMG_ADDR_DISTURD_OFF          UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_03_BIN   ///勿扰->关
#define IMG_ADDR_MUTE_ON              UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_06_BIN   ///静音->开
#define IMG_ADDR_MUTE_OFF             UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_05_BIN   ///静音->关
#define IMG_ADDR_LIGHT_FUN            UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_07_BIN   ///亮度->功能
#define IMG_ADDR_WRIST_ON             UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_09_BIN   ///抬腕亮屏->开
#define IMG_ADDR_WRIST_OFF            UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_08_BIN   ///抬腕亮屏->关
#define IMG_ADDR_FIND_PHONE_FUN       UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_10_BIN   ///找手机->功能
#define IMG_ADDR_FLASHLIGHT_FUN       UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_11_BIN   ///手电筒->功能
#define IMG_ADDR_SETTING_FUN          UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_13_BIN   ///设置->功能
#define IMG_ADDR_ABOUT_FUN            UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_17_BIN   ///关于->功能
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT// 床头钟模式开关
#define IMG_ADDR_BED_LAMP_ON          UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_CHAUNGTOUDENG01_BIN   ///床头灯->开
#define IMG_ADDR_BED_LAMP_OFF         UI_BUF_I335001_DROP_DOWN_MENU_TUBIAO_ICON_PIC108X56_X8_124_Y84_148_212_CHAUNGTOUDENG00_BIN   ///床头灯->关
#endif
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
        btn = compo_button_create_by_image(frm, tbl_dropdown_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn,  tbl_dropdown_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_dropdown_disp_btn_item[idx_btn].x, tbl_dropdown_disp_btn_item[idx_btn].y+67);

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
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT// 床头钟更新
                func_clock_sub_dropdown_bedside_mode_pic_update(btn); //床头钟更新
#endif
                break;
        }
    }

    //电池
    compo_picturebox_t *battery_pic = compo_picturebox_create_for_page(frm,frm->page, UI_BUF_I335001_DROP_DOWN_MENU_ICON_BATTERY_34X18_X190_Y11_00_BIN);
    compo_picturebox_set_pos(battery_pic, 207, 20);
    battery_pic->bond_data = COMPO_BOND_VBAT_PROGRESS;
    battery_pic->radix  = 5;
    compo_set_bonddata((component_t *)battery_pic, time_to_tm(compo_cb.rtc_cnt));
    //蓝牙状态 Bt
    compo_picturebox_t *bt_pic = compo_picturebox_create_for_page(frm,frm->page, UI_BUF_I335001_DROP_DOWN_MENU_PHONE_ICON_PIC26X11_X54_Y15_01_BIN);
    compo_picturebox_set_pos(bt_pic, 67, 20);
    bt_pic->bond_data = COMPO_BOND_BT_STA;
    bt_pic->radix  = 2;
    compo_set_bonddata((component_t *)bt_pic, time_to_tm(compo_cb.rtc_cnt));
    //蓝牙状态 Ble
    compo_picturebox_t *ble_pic  = compo_picturebox_create_for_page(frm,frm->page, UI_BUF_I335001_DROP_DOWN_MENU_PHONE_ICON_PIC26X11_X54_Y15_01_BIN);
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
    compo_textbox_t *textbox = compo_textbox_create_for_page(frm,frm->page,strlen(txt_buf));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/5.5,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/10.5);
    compo_textbox_set(textbox,txt_buf );
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",uteDrvBatteryCommonGetLvl() );//电量百分比数值
    textbox = compo_textbox_create_for_page(frm,frm->page,4);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_20_BIN);
    compo_textbox_set_align_center(textbox,false);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 10,65,30);
    compo_textbox_set_right_align(textbox,true);
    compo_textbox_set(textbox,txt_buf );

    u16 page_num_pos[COMPO_ID_PAGE_LAST-COMPO_ID_PAGE_FIRST+1]= {114,126};
    ///创建页码点
    for (int idx = COMPO_ID_PAGE_FIRST; idx <= COMPO_ID_PAGE_LAST; idx++)
    {
        compo_shape_t *page_num = compo_shape_create_for_page(frm,frm->page, COMPO_SHAPE_TYPE_RECTANGLE);
        if(idx!=COMPO_ID_PAGE_FIRST)
        {
            compo_shape_set_color(page_num, make_color(51,51,51));
        }
        compo_shape_set_location(page_num, page_num_pos[idx-COMPO_ID_PAGE_FIRST], 278, 8, 8);
        compo_shape_set_radius(page_num,4);
        compo_setid(page_num,idx);
    }
    return frm;
}
//更新显示
static void func_clock_dropdown_update(void)
{

}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

#define PAGE_HEIGHT     GUI_SCREEN_HEIGHT - GUI_SCREEN_HEIGHT/3.7
#define  BT_ON_PIC_BIN       UI_BUF_I341001_PRIMARY_FUNCTION_CALL_BIN           ///BT 连接状态图片
#define  BT_OFF_PIC_BIN      UI_BUF_I341001_PRIMARY_FUNCTION_GRAY_CALL_BIN      ///BT 断开状态图片
#define  BLE_ON_PIC_BIN      UI_BUF_I341001_PRIMARY_FUNCTION_BLUETOOTH_BIN      ///BLE 连接状态图片
#define  BLE_OFF_PIC_BIN     UI_BUF_I341001_PRIMARY_FUNCTION_GRAY_BLUETOOTH_BIN ///BLE 断开状态图片
#define  PAGE_HEIGHT_ADJUST    -3   ///page 页面y轴调节

static uint8_t dropdown_disturb_sw;     //功能未做，加个开关先放着

enum
{
    COMPO_ID_BTN_START = FUNC_MAX_NUM, //从任务最大枚举开始，避免和表盘跳转et的d冲突
    //按键
    COMPO_ID_BTN_CONNECT,       //蓝牙连接开关
    COMPO_ID_BTN_FLASHLIGHT,    //手电筒
    COMPO_ID_BTN_LIGHT,         //亮度调节
    COMPO_ID_BTN_DISCURD,       //勿扰模式开关
    COMPO_ID_BTN_WIRST,         //抬婉亮屏
#if UTE_MODULE_POWER_SAVING_SUPPORT
    COMPO_ID_BTN_POWER_SAVE,    //省电模式
#endif
    COMPO_ID_BTN_MUTE,          //静音模式开关
    COMPO_ID_BTN_VOICE,         //音量调节
    COMPO_ID_BTN_SETTING,       //设置

    //电池百分比文本
    COMPO_ID_TXT_BATTERY_PERCENT,
    //电池百分比图片
    COMPO_ID_TXT_BATTERY_PIC,
    //蓝牙状态图片
    COMPO_ID_TXT_BLUETOOTH_STA_PIC,
    COMPO_ID_TXT_BTETOOTH_STA_PIC,
};

typedef struct dropdown_disp_btn_item_t_
{
    u32 res_addr;
    u16 btn_id;
    s16 x;
    s16 y;
    u8 sel_idx;
} dropdown_disp_btn_item_t;

#define DROPDOWN_DISP_BTN_ITEM_CNT    ((int)(sizeof(tbl_dropdown_disp_btn_item) / sizeof(tbl_dropdown_disp_btn_item[0])))
//类型有显示信息，控制开关，点击跳转任务
//按钮item，创建时遍历一下
#define ICON_W (90 / 2)
#define ICON_H (90 / 2)
static const  dropdown_disp_btn_item_t tbl_dropdown_disp_btn_item[] =
{
    ///*第一页*/
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_CALL_BIN,                COMPO_ID_BTN_CONNECT,    29  + ICON_W,109 + ICON_H},///蓝牙连接开关
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_FLASHLIGHT_BIN,          COMPO_ID_BTN_FLASHLIGHT, 139 + ICON_W,109 + ICON_H},//手电筒
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_BRIGHTNESS_BIN,          COMPO_ID_BTN_LIGHT,      249 + ICON_W,109 + ICON_H},//亮度调节
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_DO_NOT_DISTURB_BIN,      COMPO_ID_BTN_DISCURD,    29  + ICON_W,217 + ICON_H},///勿扰模式开关
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_PHONE_VIBRATION_BIN,     COMPO_ID_BTN_WIRST,      139 + ICON_W,217 + ICON_H},//翻腕开关
#if UTE_MODULE_POWER_SAVING_SUPPORT
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_POWER_SAVING_BIN,        COMPO_ID_BTN_POWER_SAVE, 249 + ICON_W,217 + ICON_H},///省电模式
#endif
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_SOUND_OFF_BIN,           COMPO_ID_BTN_MUTE,       29  + ICON_W,325 + ICON_H},///勿扰模式
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_SOUND_BIN,               COMPO_ID_BTN_VOICE,      139 + ICON_W,325 + ICON_H},///音量调节
    {UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_SETTING_BIN,             COMPO_ID_BTN_SETTING,    249 + ICON_W,325 + ICON_H},///设置
};

//下拉电量图标更新
static void func_clock_sub_dropdown_battery_pic_update(void)
{
    compo_picturebox_t *battery_pic = compo_getobj_byid(COMPO_ID_TXT_BATTERY_PIC);
    compo_picturebox_cut(battery_pic,uteDrvBatteryCommonGetBatteryIndex(6),6);
}
////下拉蓝牙连接标志更新
static void func_clock_sub_dropdown_bluetooth_pic_update(void)
{
    compo_picturebox_t *bluetooth_pic = compo_getobj_byid(COMPO_ID_TXT_BLUETOOTH_STA_PIC);
    compo_picturebox_t *btooth_pic = compo_getobj_byid(COMPO_ID_TXT_BTETOOTH_STA_PIC);
    if (ble_is_connect())
    {
        compo_picturebox_set(bluetooth_pic, BLE_ON_PIC_BIN);
    }
    else
    {
        compo_picturebox_set(bluetooth_pic, BLE_OFF_PIC_BIN);
    }

    if(uteModuleCallBtIsConnected())
    {
        compo_picturebox_set(btooth_pic, BT_ON_PIC_BIN);
    }
    else
    {
        compo_picturebox_set(btooth_pic, BT_OFF_PIC_BIN);
    }

}
//下拉静音图标更新
static void func_clock_sub_dropdown_mute_pic_update(void)
{
    compo_button_t *mute_pic = compo_getobj_byid(COMPO_ID_BTN_MUTE);
    compo_button_t *voic_pic = compo_getobj_byid(COMPO_ID_BTN_VOICE);
    // if(sys_cb.mute)
    if(!uteModuleLocalRingtoneGetMuteStatus())
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_SOUND_OFF_BIN);
        compo_button_set_bgimg(voic_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_BLUE_SOUND_BIN);
    }
    else
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_BLUE_SOUND_OFF_BIN);
        compo_button_set_bgimg(voic_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_SOUND_BIN);
    }
}

// 下拉床头钟图标更新
static void func_clock_sub_dropdown_bedside_mode_pic_update(void)
{
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
    compo_button_t *mute_pic = compo_getobj_byid(COMPO_ID_BTN_BEDSIDE_MODE);
    // if(sys_cb.mute)
    if(uteModuleBedsideModeIsOpen())
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_SOUND_OFF_BIN);
    }
    else
    {
        compo_button_set_bgimg(mute_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_SOUND_OFF_BIN);
    }
#endif
}

////下拉蓝牙按钮更新
static void func_clock_sub_dropdown_bluetooth_btn_pic_update(void)
{
    compo_button_t *bluetooth_pic = compo_getobj_byid(COMPO_ID_BTN_CONNECT);
    if(uteModuleCallBtIsPowerOn())
    {
        compo_button_set_bgimg(bluetooth_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_BLUE_CALL_BIN);
    }
    else
    {
        compo_button_set_bgimg(bluetooth_pic,UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_CALL_BIN );
    }
}
//下拉抬婉亮屏按钮状态切换
static void switch_set_sub_wrist(void)
{
    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if (quick.isTurnTheWrist)
    {
        if (!uteModuleLiftWristIsTimeBucket() && !uteModuleLiftWristIsOpen())
        {
            return;
        }
    }
    quick.isTurnTheWrist = !quick.isTurnTheWrist;
    bool isHandOpen = quick.isTurnTheWrist;
    uteModuleSportSaveHandScreenOnStepsTargetCnt(isHandOpen,uteModuleSportGetStepsTargetCnt());
    uteApplicationCommonSetQuickSwitchStatus(&quick);
    //uteApplicationCommonSendQuickSwitchStatus();
}
//下拉抬婉亮屏按钮更新
static void func_clock_sub_dropdown_wrist_pic_update(void)
{
    compo_button_t *wrist_pic = compo_getobj_byid(COMPO_ID_BTN_WIRST);

    ute_quick_switch_t quick;
    uteApplicationCommonGetQuickSwitchStatus(&quick);
    if(quick.isTurnTheWrist && (uteModuleLiftWristIsTimeBucket() || uteModuleLiftWristIsOpen()))
    {
        compo_button_set_bgimg(wrist_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_BLUE_PHONE_VIBRATION_BIN);
    }
    else
    {
        compo_button_set_bgimg(wrist_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_PHONE_VIBRATION_BIN);
    }

}
//下拉勿扰按钮更新
extern bool scheduleOldState;
static void func_clock_sub_dropdown_disturb_pic_update(void)
{
    compo_button_t *disturb_pic = compo_getobj_byid(COMPO_ID_BTN_DISCURD);

    if (dropdown_disturb_sw)
    {
        if (scheduleOldState)
        {
            sys_cb.disturd_tim = 1;
        }
        else
        {
            sys_cb.disturd_adl = 1;
        }
        compo_button_set_bgimg(disturb_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_BLUE_DO_NOT_DISTURB_BIN);
        if (!uteModuleNotDisturbIsTimeBucket() && !uteModuleNotDisturbGetOpenStatus())
        {
            compo_button_set_bgimg(disturb_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_DO_NOT_DISTURB_BIN);
        }
    }
    else
    {
        sys_cb.disturd_adl = 0;
        sys_cb.disturd_tim = 0;
        compo_button_set_bgimg(disturb_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_DO_NOT_DISTURB_BIN);
    }

}

//省电模式更新
#if UTE_MODULE_POWER_SAVING_SUPPORT
static void get_saving_mode_update(void)
{
    compo_button_t *saving_pic = compo_getobj_byid(COMPO_ID_BTN_POWER_SAVE);

    if(uteModuleGuiCommonGetPowerSavingModeOpen())
    {
        compo_button_set_bgimg(saving_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_BLUE_POWER_SAVING_BIN);
    }
    else
    {
        compo_button_set_bgimg(saving_pic, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_GRAY_POWER_SAVING_BIN);
    }
}
#endif

///               /                 *更新*                    /                 ///


//创建下拉菜单
compo_form_t *func_clock_dropdown_menu_form_create(void)
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
#if 0//UTE_MODULE_LOCAL_SET_NOT_DISTURB_SUPPORT
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

    ///创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);


    ///创建按钮
    compo_button_t *btn;
    for (u8 idx_btn = 0; idx_btn < DROPDOWN_DISP_BTN_ITEM_CNT; idx_btn++)
    {
        btn =compo_button_create_by_image(frm,tbl_dropdown_disp_btn_item[idx_btn].res_addr);
        compo_setid(btn,  tbl_dropdown_disp_btn_item[idx_btn].btn_id);
        compo_button_set_pos(btn, tbl_dropdown_disp_btn_item[idx_btn].x, tbl_dropdown_disp_btn_item[idx_btn].y);
    }
    //电池
    compo_picturebox_t *battery_pic = compo_picturebox_create(frm, UI_BUF_I341001_PRIMARY_FUNCTION_ICON_BATT_BIN);
    compo_setid(battery_pic, COMPO_ID_TXT_BATTERY_PIC);
    compo_picturebox_set_pos(battery_pic, 287+26, 17+14);
    compo_picturebox_cut(battery_pic,uteDrvBatteryCommonGetBatteryIndex(6),6);

    //蓝牙状态 Bt
    compo_picturebox_t *bluetooth_pic = compo_picturebox_create(frm, BT_OFF_PIC_BIN);
    compo_setid(bluetooth_pic, COMPO_ID_TXT_BTETOOTH_STA_PIC);
    compo_picturebox_set_pos(bluetooth_pic, 82+21, 23+9);

    //蓝牙状态 Ble
    bluetooth_pic = compo_picturebox_create(frm, BLE_OFF_PIC_BIN);
    compo_setid(bluetooth_pic, COMPO_ID_TXT_BLUETOOTH_STA_PIC);
    compo_picturebox_set_pos(bluetooth_pic, 28+20, 23+9);

    char txt_buf[50];
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);//获取系统时间

    snprintf(txt_buf,sizeof(txt_buf),"%02d/%02d %s",time.month,time.day,i18n[STR_SUNDAY+time.week]);
    compo_textbox_t *textbox = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/5.5,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT/10.5);
    compo_textbox_set(textbox,txt_buf );

//#define BAT_PERCENT_VALUE       uteDrvBatteryCommonGetLvl()  //电量百分比数值
//    memset(txt_buf,0,sizeof(txt_buf));
//    snprintf(txt_buf,sizeof(txt_buf),"%d%%",BAT_PERCENT_VALUE);
//    textbox = compo_textbox_create(frm,4);
//    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_28_BIN);
//    compo_textbox_set_align_center(textbox,false);
//    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X, 16,97,32);
//    compo_textbox_set_right_align(textbox,true);
//    compo_textbox_set(textbox,txt_buf );

    func_clock_sub_dropdown_battery_pic_update();//下拉电量图标更新
    func_clock_sub_dropdown_bluetooth_pic_update();     //蓝牙更新
    func_clock_sub_dropdown_bluetooth_btn_pic_update();////下拉蓝牙按钮更新
    func_clock_sub_dropdown_mute_pic_update();          //静音更新
    func_clock_sub_dropdown_disturb_pic_update();       //勿扰
#if UTE_MODULE_POWER_SAVING_SUPPORT
    get_saving_mode_update();                           //省电
#endif
    func_clock_sub_dropdown_wrist_pic_update();//下拉抬婉亮屏按钮更新
    //func_clock_sub_dropdown_menu_pic_update();//下拉菜单->菜单切换按钮更新
    func_clock_sub_dropdown_bedside_mode_pic_update(); // 下拉床头钟图标更新

    return frm;
//    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
//    f_clk->sub_frm = frm;
//    f_clk->masklayer = masklayer;
}

bool savingState = false;
static void func_clock_sub_dropdown_click_handler(void)
{
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
            func_clock_sub_dropdown_bluetooth_btn_pic_update();
            break;
        case COMPO_ID_BTN_DISCURD:
        {
#if UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT
            if (uteModuleSettingRemindGetWhetherShowId(SETTING_REMIND_ID_DISTUB))
            {
                uteModuleSettingRemindSetRemindId(SETTING_REMIND_ID_DISTUB);
                uteTaskGuiStartScreen(FUNC_SETTING_REMIND, 0, __func__);
            }
            else
#endif
            {
                ute_quick_switch_t quick;
                uteApplicationCommonGetQuickSwitchStatus(&quick);
                if(!quick.isNotDisturb)
                {
                    if (scheduleOldState)
                    {
                        uteModuleNotDisturbScheduledSwitch();
                    }
                    else
                    {
                        uteModuleNotDisturbAllDaySwitch();
                    }
                }
                else
                {
                    if (uteModuleNotDisturbGetOpenStatus())
                    {
                        uteModuleNotDisturbAllDaySwitch();
                    }
                    else
                    {
                        if (!uteModuleNotDisturbIsTimeBucket())
                        {
                            uteModuleNotDisturbScheduledSwitch();
                            uteModuleNotDisturbAllDaySwitch();
                            scheduleOldState = false;
                        }
                        else
                        {
                            uteModuleNotDisturbScheduledSwitch();
                        }
                    }
                }
                //uteApplicationCommonSendQuickSwitchStatus();
                if (uteModuleNotDisturbGetOpenStatus() || uteModuleNotDisturbIsTimeBucket())
                {
                    dropdown_disturb_sw = !quick.isNotDisturb;
                }
                func_clock_sub_dropdown_disturb_pic_update();
                //func_clock_sub_dropdown_wrist_pic_update();
            }
        }
        break;
#if UTE_MODULE_POWER_SAVING_SUPPORT
        case COMPO_ID_BTN_POWER_SAVE:
            if (uteModuleGuiCommonGetPowerSavingModeOpen())
            {
//                uteModuleGuiCommonSetPowerSavingModeOpen(false);
//                tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(), true);
            }
            else
            {
                savingState = true;
                //uteModuleGuiCommonSetPowerSavingModeOpen(true);
            }
            //tft_bglight_set_level(uteModuleGuiCommonGetBackLightPercent(), true);
#if UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT
            if (uteModuleSettingRemindGetWhetherShowId(SETTING_REMIND_ID_SAVING))
            {
                uteModuleSettingRemindSetRemindId(SETTING_REMIND_ID_SAVING);
                uteTaskGuiStartScreen(FUNC_SETTING_REMIND, 0, __func__);
            }
#endif
            get_saving_mode_update();
            break;
#endif
        case COMPO_ID_BTN_MUTE:
#if UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT
            if (uteModuleSettingRemindGetWhetherShowId(SETTING_REMIND_ID_MUTE))
            {
                uteModuleSettingRemindSetRemindId(SETTING_REMIND_ID_MUTE);
                uteTaskGuiStartScreen(FUNC_SETTING_REMIND, 0, __func__);
            }
            else
#endif
            {
                if(uteModuleLocalRingtoneGetMuteStatus())
                {
                    uteModuleLocalRingtoneSetMuteStatus(false,true);
                }
                else
                {
                    uteModuleLocalRingtoneSetMuteStatus(true,true);
                }
                func_clock_sub_dropdown_mute_pic_update();          //静音更新
            }
            break;
        //点击任务跳转
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
        case COMPO_ID_BTN_VOICE:
#if 1//UTE_MODULE_SCREENS_VOLUME_SUPPORT
            func_cb.sta = FUNC_VOLUME;//FUNC_SET_SUB_VOLUME;
#endif
            break;
        case COMPO_ID_BTN_WIRST:
//            if (uteModuleNotDisturbGetOpenStatus() || uteModuleNotDisturbIsTimeBucket() \
//                || uteModuleGuiCommonGetPowerSavingModeOpen())
//                break;
#if UTE_MODULE_SCREENS_SETTING_REMIND_SUPPORT
            if (uteModuleSettingRemindGetWhetherShowId(SETTING_REMIND_ID_WRIST))
            {
                uteModuleSettingRemindSetRemindId(SETTING_REMIND_ID_WRIST);
                uteTaskGuiStartScreen(FUNC_SETTING_REMIND, 0, __func__);
            }
            else
#endif
            {
                switch_set_sub_wrist();
                ute_quick_switch_t quick;
                uteApplicationCommonGetQuickSwitchStatus(&quick);
                if (quick.isTurnTheWrist)
                {
                    if (uteModuleLiftWristGetOldScheduledOpenStatus())
                    {
                        uteModuleLiftWristSetScheduled(true);
                        if (!uteModuleLiftWristIsTimeBucket())
                        {
                            uteModuleLiftWristSetOpenStatus(true);
                            uteModuleLiftWristSetScheduled(false);
                        }
                    }
                    else
                    {
                        uteModuleLiftWristSetOpenStatus(true);
                    }
                    uteModuleLiftWristRecordOpenStatus();
                }
                else
                {
                    uteModuleLiftWristSetOpenStatus(false);
                    uteModuleLiftWristSetScheduled(false);
                    uteModuleLiftWristSetNowOpenStatus(LIFT_WRIST_CLOSE);
                }
                func_clock_sub_dropdown_wrist_pic_update();
            }
            break;
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT
        case COMPO_ID_BTN_BEDSIDE_MODE:
            uteModuleBedsideModeSwitch();
            func_clock_sub_dropdown_bedside_mode_pic_update();
            break;
#endif
        default:
            break;
    }
}
//更新显示
static void func_clock_dropdown_update(void)
{

}
//更新页码点显示
static void func_clock_dropdown_update_page_num(void)
{

}

#else
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
#if UTE_MODULE_BEDSIDE_MODE_SUPPORT// 床头钟模式开关
#define IMG_ADDR_BED_LAMP_ON          0   ///床头灯->开
#define IMG_ADDR_BED_LAMP_OFF         0   ///床头灯->关
#endif
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
    {STR_STYLE_LIST_1,          0,           .menu_style = MENU_STYLE_LIST},             //列表
    {STR_GONG_GE,               0,           .menu_style = MENU_STYLE_SUDOKU},           //宫格
    {STR_STYLE_HONEYCOMB,       0,           .menu_style = MENU_STYLE_HONEYCOMB},        //蜂窝
    {STR_SPHERE,                0,           .menu_style = MENU_STYLE_FOOTBALL},         //球体
    {STR_CHECKERBOARD,          0,           .menu_style = MENU_STYLE_GRID},             //棋盘
    {STR_HALO,                  0,           .menu_style = MENU_STYLE_KALE},             //光环
    {STR_STYLE_SKYRER,          0,           .menu_style = MENU_STYLE_SKYRER},           //天圆地方
    {STR_STYLE_GRID_1,          0,           .menu_style = MENU_STYLE_CUM_GRID},         //网格
    {STR_STYLE_WATERFALL,       0,           .menu_style = MENU_STYLE_WATERFALL},        //瀑布
};

//创建下拉菜单
compo_form_t *func_clock_dropdown_menu_form_create(void)
{

    compo_form_t *frm = compo_form_create(true);
    return frm;
}
//更新显示
static void func_clock_dropdown_update(void)
{

}
#endif
//更新页码点显示


//static void func_clock_dropdown_update_page_num(void)
//{
//    f_clock_dropdwon_menu_t *f_clock_dropdwon_menu = (f_clock_dropdwon_menu_t *)func_cb.f_cb;
//
//#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE
//    if(f_clock_dropdwon_menu->ptm && f_clock_dropdwon_menu->touch_flag)
//    {
//        compo_page_move_process(f_clock_dropdwon_menu->ptm);
//
//        if(f_clock_dropdwon_menu->ptm->auto_move_offset == 0)
//        {
//            s32 dx,dy;
//            f_clock_dropdwon_menu->touch_flag = ctp_get_dxy(&dx,&dy);
//        }
//        int16_t move_y = compo_page_move_get_offset(f_clock_dropdwon_menu->ptm);
//
//        if(move_y%GUI_SCREEN_WIDTH == 0)
//        {
//            f_clock_dropdwon_menu->up_date_flag = true;
//            f_clock_dropdwon_menu->page_num = -(move_y/GUI_SCREEN_WIDTH);
//        }
//
//    }
//#endif
//    if(f_clock_dropdwon_menu->up_date_flag)
//    {
//        f_clock_dropdwon_menu->up_date_flag = false;
//        ///更新页码点
//        for (int idx = COMPO_ID_PAGE_FIRST; idx <= COMPO_ID_PAGE_LAST; idx++)
//        {
//            compo_shape_t *page_num = compo_getobj_byid(idx);
//            if(page_num!=NULL)
//            {
//                compo_shape_set_color(page_num, (idx-COMPO_ID_PAGE_FIRST == f_clock_dropdwon_menu->page_num) ? COLOR_WHITE:make_color(51,51,51));
//            }
//        }
//        widget_page_set_client(func_cb.frm_main->page_body, -(f_clock_dropdwon_menu->page_num*GUI_SCREEN_WIDTH), 0);
//    }
//
//}

static void func_clock_sub_dropdown_process(void)
{
    func_clock_dropdown_update();//预留更新函数
    func_clock_dropdown_update_page_num();//更新页码点
    func_process();
}

//下拉返回表盘
static void func_message_down_to_clock(bool auto_switch)
{
    printf("%s\n", __func__);
    u16 switch_mode = FUNC_SWITCH_MENU_DROPDOWN_UP | (auto_switch ? FUNC_SWITCH_AUTO : 0);
    compo_form_destroy(func_cb.frm_main);
#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE//是否使用滑动效果
    f_clock_dropdwon_menu_t *f_clock_dropdwon_menu =(f_clock_dropdwon_menu_t *)func_cb.f_cb;
    if(f_clock_dropdwon_menu->ptm != NULL)
    {
        func_free(f_clock_dropdwon_menu->ptm);
    }
#endif
    compo_form_t *frm_clock = func_create_form(FUNC_CLOCK);
    compo_form_t *frm = func_clock_dropdown_menu_form_create();
    func_cb.frm_main = frm;

    if (func_switching(switch_mode, NULL))
    {
        func_cb.sta = FUNC_CLOCK;
    }
    compo_form_destroy(frm_clock);
#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE//是否使用滑动效果
    f_clock_dropdwon_menu->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .dir = 1,
#if (UTE_DRV_SCREEN_SHAPE==1)
        .page_size =  GUI_SCREEN_WIDTH+(GUI_SCREEN_HEIGHT-GUI_SCREEN_WIDTH)/2,
#else
        .page_size =  GUI_SCREEN_WIDTH,
#endif
        .page_count = CLOCK_DWON_PAGE_NUM,//项目config定义页面数量
        .jump_perc  = GUI_SCREEN_WIDTH/20,
    };
    compo_page_move_init(f_clock_dropdwon_menu->ptm,func_cb.frm_main->page_body,&info);
#endif
}

//时钟表盘下拉菜单功能消息处理
static void func_clock_sub_dropdown_message(size_msg_t msg)
{
    f_clock_dropdwon_menu_t *f_clock_dropdwon_menu =(f_clock_dropdwon_menu_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE//是否使用滑动效果
            f_clock_dropdwon_menu->touch_flag = true;
            if(f_clock_dropdwon_menu->ptm)
            {
                compo_page_move_touch_handler(f_clock_dropdwon_menu->ptm);
            }
#endif
            break;
        case MSG_CTP_CLICK:
            func_clock_sub_dropdown_click_handler();//点击事件
            break;
        case MSG_CTP_SHORT_LEFT:
#if !UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE//是否使用滑动效果
            if(f_clock_dropdwon_menu->page_num < CLOCK_DWON_PAGE_NUM-1)
            {
                f_clock_dropdwon_menu->page_num++;
                f_clock_dropdwon_menu->up_date_flag=true;
            }
#endif
            break;
        case MSG_CTP_SHORT_RIGHT:
#if !UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE//是否使用滑动效果
            if(f_clock_dropdwon_menu->page_num != 0)
            {
                f_clock_dropdwon_menu->page_num--;
                f_clock_dropdwon_menu->up_date_flag=true;
            }
#endif
            break;
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        {
            func_message_down_to_clock(false);//上下切换效果
        }
        break;
        case KU_DELAY_BACK:
            func_message_down_to_clock(true);
            break;
        case MSG_SYS_1S:
            func_clock_sub_dropdown_bluetooth_pic_update();
            break;
        default:
            func_message(msg);
            break;
    }
}

//时钟表盘下拉菜单进入处理
static void func_clock_sub_dropdown_enter(void)
{
    if(uteModuleNotDisturbIsOpenScheduled())
    {
        scheduleOldState = 1;
    }
    func_cb.f_cb = func_zalloc(sizeof(f_clock_dropdwon_menu_t));
    func_cb.frm_main = func_clock_dropdown_menu_form_create();
#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE//是否使用滑动效果
    f_clock_dropdwon_menu_t *f_clock_dropdwon_menu = (f_clock_dropdwon_menu_t *)func_cb.f_cb;
    f_clock_dropdwon_menu->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .dir = 1,
#if (UTE_DRV_SCREEN_SHAPE==1)
        .page_size =  GUI_SCREEN_WIDTH+(GUI_SCREEN_HEIGHT-GUI_SCREEN_WIDTH)/2,
#else
        .page_size =  GUI_SCREEN_WIDTH,
#endif
        .page_count = CLOCK_DWON_PAGE_NUM,//项目config定义页面数量
        .jump_perc  = GUI_SCREEN_WIDTH/20,
    };
    compo_page_move_init(f_clock_dropdwon_menu->ptm,func_cb.frm_main->page_body,&info);
#endif
}

//时钟表盘下拉菜单退出处理
static void func_clock_sub_dropdown_exit(void)
{
#if UTE_MODULE_SCREENS_CLOCK_DWON_MENU_MOVE_MODE//是否使用滑动效果
    f_clock_dropdwon_menu_t *f_clock_dropdwon_menu = (f_clock_dropdwon_menu_t *)func_cb.f_cb;
    if (f_clock_dropdwon_menu->ptm)
    {
        func_free(f_clock_dropdwon_menu->ptm);
    }
#endif
}

//时钟表盘下拉菜单
void func_clock_dropdown_menu(void)
{
    printf("%s\n", __func__);
    func_clock_sub_dropdown_enter();
    while (func_cb.sta == FUNC_CLOCK_DROPDOWN_MENU)
    {
        func_clock_sub_dropdown_process();
        func_clock_sub_dropdown_message(msg_dequeue());
    }
    func_clock_sub_dropdown_exit();
}

#endif // UTE_MODULE_SCREENS_NEW_DWON_MENU_SUPPORT

