#include "include.h"
#include "func.h"
#include "ute_module_watchonline.h"

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if UTE_MODULE_SCREENS_SYNC_WATCH_ONLINE_SUPPORT

#define SWITCH_TIME      2000

enum
{
    ROCKET_ID=1,
    ROCKET_BASE_ID,
    PROGRESS_BAR_ID,
    UPDATING_TXT_ID,
    UPDATING_TXT_TIP_ID,
};
enum
{
    UPGRADE_SUCCESSFUL=0,//成功
    UPGRADE_FAILED,//失败
    UPGRADING,//升级中
};

typedef struct f_up_watch_dial_t_
{
    u8 state;
    u32 ticks;
    u32 switch_page_time;
} f_up_watch_dial_t;


#if GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
compo_form_t *func_up_watch_dial_form_create(void)
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
    compo_textbox_t* txt = compo_textbox_create(frm, 40);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 186+26/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_INS]);

    txt = compo_textbox_create(frm,  40);
    compo_setid(txt, UPDATING_TXT_TIP_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 224+10/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_APP_AGIAN]);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);
    compo_textbox_set_visible(txt, false);

    return frm;
}

//显示升级界面处理
static void func_up_watch_dial_disp(void)
{
    f_up_watch_dial_t *f_up_watch_dial = (f_up_watch_dial_t *)func_cb.f_cb;

    if(f_up_watch_dial->state  != UPGRADING)
    {
        if(tick_get() > f_up_watch_dial->switch_page_time+SWITCH_TIME)
        {
            uteTaskGuiStartScreen(FUNC_CLOCK,0,__func__);
        }
        return;
    }

    if (tick_check_expire(f_up_watch_dial->ticks, 100))
    {
        f_up_watch_dial->ticks = tick_get();
        reset_sleep_delay_all();

        compo_progressbar_t *bar    = compo_getobj_byid(PROGRESS_BAR_ID);
        compo_textbox_t* txt_tip    = compo_getobj_byid(UPDATING_TXT_TIP_ID);
        compo_textbox_t* txt_state  = compo_getobj_byid(UPDATING_TXT_ID);
        compo_picturebox_t * picbox = compo_getobj_byid(ROCKET_ID);
        if(ble_is_connect())//ble状态
        {
            uint32_t progress = uteModuleWatchOnlineGetSynchronizeWatchSize()*100/uteModuleWatchOnlineGetTotileWatchSize();
            printf("progress:%d,SynchronizeWatchSize:%d,TotileWatchSize:%d\n",progress,uteModuleWatchOnlineGetSynchronizeWatchSize(),uteModuleWatchOnlineGetTotileWatchSize());
            if (progress>=100)
            {
                progress=100;
                f_up_watch_dial->state  = UPGRADE_SUCCESSFUL;
//                compo_textbox_set_visible(txt_tip, true);
                compo_textbox_set(txt_state, i18n[STR_SYNC_SUC]);
                compo_picturebox_set(picbox, UI_BUF_I330001_OTA_00_BIN);
                compo_progressbar_set_color(bar, make_color(0,0,0), make_color(0,0,0));
                f_up_watch_dial->switch_page_time = tick_get();
            }
            compo_progressbar_set_value(bar,progress);
        }
        else
        {
            f_up_watch_dial->state  = UPGRADE_FAILED;
            compo_textbox_set_visible(txt_tip, true);
            compo_textbox_set(txt_state, i18n[STR_SYNC_FAIL]);
            compo_picturebox_set(picbox, UI_BUF_I330001_OTA_02_BIN);
            compo_progressbar_set_color(bar, make_color(0,0,0), make_color(0,0,0));
            f_up_watch_dial->switch_page_time = tick_get();
        }
    }
}
#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
compo_form_t *func_up_watch_dial_form_create(void)
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
    compo_textbox_t* txt = compo_textbox_create(frm, 40);
    compo_setid(txt, UPDATING_TXT_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 250+32/2, 196, 30);
    compo_textbox_set(txt, i18n[STR_INS]);

    txt = compo_textbox_create(frm,  40);
    compo_setid(txt, UPDATING_TXT_TIP_ID);
    compo_textbox_set_location(txt, GUI_SCREEN_CENTER_X, 293+32/2, 224, 30);
    compo_textbox_set(txt, i18n[STR_APP_AGIAN]);
    compo_textbox_set_forecolor(txt, COLOR_GRAY);
    compo_textbox_set_visible(txt, false);

    return frm;
}

