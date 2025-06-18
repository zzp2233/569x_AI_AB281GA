#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_gui_common.h"
#include "ute_module_watchonline.h"
#include "ute_project_config.h"
#include "ute_module_log.h"
#include "ute_module_systemtime.h"
#include "ute_drv_motor.h"
#include "ute_application_common.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

extern u16 func_clock_preview_get_type(void);

#define DIALPLATE_NUM               uteModuleGuiCommonGetCurrWatchMaxIndex()
#define DIALPLATE_BTF_IDX           UTE_WATCHS_DIALPLATE_BTF_INDEX        //蝴蝶表盘默认最后一个
#define DIALPLATE_CUBE_IDX          UTE_WATCHS_DIALPLATE_CUBE_INDEX        //立方体表盘默认倒数第二个
#define DIALPLATE_LIGHT_CUBE_IDX    UTE_WATCHS_DIALPLATE_LIGHT_CUBE_INDEX        //立方体表盘默认倒数第二个

u32 dialplate_info[UTE_MODULE_SCREENS_WATCH_CNT_MAX + UTE_MODULE_WATCHONLINE_MULTIPLE_MAX_CNT] = UTE_MODULE_WATCHS_SORT_ADDRESS_ARRAYS;

//表盘快捷按钮编号表 对应表盘工具
const u8 quick_btn_tbl[] =
{
    [0]     = FUNC_NULL,                    //空
#if UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
    [1]     = FUNC_HEARTRATE,               //心率
#endif // UTE_MODULE_SCREENS_HEARTRATE_SUPPORT
#if UTE_MODULE_SCREENS_MUSIC_SUPPORT
    [2]     = FUNC_BT,                      //音乐
#endif // UTE_MODULE_SCREENS_MUSIC_SUPPORT
#if UTE_MODULE_SCREENS_ALARM_SUPPORT
    [3]     = FUNC_ALARM_CLOCK,             //闹钟
#endif // UTE_MODULE_SCREENS_ALARM_SUPPORT
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    [4]     = FUNC_BLOOD_OXYGEN,            //血氧
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT
    [5]     = FUNC_NULL,                    //血糖
    [6]     = FUNC_NULL,          //血压
#if UTE_MODULE_SCREENS_BREATHE_SUPPORT
    [7]     = FUNC_BREATHE,                 //呼吸
#endif // UTE_MODULE_SCREENS_BREATHE_SUPPORT
#if UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
    [8]     = FUNC_CALCULATOR,              //计算器
#endif // UTE_MODULE_SCREENS_CALCULATOR_SUPPORT
#if UTE_MODULE_SCREENS_CAMERA_SUPPORT
    [9]     = FUNC_CAMERA,                  //相机
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
#if UTE_MODULE_SCREENS_TIMER_SUPPORT
    [10]    = FUNC_TIMER,                   //定时器
#endif // UTE_MODULE_SCREENS_TIMER_SUPPORT
#if UTE_MODULE_SCREENS_SLEEP_SUPPORT
    [11]    = FUNC_SLEEP,                   //睡眠
#endif // UTE_MODULE_SCREENS_SLEEP_SUPPORT
#if UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
    [12]    = FUNC_STOPWATCH,               //秒表
#endif // UTE_MODULE_SCREENS_STOPWATCH_SUPPORT
#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
    [13]    = FUNC_WEATHER,                 //天气
#endif // UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if UTE_MODULE_SCREENS_GAME_SUPPORT
    [14]    = FUNC_GAME,                    //游戏
#else
    [14]    = FUNC_NULL,                    //游戏
#endif // UTE_MODULE_SCREENS_GAME_SUPPORT
#if UTE_MODULE_SCREENS_STYLE_SUPPORT
    [15]    = FUNC_STYLE,                   //菜单风格
#endif // UTE_MODULE_SCREENS_STYLE_SUPPORT
    [16]    = FUNC_ALTITUDE,                //海拔
    [17]    = FUNC_MAP,                     //地图
#if UTE_MODULE_SCREENS_MESSAGE_SUPPORT
    [18]    = FUNC_MESSAGE,                 //消息
#endif // UTE_MODULE_SCREENS_MESSAGE_SUPPORT
#if UTE_MODULE_SCREENS_SCAN_SUPPORT
    [19]    = FUNC_SCAN,                    //扫一扫
#endif // UTE_MODULE_SCREENS_SCAN_SUPPORT
#if UTE_MODULE_SCREENS_VOICE_SUPPORT
    [20]    = FUNC_VOICE,                   //语音助手
#endif // UTE_MODULE_SCREENS_VOICE_SUPPORT
#if SECURITY_PAY_EN
    [21]    = FUNC_ALIPAY,                  //支付宝
#else
    [21]    = FUNC_NULL,                    //支付宝
#endif // SECURITY_PAY_EN
//    [22]    = FUNC_COMPASS,                 //指南针
#if UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
    [23]    = FUNC_ADDRESS_BOOK,            //电话簿
#endif // UTE_MODULE_SCREENS_LINK_MAN_SUPPORT
#if UTE_MODULE_SCREENS_SPORT_SUPPORT
    [24]    = FUNC_SPORT,                   //运动
#endif // UTE_MODULE_SCREENS_SPORT_SUPPORT
#if UTE_MODULE_SCREENS_CALL_SUPPORT
    [25]    = FUNC_CALL,                    //电话
#endif // UTE_MODULE_SCREENS_CALL_SUPPORT
    [26]    = FUNC_FINDPHONE,               //寻找手机
#if UTE_MODULE_SCREENS_CALENDAER_SUPPORT
    [27]    = FUNC_CALENDAER,               //日历
#endif // UTE_MODULE_SCREENS_CALENDAER_SUPPORT
#if UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
    [28]    = FUNC_ACTIVITY,                //活动记录
#endif // UTE_MODULE_SCREENS_ACTIVITY_SUPPORT
#if UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
    [29]    = FUNC_FLASHLIGHT,              //手电筒
#endif // UTE_MODULE_SCREENS_FLASHLIGHT_SUPPORT
#if UTE_MODULE_SCREENS_SETTING_SUPPORT
    [30]    = FUNC_SETTING,                 //设置
#endif // UTE_MODULE_SCREENS_SETTING_SUPPORT
};


