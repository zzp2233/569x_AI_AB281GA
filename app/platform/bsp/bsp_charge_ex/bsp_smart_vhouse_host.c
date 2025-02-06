#include "include.h"

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#if SMART_VHOUSE_HOST_EN

const uart_cfg_t uart1_cfg =
{
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_SY8827)
    .tx = UART1_TX_PB1,
#else
    .rx = UART1_RX_PB6,
    .tx = UART1_TX_PB7,
#endif
};

uart_cb_t uart_cb AT(.app.uart);
static u8 uart_rx_buf[64];
static u8 uart_rx_data = 0;
static u8 start_uart_rx_data_flg = false;
static u8 finish_uart_rx_data_flg = false;

AT(.text.func.idle.vhouse)
void smart_vhouse_tx_dis(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, SMART_VHOUSE_HOST_TX);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);
        gpio.sfr[GPIOxPU]  &= ~BIT(gpio.num);
        gpio.sfr[GPIOxPD]  |= BIT(gpio.num);
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
        gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);
    }
    UART1CON &= ~BIT(0);
}

AT(.text.func.idle.vhouse)
void smart_vhouse_tx_en(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, SMART_VHOUSE_HOST_TX);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxFEN] |= BIT(gpio.num);
        gpio.sfr[GPIOxPU]  |= BIT(gpio.num);
        gpio.sfr[GPIOxPD]  &= ~BIT(gpio.num);
        if (uart1_cfg.rx)
        {
            gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);   //控制IO的方向:  0为输出, 1为输入.
        }
        else        //单线uart
        {
            gpio.sfr[GPIOxDIR] |= BIT(gpio.num);
        }
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);
    }
    UART1CON |= BIT(0);
}

bool smart_vhouse_tx_is_en(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, SMART_VHOUSE_HOST_TX);
    if (gpio.sfr)
    {
        if (gpio.sfr[GPIOxDE] & BIT(gpio.num))
        {
            return true;
        }
    }
    return false;
}

void smart_vhouse_rx_dis(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, SMART_VHOUSE_HOST_RX);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);    //切模拟
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
        gpio.sfr[GPIOxPU] &= ~BIT(gpio.num);    //上拉, 防止通信电平不一致漏电
#endif
    }
}

void smart_vhouse_rx_en(void)
{
    gpio_t gpio;
    bsp_gpio_cfg_init(&gpio, SMART_VHOUSE_HOST_RX);
    if (gpio.sfr)
    {
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);    //切数字
#if (CHARGE_EX_IC_SELECT == CHARGE_IC_LX3318BS)
        gpio.sfr[GPIOxPU] |= BIT(gpio.num);    //上拉
#endif
    }
}

void bsp_smart_vhouse_uart_recv_ear_clear(void)
{
    start_uart_rx_data_flg = false;
    finish_uart_rx_data_flg = false;
    uart_rx_data = 0;
}


void bsp_smart_vhouse_uart_set_recv_ear_respon(void)
{
    start_uart_rx_data_flg = true;
    finish_uart_rx_data_flg = false;
}

u8 bsp_smart_vhouse_uart_is_finish_get_data(void)
{
    return finish_uart_rx_data_flg;
}

u8 bsp_smart_vhouse_uart_get_buf_data(void)
{
    return uart_rx_data;
}

AT(.com_text.uart)
u8 bsp_smart_vhouse_uart_get(u8 *ch)
{
    if (uart_cb.r_cnt != uart_cb.w_cnt)
    {
        *ch = uart_cb.buf[uart_cb.r_cnt & 0x3f];
        uart_cb.r_cnt++;
        return 1;
    }
    return 0;
}

AT(.com_text.uart)
void bsp_charge_vhouse_uart_isr(uint8_t *buf, uint16_t len)
{
//    print_r(buf, len);

    if (tick_check_expire(uart_cb.ticks, 300))      //超时处理数据, 直接清空buf
    {
        uart_cb.w_cnt = uart_cb.r_cnt = 0;
    }
    uart_cb.ticks = tick_get();
    uart_cb.buf[uart_cb.w_cnt & 0x3f] = *buf;       //len一般都是1, 直接等于
    uart_cb.w_cnt++;

    if (start_uart_rx_data_flg == true && finish_uart_rx_data_flg == false)
    {
        finish_uart_rx_data_flg = true;
        uart_rx_data = *buf;
    }
}

void bsp_charge_vhouse_uart_send(char ch)
{
//    printf("send_byte:0x%x\n", ch);
    bsp_uart_send_byte(&uart1_cfg, ch);
}

void bsp_charge_vhouse_send_str(char *str)
{
    bsp_uart_send_str(&uart1_cfg, str);
}

AT(.text.func.idle)
void bsp_smart_vhouse_host_process(void)
{
    bsp_smart_vhouse_host_cmd_process();
    bsp_smart_vhouse_host_parser_process();
}

void bsp_smart_vhouse_host_init(void)
{
    u32 baud_rate = 9600;
#if SMART_VHOUSE_HOST_HS_EN
    baud_rate = 1500000;
#endif

    bsp_uart_init(&uart1_cfg, baud_rate, bsp_charge_vhouse_uart_isr);
    memset(&uart_cb, 0, sizeof(uart_cb));
    uart_cb.buf = uart_rx_buf;

    bsp_smart_vhouse_host_cmd_init();
}

#else
AT(.text.func.idle.vhouse)
void smart_vhouse_tx_dis(void) {}

AT(.text.func.idle.vhouse)
void smart_vhouse_tx_en(void) {}
#endif // SMART_VHOUSE_HOST_EN
