#include "include.h"
#include "ecig.h"
#include "ute_module_message.h"
#include "ute_module_lockScreen.h"
#include "ute_module_smoke.h"
#if ECIG_POWER_CONTROL
#define TRACE_EN                             1



uint8_t  power_ecig  = 0;    // 改变功率标志位
uint8_t  power_idx  = 0;    // 当前数组下标
uint16_t tick_200ms = 0;    // 0.2s 计数器 (以 20ms 为单位)
extern ute_module_smoke_data_t uteModuleSmokeData;

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif
char flag_str[] = " 99999999999999999999return 1\n";
#if TRACE_EN
AT(.com_rodata.str)
const char not_finish_str[] = "#%d:%x-%x\n";
#endif
ecig_cb_t ecig;

#if TRACE_EN
AT(.com_rodata.str)
const char res_str[] = "res %d %d %d %d\n";
extern const char short_circuit_str[];
extern const char info_8s[];
AT(.com_rodata.str)
const char info_det[] = "info_det %d\n";
#endif


/**
  * @brief  检测咪头是否被触发
  * @note   该函数用于检测用户是否开始吸烟，支持两种模式：
  *         1. 老化测试模式：通过配置参数模拟触发状态
  *         2. 正常模式：读取GPIO电平状态判断咪头是否被触发
  * @param  None
  * @retval uint8_t: 0-咪头未触发，1-咪头已触发
  */
AT(.com_text.ecig.init)
uint8_t mic_start_or_not(void)                                       // 咪头状态检测函数
{
    if(func_cb.sta == FUNC_AGEING && ( ecig.cfg->smoke_start_or_not ==1)) // 检查是否处于老化测试状态且允许模拟抽烟
    {
        // printf(flag_str);
        return 1;                                                    // 老化测试模式下直接返回1，模拟咪头被触发
    }

    gpio_t gpio;                                                     // 声明GPIO结构体变量，用于GPIO操作
    gpio_cfg_init(&gpio, ecig.cfg->io_mic);                          // 根据配置中的咪头IO引脚号初始化GPIO结构体
    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        return ((gpio.sfr[GPIOx] & BIT(gpio.num)) >> gpio.num);      // 读取GPIO数据寄存器，提取指定引脚状态并右移到最低位
    }
    return 0;                                                        // GPIO配置无效时返回0，表示咪头未触发
}

/**
  * @brief  初始化咪头GPIO引脚
  * @note   该函数配置咪头检测引脚为输入模式，关闭上拉电阻
  *         设置为数字输入模式，用于检测用户吸烟时的气流变化
  * @param  None
  * @retval None
  */
void ecig_mic_io_init(void)                                          // 咪头GPIO初始化函数
{
    gpio_t gpio;                                                     // 声明GPIO结构体变量
    gpio_cfg_init(&gpio, ecig.cfg->io_mic);                          // 根据配置中的咪头IO引脚号初始化GPIO结构体

    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);                        // 清除对应位，关闭该引脚的上拉电阻功能
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);                          // 设置对应位，使能该引脚的数字输入功能
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);                         // 设置对应位，配置该引脚为输入方向
    }
}


/**
  * @brief  检测第一路烟弹是否插入
  * @note   该函数用于检测第一路烟弹的插拔状态，通过读取检测引脚的电平状态
  *         返回值取反是因为硬件设计中插入时为低电平，拔出时为高电平
  * @param  None
  * @retval uint8_t: 0-烟弹未插入，1-烟弹已插入
  */
AT(.com_text.ecig.init)
uint8_t det1_start_or_not(void)                                      // 第一路烟弹检测函数
{
    //gpio_t gpio;
    //gpio_cfg_init(&gpio, ecig.cfg->io_hot_det1);
    if (ecig.cfg->ecig_gpio[ECIG_DET1_IO].sfr)                       // 检查第一路检测引脚的GPIO寄存器指针是否有效
    {
        return !((ecig.cfg->ecig_gpio[ECIG_DET1_IO].sfr[GPIOx] & BIT(ecig.cfg->ecig_gpio[ECIG_DET1_IO].num)) >> ecig.cfg->ecig_gpio[ECIG_DET1_IO].num); // 读取GPIO状态，提取指定位并取反（插入时低电平返回1，拔出时高电平返回0）
    }
    return 0;                                                        // GPIO无效时返回0，表示烟弹未插入
}


/**
  * @brief  检测第二路烟弹是否插入
  * @note   该函数用于检测第二路烟弹的插拔状态
  *         通过读取检测引脚的电平状态并取反来判断烟弹是否插入
  * @param  None
  * @retval uint8_t: 0-烟弹未插入，1-烟弹已插入
  */
AT(.com_text.ecig.init)
uint8_t det2_start_or_not(void)                                      // 第二路烟弹检测函数
{
    //gpio_t gpio;ecig.cfg->smoke_start_or_not
    //gpio_cfg_init(&gpio, ecig.cfg->io_hot_det1);
    if (ecig.cfg->ecig_gpio[ECIG_DET2_IO].sfr)                       // 检查第二路检测引脚的GPIO寄存器指针是否有效
    {
        return !((ecig.cfg->ecig_gpio[ECIG_DET2_IO].sfr[GPIOx] & BIT(ecig.cfg->ecig_gpio[ECIG_DET2_IO].num)) >> ecig.cfg->ecig_gpio[ECIG_DET2_IO].num); // 读取GPIO状态，提取指定位并取反（插入时低电平返回1，拔出时高电平返回0）
    }
    return 0;                                                        // GPIO无效时返回0，表示烟弹未插入
}

/**
  * @brief  初始化烟弹插拔检测GPIO引脚
  * @note   该函数配置两路烟弹检测引脚为数字输入模式，用于检测烟弹的插拔状态
  *         只有在启用烟弹检测模式时才执行初始化，配置完成后将GPIO信息保存到全局配置中
  * @param  None
  * @retval None
  */
//插拔检测
void ecig_det_io_init(void)                                          // 烟弹插拔检测GPIO初始化函数
{
    gpio_t gpio;                                                     // 声明GPIO结构体变量，用于GPIO配置操作
    if(!ecig.cfg->io_hot_mode)                                       // 检查是否启用烟弹检测模式，如果未启用则直接返回
    {
        return;                                                      // 烟弹检测模式未启用，退出函数
    }

    gpio_cfg_init(&gpio, ecig.cfg->io_hot_det1);                     // 根据配置中的第一路检测引脚号初始化GPIO结构体
    memcpy(&ecig.cfg->ecig_gpio[ECIG_DET1_IO],&gpio,sizeof(gpio_t)); // 将第一路检测引脚的GPIO配置信息复制到全局配置数组中

    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);                        // 清除对应位，关闭该引脚的上拉电阻功能
        gpio.sfr[GPIOxDE]  |= BIT(gpio.num);                         // 设置对应位，使能该引脚的数字输入功能
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);                         // 设置对应位，配置该引脚为输入方向
    }

    memset(&gpio,0,sizeof(gpio_t));                                  // 清零GPIO结构体，为配置第二路检测引脚做准备
    gpio_cfg_init(&gpio, ecig.cfg->io_hot_det2);                     // 根据配置中的第二路检测引脚号初始化GPIO结构体
    memcpy(&ecig.cfg->ecig_gpio[ECIG_DET2_IO],&gpio,sizeof(gpio_t)); // 将第二路检测引脚的GPIO配置信息复制到全局配置数组中

    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);                        // 清除对应位，关闭该引脚的上拉电阻功能
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);                          // 设置对应位，使能该引脚的数字输入功能
        gpio.sfr[GPIOxDIR] |= BIT(gpio.num);                         // 设置对应位，配置该引脚为输入方向
    }
}