int compo_get_animation_id(void);
void compo_animation_manual_next(compo_animation_t *animation);
void func_switch_to(u8 sta, u16 switch_mode);

u8 func_clock_get_max_dialplate_num(void)
{
    return uteModuleGuiCommonGetCurrWatchMaxIndex();
}

u32 func_clock_get_dialplate_info(u8 index)
{
    return dialplate_info[index];
}

//单击按钮
static void func_clock_button_click(void)
{
    u16 btn_id = compo_get_button_id();
    u16 animation_id = compo_get_animation_id();
    if (btn_id)
    {
        func_switch_to(quick_btn_tbl[btn_id], FUNC_SWITCH_LR_ZOOM_LEFT | FUNC_SWITCH_AUTO);
    }
    else if (animation_id)
    {
        compo_animation_t *animation = compo_getobj_byid(animation_id);
        if (animation->bond_data == COMPO_BOND_IMAGE_CLICK)
        {
            compo_animation_manual_next(animation);
        }
        else if (animation->bond_data == COMPO_BOND_ANIMATION_AREA_CLICK)
        {
            compo_animation_click_set_vis(animation);
        }
    }
}

compo_form_t *func_clock_form_create(void)
{
    compo_form_t *frm;

    uteModuleGuiCommonGetCurrWatchIndex(&sys_cb.dialplate_index);
#if UTE_MODULE_WATCHONLINE_SUPPORT
    if (uteModuleWatchOnlineGetVailWatchCnt())
    {
        for (uint8_t i = 0; i < uteModuleWatchOnlineGetVailWatchCnt(); i++)
        {
            dialplate_info[UTE_MODULE_SCREENS_WATCH_CNT_MAX + i] = uteModuleWatchOnlineGetBaseAddress(i);
            UTE_MODULE_LOG(UTE_LOG_WATCHONLINE_LVL, "%s,watch online index %d: 0x%x", __func__, i, dialplate_info[UTE_MODULE_SCREENS_WATCH_CNT_MAX + i]);
        }
    }
#endif

    if (sys_cb.dialplate_index > uteModuleGuiCommonGetCurrWatchMaxIndex() - 1)
    {
        sys_cb.dialplate_index = DEFAULT_WATCH_INDEX;
    }

    switch (sys_cb.dialplate_index)
    {
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
        case DIALPLATE_BTF_IDX:
        {
            frm = func_clock_butterfly_form_create();
        }
        break;
#endif
#if UTE_WATCHS_CUBE_DIAL_SUPPORT
        case DIALPLATE_CUBE_IDX:
        {
            frm = func_clock_cube_form_create();
        }
        break;
#endif
#if UTE_WATCHS_LIGHT_CUBE_DIAL_SUPPORT
        case DIALPLATE_LIGHT_CUBE_IDX:
        {
            frm = func_clock_light_cube_form_create();
        }
        break;
#endif
        default:
        {
            u32 base_addr = dialplate_info[sys_cb.dialplate_index];
#if UTE_MODULE_WATCH_PHOTO_SUPPORT
            watchConfig_t watchConfig;
            uteModulePlatformFlashNorRead((uint8_t *)&watchConfig, base_addr, sizeof(watchConfig_t));
            printf("watchConfig.snNo = %d\n", watchConfig.snNo);
            if (watchConfig.snNo == UTE_MODULE_WATCH_PHOTO_DEFAULT_ID && uteModuleWatchOnlineIsHasPhoto())
            {
                frm = func_clock_photo_form_create();
            }
            else
#endif
            {
                u16 compo_num = bsp_uitool_header_phrase(base_addr);
                if (!compo_num)
                {
                    halt(HALT_GUI_DIALPLATE_HEAD);
                }
                frm = compo_form_create(true);
                bsp_uitool_create(frm, base_addr, compo_num);
            }
        }
        break;
    }

    return frm;
}


