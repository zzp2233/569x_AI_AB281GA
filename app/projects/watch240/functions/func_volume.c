#include "include.h"
#include "func.h"
#include "ute_module_localRingtone.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define VOL_CHANGE          1  //音量等级每次增加或者减少量

typedef struct f_volume_t_
{
    bool touch_flag;
    s8   level_value;
} f_volume_t;

enum
{
    //按键
    COMPO_ID_BTN_REDUCE = 1,
    COMPO_ID_BTN_INCREASE,

    //图像
    COMPO_ID_PIC_LEVEL1,            //等级 1~5
    COMPO_ID_PIC_LEVEL2,
    COMPO_ID_PIC_LEVEL3,
    COMPO_ID_PIC_LEVEL4,
    COMPO_ID_PIC_LEVEL5,
};

#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
typedef struct volume_disp_pic_item_t_
{
    u32 res_addr;
    u32 res_addr_bg;
    u16 pic_id;
    s16 x;
    s16 y;
    bool visible_en;
} volume_disp_pic_item_t;

#define VOLUME_DISP_PIC_ITEM_CNT    ((int)(sizeof(tbl_volume_disp_pic_item) / sizeof(tbl_volume_disp_pic_item[0])))

//图片item，创建时遍历一下
static const volume_disp_pic_item_t tbl_volume_disp_pic_item[] =
{
    {UI_BUF_I330001_SETTINGS_SOUND_LEFT_GREEN_BIN,   UI_BUF_I330001_SETTINGS_SOUND_LEFT_GREY_BIN,    COMPO_ID_PIC_LEVEL1,          68,     257},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL2,          68+21,    257},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL3,          68+21*2,    257},
    {UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_ZHONG_GREY_BIN,   COMPO_ID_PIC_LEVEL4,          68+21*3,    257},
    {UI_BUF_I330001_SETTINGS_SOUND_RIGHT_GREEN_BIN,  UI_BUF_I330001_SETTINGS_SOUND_RIGHT_GREY_BIN,   COMPO_ID_PIC_LEVEL5,          68+21*4,    257},
};

//创建调节音量窗体
compo_form_t *func_volume_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    compo_form_add_image(frm, UI_BUF_I330001_THEME_1_SOUND_BIN, 120, 148);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOLUME]);

    compo_picturebox_t *picbox = compo_picturebox_create(frm, UI_BUF_I330001_SETTINGS_SOUND_BTN_BIN);
    compo_picturebox_set_pos(picbox,GUI_SCREEN_CENTER_X,tbl_volume_disp_pic_item[0].y );

    //新建图像
    for (u8 idx = 0; idx < VOLUME_DISP_PIC_ITEM_CNT; idx++)
    {
        compo_picturebox_t *pic_level = compo_picturebox_create(frm, tbl_volume_disp_pic_item[idx].res_addr_bg);
        compo_setid(pic_level, tbl_volume_disp_pic_item[idx].pic_id);
        compo_picturebox_set_pos(pic_level, tbl_volume_disp_pic_item[idx].x, tbl_volume_disp_pic_item[idx].y);
    }

    compo_button_t *btn = compo_button_create(frm);
    compo_button_set_location(btn, 35,tbl_volume_disp_pic_item[0].y, 40,40);
    compo_setid(btn,COMPO_ID_BTN_REDUCE);

    btn = compo_button_create(frm);
    compo_button_set_location(btn, 200,tbl_volume_disp_pic_item[0].y, 40,40);
    compo_setid(btn,COMPO_ID_BTN_INCREASE);

    return frm;
}
//更新显示界面
static void func_set_sub_volume_disp(void)
{
    for(int idx=0; idx<5; idx++)
    {
        compo_picturebox_t *picbox = compo_getobj_byid(tbl_volume_disp_pic_item[idx].pic_id);

        if(sys_cb.vol/(VOL_CHANGE*(idx+1)))
        {
            compo_picturebox_set(picbox,tbl_volume_disp_pic_item[idx].res_addr);
        }
        else
        {
            compo_picturebox_set(picbox,tbl_volume_disp_pic_item[idx].res_addr_bg);
        }
    }
}

//单击按钮
static void func_volume_button_click(void)
{
    int id = compo_get_button_id();

    switch (id)
    {
        case COMPO_ID_BTN_REDUCE:
            if (sys_cb.vol > 0 && sys_cb.vol <= VOL_MAX)
            {
                sys_cb.vol -= VOL_CHANGE;
            }
            break;

        case COMPO_ID_BTN_INCREASE:
            if (sys_cb.vol >= 0 && sys_cb.vol < VOL_MAX)
            {
                sys_cb.vol += VOL_CHANGE;
            }
            break;
        default:
            break;
    }

    func_set_sub_volume_disp();
}

