#include "include.h"
#include "api.h"
#include "func.h"

bsp_bt_t bt_cb;

void dev_vol_set_cb(uint8_t dev_vol, uint8_t media_index, uint8_t setting_type);

void bsp_bt_init(void)
{
    //更新配置工具的设置
    cfg_bt_rf_def_txpwr = 0;  //xcfg_cb.bt_txpwr;
    cfg_bt_page_txpwr = xcfg_cb.bt_rf_txpwr_recon;

    cfg_bt_support_profile = (PROF_HFP*BT_HFP_EN*xcfg_cb.bt_sco_en)\
                             | (PROF_SPP*BT_SPP_EN*(xcfg_cb.bt_spp_en||xcfg_cb.eq_dgb_spp_en)) | (PROF_HID*BT_HID_EN) | (PROF_HSP*BT_HSP_EN*xcfg_cb.bt_sco_en)\
                             | (PROF_PBAP*BT_PBAP_EN) | (PROF_MAP*BT_MAP_EN);

#if BT_A2DP_PROFILE_DEFAULT_EN
    cfg_bt_support_profile |= (PROF_A2DP*BT_A2DP_EN*xcfg_cb.bt_a2dp_en);
#endif

#if BT_A2DP_VOL_CTRL_EN
    if(!xcfg_cb.bt_a2dp_vol_ctrl_en)
    {
        cfg_bt_a2dp_feature &= ~A2DP_AVRCP_VOL_CTRL;
        cfg_bt_a2dp_feature1 &= ~A2DP_AVRCP_RECORD_DEVICE_VOL;
    }
    else
    {
        cfg_bt_a2dp_feature |= A2DP_AVRCP_VOL_CTRL;
    }
#endif

#if BT_A2DP_AVRCP_PLAY_STATUS_EN
    cfg_bt_a2dp_feature |= A2DP_AVRCP_PLAY_STATUS;
#endif
    cfg_bt_dual_mode = BT_DUAL_MODE_EN * xcfg_cb.ble_en;
    cfg_bt_max_acl_link = BT_2ACL_EN + 1;
#if BT_TWS_EN
    cfg_bt_tws_mode = BT_TWS_EN * xcfg_cb.bt_tws_en;
    if(xcfg_cb.bt_tws_en == 0)
    {
        cfg_bt_tws_feat      = 0;
        cfg_bt_tws_pair_mode = 0;
    }
    else
    {
        cfg_bt_tws_pair_mode &= ~TWS_PAIR_OP_MASK;
        cfg_bt_tws_pair_mode |= xcfg_cb.bt_tws_pair_mode & TWS_PAIR_OP_MASK;
#if BT_TWS_PAIR_BONDING_EN
        if(xcfg_cb.bt_tws_pair_bonding_en)
        {
            cfg_bt_tws_pair_mode |= TWS_PAIR_MS_BONDING;
            cfg_bt_tws_feat |= TWS_FEAT_MS_BONDING;
        }
        else
        {
            cfg_bt_tws_pair_mode &= ~TWS_PAIR_MS_BONDING;
        }
#endif
#if BT_TWS_MS_SWITCH_EN
        if(xcfg_cb.bt_tswi_en)
        {
            cfg_bt_tws_feat |= TWS_FEAT_MS_SWITCH;
        }
        else
        {
            cfg_bt_tws_feat &= ~TWS_FEAT_MS_SWITCH;
        }
#endif
        if(xcfg_cb.bt_tws_lr_mode > 8)  //开机时PWRKEY可能按住，先不检测
        {
            tws_lr_xcfg_sel();
        }
    }
#endif // BT_TWS_EN

#if BT_HFP_EN
    if(!xcfg_cb.bt_hfp_ring_number_en)
    {
        cfg_bt_hfp_feature &= ~HFP_RING_NUMBER_EN;
    }
#endif

#if BT_FCC_TEST_EN
    bt_fcc_init();
#endif

#if BT_PBAP_EN
    bt_pbap_param_reset();
#endif

    memset(&bt_cb, 0, sizeof(bsp_bt_t));
    bt_cb.disp_status = 0xfe;
    bt_cb.need_pairing = 1;          //开机若回连不成功，需要播报pairing
    bt_cb.hid_menu_flag = 1;

    bt_setup();
}

void bsp_bt_close(void)
{
}

void bsp_bt_vol_change(void)
{
#if BT_A2DP_VOL_CTRL_EN
    if((xcfg_cb.bt_a2dp_vol_ctrl_en && (bt_get_status() >= BT_STA_CONNECTED))|| bt_tws_is_connected())
    {
        bt_music_vol_change();     //通知手机音量已调整
    }
#endif
}

