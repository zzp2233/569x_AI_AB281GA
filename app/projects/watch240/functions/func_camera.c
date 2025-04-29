#include "include.h"
#include "func.h"
#include "ute_module_sport.h"
#include "ute_module_message.h"
#include "ute_drv_motor.h"
#include "ute_application_common.h"

#if UTE_MODULE_SCREENS_CAMERA_SUPPORT


#if FUNC_CAMERA_TRANS_EN

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//组件ID
enum
{
    //按钮
    COMPO_ID_BTN_TAKE_A_PIC = 1,
    COMPO_ID_BTN_START = 1,
    COMPO_ID_PIC,
    COMPO_ID_MASK,
};

typedef struct f_picture_t_
{
    compo_shape_t *mask;
    u32 mask_tick;
    u8 phone_status;        //用于区分HID还是APP控制
} f_camera_t;

#define JPEG_WID            160
#define JPEG_HEI            192
#define JPEG_ROTATION       0       //旋转角度 0:0°，1:90°，2:180°，3:270°

#define JPEG_RX_BUF_SIZE    5*1024
#define JPEG2RGB_BUF_SIZE   JPEG_WID*JPEG_HEI*2+8

#define JPEG_LOCAL_TEST     0

#if JPEG_LOCAL_TEST
//AT(.jpeg_tbuf)
u8 t_buf1[JPEG_RX_BUF_SIZE] =
{

};
#endif

static u8 jpeg_rx_buf[2][JPEG_RX_BUF_SIZE];// AT(.jpeg_rx);
void func_camera_set(u8 status);

typedef struct
{
    u8 rgb_buf[JPEG2RGB_BUF_SIZE];
    u16 wptr;
    u8 *buf;

    bool flag;
} f_camera_jpeg_t;
f_camera_jpeg_t f_camera_jpeg;// AT(.jpeg_rgb);

//按钮
enum
{
    CAMERA_CLOSE = 0,
    CAMERA_OPEN,
    CAMERA_TAKE_PHOTOS,
    CAMERA_TRANS_START,
};

static u32 my_tick = 0;


void func_camera_jpeg_rx(u8 *buf, u16 len)
{
    if (buf[0] == 0xaa && buf[1] == 0x55)
    {
        f_camera_t *f_camera = (f_camera_t *)func_cb.f_cb;
        u8 status = buf[2];
        bool res = buf[3];
        if (res)
        {
            switch(status)
            {
                case CAMERA_OPEN:
                    f_camera->phone_status = CAMERA_OPEN;
                    printf("RECV PHONE CAMERA_OPEN\n");
                    ble_update_conn_param(20, 0, 500);     //interval: 400*1.25ms = 500ms
                    func_camera_set(CAMERA_TRANS_START);
                    break;

                case CAMERA_CLOSE:
                    f_camera->phone_status = CAMERA_CLOSE;
                    printf("RECV PHONE CAMERA_CLOSE\n");
                    func_cb.sta = func_cb.last;
                    break;

                default:
                    printf("UNDEFINED STATUS!\n");
                    break;
            }
        }

    }
    else
    {

        if (f_camera_jpeg.wptr == 0)
        {
            my_tick = tick_get();
        }

        memcpy(f_camera_jpeg.buf + f_camera_jpeg.wptr, buf, len - 1);
        f_camera_jpeg.wptr += len - 1;

        //结束
        if (buf[len - 1] == 0)
        {
            //        printf("rx tick:%d, total:%d\n", tick_get() - my_tick, f_camera_jpeg.wptr);
            f_camera_jpeg.wptr = 0;
            f_camera_jpeg.flag ^= 1;

            f_camera_jpeg.buf = jpeg_rx_buf[f_camera_jpeg.flag];
            msg_enqueue(EVT_JPEG_DECODE);

        }
    }

}

void func_camera_set(u8 status)
{

    if (!ble_is_connect())
    {
        printf("please connect ble!\n");
        return;
    }

    u8 buf[13] = {0xaa, 0x55, status, JPEG_WID & 0xff, JPEG_WID >> 8, JPEG_HEI & 0xff, JPEG_HEI >> 8, JPEG_ROTATION, JPEG_RX_BUF_SIZE & 0xff, JPEG_RX_BUF_SIZE >> 8, 0, 0, 0};
    u8 size = sizeof(buf);
    switch(status)
    {
        case CAMERA_OPEN:
            break;

        case CAMERA_TRANS_START:
        case CAMERA_TAKE_PHOTOS:
        case CAMERA_CLOSE:
            buf[3] = 0;
            size = 4;
            break;

        default:
            printf("UNDEFINED STATUS!\n");
            break;
    }
    app_protocol_tx(buf, size);
}

