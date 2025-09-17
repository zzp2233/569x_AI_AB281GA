#include "include.h"
#include "ute_module_message.h"
#include "ute_module_lockScreen.h"
#if ECIG_POWER_CONTROL

#if ECIG_POLLING_CONTROL

#define TRACE_EN                             1


#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif


#if TRACE_EN
AT(.com_rodata.str)
const char hot_str[] = "%d %d %d\n";
AT(.com_rodata.str)
const char vol_hot_str[] = "123123  %d %d %d\n";
AT(.com_rodata.str)
const char short_circuit_str[] = "short circuit! %d sta [%d]\n";
AT(.com_rodata.str)
const char info_8s[] = "8S warning\n";
AT(.com_rodata.str)
const char hot_str11[] = "smoke_res_swich%d\n";
AT(.com_rodata.str)
const char vol_hot_test_str[] = " %d %d %d %d %d %d\n";

#endif

void ecig_clear_short_flag(bool state)
{
    ecig.short_circuit_flag = state;
}

/**
  * @brief  读取发热丝两路电压并换算成内部使用的固定点格式
  * @param  None
  * @retval None

    adc1_ch：第一路发热丝电压检测通道
    adc2_ch：第二路发热丝电压检测通道

    ecig.AD_hot_voltage_mv  = (hot_voltage  * ECIG_VBG_VOLTAGE / adc_vbg) * 48 / 33 / ECIG_VBG_VOLTAGE_MULTIPLE;
    计算步骤：
                基准校准：hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg  通过基准电压比值校准
                分压补偿：* 48/33 或者* 4 / 3  ADC测量值是分压后的电压，需要还原为实际电压（测量的时候缩小，现在放大）
                倍数校正：/ ECIG_VBG_VOLTAGE_MULTIPLE  消除放大倍数影响

    ecig.AD_hot_voltage  = (ecig.AD_hot_voltage_mv  << 13) / 1000;
    保留小数部分的精度
    将浮点运算转换为整数运算，左移13位相当于乘以 2^13 = 8192
    除以1000是为了将毫伏转换为伏特(V)
  */
AT(.com_text.isr)                                                  // 将函数放入中断代码段，满足实时性要求
void timer_hot_hot_vol(void)                                       // 定时中断处理函数：采样并计算发热丝电压
{
    u32 adc_vbg      = saradc_get_value10(ADCCH_BGOP);             // 读取基准电压(≈1.2 V)，用于校准ADC参考电压
    u32 hot_voltage  = saradc_get_value10(ecig.cfg->adc1_ch);      // 读取发热丝1分压节点电压(R43=48k / R44=33k) -> ADC1
    u32 hot_voltage2 = saradc_get_value10(ecig.cfg->adc2_ch);      // 读取发热丝2分压节点电压(若存在双发热丝) -> ADC2
    // TRACE(hot_str, adc_vbg,hot_voltage,hot_voltage2);           // 调试打印：原始ADC值

#if DEVELOPMENT_BOARD_TYPE == DEVELOPMENT_BOARD_USER
    ecig.AD_hot_voltage_mv  = (hot_voltage  * ECIG_VBG_VOLTAGE / adc_vbg) * 48 / 33 / ECIG_VBG_VOLTAGE_MULTIPLE;  // 分压比48k/33k，计算实际发热丝1电压(mV)
    ecig.AD_hot_voltage_mv2 = (hot_voltage2 * ECIG_VBG_VOLTAGE / adc_vbg) * 48 / 33 / ECIG_VBG_VOLTAGE_MULTIPLE;  // 同上，用于发热丝2
    TRACE(hot_str, 9999,ecig.AD_hot_voltage_mv,ecig.AD_hot_voltage_mv2);                                          // 换算后的电压值
#else
    ecig.AD_hot_voltage_mv  = (hot_voltage  * ECIG_VBG_VOLTAGE / adc_vbg) * 4 / 3 / ECIG_VBG_VOLTAGE_MULTIPLE;    // 分压比4k/3k，计算发热丝1电压(mV)
#endif
    ecig.AD_hot_voltage  = (ecig.AD_hot_voltage_mv  << 13) / 1000;                                                // 将毫伏值转换成Q13固定点格式供控制算法使用
    ecig.AD_hot_voltage2 = (ecig.AD_hot_voltage_mv2 << 13) / 1000;                                                // 同上，针对发热丝2
}

