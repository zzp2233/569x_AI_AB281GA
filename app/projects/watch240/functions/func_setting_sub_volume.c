#include "include.h"
#include "func.h"

#if UTE_MODULE_SCREENS_VOLUME_SUPPORT

enum
{
    COMOP_PIC_VOL =1,
    COMOP_BTN_VOL,
};

typedef struct f_volume_t_
{
    bool touch_flag;
    u8 level_old;
    u8 level_value;
    u32 tick;
} f_volume_t;
#define SPACING_COUNT  (-40)
compo_form_t* func_set_sub_volume_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MEDIA_VOL]);
    //亮度
    compo_picturebox_t *light_pic = compo_picturebox_create(frm, UI_BUF_I341001_28_SET_BRIGHTNESS_BIN);
    compo_setid(light_pic, COMOP_PIC_VOL);
    compo_picturebox_set_pos(light_pic, GUI_SCREEN_CENTER_X, 93+300/2);
    compo_picturebox_cut(light_pic, sys_cb.light_level-1, 5);

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn,GUI_SCREEN_CENTER_X, 97+306/2,106,306);
    compo_setid(btn,COMOP_BTN_VOL);

    return frm;
}

static void func_light_disp_move_handle(void)
{
    f_volume_t *f_volume = (f_volume_t *)func_cb.f_cb;
    if(f_volume->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_volume->touch_flag = ctp_get_dxy(&dx, &dy);

        compo_picturebox_t *light_pic = compo_getobj_byid(COMOP_PIC_VOL);
        if(f_volume->touch_flag)//触摸状态
        {
            s8 level_data= (dy/SPACING_COUNT)+f_volume->level_old;

            if(level_data<1)
                level_data=1;
            else if(level_data>5)
                level_data=5;

            if(level_data != f_volume->level_value)
            {
                if(tick_check_expire(f_volume->tick, 70))//自动滑动
                {
                    f_volume->tick = tick_get();
                    // printf("level_data:%d\n",level_data);
                    f_volume->level_value = level_data;
                    compo_picturebox_cut(light_pic, (uint8_t)level_data-1, 5);
                }
            }
        }
        else
        {
            f_volume->level_old = f_volume->level_value;
        }
    }
}

static void func_set_sub_volume_process()
{
    func_process();
}

static void func_set_sub_volume_message(size_msg_t msg)
{

    f_volume_t *f_volume = (f_volume_t *)func_cb.f_cb;
    switch (msg)
    {
        case MSG_CTP_TOUCH:
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
            if(compo_get_button_id() == COMOP_BTN_VOL && f_volume->touch_flag == false)
            {
                f_volume->touch_flag = true;
            }
#endif
            break;
        case MSG_CTP_CLICK:

            break;

        default:
            func_message(msg);
            break;
    }
}
//进入亮度调节功能
static void func_set_sub_volume_enter(void)
{
    func_cb.frm_main = func_set_sub_volume_form_create();
    func_cb.f_cb = func_zalloc(sizeof(f_volume_t));
}

//退出亮度调节功能
static void func_set_sub_volume_exit(void)
{
    func_cb.last = FUNC_SET_SUB_VOLUME;
}

//亮度调节功能
void func_set_sub_volume(void)
{
    printf("%s\n", __func__);
    func_set_sub_volume_enter();
    while (func_cb.sta == FUNC_SET_SUB_VOLUME)
    {
        func_set_sub_volume_process();
        func_set_sub_volume_message(msg_dequeue());
    }
    func_set_sub_volume_exit();
}
#endif //UTE_MODULE_SCREENS_VOLUME_SUPPORT