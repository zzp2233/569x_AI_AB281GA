#include "include.h"
#include "chatbot.h"
#include "../../3rdparty/cjson/cJSON.h"
#include "lwip/arch.h"

#include <nopoll.h>
#include <nopoll_decl.h>
#include <nopoll_private.h>

#define SEND_MQ_NUM 4
#define SEND_POOL_SIZE (8 * SEND_MQ_NUM)

#define OPUS_AEC_EN 0
#define OPUS_ENC_FRAME_SIZE 40
#define OPUS_ENC_FRAME_NUM 1

#define OPUS_DEC_FRAME_SIZE 40

#define STR_CONCAT 0
#define CHATBOT_RECV_BUF_SIZE 640

#define CHATBOT_TX_FIFO_SIZE (9600)
#define CHATBOT_RX_FIFO_SIZE (5120)
#define AUDIO_THREAD_STACK_SIZE (1024 + 512)
#define SEND_THREAD_STACK_SIZE (2048)
#define RECV_THREAD_STACK_SIZE (1024 + 512)

#define malloc ab_malloc
#define zalloc ab_zalloc
#define free ab_free

u8 chatbot_tx_fifo[CHATBOT_TX_FIFO_SIZE] AT(.avio_buf);
u8 chatbot_rx_fifo[CHATBOT_RX_FIFO_SIZE] AT(.opus_buf.input);

#define HOST_NAME "106.52.115.173"
#define HOST_PORT "8000"
#define HOST_URI "/?token=your-token1&device_id=web_test_device&device_mac=00%3A11%3A22%3A33%3A44%3A55"

static const char *json_str = "{\"type\": \"hello\", \"version\": 1, \"transport\": \"websocket\", \"audio_params\": {\"format\": \"opus\", \"sample_rate\": 16000, \"channels\": 1, \"frame_duration\": 20}}";
static const char *json_listen_start = "{\"type\":\"listen\",\"state\":\"start\",\"mode\":\"auto\"}";
// static const char *json_listen_stop = "{\"type\":\"listen\",\"state\":\"stop\"}";
// static const char *json_listen_abort = "{\"type\":\"abort\"}";

typedef struct
{
    struct os_thread recv_thread;
    struct os_thread send_thread;
    struct os_thread audio_thread;

    struct os_mutex init_mutex;
    struct os_mutex opus_mutex;
    struct os_mutex send_mutex;
    struct os_messagequeue send_mq;
    u8 send_mq_pool[SEND_POOL_SIZE];

    void *recv_stack;
    void *audio_stack;

    chatbot_text_callback_t text_cb;
    chatbot_event_callback_t event_cb;

    u8 opus_enc_frame[OPUS_ENC_FRAME_SIZE * OPUS_ENC_FRAME_NUM];
    u8 recv_buf[CHATBOT_RECV_BUF_SIZE];
    u8 send_buf[CHATBOT_RECV_BUF_SIZE];

    void *res_addr;
    u16 res_len;
    u16 res_offset;

    u32 tick;

    void *frag_msg;
    noPollConn *conn;

    au_stm_t tx_fifo;
    au_stm_t rx_fifo;
    volatile u32 flags;
} chatbot_cb_t;

#define FIRST_FLAG BIT(0)
#define THREAD_START_FLAG BIT(1)
#define NOTIFY_FLAG BIT(3)
#define EXITING_FLAG BIT(4)
#define SEND_EXIT_DONE BIT(5)
#define RECV_EXIT_DONE BIT(6)
#define AUDIO_EXIT_DONE BIT(7)
#define MIC_START_FLAG BIT(8)
#define SEND_LISTEN_START BIT(9)
#define SEND_LISTEN_STOP BIT(10)
#define SEND_LISTEN_ABORT BIT(11)
#define SENTENCED_END_FLAG BIT(12)

static void chatbot_run_once(void);
static int chatbot_send_data(noPollConn *conn, const char *content, u32 length, bool is_text);

bool opus_enc_init(u32 spr, u32 nch, u32 bitrate);
int opus_enc_frame(s16 *pcm, u8 *packet);
bool opus_dec_init(u32 spr, u32 nch);
void opus_start_init(u8 nr_type);

bool bnep_network_is_ok(void);
os_err_t os_mq_init(os_mq_t mq, const char *name, void *msgpool, os_size_t msg_size, os_size_t pool_size, os_uint8_t flag);
long list_thread(void);

