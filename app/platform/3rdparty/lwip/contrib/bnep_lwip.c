#include "include.h"

#include "lwip/sys.h"
#include "lwip/arch.h"
#include "lwip/api.h"
#include "lwip/netifapi.h"
#include "lwip/tcpip.h"
#include "lwip/ip.h"
#include "lwip/dhcp.h"
#include "lwip/sockets.h"
#include "lwip/prot/dhcp.h"
#include "lwip/ethip6.h"
#include "lwip/dns.h"
#include "lwip/mem.h"
#include "lwip/apps/sntp.h"
#include "netif/etharp.h"
#include "lwip/apps/sntp.h"
#include "arch/sys_arch.h"
#include "rt_thread.h"
#define L2CAP_DEFAULT_MTU (1460)
// #define L2CAP_DEFAULT_MTU (661)

/* Short name used for netif in lwIP */
#define IFNAME0     'b'
#define IFNAME1     't'

#define log_debug   printf
#define log_info    printf
#define log_error   printf

typedef uint8_t bd_addr_t[6];

#if BT_PANU_EN

static os_mq_t bnep_lwip_outgoing_mbox = NULL;
static struct netif btstack_netif;

// next packet only modified from btstack context
static void *bnep_lwip_outgoing_next_packet;

static u32 netif_tick;
static bool bnep_profile_enable;

void lwip_sys_init(void);
void bt_thread_check_trigger(void);
char *bd_addr_to_str(bd_addr_t addr);
void bnep_network_send_packet(const uint8_t *packet, uint16_t size);

os_err_t os_mq_init(os_mq_t mq, const char *name, void *msgpool, os_size_t msg_size, os_size_t pool_size, os_uint8_t flag);

static uint16_t
bnep_lwip_pbuf_copy_partial(void *pbuf, void *dest, uint16_t dest_size)
{
    struct pbuf *p   = pbuf;
    uint16_t     len = minu(dest_size, p->tot_len);
    //printf("copy: %d %d %d\n", p->tot_len, dest_size, len);
    pbuf_copy_partial(p, dest, len, 0);
    return len;
}

static void bnep_lwip_free_pbuf(struct pbuf *p)
{
    if (pbuf_free_callback(p) != ERR_OK)
    {
        // TODO:看看怎么处理
        printf("%s err\n", __func__);
        WDT_RST();
    }
}

static void dhcp_status_callback(struct netif *netif)
{
    struct dhcp *dhcp = netif_dhcp_data(netif);

    switch (dhcp->state)
    {
        case DHCP_STATE_OFF:
            printf("DHCP state: off\n");
            // bsp_net_set_state(BSP_NET_OFF);
            break;
        case DHCP_STATE_REQUESTING:
            printf("DHCP state: request\n");
            // bsp_net_set_state(BSP_NET_SEARCHING);
            break;
        case DHCP_STATE_BOUND:
            printf("DHCP state: bound\n");
            printf("IP : %s\n", ipaddr_ntoa(&netif->ip_addr));
            printf("gw : %s\n", ipaddr_ntoa(&netif->gw));
            printf("netmask : %s\n", ipaddr_ntoa(&netif->netmask));
            // bsp_net_set_state(BSP_NET_BOUND);
            {
                // ip_addr_t ping_addr;
                // IP_ADDR4(&ping_addr, 180, 76, 76, 76);
                // void ping_init(const ip_addr_t *ping_addr);
                // ping_init(&ping_addr);

                // sys_thread_new("http_thread", http_thread, NULL, DEFAULT_THREAD_STACKSIZE, DEFAULT_THREAD_PRIO);

                ip_addr_t ntp_addr;
                // ipaddr_aton("ntp.aliyun.com", &ntp_addr);
                IP_ADDR4(&ntp_addr, 203, 107, 6, 88); //ntp.aliyun.com

                sntp_setserver(0, &ntp_addr);
                sntp_init();
            }
            break;
        // 其他状态可根据需要添加
        default:
            printf("DHCP state: %d\n", dhcp->state);
            break;
    }
}

static void bnep_lwip_outgoing_packet_processed(void)
{
    // free pbuf
    bnep_lwip_free_pbuf(bnep_lwip_outgoing_next_packet);
    // mark as done
    bnep_lwip_outgoing_next_packet = NULL;
}

static int bnep_lwip_outgoing_init_queue(void)
{
    // bnep_lwip_outgoing_mbox =
    //     os_mq_create("lb", 4, TCP_SND_QUEUELEN + 2, OS_IPC_FLAG_FIFO);

    int pool_size = (4 + 4) * (TCP_SND_QUEUELEN + 2);

    bnep_lwip_outgoing_mbox = mem_malloc(sizeof(struct os_messagequeue));
    LWIP_ASSERT("bnep_lwip_outgoing_mbox", bnep_lwip_outgoing_mbox);

    void *msgpool = mem_malloc(pool_size);
    LWIP_ASSERT("bnep_lwip_outgoing_mbox pool", msgpool);

    os_mq_init(bnep_lwip_outgoing_mbox, "lb", msgpool, 4, pool_size, OS_IPC_FLAG_FIFO);
    return 0;
}

