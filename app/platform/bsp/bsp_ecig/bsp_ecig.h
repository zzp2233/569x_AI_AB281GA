
#ifndef _BSP_ECIG_H
#define _BSP_ECIG_H


#if (CHIP_PACKAGE_SELECT == CHIP_5690C_F)
#define ECIG_PWM1_GPIO  IO_PE5
#define ECIG_MIC_GPIO   IO_PB5
#define ECIG_PWM2_GPIO  IO_PE4
#define ECIG_GND_GPIO   IO_PG7
#define ECIG_ADC1_GPIO  ADCCH_PE7
#define ECIG_ADC2_GPIO  ADCCH_PE6
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PH7
#endif
#elif (CHIP_PACKAGE_SELECT == CHIP_5690G)
#define ECIG_PWM1_GPIO  IO_PE5
#define ECIG_MIC_GPIO   IO_PF3
#define ECIG_PWM2_GPIO  IO_PE4
#define ECIG_GND_GPIO   IO_PG7
#define ECIG_ADC1_GPIO  ADCCH_PE7
#define ECIG_ADC2_GPIO  ADCCH_PE6
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PH7
#endif
#elif (CHIP_PACKAGE_SELECT == CHIP_5691G)
#define ECIG_PWM1_GPIO  IO_PF1
#define ECIG_MIC_GPIO   IO_PF3
#define ECIG_PWM2_GPIO  IO_PF2
#define ECIG_GND_GPIO   IO_PB5
#define ECIG_ADC1_GPIO  ADCCH_PB4
#define ECIG_ADC2_GPIO  ADCCH_PB7
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PB6
#endif
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
#define ECIG_PWM1_GPIO  IO_PE1
#define ECIG_MIC_GPIO   IO_PE3
#define ECIG_PWM2_GPIO  IO_PE2
#define ECIG_GND_GPIO   IO_PB5
#define ECIG_ADC1_GPIO  ADCCH_PB4
#define ECIG_ADC2_GPIO  ADCCH_PB7
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PB6
#endif
#elif (CHIP_PACKAGE_SELECT == CHIP_5690F)
#define ECIG_PWM1_GPIO  IO_PB2 // 发热丝发热PWM
#define ECIG_MIC_GPIO   IO_PF3
#define ECIG_PWM2_GPIO  IO_PE5  // 阻值检测PWM
#define ECIG_GND_GPIO   IO_PG7
#define ECIG_ADC1_GPIO  ADCCH_PE7//ADCCH_PE6
#define ECIG_ADC2_GPIO  ADCCH_PE6//ADCCH_PE7
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PE3
#endif
#else
#define ECIG_PWM1_GPIO  IO_PF1
#define ECIG_MIC_GPIO   IO_PF3
#define ECIG_PWM2_GPIO  IO_PF2
#define ECIG_GND_GPIO   IO_PB5
#define ECIG_ADC1_GPIO  ADCCH_PB4
#define ECIG_ADC2_GPIO  ADCCH_PB7
#if ECIG_DET_EN
#define ECIG_DET_GPIO   IO_PB6
#endif
#endif


#define bsp_ecig_exit()         ecigarette_exit()
#define bsp_ecig_is_working()   ecig_is_working()   //正在吸烟（mic高电平）

#define bsp_ecig_is_working_sta()   ecig_is_working_sta()   //正在吸烟各种状态

enum
{
    REMINDER_ECIG_SMOKING,  //吸烟
    REMINDER_ECIG_SHORT,    //短路
    REMINDER_ECIG_UP,       //拔出
    REMINDER_ECIG_IN,       //插入
};

void bsp_ecig_init(void);
void bsp_ecig_sleep_wakeup(void);
void ecig_set_power(u8 value);
u8 ecig_get_power(void);
void ecig_set_res(u8 value);
u8 ecig_get_res(void);
#endif // _BSP_ECIG_H
