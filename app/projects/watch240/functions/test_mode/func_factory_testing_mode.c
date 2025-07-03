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
#include "ute_module_compass.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define SIDEBAR_PAGE_HEIGHT (((sizeof(result_txt) / sizeof(result_txt[0]))))    ///长图总高度
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
compo_form_t * func_factory_testing_heart(void);
#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
compo_form_t * func_factory_testing_blood_oxygen(void);
#endif
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
compo_form_t *func_factory_testing_heart_light(void);
#endif
compo_form_t * func_factory_testing_gsensor(void);
compo_form_t * func_factory_testing_key(void);
compo_form_t * func_factory_testing_motor(void);
compo_form_t * func_factory_testing_charging(void);
compo_form_t * func_factory_testing_mic_speaker(void);
compo_form_t * func_factory_testing_mode_result(void);
compo_form_t * func_factory_testing_ring(void);
#if UTE_MODULE_NEW_FACTORY_TEST_GM_SUPPORT
compo_form_t * func_factory_testing_gm(void);
#endif

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
    KEY_TXT_ID_0,
    KEY_TXT_ID_1,
    KEY_TXT_ID_2,
    QDEC_FORWARD_TXT_ID,
    QDEC_BACKWARD_TXT_ID,
    ///*马达测试*/
    MOTOR_TXT_ID,
    MOTOR_BTN_ID,
    ///*充电测试*/
    CHARGE_TXT_ID,
    ///*录音测试*/
    TAPE_TXT_ID,
    ///*喇叭测试*/
    HORN_TXT_ID,
    RECORDING_BTN_ID,
    ///*地磁测试*/
    ANGLE_TXT_DATA_ID,
    GM_TXT_DATA_ID,
    ANGLE_TXT_X_ID,
    ANGLE_TXT_Y_ID,
    ANGLE_TXT_Z_ID,
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
    bool motor_flag;///测试马达标志位
    bool tape_flag;///测试录音标志位
    bool horn_flag;///测试喇叭标志位
    u8   count_num;//测试喇叭标音频播放时间
    u32 tick;      ///测试喇叭标音频播放时间
    bool touch_flag;///九宫格触摸测试标志位
    bool no_touch_flag;///九宫格触摸测试标志位
} f_factory_testing_t;

#if !SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT
const char result_txt[UTE_MODULE_NEW_FACTORY_MODULE_MAX][30]=
{
    "Product Information",//产品信息
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
    "Screen Cross Test",//屏十字测试
    "Screen RGB test",//屏RGB测试
#endif
    "TP test",//TP测试
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
    "Leakage test",//漏光测试
#endif
    "Heart rate test",//心率测试
#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
    "Blood oxygen test",//血氧测试
#endif
    "Gsensor test",//gsensor测试
    "Motor test",//马达测试
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
    "Charging test",//充电测试
#endif
    "Mic Head Speaker Test",//咪头喇叭测试
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
    "Audio test",//音频测试
#endif
    "Key test",//按键测试
};
#else
const char result_txt[UTE_MODULE_NEW_FACTORY_MODULE_MAX][30]=
{
    "产品信息",
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
    "屏十字测试",
    "屏RGB测试",
#endif
    "TP测试",
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
    "漏光测试",
#endif
    "心率测试",
#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
    "血氧测试",
#endif
    "gsensor测试",
    "马达测试",
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
    "充电测试",
#endif
    "咪头喇叭测试",
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
    "音频测试",
#endif
#if UTE_MODULE_NEW_FACTORY_TEST_GM_SUPPORT
    "地磁测试",
#endif
    "按键测试",
};
#endif


#if !SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT
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
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    if(uteModuleHeartGetWorkMode() == HR_WORK_MODE_HR)
    {
        uteModuleHeartStopSingleTesting(TYPE_HEART);
    }
    else if (uteModuleHeartGetWorkMode() == HR_WORK_MODE_SPO2)
    {
        uteModuleHeartStopSingleTesting(TYPE_BLOODOXYGEN);
    }

    if(f_factory_testing->motor_flag)
    {
        f_factory_testing->motor_flag = false;
        uteDrvMotorDisable();
    }

    if (test_data->moduleType == FACTORY_MODULE_VERSION)
    {
        frm = func_factory_testing_drv_info();
    }
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
    else if (test_data->moduleType == FACTORY_MODULE_CROSS)
    {
        frm = func_factory_testing_cross();
    }
    else if (test_data->moduleType == FACTORY_MODULE_RGB)
    {
        frm = func_factory_testing_rgb();
    }
    else if (test_data->moduleType == FACTORY_MODULE_CHARGING)
    {
        frm = func_factory_testing_charging();
    }
#endif
    else if (test_data->moduleType == FACTORY_MODULE_TP)
    {
        frm = func_factory_testing_tp();
    }
    else if (test_data->moduleType == FACTORY_MODULE_HEART)
    {
        frm = func_factory_testing_heart();
    }
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
    else if (test_data->moduleType == FACTORY_MODULE_HEART_CHECK_LIGHT)
    {
        frm = func_factory_testing_heart_light();
    }
#endif
#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
    else if (test_data->moduleType == FACTORY_MODULE_SPO2)
    {
        frm = func_factory_testing_blood_oxygen();
    }
#endif
    else if (test_data->moduleType == FACTORY_MODULE_GSENSOR)
    {
        frm = func_factory_testing_gsensor();
    }
    else if (test_data->moduleType == FACTORY_MODULE_MOTOR)
    {
        frm = func_factory_testing_motor();
    }
    else if (test_data->moduleType == FACTORY_MODULE_MIC_SPEAKER)
    {
        frm = func_factory_testing_mic_speaker();
    }
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
    else if (test_data->moduleType == FACTORY_MODULE_SPEAKER)
    {
        f_factory_testing->horn_flag = true;
        f_factory_testing->tick = tick_get();
        frm = func_factory_testing_ring();
    }
#endif
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

    uint16_t page_size = SIDEBAR_PAGE_HEIGHT*(GUI_SCREEN_HEIGHT/9);          //分页大小
#if (UTE_DRV_SCREEN_SHAPE == 0)
    page_size = page_size+(GUI_SCREEN_HEIGHT/7.2)*2;
#endif

    page_move_info_t info =
    {
        .page_size = page_size,
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
    f_factory_testing->motor_flag =false;///初始化测试马达标志位

    uteModuleNewFactoryTestResetParam();
    uteModuleNewFactoryTestSetMode(&test_data);
    test_data->mode = FACTORY_TEST_MODE_FACTORY;
}

static void func_factory_testing_pass_fail_bnt_click(void)
{
    int id = compo_get_button_id();

    if(id == PASS_ID || id == FALL_ID)
    {
        if(id == PASS_ID)
        {
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;
        }
        else
        {
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;
        }
        test_data->moduleType ++; //切换下一个模式

        compo_form_t *frm = func_cb.frm_main;
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        func_cb.frm_main = func_factory_testing_create();
    }
}

static void func_factory_testing_pass_fail_pop_click(void) //弹窗
{
    u8 ret = msgbox((char *)"Did it pass?", NULL, NULL, MSGBOX_MODE_BTN_FACTORR, MSGBOX_MSG_TYPE_NONE);

    if(ret == MSGBOX_RES_OK)
    {
        test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;
        test_data->moduleType ++;//切换下一个模式

        compo_form_t *frm = func_cb.frm_main;
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        func_cb.frm_main = func_factory_testing_create();
    }
    else if(ret == MSGBOX_RES_CANCEL)
    {
        test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;
        test_data->moduleType ++;//切换下一个模式

        compo_form_t *frm = func_cb.frm_main;
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        func_cb.frm_main = func_factory_testing_create();
    }
}

//创建PASS FAIL按钮
static void func_factory_testing_pass_fail_bnt_create(compo_form_t *frm)
{
    compo_textbox_t *textbox;

    textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); // FALL
    compo_textbox_set(textbox, (const char *)"FALL");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_HEIGHT * 5 / 6);
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    rect_t location_data = widget_get_location(textbox->txt);
    area_t txt_data = widget_text_get_area(textbox->txt);

    compo_button_t *btn = compo_button_create(frm); // 按钮FALL
    compo_button_set_location(btn,location_data.x, location_data.y,txt_data.wid,txt_data.hei*2.5);
    compo_setid(btn, FALL_ID);

    textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); // PASS
    compo_textbox_set(textbox, (const char *)"PASS");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_HEIGHT * 5 / 6);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);
    location_data = widget_get_location(textbox->txt);
    txt_data = widget_text_get_area(textbox->txt);

    btn = compo_button_create(frm); // 按钮PASS
    compo_button_set_location(btn,location_data.x, location_data.y,txt_data.wid,txt_data.hei*2.5);
    compo_setid(btn, PASS_ID);
}

