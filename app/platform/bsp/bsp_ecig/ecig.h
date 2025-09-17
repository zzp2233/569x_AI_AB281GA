#ifndef _ECIG_H
#define _ECIG_H

#define ECIG_VBG_VOLTAGE             sys_trim.vbg_volt
#define ECIG_VBG_VOLTAGE_MULTIPLE    10                 //放大倍数

typedef enum
{
    IDLE = 0,           // 空闲状态：设备待机，未进行任何操作
    SMOKING,            // 吸烟状态：用户正在抽吸，发热丝正常工作
    SHORT_CIRCUIT,      // 短路状态：检测到发热丝短路故障
    OPEN_CIRCUIT,       // 开路状态：检测到发热丝开路或烟弹拔出
    LOW_POWER,          // 低电量状态：电池电压过低，停止加热保护
    IN_DEVICE,          // 插入状态：检测到烟弹插入设备
    SMOKE_TIMEOUT,      // 超时状态：单次抽吸时间超过8秒限制
} SMOKE_STA;

typedef enum
{
    ECIG_PWM1_IO = 0,   // PWM1输出引脚索引：控制第一路发热丝
    ECIG_PWM2_IO,       // PWM2输出引脚索引：控制第二路发热丝
    ECIG_MIC_IO,        // 咪头检测引脚索引：检测用户吸烟动作
    ECIG_VEN_IO,        // VEN电源控制引脚索引：控制发热丝电路供电
    ECIG_DET1_IO,       // 第一路烟弹检测引脚索引：检测烟弹插拔
    ECIG_DET2_IO,       // 第二路烟弹检测引脚索引：检测烟弹插拔
    ECIG_ADGND_IO,      // ADC接地引脚索引：提供ADC采样基准
    ECIG_MAX_IO,        // 最大IO数量：用于数组大小定义
} ECIG_IO;


#define ECIG_PWM1_ON()     ecig_pwm_set(1, 1)    // 开启第一路PWM输出  
#define ECIG_PWM1_OFF()    ecig_pwm_set(1, 0)    // 关闭第一路PWM输出  
#define ECIG_PWM2_ON()     ecig_pwm_set(2, 1)    // 开启第二路PWM输出  
#define ECIG_PWM2_OFF()    ecig_pwm_set(2, 0)    // 关闭第二路PWM输出  
#define ECIG_VEN_ON()      ecig_ven_set(1, 1)    // 开启VEN电源供电  
#define ECIG_VEN_OFF()     ecig_ven_set(1, 0)    // 关闭VEN电源供电



void ecig_hot_detect_init(void);
void ecig_hot_proc(void);
void ecig_hot_proc_vddio(void);
void ecig_res_proc(void);
void ecig_vbat_proc(void);

u32 saradc_vbat_get_calc_value(u32 vbat2, u32 bg, u32 vrtc_val, u32 vrtc_first);
u32 ecig_vbat_get(void);
void caculate_res(void);
void caculate_res2(void);
void ecig_adgnd_io_set_gnd(void);
void ecig_ven_io_init(void);
/**
  * @brief  电子烟配置结构体
  * @note   存储电子烟系统的所有静态硬件配置参数和功能设置
  *         包括GPIO引脚分配、ADC通道配置、安全保护参数、功率控制参数等
  *         在系统初始化时被填充，作为整个电子烟系统的配置基础
  */
