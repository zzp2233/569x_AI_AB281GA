#ifndef __BSP_CHARGE_VHOUSE_HOST_H__
#define __BSP_CHARGE_VHOUSE_HOST_H__

#include "bsp_smart_vhouse_host_cmd.h"

#define SMART_BOX_HEADER            0xAA55
#define SMART_BOX_DISTINGUISH       0xFF
#define SMART_BOX_HEAD_LEN          5

#define VH_DATA_LEN                     40
#define VHOUSE_DISTINGUISH              0xFF           //充电仓产商识别码

#define LEFT_CHANNEL_USER               0x11
#define RIGHT_CHANNEL_USER              0x22
#define ALL_CHANNEL_USER                0x55

#define CMD_REPEAT_CNT                  2

enum
{
    VHOUSE_CMD_NULL = 0x0,
    VHOUSE_CMD_GET_VBAT,                //获取电池电量，也可作为开始标志
    VHOUSE_CMD_PAIR,                    //充电仓发起的配对消息
    VHOUSE_CMD_GET_TWS_BTADDR,          //获取对耳的地址信息
    VHOUSE_CMD_CLEAR_PAIR,              //清除配对消息
    VHOUSE_CMD_PWROFF,                  //关机消息
    VHOUSE_CMD_ERR,                     //接收消息错误响应
    VHOUSE_CMD_SUCCESS,                 //成功响应
    VHOUSE_CMD_CLOSE_WINDOW,            //关盖
    VHOUSE_CMD_OPEN_WINDOW,             //开盖
    VHOUSE_CMD_CLOSE_WIN_GET_VBAT,      //关盖获取电量包

    VHOUSE_CMD_ENTER_DUT  = 0x10,
    VHOUSE_CMD_TWS_CLR    = 0x1D,
    VHOUSE_CMD_NOR_CLR    = 0x1E,
    VHOUSE_CMD_EAR_PWROFF = 0x1F,

    /// IODM 使用范围 11起 ， 29 结束，请勿在此范围添加

    VHOUSE_CMD_ENABLE_POPUP = 0x80,     //开关广播功能控制

    VHOUSE_CMD_CUSTOM_RESV1=0xE0,       //客户保留指令
    VHOUSE_CMD_GET_EAR_ADDR,            //获取耳机地址
    VHOUSE_CMD_CUSTOM_RESV_END=0xEF,

    VHOUSE_CMD_SYS_RST=0xFF,             //系统复位指令

};

typedef struct
{
    u16 header;
    u8  distinguish;                    //充电仓产商识别码
    u8  cmd;
    u8  length;
    u8  buf[VH_DATA_LEN];
    u8  checksum;
    u8  cnt;
    u8  crc;
} vh_packet_t;

typedef struct
{
    volatile u8 w_cnt;
    volatile u8 r_cnt;
    u8 resv0;
    u8 resv1;
    u8 *buf;
    u32 ticks;
} uart_cb_t;

extern uart_cb_t uart_cb;

/**
 * @brief 获取smart_vhouse串口字节数据
 * @param[in] ch: 获取的字符
 * @return 是否获取成功
 **/
u8 bsp_smart_vhouse_uart_get(u8 *ch);

/**
 * @brief smart_vhouse串口发送字节数据
 * @param[in] ch: 获取的字符
 * @return 是否获取成功
 **/
void bsp_charge_vhouse_uart_send(char ch);
void smart_vhouse_tx_dis(void);
void smart_vhouse_tx_en(void);
bool smart_vhouse_tx_is_en(void);
void smart_vhouse_rx_dis(void);
void smart_vhouse_rx_en(void);
void bsp_smart_vhouse_host_process(void);
void bsp_smart_vhouse_host_init(void);

void bsp_smart_vhouse_uart_recv_ear_clear(void);

void bsp_smart_vhouse_uart_set_recv_ear_respon(void);
u8 bsp_smart_vhouse_uart_is_finish_get_data(void);
u8 bsp_smart_vhouse_uart_get_buf_data(void);

///cmd
/**
 * @brief 查询命令队列是否已存在目标命令
 * @param[in] cmd: 目标命令
 **/
bool vhouse_cmd_inqueue(u8 cmd);

///parser
void bsp_smart_vhouse_host_parser_process(void);
#endif /* __BSP_CHARGE_VHOUSE_HOST_H__*/