/**
  * @brief  启动电阻检测ADC采样
  * @note   该函数用于启动多通道ADC采样，包括发热丝电压和电阻检测通道
  *         在电阻测量前需要确保ADC15通道配置为内部基准电压模式
  * @param  None
  * @retval None
  */
//获取电阻ADC
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，优化执行效率
void timer_hot_res_kick(void)//                                      // 电阻检测ADC启动函数
{
    if (!saradc_adc15_is_bg())                                       // 检查ADC15通道是否已配置为内部基准电压模式
    {
        saradc_adc15_analog_select(ADCCH15_ANA_BG);                  // 将ADC15通道切换到内部基准电压(BG)模式，用于电压校准
    }
    saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) |                   // 启动多通道ADC同时采样：
                         BIT(ecig.cfg->adc_res1_ch)|                 // - 第一路发热丝电压检测通道(adc1_ch)
                         BIT(ecig.cfg->adc2_ch) |                    // - 第一路电阻检测通道(adc_res1_ch)
                         BIT(ecig.cfg->adc_res2_ch) |                // - 第二路发热丝电压检测通道(adc2_ch)
                         SADCCH, 0, 0);                              // - 第二路电阻检测通道(adc_res2_ch)和其他系统ADC通道
}

/**
  * @brief  电阻计算和ADC数据处理
  * @note   该函数负责读取ADC采样结果并计算发热丝电阻值
  *         包含ADC采样完成检测、数据读取、电阻计算和硬件控制
  *         采用状态机方式，确保ADC采样和计算的时序正确性
  * @param  None
  * @retval None
  */
//电阻计算
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保实时性
void timer_hot_res_cul(void)//                                       // 电阻计算主函数
{
    if (saradc_is_finish() && ecig.timer_switch_adc_flag)            // 检查ADC采样是否完成且ADC标志位有效
    {
        ecig.timer_switch_acc_cnt ++;                                // 递增状态机计数器，用于控制电阻检测流程
        ecig.timer_switch_adc_flag = false;                          // 清除ADC标志位，防止重复处理

        //电阻1                                                       // 第一路发热丝电阻计算
        ecig.adc1 = saradc_get_value10(ecig.cfg->adc1_ch);           // 读取第一路发热丝电压ADC值(10位精度)
        ecig.adc_res1 = saradc_get_value10(ecig.cfg->adc_res1_ch);   // 读取第一路电阻检测ADC值(10位精度)
        caculate_res();                                              // 调用第一路电阻计算函数，根据电压差计算实际电阻值

        //电阻2                                                       // 第二路发热丝电阻计算
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
            timer_hot_res_kick();                                    // 调用ADC启动函数，开始新一轮的电阻检测采样
            ecig.timer_switch_adc_flag = true;                       // 设置ADC标志位为真，标记ADC采样已启动
        }
    }
}
/**
  * @brief  电池电压检测和欠压保护处理
  * @note   该函数用于检测电池电压并进行欠压保护，当电池电压低于3.3V时
  *         会停止PWM输出并设置低电量状态，防止电池过度放电
  *         同时启动电池电压和基准电压的ADC采样
  * @param  None
  * @retval None
  */
