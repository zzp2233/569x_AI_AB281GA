#ifndef _BSP_UART_H
#define _BSP_UART_H

//由于串口中断占用com区，以及高速串口使用dma缓存，根据需要开启相应模块
#define UART0_MODULE_EN   0
#define UART1_MODULE_EN   1
#define HSUART_MODULE_EN  0

typedef enum
{
    UART_RTX_NONE = 0,

    UART0_TX_PB3,         //uart0 TX
    UART0_TX_PA1,
    UART0_TX_PE7_G3,
    UART0_TX_PE3,
    UART0_TX_PE5,
    UART0_TX_PE7_G6,
    UART0_TX_PF5,
    UART0_TX_VUSB,

    UART0_RX_PB4,         //uart0 RX
    UART0_RX_PA0,
    UART0_RX_PG3,
    UART0_RX_PE2,
    UART0_RX_PE4,
    UART0_RX_PE6,
    UART0_RX_NONE0,
    UART0_RX_NONE1,

    HSUART_TX_PA1,        //hs_uart TX
    HSUART_TX_PA7,
    HSUART_TX_PB1,
    HSUART_TX_PB3,
    HSUART_TX_PE0,
    HSUART_TX_PE7,
    HSUART_TX_PF5,
    HSUART_TX_VUSB,

    HSUART_RX_PA1,        //hs_uart RX
    HSUART_RX_PA7,
    HSUART_RX_PB1,
    HSUART_RX_PB3,
    HSUART_RX_PE0,
    HSUART_RX_PE7,
    HSUART_RX_PF5,
    HSUART_RX_VUSB,

    UART1_TX_VUSB,        //uart1 TX
    UART1_TX_PA5,
    UART1_TX_PA7,
    UART1_TX_PB1,
    UART1_TX_PB7,
    UART1_TX_PE0,
    UART1_TX_PF0,
    UART1_TX_PF2,

    UART1_RX_NONE,        //uart1 RX
    UART1_RX_PA4,
    UART1_RX_PA6,
    UART1_RX_PB0,
    UART1_RX_PB6,
    UART1_RX_PE1,
    UART1_RX_PF1,
    UART1_RX_PF3,

} uart_pin;

typedef enum
{
    UART_NONE,
    UART0,
    HS_UART,
    UART1,
    UART_MAX,
} uart_type;

typedef enum
{
    CROSSBAR_NONE = 0,
    CROSSBAR_UART0,
    CROSSBAR_HUART,
    CROSSBAR_UART1,
} uart_crossbar_type;

typedef void (*uart_isr_rece_callback_t)(uint8_t *buf, uint16_t len);

typedef struct _uart_cfg_t_
{
    uart_pin                 tx;               //指定tx pin
    uart_pin                 rx;               //指定rx pin, 未指定时默认使用单线模式(tx可做rx)
    uart_crossbar_type       crossbar;         //任意映射pin开关
} uart_cfg_t;


/**
 * @brief 串口初始化
 * @param[in] cfg         对应的pin配置
 * @param[in] baudrate    波特率
 * @param[in] rece_handle 接收中断处理回调，需指定.com_text.uart
 *
 * @return  返回是否成功
 **/
bool bsp_uart_init(const uart_cfg_t *cfg, uint32_t baudrate, uart_isr_rece_callback_t rece_handle);

/**
 * @brief 设置波特率
 * @param[in] cfg       对应的pin配置
 * @param[in] baudrate  波特率
 *
 * @return  无
 **/
void bsp_uart_baudrate_set(const uart_cfg_t *cfg, uint32_t baudrate);

/**
 * @brief 发送单字节
 * @param[in] cfg  uart0 或 uart1 对应的pin配置
 * @param[in] ch   需发送的字符
 *
 * @return  无
 **/
void bsp_uart_send_byte(const uart_cfg_t *cfg, char ch);

/**
 * @brief 发送字符串
 * @param[in] cfg   uart0 或 uart1 对应的pin配置
 * @param[in] str   需发送的字符串
 *
 * @return  无
 **/
void bsp_uart_send_str(const uart_cfg_t *cfg, char *str);


/**
 * @brief 高速串口发送
 * @param[in] buf   发送数据Buf
 * @param[in] len   发送数据长度
 *
 * @return  无
 **/
void hsuart_send(uint8_t *buf, uint16_t len);


#if UART_EXAMPLE_EN
/**
 * @brief 串口收发测试实例
 * @param 无
 *
 * @return 无
 **/
void bsp_uart_example_process(void);
#endif

#endif