//显示升级界面处理
static void func_up_watch_dial_disp(void)
{
    f_up_watch_dial_t *f_up_watch_dial = (f_up_watch_dial_t *)func_cb.f_cb;

    if(f_up_watch_dial->state  != UPGRADING)
    {
        if(tick_get() > f_up_watch_dial->switch_page_time+SWITCH_TIME)
        {
            uteTaskGuiStartScreen(FUNC_CLOCK,0,__func__);
        }
        return;
    }

    if (tick_check_expire(f_up_watch_dial->ticks, 100))
    {
        f_up_watch_dial->ticks = tick_get();
        reset_sleep_delay_all();

        compo_progressbar_t *bar    = compo_getobj_byid(PROGRESS_BAR_ID);
        compo_textbox_t* txt_tip    = compo_getobj_byid(UPDATING_TXT_TIP_ID);
        compo_textbox_t* txt_state  = compo_getobj_byid(UPDATING_TXT_ID);
        compo_picturebox_t * picbox = compo_getobj_byid(ROCKET_ID);
        if(ble_is_connect())//ble状态
        {
            uint32_t progress = uteModuleWatchOnlineGetSynchronizeWatchSize()*100/uteModuleWatchOnlineGetTotileWatchSize();
            printf("progress:%d,SynchronizeWatchSize:%d,TotileWatchSize:%d\n",progress,uteModuleWatchOnlineGetSynchronizeWatchSize(),uteModuleWatchOnlineGetTotileWatchSize());
            if (progress>=100)
            {
                progress=100;
                f_up_watch_dial->state  = UPGRADE_SUCCESSFUL;
                compo_textbox_set(txt_state, i18n[STR_SYNC_SUC]);
                f_up_watch_dial->switch_page_time = tick_get();
                compo_arc_set_visible(bar,false);
                compo_picturebox_set(picbox,UI_BUF_I332001_OTA_01_BIN);
            }
            compo_arc_set_value(bar,progress*10);
        }
        else
        {
            f_up_watch_dial->state  = UPGRADE_FAILED;
            compo_textbox_set_visible(txt_tip, true);
            compo_textbox_set(txt_state, i18n[STR_SYNC_FAIL]);
            f_up_watch_dial->switch_page_time = tick_get();
            compo_arc_set_visible(bar,false);
            compo_picturebox_set(picbox,UI_BUF_I332001_OTA_02_BIN);
        }
    }
}
#elif GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
compo_form_t *func_up_watch_dial_form_create(void)
{
    compo_form_t *frm = compo_form_create(true);
    char txt_buf[30];

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
//////////////////////////////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_UPGRADE_UPDATE_SUCCESSED_ICON_SUCCEEDED_92X92_X74_Y65_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-35);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+55,150,50);
    compo_textbox_set(textbox,i18n[STR_UPDATED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_WATCH_RESTART]));
    compo_textbox_set_location(textbox,GUI_SCREEN_WIDTH+GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+95,150,30);
    compo_textbox_set(textbox,i18n[STR_WATCH_RESTART]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));
