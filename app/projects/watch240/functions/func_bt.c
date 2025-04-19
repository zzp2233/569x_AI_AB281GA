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

#if UTE_MODULE_SCREENS_MUSIC_SUPPORT

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

//单击按钮
#if GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT
#define TITLE_BUF_LEN     UTE_MUSIC_TITLE_MAX_SIZE   //歌名buf长度
#define ARTIST_BUF_LEN    UTE_MUSIC_ARTLIST_MAX_SIZE     //歌手/歌手buf长度
#define PROGRESS_BAR_LENGTH 146
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
    COMPO_ID_MUSIC_BG_FIG,
};

typedef struct f_bt_t_
{
    uint32_t tick;
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    char title_buf_old[TITLE_BUF_LEN];
    char artist_buf_old[ARTIST_BUF_LEN];
    bool refresh_data;
} f_bt_t;
/*****************************************************************************
 *          func_bt_music(UI)
*****************************************************************************/
//创建蓝牙音乐播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_form_create(void)
{
    char title_buf[TITLE_BUF_LEN];
    uint16_t title_size_leng  = 0;
    memset(title_buf,0,sizeof(title_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)title_buf,&title_size_leng);
    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        memset(title_buf,0,sizeof(title_buf));
        snprintf(title_buf,sizeof(title_buf),"%s",i18n[STR_CONNECT_BLUETOOTH]);
    }
    else if(title_size_leng==0)
    {
        memset(title_buf,0,sizeof(title_buf));
        snprintf(title_buf,sizeof(title_buf),"%s",i18n[STR_UNKNOWN]);
    }
    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MUSIC]);

    //bg
    compo_picturebox_t *bg_pic = compo_picturebox_create(frm, UI_BUF_I335001_MUSIC_1_MUSIC_BG_240X210_X0_Y46_BIN);
    compo_setid(bg_pic, COMPO_ID_MUSIC_BG_FIG);
    compo_picturebox_set_pos(bg_pic, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_picturebox_cut(bg_pic,0,2);

    //歌名
    compo_textbox_t *lyric_txt = compo_textbox_create(frm, TITLE_BUF_LEN);
    compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X,44,230, 30);
    compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
    compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);
    compo_textbox_set(lyric_txt,title_buf);

    compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I335001_MUSIC_1_MUSIC_ICON_BUTTON_28X30_X24_Y137_X188_Y137_PREVIOUS_SONG_BIN);///上一曲
    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_BUTTON_28X30_X24_Y137_X188_Y137_PREVIOUS_SONG_BIN);///上一曲
    }
    compo_setid(btn, COMPO_ID_BTN_PREV);
    compo_button_set_pos(btn,24+14, 137+15);

    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_BUTTON_28X30_X24_Y137_X188_Y137_NEXT_SONG_BIN);///下一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I335001_MUSIC_1_MUSIC_ICON_BUTTON_28X30_X24_Y137_X188_Y137_NEXT_SONG_BIN);///下一曲
    }
    compo_setid(btn, COMPO_ID_BTN_NEXT);
    compo_button_set_pos(btn,188+14, 137+15);

    if(ble_is_connect())
    {
        bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
    }
    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        // compo_button_set_bgimg(btn,UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_96X96_X72_Y104_BIN);
        btn = compo_button_create_by_image(frm,UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_96X96_X72_Y104_BIN);///灰色的暂停按钮
    }
    else
    {
        btn = compo_button_create_by_image(frm,bt_cb.music_playing ? UI_BUF_I335001_MUSIC_1_MUSIC_ICON_BUTTON_96X96_X72_Y104_01_BIN:UI_BUF_I335001_MUSIC_1_MUSIC_ICON_BUTTON_96X96_X72_Y104_02_BIN);///暂停 播放
    }
    compo_setid(btn, COMPO_ID_BTN_PLAY);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, 152);

    btn = compo_button_create_by_image(frm, UI_BUF_I335001_MUSIC_1_MUSIC_ICON_26X22_X26_Y248_X192_Y248_REDUCTION_BIN);///音量减
    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_26X22_X26_Y248_X192_Y248_REDUCTION_BIN);///音量减
    }
    compo_setid(btn, COMPO_ID_BTN_VOL_DOWN);
    compo_button_set_pos(btn, 26+13,248+11);

    btn = compo_button_create_by_image(frm, UI_BUF_I335001_MUSIC_1_MUSIC_ICON_26X22_X26_Y248_X192_Y248_ADD_BIN);///音量加
    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_26X22_X26_Y248_X192_Y248_ADD_BIN);///音量加
    }
    compo_setid(btn, COMPO_ID_BTN_VOL_UP);
    compo_button_set_pos(btn, 192+13,248+11);

    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    if(vol>16)vol=16;

    compo_picturebox_t *pic = compo_picturebox_create(frm,UI_BUF_I335001_MUSIC_1_MUSIC_ICON_SOUND_120X4_X60_Y257_BIN);
    compo_picturebox_set_pos(pic,60+60,257+2);
    compo_picturebox_cut(pic,vol,17);
    compo_setid(pic,COMPO_ID_SHAPE_MUSIC_VOL);


    return frm;
}
static void func_bt_music_refresh_disp(void)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    compo_picturebox_t *pic    = compo_getobj_byid(COMPO_ID_SHAPE_MUSIC_VOL);
    compo_button_t *btn_play   = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_LYRIC);

    char title_buf[TITLE_BUF_LEN];
    uint16_t title_size_leng  = 0;
    memset(title_buf,0,sizeof(title_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)title_buf,&title_size_leng);
    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        memset(title_buf,0,sizeof(title_buf));
        snprintf(title_buf,sizeof(title_buf),"%s",i18n[STR_CONNECT_BLUETOOTH]);
    }
    else if(title_size_leng==0)
    {
        memset(title_buf,0,sizeof(title_buf));
        snprintf(title_buf,sizeof(title_buf),"%s",i18n[STR_UNKNOWN]);
    }
    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_PLAY);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_96X96_X72_Y104_BIN);///灰色的暂停按钮
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_PREV);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_BUTTON_28X30_X24_Y137_X188_Y137_PREVIOUS_SONG_BIN);//上一首
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_NEXT);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_BUTTON_28X30_X24_Y137_X188_Y137_NEXT_SONG_BIN);//下一首
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_26X22_X26_Y248_X192_Y248_REDUCTION_BIN);//音量减
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_2_NOT_CONNECTED_ICON_26X22_X26_Y248_X192_Y248_ADD_BIN);//音量加
    }
    else
    {
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_PLAY);
        compo_button_set_bgimg(btn_play,bt_cb.music_playing ? UI_BUF_I335001_MUSIC_1_MUSIC_ICON_BUTTON_96X96_X72_Y104_01_BIN:UI_BUF_I335001_MUSIC_1_MUSIC_ICON_BUTTON_96X96_X72_Y104_02_BIN);///暂停 播放
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_PREV);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_1_MUSIC_ICON_BUTTON_28X30_X24_Y137_X188_Y137_PREVIOUS_SONG_BIN);//上一首
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_NEXT);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_1_MUSIC_ICON_BUTTON_28X30_X24_Y137_X188_Y137_NEXT_SONG_BIN);//下一首
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_1_MUSIC_ICON_26X22_X26_Y248_X192_Y248_REDUCTION_BIN);//音量减
        btn_play   = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
        compo_button_set_bgimg(btn_play,UI_BUF_I335001_MUSIC_1_MUSIC_ICON_26X22_X26_Y248_X192_Y248_ADD_BIN);//音量加
    }
    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    if(vol>16)vol=16;
    compo_picturebox_cut(pic,vol,17);

    if(!uteModuleCallBtIsConnected() && !ble_is_connect())
    {
        memset(title_buf,0,sizeof(title_buf));
        snprintf(title_buf,sizeof(title_buf),"%s",i18n[STR_CONNECT_BLUETOOTH]);
        compo_textbox_set(tilte_txt, title_buf);
    }
    else if(strcmp(f_bt->title_buf, f_bt->title_buf_old)!=0 || title_size_leng == 0) //歌名刷新
    {
        f_bt->refresh_data = false;
        memcpy(f_bt->title_buf_old, f_bt->title_buf, sizeof(f_bt->title_buf));
        compo_textbox_set(tilte_txt, f_bt->title_buf);
        if(title_size_leng == 0)
        {
            compo_textbox_set(tilte_txt, i18n[STR_UNKNOWN]);
        }
    }
}
static void func_bt_button_release_handle()
{
}