#elif GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT

enum
{
    COMPO_ID_PIC_VOLUME = 1,
};
compo_form_t *func_volume_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
    //compo_form_add_image(frm, UI_BUF_I330001_THEME_1_SOUND_BIN, 120, 148);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_VOL]);

    //音量
    compo_picturebox_t *volume_pic = compo_picturebox_create(frm, UI_BUF_I341001_28_SET_SOUND_SOUND_BIN);
    compo_setid(volume_pic, COMPO_ID_PIC_VOLUME);
    compo_picturebox_set_pos(volume_pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y + 10);
    if ((uteModuleLocalRingtoneGetVolume() == 0) || uteModuleLocalRingtoneGetMuteStatus())
        compo_picturebox_set(volume_pic, UI_BUF_I341001_28_SET_SOUND_SOUND_MUTE_BIN);
    else
        compo_picturebox_cut(volume_pic, uteModuleLocalRingtoneGetVolume()-1, 15);

    return frm;
}
//更新显示界面
static void func_set_sub_volume_disp(void)
{
    f_volume_t *f_volume = (f_volume_t *)func_cb.f_cb;
    bool muteState = false;

    if(f_volume->touch_flag)//触摸状态
    {
        s32 dx, dy;
        f_volume->touch_flag = ctp_get_dxy(&dx, &dy);//上一个条件默认是true,在这里重新获取状态以及更新

        compo_picturebox_t *volume_pic = compo_getobj_byid(COMPO_ID_PIC_VOLUME);
        if(f_volume->touch_flag && dy != 0)//触摸状态
        {
            s8 level_data;
            if (uteModuleLocalRingtoneGetMuteStatus())
            {
                level_data = dy/(-40);
                muteState = true;
            }
            else
            {
                level_data = dy/(-40) + uteModuleLocalRingtoneGetVolume();
            }

            if(level_data < 0)
            {
                level_data = 0;
            }
            else if(level_data > 15)
            {
                level_data = 15;
            }
            //sys_cb.hfp_vol = level_data;
            if(level_data != f_volume->level_value)
            {
                f_volume->level_value = level_data;
                if (level_data == 0)
                {
                    compo_picturebox_set(volume_pic, UI_BUF_I341001_28_SET_SOUND_SOUND_MUTE_BIN);
                    uteModuleLocalRingtoneSetMuteStatus(true, true);
                }
                else
                {
                    compo_picturebox_set(volume_pic, UI_BUF_I341001_28_SET_SOUND_SOUND_BIN);
                    compo_picturebox_cut(volume_pic, (uint8_t)level_data-1, 15);
                    if (muteState)
                    {
                        uteModuleLocalRingtoneSetMuteStatus(false, true);
                        uteModuleLocalRingtoneSetVolume(f_volume->level_value);
                    }
                }
            }
        }
    }
    else
    {
        uteModuleLocalRingtoneSetVolume(f_volume->level_value);
    }
}
//单击按钮
static void func_volume_button_click(void)
{

}
#else

compo_form_t *func_volume_form_create(void)
{
    //新建窗体和背景
    compo_form_t *frm = compo_form_create(true);
}

//更新显示界面
static void func_set_sub_volume_disp(void)
{
}

//单击按钮
static void func_volume_button_click(void)
{
}

#endif

//音量功能事件处理
static void func_volume_process(void)
{
    func_set_sub_volume_disp();
    func_process();
}

//音量功能消息处理
static void func_volume_message(size_msg_t msg)
{
    f_volume_t *f_volume = (f_volume_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_volume_button_click();
            break;
#if GUI_SCREEN_SIZE_368X448RGB_I345001_SUPPORT
        case MSG_CTP_TOUCH:
            f_volume->touch_flag = true;
            break;
#endif
        case MSG_CTP_SHORT_UP:
            break;

        case MSG_CTP_SHORT_DOWN:
            break;

        case MSG_CTP_LONG:
            break;

        default:
            func_message(msg);
            break;
    }
}

//进入音量控制功能
static void func_volume_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_volume_t));
    func_cb.frm_main = func_volume_form_create();

    f_volume_t *f_volume = (f_volume_t *)func_cb.f_cb;
    f_volume->level_value = uteModuleLocalRingtoneGetVolume();
    //func_set_sub_volume_disp();
}

//退出音量功能
static void func_volume_exit(void)
{
    func_cb.last = FUNC_VOLUME;
}

//音量功能
void func_volume(void)
{
    printf("%s\n", __func__);
    func_volume_enter();
    while (func_cb.sta == FUNC_VOLUME)
    {
        func_volume_process();
        func_volume_message(msg_dequeue());
    }
    func_volume_exit();
}
