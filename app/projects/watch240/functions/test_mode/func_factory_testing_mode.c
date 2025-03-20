#include "include.h"
#include "func.h"
#include "ute_module_heart.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_sport.h"
#include "ute_drv_motor.h"
#include "ute_application_common.h"
#include "ute_module_newFactoryTest.h"
#include "ute_drv_gsensor_common.h"
#include "ute_drv_battery_common.h"
#include "ute_module_micrecord.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define SIDEBAR_PAGE_HEIGHT GUI_SCREEN_HEIGHT+GUI_SCREEN_HEIGHT/3    ///长图总高度
#define MODE_ONE_SPACING_Y          GUI_SCREEN_HEIGHT/11
#define MODE_ONE_INTIAL_SPACING_Y   GUI_SCREEN_CENTER_Y/10
#define PURPLE_COLOR                make_color(0x66,0x33,0xcc)
#define RESULT_Y                    GUI_SCREEN_HEIGHT/10

#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT

static ute_new_factory_test_data_t *test_data;

compo_form_t * func_factory_testing_drv_info(void);
compo_form_t * func_factory_testing_cross(void);
compo_form_t * func_factory_testing_rgb(void);
compo_form_t * func_factory_testing_tp(void);
compo_form_t * func_factory_testing_mode_5(void);
compo_form_t * func_factory_testing_heart(void);
compo_form_t * func_factory_testing_blood_oxygen(void);
compo_form_t * func_factory_testing_gsensor(void);
compo_form_t * func_factory_testing_key(void);
compo_form_t * func_factory_testing_motor(void);
compo_form_t * func_factory_testing_charging(void);
compo_form_t * func_factory_testing_mic_speaker(void);
compo_form_t * func_factory_testing_mode_13(void);
compo_form_t * func_factory_testing_mode_result(void);

enum
{
    MODE_1=0,
    MODE_2,
    MODE_3,
    MODE_4,
    MODE_5,
    MODE_6,
    MODE_7,
    MODE_8,
    MODE_9,
    MODE_10,
    MODE_11,
    MODE_12,
    MODE_13,
    MODE_RESULT,
};

enum
{
    FALL_ID=1,
    PASS_ID,
    ///*RGB测试*/
    RGB_TEST_ID,
    ///*tp九宫格测试*/
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
    ///*漏光测试*/
    LEAKAGE_TXT_1_ID,
    LEAKAGE_TXT_2_ID,
    LEAKAGE_TXT_3_ID,
    LEAKAGE_TXT_4_ID,
    LEAKAGE_TXT_5_ID,
    LEAKAGE_TXT_6_ID,
    ///*心率测试*/
    HEART_TXT_1_ID,
    HEART_TXT_2_ID,
    HEART_TXT_3_ID,
    ///*血氧测试*/
    BLOOD_OXYGEN_TXT_1_ID,
    BLOOD_OXYGEN_TXT_2_ID,
    ///*角度测试*/
    ANGLE_TXT_1_ID,
    ANGLE_TXT_2_ID,
    ANGLE_TXT_3_ID,
    ANGLE_TXT_4_ID,
    ///*按键测试*/
    KEY_TXT_ID,
    ///*马达测试*/
    MOTOR_TXT_ID,
    ///*充电测试*/
    CHARGE_TXT_ID,
    ///*录音测试*/
    TAPE_TXT_ID,
    ///*喇叭测试*/
    HORN_TXT_ID,
};

// bool mode_test_result_data[13];

typedef struct f_factory_testing_t_
{
    page_tp_move_t *ptm;
    u8 rgb_num;///RBB点击次数
    bool tp_test[9];///tp九宫格点击次状态
    u8 leakage_test_mode;///漏光测试
    bool leakage_test_result[3];///漏光测试结果
    bool leakage_mode3_flag;///漏光测试-对空采集标志位
    u8 heart_max;///心率测试最大值
    u8 heart_dif;///心率测试dif值
    // bool motor_flag;///测试马达标志位
    bool tape_flag;///测试录音标志位
    bool horn_flag;///测试喇叭标志位
} f_factory_testing_t;

const char result_txt[UTE_MODULE_NEW_FACTORY_MODULE_MAX][30]=
{
    "产品信息",
    "屏十字测试",
    "屏RGB测试",
    "TP测试",
    "心率测试",
    "血氧测试",
    "gsensor测试",
    "马达测试",
    "充电测试",
    "咪头喇叭测试",
    "按键测试",
};