static bool bnep_lwip_outgoing_queue_packet(struct pbuf *p)
{
    // printf("%s\n", __func__);
    u32 addr = (u32)p;
    if (os_mq_send(bnep_lwip_outgoing_mbox, &addr, 4) != OS_EOK)
    {
        printf("outgoing full\n");
        return false;
    }
    return true;
}

static struct pbuf *bnep_lwip_outgoing_pop_packet(void)
{
    if (bnep_lwip_outgoing_mbox == NULL)
    {
        return NULL;
    }
    u32          addr = 0;
    struct pbuf *p    = NULL;
    if (os_mq_recv(bnep_lwip_outgoing_mbox, &addr, 4, 0) != 0)
    {
        return NULL;
    }
    p = (struct pbuf *)addr;
    return p;
}

static void bnep_lwip_outgoing_reset_queue(void)
{
    if (bnep_lwip_outgoing_mbox)
    {
        while (bnep_lwip_outgoing_mbox->entry > 0)
        {
            bnep_lwip_free_pbuf(bnep_lwip_outgoing_pop_packet());
        }
        // os_mq_detach(bnep_lwip_outgoing_mbox);
        // mem_free(bnep_lwip_outgoing_mbox->msg_pool);
        // mem_free(bnep_lwip_outgoing_mbox);
        // bnep_lwip_outgoing_mbox = NULL;
    }
}

static int bnep_lwip_outgoing_packets_empty(void)
{
    return (bnep_lwip_outgoing_mbox->entry == 0);
}

static void bnep_lwip_packet_init(void)
{
    bnep_lwip_outgoing_next_packet = NULL;
}

static err_t low_level_output(struct netif *netif, struct pbuf *p)
{
    // log_info("low_level_output: packet %p, len %u, total len %u\n",
    //          p,
    //          p->len,
    //          p->tot_len);

    // // bnep up?
    // if (bnep_cid == 0)
    //     return ERR_OK;
    // if (!(netif->flags & NETIF_FLAG_LINK_UP))
    //     return ERR_OK;

    // inc refcount
    pbuf_ref(p);

    LWIP_ASSERT("bnep_lwip_outgoing_mbox != NULL",
                bnep_lwip_outgoing_mbox != NULL);

    // queue empty now?
    int queue_empty = bnep_lwip_outgoing_packets_empty();

    // queue up
    if (!bnep_lwip_outgoing_queue_packet(p))
    {
        return ERR_MEM;
    }

    // trigger processing if queue was empty (might be new packet)
    if (queue_empty)
    {
        bt_thread_check_trigger();
    }

    return ERR_OK;
}

/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
static err_t bnep_lwip_netif_init(struct netif *netif)
{
    // interface short name
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;

    // mtu
    netif->mtu = L2CAP_DEFAULT_MTU;

    /* device capabilities */
    netif->flags |= NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP;

    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...)
     */
    netif->output = etharp_output;
#if LWIP_IPV6
    netif->output_ip6 = ethip6_output;
#endif
    netif->linkoutput = low_level_output;

    return ERR_OK;
}

static void bnep_lwip_init(void)
{
    log_debug("%s\n", __func__);
    lwip_sys_init();
    bnep_lwip_outgoing_init_queue();
    //bnep_lwip_outgoing_next_packet = NULL;
    bnep_profile_enable = false;

    // input function differs for sys vs nosys
    netif_input_fn input_function;
    input_function = tcpip_input;

    LOCK_TCPIP_CORE();
    netif_add(&btstack_netif,
              NULL,
              NULL,
              NULL,
              NULL,
              bnep_lwip_netif_init,
              input_function);

    // 启用 IPv6 地址
    // netif_ip6_addr_set_state(&btstack_netif, 0, IP6_ADDR_VALID);

    // dhcp_start(&btstack_netif);
    netif_set_status_callback(&btstack_netif, dhcp_status_callback);

    netif_set_up(&btstack_netif);
    dhcp_start(&btstack_netif);

    netif_set_default(&btstack_netif);
    UNLOCK_TCPIP_CORE();
}

/**
 * @brief Bring up network interfacd
 * @param network_address
 * @return 0 if ok
 */
static int bnep_lwip_netif_up(bd_addr_t network_address)
{
    log_info("bnep_lwip_netif_up start addr %s\n",
             bd_addr_to_str(network_address));

    LOCK_TCPIP_CORE();
    // set mac address
    btstack_netif.hwaddr_len = 6;
    memcpy(btstack_netif.hwaddr, network_address, 6);

    // link is up
    netif_set_link_up(&btstack_netif);

    UNLOCK_TCPIP_CORE();
    return 0;
}

