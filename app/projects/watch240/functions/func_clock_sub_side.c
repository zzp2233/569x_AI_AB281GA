#include "include.h"
#include "func.h"
#include "func_clock.h"
#include "ute_module_systemtime.h"
#include "ute_module_weather.h"

#if UTE_MODULE_SCREENS_CLOCK_SUB_SIDE_SUPPORT

typedef struct f_clock_sub_side_t_
{
    u32 wether_res;
    s16 last_app_x;
    s16 last_app_y;
    u16 last_app_id;
} f_clock_sub_sider_t;

enum
{
    COMPO_ID_APP_1=1,
    COMPO_ID_APP_2,
    COMPO_ID_APP_3,
    COMPO_ID_APP_4,
};

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
static const f_clock_sub_sider_t f_clock_sub_sider[] =
{
    [WEATHER_TYPE_UNKNOWN]={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_00_UNKNOWEN_BIN},
    [WEATHER_TYPE_SUNNY]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_01_SUNNY_BIN},
    [WEATHER_TYPE_CLOUDY]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_02_CLOUDY_BIN},
    [WEATHER_TYPE_OVERCAST]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_03_OVERCAST_BIN},
    [WEATHER_TYPE_SHOWER_RAIN]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_04_SHOWER_BIN},
    [WEATHER_TYPE_THUNDERSHOWER_RAIN]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_05_T_STORM_BIN},
    [WEATHER_TYPE_RAIN_SNOW]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_06_SLEET_BIN},
    [WEATHER_TYPE_LIGHT_RAIN]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_07_LIGHT_RAIN_BIN},
    [WEATHER_TYPE_HEAVY_RAIN]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_08_HEAVY_RAIN_BIN},
    [WEATHER_TYPE_SNOW]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_09_SNOW_BIN},
    [WEATHER_TYPE_SAND_STORM]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_10_SAND_STORM_BIN},
    [WEATHER_TYPE_HAZE]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_11_HAZE_BIN},
    [WEATHER_TYPE_WIND]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_12_WINDY_BIN},
    [WEATHER_TYPE_NIGHT]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_13_NIGHT_SUNNY_BIN},
    [WEATHER_TYPE_NIGHT+1]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_14_NIGHT_CLOUDY_BIN},
    [WEATHER_TYPE_NIGHT+2]  ={.wether_res=UI_BUF_I335001_1_WEATHER_ICON_WEATHER_80X80_X18_Y86_15_NIGHT_RAIN_BIN},
};

#define GUI_SIDE_WIDTH_SIZE (GUI_SIDE_MENU_WIDTH+20)
#define GUI_SIDE_CENTER_X  (GUI_SIDE_WIDTH_SIZE/2)
static const f_clock_sub_sider_t f_clock_sub_sider_app[] =
{
    {.last_app_x=GUI_SIDE_CENTER_X-28, .last_app_y =120/2+154-28, .last_app_id=COMPO_ID_APP_1 },
    {.last_app_x=GUI_SIDE_CENTER_X+28, .last_app_y =120/2+154-28, .last_app_id=COMPO_ID_APP_1 },
    {.last_app_x=GUI_SIDE_CENTER_X-28, .last_app_y =120/2+154+28, .last_app_id=COMPO_ID_APP_1 },
    {.last_app_x=GUI_SIDE_CENTER_X+28, .last_app_y =120/2+154+28, .last_app_id=COMPO_ID_APP_1 },
};


#define DEFAULT_LATEST_TASK_NUM 4   //最近任务不足5个时默认值补上
const u8 last_default[DEFAULT_LATEST_TASK_NUM] = {FUNC_SLEEP, FUNC_SPORT, FUNC_HEARTRATE,FUNC_ACTIVITY};
//根据序号获取最近任务序号（idx=0为最近，无任务返回0）(idx<=3)
static u8 side_get_latest_func(u8 idx)
{
    u8 i, j;
    u8 latest_default_tmp[DEFAULT_LATEST_TASK_NUM] = {0};
    u8 latest_cnt = latest_task_count();
    if (latest_cnt > idx)
    {
        return latest_task_get(idx);
    }
    else
    {
        j = 0;
        for (i = 0; i < DEFAULT_LATEST_TASK_NUM; i++)     //最近任务不足DEFAULT_LATEST_TASK_NUM个且包含默认值
        {
            if (latest_task_find(last_default[i]) == -1)
            {
                latest_default_tmp[j] = last_default[i];
                j++;
            }
        }
        return latest_default_tmp[idx - latest_cnt];
    }
}