typedef struct f_tp_test_t_///tp九宫格
{
    s16 x;
    s16 y;
} f_tp_test_t;
///tp九宫格
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
    compo_form_t *frm = NULL;

    if (test_data->moduleType == FACTORY_MODULE_VERSION)
    {
        frm = func_factory_testing_drv_info();
    }
    else if (test_data->moduleType == FACTORY_MODULE_CROSS)
    {
        frm = func_factory_testing_cross();
    }
    else if (test_data->moduleType == FACTORY_MODULE_RGB)
    {
        frm = func_factory_testing_rgb();
    }
    else if (test_data->moduleType == FACTORY_MODULE_TP)
    {
        frm = func_factory_testing_tp();
    }
    else if (test_data->moduleType == FACTORY_MODULE_HEART)
    {
        frm = func_factory_testing_heart();
    }
    else if (test_data->moduleType == FACTORY_MODULE_SPO2)
    {
        frm = func_factory_testing_blood_oxygen();
    }
    else if (test_data->moduleType == FACTORY_MODULE_GSENSOR)
    {
        frm = func_factory_testing_gsensor();
    }
    else if (test_data->moduleType == FACTORY_MODULE_MOTOR)
    {
        frm = func_factory_testing_motor();
    }
    else if (test_data->moduleType == FACTORY_MODULE_CHARGING)
    {
        frm = func_factory_testing_charging();
    }
    else if (test_data->moduleType == FACTORY_MODULE_MIC_SPEAKER)
    {
        frm = func_factory_testing_mic_speaker();
    }
    else if (test_data->moduleType == FACTORY_MODULE_KEY)
    {
        frm = func_factory_testing_key();
    }
    else if (test_data->moduleType == FACTORY_MODULE_MAX)
    {
        frm = func_factory_testing_mode_result();
    }

    return frm;
}

///创建结果长图
static void func_result_long_pic(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    f_factory_testing->ptm = (page_tp_move_t *)func_zalloc(sizeof(page_tp_move_t));
    page_move_info_t info =
    {
        .page_size = SIDEBAR_PAGE_HEIGHT,
        .page_count = 1,
        .up_over_perc = 0,
        .down_over_perc = 0,
    };
    compo_page_move_init(f_factory_testing->ptm, func_cb.frm_main->page_body, &info);
}
///数据初始化
static void func_factory_testing_init(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    f_factory_testing->rgb_num = 0;///初始化RBG
    memset(f_factory_testing->tp_test, false, sizeof(f_factory_testing->tp_test));///初始化TP标志位
    f_factory_testing->leakage_test_mode = 0;///初始化漏光测试模式
    memset(f_factory_testing->leakage_test_result, false, sizeof(f_factory_testing->leakage_test_result));///初始化漏光测试结果
    f_factory_testing->leakage_mode3_flag = false;///初始化漏光测试-对空采集标志位
    f_factory_testing->heart_dif = 0;///初始化心率dif值
    f_factory_testing->heart_max = 0;///初始化心率最大值
    //f_factory_testing->motor_flag =0;///初始化测试马达标志位
    f_factory_testing->tape_flag = false;///初始化测试录音标志位
    f_factory_testing->horn_flag = false;///初始化测试喇叭标志位

    uteModuleNewFactoryTestResetParam();
    uteModuleNewFactoryTestSetMode(&test_data);
    test_data->mode = FACTORY_TEST_MODE_FACTORY;
}
///创建版本测试窗体   模式一*/
compo_form_t *func_factory_testing_drv_info(void)
{
    compo_textbox_t *textbox;
    compo_button_t *btn;
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_DEV_NEME])); // 设备名称
    compo_textbox_set(textbox, i18n[STR_DEV_NEME]);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 0);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    char davName[40];
    memset(davName, '\0', sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
    uteModulePlatformGetDevName((uint8_t *)davName, &davNameLength); // 获取设备名称
    textbox = compo_textbox_create(frm, strlen(davName));            // 设备名称数据
    compo_textbox_set(textbox, davName);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 1);

    textbox = compo_textbox_create(frm, strlen((const char *)"固件版本"));
    compo_textbox_set(textbox, (const char *)"固件版本");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 2);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    textbox = compo_textbox_create(frm, strlen(UTE_SW_VERSION)); // 版本号数据
    compo_textbox_set(textbox, UTE_SW_VERSION);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 3);

    uint8_t Ble_Address[6];                                         // 获取蓝牙地址数组
    char Ble_Address_str_buf[20];                                   // 蓝牙地址文本数组
    memset(Ble_Address_str_buf, '\0', sizeof(Ble_Address_str_buf)); // 初始化数组
    uteModulePlatformGetBleMacAddress(Ble_Address);                 // 获取蓝牙地址
    snprintf((char *)Ble_Address_str_buf, sizeof(Ble_Address_str_buf), "%02X:%02X:%02X:%02X:%02X:%02X",
             Ble_Address[0], Ble_Address[1], Ble_Address[2], Ble_Address[3], Ble_Address[4], Ble_Address[5]); // 信息

    textbox = compo_textbox_create(frm, strlen((const char *)"蓝牙地址")); // MAC数据
    compo_textbox_set(textbox, (const char *)"蓝牙地址");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 4);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    textbox = compo_textbox_create(frm, strlen(Ble_Address_str_buf)); // MAC数据
    compo_textbox_set(textbox, Ble_Address_str_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 5);

    textbox = compo_textbox_create(frm, strlen((const char *)"TP版本")); // TP版本号
    compo_textbox_set(textbox, (const char *)"TP版本");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 6);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    char tp_version[20];
    memset(tp_version, '\0', sizeof(tp_version));
    snprintf(tp_version, sizeof(tp_version), "%d", uteModuleNewFactoryTestGetTpVersion());
    textbox = compo_textbox_create(frm, strlen((const char *)tp_version)); // TP版本号
    compo_textbox_set(textbox, tp_version);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 7);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_FALL])); // FALL
    compo_textbox_set(textbox, i18n[STR_FALL]);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 2, MODE_ONE_INTIAL_SPACING_Y * 2 + MODE_ONE_SPACING_Y * 9);
    compo_textbox_set_forecolor(textbox, COLOR_RED);

    btn = compo_button_create(frm); // 透明按钮FALL
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 2, MODE_ONE_INTIAL_SPACING_Y * 2 + MODE_ONE_SPACING_Y * 9, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, FALL_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PASS])); // PASS
    compo_textbox_set(textbox, i18n[STR_PASS]);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 2, MODE_ONE_INTIAL_SPACING_Y * 2 + MODE_ONE_SPACING_Y * 9);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);

    btn = compo_button_create(frm); // 透明按钮PASS
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 2, MODE_ONE_INTIAL_SPACING_Y * 2 + MODE_ONE_SPACING_Y * 9, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, PASS_ID);

    return frm;
}

