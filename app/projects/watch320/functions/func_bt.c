#include "include.h"
#include "func.h"
#include "func_bt.h"

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define TITLE_BUF_LEN     128   //歌词buffer长度
#define ARTIST_BUF_LEN    128   //歌名/歌手buffer长度

enum {
    COMPO_ID_BTN_PREV = 1,
    COMPO_ID_BTN_NEXT,
    COMPO_ID_BTN_PLAY,
    COMPO_ID_BTN_VOL_UP,
    COMPO_ID_BTN_VOL_DOWN,
    COMPO_ID_TXT_MUSIC_NAME,
    COMPO_ID_TXT_MUSIC_LYRIC,
    COMPO_ID_PIC_MUSIC_VOL,
};

typedef struct f_bt_t_ {
    bool ams_play_sta;
    bool bt_play_sta;
    u8 vol;
    u8 ams_vol;
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
} f_bt_t;

/*****************************************************************************
 * 1.当BT连接时，优先同步id3信息，控制通道走BT
 * 2.当只连接BLE时，IOS走AMS服务，安卓走私有协议
*****************************************************************************/

#if (BT_ID3_TAG_EN || LE_AMS_CLIENT_EN)
static void func_bt_music_title_refresh(void *buf);
static void func_bt_music_artist_refresh(void *buf);
#endif

static void func_bt_music_play_btnpic_refresh(u8 sta);
static void func_bt_music_vol_btnpic_refresh(u8 vol);

/*****************************************************************************
 *          BT or BLE interface
*****************************************************************************/

void func_bt_mp3_res_play(u32 addr, u32 len)
{
    if (len == 0) {
        return;
    }
    bt_cb.res_bt_bypass = true;

    if (!sbc_is_bypass()) {
        bt_audio_bypass();
    }
    mp3_res_play(addr, len);
}

void func_bt_mp3_play_restore(void)
{
    if (bt_cb.res_bt_bypass) {
        bt_cb.res_bt_bypass = false;
        bt_audio_enable();
    }
}

void func_bt_init(void)
{
    if (!bt_cb.bt_is_inited) {
        msg_queue_clear();
        dis_auto_pwroff();
        bsp_bt_init();
        bt_redial_init();
        bt_cb.bt_is_inited = 1;
    }
}

void func_bt_chk_off(void)
{
    if ((func_cb.sta != FUNC_BT) && (bt_cb.bt_is_inited)) {
        bt_disconnect(1);
        bt_off();
        bt_cb.bt_is_inited = 0;
    }
}


//播放暂停控制
static void bt_control_play_pause(void)
{
    //f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;

    if (bt_is_connected()) {
        bt_music_play_pause();
    } else if (ble_ams_is_connected()) {
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_PLAY_PAUSE);
    }
}

//切换上一曲
static void bt_control_prev(void)
{
    if (bt_is_connected()) {
        bt_music_prev();
    } else if (ble_ams_is_connected()) {
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_PREV_TRACK);
    }
}

//切换下一曲
static void bt_control_next(void)
{
    if (bt_is_connected()) {
        bt_music_next();
    } else if (ble_ams_is_connected()) {
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_NEXT_TRACK);
    }
}

//音量更新
static void bt_vol_update(void)
{
    if (!bt_cb.bt_form_created) {
        return ;
    }

    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;

    bool vol_uodate_flag = false;

    if (bt_is_connected()) {
        if (f_bt->vol != sys_cb.vol) {
            f_bt->vol = sys_cb.vol;
            vol_uodate_flag = true;
        }
    } else if (ble_ams_is_connected()) {
        if (f_bt->vol != f_bt->ams_vol) {
            f_bt->vol = f_bt->ams_vol;
            vol_uodate_flag = true;
        }
    }

    if (vol_uodate_flag) {
        func_bt_music_vol_btnpic_refresh(f_bt->vol);
    }
}