//创建版本测试窗体   模式一*/
compo_form_t *func_factory_testing_drv_info(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, strlen((const char *)"Device name")); // 设备名称
    compo_textbox_set(textbox, (const char *)"Device name");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 0);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    char davName[40];
    memset(davName, '\0', sizeof(davName));
    uint8_t davNameLength = sizeof(davName);
    uteModulePlatformGetDevName((uint8_t *)davName, &davNameLength); // 获取设备名称
    textbox = compo_textbox_create(frm, strlen(davName));            // 设备名称数据
    compo_textbox_set(textbox, davName);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 1);

    textbox = compo_textbox_create(frm, strlen((const char *)"Firmware version"));
    compo_textbox_set(textbox, (const char *)"Firmware version");
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

    textbox = compo_textbox_create(frm, strlen((const char *)"Bluetooth address")); // MAC数据
    compo_textbox_set(textbox, (const char *)"Bluetooth address");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 4);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    textbox = compo_textbox_create(frm, strlen(Ble_Address_str_buf)); // MAC数据
    compo_textbox_set(textbox, Ble_Address_str_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 5);

    textbox = compo_textbox_create(frm, strlen((const char *)"TP version")); // TP版本号
    compo_textbox_set(textbox, (const char *)"TP version");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 6);
    compo_textbox_set_forecolor(textbox, PURPLE_COLOR);

    char tp_version[20];
    memset(tp_version, '\0', sizeof(tp_version));
    snprintf(tp_version, sizeof(tp_version), "%d", uteModuleNewFactoryTestGetTpVersion());
    textbox = compo_textbox_create(frm, strlen((const char *)tp_version)); // TP版本号
    compo_textbox_set(textbox, tp_version);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, MODE_ONE_INTIAL_SPACING_Y + MODE_ONE_SPACING_Y * 7);

    func_factory_testing_pass_fail_bnt_create(frm);

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

//创建心率漏光窗体  /
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
compo_form_t *func_factory_testing_heart_light(void)
{
    uteModuleNewFactoryTestSetCheckLightMode(NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK);
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];
    compo_textbox_t *textbox = NULL;

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof("Leakage test:green light"), "Leakage test:green light");
    textbox = compo_textbox_create(frm, strlen(txt_buf)+10); // dif___, Max___
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 1 / 7);
    compo_setid(textbox, LEAKAGE_TXT_1_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "preV:000");
    textbox = compo_textbox_create(frm, strlen(txt_buf)); /// 心率:未佩戴
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 7);
    compo_setid(textbox, LEAKAGE_TXT_2_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "curV:000");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 7);
    compo_setid(textbox, LEAKAGE_TXT_3_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "psV:000");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 7);
    compo_setid(textbox, LEAKAGE_TXT_4_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "fail");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 5 / 7);
    compo_setid(textbox, LEAKAGE_TXT_5_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
#endif

///创建心率测试窗体   模式六/
compo_form_t * func_factory_testing_heart(void)
{
    uteModuleHeartStartSingleTesting(TYPE_HEART);
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];
    compo_textbox_t *textbox = NULL;

#if 0//UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "dif:---,Max:---");
    textbox = compo_textbox_create(frm, strlen(txt_buf));///dif___, Max___
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y/2);
    compo_setid(textbox,HEART_TXT_1_ID);
#endif

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "Heart: Not worn");
    textbox = compo_textbox_create(frm, strlen(txt_buf)+10);///心率:未佩戴
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y-MODE_ONE_INTIAL_SPACING_Y/2);
    compo_setid(textbox,HEART_TXT_2_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "---BPM");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  MODE_ONE_INTIAL_SPACING_Y+GUI_SCREEN_CENTER_Y);
    compo_setid(textbox,HEART_TXT_3_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
///创建血氧测试窗体   模式七/
#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
compo_form_t * func_factory_testing_blood_oxygen(void)
{
    uteModuleHeartStartSingleTesting(TYPE_BLOODOXYGEN);
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "BO: Not worn");
    compo_textbox_t *textbox = compo_textbox_create(frm, strlen(txt_buf)+10);///心率:未佩戴
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y);
    compo_setid(textbox,BLOOD_OXYGEN_TXT_1_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "---%%");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+MODE_ONE_INTIAL_SPACING_Y);
    compo_setid(textbox,BLOOD_OXYGEN_TXT_2_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
#endif

///创建角度测试窗体   模式八/
compo_form_t * func_factory_testing_gsensor(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("Step"));
    compo_textbox_set(textbox, "Step");
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

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
///创建按键测试窗体   模式九*/
compo_form_t * func_factory_testing_key(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
//    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("KEY test"));
    compo_textbox_set(textbox, "KEY test");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 1 / 8 - MODE_ONE_INTIAL_SPACING_Y);

    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 8, GUI_SCREEN_WIDTH/3, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "KEY: 0");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,KEY_TXT_ID_0);

#if UTE_DRV_PWRKEY_MAX_CNT > 1
    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 8, GUI_SCREEN_WIDTH/3, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "KEY: 1");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,KEY_TXT_ID_1);
#endif

#if UTE_DRV_PWRKEY_MAX_CNT > 2
    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 8, GUI_SCREEN_WIDTH/3, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "KEY: 2");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,KEY_TXT_ID_2);
#endif

#if DRV_ENCODER_KEYS_SUPPORT
    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 5 / 8, GUI_SCREEN_WIDTH/2, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "Encoder: UP");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,QDEC_BACKWARD_TXT_ID);

    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 6 / 8, GUI_SCREEN_WIDTH/2, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "Encoder: Down");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,QDEC_FORWARD_TXT_ID);
#endif

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
///创建马达测试窗体   模式十*/
compo_form_t * func_factory_testing_motor(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
//    compo_button_t *btn;

    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    f_factory_testing->motor_flag = false;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("Motor test"));
    compo_textbox_set(textbox, "Motor test");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("Vibration off"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "Vibration off");
    compo_setid(textbox,MOTOR_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, MOTOR_BTN_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}

///创建充电测试窗体   模式十一*/
compo_form_t * func_factory_testing_charging(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
//    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("Charging test"));
    compo_textbox_set(textbox, "Charging test");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("Battery: uncharged")+10);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "Battery: uncharged");
    compo_setid(textbox,CHARGE_TXT_ID);
    compo_textbox_set_forecolor(textbox, COLOR_RED);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
///创建咪喇叭测试窗体   模式十二*/
compo_form_t * func_factory_testing_mic_speaker(void)
{
    uteModuleMicRecordFactoryEnter();
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("Mic Head Speaker Test"));
    compo_textbox_set(textbox, "Mic Head Speaker Test");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("Click recording")+10);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "Click recording");
    compo_setid(textbox,TAPE_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

    compo_button_t *btn = compo_button_create(frm); // 录音
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, RECORDING_BTN_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}

#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
///创建音频测试窗体   模式十二*/
compo_form_t * func_factory_testing_ring(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("音频测试"));
    compo_textbox_set(textbox, "音频测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("点击暂停播放"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "点击暂停播放");
    compo_setid(textbox,TAPE_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

    compo_button_t *btn = compo_button_create(frm); // 按键
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, RECORDING_BTN_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
#endif

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
        // compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/5,RESULT_Y*i+MODE_ONE_INTIAL_SPACING_Y-widget_text_get_area(textbox->txt).hei/3);
#if !SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT
        compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X/5, RESULT_Y*i+MODE_ONE_INTIAL_SPACING_Y-widget_text_get_area(textbox->txt).hei/3, GUI_SCREEN_CENTER_X, widget_text_get_area(textbox->txt).hei);
        compo_textbox_set_autoroll_mode(textbox, true);
#else
        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X/5,RESULT_Y*i+MODE_ONE_INTIAL_SPACING_Y-widget_text_get_area(textbox->txt).hei/3);
