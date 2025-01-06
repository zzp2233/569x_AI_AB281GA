#include "include.h"
#include "func.h"
#include "func_bt.h"
#include "ute_module_music.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define TITLE_BUF_LEN     UTE_MUSIC_TITLE_MAX_SIZE   //歌名buf长度
#define ARTIST_BUF_LEN    UTE_MUSIC_ARTLIST_MAX_SIZE     //歌手/歌手buf长度
#define PROGRESS_BAR_LENGTH GUI_SCREEN_WIDTH/1.65
#define BTN_REST_DISP_TIME 300   //按钮释放时间 ms 级别

enum
{
    COMPO_ID_BTN_PREV = 1,
    COMPO_ID_BTN_NEXT,
    COMPO_ID_BTN_PLAY,
    COMPO_ID_BTN_VOL_UP,
    COMPO_ID_BTN_VOL_DOWN,
    COMPO_ID_TXT_MUSIC_NAME,
    COMPO_ID_TXT_MUSIC_LYRIC,
    COMPO_ID_SHAPE_MUSIC_VOL,
};

typedef struct f_bt_t_
{
    uint32_t tick;
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    char title_buf_old[TITLE_BUF_LEN];
    char artist_buf_old[ARTIST_BUF_LEN];
} f_bt_t;

/*****************************************************************************
 * 1.当BT连接时，优先同步id3信息，控制通道走BT
 * 2.当只连接BLE时，IOS走AMS服务，安卓走私有协议
*****************************************************************************/

#if (BT_ID3_TAG_EN || LE_AMS_CLIENT_EN)
//static void func_bt_music_title_refresh(void *buf);
//static void func_bt_music_artist_refresh(void *buf);
#endif

//static void func_bt_music_play_btnpic_refresh(u8 sta);
//static void func_bt_music_vol_btnpic_refresh(u8 vol);

// static void bt_id3_tag_update_handle(u8 type, char *str)
// {
//    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
//    if (BT_ID3_TAG_TITLE == type)
//    {
//        memcpy(f_bt->title_buf, str, TITLE_BUF_LEN-1);
//        msg_enqueue(EVT_ID3_TITLE_UPDATE);
//    }
//    else if (BT_ID3_TAG_ARTIST == type)
//    {
//        memcpy(f_bt->artist_buf, str, ARTIST_BUF_LEN-1);
//        msg_enqueue(EVT_ID3_ARTIST_UPDATE);
//    }
// }

/*****************************************************************************
 *          BT or BLE interface
*****************************************************************************/

void func_bt_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0)
    {
        return;
    }
    bt_cb.res_bt_bypass = true;

    if (!sbc_is_bypass())
    {
        bt_audio_bypass();
    }
    mp3_res_play(addr, len);
}

void func_bt_mp3_play_restore(void)
{
    if (bt_cb.res_bt_bypass)
    {
        bt_cb.res_bt_bypass = false;
        bt_audio_enable();
    }
}

void func_bt_init(void)
{
    if (!bt_cb.bt_is_inited)
    {
        msg_queue_clear();
        dis_auto_pwroff();
        bsp_bt_init();
        bt_redial_init();
        bt_cb.bt_is_inited = 1;
    }
}

void func_bt_chk_off(void)
{
    if ((func_cb.sta != FUNC_BT) && (bt_cb.bt_is_inited))
    {
        bt_disconnect(1);
        bt_off();
        bt_cb.bt_is_inited = 0;
    }
}


