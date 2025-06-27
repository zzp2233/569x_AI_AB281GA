#include "include.h"
#include "func.h"
#include "ute_module_notdisturb.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//组件ID
enum
{
    COMPO_ID_BTN_SURE=1,
    COMPO_ID_BTN_AM_BG,
    COMPO_ID_BTN_PM_BG,
    COMPO_ID_TXT_AM_BG,
    COMPO_ID_TXT_PM_BG,
    COMPO_ID_BTN_HOUR_BG,
    COMPO_ID_BTN_MIN_BG,

    COMPO_ID_TXT_HOUR_1,
    COMPO_ID_TXT_HOUR_2,
    COMPO_ID_TXT_HOUR_3,
    COMPO_ID_TXT_HOUR_4,
    COMPO_ID_TXT_HOUR_5,

    COMPO_ID_TXT_MIN_1,
    COMPO_ID_TXT_MIN_2,
    COMPO_ID_TXT_MIN_3,
    COMPO_ID_TXT_MIN_4,
    COMPO_ID_TXT_MIN_5,
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT \
    || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    COMPO_ID_BTN_PM_AM,
#endif
};
typedef struct f_disturd_sub_set_t_
{
    bool time_scale;
    bool touch_flag;
    bool set_hour_flag;
    bool am_pm_flag;
    u8 disturd_hour[5];
    u8 disturd_min[5];
    s32 move_dy;
    s32 move_dy_data;
    u8 hour;
    u8 min;
    u8 hour_old;
    u8 min_old;
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT \
    || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    u8 set_num_flag;
    s32 move_am_pm_dy;
#endif
} f_disturd_sub_set_t;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#define TXT_SPACING    (105-62)
#define CENTER_TXT_Y   (130/2)
#define TXT_12_HOUR_X  (85+58/2)
#define TXT_12_MIN_X   (172+58/2)
#define TXT_24_HOUR_X  (4+100/2)
#define TXT_24_MIN_X   (168+34/2)

const uint16_t TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

/*函数功能：获取设置时间（上下 & 上上下下 ）的数*/
/*入口参数：mode->时间参数 ，0为24小时制 ，1为12小时制*/
/*入口参数：timer->设置时间*/
/*入口参数：hour_flag->设置小时或分钟 0小时 1分钟*/
/*返回参数：timer_data->返回timer（上下 & 上上下下 ）的时间1~5数组*/
static void func_disturd_sub_get_timer(bool mode,bool hour_flag,u8 timer,s8 *timer_data)
{
    s8 num=-2;

    if(mode == 0)
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+24+(num))%24;
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
    else
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+12+(num))%12;
                if(timer_data[i] == 0)
                {
                    timer_data[i] = 12;
                }
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }

    }

}
/*函数功能：12小时制刷新按钮颜色*/
static void func_disturd_sub_set_refresh(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    compo_button_t * btn_am = compo_getobj_byid(COMPO_ID_BTN_AM_BG);
    compo_button_t * btn_pm = compo_getobj_byid(COMPO_ID_BTN_PM_BG);
    compo_textbox_t *txt_am = compo_getobj_byid(COMPO_ID_TXT_AM_BG);
    compo_textbox_t *txt_pm = compo_getobj_byid(COMPO_ID_TXT_PM_BG);

    if(f_disturd_set->am_pm_flag == true)
    {
        compo_button_set_bgimg(btn_am, UI_BUF_I330001_ALARM_02_BIN);
        compo_button_set_bgimg(btn_pm, UI_BUF_I330001_ALARM_00_BIN);
        compo_textbox_set_forecolor(txt_am, COLOR_WHITE);
        compo_textbox_set_forecolor(txt_pm, COLOR_GRAY);
    }
    else
    {
        compo_button_set_bgimg(btn_pm, UI_BUF_I330001_ALARM_02_BIN);
        compo_button_set_bgimg(btn_am, UI_BUF_I330001_ALARM_00_BIN);
        compo_textbox_set_forecolor(txt_pm, COLOR_WHITE);
        compo_textbox_set_forecolor(txt_am, COLOR_GRAY);
    }
}
/*函数功能：初始化数据*/
static void func_disturd_sub_time_init(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    f_disturd_set->time_scale = uteModuleSystemtime12HOn();
    if(sys_cb.disturd_sel)
    {
        f_disturd_set->hour = sys_cb.disturd_end_time_sec/3600;
        f_disturd_set->min = sys_cb.disturd_end_time_sec%3600/60;

    }
    else
    {
        f_disturd_set->hour = sys_cb.disturd_start_time_sec/3600;
        f_disturd_set->min = sys_cb.disturd_start_time_sec%3600/60;
    }

    if(f_disturd_set->time_scale)//12小时制
    {
        if(f_disturd_set->hour && f_disturd_set->hour<=12)
        {
            f_disturd_set->am_pm_flag = true;
        }
        else
        {
            f_disturd_set->am_pm_flag = false;
        }
        f_disturd_set->hour %=12;
    }
    else
    {
        f_disturd_set->hour %=24;
    }
    f_disturd_set->hour_old = f_disturd_set->hour;
    f_disturd_set->min_old  = f_disturd_set->min;

    if(f_disturd_set->time_scale)
    {
        func_disturd_sub_set_refresh();
    }
    // printf("hour1:%d\n",sys_cb.disturd_start_time_sec);
}
/*函数功能：设置勿扰时间*/
static void func_disturd_sub_set_time(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    u8 hour = f_disturd_set->hour;
    u8 min  = f_disturd_set->min;


    if(f_disturd_set->time_scale)
    {
        if(f_disturd_set->am_pm_flag)
        {
            if(hour==0)hour=12;
        }
        else
        {
            if(hour!=0)hour+=12;
        }
    }

    if(sys_cb.disturd_sel)
    {
        sys_cb.disturd_end_time_sec = hour*3600+min*60;
        uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
        uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
    }
    else
    {
        sys_cb.disturd_start_time_sec = hour*3600+min*60;
        uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
        uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
    }
    // printf("hour2:%d\n",sys_cb.disturd_start_time_sec);
}

/*函数功能：设置小时与分钟背景图片*/
static void func_disturd_sub_set_timer_pic_bg(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    if(f_disturd_set->touch_flag == false)
    {
        compo_button_t * btn_hour = compo_getobj_byid(COMPO_ID_BTN_HOUR_BG);
        compo_button_t * btn_min  = compo_getobj_byid(COMPO_ID_BTN_MIN_BG);

        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_HOUR_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_hour_flag = true;
                if(f_disturd_set->time_scale)//12小时制
                {
                    compo_button_set_bgimg(btn_hour,UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN);
                    compo_button_set_bgimg(btn_min,UI_BUF_I330001_PUBLIC_KUANG_GREY_SMALL_BIN);
                }
                else
                {
                    compo_button_set_bgimg(btn_hour,UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIG_BIN);
                    compo_button_set_bgimg(btn_min,UI_BUF_I330001_PUBLIC_KUANG_GREY_BIG_BIN);
                }
                break;
            case COMPO_ID_BTN_MIN_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_hour_flag = false;
                if(f_disturd_set->time_scale)//12小时制
                {
                    compo_button_set_bgimg(btn_min,UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN);
                    compo_button_set_bgimg(btn_hour,UI_BUF_I330001_PUBLIC_KUANG_GREY_SMALL_BIN);
                }
                else
                {
                    compo_button_set_bgimg(btn_min,UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIG_BIN);
                    compo_button_set_bgimg(btn_hour,UI_BUF_I330001_PUBLIC_KUANG_GREY_BIG_BIN);
                }
                break;
        }

    }
}


///创建定时勿扰设置窗体
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    if(sys_cb.disturd_sel)
    {
        compo_form_set_title(frm, i18n[STR_DISTURD_TIM_END]);
    }
    else
    {
        compo_form_set_title(frm, i18n[STR_DISTURD_TIM_START]);
    }

    u8 hour ;
    u8 min  ;
    char txt_buf[20];
    s8 hour_data[5];
    s8 min_data[5];
    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec / 3600;
        min = (sys_cb.disturd_end_time_sec % 3600) / 60;
    }
    else
    {
        hour = sys_cb.disturd_start_time_sec / 3600;
        min = (sys_cb.disturd_start_time_sec % 3600) / 60;
    }

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 62+130/2, GUI_SCREEN_WIDTH,130);

    if(uteModuleSystemtime12HOn())///12小时制
    {
        compo_button_t * btn_am = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 上午
        compo_button_set_pos(btn_am,16+48/2,87-62+30/2);
        compo_setid(btn_am,COMPO_ID_BTN_AM_BG);

        compo_button_t * btn_pm = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 下午
        compo_button_set_pos(btn_pm,16+48/2,137-62+30/2);
        compo_setid(btn_pm,COMPO_ID_BTN_PM_BG);

        compo_textbox_t *txt_am = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_AM]));///12小时制文本 上午
        compo_textbox_set_location(txt_am,16+48/2,87-62+30/2,48,30);
        compo_textbox_set(txt_am,i18n[STR_AM]);
        compo_setid(txt_am,COMPO_ID_TXT_AM_BG);

        compo_textbox_t *txt_pm = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_PM]));///12小时制文本 下午
        compo_textbox_set_location(txt_pm,16+48/2,137-62+30/2,48,30);
        compo_textbox_set(txt_pm,i18n[STR_PM]);
        compo_setid(txt_pm,COMPO_ID_TXT_PM_BG);

        if(hour && hour<=12)  //更新按钮与文本颜色
        {
            compo_button_set_bgimg(btn_am, UI_BUF_I330001_ALARM_02_BIN);
            compo_button_set_bgimg(btn_pm, UI_BUF_I330001_ALARM_00_BIN);
            compo_textbox_set_forecolor(txt_am, COLOR_WHITE);
            compo_textbox_set_forecolor(txt_pm, COLOR_GRAY);
        }
        else
        {
            compo_button_set_bgimg(btn_pm, UI_BUF_I330001_ALARM_02_BIN);
            compo_button_set_bgimg(btn_am, UI_BUF_I330001_ALARM_00_BIN);
            compo_textbox_set_forecolor(txt_pm, COLOR_WHITE);
            compo_textbox_set_forecolor(txt_am, COLOR_GRAY);
        }

        func_disturd_sub_get_timer(1,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(1,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
        }

        compo_button_t * btn_hour = compo_button_create_page_by_image(frm,page,UI_BUF_I330001_PUBLIC_KUANG_GREY_SMALL_BIN);///小时滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_hour,86+58/2,130/2);
        compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

        compo_button_t * btn_min = compo_button_create_page_by_image(frm,page,UI_BUF_I330001_PUBLIC_KUANG_GREY_SMALL_BIN);///分钟滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_min,172+58/2,130/2);
        compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

        compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        compo_textbox_set_pos(txt_colon,32/2+141,CENTER_TXT_Y);
        compo_textbox_set(txt_colon,":");
    }
    else
    {
        func_disturd_sub_get_timer(0,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(0,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
        }

        compo_button_t * btn_hour = compo_button_create_page_by_image(frm,page,UI_BUF_I330001_PUBLIC_KUANG_GREY_BIG_BIN);///小时滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_hour,TXT_24_HOUR_X,CENTER_TXT_Y);
        compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

        compo_button_t * btn_min = compo_button_create_page_by_image(frm,page,UI_BUF_I330001_PUBLIC_KUANG_GREY_BIG_BIN);///分钟滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_min,TXT_24_MIN_X,CENTER_TXT_Y);
        compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

        compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        compo_textbox_set_pos(txt_colon,32/2+103,CENTER_TXT_Y);
        compo_textbox_set(txt_colon,":");
    }


    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I330001_PUBLIC_RECTANGLE01_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,60/2+216);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    compo_textbox_t *txt_ok = compo_textbox_create(frm,strlen(i18n[STR_OK]));
    compo_textbox_set_location(txt_ok,GUI_SCREEN_CENTER_X,60/2+216, 214,62);
    compo_textbox_set(txt_ok,i18n[STR_OK]);

    if(func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t*f_disturd_sub_set = (f_disturd_sub_set_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_disturd_sub_set->disturd_hour[i] = hour_data[i];
            f_disturd_sub_set->disturd_min[i] = min_data[i];
        }
    }

    return frm;
}