int nopoll_rtt_init(void);
uint32_t celt_stack_get_size(void);
void opus_enc_process_do(void *buf);
bool music_is_playing(void);

static noPollConn *chatbot_new_conn(void);
static void chatbot_recv_entry(void *p);
static void chatbot_send_entry(void *p);

static void on_message(noPollCtx  *ctx,
                       noPollConn *conn,
                       noPollMsg  *msg,
                       noPollPtr   user_data);

static chatbot_cb_t *chatbot_cb;
AT(.com_rodata.str)
const char info_chatbot[] = "puts_stm_bufputs_stm_bufputs_stm_buf\n";
AT(.com_text) NO_INLINE
static void chatbot_set_flags(u32 flag)
{
    GLOBAL_INT_DISABLE();
    chatbot_cb->flags |= flag;
    GLOBAL_INT_RESTORE();
}

AT(.com_text) NO_INLINE
static void chatbot_unset_flags(u32 flag)
{
    GLOBAL_INT_DISABLE();
    chatbot_cb->flags &= ~flag;
    GLOBAL_INT_RESTORE();
}

AT(.com_text)
void chatbot_sdadc_process(u8 *ptr, u32 samples, int ch_mode)
{
    if (chatbot_cb && (chatbot_cb->flags & THREAD_START_FLAG) &&
        (chatbot_cb->flags & MIC_START_FLAG))
    {
        // GPIOHCLR = BIT(6);
        // bt_sco_dump(0, 0, ptr, samples);
        // uart_putchar('1');
        puts_stm_buf(&chatbot_cb->tx_fifo, ptr, samples * 2);
        // printf(info_chatbot);
        chatbot_run_once();
        // GPIOHSET = BIT(6);
    }
}

static void chatbot_tx_fifo_init(chatbot_cb_t *chatbot_cb)
{
    au_stm_t *stm = &chatbot_cb->tx_fifo;
    stm->buf = stm->wptr = stm->rptr = chatbot_tx_fifo;
    stm->size = CHATBOT_TX_FIFO_SIZE;
    stm->len = 0;
}

static void chatbot_rx_fifo_init(chatbot_cb_t *chatbot_cb)
{
    au_stm_t *stm = &chatbot_cb->rx_fifo;
    stm->buf = stm->wptr = stm->rptr = chatbot_rx_fifo;
    stm->size = CHATBOT_RX_FIFO_SIZE;
    stm->len = 0;
}

void print_all_thread_remain(void)
{
    list_thread();
}

bool chatbot_init(void)
{
    if (chatbot_cb)
    {
        printf("%s exiting\n", __func__);
        return false;
    }

    if (!bnep_network_is_ok())
    {
        printf("%s bnep_network_is_ok()=%d\r\n", __func__,bnep_network_is_ok());
        chatbot_cb = false;
        return false;
    }

    nopoll_rtt_init();

    int ret;
    void *send_stack = NULL;
    chatbot_cb = zalloc(sizeof(chatbot_cb_t));
    if (!chatbot_cb)
    {
        goto _fail;
    }

    chatbot_cb->recv_stack = malloc(RECV_THREAD_STACK_SIZE);
    if (chatbot_cb->recv_stack == NULL)
    {
        goto _fail;
    }

    chatbot_cb->audio_stack = malloc(AUDIO_THREAD_STACK_SIZE);
    if (chatbot_cb->audio_stack == NULL)
    {
        goto _fail;
    }

    send_stack = malloc(SEND_THREAD_STACK_SIZE);
    if (send_stack == NULL)
    {
        goto _fail;
    }

    bt_audio_bypass();
    // bt_music_stop();
    music_control(MUSIC_MSG_STOP);

    if (!opus_enc_init(SPR_16000, 1, 16000))
    {
        goto _fail;
    }
    if (!opus_dec_init(SPR_16000, 1))
    {
        goto _fail;
    }

    os_mutex_init(&chatbot_cb->init_mutex, "imu", OS_IPC_FLAG_FIFO);
    os_mutex_init(&chatbot_cb->opus_mutex, "omu", OS_IPC_FLAG_FIFO);
    os_mutex_init(&chatbot_cb->send_mutex, "smu", OS_IPC_FLAG_FIFO);
    ret = os_mq_init(&chatbot_cb->send_mq, "cs", chatbot_cb->send_mq_pool, 4, SEND_POOL_SIZE, OS_IPC_FLAG_FIFO);
    printf("mq init:%d\n", ret);

    chatbot_tx_fifo_init(chatbot_cb);
    chatbot_rx_fifo_init(chatbot_cb);
    mem_monitor_run();
    os_thread_init(&chatbot_cb->send_thread, "snd", chatbot_send_entry, NULL, send_stack, SEND_THREAD_STACK_SIZE, 28, -1);
    return true;

_fail:
    printf("%s failed, chatbot_cb:%x, recv_stack:%x, send_stack:%x\n",
           __func__,
           chatbot_cb,
           chatbot_cb->recv_stack,
           send_stack);

    if (chatbot_cb->recv_stack)
    {
        free(chatbot_cb->recv_stack);
    }
    if (chatbot_cb->audio_stack)
    {
        free(chatbot_cb->audio_stack);
    }
    if (send_stack)
    {
        free(send_stack);
    }
    if (chatbot_cb)
    {
        free(chatbot_cb);
        chatbot_cb = NULL;
    }
    return false;
}