//音量控制+
static void bt_control_vol_up(void)
{
    if (bt_is_connected()) {
        bt_volume_up();
    } else if (ble_ams_is_connected()) {
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_VOL_UP);
    }
    bt_vol_update();
}

//音量控制-
static void bt_control_vol_down(void)
{
    if (bt_is_connected()) {
        bt_volume_down();
    } else if (ble_ams_is_connected()) {
        ble_ams_remote_ctrl(AMS_REMOTE_CMD_VOL_DOWN);
    }
    bt_vol_update();
}

#if LE_AMS_CLIENT_EN
//AMS推送状态更新处理
static void ble_ams_sta_update_handle(u8 sta, void *p_data, u16 len)
{
    if (bt_is_connected() || !ble_ams_is_connected() || !bt_cb.bt_form_created) {
        return ;
    }
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;

    switch (sta) {
        case BLE_AMS_STA_UPDATE_PAUSE:
            {
                f_bt->ams_play_sta = false;
                func_bt_music_play_btnpic_refresh(false);
            }
            break;

        case BLE_AMS_STA_UPDATE_PLAYING:
            {
                 f_bt->ams_play_sta = true;
                func_bt_music_play_btnpic_refresh(true);
            }
            break;

        case BLE_AMS_STA_UPDATE_VOLUME:
            {
                u32 volume = GET_LE32(p_data);
                f_bt->ams_vol = volume / 625;
                bt_vol_update();
                TRACE("ams volume:%d, ams_vol:%d\n", volume, f_bt->ams_vol);
            }
            break;

        case BLE_AMS_STA_UPDATE_TITLE:
            {
                char *title = (char *)p_data;
                if (strlen(title)) {
                    memcpy(f_bt->title_buf, title, TITLE_BUF_LEN-1);
                    msg_enqueue(EVT_ID3_TITLE_UPDATE);
                    TRACE("ams title:%s\n",title);
                }
            }
            break;

        case BLE_AMS_STA_UPDATE_ALBUM:
            {
                char * album = (char *)p_data;
                if (strlen(album)) {
                    memcpy(f_bt->artist_buf, album, ARTIST_BUF_LEN-1);
                    msg_enqueue(EVT_ID3_ARTIST_UPDATE);
                    TRACE("ams album:%s\n",album);
                }
            }
            break;

        case BLE_AMS_STA_UPDATE_ARTIST:
            {
                char * artist = (char *)p_data;
                if (strlen(artist)) {
                    memcpy(f_bt->artist_buf, artist, ARTIST_BUF_LEN-1);
                    msg_enqueue(EVT_ID3_ARTIST_UPDATE);
                    TRACE("ams artist:%s\n",artist);
                }
            }
            break;

        case BLE_AMS_STA_UPDATE_APP_NAME:
            {
                char * app_name = (char *)p_data;
                if (strlen(app_name)) {
                    compo_form_set_title(func_cb.frm_main, app_name);
                    TRACE("ams app name:%s\n",app_name);
                }
            }
            break;
    }
}
#endif // LE_AMS_CLIENT_EN

#if BT_ID3_TAG_EN
//id3信息更新处理
static void bt_id3_tag_update_handle(u8 type, char *str)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    if (BT_ID3_TAG_TITLE == type) {
        memcpy(f_bt->title_buf, str, TITLE_BUF_LEN-1);
        msg_enqueue(EVT_ID3_TITLE_UPDATE);
    } else if (BT_ID3_TAG_ARTIST == type) {
        memcpy(f_bt->artist_buf, str, ARTIST_BUF_LEN-1);
        msg_enqueue(EVT_ID3_ARTIST_UPDATE);
    }
}
#endif // BT_ID3_TAG_EN


