#include "include.h"
#include "func.h"
#include "ute_module_sleep.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FINT_HEIGHT 24
#define GUI_PAGE_HEAD_HEIGHT            (GUI_SCREEN_HEIGHT / 8)
#define GUI_PAGE_BODY_HEIGHT            (GUI_SCREEN_HEIGHT - GUI_PAGE_HEAD_HEIGHT)
#define GUI_PAGE_BODY_CENTER_Y          (GUI_PAGE_HEAD_HEIGHT + GUI_PAGE_BODY_HEIGHT / 2)
#define GUI_PAGE_TITLE_WIDTH            (GUI_SCREEN_WIDTH * 2 / 5)
#define GUI_PAGE_TIME_WIDTH             (GUI_SCREEN_WIDTH - FORM_TITLE_RIGHT - GUI_SCREEN_WIDTH/12)//(GUI_SCREEN_WIDTH / 3)
#define CONTROL_Y  16

enum
{
    COMPO_ID_TEXT_DEEP_TIME_HOUR = 1,
    COMPO_ID_TEXT_DEEP_TIME_MIN,
    COMPO_ID_TEXT_LIGHT_TIME_HOUR,
    COMPO_ID_TEXT_LIGHT_TIME_MIN,

    COMPO_ID_CHART_SHALLOW,
    COMPO_ID_CHART_DEEP,
    COMPO_ID_TITLE_TIME,
    COMPO_ID_TXT_TIME,
};

enum
{
    CHART_ANIM_STA_IDLE,
    CHART_ANIM_STA_START,
    CHART_ANIM_STA_END,

};

typedef struct f_sleep_t_
{
    page_tp_move_t *ptm;

    u8 anim_sta;
    u16 data[7];
} f_sleep_t;


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//创建睡眠窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_sleep_form_create(void)
{
    compo_textbox_t * txt;
    compo_picturebox_t *pic;
    char buf[50];

    ute_module_sleep_display_data_t * sleep_data = (ute_module_sleep_display_data_t *)ab_zalloc(sizeof(ute_module_sleep_display_data_t));
    uteModuleSleepGetCurrDayDataDisplay(sleep_data);

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
//    compo_form_set_title(frm, i18n[STR_SLEEP]);
//    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_BG_BIN);///* 背景图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    ///设置标题栏名字///
    txt = compo_textbox_create(frm,strlen(i18n[STR_SLEEP]));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_24_BIN);
    compo_textbox_set(txt, i18n[STR_SLEEP]);
    compo_textbox_set_location(txt,GUI_SCREEN_WIDTH/12,GUI_SCREEN_HEIGHT/21.8,GUI_SCREEN_WIDTH * 2 / 5,GUI_SCREEN_HEIGHT/(284/28));
     compo_textbox_set(txt, i18n[STR_SLEEP]);
    compo_textbox_set_align_center(txt, false);

    ///设置标题栏时间///
//    compo_textbox_t* time_title = compo_textbox_create(frm, 32);
//    compo_textbox_set_align_center(time_title, false);
//    compo_textbox_set_location(time_title, 168, 15, 52, 23);
//    compo_setid(time_title, COMPO_ID_TXT_TIME);
//    compo_bonddata(time_title, COMPO_BOND_HOURMIN_TXT);
sleep_data->totalSleepMin = 138;
    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin/60);///* 总睡眠小时*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--");///* 总睡眠小时*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.7-CONTROL_Y);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_HOUR]));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.7-4,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.5-FINT_HEIGHT/2-CONTROL_Y,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_HOUR]);

    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d", sleep_data->totalSleepMin%60);///* 总睡眠分钟*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--");///* 总睡眠分钟*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_48_BIN);
    compo_textbox_set_pos(txt,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/7-10,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.7-CONTROL_Y);
    compo_textbox_set(txt, buf);

    txt = compo_textbox_create(frm,strlen(i18n[STR_MIN]));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2.4-14,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.5-FINT_HEIGHT/2-CONTROL_Y,FINT_HEIGHT,FINT_HEIGHT);
    compo_textbox_set_autoroll_mode(txt, 0);
    compo_textbox_set(txt, i18n[STR_MIN]);

    memset(buf,0,sizeof(buf));
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d - %02d:%02d", sleep_data->fallAsSleepTime.hour,sleep_data->fallAsSleepTime.min,sleep_data->getUpSleepTime.hour,sleep_data->getUpSleepTime.min);///* 睡-起 点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:-- - --:--");///* 睡-起 点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.7,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2-CONTROL_Y);
    compo_textbox_set(txt, buf);

    ///* 第二页*/
    if(sleep_data->totalSleepMin == 0) ///是否有睡眠时长
    {
        txt = compo_textbox_create(frm,strlen(i18n[STR_NO_DATA]));
        compo_textbox_set(txt, i18n[STR_NO_DATA]);
        compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y/3+GUI_SCREEN_HEIGHT);
    }