/**
  * @brief  设置ADC接地引脚为接地状态
  * @note   该函数将ADC接地引脚配置为输出模式并输出低电平，为ADC采样提供稳定的接地基准
  *         使用预配置的GPIO信息，直接操作寄存器设置引脚状态
  * @param  None
  * @retval None
  */
AT(.com_text.ecig.init)
void ecig_adgnd_io_set_gnd(void)                                     // ADC接地引脚设置为接地状态函数
{
    //gpio_t gpio;                                                   // 注释掉的GPIO变量声明（使用预配置的GPIO信息）
    //gpio_cfg_init(&gpio, ecig.cfg->io_adgnd);                      // 注释掉的GPIO初始化（使用预配置的GPIO信息）
    if (ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr)                      // 检查ADC接地引脚的GPIO寄存器指针是否有效
    {
        ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr[GPIOxFEN] &= ~BIT(ecig.cfg->ecig_gpio[ECIG_ADGND_IO].num); // 清除对应位，关闭该引脚的上拉电阻功能
        ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr[GPIOxDE] |= BIT(ecig.cfg->ecig_gpio[ECIG_ADGND_IO].num);   // 设置对应位，使能该引脚的数字功能
        ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr[GPIOxDIR] &= ~BIT(ecig.cfg->ecig_gpio[ECIG_ADGND_IO].num); // 清除对应位，配置该引脚为输出方向
        ecig.cfg->ecig_gpio[ECIG_ADGND_IO].sfr[GPIOxCLR] = BIT(ecig.cfg->ecig_gpio[ECIG_ADGND_IO].num);   // 设置对应位，输出低电平（接地状态）
    }
}

/**
  * @brief  反初始化ADC接地引脚
  * @note   该函数将ADC接地引脚恢复为默认状态，关闭数字功能
  *         重新初始化GPIO配置并更新全局配置信息
  * @param  None
  * @retval None
  */
AT(.com_text.ecig.init)
void ecig_adgnd_io_deinit(void)                                      // ADC接地引脚反初始化函数
{
    gpio_t gpio;                                                     // 声明GPIO结构体变量
    gpio_cfg_init(&gpio, ecig.cfg->io_adgnd);                        // 根据配置中的ADC接地引脚号重新初始化GPIO结构体
    memcpy(&ecig.cfg->ecig_gpio[ECIG_ADGND_IO],&gpio,sizeof(gpio_t)); // 将重新初始化的GPIO配置信息复制到全局配置数组中
    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        gpio.sfr[GPIOxDE] &= ~BIT(gpio.num);                         // 清除对应位，关闭该引脚的数字功能，恢复为模拟状态
    }
}

/**
  * @brief  初始化PWM输出GPIO引脚
  * @note   该函数配置两路PWM输出引脚为数字输出模式，初始输出低电平
  *         用于控制发热丝的PWM信号输出，配置完成后将GPIO信息保存到全局配置中
  * @param  None
  * @retval None
  */
void ecig_pwm_io_init(void)                                          // PWM输出GPIO初始化函数
{
    gpio_t gpio;                                                     // 声明GPIO结构体变量，用于GPIO配置操作
    gpio_cfg_init(&gpio, ecig.cfg->io_pwm1);                         // 根据配置中的第一路PWM引脚号初始化GPIO结构体
    memcpy(&ecig.cfg->ecig_gpio[ECIG_PWM1_IO],&gpio,sizeof(gpio_t)); // 将第一路PWM引脚的GPIO配置信息复制到全局配置数组中
    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);                        // 清除对应位，关闭该引脚的上拉电阻功能
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);                          // 设置对应位，使能该引脚的数字功能
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);                        // 清除对应位，配置该引脚为输出方向
        gpio.sfr[GPIOxCLR] = BIT(gpio.num);                          // 设置对应位，初始输出低电平（PWM关闭状态）
    }


    gpio_cfg_init(&gpio, ecig.cfg->io_pwm2);                         // 根据配置中的第二路PWM引脚号初始化GPIO结构体
    memcpy(&ecig.cfg->ecig_gpio[ECIG_PWM2_IO],&gpio,sizeof(gpio_t)); // 将第二路PWM引脚的GPIO配置信息复制到全局配置数组中
    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);                        // 清除对应位，关闭该引脚的上拉电阻功能
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);                          // 设置对应位，使能该引脚的数字功能
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);                        // 清除对应位，配置该引脚为输出方向
        gpio.sfr[GPIOxCLR] = BIT(gpio.num);                          // 设置对应位，初始输出低电平（PWM关闭状态）
    }
}


/**
  * @brief  初始化VEN电源控制GPIO引脚
  * @note   该函数配置VEN电源控制引脚为数字输出模式，初始输出低电平
  *         VEN用于控制整个发热丝电路的供电，配置完成后将GPIO信息保存到全局配置中
  * @param  None
  * @retval None
  */
void ecig_ven_io_init(void)                                          // VEN电源控制GPIO初始化函数
{
    gpio_t gpio;                                                     // 声明GPIO结构体变量，用于GPIO配置操作
    gpio_cfg_init(&gpio, ecig.cfg->io_ven);                          // 根据配置中的VEN引脚号初始化GPIO结构体
    memcpy(&ecig.cfg->ecig_gpio[ECIG_VEN_IO],&gpio,sizeof(gpio_t));  // 将VEN引脚的GPIO配置信息复制到全局配置数组中
    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        gpio.sfr[GPIOxFEN] &= ~BIT(gpio.num);                        // 清除对应位，关闭该引脚的上拉电阻功能
        gpio.sfr[GPIOxDE] |= BIT(gpio.num);                          // 设置对应位，使能该引脚的数字功能
        gpio.sfr[GPIOxDIR] &= ~BIT(gpio.num);                        // 清除对应位，配置该引脚为输出方向
        gpio.sfr[GPIOxCLR] = BIT(gpio.num);                          // 设置对应位，初始输出低电平（VEN关闭状态）
    }
}


/**
  * @brief  设置PWM输出状态
  * @note   该函数用于控制指定PWM通道的输出状态，支持PWM1和PWM2两路输出
  *         通过直接操作GPIO寄存器来控制PWM信号的高低电平
  * @param  pwm_num: PWM通道号（1=PWM1，其他=PWM2）
  * @param  pwm_on: PWM输出状态（true=高电平，false=低电平）
  * @retval None
  */
AT(.com_text.ecig.pwm)
void ecig_pwm_set(u8 pwm_num, bool pwm_on)                          // PWM输出控制函数
{
    gpio_t gpio;                                                     // 声明GPIO结构体变量，用于GPIO操作
    if(pwm_num == 1)                                                 // 判断是否为PWM1通道
    {
        memcpy(&gpio,&ecig.cfg->ecig_gpio[ECIG_PWM1_IO],sizeof(gpio_t)); // 复制PWM1引脚的GPIO配置信息到临时变量
    }
    else                                                             // PWM2通道的处理分支
    {
        memcpy(&gpio,&ecig.cfg->ecig_gpio[ECIG_PWM2_IO],sizeof(gpio_t)); // 复制PWM2引脚的GPIO配置信息到临时变量
    }
    if (gpio.sfr)                                                    // 检查GPIO寄存器指针是否有效
    {
        if (pwm_on)                                                  // 判断是否需要开启PWM输出
        {
            gpio.sfr[GPIOxSET] = BIT(gpio.num);                      // 设置对应位，输出高电平（PWM开启）
        }
        else                                                         // PWM关闭的处理分支
        {
            gpio.sfr[GPIOxCLR] = BIT(gpio.num);                      // 清除对应位，输出低电平（PWM关闭）
        }
    }
}

