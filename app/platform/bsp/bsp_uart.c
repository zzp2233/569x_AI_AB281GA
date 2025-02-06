#include "include.h"

#define TRACE_EN                0

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#define TRACE_R(...)            print_r(__VA_ARGS__)
#else
#define TRACE(...)
#define TRACE_R(...)
#endif

#define HSUART_DMA_BUF_LEN   64

static const int uart_gpio_map[] =
{
    0,
    IO_PB3,     IO_PA1, IO_PA3, IO_PE3, IO_PE5, IO_PE7, IO_PF5,     IO_MAX_NUM,
    IO_PB4,     IO_PA0, IO_PA2, IO_PE2, IO_PE4, IO_PE6, IO_MAX_NUM, IO_MAX_NUM,
    IO_PA1,     IO_PA7, IO_PB1, IO_PB3, IO_PE0, IO_PE3, IO_PF5,     IO_MAX_NUM,
    IO_PA1,     IO_PA7, IO_PB1, IO_PB3, IO_PE0, IO_PE3, IO_PF5,     IO_MAX_NUM,
    IO_MAX_NUM, IO_PA5, IO_PA7, IO_PB1, IO_PB7, IO_PE0, IO_PF0,     IO_PF2,
    IO_MAX_NUM, IO_PA4, IO_PA6, IO_PB0, IO_PB6, IO_PE1, IO_PF1,     IO_PF3,
};


static uart_isr_rece_callback_t uart_isr_rece_callback[UART_MAX - 1] = {NULL};


#if HSUART_MODULE_EN
static uint32_t hsuart_dma_rx_buf[HSUART_DMA_BUF_LEN];
static uint32_t hsuart_dma_tx_buf[HSUART_DMA_BUF_LEN];
#endif


//UART中断处理
#if (UART0_MODULE_EN | UART1_MODULE_EN | HSUART_MODULE_EN)
AT(.com_text.uart)
void uart_isr(void)
{
    uint32_t uart_read_data;
    uint8_t uart_buf;

#if UART0_MODULE_EN             //uart0_rx
    if(UART0CON & BIT(9))
    {
        UART0CPND |= BIT(9);

        uart_read_data = UART0DATA;
        uart_buf = uart_read_data & 0xff;
        if (uart_isr_rece_callback[UART0 - 1])
        {
            uart_isr_rece_callback[UART0 - 1]((uint8_t *)&uart_buf, sizeof(uart_buf));
        }
    }
#endif //UART0_MODULE_EN

#if UART1_MODULE_EN             //uart1_rx
    if(UART1CON & BIT(9))
    {
        UART1CPND |= BIT(9);

        uart_read_data = UART1DATA;
        uart_buf = uart_read_data & 0xff;
        if (uart_isr_rece_callback[UART1 - 1])
        {
            uart_isr_rece_callback[UART1 - 1]((uint8_t *)&uart_buf, sizeof(uart_buf));
        }
    }
#endif //HSUART_MODULE_EN

#if HSUART_MODULE_EN
    if(HSUT0CON & BIT(12))     //hsuart rx
    {
        HSUT0CPND = BIT(0);
        HSUT0CPND = BIT(12);
        if (uart_isr_rece_callback[HS_UART - 1])
        {
            uart_isr_rece_callback[HS_UART - 1]((uint8_t *)hsuart_dma_rx_buf, HSUT0FIFOCNT);
        }
        HSUT0FIFOCNT = 0;
        HSUT0CPND = BIT(15);
        HSUT0RXADR = DMA_ADR(&hsuart_dma_rx_buf);
        HSUT0RXCNT = 4 * HSUART_DMA_BUF_LEN;
    }
#endif //HSUART_MODULE_EN
}
#endif //(UART0_MODULE_EN | UART1_MODULE_EN | HSUART_MODULE_EN)

//获取uart_type
static uart_type bsp_uart_type_get(uart_pin pin, uart_crossbar_type crossbar)
{
    if (crossbar)
    {
        return crossbar;
    }

    if (UART_RTX_NONE == pin)
    {
        return UART_NONE;
    }
    uint8_t uart_type_tmp = (pin - 1) / 16 + 1;

    if (uart_type_tmp >= UART_MAX)
    {
        return UART_NONE;
    }

    return uart_type_tmp;
}