//单击按钮
static void func_camera_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_START:
        {
            f_camera_t *f_camera = (f_camera_t *)func_cb.f_cb;
            printf("COMPO_ID_BTN_START\n");
            func_camera_set(CAMERA_TAKE_PHOTOS);
            if (func_cb.sta == FUNC_CAMERA)
            {
                compo_shape_set_visible(f_camera->mask, true);
                f_camera->mask_tick = tick_get();
            }
        }
        break;

        default:
            break;
    }
}

int jpeg_decode(void *out_buf, int out_size_max, void *in_buf, int in_size);


//创建窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_camera_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

//  创建图片
    compo_picturebox_t *pic = compo_picturebox_create(frm, 0);
    compo_picturebox_set_pos(pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(pic, COMPO_ID_PIC);

    //新建按钮
    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_ICON_CLOCK_BG_BIN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 150);
    compo_setid(btn, COMPO_ID_BTN_START);

    if (func_cb.sta == FUNC_CAMERA)
    {
        f_camera_t *f_camera = (f_camera_t *)func_cb.f_cb;
        f_camera->mask = compo_shape_create(frm, COMPO_SHAPE_TYPE_RECTANGLE);
        compo_shape_set_location(f_camera->mask, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
        compo_shape_set_color(f_camera->mask, COLOR_BLACK);
        compo_shape_set_visible(f_camera->mask, false);
    }

    return frm;
}

//拍照事件处理
static void func_camera_process(void)
{
    f_camera_t *f_camera = (f_camera_t *)func_cb.f_cb;
    if (compo_shape_get_visible(f_camera->mask) && tick_check_expire(f_camera->mask_tick, 100))
    {
        compo_shape_set_visible(f_camera->mask, false);
    }

    func_process();
}

//拍照消息处理
static void func_camera_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_camera_button_click();
            break;

        //不需要点击mask效果这里return回去
        case MSG_CTP_TOUCH:
            break;


        case KU_LEFT:
        {
#if JPEG_LOCAL_TEST
            compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC);
            jpeg_decode(f_picture_jpeg.rgb_buf, JPEG2RGB_BUF_SIZE, t_buf2, JPEG_RX_BUF_SIZE);
            my_tick = 0;
            compo_picturebox_set_ram(pic, f_picture_jpeg.rgb_buf);
            compo_picturebox_set_size(pic, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
#else
            func_camera_set(CAMERA_TRANS_START);
#endif
        }
        break;

        case EVT_JPEG_DECODE:
        {
            int res = 0;
            func_camera_set(CAMERA_TRANS_START);
            compo_picturebox_t *pic = compo_getobj_byid(COMPO_ID_PIC);

#if JPEG_LOCAL_TEST
            res = jpeg_decode(f_picture_jpeg.rgb_buf, JPEG2RGB_BUF_SIZE, t_buf1, JPEG_RX_BUF_SIZE);
#else
            res = jpeg_decode(f_camera_jpeg.rgb_buf, JPEG2RGB_BUF_SIZE, jpeg_rx_buf[!f_camera_jpeg.flag], JPEG_RX_BUF_SIZE);
#endif
            memset(&jpeg_rx_buf[!f_camera_jpeg.flag], 0, JPEG_RX_BUF_SIZE);

//            printf("### jpeg_decode time:%d, flag:%d\n", tick_get() - my_tick, f_picture_jpeg.flag);
            my_tick = 0;
            if (res > 0 )
            {
//                printf("### set\n");
                compo_picturebox_set_ram(pic, f_camera_jpeg.rgb_buf);
                compo_picturebox_set_size(pic, GUI_SCREEN_WIDTH, GUI_SCREEN_HEIGHT);
            }
            else
            {
//                printf("### JPEG DECODE err:%d\n", res);
            }
        }
        break;

        default:
            func_message(msg);
            break;
    }
}

//进入音量页面
static void func_camera_enter(void)
{
    func_camera_set(CAMERA_OPEN);

    func_cb.f_cb = func_zalloc(sizeof(f_camera_t));
    func_cb.frm_main = func_camera_form_create();
    memset(&f_camera_jpeg, 0, sizeof(f_camera_jpeg_t));
    f_camera_jpeg.buf = jpeg_rx_buf[f_camera_jpeg.flag];
//    delay_5ms(200);

}

//退出拍照页面
static void func_camera_exit(void)
{
    func_camera_set(CAMERA_CLOSE);
    func_cb.last = FUNC_CAMERA;
}

