#include "include.h"
#include "func.h"
#include "ute_application_common.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//三个图标与矩形的Y轴
#define RECT_Y_1 GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y*0.4
#define RECT_Y_2 GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y*0.1
#define RECT_Y_3 GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y*0.6

//矩形的宽度
#define RECT_WIDTH GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6

//图标对应矩形X轴前部坐标
#define IMG_BTN_FIRST_X GUI_SCREEN_CENTER_X - (GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6)/2 + gui_image_get_size(UI_BUF_LONG_PRESS_RESTART_BIN).wid/2+3
#define IMG_BTN_LAST_X  GUI_SCREEN_CENTER_X + (GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6)/2 - gui_image_get_size(UI_BUF_LONG_PRESS_RESTART_BIN).wid/2-3

//#define IMG_WIDTH    gui_image_get_size(UI_BUF_LONG_PRESS_RESTART_BIN).wid//图片宽度
#define IMG_WIDTH  56//图片宽度

//进度条前后点
#define RIGHT_TOUCH  IMG_BTN_FIRST_X+IMG_WIDTH/2
#define LENGTH_TOUCH IMG_BTN_FIRST_X-IMG_WIDTH/2

typedef struct f_long_press_t_
{
    bool touch_flag; //触屏状态标志位
    bool touch_btn_flag;//触屏按键状态标志位
    u16  touch_btn_id; //触屏按键id状态
} f_long_press_t;


enum
{
    RECT_ID_1 = 1,

    IMG_BTN_ID_1,
    IMG_BTN_ID_2,
    IMG_BTN_ID_3
};

//创建睡眠窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_long_press_form_create(void)
{

    compo_button_t * img_btn;
    compo_shape_t  * rect_cover;

    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CANCEL]);

    /*创建三个底部椭圆*/
    compo_shape_t * rectangle;
    rectangle = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(rectangle,GUI_SCREEN_CENTER_X,RECT_Y_1,RECT_WIDTH, IMG_WIDTH+6);
    compo_shape_set_color(rectangle,make_color(0x33,0x33,0x33));
    compo_shape_set_radius(rectangle, GUI_SCREEN_HEIGHT/4/2);

    rectangle = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(rectangle,GUI_SCREEN_CENTER_X,RECT_Y_2,RECT_WIDTH,IMG_WIDTH+6);
    compo_shape_set_color(rectangle,make_color(0x33,0x33,0x33));
    compo_shape_set_radius(rectangle, GUI_SCREEN_HEIGHT/4/2);

    rectangle = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(rectangle,GUI_SCREEN_CENTER_X,RECT_Y_3,RECT_WIDTH, IMG_WIDTH+6);
    compo_shape_set_color(rectangle,make_color(0x33,0x33,0x33));
    compo_shape_set_radius(rectangle, GUI_SCREEN_HEIGHT/4/2);

    /*创建三个文本*/
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm,strlen(i18n[STR_SLIDE_CALL]));
    compo_textbox_set(txt,i18n[STR_SLIDE_CALL]);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.1,RECT_Y_3 );

    txt = compo_textbox_create(frm,strlen(i18n[STR_SLIDE_OFF]));
    compo_textbox_set(txt,i18n[STR_SLIDE_OFF]);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.1, RECT_Y_2);

    txt = compo_textbox_create(frm,strlen(i18n[STR_SLIDE_REST]));
    compo_textbox_set(txt,i18n[STR_SLIDE_REST]);
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.1, RECT_Y_1);

    /*创建一个椭圆用于滑动时覆盖字体*/
    rect_cover = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_setid(rect_cover, RECT_ID_1);
    compo_shape_set_color(rect_cover,make_color(0xff, 0x39, 0x10));
    compo_shape_set_radius(rect_cover, GUI_SCREEN_HEIGHT/4/2);

    /*创建三个滑动图标*/
    img_btn = compo_button_create_by_image(frm,UI_BUF_I330001_POWEROFF_ICON_RESTART_BIN);
    compo_button_set_pos(img_btn, IMG_BTN_FIRST_X,RECT_Y_1);
    widget_set_size(img_btn->widget, IMG_WIDTH, IMG_WIDTH);
    compo_setid(img_btn, IMG_BTN_ID_3);

    img_btn = compo_button_create_by_image(frm,UI_BUF_I330001_POWEROFF_ICON_SHUTDOWN_BIN);
    compo_button_set_pos(img_btn, IMG_BTN_FIRST_X,RECT_Y_2);
    widget_set_size(img_btn->widget, IMG_WIDTH, IMG_WIDTH);
    compo_setid(img_btn, IMG_BTN_ID_2);

    img_btn = compo_button_create_by_image(frm,UI_BUF_I330001_POWEROFF_ICON_SOS_BIN);
    compo_button_set_pos(img_btn, IMG_BTN_FIRST_X,RECT_Y_3);
    widget_set_size(img_btn->widget, IMG_WIDTH, IMG_WIDTH);
    compo_setid(img_btn, IMG_BTN_ID_1);


    return frm;
}