//    area_t pic_size = gui_image_get_size(UI_BUF_SLEEP_SLEEP_DATE_BG_BIN);
//    compo_form_add_image(frm, UI_BUF_SLEEP_SLEEP_DATE_BG_BIN, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4);///* 背景图*/
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->fallAsSleepTime.hour,sleep_data->fallAsSleepTime.min);///* 睡眠初始点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:--");///* 睡眠结束点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X-236/2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+108/2-FINT_HEIGHT);
    compo_textbox_set(txt, buf);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);

    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d:%02d", sleep_data->getUpSleepTime.hour,sleep_data->getUpSleepTime.min);///* 睡眠结束点*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--:--");///* 睡眠结束点*/
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X+236/2-FINT_HEIGHT/2*strlen(buf), GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/4+108/2-FINT_HEIGHT);
    compo_textbox_set(txt, buf);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_DOT1_BIN);///* 深度睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.1, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_DOT2_BIN);///* 浅度睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/10, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_DOT3_BIN);///* 清醒睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/10, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*3);

    pic = compo_picturebox_create(frm, UI_BUF_I330001_SLEEP_DOT4_BIN);///* 快速眼动睡眠图*/
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.1, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*3);

    txt = compo_textbox_create(frm,strlen(i18n[STR_DEEP_SLEEP]));///* 深睡*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8-widget_text_get_height()/2,86,32);
    compo_textbox_set(txt, i18n[STR_DEEP_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x69, 0x7e, 0xff));

    txt = compo_textbox_create(frm,strlen(i18n[STR_LIGHT_SLEEP]));///* 浅睡*
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8-widget_text_get_height()/2,86,32);
    compo_textbox_set(txt, i18n[STR_LIGHT_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x36, 0xb7, 0xff));

    txt = compo_textbox_create(frm,strlen(i18n[STR_ALWAKE_SLEEP]));///* 清醒*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*3-widget_text_get_height()/2,86,32);
    compo_textbox_set(txt, i18n[STR_ALWAKE_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0xff, 0x87, 0x0f));

    txt = compo_textbox_create(frm,strlen(i18n[STR_EYE_MOVE_SLEEP]));///* 快速眼动*/
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*3-widget_text_get_height()/2,86,32);
    compo_textbox_set(txt, i18n[STR_EYE_MOVE_SLEEP]);
    compo_textbox_set_forecolor(txt, make_color(0x00, 0xf7, 0xd6));
//////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s %02d%s",sleep_data->deepSleepMin/60,i18n[STR_HOUR], sleep_data->deepSleepMin%60,i18n[STR_MIN]);///* 深睡数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()-widget_text_get_height()/3,86,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s %02d%s",sleep_data->lightSleepMin/60,i18n[STR_HOUR], sleep_data->lightSleepMin%60,i18n[STR_MIN]);///* 浅睡数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()-widget_text_get_height()/3,86,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s %02d%s",sleep_data->wakeSleepMin/60,i18n[STR_HOUR], sleep_data->wakeSleepMin%60,i18n[STR_MIN]);///* 清醒数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/5, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*4-widget_text_get_height()/3,86,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    if(sleep_data->totalSleepMin) ///是否有睡眠时长
    {
        snprintf(buf, sizeof(buf), "%02d%s %02d%s",sleep_data->RemSleepMin/60,i18n[STR_HOUR], sleep_data->RemSleepMin%60,i18n[STR_MIN]);///* 快速眼动数据*/
    }
    else
    {
        snprintf(buf, sizeof(buf), "--%s --%s",i18n[STR_HOUR],i18n[STR_MIN]);
    }
    txt = compo_textbox_create(frm,strlen(buf));
    compo_textbox_set_align_center(txt, false);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.2, GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/1.8+widget_text_get_height()*4-widget_text_get_height()/3,86,36);
    compo_textbox_set(txt, buf);
