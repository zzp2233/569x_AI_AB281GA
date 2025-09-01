#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "ute_drv_motor.h"
#include "ute_drv_gsensor_common.h"
#include "ute_application_common.h"
#include "ute_module_heart.h"
#include "ute_drv_battery_common.h"
#include "ute_module_platform.h"
#include "ute_module_message.h"
#include "ute_module_newFactoryTest.h"

#if UTE_MODULE_NEW_FACTORY_MODULE_USE_OLD_AGING_TEST
#define TXT_SPACING    GUI_SCREEN_HEIGHT/13

static ute_new_factory_test_data_t *test_data;

enum
{
    MODE_BTN_ID = 1,
    TIME_BTN_ID,
    NO_BTN_ID,
    YES_BTN_ID,
    MODE_TXT_ID,
    TMIE_TXT_ID,

    SHAPE_BG_ID, //
    TXT_1_ID,    /// 发热丝状态
    TXT_2_ID,    /// 电池状态
    // TXT_3_ID,    /// 震动次数文字
    // TXT_4_ID,    /// 震动次数
    TXT_5_ID,    // 抽吸口数
    TXT_6_ID,    /// 发热丝1
    TXT_7_ID,    /// 发热丝2
    TXT_8_ID,    /// 运行时间
    SHAPE_1_ID,  // 发热丝1背景
    SHAPE_2_ID,  // 发热丝2背景
    SHAPE_3_ID,  // 单发双发背景
    SHAPE_4_ID,  // 铃声开关背景
    TXT_9_ID,    /// 单发双发文字
    BUTTON_1,    // 切换单双发按钮
    BUTTON_2,    // 铃声打开关闭按钮
    TXT_10_ID,   /// 阻值
    TXT_11_ID,   /// 阻值
    TXT_12_ID,   /// 输入模式
    TXT_13_ID,   //铃声打开关闭
    PASS_SHAPE_BG_ID,
    PASS_TXT_ID,
    PASS_BTN_ID,
};

ute_new_factory_test_data_t *data;
typedef struct f_ageing_t_
{
    uint32_t tick;
    u8 mode_flag;
    u8 play_flag;
    u8 time_flag;
    u8 count_num;
    bool test_state;
    u16 hour;
    u8 min;
    u8 sec;
} f_ageing_t;


#if !SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT
compo_form_t * func_ageing_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox = compo_textbox_create(frm, 15);
    compo_textbox_set(textbox,"Aging Test");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3);

    textbox = compo_textbox_create(frm, 10);
    compo_textbox_set(textbox,"Mode 1");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6);
    compo_textbox_set_forecolor(textbox, make_color(0x00,0xbb,0xff));
    compo_setid(textbox,MODE_TXT_ID);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,MODE_BTN_ID);

    textbox = compo_textbox_create(frm, 10);
    compo_textbox_set(textbox,"4 hours");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6);
    compo_textbox_set_forecolor(textbox, make_color(0x00,0xbb,0xff));
    compo_setid(textbox,TMIE_TXT_ID);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,TIME_BTN_ID);

    textbox = compo_textbox_create(frm, 2);
    compo_textbox_set(textbox,"NO");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_X, 60);
    compo_setid(btn,NO_BTN_ID);

    textbox = compo_textbox_create(frm, 3);
    compo_textbox_set(textbox,"YES");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_X, 60);
    compo_setid(btn,YES_BTN_ID);

    return frm;
}