//拍照页面
void func_camera(void)
{
    printf("%s\n", __func__);
    func_camera_enter();
//    msg_enqueue(EVT_JPEG_DECODE);
    while (func_cb.sta == FUNC_CAMERA)
    {
        func_camera_process();
        func_camera_message(msg_dequeue());
    }
    func_camera_exit();
}
#else

#include "func_cover.h"
typedef struct f_camera_t_
{
    bool ble_connect_sta; //蓝牙连接状态
    bool open_camera_sta; //打开相机状态
    bool animation_start; //动画开始状态
    uint16_t animation_frame; //动画帧
    uint32_t tick;
} f_camera_t;

static void func_camera_recreate(void);

#if  GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
///非相机传输功能
enum
{
    START_BTN_ID=1,
    START_BTN_CLICK_ID,
    START_TXT_ID,

};

typedef struct ui_handle_t_
{

    struct btn_t
    {
        u16 id;
        u16 click_id;
        s16 x,y;
        u16 w,h;
        u32 res;
        u32 click_res;
    } btn;

    struct text_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;
        u32 res;
        bool center;
        u16 str_id1;
    } text;
} ui_handle_t;

const static ui_handle_t ui_handle =
{
    .btn = {
        .id = START_BTN_ID,
        .click_id = START_BTN_CLICK_ID,
        .x  = 58+124/2,
        .y  = 85+124/2,
        .w  = 0,
        .h  = 0,
        .res = UI_BUF_I330001_PHOTO_BTN_BIN,
        .click_res = 0,
    },

    .text = {
        .id = START_TXT_ID,
        .x  = 48+144/2,
        .y  = 240+28/2,
        .w  = 144,
        .h  = 28,
        .center = true,
        .str_id1 = STR_CLICK_START_PIC,
    },
};

//创建相机窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_camera_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CAMERA]);

    //按钮
    compo_button_t *btn = compo_button_create_by_image(frm, ui_handle.btn.res);
    compo_button_set_pos(btn, ui_handle.btn.x, ui_handle.btn.y);
    compo_setid(btn,ui_handle.btn.id);

    //按钮点击图片
    compo_picturebox_t* pic = compo_picturebox_create(frm, ui_handle.btn.click_res);
    compo_picturebox_set_pos(pic, ui_handle.btn.x, ui_handle.btn.y);
    compo_setid(pic, ui_handle.btn.click_id);
    compo_picturebox_set_visible(pic, false);

    //创建提示文本
    compo_textbox_t *txt_start = compo_textbox_create(frm, strlen(i18n[ui_handle.text.str_id1]));
    compo_textbox_set_location(txt_start, ui_handle.text.x, ui_handle.text.y, ui_handle.text.w, ui_handle.text.h);
    compo_textbox_set_align_center(txt_start, ui_handle.text.center);
    compo_textbox_set(txt_start, i18n[ui_handle.text.str_id1]);
    compo_setid(txt_start,ui_handle.text.id);

    return frm;
}

static void func_camera_button_handle(void)
{
    int id = compo_get_button_id();

    if(id == START_BTN_ID)
    {
        if (ble_is_connect())
        {
            if(uteModuleSportIsTakePicture())
            {
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_TAKE_PICTURE_NOTIFY,0);
            }
        }
        else
        {
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
}

static void func_camera_process_handle(void)
{
    return;
}

#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
///非相机传输功能
enum
{
    START_BTN_ID=1,
    START_BTN_CLICK_ID,
    START_TXT_ID,

};


typedef struct ui_handle_t_
{

    struct btn_t
    {
        u16 id;
        u16 click_id;
        s16 x,y;
        u16 w,h;
        u32 res;
        u32 click_res;
    } btn;

    struct text_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;
        u32 res;
        bool center;
        u16 str_id1;
    } text;
} ui_handle_t;

const static ui_handle_t ui_handle =
{
    .btn = {
        .id = START_BTN_ID,
        .click_id = START_BTN_CLICK_ID,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = GUI_SCREEN_CENTER_Y,
        .w  = 0,
        .h  = 0,
        .res = UI_BUF_I332001_PHOTO_BTN_BIN,
        .click_res = 0,
    },

    .text = {
        .id = START_TXT_ID,
        .x  = GUI_SCREEN_CENTER_X,
        .y  = 284+34/2,
        .w  = 180,
        .h  = 34,
        .center = true,
        .str_id1 = STR_CLICK_START_PIC,
    },
};