void chatbot_deinit(void)
{
    if (!chatbot_cb)
    {
        return;
    }

    noPollConn *conn = chatbot_cb->conn;
    os_mutex_take(&chatbot_cb->init_mutex, OS_WAITING_FOREVER);
    chatbot_set_flags(EXITING_FLAG);
    chatbot_tx_fifo_init(chatbot_cb);
    chatbot_rx_fifo_init(chatbot_cb);
    chatbot_stop();

    bt_auto_sniff_set(1);

    void *buf;

    buf = chatbot_cb->send_thread.stack_addr;
    printf("send_thread: %x\n", buf);

    if (!(chatbot_cb->flags & THREAD_START_FLAG))
    {
        if (buf)
        {
            os_thread_detach(&chatbot_cb->send_thread);
            free(buf);
        }
    }
    else
    {
        int ret = nopoll_conn_set_sock_block(conn->session, false);
        printf("set block: %d\n", ret);

        while ((chatbot_cb->flags &
                (SEND_EXIT_DONE | RECV_EXIT_DONE | AUDIO_EXIT_DONE)) !=
               (SEND_EXIT_DONE | RECV_EXIT_DONE | AUDIO_EXIT_DONE))
        {
            func_process();
        }
        free(buf);
    }

    printf("frag_msg:%x\n", chatbot_cb->frag_msg);
    if (chatbot_cb->frag_msg)
    {
        nopoll_msg_unref(chatbot_cb->frag_msg);
    }

    printf("recv_stack %x\n", chatbot_cb->recv_stack);
    free(chatbot_cb->recv_stack);
    printf("audio_stack %x\n", chatbot_cb->audio_stack);
    free(chatbot_cb->audio_stack);

    printf("deinit 4\n");
    os_mq_detach(&chatbot_cb->send_mq);
    os_mutex_detach(&chatbot_cb->opus_mutex);
    os_mutex_detach(&chatbot_cb->send_mutex);

    os_mutex_release(&chatbot_cb->init_mutex);
    os_mutex_detach(&chatbot_cb->init_mutex);

    free(chatbot_cb);
    chatbot_cb = NULL;
    bt_audio_enable();
}

void chatbot_500ms_callback(void)
{
}

void chatbot_start(void)
{
    os_thread_startup(&chatbot_cb->send_thread);

#if OPUS_AEC_EN
    // CLKGAT1 |= BIT(4)|BIT(5)|BIT(27);
    // CLKGAT2 |= BIT(18);

    // bt_call_alg_init();
#endif
    chatbot_tx_fifo_init(chatbot_cb);
    aubuf0_gpdma_init(1);
    audio_path_init(AUDIO_PATH_CHATBOT);
    audio_path_start(AUDIO_PATH_CHATBOT);

    chatbot_start_mic();
    bsp_change_volume(VOL_MAX);
    // bsp_change_volume(bsp_bt_get_hfp_vol(sys_cb.hfp_vol));
    dac_fade_in();

    printf("DACVOLCON=0x%x DACDIGCON0=0x%x, DACDIGCON1=0x%x\n", DACVOLCON, DACDIGCON0, DACDIGCON1);
}