///创建十字架测试窗体   模式二*/
compo_form_t * func_factory_testing_cross(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *shape;
    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, 2);
    compo_shape_set_color(shape, COLOR_YELLOW );

    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, 2, GUI_SCREEN_HEIGHT);
    compo_shape_set_color(shape, COLOR_YELLOW );

    return frm;
}
///创建RGB测试窗体   模式三*/
compo_form_t * func_factory_testing_rgb(void)
{
    ///新建窗体
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    compo_form_t *frm = compo_form_create(true);
    compo_shape_t *shape;
    shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
    compo_setid(shape,RGB_TEST_ID);
    compo_shape_set_color(shape, COLOR_WHITE );
    f_factory_testing->rgb_num = 0;

    return frm;
}
///创建TP测试窗体   模式四*/
compo_form_t * func_factory_testing_tp(void)
{
    printf("%s\n", __func__);
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_shape_t *shape;
    compo_button_t *btn;
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    memset(f_factory_testing->tp_test, false, sizeof(f_factory_testing->tp_test));

    for(int i=0; i<9; i++)
    {
        shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_location(shape, f_tp_test_t_xy[i].x, f_tp_test_t_xy[i].y, GUI_SCREEN_WIDTH/3-2, GUI_SCREEN_HEIGHT/3-2);
        compo_setid(shape,i+SHAPE_1_ID);
        compo_shape_set_color(shape, COLOR_RED );

        btn = compo_button_create(frm);
        compo_button_set_location(btn,f_tp_test_t_xy[i].x, f_tp_test_t_xy[i].y, GUI_SCREEN_WIDTH/3-2, GUI_SCREEN_HEIGHT/3-2);
        compo_setid(btn,i+SHAPE_btn_1_ID);
    }

    return frm;
}
///创建漏光测试窗体   模式五/
compo_form_t * func_factory_testing_mode_5(void)
{
//    printf("%s\n", __func__);
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(i18n[STR_DEV_NEME]));///设备名称
    compo_textbox_set(textbox, i18n[STR_DEV_NEME]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y/2);
    compo_setid(textbox,LEAKAGE_TXT_1_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_FALL]));///FALL
    compo_textbox_set(textbox, i18n[STR_FALL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,LEAKAGE_TXT_2_ID);
//    printf("%s1\n", __func__);

    char txt_buf[50];
    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "perV_Pass<=32");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*3+MODE_ONE_SPACING_Y/2);
    compo_setid(textbox,LEAKAGE_TXT_3_ID);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "perV_Pass>=100");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*4+MODE_ONE_SPACING_Y/2);
    compo_setid(textbox,LEAKAGE_TXT_4_ID);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "perV:255");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*6);
    compo_setid(textbox,LEAKAGE_TXT_5_ID);

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "curV:121");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*7);
    compo_setid(textbox,LEAKAGE_TXT_6_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_FALL]));///FALL
    compo_textbox_set(textbox, i18n[STR_FALL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9);
    compo_textbox_set_forecolor(textbox, COLOR_RED);

//    printf("%s2\n", __func__);

    compo_button_t *btn = compo_button_create(frm);///透明按钮FALL
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,FALL_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_PASS]));///PASS
    compo_textbox_set(textbox, i18n[STR_PASS]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);

    btn = compo_button_create(frm);///透明按钮PASS
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9, widget_text_get_area(textbox->txt).wid, widget_text_get_area(textbox->txt).hei*2);
    compo_setid(btn,PASS_ID);