#elif GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT
#define TITLE_BUF_LEN     UTE_MUSIC_TITLE_MAX_SIZE   //歌名buf长度
#define ARTIST_BUF_LEN    UTE_MUSIC_ARTLIST_MAX_SIZE     //歌手/歌手buf长度
#define PROGRESS_BAR_LENGTH 146
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
    COMPO_ID_PIC_MUSIC_VOL,
};

typedef struct f_bt_t_
{
    uint32_t tick;
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    char title_buf_old[TITLE_BUF_LEN];
    char artist_buf_old[ARTIST_BUF_LEN];
    bool refresh_data;
} f_bt_t;
/*****************************************************************************
 *          func_bt_music(UI)
*****************************************************************************/
//创建蓝牙音乐播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_form_create(void)
{
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    uint16_t title_size_leng  = 0;
    uint16_t artist_size_leng = 0;
    memset(title_buf,0,sizeof(title_buf));
    memset(artist_buf,0,sizeof(artist_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)title_buf,&title_size_leng);
    uteModuleMusicGetPlayerArtistSize((uint8_t *)artist_buf,&artist_size_leng);

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MUSIC]);
    compo_form_add_image(frm, UI_BUF_I330001_MUSIC_BG_BIN, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y );

    compo_textbox_t *name_txt;

    if(bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        //歌词/歌手
        name_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);

        compo_textbox_set(name_txt,artist_buf);
        if(title_size_leng == 0)
        {
            compo_textbox_set(name_txt, i18n[STR_UNKNOWN]);
        }
        compo_textbox_set_forecolor(name_txt, COLOR_GRAY);

        //歌名
        compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/1.8, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);


        compo_textbox_set(lyric_txt,title_buf);
        if(artist_size_leng == 0)
        {
            compo_textbox_set(lyric_txt, i18n[STR_UNKNOWN]);
        }
    }
    else
    {
        name_txt = compo_textbox_create(frm, 50);
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
    if(bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_PREVIOUSSONG02_BIN);///上一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_PREVIOUSSONG01_BIN);///上一曲
    }
    compo_setid(btn, COMPO_ID_BTN_PREV);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_CENTER_Y);

    btn = compo_button_create_by_image(frm, 0);///暂停 播放
    if(bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        if(bt_cb.music_playing)
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_MUSIC_PAUSED_BIN);
        }
        else
        {
            compo_button_set_bgimg(btn, UI_BUF_I330001_MUSIC_PLAY01_BIN);
        }
    }
    else
    {
        compo_button_set_bgimg(btn, UI_BUF_I330001_MUSIC_PLAY02_BIN);
    }
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);
    compo_setid(btn, COMPO_ID_BTN_PLAY);


    if(bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_NEXTSONG02_BIN);///下一曲
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_NEXTSONG01_BIN);///下一曲
    }
    compo_setid(btn, COMPO_ID_BTN_NEXT);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_CENTER_Y);

    widget_page_t *page = widget_page_create(frm->page);///创建按键页面
    widget_set_location(page,GUI_SCREEN_CENTER_X, 259+4/2,128,4);

    // compo_shape_t *shape = compo_shape_create_for_page(frm,page,COMPO_SHAPE_TYPE_RECTANGLE);///音量进度条背景
    // compo_shape_set_location(shape, 58/2-2, 4/2, 360, 360 );

    compo_picturebox_t *picbox = compo_picturebox_create_for_page(frm,page,UI_BUF_I330001_MUSIC_ICON3_128X4_X56_Y259_BIN);
    compo_picturebox_set_pos(picbox, 128/2, 4/2);
    picbox = compo_picturebox_create_for_page(frm,page,UI_BUF_I330001_MUSIC_ICON2_128X4_X56_Y259_BIN);
    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    if(vol>16)vol=16;
    compo_picturebox_set_pos(picbox, (-128/2)+vol*(128/16), 4/2);
    compo_picturebox_set_visible(picbox, ble_is_connect()||bt_a2dp_profile_completely_connected());
    compo_setid(picbox,COMPO_ID_PIC_MUSIC_VOL);

    picbox = compo_picturebox_create_for_page(frm,page,UI_BUF_I330001_MUSIC_ICON1_58X4_X0_Y259_BIN);
    compo_picturebox_set_pos(picbox, -(58/2)+2, 4/2);

    if(bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_VOLUME02_BIN);///音量减
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_VOLUME01_BIN);///音量减
    }
    compo_setid(btn, COMPO_ID_BTN_VOL_DOWN);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.3, 28/2+247);

    if(bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_VOLUME04_BIN);///音量加
    }
    else
    {
        btn = compo_button_create_by_image(frm, UI_BUF_I330001_MUSIC_VOLUME03_BIN);///音量加
    }
    compo_setid(btn, COMPO_ID_BTN_VOL_UP);
    compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.3,  28/2+247);

    if(name_txt != NULL && ble_is_connect())
    {
        if(!bt_a2dp_profile_completely_connected() )
        {
            compo_textbox_set_visible(name_txt, false);
        }
        else
        {
            compo_textbox_set_visible(name_txt, true);
        }
    }



    return frm;
}