//子功能公共事件处理
void func_clock_sub_process(void)
{
    func_process();                                     //刷新UI
}

//子功能公共消息处理
void func_clock_sub_message(size_msg_t msg)
{
    func_message(msg);
}

//时钟表盘功能事件处理
static void func_clock_process(void)
{
#if UTE_WATCHS_CUBE_DIAL_SUPPORT
    func_clock_cube_process();
#endif
#if UTE_WATCHS_LIGHT_CUBE_DIAL_SUPPORT
    func_clock_light_cube_process();
#endif
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    func_clock_butterfly_process();
#endif
    func_process();                                  //刷新UI
}


//重建表盘界面
void func_clock_recreate_dial(void)
{
    printf("dialplate index:%d\n", sys_cb.dialplate_index);
    if(func_cb.sta == FUNC_CLOCK)
    {
        compo_form_destroy(func_cb.frm_main);
        func_cb.frm_main = func_clock_form_create();
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
        if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
        {
            sys_cb.dialplate_btf_ready = true;
#if UTE_WATCH_BUTTERFLY_DIAL_TE_MODE_DEFAULT_SUPPORT
            tft_set_temode(DEFAULT_TE_MODE);
#else
            tft_set_temode(0);
#endif
        }
#endif
    }
}

//时钟表盘功能消息处理
static void func_clock_message(size_msg_t msg)
{

#if UTE_WATCHS_CUBE_DIAL_SUPPORT
    if(sys_cb.dialplate_index == DIALPLATE_CUBE_IDX)
    {
        if(func_clock_cube_message(msg))
        {
            TRACE("func_clock_cube_message return\n");
            return;
        }
    }
#endif
#if UTE_WATCHS_LIGHT_CUBE_DIAL_SUPPORT
    if(sys_cb.dialplate_index == DIALPLATE_LIGHT_CUBE_IDX)
    {
        if(func_clock_light_cube_message(msg))
        {
            TRACE("func_clock_cube_message return\n");
            return;
        }
    }

#endif

#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    if(sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        if(func_clock_butterfly_message(msg))
        {
            TRACE("func_clock_butterfly_message return\n");
            return;
        }
    }
