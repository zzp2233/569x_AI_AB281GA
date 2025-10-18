#include "include.h"
#include "func.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_heart.h"
#include "ute_drv_motor.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_SUPPORT

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
//血氧检测状态
enum
{
    BO_STA_IDLE,            //空闲
    BO_STA_TESTING,         //测量中
    BO_STA_SUCCSEE,         //测量成功
    BO_STA_FAIL,            //测量失败
    BO_STA_UNWEAR,          //未佩戴
};
#else
//血氧检测状态
enum
{
    BO_STA_IDLE,            //空闲
    BO_STA_TESTING,         //测量中
    BO_STA_UNWEAR,          //未佩戴
};
#endif
//组件ID
enum
{
    //按键
    COMPO_ID_SURE_BTN = 1,
    COMPO_ID_AGAIN_BTN,
    COMPO_ID_PIC_BG,
    COMPO_ID_TXT_VALUE,
    COMPO_ID_PIC_UNIT,
    COMPO_ID_SAHPE_BG,
    COMPO_ID_MAX_VLA,
    COMPO_ID_MIN_VLA,
    COMPO_ID_PIC_ABOUT,
    COMPO_ID_ABOUT_BTN,
    COMPO_ID_TXT_TEST,
    COMPO_ID_CHART_VALUE,

    COMPO_ID_ABOUT_PIC,
    COMPO_ID_TXT_TMEE_TIP,
    COMPO_ID_TXT_TMEE,
    COMPO_ID_TXT_START,
    COMPO_ID_BTN_START,
    COMPO_ID_PIC_MAX,
    COMPO_ID_PIC_MIN,
    COMPO_ID_PIC_CHART,
    COMPO_ID_TXT_NODATE,

    COMPO_ID_PIC_GIF,
    COMPO_ID_TXT_TIP,

    COMPO_ID_TXT_MES_FAIL,

    COMPO_ID_TXT_WEAR_FAIL,

    COMPO_ID_BTN_RSTART,
};

typedef struct f_blood_oxygen_t_
{
    uint32_t tick;
    uint32_t up_data_tick;
    uint32_t tick_start;
    u8 blood_oxygen_state;
    u8 blood_oxygen_state_refresh;
    u8 pic_type;
    bool pop_disp_flag;
    bool need_auto_test_flag;
    bool Refresh_flag;
    bool Refresh_old_flag;
    page_tp_move_t *ptm;
} f_blood_oxygen_t;

#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    uint8_t oxygen_max;
    uint8_t oxygen_min;

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I335001_7_SPO2_1_SPO2_GIF_52X52_X10_Y62_BIN);
    compo_picturebox_set_pos(picbox, 14+52/2, 54+52/2);
    compo_picturebox_cut(picbox, 0, 20);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,74,65);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 2 );
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,80+txt_leng.wid,80,100, widget_text_get_max_height());
    compo_textbox_set(textbox,"%");
    compo_setid(textbox,COMPO_ID_PIC_UNIT);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I335001_7_SPO2_1_ICON_PLAY_44X44_X186_Y65_01_BIN);///重新测量按钮
    compo_button_set_pos(btn,22+184,22+58);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_6_HEART_1_1_ICON_ABOUT_18X18_X176_Y299_BIN);//////关于
    compo_picturebox_set_pos(picbox,158,5+66);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,158,5+66,30,30);///关于
    compo_setid(btn,COMPO_ID_ABOUT_BTN);
////////////////////////////////////////////////////////////////////

    uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    // compo_textbox_set_location(textbox,14,114,60, widget_text_get_max_height());//有些语言自适应长度处理不好，暂时不做自适应长度
    compo_textbox_set_location(textbox,14,114,50, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_max > 0 && oxygen_max != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最高数据
    // compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    // compo_textbox_set_pos(textbox,20+txt_leng.wid,114);
    compo_textbox_set_pos(textbox,70,114);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MAX_VLA);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    // compo_textbox_set_location(textbox,120,114,60, widget_text_get_max_height());
    compo_textbox_set_location(textbox,120,114,50, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_min > 0 && oxygen_min != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最低数据
    // compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    // compo_textbox_set_pos(textbox,126+txt_leng.wid,114);
    compo_textbox_set_pos(textbox,180,114);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MIN_VLA);

    picbox = compo_picturebox_create(frm, UI_BUF_I335001_7_SPO2_1_NODATE_BG_224X110_X8_Y164_BIN);
    compo_picturebox_set_pos(picbox, 120,148+60);

    uint8_t oxygen_date[24];
    uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+38/2,202,168,71);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART_VALUE);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 4;   ///像素点
    for (int i=0; i<24; i++)
    {
        // oxygen_date[i] =100;
        chart_info.x = i*chart_info.width + i*3;
        chart_info.height = oxygen_date[i]*0.68;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}
// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);
    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    uint8_t oxygen_max;
    uint8_t oxygen_min;
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }

    if(tick_check_expire(f_bo->up_data_tick, 1000))
    {
        f_bo->up_data_tick = tick_get();
        uint8_t oxygen_date[24]= {0};
        uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧
        chart_t chart_info;
        chart_info.y = 0;
        chart_info.width = 4;   ///像素点
        for (int i=0; i<24; i++)
        {
            // oxygen_date[i] =100;
            chart_info.x = i*chart_info.width + i*3;
            chart_info.height = oxygen_date[i]*0.68;///心率数据转换为柱形条显示数据
            compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
        }

        uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);
        memset(txt_buf,0,sizeof(txt_buf));
        if(oxygen_max > 0 && oxygen_max != 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--%%");
        }
        compo_textbox_set(max_value,txt_buf);

        if(oxygen_min > 0 && oxygen_min != 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--%%");
        }
        compo_textbox_set(min_value,txt_buf);


        memset(txt_buf,0,sizeof(txt_buf));
        if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
        }
        compo_textbox_set(textbox,txt_buf);

        area_t txt_leng = widget_text_get_area(textbox->txt);
        compo_textbox_set_pos(pic_uint,80+txt_leng.wid,80);
    }

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==20)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            compo_picturebox_cut(picbox, f_bo->pic_type, 20); ///图片动态显示
            compo_button_set_visible(btn,false);
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        u8 msg_flag = msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
        if(msg_flag==MSGBOX_RES_OK)
        {
            uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
        else
        {
            f_bo->blood_oxygen_state = BO_STA_IDLE;
        }

    }
    else
    {
        compo_button_set_visible(btn,true);
        compo_button_set_bgimg(btn,UI_BUF_I335001_7_SPO2_1_ICON_PLAY_44X44_X186_Y65_01_BIN );
    }
}
//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            if(f_bo->blood_oxygen_state == BO_STA_TESTING)
            {
                uteModuleBloodoxygenStopSingleTesting();
                f_bo->blood_oxygen_state = BO_STA_IDLE;
            }
            else
            {
                uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
                f_bo->blood_oxygen_state = BO_STA_TESTING;
            }

        }
        break;
        case COMPO_ID_ABOUT_BTN:
            uteTaskGuiStartScreen(FUNC_OXYGEN_ABOUT, 0, __func__);
            break;
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
#define DATE_DEBUG 0
extern bool start_testing;
//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    char txt_buf[100];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    bool isWoking = (start_testing && !uteModuleBloodoxygenIsBloodOxygenAutoTesting());

    ute_module_bloodoxygen_test_last_info_t lastInfo;
    uteModuleBloodoxygenGetLastTestInfo(&lastInfo);

    uint8_t oxygen_date[24];
    uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧

    uint8_t oxygen_max;
    uint8_t oxygen_min;
    uint8_t oxygen_val = (lastInfo.lastValue == 255) ? 0 : lastInfo.lastValue;

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);


    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_SPO2_BIN);
    compo_picturebox_set_pos(picbox, 25 + 60 / 2, 79 + 60 / 2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    widget_page_t *widget_page = widget_page_create(frm->page_body);
    widget_set_location(widget_page, GUI_SCREEN_CENTER_X,154,GUI_SCREEN_WIDTH,70);
    widget_page_scale_to(widget_page,GUI_SCREEN_WIDTH * 0.8,70 * 0.8);

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(i18n[STR_LAST_UPDATED]) + 2);
    compo_textbox_set_location(textbox,0,30,230,46);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_visible(textbox,isWoking ? false : (oxygen_val ? true :false));
    compo_textbox_set_alpha(textbox,180);
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%s:",i18n[STR_LAST_UPDATED]);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_TMEE_TIP);
    area_t txt_leng = widget_text_get_area(textbox->txt);

    ute_module_systemtime_time_t sysTime;
    uteModuleSystemtimeGetTime(&sysTime);
    memset(txt_buf,0,sizeof(txt_buf));

    if (lastInfo.lastTestTime.day == sysTime.day)
    {
        if (sysTime.is12HourTime)
        {
            uint8_t hour = lastInfo.lastTestTime.hour;
            hour = hour % 12;
            hour = hour ? hour : 12;
            if (lastInfo.lastTestTime.hour >= 12)
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_PM]);
            }
            else
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_AM]);
            }
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d", lastInfo.lastTestTime.hour, lastInfo.lastTestTime.min);
        }
    }
    else
    {
        if (lastInfo.lastTestTime.year == sysTime.year)
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d/%02d", lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%04d/%02d/%02d", lastInfo.lastTestTime.year, lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
    }

    textbox = compo_textbox_create_for_page(frm,widget_page, strlen(txt_buf));
    compo_textbox_set_visible(textbox,isWoking ? false : (oxygen_val ? true :false));
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,10 + (txt_leng.wid > 230 ? 230 : txt_leng.wid),30,133,46);
    compo_textbox_set_alpha(textbox,180);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_TMEE);

    textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,92,64 + 20);
    compo_textbox_set_align_center(textbox, false);
    if(oxygen_val)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",oxygen_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 2);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,92+txt_leng.wid,94,100, widget_text_get_max_height());
    compo_textbox_set(textbox,"%");
    compo_textbox_set_visible(textbox,isWoking ? false : (oxygen_val ? true :false));
    compo_setid(textbox,COMPO_ID_PIC_UNIT);


    textbox = compo_textbox_create(frm, 30);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,390 + 40 / 2,300,46);
    compo_textbox_set(textbox,i18n[STR_START_MEAR]);
    compo_textbox_set_forecolor(textbox,make_color(0xff,0x45,0x1f));
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_TXT_START);

    compo_button_t *btn = compo_button_create(frm);///重新/测量按钮
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X,392 + 44 / 2, 300, 80);
    compo_button_set_visible(btn, !isWoking);
    compo_setid(btn,COMPO_ID_BTN_START);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,212 + 30 / 2,75 + 30 / 2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_ABOUT_PIC);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,212 + 30 / 2,75 + 30 / 2,80,80);///关于
    compo_button_set_visible(btn, !isWoking);
    compo_setid(btn,COMPO_ID_ABOUT_BTN);
////////////////////////////////////////////////////////////////////

    uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_max > 0 && oxygen_max != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最高数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,75,344,100,56);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_MAX_VLA);


    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_min > 0 && oxygen_min != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最低数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,240,344,100,56);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_visible(textbox,!isWoking);
    compo_setid(textbox,COMPO_ID_MIN_VLA);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_ICON_MAX_1_BIN);
    compo_picturebox_set_pos(picbox, 46 + 26 / 2,358+21/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_MAX);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_ICON_MINI_1_BIN);
    compo_picturebox_set_pos(picbox, 209 + 26 / 2,358+21/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_MIN);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,171 + 170/2);
    compo_picturebox_set_visible(picbox,!isWoking);
    compo_setid(picbox,COMPO_ID_PIC_CHART);

#if DATE_DEBUG
    for (int i=0; i<24; i++)
    {
        oxygen_date[i] = get_random(101);
    }
    oxygen_date[1] = 0;
    oxygen_date[23] = 100;
#endif
    int crc = 0;
    for (int i=0; i<24; i++)
    {
        crc = crc + oxygen_date[i];
    }
    textbox = compo_textbox_create(frm, strlen(i18n[STR_NO_DATA]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,235 + 40 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking ? false : (crc ? false :true));
    compo_textbox_set_alpha(textbox,180);
    compo_textbox_set(textbox,i18n[STR_NO_DATA]);
    compo_setid(textbox,COMPO_ID_TXT_NODATE);

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC_U, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X, 194 + 124 / 2, 312, 124);
    compo_chartbox_set_pixel(chart, 1);
    widget_set_visible(chart->chart,!isWoking);
    compo_setid(chart,COMPO_ID_CHART_VALUE);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*7 + 4;
        chart_info.height = oxygen_date[i]*1.24;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0xff,0x88,0x24));
    }

    //测量中界面
    picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_ICON_GIF_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 137 + 124 / 2);
    compo_picturebox_cut(picbox, 0, 12);
    compo_picturebox_set_visible(picbox,isWoking);
    compo_setid(picbox,COMPO_ID_PIC_GIF);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_MESSAGE1]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,300 + 46 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking);
    compo_textbox_set(textbox,i18n[STR_MESSAGE1]);
    compo_setid(textbox,COMPO_ID_TXT_TEST);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_SOS_QBCSWJZ]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,358 + 46 / 2,300,46);
    compo_textbox_set_visible(textbox,isWoking);
    compo_textbox_set(textbox,i18n[STR_SOS_QBCSWJZ]);
    compo_setid(textbox,COMPO_ID_TXT_TIP);


    //测量失败
    textbox = compo_textbox_create(frm, strlen(i18n[STR_WEAR_CHECK]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,300,150);
    compo_textbox_set_visible(textbox,false);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox,i18n[STR_WEAR_CHECK]);
    compo_setid(textbox,COMPO_ID_TXT_MES_FAIL);

    picbox = compo_picturebox_create(frm,UI_BUF_I341001_7_SPO2_AGAIN01_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,346 + 80 / 2);
    compo_picturebox_set_visible(picbox, false);
    compo_setid(picbox,COMPO_ID_BTN_RSTART);

    //佩戴检测失败
    textbox = compo_textbox_create(frm, strlen(i18n[STR_PLEASE_WEAR]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y,300,100);
    compo_textbox_set_visible(textbox,false);
    compo_textbox_set_multiline(textbox,true);
    compo_textbox_set(textbox,i18n[STR_PLEASE_WEAR]);
    compo_setid(textbox,COMPO_ID_TXT_WEAR_FAIL);

    return frm;
}
// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox_gif = compo_getobj_byid(COMPO_ID_PIC_GIF);
    uint8_t state = f_bo->blood_oxygen_state;
    static uint8_t index_pic = 0;
    switch(state)
    {
        case BO_STA_TESTING:
            if(tick_check_expire(f_bo->tick_start, 100))
            {
                f_bo->tick_start = tick_get();
                index_pic++;
                index_pic > 11 ? (index_pic = 0) : index_pic;
                compo_picturebox_cut(picbox_gif, index_pic, 12);
            }
            break;
        case BO_STA_SUCCSEE:
            break;
        default:
            break;
    }
}