void chatbot_stop(void)
{
    chatbot_stop_mic();
    audio_path_exit(AUDIO_PATH_CHATBOT);

    dac_fade_out();
    dac_aubuf_clr();
#if OPUS_AEC_EN
    // bt_call_alg_exit();
    // dac_set_anl_offset(0);

    // CLKGAT1 &= ~(BIT(4)|BIT(5)|BIT(27));
    // CLKGAT2 &= ~BIT(18);
#endif
    bsp_change_volume(sys_cb.vol);
}

void chatbot_set_text_callback(chatbot_text_callback_t cb)
{
    if (!chatbot_cb)
    {
        return;
    }
    chatbot_cb->text_cb = cb;
}

void chatbot_set_event_callback(chatbot_event_callback_t cb)
{
    if (!chatbot_cb)
    {
        return;
    }
    chatbot_cb->event_cb = cb;
}

bool chatbot_set_notify_res(void *addr, u16 len)
{
    if (len < OPUS_ENC_FRAME_SIZE)
    {
        return false;
    }
    u16 align = len / OPUS_ENC_FRAME_SIZE * OPUS_ENC_FRAME_SIZE;
    chatbot_cb->res_addr = addr;
    chatbot_cb->res_len = align;
    printf("%s %x %d\n", __func__, addr, align);
    return true;
}

void chatbot_set_debug_log(void *cb)
{}

static void chatbot_emit_event(chatbot_event_t event)
{
    if (chatbot_cb && chatbot_cb->event_cb)
    {
        chatbot_cb->event_cb(event);
    }
}

AT(.com_text)
static void chatbot_run_once(void)
{
    u32 msg = 0;
    os_mq_send(&chatbot_cb->send_mq, &msg, 4);
}

void chatbot_start_mic(void)
{
    print_all_thread_remain();

    chatbot_set_flags(MIC_START_FLAG | SEND_LISTEN_START | SEND_LISTEN_ABORT);
    chatbot_run_once();
}

void chatbot_stop_mic(void)
{
    chatbot_unset_flags(MIC_START_FLAG);
    chatbot_set_flags(SEND_LISTEN_STOP);
    chatbot_tx_fifo_init(chatbot_cb);
    chatbot_run_once();
}

u8 bsp_opus_encode_get_bypass(void);
u16 dac_fifo_get_remain_size(void);
int opus_dec_frame(s16 *pcm_obuf, u8 *packet, u32 packet_len);

static void chatbot_send_process(noPollConn *conn);
static void chatbot_audio_entry(void *p);

void print_ctx(noPollCtx *ctx)
{
    print_r(ctx, sizeof(noPollCtx));
    printf("ctx1: %x %x %x %x %x %x\n", ctx->refs, ctx->not_executed, ctx->debug_enabled, ctx->not_executed_color, ctx->debug_color_enabled, ctx->keep_looping);
    printf("ctx2: %x %x %x %x %x\n", ctx->conn_connect_std_timeout, ctx->backlog, ctx->io_engine, ctx->conn_id, ctx->conn_list);
    printf("ctx3: %x %x %x %x\n", ctx->conn_length, ctx->conn_num, ctx->on_accept, ctx->on_accept_data);
    printf("ctx4: %x %x %x %x %x %x\n", ctx->on_ready, ctx->on_ready_data, ctx->on_open, ctx->on_open_data, ctx->on_msg, ctx->on_msg_data);
    printf("ctx5: %x %x %x %x %x %x\n", ctx->protocol_version, ctx->certificates, ctx->certificates_length, ctx->ref_mutex, ctx->log_handler, ctx->log_user_data);
    printf("ctx6: %x %x %x %x\n", ctx->context_creator, ctx->context_creator_data, ctx->post_ssl_check, ctx->post_ssl_check_data);
}

static void on_close(noPollCtx  * ctx,
                     noPollConn * conn,
                     noPollPtr    user_data)
{
    chatbot_emit_event(CHATEVT_ERROR_RECONN);
    chatbot_set_flags(EXITING_FLAG);
}

static void chatbot_close_conn(void)
{
    noPollConn *conn = chatbot_cb->conn;
    noPollCtx *ctx = conn->ctx;
    print_ctx(ctx);
    nopoll_loop_stop(conn->ctx);
    os_mutex_take(&chatbot_cb->send_mutex, OS_WAITING_FOREVER);
    nopoll_conn_close(conn);
    os_mutex_release(&chatbot_cb->send_mutex);
    chatbot_set_flags(SEND_EXIT_DONE);
    print_ctx(ctx);
}

