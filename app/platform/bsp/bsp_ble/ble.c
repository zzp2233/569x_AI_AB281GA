#include "include.h"
#include "bsp_ble.h"
#include "ute_application_common.h"

#if LE_EN

#define TRACE_EN    0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


const bool cfg_ble_security_en = LE_PAIR_EN;
bool cfg_ble_sm_sc_en          = LE_SM_SC_EN;
bool cfg_bt_ble_adv            = LE_ADV_POWERON_EN;

static ble_gatt_characteristic_cb_info_t *characteristic_cb_info[LE_ATT_NUM] AT(.ble_cache.att);
static uint8_t  gatts_profile_table[LE_ATT_NUM * 10] AT(.ble_cache.att);
ble_cb_t ble_cb;

void bt_get_local_bd_addr(u8 *addr);
void ble_txpkt_init(void);

//上电默认BLE广播的间隔
uint16_t ble_get_adv_pwron_interval(void)
{
    return 0x60;        //625us * 90 = 60ms
}

//可重定义该函数修改ble地址类型
u8 ble_get_local_addr_mode(void)
{
#if LE_SM_SC_EN
    return GAP_RANDOM_ADDRESS_TYPE_OFF;
#else
    return GAP_RANDOM_ADDRESS_TYPE_STATIC;
#endif
}

//ble通知蓝牙线程回调
void ble_emit_notice(u8 evt, u8 *param)
{

    switch (evt)
    {

        case LE_NOTICE_CONNECTED:
        {
            u16 interval = param[14] | (param[15] << 8);
            u16 latency = param[16] | (param[17] << 8);
            u16 timeout = param[18] | (param[19] << 8);
            ble_cb.ble_connected = true;
            ble_app_connect_callback();
            printf("LE_NOTICE_CONNECTED: interval: %d, latency: %d, timeout: %d\n",
                   interval,  latency,  timeout);

            msg_enqueue(EVT_READY_EXCHANGE_MTU);
        }
        break;

        case LE_NOTICE_DISCONNECT:
            ble_cb.ble_connected = false;
            ble_app_disconnect_callback();
            ble_txpkt_init();
            printf("LE_NOTICE_DISCONNECT:%x\n", param[5]);
            break;

        case LE_NOTICE_CONN_PARAM_UPDATE:
        {
            u16 interval = param[6] | (param[7] << 8);
            u16 latency = param[8] | (param[9] << 8);
            u16 timeout = param[10] | (param[11] << 8);
            printf("LE_NOTICE_CONN_PARAM_UPDATE: interval: %d, latency: %d, timeout: %d\n",
                   interval,  latency,  timeout);
        }
        break;

        case LE_NOTICE_DATA_LEN_CHANGE:
        {
            uint16_t max_rx_octets = param[5] | (param[6] << 8);
            uint16_t max_tx_octets = param[9] | (param[10] << 8);
            printf("LE_NOTICE_DATA_LEN_CHANGE: rx: %d, tx: %d\n", max_rx_octets, max_tx_octets);
            uteApplicationCommonSetMtuSize(max_tx_octets-3);
        }
        break;

        case LE_NOTICE_CLINET_CFG:
        {
            u16 handle = param[0] | (param[1] << 8);
            u8 cfg = param[2];
            ble_app_client_cfg_callback(handle, cfg);
            printf("LE_NOTICE_CLINET_CFG: handle: %d, cfg: %d\n", handle, cfg);
        }
        break;

#if LE_ANCS_CLIENT_EN
        case LE_NOTICE_ANCS_CONN_EVT:
            if (param[0])
            {
                ble_cb.ancs_connected = true;
                printf("ANCS connect\n");
            }
            else
            {
                ble_cb.ancs_connected = false;
                ble_cb.ansc_uid = 0; // ute add
                printf("ANCS disconnect\n");
            }
            uteApplicationCommonSetBlePariring(ble_cb.ancs_connected);
            break;
#endif // LE_ANCS_CLIENT_EN

#if LE_AMS_CLIENT_EN
        case LE_NOTICE_AMS_CONN_EVT:
            if (param[0])
            {
                ble_cb.ams_connected = true;
                printf("AMS connect\n");
            }
            else
            {
                ble_cb.ams_connected = false;
                ble_ams_var_init();
                printf("AMS disconnect\n");
            }
            break;
#endif // LE_AMS_CLIENT_EN

        default:
            break;
    }
}