//计算电池电量
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保实时性
void timer_hot_bat_vol(void)//                                       // 电池电压检测主函数
{
    if (saradc_is_finish() && ecig.timer_switch_adc_flag)            // 检查ADC采样是否完成且ADC标志位有效
    {
        ecig.timer_switch_acc_cnt ++;                                // 递增状态机计数器，用于控制电池检测流程
        ecig.timer_switch_adc_flag = false;                          // 清除ADC标志位，防止重复处理

        ecig.AD_BAT_voltage_mv = ecig_vbat_get();                    // 获取电池电压值（毫伏），通过ADC采样和校准计算得出

        if(ecig.AD_BAT_voltage_mv < 3300)                            //欠压保护 // 检查电池电压是否低于3.3V欠压保护阈值
        {
            ECIG_PWM_OFF_FUNC();                                     // 立即关闭PWM输出，停止对发热丝供电
            ecig.smoke_sta = LOW_POWER;                              // 设置烟控状态为低电量状态
            ecig.power_on_flag = 0;                                  // 清除功率开启标志，禁止后续加热操作
            // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0); // 注释掉的消息发送（旧版本接口）
            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                    // 向消息队列发送电子烟提醒事件，通知上层应用
            sys_cb.smoke_index = LOW_POWER;                          // 设置系统控制块中的烟控索引为低电量状态
            printf(hot_str,13,ecig.AD_BAT_voltage_mv,0);             // 打印调试信息：标识码13，当前电池电压，占位符0
        }
        else                                                         // 电池电压正常的处理分支
        {
            //ECIG_PWM1_ON();                                        // 注释掉的PWM开启（在其他地方控制）
            ecig.smoke_sta = SMOKING;                                // 设置烟控状态为正在吸烟状态
            ecig.power_on_flag = 1;                                  // 设置功率开启标志，允许后续加热操作
            //printf(hot_str,14,ecig.AD_BAT_voltage_mv,0);           // 注释掉的调试打印（正常电压时）
        }
        //printf(hot_str,55,ecig.AD_BAT_voltage_mv,ecig.timer_cycle_cnt); // 注释掉的周期性调试打印
    }
    else                                                             // ADC采样未完成或标志位无效时的处理
    {
        if(!ecig.timer_switch_adc_flag)                              // 检查ADC标志位是否为假（未启动采样）
        {
            ecig.timer_switch_adc_flag = true;                       // 设置ADC标志位为真，标记ADC采样已启动
            ECIG_PWM_OFF_FUNC();                                     // 关闭PWM输出，避免PWM信号干扰电压测量
            if (!saradc_adc15_is_bg())                               // 检查ADC15通道是否已配置为内部基准电压模式
            {
                saradc_adc15_analog_select(ADCCH15_ANA_BG);          // 将ADC15通道切换到内部基准电压(BG)模式，用于电压校准
            }
            saradc_kick_start_do(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH, 0, 0); // 启动ADC采样：电池电压通道、基准电压通道和其他系统通道
        }

        //printf(hot_str,66,ecig.AD_BAT_voltage_mv,ecig.timer_cycle_cnt); // 注释掉的等待期间调试打印
    }
}


/**
  * @brief  检查电子烟是否满足工作条件
  * @note   该函数检查多个安全条件，包括咪头状态、超时时间、电量状态、
  *         短路开路检测、设备开关机状态和童锁状态
  *         只有所有条件都满足时才允许电子烟开始工作
  * @param  None
  * @retval bool: true-满足工作条件，false-不满足工作条件
  */
//开始工作进行判断
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保实时性
bool timer_hot_mic_work(void)                                        // 工作条件检查函数，返回布尔值
{
    if (ecig.mic_sta //mic工作状态                                    // 检查咪头是否被触发（用户是否在吸烟）
        && (ecig.hot_time_cnt < ecig.cfg->heat_time_max)             // 检查加热时间是否未超过最大允许时间（通常8秒）
        && (ecig.smoke_sta != LOW_POWER)                             // 检查当前状态是否不是低电量状态
        && (ecig.short_circuit_flag != SHORT_CIRCUIT)                // 检查是否未检测到短路故障
        && (ecig.poweroff_flag != 0)                                 // 检查设备是否未处于关机状态（非0表示开机）
        && (uteModulePasswordData.password_flag != 1)                // 检查是否未启用童锁功能（1表示童锁开启）
        &&  (ecig.short_circuit_flag != OPEN_CIRCUIT))               // 检查是否未检测到开路故障
    {
        return true;                                                 // 所有安全条件都满足，返回真值允许工作
    }
    return false;                                                    // 任一安全条件不满足，返回假值禁止工作
}