compo_form_t * func_ageing_mode_create(u8 mode,u8 time)
{
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t * shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_GREEN );
    compo_setid(shape,SHAPE_BG_ID);

    compo_textbox_t *textbox = compo_textbox_create(frm,1);
    compo_textbox_set(textbox,"0");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING*2);
    compo_setid(textbox,TXT_1_ID);

    textbox = compo_textbox_create(frm,10);
    compo_textbox_set_align_center(textbox, false );
    if(mode==0)compo_textbox_set(textbox,"Mode 1");
    else if(mode==1)compo_textbox_set(textbox,"Mode 2");
    else if(mode==2)compo_textbox_set(textbox,"Mode 3");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*4-widget_text_get_area(textbox->txt).hei/2);

    textbox = compo_textbox_create(frm,40);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"Battery:Not Charging  --%");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*5.5-widget_text_get_area(textbox->txt).hei/2);
    compo_setid(textbox,TXT_2_ID);

    textbox = compo_textbox_create(frm,20);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"Heart: Not Worn");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*6.5-widget_text_get_area(textbox->txt).hei/2);
    compo_setid(textbox,TXT_3_ID);

    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"--BPM");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*7.5-widget_text_get_area(textbox->txt).hei/2);
    compo_setid(textbox,TXT_4_ID);

    textbox = compo_textbox_create(frm,9);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"G-sensor:");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*8.8-widget_text_get_area(textbox->txt).hei/2);

    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"X:--");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*9.8-widget_text_get_area(textbox->txt).hei/2);
    compo_setid(textbox,TXT_5_ID);

    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"Y:--");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING*9.8-widget_text_get_area(textbox->txt).hei/2);
    compo_setid(textbox,TXT_6_ID);

    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"Z:--");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/1.7, TXT_SPACING*10.8-widget_text_get_area(textbox->txt).hei/2);
    compo_setid(textbox,TXT_7_ID);

    textbox = compo_textbox_create(frm,20);
    compo_textbox_set(textbox,"Run Time:00:00:00");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING*12.2);
    compo_setid(textbox,TXT_8_ID);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE );
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_setid(shape,PASS_SHAPE_BG_ID);
    compo_shape_set_color(shape, COLOR_BLACK );
    compo_shape_set_visible(shape, false );


    textbox = compo_textbox_create(frm, 4 );
    compo_textbox_set(textbox,"PASS");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);
    compo_setid(textbox,PASS_TXT_ID);
    compo_textbox_set_visible(textbox, false );

    return frm;
}