/*函数功能：滑动处理*/
static void func_disturd_sub_move(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        // printf("11111111111\n");
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            if(f_disturd_set->set_hour_flag == true)//小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    if(f_disturd_set->time_scale) //12小时制
                    {
                        f_disturd_set->hour = (12+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%12;
                        func_disturd_sub_get_timer(1,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        f_disturd_set->hour = (24+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%24;
                        func_disturd_sub_get_timer(0,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else    //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    f_disturd_set->min = (60+f_disturd_set->min_old-f_disturd_set->move_dy_data/TXT_SPACING)%60;
                    func_disturd_sub_get_timer(1,1,f_disturd_set->min,f_disturd_set->disturd_min);

                    if(f_disturd_set->time_scale) //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    else    //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_min[idx-COMPO_ID_TXT_MIN_1]);
                    compo_textbox_set(txt,txt_buf);
                }
            }
        }
        else   //松手状态
        {
            if(f_disturd_set->set_hour_flag == true) //小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }

                }
                f_disturd_set->hour_old = f_disturd_set->hour;
            }
            else  //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }

                }
                f_disturd_set->min_old = f_disturd_set->min;
            }
        }

    }
}

//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_AM_BG:
            f_disturd_set->am_pm_flag = true;
            break;
        case COMPO_ID_BTN_PM_BG:
            f_disturd_set->am_pm_flag = false;
            break;
        case COMPO_ID_BTN_SURE:
            func_disturd_sub_set_time();
            func_cb.sta = FUNC_SET_SUB_DISTURD;
            task_stack_pop();
            break;

        default:
            break;
    }
    if(f_disturd_set->time_scale)
    {
        func_disturd_sub_set_refresh();
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//组件ID

#define TXT_SPACING    (142-89)
#define CENTER_TXT_Y   (158/2)
#define TXT_12_HOUR_X  (160+30/2)
#define TXT_12_MIN_X   (266+40/2)
#define TXT_24_HOUR_X  (84+40/2)
#define TXT_24_MIN_X   (237+43/2)

const uint16_t TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

/*函数功能：获取设置时间（上下 & 上上下下 ）的数*/
/*入口参数：mode->时间参数 ，0为24小时制 ，1为12小时制*/
/*入口参数：timer->设置时间*/
/*入口参数：hour_flag->设置小时或分钟 0小时 1分钟*/
/*返回参数：timer_data->返回timer（上下 & 上上下下 ）的时间1~5数组*/
static void func_disturd_sub_get_timer(bool mode,bool hour_flag,u8 timer,u8 *timer_data)
{
    s8 num=-2;

    if(mode == 0)
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+24+(num))%24;
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
    else
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+12+(num))%12;
                if(timer_data[i] == 0)
                {
                    timer_data[i] = 12;
                }
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }

    }

}
/*函数功能：12小时制刷新按钮颜色*/
static void func_disturd_sub_set_refresh(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    compo_button_t * btn_am = compo_getobj_byid(COMPO_ID_BTN_AM_BG);
    compo_button_t * btn_pm = compo_getobj_byid(COMPO_ID_BTN_PM_BG);
    compo_textbox_t *txt_am = compo_getobj_byid(COMPO_ID_TXT_AM_BG);
    compo_textbox_t *txt_pm = compo_getobj_byid(COMPO_ID_TXT_PM_BG);

    if(f_disturd_set->am_pm_flag == true)
    {
        compo_button_set_bgimg(btn_am, UI_BUF_I332001_PUBLIC_SHANGWU02_BIN);
        compo_button_set_bgimg(btn_pm, UI_BUF_I332001_PUBLIC_SHANGWU01_BIN);
        compo_textbox_set_forecolor(txt_am, COLOR_WHITE);
        compo_textbox_set_forecolor(txt_pm, COLOR_GRAY);
    }
    else
    {
        compo_button_set_bgimg(btn_pm, UI_BUF_I332001_PUBLIC_SHANGWU02_BIN);
        compo_button_set_bgimg(btn_am, UI_BUF_I332001_PUBLIC_SHANGWU01_BIN);
        compo_textbox_set_forecolor(txt_pm, COLOR_WHITE);
        compo_textbox_set_forecolor(txt_am, COLOR_GRAY);
    }
}
/*函数功能：初始化数据*/
static void func_disturd_sub_time_init(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    f_disturd_set->time_scale = uteModuleSystemtime12HOn();
    if(sys_cb.disturd_sel)
    {
        f_disturd_set->hour = sys_cb.disturd_end_time_sec/3600;
        f_disturd_set->min = sys_cb.disturd_end_time_sec%3600/60;

    }
    else
    {
        f_disturd_set->hour = sys_cb.disturd_start_time_sec/3600;
        f_disturd_set->min = sys_cb.disturd_start_time_sec%3600/60;
    }

    if(f_disturd_set->time_scale)//12小时制
    {
        if(f_disturd_set->hour && f_disturd_set->hour<=12)
        {
            f_disturd_set->am_pm_flag = true;
        }
        else
        {
            f_disturd_set->am_pm_flag = false;
        }
        f_disturd_set->hour %=12;
    }
    else
    {
        f_disturd_set->hour %=24;
    }
    f_disturd_set->hour_old = f_disturd_set->hour;
    f_disturd_set->min_old  = f_disturd_set->min;

    if(f_disturd_set->time_scale)
    {
        func_disturd_sub_set_refresh();
    }
    // printf("hour1:%d\n",f_disturd_set->hour_old);
}
/*函数功能：设置勿扰时间*/
static void func_disturd_sub_set_time(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    u8 hour = f_disturd_set->hour;
    u8 min  = f_disturd_set->min;


    if(f_disturd_set->time_scale)
    {
        if(f_disturd_set->am_pm_flag)
        {
            if(hour==0)hour=12;
        }
        else
        {
            if(hour!=0)hour+=12;
        }
    }

    if(sys_cb.disturd_sel)
    {
        sys_cb.disturd_end_time_sec = hour*3600+min*60;
        uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
        uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
    }
    else
    {
        sys_cb.disturd_start_time_sec = hour*3600+min*60;
        uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
        uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
    }
    // printf("hour2:%d\n",sys_cb.disturd_start_time_sec);
}

/*函数功能：设置小时与分钟背景图片*/
static void func_disturd_sub_set_timer_pic_bg(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    if(f_disturd_set->touch_flag == false)
    {
        compo_button_t * btn_hour = compo_getobj_byid(COMPO_ID_BTN_HOUR_BG);
        compo_button_t * btn_min  = compo_getobj_byid(COMPO_ID_BTN_MIN_BG);

        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_HOUR_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_hour_flag = true;
                if(f_disturd_set->time_scale)//12小时制
                {
                    compo_button_set_bgimg(btn_hour,UI_BUF_I332001_PUBLIC_KUANG_BLUE_BIN);
                    compo_button_set_bgimg(btn_min,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN);
                }
                else
                {
                    compo_button_set_bgimg(btn_hour,UI_BUF_I332001_PUBLIC_KUANG_BLUE_BIG_BIN);
                    compo_button_set_bgimg(btn_min,UI_BUF_I332001_PUBLIC_KUANG_GREY_BIG_BIN);
                }
                break;
            case COMPO_ID_BTN_MIN_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_hour_flag = false;
                if(f_disturd_set->time_scale)//12小时制
                {
                    compo_button_set_bgimg(btn_min,UI_BUF_I332001_PUBLIC_KUANG_BLUE_BIN);
                    compo_button_set_bgimg(btn_hour,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN);
                }
                else
                {
                    compo_button_set_bgimg(btn_min,UI_BUF_I332001_PUBLIC_KUANG_BLUE_BIG_BIN);
                    compo_button_set_bgimg(btn_hour,UI_BUF_I332001_PUBLIC_KUANG_GREY_BIG_BIN);
                }
                break;
        }

    }
}


