#include "include.h"
#include "ute_module_smoke.h"
#if ECIG_POWER_CONTROL


ecig_cfg_t ecig_cfg;

/**
  * @brief  电子烟BSP层初始化函数
  * @note   该函数负责初始化电子烟系统的所有硬件配置，包括GPIO引脚分配、
  *         ADC通道配置、电阻检测参数、安全保护参数等
  *         根据不同的开发板类型和项目支持情况进行条件编译配置
  * @param  None
  * @retval None
  */
void bsp_ecig_init(void)                                            // BSP电子烟初始化主函数
{
    printf("%s\\n", __func__);                                      // 打印当前函数名，用于调试跟踪
    ecig_cfg_t *p = &ecig_cfg;                                      // 获取全局配置结构体的指针，便于后续操作
    memset(p, 0, sizeof(ecig_cfg_t));                               // 将配置结构体清零，确保初始状态干净

    p->io_pwm1 = ECIG_PWM1_GPIO;                                    // 配置PWM1输出引脚，用于控制第一路发热丝
    p->io_mic = ECIG_MIC_GPIO;                                      // 配置咪头检测引脚，用于检测用户吸烟动作

    p->io_adgnd = ECIG_GND_GPIO;                                    // 配置ADC接地引脚，为ADC采样提供稳定基准
    p->adc1_ch = ECIG_ADC1_GPIO;                                    // 配置第一路ADC通道，用于检测第一路发热丝电压

#if ECIG_ADC2_EN                                                    // 如果启用第二路ADC功能  

#if PROJECT_ABE013_SUPPORT
    p->adc2_en = 1;                                                 // 启用第二路ADC功能标志
    p->io_pwm2 = ECIG_PWM1_GPIO,                                    // 配置PWM2输出引脚(注意：这里使用PWM1引脚，可能是硬件复用)
       p->io_ven = ECIG_VEN_GPIO;                                      // 配置VEN电源控制引脚，用于控制发热丝电路供电

    p->adc2_ch = ECIG_ADC1_GPIO;                                    // 配置第二路ADC通道(注意：这里使用ADC1通道，可能是硬件复用)

    p->adc_res1_ch = ECIG_ADC_RES1_GPIO;                            // 配置第一路电阻检测ADC通道
    p->adc_res2_ch = ECIG_ADC_RES1_GPIO;                            // 配置第二路电阻检测ADC通道(注意：使用RES1通道，可能是硬件复用)
#else
    p->adc2_en = 1;                                                 // 启用第二路ADC功能标志
    p->io_pwm2 = ECIG_PWM2_GPIO,                                    // 配置PWM2输出引脚，使用独立的PWM2引脚
       p->io_ven = ECIG_VEN_GPIO;                                   // 配置VEN电源控制引脚
    p->adc_res1_ch = ECIG_ADC_RES1_GPIO;                            // 配置第一路电阻检测ADC通道
    p->adc2_ch = ECIG_ADC2_GPIO;                                    // 配置第二路ADC通道，使用独立的ADC2通道
    p->adc_res2_ch = ECIG_ADC_RES2_GPIO;                            // 配置第二路电阻检测ADC通道，使用独立的RES2通道
#endif


#if DEVELOPMENT_BOARD_TYPE == DEVELOPMENT_BOARD_USER
    p->res_diff = (u16)(10.0* 8192);                                // 设置分压电阻值为10Ω，乘以8192进行定点数转换
    p->short_res_proportion = 22;                                   //计算方法：res_diff/res_min + 1(去掉小数部分),例res_diff = 4.7Ω，res_min = 0.4Ω // 设置短路检测比例阈值，用于ADC比值计算
    p->open_res_proportion  = 4;                                    //计算方法：res_diff/res_max + 1(有小数进一),例res_diff = 4.7Ω，res_max = 2Ω // 设置开路检测比例阈值，用于ADC比值计算
#else                                                               // 开发版本开发板配置  
    p->res_diff = (u16)(4.7* 8192);                                 // 设置分压电阻值为4.7Ω，乘以8192进行定点数转换
    p->short_res_proportion = 12;                                   //计算方法：res_diff/res_min + 1(去掉小数部分),例res_diff = 4.7Ω，res_min = 0.4Ω // 设置短路检测比例阈值
    p->open_res_proportion  = 4;                                    // 设置开路检测比例阈值
#endif

#else                                                               // 如果未启用第二路ADC功能  
    p->res_wire = (u16)(1.0 * 8192);                                // 设置固定电阻丝阻值为1.0Ω，乘以8192进行定点数转换

#endif

#if ECIG_DET_EN
    p->io_hot_det1 = ECIG_DET1_GPIO;                                // 配置第一路烟弹检测引脚
    p->io_hot_det2 = ECIG_DET2_GPIO;                                // 配置第二路烟弹检测引脚
    p->io_hot_mode = true;                                          // 启用烟弹检测模式标志
#endif

    //p->aim_power = 5,                                             // 注释掉的目标功率设置
    //ecig_set_power(15);                                           // 注释掉的功率设置函数调用
    ecig_cfg.aim_voltage = 3290;                                    // 设置目标电压为3290mV，用于功率控制
    p->heat_time_max = 8;                                           // 设置最大加热时间为8秒，用于超时保护
    p->short_res_prop = 14;                                         // 设置短路保护电阻比例阈值
    p->open_res_prop = 320;                                         // 设置开路保护电阻比例阈值
    test_1st_gear_func();                                           // 调用一档功能测试函数，初始化档位设置
    ecigarette_init(p); //需要用到硬件定时器TMR3                      // 调用电子烟核心初始化函数，传入配置参数，启动TMR3定时器
}