///老化测试按键处理
static void func_test_mode_click(void)
{
    f_ageing_t *f_ageing = (f_ageing_t*)func_cb.f_cb;
    compo_textbox_t *textbox_mode = compo_getobj_byid(MODE_TXT_ID);
    compo_textbox_t *textbox_time = compo_getobj_byid(TMIE_TXT_ID);
    int id = compo_get_button_id();

    switch(id)
    {
        case MODE_BTN_ID:
            if(++f_ageing->mode_flag == 3)f_ageing->mode_flag = 0;

            if(f_ageing->mode_flag == 0)compo_textbox_set(textbox_mode,"Mode 1");
            else if(f_ageing->mode_flag == 1)compo_textbox_set(textbox_mode,"Mode 2");
            else if(f_ageing->mode_flag == 2)compo_textbox_set(textbox_mode,"Mode 3");
            break;
        case TIME_BTN_ID:
            if (++f_ageing->time_flag == 4)f_ageing->time_flag = 0;

            if(f_ageing->time_flag == 0)compo_textbox_set(textbox_time,"4 hours");
            else if(f_ageing->time_flag == 1)compo_textbox_set(textbox_time,"12 hours");
            else if(f_ageing->time_flag == 2)compo_textbox_set(textbox_time,"20 hours");
            else compo_textbox_set(textbox_time,"Forever");
            break;
        case NO_BTN_ID:
            func_back_to();
            break;
        case YES_BTN_ID:
        {
            f_ageing->test_state = true;
            compo_form_t *frm = func_cb.frm_main;
            if(frm !=  NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            uteModuleHeartStartSingleTesting(TYPE_HEART);

            if (f_ageing->mode_flag == 1)
            {
                uteDrvMotorStart(300,200,2);
            }
            else
            {
                uteDrvMotorStart(1000,1000,1);
            }

            func_cb.frm_main = func_ageing_mode_create(f_ageing->mode_flag,f_ageing->time_flag);
        }
        break;
    }

}
///老化测试功能消息处理
static void func_ageing_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_test_mode_click();
            break;
        case KL_BACK:
        {
            music_control(MUSIC_MSG_PAUSE);
            uint8_t ret = msgbox("Exit current test?", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if (ret == MSGBOX_RES_OK)
            {
                uteModuleHeartStopSingleTesting(TYPE_HEART);
                music_control(MUSIC_MSG_STOP);
                uteModulePlatformSystemReboot();
            }
            else
            {
                music_control(MUSIC_MSG_PLAY);
            }
        }
        break;
        default:
            evt_message(msg);
            break;
    }
}
///老化测试功能事件处理
static void func_ageing_process(void)
{
    f_ageing_t *f_ageing = (f_ageing_t*)func_cb.f_cb;

    if (sys_cb.gui_sleep_sta) ///唤醒屏幕
    {
        sys_cb.gui_need_wakeup = 1;
    }
    reset_sleep_delay_all();///重置休眠时间

    if(f_ageing->test_state == true)
    {
        if(tick_check_expire(f_ageing->tick, 1000))
        {
            char txt_buf[50];

            f_ageing->tick = tick_get();
            compo_shape_t   *shape    = compo_getobj_byid(SHAPE_BG_ID);///背景
            compo_textbox_t *textbox1 = compo_getobj_byid(TXT_1_ID);///序号
            compo_textbox_t *textbox2 = compo_getobj_byid(TXT_2_ID);///电池状态
            compo_textbox_t *textbox3 = compo_getobj_byid(TXT_3_ID);///心率佩戴状态
            compo_textbox_t *textbox4 = compo_getobj_byid(TXT_4_ID);///心率值
            compo_textbox_t *textbox5 = compo_getobj_byid(TXT_5_ID);///x轴
            compo_textbox_t *textbox6 = compo_getobj_byid(TXT_6_ID);///y轴
            compo_textbox_t *textbox7 = compo_getobj_byid(TXT_7_ID);///z轴
            compo_textbox_t *textbox8 = compo_getobj_byid(TXT_8_ID);///运行时间
            compo_shape_t *shape_pass = compo_getobj_byid(PASS_SHAPE_BG_ID);///运行时间
            compo_textbox_t *pass_txt = compo_getobj_byid(PASS_TXT_ID);///运行时间


            if(f_ageing->sec%3 == 0)compo_shape_set_color(shape, COLOR_GREEN);
            else if(f_ageing->sec%3 == 1)compo_shape_set_color(shape, COLOR_BLUE);
            else if(f_ageing->sec%3 == 2)
            {
                compo_shape_set_color(shape, COLOR_RED );
                if (f_ageing->mode_flag == 0)   //模式一不停震动
                {
                    uteDrvMotorStart(1000,1000,1);
                }
            }

            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",f_ageing->sec%10);
            compo_textbox_set(textbox1,txt_buf);

            memset(txt_buf,0,sizeof(txt_buf));
            if(uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE) ///获取充电状态
            {
                snprintf(txt_buf,sizeof(txt_buf),"Battery: Charging  %d%%",bsp_vbat_percent_get());
                compo_textbox_set(textbox2,txt_buf);
            }
            else
            {
                snprintf(txt_buf,sizeof(txt_buf),"Battery:Not Charging  %d%%",bsp_vbat_percent_get());
                compo_textbox_set(textbox2,txt_buf);
            }

            memset(txt_buf,0,sizeof(txt_buf));
            if(uteModuleHeartIsWear())
            {
                compo_textbox_set(textbox3,"Heart: Detecting");
                snprintf(txt_buf,sizeof(txt_buf),"%dBPM",uteModuleHeartGetHeartValue());
                compo_textbox_set(textbox4,txt_buf);
            }
            else
            {
                compo_textbox_set(textbox3,"Heart: Not Worn");
                compo_textbox_set(textbox4,"--BPM");
            }


            int16_t x, y, z;
            uteDrvGsensorCommonGetAccXyz(&x, &y, &z);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"X:%d",x);
            compo_textbox_set(textbox5,txt_buf);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"Y:%d",y);
            compo_textbox_set(textbox6,txt_buf);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"Z:%d",z);
            compo_textbox_set(textbox7,txt_buf);

            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"Run Time:%d:%02d:%02d",f_ageing->hour,f_ageing->min,f_ageing->sec);
            compo_textbox_set(textbox8,txt_buf);
#ifdef RES_BUF_RING_VOICE_DIAL_MP3
            if(f_ageing->mode_flag == 2 && !sys_cb.mp3_res_playing)///模式三 放音乐
            {
                func_bt_mp3_res_play(RES_BUF_RING_VOICE_DIAL_MP3, RES_LEN_RING_VOICE_DIAL_MP3);
            }
