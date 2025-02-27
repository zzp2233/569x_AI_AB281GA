#include "include.h"
#include "func.h"
#include "ute_module_systemtime.h"
#include "api_gui.h"
#if UTE_MODULE_SCREENS_CALENDAER_SUB_SUPPORT





#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT



//组件ID
enum
{
    COMPO_ID_BTN_CALENDAR_ENTER = 1,
    COMPO_ID_CHART,
    COMPO_ID_SCROLLBAR,
};
uint8_t ecig_date[24];

typedef struct f_ecig_t_
{
    page_tp_move_t *ptm;
    uint32_t tick;
    u8 last_hr;
    u8 max_hr;
    u8 min_hr;
    bool heart_pic_state;
    u8 heart_pic_size;
} f_ecig_t;

compo_form_t *func_ecig_vpae_sub_form_create(void)
{
    char txt_buf[30];
    //新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_ECIG_RECORD]);

    ///设置图片
    // compo_form_add_image(frm,UI_BUF_I330001_VAPE_RECORD_GROUP_808_BIN,120,196);
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_VAPE_RECORD_GROUP_808_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 196);


    ///创建图表
    compo_chartbox_t *chart = compo_chartbox_create(frm, 1, 24);///图表内的柱形图
    compo_chartbox_set_location(chart, GUI_SCREEN_CENTER_X+13, 196,228,105);
    compo_chartbox_set_pixel(chart, 1);
    compo_setid(chart, COMPO_ID_CHART);

    chart_t chart_info;
    chart_info.y = 8;

    chart_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        // 提取公共计算部分
        int base_x = i * chart_info.width + 2 * i;

        // 处理不同范围的 i 值
        if (i > 17)
        {
            chart_info.x = base_x + 12;
        }
        else if (i > 11)
        {
            chart_info.x = base_x + 8;
        }
        else if (i > 5)
        {
            chart_info.x = base_x + 4;
        }
        else
        {
            chart_info.x = base_x;
        }

        chart_info.height = ecig_date[i]/1.31;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart, i, chart_info, make_color(255,116,169));
    }

    ///设置图片

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_VAPE_RECORD_P_TODAY_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 73);

    //当天口数  文字
    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[STR_ECIG_P]);
    compo_textbox_t *textbox_p = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox_p, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox_p, false);
    compo_textbox_set_location(textbox_p,80, 62,64,36);
    compo_textbox_set(textbox_p,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",20);
    compo_textbox_t *textbox_num = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_align_center(textbox_num, false);
    compo_textbox_set_location(textbox_num,130, 63,64,36);
    compo_textbox_set(textbox_num,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[STR_ECIG_TODAT_P]);
    compo_textbox_t *textbox_today = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox_today, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox_today, false);
    compo_textbox_set_location(textbox_today,72, 105,120,36);
    compo_textbox_set(textbox_today,txt_buf);
    compo_textbox_set_forecolor(textbox_today,make_color(255,116,169));

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_VAPE_RECORD_T_TODAYS_VAPE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+33);

    //15天口数  文字
    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[STR_ECIG_P]);
    compo_textbox_t *textbox2_p = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox2_p, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox2_p, false);
    compo_textbox_set_location(textbox2_p,80, GUI_SCREEN_HEIGHT+22,64,36);
    compo_textbox_set(textbox2_p,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d",200);
    compo_textbox_t *textbox2_num = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox2_num, UI_BUF_0FONT_FONT_NUM_24_BIN);
    compo_textbox_set_align_center(textbox2_num, false);
    compo_textbox_set_location(textbox2_num,130, GUI_SCREEN_HEIGHT+23,64,36);
    compo_textbox_set(textbox2_num,txt_buf);

    memset(txt_buf,0,sizeof(txt_buf));
    sprintf(txt_buf, "%s", i18n[STR_ECIG_15_DAY]);
    compo_textbox_t *textbox_15today = compo_textbox_create(frm,strlen(txt_buf));
    compo_textbox_set_font(textbox_15today, UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set_align_center(textbox_15today, false);
    compo_textbox_set_location(textbox_15today,72, GUI_SCREEN_HEIGHT+65,120,36);
    compo_textbox_set(textbox_15today,txt_buf);
    compo_textbox_set_forecolor(textbox_15today,make_color(0,242,214));


    picbox = compo_picturebox_create(frm, UI_BUF_I330001_VAPE_RECORD_GROUP_809_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT+156);

    ///创建图表
    compo_chartbox_t *chart2 = compo_chartbox_create(frm, 1, 24);///图表内的柱形图
    compo_chartbox_set_location(chart2, GUI_SCREEN_CENTER_X+14, GUI_SCREEN_HEIGHT+156,228,105);
    compo_chartbox_set_pixel(chart2, 1);
    compo_setid(chart2, COMPO_ID_CHART);

    chart_t chart2_info;
    chart2_info.y = 8;

    chart2_info.width = 6;   ///像素点
    for (int i=0; i<24; i++)
    {
        // 提取公共计算部分
        int base_x = i * chart2_info.width + 2 * i;

        // 处理不同范围的 i 值
        if (i > 17)
        {
            chart2_info.x = base_x + 12;
        }
        else if (i > 11)
        {
            chart2_info.x = base_x + 8;
        }
        else if (i > 5)
        {
            chart2_info.x = base_x + 4;
        }
        else
        {
            chart2_info.x = base_x;
        }

        chart2_info.height = ecig_date[i]/1.31;///心率数据转换为柱形条显示数据
        compo_chartbox_set_value(chart2, i, chart2_info, make_color(0,242,214));
    }

    return frm;
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT


static void func_ecig_vpae_sub_date_update(bool next)
{

}

//单击按钮
static void func_ecig_vpae_sub_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_CALENDAR_ENTER:
            //  func_cb.flag_sort = false;
            //  func_cb.sta = FUNC_CALENDAER;
            break;
        default:
            break;
    }


}