//att_exchange_mtu_finish_callback(蓝牙线程请勿阻塞)
void ble_att_exchange_mtu_finish_callback(uint16_t mtu)
{

}

// void ble_init_att(void)
// {
//     memset(&ble_cb, 0, sizeof(ble_cb));
//     ble_gatts_init(gatts_profile_table, sizeof(gatts_profile_table),
//                     characteristic_cb_info,
//                     LE_ATT_NUM);
//     ble_app_init();
//     ble_ams_var_init();
// }

//可重定义该函数修改ble地址
void ble_get_local_bd_addr(u8 *addr)
{
    memcpy(addr, xcfg_cb.bt_addr, 6);
#if !LE_SM_SC_EN
    addr[5] ^= 0x55;
#endif
}

#if LE_SM_SC_EN

//发起一键双连
void ble_bt_connect(void)
{
    if (!bt_is_connected())
    {
        ble_send_sm_req();
    }
}

//重置bt地址为bt_get_local_bd_addr的返回值
void bsp_change_bt_mac(void)
{
    // bool bt_dual = (bt_get_scan() == 0x3) ? true: false;

    bt_reset_addr();
    delay_5ms(10);
    // bt_set_scan(0);

    // if (bt_dual)
    {
        delay_5ms(10);
        // bt_set_scan(0x03);
    }
}

void ble_get_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_read: %04x,%04x, %08lx\n", addr, size, BLE_CM_PAGE(addr));

    cm_read(buf, BLE_CM_PAGE(addr), size);

    //print_r(buf, size);
}

void ble_put_link_info(void *buf, u16 addr, u16 size)
{
    //printf("bt_write: %04x,%04x, %08lx\n", addr, size, BLE_CM_PAGE(addr));
    //print_r(buf, size);

    cm_write(buf, BLE_CM_PAGE(addr), size);
}

void ble_sync_link_info(void)
{
    cm_sync();
}
#endif  //LE_SM_SC_EN

///////////////////////////////////////////////////////////////////////////
#if AB_MATE_APP_EN || LE_AB_LINK_APP_EN || LE_PRIV_EN || LE_EN

#if AB_MATE_EQ_USE_DEVICE
#define BLE_NOTIFY_MTU          256
#else
#define BLE_NOTIFY_MTU          185     //max=256
#endif

#define NOTIFY_POOL_PAGE_NB     1
#define NOTIFY_POOL_SIZE        (BLE_NOTIFY_MTU*4)

AT(.ble_cache.att)
uint8_t notify_tx_buf[BLE_NOTIFY_MTU];

AT(.ble_cache.att)
uint8_t notify_pool_buf[NOTIFY_POOL_SIZE];

AT(.ble_cache.att)
ring_buf_t notify_pool;

AT(.rodata.ble.tbl)
const rbuf_tbl_t notify_page_tbl[NOTIFY_POOL_PAGE_NB] =
{
    {
        .buf = notify_pool_buf,
        .size = NOTIFY_POOL_SIZE,
    },
};

void ble_txpkt_init(void)
{
    ring_buf_init(&notify_pool, notify_page_tbl, NOTIFY_POOL_PAGE_NB, 0);
    txpkt_init(&notify_tx, notify_tx_buf, &notify_pool, BLE_NOTIFY_MTU, ble_send_kick);
}
#endif // AB_MATE_APP_EN

void ble_init_att(void)
{
    memset(&ble_cb, 0, sizeof(ble_cb));
    ble_gatts_init(gatts_profile_table, sizeof(gatts_profile_table),
                   characteristic_cb_info,
                   LE_ATT_NUM);
    ble_app_init();
    ble_ams_var_init();
}

#endif  // LE_EN
