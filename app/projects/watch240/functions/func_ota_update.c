#include "include.h"
#include "func.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
enum
{
    ROCKET_ID=1,
    ROCKET_BASE_ID,
    PROGRESS_BAR_ID,
    UPDATING_TXT_ID,
};

typedef struct f_ota_update_t_
{
    u32 launch_roket_time;
} f_ota_update_t;



#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

compo_form_t *func_ota_update_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I330001_OTA_00_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 35+112/2);
    compo_setid(picbox,ROCKET_ID);

    //CODE 进度条
    compo_progressbar_t * bar = compo_progressbar_create(frm, PROGRESSBAR_HORIZONTAL);
    compo_setid(bar, PROGRESS_BAR_ID);
    compo_progressbar_set_location(bar, GUI_SCREEN_CENTER_X, 224+10/2, 196, 10);
    compo_progressbar_set_range(bar, 0, 100);
    compo_progressbar_set_edge_circle(bar, 10/2);
    compo_progressbar_set_color(bar, make_color(35,141,255), make_color(51,51,51));

    //TXT1 升级中
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_UPGRADING]);

    return frm;
}

//显示升级界面处理
static void func_ota_update_disp(void)
{
    static u32 ticks = 0;
//    f_ota_update_t *f_ota_update = (f_ota_update_t *)func_cb.f_cb;

    if (tick_check_expire(ticks, 100))
    {
        ticks = tick_get();
        reset_sleep_delay_all();
//        f_ota_update->launch_roket_time -=10;
//        if(f_ota_update->launch_roket_time < 40)
//        {
//            f_ota_update->launch_roket_time = GUI_SCREEN_HEIGHT*0.5;
//        }
//
//        compo_picturebox_t *picbox = compo_getobj_byid(ROCKET_ID);
//        compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, f_ota_update->launch_roket_time);

        extern fot_progress_t *bsp_fot_progress_get(void);
        fot_progress_t *fot_data = bsp_fot_progress_get();

        if(fot_data->type == FOT_FILE_TYPE_FOT)
        {
//            compo_arc_t *arc = compo_getobj_byid(PROGRESS_BAR_ID);
            compo_progressbar_t *bar = compo_getobj_byid(PROGRESS_BAR_ID);
            if(fot_data->percent)
            {
                if(fot_data->percent >= 97)
                {
                    fot_data->percent = 100;
                }
                compo_progressbar_set_value(bar,fot_data->percent);
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT

compo_form_t *func_ota_update_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I332001_OTA_00_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 60+150/2);
    compo_setid(picbox,ROCKET_ID);

    //CODE 进度条
    compo_arc_t *arc = compo_arc_create(frm);
    compo_setid(arc, PROGRESS_BAR_ID);
    compo_arc_set_location(arc, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH-5, GUI_SCREEN_HEIGHT-5);
    compo_arc_set_rotation(arc, 0);
    compo_arc_set_angles(arc, 0, 3600);
    compo_arc_set_edge_circle(arc,true,true);
    compo_arc_set_color(arc,make_color(0x23,0x8d,0xff),make_color(0x33,0x33,0x33));
    compo_arc_set_alpha(arc,255,255);
    compo_arc_set_value(arc,0);
    compo_arc_set_width(arc, 10);

    //TXT1 升级中
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 250+32/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_UPGRADING]);

    return frm;
}

//显示升级界面处理
static void func_ota_update_disp(void)
{
    static u32 ticks = 0;

    if (tick_check_expire(ticks, 100))
    {
        ticks = tick_get();
        reset_sleep_delay_all();

        extern fot_progress_t *bsp_fot_progress_get(void);
        fot_progress_t *fot_data = bsp_fot_progress_get();
        if(fot_data->type == FOT_FILE_TYPE_FOT)
        {
            if(fot_data->percent)
            {
                if(fot_data->percent >= 97)
                {
                    fot_data->percent = 100;
                }
                compo_arc_t *bar = compo_getobj_byid(PROGRESS_BAR_ID);
                compo_arc_set_value(bar,fot_data->percent*10);
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
compo_form_t *func_ota_update_form_create(void)
{
    char txt_buf[30];
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I335001_UPGRADE_DURING_UPGRADE_ICON_SYNC_88X118_X76_Y37_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-35);

    extern fot_progress_t *bsp_fot_progress_get(void);
    fot_progress_t *fot_data = bsp_fot_progress_get();

    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",fot_data->percent);
    compo_textbox_t *textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+55,150,50);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,PROGRESS_BAR_ID);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_UPGRADING]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+95,150,30);
    compo_textbox_set(textbox,i18n[STR_UPGRADING]);

    return frm;
}
static void func_ota_update_disp(void)
{
    static u32 ticks = 0;

    if (tick_check_expire(ticks, 100))
    {
        ticks = tick_get();
        reset_sleep_delay_all();

        extern fot_progress_t *bsp_fot_progress_get(void);
        fot_progress_t *fot_data = bsp_fot_progress_get();
        if(fot_data->type == FOT_FILE_TYPE_FOT)
        {
            if(fot_data->percent)
            {
                if(fot_data->percent >= 97)
                {
                    fot_data->percent = 100;
                }
                char txt_buf[30];
                compo_textbox_t *txt_val   = compo_getobj_byid(PROGRESS_BAR_ID);
                snprintf(txt_buf,sizeof(txt_buf),"%d%%",fot_data->percent );
                compo_textbox_set(txt_val,txt_buf);
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I338001_SUPPORT

compo_form_t *func_ota_update_form_create(void)
{
    char txt_buf[30];

    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I338001_UPGRADE_02_UPGRADE_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 40+158/2);
    compo_setid(picbox,ROCKET_ID);

    extern fot_progress_t *bsp_fot_progress_get(void);
    fot_progress_t *fot_data = bsp_fot_progress_get();

    //数字
    memset(txt_buf,0,sizeof(txt_buf));
    snprintf(txt_buf,sizeof(txt_buf),"%d%%",fot_data->percent);
    compo_textbox_t *textbox = compo_textbox_create(frm, 5);
    compo_textbox_set_font(textbox,UI_BUF_0FONT_FONT_NUM_32_BIN);
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X,225+25,150,50);
    compo_textbox_set(textbox,txt_buf);
    compo_setid(textbox,PROGRESS_BAR_ID);

    //TXT1 升级中
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 286+30/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_UPGRADING]);

    return frm;
}