#endif

        textbox = compo_textbox_create(frm, 4);
        compo_textbox_set_align_center(textbox, false );
        if(test_data->moduleResult[i] == MODULE_TEST_RESULT_PASS)
        {
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);
        }
        else
        {
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_forecolor(textbox, COLOR_RED);
        }
        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3,RESULT_Y*i+MODE_ONE_INTIAL_SPACING_Y-widget_text_get_area(textbox->txt).hei/3);

    }

    func_result_long_pic();
#if (UTE_DRV_SCREEN_SHAPE == 0)
    widget_page_set_client(frm->page,0,(GUI_SCREEN_HEIGHT/7.2));
#endif

    return frm;

}

///*事件监听*/

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
        default:
            func_factory_testing_pass_fail_pop_click();
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
        func_factory_testing_pass_fail_pop_click();
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
#if UTE_MODULE_NEW_FACTORY_MODULE_3x3_TP_TOUCH_TEST_SUPPORT
static void func_mode_tp_touch(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    if(f_factory_testing->no_touch_flag == false)
    {
        s32 dx,dy;
        f_factory_testing->no_touch_flag = ctp_get_dxy(&dx,&dy);
    }

    if(f_factory_testing->touch_flag == true && f_factory_testing->no_touch_flag == true)
    {
        s32 dx,dy,x,y;
        f_factory_testing->touch_flag = ctp_get_cur_point(&dx,&dy,&x,&y);

        int id = ID_NULL;
        for (int i = SHAPE_1_ID; i <= SHAPE_9_ID; i++)
        {
            rect_t rect = compo_shape_get_location(compo_getobj_byid(i));
            if (abs_s(x - rect.x) * 2 <= rect.wid && abs_s(y - rect.y) * 2 <= rect.hei)
            {
                id = i;
                break;
            }
        }
        if(id<SHAPE_1_ID || id>SHAPE_9_ID)
        {
            return;
        }
        compo_shape_t *shape = compo_getobj_byid(id);
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
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;
            test_data->moduleType ++;//切换下一个模式
            compo_form_t *frm = func_cb.frm_main;
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
            return;
        }

        f_factory_testing->tp_test[id-SHAPE_1_ID] = true;
        compo_shape_set_color(shape, COLOR_GREEN );
    }
}
#endif
static void func_mode_motor_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(MOTOR_TXT_ID);///马达震动

    if(id == MOTOR_BTN_ID)
    {
        if(!f_factory_testing->motor_flag)
        {
            f_factory_testing->motor_flag = true;
            compo_textbox_set(textbox, (const char *)"Vibration on");
            uteDrvMotorEnable(); //开启马达
        }
        else
        {
            f_factory_testing->motor_flag = false;
            compo_textbox_set(textbox, (const char *)"Vibration off");
            uteDrvMotorDisable(); //关闭马达
        }
    }
}

#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
static void func_mode_test_ring_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            if (sys_cb.mp3_res_playing)
            {
                music_control(MUSIC_MSG_STOP);
            }
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            if (sys_cb.mp3_res_playing)
            {
                music_control(MUSIC_MSG_STOP);
            }
        }
        break;
        case RECORDING_BTN_ID:
            if(f_factory_testing->horn_flag == true)
            {
                f_factory_testing->horn_flag = false;
                f_factory_testing->count_num = 0;
                if (sys_cb.mp3_res_playing)
                {
                    music_control(MUSIC_MSG_STOP);
                }
            }
            else
            {
                f_factory_testing->horn_flag = true;
            }
            break;
    }
}
#endif

static void func_mode_mic_speaker_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            uteModuleMicRecordFactoryExit();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            uteModuleMicRecordFactoryExit();
        }
        break;
        case RECORDING_BTN_ID:
            if(uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_IDLE)
            {
                uteModuleMicRecordFactoryStart();
            }
            break;
    }
}

#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
static void func_factory_testing_heart_light_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    switch(id)
    {
        case FALL_ID:
        {
            uteModuleHeartStopSingleTesting(TYPE_FACTORY0);
        }
        break;
        case PASS_ID:
        {
            uteModuleHeartStopSingleTesting(TYPE_FACTORY0);
        }
        break;
        default:
        {
            if (uteModuleNewFactoryTestGetCheckLightMode() == NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK)
            {
                uteModuleNewFactoryTestSetCheckLightMode(NEW_FACTORY_VCXX_TEST_MODE_INFRARED);
            }
            else
            {
                uteModuleNewFactoryTestSetCheckLightMode(NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK);
            }
        }
        break;
    }
}
#endif

///工厂测试功能消息处理
static void func_factory_testing_message(size_msg_t msg)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
        {
            if (test_data->moduleType == FACTORY_MODULE_TP)
            {
                func_mode_tp_click();
            }
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
            else if (test_data->moduleType == FACTORY_MODULE_CROSS)
            {
                func_factory_testing_pass_fail_pop_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_RGB)
            {
                func_mode_rgb_click();
            }
#endif
            else if (test_data->moduleType == FACTORY_MODULE_MOTOR)
            {
                func_mode_motor_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_MIC_SPEAKER)
            {
                func_mode_mic_speaker_click();
            }
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
            else if (test_data->moduleType == FACTORY_MODULE_SPEAKER)
            {
                func_mode_test_ring_click();
            }
#endif
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
            else if (test_data->moduleType == FACTORY_MODULE_HEART_CHECK_LIGHT)
            {
                func_factory_testing_heart_light_click();
            }
#endif
            func_factory_testing_pass_fail_bnt_click();
        }
        break;
        case MSG_CTP_TOUCH:
#if UTE_MODULE_NEW_FACTORY_MODULE_3x3_TP_TOUCH_TEST_SUPPORT  //3*3 = 9宫格TP触摸测试
            f_factory_testing->touch_flag = true;
#endif
            if (test_data->moduleType == FACTORY_MODULE_MAX)
            {
                compo_page_move_touch_handler(f_factory_testing->ptm);
            }
            break;
        case K_BACK:
        {
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                compo_textbox_t *textbox1 = compo_getobj_byid(KEY_TXT_ID_0);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
        }
        break;
        case K_LEFT:
        {
#if UTE_DRV_PWRKEY_MAX_CNT > 1
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                compo_textbox_t *textbox1 = compo_getobj_byid(KEY_TXT_ID_1);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
#endif
        }
        break;
        case K_RIGHT:
        {
#if UTE_DRV_PWRKEY_MAX_CNT > 2
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                compo_textbox_t *textbox1 = compo_getobj_byid(KEY_TXT_ID_2);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
#endif
        }
        break;

#if DRV_ENCODER_KEYS_SUPPORT
        case MSG_QDEC_FORWARD: // 向前滚动菜单
        {
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                msg_queue_detach(MSG_QDEC_FORWARD, 0);
                compo_textbox_t *textbox1 = compo_getobj_byid(QDEC_FORWARD_TXT_ID);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
            break;
        }

        case MSG_QDEC_BACKWARD: // 向后滚动菜单
        {
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                msg_queue_detach(MSG_QDEC_BACKWARD, 0);
                compo_textbox_t *textbox1 = compo_getobj_byid(QDEC_BACKWARD_TXT_ID);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
            break;
        }
#endif

        case KL_BACK:
        {
            uint8_t ret = msgbox("Exit the current test?", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if (ret == MSGBOX_RES_OK)
            {
#if UTE_MODULE_LOG_SUPPORT
                uteTaskGuiStartScreen(FUNC_TEST_MODE, 0, __func__);
                uteTaskGuiStackRemoveScreenId(FUNC_FACTORY_TESTING);
#else
                uteModulePlatformSystemReboot();
#endif
            }
        }
        break;

        default:
            evt_message(msg);
            break;
    }
}
///*事件监听*/

///*数据刷新*/
static void func_mode_heart_process(void)
{
//    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    char txt_buf[50];

    if(uteModuleHeartIsWear())
    {
        compo_textbox_t *textbox2 = compo_getobj_byid(HEART_TXT_2_ID);
        snprintf((char *)txt_buf, sizeof(txt_buf), "Heart: detecting");
        compo_textbox_set(textbox2, txt_buf);

        compo_textbox_t *textbox3 = compo_getobj_byid(HEART_TXT_3_ID);
        snprintf((char *)txt_buf, sizeof(txt_buf), "%d BPM",uteModuleHeartGetHeartValue());
        compo_textbox_set(textbox3,txt_buf);
    }
    else
    {
        compo_textbox_t *textbox2 = compo_getobj_byid(HEART_TXT_2_ID);
        snprintf((char *)txt_buf, sizeof(txt_buf), "Heart: Not worn");
        compo_textbox_set(textbox2, txt_buf);
    }
}