//////////////////////////////////////////////////////////////////////////////////////////////////////
    uint16_t width = 236;    // 总宽度
    uint8_t fillRect_h = 21; // 绘制单个高度
    uint16_t startX = 6;     // 相对坐标
    uint16_t fillRect_y = GUI_SCREEN_HEIGHT + GUI_SCREEN_HEIGHT / 4.1;

    compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, fillRect_y + fillRect_h+1, GUI_SCREEN_WIDTH-10, 1);
    compo_shape_set_color(shape, make_color(0X3D,0X3D,0X3D));

    if(sleep_data->totalSleepMin)///是否有睡眠时长
    {
        compo_shape_t *shape;
        uint16_t realTimeInterval;
        uint16_t startTime = sleep_data->fallAsSleepTime.hour * 60 + sleep_data->fallAsSleepTime.min; // 终止时间23:00
        uint16_t endTime = sleep_data->getUpSleepTime.hour * 60 + sleep_data->getUpSleepTime.min;     // 起始时间8:00
        uint16_t timeInterval = (endTime + 24 * 60 - startTime) % (24 * 60);                          // 总时间间隔（分钟）
        uint16_t sumInterval = 0;                                                                     // 累积间隔
        realTimeInterval = ((sleep_data->sleep_record[0].startTime.hour * 60 +
                             sleep_data->sleep_record[0].startTime.min) +
                            24 * 60 - startTime) %
                           (24 * 60); // 实时间隔
        if (timeInterval == 0)
            timeInterval = 24 * 60;
        uint16_t fillRect_x = (width * realTimeInterval) / timeInterval;                   // 绘制的起始点
        uint16_t fillRect_w = (width * sleep_data->sleep_record[0].period) / timeInterval; // 绘制图的总宽度
        if (fillRect_w < 1 && sleep_data->sleep_record[0].period != 0)                     // 起始点宽度不为0的数据最少显示1像素宽
        {
            fillRect_w = 1;
        }

        for (uint16_t i = 0; i < sleep_data->recordCnt; i++)
        {
            shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE); /// 创建显示块矩形
            if (sleep_data->sleep_record[i].state == DEEP_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y - fillRect_h * 2, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0x78, 0x35, 0xf7));
            }
            else if (sleep_data->sleep_record[i].state == LIGHT_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y - fillRect_h, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0xad, 0x86, 0xfc));
            }
            else if (sleep_data->sleep_record[i].state == AWAKE_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0xff, 0x87, 0x0f));
            }
            else if (sleep_data->sleep_record[i].state == REM_SLEEP)
            {
                compo_shape_set_location(shape, startX + fillRect_x + (fillRect_w / 2), fillRect_y + fillRect_h, fillRect_w, fillRect_h);
                compo_shape_set_color(shape, make_color(0x00, 0xf7, 0xd6));
            }

            if (i < sleep_data->recordCnt - 1) // 超出段数范围减1
            {
                fillRect_x += fillRect_w;
                sumInterval += sleep_data->sleep_record[i + 1].period;
                fillRect_w = (width * sumInterval) / timeInterval - fillRect_x;
                if (fillRect_w < 1 && sleep_data->sleep_record[i + 1].period != 0) // 不为0的数据最少显示1像素宽
                {
                    fillRect_w = 1;
                }
            }

            if ((fillRect_x - startX) + fillRect_w > width)
            {
                fillRect_w = width - (fillRect_x - startX);
            }

            if (i == sleep_data->recordCnt - 1)
            {
                if ((fillRect_x - startX) + fillRect_w < width)
                {
                    fillRect_w = width - (fillRect_x - startX);
                }
            }
        }
    }

    ab_free(sleep_data);
    return frm;
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//睡眠功能消息处理
static void func_sleep_process(void)
{
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
    compo_page_move_process(f_sleep->ptm);
    func_process();
}

static void func_sleep_message(size_msg_t msg)
{
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_sleep->ptm);
            break;

        case MSG_CTP_CLICK:
        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_LONG:
            break;

        case MSG_QDEC_BACKWARD:
            if (func_cb.flag_sort)      //快捷应用状态下不滚动页面
            {
                func_message(msg);
            }
            else
            {
                compo_page_move_set_by_pages(f_sleep->ptm, -1);
            }
            break;

        case MSG_QDEC_FORWARD:
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            else
            {
                compo_page_move_set_by_pages(f_sleep->ptm, 1);
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入睡眠功能
static void func_sleep_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_sleep_t));
    func_cb.frm_main = func_sleep_form_create();

    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
    f_sleep->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .page_size = GUI_SCREEN_HEIGHT,
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = 100,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_sleep->ptm, func_cb.frm_main->page_body, &info);
}

//退出睡眠功能
static void func_sleep_exit(void)
{
    f_sleep_t *f_sleep = (f_sleep_t *)func_cb.f_cb;
    if (f_sleep->ptm)
    {
        func_free(f_sleep->ptm);
    }
}

//睡眠功能
void func_sleep(void)
{
    printf("%s\n", __func__);
    func_sleep_enter();
    while (func_cb.sta == FUNC_SLEEP)
    {
        func_sleep_process();
        func_sleep_message(msg_dequeue());
    }
    func_sleep_exit();
}