///创建定时勿扰设置窗体
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    if(sys_cb.disturd_sel)
    {
        compo_form_set_title(frm, i18n[STR_DISTURD_TIM_END]);
    }
    else
    {
        compo_form_set_title(frm, i18n[STR_DISTURD_TIM_START]);
    }

    u8 hour ;
    u8 min  ;
    char txt_buf[20];
    s8 hour_data[5];
    s8 min_data[5];
    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec / 3600;
        min = (sys_cb.disturd_end_time_sec % 3600) / 60;
    }
    else
    {
        hour = sys_cb.disturd_start_time_sec / 3600;
        min = (sys_cb.disturd_start_time_sec % 3600) / 60;
    }

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 84+CENTER_TXT_Y, GUI_SCREEN_WIDTH,CENTER_TXT_Y*2);

    if(uteModuleSystemtime12HOn())///12小时制
    {
        compo_button_t * btn_am = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 上午
        compo_button_set_pos(btn_am,42+58/2,120-84+32/2);
        compo_setid(btn_am,COMPO_ID_BTN_AM_BG);

        compo_button_t * btn_pm = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 下午
        compo_button_set_pos(btn_pm,42+58/2,176-84+32/2);
        compo_setid(btn_pm,COMPO_ID_BTN_PM_BG);

        compo_textbox_t *txt_am = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_AM]));///12小时制文本 上午
        compo_textbox_set_location(txt_am,42+58/2,120-84+32/2,56,32);
        compo_textbox_set(txt_am,i18n[STR_AM]);
        compo_setid(txt_am,COMPO_ID_TXT_AM_BG);

        compo_textbox_t *txt_pm = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_PM]));///12小时制文本 下午
        compo_textbox_set_location(txt_pm,42+58/2,176-84+32/2,56,32);
        compo_textbox_set(txt_pm,i18n[STR_PM]);
        compo_setid(txt_pm,COMPO_ID_TXT_PM_BG);

        if(hour && hour<=12)  //更新按钮与文本颜色
        {
            compo_button_set_bgimg(btn_am, UI_BUF_I332001_PUBLIC_SHANGWU02_BIN);
            compo_button_set_bgimg(btn_pm, UI_BUF_I332001_PUBLIC_SHANGWU01_BIN);
            compo_textbox_set_forecolor(txt_am, COLOR_WHITE);
            compo_textbox_set_forecolor(txt_pm, COLOR_GRAY);
        }
        else
        {
            compo_button_set_bgimg(btn_pm, UI_BUF_I332001_PUBLIC_SHANGWU02_BIN);
            compo_button_set_bgimg(btn_am, UI_BUF_I332001_PUBLIC_SHANGWU01_BIN);
            compo_textbox_set_forecolor(txt_pm, COLOR_WHITE);
            compo_textbox_set_forecolor(txt_am, COLOR_GRAY);
        }

        func_disturd_sub_get_timer(1,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(1,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
        }

        compo_button_t * btn_hour = compo_button_create_page_by_image(frm,page,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN);///小时滑动按钮 UI_BUF_I332001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_hour,TXT_12_HOUR_X,CENTER_TXT_Y);
        compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

        compo_button_t * btn_min = compo_button_create_page_by_image(frm,page,UI_BUF_I332001_PUBLIC_KUANG_GREY_SMALL_BIN);///分钟滑动按钮 UI_BUF_I332001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_min,TXT_12_MIN_X,CENTER_TXT_Y);
        compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

        compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        compo_textbox_set_pos(txt_colon,32/2+213,CENTER_TXT_Y);
        compo_textbox_set(txt_colon,":");
    }
    else
    {
        func_disturd_sub_get_timer(0,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(0,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
        }

        compo_button_t * btn_hour = compo_button_create_page_by_image(frm,page,UI_BUF_I332001_PUBLIC_KUANG_GREY_BIG_BIN);///小时滑动按钮 UI_BUF_I332001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_hour,TXT_24_HOUR_X,CENTER_TXT_Y);
        compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

        compo_button_t * btn_min = compo_button_create_page_by_image(frm,page,UI_BUF_I332001_PUBLIC_KUANG_GREY_BIG_BIN);///分钟滑动按钮 UI_BUF_I332001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_min,TXT_24_MIN_X,CENTER_TXT_Y);
        compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

        compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        compo_textbox_set_pos(txt_colon,32/2+164,CENTER_TXT_Y);
        compo_textbox_set(txt_colon,":");
    }


    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I332001_PUBLIC_RECTANGLE01_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,64/2+266);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    compo_textbox_t *txt_ok = compo_textbox_create(frm,strlen(i18n[STR_OK]));
    compo_textbox_set_location(txt_ok,GUI_SCREEN_CENTER_X,64/2+266, 214,64);
    compo_textbox_set(txt_ok,i18n[STR_OK]);

    if(func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t*f_disturd_sub_set = (f_disturd_sub_set_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_disturd_sub_set->disturd_hour[i] = hour_data[i];
            f_disturd_sub_set->disturd_min[i] = min_data[i];
        }
    }

    return frm;
}


/*函数功能：滑动处理*/
static void func_disturd_sub_move(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        // printf("11111111111\n");
        char txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            if(f_disturd_set->set_hour_flag == true)//小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    if(f_disturd_set->time_scale) //12小时制
                    {
                        f_disturd_set->hour = (12+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%12;
                        func_disturd_sub_get_timer(1,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        f_disturd_set->hour = (24+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%24;
                        func_disturd_sub_get_timer(0,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else    //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    f_disturd_set->min = (60+f_disturd_set->min_old-f_disturd_set->move_dy_data/TXT_SPACING)%60;
                    func_disturd_sub_get_timer(1,1,f_disturd_set->min,f_disturd_set->disturd_min);

                    if(f_disturd_set->time_scale) //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    else    //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_min[idx-COMPO_ID_TXT_MIN_1]);
                    compo_textbox_set(txt,txt_buf);
                }
            }
        }
        else   //松手状态
        {
            if(f_disturd_set->set_hour_flag == true) //小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }

                }
                f_disturd_set->hour_old = f_disturd_set->hour;
            }
            else  //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }

                }
                f_disturd_set->min_old = f_disturd_set->min;
            }
        }

    }
}

//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_AM_BG:
            f_disturd_set->am_pm_flag = true;
            break;
        case COMPO_ID_BTN_PM_BG:
            f_disturd_set->am_pm_flag = false;
            break;
        case COMPO_ID_BTN_SURE:
            func_disturd_sub_set_time();
            func_cb.sta = FUNC_SET_SUB_DISTURD;
            task_stack_pop();
            break;

        default:
            break;
    }
    // printf("min:%d\n",f_disturd_set->min_old);
    if(f_disturd_set->time_scale)
    {
        func_disturd_sub_set_refresh();
    }
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
enum
{
    SET_HOUR_MODE=0,
    SET_MIN_MODE,
    SET_AM_PS,
};

#define TXT_SPACING    (105-62)
#define CENTER_TXT_Y   (130/2)
#define TXT_12_HOUR_X  (85+58/2)
#define TXT_12_MIN_X   (172+58/2)
#define TXT_24_HOUR_X  (4+100/2)
#define TXT_24_MIN_X   (168+34/2)

const uint16_t DISTURD_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