//显示升级界面处理
static void func_ota_update_disp(void)
{
    static u32 ticks = 0;

    if (tick_check_expire(ticks, 100))
    {
        ticks = tick_get();
        reset_sleep_delay_all();

        extern fot_progress_t *bsp_fot_progress_get(void);
        fot_progress_t *fot_data = bsp_fot_progress_get();
        if(fot_data->type == FOT_FILE_TYPE_FOT)
        {
            if(fot_data->percent)
            {
                if(fot_data->percent >= 97)
                {
                    fot_data->percent = 100;
                }
                compo_arc_t *bar = compo_getobj_byid(PROGRESS_BAR_ID);
                compo_arc_set_value(bar,fot_data->percent*10);
            }
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I340001_SUPPORT

compo_form_t *func_ota_update_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);

    compo_picturebox_t * picbox = compo_picturebox_create(frm, UI_BUF_I340001_OTA_00_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X, 60+150/2);
    compo_setid(picbox,ROCKET_ID);

    //CODE 进度条
    compo_arc_t *arc = compo_arc_create(frm);
    compo_setid(arc, PROGRESS_BAR_ID);
    compo_arc_set_location(arc, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y, GUI_SCREEN_WIDTH-5, GUI_SCREEN_HEIGHT-5);
    compo_arc_set_rotation(arc, 0);
    compo_arc_set_angles(arc, 0, 3600);
    compo_arc_set_edge_circle(arc,true,true);
    compo_arc_set_color(arc,make_color(0x23,0x8d,0xff),make_color(0x33,0x33,0x33));
    compo_arc_set_alpha(arc,255,255);
    compo_arc_set_value(arc,0);
    compo_arc_set_width(arc, 10);

    //TXT1 升级中
    compo_textbox_t* txt = compo_textbox_create(frm, 20);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 250+32/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_UPGRADING]);

    return frm;
}

//显示升级界面处理
static void func_ota_update_disp(void)
{
    static u32 ticks = 0;

    if (tick_check_expire(ticks, 100))
    {
        ticks = tick_get();
        reset_sleep_delay_all();

        extern fot_progress_t *bsp_fot_progress_get(void);
        fot_progress_t *fot_data = bsp_fot_progress_get();
        if(fot_data->type == FOT_FILE_TYPE_FOT)
        {
            if(fot_data->percent)
            {
                if(fot_data->percent >= 97)
                {
                    fot_data->percent = 100;
                }
                compo_arc_t *bar = compo_getobj_byid(PROGRESS_BAR_ID);
                compo_arc_set_value(bar,fot_data->percent*10);
            }
        }
    }
}

#else
compo_form_t *func_ota_update_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    return frm;
}
static void func_ota_update_disp(void)
{}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT

//刷新显示
static void func_ota_update_process(void)
{

    func_ota_update_disp();
    func_process();
}

//进入升级界面
static void func_ota_update_enter(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(false);
    func_cb.f_cb = func_zalloc(sizeof(f_ota_update_t));
    func_cb.frm_main = func_ota_update_form_create();

}

//退出升级界面
static void func_ota_update_exit(void)
{
    uteModuleGuiCommonDisplayOffAllowGoBack(true);
    func_cb.last = FUNC_OTA_MODE;
}

//升级界面
void func_ota_update(void)
{
    printf("%s\n", __func__);
    func_ota_update_enter();
    reset_sleep_delay_all();
    while (func_cb.sta == FUNC_OTA_MODE)
    {
        func_ota_update_process();

    }
    func_ota_update_exit();
}