//公共事件处理
static void func_ecig_vpae_sub_comm_process(void)
{
    f_ecig_t *f_ecig = (f_ecig_t *)func_cb.f_cb;
    compo_page_move_process(f_ecig->ptm);
    func_process();
}

//消息处理
static void func_ecig_vpae_sub_message(size_msg_t msg)
{
    f_ecig_t *f_ecig = (f_ecig_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_TOUCH:
            compo_page_move_touch_handler(f_ecig->ptm);
            break;

        default:
            func_message(msg);
            break;
    }
}


static void func_ecig_vpae_sub_enter(void)
{
    for (int i=0; i<24; i++)ecig_date[i]=5*i;
    func_cb.f_cb = func_zalloc(sizeof(f_ecig_t));
    func_cb.frm_main = func_ecig_vpae_sub_form_create();
    f_ecig_t *f_ecig = (f_ecig_t *)func_cb.f_cb;
    f_ecig->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
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
    compo_page_move_init(f_ecig->ptm, func_cb.frm_main->page_body, &info);
    func_cb.flag_animation = false;
    //刷新文本内容
    // func_calender_sub_refresh(time.year, time.month, time.day);
}


static void func_ecig_vpae_sub_exit(void)
{
    f_ecig_t *f_ecig = (f_ecig_t *)func_cb.f_cb;
    if (f_ecig->ptm)
    {
        func_free(f_ecig->ptm);
    }
    // func_cb.last = FUNC_ECIG_VPAE_SUB;
}


void func_ecig_vpae_sub(void)
{
    printf("%s\n", __func__);
    func_ecig_vpae_sub_enter();
    while (func_cb.sta == FUNC_ECIG_VPAE_SUB)
    {
        func_ecig_vpae_sub_comm_process();
        func_ecig_vpae_sub_message(msg_dequeue());
    }
    func_ecig_vpae_sub_exit();
}


#endif