/*函数功能：获取设置时间（上下 & 上上下下 ）的数*/
/*入口参数：mode->时间参数 ，0为24小时制 ，1为12小时制*/
/*入口参数：timer->设置时间*/
/*入口参数：hour_flag->设置小时或分钟 0小时 1分钟*/
/*返回参数：timer_data->返回timer（上下 & 上上下下 ）的时间1~5数组*/
static void func_disturd_sub_get_timer(bool mode,bool hour_flag,u8 timer,s8 *timer_data)
{
    s8 num=-2;

    if(mode == 0)
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+24+(num))%24;
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
    else
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+12+(num))%12;
                if(timer_data[i] == 0)
                {
                    timer_data[i] = 12;
                }
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
}
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_SET]);

    u8 hour ;
    u8 min  ;
    char txt_buf[20];
    s8 hour_data[5];
    s8 min_data[5];

    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec/3600;
        min = sys_cb.disturd_end_time_sec%3600/60;

    }
    else
    {
        hour = sys_cb.disturd_start_time_sec/3600;
        min = sys_cb.disturd_start_time_sec%3600/60;
    }

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 62+130/2, GUI_SCREEN_WIDTH,130);

    if(uteModuleSystemtime12HOn())///12小时制
    {
        compo_button_t * btn_am = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 上午 下午
        compo_button_set_location(btn_am,16+48/2,130/2,58,130);
        compo_setid(btn_am,COMPO_ID_BTN_PM_AM);

        compo_textbox_t *txt_am = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_AM]));///12小时制文本 上午
        compo_textbox_set_location(txt_am,16+48/2,0,48,30);
        compo_textbox_set(txt_am,i18n[STR_AM]);
        compo_setid(txt_am,COMPO_ID_TXT_AM_BG);

        compo_textbox_t *txt_pm = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_PM]));///12小时制文本 下午
        compo_textbox_set_location(txt_pm,16+48/2,0,48,30);
        compo_textbox_set(txt_pm,i18n[STR_PM]);
        compo_setid(txt_pm,COMPO_ID_TXT_PM_BG);

        compo_textbox_set_pos(txt_am,16+48/2,(hour<12) ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
        compo_textbox_set_pos(txt_pm,16+48/2,(hour<12) ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
        compo_textbox_set_forecolor(txt_am,(hour<12) ? COLOR_WHITE : COLOR_GRAY);
        compo_textbox_set_forecolor(txt_pm,!(hour<12) ? COLOR_WHITE : COLOR_GRAY);

        func_disturd_sub_get_timer(1,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(1,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_28_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+141,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }
    else
    {
        func_disturd_sub_get_timer(0,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(0,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_28_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+103,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }

    compo_button_t * btn_hour = compo_button_create(frm);///小时滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_hour,uteModuleSystemtime12HOn() ? TXT_12_HOUR_X : TXT_24_HOUR_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_min,uteModuleSystemtime12HOn() ? TXT_12_MIN_X : TXT_24_MIN_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I335001_20_ALARM_CLOCK_6_QUANTITY_LIMIT_REMINDER_ICON_YES_208X52_X16_Y222_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I335001_20_ALARM_CLOCK_6_QUANTITY_LIMIT_REMINDER_ICON_YES_208X52_X16_Y222_BIN).hei/2-10);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    if(func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t*f_alarm_clock_sub_set = (f_disturd_sub_set_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_alarm_clock_sub_set->disturd_hour[i] = hour_data[i];
            f_alarm_clock_sub_set->disturd_min[i]  = min_data[i];
        }
        f_alarm_clock_sub_set->time_scale = uteModuleSystemtime12HOn();
        f_alarm_clock_sub_set->hour_old = f_alarm_clock_sub_set->hour = hour_data[2];
        f_alarm_clock_sub_set->min_old  = f_alarm_clock_sub_set->min  = min_data[2];
        f_alarm_clock_sub_set->am_pm_flag = (hour<12) ? true : false;
    }

    return frm;
}


//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    u8 hour = f_disturd_set->hour;
    u8 min  = f_disturd_set->min;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
        {
            if(f_disturd_set->time_scale)
            {
                if(f_disturd_set->am_pm_flag)
                {
                    if(hour==12)hour=0;
                }
                else
                {
                    if(hour!=12)hour+=12;
                }
            }
            if(sys_cb.disturd_sel)
            {
                sys_cb.disturd_end_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
            }
            else
            {
                sys_cb.disturd_start_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
            }
            uteTaskGuiStartScreen(FUNC_SET_SUB_DISTURD, 0, __func__);
        }
        break;
        default:
            break;
    }
}
/*函数功能：设置小时与分钟背景图片*/
static void func_disturd_sub_set_timer_mode(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    if(f_disturd_set->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_HOUR_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_HOUR_MODE;
                break;
            case COMPO_ID_BTN_MIN_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_MIN_MODE;
                break;
            case COMPO_ID_BTN_PM_AM:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_AM_PS;
                break;
        }

    }
}
/*函数功能：滑动处理*/
static void func_disturd_sub_move(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            if(f_disturd_set->set_num_flag == SET_HOUR_MODE)//小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    if(f_disturd_set->time_scale) //12小时制
                    {
                        f_disturd_set->hour = (12+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%12;
                        func_disturd_sub_get_timer(1,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        f_disturd_set->hour = (24+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%24;
                        func_disturd_sub_get_timer(0,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_HOUR_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    f_disturd_set->min = (60+f_disturd_set->min_old-f_disturd_set->move_dy_data/TXT_SPACING)%60;
                    func_disturd_sub_get_timer(1,1,f_disturd_set->min,f_disturd_set->disturd_min);

                    if(f_disturd_set->time_scale) //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    else    //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_min[idx-COMPO_ID_TXT_MIN_1]);
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_MIN_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                f_disturd_set->move_am_pm_dy = dy;
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy <0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    else if(f_disturd_set->move_am_pm_dy> TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = TXT_SPACING;
                    }

                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy >0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    if(f_disturd_set->move_am_pm_dy< -TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = -TXT_SPACING;
                    }
                }

                for(int i=1,idx=COMPO_ID_TXT_PM_BG; idx>=COMPO_ID_TXT_AM_BG; idx--,i++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    compo_textbox_set_pos(txt,16+48/2,DISTURD_TXT_Y[i+(1-f_disturd_set->am_pm_flag)]+f_disturd_set->move_am_pm_dy);
                }
            }
        }
        else   //松手状态
        {
            if(f_disturd_set->set_num_flag == SET_HOUR_MODE) //小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }

                }
                f_disturd_set->hour_old = f_disturd_set->hour;
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }

                }
                f_disturd_set->min_old = f_disturd_set->min;
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy == TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = false;
                    }
                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy == -TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = true;
                    }
                }

                compo_textbox_t *txt_am = compo_getobj_byid(COMPO_ID_TXT_AM_BG);
                compo_textbox_t *txt_pm = compo_getobj_byid(COMPO_ID_TXT_PM_BG);

                compo_textbox_set_pos(txt_am,16+48/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
                compo_textbox_set_pos(txt_pm,16+48/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
                compo_textbox_set_forecolor(txt_am,f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
                compo_textbox_set_forecolor(txt_pm,!f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
            }
        }

    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT
enum
{
    SET_HOUR_MODE=0,
    SET_MIN_MODE,
    SET_AM_PS,
};

#define TXT_SPACING    (92)
#define CENTER_TXT_Y   (170 - TXT_SPACING + 66/2)
#define TXT_12_HOUR_X  (148 + 58/2)
#define TXT_12_MIN_X   (252 + 58/2)
#define TXT_24_HOUR_X  (72  + 58/2)
#define TXT_24_MIN_X   (232 + 58/2)

const uint16_t DISTURD_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

/*函数功能：获取设置时间（上下 & 上上下下 ）的数*/
/*入口参数：mode->时间参数 ，0为24小时制 ，1为12小时制*/
/*入口参数：timer->设置时间*/
/*入口参数：hour_flag->设置小时或分钟 0小时 1分钟*/
/*返回参数：timer_data->返回timer（上下 & 上上下下 ）的时间1~5数组*/
static void func_disturd_sub_get_timer(bool mode,bool hour_flag,u8 timer,s8 *timer_data)
{
    s8 num=-2;

    if(mode == 0)
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+24+(num))%24;
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
    else
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+12+(num))%12;
                if(timer_data[i] == 0)
                {
                    timer_data[i] = 12;
                }
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
}
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_SET]);

    u8 hour ;
    u8 min  ;
    char txt_buf[20];
    s8 hour_data[5];
    s8 min_data[5];

    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec/3600;
        min = sys_cb.disturd_end_time_sec%3600/60;

    }
    else
    {
        hour = sys_cb.disturd_start_time_sec/3600;
        min = sys_cb.disturd_start_time_sec%3600/60;
    }

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 170 + 33, GUI_SCREEN_WIDTH, 248);

    if(uteModuleSystemtime12HOn())///12小时制
    {
        compo_button_t * btn_am = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 上午 下午
        compo_button_set_location(btn_am,40+34,90,78,180);
        compo_setid(btn_am,COMPO_ID_BTN_PM_AM);

        compo_textbox_t *txt_am = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_AM]));///12小时制文本 上午
        compo_textbox_set_location(txt_am,50+34,0,68,34);
        compo_textbox_set(txt_am,i18n[STR_AM]);
        compo_setid(txt_am,COMPO_ID_TXT_AM_BG);

        compo_textbox_t *txt_pm = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_PM]));///12小时制文本 下午
        compo_textbox_set_location(txt_am,50+34,0,68,34);
        compo_textbox_set(txt_pm,i18n[STR_PM]);
        compo_setid(txt_pm,COMPO_ID_TXT_PM_BG);

        compo_textbox_set_pos(txt_am,50+34,(hour && hour<=12) ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
        compo_textbox_set_pos(txt_pm,50+34,(hour && hour<=12) ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
        compo_textbox_set_forecolor(txt_am,(hour && hour<=12) ? COLOR_WHITE : COLOR_GRAY);
        compo_textbox_set_forecolor(txt_pm,!(hour && hour<=12) ? COLOR_WHITE : COLOR_GRAY);

        func_disturd_sub_get_timer(1,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(1,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_54_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+141,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }
    else
    {
        func_disturd_sub_get_timer(0,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(0,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_54_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+103,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }

    compo_button_t * btn_hour = compo_button_create(frm);///小时滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_hour,uteModuleSystemtime12HOn() ? TXT_12_HOUR_X : TXT_24_HOUR_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_min,uteModuleSystemtime12HOn() ? TXT_12_MIN_X : TXT_24_MIN_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I341001_28_SET_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I341001_20_ALARM_CLOCK_CONFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    if(func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t*f_alarm_clock_sub_set = (f_disturd_sub_set_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_alarm_clock_sub_set->disturd_hour[i] = hour_data[i];
            f_alarm_clock_sub_set->disturd_min[i]  = min_data[i];
        }
        f_alarm_clock_sub_set->time_scale = uteModuleSystemtime12HOn();
        f_alarm_clock_sub_set->hour_old = f_alarm_clock_sub_set->hour = hour_data[2];
        f_alarm_clock_sub_set->min_old  = f_alarm_clock_sub_set->min  = min_data[2];
        f_alarm_clock_sub_set->am_pm_flag = (hour && hour<=12) ? true : false;
    }

    return frm;
}


//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    u8 hour = f_disturd_set->hour;
    u8 min  = f_disturd_set->min;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
        {
            if(f_disturd_set->time_scale)
            {
                if(f_disturd_set->am_pm_flag)
                {
                    if(hour==0)hour=12;
                }
                else
                {
                    if(hour!=0)hour+=12;
                }
            }
            if(sys_cb.disturd_sel)
            {
                sys_cb.disturd_end_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
            }
            else
            {
                sys_cb.disturd_start_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
            }
            uteTaskGuiStartScreen(FUNC_SET_SUB_DISTURD, 0, __func__);
        }
        break;
        default:
            break;
    }
}
/*函数功能：设置小时与分钟背景图片*/
static void func_disturd_sub_set_timer_mode(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    if(f_disturd_set->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_HOUR_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_HOUR_MODE;
                break;
            case COMPO_ID_BTN_MIN_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_MIN_MODE;
                break;
            case COMPO_ID_BTN_PM_AM:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_AM_PS;
                break;
        }

    }
}
/*函数功能：滑动处理*/
static void func_disturd_sub_move(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            if(f_disturd_set->set_num_flag == SET_HOUR_MODE)//小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    if(f_disturd_set->time_scale) //12小时制
                    {
                        f_disturd_set->hour = (12+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%12;
                        func_disturd_sub_get_timer(1,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        f_disturd_set->hour = (24+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%24;
                        func_disturd_sub_get_timer(0,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_HOUR_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    f_disturd_set->min = (60+f_disturd_set->min_old-f_disturd_set->move_dy_data/TXT_SPACING)%60;
                    func_disturd_sub_get_timer(1,1,f_disturd_set->min,f_disturd_set->disturd_min);

                    if(f_disturd_set->time_scale) //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    else    //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_min[idx-COMPO_ID_TXT_MIN_1]);
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_MIN_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                f_disturd_set->move_am_pm_dy = dy;
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy <0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    else if(f_disturd_set->move_am_pm_dy> TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = TXT_SPACING;
                    }

                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy >0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    if(f_disturd_set->move_am_pm_dy< -TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = -TXT_SPACING;
                    }
                }

                for(int i=1,idx=COMPO_ID_TXT_PM_BG; idx>=COMPO_ID_TXT_AM_BG; idx--,i++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    compo_textbox_set_pos(txt,50+34,DISTURD_TXT_Y[i+(1-f_disturd_set->am_pm_flag)]+f_disturd_set->move_am_pm_dy);
                }
            }
        }
        else   //松手状态
        {
            if(f_disturd_set->set_num_flag == SET_HOUR_MODE) //小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }

                }
                f_disturd_set->hour_old = f_disturd_set->hour;
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }

                }
                f_disturd_set->min_old = f_disturd_set->min;
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy == TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = false;
                    }
                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy == -TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = true;
                    }
                }

                compo_textbox_t *txt_am = compo_getobj_byid(COMPO_ID_TXT_AM_BG);
                compo_textbox_t *txt_pm = compo_getobj_byid(COMPO_ID_TXT_PM_BG);

                compo_textbox_set_pos(txt_am,16+48/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
                compo_textbox_set_pos(txt_pm,16+48/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
                compo_textbox_set_forecolor(txt_am,f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
                compo_textbox_set_forecolor(txt_pm,!f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
            }
        }

    }
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

enum
{
    SET_HOUR_MODE=0,
    SET_MIN_MODE,
    SET_AM_PS,
};

#define TXT_SPACING    (80)
#define CENTER_TXT_Y   (140 - TXT_SPACING + 56/2)
#define TXT_12_HOUR_X  (118 + 58/2)
#define TXT_12_MIN_X   (227 + 58/2)
#define TXT_24_HOUR_X  (66  + 58/2)
#define TXT_24_MIN_X   (206 + 58/2)

const uint16_t DISTURD_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

/*函数功能：获取设置时间（上下 & 上上下下 ）的数*/
/*入口参数：mode->时间参数 ，0为24小时制 ，1为12小时制*/
/*入口参数：timer->设置时间*/
/*入口参数：hour_flag->设置小时或分钟 0小时 1分钟*/
/*返回参数：timer_data->返回timer（上下 & 上上下下 ）的时间1~5数组*/
static void func_disturd_sub_get_timer(bool mode,bool hour_flag,u8 timer,s8 *timer_data)
{
    s8 num=-2;

    if(mode == 0)
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+24+(num))%24;
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
    else
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+12+(num))%12;
                if(timer_data[i] == 0)
                {
                    timer_data[i] = 12;
                }
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
}
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_SET]);

    u8 hour ;
    u8 min  ;
    char txt_buf[20];
    s8 hour_data[5];
    s8 min_data[5];

    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec/3600;
        min = sys_cb.disturd_end_time_sec%3600/60;

    }
    else
    {
        hour = sys_cb.disturd_start_time_sec/3600;
        min = sys_cb.disturd_start_time_sec%3600/60;
    }

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 170 + 33, GUI_SCREEN_WIDTH, 248);

    if(uteModuleSystemtime12HOn())///12小时制
    {
        compo_button_t * btn_am = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 上午 下午
        compo_button_set_location(btn_am,29+60/2,180/2,78,180);
        compo_setid(btn_am,COMPO_ID_BTN_PM_AM);

        compo_textbox_t *txt_am = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_AM]));///12小时制文本 上午
        compo_textbox_set_location(txt_am,29+60/2,0,68,34);
        compo_textbox_set(txt_am,i18n[STR_AM]);
        compo_setid(txt_am,COMPO_ID_TXT_AM_BG);

        compo_textbox_t *txt_pm = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_PM]));///12小时制文本 下午
        compo_textbox_set_location(txt_am,29+60/2,0,68,34);
        compo_textbox_set(txt_pm,i18n[STR_PM]);
        compo_setid(txt_pm,COMPO_ID_TXT_PM_BG);

        compo_textbox_set_pos(txt_am,29+60/2,(hour && hour<=12) ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
        compo_textbox_set_pos(txt_pm,29+60/2,(hour && hour<=12) ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
        compo_textbox_set_forecolor(txt_am,(hour && hour<=12) ? COLOR_WHITE : COLOR_GRAY);
        compo_textbox_set_forecolor(txt_pm,!(hour && hour<=12) ? COLOR_WHITE : COLOR_GRAY);

        func_disturd_sub_get_timer(1,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(1,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_54_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+141,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }
    else
    {
        func_disturd_sub_get_timer(0,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(0,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_54_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+103,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }

    compo_button_t * btn_hour = compo_button_create(frm);///小时滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_hour,uteModuleSystemtime12HOn() ? TXT_12_HOUR_X : TXT_24_HOUR_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_min,uteModuleSystemtime12HOn() ? TXT_12_MIN_X : TXT_24_MIN_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I343001_COMM_OK_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I343001_COMM_OK_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    if(func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t*f_alarm_clock_sub_set = (f_disturd_sub_set_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_alarm_clock_sub_set->disturd_hour[i] = hour_data[i];
            f_alarm_clock_sub_set->disturd_min[i]  = min_data[i];
        }
        f_alarm_clock_sub_set->time_scale = uteModuleSystemtime12HOn();
        f_alarm_clock_sub_set->hour_old = f_alarm_clock_sub_set->hour = hour_data[2];
        f_alarm_clock_sub_set->min_old  = f_alarm_clock_sub_set->min  = min_data[2];
        f_alarm_clock_sub_set->am_pm_flag = (hour && hour<=12) ? true : false;
    }

    return frm;
}


//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    u8 hour = f_disturd_set->hour;
    u8 min  = f_disturd_set->min;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
        {
            if(f_disturd_set->time_scale)
            {
                if(f_disturd_set->am_pm_flag)
                {
                    if(hour==0)hour=12;
                }
                else
                {
                    if(hour!=0)hour+=12;
                }
            }
            if(sys_cb.disturd_sel)
            {
                sys_cb.disturd_end_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
            }
            else
            {
                sys_cb.disturd_start_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
            }
            uteTaskGuiStartScreen(FUNC_SET_SUB_DISTURD, 0, __func__);
        }
        break;
        default:
            break;
    }
}
/*函数功能：设置小时与分钟背景图片*/
static void func_disturd_sub_set_timer_mode(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    if(f_disturd_set->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_HOUR_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_HOUR_MODE;
                break;
            case COMPO_ID_BTN_MIN_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_MIN_MODE;
                break;
            case COMPO_ID_BTN_PM_AM:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_AM_PS;
                break;
        }

    }
}
/*函数功能：滑动处理*/
static void func_disturd_sub_move(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            if(f_disturd_set->set_num_flag == SET_HOUR_MODE)//小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    if(f_disturd_set->time_scale) //12小时制
                    {
                        f_disturd_set->hour = (12+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%12;
                        func_disturd_sub_get_timer(1,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        f_disturd_set->hour = (24+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%24;
                        func_disturd_sub_get_timer(0,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_HOUR_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    f_disturd_set->min = (60+f_disturd_set->min_old-f_disturd_set->move_dy_data/TXT_SPACING)%60;
                    func_disturd_sub_get_timer(1,1,f_disturd_set->min,f_disturd_set->disturd_min);

                    if(f_disturd_set->time_scale) //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    else    //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_min[idx-COMPO_ID_TXT_MIN_1]);
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_MIN_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                f_disturd_set->move_am_pm_dy = dy;
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy <0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    else if(f_disturd_set->move_am_pm_dy> TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = TXT_SPACING;
                    }

                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy >0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    if(f_disturd_set->move_am_pm_dy< -TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = -TXT_SPACING;
                    }
                }

                for(int i=1,idx=COMPO_ID_TXT_PM_BG; idx>=COMPO_ID_TXT_AM_BG; idx--,i++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    compo_textbox_set_pos(txt,29+60/2,DISTURD_TXT_Y[i+(1-f_disturd_set->am_pm_flag)]+f_disturd_set->move_am_pm_dy);
                }
            }
        }
        else   //松手状态
        {
            if(f_disturd_set->set_num_flag == SET_HOUR_MODE) //小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }

                }
                f_disturd_set->hour_old = f_disturd_set->hour;
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }

                }
                f_disturd_set->min_old = f_disturd_set->min;
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy == TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = false;
                    }
                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy == -TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = true;
                    }
                }

                compo_textbox_t *txt_am = compo_getobj_byid(COMPO_ID_TXT_AM_BG);
                compo_textbox_t *txt_pm = compo_getobj_byid(COMPO_ID_TXT_PM_BG);

                compo_textbox_set_pos(txt_am,29+60/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
                compo_textbox_set_pos(txt_pm,29+60/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
                compo_textbox_set_forecolor(txt_am,f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
                compo_textbox_set_forecolor(txt_pm,!f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
            }
        }

    }
}


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT
enum
{
    SET_HOUR_MODE=0,
    SET_MIN_MODE,
    SET_AM_PS,
};

