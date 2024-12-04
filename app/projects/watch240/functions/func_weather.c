#include "include.h"
#include "func.h"
#include "ute_module_weather.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           UI_BUF_I330001_WEATHER_ICON_WEATHER_00_BIN  },                //未知
    {STR_SUNNY,             UI_BUF_I330001_WEATHER_ICON_WEATHER_01_BIN  },                //晴天
    {STR_CLOUDY,            UI_BUF_I330001_WEATHER_ICON_WEATHER_02_BIN  },                //多云
    {STR_OVERCAST,          UI_BUF_I330001_WEATHER_ICON_WEATHER_03_BIN  },                //阴天
    {STR_MODERATE_RAIN,     UI_BUF_I330001_WEATHER_ICON_WEATHER_04_BIN  },                //阵雨
    {STR_RAINY_SHOWERS,     UI_BUF_I330001_WEATHER_ICON_WEATHER_05_BIN  },                //雷阵雨
    {STR_SLEET,             UI_BUF_I330001_WEATHER_ICON_WEATHER_06_BIN  },                //雨夹雪
    {STR_DRIZZLE,           UI_BUF_I330001_WEATHER_ICON_WEATHER_07_BIN  },                //小雨
    {STR_HEAVY_RAIN,        UI_BUF_I330001_WEATHER_ICON_WEATHER_08_BIN  },                //大雨
    {STR_RAINY,             UI_BUF_I330001_WEATHER_ICON_WEATHER_09_BIN  },                //雪
    {STR_SAND_AND_DUST,     UI_BUF_I330001_WEATHER_ICON_WEATHER_10_BIN  },                //沙尘暴
    {STR_HAZE,              UI_BUF_I330001_WEATHER_ICON_WEATHER_11_BIN  },                //雾霾
    {STR_WINDY,             UI_BUF_I330001_WEATHER_ICON_WEATHER_12_BIN  },                //大风
    {STR_SUNNY,             UI_BUF_I330001_WEATHER_ICON_WEATHER_13_BIN  },                //明夜
    {STR_CLOUDY,            UI_BUF_I330001_WEATHER_ICON_WEATHER_14_BIN  },                //云遮月
    {STR_RAINY_SHOWERS,     UI_BUF_I330001_WEATHER_ICON_WEATHER_15_BIN  },                //阵雨夜

};

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{
    compo_textbox_t *txt;
    compo_picturebox_t * picbox;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    u8 week_sort[7];
    u8 week_day=0;
    u8 get_weather_id[7];//存放一个星期的排序
    char str_buff[30];//用于存放打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    week_day = time.week; //获取星期
    for(int i=0; i<=6; i++) //星期排序
    {
        switch(week_day)
        {
            case 0 :
                week_sort[i] = 6;
                break;
            case 1 :
                week_sort[i] = 0;
                break;
            case 2 :
                week_sort[i] = 1;
                break;
            case 3 :
                week_sort[i] = 2;
                break;
            case 4 :
                week_sort[i] = 3;
                break;
            case 5 :
                week_sort[i] = 4;
                break;
            case 6 :
                week_sort[i] = 5;
                break;
        }

        if(++week_day==7)
        {
            week_day = 0;
        }
    }

    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[0])
                {
                    case WEATHER_TYPE_SUNNY:
                        get_weather_id[i] = 13;
                        break;
                    case WEATHER_TYPE_CLOUDY:
                        get_weather_id[i] = 14;
                        break;
                    case WEATHER_TYPE_THUNDERSHOWER_RAIN:
                        get_weather_id[i] = 15;
                        break;
                }
            }
            get_weather_id[i] = weather_date.DayWeather[i]>>8;//赋值排序天气状态
        }

        if(displayInfo.isFahrenheit)    //是否为华氏度
        {
            weather_date.fristDayCurrTemperature= weather_date.fristDayCurrTemperature*9/5+32;
            /*pcm 2022-09-19 */
            if(weather_date.fristDayCurrTemperature<(-99))
            {
                weather_date.fristDayCurrTemperature=-99;
            }

            for(int i=0; i<7; i++)
            {
                weather_date.dayTemperatureMax[i]= weather_date.dayTemperatureMax[i]*9/5+32;
                weather_date.dayTemperatureMin[i]= weather_date.dayTemperatureMin[i]*9/5+32;
                if(weather_date.dayTemperatureMax[i]<(-99))  weather_date.dayTemperatureMax[i]=-99;
                if(weather_date.dayTemperatureMin[i]<(-99))  weather_date.dayTemperatureMin[i]=-99;
            }
        }
    }

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
//    compo_form_set_title(frm, i18n[STR_WEATHER]);
    char buf[50];
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_WEATHER_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.6,FORM_TITLE_HEIGHT/2);

    ///设置标题栏时间///
    memset(buf,0,sizeof(buf));
    snprintf(buf, sizeof(buf), "%d:%02d",time.hour,time.min);///* 刷新标题栏时间*/
    txt = compo_textbox_create(frm,5);
    compo_textbox_set(txt, buf);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.8,FORM_TITLE_HEIGHT/2);
    compo_setid(txt,COMPO_ID_TITLE_TIME);

    picbox = compo_picturebox_create(frm, weather_list[get_weather_id[0]].res_addr);///背景图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5);

    if(weather_date.fristDayCurrTemperature == 0)weather_date.fristDayCurrTemperature = 0;

    compo_number_t *num = compo_number_create(frm,UI_BUF_I330001_WEATHER_NUM_BIN, 3);
    compo_number_set_margin(num, -2);
    compo_number_set_radix(num, 11, true);
    compo_number_set(num, weather_date.fristDayCurrTemperature);
    compo_number_set_align(num, 1 );
    compo_number_set_pos(num, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);

    if(displayInfo.isFahrenheit)     ///是否为华氏度
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I330001_WEATHER_DF_BIN);///温度符号
    }
    else
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I330001_WEATHER_DC_BIN);///温度符号
    }

    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+compo_number_get_rel_location(num).wid/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_HIGHEST],weather_date.dayTemperatureMax[0]); ///今天 最高温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_HIGHEST]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set(txt, str_buff);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/3, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.8);

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s%d",i18n[STR_LOWSET],weather_date.dayTemperatureMin[0]); ///今天 最低温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_LOWSET]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set(txt, str_buff);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.8);

    txt = compo_textbox_create(frm, strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.25);
    compo_textbox_set_forecolor(txt, make_color(0xc2,0xe1,0xf9));

    //第二页
    for(int i=0; i<7; i++)
    {
        txt = compo_textbox_create(frm,strlen(i18n[STR_SUNDAY+week_sort[i]]));
        compo_textbox_set(txt,i18n[STR_SUNDAY+week_sort[i]]);/// 星期
        compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X/6,GUI_SCREEN_HEIGHT+30+(i*36)-8);
        compo_textbox_set_align_center(txt,false);

        picbox = compo_picturebox_create(frm,weather_list[get_weather_id[i]].res_addr);/// 天气
        compo_picturebox_set_size(picbox,gui_image_get_size( weather_list[get_weather_id[i]].res_addr).wid/3,\
                                  gui_image_get_size( weather_list[get_weather_id[i]].res_addr).hei/3);
        compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/10,GUI_SCREEN_HEIGHT+30+(i*36));

        if(uteModuleWeatherGetCurrDay() == time.day)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d~%02d℃",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), "      ---");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.2,GUI_SCREEN_HEIGHT+30+(i*36)- 10);
        compo_textbox_set_align_center(txt, false);

    }

    return frm;
}

