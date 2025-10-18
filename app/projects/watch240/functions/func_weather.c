#include "include.h"
#include "func.h"
#include "ute_module_weather.h"
#include "ute_module_systemtime.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_WEATHER_SUPPORT
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

static int16_t page_y = 0;
typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;
    bool no_weather_dada;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           0  },                //未知
    {STR_SUNNY,             1  },                //晴天
    {STR_CLOUDY,            2  },                //多云
    {STR_OVERCAST,          3  },                //阴天
    {STR_MODERATE_RAIN,     5  },                //阵雨
    {STR_RAINY_SHOWERS,     4  },                //雷阵雨
    {STR_SLEET,             6  },                //雨夹雪
    {STR_DRIZZLE,           7  },                //小雨
    {STR_HEAVY_RAIN,        8  },                //大雨
    {STR_SNOWY,             9  },                //雪
    {STR_SAND_AND_DUST,     10  },                //沙尘暴
    {STR_HAZE,              11  },                //雾霾
    {STR_WINDY,             12  },                //大风
    {STR_SUNNY,             13  },                //明夜
    {STR_CLOUDY,            14  },                //云遮月
    {STR_MODERATE_RAIN,     15  },                //阵雨夜
};
static const u16 weather_uv[5] =
{
    STR_UV_VERY_WEAK, // 很弱
    STR_UV_WEAK, // 弱
    STR_UV_MODERATE, // 中等
    STR_UV_STRONG, // 强
    STR_UV_VERY_STRONG, // 很强
};
static const u32 future_weather_icon[16] =
{
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_00_UNKNOWEN_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_01_SUNNY_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_02_CLOUDY_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_03_OVERCAST_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_04_SHOWER_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_05_T_STORM_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_06_SLEET_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_07_LIGHT_RAIN_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_08_HEAVY_RAIN_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_09_SNOW_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_10_SAND_STORM_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_11_HAZE_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_12_WINDY_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_13_NIGHT_SUNNY_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_14_NIGHT_CLOUDY_BIN,
    UI_BUF_I335001_WEATHER_ICON_WEATHER_40X40_X18_X86_X154_X222_Y108_15_NIGHT_RAIN_BIN,
};
static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t *weather_date = (ute_module_weather_data_t *)ab_zalloc(sizeof(ute_module_weather_data_t));
    // ute_module_weather_data_t  weather_date;
    uteModuleWeatherGetData(weather_date);//获取天气状态
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        // printf(" f_weather->DayWeather[%d] = weather_date.DayWeather[%d]\n", f_weather->DayWeather[i] ,weather_date->DayWeather[i]);
        if(f_weather->DayWeather[i] != weather_date->DayWeather[i])
        {
            f_weather->DayWeather[i] = weather_date->DayWeather[i];
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        // printf("refresh->isFahrenheit:%d,%d weather_flag:%d\n",displayInfo.isFahrenheit,f_weather->isFahrenheit_flag,weather_flag);
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
    ab_free(weather_date);
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    uteModuleWeatherGetData(&weather_date);//获取天气状态
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
    // printf("create->isFahrenheit:%d,%d\n",displayInfo.isFahrenheit,f_weather->isFahrenheit_flag);
}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{
    compo_textbox_t *txt;
    compo_picturebox_t * picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    const u8 week_sort[7]= {STR_SUNDAY, // 周日
                            STR_MONDAY, // 周一
                            STR_TUESDAY, // 周二
                            STR_WEDNESDAY, // 周三
                            STR_THURSDAY, // 周四
                            STR_FRIDAY, // 周五
                            STR_SATURDAY, // 周六
                           };
    // u8 week_day=0;
    u8 get_weather_id[7];//存放一个星期的排序
    char str_buff[50];//用于存放打印数据
    char str_humidity_buff[8];//用于存放湿度打印数据
    memset(get_weather_id,0,sizeof(get_weather_id));
    uteModuleSystemtimeGetTime(&time);//获取系统时间
//    printf("week:%d\n",time.week);
//     week_day = time.week; //获取星期
//     for(int i=0; i<=6; i++) //星期排序
//     {
//         week_sort[i] = week_day;
//         if(++week_day==7)
//         {
//             week_day = 0;
//         }
//     }
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态
        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }

        for(int i=0; i<7; i++) //获取一周的天气
        {
            get_weather_id[i] = weather_date.DayWeather[i]>>8;//赋值排序天气状态
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
                {
                    case WEATHER_TYPE_SUNNY:
                        get_weather_id[i] = 13;
                        break;
                    case WEATHER_TYPE_CLOUDY:
                        get_weather_id[i] = 14;
                        break;
                    case WEATHER_TYPE_SHOWER_RAIN:
                    case WEATHER_TYPE_THUNDERSHOWER_RAIN:
                    case WEATHER_TYPE_RAIN_SNOW:         //雨夹雪
                    case WEATHER_TYPE_LIGHT_RAIN:        //小雨
                    case WEATHER_TYPE_HEAVY_RAIN:        //大雨
                        get_weather_id[i] = 15;
                        break;
                }
            }
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

    if(weather_no_data_flag)
    {
        if(func_cb.sta == FUNC_WEATHER)
        {
            f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
            f_weather->no_weather_dada = true;
        }
        ///设置标题栏名字///
        txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
//        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_align_center(txt, false);

        picbox = compo_picturebox_create(frm,UI_BUF_I335001_1_WEATHER_ICON_WEATHER_96X70_X72_Y75_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,84+50/2);

        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,160+56/2, GUI_SCREEN_WIDTH/1.1,widget_text_get_max_height()*2);
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set_multiline_drag(txt, false);      //避免既有滚动又有省略号的情况
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);
        return frm;
    }
#if GUI_SCREEN_SIZE_240X296RGB_I335002_SUPPORT
    picbox = compo_picturebox_create(frm,UI_BUF_I335002_WEATHER_240X296_BG_BIN);///背景图片
#elif GUI_SCREEN_SIZE_240X296RGB_I335004_SUPPORT
    picbox = compo_picturebox_create(frm,UI_BUF_I335004_WEATHER_BG_BIN);///背景图片
#else
    picbox = compo_picturebox_create(frm,UI_BUF_I335001_WEATHER_BG_BIN);///背景图片
#endif
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    picbox = compo_picturebox_create(frm,UI_BUF_I335001_WEATHER_ICON_WEATHER_BIN);///
    compo_picturebox_set_pos(picbox, 40+14,40+72);
    compo_picturebox_cut(picbox, weather_list[get_weather_id[0]].res_addr,16 );
    memset(str_buff,0,sizeof(str_buff));
    if(weather_no_data_flag)
    {
        snprintf(str_buff,sizeof(str_buff),"--");
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"%d",weather_date.fristDayCurrTemperature);
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(txt,76/2+120,52/2+63);
    compo_textbox_set(txt, str_buff);
    picbox = compo_picturebox_create(frm,UI_BUF_I335001_WEATHER_DU_BIN);
    compo_picturebox_set_pos(picbox,207,70);
    memset(str_buff,0,sizeof(str_buff));
    if(!weather_no_data_flag)
    {
        if(!displayInfo.isFahrenheit)      ///是否为华氏度
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℃",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
        else
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℉",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"--/--℃");
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_pos(txt,77/2+120,120+9);
    compo_textbox_set(txt, str_buff);
    txt = compo_textbox_create(frm,strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set_location(txt,77/2+120,144+9,100,widget_text_get_max_height());
    compo_textbox_set_forecolor(txt, make_color(0x98,0xd0,0xff));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    //紫外线icon
    picbox = compo_picturebox_create(frm,UI_BUF_I335001_WEATHER_UV_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 13+51,13+188);
    //紫外线data
    txt = compo_textbox_create(frm,strlen(i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]));
    // compo_textbox_set_pos(txt,64,229);
    compo_textbox_set_location(txt,64,229,65,40);//部分语言显示超出屏幕，改为限制文本框大小
    compo_textbox_set(txt,i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]);
    //紫外线txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_UV]));
    compo_textbox_set_location(txt,64,243+8,65,40);
    compo_textbox_set(txt,i18n[STR_UV]);
    //湿度icon
    picbox = compo_picturebox_create(frm,UI_BUF_I335001_WEATHER_HUMIDITY_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 152+42/2,13+188);
    //湿度data
    memset(str_humidity_buff,0,sizeof(str_humidity_buff));
    snprintf(str_humidity_buff,sizeof(str_humidity_buff),"%d%%",weather_date.fristDayWeatherHumidity);
    txt = compo_textbox_create(frm,strlen(str_humidity_buff));
    compo_textbox_set_pos(txt,152+42/2,229);
    compo_textbox_set(txt,str_humidity_buff);
    //湿度txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_HUMIDITY]));
    compo_textbox_set_location(txt,152+42/2,245+8,60,40);
    compo_textbox_set(txt,i18n[STR_HUMIDITY]);
    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_align_center(txt, false);

    //未来天气txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_FUTURE_WEATHER]));
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
    compo_textbox_set(txt,i18n[STR_FUTURE_WEATHER]);
    compo_textbox_set_align_center(txt, false);
    //第二页
    for(int i=0; i<3; i++)
    {
        //bg pic
        picbox = compo_picturebox_create(frm,UI_BUF_I335001_WEATHER_ICON_BG_X60_Y186_X8_Y61_X76_Y61_X144_Y61_X212_Y61_01_BIN);
        compo_picturebox_set_pos(picbox,8+30+(i)*(82),GUI_SCREEN_HEIGHT+61+93);

        u16 str_id = week_sort[(time.week+i)%7];
        if(i==0)
        {
            str_id = STR_TO_DAY;
        }
        //星期 txt
        txt = compo_textbox_create(frm,strlen(i18n[str_id]));
        compo_textbox_set_location(txt, 8+30+(i)*(82),GUI_SCREEN_HEIGHT+82,45,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,true);
        compo_textbox_set(txt,i18n[str_id]);/// 星期

        //天气 icon
        //图切的不对，80x80了，
        picbox = compo_picturebox_create(frm,future_weather_icon[get_weather_id[i]]);
        compo_picturebox_set_pos(picbox,8+30+(i)*(82),GUI_SCREEN_HEIGHT+108+30);
        compo_picturebox_set_size(picbox,40,40);
        //温度调整为上方是低温，下方是高温，与app保持一致
        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d°",weather_date.dayTemperatureMin[i]);//一周 小~大 温度
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --°");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set_location(txt, 8+30+(i)*(82),GUI_SCREEN_HEIGHT+167+12,40,30);
        // compo_textbox_set_pos(txt, 16+(i-1)*(82),GUI_SCREEN_HEIGHT+167);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set(txt,str_buff);
        // compo_textbox_set_autoroll_mode(txt, true); //开启自动滚动
        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d°",weather_date.dayTemperatureMax[i]);//一周 小~大 温度
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --°");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set_location(txt, 8+30+(i)*(82),GUI_SCREEN_HEIGHT+202+12,40,30);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set(txt,str_buff);
        // compo_textbox_set_autoroll_mode(txt, true); //开启自动滚动
    }
    widget_page_set_client(frm->page_body, 0, page_y);

    return frm;
}


#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;
    bool no_weather_dada;
    u32 tick;
    bool touch_flag;
    //bool exit_flag;
    uint16_t pageHeight;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

typedef struct
{
    u8 hour;
    u8 am_pm;
} func_12hour_convert_t;

static func_12hour_convert_t func_convert_to_12hour(u8 time)
{
    u8 am_pm = (time >= 12) ? 2 : 1;    //2 PM, 1 AM
    func_12hour_convert_t hour12;
    if(uteModuleSystemtime12HOn())
    {
        if (time == 0)
        {
            hour12.hour = 12;
        }
        else if (time > 12)
        {
            hour12.hour = time - 12;
        }
        else
        {
            hour12.hour = time;
        }
        hour12.am_pm = am_pm;
        return hour12;
    }
    hour12.hour = time;
    hour12.am_pm = 0;
    return hour12;
}

