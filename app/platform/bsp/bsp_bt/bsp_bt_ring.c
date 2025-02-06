#include "include.h"
#include "func.h"
#include "func_bt.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif // TRACE_EN

#if FUNC_BT_EN

enum
{
    RING_FLAG_RES       = BIT(0),       //本地铃声标志
    RING_FLAG_SCO       = BIT(1),       //SCO铃声标志
    RING_FLAG_NUM       = BIT(2),       //报号标志
    RING_FLAG_SYNC      = BIT(3),       //是否同步状态标志
};

enum
{
    BT_RING_STA_IDLE    = 0,
    BT_RING_STA_NEGO,                   //协商状态
    BT_RING_STA_SCO,                    //播放SCO铃声同步状态(不支持手机或选择不同步，播放本地ring)
    BT_RING_STA_RES,                    //播放ring res状态
    BT_RING_STA_NUM,                    //播放来电报号
};

struct bt_ring_st
{
    u8 rem_sta;
    u8 rem_flag;
    u8 num_index;
    u8 num_len;
    u8 num_buf[8];

    u32 tickn;
    u32 nego_tickn;
    u8 loc_sta;
    u8 loc_flag;
};


//------------------------------------------------------------------------
static struct bt_ring_st bt_ring;
//------------------------------------------------------------------------
u8 hfp_notice_ring_number(uint8_t index, char *buf, u32 len)
{
    bt_update_redial_number(index, (char *)buf, len);

    if(bt_ring.num_len == 0)
    {
        if(len > 16)
        {
            len = 16;
        }

        memset(bt_ring.num_buf, 0x00, sizeof(bt_ring.num_buf));
        for(u8 i = 0; i < len; i++)
        {
            u8 offset = (bool)(i & 0x1)? 4 : 0;
            if (buf[i] >= '0' && buf[i] <= '9')
            {
                bt_ring.num_buf[i/2] |= (buf[i] - 0x30) << offset;
            }
        }

        bt_ring.num_len = len;
        TRACE("ring_num: %s\n", buf);
//        TRACE_R(bt_ring.num_buf, (len+1)/2);

        return 1;
    }
    return 0;
}

//------------------------------------------------------------------------
//
#if BT_TWS_EN
ALIGNED(128)
void tws_local_get_ring_sta_cb(u8 *buf)
{
    struct bt_ring_st *p = (void *)buf;

    GLOBAL_INT_DISABLE();
    memcpy(buf, &bt_ring, offsetof(struct bt_ring_st, tickn));
    p->rem_sta  = bt_ring.loc_sta;
    p->rem_flag = bt_ring.loc_flag;
    GLOBAL_INT_RESTORE();
}

ALIGNED(128)
void tws_remote_set_ring_sta_cb(u8 *buf)
{
    struct bt_ring_st *p = (void *)buf;

    if(bt_tws_is_slave())
    {
        GLOBAL_INT_DISABLE();
        memcpy(&bt_ring, p, offsetof(struct bt_ring_st, tickn));
        GLOBAL_INT_RESTORE();
    }
    else
    {
        bt_ring.rem_sta  = p->rem_sta;
        bt_ring.rem_flag |= p->rem_flag & (~RING_FLAG_SYNC);
    }
}

void ring_tws_disconnect_cb(void)
{
    bt_ring.rem_sta  = BT_RING_STA_IDLE;
    bt_ring.rem_flag = 0;
}

static void ring_res_play(uint8_t res_idx)
{
    uint8_t flag = 0;
    if(bt_tws_is_connected() && (bt_ring.rem_sta != BT_RING_STA_IDLE))
    {
        flag = RES_FLAG_TWS;
    }

    TRACE("==>ring_flag: %x, rem_sta=%x\n", flag, bt_ring.rem_sta);
    tws_res_add(res_idx, flag);
}
#else
#define ring_res_play(res_idx)      bsp_res_play(res_idx)
#endif

#if BT_HFP_INBAND_RING_EN
bool ring_mp3_msg_work(void)
{
    bool ret = false;
    if (bt_ring.loc_sta == BT_RING_STA_RES)      //判断是否播放本地铃声
    {
        bt_ring.loc_sta = BT_RING_STA_IDLE;      //暂停本地铃声播放
        ret = true;
    }
    return ret;
}
#endif

