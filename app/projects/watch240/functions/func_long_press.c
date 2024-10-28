#include "include.h"
#include "func.h"

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
#define IMG_BTN_FIRST_X GUI_SCREEN_CENTER_X - (GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6)/2 + gui_image_get_size(UI_BUF_LONG_PRESS_RESTART_BIN).wid/2
#define IMG_BTN_LAST_X  GUI_SCREEN_CENTER_X + (GUI_SCREEN_WIDTH-GUI_SCREEN_WIDTH/6)/2 - gui_image_get_size(UI_BUF_LONG_PRESS_RESTART_BIN).wid/2

#define IMG_WIDTH    gui_image_get_size(UI_BUF_LONG_PRESS_RESTART_BIN).wid//图片宽度

//进度条前后点
#define RIGHT_TOUCH  IMG_BTN_FIRST_X+IMG_WIDTH/2
#define LENGTH_TOUCH IMG_BTN_FIRST_X-IMG_WIDTH/2

typedef struct f_long_press_t_ {
    s32 sil_x;

} f_long_press_t;


enum{
    RECT_ID_1 = 1,
    RECT_ID_2,
    RECT_ID_3,

    IMG_BTN_ID_1,
    IMG_BTN_ID_2,
    IMG_BTN_ID_3
};

compo_button_t * img_btn1;
compo_button_t * img_btn2;
compo_button_t * img_btn3;
compo_shape_t  * rect_cover;

u8 refresh_Icon_flag  = false; //触屏区间标志位
u16 id = IMG_BTN_ID_1; //图片控件id变量
u16 id_old = IMG_BTN_ID_1;//图片控件上一次id变量
bool touch_flag = false; //触屏状态标志位

//创建睡眠窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_long_press_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_FIND_PHONE]);

    /*创建三个底部椭圆*/
    compo_shape_t * rectangle;
    rectangle = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(rectangle,GUI_SCREEN_CENTER_X,RECT_Y_1,RECT_WIDTH, GUI_SCREEN_HEIGHT/5);
    compo_shape_set_color(rectangle,COLOR_GRAY);
    compo_shape_set_radius(rectangle, GUI_SCREEN_HEIGHT/4/2);
    compo_setid(rectangle, RECT_ID_1);

    rectangle = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(rectangle,GUI_SCREEN_CENTER_X,RECT_Y_2,RECT_WIDTH, GUI_SCREEN_HEIGHT/5);
    compo_shape_set_color(rectangle,COLOR_GRAY);
    compo_shape_set_radius(rectangle, GUI_SCREEN_HEIGHT/4/2);
    compo_setid(rectangle, RECT_ID_2);

    rectangle = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(rectangle,GUI_SCREEN_CENTER_X,RECT_Y_3,RECT_WIDTH, GUI_SCREEN_HEIGHT/5);
    compo_shape_set_color(rectangle,COLOR_GRAY);
    compo_shape_set_radius(rectangle, GUI_SCREEN_HEIGHT/4/2);
    compo_setid(rectangle, RECT_ID_3);

    /*创建三个文本*/
    compo_textbox_t *txt;
    txt = compo_textbox_create(frm,5);
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set(txt,"滑动SOS");
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.1, RECT_Y_1);

    txt = compo_textbox_create(frm,4);
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set(txt,"滑动关机");
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.1, RECT_Y_2);

    txt = compo_textbox_create(frm,4);
    compo_textbox_set_font(txt,UI_BUF_0FONT_FONT_BIN);
    compo_textbox_set(txt,"滑动重启");
    compo_textbox_set_pos(txt, GUI_SCREEN_CENTER_X*1.1, RECT_Y_3);

    /*创建一个椭圆用于滑动时覆盖字体*/
    rect_cover = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_color(rect_cover,COLOR_RED);
    compo_shape_set_radius(rect_cover, GUI_SCREEN_HEIGHT/4/2);


    /*创建三个滑动图标*/
    img_btn1 = compo_button_create_by_image(frm,UI_BUF_LONG_PRESS_SOS_BIN);
    compo_button_set_pos(img_btn1, IMG_BTN_FIRST_X,RECT_Y_1);
    compo_setid(img_btn1, IMG_BTN_ID_1);

    img_btn2 = compo_button_create_by_image(frm,UI_BUF_LONG_PRESS_OFF_BIN);
    compo_button_set_pos(img_btn2, IMG_BTN_FIRST_X,RECT_Y_2);
    compo_setid(img_btn2, IMG_BTN_ID_2);

    img_btn3 = compo_button_create_by_image(frm,UI_BUF_LONG_PRESS_RESTART_BIN);
    compo_button_set_pos(img_btn3, IMG_BTN_FIRST_X,RECT_Y_3);
    compo_setid(img_btn3, IMG_BTN_ID_3);




    return frm;
}