static const u8 weather_list[] =
{
    STR_UNKNOWN,       // 未知
    STR_SUNNY,         // 晴天
    STR_CLOUDY,        // 阴天(改多云)
    STR_SUN_CLOUDY,    // 晴间多云
    STR_MODERATE_RAIN, // 阵雨
    STR_RAINY_SHOWERS, // 雷阵雨
    STR_SLEET,         // 雨夹雪
    STR_DRIZZLE,       // 小雨
    STR_HEAVY_RAIN,    // 大雨
    STR_SNOW_WEAK,     // 雪
    STR_SAND_AND_DUST, // 沙尘暴
    STR_HAZE,          // 雾霾
    STR_WINDY,         // 大风
    STR_SNOW_WEAK,     // 小雪
    STR_SNOW_HAV,      // 大雪
    STR_SUNNY,         // 夜间晴天
    STR_SUN_CLOUDY,    // 夜间晴间多云
    STR_MODERATE_RAIN, // 夜间雨
};
static const u16 weather_uv[5] =
{
    STR_UV_VERY_WEAK, // 很弱
    STR_UV_WEAK, // 弱
    STR_UV_MODERATE, // 中等
    STR_UV_STRONG, // 强
    STR_UV_VERY_STRONG, // 很强
};
static const u32 future_weather_icon[] =
{
    UI_BUF_I341001_12_WEATHER_WEATHER_00_UNKNOWEN_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_01_SUNNY_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_03_OVERCAST_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_02_CLOUDY_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_04_SHOWER_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_05_T_STORM_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_06_SLEET_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_07_LIGHT_RAIN_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_08_HEAVY_RAIN_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_09_SNOW_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_10_SAND_STORM_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_11_HAZE_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_12_WINDY_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_13_MINOR_SNOW_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_14_HEAVY_SNOW_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_15_NIGHT_SUNNY_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_16_NIGHT_CLOUDY_BIN,
    UI_BUF_I341001_12_WEATHER_WEATHER_17_NIGHT_RAIN_BIN
};
static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t *weather_date = (ute_module_weather_data_t *)ab_zalloc(sizeof(ute_module_weather_data_t));
    // ute_module_weather_data_t  weather_date;
    uteModuleWeatherGetData(weather_date);//获取天气状态
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        // printf(" f_weather->DayWeather[%d] = weather_date.DayWeather[%d]\n", f_weather->DayWeather[i] ,weather_date->DayWeather[i]);
        if(f_weather->DayWeather[i] != weather_date->DayWeather[i])
        {
            f_weather->DayWeather[i] = weather_date->DayWeather[i];
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        // printf("displayInfo.isFahrenheit:%d f_weather->isFahrenheit_flag:%d weather_flag:%d\n",displayInfo.isFahrenheit,f_weather->isFahrenheit_flag,weather_flag);
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
    ab_free(weather_date);
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{
    compo_textbox_t *txt;
    compo_picturebox_t *picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;
    u32 weather_bg = 0;

    const u8 week_sort[7]= {STR_SUNDAY,     // 周日
                            STR_MONDAY,     // 周一
                            STR_TUESDAY,    // 周二
                            STR_WEDNESDAY,  // 周三
                            STR_THURSDAY,   // 周四
                            STR_FRIDAY,     // 周五
                            STR_SATURDAY,   // 周六
                           };
    u8 get_weather_id[7];               //存放一个星期的排序
    char str_buff[20];                  //用于存放打印数据
    char time_buff[20] = {0};           //用于存放标题时间
    char str_humidity_buff[8];          //用于存放湿度打印数据
    uint16_t pageHei = 0;
    //城市
    uint8_t cityName[UTE_WEATHER_CITY_NAME_TITLE_MAX_SIZE] = {0};
    uint16_t cityLen = 0;

    memset(get_weather_id,0,sizeof(get_weather_id));
    uteModuleSystemtimeGetTime(&time);  //获取系统时间

    if(uteModuleWeatherGetCurrDay() == time.day)        //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo); //获取温度
        uteModuleWeatherGetData(&weather_date);         //获取天气状态
        uteModuleWeatherGetCityName(cityName, &cityLen);
        for(int i=0; i<7; i++)                          //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }

        //背景选择
        u8 get_weather_typeBg = weather_date.DayWeather[0] >> 8;
        if ((get_weather_typeBg > WEATHER_TYPE_OVERCAST && get_weather_typeBg < WEATHER_TYPE_SNOW) || get_weather_typeBg == WEATHER_TYPE_CLOUDY)
        {
            weather_bg = UI_BUF_I341001_12_WEATHER_BG_BG_CLOUDY_SHOWERS_BIN;
        }
        else if (get_weather_typeBg > WEATHER_TYPE_HEAVY_RAIN && get_weather_typeBg < WEATHER_TYPE_NIGHT)
        {
            weather_bg = UI_BUF_I341001_12_WEATHER_BG_BG_EXTREME_WEATHER_BIN;
        }
        else
        {
            weather_bg = UI_BUF_I341001_12_WEATHER_BG_BG_SUNNY_BIN;
        }

        for(int i=0; i<7; i++) //获取一周的天气
        {
            get_weather_id[i] = weather_date.DayWeather[i]>>8;//赋值排序天气状态
            if (get_weather_id[i] >= WEATHER_TYPE_NIGHT)
            {
                get_weather_id[i] = WEATHER_TYPE_UNKNOWN;
            }
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch (get_weather_id[i])
                {
                    case WEATHER_TYPE_SUNNY:
                        get_weather_id[i] = WEATHER_TYPE_NIGHT;
                        break;
                    case WEATHER_TYPE_OVERCAST:
                        get_weather_id[i] = WEATHER_TYPE_NIGHT + 1;
                        break;
                    case WEATHER_TYPE_SHOWER_RAIN:
                    case WEATHER_TYPE_THUNDERSHOWER_RAIN:
                    case WEATHER_TYPE_RAIN_SNOW:  // 雨夹雪
                    case WEATHER_TYPE_LIGHT_RAIN: // 小雨
                    case WEATHER_TYPE_HEAVY_RAIN: // 大雨
                        get_weather_id[i] = WEATHER_TYPE_NIGHT + 2;
                        break;
                }
            }
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

    func_12hour_convert_t timeHour = func_convert_to_12hour(time.hour);
    if (timeHour.am_pm > 0)
    {
        snprintf(time_buff, sizeof(time_buff), "%s%02d:%02d", timeHour.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM], timeHour.hour, time.min);
    }
    else
    {
        snprintf(time_buff, sizeof(time_buff), "%02d:%02d", timeHour.hour, time.min);
    }

    if(weather_no_data_flag)
    {
        if(func_cb.sta == FUNC_WEATHER)
        {
            f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
            f_weather->no_weather_dada = true;
        }
        ///设置标题栏名字///
        txt = compo_textbox_create(frm, strlen(i18n[STR_WEATHER]));
        compo_textbox_set_location(txt, FORM_TITLE_LEFT, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 4 - FORM_TITLE_LEFT, 50);
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_align_center(txt, false);
        txt = compo_textbox_create(frm, 20);
        compo_textbox_set_location(txt, FORM_TITLE_RIGHT + 20, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12, FORM_TITLE_HEIGHT);
        compo_textbox_set_align_center(txt, false);
        compo_textbox_set_multiline(txt, false);
        compo_textbox_set(txt, time_buff);

        picbox = compo_picturebox_create(frm, UI_BUF_I341001_12_WEATHER_ICON_NODATE_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 117+36);

        txt = compo_textbox_create(frm, strlen(i18n[STR_TIP2]));
        compo_textbox_set_location(txt, 24, 248, 320, 200);
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_align_center(txt, false);
        compo_textbox_set_multiline_drag(txt, false);      //避免既有滚动又有省略号的情况
        compo_textbox_set(txt, i18n[STR_TIP2]);
        return frm;
    }

    ///背景图片
    picbox = compo_picturebox_create(frm, weather_bg);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 1030 / 2);
    compo_picturebox_set_size(picbox, 368, 1030);