static void func_blood_oxygen_date_refresh(void)
{
    compo_textbox_t *text_value = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_textbox_t *text_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    compo_textbox_t *txt_nodate = compo_getobj_byid(COMPO_ID_TXT_NODATE);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);
    compo_textbox_t *text_tip = compo_getobj_byid(COMPO_ID_TXT_TMEE_TIP);
    compo_textbox_t *text_time = compo_getobj_byid(COMPO_ID_TXT_TMEE);

    ute_module_bloodoxygen_test_last_info_t lastInfo;
    uteModuleBloodoxygenGetLastTestInfo(&lastInfo);

    char txt_buf[100];
    uint8_t oxygen_max;
    uint8_t oxygen_min;
    uint8_t oxygen_val = (lastInfo.lastValue == 255 ) ? 0 : lastInfo.lastValue;

    uint8_t oxygen_date[24];
    uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧
    uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);
    if(oxygen_val)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",oxygen_val);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(text_value,txt_buf);
    area_t txt_leng = widget_text_get_area(text_value->txt);
    compo_textbox_set_location(text_uint,92+txt_leng.wid,94,100, widget_text_get_max_height());
    compo_textbox_set_visible(text_uint,oxygen_val ? true : false);
    compo_textbox_set_visible(text_tip,oxygen_val ? true : false);

    ute_module_systemtime_time_t sysTime;
    uteModuleSystemtimeGetTime(&sysTime);
    if (lastInfo.lastTestTime.day == sysTime.day)
    {
        if (sysTime.is12HourTime)
        {
            uint8_t hour = lastInfo.lastTestTime.hour;
            hour = hour % 12;
            hour = hour ? hour : 12;
            if (lastInfo.lastTestTime.hour >= 12)
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_PM]);
            }
            else
            {
                snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d %s", hour, lastInfo.lastTestTime.min, i18n[STR_AM]);
            }
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d:%02d", lastInfo.lastTestTime.hour, lastInfo.lastTestTime.min);
        }
    }
    else
    {
        if (lastInfo.lastTestTime.year == sysTime.year)
        {
            snprintf(txt_buf, sizeof(txt_buf), "%02d/%02d", lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "%04d/%02d/%02d", lastInfo.lastTestTime.year, lastInfo.lastTestTime.month, lastInfo.lastTestTime.day);
        }
    }

    compo_textbox_set(text_time,txt_buf);
    compo_textbox_set_visible(text_time,oxygen_val ? true : false);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_max > 0 && oxygen_max != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(max_value,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_min > 0 && oxygen_min != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--");
    }
    compo_textbox_set(min_value,txt_buf);

#if DATE_DEBUG
    for (int i=0; i<24; i++)
    {
        oxygen_date[i] = get_random(101);
    }
    oxygen_date[1] = 0;
    oxygen_date[23] = 100;
#endif
    int crc = 0;
    for (int i=0; i<24; i++)
    {
        crc = crc + oxygen_date[i];
    }
    compo_textbox_set_visible(txt_nodate,crc ? false : true);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*7 + 4;
        chart_info.height = oxygen_date[i]*1.25;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0xff,0x88,0x24));
    }

}


static void func_blood_oxygen_layout_init(u8 state)
{
    //f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_textbox_t *text_tip = compo_getobj_byid(COMPO_ID_TXT_TMEE_TIP);
    compo_textbox_t *text_time = compo_getobj_byid(COMPO_ID_TXT_TMEE);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *text_value = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_textbox_t *text_start = compo_getobj_byid(COMPO_ID_TXT_START);

    compo_button_t *btn_start = compo_getobj_byid(COMPO_ID_BTN_START);

    compo_picturebox_t * picbox_about = compo_getobj_byid(COMPO_ID_ABOUT_PIC);
    compo_button_t *btn_about = compo_getobj_byid(COMPO_ID_ABOUT_BTN);

    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    compo_picturebox_t * picbox_max = compo_getobj_byid(COMPO_ID_PIC_MAX);
    compo_picturebox_t * picbox_min = compo_getobj_byid(COMPO_ID_PIC_MIN);
    compo_picturebox_t * picbox_chart = compo_getobj_byid(COMPO_ID_PIC_CHART);
    compo_textbox_t *txt_nodate = compo_getobj_byid(COMPO_ID_TXT_NODATE);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);

    compo_picturebox_t * picbox_gif = compo_getobj_byid(COMPO_ID_PIC_GIF);
    compo_textbox_t *txt_test = compo_getobj_byid(COMPO_ID_TXT_TEST);
    compo_textbox_t *txt_tip = compo_getobj_byid(COMPO_ID_TXT_TIP);

    compo_textbox_t *txt_fail = compo_getobj_byid(COMPO_ID_TXT_MES_FAIL);
    compo_textbox_t *txt_w_fail = compo_getobj_byid(COMPO_ID_TXT_WEAR_FAIL);
    compo_picturebox_t *pic_rstart = compo_getobj_byid(COMPO_ID_BTN_RSTART);

    switch (state)
    {
        case BO_STA_IDLE:
            compo_picturebox_set_visible(picbox,true);
            compo_textbox_set_visible(text_tip,true);
            compo_textbox_set_visible(text_time,true);
            compo_textbox_set_visible(text_value,true);
            compo_textbox_set_visible(pic_uint,true);
            compo_textbox_set(text_start,i18n[STR_START_MEAR]);
            compo_textbox_set_visible(text_start,true);
            compo_button_set_visible(btn_start, true);
            compo_picturebox_set_visible(picbox_about,true);
            compo_button_set_visible(btn_about, true);
            compo_textbox_set_visible(max_value,true);
            compo_textbox_set_visible(min_value,true);
            compo_picturebox_set_visible(picbox_max,true);
            compo_picturebox_set_visible(picbox_min,true);
            compo_picturebox_set_visible(picbox_chart,true);
            compo_textbox_set_visible(txt_nodate,true);
            widget_set_visible(chart->chart, true);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            func_blood_oxygen_date_refresh();
            break;
        case BO_STA_TESTING:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(pic_uint,false);
            compo_textbox_set_visible(text_start,false);
            compo_button_set_visible(btn_start, false);
            compo_picturebox_set_visible(picbox_about,false);
            compo_button_set_visible(btn_about, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_picturebox_set_visible(pic_rstart, false);

            compo_picturebox_set_visible(picbox_gif,true);
            compo_textbox_set_visible(txt_test,true);
            compo_textbox_set_visible(txt_tip,true);
            break;
        case BO_STA_SUCCSEE:
            compo_picturebox_set_visible(picbox,true);
            compo_textbox_set_visible(text_tip,true);
            compo_textbox_set_visible(text_time,true);
            compo_textbox_set_visible(text_value,true);
            compo_textbox_set_visible(pic_uint,true);
            compo_textbox_set(text_start,i18n[STR_AGAIN]);
            compo_textbox_set_visible(text_start,true);
            compo_button_set_visible(btn_start, true);
            compo_picturebox_set_visible(picbox_about,true);
            compo_button_set_visible(btn_about, true);
            compo_textbox_set_visible(max_value,true);
            compo_textbox_set_visible(min_value,true);
            compo_picturebox_set_visible(picbox_max,true);
            compo_picturebox_set_visible(picbox_min,true);
            compo_picturebox_set_visible(picbox_chart,true);
            compo_textbox_set_visible(txt_nodate,true);
            widget_set_visible(chart->chart, true);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_picturebox_set_visible(pic_rstart, false);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            func_blood_oxygen_date_refresh();
            break;
        case BO_STA_FAIL:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(pic_uint,false);
            compo_textbox_set_visible(text_start,false);
            compo_button_set_visible(btn_start, false);
            compo_picturebox_set_visible(picbox_about,false);
            compo_button_set_visible(btn_about, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);

            compo_textbox_set_visible(txt_fail,true);
            compo_textbox_set_visible(txt_w_fail,false);
            compo_picturebox_set_visible(pic_rstart, true);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            break;
        case BO_STA_UNWEAR:
            compo_picturebox_set_visible(picbox,false);
            compo_textbox_set_visible(text_tip,false);
            compo_textbox_set_visible(text_time,false);
            compo_textbox_set_visible(text_value,false);
            compo_textbox_set_visible(pic_uint,false);
            compo_textbox_set_visible(text_start,false);
            compo_button_set_visible(btn_start, false);
            compo_picturebox_set_visible(picbox_about,false);
            compo_button_set_visible(btn_about, false);
            compo_textbox_set_visible(max_value,false);
            compo_textbox_set_visible(min_value,false);
            compo_picturebox_set_visible(picbox_max,false);
            compo_picturebox_set_visible(picbox_min,false);
            compo_picturebox_set_visible(picbox_chart,false);
            compo_textbox_set_visible(txt_nodate,false);
            widget_set_visible(chart->chart, false);

            compo_textbox_set_visible(txt_fail,false);
            compo_textbox_set_visible(txt_w_fail,true);
            compo_picturebox_set_visible(pic_rstart, true);

            compo_picturebox_set_visible(picbox_gif,false);
            compo_textbox_set_visible(txt_test,false);
            compo_textbox_set_visible(txt_tip,false);
            break;
        default :
            break;
    }
}
//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_START:
        {
            if(f_bo->blood_oxygen_state == BO_STA_IDLE || f_bo->blood_oxygen_state == BO_STA_SUCCSEE)
            {
                uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
                f_bo->blood_oxygen_state = BO_STA_TESTING;
                func_blood_oxygen_layout_init(f_bo->blood_oxygen_state);
            }
        }
        break;
        case COMPO_ID_ABOUT_BTN:
            uteTaskGuiStartScreen(FUNC_OXYGEN_ABOUT, 0, __func__);
            break;
    }
}

