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

#if UTE_MODULE_NEW_FACTORY_MODULE_USE_OLD_AGING_TEST
#define TXT_SPACING    GUI_SCREEN_HEIGHT/13

extern ute_drv_motor_t uteDrvMotorData;

enum
{
    MODE_BTN_ID=1,
    TIME_BTN_ID,
    NO_BTN_ID,
    YES_BTN_ID,
    MODE_TXT_ID,
    TMIE_TXT_ID,

    SHAPE_BG_ID,///背景
    TXT_1_ID,///序号
    TXT_2_ID,///电池状态
    TXT_3_ID,///心率佩戴状态
    TXT_4_ID,///心率值
    TXT_5_ID,///x轴
    TXT_6_ID,///y轴
    TXT_7_ID,///z轴
    TXT_8_ID,///运行时间

    PASS_SHAPE_BG_ID,
    PASS_TXT_ID,
};



typedef struct f_ageing_t_
{
    uint32_t tick;
    u8 mode_flag;
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
                uteDrvMotorData.durationTimeMsec = 300;
                uteDrvMotorData.intervalTimeMsec = 200;
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_START, 2);
            }
            else
            {
                uteDrvMotorData.durationTimeMsec = 1000;
                uteDrvMotorData.intervalTimeMsec = 1000;
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_START, 1);
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
                    uteDrvMotorData.durationTimeMsec = 1000;
                    uteDrvMotorData.intervalTimeMsec = 1000;
                    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_START, 1);
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

            if(f_ageing->mode_flag == 2 && !sys_cb.mp3_res_playing)///模式三 放音乐
            {
                func_bt_mp3_res_play(RES_BUF_RING_VOICE_DIAL_MP3, RES_LEN_RING_VOICE_DIAL_MP3);
            }

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

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2,GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,MODE_BTN_ID);

    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set(textbox,"4小时");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6);
    compo_textbox_set_forecolor(textbox, make_color(0x00,0xbb,0xff));
    compo_setid(textbox,TMIE_TXT_ID);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/3, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,TIME_BTN_ID);

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

    textbox = compo_textbox_create(frm,3);
    compo_textbox_set_align_center(textbox, false );
    if(mode==0)compo_textbox_set(textbox,"模式一");
    else if(mode==1)compo_textbox_set(textbox,"模式二");
    else if(mode==2)compo_textbox_set(textbox,"模式三");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*4-widget_text_get_area(textbox->txt).hei/2);

    textbox = compo_textbox_create(frm,20);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"电池:未充电  --%");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*5.5-widget_text_get_area(textbox->txt).hei/2);
    compo_setid(textbox,TXT_2_ID);

    textbox = compo_textbox_create(frm,10);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"心率:未佩戴");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X/5, TXT_SPACING*6.5-widget_text_get_area(textbox->txt).hei/2);
    compo_setid(textbox,TXT_3_ID);

    textbox = compo_textbox_create(frm,6);
    compo_textbox_set_align_center(textbox, false );
    compo_textbox_set(textbox,"--次/分");
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
    compo_textbox_set(textbox,"运行时间:00:00:00");
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

            if(f_ageing->mode_flag == 0)compo_textbox_set(textbox_mode,"模式一");
            else if(f_ageing->mode_flag == 1)compo_textbox_set(textbox_mode,"模式二");
            else if(f_ageing->mode_flag == 2)compo_textbox_set(textbox_mode,"模式三");
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
                uteDrvMotorData.durationTimeMsec = 300;
                uteDrvMotorData.intervalTimeMsec = 200;
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_START, 2);
            }
            else
            {
                uteDrvMotorData.durationTimeMsec = 1000;
                uteDrvMotorData.intervalTimeMsec = 1000;
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_START, 1);
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
                    uteDrvMotorData.durationTimeMsec = 1000;
                    uteDrvMotorData.intervalTimeMsec = 1000;
                    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_DRV_MOTOR_START, 1);
                }
            }

            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",f_ageing->sec%10);
            compo_textbox_set(textbox1,txt_buf);

            memset(txt_buf,0,sizeof(txt_buf));
            if(uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE) ///获取充电状态
            {
                snprintf(txt_buf,sizeof(txt_buf),"电池:充电中  %d%%",bsp_vbat_percent_get());
                compo_textbox_set(textbox2,txt_buf);
            }
            else
            {
                snprintf(txt_buf,sizeof(txt_buf),"电池:未充电  %d%%",bsp_vbat_percent_get());
                compo_textbox_set(textbox2,txt_buf);
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
                compo_textbox_set(textbox3,"心率:未佩戴");
                compo_textbox_set(textbox4,"--次/分");
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
            snprintf(txt_buf,sizeof(txt_buf),"运行时间:%d:%02d:%02d",f_ageing->hour,f_ageing->min,f_ageing->sec);
            compo_textbox_set(textbox8,txt_buf);

            if(f_ageing->mode_flag == 2 && !sys_cb.mp3_res_playing)///模式三 放音乐
            {
                func_bt_mp3_res_play(RES_BUF_RING_VOICE_DIAL_MP3, RES_LEN_RING_VOICE_DIAL_MP3);
            }

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
    func_cb.f_cb = func_zalloc(sizeof(f_ageing_t));
    func_cb.frm_main = func_ageing_create();
}
///退出老化测试功能
static void func_ageing_exit(void)
{
    func_cb.last = FUNC_AGEING;
}
///老化测试功能
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