//    printf("%s3\n", __func__);

    return frm;
}
///创建心率测试窗体   模式六/
compo_form_t * func_factory_testing_heart(void)
{
    uteModuleHeartStartSingleTesting(TYPE_HEART);
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];
    compo_textbox_t *textbox = NULL;

#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "dif:---,Max:---");
    textbox = compo_textbox_create(frm, strlen(txt_buf));///dif___, Max___
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2);
    compo_setid(textbox,HEART_TXT_1_ID);
#endif

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "心率:未佩戴");
    textbox = compo_textbox_create(frm, strlen(txt_buf));///心率:未佩戴
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y-MODE_ONE_INTIAL_SPACING_Y/2);
    compo_setid(textbox,HEART_TXT_2_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "---次/分");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  MODE_ONE_INTIAL_SPACING_Y+GUI_SCREEN_CENTER_Y);
    compo_setid(textbox,HEART_TXT_3_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_FALL]));///FALL
    compo_textbox_set(textbox, i18n[STR_FALL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9);
    compo_textbox_set_forecolor(textbox, COLOR_RED);


    compo_button_t *btn = compo_button_create(frm);///透明按钮FALL
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
///创建血氧测试窗体   模式七/
compo_form_t * func_factory_testing_blood_oxygen(void)
{
    uteModuleHeartStartSingleTesting(TYPE_BLOODOXYGEN);
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "心率:未佩戴");
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf));///心率:未佩戴
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y);
    compo_setid(textbox,BLOOD_OXYGEN_TXT_1_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "---%%");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+MODE_ONE_INTIAL_SPACING_Y);
    compo_setid(textbox,BLOOD_OXYGEN_TXT_2_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_FALL]));///FALL
    compo_textbox_set(textbox, i18n[STR_FALL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9);
    compo_textbox_set_forecolor(textbox, COLOR_RED);


    compo_button_t *btn = compo_button_create(frm);///透明按钮FALL
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
///创建角度测试窗体   模式八/
compo_form_t * func_factory_testing_gsensor(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("步数"));
    compo_textbox_set(textbox, "步数");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_SPACING_Y);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "0");
    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  MODE_ONE_SPACING_Y+MODE_ONE_INTIAL_SPACING_Y*2);
    compo_setid(textbox,ANGLE_TXT_1_ID);

    textbox = compo_textbox_create(frm, strlen("G-sensor"));
    compo_textbox_set(textbox, "G-sensor");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y*3);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "X:0");
    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y);
    compo_setid(textbox,ANGLE_TXT_2_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "Y:0");
    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+MODE_ONE_INTIAL_SPACING_Y);
    compo_setid(textbox,ANGLE_TXT_3_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "Z:0");
    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+MODE_ONE_INTIAL_SPACING_Y*3);
    compo_setid(textbox,ANGLE_TXT_4_ID);


    textbox = compo_textbox_create(frm, strlen(i18n[STR_FALL]));///FALL
    compo_textbox_set(textbox, i18n[STR_FALL]);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2, MODE_ONE_INTIAL_SPACING_Y*2+MODE_ONE_SPACING_Y*9);
    compo_textbox_set_forecolor(textbox, COLOR_RED);

    compo_button_t *btn = compo_button_create(frm);///透明按钮FALL
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
///创建按键测试窗体   模式九*/
compo_form_t * func_factory_testing_key(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("按键测试"));
    compo_textbox_set(textbox, "按键测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2);

    // compo_shape_t *shape = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
    // compo_shape_set_location(shape, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH/3, GUI_SCREEN_HEIGHT/5);
    // compo_shape_set_color(shape, COLOR_RED );

    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH/3, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "按键: 0");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,KEY_TXT_ID);

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
///创建马达测试窗体   模式十*/
compo_form_t * func_factory_testing_motor(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("马达测试"));
    compo_textbox_set(textbox, "马达测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("震动关闭"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "震动关闭");
    compo_setid(textbox,MOTOR_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

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

///创建充电测试窗体   模式十一*/
compo_form_t * func_factory_testing_charging(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("充电测试"));
    compo_textbox_set(textbox, "充电测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("电池:未充电"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "电池:未充电");
    compo_setid(textbox,CHARGE_TXT_ID);
    compo_textbox_set_forecolor(textbox, COLOR_RED);

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
///创建咪喇叭测试窗体   模式十二*/
compo_form_t * func_factory_testing_mic_speaker(void)
{
    uteModuleMicRecordFactoryEnter();
    uteModuleMicRecordFactoryStart();
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("咪头喇叭测试"));
    compo_textbox_set(textbox, "咪头喇叭测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("开始录音"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "开始录音");
    compo_setid(textbox,TAPE_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

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
///创建喇叭测试窗体   模式十三*/
compo_form_t * func_factory_testing_mode_13(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("喇叭测试"));
    compo_textbox_set(textbox, "喇叭测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("声音已关闭"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "声音已关闭");
    compo_setid(textbox,HORN_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

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
///创建测试结果窗体   测试结果*/
compo_form_t * func_factory_testing_mode_result(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    for(int i=0; i< UTE_MODULE_NEW_FACTORY_MODULE_MAX; i++)
    {
        compo_textbox_t *textbox = compo_textbox_create(frm, strlen(result_txt[i]));
        compo_textbox_set_align_center(textbox, false );
        compo_textbox_set(textbox, result_txt[i]);
        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/5,RESULT_Y*i+MODE_ONE_INTIAL_SPACING_Y-widget_text_get_area(textbox->txt).hei/3);

        textbox = compo_textbox_create(frm, 4);
        compo_textbox_set_align_center(textbox, false );
        if(test_data->moduleResult[i] == MODULE_TEST_RESULT_PASS)
        {
            compo_textbox_set(textbox, i18n[STR_PASS]);
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);
        }
        else
        {
            compo_textbox_set(textbox, i18n[STR_FALL]);
            compo_textbox_set_forecolor(textbox, COLOR_RED);
        }
        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3,RESULT_Y*i+MODE_ONE_INTIAL_SPACING_Y-widget_text_get_area(textbox->txt).hei/3);

    }

    func_result_long_pic();
    return frm;

}

///*事件监听*/
static void func_mode_pop_click(void)///弹窗
{
    u8 ret = msgbox((char *)i18n[STR_DO_PASS], NULL, NULL, MSGBOX_MODE_BTN_FACTORR, MSGBOX_MSG_TYPE_NONE);

    if(ret == MSGBOX_RES_OK)
    {
        compo_form_t *frm = func_cb.frm_main;
        test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;
        test_data->moduleType ++;//切换下一个模式
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        func_cb.frm_main = func_factory_testing_create();
    }
    else if(ret == MSGBOX_RES_CANCEL)
    {
        compo_form_t *frm = func_cb.frm_main;
        test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;
        test_data->moduleType ++;//切换下一个模式
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        func_cb.frm_main = func_factory_testing_create();
    }
}
static void func_mode_version_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;
            test_data->moduleType ++;//切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;
            test_data->moduleType ++;//切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
    }
}

static void func_mode_rgb_click(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    compo_shape_t *shape = compo_getobj_byid(RGB_TEST_ID);
    f_factory_testing->rgb_num++;
    switch(f_factory_testing->rgb_num)
    {
        case 1:
            compo_shape_set_color(shape, COLOR_RED );
            break;
        case 2:
            compo_shape_set_color(shape, COLOR_GREEN );
            break;
        case 3:
            compo_shape_set_color(shape, COLOR_BLUE );
            break;
        case 4:
            func_mode_pop_click();
            break;
    }

}

static void func_mode_tp_click(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    int id = compo_get_button_id();
    compo_shape_t *shape = compo_getobj_byid(id-9);

    uint8_t click_num = 0;
    for (uint8_t i = 0; i < 9; i++)
    {
        if (f_factory_testing->tp_test[i] == true)
        {
            click_num ++;
        }
    }

    if (click_num > 8)
    {
        func_mode_pop_click();
        return;
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

static void func_mode_5_click(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    int id = compo_get_button_id();

    if(f_factory_testing->leakage_test_mode == 2)
    {
        switch(id)
        {
            case FALL_ID: ///不通过后切换下一个模式
            {
                compo_form_t *frm = func_cb.frm_main;
                test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
                test_data->moduleType ++;///切换下一个模式
                if (frm != NULL)
                {
                    compo_form_destroy(frm);
                    frm = NULL;
                }
                func_cb.frm_main = func_factory_testing_create();
            }
            return;
            case PASS_ID: ///通过后切换下一个模式
            {
                compo_form_t *frm = func_cb.frm_main;
                test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
                test_data->moduleType ++;///切换下一个模式
                if (frm != NULL)
                {
                    compo_form_destroy(frm);
                    frm = NULL;
                }
                func_cb.frm_main = func_factory_testing_create();
            }
            return;
            default:
                f_factory_testing->leakage_mode3_flag = true;
                break;
        }
    }

    switch(id)
    {
        case FALL_ID:
            f_factory_testing->leakage_test_result[f_factory_testing->leakage_test_mode] = false;
            f_factory_testing->leakage_test_mode ++;
            break;
        case PASS_ID:
            f_factory_testing->leakage_test_result[f_factory_testing->leakage_test_mode] = true;
            f_factory_testing->leakage_test_mode ++;
            break;
    }

}

static void func_mode_heart_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            uteModuleHeartStopSingleTesting(TYPE_HEART);
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            uteModuleHeartStopSingleTesting(TYPE_HEART);
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
    }
}

static void func_mode_blood_oxygen_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            uteModuleHeartStopSingleTesting(TYPE_BLOODOXYGEN);
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            uteModuleHeartStopSingleTesting(TYPE_BLOODOXYGEN);
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
    }
}