/**
  * @brief  设置VEN电源控制状态
  * @note   该函数用于控制VEN电源的开启和关闭状态
  *         VEN控制整个发热丝电路的供电，直接使用预配置的GPIO信息
  * @param  pwm_num: 未使用的参数（保持接口一致性）
  * @param  pwm_on: VEN电源状态（true=开启，false=关闭）
  * @retval None
  */
AT(.com_text.ecig.pwm)
void ecig_ven_set(u8 pwm_num, bool pwm_on)                           // VEN电源控制函数
{
    //ecig.cfg->ecig_gpio[ECIG_VEN_IO];                              // 注释掉的VEN GPIO引用
    if (ecig.cfg->ecig_gpio[ECIG_VEN_IO].sfr)                        // 检查VEN引脚的GPIO寄存器指针是否有效
    {
        if (pwm_on)                                                  // 判断是否需要开启VEN电源
        {
            ecig.cfg->ecig_gpio[ECIG_VEN_IO].sfr[GPIOxSET] = BIT(ecig.cfg->ecig_gpio[ECIG_VEN_IO].num); // 设置对应位，开启VEN电源供电
        }
        else                                                         // VEN电源关闭的处理分支
        {
            ecig.cfg->ecig_gpio[ECIG_VEN_IO].sfr[GPIOxCLR] = BIT(ecig.cfg->ecig_gpio[ECIG_VEN_IO].num); // 清除对应位，关闭VEN电源供电
        }
    }
}


/**
  * @brief  计算发热丝功率
  * @note   该函数根据发热丝电压和电阻值计算实际功率，使用P=V²/R公式
  *         根据系统配置选择使用实时测量电阻值或配置电阻值
  * @param  hot_voltage: 发热丝电压值（定点数格式）
  * @retval u32: 计算得到的功率值
  */
AT(.com_text.ecig.power)
u32 calculate_power(u32 hot_voltage)                                 // 功率计算函数
{
    u16 hot_v = (u16)hot_voltage;                                    // 将32位电压值转换为16位，提取有效电压数据
    u32 power;                                                       // 功率计算结果变量

    if (ecig.cfg->adc2_en)                                           // 检查是否启用第二路ADC功能（双路检测模式）
    {
        power = hot_v * hot_v / ecig.hot_res;                        // 双路模式：使用实时测量的电阻值计算功率 P=V²/R
    }
    else                                                             // 单路检测模式的处理分支
    {
        power = hot_v * hot_v / ecig.cfg->res_wire;                  // 单路模式：使用配置中的固定电阻值计算功率 P=V²/R
    }
    return power;                                                    // 返回计算得到的功率值
}
/**
  * @brief  字符串常量定义区域
  * @note   定义用于调试输出的字符串常量，存储在只读数据段中
  */
AT(.com_rodata.str)
const char ecig_end[] = "ecig_end \\n";                               // 定义电子烟结束标识字符串常量


#if TRACE_EN
AT(.com_rodata.str)
const char res_ch[] = "res :%d %d\\n";                                // 定义电阻调试输出格式字符串

#endif

/**
  * @brief  计算第一路发热丝电阻值
  * @note   通过ADC采样值计算发热丝的实际电阻值，并进行开路和短路检测
  *         使用分压电路原理：R = (V1 * R_diff) / (V2 - V1)
  *         同时检测电阻比值来判断开路和短路故障
  * @param  None
  * @retval None
  */
AT(.com_text.ecig.res)
void caculate_res(void)
{
    u32 res = 0;                                                            // 声明32位电阻计算结果变量，初始化为0
    u16 diff;                                                               // 声明16位电压差值变量

    if (ecig.adc1 == 0)                                                     // 检查第一路ADC采样值是否为0（无信号）
    {
        ecig.hot_res = 0;                                                   // 设置第一路电阻值为0，表示无电阻或故障
        TRACE(res_ch, -1,0);                                                // 打印调试信息：标识码-1，电阻值0
        return;                                                             // 提前返回，结束函数执行
    }

    if ((ecig.adc_res1 / ecig.adc1) <= ecig.cfg->open_res_proportion)       // 检查电阻比值是否小于等于开路检测阈值
    {
        ecig.hot_res = 0;                                                   // 设置电阻值为0，表示检测到开路故障
        //  printf(flag_str,__LINE__);
        ecig.smoke_sta = OPEN_CIRCUIT;                                      // 设置烟控状态为开路状态
        TRACE(res_ch, -2,(ecig.adc_res1 / ecig.adc1) );
    }
    else if ((ecig.adc_res1 / ecig.adc1) >= ecig.cfg->short_res_proportion) // 检查电阻比值是否大于等于短路检测阈值
    {
        ecig.hot_res = 0;                                                   // 设置电阻值为0，表示检测到短路故障
        ecig.smoke_sta = SHORT_CIRCUIT;                                     // 设置烟控状态为短路状态
        TRACE(res_ch, -3,(ecig.adc_res1 / ecig.adc1) );                     // 打印调试信息：标识码-3，电阻比值
    }
    else                                                                    // 电阻比值正常，进行实际电阻计算
    {
        diff = (u16)(ecig.adc_res1 - ecig.adc1);                            // 计算电阻检测电压与发热丝电压的差值
        res = ecig.adc1 * ecig.cfg->res_diff;                               // 计算电阻分子：发热丝电压 × 分压电阻值
        ecig.hot_res = res / diff;                                          // 计算实际电阻值：分子 ÷ 电压差值
        ecig.cfg->res_wire = ecig.hot_res;                                  // 将计算结果保存到配置结构体中
    }
    TRACE(res_str,1111, ecig.hot_res*1000/8192, ecig.adc1, ecig.adc_res1);  // 打印调试信息：标识码1111，电阻值(mΩ)，ADC1值，电阻检测ADC值
}

/**
  * @brief  计算第二路发热丝电阻值
  * @note   用于计算第二路发热丝的电阻值
  *         支持双发模式下的独立电阻检测和故障判断
  * @param  None
  * @retval None
  */