//    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
//    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + GUI_SCREEN_HEIGHT - 24, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT + 60);
//    compo_shape_set_color(shape, make_color(84, 110, 130));

    picbox = compo_picturebox_create(frm, future_weather_icon[get_weather_id[0]]);
    compo_picturebox_set_pos(picbox, 22+61, 98+61);

    memset(str_buff,0,sizeof(str_buff));
    if(weather_no_data_flag)
    {
        snprintf(str_buff,sizeof(str_buff),"--");
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"%d",weather_date.fristDayCurrTemperature);
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, 200, 71 + 20);
    compo_textbox_set(txt, str_buff);
    picbox = compo_picturebox_create(frm, displayInfo.isFahrenheit ? UI_BUF_I341001_12_WEATHER_01_BIN : UI_BUF_I341001_12_WEATHER_00_BIN);
    compo_picturebox_set_pos(picbox, 200+widget_text_get_area(txt->txt).wid+22, 113);

    memset(str_buff,0,sizeof(str_buff));
    if(!weather_no_data_flag)
    {
        if(!displayInfo.isFahrenheit)      ///是否为华氏度
        {
            snprintf(str_buff, sizeof(str_buff), "%d/%d℃", weather_date.dayTemperatureMin[0], weather_date.dayTemperatureMax[0]);
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), "%d/%d℉", weather_date.dayTemperatureMin[0], weather_date.dayTemperatureMax[0]);
        }
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "--/--");
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_pos(txt, 200, 160);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm, strlen(i18n[weather_list[get_weather_id[0]]]));
    compo_textbox_set_location(txt, 200, 202, 150, 48);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]]]);

    //城市名
    if (cityLen > 0)
    {
        txt = compo_textbox_create(frm, cityLen);
        compo_textbox_set_location(txt, 200, 56, 144, 50);
        compo_textbox_set_align_center(txt, false);
        compo_textbox_set(txt, (char *)cityName);
    }

    //更新时间
    memset(str_buff, 0, sizeof(str_buff));
    if (uteModuleWeatherGetCurrDay() == time.day)
    {
        if (weather_date.updateTime.day == time.day)
        {
            func_12hour_convert_t hours = func_convert_to_12hour(weather_date.updateTime.hour);
            if (hours.am_pm > 0)
            {
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d %s", hours.hour, weather_date.updateTime.min, hours.am_pm == 2 ? i18n[STR_PM] : i18n[STR_AM]);
            }
            else
            {
                snprintf(str_buff, sizeof(str_buff), "%02d:%02d", hours.hour, weather_date.updateTime.min);
            }
        }
        else
        {
            if (weather_date.updateTime.year == time.year)
            {
                snprintf(str_buff, sizeof(str_buff), "%d/%d", weather_date.updateTime.month, weather_date.updateTime.day);
            }
            else
            {
                snprintf(str_buff, sizeof(str_buff), "%d/%d/%d", weather_date.updateTime.year, weather_date.updateTime.month, weather_date.updateTime.day);
            }
        }
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "--:--");
    }
    txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(txt, 0, 255, GUI_SCREEN_WIDTH-19, 40);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_right_align(txt, true);
    compo_textbox_set(txt, str_buff);
    txt = compo_textbox_create(frm, strlen(i18n[STR_LAST_UPDATED]));
    compo_textbox_set_location(txt, 19, 255, timeHour.am_pm > 0 ? 188 : 230, 40);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, i18n[STR_LAST_UPDATED]);

    //紫外线data txt
    txt = compo_textbox_create(frm, strlen(i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]));
    compo_textbox_set_location(txt, 217, 308, GUI_SCREEN_WIDTH-19-217, 40);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_right_align(txt, true);
    compo_textbox_set(txt, i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]);
    if (widget_text_get_area(txt->txt).wid > GUI_SCREEN_WIDTH-19-217)
    {
        compo_textbox_set_right_align(txt, false);
        compo_textbox_set(txt, i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]);
    }
    txt = compo_textbox_create(frm, strlen(i18n[STR_UV]));
    compo_textbox_set_location(txt, 19, 308, 188, 40);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, i18n[STR_UV]);

    //湿度data txt
    memset(str_humidity_buff,0,sizeof(str_humidity_buff));
    snprintf(str_humidity_buff,sizeof(str_humidity_buff),"%d%%",weather_date.fristDayWeatherHumidity);
    txt = compo_textbox_create(frm,strlen(str_humidity_buff));
    compo_textbox_set_location(txt, 0, 361, GUI_SCREEN_WIDTH-19, 40);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_right_align(txt, true);
    compo_textbox_set(txt, str_humidity_buff);
    txt = compo_textbox_create(frm, strlen(i18n[STR_HUMIDITY]));
    compo_textbox_set_location(txt, 19, 361, 230, 40);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, i18n[STR_HUMIDITY]);

    ///设置标题栏名字///
    txt = compo_textbox_create(frm, strlen(i18n[STR_WEATHER]));
    compo_textbox_set_location(txt, FORM_TITLE_LEFT, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 4 - FORM_TITLE_LEFT, 50);
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_align_center(txt, false);
    txt = compo_textbox_create(frm, 20);
    compo_textbox_set_location(txt, FORM_TITLE_RIGHT + 20, GUI_SCREEN_HEIGHT / 8 - FORM_TITLE_HEIGHT, GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12, FORM_TITLE_HEIGHT);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_multiline(txt, false);
    compo_textbox_set(txt, time_buff);

    //第二页
    // 未来天气txt
    txt = compo_textbox_create(frm, strlen(i18n[STR_FUTURE_WEATHER]));
    compo_textbox_set_location(txt, GUI_SCREEN_WIDTH/12, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/21.8, 260, 48);
    compo_textbox_set(txt, i18n[STR_FUTURE_WEATHER]);
    compo_textbox_set_align_center(txt, false);

    pageHei = 536;
    for(int i = 1; i < 7; i++)
    {
        memset(str_buff, 0, sizeof(str_buff));
        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            if(!displayInfo.isFahrenheit)      ///是否为华氏度
            {
                snprintf(str_buff, sizeof(str_buff), "%d/%d℃", weather_date.dayTemperatureMin[i], weather_date.dayTemperatureMax[i]);
            }
            else
            {
                snprintf(str_buff, sizeof(str_buff), "%d/%d℉", weather_date.dayTemperatureMin[i], weather_date.dayTemperatureMax[i]);
            }
        }
        else
        {
            //snprintf(str_buff, sizeof(str_buff), "--/--");
            break;
        }

        //星期 txt
        txt = compo_textbox_create(frm, 20);
        compo_textbox_set_location(txt, 19, pageHei, 132, 48);
        compo_textbox_set_align_center(txt, false);
        compo_textbox_set(txt, i18n[week_sort[(time.week+i)%7]]);/// 星期

        //天气 icon
        picbox = compo_picturebox_create(frm, future_weather_icon[get_weather_id[i]]);
        compo_picturebox_set_pos(picbox, 150+28, pageHei + 18);
        compo_picturebox_set_size(picbox, 46, 46);

        txt = compo_textbox_create(frm, strlen(str_buff));
        compo_textbox_set_location(txt, 0, pageHei, GUI_SCREEN_WIDTH-19, 40);
        compo_textbox_set_align_center(txt, false);
        compo_textbox_set_right_align(txt, true);
        compo_textbox_set(txt, str_buff);

        pageHei += 68;
    }

    //更新提醒
    txt = compo_textbox_create(frm, strlen(i18n[STR_PLEASE_APP_DATA]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, pageHei, 320, widget_text_get_max_height());
    compo_textbox_set(txt, i18n[STR_PLEASE_APP_DATA]);
    compo_textbox_set_visible(txt, (pageHei-536)/68 < 5 ? true : false);

    //服务商logo
    pageHei = (pageHei-536)/68 < 5 ? pageHei + 52 : pageHei + 22;//(weather_date.updateTime.day + 1) < time.day ?
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_12_WEATHER_ICON_LOGO01_BIN);
    compo_picturebox_set_pos(picbox, 282+24, pageHei);

    compo_scroll_t *scroll = compo_scroll_create(frm, SCROLL_TYPE_VERTICAL);
    compo_setid(scroll, CARBOX_ID);
    compo_scroll_set_w_r(scroll, 10);
    compo_scroll_set_range(scroll, 96);
    compo_scroll_set_pos(scroll, 356, 78);
    compo_scroll_set_color(scroll, COLOR_WHITE, 0xFEFE);//make_color(51, 51, 51));
    compo_scroll_set_value(scroll, 0);
    compo_scroll_set_edge_circle(scroll, true);
    widget_set_alpha(scroll->scroll_bg, 64);

    if(func_cb.sta == FUNC_WEATHER)
    {
        f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
        f_weather->pageHeight = pageHei + 60;
    }

    return frm;
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

static int16_t page_y = 0;
typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;
    bool no_weather_dada;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           0  },                //未知
    {STR_SUNNY,             1  },                //晴天
    {STR_CLOUDY,            2  },                //多云
    {STR_OVERCAST,          3  },                //阴天
    {STR_MODERATE_RAIN,     4  },                //阵雨
    {STR_RAINY_SHOWERS,     5  },                //雷阵雨
    {STR_SLEET,             6  },                //雨夹雪
    {STR_DRIZZLE,           7  },                //小雨
    {STR_HEAVY_RAIN,        8  },                //大雨
    {STR_RAINY,             9  },                //雪
    {STR_SAND_AND_DUST,     10  },                //沙尘暴
    {STR_HAZE,              11  },                //雾霾
    {STR_WINDY,             12  },                //大风
    {STR_SUNNY,             13  },                //明夜
    {STR_CLOUDY,            14  },                //云遮月
    {STR_RAINY_SHOWERS,     15  },                //阵雨夜
    {STR_RAINY_SHOWERS,     16  },                //阵雨夜
};
static const u16 weather_uv[5] =
{
    STR_UV_VERY_WEAK, // 很弱
    STR_UV_WEAK, // 弱
    STR_UV_MODERATE, // 中等
    STR_UV_STRONG, // 强
    STR_UV_VERY_STRONG, // 很强
};
static const u32 future_weather_icon[16] =
{
    UI_BUF_I343001_12_WEATHER_WEATHER_00_UNKNOWEN_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_01_SUNNY_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_02_CLOUDY_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_03_OVERCAST_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_04_SHOWER_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_05_T_STORM_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_06_SLEET_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_07_LIGHT_RAIN_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_08_HEAVY_RAIN_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_09_SNOW_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_10_SAND_STORM_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_11_HAZE_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_12_WINDY_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_13_NIGHT_SUNNY_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_14_NIGHT_CLOUDY_BIN,
    UI_BUF_I343001_12_WEATHER_WEATHER_15_NIGHT_RAIN_BIN,
};
static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t *weather_date = (ute_module_weather_data_t *)ab_zalloc(sizeof(ute_module_weather_data_t));
    // ute_module_weather_data_t  weather_date;
    uteModuleWeatherGetData(weather_date);//获取天气状态
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        // printf(" f_weather->DayWeather[%d] = weather_date.DayWeather[%d]\n", f_weather->DayWeather[i] ,weather_date->DayWeather[i]);
        if(f_weather->DayWeather[i] != weather_date->DayWeather[i])
        {
            f_weather->DayWeather[i] = weather_date->DayWeather[i];
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        // printf("displayInfo.isFahrenheit:%d f_weather->isFahrenheit_flag:%d weather_flag:%d\n",displayInfo.isFahrenheit,f_weather->isFahrenheit_flag,weather_flag);
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
    ab_free(weather_date);
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{
    compo_textbox_t *txt;
    compo_picturebox_t * picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    const u8 week_sort[7]= {STR_SUNDAY,     // 周日
                            STR_MONDAY,     // 周一
                            STR_TUESDAY,    // 周二
                            STR_WEDNESDAY,  // 周三
                            STR_THURSDAY,   // 周四
                            STR_FRIDAY,     // 周五
                            STR_SATURDAY,   // 周六
                           };
    u8 get_weather_id[7];               //存放一个星期的排序
    char str_buff[50];                  //用于存放打印数据
    char str_humidity_buff[8];          //用于存放湿度打印数据
    memset(get_weather_id,0,sizeof(get_weather_id));
    uteModuleSystemtimeGetTime(&time);  //获取系统时间

    if(uteModuleWeatherGetCurrDay() == time.day)        //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo); //获取温度
        uteModuleWeatherGetData(&weather_date);         //获取天气状态
        for(int i=0; i<7; i++)                          //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
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

    if(weather_no_data_flag)
    {
        if(func_cb.sta == FUNC_WEATHER)
        {
            f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
            f_weather->no_weather_dada = true;
        }
        ///设置标题栏名字///
        txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
//        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_align_center(txt, false);

        picbox = compo_picturebox_create(frm,UI_BUF_I343001_12_WEATHER_CLOUD_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,105+90/2);

        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,238+28/2, GUI_SCREEN_WIDTH/1.1,widget_text_get_max_height()*2);
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set_multiline_drag(txt, false);      //避免既有滚动又有省略号的情况
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);
        return frm;
    }

