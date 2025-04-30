#include "include.h"
#include "func.h"
#include "ute_module_gui_common.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_CALL_VOLUME_SET_SUPPORT

#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
//组件ID
enum
{
    COMPO_ID_PIC_VOLUME= 1,
    //按键
    COMPO_ID_BTN_REDUCE,
    COMPO_ID_BTN_INCREASE,
};

#define SPACING_COUNT  (-40)
typedef struct f_call_volume_t
{
    bool touch_flag;
    s32  move_offset;
    s32  move_offset_old;
    s8   level_old;
    s8   level_value;
} f_call_volume_t;
//创建亮度调节窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_call_volume_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_CALL_VOL]);
    //音量
    if(sys_cb.vol<=8)
    {
        compo_picturebox_t *volume_pic = compo_picturebox_create(frm, UI_BUF_I332001_CALL_CALLING_VOLUME_PROGRESS_00_BIN);
        compo_setid(volume_pic, COMPO_ID_PIC_VOLUME);
        compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X, 88+144);
        compo_picturebox_cut(volume_pic,sys_cb.vol,9);

    }
    else
    {
        compo_picturebox_t *volume_pic = compo_picturebox_create(frm, UI_BUF_I332001_CALL_CALLING_VOLUME_PROGRESS_01_BIN);
        compo_setid(volume_pic, COMPO_ID_PIC_VOLUME);
        compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X, 88+144);
        compo_picturebox_cut(volume_pic,sys_cb.vol-9,8);
    }


    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_I332001_SETTINGS_BRIGHTNESS_LESS_BIN);
    compo_setid(btn, COMPO_ID_BTN_REDUCE);
    compo_button_set_pos(btn, 62/2+41,62/2+159);

    btn = compo_button_create_by_image(frm, UI_BUF_I332001_SETTINGS_BRIGHTNESS_PLUS_BIN);
    compo_setid(btn, COMPO_ID_BTN_INCREASE);
    compo_button_set_pos(btn,  62/2+257,62/2+159);

    return frm;
}

static void func_call_volume_disp_Refresh(void)
{
    compo_picturebox_t *volume_pic ;
    volume_pic =compo_getobj_byid(COMPO_ID_PIC_VOLUME);
    if(sys_cb.vol<=8)
    {
        compo_picturebox_set(volume_pic, UI_BUF_I332001_CALL_CALLING_VOLUME_PROGRESS_00_BIN);
        compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X, 88+144);
        compo_picturebox_cut(volume_pic,sys_cb.vol,9);

    }
    else
    {
        compo_picturebox_set(volume_pic, UI_BUF_I332001_CALL_CALLING_VOLUME_PROGRESS_01_BIN);
        compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X, 88+144);
        compo_picturebox_cut(volume_pic,sys_cb.vol-9,8);
    }
}



//单击按钮
static void func_call_volume_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REDUCE:
            bt_volume_down();
            break;

        case COMPO_ID_BTN_INCREASE:
            bt_volume_up();
            break;

        default:
            break;
    }
    func_call_volume_disp_Refresh();
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//亮度调节功能事件处理
static void func_call_volume_process(void)
{
    func_process();
}

//亮度调节功能消息处理
static void func_call_volume_message(size_msg_t msg)
{
    f_call_volume_t *f_call_volume = (f_call_volume_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
            break;
        case MSG_CTP_CLICK:
#if GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
            func_call_volume_button_click();
#endif
            break;
        default:
            func_message(msg);
            break;
    }
}

//进入亮度调节功能
static void func_call_volume_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_call_volume_t));
    func_cb.frm_main = func_call_volume_form_create();
#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
    func_call_volume_disp_Refresh();
#endif
}

//退出亮度调节功能
static void func_call_volume_exit(void)
{
    f_call_volume_t *f_light = (f_call_volume_t *)func_cb.f_cb;
    func_cb.last = FUNC_BT_CALL_VOLUME;
}

//亮度调节功能
void func_call_volume(void)
{
    printf("%s\n", __func__);
    func_call_volume_enter();
    while (func_cb.sta == FUNC_BT_CALL_VOLUME)
    {
        func_call_volume_process();
        func_call_volume_message(msg_dequeue());
    }
    func_call_volume_exit();
}

#endif