//天气功能事件处理
static void func_weather_process(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    char buf[50];
    compo_textbox_t * title = compo_getobj_byid(COMPO_ID_TITLE_TIME);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    memset(buf,0,sizeof(buf));
    snprintf(buf, sizeof(buf), "%d:%02d",time.hour,time.min);///* 刷新标题栏时间*/
    compo_textbox_set(title, buf);

    compo_page_move_process(f_weather->ptm);
    func_process();
}

//天气功能消息处理
static void func_weather_message(size_msg_t msg)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_weather->ptm);
            break;
        case MSG_CTP_CLICK:
//            if(++(f_weather->mode_num) == WHEATHER_CNT) {
//                f_weather->mode_num = 0;
//            }
//              cardbox = compo_getobj_byid(CARBOX_ID);
//              compo_cardbox_icon_set(cardbox,0,  weather_list[f_weather->mode_num].res_addr);  //天气图标
//              compo_cardbox_text_set(cardbox, 1, i18n[weather_list[f_weather->mode_num].txt_num]);//天气文本
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入天气功能
static void func_weather_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_weather_t));
    func_cb.frm_main = func_weather_form_create();
    f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;

    f_weather->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
//        .title_used = true,
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = 100,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_weather->ptm, func_cb.frm_main->page_body, &info);

}

//退出天气功能
static void func_weather_exit(void)
{
    f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
    func_cb.last = FUNC_WEATHER;
    if (f_weather->ptm)
    {
        func_free(f_weather->ptm);
    }
}

//天气功能
void func_weather(void)
{
    printf("%s\n", __func__);
    func_weather_enter();
    while (func_cb.sta == FUNC_WEATHER)
    {
        func_weather_process();
        func_weather_message(msg_dequeue());
    }
    func_weather_exit();
}