#endif

    switch (msg)
    {
        case MSG_CTP_SHORT_UP:
            if(UTE_CUI_SCREEN_WATCHDIAL_PULLUP != FUNC_NULL)
            {
                func_cb.pullup_sta = UTE_CUI_SCREEN_WATCHDIAL_PULLUP;
                func_switch_to(UTE_CUI_SCREEN_WATCHDIAL_PULLUP, FUNC_SWITCH_MENU_PULLUP_UP);  //上拉卡片界面
            }
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
            func_clock_butterfly_set_light_visible(false);
            if (func_cb.sta == FUNC_CLOCK)
            {
                func_clock_butterfly_set_light_visible(true);
            }
#endif
            break;

        case MSG_CTP_SHORT_RIGHT:
#if UTE_MODULE_SCREENS_CLOCK_SUB_SIDE_SUPPORT
            func_clock_sub_side();
#else
            if(UTE_CUI_SCREEN_WATCHDIAL_LEFT != FUNC_NULL)
            {
                func_cb.left_sta = UTE_CUI_SCREEN_WATCHDIAL_LEFT;
                if(UTE_CUI_SCREEN_WATCHDIAL_LEFT == FUNC_MENU)
                {
                    func_switch_to_menu();
                }
                else
                {
                    func_switch_to(UTE_CUI_SCREEN_WATCHDIAL_LEFT, FUNC_SWITCH_LR_ZOOM_RIGHT);
                }

            }

#endif
            break;

        case MSG_CTP_SHORT_DOWN:
#if UTE_MODULE_SCREENS_NEW_DWON_MENU_SUPPORT
            if(UTE_CUI_SCREEN_WATCHDIAL_DOWN != FUNC_NULL)
            {
                func_cb.down_sta = UTE_CUI_SCREEN_WATCHDIAL_DOWN;
                func_switch_to(UTE_CUI_SCREEN_WATCHDIAL_DOWN, FUNC_SWITCH_MENU_DROPDOWN_DOWN);  //上拉卡片界面
            }
#elif UTE_MODULE_SCREENS_DWON_MENU_SUPPORT
            func_clock_sub_dropdown();              //下拉菜单
#endif
            break;

        case MSG_CTP_CLICK:
            func_clock_button_click();
            break;

        case MSG_SYS_500MS: //秒跳动处理

            break;

#if DRV_ENCODER_KEYS_WATCHMAIN_SCREEN_SWITCHOVER_SUPPORT
        case MSG_QDEC_FORWARD:                              //向前滚动菜单
        {
            msg_queue_detach(MSG_QDEC_FORWARD, 0); // 防止不停滚动
#if UTE_MODULE_ENCODER_SWITCH_WATCHMAIN_LOCK_SUPPORT
            if (uteModuleGuiCommonGetEncoderKeysSwitchoverWatchMainLock())
            {
                func_message(msg);
            }
            else
#endif
            {
                sys_cb.dialplate_index++;
                if (sys_cb.dialplate_index > uteModuleGuiCommonGetCurrWatchMaxIndex() - 1)
                {
                    sys_cb.dialplate_index = 0;
                }
                uteModuleGuiCommonSetCurrWatchIndex(sys_cb.dialplate_index);
                func_clock_recreate_dial();
            }
        }
        break;

        case MSG_QDEC_BACKWARD:                             //向后滚动菜单
        {
            msg_queue_detach(MSG_QDEC_BACKWARD, 0);  //防止不停滚动
#if UTE_MODULE_ENCODER_SWITCH_WATCHMAIN_LOCK_SUPPORT
            if (uteModuleGuiCommonGetEncoderKeysSwitchoverWatchMainLock())
            {
                func_message(msg);
            }
            else
#endif
            {
                if(sys_cb.dialplate_index > 0)
                {
                    sys_cb.dialplate_index --;
                }
                else
                {
                    sys_cb.dialplate_index = uteModuleGuiCommonGetCurrWatchMaxIndex() - 1;
                }
                uteModuleGuiCommonSetCurrWatchIndex(sys_cb.dialplate_index);
                func_clock_recreate_dial();
            }
        }
        break;
#endif

        case MSG_CTP_LONG:
            if (func_clock_preview_get_type() == PREVIEW_ROTARY_STYLE)
            {
                func_cb.sta = FUNC_CLOCK_PREVIEW;
            }
            else
            {
                func_switch_to(FUNC_CLOCK_PREVIEW, FUNC_SWITCH_ZOOM_FADE_ENTER | FUNC_SWITCH_AUTO);                    //切换回主时钟
            }
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            break;

        default:
            func_message(msg);
            break;
    }
}
//进入时钟表盘功能
static void func_clock_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_clock_t));
    func_cb.frm_main = func_clock_form_create();
    func_cb.flag_animation = true;
    func_cb.left_sta = FUNC_NULL;
    func_cb.pullup_sta = FUNC_NULL;
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        sys_cb.dialplate_btf_ready = true;
#if UTE_WATCH_BUTTERFLY_DIAL_TE_MODE_DEFAULT_SUPPORT
        tft_set_temode(DEFAULT_TE_MODE);
#else
        tft_set_temode(0);
#endif
    }
#endif
}

//退出时钟表盘功能
static void func_clock_exit(void)
{
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    if (sys_cb.dialplate_index == DIALPLATE_BTF_IDX)
    {
        sys_cb.dialplate_btf_ready = false;
    }
#endif

    if(tft_cb.te_mode != DEFAULT_TE_MODE)
    {
        tft_set_temode(DEFAULT_TE_MODE);
    }

    func_cb.last = FUNC_CLOCK;

    if(!uteApplicationCommonIsHasConnectOurApp())//防止上电直接进入表盘特殊情况
    {
        sys_cb.power_on_state=true;//开机绑定流程
    }
}

//时钟表盘功能
void func_clock(void)
{
    printf("%s\n", __func__);
    func_clock_enter();
    while (func_cb.sta == FUNC_CLOCK)
    {
        func_clock_process();
        func_clock_message(msg_dequeue());
    }
    func_clock_exit();
}
