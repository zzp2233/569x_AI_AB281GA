#ifndef _BSP_UART_H
#define _BSP_UART_H

//���ڴ����ж�ռ��com�����Լ����ٴ���ʹ��dma���棬������Ҫ������Ӧģ��
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
    uart_pin                 tx;               //ָ��tx pin
    uart_pin                 rx;               //ָ��rx pin, δָ��ʱĬ��ʹ�õ���ģʽ(tx����rx)
    uart_crossbar_type       crossbar;         //����ӳ��pin����
} uart_cfg_t;


/**
 * @brief ���ڳ�ʼ��
 * @param[in] cfg         ��Ӧ��pin����
 * @param[in] baudrate    ������
 * @param[in] rece_handle �����жϴ���ص�����ָ��.com_text.uart
 *
 * @return  �����Ƿ�ɹ�
 **/
bool bsp_uart_init(const uart_cfg_t *cfg, uint32_t baudrate, uart_isr_rece_callback_t rece_handle);

/**
 * @brief ���ò�����
 * @param[in] cfg       ��Ӧ��pin����
 * @param[in] baudrate  ������
 *
 * @return  ��
 **/
void bsp_uart_baudrate_set(const uart_cfg_t *cfg, uint32_t baudrate);

/**
 * @brief ���͵��ֽ�
 * @param[in] cfg  uart0 �� uart1 ��Ӧ��pin����
 * @param[in] ch   �跢�͵��ַ�
 *
 * @return  ��
 **/
void bsp_uart_send_byte(const uart_cfg_t *cfg, char ch);

/**
 * @brief �����ַ���
 * @param[in] cfg   uart0 �� uart1 ��Ӧ��pin����
 * @param[in] str   �跢�͵��ַ���
 *
 * @return  ��
 **/
void bsp_uart_send_str(const uart_cfg_t *cfg, char *str);


/**
 * @brief ���ٴ��ڷ���
 * @param[in] buf   ��������Buf
 * @param[in] len   �������ݳ���
 *
 * @return  ��
 **/
void hsuart_send(uint8_t *buf, uint16_t len);


#if UART_EXAMPLE_EN
/**
 * @brief �����շ�����ʵ��
 * @param ��
 *
 * @return ��
 **/
void bsp_uart_example_process(void);
#endif

#endif