#elif GUI_SCREEN_SIZE_368X448RGB_I341001_SUPPORT

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    uint8_t oxygen_max;
    uint8_t oxygen_min;

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_SPO2_GIF_74X74_BIN);
    compo_picturebox_set_pos(picbox, 23+74/2, 94+74/2);
    compo_picturebox_cut(picbox, 0, 20);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,111,79);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 2);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,115+txt_leng.wid,116,100, widget_text_get_max_height());
    compo_textbox_set(textbox,"%");
    compo_setid(textbox,COMPO_ID_PIC_UNIT);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I341001_16_PRESSURE_PAUSE_BIN);///重新测量按钮
    compo_button_set_pos(btn,33+274,33+92);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_6_HEART_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,218+12,90+12);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,218+12,90+12,40,40);///关于
    compo_setid(btn,COMPO_ID_ABOUT_BTN);
////////////////////////////////////////////////////////////////////

    uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    // compo_textbox_set_location(textbox,14,114,60, widget_text_get_max_height());//有些语言自适应长度处理不好，暂时不做自适应长度
    compo_textbox_set_location(textbox,26,205,100, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_max > 0 && oxygen_max != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最高数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,34+txt_leng.wid,204);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MAX_VLA);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    // compo_textbox_set_location(textbox,120,114,60, widget_text_get_max_height());
    compo_textbox_set_location(textbox,195,205,100, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_min > 0 && oxygen_min != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最低数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,203+txt_leng.wid,204);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MIN_VLA);

    picbox = compo_picturebox_create(frm, UI_BUF_I341001_7_SPO2_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,254+170/2);

    uint8_t oxygen_date[24];
    uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+26,333,244,104);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART_VALUE);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 5;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*5;
        chart_info.height = oxygen_date[i]*1.04;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}
// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);
    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    uint8_t oxygen_max;
    uint8_t oxygen_min;
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }

    if(tick_check_expire(f_bo->up_data_tick, 1000))
    {
        f_bo->up_data_tick = tick_get();
        uint8_t oxygen_date[24]= {0};
        uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧
        chart_t chart_info;
        chart_info.y = 0;
        chart_info.width = 5;   ///像素点
        for (int i=0; i<24; i++)
        {
            // oxygen_date[i] =100;
            chart_info.x = i*chart_info.width + i*5;
            chart_info.height = oxygen_date[i]*0.96;///心率数据转换为柱形条显示数据
            compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
        }

        uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);
        memset(txt_buf,0,sizeof(txt_buf));
        if(oxygen_max > 0 && oxygen_max != 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--%%");
        }
        compo_textbox_set(max_value,txt_buf);

        if(oxygen_min > 0 && oxygen_min != 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--%%");
        }
        compo_textbox_set(min_value,txt_buf);


        memset(txt_buf,0,sizeof(txt_buf));
        if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
        }
        compo_textbox_set(textbox,txt_buf);

        area_t txt_leng = widget_text_get_area(textbox->txt);
        compo_textbox_set_pos(pic_uint,115+txt_leng.wid,111);
    }

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==20)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            compo_picturebox_cut(picbox, f_bo->pic_type, 20); ///图片动态显示
            compo_button_set_visible(btn,true);
            compo_button_set_bgimg(btn,UI_BUF_I341001_7_SPO2_CONTINUE_BIN);
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        u8 msg_flag = msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
        if(msg_flag==MSGBOX_RES_OK)
        {
            uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
        else
        {
            f_bo->blood_oxygen_state = BO_STA_IDLE;
        }

    }
    else
    {
        compo_button_set_visible(btn,true);
        compo_button_set_bgimg(btn,UI_BUF_I341001_16_PRESSURE_PAUSE_BIN);
    }
}
//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            if(f_bo->blood_oxygen_state == BO_STA_TESTING)
            {
                uteModuleBloodoxygenStopSingleTesting();
                f_bo->blood_oxygen_state = BO_STA_IDLE;
            }
            else
            {
                uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
                f_bo->blood_oxygen_state = BO_STA_TESTING;
            }

        }
        break;
        case COMPO_ID_ABOUT_BTN:
            uteTaskGuiStartScreen(FUNC_OXYGEN_ABOUT, 0, __func__);
            break;
    }
}