static int bnep_lwip_netif_down(void)
{
    log_info("bnep_lwip_netif_down");

    LOCK_TCPIP_CORE();
    netif_set_link_down(&btstack_netif);
    UNLOCK_TCPIP_CORE();
    return 0;
}

AT(.com_text.stack.run_loop)
static void bnep_lwip_discard_packets(void)
{
    // discard current packet
    if (bnep_lwip_outgoing_next_packet)
    {
        bnep_lwip_outgoing_packet_processed();
    }

    // reset queue
    bnep_lwip_outgoing_reset_queue();
}

static void bnep_lwip_netif_process_packet(const uint8_t *packet, uint16_t size)
{
    struct pbuf *p = pbuf_alloc(PBUF_RAW, size, PBUF_POOL);
    // log_debug("%s alloc=%x\n", __func__, p);

    if (!p)
        return;

    /* store packet in pbuf chain */
    struct pbuf *q = p;
    while (q != NULL && size)
    {
        memcpy(q->payload, packet, q->len);
        packet += q->len;
        size -= q->len;
        q = q->next;
    }

    if (size != 0)
    {
        log_error("failed to copy data into pbuf");
        bnep_lwip_free_pbuf(p);
        return;
    }

    /* pass all packets to ethernet_input, which decides what packets it supports */
    int res = btstack_netif.input(p, &btstack_netif);
    if (res != ERR_OK)
    {
        log_error("bnep_lwip_netif_process_packet: IP input error %d\n", res);
        bnep_lwip_free_pbuf(p);
        p = NULL;
    }
}

void bnep_network_init(void)
{
    bnep_lwip_packet_init();
    bnep_lwip_init();
}

void bnep_network_up(bd_addr_t addr)
{
    bnep_lwip_discard_packets();
    bnep_lwip_netif_up(addr);
    netif_tick = tick_get();
    bnep_profile_enable = true;
}

void bnep_network_down(void)
{
    if (!tick_check_expire(netif_tick, 500))
    {
        bnep_profile_enable = false;
    }
    bnep_lwip_discard_packets();
    bnep_lwip_netif_down();
}

void bnep_network_process_packet(const uint8_t *packet, uint16_t size)
{
    bnep_lwip_netif_process_packet(packet, size);
}

AT(.com_text.stack.run_loop)
void bnep_network_outgoing_process(void)
{
    uint16_t len;

    // previous packet not sent yet
    if (bnep_lwip_outgoing_next_packet)
    {
        return;
    }

    if (bnep_lwip_outgoing_packets_empty())
    {
        return;
    }

    void *buf = mem_malloc(L2CAP_DEFAULT_MTU);
    if (buf == NULL)
    {
        log_error("%s malloc failed!\n", __func__);
        return;
    }

    void *packet = bnep_lwip_outgoing_pop_packet();
    if (packet == NULL)
    {
        printf("%s empty\n", __func__);
        mem_free(buf);
        return;
    }
    bnep_lwip_outgoing_next_packet = packet;

    // flatten into our buffer
    len = bnep_lwip_pbuf_copy_partial(bnep_lwip_outgoing_next_packet,
                                      buf,
                                      L2CAP_DEFAULT_MTU);

    // request can send now
    bnep_network_send_packet(buf, len);
}

void bnep_network_packet_sent(uint8_t *buf)
{
    // log_debug("bnep_lwip_packet_sent: %p\n", bnep_lwip_outgoing_next_packet);

    mem_free(buf);

    // release current packet
    bnep_lwip_outgoing_packet_processed();

    // more ?
    if (bnep_lwip_outgoing_packets_empty())
    {
        return;
    }
    bt_thread_check_trigger();
}

bool bnep_network_is_ok(void)
{
    struct netif *netif = &btstack_netif;

    // 检查接口状态
    if (!netif_is_up(netif) || !netif_is_link_up(netif))
    {
        printf("1111111\r\n");
        return false;
    }

    // 检查IP地址
    const ip_addr_t* ip_addr = netif_ip_addr4(netif);
    if (ip_addr_isany(ip_addr))
    {
        printf("222222\r\n");
        return false;
    }

    // 检查网关
    const ip_addr_t* gw_addr = netif_ip_gw4(netif);
    if (ip_addr_isany(gw_addr))
    {
        printf("333333\r\n");
        return false;
    }


    // 检查DNS
    const ip_addr_t* dns_addr = dns_getserver(0);
    if (ip_addr_isany(dns_addr))
    {
        printf("444444\r\n");
        return false;
    }

#if LWIP_DHCP
    if (!dhcp_supplied_address(netif))
    {
        printf("555555\r\n");
        return false;
    }
#endif

    return true;
}

bool bnep_profile_is_on(void)
{
    if (tick_check_expire(netif_tick, 500))
    {
        return bnep_profile_enable;
    }
    return false;
}

#endif