#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
static void func_mode_blood_oxygen_process(void)
{
    char txt_buf[50];

    if(uteModuleBloodoxygenIsWear())
    {
        compo_textbox_t *textbox1 = compo_getobj_byid(BLOOD_OXYGEN_TXT_1_ID);
        compo_textbox_t *textbox2 = compo_getobj_byid(BLOOD_OXYGEN_TXT_2_ID);

        snprintf((char *)txt_buf, sizeof(txt_buf), "BO: detecting");
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

        snprintf((char *)txt_buf, sizeof(txt_buf), "BO: Not worn");
        compo_textbox_set(textbox1, txt_buf);
    }
}
#endif

#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
static void func_factory_testing_heart_light_process(void)
{
    int preV;
    int curV;
    int psV;
    bool isPass;
    compo_textbox_t *TextTitle = compo_getobj_byid(LEAKAGE_TXT_1_ID);
    compo_textbox_t *TextPreV = compo_getobj_byid(LEAKAGE_TXT_2_ID);
    compo_textbox_t *TextCurV = compo_getobj_byid(LEAKAGE_TXT_3_ID);
    compo_textbox_t *TextPsV = compo_getobj_byid(LEAKAGE_TXT_4_ID);
    compo_textbox_t *TextIsPass = compo_getobj_byid(LEAKAGE_TXT_5_ID);
    char txt_buf[50];

    if (uteModuleNewFactoryTestGetCheckLightMode() == NEW_FACTORY_VCXX_TEST_MODE_INFRARED)
    {
        memset(txt_buf, '\0', sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof("Leakage test:infrared"), "Leakage test:infrared");
        compo_textbox_set(TextTitle, txt_buf);
    }
    else
    {
        memset(txt_buf, '\0', sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof("Leakage test:green light"), "Leakage test:green light");
        compo_textbox_set(TextTitle, txt_buf);
    }

    uteModuleNewFactoryTestGetVkData(&preV, &curV, &psV, &isPass);

    snprintf((char *)txt_buf, sizeof(txt_buf), "preV:%d",preV);
    compo_textbox_set(TextPreV, txt_buf);
    snprintf((char *)txt_buf, sizeof(txt_buf), "curV:%d",curV);
    compo_textbox_set(TextCurV, txt_buf);
    snprintf((char *)txt_buf, sizeof(txt_buf), "psV:%d",psV);
    compo_textbox_set(TextPsV, txt_buf);
    if(isPass)
    {
        snprintf((char *)txt_buf, sizeof(txt_buf), "%s","PASS");
        compo_textbox_set_forecolor(TextIsPass, COLOR_GREEN);
    }
    else
    {
        snprintf((char *)txt_buf, sizeof(txt_buf), "%s","FAIL");
        compo_textbox_set_forecolor(TextIsPass, COLOR_RED);
    }
    compo_textbox_set(TextIsPass, txt_buf);
}
#endif

