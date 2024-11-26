#include "include.h"

#if (GUI_SELECT == GUI_TFT_170_560_AXS15231B)
//样机只能到20M以下
const u8 tbl_despi_clk1[] =
{
    [SYS_88M] = 4,          //20M
    [SYS_132M] = 6,         //20M
    [SYS_144M] = 7,         //20M
    [SYS_176M] = 8,         //20M
    [SYS_192M] = 15,         //20M
};
const u8 tbl_despi_clk2[] =
{
    [SYS_88M] = 4,          //20M
    [SYS_132M] = 6,         //20M
    [SYS_144M] = 7,         //20M
    [SYS_176M] = 8,         //20M
    [SYS_192M] = 15,         //20M
};
#else
//320x385 60Hz 数据量为14.784MByte
const u8 tbl_despi_clk1[] =
{
    [SYS_88M] = 1,          //44M
    [SYS_132M] = 3,         //33M
    [SYS_144M] = 3,         //36M
    [SYS_176M] = 4,         //35M
    [SYS_192M] = 6,         //38M
};
const u8 tbl_despi_clk2[] =
{
    [SYS_88M] = 2,          //29.3M
    [SYS_132M] = 4,         //26.4M
    [SYS_144M] = 4,         //28.8M
    [SYS_176M] = 5,         //29.3M
    [SYS_192M] = 6,         //27.4M
};
#endif
//设置DESPI CLK接口
void sys_set_despi_baud_hook(u32 sys_clk)
{
    tft_set_baud(tbl_despi_clk1[sys_clk], tbl_despi_clk2[sys_clk]);
}

const uint8_t *bt_rf_get_inq_param(void)
{
    return NULL;
}

u8 get_chip_package(void)
{
    return 0;
}

const uint8_t *bt_rf_get_param(void)
{
    return (const uint8_t *)&xcfg_cb.rf_pa_gain;
}

//正常启动Main函数
int main(void)
{
    u32 rst_reason, rtccon10;

    rst_reason = LVDCON;
    rtccon10 = RTCCON10;
    printf("Hello AB569X: %08x\n", rst_reason);
    if (rst_reason & BIT(24))
    {
        // bsp_rtc_recode_set(1);
        printf("SW reset\n");
    }
    else if (rst_reason & BIT(19))
    {
        if (rtccon10 & BIT(10))
        {
            printf("WKO10S reset\n");
        }
        else
        {
            printf("RTC_WDT reset\n");
        }
    }
    else if (rst_reason & BIT(18))
    {
        // bsp_rtc_recode_set(1);
        printf("WKUP reset\n");
    }
    else if (rst_reason & BIT(17) || rtccon10 & BIT(3))
    {
        // bsp_rtc_recode_set(1);
        printf("VUSB reset\n");
    }
    else if (rst_reason & BIT(16))
    {
        // bsp_rtc_recode_set(1);
        printf("WDT reset\n");
    }

    bsp_sys_init();
//    GPIOEDIR &= ~(BIT(7) | BIT(2) | BIT(1));
//    GPIOEFEN &= ~(BIT(7) | BIT(2) | BIT(1));
//    GPIOEDE |= (BIT(7) | BIT(2) | BIT(1));

    // GPIOBFEN &= ~BIT(0);                        // CS
    // GPIOBDE  |=  BIT(0);
    // GPIOBDIR &= ~BIT(0);
    // GPIOBSET = BIT(0);

    // GPIOBFEN &= ~BIT(1);                        // CS
    // GPIOBDE  |=  BIT(1);
    // GPIOBDIR &= ~BIT(1);
    // GPIOBSET = BIT(1);
    // printf("GPIOA & BIT(7) = %d\n", GPIOA & BIT(7));
    // GPIOASET = BIT(7);
    // printf("GPIOA & BIT(7) = %d\n", GPIOA & BIT(7));

    func_run();
    return 0;
}

//升级完成
void update_complete(int mode)
{
    printf("update_complete: %d\n", mode);
    bsp_update_init();
    if (mode == 0)
    {
        WDT_DIS();
        while (1);
    }
    WDT_RST();
}