AT(.com_text.ecig.res)
void caculate_res2(void)
{
    u32 res = 0;                                                            // 声明32位电阻计算结果变量，初始化为0
    u16 diff;                                                               // 声明16位电压差值变量
    if (ecig.adc2 == 0)                                                     // 检查第二路ADC采样值是否为0（无信号）
    {
        ecig.hot_res2 = 0;                                                  // 设置第二路电阻值为0，表示无电阻或故障
        TRACE(res_ch, -1,0);
        return;                                                             // 提前返回，结束函数执行
    }

    if ((ecig.adc_res2 / ecig.adc2) <= ecig.cfg->open_res_proportion)       // 检查电阻比值是否小于等于开路检测阈值
    {
        ecig.hot_res2 = 0;                                                  // 设置电阻值为0，表示检测到开路故障
        // printf(flag_str,__LINE__);                                       // 注释掉的调试打印语句
        ecig.smoke_sta = OPEN_CIRCUIT;                                      // 设置烟控状态为开路状态
        TRACE(res_ch, -2,(ecig.adc_res2 / ecig.adc2) );
    }
    else if ((ecig.adc_res2 / ecig.adc2) >= ecig.cfg->short_res_proportion) // 检查电阻比值是否大于等于短路检测阈值
    {
        ecig.hot_res2 = 0;                                                  // 设置电阻值为0，表示检测到短路故障
        ecig.smoke_sta = SHORT_CIRCUIT;                                     // 设置烟控状态为短路状态
        TRACE(res_ch, -3,(ecig.adc_res2 / ecig.adc2) );                     // 打印调试信息：标识码-3，电阻比值
    }
    else                                                                    // 电阻比值正常，进行实际电阻计算
    {
        diff = (u16)(ecig.adc_res2 - ecig.adc2);                            // 计算电阻检测电压与发热丝电压的差值
        res = ecig.adc2 * ecig.cfg->res_diff;                               // 计算电阻分子：发热丝电压 × 分压电阻值
        ecig.hot_res2 = res / diff;                                         // 计算实际电阻值：分子 ÷ 电压差值
        ecig.cfg->res_wire2 = ecig.hot_res2;                                // 将计算结果保存到配置结构体中
    }
    TRACE(res_str,222, ecig.hot_res2*1000/8192, ecig.adc2, ecig.adc_res2); // 打印调试信息：标识码222，电阻值(mΩ)，ADC2值，电阻检测ADC值
}

/**
  * @brief  单发模式数据初始化和状态清理函数
  * @note   在单发模式下清理系统状态，处理短路保护和超时保护
  *         当咪头未触发或检测到故障时，重置所有相关状态变量
  *         同时处理短路累计判断和8秒超时保护机制
  * @param  None
  * @retval None
  */
static u16 cnt_1s = 0;                                               // 声明静态1秒计数器变量，用于定时处理
AT(.com_text.isr)
void timer_hot_single_data_init(void)                                // 单发模式数据初始化函数
{
    ecig_adgnd_io_deinit();                                          // 反初始化ADC接地IO，释放GPIO资源
    ECIG_PWM_OFF_FUNC();                                             // 关闭PWM输出，停止对发热丝供电

    if (!ecig.mic_sta)                                               // 检查咪头是否未触发（用户未吸烟）
    {
        ecig.timer_cycle_cnt = 0;                                    // 重置周期计数器为0
        ecig.hot_time_cnt = 0;                                       // 重置加热时间计数器为0
        ecig.hot_time_flag = 0;                                      // 清除加热时间标志
        cnt_1s = 0;                                                  // 重置1秒计数器为0
        ecig.timer_switch_acc_cnt = 0;                               // 重置状态机累积计数器为0
        ecig.timer_switch_adc_flag = false;                          // 清除ADC标志位
        ecig.smoke_sta = IDLE;                                       // 设置烟控状态为空闲状态
        if(ecig.short_circuit_flag != SHORT_CIRCUIT)                 // 检查短路标志是否不是短路状态
        {
            ecig.short_circuit_flag = IDLE;                          // 清除短路标志，设置为空闲状态
        }
    }


    if (ecig.short_circuit_cnt >= 10 || ecig.short_circuit_flag == SHORT_CIRCUIT)           // 检查短路计数是否达到10次或短路标志为短路状态
    {
        ecig.power_on_flag = 0;                                      // 清除功率开启标志，禁止加热
        //ecig.short_circuit_flag = 1;                               // 注释掉的短路标志设置
        ecig.hot_time_cnt = 0;                                       // 重置加热时间计数器
        ecig.hot_time_flag = 0;                                      // 清除加热时间标志

        if (cnt_1s == 1)                                             // 检查1秒计数器是否为1（确保每秒只执行一次）
        {
            printf(short_circuit_str,1,ecig.short_circuit_flag);     // 打印短路故障信息：标识码1，短路标志值

            if(ecig.smoke_sta != SHORT_CIRCUIT)                      // 检查烟控状态是否不是短路状态
            {
                //    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件
                sys_cb.smoke_index = SHORT_CIRCUIT;                  // 设置系统控制块中的烟控索引为短路状态
            }
        }
    }


    if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)                 // 检查加热时间是否超过最大允许时间
    {
        if (cnt_1s == 1)                                             // 检查1秒计数器是否为1（确保每秒只执行一次）
        {
            if(ecig.smoke_sta != SMOKE_TIMEOUT)                      // 检查烟控状态是否不是超时状态
            {
                //uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件，通知上层应用超时
                sys_cb.smoke_index = SMOKE_TIMEOUT;                  // 设置系统控制块中的烟控索引为超时状态
            }
            printf(info_8s);                                         // 打印8秒超时警告信息
        }
    }
}


/**
  * @brief  双发模式数据初始化和状态清理函数
  * @note   在咪头未触发时清理所有状态变量，处理超时和短路保护
  *         同时关闭所有硬件输出，确保系统回到安全的空闲状态
  *         在检测到故障时发送相应的提醒消息
  * @param  None
  * @retval None
  */
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保实时性
void timer_hot_dual_data_init(void)
{
    if (!ecig.mic_sta)                                               // 检查咪头是否未触发（用户未吸烟）
    {
        ecig.timer_cycle_cnt = 0;                                    // 重置周期计数器，清零50μs基础周期计数
        ecig.mic_start = 0;                                          // 清除咪头启动标志，标记未开始工作
        ecig.hot_time_cnt = 0;                                       // 重置加热时间计数器，清零秒级计时
        ecig.hot_time_flag = 0;                                      // 清除加热时间标志，停止8秒计时
        cnt_1s = 0;                                                  // 重置1秒计数器，用于控制每秒执行一次的操作
        ecig.smoke_sta = IDLE;                                       // 设置烟控状态为空闲状态

        ecig.timer_switch_acc_cnt = 0;                               // 重置状态机累积计数器，回到初始状态
        ecig.timer_switch_adc_flag = false;                          // 清除ADC标志位，标记ADC未启动
        if(ecig.short_circuit_flag != SHORT_CIRCUIT)                 // 检查短路标志是否不是短路状态
        {
            ecig.short_circuit_flag = IDLE;                          // 清除短路标志，设置为空闲状态（保留短路状态不清除）
        }
    }


    if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)                 // 检查加热时间是否超过最大允许时间（通常8秒）
    {
        if (cnt_1s == 1)                                             // 检查1秒计数器是否为1（确保每秒只执行一次）
        {
            printf(info_8s);                                         // 打印8秒超时警告信息
            if(ecig.smoke_sta != SMOKE_TIMEOUT)                      // 检查烟控状态是否不是超时状态
            {
                //  uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0); // 注释掉的旧版本消息发送
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件，通知上层应用超时
                sys_cb.smoke_index = SMOKE_TIMEOUT;                  // 设置系统控制块中的烟控索引为超时状态
            }
        }
    }


    if (ecig.short_circuit_flag == SHORT_CIRCUIT)                    // 检查短路标志是否为短路状态
    {
        if (cnt_1s == 1)                                             // 检查1秒计数器是否为1（确保每秒只执行一次）
        {
            printf(short_circuit_str,0,ecig.short_circuit_flag);     // 打印短路故障信息：标识码0，短路标志值
            if(ecig.smoke_sta != SHORT_CIRCUIT)                      // 检查烟控状态是否不是短路状态
            {
                // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0); // 注释掉的旧版本消息发送
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件，通知上层应用短路故障
                sys_cb.smoke_index = SHORT_CIRCUIT;                  // 设置系统控制块中的烟控索引为短路状态
            }
        }
    }

    ecig_adgnd_io_deinit();                                          // 反初始化ADC接地IO，释放GPIO资源
    ECIG_PWM_OFF_FUNC();                                             // 关闭PWM输出，停止对发热丝供电
    ECIG_VEN_OFF();                                                  // 关闭VEN电源，切断整个发热丝电路的供电

}