/*****************************************************************************
 *          func_bt_music(UI)
*****************************************************************************/
//创建蓝牙音乐播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_form_create(void)
{
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MUSIC]);

    //歌名
    compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 100, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
    compo_textbox_set(name_txt, i18n[STR_UNKNOWN]);

    //歌词
    compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
    compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y - 50, GUI_SCREEN_WIDTH, 50);
    compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);
    compo_textbox_set(lyric_txt, i18n[STR_UNKNOWN]);

    //新建按钮
    compo_button_t *btn;
    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_PREV_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_PREV);
    compo_button_set_pos(btn, 53, 248);

    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_PLAY_BIN);
    compo_setid(btn, COMPO_ID_BTN_PLAY);
    compo_button_set_pos(btn, 160, 245);

    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_NEXT_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_NEXT);
    compo_button_set_pos(btn, 267, 248);

    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_VOLUME_DOWN_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_VOL_DOWN);
    compo_button_set_pos(btn, 62, 340);

    btn = compo_button_create_by_image(frm, UI_BUF_MUSIC_VOLUME_UP_CLICK_BIN);
    compo_setid(btn, COMPO_ID_BTN_VOL_UP);
    compo_button_set_pos(btn, 258, 340);

    compo_picturebox_t *vol_pic = compo_picturebox_create(frm, UI_BUF_MUSIC_VOLUME1_BIN);
    compo_setid(vol_pic, COMPO_ID_PIC_MUSIC_VOL);
    compo_picturebox_set_pos(vol_pic, 110, 335);
    widget_set_align_center(vol_pic->img, false);
    compo_picturebox_set_visible(vol_pic, false);

    return frm;
}

#if (BT_ID3_TAG_EN || LE_AMS_CLIENT_EN)
static void func_bt_music_title_refresh(void *buf)
{
    if (bt_cb.bt_form_created) {
        compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_LYRIC);
        compo_textbox_set(tilte_txt, buf);
    }
}

static void func_bt_music_artist_refresh(void *buf)
{
    if (bt_cb.bt_form_created) {
        compo_textbox_t *tilte_art_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_NAME);
        compo_textbox_set(tilte_art_txt, buf);
    }
}
#endif // BT_ID3_TAG_EN

static void func_bt_music_play_btnpic_refresh(u8 sta)
{
    if (!bt_cb.bt_form_created) {
        return ;
    }
    compo_button_t *btn = compo_getobj_byid(COMPO_ID_BTN_PLAY);

    if (sta) {
        compo_button_set_bgimg(btn, UI_BUF_MUSIC_PAUSE_BIN);
    } else {
        compo_button_set_bgimg(btn, UI_BUF_MUSIC_PLAY_BIN);
    }
}

