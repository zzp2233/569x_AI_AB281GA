#ifndef _FUNC_H
#define _FUNC_H

#include "common/msgbox.h"
#include "common/listbox.h"
#include "common/rotary.h"
#include "common/func_switching.h"
#include "func_clock.h"
#include "func_bt.h"
#include "common/func_idle.h"
#include "common/func_lowpwr.h"
#include "common/func_manage.h"
#include "common/func_update.h"
#include "func_bt_dut.h"
#include "func_music.h"
#include "func_usbdev.h"
#include "func_recorder.h"

#define TICK_IGNORE_KEY            700      //忽略700ms内的部分消息

//task number
enum {
    FUNC_NULL = 0,
    FUNC_MENU,                          //主菜单
    FUNC_MENUSTYLE,                     //主菜单样式选择
    FUNC_CLOCK,                         //时钟表盘
    FUNC_CLOCK_PREVIEW,                 //时钟表盘预览
    FUNC_SIDEBAR,                       //表盘右滑
    FUNC_CARD,                          //表盘上拉
    FUNC_HEARTRATE,                     //心率
    FUNC_BT,                            //蓝牙播放器(控制手机音乐)
	FUNC_COMPO_SELECT,                  //组件选择
    FUNC_COMPO_SELECT_SUB,              //组件选择子界面
    FUNC_BT_RING,                       //来电界面
    FUNC_BT_CALL,                       //通话界面
    FUNC_ALARM_CLOCK,			        //闹钟
    FUNC_ALARM_CLOCK_SUB_SET,			//闹钟--设置
    FUNC_ALARM_CLOCK_SUB_REPEAT,		//闹钟--重复
    FUNC_ALARM_CLOCK_SUB_EDIT,		    //闹钟--编辑
    FUNC_ALARM_CLOCK_SUB_POP,		    //闹钟--弹出
    FUNC_BLOOD_OXYGEN,			        //血氧
    FUNC_BLOODSUGAR,                    //血糖
    FUNC_BLOOD_PRESSURE,                //血压
    FUNC_BREATHE,				        //呼吸
    FUNC_BREATHE_SUB_MODE,              //呼吸--模式设置
    FUNC_BREATHE_SUB_TIME,              //呼吸--时间设置
    FUNC_CALCULATOR,			        //计算器
    FUNC_CAMERA,				        //相机
    FUNC_LIGHT,					        //亮度调节
    FUNC_TIMER,					        //定时器
    FUNC_SLEEP,					        //睡眠
    FUNC_STOPWATCH,				        //秒表
    FUNC_STOPWATCH_SUB_RECORD,			//秒表--秒表记录
    FUNC_WEATHER,				        //天气
    FUNC_GAME,                          //游戏
    FUNC_STYLE,                         //菜单风格
    FUNC_ALTITUDE,                      //海拔
    FUNC_MAP,                           //地图
    FUNC_MESSAGE,                       //消息
    FUNC_SCAN,                          //扫一扫
    FUNC_VOICE,                         //语音助手
    FUNC_COMPASS,                       //指南针
    FUNC_ADDRESS_BOOK,                  //电话簿
    FUNC_CALL_SUB_LINKMAN = FUNC_ADDRESS_BOOK,
    FUNC_SPORT,				            //运动
    FUNC_SPORT_CONFIG,                  //运动配置
    FUNC_SPORT_SUB_RUN,                 //运动--室内跑步
    FUNC_SPORT_SWITCH,                  //运动开启
    FUNC_CALL,                          //电话
    FUNC_CALL_SUB_RECORD,               //电话-最近通话
    FUNC_CALL_SUB_DIAL,                 //电话-拨打电话
    FUNC_FINDPHONE,                     //寻找手机
    FUNC_CALENDAER,                     //日历
    FUNC_VOLUME,                        //音量
    FUNC_ACTIVITY,                      //活动记录
    FUNC_FLASHLIGHT,                    //手电筒
    FUNC_SETTING,				        //设置
    FUNC_SET_SUB_DOUSING,               //设置--熄屏
    FUNC_SET_SUB_WRIST,                 //设置--抬腕
    FUNC_SET_SUB_DISTURD,               //设置--勿扰
    FUNC_DISTURD_SUB_SET,               //勿扰--时间设置
    FUNC_SET_SUB_SAV,                   //设置--声音与振动
    FUNC_SET_SUB_LANGUAGE,              //设置--语言
    FUNC_LANGUAGE = FUNC_SET_SUB_LANGUAGE,
    FUNC_SET_SUB_TIME,                  //设置--时间
    FUNC_TIME_SUB_CUSTOM,               //调整日期
    FUNC_SET_SUB_PASSWORD,              //设置--密码锁
    FUNC_PASSWORD_SUB_DISP,             //新密码锁设置
    FUNC_PASSWORD_SUB_SELECT,           //确认密码锁
    FUNC_SET_SUB_ABOUT,                 //设置--关于
    FUNC_SET_SUB_4G,                    //设置--4G
    FUNC_SET_SUB_RESTART,               //设置--重启
    FUNC_RESTART = FUNC_SET_SUB_RESTART,//重启
    FUNC_SET_SUB_RSTFY,                 //设置--恢复出厂
    FUNC_RSTFY = FUNC_SET_SUB_RSTFY,
    FUNC_SET_SUB_OFF,                   //设置--关机
    FUNC_OFF = FUNC_SET_SUB_OFF,        //关机
#if FUNC_MUSIC_EN
    FUNC_MUSIC,
#endif
    FUNC_BTHID,
#if FUNC_USBDEV_EN
    FUNC_USBDEV,
#endif
    FUNC_AUX,
    FUNC_SPDIF,
    FUNC_SPEAKER,
    FUNC_PWROFF,
    FUNC_SLEEPMODE,
    FUNC_I2S,
#if FUNC_RECORDER_EN
    FUNC_RECORDER,
#endif
    FUNC_BT_DUT,
    FUNC_BT_UPDATE,
    FUNC_IDLE,
    FUNC_CHARGE,
    FUNC_DEBUG_INFO,
    FUNC_SMARTSTACK,
    FUNC_MODEM_CALL,
    FUNC_MODEM_RING,
    FUNC_MESSAGE_REPLY,                  //消息发送
    FUNC_MIC_TEST,
    FUNC_OTA_UI_MODE,

