#include "include.h"
#include "func.h"
#include "ute_module_bloodoxygen.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

//血氧检测状态
enum
{
    BO_STA_IDLE,            //空闲
    BO_STA_UNWEAR,          //未佩戴
};

//组件ID
enum
{
    //按键
    COMPO_ID_SURE_BTN = 1,
    COMPO_ID_AGAIN_BTN,
    COMPO_ID_PIC_BG,
    COMPO_ID_TXT_VALUE,
    COMPO_ID_SAHPE_BG,
};

typedef struct f_blood_oxygen_t_
{
    uint32_t tick;
    bool blood_oxygen_state;
    u8 pic_type;
    bool pop_disp_flag;
} f_blood_oxygen_t;


//创建血氧窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_blood_oxygen_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_BLOOD_OXYGEN]);

    ///设置图片
    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_BLOODOXYGEN_GIF_BIN);
    compo_picturebox_cut(picbox, 0, 16);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/6);
    compo_setid(picbox,COMPO_ID_PIC_BG);

    compo_textbox_t *textbox;
    textbox = compo_textbox_create(frm, 3 );///血氧数据
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_38_BIN);
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.75,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/2);
    compo_textbox_set(textbox,"--");
    compo_setid(textbox,COMPO_ID_TXT_VALUE);

    picbox = compo_picturebox_create(frm, UI_BUF_I330001_BLOODOXYGEN_PERCENT_BIN);///  % 图片
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/8,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.75);

    textbox = compo_textbox_create(frm, strlen("血氧饱和度") );///血氧和饱和度
    compo_textbox_set_pos(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/2.7,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.2);
    compo_textbox_set(textbox,"血氧饱和度");
    compo_textbox_set_forecolor(textbox, COLOR_GRAY);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I330001_BLOODOXYGEN_ICON_DETECTION_BIN);///重新测量按钮
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.5,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.5);
    compo_setid(btn,COMPO_ID_AGAIN_BTN);
//    compo_button_set_visible(btn, false);

    return frm;
}


static void func_blood_oxygen_disp_handle(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_picturebox_t * picbox = compo_getobj_byid(COMPO_ID_PIC_BG);
    compo_textbox_t *textbox = compo_getobj_byid(COMPO_ID_TXT_VALUE);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    char txt_buf[20];

    if(f_bo->blood_oxygen_state == BO_STA_IDLE) ///血氧检测界面
    {
        if(tick_check_expire(f_bo->tick, 100))
        {
            f_bo->tick = tick_get();

            if(++f_bo->pic_type==16)
            {
                f_bo->pic_type=0;
            }
            compo_picturebox_cut(picbox, f_bo->pic_type, 16); ///图片动态显示

            memset(txt_buf,0,sizeof(txt_buf));
            snprintf(txt_buf,sizeof(txt_buf),"%d",bsp_sensor_spo2_data_get());///血氧值
            compo_textbox_set(textbox,txt_buf);

            compo_button_set_visible(btn, false);
        }
    }
    else  ///未佩戴界面
    {
        compo_picturebox_cut(picbox, 0, 16); ///图片动态显示
        memset(txt_buf,0,sizeof(txt_buf));
        snprintf(txt_buf,sizeof(txt_buf),"--");///血氧值
        compo_textbox_set(textbox,txt_buf);
    }

    if(bsp_sensor_hr_wear_sta_get() == true) ///佩戴处理
    {
        f_bo->blood_oxygen_state = BO_STA_IDLE;
        compo_button_set_visible(btn, false);
    }
    else
    {
        f_bo->blood_oxygen_state = BO_STA_UNWEAR;
        compo_button_set_visible(btn, true);
    }
}
//血氧功能事件处理
static void func_blood_oxygen_process(void)
{
    func_blood_oxygen_disp_handle();
    func_process();
}


//单击按钮
static void func_blood_oxygen_button_click(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_AGAIN_BTN);
    int id = compo_get_button_id();
    u8 ret;

    switch(id)
    {
        case COMPO_ID_AGAIN_BTN:
            ret = msgbox("请佩戴手表测量", NULL, NULL, MSGBOX_MODE_BTN_SURE, MSGBOX_MSG_TYPE_NONE);
//        if(ret == MSGBOX_RES_OK){
//            compo_button_set_visible(btn, true);
//        }
            f_bo->blood_oxygen_state = BO_STA_IDLE;
            uteModuleBloodoxygenStartSingleTesting();///开启测试

            break;
    }

}
//血氧功能消息处理
static void func_blood_oxygen_message(size_msg_t msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_blood_oxygen_button_click();
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入血氧功能
static void func_blood_oxygen_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_blood_oxygen_t));
    func_cb.frm_main = func_blood_oxygen_form_create();
}

//退出血氧功能
static void func_blood_oxygen_exit(void)
{
    f_blood_oxygen_t *f_bo = (f_blood_oxygen_t *)func_cb.f_cb;
    if (BO_STA_IDLE == f_bo->blood_oxygen_state)
    {
        uteModuleBloodoxygenStopSingleTesting();///关闭测试
    }
    func_cb.last = FUNC_BLOOD_OXYGEN;
}

//血氧功能
void func_blood_oxygen(void)
{
    printf("%s\n", __func__);
    func_blood_oxygen_enter();
    while (func_cb.sta == FUNC_BLOOD_OXYGEN)
    {
        func_blood_oxygen_process();
        func_blood_oxygen_message(msg_dequeue());
    }
    func_blood_oxygen_exit();
}