uint bsp_bt_get_hfp_vol(uint hfp_vol)
{
    uint vol;
    vol = (hfp_vol + 1) * sys_cb.hfp2sys_mul;
    if (vol > VOL_MAX)
    {
        vol = VOL_MAX;
    }
    return vol;
}

bool phone_conecting_flag = false;
void bt_emit_notice(uint evt, void *params)
{
    u8 *packet = params;
    u8 opcode = 0;
    u8 scan_status = 0x03;

    printf("bt notice:0x%x\n", evt);

    switch(evt)
    {
        case BT_NOTICE_INIT_FINISH:
            printf("BT_NOTICE_INIT_FINISH >>>>>>>>>>>> %d \n\n",xcfg_cb.bt_tws_pair_mode);
#if BT_TWS_EN
            if(xcfg_cb.bt_tws_pair_mode > 1)
            {
                bt_tws_set_scan(0x03);
            }
#endif
            if(cfg_bt_work_mode == MODE_BQB_RF_BREDR)
            {
                opcode = 1;                     //测试模式，不回连，打开可被发现可被连接
            }
            else if(bt_nor_get_link_info(NULL))
            {
                scan_status = 0x02;         //有回连信息，不开可被发现
            }
            bt_start_work(opcode, scan_status);
            break;

        case BT_NOTICE_DISCONNECT:
            bt_cb.warning_status |= BT_WARN_DISCON;
            bt_redial_reset(((u8 *)params)[0] & 0x01);
#if BT_HID_ONLY_FOR_IOS_EN
            bt_init_lib_hid();
#endif
            break;

        case BT_NOTICE_CONNECTED:
            bt_cb.warning_status |= BT_WARN_CON;
            bt_redial_reset(((u8 *)params)[0] & 0x01);
#if BT_PANU_EN
            bt_panu_network_connect();
#endif
            break;

//    case BT_NOTICE_LOSTCONNECT:
//        break;
//    case BT_NOTICE_INCOMING:
//    case BT_NOTICE_RING:
//    case BT_NOTICE_OUTGOING:
//    case BT_NOTICE_CALL:
//        break;

        case BT_NOTICE_SET_SPK_GAIN:
            dev_vol_set_cb(packet[0], packet[1], 1 | BIT(3));
            break;

        case BT_NOTICE_MUSIC_PLAY:
            msg_enqueue(EVT_A2DP_MUSIC_PLAY);
            break;

        case BT_NOTICE_MUSIC_STOP:
            if (bt_get_disp_status() > BT_STA_PLAYING)
            {
                break;
            }
            msg_enqueue(EVT_A2DP_MUSIC_STOP);
            break;

        case BT_NOTICE_MUSIC_CHANGE_VOL:
            if(params == 0)
            {
                msg_enqueue(KU_VOL_DOWN);
            }
            else
            {
                msg_enqueue(KU_VOL_UP);
            }
            break;

        case BT_NOTICE_MUSIC_SET_VOL:
            if((sys_cb.incall_flag & INCALL_FLAG_SCO) == 0)
            {
                dev_vol_set_cb(packet[0], packet[1], 1);
            }
            break;

        case BT_NOTICE_MUSIC_CHANGE_DEV:
            dev_vol_set_cb(packet[0], packet[1], 0);
            break;

        case BT_NOTICE_CALL_CHANGE_DEV:
            dev_vol_set_cb(packet[0], packet[1], 0 | BIT(3));
            break;

        case BT_NOTICE_HID_CONN_EVT:
#if BT_HID_MENU_EN
            if (xcfg_cb.bt_hid_menu_en)
            {
                if (bt_cb.hid_menu_flag == 2)
                {
                    //按键连接/断开HID Profile完成
                    if (param)
                    {
                        bt_cb.warning_status |= BT_WARN_HID_CON;
                    }
                    else
                    {
                        bt_cb.warning_status |= BT_WARN_HID_DISCON;
                    }
                    bt_cb.hid_menu_flag = 1;
                }
#if BT_HID_DISCON_DEFAULT_EN
                else if (bt_cb.hid_menu_flag == 1)
                {
                    if ((param) & (xcfg_cb.bt_hid_discon_default_en))
                    {
                        bt_cb.hid_discon_flag = 1;
                    }
                }
#endif // BT_HID_DISCON_DEFAULT_EN
            }
#endif // BT_HID_MENU_EN
            break;

        case BT_NOTICE_CONNECT_FAIL:
        {
#if LE_SM_SC_EN
            if (packet[0] != 0x0B)     //BB_ERROR_ACL_CON_EXISTS
            {
                if (sys_cb.bt_reconn_flag)
                {
                    sys_cb.bt_reconn_flag = false;
                    ble_bt_connect();       //双连接
                }
            }
#endif // LE_SM_SC_EN
            break;
        }
        case BT_NOTICE_SCO_SETUP:
            printf("BT_NOTICE_SCO_SETUP\n");
            sys_cb.sco_state = true;
            break;

        case BT_NOTICE_SCO_KILL:
            printf("BT_NOTICE_SCO_KILL\n");
            sys_cb.sco_state = false;
            bt_cb.call_type = CALL_TYPE_NONE;
            break;

#if BT_PBAP_EN
        case BT_NOTICE_PBAP_CONNECTED:
        case BT_NOTICE_PBAP_GET_PHONEBOOK_SIZE_COMPLETE:
        case BT_NOTICE_PBAP_PULL_PHONEBOOK_COMPLETE:
        case BT_NOTICE_PBAP_DISCONNECT:
            bt_pbap_event_handle(evt, params);
            break;
#endif
#if BT_TWS_EN
        case BT_NOTICE_TWS_SEARCH_FAIL:
            printf("BT_NOTICE_TWS_SEARCH_FAIL %x \n",((u8 *)params)[0]);
            break;
//    case BT_NOTICE_TWS_CONNECT_START:
//        break;
        case BT_NOTICE_TWS_DISCONNECT:
            printf("BT_NOTICE_TWS_DISCONNECT >>> \n");
            bsp_res_set_break(false);
            bsp_sys_unmute();
            bt_cb.tws_status = 0;
            bt_cb.warning_status |= BT_WARN_TWS_DISCON;      //TWS断线不播报提示音，仅更改声道配置
//        msg_enqueue(EVT_BLE_ADV0_BAT);
//        app_tws_disconnect_callback();
//        ring_tws_disconnect_cb();
//        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示
//      if(!bt_is_connected()){
//          msg_enqueue(EVT_AUTO_PWFOFF_EN);
//      }
            break;
        case BT_NOTICE_TWS_CONNECTED:
            printf("BT_NOTICE_TWS_CONNECTED >>> \n");
            printf(mute_str, 02);
            bsp_sys_mute();
            if(bt_tws_is_slave())
            {
                ble_adv_dis();          //副机关闭BLE广播
                tws_res_cleanup();      //播tws提示音前先清一下单耳的提示音。不清会导致rpos慢主耳一步，导致一直w4
                tws_res_reset_lable();  //避免lable不对导致副耳丢失提示音
            }
            bt_cb.tws_status = packet[0];
            if(bt_cb.tws_status & FEAT_TWS_MUTE_FLAG)
            {
                bt_cb.warning_status |= BT_WARN_TWS_CON;     //无连接提示音，仅更改声道配置
            }
            else if(bt_cb.tws_status & FEAT_TWS_ROLE)
            {
                bt_cb.warning_status |= BT_WARN_TWS_SCON;    //TWS连接提示音
            }
            else
            {
                bt_cb.warning_status |= BT_WARN_TWS_MCON;    //TWS连接提示音
            }
//        app_tws_connect_callback();
//        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示

            bsp_res_set_break(true);                        //打断开机提示音

//      if(bt_tws_is_slave()){
//            msg_enqueue(EVT_AUTO_PWFOFF_DIS);
//        }else{
//            if(!bt_is_connected()){
//                msg_enqueue(EVT_AUTO_PWFOFF_EN);
//            }
//        }
            break;
//    case BT_NOTICE_TWS_CONNECT_FAIL:
//        break;
        case BT_NOTICE_TWS_LOSTCONNECT:
            break;

        case BT_NOTICE_TWS_INIT_VOL:
            sys_cb.vol = a2dp_vol_conver(packet[0]);
            msg_enqueue(EVT_TWS_INIT_VOL);
            break;

        case BT_NOTICE_TWS_USER_KEY:
            tws_user_key_process(params);
            break;

        case BT_NOTICE_TWS_STATUS_CHANGE:
//        msg_enqueue(EVT_BT_UPDATE_STA);                 //刷新显示
            break;

        case BT_NOTICE_TWS_ROLE_CHANGE:
//        if(packet[0] == 0) {
//            bsp_res_set_break(false);
//        }
            break;

#endif

        default:
            break;
    }
}