#elif GUI_SCREEN_SIZE_320X380RGB_I343001_SUPPORT

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    char txt_buf[20];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    uint8_t oxygen_max;
    uint8_t oxygen_min;

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I343001_7_SPO2_SPO2_GIF_70X70_BIN);
    compo_picturebox_set_pos(picbox, 20+64/2, 82+64/2);
    compo_picturebox_cut(picbox, 0, 20);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_64_BIN);
    compo_textbox_set_pos(textbox,95,81);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 2);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,95+txt_leng.wid,104,100, widget_text_get_max_height());
    compo_textbox_set(textbox,"%");
    compo_setid(textbox,COMPO_ID_PIC_UNIT);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I343001_7_SPO2_PAUSE_BIN);///重新测量按钮
    compo_button_set_pos(btn,59/2+239,59/2+78);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    picbox = compo_picturebox_create(frm, UI_BUF_I343001_6_HEART_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,195+20/2,79+20/2);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,195+20/2,79+20/2,40,40);///关于
    compo_setid(btn,COMPO_ID_ABOUT_BTN);
////////////////////////////////////////////////////////////////////

    uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    // compo_textbox_set_location(textbox,14,114,60, widget_text_get_max_height());//有些语言自适应长度处理不好，暂时不做自适应长度
    compo_textbox_set_location(textbox,23,179,100, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_max > 0 && oxygen_max != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最高数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,23+txt_leng.wid,178);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MAX_VLA);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    // compo_textbox_set_location(textbox,120,114,60, widget_text_get_max_height());
    compo_textbox_set_location(textbox,169,179,100, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_min > 0 && oxygen_min != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最低数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,169+txt_leng.wid,179);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MIN_VLA);

    picbox = compo_picturebox_create(frm, UI_BUF_I343001_7_SPO2_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X,215+148/2);

    uint8_t oxygen_date[24];
    uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, 77+212/2,240+90/2,220,90);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART_VALUE);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 5;   ///像素点
    for (int i=0; i<24; i++)
    {
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = oxygen_date[i]*1.04;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }

    return frm;
}
// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);
    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    uint8_t oxygen_max;
    uint8_t oxygen_min;
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }

    if(tick_check_expire(f_bo->up_data_tick, 1000))
    {
        f_bo->up_data_tick = tick_get();
        uint8_t oxygen_date[24]= {0};
        uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧
        chart_t chart_info;
        chart_info.y = 0;
        chart_info.width = 5;   ///像素点
        for (int i=0; i<24; i++)
        {
//             oxygen_date[i] =100;
            chart_info.x = i*chart_info.width + i*4;
            chart_info.height = oxygen_date[i]*0.96;///心率数据转换为柱形条显示数据
            compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
        }

        uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);
        memset(txt_buf,0,sizeof(txt_buf));
        if(oxygen_max > 0 && oxygen_max != 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--%%");
        }
        compo_textbox_set(max_value,txt_buf);

        if(oxygen_min > 0 && oxygen_min != 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--%%");
        }
        compo_textbox_set(min_value,txt_buf);


        memset(txt_buf,0,sizeof(txt_buf));
        if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
        }
        compo_textbox_set(textbox,txt_buf);

        area_t txt_leng = widget_text_get_area(textbox->txt);
        compo_textbox_set_pos(pic_uint,95+txt_leng.wid,104);
    }

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==20)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            compo_picturebox_cut(picbox, f_bo->pic_type, 20); ///图片动态显示
            compo_button_set_visible(btn,true);
            compo_button_set_bgimg(btn,UI_BUF_I343001_7_SPO2_CONTINUE_BIN);
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        u8 msg_flag = msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
        if(msg_flag==MSGBOX_RES_OK)
        {
            uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
        else
        {
            f_bo->blood_oxygen_state = BO_STA_IDLE;
        }

    }
    else
    {
        compo_button_set_visible(btn,true);
        compo_button_set_bgimg(btn,UI_BUF_I343001_7_SPO2_PAUSE_BIN);
    }
}
//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            if(f_bo->blood_oxygen_state == BO_STA_TESTING)
            {
                uteModuleBloodoxygenStopSingleTesting();
                f_bo->blood_oxygen_state = BO_STA_IDLE;
            }
            else
            {
                uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
                f_bo->blood_oxygen_state = BO_STA_TESTING;
            }

        }
        break;
        case COMPO_ID_ABOUT_BTN:
            uteTaskGuiStartScreen(FUNC_OXYGEN_ABOUT, 0, __func__);
            break;
    }
}


#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    char txt_buf[20];
    uint8_t oxygen_max;
    uint8_t oxygen_min;

    //新建窗体
    compo_form_t *frm = compo_form_create(true);
#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT

    ///设置背景图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I338002_7_SPO2_NO_DATA_BG_BIN);
    compo_picturebox_set_pos(picbox, 180, 180);

    compo_animation_t *animation = compo_animation_create(frm, UI_BUF_I338002_7_SPO2_7_SPO2_BIN);
    compo_animation_set_pos(animation,150,60);  //需要更替为弹窗图标
    compo_animation_set_radix(animation,19);
    compo_animation_set_interval(animation,15);
    compo_setid(animation,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 5);/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,180,45);
    compo_textbox_set_align_center(textbox, false);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_7_SPO2_EXPLAIN_BIN);
    compo_picturebox_set_pos(picbox, 280, 70);
    compo_setid(picbox,COMPO_ID_PIC_ABOUT);
    memset(txt_buf, 0, sizeof(txt_buf));
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xFF)
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
        compo_picturebox_set_visible(picbox, false);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",uteModuleBloodoxygenGetValue());//血氧值
        compo_picturebox_set_visible(picbox, true);
    }
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,180,34/2+307,200,60);///点击测量区域
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, 280, 70, 80, 80);///关于
    compo_setid(btn,COMPO_ID_ABOUT_BTN);
    compo_button_set_visible(btn,false);
    ////////////////////////////////////////////////////////////////////
    uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_max > 0 && oxygen_max != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    textbox = compo_textbox_create(frm, 10);///最高数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,109,275);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MAX_VLA);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_7_SPO2_HIGH_BIN);//////最高标志
    compo_picturebox_set_pos(picbox,89,290);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_min > 0 && oxygen_min != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    textbox = compo_textbox_create(frm, 10);///最低数据
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,230,275);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MIN_VLA);

    picbox = compo_picturebox_create(frm, UI_BUF_I338002_7_SPO2_LOW_BIN);//////最低标志
    compo_picturebox_set_pos(picbox,210,290);

    uint8_t oxygen_date[24];
    uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    // compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,166,312,100);
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X,173,292,102);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART_VALUE);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        // oxygen_date[i] =50;
        chart_info.x = i*chart_info.width + i*6;
        chart_info.height = oxygen_date[i]*0.89;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(0,242,214));
    }

    textbox = compo_textbox_create(frm, strlen(i18n[STR_STATE_TEST]));///最低数据
    compo_textbox_set(textbox,i18n[STR_STATE_TEST]);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,34/2+307,100,34);
    compo_setid(textbox,COMPO_ID_TXT_TEST);

