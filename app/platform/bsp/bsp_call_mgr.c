#include "include.h"

void bsp_bt_call_process(bsp_call_mgr_msg_t msg);
void bsp_modem_call_process(bsp_call_mgr_msg_t msg);
void bsp_modem_call_get_current_call(void);
bool cat1_is_bypass(void);

typedef struct {
    u8 modem_call_num;
    u8 bt_call_num;
    u8 msg;

    volatile u8 update;

    u32 ticks;
} bsp_call_mgr_t;

bsp_call_mgr_t call_mgr;

void bsp_call_mgr_init(void)
{
    memset(&call_mgr, 0, sizeof(call_mgr));
}

// clang-format off
AT(.com_text.bsp_call_mgr) NO_INLINE
// clang-format on
static void bsp_call_mgr_send_do(bsp_call_mgr_msg_t msg)
{
    GLOBAL_INT_DISABLE();
    switch (msg) {
    case CALL_MGR_BT_INCOM:
    case CALL_MGR_BT_OUTGO:
    case CALL_MGR_BT_INCALL:
        call_mgr.bt_call_num = 1;
        break;
    case CALL_MGR_BT_ENDCALL:
        call_mgr.bt_call_num = 0;
        break;
    case CALL_MGR_MODEM_INCOM:
    case CALL_MGR_MODEM_OUTGO:
    case CALL_MGR_MODEM_INCALL:
        call_mgr.modem_call_num = 1;
        break;
    case CALL_MGR_MODEM_ENDCALL:
        call_mgr.modem_call_num = 0;
        break;
    default:
        break;
    }
    GLOBAL_INT_RESTORE();
}

static void bsp_call_mgr_endcall(void)
{
    while (func_cb.sta == FUNC_BT_RING || func_cb.sta == FUNC_BT_CALL
#if MODEM_CAT1_EN
           || func_cb.sta == FUNC_MODEM_RING || func_cb.sta == FUNC_MODEM_CALL
#endif
    ) {
        func_cb.sta = func_directly_back_to();
    }
}

// 重点在于解决挂断通话的时候，如何决定通话状态
void bsp_call_mgr_send(bsp_call_mgr_msg_t msg)
{
    bsp_call_mgr_send_do(msg);

    if (msg == CALL_MGR_BT_ENDCALL) {
        call_mgr.update = 0;
        if (call_mgr.modem_call_num != 0) {
            bsp_modem_call_get_current_call();
            return;
        } else {
            bsp_call_mgr_endcall();
        }
    } else if (msg == CALL_MGR_MODEM_ENDCALL) {
        if (call_mgr.bt_call_num != 0) {
            bt_call_get_remote_phone_number();
        } else {
            bsp_call_mgr_endcall();
        }
    }

    if (msg == CALL_MGR_BT_INCOM || msg == CALL_MGR_BT_OUTGO ||
        msg == CALL_MGR_BT_INCALL) {
        if (!((func_cb.sta != FUNC_BT_CALL) &&
              (call_mgr.modem_call_num == 0))) {
            return;
        }
    }

#if BT_VOIP_REJECT_EN
    // 网络电话会在短时间内自动断开
    if (msg == CALL_MGR_BT_INCOM || msg == CALL_MGR_BT_OUTGO) {
        call_mgr.update = 1;
        call_mgr.msg    = msg;
        call_mgr.ticks  = tick_get();
        return;
    }
#endif

    // 传递给对应模块
    bsp_bt_call_process(msg);
    bsp_modem_call_process(msg);
}

void bsp_call_mgr_process(void)
{
#if BT_VOIP_REJECT_EN
    if (call_mgr.update && tick_check_expire(call_mgr.ticks, 500)) {
        call_mgr.update = 0;

        bsp_bt_call_process(call_mgr.msg);
    }
#endif
}

bool bsp_call_mgr_need_update(void)
{
    return call_mgr.update;
}

void bsp_call_mgr_set_audio_path(bsp_call_audio_path_t path)
{
    switch (path) {
    case CALL_AUDIO_PATH_NONE:
        bt_hfp_switch_to_phone();
        if (!cat1_is_bypass()) {
            bsp_modem_audio_stop();
        }
        break;
    case CALL_AUDIO_PATH_BTHF2DAC:
        bsp_iis_stop();
        bt_hfp_switch_to_watch();
        break;
    case CALL_AUDIO_PATH_MODEM2DAC:
        bt_hfp_switch_to_phone();
        bsp_modem_audio_start();
        break;

    default:
        break;
    }
}