/**
  * @brief  第一路发热丝电阻计算函数
  * @note   第一路发热丝的电阻检测，包括ADC采样启动和电阻值计算
  *         采用状态机方式，先启动ADC采样，采样完成后读取数据并计算电阻
  *         需要开启VEN电源和设置ADC接地以确保测量准确性
  * @param  None
  * @retval None
  */
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保实时性
void timer_hot_dual_caculate_res1(void)//
{
    if (saradc_is_finish() && ecig.timer_switch_adc_flag)            // 检查ADC采样是否完成且ADC标志位有效
    {
        ecig.timer_switch_adc_flag = false;                          // 清除ADC标志位，防止重复处理
        ecig.adc1 = saradc_get_value10(ecig.cfg->adc1_ch);           // 读取第一路发热丝电压ADC值(10位精度)
        ecig.adc_res1 = saradc_get_value10(ecig.cfg->adc_res1_ch);   // 读取第一路电阻检测ADC值(10位精度)
        caculate_res();                                              // 调用第一路电阻计算函数，根据电压差计算实际电阻值
    }
    else                                                             // ADC采样未完成或标志位无效时的处理
    {
        ECIG_VEN_ON();                                               // 开启VEN电源，为发热丝电路供电，确保电阻测量的准确性
        ECIG_PWM_OFF_FUNC();                                         // 关闭PWM输出，避免PWM信号干扰电阻测量
        ecig_adgnd_io_set_gnd();                                     // 设置ADC接地IO为接地状态，提供稳定的测量基准

        if(!ecig.timer_switch_adc_flag)                              // 检查ADC标志位是否为假(未启动采样)
        {
            if (!saradc_adc15_is_bg())                               // 检查ADC15通道是否已配置为内部基准电压模式
            {
                saradc_adc15_analog_select(ADCCH15_ANA_BG);          // 将ADC15通道切换到内部基准电压(BG)模式，用于电压校准
            }
            saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch) | SADCCH, 0, 0); // 启动多通道ADC同时采样：第一路发热丝电压通道、第一路电阻检测通道和其他系统ADC通道
            ecig.timer_switch_adc_flag = true;                                                        // 设置ADC标志位为真，标记ADC采样已启动
        }
    }
}

/**
  * @brief  第二路发热丝电阻计算函数
  * @note   第二路发热丝的电阻检测
  *         包括ADC采样启动和电阻值计算，采用相同的状态机控制方式
  *         用于双发模式下的第二路发热丝电阻检测
  * @param  None
  * @retval None
  */
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保实时性
void timer_hot_dual_caculate_res2(void)//                            /
{
    if (saradc_is_finish() && ecig.timer_switch_adc_flag)            // 检查ADC采样是否完成且ADC标志位有效
    {
        ecig.timer_switch_adc_flag = false;                          // 清除ADC标志位，防止重复处理
        ecig.adc2 = saradc_get_value10(ecig.cfg->adc2_ch);           // 读取第二路发热丝电压ADC值(10位精度)
        ecig.adc_res2 = saradc_get_value10(ecig.cfg->adc_res2_ch);   // 读取第二路电阻检测ADC值(10位精度)
        caculate_res2();                                             // 调用第二路电阻计算函数，根据电压差计算实际电阻值
    }
    else                                                             // ADC采样未完成或标志位无效时的处理
    {
        ECIG_VEN_ON();                                               // 开启VEN电源，为发热丝电路供电，确保电阻测量的准确性
        ECIG_PWM_OFF_FUNC();                                         // 关闭PWM输出，避免PWM信号干扰电阻测量
        ecig_adgnd_io_set_gnd();                                     // 设置ADC接地IO为接地状态，提供稳定的测量基准

        if(!ecig.timer_switch_adc_flag)                              // 检查ADC标志位是否为假(未启动采样)
        {
            if (!saradc_adc15_is_bg())                               // 检查ADC15通道是否已配置为内部基准电压模式
            {
                saradc_adc15_analog_select(ADCCH15_ANA_BG);          // 将ADC15通道切换到内部基准电压(BG)模式，用于电压校准
            }
            saradc_kick_start_do(BIT(ecig.cfg->adc2_ch) | BIT(ecig.cfg->adc_res2_ch) | SADCCH, 0, 0); // 启动多通道ADC同时采样：第二路发热丝电压通道、第二路电阻检测通道和其他系统ADC通道
            ecig.timer_switch_adc_flag = true;                       // 设置ADC标志位为真，标记ADC采样已启动
        }
    }
}

/**
  * @brief  烟弹插拔检测和电阻测量函数
  * @note   检测两路烟弹的插拔状态，并在检测到插入时进行电阻测量
  *         使用20ms防抖机制过滤信号噪声，支持双路ADC模式下的电阻计算
  *         当检测到烟弹插入时会进行多次电阻测量以确保准确性
  * @param  None
  * @retval bool: true-正在进行检测处理，false-检测完成
  */