//获取uart map
static uint8_t bsp_uart_map_value(uart_pin pin)
{
    if (UART_RTX_NONE == pin)
    {
        return 0;
    }
    uint8_t map_value = (pin - 1) % 8;
    map_value += 1;
    map_value &= 0x0f;

    return map_value;
}

//串口初始化
bool bsp_uart_init(const uart_cfg_t *cfg, uint32_t baudrate, uart_isr_rece_callback_t rece_handle)
{
    if (NULL == cfg)
    {
        return false;
    }

    uint32_t baud_cfg;
    gpio_t gpio_reg;
    uint8_t io_num_rx = uart_gpio_map[cfg->rx];
    uint8_t io_num_tx = uart_gpio_map[cfg->tx];
    uart_type rx_uart_type = bsp_uart_type_get(cfg->rx, cfg->crossbar);
    uart_type tx_uart_type = bsp_uart_type_get(cfg->tx, cfg->crossbar);
    uart_type comm_uart_type = rx_uart_type | tx_uart_type;
    uint8_t map_value = 0;
    uint8_t reg_pos;
    bool use_oneline_mode = false;


    if (UART_NONE == comm_uart_type)
    {
        return false;
    }

    if (UART_NONE != rx_uart_type && tx_uart_type != rx_uart_type)
    {
        return false;
    }

    if (UART_NONE == rx_uart_type)
    {
        use_oneline_mode = true;
    }


    TRACE("%s: rx[%d] tx[%d] crossbar[%d] type[%d]\n", __func__, cfg->rx, cfg->tx, cfg->crossbar, comm_uart_type);
    if (cfg->crossbar)
    {
        if (cfg->rx > IO_PF6 || cfg->tx > IO_PF6)
        {
            return false;
        }
        io_num_rx = cfg->rx;
        io_num_tx = cfg->tx;
    }

    //RX
    if (!use_oneline_mode)
    {
        if (io_num_rx < IO_MAX_NUM)
        {
            //IO init
            bsp_gpio_cfg_init(&gpio_reg, io_num_rx);
            gpio_reg.sfr[GPIOxFEN] |= BIT(gpio_reg.num);
            gpio_reg.sfr[GPIOxDIR] |= BIT(gpio_reg.num);
            gpio_reg.sfr[GPIOxDE] |= BIT(gpio_reg.num);
            gpio_reg.sfr[GPIOxPU] |= BIT(gpio_reg.num);

            map_value = bsp_uart_map_value(cfg->rx);
        }
        else
        {
            //VUSB做串口
            PWRCON0 |= BIT(30);
            RTCCON &= ~BIT(20);
        }
    }
    else
    {
        if (UART0 == comm_uart_type)
        {
            map_value = bsp_uart_map_value(UART0_RX_NONE0);
        }
        else if (UART1 == comm_uart_type)
        {
            map_value = bsp_uart_map_value(UART1_RX_NONE);
        }
        else if(HS_UART == comm_uart_type)
        {
            map_value = bsp_uart_map_value(cfg->tx);
        }
    }

    if (cfg->crossbar)
    {
        if (cfg->crossbar == CROSSBAR_UART0)
        {
            FUNCMCON0 |= (0xf << 12);
            FUNCMCON0 |= (9 << 12);     // uart0要先映射G9/G10
        }
        else if (cfg->crossbar == CROSSBAR_UART1)
        {
            FUNCMCON0 |= (0xf << 28);
            FUNCMCON0 |= (9 << 28);
        }
        CH2_FUNI_SEL((cfg->rx - 1));          //G9为ch2
    }
    else
    {
        reg_pos = 8 + (comm_uart_type * 2 - 1) * 4;
        FUNCMCON0 |= (0xf << reg_pos);
        FUNCMCON0 |= (map_value << reg_pos);
    }

    //TX
    if (UART_NONE != tx_uart_type)
    {
        if(io_num_tx < IO_MAX_NUM)
        {
            //IO init
            bsp_gpio_cfg_init(&gpio_reg, io_num_tx);
            gpio_reg.sfr[GPIOxFEN] |= BIT(gpio_reg.num);
            gpio_reg.sfr[GPIOxDE] |= BIT(gpio_reg.num);
            gpio_reg.sfr[GPIOxPU] |= BIT(gpio_reg.num);
            if(!use_oneline_mode)
            {
                gpio_reg.sfr[GPIOxDIR] &= ~BIT(gpio_reg.num);
            }
            else
            {
                gpio_reg.sfr[GPIOxDIR] |= BIT(gpio_reg.num);
            }

            if (cfg->crossbar)
            {
                if (cfg->crossbar == CROSSBAR_UART0)
                {
                    CH3_FUNO_SEL(FO_UR0TX); //CH3为输出通道的最高级，2/1/0依次下降
                }
                else if (cfg->crossbar == CROSSBAR_UART1)
                {
                    CH3_FUNO_SEL(FO_UR1TX);
                }

                FUNCOUTMCON = (cfg->tx << 24);    //选择要映射到tx的引脚
            }
            else
            {
                //mapping uart
                map_value = bsp_uart_map_value(cfg->tx);
                u8 reg_pos = 8 + (tx_uart_type - 1) * 2 * 4;
                FUNCMCON0 |= (0xf << reg_pos);
                FUNCMCON0 |= (map_value << reg_pos);
            }
        }
        else
        {
            //VUSB做串口
            PWRCON0 |= BIT(30);
            RTCCON &= ~BIT(20);
        }
    }

    if (UART0 == comm_uart_type)
    {
        CLKCON0 &= ~BIT(20);      //uart_inc select xosc26m_clk
        CLKCON0 |= BIT(21);
        CLKGAT0 |= BIT(10);        //enable uart0 clk
        UART0CON = 0;

        UART0CON |= BIT(5);       //set baud
        baud_cfg = (XOSC_CLK_HZ / baudrate) - 1;   //baud_cfg=(串口时钟/波特率)-1; 四舍五入
        UART0BAUD = (baud_cfg << 16) | baud_cfg;

        UART0CON |= (BIT(0) | BIT(2) | BIT(7));    //使能uart,接收中断
        if(use_oneline_mode)                       //use oneline
        {
            UART0CON |= BIT(6);
        }
    }
    else if (UART1 == comm_uart_type)
    {
        CLKCON0 &= ~BIT(22);       //uart_inc select xosc26m_clk
        CLKCON0 |= BIT(23);
        CLKGAT0 |= BIT(21);        //enable uart1 clk

        UART1CON = 0;
        UART1CON |= BIT(5);        //set baud
        baud_cfg = (XOSC_CLK_HZ / baudrate) - 1;   //baud_cfg=(串口时钟/波特率)-1; 四舍五入
        UART1BAUD = (baud_cfg << 16) | baud_cfg;

        UART1CON |= (BIT(0) | BIT(2) | BIT(7));         //使能uart,接收中断
        if(use_oneline_mode)                            //use oneline
        {
            UART1CON |= BIT(6);
        }
    }
    else if (HS_UART == comm_uart_type)
    {
        CLKGAT0 |= BIT(11);             //enable hsuart clk
        delay_us(10);
        CLKCON0 &= ~(7 << 17);          //uart_inc select xosc26m_clk

        baud_cfg = ((XOSC_CLK_HZ + (baudrate / 2)) / baudrate) - 1; //set baudrate
        HSUT0BAUD = (baud_cfg << 16) | baud_cfg;

        HSUT0TMRCNT = 32;
        HSUT0CON = (BIT(0) | BIT(1) | BIT(2) | BIT(4) | BIT(7) | BIT(10));   //enable hsuart dma

        if(use_oneline_mode)
        {
            HSUT0CON |= BIT(18);
        }

        HSUT0CPND = (BIT(0) | BIT(1) | BIT(12) | BIT(15));

#if HSUART_MODULE_EN
        HSUT0RXADR = DMA_ADR(hsuart_dma_rx_buf);
        HSUT0RXCNT = 4 * HSUART_DMA_BUF_LEN;
#endif
    }

    uart_isr_rece_callback[comm_uart_type - 1] = rece_handle;   //数据接收回调

    if(HS_UART == comm_uart_type)
    {
#if HSUART_MODULE_EN
        register_isr(IRQ_HSUART_VECTOR, uart_isr);              // 注册串口中断
#endif
        PICPR &= ~BIT(IRQ_HSUART_VECTOR);                       //配置中断优先级
        PICEN |= BIT(IRQ_HSUART_VECTOR);

    }
    else
    {
#if (UART0_MODULE_EN | UART1_MODULE_EN)
        register_isr(IRQ_UART_VECTOR, uart_isr);                // 注册串口中断
#endif
        PICPR &= ~BIT(IRQ_UART_VECTOR);                         //配置中断优先级
        PICEN |= BIT(IRQ_UART_VECTOR);
    }

    TRACE("rx FUNCMCON0[%x] FUNCINCON[%x],tx FUNCOUTMCON[%x] FUNCOUTCON[%x]\n", FUNCMCON0, FUNCINCON, FUNCOUTMCON, FUNCOUTCON);
    TRACE("isr PICPR[%x] PICPR[%x]\n", PICPR, PICEN);
    TRACE("uart CLKCON0[%x] CLKGAT0[%x] UART0BAUD[%x] UART0CON[%x] UART1BAUD[%x] UART1CON[%x]\n", CLKCON0, CLKGAT0, UART0BAUD, UART0CON, UART1BAUD, UART1CON);
    return true;
}