//滑动事件处理
static void func_long_press_event_handle(s32 distance , u16 id)
{
    if(id){
        if(distance>GUI_SCREEN_WIDTH/1.6 && distance<GUI_SCREEN_WIDTH/1.3){//滑动到设定X轴区间松开屏幕处理
          switch(id){
            case IMG_BTN_ID_1://SOS
                break;
            case IMG_BTN_ID_2://关机
               func_cb.sta = FUNC_PWROFF;
                break;
            case IMG_BTN_ID_3://重启
               ble_disconnect();
               bt_disconnect(1);
               WDT_RST();
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
    s32 distance,y;
    s16 x,rect_width;
    s32 btn_location=0;

    x = ctp_get_sxy().x;//获取第一下触屏x坐标
    touch_flag = ctp_get_dxy(&distance,&y);//获取触屏状态与滑动长度

    if(touch_flag){ //是否在触屏状态
//    printf("X:%d  L:%d  R:%d T:%d\n",x,LENGTH_TOUCH,RIGHT_TOUCH,refresh_Icon_flag);
//    printf("X:%d \n",distance);
      if(x>=LENGTH_TOUCH && RIGHT_TOUCH>=x){//第一下触屏是否在图标上
          refresh_Icon_flag = true;   //打开获取图标刷新长度
          id = compo_get_button_id(); //获取图标id

          if(id!=0)                   //获取id时防止获取到空id导致图标不能回弹
          id_old = id;
          else id = id_old;
      }
      if(refresh_Icon_flag)btn_location = distance; //获取图标滑动长度
     }else{ //不在触屏状态
        func_long_press_event_handle(distance,id);//事件处理
        btn_location = 0; //松开回弹
        refresh_Icon_flag = false; //刷新图标关闭
    }

    btn_location += IMG_BTN_FIRST_X;//获取滑动长度加上图标初始位置
    //滑动局限显示处理
    if(btn_location<IMG_BTN_FIRST_X)btn_location = IMG_BTN_FIRST_X;
    else if(btn_location>IMG_BTN_LAST_X)btn_location = IMG_BTN_LAST_X;

    rect_width = btn_location;

    //刷新图标位置
    switch(id){
    case IMG_BTN_ID_1:
        compo_button_set_pos(img_btn1, btn_location,RECT_Y_1);
        compo_shape_set_location(rect_cover,rect_width/2+IMG_WIDTH/2-IMG_WIDTH/19,RECT_Y_1,rect_width+IMG_WIDTH/5, GUI_SCREEN_HEIGHT/5);
        break;
    case IMG_BTN_ID_2:
        compo_button_set_pos(img_btn2, btn_location,RECT_Y_2);
        compo_shape_set_location(rect_cover,rect_width/2+IMG_WIDTH/2-IMG_WIDTH/19,RECT_Y_2,rect_width+IMG_WIDTH/5, GUI_SCREEN_HEIGHT/5);
        break;
    case IMG_BTN_ID_3:
        compo_button_set_pos(img_btn3, btn_location,RECT_Y_3);
        compo_shape_set_location(rect_cover,rect_width/2+IMG_WIDTH/2-IMG_WIDTH/19,RECT_Y_3,rect_width+IMG_WIDTH/5, GUI_SCREEN_HEIGHT/5);
        break;
    default:
        break;
    }
}

//长按滑动关机显示处理
static void func_long_press_process(void)
{
    func_long_press_slide_disp_handle();
    func_process();
}


static void func_long_press_message(size_msg_t msg)
{
    switch (msg) {
    case KU_BACK:
        func_switch_to_clock();
        break;
    default:
        //func_message(msg);
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
    while (func_cb.sta == FUNC_LONG_PRESS) {
          func_long_press_process();
          func_long_press_message(msg_dequeue());
    }
    func_long_press_exit();
}