AT(.com_text.isr)
bool timer_hot_det_res(void)
{
    static u32 cnt_det_20ms = 0;                                     // 第一路检测防抖计数器，用于20ms防抖
    static u32 cnt_det2_20ms = 0;                                    // 第二路检测防抖计数器，用于20ms防抖

    //插拔检测
    if(det1_start_or_not() != ecig.det1_sta)                         // 检查第一路检测引脚实时状态是否与稳定状态不同
    {
        //  printf(flag_str,__LINE__);
        if (++cnt_det_20ms >= (ecig.timer_20ms_cnt))      //20ms     // 递增防抖计数器，检查是否达到20ms防抖时间
        {
            if(det1_start_or_not())                                  // 检查第一路烟弹是否插入（返回真表示插入）
            {
                //printf(flag_str,__LINE__);
#if ECIG_ADC2_EN
                if(ecig.timer_switch_acc_cnt < 24)                   // 检查状态计数器是否小于24（需要进行24次电阻测量）
                {
                    //  printf(flag_str,__LINE__);
                    timer_hot_dual_caculate_res1();                  // 调用第一路电阻计算函数，进行ADC采样和电阻计算
                    ecig.timer_switch_acc_cnt++;                     // 递增状态计数器，记录已完成的测量次数
                    return true;                                     // 返回真值，表示正在进行检测处理，需要继续
                }
                else if(ecig.timer_switch_acc_cnt >= 24)             // 检查是否已完成24次电阻测量
                {
                    ecig.det1_sta = !ecig.det1_sta;                  // 翻转第一路检测稳定状态，确认状态变化
                    timer_hot_dual_data_init();                      // 调用双发模式数据初始化函数，清理状态
                    ecig.det1_start = true;                          // 设置第一路烟弹启动标志为真
                    //   printf(flag_str,__LINE__);
                    //  uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);            // 向消息队列发送电子烟提醒事件，通知上层应用烟弹插入
                    sys_cb.smoke_index = IN_DEVICE;                  // 设置系统控制块中的烟控索引为设备插入状态
                    printf(info_det,ecig.det1_start);                // 打印检测信息：第一路烟弹启动状态
                }
#else
                ecig.det1_sta = !ecig.det1_sta;                      // 直接翻转第一路检测稳定状态
                ecig.det1_start = true;                              // 设置第一路烟弹启动标志为真
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件
                sys_cb.smoke_index = IN_DEVICE;                      // 设置系统控制块烟控索引为设备插入状态
#endif
            }
            else                                                     // 第一路烟弹未插入的处理分支
            {
                //   printf(flag_str,__LINE__);
                ecig.det1_sta = !ecig.det1_sta;                      // 翻转第一路检测稳定状态，确认烟弹拔出
                ecig.det1_start = false;                             // 设置第一路烟弹启动标志为假
                ecig_set_res(0);                                     // 调用函数将第一路电阻值设置为0，表示无电阻
                // printf(flag_str,__LINE__);
                // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件，通知烟弹拔出
                sys_cb.smoke_index = OPEN_CIRCUIT;                   // 设置系统控制块烟控索引为开路状态
                printf(info_det,ecig.det1_start);
            }

        }
    }
    else                                                             // 第一路检测状态无变化的处理
    {
        cnt_det_20ms = 0;                                            // 重置第一路防抖计数器，准备下次检测
    }


    if(det2_start_or_not() != ecig.det2_sta)                         // 检查第二路检测引脚实时状态是否与稳定状态不同
    {
        //  printf(flag_str,__LINE__);
        if (++cnt_det2_20ms >= (ecig.timer_20ms_cnt))      //20ms     // 递增第二路防抖计数器，检查是否达到20ms防抖时间
        {
            if(det2_start_or_not())                                  // 检查第二路烟弹是否插入（返回真表示插入）
            {
#if ECIG_ADC2_EN
                if(ecig.timer_switch_acc_cnt < 6)                    // 检查状态计数器是否小于6（需要进行6次电阻测量）
                {
                    //  printf(flag_str,__LINE__);
                    timer_hot_dual_caculate_res2();                  // 调用第二路电阻计算函数，进行ADC采样和电阻计算
                    ecig.timer_switch_acc_cnt++;                     // 递增状态计数器，记录已完成的测量次数
                    return true;                                     // 返回真值，表示正在进行检测处理，需要继续
                }
                else if(ecig.timer_switch_acc_cnt >= 6)              // 检查是否已完成6次电阻测量
                {
                    ecig.det2_sta = !ecig.det2_sta;                  // 翻转第二路检测稳定状态，确认状态变化
                    timer_hot_dual_data_init();                      // 调用双发模式数据初始化函数，清理状态
                    ecig.det2_start = true;                          // 设置第二路烟弹启动标志为真
                    //  printf(flag_str,__LINE__);
                    // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);            // 向消息队列发送电子烟提醒事件，通知上层应用烟弹插入
                    sys_cb.smoke_index = IN_DEVICE;                  // 设置系统控制块中的烟控索引为设备插入状态
                    printf(info_det,ecig.det2_start);                // 打印检测信息：第二路烟弹启动状态
                }
#else
                ecig.det2_sta = !ecig.det2_sta;                      // 直接翻转第二路检测稳定状态
                ecig.det2_start = true;                              // 设置第二路烟弹启动标志为真
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件
                sys_cb.smoke_index = IN_DEVICE;                      // 设置系统控制块烟控索引为设备插入状态
#endif
            }
            else                                                     // 第二路烟弹未插入的处理分支
            {
                ecig.det2_sta = !ecig.det2_sta;                      // 翻转第二路检测稳定状态，确认烟弹拔出
                ecig.det2_start = false;                             // 设置第二路烟弹启动标志为假
                ecig_set_res2(0);                                    // 调用函数将第二路电阻值设置为0，表示无电阻
                //  printf(flag_str,__LINE__);
                //    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0);
                msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件，通知烟弹拔出
                sys_cb.smoke_index = OPEN_CIRCUIT;                   // 设置系统控制块烟控索引为开路状态
                printf(info_det,ecig.det2_start);                    // 打印检测信息：第二路烟弹启动状态（假）
            }

        }
    }
    else                                                             // 第二路检测状态无变化的处理
    {
        cnt_det2_20ms = 0;                                           // 重置第二路防抖计数器，准备下次检测
    }

    if(!ecig.det1_start || !ecig.det2_start)                         // 检查是否有任一路烟弹未启动
    {
        //   printf(flag_str,__LINE__);
#if ECIG_ADC2_EN
        timer_hot_dual_data_init();                                  // 调用双发模式数据初始化函数，清理所有状态
#else
        timer_hot_single_data_init();                                // 调用单发模式数据初始化函数，清理所有状态
#endif
        return true;                                                 // 返回真值，表示需要继续处理
        ecig.short_circuit_flag = IDLE;                              // 设置短路标志为空闲状态（注意：这行代码永远不会执行，因为前面有return）
    }
    return false;                                                    // 返回假值，表示检测处理完成，可以继续其他操作
}



/**
  * @brief  电子烟系统主中断服务函数
  * @note  电子烟中断处理函数，以50μs周期执行
    1. 中断源验证
        检查TMR3定时器中断标志
        确认系统时钟就绪状态
    2. 咪头状态管理
        实现20ms防抖机制
        管理吸烟时间计数
    3. 烟弹检测处理
        可选的烟弹插拔检测
        电阻测量和状态管理
    4. 童锁保护机制
        检测童锁状态下的误操作
        触发相应的提醒界面
    5. 核心控制调度
        根据ADC配置选择单发或双发模式
        调用相应的发热丝控制算法
  * @param  None
  * @retval None
  */