static void func_mode_gsensor_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
    }
}

static void func_mode_key_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
    }
}

static void func_mode_motor_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(MOTOR_TXT_ID);///马达震动

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            if(uteDrvMotorGetRunningStatus())
            {
                uteDrvMotorStop();///关闭马达
            }
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            if(uteDrvMotorGetRunningStatus())
            {
                uteDrvMotorStop();///关闭马达
            }
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        default:
            if(!uteDrvMotorGetRunningStatus())
            {
                compo_textbox_set(textbox, "震动开启");
                uteDrvMotorEnable();///开启马达
            }
            else
            {
                compo_textbox_set(textbox, "震动关闭");
                uteDrvMotorDisable();///关闭马达
            }

            break;
    }
}

static void func_mode_charging_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            uteDrvMotorSetIsAllowMotorVibration(false);///关闭马达
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
    }
}

static void func_mode_mic_speaker_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(TAPE_TXT_ID);///录音状态

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            uteModuleMicRecordFactoryExit();
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            uteModuleMicRecordFactoryExit();
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
        }
        break;
    }
}

static void func_mode_13_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(HORN_TXT_ID);///喇叭状态

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            uteDrvMotorSetIsAllowMotorVibration(false);///关闭马达
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
            func_result_long_pic();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            uteDrvMotorSetIsAllowMotorVibration(false);///关闭马达
            compo_form_t *frm = func_cb.frm_main;
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;///获取测试结果
            test_data->moduleType ++;///切换下一个模式
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
            func_result_long_pic();
        }
        break;
        default:
            printf("%s=test",__func__);
            f_factory_testing->horn_flag ^=1;

            if(f_factory_testing->horn_flag == true)
            {
                compo_textbox_set(textbox, "声音已开启");
            }
            else
            {
                compo_textbox_set(textbox, "声音已关闭");
            }
            break;
    }
}