static void chatbot_send_entry(void *p)
{
    printf("%s \r\n", __func__);
    noPollConn *conn;
    os_mutex_take(&chatbot_cb->init_mutex, OS_WAITING_FOREVER);

    bt_sniff_drop_out();
    delay_5ms(100);

    // connect
    conn = chatbot_new_conn();
    if (!conn)
    {
        printf("!conn\r\n");
        os_mutex_release(&chatbot_cb->init_mutex);
        chatbot_emit_event(CHATEVT_ERROR_BROKEN);
        os_thread_exit();
    }

    chatbot_cb->conn = conn;

    nopoll_conn_set_on_msg(conn, on_message, NULL);
    nopoll_conn_set_sock_block(conn->session, true);
    nopoll_conn_set_on_close(conn, on_close, NULL);
    nopoll_log_enable(conn->ctx, true);

    os_thread_init(&chatbot_cb->recv_thread, "crt", chatbot_recv_entry, conn->ctx, chatbot_cb->recv_stack, RECV_THREAD_STACK_SIZE, 29, -1);
    os_thread_init(&chatbot_cb->audio_thread, "cat", chatbot_audio_entry, NULL, chatbot_cb->audio_stack, AUDIO_THREAD_STACK_SIZE, 29, -1);
    os_thread_startup(&chatbot_cb->audio_thread);
    os_thread_startup(&chatbot_cb->recv_thread);
    chatbot_set_flags(THREAD_START_FLAG);
    os_mutex_release(&chatbot_cb->init_mutex);

    chatbot_cb->tick = tick_get();
    chatbot_send_data(conn, json_listen_start, strlen(json_listen_start), true);
    chatbot_cb->event_cb(CHATEVT_IS_CONN);

    while (1)
    {
        // if (chatbot_cb->flags & SEND_LISTEN_ABORT) {
        //     chatbot_unset_flags(SEND_LISTEN_ABORT);
        //     chatbot_send_data(conn, json_listen_abort, strlen(json_listen_abort), true);
        // }
        // if (chatbot_cb->flags & SEND_LISTEN_START) {
        //     chatbot_unset_flags(SEND_LISTEN_START);
        //     chatbot_send_data(conn, json_listen_start, strlen(json_listen_start), true);
        // } else if (chatbot_cb->flags & SEND_LISTEN_STOP) {
        //     chatbot_unset_flags(SEND_LISTEN_STOP);
        //     chatbot_send_data(conn, json_listen_stop, strlen(json_listen_stop), true);
        // }
        if ((chatbot_cb->flags & SENTENCED_END_FLAG) && (chatbot_cb->rx_fifo.len < OPUS_ENC_FRAME_SIZE))
        {
            printf("SENTENCED_END_FLAGSENTENCED_END_FLAG\r\n");
            chatbot_unset_flags(SENTENCED_END_FLAG);
            chatbot_start_mic();
            chatbot_cb->event_cb(CHATEVT_LISTENING);
        }

        if (chatbot_cb->flags & EXITING_FLAG)
        {
            chatbot_close_conn();
            printf("%s close2\n", __func__);
            os_thread_exit();
        }
        chatbot_send_process(conn);
    }
}

static void chatbot_send_process(noPollConn *conn)
{
    u32 msg;
    os_mq_recv(&chatbot_cb->send_mq, &msg, 4, 5);

    if (tick_check_expire(chatbot_cb->tick, 1000))
    {
        os_mutex_take(&chatbot_cb->send_mutex, OS_WAITING_FOREVER);
        nopoll_conn_send_ping(conn);
        os_mutex_release(&chatbot_cb->send_mutex);
        chatbot_cb->tick = tick_get();
    }

    // GPIOHCLR = BIT(5);
    if (!gets_stm_buf(&chatbot_cb->tx_fifo, chatbot_cb->send_buf, 640))
    {
        return;
    }
    //bt_sco_dump(1, 2, chatbot_cb->send_buf, 320);

    // uart_putchar('2');
    os_mutex_take(&chatbot_cb->opus_mutex, OS_WAITING_FOREVER);
    int ret = opus_enc_frame((void *)chatbot_cb->send_buf, chatbot_cb->send_buf);
    os_mutex_release(&chatbot_cb->opus_mutex);
    if (ret == OPUS_ENC_FRAME_SIZE)
    {
        chatbot_send_data(conn, (void *)chatbot_cb->send_buf, OPUS_ENC_FRAME_SIZE, false);
    }
    else
    {
        uart_putchar('q');
    }
    // uart_putchar('3');
    // GPIOHSET = BIT(5);
}