/**
  * @brief  电子烟休眠唤醒处理函数
  * @note   该函数在系统从休眠状态唤醒时调用，用于恢复电子烟的工作状态
  *         主要处理GPIO状态恢复和中断重新配置
  * @param  None
  * @retval None
  */
void bsp_ecig_sleep_wakeup(void)                                     // 电子烟休眠唤醒处理函数
{
    ecig_cfg_t *p = &ecig_cfg;                                       // 获取全局配置结构体指针
    ecigarette_sleep_wakeup(p);                                      // 调用电子烟核心唤醒函数，传入配置参数
}




/**
  * @brief  获取电池电压值
  * @note   该函数封装了底层电池电压获取接口，用于电子烟系统的电池电压监控
  *         返回的电压值用于欠压保护和电量显示
  * @param  None
  * @retval u32: 电池电压值(毫伏)
  */
AT(.com_text.ecig.process)                                           // 将函数放置在电子烟处理代码段，优化内存布局
u32 ecig_vbat_get(void)                                              // 电池电压获取函数
{
    return bsp_vbat_get_voltage(0);                                  // 调用BSP层电池电压获取函数，参数0表示使用默认配置
}
typedef struct
{
    u16 power;   // 功率值
    u16 voltage; // 对应电压值
} PowerVoltagePair;
//单发0.8欧
const PowerVoltagePair power_voltage_singe_limi_0_8[]=
{
    {10,2690},
    {11,2740},
    {12,2790},
    {13,2840},
    {14,3050},
    {15,3290},
    {16,3480},
    {17,3530},
    {18,3570},
};
//单发0.8欧
const PowerVoltagePair power_voltage_dual_limi_0_4[]=
{
    {20,3010},
    {21,3060},
    {22,3110},
    {23,3160},
    {24,3260},
    {25,3310},
    {26,3410},
    {27,3440},
    {28,3460},
    {29,3510},
    {30,3560},
};
//单发1.2
const PowerVoltagePair power_voltage_singe_limi_1_2[]=
{
    {10,3350},
    {11,3400},
    {12,3440},
    {13,3480},
    {14,3520},
    {15,3560},
    {16,3600},
    {17,3640},
    {18,3680},
    {19,3720},
    {20,3760},
};
//双发1.2
const PowerVoltagePair power_voltage_dual_limi_1_2[]=
{
    {20,2940},
    {21,2960},
    {22,2980},
    {23,3000},
    {24,3020},
    {25,3040},
    {26,3060},
    {27,3080},
    {28,3100},
    {29,3120},
    {30,3140},
};
/**
  * @brief  获取当前设置的功率值
  * @note   该函数返回全局烟雾模块数据中存储的当前功率值
  *         用于其他模块查询当前的功率设置
  * @param  None
  * @retval u8: 当前功率值(W)
  */