#endif // RES_BUF_RING_VOICE_DIAL_MP3
            if(++f_ageing->sec == 60)
            {
                f_ageing->sec = 0;
                if(++f_ageing->min == 60)
                {
                    f_ageing->min = 0;
                    f_ageing->hour++;
                }
            }

            if(f_ageing->time_flag != 3)///时间模式
            {
                if(f_ageing->sec == 0 && f_ageing->min == 0 && f_ageing->hour == (4 + f_ageing->time_flag * 8))
                {
                    f_ageing->test_state = false;
                    music_control(MUSIC_MSG_STOP);
                    uteModuleHeartStopSingleTesting(TYPE_HEART);
                    compo_textbox_set_visible(pass_txt, true);
                    compo_shape_set_visible(shape_pass, true);
                }
            }
        }
    }

    func_process();
}
#else
compo_form_t * func_ageing_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox = compo_textbox_create(frm, 4);
    compo_textbox_set(textbox,"老化测试");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/3);

    textbox = compo_textbox_create(frm, 3);
    compo_textbox_set(textbox,"模式一");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6);
    compo_textbox_set_forecolor(textbox, make_color(0x00,0xbb,0xff));
    compo_setid(textbox,MODE_TXT_ID);

    // compo_button_t *btn = compo_button_create(frm);
    // compo_button_set_location(btn, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 6, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei * 2);
    // compo_setid(btn, MODE_BTN_ID);

    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set(textbox,"4小时");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6);
    compo_textbox_set_forecolor(textbox, make_color(0x00,0xbb,0xff));
    compo_setid(textbox,TMIE_TXT_ID);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - GUI_SCREEN_CENTER_Y / 6, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, TIME_BTN_ID);

    textbox = compo_textbox_create(frm, 1);
    compo_textbox_set(textbox,"否");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_X, 60);
    compo_setid(btn,NO_BTN_ID);

    textbox = compo_textbox_create(frm, 1);
    compo_textbox_set(textbox,"是");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_X, 60);
    compo_setid(btn,YES_BTN_ID);

    return frm;
}