static void func_mode_gsensor_process(void)
{
    char txt_buf[50];
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt, NULL, NULL); /// 获取步数

    if (tick_get() % 500 == 0)
    {
        ute_drv_gsensor_common_axis_data_t *data = NULL;
        uteDrvGsensorCommonReadFifo(&data);

        int16_t *x = (int16_t *)uteModulePlatformMemoryAlloc(UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
        int16_t *y = (int16_t *)uteModulePlatformMemoryAlloc(UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
        int16_t *z = (int16_t *)uteModulePlatformMemoryAlloc(UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));

        if (x && y && z)
        {
            memset(x, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
            memset(y, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
            memset(z, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));

            uint8_t frameCnt = 0;

            frameCnt = data->frameCnt;
            ute_drv_gsensor_common_axis_bit_change_t axisBitChange;
            axisBitChange.inputXaxis = &data->accXaxis[0];
            axisBitChange.inputYaxis = &data->accYaxis[0];
            axisBitChange.inputZaxis = &data->accZaxis[0];
            axisBitChange.outputXaxis = &x[0];
            axisBitChange.outputYaxis = &y[0];
            axisBitChange.outputZaxis = &z[0];
            uteDrvGsensorCommonXYZaxisDataBitChange(&axisBitChange, frameCnt, GSENSOR_DATA_BIT_STEP);

            compo_textbox_t *textbox1 = compo_getobj_byid(ANGLE_TXT_1_ID); // 步数
            compo_textbox_t *textbox2 = compo_getobj_byid(ANGLE_TXT_2_ID); // X轴
            compo_textbox_t *textbox3 = compo_getobj_byid(ANGLE_TXT_3_ID); // Y轴
            compo_textbox_t *textbox4 = compo_getobj_byid(ANGLE_TXT_4_ID); // Z轴

            snprintf(txt_buf, sizeof(txt_buf), "%ld", totalStepCnt);
            compo_textbox_set(textbox1, txt_buf);
            snprintf(txt_buf, sizeof(txt_buf), "X:%d", *axisBitChange.outputXaxis);
            compo_textbox_set(textbox2, txt_buf);
            snprintf(txt_buf, sizeof(txt_buf), "Y:%d", *axisBitChange.inputYaxis);
            compo_textbox_set(textbox3, txt_buf);
            snprintf(txt_buf, sizeof(txt_buf), "Z:%d", *axisBitChange.outputZaxis);
            compo_textbox_set(textbox4, txt_buf);
        }

        uteModulePlatformMemoryFree(x);
        uteModulePlatformMemoryFree(y);
        uteModulePlatformMemoryFree(z);
    }
}
static void func_mode_charging_process(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(CHARGE_TXT_ID);///充电状态

    if(uteDrvBatteryCommonGetChargerStatus() != BAT_STATUS_NO_CHARGE) ///获取充电状态
    {
        compo_textbox_set(textbox, "Battery: charging");
        compo_textbox_set_forecolor(textbox, COLOR_GREEN);
    }
    else
    {
        compo_textbox_set_forecolor(textbox, COLOR_RED);
        compo_textbox_set(textbox, "Battery: uncharged");
    }

}

static void func_mode_mic_speaker_process(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(TAPE_TXT_ID);
    static bool isNeedPlay = false;
    if (textbox == NULL)
        return;
    if (uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_IDLE)
    {
        compo_textbox_set(textbox, "Click recording");
    }
    else if (uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_RECORDING)
    {
        compo_textbox_set(textbox, "In the recording...");
        isNeedPlay = true;
    }
    else if (uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_RECORDED)
    {
        if (isNeedPlay)
        {
            compo_textbox_set(textbox, "Playing...");
            uteModuleMicRecordFactorySetrecordState(FACTORY_TEST_RECORD_PLAYING);
            // uteModuleMicRecordFactoryPlayStart();
            // isNeedPlay = false;
        }
    }
    else if (uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_PLAYING)
    {
        compo_textbox_set(textbox, "Playing...");
        if (isNeedPlay)
        {
            uteModuleMicRecordFactoryPlay();
            isNeedPlay = false;
        }
    }
}

#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
static void func_mode_test_ring_process(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(TAPE_TXT_ID);
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    if(f_factory_testing->horn_flag == true)
    {
        compo_textbox_set(textbox, "点击暂停播放");
        if(tick_check_expire(f_factory_testing->tick, 1000))
        {
            f_factory_testing->tick = tick_get();
#ifdef UTE_MODULE_NEW_FACTORY_TEST_RING_MP3_ADDR_SUPPORT
            if(f_factory_testing->count_num == 0)
            {
                func_bt_mp3_res_play(UTE_MODULE_NEW_FACTORY_TEST_RING_MP3_ADDR_SUPPORT, UTE_MODULE_NEW_FACTORY_TEST_RING_MP3_LEN_SUPPORT);
            }

            if(++f_factory_testing->count_num==UTE_MODULE_NEW_FACTORY_TEST_RING_MP3_TIMER_SUPPORT)
            {
                f_factory_testing->count_num = 0;
            }
#else
            if(f_factory_testing->count_num == 0)
            {
                func_bt_mp3_res_play(RES_BUF_RING_REDIAL_MP3, RES_LEN_RING_REDIAL_MP3);
            }

            if(++f_factory_testing->count_num==13)
            {
                f_factory_testing->count_num = 0;
            }
#endif
        }
    }
    else
    {
        compo_textbox_set(textbox, "点击开始播放");
    }
}
#endif

#else
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
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    if(uteModuleHeartGetWorkMode() == HR_WORK_MODE_HR)
    {
        uteModuleHeartStopSingleTesting(TYPE_HEART);
    }
    else if (uteModuleHeartGetWorkMode() == HR_WORK_MODE_SPO2)
    {
        uteModuleHeartStopSingleTesting(TYPE_BLOODOXYGEN);
    }

#if UTE_MODULE_NEW_FACTORY_TEST_GM_SUPPORT
    if (test_data->moduleType == FACTORY_MODULE_GM)
    {
        uteModuleCompassStart();
    }
    else if(uteModulecompassOnoff())
    {
        uteModuleCompassStop();
    }
#endif

    if(f_factory_testing->motor_flag)
    {
        f_factory_testing->motor_flag = false;
        uteDrvMotorDisable();
    }

    if (test_data->moduleType == FACTORY_MODULE_VERSION)
    {
        frm = func_factory_testing_drv_info();
    }
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
    else if (test_data->moduleType == FACTORY_MODULE_CROSS)
    {
        frm = func_factory_testing_cross();
    }
    else if (test_data->moduleType == FACTORY_MODULE_RGB)
    {
        frm = func_factory_testing_rgb();
    }
    else if (test_data->moduleType == FACTORY_MODULE_CHARGING)
    {
        frm = func_factory_testing_charging();
    }
#endif
    else if (test_data->moduleType == FACTORY_MODULE_TP)
    {
        frm = func_factory_testing_tp();
    }
    else if (test_data->moduleType == FACTORY_MODULE_HEART)
    {
        frm = func_factory_testing_heart();
    }
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
    else if (test_data->moduleType == FACTORY_MODULE_HEART_CHECK_LIGHT)
    {
        frm = func_factory_testing_heart_light();
    }
#endif
#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
    else if (test_data->moduleType == FACTORY_MODULE_SPO2)
    {
        frm = func_factory_testing_blood_oxygen();
    }
#endif
    else if (test_data->moduleType == FACTORY_MODULE_GSENSOR)
    {
        frm = func_factory_testing_gsensor();
    }
    else if (test_data->moduleType == FACTORY_MODULE_MOTOR)
    {
        frm = func_factory_testing_motor();
    }
    else if (test_data->moduleType == FACTORY_MODULE_MIC_SPEAKER)
    {
        frm = func_factory_testing_mic_speaker();
    }
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
    else if (test_data->moduleType == FACTORY_MODULE_SPEAKER)
    {
        f_factory_testing->horn_flag = true;
        f_factory_testing->tick = tick_get();
        frm = func_factory_testing_ring();
    }
#endif
#if UTE_MODULE_NEW_FACTORY_TEST_GM_SUPPORT
    else if (test_data->moduleType == FACTORY_MODULE_GM)
    {
        f_factory_testing->tick = tick_get();
        frm = func_factory_testing_gm();
    }
#endif
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

    uint16_t page_size = SIDEBAR_PAGE_HEIGHT*(GUI_SCREEN_HEIGHT/9);          //分页大小
#if (UTE_DRV_SCREEN_SHAPE == 0)
    page_size = page_size+(GUI_SCREEN_HEIGHT/7.2)*2;
#endif

    page_move_info_t info =
    {
        .page_size = page_size,
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
    f_factory_testing->motor_flag =false;///初始化测试马达标志位

    uteModuleNewFactoryTestResetParam();
    uteModuleNewFactoryTestSetMode(&test_data);
    test_data->mode = FACTORY_TEST_MODE_FACTORY;
}

static void func_factory_testing_pass_fail_bnt_click(void)
{
    int id = compo_get_button_id();

    if(id == PASS_ID || id == FALL_ID)
    {
        if(id == PASS_ID)
        {
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;
        }
        else
        {
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;
        }
        test_data->moduleType ++; //切换下一个模式

        compo_form_t *frm = func_cb.frm_main;
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        func_cb.frm_main = func_factory_testing_create();
    }
}

static void func_factory_testing_pass_fail_pop_click(void) //弹窗
{
    u8 ret = msgbox((char *)"是否通过？", NULL, NULL, MSGBOX_MODE_BTN_FACTORR, MSGBOX_MSG_TYPE_NONE);

    if(ret == MSGBOX_RES_OK)
    {
        test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;
        test_data->moduleType ++;//切换下一个模式

        compo_form_t *frm = func_cb.frm_main;
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        func_cb.frm_main = func_factory_testing_create();
    }
    else if(ret == MSGBOX_RES_CANCEL)
    {
        test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_FAIL;
        test_data->moduleType ++;//切换下一个模式

        compo_form_t *frm = func_cb.frm_main;
        if (frm != NULL)
        {
            compo_form_destroy(frm);
            frm = NULL;
        }
        func_cb.frm_main = func_factory_testing_create();
    }
}

//创建PASS FAIL按钮
static void func_factory_testing_pass_fail_bnt_create(compo_form_t *frm)
{
    compo_textbox_t *textbox;

    textbox = compo_textbox_create(frm, strlen((const char *)"FALL")); // FALL
    compo_textbox_set(textbox, (const char *)"FALL");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X - GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_HEIGHT * 5 / 6);
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    rect_t location_data = widget_get_location(textbox->txt);
    area_t txt_data = widget_text_get_area(textbox->txt);

    compo_button_t *btn = compo_button_create(frm); // 按钮FALL
    compo_button_set_location(btn,location_data.x, location_data.y,txt_data.wid,txt_data.hei*2.5);
    compo_setid(btn, FALL_ID);

    textbox = compo_textbox_create(frm, strlen((const char *)"PASS")); // PASS
    compo_textbox_set(textbox, (const char *)"PASS");
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X + GUI_SCREEN_CENTER_X / 2, GUI_SCREEN_HEIGHT * 5 / 6);
    compo_textbox_set_forecolor(textbox, COLOR_GREEN);
    location_data = widget_get_location(textbox->txt);
    txt_data = widget_text_get_area(textbox->txt);

    btn = compo_button_create(frm); // 按钮PASS
    compo_button_set_location(btn,location_data.x, location_data.y,txt_data.wid,txt_data.hei*2.5);
    compo_setid(btn, PASS_ID);
}

//创建版本测试窗体   模式一*/
compo_form_t *func_factory_testing_drv_info(void)
{
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, strlen((const char *)"设备名称")); // 设备名称
    compo_textbox_set(textbox, (const char *)"设备名称");
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

    func_factory_testing_pass_fail_bnt_create(frm);

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

//创建心率漏光窗体  /
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
compo_form_t *func_factory_testing_heart_light(void)
{
    uteModuleNewFactoryTestSetCheckLightMode(NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK);
    // 新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];
    compo_textbox_t *textbox = NULL;

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof("漏光测试:绿灯"), "漏光测试:绿灯");
    textbox = compo_textbox_create(frm, strlen(txt_buf)); // dif___, Max___
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 1 / 7);
    compo_setid(textbox, LEAKAGE_TXT_1_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "preV:000");
    textbox = compo_textbox_create(frm, strlen(txt_buf)); /// 心率:未佩戴
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 7);
    compo_setid(textbox, LEAKAGE_TXT_2_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "curV:000");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 7);
    compo_setid(textbox, LEAKAGE_TXT_3_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "psV:000");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 7);
    compo_setid(textbox, LEAKAGE_TXT_4_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "fail");
    textbox = compo_textbox_create(frm, strlen(txt_buf));
    compo_textbox_set(textbox, txt_buf);
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 5 / 7);
    compo_setid(textbox, LEAKAGE_TXT_5_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
#endif

///创建心率测试窗体   模式六/
compo_form_t * func_factory_testing_heart(void)
{
    uteModuleHeartStartSingleTesting(TYPE_HEART);
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];
    compo_textbox_t *textbox = NULL;

#if 0//UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
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

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
///创建血氧测试窗体   模式七/
#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
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

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
#endif

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

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
///创建按键测试窗体   模式九*/
compo_form_t * func_factory_testing_key(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
//    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("按键测试"));
    compo_textbox_set(textbox, "按键测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 1 / 8 - MODE_ONE_INTIAL_SPACING_Y);

    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 2 / 8, GUI_SCREEN_WIDTH/3, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "按键: 0");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,KEY_TXT_ID_0);

#if UTE_DRV_PWRKEY_MAX_CNT > 1
    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 3 / 8, GUI_SCREEN_WIDTH/3, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "按键: 1");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,KEY_TXT_ID_1);