#define TXT_SPACING    (135-82)
#define CENTER_TXT_Y   (140/2)
#define TXT_12_HOUR_X  (146+54/2)
#define TXT_12_MIN_X   (236+54/2)
#define TXT_24_HOUR_X  (86+54/2)
#define TXT_24_MIN_X   (226+54/2)
#define TXT_AP_PM_X    (TXT_12_HOUR_X+(TXT_12_HOUR_X - TXT_12_MIN_X))

const uint16_t DISTURD_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

/*函数功能：获取设置时间（上下 & 上上下下 ）的数*/
/*入口参数：mode->时间参数 ，0为24小时制 ，1为12小时制*/
/*入口参数：timer->设置时间*/
/*入口参数：hour_flag->设置小时或分钟 0小时 1分钟*/
/*返回参数：timer_data->返回timer（上下 & 上上下下 ）的时间1~5数组*/
static void func_disturd_sub_get_timer(bool mode,bool hour_flag,u8 timer,s8 *timer_data)
{
    s8 num=-2;

    if(mode == 0)
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+24+(num))%24;
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
    else
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+12+(num))%12;
                if(timer_data[i] == 0)
                {
                    timer_data[i] = 12;
                }
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
}
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_SET]);

    u8 hour ;
    u8 min  ;
    char txt_buf[20];
    s8 hour_data[5];
    s8 min_data[5];

    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec/3600;
        min = sys_cb.disturd_end_time_sec%3600/60;

    }
    else
    {
        hour = sys_cb.disturd_start_time_sec/3600;
        min = sys_cb.disturd_start_time_sec%3600/60;
    }

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-20, GUI_SCREEN_WIDTH,140);

    if(uteModuleSystemtime12HOn())///12小时制
    {
        compo_button_t * btn_am = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 上午 下午
        compo_button_set_location(btn_am,16+48/2,130/2,58,130);
        compo_setid(btn_am,COMPO_ID_BTN_PM_AM);

        compo_textbox_t *txt_am = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_AM]));///12小时制文本 上午
        compo_textbox_set_location(txt_am,16+48/2,0,48,30);
        compo_textbox_set(txt_am,i18n[STR_AM]);
        compo_setid(txt_am,COMPO_ID_TXT_AM_BG);

        compo_textbox_t *txt_pm = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_PM]));///12小时制文本 下午
        compo_textbox_set_location(txt_pm,16+48/2,0,48,30);
        compo_textbox_set(txt_pm,i18n[STR_PM]);
        compo_setid(txt_pm,COMPO_ID_TXT_PM_BG);

        compo_textbox_set_pos(txt_am,16+48/2,( hour<12) ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
        compo_textbox_set_pos(txt_pm,16+48/2,(hour<12) ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
        compo_textbox_set_forecolor(txt_am,(hour<12) ? COLOR_WHITE : COLOR_GRAY);
        compo_textbox_set_forecolor(txt_pm,!( hour<12) ? COLOR_WHITE : COLOR_GRAY);

        func_disturd_sub_get_timer(1,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(1,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_28_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+141,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }
    else
    {
        func_disturd_sub_get_timer(0,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(0,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_28_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+103,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }

    compo_button_t * btn_hour = compo_button_create(frm);///小时滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_hour,uteModuleSystemtime12HOn() ? TXT_12_HOUR_X : TXT_24_HOUR_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_min,uteModuleSystemtime12HOn() ? TXT_12_MIN_X : TXT_24_MIN_X,58+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I338001_28_SET_COMFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I338001_28_SET_COMFIRM_BIN).hei/2-20);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    if(func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t*f_alarm_clock_sub_set = (f_disturd_sub_set_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_alarm_clock_sub_set->disturd_hour[i] = hour_data[i];
            f_alarm_clock_sub_set->disturd_min[i]  = min_data[i];
        }
        f_alarm_clock_sub_set->time_scale = uteModuleSystemtime12HOn();
        f_alarm_clock_sub_set->hour_old = f_alarm_clock_sub_set->hour = hour_data[2];
        f_alarm_clock_sub_set->min_old  = f_alarm_clock_sub_set->min  = min_data[2];
        f_alarm_clock_sub_set->am_pm_flag = (hour<12) ? true : false;
    }

    return frm;
}


//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    u8 hour = f_disturd_set->hour;
    u8 min  = f_disturd_set->min;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
        {
            if(f_disturd_set->time_scale)
            {
                if(f_disturd_set->am_pm_flag)
                {
                    if(hour==12)hour=0;
                }
                else
                {
                    if(hour!=12)hour+=12;
                }
            }
            if(sys_cb.disturd_sel)
            {
                sys_cb.disturd_end_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
            }
            else
            {
                sys_cb.disturd_start_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
            }
            // printf("set.disturd_end_time_sec:%d\n", sys_cb.disturd_end_time_sec);
            uteTaskGuiStartScreen(FUNC_SET_SUB_DISTURD, 0, __func__);
        }
        break;
        default:
            break;
    }
}
/*函数功能：设置小时与分钟背景图片*/
static void func_disturd_sub_set_timer_mode(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    if(f_disturd_set->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_HOUR_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_HOUR_MODE;
                break;
            case COMPO_ID_BTN_MIN_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_MIN_MODE;
                break;
            case COMPO_ID_BTN_PM_AM:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_AM_PS;
                break;
        }

    }
}
/*函数功能：滑动处理*/
static void func_disturd_sub_move(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            if(f_disturd_set->set_num_flag == SET_HOUR_MODE)//小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    if(f_disturd_set->time_scale) //12小时制
                    {
                        f_disturd_set->hour = (12+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%12;
                        func_disturd_sub_get_timer(1,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        f_disturd_set->hour = (24+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%24;
                        func_disturd_sub_get_timer(0,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_HOUR_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    f_disturd_set->min = (60+f_disturd_set->min_old-f_disturd_set->move_dy_data/TXT_SPACING)%60;
                    func_disturd_sub_get_timer(1,1,f_disturd_set->min,f_disturd_set->disturd_min);

                    if(f_disturd_set->time_scale) //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    else    //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_min[idx-COMPO_ID_TXT_MIN_1]);
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_MIN_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                f_disturd_set->move_am_pm_dy = dy;
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy <0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    else if(f_disturd_set->move_am_pm_dy> TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = TXT_SPACING;
                    }

                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy >0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    if(f_disturd_set->move_am_pm_dy< -TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = -TXT_SPACING;
                    }
                }

                for(int i=1,idx=COMPO_ID_TXT_PM_BG; idx>=COMPO_ID_TXT_AM_BG; idx--,i++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    compo_textbox_set_pos(txt,16+48/2,DISTURD_TXT_Y[i+(1-f_disturd_set->am_pm_flag)]+f_disturd_set->move_am_pm_dy);
                }
            }
        }
        else   //松手状态
        {
            if(f_disturd_set->set_num_flag == SET_HOUR_MODE) //小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }

                }
                f_disturd_set->hour_old = f_disturd_set->hour;
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }

                }
                f_disturd_set->min_old = f_disturd_set->min;
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy == TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = false;
                    }
                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy == -TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = true;
                    }
                }

                compo_textbox_t *txt_am = compo_getobj_byid(COMPO_ID_TXT_AM_BG);
                compo_textbox_t *txt_pm = compo_getobj_byid(COMPO_ID_TXT_PM_BG);

                compo_textbox_set_pos(txt_am,16+48/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
                compo_textbox_set_pos(txt_pm,16+48/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
                compo_textbox_set_forecolor(txt_am,f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
                compo_textbox_set_forecolor(txt_pm,!f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
            }
        }

    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
//组件ID

#define TXT_SPACING    (142-89)
#define CENTER_TXT_Y   (158/2)
#define TXT_12_HOUR_X  (160+30/2)
#define TXT_12_MIN_X   (266+40/2)
#define TXT_24_HOUR_X  (84+40/2)
#define TXT_24_MIN_X   (237+43/2)

const uint16_t TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

/*函数功能：获取设置时间（上下 & 上上下下 ）的数*/
/*入口参数：mode->时间参数 ，0为24小时制 ，1为12小时制*/
/*入口参数：timer->设置时间*/
/*入口参数：hour_flag->设置小时或分钟 0小时 1分钟*/
/*返回参数：timer_data->返回timer（上下 & 上上下下 ）的时间1~5数组*/
static void func_disturd_sub_get_timer(bool mode,bool hour_flag,u8 timer,u8 *timer_data)
{
    s8 num=-2;

    if(mode == 0)
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+24+(num))%24;
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
    else
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+12+(num))%12;
                if(timer_data[i] == 0)
                {
                    timer_data[i] = 12;
                }
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }

    }

}
/*函数功能：12小时制刷新按钮颜色*/
static void func_disturd_sub_set_refresh(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    compo_button_t * btn_am = compo_getobj_byid(COMPO_ID_BTN_AM_BG);
    compo_button_t * btn_pm = compo_getobj_byid(COMPO_ID_BTN_PM_BG);
    compo_textbox_t *txt_am = compo_getobj_byid(COMPO_ID_TXT_AM_BG);
    compo_textbox_t *txt_pm = compo_getobj_byid(COMPO_ID_TXT_PM_BG);

    if(f_disturd_set->am_pm_flag == true)
    {
        compo_button_set_bgimg(btn_am, UI_BUF_I340001_PUBLIC_SHANGWU02_BIN);
        compo_button_set_bgimg(btn_pm, UI_BUF_I340001_PUBLIC_SHANGWU01_BIN);
        compo_textbox_set_forecolor(txt_am, COLOR_WHITE);
        compo_textbox_set_forecolor(txt_pm, COLOR_GRAY);
    }
    else
    {
        compo_button_set_bgimg(btn_pm, UI_BUF_I340001_PUBLIC_SHANGWU02_BIN);
        compo_button_set_bgimg(btn_am, UI_BUF_I340001_PUBLIC_SHANGWU01_BIN);
        compo_textbox_set_forecolor(txt_pm, COLOR_WHITE);
        compo_textbox_set_forecolor(txt_am, COLOR_GRAY);
    }
}
/*函数功能：初始化数据*/
static void func_disturd_sub_time_init(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    f_disturd_set->time_scale = uteModuleSystemtime12HOn();
    if(sys_cb.disturd_sel)
    {
        f_disturd_set->hour = sys_cb.disturd_end_time_sec/3600;
        f_disturd_set->min = sys_cb.disturd_end_time_sec%3600/60;

    }
    else
    {
        f_disturd_set->hour = sys_cb.disturd_start_time_sec/3600;
        f_disturd_set->min = sys_cb.disturd_start_time_sec%3600/60;
    }

    if(f_disturd_set->time_scale)//12小时制
    {
        if(f_disturd_set->hour && f_disturd_set->hour<=12)
        {
            f_disturd_set->am_pm_flag = true;
        }
        else
        {
            f_disturd_set->am_pm_flag = false;
        }
        f_disturd_set->hour %=12;
    }
    else
    {
        f_disturd_set->hour %=24;
    }
    f_disturd_set->hour_old = f_disturd_set->hour;
    f_disturd_set->min_old  = f_disturd_set->min;

    if(f_disturd_set->time_scale)
    {
        func_disturd_sub_set_refresh();
    }
    // printf("hour1:%d\n",f_disturd_set->hour_old);
}
/*函数功能：设置勿扰时间*/
static void func_disturd_sub_set_time(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    u8 hour = f_disturd_set->hour;
    u8 min  = f_disturd_set->min;


    if(f_disturd_set->time_scale)
    {
        if(f_disturd_set->am_pm_flag)
        {
            if(hour==0)hour=12;
        }
        else
        {
            if(hour!=0)hour+=12;
        }
    }

    if(sys_cb.disturd_sel)
    {
        sys_cb.disturd_end_time_sec = hour*3600+min*60;
        uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
        uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
    }
    else
    {
        sys_cb.disturd_start_time_sec = hour*3600+min*60;
        uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
        uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
    }
    // printf("hour2:%d\n",sys_cb.disturd_start_time_sec);
}