///工厂测试功能消息处理
static void func_factory_testing_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
        {
            if (test_data->moduleType == FACTORY_MODULE_VERSION)
            {
                func_mode_version_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_CROSS)
            {
                func_mode_pop_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_RGB)
            {
                func_mode_rgb_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_TP)
            {
                func_mode_tp_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_HEART)
            {
                func_mode_heart_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_SPO2)
            {
                func_mode_blood_oxygen_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_GSENSOR)
            {
                func_mode_gsensor_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_MOTOR)
            {
                func_mode_motor_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_CHARGING)
            {
                func_mode_charging_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_MIC_SPEAKER)
            {
                func_mode_mic_speaker_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                func_mode_key_click();
            }
        }
        break;
        case MSG_CTP_TOUCH:
            if (test_data->moduleType == FACTORY_MODULE_MAX)
            {
                f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
                compo_page_move_touch_handler(f_factory_testing->ptm);
            }
            break;
        case KU_DELAY_BACK:
        case K_BACK:
        {
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                compo_textbox_t *textbox1 = compo_getobj_byid(KEY_TXT_ID);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
            else
            {
                evt_message(msg);
            }
        }
        break;

        case KL_BACK:
        {
            uint8_t ret = msgbox("退出当前测试？", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if (ret == MSGBOX_RES_OK)
            {
                uteTaskGuiStartScreen(FUNC_TEST_MODE,0,__func__);
                uteTaskGuiStackRemoveScreenId(FUNC_FACTORY_TESTING);
            }
        }
        break;

        default:
            evt_message(msg);
            break;
    }

#if 0
    switch (test_data->moduleType)
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
        case MODE_5:
            func_mode_5_message(msg);
            break;
        case MODE_6:
            func_mode_6_message(msg);
            break;
        case MODE_7:
            func_mode_7_message(msg);
            break;
        case MODE_8:
            func_mode_8_message(msg);
            break;
        case MODE_9:
            func_mode_9_message(msg);
            break;
        case MODE_10:
            func_mode_10_message(msg);
            break;
        case MODE_11:
            func_mode_11_message(msg);
            break;
        case MODE_12:
            func_mode_12_message(msg);
            break;
        case MODE_13:
            func_mode_13_message(msg);
            break;
        case MODE_RESULT:
            func_mode_result_message(msg);
            func_message(msg);
            break;
        default:
//            func_message(msg);
            break;
    }
#endif
}
///*事件监听*/