#endif

#if UTE_DRV_PWRKEY_MAX_CNT > 2
    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 4 / 8, GUI_SCREEN_WIDTH/3, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "按键: 2");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,KEY_TXT_ID_2);
#endif

#if DRV_ENCODER_KEYS_SUPPORT
    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 5 / 8, GUI_SCREEN_WIDTH/2, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "编码器: 上旋");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,QDEC_BACKWARD_TXT_ID);

    textbox = compo_textbox_create(frm, 20);
    compo_textbox_set_location(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_HEIGHT * 6 / 8, GUI_SCREEN_WIDTH/2, GUI_SCREEN_HEIGHT/5);
    compo_textbox_set_multiline(textbox, true);
    compo_textbox_set(textbox, "编码器: 下旋");
    compo_textbox_set_forecolor(textbox, COLOR_RED);
    compo_setid(textbox,QDEC_FORWARD_TXT_ID);
#endif

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
///创建马达测试窗体   模式十*/
compo_form_t * func_factory_testing_motor(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
//    compo_button_t *btn;

    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    f_factory_testing->motor_flag = false;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("马达测试"));
    compo_textbox_set(textbox, "马达测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("震动关闭"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "震动关闭");
    compo_setid(textbox,MOTOR_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, MOTOR_BTN_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}

///创建充电测试窗体   模式十一*/
compo_form_t * func_factory_testing_charging(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
//    compo_button_t *btn;

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("充电测试"));
    compo_textbox_set(textbox, "充电测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("电池:未充电"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "电池:未充电");
    compo_setid(textbox,CHARGE_TXT_ID);
    compo_textbox_set_forecolor(textbox, COLOR_RED);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
///创建咪喇叭测试窗体   模式十二*/
compo_form_t * func_factory_testing_mic_speaker(void)
{
    uteModuleMicRecordFactoryEnter();
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("咪头喇叭测试"));
    compo_textbox_set(textbox, "咪头喇叭测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("点击开始录音"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "点击开始录音");
    compo_setid(textbox,TAPE_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

    compo_button_t *btn = compo_button_create(frm); // 录音
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, RECORDING_BTN_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}

#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
///创建音频测试窗体   模式十二*/
compo_form_t * func_factory_testing_ring(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("音频测试"));
    compo_textbox_set(textbox, "音频测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_SPACING_Y);

    textbox = compo_textbox_create(frm, strlen("点击暂停播放"));
    compo_textbox_set_pos(textbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y);
    compo_textbox_set(textbox, "点击暂停播放");
    compo_setid(textbox,TAPE_TXT_ID);
    compo_textbox_set_forecolor(textbox, make_color(0,191,255));

    compo_button_t *btn = compo_button_create(frm); // 按键
    compo_button_set_location(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y+MODE_ONE_SPACING_Y, (GUI_SCREEN_WIDTH / 2), widget_text_get_area(textbox->txt).hei * 2);
    compo_setid(btn, RECORDING_BTN_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
#endif
///创建地磁测试窗体 /
#if UTE_MODULE_NEW_FACTORY_TEST_GM_SUPPORT
compo_form_t * func_factory_testing_gm(void)
{
    ///新建窗体
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[50];

    compo_textbox_t *textbox = compo_textbox_create(frm, strlen("地磁测试"));
    compo_textbox_set(textbox, "地磁测试");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, MODE_ONE_SPACING_Y);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "0°");
    textbox = compo_textbox_create(frm, 5);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  MODE_ONE_SPACING_Y+MODE_ONE_INTIAL_SPACING_Y*2);
    compo_setid(textbox,ANGLE_TXT_DATA_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "地磁质量:0");
    textbox = compo_textbox_create(frm, 9);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y*4.5);
    compo_setid(textbox,GM_TXT_DATA_ID);

    textbox = compo_textbox_create(frm, strlen("G-sensor"));
    compo_textbox_set(textbox, "G-sensor");
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y*2.5);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "X:0");
    textbox = compo_textbox_create(frm, 9);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y-MODE_ONE_INTIAL_SPACING_Y/2);
    compo_setid(textbox,ANGLE_TXT_X_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "Y:0");
    textbox = compo_textbox_create(frm, 9);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+MODE_ONE_INTIAL_SPACING_Y*1.5);
    compo_setid(textbox,ANGLE_TXT_Y_ID);

    memset(txt_buf, '\0', sizeof(txt_buf));
    snprintf((char *)txt_buf, sizeof(txt_buf), "Z:0");
    textbox = compo_textbox_create(frm, 9);
    compo_textbox_set(textbox,txt_buf);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X,  GUI_SCREEN_CENTER_Y+MODE_ONE_INTIAL_SPACING_Y*3.5);
    compo_setid(textbox,ANGLE_TXT_Z_ID);

    func_factory_testing_pass_fail_bnt_create(frm);

    return frm;
}
#endif
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
            compo_textbox_set(textbox, (const char *)"PASS");
            compo_textbox_set_forecolor(textbox, COLOR_GREEN);
        }
        else
        {
            compo_textbox_set(textbox, (const char *)"FALL");
            compo_textbox_set_forecolor(textbox, COLOR_RED);
        }
        compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/3,RESULT_Y*i+MODE_ONE_INTIAL_SPACING_Y-widget_text_get_area(textbox->txt).hei/3);

    }

    func_result_long_pic();
#if (UTE_DRV_SCREEN_SHAPE == 0)
    widget_page_set_client(frm->page,0,(GUI_SCREEN_HEIGHT/7.2));
#endif

    return frm;

}

///*事件监听*/

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
        default:
            func_factory_testing_pass_fail_pop_click();
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
        func_factory_testing_pass_fail_pop_click();
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
#if UTE_MODULE_NEW_FACTORY_MODULE_3x3_TP_TOUCH_TEST_SUPPORT
static void func_mode_tp_touch(void)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;

    if(f_factory_testing->no_touch_flag == false)
    {
        s32 dx,dy;
        f_factory_testing->no_touch_flag = ctp_get_dxy(&dx,&dy);
    }

    if(f_factory_testing->touch_flag == true && f_factory_testing->no_touch_flag == true)
    {
        s32 dx,dy,x,y;
        f_factory_testing->touch_flag = ctp_get_cur_point(&dx,&dy,&x,&y);

        int id = ID_NULL;
        for (int i = SHAPE_1_ID; i <= SHAPE_9_ID; i++)
        {
            rect_t rect = compo_shape_get_location(compo_getobj_byid(i));
            if (abs_s(x - rect.x) * 2 <= rect.wid && abs_s(y - rect.y) * 2 <= rect.hei)
            {
                id = i;
                break;
            }
        }
        if(id<SHAPE_1_ID || id>SHAPE_9_ID)
        {
            return;
        }
        compo_shape_t *shape = compo_getobj_byid(id);
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
            test_data->moduleResult[test_data->moduleType] = MODULE_TEST_RESULT_PASS;
            test_data->moduleType ++;//切换下一个模式
            compo_form_t *frm = func_cb.frm_main;
            if (frm != NULL)
            {
                compo_form_destroy(frm);
                frm = NULL;
            }
            func_cb.frm_main = func_factory_testing_create();
            return;
        }

        f_factory_testing->tp_test[id-SHAPE_1_ID] = true;
        compo_shape_set_color(shape, COLOR_GREEN );
    }
}
#endif
static void func_mode_motor_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    compo_textbox_t *textbox = compo_getobj_byid(MOTOR_TXT_ID);///马达震动

    if(id == MOTOR_BTN_ID)
    {
        if(!f_factory_testing->motor_flag)
        {
            f_factory_testing->motor_flag = true;
            compo_textbox_set(textbox, (const char *)"震动开启");
            uteDrvMotorEnable(); //开启马达
        }
        else
        {
            f_factory_testing->motor_flag = false;
            compo_textbox_set(textbox, (const char *)"震动关闭");
            uteDrvMotorDisable(); //关闭马达
        }
    }
}

#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
static void func_mode_test_ring_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            if (sys_cb.mp3_res_playing)
            {
                music_control(MUSIC_MSG_STOP);
            }
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            if (sys_cb.mp3_res_playing)
            {
                music_control(MUSIC_MSG_STOP);
            }
        }
        break;
        case RECORDING_BTN_ID:
            if(f_factory_testing->horn_flag == true)
            {
                f_factory_testing->horn_flag = false;
                f_factory_testing->count_num = 0;
                if (sys_cb.mp3_res_playing)
                {
                    music_control(MUSIC_MSG_STOP);
                }
            }
            else
            {
                f_factory_testing->horn_flag = true;
            }
            break;
    }
}
#endif