/**
  * @brief  电子烟双发模式主控制中断服务函数
  * @note   该函数是电子烟系统的核心控制函数，以50μs周期执行
  *         负责电阻检测、电池电压监控、功率控制、短路保护等所有功能
  *         包含两个主要状态机：初始化阶段(电阻检测)和正常工作阶段(功率控制)
  *         通过timer_switch_acc_cnt状态计数器控制不同的执行阶段
  * @param  None
  * @retval None
    1. 初始化阶段（Case 0-6）
        电阻检测：前6个周期进行发热丝电阻测量
        结果验证：第6个周期验证电阻值，判断是否存在短路或开路
    2. 正常工作阶段
        电池检测：Case 0-1进行电池电压检测和欠压保护
        电压稳定：Case 2-5等待PWM开启后电压稳定
        功率控制：Case 6持续进行功率累积和安全监控
    3. 安全保护机制
        短路检测：实时监控电阻比值，检测短路故障
        超时保护：8秒最大加热时间限制
        欠压保护：电池电压低于3.3V时停止加热
    4. 状态清理
        咪头释放时清理所有状态变量
        关闭PWM和VEN电源
        重置计数器和标志位
  */
AT(.com_text.isr)                                                    // 将函数放置在中断代码段，确保最高执行优先级
void timer_hot_dual_isr(void)//                                      // 双发模式主控制中断服务函数
{

    if (timer_hot_mic_work())                                        // 检查是否满足工作条件(咪头触发、无故障、未超时等7重安全检查)
    {
        if (ecig.mic_sta != ecig.mic_start)                          // 判断是否为首次触发咪头(初始化阶段)
        {
            ecig.timer_cycle_cnt++;                                  // 递增周期计数器，用于10ms大周期控制
            ecig.hot_time_flag++;                                    // 递增加热时间标志，用于8秒超时计时
            switch(ecig.timer_switch_acc_cnt)                        // 根据状态计数器执行不同的初始化阶段
            {
                case 0://采集阻值                                    // 状态0-5：电阻检测阶段，持续6个周期(300μs)
                case 1:                                              // 每个case代表一个50μs的执行周期
                case 2:                                              // 通过多个周期确保电阻测量的准确性
                case 3:                                              // 避免单次测量的偶然误差
                case 4:                                              // 为ADC采样和计算提供充足时间
                case 5:                                              // 最后一个电阻检测周期

                    //电阻计算                                       // 执行电阻计算和ADC采样
                    timer_hot_res_cul();                             // 调用电阻计算函数，进行ADC采样和电阻值计算
                    break;                                           // 跳出switch语句，等待下一个周期

                case 6:                                              // 状态6：电阻检测完成，进行结果验证和状态设置
                {
                    ECIG_VEN_OFF();                                             // 关闭VEN电源，结束电阻检测阶段的供电
                    ecig.timer_cycle_cnt = 0;                                   // 重置周期计数器，准备进入正常工作阶段
                    ecig.timer_switch_acc_cnt = 0;                              // 重置状态计数器，准备进入正常工作状态机

                    if (!ecig.hot_res)                                          // 检查电阻检测结果，0表示检测到短路或开路
                    {
                        ECIG_PWM_OFF_FUNC();                                    // 立即关闭PWM输出，停止加热
                        ecig.power_on_flag = 0;                                 // 清除功率开启标志，禁止后续加热
                        ecig.short_circuit_flag = ecig.smoke_sta;               // 设置短路标志为当前烟控状态(SHORT_CIRCUIT或OPEN_CIRCUIT)
                        printf(short_circuit_str,-1,ecig.short_circuit_flag); // 打印短路/开路调试信息
                        //  uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0); //  旧版本消息发送
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                   // 向消息队列发送电子烟提醒事件，通知上层应用
                        sys_cb.smoke_index = ecig.smoke_sta;                    // 设置系统控制块中的烟控索引为故障状态
                        //break;
                    }
                    else                                             // 电阻检测正常的处理分支
                    {
                        ecig.smoke_sta = SMOKING;                               // 设置烟控状态为正在吸烟状态
                        ecig.power_on_flag = 1;                                 // 设置功率开启标志，允许后续加热操作
                        //uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0); //  旧版本消息发送
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                   // 向消息队列发送电子烟提醒事件
                        sys_cb.smoke_index = SMOKING;                           // 设置系统控制块中的烟控索引为吸烟状态
                    }
                    printf(hot_str,6,ecig.hot_res,0xff);                        // 打印调试信息：标识码6，电阻值，固定值0xff
                    ecig.mic_start = 1;                                         // 设置咪头启动标志，标记已完成初始化，进入正常工作模式
                }
                break;                                               // 跳出switch语句
            }
        }
        else                                                         // 正常工作阶段(mic_sta == mic_start)
        {
            switch(ecig.timer_switch_acc_cnt)                        // 根据状态计数器执行不同的工作阶段
            {
                case 0:////获取电量                                  // 状态0-1：电池电压检测阶段，持续2个周期(100μs)
                case 1:                                              // 通过两个周期确保电池电压检测的稳定性
                {
                    timer_hot_bat_vol();                             // 调用电池电压检测函数，进行欠压保护检查
                }
                break;                                               // 跳出switch语句，等待下一个周期
                case 2:                                              // 状态2-5：电压稳定和功率累积阶段，持续4个周期(200μs)
                case 3:////等一段时间电压稳定，做VDDIO校准              //  等待电压稳定，进行VDDIO校准
                case 4:                                              // 为PWM开启后的电压稳定提供时间
                case 5:                                              // 最后一个电压稳定周期
                {
                    if (saradc_is_finish() && ecig.timer_switch_adc_flag) // 检查ADC采样是否完成且ADC标志位有效
                    {
                        ecig.timer_switch_adc_flag = false;                                                                     // 清除ADC标志位，防止重复处理
                        timer_hot_hot_vol();                                                                                    // 调用发热丝电压采集函数，获取实时电压值
                        //TRACE(hot_str, ecig.timer_switch_acc_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );            //  调试打印
                        ecig.timer_switch_acc_cnt ++;                                                                           // 递增状态计数器，进入下一个状态
                    }
                    else                                             // ADC采样未完成或标志位无效时的处理
                    {
                        ecig.timer_switch_adc_flag = true;                                                                      // 设置ADC标志位为真，标记ADC采样已启动
                        ECIG_PWM_ON_FUNC();                                                                                     // 开启PWM输出，开始对发热丝供电加热
                        saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc2_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0); // 启动多通道ADC采样：发热丝1、发热丝2、基准电压和其他系统通道
                    }
                    //单发                                           //  单发模式功率累积计算
                    if(!ecig.cfg->smoke_position_swich)              // 检查是否为单发模式(smoke_position_swich=0表示单发)
                    {
                        if(ecig.cfg->smoke_res_swich)                // 检查单发模式下使用哪个发热丝(smoke_res_swich选择发热丝1或2)
                        {
                            // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);              //  功率计算方式
                            ecig.p_current = (u16)ecig.AD_hot_voltage_mv + ecig.p_prev;                                         // 使用发热丝1电压累积功率(直接电压累积)
                        }
                        else                                         // 使用发热丝2的情况
                        {
                            ecig.p_current = (u16)ecig.AD_hot_voltage_mv2+ ecig.p_prev;                                         // 使用发热丝2电压累积功率
                        }
                    }
                    else                                             // 双发模式的处理分支
                    {
                        //双发                                       //  双发模式功率累积计算
                        //  ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);          //  功率计算方式1
                        //  ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage2) >> 13) + ecig.p_current);      //  功率计算方式2
                        //  ecig.p_current = (u16)ecig.AD_hot_voltage_mv + ecig.p_prev;                                  //  单独累积方式1
                        //  ecig.p_current = (u16)ecig.AD_hot_voltage_mv2  + ecig.p_current;                             //  单独累积方式2
                        ecig.p_current = ((u16)ecig.AD_hot_voltage_mv2 +(u16)ecig.AD_hot_voltage_mv)/2 + ecig.p_current; // 双发模式：使用两个发热丝电压的平均值累积功率
                    }
                    ecig.p_prev = ecig.p_current;                                                                        // 保存当前功率值作为下次累积的基础值
                }
                break;                                               // 跳出switch语句


                case 6:                                              // 状态6：持续功率控制和安全监控阶段
                {
                    if (saradc_is_finish() && ecig.timer_switch_adc_flag && ecig.power_on_flag) // 检查ADC完成、标志位有效且功率开启
                    {
                        ecig.timer_switch_acc_cnt  = 6;              // 保持状态6，持续进行功率控制
                        ecig.timer_switch_adc_flag = false;          // 清除ADC标志位
                        timer_hot_hot_vol();                         // 调用发热丝电压采集函数
                        TRACE(hot_str, 77, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv ); // 打印调试信息：标识码77，发热丝电压，电池电压
                        if (( (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) <= ecig.cfg->short_res_prop) && (ecig.cfg->smoke_position_swich || ecig.cfg->smoke_res_swich))||
                            ((ecig.AD_hot_voltage_mv2 * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2) <= ecig.cfg->short_res_prop ) && (ecig.cfg->smoke_position_swich || !ecig.cfg->smoke_res_swich) ))     // 发热丝1，2短路检测：电阻比值小于等于短路阈值
                        {
                            //短路判断                                //  检测到短路故障的处理
                            ECIG_PWM_OFF_FUNC();                      // 立即关闭PWM输出，停止加热
                            ecig.power_on_flag = 0;                   // 清除功率开启标志，禁止后续加热
                            ecig.smoke_sta = SHORT_CIRCUIT;           // 设置烟控状态为短路状态
                            ecig.short_circuit_flag = ecig.smoke_sta; // 设置短路标志为短路状态
                            //  uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0); //  旧版本消息发送
                            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);     // 向消息队列发送电子烟提醒事件
                            sys_cb.smoke_index = SHORT_CIRCUIT;       // 设置系统控制块中的烟控索引为短路状态

                            TRACE(hot_str, 88,ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);                                                                                                                    // 打印短路调试信息：标识码88，电池电压，发热丝1电压
                            TRACE(hot_str,  ecig.cfg->smoke_res_swich,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv),ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));        // 打印发热丝1短路详细信息：电阻开关状态，电压差，电阻比值

                            TRACE(hot_str, 888, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv2);                                                                                                                 // 打印发热丝2短路调试信息：标识码888，电池电压，发热丝2电压
                            TRACE(hot_str, ecig.cfg->smoke_position_swich,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2),ecig.AD_hot_voltage_mv2 * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2)); // 打印发热丝2短路详细信息：位置开关状态，电压差，电阻比值

                        }
                        // else if (((ecig.AD_hot_voltage_mv  * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) >= ecig.cfg->open_res_prop) && (ecig.cfg->smoke_position_swich || ecig.cfg->smoke_res_swich)) ||     //  开路检测条件1：发热丝1电阻比值大于等于开路阈值
                        //          ((ecig.AD_hot_voltage_mv2 * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2) >= ecig.cfg->open_res_prop) && (ecig.cfg->smoke_position_swich || !ecig.cfg->smoke_res_swich)))     //检阻部分已有开路保护，可以屏蔽 //  开路检测条件2：发热丝2电阻比值大于等于开路阈值
                        // {
                        //     //开路判断                                // 检测到开路故障的处理
                        //     ECIG_PWM_OFF_FUNC();                      // PWM关闭
                        //     ecig.power_on_flag = 0;/*  */             // 功率标志清除
                        //     ecig.smoke_sta = OPEN_CIRCUIT;            // 开路状态设置
                        //     ecig.short_circuit_flag = ecig.smoke_sta; // 开路标志设置
                        //     msg_enqueue(EVT_ECIG_SMOKE_REMINDER);     // 开路提醒消息
                        //     sys_cb.smoke_index = OPEN_CIRCUIT;        // 系统开路索引设置
                        //     TRACE(hot_str, 99,ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                        //     TRACE(hot_str,  ecig.cfg->smoke_res_swich,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv),ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));

                        //     TRACE(hot_str, 999, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv2);
                        //     TRACE(hot_str, ecig.cfg->smoke_res_swich,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2),ecig.AD_hot_voltage_mv2 * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv2));

                        // }
                        // else                                         //  正常情况处理
                        // {
                        ecig.power_on_flag = 1;                          // 设置功率开启标志为1，允许继续加热
                        //TRACE(hot_str, ecig.timer_cycle_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv ); //  正常状态调试打印
                        //  }                                           //  else结束
                    }
                    else                                                 // ADC采样未完成或标志位无效或功率未开启时的处理
                    {
                        if(ecig.power_on_flag)                           // 检查功率开启标志是否为真
                        {
                            ECIG_PWM_ON_FUNC();                                                                                         // 开启PWM输出，继续对发热丝供电
                            if(!ecig.timer_switch_adc_flag)                                                                             // 检查ADC标志位是否为假（未启动采样）
                            {
                                ecig.timer_switch_adc_flag = true;                                                                      // 设置ADC标志位为真，标记ADC采样已启动
                                saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc2_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0); // 启动多通道ADC采样：发热丝1、发热丝2、基准电压和其他系统通道
                                //TRACE(hot_str, ecig.timer_cycle_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );                 //  ADC启动调试打印
                            }
                        }

                    }

                    if (ecig.power_on_flag)                              // 检查功率开启标志，进行功率累积计算
                    {
                        //ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev); //  功率计算方式
                        //TRACE(hot_str11,ecig.cfg->smoke_res_swich );

                        if(!ecig.cfg->smoke_position_swich)              // 检查是否为单发模式（smoke_position_swich=0表示单发）
                        {
                            if(ecig.cfg->smoke_res_swich)                // 检查单发模式下使用哪个发热丝（smoke_res_swich选择发热丝1或2）
                            {
                                // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev); //  功率计算方式
                                ecig.p_current = (u16)ecig.AD_hot_voltage_mv + ecig.p_prev;  // 使用发热丝1电压累积功率（直接电压累积）
                            }
                            else                                         // 使用发热丝2的情况
                            {
                                // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage2) >> 13) + ecig.p_prev); //  功率计算方式
                                ecig.p_current = (u16)ecig.AD_hot_voltage_mv2  + ecig.p_prev; // 使用发热丝2电压累积功率
                            }
                        }
                        else                                             // 双发模式的处理分支
                        {
                            //双发                                       //  双发模式功率累积计算
                            // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);        //  功率计算方式1
                            // ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage2) >> 13) + ecig.p_current);    //  功率计算方式2
                            // ecig.p_current = (u16)ecig.AD_hot_voltage_mv + ecig.p_prev;                                //  单独累积方式1
                            // ecig.p_current = (u16)ecig.AD_hot_voltage_mv2 + ecig.p_current;                            //  单独累积方式2
                            ecig.p_current = ((u16)ecig.AD_hot_voltage_mv +(u16)ecig.AD_hot_voltage_mv2)/2 + ecig.p_prev; // 双发模式：使用两个发热丝电压的平均值累积功率
                        }

                        // if (ecig.p_current >= ecig.cfg->aim_power * ecig.timer_big_cycles_10ms) //  功率比较条件
                        // printf(hot_str,ecig.AD_hot_voltage_mv,ecig.timer_cycle_cnt,12340);
                        // printf(vol_hot_test_str,ecig.p_current,ecig.AD_hot_voltage_mv,ecig.AD_hot_voltage_mv2,ecig.timer_cycle_cnt, ecig.cfg->aim_voltage,ecig.cfg->smoke_res_swich);

                        if (ecig.p_current >= ecig.cfg->aim_voltage * ecig.timer_big_cycles_10ms) // 检查累积功率是否达到目标电压乘以大周期数的阈值
                        {
                            // printf(vol_hot_str,ecig.AD_hot_voltage_mv,ecig.timer_cycle_cnt, ecig.cfg->aim_voltage); //  功率达标打印
                            if (ecig.power_on_flag)                     // 再次检查功率开启标志
                            {
                                //TRACE(hot_str, ecig.timer_cycle_cnt,ecig.cfg->aim_power, ecig.p_current); //  功率达标调试信息
                                //                                TRACE(hot_str, 12, ecig.p_prev); //  前一功率值打印
                            }
                            ECIG_PWM_OFF_FUNC();                        // 关闭PWM输出，停止加热
                            ecig.power_on_flag = 0;                     // 清除功率开启标志，禁止后续加热
                        }
                        else                                            // 功率未达到目标值的处理
                        {
                            ecig.p_prev = ecig.p_current;               // 保存当前功率值作为下次累积的基础值
                        }
                    }
                }
                break;                                                   // 跳出case 6

            }
            if (ecig.timer_cycle_cnt == ecig.timer_big_cycles_10ms)      // 检查是否完成一个大周期（10ms，200个50μs周期）
            {
                ecig.timer_cycle_cnt = 0;                                // 重置周期计数器，开始新的大周期
                ecig.timer_switch_acc_cnt = 0;                           // 重置状态计数器，回到状态0
                ECIG_PWM_OFF_FUNC();                                     // 关闭PWM输出，结束当前大周期的加热
                //ecig_vbat_proc();//获取电量                            // 电池电量获取
                ecig.power_on_flag = 1;                                  // 重新设置功率开启标志，准备下一个大周期
                ecig.p_current = 0;                                      // 清零当前功率累积值
                ecig.p_prev = 0;                                         // 清零前一功率累积值
                return;                                                  // 提前返回，结束当前中断处理
            }
            ecig.timer_cycle_cnt++;                                      // 递增周期计数器，进入下一个50μs周期
        }
    }
    else                                                                 // 不满足工作条件时的处理（咪头未触发或存在故障）
    {
        if (!ecig.mic_sta)                                               // 检查咪头是否未触发（用户停止吸烟）
        {
            ecig.mic_start = 0;                                          // 清除咪头启动标志，标记未开始工作
            ecig.hot_time_cnt = 0;                                       // 清零加热时间计数器
            ecig.hot_time_flag = 0;                                      // 清除加热时间标志
            ecig.cnt_1s = 0;                                             // 清零1秒计数器
            if(ecig.smoke_sta != IDLE)                                   // 检查烟控状态是否不是空闲状态
            {
                ecig.cfg->smoke_res_swich = !ecig.cfg->smoke_res_swich;  // 切换电阻开关状态，实现发热丝轮换使用
                //  TRACE(hot_str11,ecig.cfg->smoke_res_swich );         //  电阻开关状态打印
            }
            ecig.smoke_sta = IDLE;                                       // 设置烟控状态为空闲状态

            ecig.timer_cycle_cnt = 0;                                    // 重置周期计数器
            ecig.timer_switch_acc_cnt = 0;                               // 重置状态计数器
            ecig.timer_switch_adc_flag = false;                          // 清除ADC标志位
            if(ecig.short_circuit_flag /* != SHORT_CIRCUIT */)           // 检查短路标志（注释掉了具体条件）
            {
                ecig.short_circuit_flag = IDLE;                          // 清除短路标志，设置为空闲状态
            }

//                ecig.short_circuit_flag = 0;                           //  短路标志清零
            //printf(ecig_end);                                          //  结束打印信息
        }


        if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)                 // 检查加热时间是否超过最大允许时间（通常8秒）
        {
            if (ecig.cnt_1s == 1)                                        // 检查1秒计数器是否为1（确保每秒只执行一次）
            {
                printf(info_8s);                                         // 打印8秒超时警告信息
                if(ecig.smoke_sta != SMOKE_TIMEOUT)                      // 检查烟控状态是否不是超时状态
                {
                    // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0); //  旧版本消息发送
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件，通知上层应用超时
                    sys_cb.smoke_index = SMOKE_TIMEOUT;                  // 设置系统控制块中的烟控索引为超时状态
                }
            }
        }


        if (ecig.short_circuit_flag == SHORT_CIRCUIT)                   // 检查短路标志是否为短路状态
        {
            if (ecig.cnt_1s == 1)                                        // 检查1秒计数器是否为1（确保每秒只执行一次）
            {
                printf(short_circuit_str,0,ecig.short_circuit_flag);    // 打印短路故障信息：标识码0，短路标志值
                if(ecig.smoke_sta != SHORT_CIRCUIT)                      // 检查烟控状态是否不是短路状态
                {
                    // uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_SMOKE_REMIND,0); //  旧版本消息发送
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);                // 向消息队列发送电子烟提醒事件，通知上层应用短路故障
                    sys_cb.smoke_index = SHORT_CIRCUIT;                  // 设置系统控制块中的烟控索引为短路状态
                }
            }
        }

        ecig_adgnd_io_deinit();                                          // 反初始化ADC接地IO，释放GPIO资源
        ECIG_PWM_OFF_FUNC();                                             // 关闭PWM输出，停止对发热丝供电
        ECIG_VEN_OFF();                                                  // 关闭VEN电源，切断整个发热丝电路的供电
    }
}

AT(.com_text.isr) WEAK
void timer_hot_single_isr(void)//
{

}

#endif

#endif
