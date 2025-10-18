#include "include.h"

#if VDDHR_TRIM_EN

#define TRACE_EN                1

#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#define VDDHR_TRIM_BASE         22      //3.0v
// #define VDDHR_NOR               3300
#define VDD_HIGH_LEVEL          3000
#define VDD_LOW_LEVEL           1000
#define VDD_HIGH                BIT(0)
#define VDD_LOW                 BIT(1)
#define VDD_BOND                (VDD_HIGH | VDD_LOW)
#define VDD_TRIM_MAX_CNT        5      //最多5次，防止卡住

AT(.com_text.vddhr_trim)
void bsp_vddhr_saradc_start(void)
{
    saradc_kick_start(0, 0);
    while(!bsp_saradc_process(1));
}

AT(.com_text.vddhr_trim)
u32 bsp_vddhr_saradc_get(void)
{
    u32 adc_pb2 = saradc_get_value10(ADCCH_PB2);
    return saradc_vddhr_get_calc_value(adc_pb2, adc_cb.bg, adc_cb.vrtc_val, adc_cb.vrtc_first);
}

bool bsp_vddhr_bond_check(void)
{
    bool res = 0;
    u8 vdd_level = 0;
    u32 vdd_pb2_h, vdd_pb2_l;

    saradc_set_channel(BIT(ADCCH_PB2));
    hr_vdd_ldo_on();
    bsp_vddhr_saradc_start();
    vdd_pb2_h = bsp_vddhr_saradc_get();
    if (vdd_pb2_h >= VDD_HIGH_LEVEL)
    {
        vdd_level |= VDD_HIGH;
    }

    hr_vdd_ldo_off();
    delay_ms(80);                //关闭需要一定时间
    bsp_vddhr_saradc_start();
    vdd_pb2_l = bsp_vddhr_saradc_get();
    if (vdd_pb2_l <= VDD_LOW_LEVEL)
    {
        vdd_level |= VDD_LOW;
    }

    if ((vdd_level & VDD_BOND) == VDD_BOND)
    {
        res = 1;
        TRACE("[VDDHR_TRIM] vddhr_bone_pb2_ok\n");
    }
    else
    {
        TRACE("[VDDHR_TRIM] vddhr_bone_pb2_not\n");
    }
    TRACE("[VDDHR_TRIM] vdd_pb2_h:%d, vdd_pb2_l:%d\n", vdd_pb2_h, vdd_pb2_l);

    return res;
}

void bsp_vddhr_trim_init(void)
{
    if (sys_trim.vddhr != 0)                            //VDDHR IS TRIMED
    {
        hr_vdd_oft_cfg(sys_trim.vddhr);
        sys_cb.vddhr_trim_sta = VDDHR_TRIM_OFF;
        TRACE("[VDDHR_TRIM] vddhr is trim ok : %d -> off\n", sys_trim.vddhr);
        return;
    }

    bool res = bsp_vddhr_bond_check();

    // if (CHIP_PACKAGE_SELECT == CHIP_5690G || CHIP_PACKAGE_SELECT == CHIP_5690C_F) {
    //     sys_cb.vddhr_trim_sta = VDDHR_TRIM_OFF;
    //     return;
    // }

    if (!res)                                           //not bond pb2
    {
        saradc_clr_channel(BIT(ADCCH_PB2));
        sys_cb.vddhr_trim_sta = VDDHR_TRIM_OFF;
        TRACE("[VDDHR_TRIM] vddhr not bond pb2 -> off\n");
        return;
    }

    param_vddhr_trim_read(&sys_cb.vddhr_trim_base);     //read vddhr_trim_base
    if (sys_cb.vddhr_trim_base != 0)
    {
        hr_vdd_oft_cfg(sys_cb.vddhr_trim_base);
        saradc_clr_channel(BIT(ADCCH_PB2));
        sys_cb.vddhr_trim_sta = VDDHR_TRIM_OFF;
        TRACE("[VDDHR_TRIM] vddhr base read ok : %d -> off\n", sys_cb.vddhr_trim_base);
    }
    else
    {
        sys_cb.vddhr_trim_sta = VDDHR_TRIM_IDLE;
    }
}