#else

    // //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I338001_7_SPO2_SPO2_GIF_70X70_BIN);
    compo_picturebox_set_pos(picbox, 41+64/2, 82+64/2);
    compo_picturebox_cut(picbox, 0, 20);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );/// 数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,140,90);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 2 );
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,145+txt_leng.wid,98,100, widget_text_get_max_height());
    compo_textbox_set(textbox,"%");
    compo_setid(textbox,COMPO_ID_PIC_UNIT);

    compo_button_t *btn = compo_button_create_by_image(frm,UI_BUF_I338001_7_SPO2_PAUSE_BIN);///重新测量按钮
    compo_button_set_pos(btn,260+59/2,78+59/2);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    picbox = compo_picturebox_create(frm, UI_BUF_I338001_7_SPO2_ABOUT_BIN);//////关于
    compo_picturebox_set_pos(picbox,20/2+220,10+82);

    btn = compo_button_create(frm);
    compo_button_set_location(btn,20/2+220,10+82,40,40);///关于
    compo_setid(btn,COMPO_ID_ABOUT_BTN);
////////////////////////////////////////////////////////////////////

    uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_HIGHEST]) );///最高
    compo_textbox_set_location(textbox,44,186,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_HIGHEST]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_max > 0 && oxygen_max != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最高数据
    // compo_setid(textbox,COMPO_ID_HEART_MAX_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,49+txt_leng.wid,186);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MAX_VLA);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_LOWSET]) );///最低
    compo_textbox_set_location(textbox,190,186,60, widget_text_get_max_height());
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set(textbox,i18n[STR_LOWSET]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_min > 0 && oxygen_min != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    txt_leng = widget_text_get_area(textbox->txt);
    textbox = compo_textbox_create(frm, 10);///最低数据
    // compo_setid(textbox,COMPO_ID_HEART_MIN_TXT);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,195+txt_leng.wid,186);
    compo_textbox_set_align_center(textbox, false);
    compo_setid(textbox,COMPO_ID_MIN_VLA);

    picbox = compo_picturebox_create(frm, UI_BUF_I338001_7_SPO2_DATE_BG_BIN);
    compo_picturebox_set_pos(picbox, 180,149/2+214);

    uint8_t oxygen_date[24];
    uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧

    compo_chartbox_t* chart = compo_chartbox_create(frm, CHART_TYPE_BAR_ARC, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+18,237+93/2,213,93);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart,COMPO_ID_CHART_VALUE);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = 5;   ///像素点
    for (int i=0; i<24; i++)
    {
        // oxygen_date[i] =200;
        chart_info.x = i*chart_info.width + i*4;
        chart_info.height = oxygen_date[i]*0.465;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }
#endif
    return frm;
}
// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_picturebox_t *picbox = compo_getobj_byid(COMPO_ID_PIC_ABOUT);
    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    compo_textbox_t *state_test = compo_getobj_byid(COMPO_ID_TXT_TEST);
    compo_animation_t *animation = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_button_t *btn_about = compo_getobj_byid(COMPO_ID_ABOUT_BTN);

    uint8_t oxygen_max;
    uint8_t oxygen_min;
    char txt_buf[20] = {0};

    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }

    compo_picturebox_set_pos(picbox, 280, 70);
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xFF)
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
        compo_picturebox_set_visible(picbox, false);
        compo_button_set_visible(btn_about, false);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",uteModuleBloodoxygenGetValue());//血氧值
        compo_picturebox_set_visible(picbox, true);
        compo_button_set_visible(btn_about, true);
    }
    compo_textbox_set(textbox,txt_buf);

    uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_max > 0 && oxygen_max != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    compo_textbox_set(max_value,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    if(oxygen_min > 0 && oxygen_min != 255)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"--%%");
    }
    compo_textbox_set(min_value,txt_buf);

    if(f_bo->blood_oxygen_state != f_bo->blood_oxygen_state_refresh)
    {
        f_bo->blood_oxygen_state_refresh = f_bo->blood_oxygen_state;

        if(f_bo->blood_oxygen_state == BO_STA_TESTING)
        {
            compo_animation_set_interval(animation,15);
            compo_textbox_set(state_test,i18n[STR_TESTING]);
        }
        else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
        {
            msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_NONE);
            f_bo->blood_oxygen_state = BO_STA_IDLE;
        }
        else
        {
            compo_animation_set_interval(animation,0);
            compo_textbox_set(state_test,i18n[STR_STATE_TEST]);
        }
    }


#else
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    compo_chartbox_t* chart = compo_getobj_byid(COMPO_ID_CHART_VALUE);
    compo_textbox_t *max_value = compo_getobj_byid(COMPO_ID_MAX_VLA);
    compo_textbox_t *min_value = compo_getobj_byid(COMPO_ID_MIN_VLA);
    uint8_t oxygen_max;
    uint8_t oxygen_min;
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }

    if(tick_check_expire(f_bo->up_data_tick, 1000))
    {
        f_bo->up_data_tick = tick_get();
        uint8_t oxygen_date[24]= {0};
        uteModuleBloodoxygenGetTodayHistoryData(oxygen_date,24);///获取一天的血氧
        // chart_t chart_info;
        // chart_info.y = 0;
        // chart_info.width = 5;   ///像素点
        // for (int i=0; i<24; i++)
        // {
        //     // heart_date[i] =200;
        //     chart_info.x = i*chart_info.width + i*4;
        //     chart_info.height = oxygen_date[i]*0.465;///心率数据转换为柱形条显示数据
        //     compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
        // }

        uteModuleBloodoxygenGetMinMaxValue(&oxygen_min,&oxygen_max);
        memset(txt_buf,0,sizeof(txt_buf));
        if(oxygen_max > 0 && oxygen_max != 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_max);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--%%");
        }
        compo_textbox_set(max_value,txt_buf);

        if(oxygen_min > 0 && oxygen_min != 255)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",oxygen_min);
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"--%%");
        }
        compo_textbox_set(min_value,txt_buf);


        memset(txt_buf,0,sizeof(txt_buf));
        if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
        {
            snprintf(txt_buf,sizeof(txt_buf),"%s","--");
        }
        else
        {
            snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
        }
        compo_textbox_set(textbox,txt_buf);

        area_t txt_leng = widget_text_get_area(textbox->txt);
        compo_textbox_set_pos(pic_uint,145+txt_leng.wid,98);
    }

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==20)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            compo_picturebox_cut(picbox, f_bo->pic_type, 20); ///图片动态显示
            compo_button_set_bgimg(btn,UI_BUF_I338001_7_SPO2_CONTINUE_BIN );
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        // msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_SURE, MSGBOX_MSG_TYPE_NONE);
        f_bo->blood_oxygen_state = BO_STA_IDLE;
    }
    else
    {
        compo_button_set_bgimg(btn,UI_BUF_I338001_7_SPO2_PAUSE_BIN );
    }
#endif
}
//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            printf("enter COMPO_ID_AGAIN_BTN\r\n");
            if(f_bo->blood_oxygen_state == BO_STA_TESTING)
            {
                uteModuleBloodoxygenStopSingleTesting();
                f_bo->blood_oxygen_state = BO_STA_IDLE;
            }
            else
            {
                uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
                f_bo->blood_oxygen_state = BO_STA_TESTING;
            }

        }
        break;
        case COMPO_ID_ABOUT_BTN:
#if UTE_MODULE_SCREENS_BLOOD_OXYGEN_INFO_SUPPORT
            uteTaskGuiStartScreen(FUNC_OXYGEN_ABOUT, 0, __func__);
#endif // UTE_MODULE_SCREENS_BLOOD_OXYGEN_INFO_SUPPORT
            break;
    }
}

#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