//    picbox = compo_picturebox_create(frm,UI_BUF_I343001_12_WEATHER_BG_BIN);///背景图片
//    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);

    picbox = compo_picturebox_create(frm,future_weather_icon[weather_list[get_weather_id[0]].res_addr]);///
    compo_picturebox_set_pos(picbox, 34+84/2,115+95/2);

    picbox = compo_picturebox_create(frm,UI_BUF_I343001_12_WEATHER_WEATHERSYM_BIN);
    if(weather_no_data_flag)
    {
        compo_picturebox_set_pos(picbox,269+16, 103+24);

        picbox = compo_picturebox_create(frm, UI_BUF_I343001_12_WEATHER_102_56X76_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11);
        compo_picturebox_set_pos(picbox, 162+38,103+38);

        picbox = compo_picturebox_create(frm, UI_BUF_I343001_12_WEATHER_102_56X76_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11);
        compo_picturebox_set_pos(picbox, 162+84, 103+38);
    }
    else
    {
        compo_number_t *num = compo_number_create(frm,UI_BUF_I343001_12_WEATHER_102_56X76_BIN, 3);
        compo_number_set_margin(num, 5);
        compo_number_set_radix(num, 11, true);
        compo_number_set(num, weather_date.fristDayCurrTemperature);
        compo_number_set_align(num, 1);
        compo_number_set_pos(num, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3-8, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/4);
        compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X+compo_number_get_rel_location(num).wid/2+GUI_SCREEN_CENTER_X/3+10, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/3-6);
    }

    memset(str_buff,0,sizeof(str_buff));
    if(!weather_no_data_flag)
    {
        if(!displayInfo.isFahrenheit)      ///是否为华氏度
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℃",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
        else
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℉",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"--/--℃");
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_pos(txt,110/2+182,35/2+200);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm,strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set_pos(txt,24,60);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_forecolor(txt, make_color(0x98,0xd0,0xff));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    //紫外线icon
    picbox = compo_picturebox_create(frm,UI_BUF_I343001_12_WEATHER_UV_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 61+20,253+20);
    //紫外线data
    txt = compo_textbox_create(frm,strlen(i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]));
    compo_textbox_set_pos(txt,26+132/2,293+40/2);
    compo_textbox_set(txt,i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]);
    //紫外线txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_UV]));
    compo_textbox_set_pos(txt,54+30,337+14);
    compo_textbox_set(txt,i18n[STR_UV]);
    //湿度icon
    picbox = compo_picturebox_create(frm,UI_BUF_I343001_12_WEATHER_HUMIDITY_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 221+14,254+20);
    //湿度data
    memset(str_humidity_buff,0,sizeof(str_humidity_buff));
    snprintf(str_humidity_buff,sizeof(str_humidity_buff),"%d%%",weather_date.fristDayWeatherHumidity);
    txt = compo_textbox_create(frm,strlen(str_humidity_buff));
    compo_textbox_set_pos(txt,214+64/2,297+20);
    compo_textbox_set(txt,str_humidity_buff);
    //湿度txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_HUMIDITY]));
    compo_textbox_set_pos(txt,221+40/2,337+14);
    compo_textbox_set(txt,i18n[STR_HUMIDITY]);
    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,140,GUI_SCREEN_HEIGHT/(284/28));
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_align_center(txt, false);

    //未来天气txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_FUTURE_WEATHER]));
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/21.8,260,GUI_SCREEN_HEIGHT/(284/28));
    compo_textbox_set(txt,i18n[STR_FUTURE_WEATHER]);
    compo_textbox_set_align_center(txt, false);
    //第二页
    for(int i=0; i<3; i++)
    {
        //bg pic
        picbox = compo_picturebox_create(frm,UI_BUF_I343001_12_WEATHER_BG2_BIN);
        compo_picturebox_set_pos(picbox,10+80/2+(i)*(110),GUI_SCREEN_HEIGHT+81+248/2);

        //星期 txt
        txt = compo_textbox_create(frm,20);
        compo_textbox_set_location(txt, 20+68/2+(i)*(110),GUI_SCREEN_HEIGHT+101+42/2,68,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,true);
        compo_textbox_set(txt,i18n[week_sort[(time.week+i)%7]]);/// 星期
        if(i==1)
        {
            compo_textbox_set(txt,i18n[STR_TO_DAY]);/// 星期
        }

        //天气 icon
        //图切的不对，80x80了，
        picbox = compo_picturebox_create(frm,future_weather_icon[get_weather_id[i]]);
        compo_picturebox_set_pos(picbox,22+56/2+(i)*(110),GUI_SCREEN_HEIGHT+157+56/2);
        compo_picturebox_set_size(picbox,56,56);
        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d°",weather_date.dayTemperatureMax[i]);//一周 小~大 温度
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --°");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_location(txt, 29+60/2+(i)*(110),GUI_SCREEN_HEIGHT+216+30/2,70,30);
        // compo_textbox_set_pos(txt, 16+(i-1)*(82),GUI_SCREEN_HEIGHT+167);
        compo_textbox_set_align_center(txt, true);
        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d°",weather_date.dayTemperatureMin[i]);//一周 小~大 温度
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --°");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_location(txt, 29+60/2+(i)*(110),GUI_SCREEN_HEIGHT+265+30/2,70,30);
        compo_textbox_set_align_center(txt, true);
    }
    widget_page_set_client(frm->page_body, 0, page_y);

    return frm;
}


#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;
    bool no_weather_dada;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
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

static int16_t page_y = 0;

static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t  weather_date;
    uteModuleWeatherGetData(&weather_date);//获取天气状态
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        if(f_weather->DayWeather[i] != weather_date.DayWeather[i])
        {
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{
    compo_textbox_t *txt;
    compo_picturebox_t * picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    u8 week_sort[7];
    u8 week_day=0;
    u8 get_weather_id[7];//存放一个星期的排序
    char str_buff[30];//用于存放打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));

    uteModuleSystemtimeGetTime(&time);//获取系统时间
//    printf("week:%d\n",time.week);
    week_day = time.week; //获取星期
    for(int i=0; i<=6; i++) //星期排序
    {
        week_sort[i] = week_day;
        if(++week_day==7)
        {
            week_day = 0;
        }
    }

    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态
//        printf("year:%d mon:%d day:%d hour:%d min:%d sec:%d\n",weather_date.updateTime.year,weather_date.updateTime.month,weather_date.updateTime.day,weather_date.updateTime.hour,weather_date.updateTime.min,weather_date.updateTime.sec);

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }
//        int pic_num=3;
        for(int i=0; i<7; i++) //获取一周的天气
        {
//            printf("weather:%d\n",weather_date.DayWeather[i]>>8);
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
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

                // if(weather_date.dayTemperatureMax[i]<0){
                //     weather_date.dayTemperatureMax[i]-=0.5;
                // }else{
                //     weather_date.dayTemperatureMax[i]+=0.5;
                // }

                // if(weather_date.dayTemperatureMin[i]<0){
                //     weather_date.dayTemperatureMin[i]-=0.5;
                // }else{
                //     weather_date.dayTemperatureMin[i]+=0.5;
                // }

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


    if(weather_no_data_flag)
    {
        if(func_cb.sta == FUNC_WEATHER)
        {
            f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
            f_weather->no_weather_dada = true;
        }
        ///设置标题栏名字///
        txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
//        compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
        compo_textbox_set(txt, i18n[STR_WEATHER]);
        compo_textbox_set_align_center(txt, false);

        picbox = compo_picturebox_create(frm,UI_BUF_I330001_WEATHER_NO_DATA_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,84+50/2);

        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,160+56/2, GUI_SCREEN_WIDTH/1.1,widget_text_get_max_height()*2);
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set_multiline_drag(txt, false);      //避免既有滚动又有省略号的情况
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);
        return frm;
    }

    ///设置标题栏时间///
//    compo_textbox_t* time_title = compo_textbox_create(frm, 32);
//    compo_textbox_set_align_center(time_title, false);
//    compo_textbox_set_location(time_title, 168, 15, 52, 23);
//    compo_setid(time_title, COMPO_ID_TXT_TIME);
//    compo_bonddata(time_title, COMPO_BOND_HOURMIN_TXT);
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_WEATHER_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
//    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
    compo_textbox_set(txt, i18n[STR_WEATHER]);
    compo_textbox_set_align_center(txt, false);

    picbox = compo_picturebox_create(frm, weather_list[get_weather_id[0]].res_addr);///背景图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/1.5);
//    weather_date.fristDayCurrTemperature = 44;
//    if(weather_date.fristDayCurrTemperature == 0)weather_date.fristDayCurrTemperature = 0;
    compo_number_t *num=NULL;
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        num = compo_number_create(frm,UI_BUF_I330001_WEATHER_NUM_BIN, 3);
        compo_number_set_margin(num, 0);
        compo_number_set_radix(num, 11, true);
        compo_number_set(num, weather_date.fristDayCurrTemperature);
        compo_number_set_align(num, 1 );
        compo_number_set_pos(num, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }
    else
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I330001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-gui_image_get_size(UI_BUF_I330001_WEATHER_NUM_BIN).wid/2+2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);

        picbox = compo_picturebox_create(frm, UI_BUF_I330001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+gui_image_get_size(UI_BUF_I330001_WEATHER_NUM_BIN).wid/2-2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }

    if(displayInfo.isFahrenheit)     ///是否为华氏度
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I330001_WEATHER_DF_BIN);///温度符号
    }
    else
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I330001_WEATHER_DC_BIN);///温度符号
    }

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+compo_number_get_rel_location(num).wid/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    else
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+gui_image_get_size(UI_BUF_I330001_WEATHER_NUM_BIN).wid, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_HIGHEST],weather_date.dayTemperatureMax[0]); ///今天 最高温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_HIGHEST]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-4-92, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,92,24);
    compo_textbox_set_right_align(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_LOWSET],weather_date.dayTemperatureMin[0]); ///今天 最低温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_LOWSET]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+4, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,92,24);
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm, strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.25);
    compo_textbox_set_forecolor(txt, make_color(0xc2,0xe1,0xf9));

    // printf("height:%d\n",widget_text_get_max_height());
    //第二页
    for(int i=0; i<7; i++)
    {

        txt = compo_textbox_create(frm,20);
        compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X/6,GUI_SCREEN_HEIGHT+30+(i*36)-8,GUI_SCREEN_WIDTH/5,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,false);
        compo_textbox_set(txt,i18n[STR_SUNDAY+week_sort[i]]);/// 星期
        if(i==0)
        {
            compo_textbox_set(txt,i18n[STR_TO_DAY]);/// 星期
        }
        // printf("y:%d\n",GUI_SCREEN_HEIGHT+30+(i*36)-8);

//        printf("weather_id=%d\n",get_weather_id[i]);
        picbox = compo_picturebox_create(frm,weather_list[get_weather_id[i]].res_addr);/// 天气
        compo_picturebox_set_size(picbox,gui_image_get_size( weather_list[get_weather_id[i]].res_addr).wid/3,\
                                  gui_image_get_size( weather_list[get_weather_id[i]].res_addr).hei/3);
        compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/10-6,GUI_SCREEN_HEIGHT+30+(i*36));

        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            if(displayInfo.isFahrenheit)     ///是否为华氏度
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℉",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
            else
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℃",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --/--");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.2,GUI_SCREEN_HEIGHT+30+(i*36)- 10);
        compo_textbox_set_align_center(txt, false);

    }
    widget_page_set_client(frm->page_body, 0, page_y);

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;

    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
} f_weather_t;


enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           UI_BUF_I332001_WEATHER_ICON_WEATHER_00_BIN  },                //未知
    {STR_SUNNY,             UI_BUF_I332001_WEATHER_ICON_WEATHER_01_BIN  },                //晴天
    {STR_CLOUDY,            UI_BUF_I332001_WEATHER_ICON_WEATHER_02_BIN  },                //多云
    {STR_OVERCAST,          UI_BUF_I332001_WEATHER_ICON_WEATHER_03_BIN  },                //阴天
    {STR_MODERATE_RAIN,     UI_BUF_I332001_WEATHER_ICON_WEATHER_04_BIN  },                //阵雨
    {STR_RAINY_SHOWERS,     UI_BUF_I332001_WEATHER_ICON_WEATHER_05_BIN  },                //雷阵雨
    {STR_SLEET,             UI_BUF_I332001_WEATHER_ICON_WEATHER_06_BIN  },                //雨夹雪
    {STR_DRIZZLE,           UI_BUF_I332001_WEATHER_ICON_WEATHER_07_BIN  },                //小雨
    {STR_HEAVY_RAIN,        UI_BUF_I332001_WEATHER_ICON_WEATHER_08_BIN  },                //大雨
    {STR_RAINY,             UI_BUF_I332001_WEATHER_ICON_WEATHER_09_BIN  },                //雪
    {STR_SAND_AND_DUST,     UI_BUF_I332001_WEATHER_ICON_WEATHER_10_BIN  },                //沙尘暴
    {STR_HAZE,              UI_BUF_I332001_WEATHER_ICON_WEATHER_11_BIN  },                //雾霾
    {STR_WINDY,             UI_BUF_I332001_WEATHER_ICON_WEATHER_12_BIN  },                //大风
    {STR_SUNNY,             UI_BUF_I332001_WEATHER_ICON_WEATHER_13_BIN  },                //明夜
    {STR_CLOUDY,            UI_BUF_I332001_WEATHER_ICON_WEATHER_14_BIN  },                //云遮月
    {STR_RAINY_SHOWERS,     UI_BUF_I332001_WEATHER_ICON_WEATHER_15_BIN  },                //阵雨夜

};
enum
{
    TOUCH_FINISH_STATE=0,
    AUTO_STATE,

};
enum
{
    PAGE_1=0,
    PAGE_2,
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
    TOTCH_MOVE,
};
static int16_t page_y = 0;
//滑动处理
static void func_weather_move(void)
{
#define   PAGE_TWO_SIZE  558  //最底y轴
#define   TOYCH_LAST_DY  40   //切换页滑动y
#define   TICK_TIME      8   //步进y像素点时间
#define   STEP_NUM       8    //步进y像素点
    f_weather_t *f_sleep = (f_weather_t *)func_cb.f_cb;


    if(f_sleep->page_num == PAGE_1)//第一页
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = dy;
            if(f_sleep->move_offset > 0)
            {
                f_sleep->move_offset = 0;
            }
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset <= (-TOYCH_LAST_DY))//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->tick = tick_get();
                        f_sleep->move_offset-=STEP_NUM;

                        if(f_sleep->move_offset <= -GUI_SCREEN_HEIGHT)
                        {
                            f_sleep->move_offset = -GUI_SCREEN_HEIGHT;
                            f_sleep->page_num = PAGE_2;//第2页
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                            f_sleep->page_old_y = f_sleep->move_offset;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->move_offset+=STEP_NUM;

                        if(f_sleep->move_offset >= 0)
                        {
                            f_sleep->move_offset = 0;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                }
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);
            }
        }

    }
    else if(f_sleep->page_num == PAGE_2)
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = f_sleep->page_old_y+dy;
            if(f_sleep->move_offset < -PAGE_TWO_SIZE)
            {
                f_sleep->move_offset = -PAGE_TWO_SIZE;
            }
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset >= -(GUI_SCREEN_HEIGHT-TOYCH_LAST_DY))//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->move_offset+=STEP_NUM;

                        if(f_sleep->move_offset >= 0)
                        {
                            f_sleep->move_offset = 0;
                            f_sleep->page_num = PAGE_1;//第2页
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->touch_state = TOUCH_FINISH_STATE;
                    }
                }
                f_sleep->page_old_y = f_sleep->move_offset;
                page_y = f_sleep->move_offset;
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);
            }
        }
    }

    //    printf("move_offset:%d\n",f_sleep->move_offset);
}