    FUNC_MAX_NUM,           //用于计数

    FUNC_BIRD,
    FUNC_TETRIS,
    FUNC_TETRIS_START,
};

//task control block
typedef struct {
    void *f_cb;                                     //当前任务控制指针
    compo_form_t *frm_main;                         //当前窗体
    void *msg_cb;                                   //对话框控制指针
    u32 enter_tick;                                 //记录进入func的tick
    u8 sta;                                         //cur working task number
    u8 last;                                        //lask task number
    u8 menu_style;                                  //菜单样式
    u8 menu_idx;                                    //菜单编号
    int32_t menu_idx_angle;                         //菜单编号角度
    u8 sta_break;                                   //被中断的任务
    u8 sort_cnt;                                    //快捷任务个数
    u8 tbl_sort[MAX_FUNC_SORT_CNT];                 //快捷任务表
    u8  flag_sort       : 1,                        //已进入快捷任务
        flag_animation  : 1;                        //入场动画

    void (*mp3_res_play)(u32 addr, u32 len);        //各任务的语音播报函数接口
    void (*set_vol_callback)(u8 dir);               //设置音量的回调函数，用于各任务的音量事件处理。
} func_cb_t;


extern func_cb_t func_cb;
extern const u8 func_sort_table[];     //任务切换排序table

ALWAYS_INLINE void func_mp3_res_play(u32 addr, u32 len)
{
    if (func_cb.mp3_res_play) {
        func_cb.mp3_res_play(addr, len);
    }
}

ALWAYS_INLINE void func_set_vol_callback(u8 dir)
{
    if (func_cb.set_vol_callback) {
        func_cb.set_vol_callback(dir);
    }
}

void func_run(void);
compo_form_t *func_create_form(u8 sta);     //根据任务名创建窗体

void func_switch_prev(bool flag_auto);
void func_switch_next(bool flag_auto,bool flag_loop);

u8 get_funcs_total(void);
void func_process(void);
void func_message(size_msg_t msg);
void evt_message(size_msg_t msg);

void func_switch_to(u8 sta, u16 switch_mode);
void func_switching_to_menu(void);
void func_backing_to(void);                     //页面滑动回退功能
void func_back_to(void);                        //页面按键回退功能
u8 func_directly_back_to(void);                 //页面直接回退,无动画效果

#endif // _FUNC_H