//烟控配置
typedef struct                                                       // 定义电子烟配置结构体类型
{
    u8 io_pwm1;                                                                   // PWM1输出引脚号，用于控制第一路发热丝的PWM信号
    u8 io_mic;                                                                    // 咪头检测引脚号，用于检测用户吸烟动作
    u8 io_adgnd;                                                                  // ADC接地引脚号，为ADC采样提供稳定的接地基准

    u8 adc1_ch;                                                                   // 第一路ADC通道号，用于检测第一路发热丝电压
    u8 adc_res1_ch;                                                               // 第一路电阻检测ADC通道号，用于测量第一路发热丝电阻

    u8 adc2_en;                                                                   // ADC2使能标志，1表示启用双路ADC检测，0表示单路检测
    u8 io_pwm2;                                                                   // PWM2输出引脚号，用于控制第二路发热丝的PWM信号（双发模式）

    u8 io_ven;                                                                    // VEN电源控制引脚号，用于控制整个发热丝电路的供电开关

    u8 adc2_ch;                                                                   // 第二路ADC通道号，用于检测第二路发热丝电压
    u8 adc_res2_ch;                                                               // 第二路电阻检测ADC通道号，用于测量第二路发热丝电阻

    u16 res_diff;                     //阻抗检测分压电阻值（Ω，8192倍）              // 分压电阻值，用于电阻计算，以8192倍定点数格式存储
    u16 res_wire;                     //无ADC2通路时电阻丝为固定阻值（Ω，8192倍）     // 单路模式下的固定电阻值，以8192倍定点数格式存储
    u16 res_wire2;                                                                // 第二路发热丝的电阻值，以8192倍定点数格式存储

    u8 io_hot_det1;                   //烟弹检测io                                 // 第一路烟弹插拔检测引脚号，用于检测烟弹是否插入
    u8 io_hot_det2;                   //烟弹检测io                                 // 第二路烟弹插拔检测引脚号，用于检测烟弹是否插入
    u8 io_hot_mode;                   //检测到是否开检测模式                      // 烟弹检测模式使能标志，1表示启用插拔检测功能
    bool hot_det_flag;                //烟弹检测标志                             // 烟弹检测状态标志，true表示检测到烟弹插入

    u8 aim_power;                     //目标功率（W）                            // 目标功率值，单位为瓦特，用于功率控制算法
    u16 aim_voltage;                  //目标电压                                   // 目标电压值，单位为毫伏，由功率值转换而来用于实际控制

    u8 smoke_power;                   //抽烟功率（W）                            // 抽烟时的实际功率值，单位为瓦特

    u8 heat_time_max;                 //单次加热最长时间（s）                     // 单次加热的最大允许时间，单位为秒，通常设置为8秒

    u8 short_res_prop;                //短路保护，电热丝阻值和MOS内阻（100mΩ）的最小比例，阻值相近乘10提高精度，10--0.1Ω视作短路 // 短路保护阈值，当电阻比值小于此值时判定为短路
    u16 open_res_prop;                //开路保护，电热丝阻值和MOS内阻（100mΩ）的最大比例，阻值相近乘10提高精度，200--2Ω视作开路（无ADC2通路时有效） // 开路保护阈值，当电阻比值大于此值时判定为开路

    u8  short_res_proportion;         //电阻短路时adc比例                           // ADC采样值的短路检测比例阈值，用于基于ADC比值的短路判断
    u8  open_res_proportion;          //电阻断路时adc比例                           // ADC采样值的开路检测比例阈值，用于基于ADC比值的开路判断

    bool smoke_res_swich;             //一档时候，吸一口换一路                       // 单发模式下的电阻切换标志，true表示使用第一路，false表示使用第二路
    u8 smoke_position_swich;          //一档:0x00二档:0x01三档:0x02              // 档位模式选择，0表示单发模式，1表示双发模式，2表示三档模式

    gpio_t ecig_gpio[ECIG_MAX_IO];                                                // GPIO配置数组，存储所有电子烟相关GPIO的配置信息

    u32 smoking_count;                //抽烟口数                                   // 抽烟次数统计，记录用户总共抽烟的口数
    bool smoke_start_or_not;          //是否抽吸                                   // 抽吸状态标志，true表示正在抽吸，false表示未抽吸
} ecig_cfg_t;                                                                     // 电子烟配置结构体类型定义结束
extern ecig_cfg_t ecig_cfg;                                                       // 声明全局电子烟配置变量，在其他文件中定义

/**
  * @brief  电子烟控制块结构体
  * @note   电子烟系统运行时的所有状态变量和控制参数
  *         包括电压测量值、功率控制、时序控制、状态标志等
  *         在系统运行过程中实时更新，用于控制电子烟的工作状态
  */
