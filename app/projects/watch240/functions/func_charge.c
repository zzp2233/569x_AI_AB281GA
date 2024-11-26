#include "include.h"
#include "func.h"

#define TRACE_EN    1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define TEST_EN 0

#if TEST_EN
static u16 test_value = 0;
#define BAT_PERCENT_VALUE       test_value  //电量百分比数值
#else
#define BAT_PERCENT_VALUE       sys_cb.vbat_percent  //电量百分比数值
#endif // TEST_EN

#define LOWPWR_PERCENT          10  //低电百分比阈值
#define NUM_PERCENT_MARGIN      6  //数字和百分号间距

enum
{
    //图像id
    COMPO_ID_PIC_LOWPWR = 1,        //红色底图
    COMPO_ID_PIC_QUARTER0,          //绿色底图
    COMPO_ID_PIC_QUARTER1,          //50%部分
    COMPO_ID_PIC_QUARTER2,          //75%部分
    COMPO_ID_PIC_QUARTER,           //1/4绿圈
    //数字
    COMPO_ID_NUM_PERCENT,
};

typedef struct f_charge_t_
{
    u8 percent_bkp;
} f_charge_t;

//根据百分比计算四分之一圈旋转的角度(0-2700)
static u16 func_charge_percent_to_deg(u16 percent)
{
    percent = MIN(100, MAX(25, percent));
    return (2700 * (percent - 25) / (100 - 25));
}

//创建充电窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_charge_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //新建图像
    compo_picturebox_t *pic;
    pic = compo_picturebox_create(frm, UI_BUF_CHARGE_LOW_POWER_BIN);
    compo_setid(pic, COMPO_ID_PIC_LOWPWR);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE <= LOWPWR_PERCENT);

    pic = compo_picturebox_create(frm, UI_BUF_CHARGE_CHARGE2_BIN);
    compo_setid(pic, COMPO_ID_PIC_QUARTER0);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE > LOWPWR_PERCENT);

    u8 quarter_size = gui_image_get_size(UI_BUF_CHARGE_CHARGE1_BIN).wid;
    pic = compo_picturebox_create(frm, UI_BUF_CHARGE_CHARGE1_BIN);
    compo_setid(pic, COMPO_ID_PIC_QUARTER1);
    compo_picturebox_set_rotation_center(pic, quarter_size, 0);
    compo_picturebox_set_rotation(pic, 900);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE > 50);

    pic = compo_picturebox_create(frm, UI_BUF_CHARGE_CHARGE1_BIN);
    compo_setid(pic, COMPO_ID_PIC_QUARTER2);
    compo_picturebox_set_rotation_center(pic, quarter_size, 0);
    compo_picturebox_set_rotation(pic, 1800);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE > 75);

    pic = compo_picturebox_create(frm, UI_BUF_CHARGE_CHARGE1_BIN);
    compo_setid(pic, COMPO_ID_PIC_QUARTER);
    compo_picturebox_set_rotation_center(pic, quarter_size, 0);
    compo_picturebox_set_rotation(pic, func_charge_percent_to_deg(BAT_PERCENT_VALUE));
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE > LOWPWR_PERCENT);

    //创建数字
    char str_buff[8];
    compo_textbox_t *txt = compo_textbox_create(frm, 4);
    compo_setid(txt, COMPO_ID_NUM_PERCENT);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT*4/4.5);
    compo_textbox_set_font(txt, UI_BUF_0FONT_FONT_NUM_38_BIN);
    snprintf(str_buff, sizeof(str_buff), "%d%%", BAT_PERCENT_VALUE);
    compo_textbox_set(txt, str_buff);

    return frm;
}

//充电功能事件处理
static void func_charge_process(void)
{
    f_charge_t *f_charge = (f_charge_t *)func_cb.f_cb;
    compo_picturebox_t *pic;
    compo_textbox_t *txt;
    char str_buff[8];

#if TEST_EN
    static u32 tick = 0;
    if (tick_check_expire(func_cb.enter_tick, 3000) && tick_check_expire(tick, 300))
    {
        tick = tick_get();
        test_value += test_value >= 100 ? 0 : 1;
    }
#endif // TEST_EN

    if (f_charge->percent_bkp != BAT_PERCENT_VALUE)
    {
        TRACE("vbat:%d-->%d\n", f_charge->percent_bkp, BAT_PERCENT_VALUE);
        pic = compo_getobj_byid(COMPO_ID_PIC_LOWPWR);
        compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE <= LOWPWR_PERCENT);
        pic = compo_getobj_byid(COMPO_ID_PIC_QUARTER0);
        compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE > LOWPWR_PERCENT);
        pic = compo_getobj_byid(COMPO_ID_PIC_QUARTER1);
        compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE > 50);
        pic = compo_getobj_byid(COMPO_ID_PIC_QUARTER2);
        compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE > 75);
        pic = compo_getobj_byid(COMPO_ID_PIC_QUARTER);
        compo_picturebox_set_rotation(pic, func_charge_percent_to_deg(BAT_PERCENT_VALUE));
        compo_picturebox_set_visible(pic, BAT_PERCENT_VALUE > LOWPWR_PERCENT);

        txt = compo_getobj_byid(COMPO_ID_NUM_PERCENT);
        snprintf(str_buff, sizeof(str_buff), "%d%%", BAT_PERCENT_VALUE);
        compo_textbox_set(txt, str_buff);

        f_charge->percent_bkp = BAT_PERCENT_VALUE;
    }

    if (bsp_charge_sta_get() == 0)
    {
        func_cb.sta = FUNC_CLOCK;
    }

    func_process();
}

//充电功能消息处理
static void func_charge_message(size_msg_t msg)
{
    switch (msg)
    {
#if TEST_EN
        case MSG_CTP_CLICK:
            test_value = 0;
            TRACE("test_value:%d\n", test_value);
            break;
#endif // TEST_EN

        case MSG_CTP_SHORT_UP:
        case MSG_CTP_SHORT_DOWN:
        case MSG_CTP_SHORT_LEFT:
        case MSG_CTP_SHORT_RIGHT:
        case MSG_CTP_LONG:
            break;

        case MSG_QDEC_FORWARD:
        case MSG_QDEC_BACKWARD:
            break;

        default:
            //func_message(msg);
            evt_message(msg);
            break;
    }
}

//进入充电功能
static void func_charge_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_charge_t));
    func_cb.frm_main = func_charge_form_create();

    f_charge_t *f_charge = (f_charge_t *)func_cb.f_cb;
    f_charge->percent_bkp = BAT_PERCENT_VALUE;
    func_cb.enter_tick = tick_get();
}

//退出充电功能
static void func_charge_exit(void)
{
    func_cb.last = FUNC_CHARGE;
}

//充电功能
void func_charge(void)
{
    printf("%s\n", __func__);
    func_charge_enter();
    while (func_cb.sta == FUNC_CHARGE)
    {
        func_charge_process();
        func_charge_message(msg_dequeue());
    }
    func_charge_exit();
}