static void func_bt_music_vol_btnpic_refresh(u8 vol)
{
    compo_picturebox_t *vol_pic;

    vol_pic = compo_getobj_byid(COMPO_ID_PIC_MUSIC_VOL);
    switch (vol){
        case 0:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME1_BIN);
            compo_picturebox_set_visible(vol_pic, false);
            break;

        case 1 ... 3:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME1_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;

        case 4 ... 6:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME2_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;
        case 7 ... 9:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME3_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;
        case 10 ... 11:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME4_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;
        case 12 ... 16:
            compo_picturebox_set(vol_pic, UI_BUF_MUSIC_VOLUME5_BIN);
            compo_picturebox_set_visible(vol_pic, true);
            break;
    }
}
extern void btstack_ble_sm_req_for_android(void);
//单击按钮
static void func_bt_button_click(void)
{
    int id = compo_get_button_id();
    switch (id) {
    case COMPO_ID_BTN_PREV:
        #if LE_HID_TEST
        ble_hid_event_tiktok(BLE_HID_MSG_UP);
        #else
        bt_control_prev();
        #endif // LE_HID_TEST
        break;

    case COMPO_ID_BTN_NEXT:
        #if LE_HID_TEST
        ble_hid_event_tiktok(BLE_HID_MSG_DOWN);
        #else
        bt_control_next();
        #endif // LE_HID_TEST
        break;

    case COMPO_ID_BTN_PLAY:
        #if LE_HID_TEST
        if (!ble_hid_peer_device_is_ios()){
            btstack_ble_sm_req_for_android();
        }
        #else
        bt_control_play_pause();
        #endif // LE_HID_TEST
        break;

    case COMPO_ID_BTN_VOL_UP:
        #if LE_HID_TEST
        ble_hid_event_tiktok(BLE_HID_MSG_VOLUME_UP);
        #else
        bt_control_vol_up();
        #endif // LE_HID_TEST
        break;

    case COMPO_ID_BTN_VOL_DOWN:
        #if LE_HID_TEST
        ble_hid_event_tiktok(BLE_HID_MSG_VOLUME_DOWN);
        #else
        bt_control_vol_down();
        #endif // LE_HID_TEST
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
    func_process();
    func_bt_sub_process();

    if(sys_cb.pwroff_delay == 0) {
        func_cb.sta = FUNC_PWROFF;
        return;
    }
}

//蓝牙音乐消息处理
static void func_bt_message_do(size_msg_t msg)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;

    switch (msg) {
    case MSG_CTP_CLICK:
        func_bt_button_click();                         //单击按钮
        break;

    case MSG_SYS_500MS:
        if (bt_is_connected() && !sys_cb.gui_sleep_sta) {
#if BT_ID3_TAG_EN
            bt_music_paly_status_info();
#endif
            if (f_bt->bt_play_sta != bt_cb.music_playing) {
                f_bt->bt_play_sta = bt_cb.music_playing;
                func_bt_music_play_btnpic_refresh(bt_cb.music_playing);
            }
            bt_vol_update();
        }
        break;

    case EVT_ID3_TITLE_UPDATE:
        func_bt_music_title_refresh(f_bt->title_buf);
        break;

    case EVT_ID3_ARTIST_UPDATE:
        func_bt_music_artist_refresh(f_bt->artist_buf);
        break;

    default:
        func_message(msg);
        break;
    }
}
#if LE_HID_TEST
//AT(.text.func.bt.msg)
void func_ble_hid_message_do(u16 msg)
{
    switch (msg) {
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

    func_cb.mp3_res_play = func_bt_mp3_res_play;
    bt_cb.bt_form_created = 1;
    f_bt->bt_play_sta = false;
    f_bt->ams_play_sta = false;
    f_bt->vol = 0;
    func_bt_music_play_btnpic_refresh(0);
#if LE_AMS_CLIENT_EN
    if (ble_ams_is_connected()) {
        if (ble_ams_cb.play_state) {
            f_bt->ams_play_sta = true;
            func_bt_music_play_btnpic_refresh(1);
        }
        f_bt->ams_vol = ble_ams_cb.vol;
        func_bt_music_vol_btnpic_refresh(f_bt->ams_vol);
        if (strlen(ble_ams_cb.app_name)) {
            compo_form_set_title(func_cb.frm_main, ble_ams_cb.app_name);
        }
    }
    ble_ams_sta_update_cb_reg(ble_ams_sta_update_handle);
#endif
    bt_id3_tag_update_cb_reg(bt_id3_tag_update_handle);

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
    if (bt_get_status() == BT_STA_PLAYING && !bt_is_testmode()) {        //蓝牙退出停掉音乐
        delay_5ms(10);
        if(bt_get_status() == BT_STA_PLAYING) {     //再次确认play状态
            u32 timeout = 850; //8.5s
            bt_music_pause();
            while (bt_get_status() == BT_STA_PLAYING && timeout > 0) {
                timeout--;
                delay_5ms(2);
            }
        }
    }
#endif  // !BT_BACKSTAGE_EN
#endif  // !BT_BACKSTAGE_MUSIC_EN
    ble_ams_sta_update_cb_unreg();
    bt_id3_tag_update_cb_unreg();

    bt_cb.bt_form_created = 0;
    bt_cb.rec_pause = 0;
    func_cb.last = FUNC_BT;
}

void func_bt(void)
{
    printf("%s\n", __func__);
    func_bt_enter();
    while (func_cb.sta == FUNC_BT) {
        func_bt_process();
        func_bt_message(msg_dequeue());
    }
    func_bt_exit();
}
