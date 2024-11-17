#include "include.h"
#include "func.h"
#include "ute_module_heart.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define FINT_HEIGHT 24*2+8

#define FUNC_HR_SIMULATE_DATA_EN  0
#define CHART_NUM 24 //柱形图数量
#define CHART_200_LENGTH GUI_SCREEN_CENTER_Y*6.25/8  //柱形图框高度

//组件ID;
enum
{
    COMPO_ID_NUM_HEARTRATE_MAX = 1,
    COMPO_ID_NUM_HEARTRATE_CUR,
    COMPO_ID_NUM_HEARTRATE_MIN,
    COMPO_ID_TEXT_HEARTRATE_TIME,
    COMPO_ID_CHART,
};

enum
{
    CHART_ANIM_STA_IDLE,
    CHART_ANIM_STA_START,
    CHART_ANIM_STA_END,
};

#define HEARTRATE_TEXT_ITEM_CNT 3

typedef struct f_heartrate_t_
{
    page_tp_move_t *ptm;
    u8 last_hr;
    u8 max_hr;
    u8 min_hr;
    u16 data[CHART_NUM];
} f_heartrate_t;

#if FUNC_HR_SIMULATE_DATA_EN
static const uint16_t total_hr_value[] = {10,20,30,40,50,60,70};
#endif


//创建心率窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_heartrate_form_create(void)
{
    u8 idx = 0;
    uint8_t heart_date[24];
    uteModuleHeartGetTodayHistoryData(heart_date,24);//获取一天的心率


    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_HEART_RATE]);

    //设置内容
    compo_form_add_image(frm, UI_BUF_HEART_RATE_HR_BIN, 124, 88);
    compo_form_add_image(frm, UI_BUF_HEART_RATE_HR_BG_BIN, 120, 230);

    compo_form_add_image(frm, UI_BUF_HEART_RATE_UP_BIN, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.45, 186);
    compo_form_add_image(frm, UI_BUF_HEART_RATE_DOWN_BIN, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2.4, 186);

    //创建文本
    s16 txt_x = 12;
    s16 txt_y = 172;
    s16 txt_x_offset = 84;

    for(idx = 0; idx < HEARTRATE_TEXT_ITEM_CNT; idx++)
    {
        compo_textbox_t *txt_title = compo_textbox_create(frm, strlen(i18n[STR_PER_MINUTE]));
        compo_textbox_set_align_center(txt_title, false);
//        compo_textbox_set_pos(txt_title, txt_x + (idx * txt_x_offset), txt_y);
        compo_textbox_set_location(txt_title, txt_x + (idx * txt_x_offset), txt_y,FINT_HEIGHT,FINT_HEIGHT);
        if(idx == 1)
        {
            compo_textbox_set_location(txt_title, txt_x + (idx * txt_x_offset), txt_y-4,FINT_HEIGHT,FINT_HEIGHT);
        }
        compo_textbox_set_autoroll_mode(txt_title, 0);
        compo_textbox_set(txt_title, i18n[STR_PER_MINUTE]);
    }



    //测量心率值
    s16 txt_val_x = 38;
    s16 txt_val_y = 148;
    s16 txt_val_x_offset = 84;
    for (idx = 0; idx < HEARTRATE_TEXT_ITEM_CNT; idx++)
    {
        compo_textbox_t *txt_val = compo_textbox_create(frm, 3);
        compo_setid(txt_val, COMPO_ID_NUM_HEARTRATE_MAX + idx);
        //compo_textbox_set_align_center(txt_val, false);
        compo_textbox_set_pos(txt_val, txt_val_x + (idx * txt_val_x_offset), txt_val_y);
        if(idx == 1)
        {
            compo_textbox_set_font(txt_val, UI_BUF_0FONT_FONT_NUM_24_BIN);
            compo_textbox_set_pos(txt_val, txt_val_x + (idx * txt_val_x_offset), txt_val_y-4);
            compo_textbox_set(txt_val, "0");
        }
        else if (idx == 0)
        {
            char hr_buf[4];
            memset(hr_buf, 0, sizeof(hr_buf));
            snprintf(hr_buf, sizeof(hr_buf), "%d", uteModuleHeartGetMaxHeartValue());
            compo_textbox_set(txt_val, hr_buf);
        }
        else if (idx == 2)
        {
            char hr_buf[4];
            memset(hr_buf, 0, sizeof(hr_buf));
            snprintf(hr_buf, sizeof(hr_buf), "%d", uteModuleHeartGetMinHeartValue() == 0xff ? 0 : uteModuleHeartGetMinHeartValue());
            compo_textbox_set(txt_val, hr_buf);
        }
    }

    //心率详情
    // compo_form_add_image(frm, UI_BUF_HEART_RATE_CHART_BG_BIN, 120, 440);
    //compo_form_add_image(frm, UI_BUF_HEART_RATE_CHART_BG2_BIN, 120, 440);
    //s16 pic_w,pic_h;

    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_HEART_RATE_CHART_BG2_BIN);
    compo_picturebox_set_size(pic,  gui_image_get_size(UI_BUF_HEART_RATE_CHART_BG2_BIN).wid/1.07,gui_image_get_size(UI_BUF_HEART_RATE_CHART_BG2_BIN).hei);
    compo_picturebox_set_pos(pic, 120, 440);


    //创建图表
    compo_chartbox_t *chart = compo_chartbox_create(frm, CHART_TYPE_BAR, CHART_NUM);//图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/10, GUI_SCREEN_HEIGHT+GUI_SCREEN_CENTER_Y/1.1+5, (GUI_SCREEN_WIDTH/40+1)*CHART_NUM,CHART_200_LENGTH);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart, COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 0;
    chart_info.width = GUI_SCREEN_WIDTH/40;   //像素点
    for (int i=0; i<CHART_NUM; i++)
    {
        chart_info.x = i*chart_info.width + i;
        chart_info.height = 1.15*(heart_date[i]/2);//心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, COLOR_RED);
    }


    return frm;
}