void bsp_vddhr_trim_save(void)
{
    if (sys_cb.vddhr_trim_sta == VDDHR_TRIM_SAVE)
    {
        printf("[VDDHR_TRIM] save vddhr trim base ok : %d ->off\n", sys_cb.vddhr_trim_base);
        param_vddhr_trim_write(&sys_cb.vddhr_trim_base);
        sys_cb.vddhr_trim_sta = VDDHR_TRIM_OFF;
    }
}

#if TRACE_EN
AT(.com_rodata.my_vddhr)
char my_vddhr_str1[] = "[VDDHR_TRIM] start_vddhr:%d, sys_cb.vddhr_trim_base:%d\n";

AT(.com_rodata.my_vddhr)
char my_vddhr_str2[] = "[VDDHR_TRIM] verify_vddhr:%d, vddhr_diff:%d\n";
#endif

AT(.com_text.vddhr_trim)
void bsp_vddhr_trim_proc(void)
{
    switch (sys_cb.vddhr_trim_sta)
    {
        case VDDHR_TRIM_IDLE:
            hr_vdd_oft_cfg(VDDHR_TRIM_BASE);
            hr_vdd_ldo_on();
            saradc_set_channel(BIT(ADCCH_PB2));
            sys_cb.vddhr_trim_sta = VDDHR_TRIM_START;
            break;
        case VDDHR_TRIM_START:
        {
            bsp_vddhr_saradc_start();
            u32 vddhr = bsp_vddhr_saradc_get();

            if ((vddhr * 5 / 300) < 1)
            {
                if (sys_cb.vddhr_trim_cnt < VDD_TRIM_MAX_CNT)
                {
                    sys_cb.vddhr_trim_cnt++;
                    sys_cb.vddhr_trim_sta = VDDHR_TRIM_IDLE;
                }
                else
                {
                    sys_cb.vddhr_trim_cnt = 0;
                    saradc_clr_channel(BIT(ADCCH_PB2));
                    sys_cb.vddhr_trim_sta = VDDHR_TRIM_OFF;
                }
            }
            else
            {
                sys_cb.vddhr_trim_base = (3300 - vddhr) / (vddhr * 5 / 300) + VDDHR_TRIM_BASE;
                if (sys_cb.vddhr_trim_base < 16)
                {
                    sys_cb.vddhr_trim_base = 16;
                }
                else if (sys_cb.vddhr_trim_base > 31)
                {
                    sys_cb.vddhr_trim_base = 31;
                }
                hr_vdd_oft_cfg(sys_cb.vddhr_trim_base);
                hr_vdd_ldo_on();
                sys_cb.vddhr_trim_sta = VDDHR_TRIM_VERIFY;
            }
            TRACE(my_vddhr_str1, vddhr, sys_cb.vddhr_trim_base);
        }
        break;
        case VDDHR_TRIM_VERIFY:
        {
#if TRACE_EN
            bsp_vddhr_saradc_start();
            u32 vddhr = bsp_vddhr_saradc_get();
            int vddhr_diff = vddhr - 3300;
            TRACE(my_vddhr_str2, vddhr, vddhr_diff);
#endif
            sys_cb.vddhr_trim_sta = VDDHR_TRIM_VERIFY_END;
        }
        break;
        case VDDHR_TRIM_VERIFY_END:
            hr_vdd_ldo_off();
            saradc_clr_channel(BIT(ADCCH_PB2));
            sys_cb.vddhr_trim_sta = VDDHR_TRIM_SAVE;
            break;
    }
}
#if VDDHR_TRIM_TEST_EN
void bsp_vddhr_test(void)
{
    if (sys_cb.vddhr_trim_sta == VDDHR_TRIM_OFF)
    {
        hr_vdd_ldo_on();
        sys_cb.vddhr_trim_sta = VDDHR_TRIM_TEST;
    }
}
#endif //VDDHR_TRIM_TEST_EN
#endif //VDDHR_TRIM_EN