//设置波特率
void bsp_uart_baudrate_set(const uart_cfg_t *cfg, uint32_t rate)
{
    uint32_t baud_cfg;
    uart_type rx_uart_type = bsp_uart_type_get(cfg->rx, cfg->crossbar);
    uart_type tx_uart_type = bsp_uart_type_get(cfg->tx, cfg->crossbar);
    uart_type comm_uart_type = rx_uart_type | tx_uart_type;

    PICEN &= ~BIT(IRQ_UART_VECTOR);

    if (UART0 == comm_uart_type)
    {
        baud_cfg = (XOSC_CLK_HZ / rate) - 1;        //baud_cfg=(串口时钟/波特率)-1; 四舍五入
        UART0BAUD = (baud_cfg << 16) | baud_cfg;

    }
    else if (UART1 == comm_uart_type)
    {
        baud_cfg = (XOSC_CLK_HZ / rate) - 1;
        UART1BAUD = (baud_cfg << 16) | baud_cfg;

    }
    else if (HS_UART == comm_uart_type)
    {
        baud_cfg = ((XOSC_CLK_HZ + (rate / 2)) / rate) - 1;
        HSUT0BAUD = (baud_cfg << 16) | baud_cfg;
    }

    PICEN |= BIT(IRQ_UART_VECTOR);
}