typedef struct                                                       // 定义电子烟控制块结构体类型
{
    ecig_cfg_t *cfg;                                                       // 指向电子烟配置结构体的指针，用于访问配置参数

    u32 AD_hot_voltage_mv;      //VAT（真实值）                             // 第一路发热丝电压的毫伏值，经过校准计算的真实电压
    u32 AD_hot_voltage;         //VAT（定点值）                             // 第一路发热丝电压的定点数值，左移13位后的高精度表示

    u32 AD_hot_voltage_mv2;     //VAT（真实值）                             // 第二路发热丝电压的毫伏值，经过校准计算的真实电压
    u32 AD_hot_voltage2;        //VAT（定点值）                             // 第二路发热丝电压的定点数值，左移13位后的高精度表示

    u32 AD_BAT_voltage_mv;      //VBAT                                     // 电池电压的毫伏值，用于欠压保护和功率计算

    u32 p_current;              //当前功率                                  // 当前功率累积值，用于功率控制算法的累积计算
    u32 p_prev;                 //前一次功率                                // 前一次的功率累积值，用于功率控制的连续计算

    u8 power_on_flag;           //PWM开启标志位                             // PWM开启状态标志，1表示PWM已开启，0表示PWM已关闭

    u8 hot_time_flag;           //开始8S计时标志                            // 8秒计时开始标志，用于控制加热时间的计时开始

    u16 hot_time_cnt;           //PWM计时第几秒                             // 加热时间计数器，记录当前已加热的秒数
    //u8 timer_100us_cnt;       //100us计时                                // 注释掉的100微秒计时器，未使用

    u16 timer_cycle_cnt;        //周期计数                                 // 基础周期计数器，以50微秒为单位的周期计数

    u8 timer_switch_adc_flag;                                             // ADC切换标志位，用于状态机中的ADC采样控制
    u8 timer_switch_acc_cnt;                                              // 状态机累积计数器，用于控制switch算法的不同阶段
    u8 timer_big_cycles_10ms;   //检测大周期 10MS                          // 10毫秒大周期计数，用于功率控制的周期管理
    u16 timer_20ms_cnt;         //20msMIC滤波时间                          // 20毫秒防抖计数阈值，用于咪头信号的防抖处理
    u16 timer_1s_cnt;           //1S MIC吸烟时间                           // 1秒计数阈值，用于秒级时间的计算和控制
    u16 cnt_1s;                                                           // 1秒计数器，用于实现秒级的时间控制和统计

    u8 short_circuit_flag;      //短路标志位                               // 短路检测标志，标识当前是否检测到短路故障
    u8 short_circuit_cnt;       //短路累计次数，超过10次算短路               // 短路累积计数器，当累积次数超过阈值时判定为短路

    u8 mic_sta;                 //mic状态（20ms防抖)                       // 咪头状态，经过20毫秒防抖处理的稳定状态
    u8 mic_start;               //mic start信号，仅双路adc使用              // 咪头启动信号，仅在双路ADC模式下使用的状态标志

    u16 hot_res;                //双路adc测量的电热丝阻值，仅双路adc使用   // 第一路发热丝的测量电阻值，仅在双路ADC模式下有效
    u16 hot_res2;                                                          // 第二路发热丝的测量电阻值，仅在双路ADC模式下有效

    u32 adc1;                                                              // 第一路发热丝电压的原始ADC采样值
    u32 adc_res1;                                                          // 第一路电阻检测的原始ADC采样值

    u32 adc2;                                                              // 第二路发热丝电压的原始ADC采样值
    u32 adc_res2;                                                          // 第二路电阻检测的原始ADC采样值

    u8  smoke_sta;              //烟控状态                                 // 烟控系统当前状态，如空闲、吸烟、短路、开路等状态

    u8 det1_sta;                //det状态（20ms防抖）插拔检测               // 第一路烟弹检测状态，经过20毫秒防抖处理
    u8 det2_sta;                //det状态（20ms防抖）插拔检测               // 第二路烟弹检测状态，经过20毫秒防抖处理

    u8 det1_start;              //det start信号                           // 第一路烟弹检测启动信号，用于检测流程控制
    u8 det2_start;              //det start信号                           // 第二路烟弹检测启动信号，用于检测流程控制

    u8 clock_flag;              //童锁标志位                              // 童锁功能标志，1表示童锁已启用，0表示童锁未启用

    u8 poweroff_flag;           //关机标志位                              // 关机状态标志，用于控制设备的开关机状态
} ecig_cb_t;

extern ecig_cb_t ecig;


void ecigarette_init(ecig_cfg_t* cfg);
void ecigarette_sleep_wakeup(ecig_cfg_t* cfg);
void ecigarette_exit(void);
bool ecig_is_working(void);
u8 ecig_is_working_sta(void);
void ecig_pwm_set(u8 pwm_num, bool pwm_on);
void ecig_ven_set(u8 pwm_num, bool pwm_on);
u32 calculate_power(u32 hot_voltage);
void ecig_adgnd_io_deinit(void);
uint8_t mic_start_or_not(void);
void ECIG_PWM_OFF_FUNC(void);
void ECIG_PWM_ON_FUNC(void);
#endif  //_ECIG_H