static void func_bt_music_refresh_disp(void)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_button_t *btn_up   = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
    compo_button_t *btn_down = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);
    compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_LYRIC);
    compo_textbox_t *tilte_art_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_NAME);
    compo_picturebox_t *pic_value = compo_getobj_byid(COMPO_ID_PIC_MUSIC_VOL);

    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    if(vol>16)vol=16;
    compo_picturebox_set_pos(pic_value, (-128/2)+vol*(128/16), 4/2);
    compo_picturebox_set_visible(pic_value, ble_is_connect()||bt_a2dp_profile_completely_connected());

    if(!bt_a2dp_profile_completely_connected() && !ble_is_connect())
    {
        compo_textbox_set_visible(tilte_art_txt, false);

        compo_textbox_set_location(tilte_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(tilte_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_textbox_set(tilte_txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
        compo_setid(tilte_txt, COMPO_ID_TXT_MUSIC_LYRIC);
        compo_button_set_bgimg(btn_play, UI_BUF_I330001_MUSIC_PLAY02_BIN);
        compo_button_set_bgimg(btn_next, UI_BUF_I330001_MUSIC_NEXTSONG01_BIN);
        compo_button_set_bgimg(btn_prev, UI_BUF_I330001_MUSIC_PREVIOUSSONG01_BIN);
        compo_button_set_bgimg(btn_up,   UI_BUF_I330001_MUSIC_VOLUME03_BIN);
        compo_button_set_bgimg(btn_down, UI_BUF_I330001_MUSIC_VOLUME01_BIN);
        memset(f_bt->title_buf,1,sizeof(f_bt->title_buf_old));
        memset(f_bt->artist_buf,1,sizeof(f_bt->artist_buf_old));
        f_bt->refresh_data = true;
        return;
    }
    else
    {
        compo_textbox_set_visible(tilte_art_txt, true);
        compo_textbox_set_location(tilte_art_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_location(tilte_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/1.8, GUI_SCREEN_WIDTH, 50);
    }
    if(tilte_art_txt != NULL)
    {
        if(!bt_a2dp_profile_completely_connected() && ble_is_connect())
        {
            compo_textbox_set_visible(tilte_art_txt, false);
        }
        else
        {
            compo_textbox_set_visible(tilte_art_txt, true);
        }
    }
    uint16_t title_size_leng  = 0;
    uint16_t artist_size_leng = 0;
    memset(f_bt->title_buf,0,sizeof(f_bt->title_buf));
    memset(f_bt->artist_buf,0,sizeof(f_bt->artist_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)f_bt->title_buf,&title_size_leng);
    uteModuleMusicGetPlayerArtistSize((uint8_t *)f_bt->artist_buf,&artist_size_leng);

    if(strcmp(f_bt->title_buf, f_bt->title_buf_old)!=0 || title_size_leng == 0 || f_bt->refresh_data) //歌名刷新
    {
        f_bt->refresh_data = false;
//        printf("1111111111111111111111111111\n");
        memcpy(f_bt->title_buf_old, f_bt->title_buf, sizeof(f_bt->title_buf));
        compo_textbox_set(tilte_txt, f_bt->title_buf);
        if(title_size_leng == 0)
        {
            compo_textbox_set(tilte_txt, i18n[STR_UNKNOWN]);
        }
    }
    if(strcmp(f_bt->artist_buf, f_bt->artist_buf_old)!=0 || artist_size_leng==0 || f_bt->refresh_data) //歌手刷新
    {
        f_bt->refresh_data = false;
//        printf("2222222222222222222222222222\n");
        memcpy(f_bt->artist_buf_old, f_bt->artist_buf, sizeof(f_bt->artist_buf));
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
    if(bt_cb.music_playing)
    {
        compo_button_set_bgimg(btn_play, UI_BUF_I330001_MUSIC_PAUSED_BIN);
    }
    else
    {
        compo_button_set_bgimg(btn_play, UI_BUF_I330001_MUSIC_PLAY01_BIN);
    }
}

///按钮释放
static void func_bt_button_release_handle()
{
    if(!bt_a2dp_profile_completely_connected() && !ble_is_connect())return;

    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    s32 dx, dy;

    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_button_t *btn_up   = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
    compo_button_t *btn_down = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);

//    func_bt_music_refresh_disp();

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


#elif GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT
#define TITLE_BUF_LEN     UTE_MUSIC_TITLE_MAX_SIZE   //歌名buf长度
#define ARTIST_BUF_LEN    UTE_MUSIC_ARTLIST_MAX_SIZE     //歌手/歌手buf长度
#define PROGRESS_BAR_LENGTH 146
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
    bool refresh_data;
} f_bt_t;
/*****************************************************************************
 *          func_bt_music(UI)
*****************************************************************************/
//创建蓝牙音乐播放器窗体，创建窗体中不要使用功能结构体 func_cb.f_cb
compo_form_t *func_bt_form_create(void)
{
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    uint16_t title_size_leng  = 0;
    uint16_t artist_size_leng = 0;
    memset(title_buf,0,sizeof(title_buf));
    memset(artist_buf,0,sizeof(artist_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)title_buf,&title_size_leng);
    uteModuleMusicGetPlayerArtistSize((uint8_t *)artist_buf,&artist_size_leng);
    if(title_size_leng==0)
    {
        memset(title_buf,0,sizeof(title_buf));
        snprintf(title_buf,sizeof(title_buf),"%s",i18n[STR_UNKNOWN]);
    }
    if(artist_size_leng==0)
    {
        memset(artist_buf,0,sizeof(artist_buf));
        snprintf(artist_buf,sizeof(artist_buf),"%s",i18n[STR_UNKNOWN]);
    }

    //新建窗体
    compo_form_t *frm = compo_form_create(true);

    //设置标题栏
    compo_form_set_mode(frm, COMPO_FORM_MODE_SHOW_TITLE | COMPO_FORM_MODE_SHOW_TIME);
    compo_form_set_title(frm, i18n[STR_MUSIC]);
    compo_form_add_image(frm, UI_BUF_I332001_MUSIC_FRAME_2313_BIN, GUI_SCREEN_CENTER_X,GUI_SCREEN_CENTER_Y );

    if(bt_a2dp_profile_completely_connected() || ble_is_connect())
    {
        //歌词/歌手
        compo_textbox_t *name_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(name_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(name_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(name_txt, COMPO_ID_TXT_MUSIC_NAME);
        compo_textbox_set(name_txt,artist_buf);
        compo_textbox_set_forecolor(name_txt, COLOR_GRAY);
        if(!bt_a2dp_profile_completely_connected())
        {
            compo_textbox_set_visible(name_txt,false);
        }

        //歌名
        compo_textbox_t *lyric_txt = compo_textbox_create(frm, 50);
        compo_textbox_set_location(lyric_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/1.8, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(lyric_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_setid(lyric_txt, COMPO_ID_TXT_MUSIC_LYRIC);
        compo_textbox_set(lyric_txt,title_buf);

        compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_PREVIOUSSONG01_BIN);///上一曲
        compo_setid(btn, COMPO_ID_BTN_PREV);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_CENTER_Y);

        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_NEXTSONG01_BIN);///下一曲
        compo_setid(btn, COMPO_ID_BTN_NEXT);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_CENTER_Y);

        if(ble_is_connect())
        {
            bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
        }
        btn = compo_button_create_by_image(frm,bt_cb.music_playing ? UI_BUF_I332001_MUSIC_PAUSED_BIN:UI_BUF_I332001_MUSIC_PLAY01_BIN);///暂停 播放
        compo_setid(btn, COMPO_ID_BTN_PLAY);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_VOLUME01_BIN);///音量减
        compo_setid(btn, COMPO_ID_BTN_VOL_DOWN);
        compo_button_set_pos(btn, 90,302);

        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_VOLUME03_BIN);///音量加
        compo_setid(btn, COMPO_ID_BTN_VOL_UP);
        compo_button_set_pos(btn, 270, 302);

        compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///音量进度条背景
        compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,302, 128, 4 );
        compo_shape_set_radius(shape, 3);
        compo_shape_set_color(shape,make_color(0x33,0x33,0x33) );

        uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
        if(vol>16)vol=16;
        shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///音量进度条
        compo_setid(shape, COMPO_ID_SHAPE_MUSIC_VOL);
        compo_shape_set_location(shape,  (GUI_SCREEN_WIDTH-128)/2+vol*(128/16)/2,302,vol*(128/16), 4 );
        compo_shape_set_radius(shape, 1);
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

        compo_button_t *btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_PREVIOUSSONG02_BIN);///上一曲
        compo_setid(btn, COMPO_ID_BTN_PREV);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X-GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_CENTER_Y);

        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_NEXTSONG02_BIN);///下一曲
        compo_setid(btn, COMPO_ID_BTN_NEXT);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X+GUI_SCREEN_CENTER_X/1.5, GUI_SCREEN_CENTER_Y);

        if(ble_is_connect())
        {
            bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
        }
        btn = compo_button_create_by_image(frm,UI_BUF_I332001_MUSIC_PLAY02_BIN);///暂停 播放
        compo_setid(btn, COMPO_ID_BTN_PLAY);
        compo_button_set_pos(btn, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y);

        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_VOLUME02_BIN);///音量减
        compo_setid(btn, COMPO_ID_BTN_VOL_DOWN);
        compo_button_set_pos(btn, 90,302);

        btn = compo_button_create_by_image(frm, UI_BUF_I332001_MUSIC_VOLUME04_BIN);///音量加
        compo_setid(btn, COMPO_ID_BTN_VOL_UP);
        compo_button_set_pos(btn, 270, 302);

        compo_shape_t *shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///音量进度条背景
        compo_shape_set_location(shape, GUI_SCREEN_CENTER_X,302, 128, 4 );
        compo_shape_set_radius(shape, 3);
        compo_shape_set_color(shape,make_color(0x33,0x33,0x33) );

        uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
        if(vol>16)vol=16;
        shape = compo_shape_create(frm,COMPO_SHAPE_TYPE_RECTANGLE);///音量进度条
        compo_setid(shape, COMPO_ID_SHAPE_MUSIC_VOL);
        compo_shape_set_location(shape,  (GUI_SCREEN_WIDTH-128)/2+vol*(128/16)/2,302,vol*(128/16), 4 );
        compo_shape_set_radius(shape, 1);
        compo_shape_set_visible(shape,false);
    }


    return frm;
}