//滑动事件处理
static void func_long_press_event_handle(s32 distance, u16 id)
{
    if(id)
    {
        if(distance==IMG_BTN_LAST_X) //滑动到设定X轴区间松开屏幕处理
        {
            switch(id)
            {
                case IMG_BTN_ID_1://SOS
                    memset(sys_cb.outgoing_number, 0, 16);
                    for(int i=0; i<strlen(sys_cb.sos_call_number); i++)
                    {
                        sys_cb.outgoing_number[i] = sys_cb.sos_call_number[i];
                    }
#if MODEM_CAT1_EN
                    if (bsp_modem_get_init_flag())
                    {
                        modem_call_dial(sys_cb.outgoing_number);
                    }
                    else
#endif
                    {
                        bt_call_redial_number();
                    }
                    break;
                case IMG_BTN_ID_2://关机
                    //    func_cb.sta = FUNC_PWROFF;
                    uteApplicationCommonPoweroff();
                    break;
                case IMG_BTN_ID_3://重启
                    //    ble_disconnect();
                    //    bt_disconnect(1);
                    //    WDT_RST();
                    uteApplicationCommonRestart();
                    break;
                default:
                    break;
            }
        }
    }
}

//长按滑动关机界面显示处理
static void func_long_press_slide_disp_handle()
{
    f_long_press_t *f_long_press = (f_long_press_t *)func_cb.f_cb;

    if(f_long_press->touch_flag)
    {
        s32 distance,y;
        int id =  f_long_press->touch_btn_id;
        compo_button_t *img_btn = compo_getobj_byid(id); //遍历按键图标控件
        compo_shape_t  *rect_cover = compo_getobj_byid(RECT_ID_1);//遍历红色拖尾控件

        f_long_press->touch_flag = ctp_get_dxy(&distance,&y);//获取触屏状态与滑动长度
        y = widget_get_location(img_btn->widget).y; //获取控件y轴
        distance += IMG_BTN_FIRST_X;//获取滑动长度加上图标初始位置

        if(distance<IMG_BTN_FIRST_X)  //最小滑动距离
        {
            distance = IMG_BTN_FIRST_X;
        }
        else if(distance>IMG_BTN_LAST_X)  //最大滑动距离
        {
            distance = IMG_BTN_LAST_X;
        }

        if(f_long_press->touch_flag == false)  //松开处理
        {
            func_long_press_event_handle(distance,f_long_press->touch_btn_id ); //事件处理
            distance = IMG_BTN_FIRST_X;//回弹
        }

        compo_button_set_pos(img_btn, distance,y);
        compo_shape_set_location(rect_cover,distance/2+IMG_WIDTH/2-IMG_WIDTH/19,y,distance, IMG_WIDTH);
    }
}

//长按滑动关机显示处理
static void func_long_press_process(void)
{
    func_long_press_slide_disp_handle();
    func_process();
}

//触摸按键处理
static void func_long_press_button(void)
{
    f_long_press_t *f_long_press = (f_long_press_t *)func_cb.f_cb;

    f_long_press->touch_btn_id = compo_get_button_id();
    compo_shape_t  *rect_cover = compo_getobj_byid(RECT_ID_1);//遍历红色拖尾控件

    if( f_long_press->touch_btn_id >=IMG_BTN_ID_1 &&  f_long_press->touch_btn_id <=IMG_BTN_ID_3)    //触摸是否是按键图标
    {
        f_long_press->touch_flag = true;

        switch(f_long_press->touch_btn_id)
        {
            case IMG_BTN_ID_3:
                compo_shape_set_color(rect_cover,COLOR_WHITE);
                break;
            case IMG_BTN_ID_2:
                compo_shape_set_color(rect_cover,COLOR_WHITE);
                break;
            case IMG_BTN_ID_1:
                compo_shape_set_color(rect_cover,make_color(0xff,0x39,0x10));
                break;
        }
    }
}


static void func_long_press_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            func_long_press_button();
            break;
        case KU_BACK:
            func_switch_to_clock();
            break;
        default:
            //func_message(msg);
            evt_message(msg);
            break;
    }
}

//创建长按滑动关机界面
static void func_long_press_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_long_press_t));
    func_cb.frm_main = func_long_press_form_create();

}

//退出长按滑动关机界面
static void func_long_press_exit(void)
{
    func_cb.last = FUNC_LONG_PRESS;
}

//进入长按滑动关机界面功能
void func_long_press(void)
{
    printf("%s\n", __func__);
    func_long_press_enter();

    while (func_cb.sta == FUNC_LONG_PRESS)
    {
        func_long_press_process();
        func_long_press_message(msg_dequeue());
    }
    func_long_press_exit();
}