void bsp_uart_tx_wait(const char *str)
{
    u32 tx_ticks = tick_get();
    while (!(UART1CON & BIT(8)))
    {
        if  (tick_check_expire(tx_ticks, 500))
        {
            printf(str);
            break;
        }
    };
}

//发送单字节
void bsp_uart_send_byte(const uart_cfg_t *cfg, char ch)
{
    uart_type rx_uart_type = bsp_uart_type_get(cfg->rx, cfg->crossbar);
    uart_type tx_uart_type = bsp_uart_type_get(cfg->tx, cfg->crossbar);
    uart_type comm_uart_type = rx_uart_type | tx_uart_type;

    if (UART0 == comm_uart_type)
    {
        bsp_uart_tx_wait("uart0 tx timeout\n");
        UART0DATA = ch;

    }
    else if (UART1 == comm_uart_type)
    {
        bsp_uart_tx_wait("uart1 tx timeout\n");
        UART1DATA = ch;
    }
}


//发送字符串
void bsp_uart_send_str(const uart_cfg_t *cfg, char *str)
{
    uart_type rx_uart_type = bsp_uart_type_get(cfg->rx, cfg->crossbar);
    uart_type tx_uart_type = bsp_uart_type_get(cfg->tx, cfg->crossbar);
    uart_type comm_uart_type = rx_uart_type | tx_uart_type;

    if (UART0 == comm_uart_type)
    {
        while(*str)
        {
            while(!(UART0CON & BIT(8)));
            UART0DATA = *str;
            str ++;
        }
    }
    else if(UART1 == comm_uart_type)
    {
        while(*str)
        {
            while(!(UART1CON & BIT(8)));
            UART1DATA = *str;
            str ++;
        }
    }
}

//hsuart 发送
#if HSUART_MODULE_EN
AT(.com_text.uart)
void hsuart_send(uint8_t *buf, uint16_t len)
{
    if(len > (4 * HSUART_DMA_BUF_LEN))
    {
        len = 4 * HSUART_DMA_BUF_LEN;
    }
    HSUT0CPND = BIT(1);
    HSUT0CPND = BIT(13);
    memcpy(hsuart_dma_tx_buf, buf, len);
    HSUT0TXADR = DMA_ADR(hsuart_dma_tx_buf);
    HSUT0TXCNT = len;
}
#else
void hsuart_send(uint8_t *buf, uint16_t len)
{

}
#endif //HSUART_MODULE_EN