/*函数功能：设置小时与分钟背景图片*/
static void func_disturd_sub_set_timer_pic_bg(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    if(f_disturd_set->touch_flag == false)
    {
        compo_button_t * btn_hour = compo_getobj_byid(COMPO_ID_BTN_HOUR_BG);
        compo_button_t * btn_min  = compo_getobj_byid(COMPO_ID_BTN_MIN_BG);

        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_HOUR_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_hour_flag = true;
                if(f_disturd_set->time_scale)//12小时制
                {
                    compo_button_set_bgimg(btn_hour,UI_BUF_I340001_PUBLIC_KUANG_BLUE_BIN);
                    compo_button_set_bgimg(btn_min,UI_BUF_I340001_PUBLIC_KUANG_GREY_SMALL_BIN);
                }
                else
                {
                    compo_button_set_bgimg(btn_hour,UI_BUF_I340001_PUBLIC_KUANG_BLUE_BIG_BIN);
                    compo_button_set_bgimg(btn_min,UI_BUF_I340001_PUBLIC_KUANG_GREY_BIG_BIN);
                }
                break;
            case COMPO_ID_BTN_MIN_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_hour_flag = false;
                if(f_disturd_set->time_scale)//12小时制
                {
                    compo_button_set_bgimg(btn_min,UI_BUF_I340001_PUBLIC_KUANG_BLUE_BIN);
                    compo_button_set_bgimg(btn_hour,UI_BUF_I340001_PUBLIC_KUANG_GREY_SMALL_BIN);
                }
                else
                {
                    compo_button_set_bgimg(btn_min,UI_BUF_I340001_PUBLIC_KUANG_BLUE_BIG_BIN);
                    compo_button_set_bgimg(btn_hour,UI_BUF_I340001_PUBLIC_KUANG_GREY_BIG_BIN);
                }
                break;
        }

    }
}