compo_form_t *func_ageing_mode_create(u8 mode, u8 time)
{
    compo_form_t *frm = compo_form_create(true);
    f_ageing_t *f_ageing = (f_ageing_t *)func_cb.f_cb;
    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_textbox_t *textbox;
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X - 57, TXT_SPACING * 2 + 16, 50, 25);
    compo_shape_set_color(shape, COLOR_GREEN);
    compo_setid(shape, SHAPE_1_ID);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X + 67, TXT_SPACING * 2 + 16, 50, 25);
    compo_shape_set_color(shape, COLOR_GREEN);
    compo_setid(shape, SHAPE_2_ID);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X / 2 +4,5 + TXT_SPACING * 10.8 +2- widget_text_get_area(textbox->txt).hei / 2, 88, 25);
    compo_shape_set_color(shape,make_color(4, 161, 251));
    compo_setid(shape, SHAPE_4_ID);

    textbox = compo_textbox_create(frm, 6);
    compo_textbox_set(textbox, "发热丝状态");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING * 2 - 15);
    compo_setid(textbox, TXT_1_ID);


    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, "电池:未充电  --%");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X / 5,5 + TXT_SPACING * 9.8 - widget_text_get_area(textbox->txt).hei / 2 );
    compo_setid(textbox, TXT_2_ID);

    // textbox = compo_textbox_create(frm, 10);
    // compo_textbox_set_align_center(textbox, false);
    // compo_textbox_set(textbox, "震动次数:");
    // compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X / 5, TXT_SPACING * 6.5 - widget_text_get_area(textbox->txt).hei / 2);
    // compo_setid(textbox, TXT_3_ID);

    // textbox = compo_textbox_create(frm, 6);
    // compo_textbox_set_align_center(textbox, false);
    // compo_textbox_set(textbox, "--次");
    // compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X / 5, TXT_SPACING * 7.5 - widget_text_get_area(textbox->txt).hei / 2);
    // compo_setid(textbox, TXT_4_ID);

    textbox = compo_textbox_create(frm, 9);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, "抽吸口数:");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X / 5, TXT_SPACING * 8.8 - widget_text_get_area(textbox->txt).hei / 2);

    textbox = compo_textbox_create(frm, 6);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, "X:--");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING * 8.8 - widget_text_get_area(textbox->txt).hei / 2);
    compo_setid(textbox, TXT_5_ID);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X +22, TXT_SPACING * 5.5 +6 - widget_text_get_area(textbox->txt).hei / 2, 50, 30);
    compo_shape_set_color(shape, make_color(4, 161, 251));
    compo_setid(shape, SHAPE_3_ID);

    compo_button_t *btn1 = compo_button_create(frm);
    compo_button_set_location(btn1, GUI_SCREEN_CENTER_X, TXT_SPACING * 5.5 -4- widget_text_get_area(textbox->txt).hei / 2, widget_text_get_area(textbox->txt).wid * 3, widget_text_get_area(textbox->txt).hei * 3);
    compo_setid(btn1, BUTTON_1);

    textbox = compo_textbox_create(frm, 6);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, get_gear_func() == 0 ? "单发" : "双发");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING * 5.5 - 4 - widget_text_get_area(textbox->txt).hei / 2);
    compo_setid(textbox, TXT_9_ID);

    textbox = compo_textbox_create(frm, 6);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, "输出模式：");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X / 5, TXT_SPACING * 5.5 - 4 - widget_text_get_area(textbox->txt).hei / 2);
    compo_setid(textbox, TXT_12_ID);

    textbox = compo_textbox_create(frm, 6);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, "烟弹阻值：");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X / 5, TXT_SPACING * 4 - widget_text_get_area(textbox->txt).hei / 2);
    compo_setid(textbox, TXT_10_ID);

    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, "--Ω");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING * 4 - widget_text_get_area(textbox->txt).hei / 2);
    compo_setid(textbox, TXT_11_ID);

    textbox = compo_textbox_create(frm, 1);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, "1");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X - 60, TXT_SPACING * 2 + 5);
    compo_setid(textbox, TXT_6_ID);

    textbox = compo_textbox_create(frm, 1);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox, "2");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X + 60, TXT_SPACING * 2 + 5);
    compo_setid(textbox, TXT_7_ID);

    // textbox = compo_textbox_create(frm,6);
    // compo_textbox_set_align_center(textbox, false );
    // compo_textbox_set(textbox,"Z:--");
    // compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/1.7, TXT_SPACING*10.8-widget_text_get_area(textbox->txt).hei/2);
    // compo_setid(textbox,TXT_7_ID);

    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set(textbox, "运行时间:00:00:00");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, TXT_SPACING * 12.2);
    compo_setid(textbox, TXT_8_ID);

    compo_button_t *btn2 = compo_button_create(frm);
    compo_button_set_location(btn2, GUI_SCREEN_CENTER_X / 5,5 + TXT_SPACING * 10.8 - widget_text_get_area(textbox->txt).hei / 2, widget_text_get_area(textbox->txt).wid * 3, widget_text_get_area(textbox->txt).hei * 3);
    compo_setid(btn2, BUTTON_2);

    textbox = compo_textbox_create(frm, 6);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,(uteModuleSmokeFactoryData.play_flag ==0) ? "铃声打开" : "铃声关闭");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X / 5,5 + TXT_SPACING * 10.8 - widget_text_get_area(textbox->txt).hei / 2 );
    compo_setid(textbox, TXT_13_ID);

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_setid(shape, PASS_SHAPE_BG_ID);
    compo_shape_set_color(shape, COLOR_BLACK);
    compo_shape_set_visible(shape, false);

    textbox = compo_textbox_create(frm, 4);
    compo_textbox_set(textbox, "PASS");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);
    compo_setid(textbox, PASS_TXT_ID);
    compo_textbox_set_visible(textbox, false);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, widget_text_get_area(textbox->txt).wid * 2, widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, PASS_BTN_ID);

    return frm;
}