u8 ecig_get_power(void)                                              // 功率获取函数
{
    return uteModuleSmokeData.current_power ;                        // 直接返回全局烟雾数据结构中的当前功率值
}

/**
  * @brief  设置第一路发热丝电阻值
  * @note   该函数用于设置第一路发热丝的电阻值到配置结构体中
  *         函数放置在中断代码段以提高执行效率
  * @param  value: 电阻值(定点数格式，需乘以8192)
  * @retval None
  */
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，优化执行效率
void ecig_set_res(u8 value)                                          // 第一路电阻设置函数
{
    ecig_cfg.res_wire = value;                                       // 将传入的电阻值保存到配置结构体的第一路电阻字段
}

/**
  * @brief  设置第二路发热丝电阻值
  * @note   该函数用于设置第二路发热丝的电阻值到配置结构体中
  *         函数放置在中断代码段以提高执行效率
  * @param  value: 电阻值(定点数格式，需乘以8192)
  * @retval None
  */
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，优化执行效率
void ecig_set_res2(u8 value)                                         // 第二路电阻设置函数
{
    ecig_cfg.res_wire2 = value;                                      // 将传入的电阻值保存到配置结构体的第二路电阻字段
}

/**
  * @brief  获取发热丝电阻值并转换为整数格式
  * @note   该函数将存储的定点数格式电阻值转换为便于显示的整数格式
  *         支持调试模式打印，包含四舍五入处理和ADC值校正
  *         根据单发/双发模式返回不同的电阻值
  * @param  None
  * @retval uint8_t: 电阻值的整数表示(如8表示0.8Ω，12表示1.2Ω)
  */