//------------------------------------------------------------------------
#if BT_HFP_RING_NUMBER_EN
AT(.text.func.btring) ALIGNED(128) NO_INLINE
static void sfunc_bt_ring_num_init(void)
{
#if BT_HFP_INBAND_RING_EN
    bool sco_flag = (bool)(bt_ring.loc_flag & RING_FLAG_SCO);
#endif
    GLOBAL_INT_DISABLE();
    bt_ring.loc_flag &= ~RING_FLAG_SCO;
    bt_ring.loc_flag |= RING_FLAG_NUM;
    bt_ring.loc_sta = BT_RING_STA_NUM;
    GLOBAL_INT_RESTORE();

    bt_tws_sync_ring_sta();

#if BT_HFP_INBAND_RING_EN
    if(sco_flag)
    {
        bt_audio_bypass();
    }
#endif
}

AT(.text.func.btring) ALIGNED(128) NO_INLINE
static void sfunc_bt_ring_num_exit(void)
{
#if BT_HFP_INBAND_RING_EN
    if((bt_ring.loc_flag & RING_FLAG_RES) == 0 && sco_is_connected())
    {
        GLOBAL_INT_DISABLE();
        bt_ring.loc_flag |= RING_FLAG_SCO;
        bt_ring.loc_sta = BT_RING_STA_SCO;
        GLOBAL_INT_RESTORE();
    }
    else
#endif
    {
        GLOBAL_INT_DISABLE();
        bt_ring.loc_flag |= RING_FLAG_RES;
        bt_ring.loc_sta = BT_RING_STA_RES;
        GLOBAL_INT_RESTORE();
    }

    bt_tws_sync_ring_sta();

#if BT_HFP_INBAND_RING_EN
    bool sco_flag = (bool)(bt_ring.loc_flag & RING_FLAG_SCO);
    if(sco_flag)
    {
        bt_audio_enable();
    }
#endif
}
#endif

AT(.text.func.btring) ALIGNED(128) NO_INLINE
static void sfunc_bt_ring_res_init(void)
{
    GLOBAL_INT_DISABLE();
    bt_ring.loc_flag &= ~RING_FLAG_SCO;
    bt_ring.loc_flag |= RING_FLAG_RES;
    bt_ring.loc_sta = BT_RING_STA_RES;
    GLOBAL_INT_RESTORE();

    bt_tws_sync_ring_sta();
}

//协商状态
AT(.text.func.btring)
static void sfunc_bt_ring_nego(void)
{
#if BT_TWS_EN
    //前1s等待对方进入ring状态
    if(bt_tws_is_connected()
       && bt_ring.rem_sta == BT_RING_STA_IDLE
       && !TICKN_IS_EXPIRE(bt_ring.nego_tickn))
    {

        printf("w ");
        return;
    }
#endif

#if BT_HFP_RING_NUMBER_EN
    if(bt_ring.num_len != 0)
    {
        bt_ring.tickn = TICKN_GET();
        bt_ring.loc_flag |= RING_FLAG_NUM;
        bt_ring.loc_sta = BT_RING_STA_NUM;
        bt_tws_sync_ring_sta();
        return;
    }
#endif

#if BT_HFP_INBAND_RING_EN
    if(sco_is_connected())
    {
        bt_ring.tickn = TICKN_GET();
        bt_ring.loc_flag |= RING_FLAG_SCO;
        bt_ring.loc_sta = BT_RING_STA_SCO;
        bt_tws_sync_ring_sta();
        bt_audio_enable();
        return;
    }
#endif

    if(TICKN_IS_EXPIRE(bt_ring.tickn))
    {
        bt_ring.loc_flag |= RING_FLAG_RES;
        bt_ring.loc_sta = BT_RING_STA_RES;
        bt_tws_sync_ring_sta();
    }
}