/// 老化测试按键处理
static void func_test_mode_click(void)
{
    f_ageing_t *f_ageing = (f_ageing_t *)func_cb.f_cb;
    compo_textbox_t *textbox_mode = compo_getobj_byid(MODE_TXT_ID);
    compo_textbox_t *textbox_time = compo_getobj_byid(TMIE_TXT_ID);
    int id = compo_get_button_id();

    switch (id)
    {
        case MODE_BTN_ID:
            if (++f_ageing->mode_flag == 3)
                f_ageing->mode_flag = 0;

            if (f_ageing->mode_flag == 0)
                compo_textbox_set(textbox_mode, "模式一");
            else if (f_ageing->mode_flag == 1)
                compo_textbox_set(textbox_mode, "模式二");
            else if (f_ageing->mode_flag == 2)
                compo_textbox_set(textbox_mode, "模式三");
            break;
        case TIME_BTN_ID:
            if (++f_ageing->time_flag == 4)f_ageing->time_flag = 0;

            if(f_ageing->time_flag == 0)compo_textbox_set(textbox_time,"4小时");
            else if(f_ageing->time_flag == 1)compo_textbox_set(textbox_time,"12小时");
            else if(f_ageing->time_flag == 2)compo_textbox_set(textbox_time,"20小时");
            else compo_textbox_set(textbox_time,"永久");
            break;
        case NO_BTN_ID:
            func_back_to();
            break;
        case PASS_BTN_ID:
            if (f_ageing->test_state == false)
            {
                func_back_to();
            }

            break;
        case BUTTON_2:
            uteModuleSmokeFactoryData.play_flag = !uteModuleSmokeFactoryData.play_flag;
            // printf("BUTTON_2,play_flag=%d\n",uteModuleSmokeFactoryData.play_flag);
            break;
        case BUTTON_1:

#if ECIG_POWER_CONTROL
            ecig_cfg.smoke_position_swich = !ecig_cfg.smoke_position_swich;
#endif
            ecig_get_res();
            // printf("BUTTON_1BUTTON_1BUTTON_1ecig_cfg.smoke_position_swich=%d,%d\n", ecig_cfg.smoke_position_swich,ecig_get_res());
            break;
        case YES_BTN_ID:
        {
            // ute_new_factory_test_data_t *data;
            uteModuleNewFactoryTestSetMode(&data);
            data->mode = FACTORY_TEST_MODE_AGING;
            data->factoryAgingTestMode = FACTORY_AGING_TEST_MODE1;
            data->secondCount = 0;
            // data->isAgingTestEnd = false;
            f_ageing->test_state = true;
            compo_form_t *frm = func_cb.frm_main;
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            uteModuleHeartStartSingleTesting(TYPE_HEART);

            if (f_ageing->mode_flag == 1)
            {
                uteDrvMotorStart(300,200,2);
            }
            else
            {
                uteDrvMotorStart(1000,1000,1);
            }

            func_cb.frm_main = func_ageing_mode_create(f_ageing->mode_flag,f_ageing->time_flag);
        }
        break;
    }
}
/// 老化测试功能消息处理
static void func_ageing_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_test_mode_click();
            break;
        case KL_BACK:
        {
            music_control(MUSIC_MSG_PAUSE);
            uint8_t ret = msgbox("退出当前测试？", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if (ret == MSGBOX_RES_OK)
            {
                uteModuleHeartStopSingleTesting(TYPE_HEART);
                music_control(MUSIC_MSG_STOP);
                uteModulePlatformSystemReboot();
            }
            else
            {
                music_control(MUSIC_MSG_PLAY);
            }
        }
        break;
        default:
            evt_message(msg);
            break;
    }
}
/// 老化测试功能事件处理
static void func_ageing_process(void)
{
    f_ageing_t *f_ageing = (f_ageing_t *)func_cb.f_cb;

    if (sys_cb.gui_sleep_sta) /// 唤醒屏幕
    {
        sys_cb.gui_need_wakeup = 1;
    }
    reset_sleep_delay_all(); /// 重置休眠时间

    if (f_ageing->test_state == true)
    {
        if (tick_check_expire(f_ageing->tick, 1000))
        {
            char txt_buf[50];

            f_ageing->tick = tick_get();
            compo_shape_t   *shape    = compo_getobj_byid(SHAPE_BG_ID);///背景
            compo_textbox_t *textbox1 = compo_getobj_byid(TXT_1_ID);///序号
            compo_textbox_t *textbox2 = compo_getobj_byid(TXT_2_ID);///电池状态
            compo_textbox_t *textbox3 = compo_getobj_byid(TXT_3_ID);///心率佩戴状态
            compo_textbox_t *textbox4 = compo_getobj_byid(TXT_4_ID);///心率值
            compo_textbox_t *textbox5 = compo_getobj_byid(TXT_5_ID);///x轴
            compo_textbox_t *textbox6 = compo_getobj_byid(TXT_6_ID);///y轴
            compo_textbox_t *textbox7 = compo_getobj_byid(TXT_7_ID);///z轴
            compo_textbox_t *textbox8 = compo_getobj_byid(TXT_8_ID);///运行时间
            compo_shape_t *shape_pass = compo_getobj_byid(PASS_SHAPE_BG_ID);///运行时间
            compo_textbox_t *pass_txt = compo_getobj_byid(PASS_TXT_ID);///运行时间


            if(f_ageing->sec%3 == 0)compo_shape_set_color(shape, COLOR_GREEN);
            else if(f_ageing->sec%3 == 1)compo_shape_set_color(shape, COLOR_BLUE);
            else if(f_ageing->sec%3 == 2)
            {
                compo_shape_set_color(shape, COLOR_RED );
                if (f_ageing->mode_flag == 0)   //模式一不停震动
                {
                    uteDrvMotorStart(1000,1000,1);
                }
            }

            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",f_ageing->sec%10);
            compo_textbox_set(textbox1,txt_buf);

            memset(txt_buf,0,sizeof(txt_buf));
            if(uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE) ///获取充电状态
            {
                compo_shape_set_color(shape1, COLOR_GREEN);
                compo_shape_set_color(shape2, COLOR_GREEN);
            }
#endif
            if (get_gear_func() == 0)
            {
                compo_textbox_set(textbox9, "单发");
            }
            else
            {
                compo_textbox_set(textbox9, "双发");
            }

            memset(txt_buf,0,sizeof(txt_buf));
            if(uteModuleHeartIsWear())
            {
                compo_textbox_set(textbox3,"心率:检测中");
                snprintf(txt_buf,sizeof(txt_buf),"%d次/分",uteModuleHeartGetHeartValue());
                compo_textbox_set(textbox4,txt_buf);
            }
            else
            {
                snprintf(txt_buf, sizeof(txt_buf), "电池:未充电  %d%%", bsp_vbat_percent_get());
                compo_textbox_set(textbox2, txt_buf);
            }


            int16_t x, y, z;
            uteDrvGsensorCommonGetAccXyz(&x, &y, &z);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"X:%d",x);
            compo_textbox_set(textbox5,txt_buf);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"Y:%d",y);
            compo_textbox_set(textbox6,txt_buf);
            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"Z:%d",z);
            compo_textbox_set(textbox7,txt_buf);

            compo_textbox_set(textbox10, "烟弹阻值:");
            uint8_t fixed_res = ecig_get_res();