// 刷新心率
static void func_heartrate_refresh(void)
{
    compo_textbox_t *txt = NULL;
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;

    u8 cur_hr = bsp_sensor_hrs_data_get();
    static char hr_buf[4];
    memset(hr_buf, 0, sizeof(hr_buf));

    //实时测量心率
    if (f_heartrate->last_hr != cur_hr)
    {
        f_heartrate->last_hr = cur_hr;
        txt = (compo_textbox_t *)compo_getobj_byid(COMPO_ID_NUM_HEARTRATE_CUR);
        snprintf(hr_buf, sizeof(hr_buf), "%d", cur_hr);

        compo_textbox_set(txt, hr_buf);

        if (f_heartrate->max_hr != uteModuleHeartGetMaxHeartValue())
        {
            f_heartrate->max_hr = uteModuleHeartGetMaxHeartValue();
            txt = (compo_textbox_t *)compo_getobj_byid(COMPO_ID_NUM_HEARTRATE_MAX);
            snprintf(hr_buf, sizeof(hr_buf), "%d", f_heartrate->max_hr);
            compo_textbox_set(txt, hr_buf);
            // return;
        }

        if (f_heartrate->min_hr != uteModuleHeartGetMinHeartValue() && uteModuleHeartGetMinHeartValue() > 0 && uteModuleHeartGetMinHeartValue() != 255)
        {
            f_heartrate->min_hr = uteModuleHeartGetMinHeartValue();
            txt = (compo_textbox_t *)compo_getobj_byid(COMPO_ID_NUM_HEARTRATE_MIN);
            snprintf(hr_buf, sizeof(hr_buf), "%d", f_heartrate->min_hr);
            compo_textbox_set(txt, hr_buf);
        }
    }

    if(cur_hr == 0 && bsp_sensor_hr_wear_sta_get())
    {
        reset_sleep_delay_all();
    }

}

//心率功能事件处理
static void func_heartrate_process(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    compo_page_move_process(f_heartrate->ptm);
    func_heartrate_refresh();
    func_process();
}

//心率功能消息处理
static void func_heartrate_message(size_msg_t msg)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_heartrate->ptm);
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
                compo_page_move_set_by_pages(f_heartrate->ptm, -1);
            }
            break;

        case MSG_QDEC_FORWARD:
            if (func_cb.flag_sort)
            {
                func_message(msg);
            }
            else
            {
                compo_page_move_set_by_pages(f_heartrate->ptm, 1);
            }
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入心率功能
static void func_heartrate_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_heartrate_t));
    func_cb.frm_main = func_heartrate_form_create();

    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    f_heartrate->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .title_used = true,
        .page_size = GUI_SCREEN_HEIGHT - (GUI_SCREEN_HEIGHT / 8),
        .page_count = 2,
        .jump_perc = 20,
        .quick_jump_perc = 100,
        .up_over_perc = 10,
        .down_over_perc = 10,
    };
    compo_page_move_init(f_heartrate->ptm, func_cb.frm_main->page_body, &info);
    // bsp_sensor_hr_init(0);
    uteModuleHeartStartSingleTesting(TYPE_HEART);
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
}

//退出心率功能
static void func_heartrate_exit(void)
{
    f_heartrate_t *f_heartrate = (f_heartrate_t *)func_cb.f_cb;
    if (f_heartrate->ptm)
    {
        func_free(f_heartrate->ptm);
    }
    // bsp_sensor_hr_stop();
    uteModuleHeartStopSingleTesting(TYPE_HEART);
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
}

//心率功能
void func_heartrate(void)
{
    printf("%s\n", __func__);
    func_heartrate_enter();
    while (func_cb.sta == FUNC_HEARTRATE)
    {
        func_heartrate_process();
        func_heartrate_message(msg_dequeue());
    }
    func_heartrate_exit();
}