/*****************************************************************************
 *          func_bt_music(UI)
*****************************************************************************/
//创建蓝牙音乐播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_form_create(void)
{
    static char title_buf[TITLE_BUF_LEN];
    static char artist_buf[ARTIST_BUF_LEN];
    uint16_t title_size_leng  = 0;
    uint16_t artist_size_leng = 0;
    memset(title_buf,0,sizeof(title_buf));
    memset(artist_buf,0,sizeof(artist_buf));
    uteModuleMusicGetPlayerTitle(title_buf,&title_size_leng);
    uteModuleMusicGetPlayerArtistSize(artist_buf,&artist_size_leng);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MUSIC]);
    compo_form_add_image(frm, UI_BUF_I330001_MUSIC_BG_BIN, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y );


    if(bt_is_connected() || ble_is_connect())
    {
        //歌词/歌手
        compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
        if(title_size_leng)
        {
            compo_textbox_set(name_txt,artist_buf);
        }
        else
        {
            compo_textbox_set(name_txt, i18n[STR_UNKNOWN]);
        }
        compo_textbox_set_forecolor(name_txt, COLOR_GRAY);

        //歌名
        compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/1.8, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);
        if(artist_size_leng)
        {
            compo_textbox_set(lyric_txt,title_buf);
        }
        else
        {
            compo_textbox_set(lyric_txt, i18n[STR_UNKNOWN]);
        }

    }
    else
    {
        compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
        compo_textbox_set(name_txt, i18n[STR_UNKNOWN]);
        compo_textbox_set_forecolor(name_txt, COLOR_GRAY );
        compo_textbox_set_visible(name_txt, false);

        compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_textbox_set(lyric_txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
        compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);
    }

    //新建按钮
    compo_button_t *btn;
    if(bt_is_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_PREVIOUSSONG02_BIN);///上一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_PREVIOUSSONG01_BIN);///上一曲
    }
    compo_setid(btn, COMPO_ID_BTN_PREV);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_CENTER_Y);

    if(bt_is_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_PLAY01_BIN);///暂停 播放
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_PLAY02_BIN);///暂停 播放
    }
    compo_setid(btn, COMPO_ID_BTN_PLAY);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }
    if(bt_cb.music_playing)
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_MUSIC_PAUSED_BIN);
    }
    else
    {
        if(bt_is_connected() || ble_is_connect())
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_MUSIC_PLAY01_BIN);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_MUSIC_PLAY02_BIN);
        }
    }


    if(bt_is_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_NEXTSONG02_BIN);///下一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_NEXTSONG01_BIN);///下一曲
    }
    compo_setid(btn, COMPO_ID_BTN_NEXT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_CENTER_Y);


    if(bt_is_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_VOLUME02_BIN);///音量减
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_VOLUME01_BIN);///音量减
    }
    compo_setid(btn, COMPO_ID_BTN_VOL_DOWN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.3, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.3);

    if(bt_is_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_VOLUME04_BIN);///音量加
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_VOLUME03_BIN);///音量加
    }
    compo_setid(btn, COMPO_ID_BTN_VOL_UP);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.3, GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.3);

    compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///音量进度条背景
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.3, PROGRESS_BAR_LENGTH, 6 );
    compo_shape_set_radius(shape, 3);
    compo_shape_set_color(shape,make_color(0x33,0x33,0x33) );

    shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///音量进度条
    compo_setid(shape, COMPO_ID_SHAPE_MUSIC_VOL);
    compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.3, PROGRESS_BAR_LENGTH, 6 );
    compo_shape_set_radius(shape, 3);
    if(bt_is_connected() || ble_is_connect())
    {
        compo_shape_set_visible(shape,true);
    }
    else
    {
        compo_shape_set_visible(shape,false);
    }
    uint8_t vol = uteModuleMusicGetPlayerVolume();
    if(vol>100)vol=100;
    compo_shape_set_location(shape, (GUI_SCREEN_WIDTH-PROGRESS_BAR_LENGTH)/2+vol*(PROGRESS_BAR_LENGTH/100)/2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.3,vol*(PROGRESS_BAR_LENGTH/100), 6 );

    return frm;
}

static void func_bt_music_refresh_disp(void)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    uint8_t vol = uteModuleMusicGetPlayerVolume();
    uint16_t title_size_leng  = 0;
    uint16_t artist_size_leng = 0;
    memset(f_bt->title_buf,0,sizeof(f_bt->title_buf));
    memset(f_bt->artist_buf,0,sizeof(f_bt->artist_buf));
    uteModuleMusicGetPlayerTitle(f_bt->title_buf,&title_size_leng);
    uteModuleMusicGetPlayerArtistSize(f_bt->artist_buf,&artist_size_leng);