static void func_bt_music_refresh_disp(void)
{
    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_button_t *btn_up   = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
    compo_button_t *btn_down = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);
    compo_textbox_t *tilte_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_LYRIC);
    compo_textbox_t *tilte_art_txt = compo_getobj_byid(COMPO_ID_TXT_MUSIC_NAME);
    compo_shape_t *shape = compo_getobj_byid(COMPO_ID_SHAPE_MUSIC_VOL);

    if(!bt_a2dp_profile_completely_connected() && !ble_is_connect())
    {
        compo_textbox_set_visible(tilte_art_txt, false);

        compo_textbox_set_location(tilte_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_autoroll_mode(tilte_txt, TEXT_AUTOROLL_MODE_SROLL_CIRC);
        compo_textbox_set(tilte_txt, i18n[STR_VOICE_BT_NOT_CONNECT]);
        compo_setid(tilte_txt, COMPO_ID_TXT_MUSIC_LYRIC);
        compo_shape_set_visible(shape,false);
        compo_button_set_bgimg(btn_play, UI_BUF_I332001_MUSIC_PLAY02_BIN);
        compo_button_set_bgimg(btn_next, UI_BUF_I332001_MUSIC_NEXTSONG02_BIN);
        compo_button_set_bgimg(btn_prev, UI_BUF_I332001_MUSIC_PREVIOUSSONG02_BIN);
        compo_button_set_bgimg(btn_up,   UI_BUF_I332001_MUSIC_VOLUME04_BIN);
        compo_button_set_bgimg(btn_down, UI_BUF_I332001_MUSIC_VOLUME02_BIN);
        memset(f_bt->title_buf,1,sizeof(f_bt->title_buf_old));
        memset(f_bt->artist_buf,1,sizeof(f_bt->artist_buf_old));
        f_bt->refresh_data = true;
        return;
    }
    else
    {
        compo_shape_set_visible(shape,true);
        compo_textbox_set_visible(tilte_art_txt, true);
        compo_textbox_set_location(tilte_art_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/2.5, GUI_SCREEN_WIDTH, 50);
        compo_textbox_set_location(tilte_txt, GUI_SCREEN_CENTER_X, GUI_SCREEN_CENTER_Y-GUI_SCREEN_CENTER_Y/1.8, GUI_SCREEN_WIDTH, 50);
    }
    uint8_t vol = uteModuleMusicGetPlayerVolume() / 6;
    uint16_t title_size_leng  = 0;
    uint16_t artist_size_leng = 0;
    memset(f_bt->title_buf,0,sizeof(f_bt->title_buf));
    memset(f_bt->artist_buf,0,sizeof(f_bt->artist_buf));
    uteModuleMusicGetPlayerTitle((uint8_t *)f_bt->title_buf,&title_size_leng);
    uteModuleMusicGetPlayerArtistSize((uint8_t *)f_bt->artist_buf,&artist_size_leng);


    if(strcmp(f_bt->title_buf, f_bt->title_buf_old)!=0 || title_size_leng == 0 || f_bt->refresh_data) //歌名刷新
    {
        f_bt->refresh_data = false;
        memcpy(f_bt->title_buf_old, f_bt->title_buf, sizeof(f_bt->title_buf));
        compo_textbox_set(tilte_txt, f_bt->title_buf);
        if(title_size_leng == 0)
        {
            compo_textbox_set(tilte_txt, i18n[STR_UNKNOWN]);
        }
    }
    if(strcmp(f_bt->artist_buf, f_bt->artist_buf_old)!=0 || artist_size_leng==0 || f_bt->refresh_data) //歌手刷新
    {
        f_bt->refresh_data = false;
        memcpy(f_bt->artist_buf_old, f_bt->artist_buf, sizeof(f_bt->artist_buf));
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

    compo_button_set_bgimg(btn_play, bt_cb.music_playing ? UI_BUF_I332001_MUSIC_PAUSED_BIN:UI_BUF_I332001_MUSIC_PLAY01_BIN);

    if(vol>16)vol=16;
    compo_shape_set_location(shape, (GUI_SCREEN_WIDTH-128)/2+vol*(128/16)/2,302,vol*(128/16), 4 );
}

///按钮释放
static void func_bt_button_release_handle()
{
    if(!bt_a2dp_profile_completely_connected() && !ble_is_connect())return;

    f_bt_t *f_bt = (f_bt_t *)func_cb.f_cb;
    s32 dx, dy;

    compo_button_t *btn_prev = compo_getobj_byid(COMPO_ID_BTN_PREV);
    compo_button_t *btn_next = compo_getobj_byid(COMPO_ID_BTN_NEXT);
    compo_button_t *btn_play = compo_getobj_byid(COMPO_ID_BTN_PLAY);
    compo_button_t *btn_up   = compo_getobj_byid(COMPO_ID_BTN_VOL_UP);
    compo_button_t *btn_down = compo_getobj_byid(COMPO_ID_BTN_VOL_DOWN);

    if(f_bt->tick <= tick_get()-BTN_REST_DISP_TIME)
    {
        compo_button_set_bgimg(btn_prev,UI_BUF_I332001_MUSIC_PREVIOUSSONG01_BIN );
        compo_button_set_bgimg(btn_next,UI_BUF_I332001_MUSIC_NEXTSONG01_BIN );
        compo_button_set_bgimg(btn_up,UI_BUF_I332001_MUSIC_VOLUME03_BIN );
        compo_button_set_bgimg(btn_down,UI_BUF_I332001_MUSIC_VOLUME01_BIN );
    }

    int id = compo_get_button_id();
    if(id == 0 || !ctp_get_dxy(&dx, &dy)) return;

    f_bt->tick = tick_get();

    switch(id)
    {
        case COMPO_ID_BTN_PREV:
            compo_button_set_bgimg(btn_prev,UI_BUF_I332001_MUSIC_PREVIOUSSONG02_BIN );
            break;
        case COMPO_ID_BTN_NEXT:
            compo_button_set_bgimg(btn_next,UI_BUF_I332001_MUSIC_NEXTSONG02_BIN );
            break;
        case COMPO_ID_BTN_PLAY:
            break;
        case COMPO_ID_BTN_VOL_UP:
            compo_button_set_bgimg(btn_up,UI_BUF_I332001_MUSIC_VOLUME04_BIN );
            break;
        case COMPO_ID_BTN_VOL_DOWN:
            compo_button_set_bgimg(btn_down,UI_BUF_I332001_MUSIC_VOLUME02_BIN );
            break;
    }
}

#else
typedef struct f_bt_t_
{
    uint32_t tick;
    char title_buf[TITLE_BUF_LEN];
    char artist_buf[ARTIST_BUF_LEN];
    char title_buf_old[TITLE_BUF_LEN];
    char artist_buf_old[ARTIST_BUF_LEN];
    bool refresh_data;
} f_bt_t;
compo_form_t *func_bt_form_create(void)
{
}

static void func_bt_button_release_handle()
{
}
#endif UI控制宏
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
            uteModuleMusicCtrlPaused(false);
            if(ble_is_connect())
            {
                bt_cb.music_playing = !uteModuleMusicGetPlayerPaused();
            }
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
#if (GUI_SCREEN_SIZE_240X284RGB_I330001_SUPPORT || GUI_SCREEN_SIZE_360X360RGB_I332001_SUPPORT || GUI_SCREEN_SIZE_240X284RGB_I335001_SUPPORT)
    switch (msg)
    {
        case MSG_CTP_CLICK:
            if(!uteModuleCallBtIsConnected() && !ble_is_connect()) return;
            func_bt_button_click();                         //单击按钮
            break;

        case MSG_SYS_500MS:
            func_bt_music_refresh_disp();
#if BT_ID3_TAG_EN
            if (bt_a2dp_profile_completely_connected())
            {
                bt_music_paly_status_info();
            }
#endif
            break;

        default:
            func_message(msg);
            break;
    }
#endif
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
    if(bt_a2dp_profile_completely_connected())
    {
        bt_music_get_id3_tag();
    }
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

#endif