static void func_mode_mic_speaker_click(void)
{
    int id = compo_get_button_id();

    switch(id)
    {
        case FALL_ID: ///不通过后切换下一个模式
        {
            uteModuleMicRecordFactoryExit();
        }
        break;
        case PASS_ID: ///通过后切换下一个模式
        {
            uteModuleMicRecordFactoryExit();
        }
        break;
        case RECORDING_BTN_ID:
            if(uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_IDLE)
            {
                uteModuleMicRecordFactoryStart();
            }
            break;
    }
}

#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
static void func_factory_testing_heart_light_click(void)
{
    int id = compo_get_button_id();
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    switch(id)
    {
        case FALL_ID:
        {
            uteModuleHeartStopSingleTesting(TYPE_FACTORY0);
        }
        break;
        case PASS_ID:
        {
            uteModuleHeartStopSingleTesting(TYPE_FACTORY0);
        }
        break;
        default:
        {
            if (uteModuleNewFactoryTestGetCheckLightMode() == NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK)
            {
                uteModuleNewFactoryTestSetCheckLightMode(NEW_FACTORY_VCXX_TEST_MODE_INFRARED);
            }
            else
            {
                uteModuleNewFactoryTestSetCheckLightMode(NEW_FACTORY_VCXX_TEST_MODE_CROSSTALK);
            }
        }
        break;
    }
}
#endif

///工厂测试功能消息处理
static void func_factory_testing_message(size_msg_t msg)
{
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_CLICK:
        {
            if (test_data->moduleType == FACTORY_MODULE_TP)
            {
                func_mode_tp_click();
            }
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
            else if (test_data->moduleType == FACTORY_MODULE_CROSS)
            {
                func_factory_testing_pass_fail_pop_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_RGB)
            {
                func_mode_rgb_click();
            }
#endif
            else if (test_data->moduleType == FACTORY_MODULE_MOTOR)
            {
                func_mode_motor_click();
            }
            else if (test_data->moduleType == FACTORY_MODULE_MIC_SPEAKER)
            {
                func_mode_mic_speaker_click();
            }
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
            else if (test_data->moduleType == FACTORY_MODULE_SPEAKER)
            {
                func_mode_test_ring_click();
            }
#endif
#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
            else if (test_data->moduleType == FACTORY_MODULE_HEART_CHECK_LIGHT)
            {
                func_factory_testing_heart_light_click();
            }
#endif
            func_factory_testing_pass_fail_bnt_click();
        }
        break;
        case MSG_CTP_TOUCH:
#if UTE_MODULE_NEW_FACTORY_MODULE_3x3_TP_TOUCH_TEST_SUPPORT  //3*3 = 9宫格TP触摸测试
            f_factory_testing->touch_flag = true;
#endif
            if (test_data->moduleType == FACTORY_MODULE_MAX)
            {
                compo_page_move_touch_handler(f_factory_testing->ptm);
            }
            break;
        case K_BACK:
        {
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                compo_textbox_t *textbox1 = compo_getobj_byid(KEY_TXT_ID_0);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
        }
        break;
        case K_LEFT:
        {
#if UTE_DRV_PWRKEY_MAX_CNT > 1
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                compo_textbox_t *textbox1 = compo_getobj_byid(KEY_TXT_ID_1);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
#endif
        }
        break;
        case K_RIGHT:
        {
#if UTE_DRV_PWRKEY_MAX_CNT > 2
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                compo_textbox_t *textbox1 = compo_getobj_byid(KEY_TXT_ID_2);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
#endif
        }
        break;

#if DRV_ENCODER_KEYS_SUPPORT
        case MSG_QDEC_FORWARD: // 向前滚动菜单
        {
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                msg_queue_detach(MSG_QDEC_FORWARD, 0);
                compo_textbox_t *textbox1 = compo_getobj_byid(QDEC_FORWARD_TXT_ID);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
            break;
        }

        case MSG_QDEC_BACKWARD: // 向后滚动菜单
        {
            if (test_data->moduleType == FACTORY_MODULE_KEY)
            {
                msg_queue_detach(MSG_QDEC_BACKWARD, 0);
                compo_textbox_t *textbox1 = compo_getobj_byid(QDEC_BACKWARD_TXT_ID);
                compo_textbox_set_forecolor(textbox1, COLOR_GREEN);
            }
            break;
        }
#endif

        case KL_BACK:
        {
            uint8_t ret = msgbox("退出当前测试？", NULL, NULL, MSGBOX_MODE_BTN_OKCANCEL, MSGBOX_MSG_TYPE_NONE);
            if (ret == MSGBOX_RES_OK)
            {
#if UTE_MODULE_LOG_SUPPORT
                uteTaskGuiStartScreen(FUNC_TEST_MODE, 0, __func__);
                uteTaskGuiStackRemoveScreenId(FUNC_FACTORY_TESTING);
#else
                uteModulePlatformSystemReboot();
#endif
            }
        }
        break;

        default:
            evt_message(msg);
            break;
    }
}
///*事件监听*/