//    bt_music_paly_status_info();

    if(strcmp(f_bt->title_buf, f_bt->title_buf_old)!=0 || title_size_leng == 0) //歌名刷新
    {
        memcpy(f_bt->title_buf_old, f_bt->title_buf, sizeof(f_bt->title_buf));
        compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_LYRIC);
        compo_textbox_set(tilte_txt, f_bt->title_buf);
        if(title_size_leng == 0)
        {
            compo_textbox_set(tilte_txt, i18n[STR_UNKNOWN]);
        }
//        printf("name=%s\n", f_bt->title_buf);
    }
//     printf("artist=%s leng=%d\n", f_bt->artist_buf ,artist_size_leng);
    if(strcmp(f_bt->artist_buf, f_bt->artist_buf_old)!=0 || artist_size_leng==0) //歌手刷新
    {
        memcpy(f_bt->artist_buf_old, f_bt->artist_buf, sizeof(f_bt->artist_buf));
        compo_textbox_t *tilte_art_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_NAME);
        compo_textbox_set(tilte_art_txt, f_bt->artist_buf);
        if(artist_size_leng == 0)
        {
            compo_textbox_set(tilte_art_txt, i18n[STR_UNKNOWN]);
        }

    }

    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }
//    printf("500ms->bt_cb.music_playing=%d\n",bt_cb.music_playing);
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    if(bt_cb.music_playing)
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_MUSIC_PAUSED_BIN);
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_MUSIC_PLAY01_BIN);
    }
    if(vol>100)vol=100;
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_SHAPE_MUSIC_VOL);
    compo_shape_set_location(shape, (GUI_SCREEN_WIDTH-PROGRESS_BAR_LENGTH)/2+vol*(PROGRESS_BAR_LENGTH/100)/2,GUI_SCREEN_CENTER_Y+GUI_SCREEN_CENTER_Y/1.3,vol*(PROGRESS_BAR_LENGTH/100), 6 );
}

///按钮释放
static void func_bt_button_release_handle()
{
    if(!bt_is_connected() && !ble_is_connect())return;

    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    s32 dx, dy;

    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_button_t *btn_up   = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
    compo_button_t *btn_down = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);

    func_bt_music_refresh_disp();

    if(f_bt->tick <= tick_get()-BTN_REST_DISP_TIME)
    {
        compo_button_set_bgimg(btn_prev,UI_BUF_I330001_MUSIC_PREVIOUSSONG02_BIN );
        compo_button_set_bgimg(btn_next,UI_BUF_I330001_MUSIC_NEXTSONG02_BIN );
        if (bt_cb.music_playing == false)
        {
//            compo_button_set_bgimg(btn_play,UI_BUF_I330001_MUSIC_PLAY01_BIN );
        }
        compo_button_set_bgimg(btn_up,UI_BUF_I330001_MUSIC_VOLUME04_BIN );
        compo_button_set_bgimg(btn_down,UI_BUF_I330001_MUSIC_VOLUME02_BIN );
    }

    int id = compo_get_button_id();
    if(id == 0 || !ctp_get_dxy(&dx, &dy)) return;

    f_bt->tick = tick_get();

    switch(id)
    {
        case COMPO_ID_BTN_PREV:
            compo_button_set_bgimg(btn_prev,UI_BUF_I330001_MUSIC_PREVIOUSSONG01_BIN );
            break;
        case COMPO_ID_BTN_NEXT:
            compo_button_set_bgimg(btn_next,UI_BUF_I330001_MUSIC_NEXTSONG01_BIN );
            break;
        case COMPO_ID_BTN_PLAY:
            if (bt_cb.music_playing == false)
            {
//                compo_button_set_bgimg(btn_play,UI_BUF_I330001_MUSIC_PLAY02_BIN );
            }
            break;
        case COMPO_ID_BTN_VOL_UP:
            compo_button_set_bgimg(btn_up,UI_BUF_I330001_MUSIC_VOLUME03_BIN );
            break;
        case COMPO_ID_BTN_VOL_DOWN:
            compo_button_set_bgimg(btn_down,UI_BUF_I330001_MUSIC_VOLUME01_BIN );
            break;
    }
}