static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t *weather_date = (ute_module_weather_data_t *)ab_zalloc(sizeof(ute_module_weather_data_t));
    // ute_module_weather_data_t  weather_date;
    uteModuleWeatherGetData(weather_date);//获取天气状态
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        // printf(" f_weather->DayWeather[%d] = weather_date.DayWeather[%d]\n", f_weather->DayWeather[i] ,weather_date->DayWeather[i]);
        if(f_weather->DayWeather[i] != weather_date->DayWeather[i])
        {
            f_weather->DayWeather[i] = weather_date->DayWeather[i];
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        // printf("displayInfo.isFahrenheit:%d f_weather->isFahrenheit_flag:%d weather_flag:%d\n",displayInfo.isFahrenheit,f_weather->isFahrenheit_flag,weather_flag);
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
    ab_free(weather_date);
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}
static bool weather_no_data_flag = true;
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
    char str_buff[50];//用于存放打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));

    uteModuleSystemtimeGetTime(&time);//获取系统时间
//    printf("week:%d\n",time.week);
    week_day = time.week; //获取星期
    for(int i=0; i<=6; i++) //星期排序
    {
        week_sort[i] = week_day;
        if(++week_day==7)
        {
            week_day = 0;
        }
    }

    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态
//        printf("year:%d mon:%d day:%d hour:%d min:%d sec:%d\n",weather_date.updateTime.year,weather_date.updateTime.month,weather_date.updateTime.day,weather_date.updateTime.hour,weather_date.updateTime.min,weather_date.updateTime.sec);

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }
//        int pic_num=3;
        for(int i=0; i<7; i++) //获取一周的天气
        {
//            printf("weather:%d\n",weather_date.DayWeather[i]>>8);
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
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


    if(weather_no_data_flag)
    {
        ///设置标题栏名字///
        picbox = compo_picturebox_create(frm,UI_BUF_I332001_WEATHER_NO_DATA_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,120);

        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,226, 276,widget_text_get_max_height()*2);
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_multiline_drag(txt, true);
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);

        return frm;
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I332001_WEATHER_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    picbox = compo_picturebox_create(frm, weather_list[get_weather_id[0]].res_addr);///背景图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 92);
    compo_number_t *num=NULL;
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        num = compo_number_create(frm,UI_BUF_I332001_WEATHER_NUM_BIN, 3);
        compo_number_set_margin(num, 0);
        compo_number_set_radix(num, 11, true);
        compo_number_set(num, weather_date.fristDayCurrTemperature);
        compo_number_set_align(num, 1 );
        compo_number_set_pos(num, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }
    else
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I332001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-gui_image_get_size(UI_BUF_I332001_WEATHER_NUM_BIN).wid/2+2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);

        picbox = compo_picturebox_create(frm, UI_BUF_I332001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+gui_image_get_size(UI_BUF_I332001_WEATHER_NUM_BIN).wid/2-2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }

    if(displayInfo.isFahrenheit)     ///是否为华氏度
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I332001_WEATHER_DF_BIN);///温度符号
    }
    else
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I332001_WEATHER_DC_BIN);///温度符号
    }

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+compo_number_get_rel_location(num).wid/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    else
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+gui_image_get_size(UI_BUF_I332001_WEATHER_NUM_BIN).wid, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_HIGHEST],weather_date.dayTemperatureMax[0]); ///今天 最高温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_HIGHEST]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-8-92, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,100, widget_text_get_max_height());
    compo_textbox_set_right_align(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_LOWSET],weather_date.dayTemperatureMin[0]); ///今天 最低温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_LOWSET]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+8, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,100, widget_text_get_max_height());
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm, strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.25,200,32);
    compo_textbox_set_forecolor(txt, make_color(0xc2,0xe1,0xf9));

    //第二页

    //设置标题栏名字//
    txt = compo_textbox_create(frm,strlen(i18n[STR_FUTRUE_WEATHER]));
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/2,390+38/2,120,38);
    compo_textbox_set(txt, i18n[STR_FUTRUE_WEATHER]);

    for(int i=0; i<7; i++)
    {
        if(i!=6)
        {
            compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+136+(i*57), 316, 1);
            compo_shape_set_color(shape, make_color(0X3D,0X3D,0X3D));
        }

        txt = compo_textbox_create(frm,20);
        compo_textbox_set_location(txt, 22,GUI_SCREEN_HEIGHT+95+(i*57),90,34);
        compo_textbox_set(txt,i18n[STR_SUNDAY+week_sort[i]]);/// 星期
        compo_textbox_set_align_center(txt,false);
        if(i==0)
        {
            compo_textbox_set(txt,i18n[STR_TO_DAY]);/// 星期
        }

        picbox = compo_picturebox_create(frm,weather_list[get_weather_id[i]].res_addr);/// 天气
        compo_picturebox_set_size(picbox,32,32);
        compo_picturebox_set_pos(picbox, 163,GUI_SCREEN_HEIGHT+108+(i*57));

        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            if(displayInfo.isFahrenheit)     ///是否为华氏度
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℉",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
            else
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℃",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --/-- ");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_location(txt, 230,GUI_SCREEN_HEIGHT+95+(i*57),108,34);
        compo_textbox_set_align_center(txt, false);

    }
    widget_page_set_client(frm->page_body, 0, page_y);

    return frm;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

static int16_t page_y = 0;
typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;
    bool no_weather_dada;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           0  },                //未知
    {STR_SUNNY,             1  },                //晴天
    {STR_CLOUDY,            2  },                //多云
    {STR_OVERCAST,          3  },                //阴天
    {STR_MODERATE_RAIN,     4  },                //阵雨
    {STR_RAINY_SHOWERS,     5  },                //雷阵雨
    {STR_SLEET,             6  },                //雨夹雪
    {STR_DRIZZLE,           7  },                //小雨
    {STR_HEAVY_RAIN,        8  },                //大雨
    {STR_RAINY,             9  },                //雪
    {STR_SAND_AND_DUST,     10  },                //沙尘暴
    {STR_HAZE,              11  },                //雾霾
    {STR_WINDY,             12  },                //大风
    {STR_SUNNY,             13  },                //明夜
    {STR_CLOUDY,            14  },                //云遮月
    {STR_RAINY_SHOWERS,     15  },                //阵雨夜
    {STR_RAINY_SHOWERS,     16  },                //阵雨夜
};
static const u16 weather_uv[5] =
{
    STR_UV_VERY_WEAK, // 很弱
    STR_UV_WEAK, // 弱
    STR_UV_MODERATE, // 中等
    STR_UV_STRONG, // 强
    STR_UV_VERY_STRONG, // 很强
};
static const u32 future_weather_icon[16] =
{
    UI_BUF_I338001_12_WEATHER_WEATHER_00_UNKNOWEN_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_01_SUNNY_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_02_CLOUDY_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_03_OVERCAST_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_04_SHOWER_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_05_T_STORM_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_06_SLEET_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_07_LIGHT_RAIN_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_08_HEAVY_RAIN_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_09_SNOW_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_10_SAND_STORM_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_11_HAZE_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_12_WINDY_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_13_NIGHT_SUNNY_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_14_NIGHT_CLOUDY_BIN,
    UI_BUF_I338001_12_WEATHER_WEATHER_15_NIGHT_RAIN_BIN,
};
static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t  weather_date;
    uteModuleWeatherGetData(&weather_date);//获取天气状态
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        if(f_weather->DayWeather[i] != weather_date.DayWeather[i])
        {
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{

#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT
    compo_textbox_t *txt;
    compo_picturebox_t * picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    u8 week_day=0;
    u8 get_weather_id[7];//存放一个星期的排序
    char str_buff[50];//用于存放打印数据
    char str_humidity_buff[8];//用于存放湿度打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    const u16 week_sort[7]= {STR_SUNDAY, // 周日
                             STR_MONDAY, // 周一
                             STR_TUESDAY, // 周二
                             STR_WEDNESDAY, // 周三
                             STR_THURSDAY, // 周四
                             STR_FRIDAY, // 周五
                             STR_SATURDAY, // 周六
                            };
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态
        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
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

    if(weather_no_data_flag)
    {
        if(func_cb.sta == FUNC_WEATHER)
        {
            f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
            f_weather->no_weather_dada = true;
        }
        ///设置标题栏名字///
        picbox = compo_picturebox_create_for_page(frm,frm->page,UI_BUF_I338001_12_WEATHER_CLOUD_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,120);

        txt = compo_textbox_create_for_page(frm,frm->page,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,226, 276,widget_text_get_max_height()*2);
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_multiline_drag(txt, true);
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);

        return frm;
    }

    picbox = compo_picturebox_create(frm,UI_BUF_I338001_12_WEATHER_WEATHER_BG_BIN);///背景图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    picbox = compo_picturebox_create(frm,0);///
    compo_picturebox_set_pos(picbox, 80/2+72,147);
    compo_picturebox_set(picbox, future_weather_icon[get_weather_id[0]]);
    memset(str_buff,0,sizeof(str_buff));
    if(weather_no_data_flag)
    {
        snprintf(str_buff,sizeof(str_buff),"--");
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"%d",weather_date.fristDayCurrTemperature);
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_pos(txt,82/2+188,58/2+92);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm,1);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_pos(txt,28/2+270,40+87);
    compo_textbox_set(txt, displayInfo.isFahrenheit ? "℉" : "℃");

    if(!weather_no_data_flag)
    {
        if(!displayInfo.isFahrenheit)      ///是否为华氏度
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℃",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
        else
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℉",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"--/--℃");
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_pos(txt,200,170);
    compo_textbox_set_align_center(txt,false);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm,strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,70,200,35);
    compo_textbox_set_forecolor(txt, make_color(152,209,255));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    //紫外线icon
    picbox = compo_picturebox_create(frm,UI_BUF_I338001_12_WEATHER_UV_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 15+100,15+233);
    //紫外线data
    txt = compo_textbox_create(frm,strlen(i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]));
    compo_textbox_set_location(txt,15+100,270+24/2,130,30);
    compo_textbox_set(txt,i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]);

    //湿度icon
    picbox = compo_picturebox_create(frm,UI_BUF_I338001_12_WEATHER_HUMIDITY_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 229+42/2,15+233);
    //湿度data
    memset(str_humidity_buff,0,sizeof(str_humidity_buff));
    snprintf(str_humidity_buff,sizeof(str_humidity_buff),"%d%%",weather_date.fristDayWeatherHumidity);
    txt = compo_textbox_create(frm,strlen(str_humidity_buff));
    compo_textbox_set_pos(txt,229+42/2,270+24/2);
    compo_textbox_set(txt,str_humidity_buff);

    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 30, 120, widget_text_get_max_height());
    compo_textbox_set(txt, i18n[STR_WEATHER]);

    u16 spacing_y=GUI_SCREEN_HEIGHT;
    //第二页
    for(int i=0; i<7; i++)
    {
        if(i==4)
        {
            spacing_y = GUI_SCREEN_HEIGHT*2+19;
        }
        u16 str_id = week_sort[(time.week+i)%7];
        if(i == 0)
        {
            str_id = STR_TO_DAY;
        }
        txt = compo_textbox_create(frm,strlen(i18n[str_id]));
        compo_textbox_set_location(txt, 39,81+spacing_y,50,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,false);
        compo_textbox_set(txt,i18n[str_id]);/// 星期

        picbox = compo_picturebox_create(frm,future_weather_icon[get_weather_id[i]]);
        compo_picturebox_set_pos(picbox,34/2+148,94+spacing_y);
        compo_picturebox_set_size(picbox,34,34);

        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d/%02d%s",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i],displayInfo.isFahrenheit ? "℉" : "℃");//一周 小~大 温度
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), "--/--%s",displayInfo.isFahrenheit ? "℉" : "℃");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set_location(txt,225,81+spacing_y,95,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,false);
        compo_textbox_set_right_align(txt,true);
        compo_textbox_set(txt,str_buff);

        compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,121+spacing_y,280,1);
        compo_shape_set_color(shape,COLOR_GRAY);

        spacing_y +=59;
    }
    return frm;