#ifdef RES_BUF_RING_VOICE_DIAL_MP3
            if(f_ageing->mode_flag == 2 && !sys_cb.mp3_res_playing)///模式三 放音乐
            {
                func_bt_mp3_res_play(RES_BUF_RING_VOICE_DIAL_MP3, RES_LEN_RING_VOICE_DIAL_MP3);
            }
#endif // RES_BUF_RING_VOICE_DIAL_MP3
            if(++f_ageing->sec == 60)
            {
                f_ageing->sec = 0;
                if(++f_ageing->min == 60)
                {
                    f_ageing->min = 0;
                    f_ageing->hour++;
                }
            }

            if(f_ageing->time_flag != 3)///时间模式
            {
                if(f_ageing->sec == 0 && f_ageing->min == 0 && f_ageing->hour == (4 + f_ageing->time_flag * 8))
                {
                    f_ageing->test_state = false;
                    music_control(MUSIC_MSG_STOP);
                    uteModuleHeartStopSingleTesting(TYPE_HEART);
                    compo_textbox_set_visible(pass_txt, true);
                    compo_shape_set_visible(shape_pass, true);
                }
            }
        }
    }

    func_process();
}
#endif //SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT

///进入老化测试功能
static void func_ageing_enter(void)
{

    uteModuleNewFactoryTestResetParam();
    uteModuleNewFactoryTestSetMode(&test_data);
    test_data->mode = FACTORY_TEST_MODE_AGING;

    func_cb.f_cb = func_zalloc(sizeof(f_ageing_t));
    func_cb.frm_main = func_ageing_create();
    get_gear_func();
}
/// 退出老化测试功能
static void func_ageing_exit(void)
{
    uteModuleSmokeFactoryData.play_flag=1;
    music_control(MUSIC_MSG_STOP);
    uteModuleNewFactoryTestResetParam();
    func_cb.last = FUNC_AGEING;
}
/// 老化测试功能
void func_ageing(void)
{
    printf("%s\n", __func__);
    func_ageing_enter();
    while (func_cb.sta == FUNC_AGEING)
    {
        func_ageing_process();
        func_ageing_message(msg_dequeue());
    }
    func_ageing_exit();
}

#endif