#define CONTROL_Y  8
//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_BLOODOXYGEN_GIF_BIN);
    compo_picturebox_cut(picbox, 0, 16);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///血氧数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_pos(textbox,12,190+18-11);
    char txt_buf[5];
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    picbox = compo_picturebox_create(frm, UI_BUF_I330001_BLOODOXYGEN_PERCENT_BIN);///  % 图片
    compo_picturebox_set_pos(picbox, 19/2+66+txt_leng.wid-42,219+19/2-3);
    compo_setid(picbox,COMPO_ID_PIC_UNIT);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));///血氧和饱和度
    compo_textbox_set_align_center(textbox, false);
    compo_textbox_set_location(textbox,12,245,140,40);
    compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    // compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_BLOODOXYGEN_ICON_DETECTION_BIN);///重新测量按钮
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.5,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.5-CONTROL_Y);
//    compo_button_set_visible(btn, false);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    return frm;
}

// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }


    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    compo_picturebox_set_pos(pic_uint,19/2+66+txt_leng.wid-42,219+19/2-3);

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==16)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            compo_picturebox_cut(picbox, f_bo->pic_type, 16); ///图片动态显示
            compo_button_set_visible(btn, false);
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_SURE, MSGBOX_MSG_TYPE_NONE);
        f_bo->blood_oxygen_state = BO_STA_IDLE;
    }
    else
    {
        compo_picturebox_cut(picbox, 0, 16); ///图片动态显示
        compo_button_set_visible(btn, true);
    }


}

//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
        break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define CONTROL_Y  8

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_BLOODOXYGEN_GIF_BIN);
    compo_picturebox_cut(picbox, 0, 16);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 113/2+36+15);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///血氧数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,77/2+153);
    char txt_buf[5];
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    picbox = compo_picturebox_create(frm, UI_BUF_I332001_BLOODOXYGEN_PERCENT_BIN);///  % 图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+txt_leng.wid/2+18,77/2+77/4+155);
    compo_setid(picbox,COMPO_ID_PIC_UNIT);

    // textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));///血氧和饱和度
    // compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,22/2+234,140,24);
    // compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);
    // compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    // compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I332001_BLOODOXYGEN_CARD_BIN);///重新测量按钮
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,64/2+266);
//    compo_button_set_visible(btn, false);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_START_MEASURING])+strlen(i18n[STR_MEASURING]));///测量中...
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,64/2+266,200,64);
    compo_textbox_set(textbox,i18n[STR_START_MEASURING]);
    compo_setid(textbox,COMPO_ID_TXT_TEST);

    return frm;
}

// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    compo_textbox_t *textbox_gauge = compo_getobj_byid(COMPO_ID_TXT_TEST);
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }


    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    compo_picturebox_set_pos(pic_uint, GUI_SCREEN_CENTER_X+txt_leng.wid/2+20,77/2+77/4+155);

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==16)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            f_bo->Refresh_flag = true;
            compo_picturebox_cut(picbox, f_bo->pic_type, 16); ///图片动态显示
            if(f_bo->Refresh_flag != f_bo->Refresh_old_flag)
            {
                f_bo->Refresh_old_flag = f_bo->Refresh_flag ;
                compo_button_set_visible(btn, false);
                compo_textbox_set(textbox_gauge,i18n[STR_MEASURING]);
            }
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_SURE, MSGBOX_MSG_TYPE_NONE);
        f_bo->blood_oxygen_state = BO_STA_IDLE;
    }
    else
    {
        compo_picturebox_cut(picbox, 0, 16); ///图片动态显示
        f_bo->Refresh_flag = false;
        if(f_bo->Refresh_flag != f_bo->Refresh_old_flag)
        {
            f_bo->Refresh_old_flag = f_bo->Refresh_flag ;
            compo_button_set_visible(btn, true);
            compo_textbox_set(textbox_gauge,i18n[STR_START_MEASURING]);
        }

    }


}

//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
        break;
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT
#define CONTROL_Y  8

//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I340001_BLOODOXYGEN_GIF_BIN);
    compo_picturebox_cut(picbox, 0, 16);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 113/2+36+15);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///血氧数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_54_BIN);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,77/2+153);
    char txt_buf[5];
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    picbox = compo_picturebox_create(frm, UI_BUF_I340001_BLOODOXYGEN_PERCENT_BIN);///  % 图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X+txt_leng.wid/2+18,77/2+77/4+155);
    compo_setid(picbox,COMPO_ID_PIC_UNIT);

    // textbox = compo_textbox_create(frm, strlen(i18n[STR_BLOOD_OXYGEN_UINT]));///血氧和饱和度
    // compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,22/2+234,140,24);
    // compo_textbox_set(textbox,i18n[STR_BLOOD_OXYGEN_UINT]);
    // compo_textbox_set_forecolor(textbox, COLOR_GRAY);
    // compo_textbox_set_autoroll_mode(textbox, TEXT_AUTOROLL_MODE_SROLL_CIRC);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I340001_BLOODOXYGEN_CARD_BIN);///重新测量按钮
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X,64/2+266);
//    compo_button_set_visible(btn, false);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_START_MEASURING])+strlen(i18n[STR_MEASURING]));///测量中...
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,64/2+266,200,64);
    compo_textbox_set(textbox,i18n[STR_START_MEASURING]);
    compo_setid(textbox,COMPO_ID_TXT_TEST);

    return frm;
}

// 界面刷新
static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_picturebox_t * pic_uint = compo_getobj_byid(COMPO_ID_PIC_UNIT);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    compo_textbox_t *textbox_gauge = compo_getobj_byid(COMPO_ID_TXT_TEST);
    char txt_buf[20];
    // 仅进入界面时自动发起一次测量
    if(f_bo->need_auto_test_flag && !uteModuleBloodoxygenIsTesting() && f_bo->need_auto_test_flag == true && f_bo->tick_start <= tick_get()-500)
    {
        uteModuleBloodoxygenStartSingleTesting();
        f_bo->need_auto_test_flag = false;
    }


    memset(txt_buf,0,sizeof(txt_buf));
    if(uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff)
    {
        snprintf(txt_buf,sizeof(txt_buf),"%s","--");
    }
    else
    {
        snprintf(txt_buf,sizeof(txt_buf),"%d",uteModuleBloodoxygenGetValue());//血氧值
    }
    compo_textbox_set(textbox,txt_buf);

    area_t txt_leng = widget_text_get_area(textbox->txt);
    compo_picturebox_set_pos(pic_uint, GUI_SCREEN_CENTER_X+txt_leng.wid/2+20,77/2+77/4+155);

    if(f_bo->blood_oxygen_state == BO_STA_TESTING) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();
            if(++f_bo->pic_type==16)
            {
                f_bo->pic_type=0;
            }
            f_bo->need_auto_test_flag = false;
            f_bo->Refresh_flag = true;
            compo_picturebox_cut(picbox, f_bo->pic_type, 16); ///图片动态显示
            if(f_bo->Refresh_flag != f_bo->Refresh_old_flag)
            {
                f_bo->Refresh_old_flag = f_bo->Refresh_flag ;
                compo_button_set_visible(btn, false);
                compo_textbox_set(textbox_gauge,i18n[STR_MEASURING]);
            }
        }
    }
    else if (f_bo->blood_oxygen_state == BO_STA_UNWEAR)
    {
        msgbox((char *)i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_SURE, MSGBOX_MSG_TYPE_NONE);
        f_bo->blood_oxygen_state = BO_STA_IDLE;
    }
    else
    {
        compo_picturebox_cut(picbox, 0, 16); ///图片动态显示
        f_bo->Refresh_flag = false;
        if(f_bo->Refresh_flag != f_bo->Refresh_old_flag)
        {
            f_bo->Refresh_old_flag = f_bo->Refresh_flag ;
            compo_button_set_visible(btn, true);
            compo_textbox_set(textbox_gauge,i18n[STR_START_MEASURING]);
        }

    }


}