#else
    compo_textbox_t *txt;
    compo_picturebox_t * picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    u8 week_sort[7];
    u8 week_day=0;
    u8 get_weather_id[7];//存放一个星期的排序
    char str_buff[50];//用于存放打印数据
    char str_humidity_buff[8];//用于存放湿度打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));
    uteModuleSystemtimeGetTime(&time);//获取系统时间
//    printf("week:%d\n",time.week);
    week_day = time.week; //获取星期
    for(int i=0; i<=6; i++) //星期排序
    {
        week_sort[i] = week_day;
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
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
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

    if(weather_no_data_flag)
    {
        ///设置标题栏名字///
        picbox = compo_picturebox_create_for_page(frm,frm->page,UI_BUF_I338001_12_WEATHER_CLOUD_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,120);

        txt = compo_textbox_create_for_page(frm,frm->page,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,226, 276,widget_text_get_max_height()*2);
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_multiline_drag(txt, true);
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);

        return frm;
    }

    picbox = compo_picturebox_create(frm,UI_BUF_I338001_12_WEATHER_WEATHER_BG_BIN);///背景图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);
    picbox = compo_picturebox_create(frm,0);///
    compo_picturebox_set_pos(picbox, 98/2+54,98/2+100);
    compo_picturebox_set(picbox, future_weather_icon[get_weather_id[0]]);
    memset(str_buff,0,sizeof(str_buff));
    if(weather_no_data_flag)
    {
        snprintf(str_buff,sizeof(str_buff),"--");
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"%d",weather_date.fristDayCurrTemperature);
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_pos(txt,82/2+188,58/2+92);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm,1);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_pos(txt,28/2+270,40+87);
    compo_textbox_set(txt, displayInfo.isFahrenheit ? "℉" : "℃");

    if(!weather_no_data_flag)
    {
        if(!displayInfo.isFahrenheit)      ///是否为华氏度
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℃",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
        else
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℉",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"--/--℃");
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_pos(txt,82/2+188,28/2+155);
    compo_textbox_set(txt, str_buff);
    txt = compo_textbox_create(frm,strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set_pos(txt,82/2+188,24/2+183);
    compo_textbox_set_forecolor(txt, make_color(152,209,255));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    //紫外线icon
    picbox = compo_picturebox_create(frm,UI_BUF_I338001_12_WEATHER_UV_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 15+100,15+233);
    //紫外线data
    txt = compo_textbox_create(frm,strlen(i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]));
    compo_textbox_set_location(txt,15+100,270+24/2,130,30);
    compo_textbox_set(txt,i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]);
    //紫外线txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_UV]));
    compo_textbox_set_location(txt,15+100,300+24/2,80,30);
    compo_textbox_set(txt,i18n[STR_UV]);
    //湿度icon
    picbox = compo_picturebox_create(frm,UI_BUF_I338001_12_WEATHER_HUMIDITY_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 229+42/2,15+233);
    //湿度data
    memset(str_humidity_buff,0,sizeof(str_humidity_buff));
    snprintf(str_humidity_buff,sizeof(str_humidity_buff),"%d%%",weather_date.fristDayWeatherHumidity);
    txt = compo_textbox_create(frm,strlen(str_humidity_buff));
    compo_textbox_set_pos(txt,229+42/2,270+24/2);
    compo_textbox_set(txt,str_humidity_buff);
    //湿度txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_HUMIDITY]));
    compo_textbox_set_location(txt,229+42/2,300+24/2,80,30);
    compo_textbox_set(txt,i18n[STR_HUMIDITY]);
    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 30, 120, widget_text_get_max_height());
    compo_textbox_set(txt, i18n[STR_WEATHER]);

    //未来天气txt
    txt = compo_textbox_create(frm,strlen(i18n[STR_FUTURE_WEATHER]));
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/2,390+38/2,120,38);
    compo_textbox_set(txt,i18n[STR_FUTURE_WEATHER]);
    //第二页
    for(int i=1; i<4; i++)
    {
        //bg pic
        picbox = compo_picturebox_create(frm,UI_BUF_I338001_12_WEATHER_BG2_BIN);
        compo_picturebox_set_pos(picbox,82+(i-1)*(98),248/2+81+GUI_SCREEN_HEIGHT);

        //星期 txt
        txt = compo_textbox_create(frm,20);
        compo_textbox_set_location(txt, 82+(i-1)*(98),42/2+101+GUI_SCREEN_HEIGHT,50,widget_text_get_max_height());
        compo_textbox_set(txt,i18n[STR_SUNDAY+week_sort[i]]);/// 星期
        if(i==1)
        {
            compo_textbox_set(txt,i18n[STR_TO_DAY]);/// 星期
        }

        //天气 icon
        //图切的不对，80x80了，
        picbox = compo_picturebox_create(frm,future_weather_icon[get_weather_id[i]]);
        compo_picturebox_set_pos(picbox,82+(i-1)*(98),56/2+157+GUI_SCREEN_HEIGHT);
        compo_picturebox_set_size(picbox,56,56);
        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {

            snprintf(str_buff, sizeof(str_buff), "%02d°",weather_date.dayTemperatureMax[i]);//一周 小~大 温度

        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --°");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_location(txt, 82+(i-1)*(98),36/2+216+GUI_SCREEN_HEIGHT,50,widget_text_get_max_height());
        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d°",weather_date.dayTemperatureMin[i]);//一周 小~大 温度

        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --°");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_location(txt, 82+(i-1)*(98),36/2+265+GUI_SCREEN_HEIGHT,50,widget_text_get_max_height());
    }
    widget_page_set_client(frm->page_body, 0, page_y);

    return frm;
#endif
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;

    bool        touch_flag;
    s32         move_offset;
    s32         page_old_y;
    u8          touch_state;
    u8          page_num;
    uint32_t    tick;
    u8          switch_page_state;
} f_weather_t;


enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           UI_BUF_I340001_WEATHER_ICON_WEATHER_00_BIN  },                //未知
    {STR_SUNNY,             UI_BUF_I340001_WEATHER_ICON_WEATHER_01_BIN  },                //晴天
    {STR_CLOUDY,            UI_BUF_I340001_WEATHER_ICON_WEATHER_02_BIN  },                //多云
    {STR_OVERCAST,          UI_BUF_I340001_WEATHER_ICON_WEATHER_03_BIN  },                //阴天
    {STR_MODERATE_RAIN,     UI_BUF_I340001_WEATHER_ICON_WEATHER_04_BIN  },                //阵雨
    {STR_RAINY_SHOWERS,     UI_BUF_I340001_WEATHER_ICON_WEATHER_05_BIN  },                //雷阵雨
    {STR_SLEET,             UI_BUF_I340001_WEATHER_ICON_WEATHER_06_BIN  },                //雨夹雪
    {STR_DRIZZLE,           UI_BUF_I340001_WEATHER_ICON_WEATHER_07_BIN  },                //小雨
    {STR_HEAVY_RAIN,        UI_BUF_I340001_WEATHER_ICON_WEATHER_08_BIN  },                //大雨
    {STR_RAINY,             UI_BUF_I340001_WEATHER_ICON_WEATHER_09_BIN  },                //雪
    {STR_SAND_AND_DUST,     UI_BUF_I340001_WEATHER_ICON_WEATHER_10_BIN  },                //沙尘暴
    {STR_HAZE,              UI_BUF_I340001_WEATHER_ICON_WEATHER_11_BIN  },                //雾霾
    {STR_WINDY,             UI_BUF_I340001_WEATHER_ICON_WEATHER_12_BIN  },                //大风
    {STR_SUNNY,             UI_BUF_I340001_WEATHER_ICON_WEATHER_13_BIN  },                //明夜
    {STR_CLOUDY,            UI_BUF_I340001_WEATHER_ICON_WEATHER_14_BIN  },                //云遮月
    {STR_RAINY_SHOWERS,     UI_BUF_I340001_WEATHER_ICON_WEATHER_15_BIN  },                //阵雨夜

};
enum
{
    TOUCH_FINISH_STATE=0,
    AUTO_STATE,

};
enum
{
    PAGE_1=0,
    PAGE_2,
};
enum
{
    SWITCH_YES=0,
    SWITCH_NO,
    TOTCH_MOVE,
};
static int16_t page_y = 0;
//滑动处理
static void func_weather_move(void)
{
#define   PAGE_TWO_SIZE  558  //最底y轴
#define   TOYCH_LAST_DY  40   //切换页滑动y
#define   TICK_TIME      8   //步进y像素点时间
#define   STEP_NUM       8    //步进y像素点
    f_weather_t *f_sleep = (f_weather_t *)func_cb.f_cb;


    if(f_sleep->page_num == PAGE_1)//第一页
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = dy;
            if(f_sleep->move_offset > 0)
            {
                f_sleep->move_offset = 0;
            }
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset <= (-TOYCH_LAST_DY))//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->tick = tick_get();
                        f_sleep->move_offset-=STEP_NUM;

                        if(f_sleep->move_offset <= -GUI_SCREEN_HEIGHT)
                        {
                            f_sleep->move_offset = -GUI_SCREEN_HEIGHT;
                            f_sleep->page_num = PAGE_2;//第2页
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                            f_sleep->page_old_y = f_sleep->move_offset;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->move_offset+=STEP_NUM;

                        if(f_sleep->move_offset >= 0)
                        {
                            f_sleep->move_offset = 0;
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                }
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);
            }
        }

    }
    else if(f_sleep->page_num == PAGE_2)
    {
        if(f_sleep->touch_flag)//触摸状态
        {
            s32 dx, dy;
            f_sleep->touch_flag = ctp_get_dxy(&dx, &dy);
            f_sleep->move_offset = f_sleep->page_old_y+dy;
            if(f_sleep->move_offset < -PAGE_TWO_SIZE)
            {
                f_sleep->move_offset = -PAGE_TWO_SIZE;
            }
            widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);

            if(f_sleep->touch_flag == false)//松手触发自动移动页
            {
                f_sleep->touch_state = AUTO_STATE;
                if(f_sleep->move_offset >= -(GUI_SCREEN_HEIGHT-TOYCH_LAST_DY))//满足切换下一页
                {
                    f_sleep->switch_page_state = SWITCH_YES;
                }
                else
                {
                    f_sleep->switch_page_state = SWITCH_NO;
                }
            }
        }
        else
        {
            if(f_sleep->touch_state == AUTO_STATE)
            {
                if(tick_check_expire(f_sleep->tick, TICK_TIME))//自动滑动
                {
                    f_sleep->tick = tick_get();
                    if(f_sleep->switch_page_state == SWITCH_YES)//满足切换下一页
                    {
                        f_sleep->move_offset+=STEP_NUM;

                        if(f_sleep->move_offset >= 0)
                        {
                            f_sleep->move_offset = 0;
                            f_sleep->page_num = PAGE_1;//第2页
                            f_sleep->touch_state = TOUCH_FINISH_STATE;
                        }
                    }
                    else if(f_sleep->switch_page_state == SWITCH_NO)
                    {
                        f_sleep->touch_state = TOUCH_FINISH_STATE;
                    }
                }
                f_sleep->page_old_y = f_sleep->move_offset;
                page_y = f_sleep->move_offset;
                widget_page_set_client(func_cb.frm_main->page_body, 0, f_sleep->move_offset);
            }
        }
    }

    //    printf("move_offset:%d\n",f_sleep->move_offset);
}