//创建相机窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_camera_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CAMERA]);

    //按钮
    compo_button_t *btn = compo_button_create_by_image(frm, ui_handle.btn.res);
    compo_button_set_pos(btn, ui_handle.btn.x, ui_handle.btn.y);
    compo_setid(btn,ui_handle.btn.id);

    //按钮点击图片
    compo_picturebox_t* pic = compo_picturebox_create(frm, ui_handle.btn.click_res);
    compo_picturebox_set_pos(pic, ui_handle.btn.x, ui_handle.btn.y);
    compo_setid(pic, ui_handle.btn.click_id);
    compo_picturebox_set_visible(pic, false);

    //创建提示文本
    compo_textbox_t *txt_start = compo_textbox_create(frm, strlen(i18n[ui_handle.text.str_id1]));
    compo_textbox_set_location(txt_start, ui_handle.text.x, ui_handle.text.y, ui_handle.text.w, ui_handle.text.h);
    compo_textbox_set_align_center(txt_start, ui_handle.text.center);
    compo_textbox_set(txt_start, i18n[ui_handle.text.str_id1]);
    compo_setid(txt_start,ui_handle.text.id);

    return frm;
}

static void func_camera_button_handle(void)
{
    int id = compo_get_button_id();

    if(id == START_BTN_ID)
    {
        if (ble_is_connect())
        {
            if(uteModuleSportIsTakePicture())
            {
                uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_TAKE_PICTURE_NOTIFY,0);
            }
        }
        else
        {
            uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
            sys_cb.cover_index = REMIND_GCOVER_BT_NOT_CONNECT;
            msgbox((char*)i18n[STR_CONNECT_BLUETOOTH], NULL, NULL, MSGBOX_MODE_BTN_NONE, MSGBOX_MSG_TYPE_REMIND_COVER);
        }
    }
}

static void func_camera_process_handle(void)
{
    return;
}

#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
///非相机传输功能
enum
{
    START_BTN_ID=1,
    START_BTN_CLICK_ID,
    START_TXT_ID,

};

typedef struct ui_handle_t_
{

    struct btn_t
    {
        u16 id;
        u16 click_id;
        s16 x,y;
        u16 w,h;
        u32 res;
        u32 click_res;
    } btn;

    struct text_t
    {
        u16 id;
        s16 x,y;
        u16 w,h;
        u32 res;
        bool center;
        u16 str_id1;
    } text;
} ui_handle_t;

const static ui_handle_t ui_handle =
{
    .btn = {
        .id = START_BTN_ID,
        .click_id = START_BTN_CLICK_ID,
        .x  = 60+121/2,
        .y  = 90+121/2,
        .w  = 0,
        .h  = 0,
        .res = UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_1_REMOTE_CAMERA_ICON_PIC121X121_X60_Y90_00_BIN,
        .click_res = 0,
    },

    .text = {
        .id = START_TXT_ID,
        .x  = 48+144/2,
        .y  = 240+28/2,
        .w  = 144,
        .h  = 28,
        .center = true,
        .str_id1 = STR_CLICK_START_PIC,
    },
};

static const uint32_t animation_icon_res[] =
{
    UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_1_REMOTE_CAMERA_ICON_PIC121X121_X60_Y90_00_BIN,
    UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_1_REMOTE_CAMERA_ICON_PIC121X121_X60_Y90_01_BIN,
    UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_1_REMOTE_CAMERA_ICON_PIC121X121_X60_Y90_02_BIN,
    UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_1_REMOTE_CAMERA_ICON_PIC121X121_X60_Y90_03_BIN,
    UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_1_REMOTE_CAMERA_ICON_PIC121X121_X60_Y90_04_BIN,
    UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_1_REMOTE_CAMERA_ICON_PIC121X121_X60_Y90_05_BIN,
    UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_1_REMOTE_CAMERA_ICON_PIC121X121_X60_Y90_06_BIN,
};