#define DEBUG 0
uint8_t ecig_get_res(void)                                           // 电阻获取和转换函数
{
#if DEBUG                                                            // 条件编译：如果开启调试模式  
    printf("%s, res_wire=%d\\n", __func__, ecig_cfg.res_wire);       // 打印函数名和原始电阻值，用于调试
#endif                                                               // 结束调试模式条件编译  
    // 使用更大的数据类型防止溢出，并提高精度                        // 注释：说明使用32位数据类型的原因
    uint32_t data_temp = (uint32_t)ecig_cfg.res_wire * 1000;         // 将电阻值乘以1000，转换为32位避免溢出，提高计算精度
    uint32_t result = data_temp / 8192;                              // 除以8192还原定点数，得到以毫欧为单位的电阻值
    uint16_t data_temp1 = 0;                                         // 声明16位临时变量，用于中间计算
    uint8_t data = 0;                                                // 声明8位最终结果变量，存储转换后的电阻值

    if(result<1000&&result>0)                                        // 判断电阻值是否在0-1000毫欧范围内(即0-1Ω)
    {
        // 对 result 进行四舍五入处理
        data_temp1 = (uint16_t)((result + 10) / 10);                 // 第一次四舍五入：加10后除以10，将毫欧转换为0.01Ω单位
        data = (uint8_t)((data_temp1 + 10) / 10);                    // 第二次四舍五入：加10后除以10，转换为0.1Ω单位的整数表示
    }
    else if(result>=1000)                                            // 判断电阻值是否大于等于1000毫欧(即1Ω以上)
    {
        // 对 result 进行四舍五入处理
        data_temp1 = (uint16_t)((result + 15) / 10);                 // 第一次四舍五入：加15后除以10，使用不同的舍入策略
        data = (uint8_t)((data_temp1 + 15) / 10);                    // 第二次四舍五入：加15后除以10，转换为0.1Ω单位的整数表示
    }

#if DEBUG
    printf("%s, data_temp1=%d,result=%lu\\n", __func__,data_temp1, result); // 打印中间计算结果，用于调试验证
    printf("%s, data=%d\\n", __func__, data);                        // 打印最终转换结果，用于调试验证
#endif

    if(ecig.adc1<60)
        data =4;
    else if(ecig.adc1>=60 && ecig.adc1<=70)
        data =7;
    else if(ecig.adc1>70)
        data =10;

    if(ecig_cfg.smoke_position_swich == 0)                           // 检查是否为单发模式(smoke_position_swich=0表示单发)
    {
        return data;                                                 // 单发模式直接返回计算得到的电阻值
    }
    else                                                             // 双发模式的处理分支
    {
        return data/2;                                               // 双发模式返回电阻值的一半(因为两个电阻并联，总阻值减半)
    }

//printf("%s, data=%u\\n", __func__, data);  //
}
//单发
/**
  * @brief  设置电子烟为单发模式（第一档）
  * @note   该函数将电子烟系统设置为单发模式，同时更新全局数据和配置结构体
  *         单发模式下只有一个发热丝工作，功率范围通常为10-20W
  * @param  None
  * @retval None
  */
void test_1st_gear_func(void)                                        // 单发模式设置函数
{
    printf("%s \\n",__func__);                                       // 打印当前函数名，用于调试跟踪
    uteModuleSmokeData.smoke_position_swich = 0;                     // 设置全局烟雾数据中的档位开关为0（单发模式）
    ecig_cfg.smoke_position_swich = 0;                               // 设置电子烟配置结构体中的档位开关为0（单发模式）
}


/**
  * @brief  根据档位和电阻值设置电子烟功率
  * @note   该函数根据当前档位（单发/双发）和电阻值（0.8Ω/1.2Ω）选择对应的
  *         功率-电压查找表，将用户设置的功率值转换为目标电压值
  *         支持四种组合：单发0.8Ω、单发1.2Ω、双发0.4Ω、双发0.6Ω
  * @param  value: 目标功率值(W)
  * @retval None
  */