///*数据刷新*/
static void func_mode_5_process(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    char txt_buf[50];

    compo_textbox_t *textbox1 = compo_getobj_byid(LEAKAGE_TXT_1_ID);
    compo_textbox_t *textbox2 = compo_getobj_byid(LEAKAGE_TXT_2_ID);
    compo_textbox_t *textbox3 = compo_getobj_byid(LEAKAGE_TXT_3_ID);
    compo_textbox_t *textbox4 = compo_getobj_byid(LEAKAGE_TXT_4_ID);
    compo_textbox_t *textbox5 = compo_getobj_byid(LEAKAGE_TXT_5_ID);
    compo_textbox_t *textbox6 = compo_getobj_byid(LEAKAGE_TXT_6_ID);

    switch(f_factory_testing->leakage_test_mode)
    {
        case 0:
            compo_textbox_set(textbox1, i18n[STR_DEV_NEME]);
            compo_textbox_set(textbox2, i18n[STR_FALL]);
            snprintf((char *)txt_buf, sizeof(txt_buf), "perV_Pass<=32");
            compo_textbox_set(textbox3,txt_buf);
            snprintf((char *)txt_buf, sizeof(txt_buf), "perV_Pass>=100");
            compo_textbox_set(textbox4,txt_buf);
            snprintf((char *)txt_buf, sizeof(txt_buf), "perV:255");
            compo_textbox_set(textbox5,txt_buf);
            snprintf((char *)txt_buf, sizeof(txt_buf), "curV:121");
            compo_textbox_set(textbox6,txt_buf);
            break;
        case 1:
            compo_textbox_set(textbox1, i18n[STR_DEV_NEME]);
            compo_textbox_set(textbox2, i18n[STR_FALL]);
            snprintf((char *)txt_buf, sizeof(txt_buf), "perV_Pass<=30");
            compo_textbox_set(textbox3,txt_buf);
            snprintf((char *)txt_buf, sizeof(txt_buf), "perV_Pass>=100");
            compo_textbox_set(textbox4,txt_buf);
            snprintf((char *)txt_buf, sizeof(txt_buf), "perV:255");
            compo_textbox_set(textbox5,txt_buf);
            snprintf((char *)txt_buf, sizeof(txt_buf), "curV:121");
            compo_textbox_set(textbox6,txt_buf);
            break;
        case 2:
            if(f_factory_testing->leakage_mode3_flag == true) ///漏光测试-对空采集标志位
            {
                compo_textbox_set(textbox1, i18n[STR_DEV_NEME]);
                compo_textbox_set(textbox2, i18n[STR_FALL]);
                snprintf((char *)txt_buf, sizeof(txt_buf), "perV_Pass<=50");
                compo_textbox_set(textbox3,txt_buf);
                snprintf((char *)txt_buf, sizeof(txt_buf), "活体");
                compo_textbox_set(textbox4,txt_buf);
                compo_textbox_set_forecolor(textbox4, COLOR_GREEN);
                compo_textbox_set_pos(textbox4,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*5);
                snprintf((char *)txt_buf, sizeof(txt_buf), "perV:255");
                compo_textbox_set(textbox5,txt_buf);
                snprintf((char *)txt_buf, sizeof(txt_buf), "curV:121");
                compo_textbox_set(textbox6,txt_buf);
            }
            else
            {
                compo_textbox_set(textbox1, i18n[STR_DEV_NEME]);
                compo_textbox_set(textbox2, i18n[STR_FALL]);
                snprintf((char *)txt_buf, sizeof(txt_buf), "perV_Pass<=50");
                compo_textbox_set(textbox3,txt_buf);
                snprintf((char *)txt_buf, sizeof(txt_buf), "点击采样");
                compo_textbox_set(textbox4,txt_buf);
                compo_textbox_set_forecolor(textbox4, COLOR_GREEN);
                compo_textbox_set_pos(textbox4,GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y+MODE_ONE_SPACING_Y*5);
                snprintf((char *)txt_buf, sizeof(txt_buf), "perV:255");
                compo_textbox_set(textbox5,txt_buf);
                snprintf((char *)txt_buf, sizeof(txt_buf), "curV:121");
                compo_textbox_set(textbox6,txt_buf);
            }
            break;
    }
}
static void func_mode_heart_process(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    char txt_buf[50];

    if(uteModuleHeartIsWear())
    {
        compo_textbox_t *textbox2 = compo_getobj_byid(HEART_TXT_2_ID);
        snprintf((char *)txt_buf, sizeof(txt_buf), "心率:检测中");
        compo_textbox_set(textbox2, txt_buf);

        compo_textbox_t *textbox3 = compo_getobj_byid(HEART_TXT_3_ID);
        snprintf((char *)txt_buf, sizeof(txt_buf), "%d 次/分",uteModuleHeartGetHeartValue());
        compo_textbox_set(textbox3,txt_buf);
    }
    else
    {
        compo_textbox_t *textbox2 = compo_getobj_byid(HEART_TXT_2_ID);
        snprintf((char *)txt_buf, sizeof(txt_buf), "心率:未佩戴");
        compo_textbox_set(textbox2, txt_buf);
    }
}