////////////////////////////////////////////////////////////////////////////////
    picbox = compo_picturebox_create(frm, UI_BUF_I335001_UPGRADE_UPGRADE_UNSUCCESSFUL_ICON_SYNC_FAILED_92X92_X74_Y65_BIN);
    compo_picturebox_set_pos(picbox, GUI_SCREEN_CENTER_X-GUI_SCREEN_WIDTH, GUI_SCREEN_CENTER_Y-35);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_UPDATE_FAILED]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_WIDTH,GUI_SCREEN_CENTER_Y+55,150,50);
    compo_textbox_set(textbox,i18n[STR_UPDATE_FAILED]);

    textbox = compo_textbox_create(frm, strlen(i18n[STR_APP_AGIAN]));
    compo_textbox_set_location(textbox,GUI_SCREEN_CENTER_X-GUI_SCREEN_WIDTH,GUI_SCREEN_CENTER_Y+95,150,30);
    compo_textbox_set(textbox,i18n[STR_APP_AGIAN]);
    widget_text_set_color(textbox->txt, make_color(128,128,128));
//////////////////////////////////////////////////////////////////////////////////////////
    return frm;
}

//显示升级界面处理
static void func_up_watch_dial_disp(void)
{
    f_up_watch_dial_t *f_up_watch_dial = (f_up_watch_dial_t *)func_cb.f_cb;

    if(f_up_watch_dial->state  != UPGRADING)
    {
        if(tick_get() > f_up_watch_dial->switch_page_time+SWITCH_TIME)
        {
            uteTaskGuiStartScreen(FUNC_CLOCK,0,__func__);
        }
        return;
    }

    if (tick_check_expire(f_up_watch_dial->ticks, 100))
    {
        f_up_watch_dial->ticks = tick_get();
        reset_sleep_delay_all();
        if(ble_is_connect())//ble状态
        {
            uint32_t progress = uteModuleWatchOnlineGetSynchronizeWatchSize()*100/uteModuleWatchOnlineGetTotileWatchSize();
            printf("progress:%d,SynchronizeWatchSize:%d,TotileWatchSize:%d\n",progress,uteModuleWatchOnlineGetSynchronizeWatchSize(),uteModuleWatchOnlineGetTotileWatchSize());
            if (progress>=100)
            {
                progress=100;
                if(f_up_watch_dial->state != UPGRADE_SUCCESSFUL)
                {
                    widget_page_set_client(func_cb.frm_main->page_body, -GUI_SCREEN_WIDTH, 0);
                }
                f_up_watch_dial->state  = UPGRADE_SUCCESSFUL;
            }
            char txt_buf[30];
            compo_textbox_t *txt_val   = compo_getobj_byid(PROGRESS_BAR_ID);
            snprintf(txt_buf,sizeof(txt_buf),"%d%%",progress);
            compo_textbox_set(txt_val,txt_buf);
        }
        else
        {
            if(f_up_watch_dial->state != UPGRADE_FAILED)
            {
                widget_page_set_client(func_cb.frm_main->page_body, GUI_SCREEN_WIDTH, 0);
            }
            f_up_watch_dial->state  = UPGRADE_FAILED;

        }
    }
}
#else
compo_form_t *func_up_watch_dial_form_create(void)
{
}
static void func_up_watch_dial_disp(void)
{
}
#endif // GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT


//刷新显示
static void func_up_watch_dial_process(void)
{
    func_up_watch_dial_disp();
    func_process();
}

//进入升级界面
static void func_up_watch_dial_enter(void)
{
    func_cb.f_cb = func_zalloc(sizeof(f_up_watch_dial_t));
    func_cb.frm_main = func_up_watch_dial_form_create();
    f_up_watch_dial_t *f_up_watch_dial = (f_up_watch_dial_t *)func_cb.f_cb;
    f_up_watch_dial->state = UPGRADING;
    f_up_watch_dial->switch_page_time = 0;
}

//退出升级界面
static void func_up_watch_dial_exit(void)
{
    func_cb.last = FUNC_UP_WATCH_DIAL;
}

//升级界面
void func_up_watch_dial(void)
{
    printf("%s\n", __func__);
    func_up_watch_dial_enter();
    reset_sleep_delay_all();
    while (func_cb.sta == FUNC_UP_WATCH_DIAL)
    {
        func_up_watch_dial_process();
//        func_message(msg_dequeue());
    }
    func_up_watch_dial_exit();
}
#endif