static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t *weather_date = (ute_module_weather_data_t *)ab_zalloc(sizeof(ute_module_weather_data_t));
    // ute_module_weather_data_t  weather_date;
    uteModuleWeatherGetData(weather_date);//获取天气状态
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        // printf(" f_weather->DayWeather[%d] = weather_date.DayWeather[%d]\n", f_weather->DayWeather[i] ,weather_date->DayWeather[i]);
        if(f_weather->DayWeather[i] != weather_date->DayWeather[i])
        {
            f_weather->DayWeather[i] = weather_date->DayWeather[i];
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        // printf("displayInfo.isFahrenheit:%d f_weather->isFahrenheit_flag:%d weather_flag:%d\n",displayInfo.isFahrenheit,f_weather->isFahrenheit_flag,weather_flag);
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
    ab_free(weather_date);
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}
static bool weather_no_data_flag = true;
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
    char str_buff[50];//用于存放打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));

    uteModuleSystemtimeGetTime(&time);//获取系统时间
//    printf("week:%d\n",time.week);
    week_day = time.week; //获取星期
    for(int i=0; i<=6; i++) //星期排序
    {
        week_sort[i] = week_day;
        if(++week_day==7)
        {
            week_day = 0;
        }
    }

    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态
//        printf("year:%d mon:%d day:%d hour:%d min:%d sec:%d\n",weather_date.updateTime.year,weather_date.updateTime.month,weather_date.updateTime.day,weather_date.updateTime.hour,weather_date.updateTime.min,weather_date.updateTime.sec);

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }
//        int pic_num=3;
        for(int i=0; i<7; i++) //获取一周的天气
        {
//            printf("weather:%d\n",weather_date.DayWeather[i]>>8);
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
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


    if(weather_no_data_flag)
    {
        ///设置标题栏名字///
        picbox = compo_picturebox_create(frm,UI_BUF_I340001_WEATHER_NO_DATA_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,120);

        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,226, 276,widget_text_get_max_height()*2);
        compo_textbox_set_multiline(txt, true);
        compo_textbox_set_multiline_drag(txt, true);
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);

        return frm;
    }

    picbox = compo_picturebox_create(frm, UI_BUF_I340001_WEATHER_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    picbox = compo_picturebox_create(frm, weather_list[get_weather_id[0]].res_addr);///背景图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 92);
    compo_number_t *num=NULL;
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        num = compo_number_create(frm,UI_BUF_I340001_WEATHER_NUM_BIN, 3);
        compo_number_set_margin(num, 0);
        compo_number_set_radix(num, 11, true);
        compo_number_set(num, weather_date.fristDayCurrTemperature);
        compo_number_set_align(num, 1 );
        compo_number_set_pos(num, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }
    else
    {
        picbox = compo_picturebox_create(frm, UI_BUF_I340001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-gui_image_get_size(UI_BUF_I340001_WEATHER_NUM_BIN).wid/2+2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);

        picbox = compo_picturebox_create(frm, UI_BUF_I340001_WEATHER_NUM_BIN);///背景图片
        compo_picturebox_cut(picbox, 10, 11 );
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+gui_image_get_size(UI_BUF_I340001_WEATHER_NUM_BIN).wid/2-2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/5);
    }

    if(displayInfo.isFahrenheit)     ///是否为华氏度
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I340001_WEATHER_DF_BIN);///温度符号
    }
    else
    {
        picbox = compo_picturebox_create(frm,UI_BUF_I340001_WEATHER_DC_BIN);///温度符号
    }

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+compo_number_get_rel_location(num).wid/2, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    else
    {
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X*1.15+gui_image_get_size(UI_BUF_I340001_WEATHER_NUM_BIN).wid, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/12);
    }
    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_HIGHEST],weather_date.dayTemperatureMax[0]); ///今天 最高温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_HIGHEST]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-8-92, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,100, widget_text_get_max_height());
    compo_textbox_set_right_align(txt, true);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    if(uteModuleWeatherGetCurrDay() == time.day)
    {
        snprintf(str_buff, sizeof(str_buff), "%s %d",i18n[STR_LOWSET],weather_date.dayTemperatureMin[0]); ///今天 最低温度
    }
    else
    {
        snprintf(str_buff, sizeof(str_buff), "%s --",i18n[STR_LOWSET]);
    }
    txt = compo_textbox_create(frm, strlen(str_buff));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+8, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.9,100, widget_text_get_max_height());
    compo_textbox_set_right_align(txt, false);
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm, strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.25,200,32);
    compo_textbox_set_forecolor(txt, make_color(0xc2,0xe1,0xf9));

    //第二页

    //设置标题栏名字//
    txt = compo_textbox_create(frm,strlen(i18n[STR_FUTRUE_WEATHER]));
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/2,390+38/2,120,38);
    compo_textbox_set(txt, i18n[STR_FUTRUE_WEATHER]);

    for(int i=0; i<7; i++)
    {
        if(i!=6)
        {
            compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
            compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+136+(i*57), 316, 1);
            compo_shape_set_color(shape, make_color(0X3D,0X3D,0X3D));
        }

        txt = compo_textbox_create(frm,20);
        compo_textbox_set_location(txt, 22,GUI_SCREEN_HEIGHT+95+(i*57),90,34);
        compo_textbox_set(txt,i18n[STR_SUNDAY+week_sort[i]]);/// 星期
        compo_textbox_set_align_center(txt,false);
        if(i==0)
        {
            compo_textbox_set(txt,i18n[STR_TO_DAY]);/// 星期
        }

        picbox = compo_picturebox_create(frm,weather_list[get_weather_id[i]].res_addr);/// 天气
        compo_picturebox_set_size(picbox,32,32);
        compo_picturebox_set_pos(picbox, 163,GUI_SCREEN_HEIGHT+108+(i*57));

        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            if(displayInfo.isFahrenheit)     ///是否为华氏度
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℉",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
            else
            {
                snprintf(str_buff, sizeof(str_buff), "%02d/%02d℃",weather_date.dayTemperatureMin[i],weather_date.dayTemperatureMax[i]);//一周 小~大 温度
            }
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), " --/-- ");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set(txt,str_buff);
        compo_textbox_set_location(txt, 230,GUI_SCREEN_HEIGHT+95+(i*57),108,34);
        compo_textbox_set_align_center(txt, false);

    }
    widget_page_set_client(frm->page_body, 0, page_y);

    return frm;
}

#elif GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT

#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

static int16_t page_y = 0;
typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;
    bool no_weather_dada;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {STR_UNKNOWN,           0  },                //未知
    {STR_SUNNY,             1  },                //晴天
    {STR_CLOUDY,            2  },                //多云
    {STR_OVERCAST,          3  },                //阴天
    {STR_MODERATE_RAIN,     4  },                //阵雨
    {STR_RAINY_SHOWERS,     5  },                //雷阵雨
    {STR_SLEET,             6  },                //雨夹雪
    {STR_DRIZZLE,           7  },                //小雨
    {STR_HEAVY_RAIN,        8  },                //大雨
    {STR_RAINY,             9  },                //雪
    {STR_SAND_AND_DUST,     10  },                //沙尘暴
    {STR_HAZE,              11  },                //雾霾
    {STR_WINDY,             12  },                //大风
    {STR_SUNNY,             13  },                //明夜
    {STR_CLOUDY,            14  },                //云遮月
    {STR_RAINY_SHOWERS,     15  },                //阵雨夜
    {STR_RAINY_SHOWERS,     16  },                //阵雨夜
};
static const u16 weather_uv[5] =
{
    STR_UV_VERY_WEAK, // 很弱
    STR_UV_WEAK, // 弱
    STR_UV_MODERATE, // 中等
    STR_UV_STRONG, // 强
    STR_UV_VERY_STRONG, // 很强
};
static const u32 future_weather_icon[16] =
{
    UI_BUF_I342001_12_WEATHER_WEATHER_00_UNKNOWEN_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_01_SUNNY_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_02_CLOUDY_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_03_OVERCAST_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_04_SHOWER_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_05_T_STORM_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_06_SLEET_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_07_LIGHT_RAIN_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_08_HEAVY_RAIN_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_09_SNOW_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_10_SAND_STORM_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_11_HAZE_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_12_WINDY_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_13_NIGHT_SUNNY_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_14_NIGHT_CLOUDY_BIN,
    UI_BUF_I342001_12_WEATHER_WEATHER_15_NIGHT_RAIN_BIN,
};
static void weather_refresh(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_display_ctrl_t displayInfo;
    ute_module_weather_data_t  weather_date;
    uteModuleWeatherGetData(&weather_date);//获取天气状态
    uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
    bool weather_flag = false;
    for(int i=0; i<7; i++)
    {
        if(f_weather->DayWeather[i] != weather_date.DayWeather[i])
        {
            weather_flag = true;
            break;
        }
    }

    if(displayInfo.isFahrenheit != f_weather->isFahrenheit_flag || weather_flag == true)    //是否为华氏度
    {
        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        msg_enqueue(MSG_CHECK_LANGUAGE);//使用切换语言中断，重新刷新数据
    }
}