static void chatbot_audio_entry(void *p)
{
    au_stm_t *stm = &chatbot_cb->rx_fifo;
    while (1)
    {
        if (chatbot_cb->flags & EXITING_FLAG)
        {
            printf("%s close\n", __func__);
            chatbot_set_flags(AUDIO_EXIT_DONE);
            os_thread_exit();
            return;
        }

        if ((chatbot_cb->flags & NOTIFY_FLAG) && chatbot_cb->res_addr &&
            (stm->len < OPUS_ENC_FRAME_SIZE * 10))
        {
            printf("(3 %d %d)\n", stm->len, stm->size - stm->len);
            // print_r(packet, OPUS_ENC_FRAME_SIZE);
            puts_stm_buf(stm,
                         chatbot_cb->res_addr + chatbot_cb->res_offset,
                         OPUS_ENC_FRAME_SIZE);
            // print_r(chatbot_cb->res_addr + chatbot_cb->res_offset, OPUS_ENC_FRAME_SIZE);
            chatbot_cb->res_offset =
                (chatbot_cb->res_offset + OPUS_ENC_FRAME_SIZE) %
                chatbot_cb->res_len;
        }

        u8 *packet = chatbot_cb->opus_enc_frame;
        if (gets_stm_buf(stm, packet, OPUS_ENC_FRAME_SIZE))
        {
            // uart_putchar('2');
            printf("(2 %d %d)\n", stm->len, stm->size - stm->len);

            if (chatbot_cb->flags & FIRST_FLAG)
            {
                chatbot_unset_flags(FIRST_FLAG);
                printf("fade in\n");
                printf("DACVOLCON=0x%x DACDIGCON0=0x%x, DACDIGCON1=0x%x\n",
                       DACVOLCON,
                       DACDIGCON0,
                       DACDIGCON1);
            }
            // GPIOHCLR = BIT(4);
            // print_r(tmp, OPUS_ENC_FRAME_SIZE);

            os_mutex_take(&chatbot_cb->opus_mutex, OS_WAITING_FOREVER);
            opus_dec_frame((void *)chatbot_cb->recv_buf, packet, OPUS_ENC_FRAME_SIZE);
            os_mutex_release(&chatbot_cb->opus_mutex);
            aubuf0_gpdma_kick(chatbot_cb->recv_buf, 320, 1);
            aubuf0_gpdma_w4_done();

            // GPIOHSET = BIT(4);
        }
        delay_5ms(1);
    }
}

static void chatbot_recv_entry(void *p)
{
    noPollCtx * ctx = p;
    if (!ctx)
    {
        printf("%s err\n", __func__);
        return;
    }
    printf("%s ctx:%x\n", __func__, ctx);
    nopoll_conn_ref(chatbot_cb->conn);
    print_ctx(ctx);
    nopoll_loop_wait(ctx, 0);
    printf("conn:%d conn->ctx:%x\n", ctx->conn_num, chatbot_cb->conn->ctx);
    nopoll_conn_unref(chatbot_cb->conn);
    nopoll_ctx_unref(ctx);
    chatbot_set_flags(RECV_EXIT_DONE);
    printf("%s close\n", __func__);
    os_thread_exit();
}

static int chatbot_send_data(noPollConn *conn, const char *content, u32 length, bool is_text)
{
    int ret;
    os_mutex_take(&chatbot_cb->send_mutex, OS_WAITING_FOREVER);
    if (is_text)
    {
        ret = nopoll_conn_send_text(conn, content, length);
    }
    else
    {
        ret = nopoll_conn_send_binary(conn, content, length);
    }
    os_mutex_release(&chatbot_cb->send_mutex);
    if (ret < 0)
    {
        printf("%s ret=%d\n", __func__, ret);
    }
    return ret;
}


bool get_formatted_date_time(tm_t tm, char *buf, u32 size, bool urlstr);

tm_t rtc_clock_get_gmt(void);