//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_AGAIN_BTN:
        {
            uteModuleBloodoxygenStartSingleTesting(); /// 开启测试
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
        break;
    }
}

#else
compo_form_t *func_blood_oxygen_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    return;
}

static void func_blood_oxygen_disp_handle(void)
{}
//单击按钮
static void func_blood_oxygen_button_click(void)
{}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

#define TIME_OUT (60)
static int time_count = 0;
//血氧界面刷新、事件处理
static void func_blood_oxygen_process(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    if (f_bo == NULL)
    {
        return;
    }

    if (uteModuleBloodoxygenIsWear() && uteModuleBloodoxygenIsTesting())
    {
        reset_guioff_delay();
    }

    if (f_bo->blood_oxygen_state == BO_STA_TESTING)
    {
        if (!uteModuleBloodoxygenIsWear() && !uteModuleBloodoxygenIsTesting())
        {
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
            u8 msg_flag = msgbox(i18n[STR_PLEASE_WEAR], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
            if (msg_flag == MSGBOX_MODE_BTN_OK)
            {
                uteModuleBloodoxygenStartSingleTesting();
                f_bo->blood_oxygen_state = BO_STA_TESTING;
            }
            else
            {
                f_bo->blood_oxygen_state = BO_STA_IDLE;
            }
            func_blood_oxygen_layout_init(f_bo->blood_oxygen_state);
        }
        else if (uteModuleBloodoxygenGetTestingSecond() >= UTE_MODULE_BLOODOXYGEN_TEST_TIMEOUT_SECOND && (uteModuleBloodoxygenGetValue() == 0 || uteModuleBloodoxygenGetValue() == 0xff))
        {
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
            u8 msg_flag = msgbox(i18n[STR_WEAR_CHECK], NULL, NULL, MSGBOX_MODE_BTN_OK, MSGBOX_MSG_TYPE_NONE);
            if (msg_flag == MSGBOX_MODE_BTN_OK)
            {
                uteModuleBloodoxygenStartSingleTesting();
                f_bo->blood_oxygen_state = BO_STA_TESTING;
            }
            else
            {
                f_bo->blood_oxygen_state = BO_STA_IDLE;
            }
            func_blood_oxygen_layout_init(f_bo->blood_oxygen_state);
        }
        else if (!uteModuleBloodoxygenIsTesting())
        {
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
            if (uteModuleBloodoxygenGetValue() > 0 && uteModuleBloodoxygenGetValue() < 0xff)
            {
                f_bo->blood_oxygen_state = BO_STA_SUCCSEE;
            }
            else
            {
                f_bo->blood_oxygen_state = BO_STA_IDLE;
            }
            func_blood_oxygen_layout_init(f_bo->blood_oxygen_state);
        }
    }
    else
    {
        if (uteModuleBloodoxygenIsTesting() && !uteModuleBloodoxygenIsBloodOxygenAutoTesting() && f_bo->blood_oxygen_state != BO_STA_TESTING)
        {
            f_bo->blood_oxygen_state = BO_STA_TESTING;
            func_blood_oxygen_layout_init(f_bo->blood_oxygen_state);
        }
    }

    if (f_bo->ptm)
    {
        compo_page_move_process(f_bo->ptm);
    }
    func_blood_oxygen_disp_handle();
    func_process();
}
#else
//血氧界面刷新、事件处理
static void func_blood_oxygen_process(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    if(f_bo == NULL)
    {
        return;
    }
    if(uteModuleBloodoxygenIsWear() && uteModuleBloodoxygenIsTesting())
    {
        reset_guioff_delay();
    }
    if(f_bo->blood_oxygen_state == BO_STA_TESTING)
    {
        if (!uteModuleBloodoxygenIsWear() && !uteModuleBloodoxygenIsTesting())
        {
            f_bo->blood_oxygen_state = BO_STA_UNWEAR;
        }
        else if (!uteModuleBloodoxygenIsTesting())
        {
            f_bo->blood_oxygen_state = BO_STA_IDLE;
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
        }
    }
    else
    {
        if (uteModuleBloodoxygenIsTesting())
        {
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
    }
    if(f_bo->ptm)
    {
        compo_page_move_process(f_bo->ptm);
    }
    func_blood_oxygen_disp_handle();
    func_process();
}
#endif
//血氧功能消息处理
static void func_blood_oxygen_message(size_msg_t msg)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            if(f_bo->ptm)
            {
                compo_page_move_touch_handler(f_bo->ptm);
            }
            break;
        case MSG_CTP_CLICK:
            func_blood_oxygen_button_click();
            break;
        case MSG_SYS_1S:
            break;
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case MSG_CTP_SHORT_RIGHT:
        case KU_BACK:
        case KU_DELAY_BACK:
        {
            if(f_bo->blood_oxygen_state == BO_STA_TESTING)
            {
                uteModuleBloodoxygenStopSingleTesting();
                f_bo->blood_oxygen_state = BO_STA_IDLE;
                func_blood_oxygen_layout_init(f_bo->blood_oxygen_state);
            }
            else
            {
                func_message(msg);
            }
        }
        break;
#endif
        default:
            func_message(msg);
            break;
    }
}

//进入血氧功能
static void func_blood_oxygen_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_blood_oxygen_t));
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    f_bo->blood_oxygen_state = BO_STA_IDLE;
    f_bo->need_auto_test_flag = true;
    func_cb.frm_main = func_blood_oxygen_form_create();
    f_bo->tick_start = tick_get();
    tft_set_temode(0);
#if GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

#if GUI_SCREEN_SIZE_360X360RGB_I338002_SUPPORT

#else
    f_bo->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = 436,
        .page_count = 1,
        .quick_jump_perc = 80,
    };
    compo_page_move_init(f_bo->ptm, func_cb.frm_main->page_body, &info);
    func_cb.flag_animation = false;
#endif
#endif

#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
    if(start_testing)//小组件主动测量
    {
        start_testing = false;
        if(!uteModuleBloodoxygenIsBloodOxygenAutoTesting())
        {
            f_bo->blood_oxygen_state = BO_STA_TESTING;
        }
    }
#endif

}

//退出血氧功能
static void func_blood_oxygen_exit(void)
{
    if (uteModuleBloodoxygenIsTesting() && !uteModuleBloodoxygenIsBloodOxygenAutoTesting())
    {
        uteModuleBloodoxygenStopSingleTesting();///关闭测试
    }
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    func_cb.last = FUNC_BLOOD_OXYGEN;
    tft_set_temode(DEFAULT_TE_MODE);
    if (f_bo->ptm)
    {
        func_free(f_bo->ptm);
    }
}

//血氧功能
void func_blood_oxygen(void)
{
    printf("%s\n", __func__);
    func_blood_oxygen_enter();
    while (func_cb.sta == FUNC_BLOOD_OXYGEN)
    {
        func_blood_oxygen_process();
        func_blood_oxygen_message(msg_dequeue());
    }
    func_blood_oxygen_exit();
}

#endif