///创建定时勿扰设置窗体
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    if(sys_cb.disturd_sel)
    {
        compo_form_set_title(frm, i18n[STR_DISTURD_TIM_END]);
    }
    else
    {
        compo_form_set_title(frm, i18n[STR_DISTURD_TIM_START]);
    }

    u8 hour ;
    u8 min  ;
    char txt_buf[20];
    s8 hour_data[5];
    s8 min_data[5];
    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec / 3600;
        min = (sys_cb.disturd_end_time_sec % 3600) / 60;
    }
    else
    {
        hour = sys_cb.disturd_start_time_sec / 3600;
        min = (sys_cb.disturd_start_time_sec % 3600) / 60;
    }

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 84+CENTER_TXT_Y, GUI_SCREEN_WIDTH,CENTER_TXT_Y*2);

    if(uteModuleSystemtime12HOn())///12小时制
    {
        compo_button_t * btn_am = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 上午
        compo_button_set_pos(btn_am,42+58/2,120-84+32/2);
        compo_setid(btn_am,COMPO_ID_BTN_AM_BG);

        compo_button_t * btn_pm = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 下午
        compo_button_set_pos(btn_pm,42+58/2,176-84+32/2);
        compo_setid(btn_pm,COMPO_ID_BTN_PM_BG);

        compo_textbox_t *txt_am = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_AM]));///12小时制文本 上午
        compo_textbox_set_location(txt_am,42+58/2,120-84+32/2,56,32);
        compo_textbox_set(txt_am,i18n[STR_AM]);
        compo_setid(txt_am,COMPO_ID_TXT_AM_BG);

        compo_textbox_t *txt_pm = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_PM]));///12小时制文本 下午
        compo_textbox_set_location(txt_pm,42+58/2,176-84+32/2,56,32);
        compo_textbox_set(txt_pm,i18n[STR_PM]);
        compo_setid(txt_pm,COMPO_ID_TXT_PM_BG);

        if(hour && hour<=12)  //更新按钮与文本颜色
        {
            compo_button_set_bgimg(btn_am, UI_BUF_I340001_PUBLIC_SHANGWU02_BIN);
            compo_button_set_bgimg(btn_pm, UI_BUF_I340001_PUBLIC_SHANGWU01_BIN);
            compo_textbox_set_forecolor(txt_am, COLOR_WHITE);
            compo_textbox_set_forecolor(txt_pm, COLOR_GRAY);
        }
        else
        {
            compo_button_set_bgimg(btn_pm, UI_BUF_I340001_PUBLIC_SHANGWU02_BIN);
            compo_button_set_bgimg(btn_am, UI_BUF_I340001_PUBLIC_SHANGWU01_BIN);
            compo_textbox_set_forecolor(txt_pm, COLOR_WHITE);
            compo_textbox_set_forecolor(txt_am, COLOR_GRAY);
        }

        func_disturd_sub_get_timer(1,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(1,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
        }

        compo_button_t * btn_hour = compo_button_create_page_by_image(frm,page,UI_BUF_I340001_PUBLIC_KUANG_GREY_SMALL_BIN);///小时滑动按钮 UI_BUF_I340001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_hour,TXT_12_HOUR_X,CENTER_TXT_Y);
        compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

        compo_button_t * btn_min = compo_button_create_page_by_image(frm,page,UI_BUF_I340001_PUBLIC_KUANG_GREY_SMALL_BIN);///分钟滑动按钮 UI_BUF_I340001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_min,TXT_12_MIN_X,CENTER_TXT_Y);
        compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

        compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        compo_textbox_set_pos(txt_colon,32/2+213,CENTER_TXT_Y);
        compo_textbox_set(txt_colon,":");
    }
    else
    {
        func_disturd_sub_get_timer(0,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(0,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
        }

        compo_button_t * btn_hour = compo_button_create_page_by_image(frm,page,UI_BUF_I340001_PUBLIC_KUANG_GREY_BIG_BIN);///小时滑动按钮 UI_BUF_I340001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_hour,TXT_24_HOUR_X,CENTER_TXT_Y);
        compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

        compo_button_t * btn_min = compo_button_create_page_by_image(frm,page,UI_BUF_I340001_PUBLIC_KUANG_GREY_BIG_BIN);///分钟滑动按钮 UI_BUF_I340001_PUBLIC_KUANG_BLUE_BIN
        compo_button_set_pos(btn_min,TXT_24_MIN_X,CENTER_TXT_Y);
        compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

        compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        compo_textbox_set_pos(txt_colon,32/2+164,CENTER_TXT_Y);
        compo_textbox_set(txt_colon,":");
    }


    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I340001_PUBLIC_RECTANGLE01_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,64/2+266);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    compo_textbox_t *txt_ok = compo_textbox_create(frm,strlen(i18n[STR_OK]));
    compo_textbox_set_location(txt_ok,GUI_SCREEN_CENTER_X,64/2+266, 214,64);
    compo_textbox_set(txt_ok,i18n[STR_OK]);

    if(func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t*f_disturd_sub_set = (f_disturd_sub_set_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_disturd_sub_set->disturd_hour[i] = hour_data[i];
            f_disturd_sub_set->disturd_min[i] = min_data[i];
        }
    }

    return frm;
}


/*函数功能：滑动处理*/
static void func_disturd_sub_move(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        // printf("11111111111\n");
        char txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            if(f_disturd_set->set_hour_flag == true)//小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    if(f_disturd_set->time_scale) //12小时制
                    {
                        f_disturd_set->hour = (12+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%12;
                        func_disturd_sub_get_timer(1,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        f_disturd_set->hour = (24+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%24;
                        func_disturd_sub_get_timer(0,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else    //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    f_disturd_set->min = (60+f_disturd_set->min_old-f_disturd_set->move_dy_data/TXT_SPACING)%60;
                    func_disturd_sub_get_timer(1,1,f_disturd_set->min,f_disturd_set->disturd_min);

                    if(f_disturd_set->time_scale) //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    else    //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_min[idx-COMPO_ID_TXT_MIN_1]);
                    compo_textbox_set(txt,txt_buf);
                }
            }
        }
        else   //松手状态
        {
            if(f_disturd_set->set_hour_flag == true) //小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }

                }
                f_disturd_set->hour_old = f_disturd_set->hour;
            }
            else  //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }

                }
                f_disturd_set->min_old = f_disturd_set->min;
            }
        }

    }
}

//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_AM_BG:
            f_disturd_set->am_pm_flag = true;
            break;
        case COMPO_ID_BTN_PM_BG:
            f_disturd_set->am_pm_flag = false;
            break;
        case COMPO_ID_BTN_SURE:
            func_disturd_sub_set_time();
            func_cb.sta = FUNC_SET_SUB_DISTURD;
            task_stack_pop();
            break;

        default:
            break;
    }
    // printf("min:%d\n",f_disturd_set->min_old);
    if(f_disturd_set->time_scale)
    {
        func_disturd_sub_set_refresh();
    }
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

enum
{
    SET_HOUR_MODE=0,
    SET_MIN_MODE,
    SET_AM_PS,
};

#define TXT_SPACING    (105-62)
#define CENTER_TXT_Y   (130/2)
#define TXT_12_HOUR_X  (85+58/2)
#define TXT_12_MIN_X   (172+58/2)
#define TXT_24_HOUR_X  (4+100/2)
#define TXT_24_MIN_X   (168+34/2)

const uint16_t DISTURD_TXT_Y[5]= {CENTER_TXT_Y-TXT_SPACING*2,CENTER_TXT_Y-TXT_SPACING,CENTER_TXT_Y,CENTER_TXT_Y+TXT_SPACING,CENTER_TXT_Y+TXT_SPACING*2}; ///文本Y轴