//播放本地铃声提示音
AT(.text.func.btring)
static void sfunc_bt_ring_res(void)
{
#if BT_HFP_RING_NUMBER_EN
    if((bt_ring.loc_flag & RING_FLAG_NUM) == 0)         //未报过号码
    {
        if(bt_ring.num_len != 0)
        {
            sfunc_bt_ring_num_init();
        }
    }
#endif

    if(TICKN_IS_EXPIRE(bt_ring.tickn) && bsp_res_is_empty())
    {
        bt_ring.tickn = TICK_ADD(TICKN_GET(), 1500);
        ring_res_play(TWS_RES_RING);
    }
}

#if BT_HFP_INBAND_RING_EN
//播放来电同步提示音, 没有同步铃声手机则播放本地的
AT(.text.func.btring) ALIGNED(256)
static void sfunc_bt_ring_sco(void)
{
#if BT_HFP_RING_NUMBER_EN
    if((bt_ring.loc_flag & RING_FLAG_NUM) == 0)         //未报过号码
    {
        if(bt_ring.num_len != 0)
        {
            sfunc_bt_ring_num_init();
        }
    }
    else
#endif
        if((bt_ring.loc_flag & RING_FLAG_SCO) && !sco_is_connected())
        {
            bt_audio_bypass();
            sfunc_bt_ring_res_init();
        }
}
#endif

#if BT_HFP_RING_NUMBER_EN
//播放来电报号
AT(.text.func.btring)
static void sfunc_bt_ring_num(void)
{
    if(bt_ring.num_index < bt_ring.num_len)
    {
        if(TICKN_IS_EXPIRE(bt_ring.tickn) && bsp_res_is_empty())
        {
            u8 res_num = bt_ring.num_buf[bt_ring.num_index/2];
            res_num >>= (4 * (bt_ring.num_index&0x1));
            res_num &= 0xF;

            TRACE("ring_num%d: %d\n", bt_ring.num_index, res_num);
            if (res_num != 0xf)
            {
                res_num = TWS_RES_NUM_0 + res_num;
                bt_ring.tickn = TICK_ADD(TICKN_GET(), 1500);
                ring_res_play(res_num);         //同步报号
            }

            bt_ring.num_index++;
            bt_tws_sync_ring_sta();
        }
    }
    else
    {
        sfunc_bt_ring_num_exit();
    }
}
#endif

static void sfunc_bt_ring_master(void)
{
    if(bt_ring.rem_sta == BT_RING_STA_NEGO
       && (bt_ring.rem_flag & RING_FLAG_SYNC) == 0
       && bt_tws_is_connected())
    {

        bt_ring.rem_flag |= RING_FLAG_SYNC;
        bt_tws_sync_ring_sta();
    }

    switch(bt_ring.loc_sta)
    {
        case BT_RING_STA_NEGO:
            sfunc_bt_ring_nego();
            break;

        case BT_RING_STA_RES:
            sfunc_bt_ring_res();
            break;

#if BT_HFP_INBAND_RING_EN
        case BT_RING_STA_SCO:
            sfunc_bt_ring_sco();
            break;
#endif

#if BT_HFP_RING_NUMBER_EN
        case BT_RING_STA_NUM:
            sfunc_bt_ring_num();
            break;
#endif
        default:
            break;
    }
}

#if BT_TWS_EN
static void sfunc_bt_ring_slave(void)
{
    if(bt_ring.rem_sta != bt_ring.loc_sta)
    {
        switch(bt_ring.rem_sta)
        {
            case BT_RING_STA_NEGO:
                bt_ring.loc_sta = BT_RING_STA_NEGO;
                break;

            case BT_RING_STA_RES:
                bt_ring.loc_flag |= RING_FLAG_RES;
                bt_ring.loc_sta = BT_RING_STA_RES;
#if BT_HFP_INBAND_RING_EN
                if((bt_ring.loc_flag & RING_FLAG_SCO))
                {
                    bt_ring.loc_flag &= ~RING_FLAG_SCO;
                    bt_audio_bypass();
                }
#endif
                break;

#if BT_HFP_INBAND_RING_EN
            case BT_RING_STA_SCO:
                bt_ring.loc_sta = BT_RING_STA_SCO;
                if((bt_ring.loc_flag & RING_FLAG_SCO) == 0)
                {
                    bt_ring.loc_flag |= RING_FLAG_SCO;
                    bt_audio_enable();
                }
                break;
#endif

#if BT_HFP_RING_NUMBER_EN
            case BT_RING_STA_NUM:
                bt_ring.loc_flag |= RING_FLAG_NUM;
                bt_ring.loc_sta = BT_RING_STA_NUM;
#if BT_HFP_INBAND_RING_EN
                if((bt_ring.loc_flag & RING_FLAG_SCO))
                {
                    bt_ring.loc_flag &= ~RING_FLAG_SCO;
                    bt_audio_bypass();
                }
#endif
                break;
#endif
            default:
                break;
        }
    }
}
#endif
//------------------------------------------------------------------------