static void weather_data_Init(void)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    uteModuleSystemtimeGetTime(&time);//获取系统时间
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态

        f_weather->isFahrenheit_flag = displayInfo.isFahrenheit;
        for(int i=0; i<7; i++)
        {
            f_weather->DayWeather[i] = weather_date.DayWeather[i];
        }
    }
}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{

    compo_textbox_t *txt;
    compo_picturebox_t * picbox;
    bool weather_no_data_flag = true;

    ute_module_systemtime_time_t time;
    ute_module_weather_data_t  weather_date;
    ute_display_ctrl_t displayInfo;

    u8 week_day=0;
    u8 get_weather_id[7];//存放一个星期的排序
    char str_buff[50];//用于存放打印数据
    char str_humidity_buff[8];//用于存放湿度打印数据

    memset(get_weather_id,0,sizeof(get_weather_id));
    uteModuleSystemtimeGetTime(&time);//获取系统时间
    const u16 week_sort[7]= {STR_SUNDAY, // 周日
                             STR_MONDAY, // 周一
                             STR_TUESDAY, // 周二
                             STR_WEDNESDAY, // 周三
                             STR_THURSDAY, // 周四
                             STR_FRIDAY, // 周五
                             STR_SATURDAY, // 周六
                            };
    if(uteModuleWeatherGetCurrDay() == time.day) //当前日期是否与系统日期一致
    {
        uteModuleGuiCommonGetDisplayInfo(&displayInfo);//获取温度
        uteModuleWeatherGetData(&weather_date);//获取天气状态
        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(weather_date.DayWeather[i] != WEATHER_TYPE_UNKNOWN)
            {
                weather_no_data_flag = false;
                break;
            }
        }

        for(int i=0; i<7; i++) //获取一周的天气
        {
            if(uteModuleSystemtimeIsNight()) //是否为夜间
            {
                switch(get_weather_id[i])
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

    if(weather_no_data_flag)
    {
        if(func_cb.sta == FUNC_WEATHER)
        {
            f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
            f_weather->no_weather_dada = true;
        }
        ///设置标题栏名字///
        picbox = compo_picturebox_create_for_page(frm,frm->page,UI_BUF_I342001_12_WEATHER_ICON_NODATA_BIN);///背景图片
        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,48/2+66);

        txt = compo_textbox_create_for_page(frm,frm->page,strlen(i18n[STR_NO_GET_WEATHER]));
        compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+24, 220, 34);
        //compo_textbox_set_multiline(txt, true);
        //compo_textbox_set_multiline_drag(txt, true);
        compo_textbox_set(txt,i18n[STR_NO_GET_WEATHER]);

        return frm;
    }

//    picbox = compo_picturebox_create(frm,UI_BUF_I338001_12_WEATHER_WEATHER_BG_BIN);///背景图片
//    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y);

    picbox = compo_picturebox_create(frm, 0);///
    compo_picturebox_set_pos(picbox, 54/2+48, 58/2+91);
    compo_picturebox_set(picbox, future_weather_icon[get_weather_id[0]]);
    memset(str_buff,0,sizeof(str_buff));
    if(weather_no_data_flag)
    {
        snprintf(str_buff,sizeof(str_buff),"--");
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"%d",weather_date.fristDayCurrTemperature);
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(txt, 52/2+124, 54/2+81);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm,1);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_28_BIN);
    compo_textbox_set_pos(txt,28/2+178,47/2+81);
    compo_textbox_set(txt, displayInfo.isFahrenheit ? "℉" : "℃");

    if(!weather_no_data_flag)
    {
        if(!displayInfo.isFahrenheit)      ///是否为华氏度
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℃",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
        else
        {
            snprintf(str_buff,sizeof(str_buff),"%d/%d℉",weather_date.dayTemperatureMin[0],weather_date.dayTemperatureMax[0]);
        }
    }
    else
    {
        snprintf(str_buff,sizeof(str_buff),"--/--℃");
    }
    txt = compo_textbox_create(frm,strlen(str_buff));
    compo_textbox_set_pos(txt,59/2+124,21/2+134);
    compo_textbox_set(txt, str_buff);

    txt = compo_textbox_create(frm,strlen(i18n[weather_list[get_weather_id[0]].txt_num]));
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X,22/2+48);
    compo_textbox_set_forecolor(txt, make_color(152,209,255));
    compo_textbox_set(txt, i18n[weather_list[get_weather_id[0]].txt_num]);
    //紫外线icon
    picbox = compo_picturebox_create(frm,UI_BUF_I342001_12_WEATHER_UV_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 72+18/2, 191+18/2);
    //紫外线data
    txt = compo_textbox_create(frm,strlen(i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]));
    compo_textbox_set_location(txt,65+32/2,170+19/2,100,30);
    compo_textbox_set(txt,i18n[weather_uv[uteModuleWeatherGetUltravioletLevel(weather_date.fristDayWeatherUltraviolet)]]);

    //湿度icon
    picbox = compo_picturebox_create(frm,UI_BUF_I342001_12_WEATHER_HUMIDITY_BIN);///背景图片
    compo_picturebox_set_pos(picbox, 155+12/2,191+16/2);
    //湿度data
    memset(str_humidity_buff,0,sizeof(str_humidity_buff));
    snprintf(str_humidity_buff,sizeof(str_humidity_buff),"%d%%",weather_date.fristDayWeatherHumidity);
    txt = compo_textbox_create(frm,strlen(str_humidity_buff));
    compo_textbox_set_pos(txt,146+30/2,171+19/2);
    compo_textbox_set(txt,str_humidity_buff);

    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_WEATHER]));
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 20+28/2, 100, widget_text_get_max_height());
    compo_textbox_set(txt, i18n[STR_WEATHER]);

    u16 spacing_y=GUI_SCREEN_HEIGHT;
    //第二页
    for(int i=0; i<7; i++)
    {
        if(i==4)
        {
            spacing_y = GUI_SCREEN_HEIGHT*2+29;
        }
        u16 str_id = week_sort[(time.week+i)%7];
        if(i == 0)
        {
            str_id = STR_TO_DAY;
        }
        txt = compo_textbox_create(frm,strlen(i18n[str_id]));
        compo_textbox_set_location(txt, 26, 48+spacing_y, 50,widget_text_get_max_height());
        compo_textbox_set(txt,i18n[str_id]);/// 星期
        compo_textbox_set_align_center(txt,false);

        picbox = compo_picturebox_create(frm,future_weather_icon[get_weather_id[i]]);
        compo_picturebox_set_pos(picbox,22/2+98,53+17/2+spacing_y);
        compo_picturebox_set_size(picbox,22,22);

        if(get_weather_id[i] != WEATHER_TYPE_UNKNOWN)
        {
            snprintf(str_buff, sizeof(str_buff), "%02d/%02d%s",weather_date.dayTemperatureMax[i],weather_date.dayTemperatureMin[i],displayInfo.isFahrenheit ? "℉" : "℃");//一周 小~大 温度
        }
        else
        {
            snprintf(str_buff, sizeof(str_buff), "--/--%s",displayInfo.isFahrenheit ? "℉" : "℃");
        }
        txt = compo_textbox_create(frm,strlen(str_buff));
        compo_textbox_set_location(txt,143,52+spacing_y,80,widget_text_get_max_height());
        compo_textbox_set_align_center(txt,false);
        compo_textbox_set(txt,str_buff);

        compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_location(shape,GUI_SCREEN_CENTER_X,81+spacing_y,186,1);
        compo_shape_set_color(shape,COLOR_GRAY);

        spacing_y += 38;
    }
    // widget_page_set_client(frm->page_body, 0, page_y);
}

#else
#define WHEATHER_CNT                         ((int)((sizeof(weather_list) / sizeof(weather_list[0]))))

typedef struct f_weather_t_
{
    u16 txt_num;
    u32 res_addr;
    u8 mode_num;
    page_tp_move_t *ptm;
    uint16_t DayWeather[7];
    bool isFahrenheit_flag;
    bool no_weather_dada;
} f_weather_t;

enum
{
    CARBOX_ID=1,
    CARBOX_LIST_1ID,
    CARBOX_LIST_2ID,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

static const f_weather_t weather_list[] =
{
    {0},
};

static int16_t page_y = 0;

static void weather_refresh(void)
{

}

static void weather_data_Init(void)
{

}

//创建天气窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_weather_form_create(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//天气功能事件处理
static void func_weather_process(void)
{
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;

    if (!f_weather->no_weather_dada)
    {
        if (f_weather->ptm && uteModuleWeatherGetCurrDay())
        {
            compo_page_move_process(f_weather->ptm);
            s32 dx, dy;
            s16 value = abs_s(f_weather->ptm->move_offset) * 1000 / abs_s(f_weather->ptm->down_spring_offset);
            compo_scroll_t* scroll = compo_getobj_byid(CARBOX_ID);
            compo_scroll_set_pos(scroll, 356, 78 - f_weather->ptm->move_offset);
            compo_scroll_set_value(scroll, value);

            if (f_weather->touch_flag)
            {
                f_weather->tick = tick_get();
                widget_set_visible(scroll->scroll_bg, true);
                widget_set_visible(scroll->scroll_fg, true);
                f_weather->touch_flag = false;
            }
            f_weather->touch_flag = ctp_get_dxy(&dx, &dy);
            if (tick_check_expire(f_weather->tick, 3000))
            {
                widget_set_visible(scroll->scroll_bg, false);
                widget_set_visible(scroll->scroll_fg, false);
            }

            // if (f_weather->exit_flag)
            // {
            //  compo_picturebox_t *picbox = compo_getobj_byid(CARBOX_LIST_1ID);
            //  compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 400 - f_weather->ptm->move_offset);
            //  compo_picturebox_set_visible(picbox, true);
            //  if (!f_weather->touch_flag)
            //  {
            //      compo_picturebox_set_visible(picbox, false);
            //      f_weather->exit_flag = false;
            //  }
            // }
        }
    }
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT \
    || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    compo_page_move_process(f_weather->ptm);
    page_y = compo_page_move_get_offset(f_weather->ptm);

#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;
    compo_page_move_process(f_weather->ptm);
//    printf("page:%d\n",compo_page_move_get_offset(f_weather->ptm));
    page_y = compo_page_move_get_offset(f_weather->ptm);
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    func_weather_move();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    func_process();
}

//天气功能消息处理
static void func_weather_message(size_msg_t msg)
{
    f_weather_t* f_weather = (f_weather_t*)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
            if(!weather_no_data_flag && f_weather->touch_state == TOUCH_FINISH_STATE)
            {
                f_weather->touch_flag = true;
            }
#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT \
      || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT || GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
            if(!f_weather->no_weather_dada)
            {
                compo_page_move_touch_handler(f_weather->ptm);
            }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
            break;
        case MSG_SYS_500MS:
            weather_refresh();
            break;
#if 0//GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case MSG_CTP_SHORT_RIGHT:
            f_weather->exit_flag = true;
            if (func_cb.flag_sort)
            {
                func_switch_prev(false);                    //切到上一个任务
            }
            else /*if (ctp_get_sxy().x <= 32)*/
            {
                func_backing_to();                          //右滑缓慢退出任务
            }
            break;
        case KU_BACK:
            f_weather->exit_flag = true;
            if (func_cb.flag_sort)
            {
                func_switch_to_clock();                     //切换回主时钟
            }
            else if (func_cb.sta == FUNC_CLOCK)
            {
                if(sys_cb.dialplate_btf_ready)
                {
                    sys_cb.dialplate_btf_ready = false;
                }
                func_switch_to_menu();                      //退回到主菜单
            }
            else
            {
                func_back_to();                             //直接退出任务
            }
            break;
#endif
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
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    f_weather->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .page_size = f_weather->pageHeight,
        .page_count = 1,
        .quick_jump_perc = 80,
        //.jump_perc = 20,
        //.up_over_perc = 10,
        //.down_over_perc = 10,
    };
    compo_page_move_init(f_weather->ptm, func_cb.frm_main->page_body, &info);
    weather_data_Init();
    f_weather->touch_flag = true;
#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    f_weather->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
//        .title_used = true,
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = GUI_SCREEN_HEIGHT,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_weather->ptm, func_cb.frm_main->page_body, &info);
    weather_data_Init();
    f_weather->ptm->move_offset = page_y;
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT
    f_weather->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = false,
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 3,
        .quick_jump_perc = GUI_SCREEN_HEIGHT,
        .jump_perc = 20,
        .up_over_perc = 0,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_weather->ptm, func_cb.frm_main->page_body, &info);
    weather_data_Init();
#else
    f_weather->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = GUI_SCREEN_HEIGHT,
        .up_over_perc = 5,
        .down_over_perc = 5,
    };
    compo_page_move_init(f_weather->ptm, func_cb.frm_main->page_body, &info);
    weather_data_Init();
    f_weather->ptm->move_offset = page_y;
#endif
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT ||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    weather_data_Init();
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
}

//退出天气功能
static void func_weather_exit(void)
{
    f_weather_t *f_weather = (f_weather_t *)func_cb.f_cb;
    func_cb.last = FUNC_WEATHER;
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT \
    || GUI_SCREEN_SIZE_240X240RGB_I342001_SUPPORT || GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT
    if(sys_cb.refresh_language_flag == false)
    {
        page_y = 0;
    }
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT||GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
    if(sys_cb.refresh_language_flag == false)
    {
        page_y = 0;
    }
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

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

#endif