AT(.com_text.isr)
void ecigarette_isr(void)//50us                                      // 电子烟主中断服务函数，50μs周期执行
{
    if(TMR3CON & BIT(9))                                             // 检查TMR3定时器的中断标志位是否被设置
    {
        TMR3CPND = BIT(9);                                           // 清除TMR3定时器的中断挂起标志，确认中断处理
    }
    else                                                             // 如果不是TMR3中断触发
    {
        return;                                                      // 直接返回，不处理其他中断源
    }
    if(sys_clk_get() < 2)                                            // 检查系统时钟是否小于2（系统未完全启动）
    {
        return;                                                      // 系统未就绪时直接返回，避免在初始化期间执行
    }


    static u16 cnt_20ms = 0;                                         // 20ms防抖计数器，用于咪头状态防抖
    if (mic_start_or_not() != ecig.mic_sta)                          // 检查咪头实时状态是否与稳定状态不同
    {
        if (++cnt_20ms >= (ecig.timer_20ms_cnt))      //20ms         // 递增防抖计数器，检查是否达到20ms防抖时间
        {
            ecig.mic_sta = !ecig.mic_sta;                            // 状态翻转：将稳定状态切换到相反状态
        }
    }
    else                                                             // 咪头实时状态与稳定状态相同时的处理
    {
        cnt_20ms = 0;                                                // 重置防抖计数器，取消状态变化检测
    }


    if (ecig.mic_sta )                                               // 检查咪头是否处于触发状态（用户正在吸烟）
    {
        if (++ecig.cnt_1s >= (ecig.timer_1s_cnt))                    // 递增1秒计数器，检查是否达到1秒时间
        {
            ecig.hot_time_cnt += (bool)ecig.hot_time_flag;           // 如果加热时间标志有效，则递增加热时间计数器（秒级）
            ecig.cnt_1s = 0;                                         // 重置1秒计数器，准备下一秒的计时
        }

        if (++tick_200ms >= 4000)                                    //0.2秒计次
        {
            tick_200ms = 0;
            power_ecig = 1;
        }
    }


#if ECIG_DET_EN
    if(timer_hot_det_res())                                          // 调用烟弹插拔检测函数，检查是否正在处理检测
    {
        return;                                                      // 如果正在处理烟弹检测，直接返回，暂停其他处理
    }
#endif


    static u16 cnt_lock_ms = 0;                                      // 童锁检测计数器，用于童锁状态下的计时
    //if(ecig.clock_flag == 1)
    if(uteModulePasswordData.password_flag == 1)                     // 检查系统是否处于童锁状态（密码保护开启）
    {
        if(mic_start_or_not())                                       // 检查咪头是否被触发（童锁状态下用户尝试吸烟）
        {
            // if(cnt_lock_ms >= (ecig.timer_20ms_cnt))
            // {
            //     if(cnt_lock_ms == (ecig.timer_20ms_cnt))
            //     {
            //         //发提醒事件
            //         func_cb.sta = FUNC_ECIG_REMINDERCLOCK;
            //         printf(flag_str);
            //     }
            // }
            // else
            // {
            //     cnt_lock_ms++;
            // }

            if(cnt_lock_ms >= (ecig.timer_20ms_cnt))                 // 检查童锁计数器是否达到20ms阈值
            {
                if(cnt_lock_ms == (ecig.timer_20ms_cnt))             // 检查是否刚好达到20ms（确保只执行一次）
                {
                    //发提醒事件
                    func_cb.sta = FUNC_ECIG_REMINDERCLOCK;           // 设置功能状态为童锁提醒界面
                    printf(flag_str);                                // 打印童锁触发调试信息
                }
            }
            cnt_lock_ms++;                                           // 递增童锁计数器，记录用户尝试吸烟的持续时间
        }
        else                                                         // 咪头未触发时的处理（用户停止尝试吸烟）
        {
            cnt_lock_ms = 0;                                         // 重置童锁计数器
        }
        return;                                                      // 童锁状态下直接返回，不执行正常的加热控制逻辑
    }


#if ECIG_ADC2_EN
    timer_hot_dual_isr();                                            // 调用双发模式中断服务函数，执行双路发热丝控制
#else                                                                // 单路ADC模式的处理分支  
    timer_hot_single_isr();                                          // 调用单发模式中断服务函数，执行单路发热丝控制
#endif

}

/**
  * @brief  电子烟定时器初始化函数
  * @note   初始化TMR3定时器，设置为50μs周期中断，用于电子烟系统的实时控制
  *         配置时钟门控、定时器参数、中断优先级和中断服务函数
  * @param  None
  * @retval None
  */
void ecig_timer_init(void)  //使用TMR3
{
    CLKGAT1 |= BIT(8);                                               // 使能TMR3时钟门控，开启定时器3的时钟源
    TMR3CNT = 0;                                                     // 清零TMR3计数器，从0开始计数
    TMR3PR = 13 * ECIG_TIMER_US - 1;  //ECIG_TIMER_US                // 设置定时器预分频值，计算公式：13 * 50μs - 1，确定中断周期
    TMR3CPND = BIT(9) | BIT(10) | BIT(12);                           // 清除TMR3中断挂起标志位，清空之前的中断状态
    TMR3CON = BIT(7) | (1<<4) | (2<<1) | BIT(0);  // xosc26_div2     // 配置TMR3控制寄存器：使能定时器、设置时钟源为26MHz/2、启动定时器

    PICPR &= ~BIT(IRQ_TMR3_VECTOR);                                  // 清除TMR3中断优先级寄存器对应位，设置中断优先级
    PICEN |= BIT(IRQ_TMR3_VECTOR);                                   // 使能TMR3中断，允许定时器产生中断
    register_isr(IRQ_TMR3_VECTOR, ecigarette_isr);                   // 注册TMR3中断服务函数，将ecigarette_isr绑定到TMR3中断向量
}

/**
  * @brief  检查电子烟是否正在工作
  * @note   通过检查咪头状态和加热时间来判断电子烟是否正在正常工作
  *         只有咪头触发且未超过最大加热时间才认为是正在工作状态
  * @param  None
  * @retval bool: true-正在工作，false-未工作
  */
AT(.com_text.ecig)
bool ecig_is_working(void)
{
    return (ecig.mic_sta && !(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)); // 返回工作状态：咪头触发且加热时间未超过最大值
}

/**
  * @brief  获取电子烟当前工作状态
  * @note   返回电子烟的详细工作状态，包括空闲、吸烟、短路、低电量等状态
  *         用于上层应用判断具体的工作状态和故障类型
  * @param  None
  * @retval u8: 当前烟控状态值(IDLE、SMOKING、SHORT_CIRCUIT等)
  */
AT(.com_text.ecig)
u8 ecig_is_working_sta(void)
{
    return ecig.smoke_sta;                                           // 直接返回当前烟控状态变量
}

/**
  * @brief  电子烟系统初始化函数
  * @note   初始化所有硬件模块和软件参数
  *         包括GPIO配置、定时器参数计算、各个子模块初始化等
  *         必须在使用电子烟功能前调用此函数
  * @param  cfg: 电子烟配置结构体指针，包含所有硬件配置参数
  * @retval None
  */
void ecigarette_init(ecig_cfg_t* cfg)
{
    if (cfg == NULL)                                                 // 检查配置参数指针是否有效
    {
        return;                                                      // 配置参数无效时直接返回，避免空指针访问
    }

    memset(&ecig, 0, sizeof(ecig_cb_t));                             // 清零电子烟控制块结构体，确保初始状态干净
    ecig.timer_20ms_cnt = (20000 / ECIG_TIMER_US);                   // 计算20ms对应的定时器计数值，用于防抖机制(20000μs / 50μs = 400)
    ecig.timer_1s_cnt   = (1000000 / ECIG_TIMER_US);                 // 计算1秒对应的定时器计数值，用于秒级计时(1000000μs / 50μs = 20000)
    ecig.timer_big_cycles_10ms = (10000  / ECIG_TIMER_US);           // 计算10ms对应的定时器计数值，用于功率控制周期(10000μs / 50μs = 200)
    ecig.cfg = cfg;                                                  // 保存配置结构体指针到全局控制块中

    ecig_pwm_io_init();                                              // 初始化PWM输出GPIO引脚，配置为输出模式
    ecig_ven_io_init();                                              // 初始化VEN电源控制GPIO引脚，配置为输出模式
    ecig_adgnd_io_deinit();                                          // 反初始化ADC接地GPIO引脚，释放GPIO资源
    ecig_mic_io_init();                                              // 初始化咪头检测GPIO引脚，配置为输入模式
    ecig_det_io_init();                                              // 初始化烟弹插拔检测GPIO引脚，配置为输入模式
    ecig_hot_detect_init();                                          // 初始化发热丝检测ADC功能，启动ADC采样
    ecig_timer_init();                                               // 初始化定时器，启动50μs周期中断
}