//创建相机窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_camera_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    f_camera_t *f_camera = (f_camera_t *)func_cb.f_cb;
    f_camera->ble_connect_sta = ble_is_connect();
    f_camera->open_camera_sta = uteModuleSportIsTakePicture();

    if(!f_camera->ble_connect_sta) //蓝牙未连接
    {
        // icon
        compo_picturebox_t *icon = compo_picturebox_create(frm, UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_3_BLUETOOTH_NOCONNECTED_ICON_BLE_70X74_X86_Y76_BIN);
        compo_picturebox_set_pos(icon, 120, 112);

        // 创建提示文本
        compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_CONNECT_BLUETOOTH]));
        compo_textbox_set_location(txt, 120, 198, 144, 28);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set(txt, i18n[STR_CONNECT_BLUETOOTH]);
    }
    else if (!f_camera->open_camera_sta) //相机未打开
    {
        // icon
        compo_picturebox_t *icon = compo_picturebox_create(frm, UI_BUF_I335001_27_MORE_5_REMOTE_CAMERA_2_OPEN_CAMERA_ICON_56X92_X92_Y66_BIN);
        compo_picturebox_set_pos(icon, 120, 112);

        // 创建提示文本
        compo_textbox_t *txt = compo_textbox_create(frm, strlen(i18n[STR_PLEASE_APP_DATA]));
        compo_textbox_set_location(txt, 120, 198, 144, 28);
        compo_textbox_set_align_center(txt, true);
        compo_textbox_set(txt, i18n[STR_PLEASE_APP_DATA]);
    }
    else
    {
        //设置标题栏
        compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
        compo_form_set_title(frm, i18n[STR_CAMERA]);

        //按钮
        compo_button_t *btn = compo_button_create_by_image(frm, ui_handle.btn.res);
        compo_button_set_pos(btn, ui_handle.btn.x, ui_handle.btn.y);
        compo_setid(btn, ui_handle.btn.click_id);

        // 创建提示文本
        // compo_textbox_t *txt_start = compo_textbox_create(frm, strlen(i18n[ui_handle.text.str_id1]));
        // compo_textbox_set_location(txt_start, ui_handle.text.x, ui_handle.text.y, ui_handle.text.w, ui_handle.text.h);
        // compo_textbox_set_align_center(txt_start, ui_handle.text.center);
        // compo_textbox_set(txt_start, i18n[ui_handle.text.str_id1]);
        // compo_setid(txt_start, ui_handle.text.id);
    }

    return frm;
}

static void func_camera_process_handle(void)
{
    f_camera_t *f_camera = (f_camera_t *)func_cb.f_cb;

    if (f_camera->ble_connect_sta != ble_is_connect() || f_camera->open_camera_sta != uteModuleSportIsTakePicture())
    {
        func_camera_recreate();
    }
    else if (f_camera->open_camera_sta)
    {
        compo_button_t *icon = compo_getobj_byid(START_BTN_CLICK_ID);
        if (icon && f_camera->animation_start)
        {
            if (tick_check_expire(f_camera->tick, 100))
            {
                f_camera->tick = tick_get();
                if (f_camera->animation_frame < sizeof(animation_icon_res) / sizeof(animation_icon_res[0]))
                {
                    compo_button_set_bgimg(icon, animation_icon_res[f_camera->animation_frame]);
                    f_camera->animation_frame++;
                }
                else
                {
                    f_camera->animation_start = false;
                    f_camera->animation_frame = 0;
                    compo_button_set_bgimg(icon, animation_icon_res[0]);
                }
            }
        }
    }
}

static void func_camera_button_handle(void)
{
    int id = compo_get_button_id();

    if (id == START_BTN_CLICK_ID)
    {
        if (uteModuleSportIsTakePicture())
        {
            f_camera_t *f_camera = (f_camera_t *)func_cb.f_cb;
            f_camera->animation_start = true;
            f_camera->animation_frame = 0;
            uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_TAKE_PICTURE_NOTIFY, 0);
        }
    }
}

#else
//创建相机窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_camera_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
static void func_camera_button_handle(void)
{
}

static void func_camera_process_handle(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
//相机功能事件处理
static void func_camera_process(void)
{
    func_camera_process_handle();
    func_process();
}

//相机功能消息处理
static void func_camera_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_camera_button_handle();
            break;

        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        case MSG_REBUILD_FUNC:
            // func_camera_recreate();
            break;

        default:
            func_message(msg);
            break;
    }
}

static void func_camera_recreate(void)
{
    if(func_cb.sta == FUNC_CAMERA)
    {
        compo_form_destroy(func_cb.frm_main);
        func_cb.frm_main = func_camera_form_create();
    }
}

//进入相机功能
static void func_camera_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_camera_t));
    func_cb.frm_main = func_camera_form_create();

#if UTE_MODULE_SPORT_TAKE_PICTURE_OPEN_APP_SCREEN
    if(!uteModuleSportIsTakePicture())
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_TAKE_PICTURE_OPEN_APP_SCREEN,0);
    }
#endif

}

//退出相机功能
static void func_camera_exit(void)
{
    func_cb.last = FUNC_CAMERA;

    if(uteModuleSportIsTakePicture())
    {
        uteModuleSportExitTakePicture();
    }

}

//相机功能
void func_camera(void)
{
    printf("%s\n", __func__);
    func_camera_enter();
    while (func_cb.sta == FUNC_CAMERA)
    {
        func_camera_process();
        func_camera_message(msg_dequeue());
    }
    func_camera_exit();
}

#endif
#endif // UTE_MODULE_SCREENS_CAMERA_SUPPORT
