#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

///*版本测试*/
#define MODE_ONE_SPACING_Y  GUI_SCREEN_HEIGHT/11
#define MODE_ONE_INTIAL_SPACING_Y  GUI_SCREEN_CENTER_Y/10
#define PURPLE_COLOR   make_color(0x66,0x33,0xcc)
///*模式一*/

compo_form_t * func_factory_testing_mode_1(void);
compo_form_t * func_factory_testing_mode_2(void);
compo_form_t * func_factory_testing_mode_3(void);
compo_form_t * func_factory_testing_mode_4(void);

enum
{
    MODE_1=0,
    MODE_2,
    MODE_3,
    MODE_4,
};

enum
{
    FALL_ID=1,
    PASS_ID,
    RGB_TEST_ID,
    SHAPE_1_ID,
    SHAPE_2_ID,
    SHAPE_3_ID,
    SHAPE_4_ID,
    SHAPE_5_ID,
    SHAPE_6_ID,
    SHAPE_7_ID,
    SHAPE_8_ID,
    SHAPE_9_ID,
    SHAPE_btn_1_ID,
    SHAPE_btn_2_ID,
    SHAPE_btn_3_ID,
    SHAPE_btn_4_ID,
    SHAPE_btn_5_ID,
    SHAPE_btn_6_ID,
    SHAPE_btn_7_ID,
    SHAPE_btn_8_ID,
    SHAPE_btn_9_ID,
};

static int mode_id = MODE_1;

typedef struct f_factory_testing_t_
{
    bool mode_test_result[20];
    u8 rgb_num;///RBB点击次数
    bool tp_test[9];///tp九宫格点击次状态
} f_factory_testing_t;

typedef struct f_tp_test_t_
{
    s16 x;
    s16 y;
} f_tp_test_t;

static const f_tp_test_t  f_tp_test_t_xy[]=
{

    {GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y},
    {GUI_SCREEN_CENTER_X-GUI_SCREEN_WIDTH/3,  GUI_SCREEN_CENTER_Y},
    {GUI_SCREEN_CENTER_X+GUI_SCREEN_WIDTH/3,  GUI_SCREEN_CENTER_Y},

    {GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+ GUI_SCREEN_HEIGHT/3},
    {GUI_SCREEN_CENTER_X-GUI_SCREEN_WIDTH/3,  GUI_SCREEN_CENTER_Y+ GUI_SCREEN_HEIGHT/3},
    {GUI_SCREEN_CENTER_X+GUI_SCREEN_WIDTH/3,  GUI_SCREEN_CENTER_Y+ GUI_SCREEN_HEIGHT/3},

    {GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y- GUI_SCREEN_HEIGHT/3},
    {GUI_SCREEN_CENTER_X-GUI_SCREEN_WIDTH/3,  GUI_SCREEN_CENTER_Y- GUI_SCREEN_HEIGHT/3},
    {GUI_SCREEN_CENTER_X+GUI_SCREEN_WIDTH/3,  GUI_SCREEN_CENTER_Y- GUI_SCREEN_HEIGHT/3},
};


///创建工厂测试窗体
compo_form_t *func_factory_testing_create(void)
{
    compo_form_t *frm;

    switch(mode_id)
    {
        case MODE_1:
            frm = func_factory_testing_mode_1();
            break;
        case MODE_2:
            frm = func_factory_testing_mode_2();
            break;
        case MODE_3:
            frm = func_factory_testing_mode_3();
            break;
        case MODE_4:
            frm = func_factory_testing_mode_4();
            break;
    }

    return frm;
}

///数据初始化
static void func_factory_testing_init(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    f_factory_testing->rgb_num = 0;///初始化RBG
    memset(f_factory_testing->tp_test, false, sizeof(f_factory_testing->tp_test));///初始化TP标志位
}