void ecig_set_power(u16 value)                                       // 功率设置主函数
{
    uint8_t res = ecig_get_res();                                    // 获取当前电阻值（经过计算的整数形式，如8表示0.8Ω）
    printf("get_gear_func()==%d && res== %d\\n",get_gear_func(),res);

    if(get_gear_func()==0 && res== 8)                               // 判断是否为单发模式且电阻为0.8Ω
    {
        // ecig_cfg.aim_voltage = 2650 ;
        // uteModuleSmokeData.current_power=value;
        for (int i = 0; i < sizeof(power_voltage_singe_limi_0_8) / sizeof(power_voltage_singe_limi_0_8[0]); i++) // 遍历单发0.8Ω功率-电压对应表
        {
            if (power_voltage_singe_limi_0_8[i].power == value)      // 查找匹配的功率值
            {
                ecig_cfg.aim_voltage = power_voltage_singe_limi_0_8[i].voltage; // 设置对应的目标电压值
                break;
            }
        }
    }
    else if(get_gear_func()==1 && res== 4)                          // 判断是否为双发模式且电阻为0.4Ω（两个0.8Ω并联）
    {
        for (int i = 0; i < sizeof(power_voltage_dual_limi_0_4) / sizeof(power_voltage_dual_limi_0_4[0]); i++) // 遍历双发0.4Ω功率-电压对应表
        {
            if (power_voltage_dual_limi_0_4[i].power == value)                  // 查找匹配的功率值
            {
                ecig_cfg.aim_voltage = power_voltage_dual_limi_0_4[i].voltage;  // 设置对应的目标电压值
                break;
            }
        }
    }
    else if(get_gear_func()==0 && res== 12)                         // 判断是否为单发模式且电阻为1.2Ω
    {
        for (int i = 0; i < sizeof(power_voltage_singe_limi_1_2) / sizeof(power_voltage_singe_limi_1_2[0]); i++) // 遍历单发1.2Ω功率-电压对应表
        {
            if (power_voltage_singe_limi_1_2[i].power == value)                 // 查找匹配的功率值
            {

                ecig_cfg.aim_voltage = power_voltage_singe_limi_1_2[i].voltage; // 设置对应的目标电压值
                break;
            }
        }
    }
    else if(get_gear_func()==1 && res== 6)                           // 判断是否为双发模式且电阻为0.6Ω（两个1.2Ω并联）
    {
        for (int i = 0; i < sizeof(power_voltage_dual_limi_1_2) / sizeof(power_voltage_dual_limi_1_2[0]); i++) // 遍历双发0.6Ω功率-电压对应表
        {
            if (power_voltage_dual_limi_1_2[i].power == value)                 // 查找匹配的功率值
            {

                ecig_cfg.aim_voltage = power_voltage_dual_limi_1_2[i].voltage; // 设置对应的目标电压值
                break;
            }
        }
    }
    uteModuleSmokeData.current_power = value;                        // 将设置的功率值保存到全局数据结构中
}

/**
  * @brief  设置电子烟为双发模式（第二档）
  * @note   该函数将电子烟系统设置为双发模式，同时更新全局数据和配置结构体
  *         双发模式下两个发热丝同时工作，功率范围通常为20-30W
  * @param  None
  * @retval None
  */
//双发
void test_2st_gear_func(void)                                        // 双发模式设置函数
{
    printf("%s \\n",__func__);
    uteModuleSmokeData.smoke_position_swich = 1;                     // 设置全局烟雾数据中的档位开关为1（双发模式）
    ecig_cfg.smoke_position_swich = 1;                               // 设置电子烟配置结构体中的档位开关为1（双发模式）
}

/**
  * @brief  获取当前档位模式
  * @note   该函数同步全局数据到配置结构体，并返回当前的档位模式
  *         确保配置结构体与全局数据保持一致
  * @param  None
  * @retval u8: 档位模式值（0=单发，1=双发）
  */
//单发
u8 get_gear_func(void)                                               // 档位获取函数（带同步功能）
{
    ecig_cfg.smoke_position_swich = uteModuleSmokeData.smoke_position_swich; // 将全局数据中的档位设置同步到配置结构体
    //  printf("%s ,%d\\n",__func__,ecig_cfg.smoke_position_swich);
    return ecig_cfg.smoke_position_swich;                                    // 返回当前档位模式（0=单发，1=双发）

}

/**
  * @brief  直接获取全局数据中的档位模式
  * @note   该函数直接返回全局烟雾数据中存储的档位模式值
  *         不进行任何同步操作，仅用于查询
  * @param  None
  * @retval u8: 档位模式值（0=单发，1=双发）
  */
u8 get_gear_func1(void)                                              // 档位获取函数（直接查询）
{
    // printf("%s \\n",__func__);
    return uteModuleSmokeData.smoke_position_swich;                  // 直接返回全局数据中的档位模式值

}
#else
void ecig_set_power(u8 value)
{

}

u8 ecig_get_power(void)
{
    return 0;
}

void ecig_set_res(u8 value)
{

}

u8 ecig_get_res(void)
{
    return 0;
}
u8 get_gear_func1(void)
{


}
u8 get_gear_func(void)
{


}
void test_1st_gear_func(void)
{

}
void test_2st_gear_func(void)
{

}
#endif