AT(.text.func.btring)
void bsp_bt_ring_process(void)
{
#if TRACE_EN
    static u8 state = -1;
    if(state != bt_ring.loc_sta)
    {
        state = bt_ring.loc_sta;
        TRACE("ring_sta: %d(%d), disp=%d,,  %d\n", bt_ring.loc_sta, bt_ring.rem_sta, bt_cb.disp_status, offsetof(struct bt_ring_st, rem_sta));
    }
#endif

    func_process();
    func_bt_sub_process();

    if(bt_ring.loc_sta >= BT_RING_STA_RES)
    {
        if(bsp_res_is_playing())        //等待当前报号播报完
        {
            return;
        }
    }

    if((bt_cb.disp_status != BT_STA_INCOMING || func_cb.sta != FUNC_BT_RING)
       && bt_ring.loc_sta != BT_RING_STA_IDLE)
    {

        bt_ring.tickn = TICK_ADD(TICKN_GET(), 500);
        bt_ring.loc_sta = BT_RING_STA_IDLE;
        bt_tws_sync_ring_sta();
    }
    else
    {
#if BT_TWS_EN
        if(bt_tws_is_slave())
        {
            sfunc_bt_ring_slave();
        }
        else
#endif
        {
            sfunc_bt_ring_master();
        }
    }
}

void bsp_bt_ring_enter(void)
{
#if (ASR_SELECT && ASR_FULL_SCENE)
    bsp_asr_stop();
#endif
    bsp_res_set_enable(true);
    bt_ring.loc_flag        = 0;
    bt_ring.loc_sta         = BT_RING_STA_NEGO;
#if BT_HFP_INBAND_RING_EN || BT_HFP_RING_NUMBER_EN
    bt_ring.tickn           = TICK_ADD(TICKN_GET(), 3500);
#else
    bt_ring.tickn           = TICK_ADD(TICKN_GET(), 1000);
#endif
    bt_ring.nego_tickn      = TICK_ADD(TICKN_GET(), 1000);
    bt_tws_sync_ring_sta();

    dac_set_anl_offset(0);
    bt_audio_bypass();
}

void bsp_bt_ring_exit(void)
{
#if BT_HFP_INBAND_RING_EN
    //避免挂断时后续等待过程漏一点音乐出来
    if((bt_ring.loc_flag & RING_FLAG_SCO))
    {
        bt_audio_bypass();
    }
#endif
    //等待对方状态变成IDLE
    while(bt_tws_is_connected()/* && func_cb.sta == FUNC_BT*/)
    {
        if(TICKN_IS_EXPIRE(bt_ring.tickn)
           || bt_ring.rem_sta == BT_RING_STA_IDLE)
        {
            break;
        }
        bt_thread_check_trigger();
        delay_5ms(1);
    }
    bsp_res_set_enable(false);
    bt_audio_enable();

    bt_ring.rem_sta         = BT_RING_STA_IDLE;
    bt_ring.rem_flag        = 0;
    bt_ring.num_index       = 0;
    bt_ring.num_len         = 0;

#if (ASR_SELECT && ASR_FULL_SCENE)
    if (bt_cb.disp_status != BT_STA_OUTGOING && bt_cb.disp_status != BT_STA_INCALL)
    {
        bsp_asr_start();
    }
#endif
}

#endif //FUNC_BT_EN