///*数据刷新*/
static void func_mode_heart_process(void)
{
//    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
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

#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
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
#endif

#if UTE_MODULE_NEW_FACTORY_MODULE_HEART_CHECK_LIGHT_SUPPORT
static void func_factory_testing_heart_light_process(void)
{
    int preV;
    int curV;
    int psV;
    bool isPass;
    compo_textbox_t *TextTitle = compo_getobj_byid(LEAKAGE_TXT_1_ID);
    compo_textbox_t *TextPreV = compo_getobj_byid(LEAKAGE_TXT_2_ID);
    compo_textbox_t *TextCurV = compo_getobj_byid(LEAKAGE_TXT_3_ID);
    compo_textbox_t *TextPsV = compo_getobj_byid(LEAKAGE_TXT_4_ID);
    compo_textbox_t *TextIsPass = compo_getobj_byid(LEAKAGE_TXT_5_ID);
    char txt_buf[50];

    if (uteModuleNewFactoryTestGetCheckLightMode() == NEW_FACTORY_VCXX_TEST_MODE_INFRARED)
    {
        memset(txt_buf, '\0', sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof("漏光测试:红外"), "漏光测试:红外");
        compo_textbox_set(TextTitle, txt_buf);
    }
    else
    {
        memset(txt_buf, '\0', sizeof(txt_buf));
        snprintf((char *)txt_buf, sizeof("漏光测试:绿灯"), "漏光测试:绿灯");
        compo_textbox_set(TextTitle, txt_buf);
    }

    uteModuleNewFactoryTestGetVkData(&preV, &curV, &psV, &isPass);

    snprintf((char *)txt_buf, sizeof(txt_buf), "preV:%d",preV);
    compo_textbox_set(TextPreV, txt_buf);
    snprintf((char *)txt_buf, sizeof(txt_buf), "curV:%d",curV);
    compo_textbox_set(TextCurV, txt_buf);
    snprintf((char *)txt_buf, sizeof(txt_buf), "psV:%d",psV);
    compo_textbox_set(TextPsV, txt_buf);
    if(isPass)
    {
        snprintf((char *)txt_buf, sizeof(txt_buf), "%s","PASS");
        compo_textbox_set_forecolor(TextIsPass, COLOR_GREEN);
    }
    else
    {
        snprintf((char *)txt_buf, sizeof(txt_buf), "%s","FAIL");
        compo_textbox_set_forecolor(TextIsPass, COLOR_RED);
    }
    compo_textbox_set(TextIsPass, txt_buf);
}
#endif

static void func_mode_gsensor_process(void)
{
    char txt_buf[50];
    uint32_t totalStepCnt = 0;
    uteModuleSportGetCurrDayStepCnt(&totalStepCnt, NULL, NULL); /// 获取步数

    if (tick_get() % 500 == 0)
    {
        ute_drv_gsensor_common_axis_data_t *data = NULL;
        uteDrvGsensorCommonReadFifo(&data);

        int16_t *x = (int16_t *)uteModulePlatformMemoryAlloc(UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
        int16_t *y = (int16_t *)uteModulePlatformMemoryAlloc(UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
        int16_t *z = (int16_t *)uteModulePlatformMemoryAlloc(UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));

        if (x && y && z)
        {
            memset(x, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
            memset(y, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
            memset(z, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));

            uint8_t frameCnt = 0;

            frameCnt = data->frameCnt;
            ute_drv_gsensor_common_axis_bit_change_t axisBitChange;
            axisBitChange.inputXaxis = &data->accXaxis[0];
            axisBitChange.inputYaxis = &data->accYaxis[0];
            axisBitChange.inputZaxis = &data->accZaxis[0];
            axisBitChange.outputXaxis = &x[0];
            axisBitChange.outputYaxis = &y[0];
            axisBitChange.outputZaxis = &z[0];
            uteDrvGsensorCommonXYZaxisDataBitChange(&axisBitChange, frameCnt, GSENSOR_DATA_BIT_STEP);

            compo_textbox_t *textbox1 = compo_getobj_byid(ANGLE_TXT_1_ID); // 步数
            compo_textbox_t *textbox2 = compo_getobj_byid(ANGLE_TXT_2_ID); // X轴
            compo_textbox_t *textbox3 = compo_getobj_byid(ANGLE_TXT_3_ID); // Y轴
            compo_textbox_t *textbox4 = compo_getobj_byid(ANGLE_TXT_4_ID); // Z轴

            snprintf(txt_buf, sizeof(txt_buf), "%ld", totalStepCnt);
            compo_textbox_set(textbox1, txt_buf);
            snprintf(txt_buf, sizeof(txt_buf), "X:%d", *axisBitChange.outputXaxis);
            compo_textbox_set(textbox2, txt_buf);
            snprintf(txt_buf, sizeof(txt_buf), "Y:%d", *axisBitChange.inputYaxis);
            compo_textbox_set(textbox3, txt_buf);
            snprintf(txt_buf, sizeof(txt_buf), "Z:%d", *axisBitChange.outputZaxis);
            compo_textbox_set(textbox4, txt_buf);
        }

        uteModulePlatformMemoryFree(x);
        uteModulePlatformMemoryFree(y);
        uteModulePlatformMemoryFree(z);
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
    compo_textbox_t *textbox = compo_getobj_byid(TAPE_TXT_ID);
    static bool isNeedPlay = false;
    if (textbox == NULL)
        return;
    if (uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_IDLE)
    {
        compo_textbox_set(textbox, "点击开始录音");
    }
    else if (uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_RECORDING)
    {
        compo_textbox_set(textbox, "录音中...");
        isNeedPlay = true;
    }
    else if (uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_RECORDED)
    {
        if (isNeedPlay)
        {
            compo_textbox_set(textbox, "播放中...");
            uteModuleMicRecordFactorySetrecordState(FACTORY_TEST_RECORD_PLAYING);
            // uteModuleMicRecordFactoryPlayStart();
            // isNeedPlay = false;
        }
    }
    else if (uteModuleMicRecordFactoryGetRecordState() == FACTORY_TEST_RECORD_PLAYING)
    {
        compo_textbox_set(textbox, "播放中...");
        if (isNeedPlay)
        {
            uteModuleMicRecordFactoryPlay();
            isNeedPlay = false;
        }
    }
}

///地磁显示刷新 /
#if UTE_MODULE_NEW_FACTORY_TEST_GM_SUPPORT
static void func_factory_testing_gm_process(void)
{
    if (tick_get() % 500 == 0)
    {
        char txt_buf[50];
        int32_t gsensor_data[3];
        uteModuleCompassAccrawgetData(gsensor_data);
        int16_t x = gsensor_data[0];
        int16_t y = gsensor_data[1];
        int16_t z = gsensor_data[2];

        compo_textbox_t *textbox0 = compo_getobj_byid(ANGLE_TXT_DATA_ID); // 角度
        compo_textbox_t *textbox1 = compo_getobj_byid(GM_TXT_DATA_ID); // 地磁质量
        compo_textbox_t *textbox2 = compo_getobj_byid(ANGLE_TXT_X_ID); // X轴
        compo_textbox_t *textbox3 = compo_getobj_byid(ANGLE_TXT_Y_ID); // Y轴
        compo_textbox_t *textbox4 = compo_getobj_byid(ANGLE_TXT_Z_ID); // Z轴

        snprintf(txt_buf, sizeof(txt_buf), "%d°", uteModuleCompassGetAzimuth());
        compo_textbox_set(textbox0, txt_buf);

        snprintf(txt_buf, sizeof(txt_buf), "地磁质量:%d", uteModuleCompassGetAccuracy());
        compo_textbox_set(textbox1, txt_buf);
        if(x<0)
        {
            snprintf(txt_buf, sizeof(txt_buf), "X:-%d.%04d", -x/10000,-x%10000);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "X:%d.%04d", x/10000,x%10000);
        }
        compo_textbox_set(textbox2, txt_buf);

        if(y<0)
        {
            snprintf(txt_buf, sizeof(txt_buf), "Y:-%d.%04d", -y/10000,-y%10000);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "Y:%d.%04d", y/10000,y%10000);
        }
        compo_textbox_set(textbox3, txt_buf);

        if(z<0)
        {
            snprintf(txt_buf, sizeof(txt_buf), "Z:-%d.%04d", -z/10000,-z%10000);
        }
        else
        {
            snprintf(txt_buf, sizeof(txt_buf), "Z:%d.%04d", z/10000,z%10000);
        }
        compo_textbox_set(textbox4, txt_buf);
    }
}
#endif

#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
static void func_mode_test_ring_process(void)
{
    compo_textbox_t *textbox = compo_getobj_byid(TAPE_TXT_ID);
    f_factory_testing_t *f_factory_testing = (f_factory_testing_t *)func_cb.f_cb;
    if(f_factory_testing->horn_flag == true)
    {
        compo_textbox_set(textbox, "点击暂停播放");
        if(tick_check_expire(f_factory_testing->tick, 1000))
        {
            f_factory_testing->tick = tick_get();
#ifdef UTE_MODULE_NEW_FACTORY_TEST_RING_MP3_ADDR_SUPPORT
            if(f_factory_testing->count_num == 0)
            {
                func_bt_mp3_res_play(UTE_MODULE_NEW_FACTORY_TEST_RING_MP3_ADDR_SUPPORT, UTE_MODULE_NEW_FACTORY_TEST_RING_MP3_LEN_SUPPORT);
            }

            if(++f_factory_testing->count_num==UTE_MODULE_NEW_FACTORY_TEST_RING_MP3_TIMER_SUPPORT)
            {
                f_factory_testing->count_num = 0;
            }
#else
            if(f_factory_testing->count_num == 0)
            {
                func_bt_mp3_res_play(RES_BUF_RING_REDIAL_MP3, RES_LEN_RING_REDIAL_MP3);
            }

            if(++f_factory_testing->count_num==13)
            {
                f_factory_testing->count_num = 0;
            }
#endif
        }
    }
    else
    {
        compo_textbox_set(textbox, "点击开始播放");
    }
}
#endif

#endif //  SCREEN_TITLE_MULTIPLE_CHINESE_LANGUAGE_SUPPORT

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
            func_factory_testing_heart_light_process();
            break;
#endif
        case FACTORY_MODULE_HEART:
            func_mode_heart_process();
            break;
#if UTE_MODULE_BLOODOXYGEN_SUPPORT && !UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
        case FACTORY_MODULE_SPO2:
            func_mode_blood_oxygen_process();
            break;
#endif
        case FACTORY_MODULE_GSENSOR:
            func_mode_gsensor_process();
            break;
#if !UTE_MODULE_NEW_FACTORY_MODULE_REDUCE_PART_FUNCTION
        case FACTORY_MODULE_CHARGING:
            func_mode_charging_process();
            break;
#endif
        case FACTORY_MODULE_MIC_SPEAKER:
            func_mode_mic_speaker_process();
            break;
#if UTE_MODULE_NEW_FACTORY_TEST_RING_SUPPORT
        case FACTORY_MODULE_SPEAKER:
            func_mode_test_ring_process();
            break;
#endif
        case FACTORY_MODULE_TP:
#if UTE_MODULE_NEW_FACTORY_MODULE_3x3_TP_TOUCH_TEST_SUPPORT
            func_mode_tp_touch();
#endif
            break;
#if UTE_MODULE_NEW_FACTORY_TEST_GM_SUPPORT
        case FACTORY_MODULE_GM:
            func_factory_testing_gm_process();
#endif
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