/**
  * @brief  电子烟系统休眠唤醒处理函数
  * @note   在系统从休眠状态唤醒时调用，重新初始化必要的硬件模块
  *         不需要重新初始化软件参数，只恢复硬件状态和定时器
  * @param  cfg: 电子烟配置结构体指针
  * @retval None
  */
void ecigarette_sleep_wakeup(ecig_cfg_t* cfg)                        // 电子烟休眠唤醒处理函数
{
    if (cfg == NULL)                                                 // 检查配置参数指针是否有效
    {
        return;                                                      // 配置参数无效时直接返回
    }

    ecig_pwm_io_init();                                              // 重新初始化PWM输出GPIO引脚
    ecig_mic_io_init();                                              // 重新初始化咪头检测GPIO引脚
    ecig_det_io_init();                                              // 重新初始化烟弹插拔检测GPIO引脚
    ecig_hot_detect_init();                                          // 重新初始化发热丝检测ADC功能
    ecig_timer_init();                                               // 重新初始化定时器，恢复中断
}


/**
  * @brief  电子烟系统退出函数
  * @note   用于安全关闭电子烟系统，关闭所有硬件输出和中断
  *         确保系统进入安全状态，防止意外加热
  * @param  None
  * @retval None
  */
void ecigarette_exit(void)                                           // 电子烟系统退出函数
{
    ECIG_PWM_OFF_FUNC();                                             // 关闭所有PWM输出，停止对发热丝供电
    ECIG_VEN_OFF();                                                  // 关闭VEN电源，切断整个发热丝电路供电
    PICEN &= ~BIT(IRQ_TMR3_VECTOR);                                  // 禁用TMR3中断，停止定时器中断服务
    TMR3CON = 0;                                                     // 清零TMR3控制寄存器，完全停止定时器
}


/**
  * @brief  关闭所有PWM输出函数
  * @note   同时关闭PWM1和PWM2两路输出，确保所有发热丝停止工作
  *         用于紧急停止和正常结束加热过程
  * @param  None
  * @retval None
  */
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保快速执行
void ECIG_PWM_OFF_FUNC(void)                                         // PWM关闭函数
{
    ECIG_PWM1_OFF();                                                 // 关闭第一路PWM输出，停止第一个发热丝工作
    ECIG_PWM2_OFF();                                                 // 关闭第二路PWM输出，停止第二个发热丝工作
}

/**
  * @brief  根据档位模式开启相应的PWM输出
  * @note   根据当前的档位设置(单发/双发)和电阻开关状态来决定开启哪路PWM
  *         单发模式：根据smoke_res_swich选择PWM1或PWM2
  *         双发模式：同时开启PWM1和PWM2
  * @param  None
  * @retval None
  */
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保实时响应
void ECIG_PWM_ON_FUNC(void)                                          // PWM开启函数
{
    if(!ecig.cfg->smoke_position_swich)                              // 检查是否为单发模式(smoke_position_swich=0表示单发)
    {
        if(ecig.cfg->smoke_res_swich)                                // 检查单发模式下的电阻开关状态
        {
            ECIG_PWM1_ON();                                          // 开启第一路PWM输出，激活第一个发热丝
        }
        else                                                         // 电阻开关为另一状态时
        {
            ECIG_PWM2_ON();                                          // 开启第二路PWM输出，激活第二个发热丝
        }
    }
    else                                                             // 双发模式的处理分支
    {
        ECIG_PWM1_ON();                                              // 开启第一路PWM输出
        ECIG_PWM2_ON();                                              // 开启第二路PWM输出，双发模式同时激活两个发热丝
    }

}

///---------------------------------------------（ADC相关）----------------------------------------------------


/**
  * @brief  初始化HOT电路ADC功能
  * @note   初始化发热丝电路的ADC采样功能，根据是否启用双路ADC
  *         选择不同的采样通道组合，完成初始化后立即进行一次电压采集
  * @param  None
  * @retval None
  */
void ecig_hot_detect_init(void)
{
    saradc_kick_start_do(ecig.cfg->adc2_en ? BIT(ecig.cfg->adc1_ch) : BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch), 0, 0); // 根据ADC2使能状态选择采样通道：双路模式仅采样adc1_ch，单路模式同时采样adc1_ch和adc_res1_ch
    while(!saradc_is_finish());                                      // 等待ADC采样完成，确保数据有效
    ecig_hot_proc();                                                 // 调用电压处理函数，读取并处理ADC采样结果
}

/**
  * @brief  读取HOT电路电压并进行校准计算
  * @note   读取发热丝电压ADC值，通过内部基准电压进行校准
  *         计算出实际的毫伏值和定点数格式，并启动下一次ADC采样
  * @param  None
  * @retval None
  */
AT(.com_text.ecig.process)
void ecig_hot_proc(void)
{
    if(!saradc_is_finish())                                          // 检查ADC采样是否完成
    {
        TRACE(not_finish_str, 1, SADCCH, BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH); // 如果ADC未完成则打印调试信息：标识码1，系统ADC通道，预期采样通道组合
    }

    u32 adc_vbg = saradc_get_value10(ADCCH_BGOP);                                                            // 获取内部基准电压(VBG)的ADC采样值，用于电压校准
    u32 hot_voltage = saradc_get_value10(ecig.cfg->adc1_ch);                                                 // 获取第一路发热丝电压的ADC采样值
    ecig.AD_hot_voltage_mv = (hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg) * 4 / 3 / ECIG_VBG_VOLTAGE_MULTIPLE; // 电压校准计算：通过基准电压比值校准，乘以4/3分压补偿系数，除以电压倍数
    ecig.AD_hot_voltage = (ecig.AD_hot_voltage_mv << 13) / 1000;                                             // 将毫伏值转换为定点数格式：左移13位(乘8192)后除1000，提高计算精度

    saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);                           // 启动下一次ADC采样：发热丝电压通道、基准电压通道和系统ADC通道
}

/**
  * @brief  电阻检测ADC数据处理
  * @note   读取发热丝电压和电阻检测通道的ADC值，用于后续的电阻计算
  *         确保ADC15通道配置为内部基准电压模式，并启动下一次采样
  * @param  None
  * @retval None
  */
AT(.com_text.ecig.process)
void ecig_res_proc(void)
{
    if(!saradc_is_finish())                                          // 检查ADC采样是否完成
    {
        TRACE(not_finish_str, 4, SADCCH, BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch) | SADCCH);
    }

    ecig.adc1 = saradc_get_value10(ecig.cfg->adc1_ch);               // 读取第一路发热丝电压的ADC原始值
    ecig.adc_res1 = saradc_get_value10(ecig.cfg->adc_res1_ch);       // 读取第一路电阻检测通道的ADC原始值

    if (!saradc_adc15_is_bg())                                       // 检查ADC15通道是否已配置为内部基准电压模式
    {
        saradc_adc15_analog_select(ADCCH15_ANA_BG);                  // 将ADC15通道切换到内部基准电压(BG)模式，确保电压校准准确性
    }

    saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc_res1_ch) | SADCCH, 0, 0); // 启动下一次ADC采样：发热丝电压通道、电阻检测通道和系统ADC通道
}
///------------------------------------------------------------------------------------------------------------
#endif