//单击按钮
static void func_bt_button_click(void)
{
    int id = compo_get_button_id();
    switch (id)
    {
        case COMPO_ID_BTN_PREV:
            uteModuleMusicCtrl(false,true,false);
            break;

        case COMPO_ID_BTN_NEXT:
            uteModuleMusicCtrl(true,true,false);
            break;

        case COMPO_ID_BTN_PLAY:
//            printf("music_play111111111111111111\n");
            uteModuleMusicCtrlPaused(false);
            if(ble_is_connect())
            {
                bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
            }
//            printf("bt_cb.music_playing=%d\n",bt_cb.music_playing);
            break;

        case COMPO_ID_BTN_VOL_UP:
            uteModuleMusicCtrlVolumeIncrease(false);
            break;

        case COMPO_ID_BTN_VOL_DOWN:
            uteModuleMusicCtrlVolumeDecrease(false);
            break;

        default:
            break;
    }
}

void func_bt_sub_process(void)
{
    bsp_bt_status();
#if LE_HID_EN
    ble_hid_service_proc();
#endif // LE_HID_EN
}

void func_bt_process(void)
{
    func_bt_button_release_handle();
    func_process();
    func_bt_sub_process();

    if(sys_cb.pwroff_delay == 0)
    {
        func_cb.sta = FUNC_PWROFF;
        return;
    }
}

//蓝牙音乐消息处理
static void func_bt_message_do(size_msg_t msg)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;

    switch (msg)
    {
        case MSG_CTP_CLICK:
            if(!uteModuleCallBtIsConnected() && !ble_is_connect()) return;
            func_bt_button_click();                         //单击按钮
            func_bt_music_refresh_disp();
            break;

        case MSG_SYS_500MS:
//            printf("1111111111111111111111\n");
//            func_bt_music_refresh_disp();
//            if (bt_is_connected() && !sys_cb.gui_sleep_sta)
//            {
#if BT_ID3_TAG_EN
            if (bt_is_connected())
            {
                bt_music_paly_status_info();
            }
#endif
//                if (f_bt->bt_play_sta != bt_cb.music_playing)
//                {
//                    f_bt->bt_play_sta = bt_cb.music_playing;
//                    func_bt_music_play_btnpic_refresh(bt_cb.music_playing);
//                }
//                bt_vol_update();
//            }
            break;

//        case EVT_ID3_TITLE_UPDATE:
////            func_bt_music_refresh_disp();
////            func_bt_music_title_refresh(f_bt->title_buf);
//            break;

//        case EVT_ID3_ARTIST_UPDATE:
////            func_bt_music_refresh_disp();
////            func_bt_music_artist_refresh(f_bt->artist_buf);
//            break;

        default:
            func_message(msg);
            break;
    }
}
#if LE_HID_TEST
//AT(.text.func.bt.msg)
void func_ble_hid_message_do(u16 msg)
{
    switch (msg)
    {
        case MSG_CTP_CLICK:
            func_bt_button_click();                         //单击按钮
            break;
//    case KU_PLAY_PWR_USER_DEF:
//        printf("tiktok click\n");
//        ble_hid_event_tiktok(BLE_HID_MSG_VOLUME_DOWN);
//        break;
//
//    case KU_PREV_VOL_DOWN:
//        printf("tiktok prev\n");
//        ble_hid_event_tiktok(BLE_HID_MSG_UP);
//        break;
//
//    case KU_NEXT_VOL_UP:
//        printf("tiktok next\n");
//        ble_hid_event_tiktok(BLE_HID_MSG_DOWN);
//        break;

        default:
            func_message(msg);
            break;
    }
}
#endif
AT(.text.func.bt.msg)
void func_bt_message(u16 msg)
{
#if LE_HID_TEST
    func_ble_hid_message_do(msg);
#else
    func_bt_message_do(msg);
#endif
}