///创建版本测试窗体   模式一*/
compo_form_t * func_factory_testing_mode_1(void)
{
    compo_textbox_t *textbox;
    compo_button_t *btn;
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DEV_NEME]));///设备名称
    compo_textbox_set(textbox, i18n[STR_DEV_NEME]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*0);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    char davName[40];
    memset(davName,'\0',sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
    uteModulePlatformGetDevName((uint8_t*)davName,&davNameLength);///获取设备名称
    textbox = compo_textbox_create(frm, strlen(davName));///设备名称数据
    compo_textbox_set(textbox, davName);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*1);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_AVERSION_MAC]));///版本号&MAC
    compo_textbox_set(textbox, i18n[STR_AVERSION_MAC]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*2);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DEV_NEME]));///版本号数据
    compo_textbox_set(textbox, i18n[STR_DEV_NEME]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*3);

    uint8_t Ble_Address[6];//获取蓝牙地址数组
    char Ble_Address_str_buf[20];//蓝牙地址文本数组
    memset(Ble_Address_str_buf,'\0',sizeof(Ble_Address_str_buf));//初始化数组
    uteModulePlatformGetBleMacAddress(Ble_Address);//获取蓝牙地址
    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02X:%02X:%02X:%02X:%02X:%02X",\
             Ble_Address[0],Ble_Address[1],Ble_Address[2],Ble_Address[3],Ble_Address[4],Ble_Address[5]); //信息
    textbox = compo_textbox_create(frm, strlen(Ble_Address_str_buf));///MAC数据
    compo_textbox_set(textbox,Ble_Address_str_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*4);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_BATTERY_INFORMATION]));///电池信息
    compo_textbox_set(textbox, i18n[STR_BATTERY_INFORMATION]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*5);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DEV_NEME]));///电池容量数据
    compo_textbox_set(textbox, i18n[STR_DEV_NEME]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*6);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DEV_NEME]));///电池数据
    compo_textbox_set(textbox, i18n[STR_DEV_NEME]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*7);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DEV_NEME]));///电池温度数据
    compo_textbox_set(textbox, i18n[STR_DEV_NEME]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*7);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_TP_VERSION]));///TP版本号
    compo_textbox_set(textbox, i18n[STR_TP_VERSION]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/3, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*8);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DEV_NEME]));///TP版本号数据
    compo_textbox_set(textbox, i18n[STR_DEV_NEME]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*8-widget_text_get_area(textbox->txt).hei/2);
    compo_textbox_set_align_center(textbox, false);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_FALL]));///FALL
    compo_textbox_set(textbox, i18n[STR_FALL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9);
    compo_textbox_set_forecolor(textbox, COLOR_RED);

    btn = compo_button_create(frm);///透明按钮FALL
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,FALL_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PASS]));///PASS
    compo_textbox_set(textbox, i18n[STR_PASS]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);

    btn = compo_button_create(frm);///透明按钮PASS
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,PASS_ID);

    return frm;
}
///创建十字架测试窗体   模式二*/
compo_form_t * func_factory_testing_mode_2(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *shape;
    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_WIDTH/20);
    compo_shape_set_color(shape, COLOR_YELLOW );

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH/20, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_YELLOW );

    return frm;
}
///创建RGB测试窗体   模式三*/
compo_form_t * func_factory_testing_mode_3(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *shape;
    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_setid(shape,RGB_TEST_ID);
    compo_shape_set_color(shape, COLOR_RED );

    return frm;
}
///创建RGB测试窗体   模式三*/
compo_form_t * func_factory_testing_mode_4(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *shape;
    compo_button_t *btn;

    for(int i=0; i<9; i++)
    {
        shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_location(shape, f_tp_test_t_xy[i].x, f_tp_test_t_xy[i].y, GUI_SCREEN_WIDTH/3-6, GUI_SCREEN_HEIGHT/3-6);
        compo_setid(shape,i+SHAPE_1_ID);
        compo_shape_set_color(shape, COLOR_RED );

        btn = compo_button_create(frm);
        compo_button_set_location(btn,f_tp_test_t_xy[i].x, f_tp_test_t_xy[i].y, GUI_SCREEN_WIDTH/3-6, GUI_SCREEN_HEIGHT/3-6);
        compo_setid(btn,i+SHAPE_btn_1_ID);
    }

    return frm;
}
static void func_mode_pop_click(void)///弹窗
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    u8 ret = msgbox((char *)i18n[STR_DISTURD_TIM_CTT], NULL, NULL, MSGBOX_MODE_BTN_FACTORR, MSGBOX_MSG_TYPE_NONE);

    if(ret == MSGBOX_RES_OK)
    {
        compo_form_t *frm = func_cb.frm_main;
        f_factory_testing->mode_test_result[mode_id] = true;///获取测试结果
        mode_id ++;///切换下一个模式
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        frm = func_factory_testing_create();
    }
    else if(ret == MSGBOX_RES_CANCEL)
    {
        compo_form_t *frm = func_cb.frm_main;
        f_factory_testing->mode_test_result[mode_id] = false;///获取测试结果
        mode_id ++;///切换下一个模式
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        frm = func_factory_testing_create();
    }
}
static void func_mode_1_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    if(id)
    {

        compo_form_t *frm = func_cb.frm_main;

        switch(id)
        {
            case FALL_ID:///不通过后切换下一个模式
                f_factory_testing->mode_test_result[mode_id] = false;///获取测试结果
                mode_id ++;///切换下一个模式
                if (frm != NULL)
                {
                    compo_form_destroy(frm);
                    frm = NULL;
                }
                frm = func_factory_testing_create();
                break;
            case PASS_ID:///通过后切换下一个模式
                f_factory_testing->mode_test_result[mode_id] = true;///获取测试结果
                mode_id ++;///切换下一个模式
                if (frm != NULL)
                {
                    compo_form_destroy(frm);
                    frm = NULL;
                }
                frm = func_factory_testing_create();
                break;
        }
    }
}
static void func_mode_1_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_mode_1_click();
            break;
        default:
            break;
    }
}
static void func_mode_2_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_mode_pop_click();
            break;
        default:
            break;
    }
}
static void func_mode_3_click(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    compo_shape_t *shape = compo_getobj_byid(RGB_TEST_ID);

    switch(f_factory_testing->rgb_num)
    {
        case 0:
            compo_shape_set_color(shape, COLOR_GREEN );
            break;
        case 1:
            compo_shape_set_color(shape, PURPLE_COLOR );
            break;
        case 2:
            compo_shape_set_color(shape, COLOR_BLACK );
            break;
        case 3:
            compo_shape_set_color(shape, COLOR_WHITE );
            break;
        case 4:
            compo_shape_set_color(shape, COLOR_GRAY );
            break;
        case 5:
            func_mode_pop_click();
            break;
    }
    f_factory_testing->rgb_num++;
}
static void func_mode_3_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_mode_3_click();
            break;
        default:
            break;
    }
}
static void func_mode_4_click(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    compo_shape_t *shape = compo_getobj_byid(id-9);

    if(f_factory_testing->tp_test[0] && f_factory_testing->tp_test[1] && f_factory_testing->tp_test[2] && f_factory_testing->tp_test[3] && f_factory_testing->tp_test[4] &&
       f_factory_testing->tp_test[5] && f_factory_testing->tp_test[6] && f_factory_testing->tp_test[7] && f_factory_testing->tp_test[8] )
    {
        func_mode_pop_click();
    }

    switch(id)
    {
        case SHAPE_btn_1_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[0] = true;
            break;
        case SHAPE_btn_2_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[1] = true;
            break;
        case SHAPE_btn_3_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[2] = true;
            break;
        case SHAPE_btn_4_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[3] = true;
            break;
        case SHAPE_btn_5_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[4] = true;
            break;
        case SHAPE_btn_6_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[5] = true;
            break;
        case SHAPE_btn_7_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[6] = true;
            break;
        case SHAPE_btn_8_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[7] = true;
            break;
        case SHAPE_btn_9_ID:
            compo_shape_set_color(shape, COLOR_GREEN );
            f_factory_testing->tp_test[8] = true;
            break;

    }

}
static void func_mode_4_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_mode_4_click();
            break;
        default:
            break;
    }
}

///工厂测试功能消息处理
static void func_factory_testing_message(size_msg_t msg)
{
    switch (mode_id)
    {
        case MODE_1:
            func_mode_1_message(msg);
            break;
        case MODE_2:
            func_mode_2_message(msg);
            break;
        case MODE_3:
            func_mode_3_message(msg);
            break;
        case MODE_4:
            func_mode_4_message(msg);
            break;
        default:
            func_message(msg);
            break;
    }
}

///工厂测试功能事件处理
static void func_factory_testing_process(void)
{
    func_process();
}

///进入工厂测试功能
static void func_factory_testing_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_factory_testing_t));
    func_cb.frm_main = func_factory_testing_create();
}

///退出工厂测试功能
static void func_factory_testing_exit(void)
{
    func_cb.last = FUNC_FACTORY_TESTING;
}

///工厂测试功能
void func_factory_testing(void)
{
    printf("%s\n", __func__);
    func_factory_testing_enter();
    while (func_cb.sta == FUNC_FACTORY_TESTING)
    {
        func_factory_testing_process();
        func_factory_testing_message(msg_dequeue());
    }
    func_factory_testing_exit();
}
