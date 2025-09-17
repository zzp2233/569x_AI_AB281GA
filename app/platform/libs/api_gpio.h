/*
 * @brief:
 * @LastEditors: ljf
 * @LastEditTime: 2025-09-17 14:51:51
 * @FilePath: \569x_AI\app\platform\libs\api_gpio.h
 * @Date: 2025-09-16 15:35:01
 */
#ifndef _BSP_GPIO_H
#define _BSP_GPIO_H

enum
{
    GPIOxSET    =   0,    // GPIO输出设置寄存器索引，用于设置引脚输出高电平
    GPIOxCLR,             // GPIO输出清除寄存器索引，用于设置引脚输出低电平
    GPIOx,                // GPIO数据寄存器索引，用于读取引脚当前电平状态
    GPIOxDIR,             // GPIO方向寄存器索引，控制引脚输入/输出方向
    GPIOxDE,              // GPIO数字功能使能寄存器索引，使能引脚数字I/O功能
    GPIOxFEN,             // GPIO上拉电阻使能寄存器索引，控制内部上拉电阻
    GPIOxDRV,             // GPIO驱动强度寄存器索引，控制输出驱动能力
    GPIOxPU,              // GPIO上拉寄存器索引，另一种上拉控制方式
    GPIOxPD,              // GPIO下拉寄存器索引，控制内部下拉电阻
    GPIOxPU200K,          // GPIO 200KΩ上拉寄存器索引，弱上拉电阻控制
    GPIOxPD200K,          // GPIO 200KΩ下拉寄存器索引，弱下拉电阻控制
    GPIOxPU300,           // GPIO 300Ω强上拉寄存器索引，强上拉电阻控制
    GPIOxPD300,           // GPIO 300Ω强下拉寄存器索引，强下拉电阻控制
};

typedef struct
{
    psfr_t sfr;           // GPIO特殊功能寄存器地址指针，指向该GPIO端口的寄存器组
    u8 num;               // GPIO引脚号，表示在该端口中的具体引脚位置(0-7)
    u8 type;              // GPIO类型标志：type=1表示没有300Ω强上下拉电阻，type=0表示普通IO有内部300Ω上下拉电阻
    u16 pin;              // GPIO引脚标识符，用于唯一标识该GPIO引脚
} gpio_t;

#define bsp_gpio_cfg_init(x, y)         gpio_cfg_init(x, y)

void gpio_cfg_init(gpio_t *g, u8 io_num);                   //根据GPIO number初始化GPIO结构体
u8 get_adc_gpio_num(u8 adc_ch);
void adcch_io_pu10k_enable(u8 adc_ch);

///port wakeup and interrupt

#define PORT_INT0_VECTOR            0
#define PORT_INT1_VECTOR            1
#define PORT_INT2_VECTOR            2
#define PORT_INT3_VECTOR            3
#define PORT_INT4_VECTOR            4
#define PORT_INT5_VECTOR            5
#define PORT_INT6_VECTOR            6
#define PORT_INT7_VECTOR            7

void port_var_init(void);
void port_irq_register(int irq_num, isr_t isr);
void port_irq_free(int irq_num);
u8 port_wakeup_get_status(void);
bool port_wakeup_init(u8 io_num, u8 edge, u8 pupd_sel);     //参数edge: 0->上升沿, 1->c,  参数pupd_sel: 0->不开内部上拉, 1->开内部上拉, 2->开内部下拉
bool port_wakeup_exit(u8 io_num);
void wko_wakeup_init(u8 edge);
void wko_wakeup_exit(void);
bool port_wko_is_wakeup(void);
//上升沿 ： 中断7  下降沿：中断6
void port_wakeup_all_init(u8 io_num, u8 edge, u8 pupd_sel);

#endif // _BSP_GPIO_H
