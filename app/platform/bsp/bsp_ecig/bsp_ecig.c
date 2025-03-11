#include "include.h"

#if ECIG_POWER_CONTROL

static ecig_cfg_t ecig_cfg;

void bsp_ecig_init(void)
{
    printf("%s\n", __func__);
    ecig_cfg_t *p = &ecig_cfg;
    memset(p, 0, sizeof(ecig_cfg_t));

    p->io_pwm1 = ECIG_PWM1_GPIO;
    p->io_mic = ECIG_MIC_GPIO;

    p->io_adgnd = ECIG_GND_GPIO;
    p->adc1_ch = ECIG_ADC1_GPIO;

#if ECIG_ADC2_EN
    p->adc2_en = 1;
    p->io_pwm2 = ECIG_PWM2_GPIO,
       p->io_ven = ECIG_VEN_GPIO;
    p->adc_res1_ch = ECIG_ADC_RES1_GPIO;
    p->adc2_ch = ECIG_ADC2_GPIO;
    p->adc_res2_ch = ECIG_ADC_RES2_GPIO;
#if DEVELOPMENT_BOARD_TYPE == DEVELOPMENT_BOARD_USER
    p->res_diff = (u16)(10.0* 8192);
    p->short_res_proportion = 22;                       //计算方法：res_diff/res_min + 1(去掉小数部分),例res_diff = 4.7Ω，res_min = 0.4Ω
    p->open_res_proportion  = 4;                        //计算方法：res_diff/res_max + 1(有小数进一),例res_diff = 4.7Ω，res_max = 2Ω
#else
    p->res_diff = (u16)(4.7* 8192);
    p->short_res_proportion = 12;                       //计算方法：res_diff/res_min + 1(去掉小数部分),例res_diff = 4.7Ω，res_min = 0.4Ω
    p->open_res_proportion  = 4;
#endif
#else
    p->res_wire = (u16)(1.0 * 8192);

#endif
#if ECIG_DET_EN
    p->io_hot_det1 = ECIG_DET1_GPIO;
    p->io_hot_det2 = ECIG_DET2_GPIO;
    p->io_hot_mode = true;
#endif

    //p->aim_power = 5,
    ecig_set_power(10);
    p->heat_time_max = 8;
    p->short_res_prop = 14;
    p->open_res_prop = 300;
    test_1st_gear_func();
    ecigarette_init(p); //需要用到硬件定时器TMR3
}

void bsp_ecig_sleep_wakeup(void)
{
    ecig_cfg_t *p = &ecig_cfg;
    ecigarette_sleep_wakeup(p);
}




AT(.com_text.ecig.process)
u32 ecig_vbat_get(void)
{
    return bsp_vbat_get_voltage(0);
}

void ecig_set_power(u8 value)
{
    ecig_cfg.aim_power = value;
}

u8 ecig_get_power(void)
{
    return ecig_cfg.aim_power;
}
AT(.com_text.isr)
void ecig_set_res(u8 value)
{
    ecig_cfg.res_wire = value;
}
AT(.com_text.isr)
void ecig_set_res2(u8 value)
{
    ecig_cfg.res_wire2 = value;
}

#define DEBUG 0
uint8_t ecig_get_res(void)
{
#if DEBUG
    printf("%s, res_wire=%d\n", __func__, ecig_cfg.res_wire);
#endif
    // 使用更大的数据类型防止溢出，并提高精度
    uint32_t data_temp = (uint32_t)ecig_cfg.res_wire * 1000;
    uint32_t result = data_temp / 8192;
    uint16_t data_temp1 = 0;
    uint8_t data = 0;
    if(result<1000&&result>0)
    {
        // 对 result 进行四舍五入处理
        data_temp1 = (uint16_t)((result + 10) / 10);
        data = (uint8_t)((data_temp1 + 10) / 10);
    }
    else if(result>=1000)
    {
        // 对 result 进行四舍五入处理
        data_temp1 = (uint16_t)((result + 15) / 10);
        data = (uint8_t)((data_temp1 + 15) / 10);
    }

#if DEBUG
    printf("%s, data_temp1=%d,result=%lu\n", __func__,data_temp1, result);
    printf("%s, data=%d\n", __func__, data);
#endif

    return data;
}
u8 ecig_get_res2(void)
{
#if DEBUG
    printf("%s, res_wire=%d\n", __func__, ecig_cfg.res_wire);
#endif
    // 使用更大的数据类型防止溢出，并提高精度
    uint32_t data_temp = (uint32_t)ecig_cfg.res_wire2 * 1000;
    uint32_t result = data_temp / 8192;
    uint16_t data_temp1 = 0;
    uint8_t data = 0;

    if(result<1000&&result>0)
    {
        // 对 result 进行四舍五入处理
        data_temp1 = (uint16_t)((result + 10) / 10);
        data = (uint8_t)((data_temp1 + 10) / 10);
    }
    else if(result>=1000)
    {
        // 对 result 进行四舍五入处理
        data_temp1 = (uint16_t)((result + 15) / 10);
        data = (uint8_t)((data_temp1 + 15) / 10);
    }
#if DEBUG
    printf("%s, data_temp1=%d,result=%lu\n", __func__,data_temp1, result);
    printf("%s, data=%d\n", __func__, data);
#endif

    return data;
    // return ecig_cfg.res_wire2 * 10 / 8192;
}

//单发
void test_1st_gear_func(void)
{
    printf("%s \n",__func__);
    ecig_cfg.smoke_position_swich = 0;

}



//双发
void test_2st_gear_func(void)
{
    printf("%s \n",__func__);
    ecig_cfg.smoke_position_swich = 1;
}

//单发
u8 get_gear_func(void)
{
    printf("%s \n",__func__);
    return ecig_cfg.smoke_position_swich;

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
#endif