static void on_message(noPollCtx  *ctx,
                       noPollConn *conn,
                       noPollMsg  *msg,
                       noPollPtr   user_data)
{
    noPollOpCode opcode = nopoll_msg_opcode(msg);
    const u8 *payload = nopoll_msg_get_payload(msg);
    size_t size = nopoll_msg_get_payload_size(msg);
    bool reasm_flag = false;

    if (!nopoll_msg_is_final(msg))
    {
        nopoll_msg_ref(msg);
        chatbot_cb->frag_msg = msg;
        return;
    }
    if (chatbot_cb->flags & EXITING_FLAG)
    {
        return;
    }
    if (chatbot_cb->frag_msg)
    {
        const u8 *old_payload = nopoll_msg_get_payload(chatbot_cb->frag_msg);
        size_t old_size = nopoll_msg_get_payload_size(chatbot_cb->frag_msg);

        void *ptr = malloc(old_size + size);
        if (!ptr)
        {
            nopoll_msg_unref(chatbot_cb->frag_msg);
            chatbot_cb->frag_msg = NULL;
            return;
        }
        opcode = nopoll_msg_opcode(chatbot_cb->frag_msg);
        memcpy(ptr, old_payload, old_size);
        memcpy(ptr + old_size, payload, size);
        payload = ptr;
        size += old_size;
        nopoll_msg_unref(chatbot_cb->frag_msg);
        chatbot_cb->frag_msg = NULL;
        reasm_flag = true;
    }

    // GPIOHCLR = BIT(7);
    if (opcode == NOPOLL_TEXT_FRAME)
    {
        const char *text = (const char *)payload;
        printf("text %s\n",  text);



        cJSON *json = cJSON_Parse(text);
        if (json)
        {
            cJSON *state = cJSON_GetObjectItemCaseSensitive(json, "state");
            if (cJSON_IsString(state))
            {
                const char *state_value = state->valuestring;
                if (strcmp(state_value, "sentence_start") == 0)
                {
                    chatbot_stop_mic();
                    chatbot_cb->event_cb(CHATEVT_TTS_PLAYING);
                }
                else if (strcmp(state_value, "sentence_end") == 0)
                {
                    // chatbot_start_mic();
                    chatbot_set_flags(SENTENCED_END_FLAG);
                }
            }
            cJSON_Delete(json);
        }
    }
    else if (opcode == NOPOLL_BINARY_FRAME)
    {
        const u8 *bin = payload;
        puts_stm_buf(&chatbot_cb->rx_fifo, (u8 *)bin, size);
    }
    if (reasm_flag)
    {
        free((void *)payload);
    }
    // GPIOHSET = BIT(7);
}

static noPollConn *chatbot_new_conn(void)
{
    noPollConn *conn;
    noPollCtx *ctx;

    chatbot_set_flags(FIRST_FLAG);

    /* initialize context */
    ctx = nopoll_ctx_new();
    printf("ctx->log_handler=%x\n", ctx->log_handler);

    /* create connection */
    conn = nopoll_conn_new(ctx, HOST_NAME, HOST_PORT, HOST_NAME, HOST_URI, NULL, NULL);
    if (!nopoll_conn_is_ok(conn))
    {
        printf("ERROR: Expected to find proper client connection status, but "
               "found error..\n");
        goto _fail;
    } /* end if */

    if (!nopoll_conn_wait_until_connection_ready(conn, 2))
    {
        printf("ERROR: websocket connection failed!\n");
        goto _fail;
    } /* end if */

    int ret = chatbot_send_data(conn, json_str, strlen(json_str), true);
    printf("send text %d\n", ret);
    return conn;
_fail:
    /* close the connection */
    if (conn)
    {
        nopoll_conn_close(conn);
    }

    /* release context */
    if (ctx)
    {
        nopoll_ctx_unref(ctx);
    }
    return NULL;
}

/// test

#if 0
#include "chatbot.h"

static void text_cb(const char *str)
{
    printf("%s", str);
}

static void event_cb(chatbot_event_t event)
{
    printf("\nchatbot event: %d\n", event);
}

static void test_enter(void)
{
    if (chatbot_init())
    {
        chatbot_set_text_callback(text_cb);
        chatbot_set_event_callback(event_cb);
        chatbot_start();
    }
}

static void test_exit(void)
{
    chatbot_deinit();
}
#endif