ALIGNED(64)
u16 bsp_bt_chkclr_warning(u16 bits)
{
    u16 value;
    GLOBAL_INT_DISABLE();
    value = bt_cb.warning_status & bits;
    if(value != 0)
    {
        bt_cb.warning_status &= ~value;
        GLOBAL_INT_RESTORE();
        return value;
    }
    GLOBAL_INT_RESTORE();
    return value;
}

void bsp_bt_warning_do(void)
{
    if(bsp_bt_chkclr_warning(BT_WARN_TWS_DISCON | BT_WARN_TWS_CON))
    {
#if BT_TWS_EN
        if(xcfg_cb.bt_tws_en)
        {
            if(xcfg_cb.bt_tws_lr_mode != 0)
            {
                bt_tws_set_channel();
            }
        }
#endif
    }

    if(bsp_bt_chkclr_warning(BT_WARN_DISCON))
    {
#if WARNING_BT_DISCONNECT
        if(!bt_tws_is_slave())
        {
            bsp_res_play(TWS_RES_DISCONNECT);
            return;
        }
#endif // WARNING_BT_DISCONNECT
    }

    if(bsp_bt_chkclr_warning(BT_WARN_PAIRING))
    {
        if(!bt_tws_is_slave())
        {
            bsp_res_play(TWS_RES_PAIRING);
            return;
        }
    }

#if BT_TWS_EN
    if(xcfg_cb.bt_tws_en)
    {
        u16 tws_warning = bsp_bt_chkclr_warning(BT_WARN_TWS_SCON | BT_WARN_TWS_MCON );
        if(tws_warning != 0)
        {
            if (xcfg_cb.bt_tws_lr_mode != 0)
            {
                bt_tws_set_channel();
            }
            ///固定声道方案，TWS连接后异步播放声音提示音。否则同步播放连接提示音
            if (xcfg_cb.bt_tws_lr_mode >= 8)
            {
                bt_tws_set_channel();
                tws_get_lr_channel();

                if (!bsp_res_is_playing())
                {
                    if(sys_cb.tws_left_channel)
                    {
                        func_cb.mp3_res_play(RES_BUF_LEFT_CH, RES_LEN_LEFT_CH);
                    }
                    else
                    {
                        bt_audio_bypass();
                        u8 timer_cnt = 100;
                        while (timer_cnt--)
                        {
                            bt_thread_check_trigger();
                            bsp_res_process();
                            delay_5ms(2);
                            WDT_CLR();
                        }
                        func_cb.mp3_res_play(RES_BUF_RIGHT_CH, RES_LEN_RIGHT_CH);
                        bt_audio_enable();
                    }
                }
            }
            else
            {
                if (tws_warning & BT_WARN_TWS_MCON)
                {
                    bsp_res_play(TWS_RES_CONNECTED);
                    return;
                }
            }
        }
    }
#endif

    if(bsp_bt_chkclr_warning(BT_WARN_CON))
    {
#if WARNING_BT_CONNECT
        if(!bt_tws_is_slave())
        {
            bsp_res_play(TWS_RES_CONNECTED);
            return;
        }
#endif
    }
#if BT_HID_MENU_EN
    //按键手动断开HID Profile的提示音
    if (xcfg_cb.bt_hid_menu_en)
    {

#if BT_HID_DISCON_DEFAULT_EN
        if (bt_cb.hid_discon_flag)
        {
            if (bt_hid_is_ready_to_discon())
            {
                bt_cb.hid_discon_flag = 0;
                bt_hid_disconnect();
            }
        }
#endif // BT_HID_DISCON_DEFAULT_EN
    }
#endif // BT_HID_MENU_EN
}

AT(.text.func.disp_status)
uint bsp_bt_disp_status(void)
{
    uint status = bt_get_disp_status();

    if(bt_cb.disp_status != status)
    {
        bt_cb.disp_status = status;
        //if(!bt_is_connected()) {
        if (0)
        {
            en_auto_pwroff();
            sys_cb.sleep_en = BT_PAIR_SLEEP_EN;
        }
        else
        {
            dis_auto_pwroff();
            sys_cb.sleep_en = 1;
        }

        if(bt_cb.disp_status >= BT_STA_CONNECTED)
        {
            bt_cb.need_pairing = 1;
        }
    }
    return bt_cb.disp_status;
}

AT(.text.func.bt)
void bsp_bt_warning(void)
{
    if(bt_cb.warning_status != 0 && !bsp_res_is_full())
    {
        bsp_bt_warning_do();
    }
}

AT(.text.func.bt)
void bsp_bt_status(void)
{
    bsp_bt_disp_status();
    bsp_bt_warning();
}



