#include "include.h"

bool mp3_res_play_kick(u32 addr, u32 len, bool kick);
void wav_res_play_kick(u32 addr, u32 len);
void wav_res_dec_process(void);
bool wav_res_is_play(void);
bool wav_res_stop(void);
void mp3_res_play_exit(void);

#if DAC_DNR_EN
static u8 msc_dnr_sta;
#endif

#if WARNING_MP3_EN
void mp3_res_play(u32 addr, u32 len)
{
#if QTEST_EN
    if(qtest_get_mode())
    {
        return;
    }
#endif

    u16 msg;
    u8 mute_bak;
//    printf("%s: addr: %x, len: %x\n", __func__, addr, len);
    if (len == 0)
    {
        return;
    }

#if BT_BACKSTAGE_EN
//    if (func_cb.sta_break != FUNC_NULL) {
//        for (u32 i = 0; i < sizeof(res_addr) / sizeof(u32); i++) {
//            if (*res_addr[i] == addr) {
//                if (func_cb.sta != FUNC_BT) {
//                    func_cb.sta_break = FUNC_NULL;
//                }
//                return;
//            }
//        }
//    }
#endif

#if SYS_KARAOK_EN
    u8 voice_bak = 0, func_sta = func_cb.sta;
    if (karaok_get_voice_rm_sta())
    {
        voice_bak = 1;
        karaok_voice_rm_disable();
    }
    bsp_karaok_exit(AUDIO_PATH_KARAOK);
#endif

    mute_bak = sys_cb.mute;
    if (sys_cb.mute)
    {
        bsp_loudspeaker_unmute();
    }
    if(get_music_dec_sta() != MUSIC_STOP)   //避免来电响铃/报号未完成，影响get_music_dec_sta()状态
    {
        music_control(MUSIC_MSG_STOP);
    }
    bsp_change_volume(WARNING_VOLUME);

    mp3_res_play_kick(addr, len, true);

    while (get_music_dec_sta() != MUSIC_STOP)
    {
        bt_thread_check_trigger();
        WDT_CLR();
        msg = msg_dequeue();
        if (sys_cb.voice_evt_brk_en)
        {
//            if (((msg == EVT_SD_INSERT) || (msg == EVT_UDISK_INSERT)) && (func_cb.sta != FUNC_MUSIC)) {
//                func_message(msg);
//                break;
//            }
        }
#if LINEIN_DETECT_EN
        if ((msg == EVT_LINEIN_INSERT) && ((sys_cb.voice_evt_brk_en) || (LINEIN_2_PWRDOWN_EN)))
        {
            func_message(msg);
            break;
        }
#endif // LINEIN_DETECT_EN
        if (msg != NO_MSG)
        {
            msg_enqueue(msg);       //还原未处理的消息
        }
#if BT_TWS_EN && WARNING_BREAK_EN
        if(sys_cb.tws_res_brk)
        {
            sys_cb.tws_res_brk = 0;
            break;
        }
#endif
    }
    music_control(MUSIC_MSG_STOP);
    bsp_change_volume(sys_cb.vol);
    mp3_res_play_exit();
    sys_cb.mute = mute_bak;
    if (sys_cb.mute)
    {
        printf(mute_str, 03);
        bsp_loudspeaker_mute();
    }

#if SYS_KARAOK_EN
    if (voice_bak)
    {
        karaok_voice_rm_enable();
    }
    bsp_karaok_init(AUDIO_PATH_KARAOK, func_sta);
#endif
}
#else
void mp3_res_play(u32 addr, u32 len) {}
#endif

#if WARNING_WAV_EN
void wav_res_play(u32 addr, u32 len)
{
    if (len == 0)
    {
        return;
    }
    sys_clk_req(INDEX_RES_PLAY, SYS_120M);

    wav_res_play_kick(addr, len, 1);
    while (wav_res_is_play())
    {
        bt_thread_check_trigger();
        wav_res_dec_process();
        WDT_CLR();
#if BT_TWS_EN && WARNING_BREAK_EN
        if(sys_cb.tws_res_brk)
        {
            sys_cb.tws_res_brk = 0;
            break;
        }
#endif
    }
    wav_res_stop();
    sys_clk_free(INDEX_RES_PLAY);
}
#endif

//提示音播报完毕，设置状态
void bsp_res_play_exit_cb(uint8_t res_idx)
{
#if BT_LOW_LATENCY_EN
    if (TWS_RES_MUSIC_MODE == res_idx)
    {
        printf("music mode\n");
        bt_low_latency_disable();
    }
    else if (TWS_RES_GAME_MODE == res_idx)
    {
        printf("game mode\n");
        bt_low_latency_enable();
    }
#endif
}

#if !BT_TWS_EN
uint8_t bsp_res_play(uint8_t res_idx)
{
    uint8_t res_type = tws_res_get_type(res_idx);

    if (res_type != RES_TYPE_INVALID)
    {
        u32 addr, len;

        tws_res_get_addr(res_idx, &addr, &len);
        if (len != 0)
        {
            if(res_type == RES_TYPE_MP3)
            {
#if WARNING_MP3_EN
                bt_audio_bypass();
                mp3_res_play(addr, len);
                bsp_res_play_exit_cb(res_idx);
                bt_audio_enable();
#endif
#if WARNING_WSBC_EN
            }
            else if(res_type == RES_TYPE_WSBC)
            {
                wsbc_res_play(addr, len);
#endif
#if WARNING_WAV_EN
            }
            else if(res_type == RES_TYPE_WAV)
            {
                wav_res_play(addr, len);
#endif
#if WARNING_PIANO_EN
            }
            else if(res_type == RES_TYPE_PIANO || res_type == RES_TYPE_TONE)
            {
                int type = (res_type == RES_TYPE_TONE)? WARNING_TONE : WARNING_PIANO;
                piano_res_play(type, (void *)addr);
#endif
            }
        }
    }

    return RES_ERR_INVALID;
}
#endif