void func_bt_enter(void)
{
    f_bt_t *f_bt;
    func_cb.f_cb = func_zalloc(sizeof(f_bt_t));
    func_cb.frm_main = func_bt_form_create();
    f_bt = (f_bt_t *)func_cb.f_cb;

//    func_cb.mp3_res_play = func_bt_mp3_res_play;
//    bt_cb.bt_form_created = 1;
//    f_bt->bt_play_sta = bt_cb.music_playing;
//    f_bt->ams_play_sta = false;
//    f_bt->vol = 0;
//    if (sys_cb.music_title_init)
//    {
//        memcpy(f_bt->title_buf, sys_cb.title_buf, sizeof(sys_cb.title_buf));
//        msg_enqueue(EVT_ID3_TITLE_UPDATE);
//    }
//    if (sys_cb.music_artist_init)
//    {
//        memcpy(f_bt->artist_buf, sys_cb.artist_buf, sizeof(sys_cb.artist_buf));
//        msg_enqueue(EVT_ID3_ARTIST_UPDATE);
//    }
//    printf("%s->%d\n", __func__, f_bt->bt_play_sta);
//    func_bt_music_play_btnpic_refresh(f_bt->bt_play_sta);
//#if LE_AMS_CLIENT_EN
//    if (ble_ams_is_connected())
//    {
//        if (ble_ams_cb.play_state)
//        {
//            f_bt->ams_play_sta = true;
//            func_bt_music_play_btnpic_refresh(1);
//        }
//        f_bt->ams_vol = ble_ams_cb.vol;
//        func_bt_music_vol_btnpic_refresh(f_bt->ams_vol);
//        if (strlen(ble_ams_cb.app_name))
//        {
//            compo_form_set_title(func_cb.frm_main, ble_ams_cb.app_name);
//        }
//    }
//    ble_ams_sta_update_cb_reg(ble_ams_sta_update_handle);
//#endif
    // bt_id3_tag_update_cb_reg(bt_id3_tag_update_handle);
#if BT_ID3_TAG_EN
    bt_music_get_id3_tag();
#endif

#if !BT_BACKSTAGE_MUSIC_EN
    func_bt_init();
    bt_audio_enable();
#if DAC_DNR_EN
    dac_dnr_set_sta(1);
    sys_cb.dnr_sta = 1;
#endif
#endif // !BT_BACKSTAGE_MUSIC_EN
}

void func_bt_exit(void)
{
#if !BT_BACKSTAGE_MUSIC_EN
    dac_fade_out();
#if DAC_DNR_EN
    dac_dnr_set_sta(0);
    sys_cb.dnr_sta = 0;
#endif
    bt_audio_bypass();

#if !BT_BACKSTAGE_EN
    bt_disconnect(1);
    bt_off();
    bt_cb.bt_is_inited = 0;
#else
    if (bt_get_status() == BT_STA_PLAYING && !bt_is_testmode())          //蓝牙退出停掉音乐
    {
        delay_5ms(10);
        if(bt_get_status() == BT_STA_PLAYING)       //再次确认play状态
        {
            u32 timeout = 850; //8.5s
            bt_music_pause();
            while (bt_get_status() == BT_STA_PLAYING && timeout > 0)
            {
                timeout--;
                delay_5ms(2);
            }
        }
    }
#endif  // !BT_BACKSTAGE_EN
#endif  // !BT_BACKSTAGE_MUSIC_EN
    ble_ams_sta_update_cb_unreg();
    bt_id3_tag_update_cb_unreg();

//    bt_cb.bt_form_created = 0;
    bt_cb.rec_pause = 0;
    func_cb.last = FUNC_BT;
}

void func_bt(void)
{
    printf("%s\n", __func__);
    func_bt_enter();
    while (func_cb.sta == FUNC_BT)
    {
        func_bt_process();
        func_bt_message(msg_dequeue());
    }
    func_bt_exit();
}