//根据序号获取最近任务图标资源地址
static u32 side_get_latest_icon(u8 idx)
{
    return func_get_icon_addr(side_get_latest_func(idx));
}

//创建边菜单
static void func_clock_sub_side_form_create(void)
{
    char txt_buf[70];

    compo_form_t *frm = compo_form_create(true);
    compo_form_set_location(frm, 0, 0, GUI_SIDE_MENU_WIDTH+20, GUI_SCREEN_HEIGHT);

    //创建遮罩层
    compo_shape_t *masklayer = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(masklayer, COLOR_BLACK);
    compo_shape_set_location(masklayer, GUI_SIDE_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SIDE_WIDTH_SIZE, GUI_SCREEN_HEIGHT);
    compo_shape_set_alpha(masklayer, 255);
    compo_shape_set_radius(masklayer,30);

    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    // hour
    compo_textbox_t *textbox = compo_textbox_create(frm, 2);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,GUI_SIDE_CENTER_X-35,40);
    compo_bonddata(textbox, COMPO_BOND_HOUR);
    compo_set_bonddata((component_t *)textbox, time_to_tm(compo_cb.rtc_cnt));

    // dot
    textbox = compo_textbox_create(frm, 1);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,GUI_SIDE_CENTER_X-3,40);
    compo_textbox_set(textbox, ":");

    // min
    textbox = compo_textbox_create(frm, 2);
    compo_textbox_set_font(textbox, UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,GUI_SIDE_CENTER_X+30,40);
    compo_bonddata(textbox, COMPO_BOND_MINUTE);
    compo_set_bonddata((component_t *)textbox, time_to_tm(compo_cb.rtc_cnt));

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%02d/%02d %s",time.month,time.day,i18n[STR_SUNDAY+time.week]);
    textbox = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_location(textbox,GUI_SIDE_CENTER_X,85,GUI_SCREEN_CENTER_X-10,30);
    compo_textbox_set(textbox,txt_buf );

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I335001_CLOCK_SIDE_ICON_120X120_X8_Y154_BIN);
    compo_picturebox_set_pos(picbox,GUI_SIDE_CENTER_X,120/2+154);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_CLOCK_SIDE_ICON_120X44_X8_Y104_BIN);
    compo_picturebox_set_pos(picbox,GUI_SIDE_CENTER_X,44/2+104);

    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        weather_date.DayWeather[0] = weather_date.DayWeather[0]>>8;//赋值排序天气状态
        if(uteModuleSystemtimeIsNight()) //是否为夜间
        {
            switch(weather_date.DayWeather[0])
            {
                case WEATHER_TYPE_SUNNY:
                    weather_date.DayWeather[0] = WEATHER_TYPE_NIGHT;
                    break;
                case WEATHER_TYPE_CLOUDY:
                    weather_date.DayWeather[0] = WEATHER_TYPE_NIGHT+1;
                    break;
                case WEATHER_TYPE_THUNDERSHOWER_RAIN:
                case WEATHER_TYPE_SHOWER_RAIN:       //阵雨
                case WEATHER_TYPE_RAIN_SNOW:         //雨夹雪
                case WEATHER_TYPE_LIGHT_RAIN:       //小雨
                case WEATHER_TYPE_HEAVY_RAIN:        //大雨
                    weather_date.DayWeather[0] = WEATHER_TYPE_NIGHT+2;
                    break;
            }
        }

        picbox = compo_picturebox_create(frm, f_clock_sub_sider[WEATHER_TYPE_SHOWER_RAIN].wether_res);
        compo_picturebox_set_pos(picbox,GUI_SIDE_CENTER_X-30,44/2+104);
        compo_picturebox_set_size(picbox,30,30);

        memset(txt_buf,0,sizeof(txt_buf));
        if(displayInfo.isFahrenheit)    //是否为华氏度
        {
            weather_date.fristDayCurrTemperature= weather_date.fristDayCurrTemperature*9/5+32;
            /*pcm 2022-09-19 */
            if(weather_date.fristDayCurrTemperature<(-99))
            {
                weather_date.fristDayCurrTemperature=-99;
            }
            snprintf(txt_buf,sizeof(txt_buf),"%d℉",weather_date.fristDayCurrTemperature);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d℃",weather_date.fristDayCurrTemperature);
        }
    }
    textbox = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_location(textbox,GUI_SIDE_CENTER_X+25,44/2+104,45,30);
    compo_textbox_set(textbox,txt_buf );

    for(int i =0; i<DEFAULT_LATEST_TASK_NUM; i++)
    {
        //创建按钮
        compo_button_t *btn_app = btn_app= compo_button_create_by_image(frm, side_get_latest_icon(i));
        compo_button_set_location(btn_app, f_clock_sub_sider_app[i].last_app_x,f_clock_sub_sider_app[i].last_app_y,50,50);
        compo_setid(btn_app,f_clock_sub_sider_app[i].last_app_id);
    }


    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sub_frm = frm;
    f_clk->masklayer = masklayer;
}