/*函数功能：获取设置时间（上下 & 上上下下 ）的数*/
/*入口参数：mode->时间参数 ，0为24小时制 ，1为12小时制*/
/*入口参数：timer->设置时间*/
/*入口参数：hour_flag->设置小时或分钟 0小时 1分钟*/
/*返回参数：timer_data->返回timer（上下 & 上上下下 ）的时间1~5数组*/
static void func_disturd_sub_get_timer(bool mode,bool hour_flag,u8 timer,s8 *timer_data)
{
    s8 num=-2;

    if(mode == 0)
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+24+(num))%24;
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
    else
    {
        for(int i=0; i<5; i++) ///前后数据处理
        {
            if(hour_flag == 0)
            {
                timer_data[i] = (timer+12+(num))%12;
                if(timer_data[i] == 0)
                {
                    timer_data[i] = 12;
                }
            }
            else
            {
                timer_data[i]  = (timer+60+(num))%60;
            }
            num+=1;
        }
    }
}
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ALARM_CLOCK_SET]);

    u8 hour ;
    u8 min  ;
    char txt_buf[20];
    s8 hour_data[5];
    s8 min_data[5];

    if(sys_cb.disturd_sel)
    {
        hour = sys_cb.disturd_end_time_sec/3600;
        min = sys_cb.disturd_end_time_sec%3600/60;

    }
    else
    {
        hour = sys_cb.disturd_start_time_sec/3600;
        min = sys_cb.disturd_start_time_sec%3600/60;
    }

    //创建一个页面用于限制滚动的时间文本
    widget_page_t* page = widget_page_create(frm->page_body);
    widget_set_location(page, GUI_SCREEN_CENTER_X, 52+130/2, GUI_SCREEN_WIDTH,130);

    if(uteModuleSystemtime12HOn())///12小时制
    {
        compo_button_t * btn_am = compo_button_create_page_by_image(frm,page,0);///12小时制按钮 上午 下午
        compo_button_set_location(btn_am,16+48/2,130/2,58,130);
        compo_setid(btn_am,COMPO_ID_BTN_PM_AM);

        compo_textbox_t *txt_am = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_AM]));///12小时制文本 上午
        compo_textbox_set_location(txt_am,16+48/2,0,48,30);
        compo_textbox_set(txt_am,i18n[STR_AM]);
        compo_setid(txt_am,COMPO_ID_TXT_AM_BG);

        compo_textbox_t *txt_pm = compo_textbox_create_for_page(frm,page,strlen(i18n[STR_PM]));///12小时制文本 下午
        compo_textbox_set_location(txt_pm,16+48/2,0,48,30);
        compo_textbox_set(txt_pm,i18n[STR_PM]);
        compo_setid(txt_pm,COMPO_ID_TXT_PM_BG);

        compo_textbox_set_pos(txt_am,16+48/2,(hour && hour<=12) ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
        compo_textbox_set_pos(txt_pm,16+48/2,(hour && hour<=12) ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
        compo_textbox_set_forecolor(txt_am,(hour && hour<=12) ? COLOR_WHITE : COLOR_GRAY);
        compo_textbox_set_forecolor(txt_pm,!(hour && hour<=12) ? COLOR_WHITE : COLOR_GRAY);

        func_disturd_sub_get_timer(1,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(1,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_28_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+141,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }
    else
    {
        func_disturd_sub_get_timer(0,0,hour,hour_data);///获取时间
        func_disturd_sub_get_timer(0,1,min,min_data);
        for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
        {
            memset(txt_buf,0,sizeof(txt_buf));
            compo_textbox_t *txt = compo_textbox_create_for_page(frm,page,2);
            compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_NUM_28_BIN);
            if(idx<=COMPO_ID_TXT_HOUR_5)
            {
                compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",hour_data[idx-COMPO_ID_TXT_HOUR_1]);
            }
            else
            {
                compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                snprintf(txt_buf,sizeof(txt_buf),"%02d",min_data[idx-COMPO_ID_TXT_MIN_1]);
            }
            compo_setid(txt,idx);
            compo_textbox_set(txt,txt_buf);
            compo_textbox_set_forecolor(txt,(idx==COMPO_ID_TXT_HOUR_3 || idx==COMPO_ID_TXT_MIN_3) ? COLOR_WHITE : COLOR_GRAY);
        }
        // compo_textbox_t *txt_colon = compo_textbox_create_for_page(frm,page,1);
        // compo_textbox_set_font(txt_colon,UI_BUF_0FONT_FONT_NUM_28_BIN);
        // compo_textbox_set_pos(txt_colon,32/2+103,CENTER_TXT_Y);
        // compo_textbox_set(txt_colon,":");
    }

    compo_button_t * btn_hour = compo_button_create(frm);///小时滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_hour,uteModuleSystemtime12HOn() ? TXT_12_HOUR_X : TXT_24_HOUR_X,48+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_hour,COMPO_ID_BTN_HOUR_BG);

    compo_button_t * btn_min = compo_button_create(frm);///分钟滑动按钮 UI_BUF_I330001_PUBLIC_KUANG_BLUE_BIN
    compo_button_set_location(btn_min,uteModuleSystemtime12HOn() ? TXT_12_MIN_X : TXT_24_MIN_X,48+CENTER_TXT_Y,70,CENTER_TXT_Y*2);
    compo_setid(btn_min,COMPO_ID_BTN_MIN_BG);

    compo_button_t * btn_ok = compo_button_create_by_image(frm,UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN);///确定按钮
    compo_button_set_pos(btn_ok,GUI_SCREEN_CENTER_X,GUI_SCREEN_HEIGHT-gui_image_get_size(UI_BUF_I342001_20_ALARM_CLOCK_CONFIRM_BIN).hei/2-10);
    compo_setid(btn_ok,COMPO_ID_BTN_SURE);

    if(func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        f_disturd_sub_set_t*f_alarm_clock_sub_set = (f_disturd_sub_set_t*)func_cb.f_cb;
        for(int i=0; i<5; i++)
        {
            f_alarm_clock_sub_set->disturd_hour[i] = hour_data[i];
            f_alarm_clock_sub_set->disturd_min[i]  = min_data[i];
        }
        f_alarm_clock_sub_set->time_scale = uteModuleSystemtime12HOn();
        f_alarm_clock_sub_set->hour_old = f_alarm_clock_sub_set->hour = hour_data[2];
        f_alarm_clock_sub_set->min_old  = f_alarm_clock_sub_set->min  = min_data[2];
        f_alarm_clock_sub_set->am_pm_flag = (hour && hour<=12) ? true : false;
    }

    return frm;
}


//单击按钮
static void func_disturd_sub_set_button_click(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    u8 hour = f_disturd_set->hour;
    u8 min  = f_disturd_set->min;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_SURE:
        {
            if(f_disturd_set->time_scale)
            {
                if(f_disturd_set->am_pm_flag)
                {
                    if(hour==0)hour=12;
                }
                else
                {
                    if(hour!=0)hour+=12;
                }
            }
            if(sys_cb.disturd_sel)
            {
                sys_cb.disturd_end_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_END_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_end_time_sec / 60);
            }
            else
            {
                sys_cb.disturd_start_time_sec = hour*3600+min*60;
                uteModuleNotDisturbSetTimeStatus(NOT_DISTURB_START_TIME);
                uteModuleNotDisturbSetTime(sys_cb.disturd_start_time_sec / 60);
            }
            uteTaskGuiStartScreen(FUNC_SET_SUB_DISTURD, 0, __func__);
        }
        break;
        default:
            break;
    }
}
/*函数功能：设置小时与分钟背景图片*/
static void func_disturd_sub_set_timer_mode(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    if(f_disturd_set->touch_flag == false)
    {
        switch(compo_get_button_id())
        {
            case COMPO_ID_BTN_HOUR_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_HOUR_MODE;
                break;
            case COMPO_ID_BTN_MIN_BG:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_MIN_MODE;
                break;
            case COMPO_ID_BTN_PM_AM:
                f_disturd_set->touch_flag = true;
                f_disturd_set->set_num_flag = SET_AM_PS;
                break;
        }

    }
}
/*函数功能：滑动处理*/
static void func_disturd_sub_move(void)
{
    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;

    if(f_disturd_set->touch_flag == true)
    {
        s8 txt_buf[20];
        s32 dx, dy;
        f_disturd_set->touch_flag = ctp_get_dxy(&dx, &dy);
        if(f_disturd_set->touch_flag == true)//触摸状态
        {
            f_disturd_set->move_dy_data = ((int)(dy/TXT_SPACING))*TXT_SPACING;
            f_disturd_set->move_dy = dy-f_disturd_set->move_dy_data;

            if(f_disturd_set->set_num_flag == SET_HOUR_MODE)//小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    if(f_disturd_set->time_scale) //12小时制
                    {
                        f_disturd_set->hour = (12+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%12;
                        func_disturd_sub_get_timer(1,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        f_disturd_set->hour = (24+f_disturd_set->hour_old-f_disturd_set->move_dy_data/TXT_SPACING)%24;
                        func_disturd_sub_get_timer(0,0,f_disturd_set->hour,f_disturd_set->disturd_hour);
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]+f_disturd_set->move_dy);
                        snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_hour[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_HOUR_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    memset(txt_buf,0,sizeof(txt_buf));
                    f_disturd_set->min = (60+f_disturd_set->min_old-f_disturd_set->move_dy_data/TXT_SPACING)%60;
                    func_disturd_sub_get_timer(1,1,f_disturd_set->min,f_disturd_set->disturd_min);

                    if(f_disturd_set->time_scale) //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    else    //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]+f_disturd_set->move_dy);
                    }
                    snprintf(txt_buf,sizeof(txt_buf),"%02d",f_disturd_set->disturd_min[idx-COMPO_ID_TXT_MIN_1]);
                    compo_textbox_set_forecolor(txt,idx==COMPO_ID_TXT_MIN_3 ? COLOR_WHITE : COLOR_GRAY);
                    compo_textbox_set(txt,txt_buf);
                }
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                f_disturd_set->move_am_pm_dy = dy;
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy <0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    else if(f_disturd_set->move_am_pm_dy> TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = TXT_SPACING;
                    }

                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy >0)
                    {
                        f_disturd_set->move_am_pm_dy = 0;
                    }
                    if(f_disturd_set->move_am_pm_dy< -TXT_SPACING)
                    {
                        f_disturd_set->move_am_pm_dy = -TXT_SPACING;
                    }
                }

                for(int i=1,idx=COMPO_ID_TXT_PM_BG; idx>=COMPO_ID_TXT_AM_BG; idx--,i++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    compo_textbox_set_pos(txt,16+48/2,DISTURD_TXT_Y[i+(1-f_disturd_set->am_pm_flag)]+f_disturd_set->move_am_pm_dy);
                }
            }
        }
        else   //松手状态
        {
            if(f_disturd_set->set_num_flag == SET_HOUR_MODE) //小时
            {
                for(int idx=COMPO_ID_TXT_HOUR_1; idx<=COMPO_ID_TXT_HOUR_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_HOUR_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_HOUR_1]);
                    }

                }
                f_disturd_set->hour_old = f_disturd_set->hour;
            }
            else if(f_disturd_set->set_num_flag == SET_MIN_MODE)   //分钟
            {
                for(int idx=COMPO_ID_TXT_MIN_1; idx<=COMPO_ID_TXT_MIN_5; idx++) ///创建滑动文本
                {
                    compo_textbox_t *txt = compo_getobj_byid(idx);
                    if(f_disturd_set->time_scale)  //12小时制
                    {
                        compo_textbox_set_pos(txt,TXT_12_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }
                    else  //24小时制
                    {
                        compo_textbox_set_pos(txt,TXT_24_MIN_X,DISTURD_TXT_Y[idx-COMPO_ID_TXT_MIN_1]);
                    }

                }
                f_disturd_set->min_old = f_disturd_set->min;
            }
            else if(f_disturd_set->set_num_flag == SET_AM_PS)//上下午
            {
                if(f_disturd_set->am_pm_flag == true)
                {
                    if(f_disturd_set->move_am_pm_dy == TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = false;
                    }
                }
                else
                {
                    if(f_disturd_set->move_am_pm_dy == -TXT_SPACING)
                    {
                        f_disturd_set->am_pm_flag = true;
                    }
                }

                compo_textbox_t *txt_am = compo_getobj_byid(COMPO_ID_TXT_AM_BG);
                compo_textbox_t *txt_pm = compo_getobj_byid(COMPO_ID_TXT_PM_BG);

                compo_textbox_set_pos(txt_am,16+48/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[2] : DISTURD_TXT_Y[3]);
                compo_textbox_set_pos(txt_pm,16+48/2,f_disturd_set->am_pm_flag ? DISTURD_TXT_Y[1] : DISTURD_TXT_Y[2]);
                compo_textbox_set_forecolor(txt_am,f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
                compo_textbox_set_forecolor(txt_pm,!f_disturd_set->am_pm_flag ? COLOR_WHITE : COLOR_GRAY);
            }
        }

    }
}

#else
///创建定时勿扰设置窗体
compo_form_t *func_disturd_sub_set_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
static void func_disturd_sub_move(void)
{
}
//单击按钮
static void func_disturd_sub_set_button_click(void)
{
}
static void func_disturd_sub_set_timer_pic_bg(void)
{
}
static void func_disturd_sub_time_init(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//定时勿扰设置功能事件处理
static void func_disturd_sub_set_process(void)
{
    func_disturd_sub_move();
    func_process();
}

//定时勿扰设置功能消息处理
static void func_disturd_sub_set_message(size_msg_t msg)
{

    f_disturd_sub_set_t *f_disturd_set = (f_disturd_sub_set_t*) func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_disturd_sub_set_button_click();
            break;
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT|| GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
            func_disturd_sub_set_timer_pic_bg();
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT \
      || GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
            func_disturd_sub_set_timer_mode();
#endif
            break;
        case MSG_SYS_500MS:
        // printf("am_pm_flag:%d\n",f_disturd_set->am_pm_flag);
        default:
            func_message(msg);
            break;
    }
}

//进入定时勿扰设置功能
static void func_disturd_sub_set_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_disturd_sub_set_t));
    func_cb.frm_main = func_disturd_sub_set_form_create();
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    func_disturd_sub_time_init();
#endif
    // printf("hour4:%d\n",sys_cb.disturd_start_time_sec);
}

//退出定时勿扰设置功能
static void func_disturd_sub_set_exit(void)
{
    printf("%s\n",__func__);
    func_cb.last = FUNC_DISTURD_SUB_SET;
    // printf("hour4:%d\n",sys_cb.disturd_start_time_sec);
}

//定时勿扰设置功能
void func_disturd_sub_set(void)
{
    printf("%s\n", __func__);
    func_disturd_sub_set_enter();
    while (func_cb.sta == FUNC_DISTURD_SUB_SET)
    {
        func_disturd_sub_set_process();
        func_disturd_sub_set_message(msg_dequeue());
    }
    func_disturd_sub_set_exit();
}