static void func_mode_blood_oxygen_process(void)
{
    char txt_buf[50];

    if(uteModuleBloodoxygenIsWear())
    {
        compo_textbox_t *textbox1 = compo_getobj_byid(BLOOD_OXYGEN_TXT_1_ID);
        compo_textbox_t *textbox2 = compo_getobj_byid(BLOOD_OXYGEN_TXT_2_ID);

        snprintf((char *)txt_buf, sizeof(txt_buf), "血氧:检测中");
        compo_textbox_set(textbox1, txt_buf);
        if(uteModuleHeartGetBloodOxygenValue())
        {
            snprintf((char *)txt_buf, sizeof(txt_buf), "%d%%",uteModuleHeartGetBloodOxygenValue());
            compo_textbox_set(textbox2, txt_buf);
        }
    }
    else
    {
        compo_textbox_t *textbox1 = compo_getobj_byid(BLOOD_OXYGEN_TXT_1_ID);

        snprintf((char *)txt_buf, sizeof(txt_buf), "血氧:未佩戴");
        compo_textbox_set(textbox1, txt_buf);
    }
}
static void func_mode_gsensor_process(void)
{
    char txt_buf[50];
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt,NULL,NULL);///获取步数

    if(tick_get() % 500 == 0)
    {
        int16_t xx,yy,zz;
        uteDrvGsensorCommonGetAccXyz(&xx,&yy,&zz);

        compo_textbox_t *textbox1 = compo_getobj_byid(ANGLE_TXT_1_ID);///步数
        compo_textbox_t *textbox2 = compo_getobj_byid(ANGLE_TXT_2_ID);///X轴
        compo_textbox_t *textbox3 = compo_getobj_byid(ANGLE_TXT_3_ID);///Y轴
        compo_textbox_t *textbox4 = compo_getobj_byid(ANGLE_TXT_4_ID);///Z轴

        snprintf((char *)txt_buf, sizeof(txt_buf), "%ld",totalStepCnt);
        compo_textbox_set(textbox1, txt_buf);
        snprintf((char *)txt_buf, sizeof(txt_buf), "X:%d",xx);
        compo_textbox_set(textbox2, txt_buf);
        snprintf((char *)txt_buf, sizeof(txt_buf), "Y:%d",yy);
        compo_textbox_set(textbox3, txt_buf);
        snprintf((char *)txt_buf, sizeof(txt_buf), "Z:%d",zz);
        compo_textbox_set(textbox4, txt_buf);
    }
}
static void func_mode_charging_process(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(CHARGE_TXT_ID);///充电状态

    if(uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE) ///获取充电状态
    {
        compo_textbox_set(textbox, "电池:充电中");
        compo_textbox_set_forecolor(textbox, COLOR_GREEN);
    }
    else
    {
        compo_textbox_set_forecolor(textbox, COLOR_RED);
        compo_textbox_set(textbox, "电池:未充电");
    }

}

static void func_mode_mic_speaker_process(void)
{
    if (!uteModuleMicRecordFactoryIsRecording() && !uteModuleMicRecordFactoryIsPlaying() && uteModuleMicRecordFactoryIsHaveData())
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_FACTORY_PLAY_SOUND_RECORDING, 0);
    }
}

static void func_mode_result_process(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    compo_page_move_process(f_factory_testing->ptm);
}
///工厂测试功能事件处理
static void func_factory_testing_process(void)
{
    reset_guioff_delay();
    switch (test_data->moduleType)
    {
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
        case FACTORY_MODULE_HEART_CHECK_LIGHT:
            func_mode_5_process();
            break;
#endif
        case FACTORY_MODULE_HEART:
            func_mode_heart_process();
            break;
        case FACTORY_MODULE_SPO2:
            func_mode_blood_oxygen_process();
            break;
        case FACTORY_MODULE_GSENSOR:
            func_mode_gsensor_process();
            break;
        case FACTORY_MODULE_CHARGING:
            func_mode_charging_process();
            break;
        case FACTORY_MODULE_MIC_SPEAKER:
            func_mode_mic_speaker_process();
            break;
        case FACTORY_MODULE_MAX:
            func_mode_result_process();
            break;
        default:
            break;
    }
    func_process();
}
///*数据刷新*/

///进入工厂测试功能
static void func_factory_testing_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_factory_testing_t));
    func_factory_testing_init();
    func_cb.frm_main = func_factory_testing_create();
}

///退出工厂测试功能
static void func_factory_testing_exit(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    if (f_factory_testing->ptm)
    {
        func_free(f_factory_testing->ptm);
    }
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
#else
func_factory_testing(void)
{

}
///创建工厂测试窗体
compo_form_t *func_factory_testing_create(void)
{

}
#endif