//单击按钮
static void func_clock_sub_side_button_click(void)
{
    int id = compo_get_button_id();

    if(id)
    {
        uteTaskGuiStartScreen(side_get_latest_func(id), 0, __func__);
    }
}
#else
//创建边菜单
static void func_clock_sub_side_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    return frm;
}

//单击按钮
static void func_clock_sub_side_button_click(void)
{

}
#endif

//时钟表盘主要事件流程处理
static void func_clock_sub_side_process(void)
{
    func_clock_sub_process();
}

//时钟表盘边菜单功能消息处理
static void func_clock_sub_side_message(size_msg_t msg)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_SHORT_LEFT:
            if (func_switching(FUNC_SWITCH_MENU_SIDE_BACK, NULL))
            {
                f_clk->sta = FUNC_CLOCK_MAIN;                   //左滑返回到时钟主界面
            }
            break;
        case MSG_CTP_SHORT_RIGHT:
            break;
        case MSG_CTP_CLICK:
            func_clock_sub_side_button_click();
            if (ctp_get_sxy().x > GUI_SIDE_MENU_WIDTH)
            {
                func_switching(FUNC_SWITCH_MENU_SIDE_BACK | FUNC_SWITCH_AUTO, NULL);
                f_clk->sta = FUNC_CLOCK_MAIN;                   //单击边栏右边空白返回到时钟主界面
            }
            break;

        case KU_BACK:
            func_switching(FUNC_SWITCH_MENU_SIDE_BACK | FUNC_SWITCH_AUTO, NULL);
            f_clk->sta = FUNC_CLOCK_MAIN;                       //单击BACK键返回到时钟主界面
            break;

        default:
            func_clock_sub_message(msg);
            break;
    }
}

//时钟表盘边菜单进入处理
static void func_clock_sub_side_enter(void)
{
    func_clock_sub_side_form_create();
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    func_clock_butterfly_set_light_visible(false);
#endif
    if (!func_switching(FUNC_SWITCH_MENU_SIDE_POP, NULL))
    {
        return;                                             //下拉到一半取消
    }
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    f_clk->sta = FUNC_CLOCK_SUB_SIDE;                       //进入到边菜单
}

//时钟表盘边菜单退出处理
static void func_clock_sub_side_exit(void)
{
    f_clock_t *f_clk = (f_clock_t *)func_cb.f_cb;
    compo_form_destroy(f_clk->sub_frm);
#if UTE_WATCHS_BUTTERFLY_DIAL_SUPPORT
    func_clock_butterfly_set_light_visible(true);
#endif
    f_clk->sub_frm = NULL;
}

//时钟表盘边菜单
void func_clock_sub_side(void)
{
    func_clock_sub_side_enter();
    while (func_cb.sta == FUNC_CLOCK && ((f_clock_t *)func_cb.f_cb)->sta == FUNC_CLOCK_SUB_SIDE)
    {
        func_clock_sub_side_process();
        func_clock_sub_side_message(msg_dequeue());
    }
    func_clock_sub_side_exit();
}

#endif
