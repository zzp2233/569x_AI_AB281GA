#include "include.h"
#include "func.h"

bool power_off_check(void);
void lock_code_pwrsave(void);
void unlock_code_pwrsave(void);
bool lp_xosc_check(void);

AT(.com_text.sleep)
void lowpwr_tout_ticks(void)
{
    if(sys_cb.sleep_delay != -1L && sys_cb.sleep_delay > 0)
    {
        sys_cb.sleep_delay--;
    }
    if(sys_cb.guioff_delay != -1L && sys_cb.guioff_delay > 0)
    {
        sys_cb.guioff_delay--;
    }
    if(sys_cb.pwroff_delay != -1L && sys_cb.pwroff_delay > 0)
    {
        sys_cb.pwroff_delay--;
    }
}

AT(.com_text.sleep)
bool sys_sleep_check(u32 *sleep_time)
{
    u32 co_min = co_timer_get_min_time(true)*2;

    if(*sleep_time > co_min)
    {
        *sleep_time = co_min;
    }

    if(*sleep_time < 4)
    {
        *sleep_time = 4;
    }

    if(*sleep_time > sys_cb.sleep_wakeup_time)
    {
        *sleep_time = sys_cb.sleep_wakeup_time;
        return true;
    }
    return false;
}

//休眠中ble断开/连接/传输是否需要退出休眠
AT(.com_text.sleep)
bool ble_is_allow_wkup(void)
{
    return LE_ALLOW_WKUP_EN;
}

AT(.sleep_text.sleep)
void sys_sleep_cb(u8 lpclk_type)
{
    //注意！！！！！！！！！！！！！！！！！
    //此函数只能调用sleep_text或com_text函数

    //此处关掉影响功耗的模块
    u32 gpiogde = GPIOGDE;
    if ((gpiogde & BIT(6)) && ((WKPINMAP >> 14) & 0x3) == 0x1)
    {
        GPIOGDE = BIT(2) | BIT(4) | BIT(6);                  //SPICS, SPICLK
    }
    else
    {
        GPIOGDE = BIT(2) | BIT(4);                  //SPICS, SPICLK
    }

    sys_enter_sleep(lpclk_type);                //enter sleep

    //唤醒后，恢复模块功能
    GPIOGDE = gpiogde;
}

AT(.sleep_text.sleep)
void sleep_set_sysclk(uint8_t sys_clk)
{
    uint8_t cur_sys_clk = sys_clk_get();

    if (sys_clk < SYS_24M || cur_sys_clk == sys_clk)
    {
        return;
    }

    if (sys_clk > SYS_24M)
    {
        if (cur_sys_clk <= SYS_24M)
        {
            CLKCON0 = (CLKCON0 & ~(0x03 << 2)) | (0x01 << 2); //sysclk select xosc26m_clk
            RSTCON0 &= ~BIT(4);                         //pllsdm disable
            adpll_init(DAC_OUT_SPR);                    //enable adpll
            adda_clk_source_sel(0);                     //adda_clk48_a select pll0
        }
        sys_clk_set(sys_clk);
    }
    else
    {
        sys_clk_set(SYS_24M);
        DACDIGCON0 &= ~BIT(0);                      //disable digital dac
        adda_clk_source_sel(1);                     //adda_clk48_a select xosc52m
        PLL0CON &= ~(BIT(18) | BIT(6));             //pll0 sdm & analog disable
    }
}

//用于未连接ble休眠后,当ble连接上后更新连接参数用
AT(.sleep_text.sleep)
void sleep_ble_param_check(void)
{
    if (sys_cb.flag_sleep_ble_status != ble_is_connect())
    {
        if ((sys_cb.flag_sleep_ble_status == false) && ble_is_connect())
        {
            ble_update_conn_param(400, 0, 500);     //interval: 400*1.25ms = 500ms
        }
        sys_cb.flag_sleep_ble_status = ble_is_connect();
    }
}


//休眠定时器，500ms进一次
AT(.sleep_text.sleep)
uint32_t sleep_timer(void)
{
    uint32_t ret = 0;

    sys_cb.sleep_counter++;
    if (sys_cb.sleep_counter == 10)
    {
        sys_cb.sleep_counter = 0;

#if LPWR_WARNING_VBAT
        u32 rtccon8 = RTCCON8;
#if CHARGE_VOL_DYNAMIC_DET
        if (RTCCON & BIT(20))                   //vusb is online?
        {
            gradient_process(1);
            RTCCON8 |= BIT(1);                  //charge stop
            delay_5ms(2);
            gradient_process(0);
        }
#endif
        saradc_init();
        saradc_adc15_analog_channel_sleep();
        delay_us(600);
        saradc_kick_start(0, 0);
        while(!bsp_saradc_process(0));
        sys_cb.vbat = bsp_vbat_get_voltage(1);
        RTCCON8 = rtccon8;
        if (sys_cb.vbat < LPWR_WARNING_VBAT)
        {
            //低电需要唤醒sniff mode
            ret = 2;
        }
        saradc_exit();
#endif
    }

#if CHARGE_EN
    if (xcfg_cb.charge_en)
    {
        charge_detect(0);
    }
#endif // CHARGE_EN

    sys_cb.sleep_wakeup_time = -1L;

    if(sys_cb.pwroff_delay != -1L)
    {
        if(sys_cb.pwroff_delay > 5)
        {
            sys_cb.pwroff_delay -= 5;
        }
        else
        {
            sys_cb.pwroff_delay = 0;
            return 1;
        }
    }

    if ((PWRKEY_2_HW_PWRON) && (!IS_PWRKEY_PRESS()))
    {
        ret = 1;
    }

    rtc_sleep_process();
    sleep_ble_param_check();

    return ret;
}

AT(.text.lowpwr.sleep)
static void sfunc_sleep(void)
{
    uint32_t status, ret;
    uint32_t usbcon0, usbcon1;
    u8 pa_de, pb_de, pe_de, pf_de, pg_de, ph_de;
    u16 adc_ch;
    uint32_t sysclk;
    u32 wkie, wkpnd;
    bool wko_wkup_flag;
    bool gui_need_wkp = false;
#if LE_EN
    u16 interval = 0;
    u16 latency = 0;
    u16 tout = 0;
    u16 adv_interval = 0;
#endif
    sys_cb.flag_sleep_ble_status = ble_is_connect();

#if VBAT_DETECT_EN
    if (bsp_vbat_get_lpwr_status())             //低电不进sniff mode
    {
        return;
    }
#endif

    printf("%s\n", __func__);

    sleep_cb.sys_is_sleep = true;
    sys_cb.gui_need_wakeup = 0;
    bt_enter_sleep();
#if LE_EN
    adv_interval = ble_get_adv_interval();
    ble_set_adv_interval(1600);                  //interval: 500 * 0.625ms = 500ms
    if (ble_is_connect())                       //ble已连接
    {
        interval = ble_get_conn_interval();
        latency = ble_get_conn_latency();
        tout = ble_get_conn_timeout();
        ble_update_conn_param(400, 0, 500);     //interval: 400*1.25ms = 500ms
    }
#endif
#if BT_SINGLE_SLEEP_LPW_EN
    if (!bt_is_connected())                     //蓝牙未连接
    {
        if (bt_get_scan())                      //双模休眠
        {
            bt_update_bt_scan_param(4096, 8, 4096, 12);
        }
        else
        {
            bt_update_bt_scan_param(4096, 0, 4096, 0);
            bt_scan_disable();
        }
    }
#else
    if (!bt_is_connected())                     //蓝牙未连接
    {
        bt_update_bt_scan_param(4096, 12, 2048, 12);
    }
#endif

#if DAC_DNR_EN
    u8 sta = dac_dnr_get_sta();
    dac_dnr_set_sta(0);
#endif
    dac_power_off();                            //dac power down

    sys_set_tmr_enable(0, 0);

    adc_ch = bsp_saradc_exit();                //close saradc及相关通路模拟
    saradc_clr_channel(adc_ch);
    saradc_set_channel(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP));

#if CHARGE_EN
    bsp_charge_set_stop_time(3600);
    charge_set_detect_cnt(1);
#if CHARGE_VOL_DYNAMIC_DET
    if(sys_cb.chg_on)
    {
        gradient_process(0);
        RTCCON8 &= ~BIT(1);                                  //charge open
    }
#endif
#endif

    usbcon0 = USBCON0;                          //需要先关中断再保存
    usbcon1 = USBCON1;
    USBCON0 = BIT(5);
    USBCON1 = 0;

    gui_sleep();

#if MODEM_CAT1_EN
    bsp_modem_sleep_enter();
#endif

    sysclk = sys_clk_get();
    sys_clk_set(SYS_24M);
    DACDIGCON0 &= ~BIT(0);                      //disable digital dac
    adda_clk_source_sel(1);                     //adda_clk48_a select xosc52m
    PLL0CON &= ~(BIT(18) | BIT(6));             //pll0 sdm & analog disable
    rtc_sleep_enter();

    //io analog input
    pa_de = GPIOADE;
    pb_de = GPIOBDE;
    pe_de = GPIOEDE;
    pf_de = GPIOFDE;
    pg_de = GPIOGDE;
    ph_de = GPIOHDE;
    GPIOADE = 0;
    GPIOBDE = BIT(3);
    GPIOGDE = 0x3F;                             //MCP FLASH
    GPIOHDE = 0;

    u32 pf_keep = 0;
    if (bsp_sensor_init_sta_get(SENSOR_INIT_ALL))
    {
        GPIOEDE = 0 | BIT(2) | BIT(1);          //SENSOR I2C
        pf_keep |= BIT(2);                      //SENSOR PG
    }
    else
    {
        GPIOEDE = 0;
    }

#if MODEM_CAT1_EN
    pf_keep |= BIT(1) | BIT(2) | BIT(3);
#endif
    GPIOFDE = pf_keep;

    wkie = WKUPCON & BIT(16);
    WKUPCON &= ~BIT(16);                        //休眠时关掉WKIE
    sleep_wakeup_config();

    sys_cb.sleep_counter = 0;
    sys_cb.sleep_wakeup_time = -1L;
    while(bt_is_sleep())
    {
        WDT_CLR();
        bt_thread_check_trigger();
        status = bt_sleep_proc();

        if(status == 1)
        {
            ret = sleep_timer();
            if(ret)
            {
                if (ret == 1)
                {
                    func_cb.sta = FUNC_PWROFF;
                }
                break;
            }
        }
        wkpnd = port_wakeup_get_status();
        wko_wkup_flag = port_wko_is_wakeup();

        port_int_sleep_process(&wkpnd);
        bsp_sensor_step_lowpwr_pro();

        if (wkpnd)
        {
            printf("port wakeup: %x\n", wkpnd);
            gui_need_wkp = true;
            break;
        }
        if ((RTCCON9 & BIT(2)) || (RTCCON10 & BIT(2)) || wko_wkup_flag)
        {
            printf("wko wakeup\n");
            gui_need_wkp = true;
            break;
        }
#if LE_EN
        if (ble_app_need_wakeup())
        {
            printf("ble_app_need_wakeup\n");
            gui_need_wkp = true;
            break;
        }
#endif

        if (co_timer_pro(true))
        {
            printf("co_timer_pro_wakeup\n");
            break;
        }

        if (bt_cb.call_type)
        {
            printf("call_wakeup\n");
            break;
        }

    }

    RTCCON9 = BIT(7) | BIT(5) | BIT(2);         //clr port, bt, wko wakeup pending
    RTCCON3 &= ~(BIT(17) | BIT(13));            //disable port, bt wakeup
    sleep_wakeup_exit();
    WKUPCON |= wkie;                            //还原WKIE

    GPIOADE = pa_de;
    GPIOBDE = pb_de;
    GPIOEDE = pe_de;
    GPIOFDE = pf_de;
    GPIOGDE = pg_de;
    GPIOHDE = ph_de;
    USBCON0 = usbcon0;
    USBCON1 = usbcon1;

    bt_update_bt_scan_param_default();
#if BT_SINGLE_SLEEP_LPW_EN
    if(!bt_is_connected() && bt_get_scan())     //单模
    {
        bt_scan_enable();
    }
#endif
    rtc_sleep_exit();
    CLKCON0 = (CLKCON0 & ~(0x03 << 2)) | (0x01 << 2); //sysclk select xosc26m_clk
    RSTCON0 &= ~BIT(4);                         //pllsdm disable
    adpll_init(DAC_OUT_SPR);                    //enable adpll
    adda_clk_source_sel(0);                     //adda_clk48_a select pll0
    sys_clk_set(sysclk);
    saradc_set_channel(adc_ch);
    bsp_saradc_init();
#if CHARGE_EN
    bsp_charge_set_stop_time(18000);
    bsp_charge_sta(sys_cb.charge_sta);          //update充灯状态
    charge_set_detect_cnt(5);
#if CHARGE_VOL_DYNAMIC_DET
    if(sys_cb.chg_on)
    {
        gradient_process(0);
        RTCCON8 &= ~BIT(1);                     //charge open
    }
#endif
#endif // CHARGE_EN
    sys_set_tmr_enable(1, 1);
    if (DAC_FAST_SETUP_EN)
    {
        bsp_loudspeaker_mute();
    }
#if MODEM_CAT1_EN
    bsp_modem_sleep_exit();
#endif
    if (gui_need_wkp)
    {
        printf("gui_wakeup\n");
        gui_wakeup();
        func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
    }
    else
    {
        sys_clk_set(SYS_24M);
    }

#if LE_EN
    ble_set_adv_interval(adv_interval);
    if (interval | latency | tout)
    {
        ble_update_conn_param(interval, latency, tout); //还原连接参数
    }
#endif
    dac_restart();
    bsp_change_volume(sys_cb.vol);
#if DAC_DNR_EN
    dac_dnr_set_sta(sta);
#endif
    bt_exit_sleep();
    sleep_cb.sys_is_sleep = false;
    printf("sleep_exit\n");
}

AT(.text.lowpwr.sleep)
bool sleep_process(is_sleep_func is_sleep)
{
    if (sys_cb.gui_need_wakeup && sys_cb.gui_sleep_sta)
    {
        gui_wakeup();                   //按键亮屏
        reset_sleep_delay_all();
        printf("sleep_exitsleep_exitsleep_exit\n");
        func_cb.sta = FUNC_SLIDING_UNLOCK_SCREEN;
        sys_cb.gui_need_wakeup = 0;
        return false;
    }

#if LE_EN
    if (ble_app_need_wakeup())
    {
        reset_sleep_delay_all();
        reset_pwroff_delay();
        return false;
    }
#endif
    if ((*is_sleep)())
    {
        if (!sys_cb.sleep_en)
        {
            reset_sleep_delay_all();
            return false;
        }
        if (sys_cb.sleep_delay == 0)
        {
            sfunc_sleep();              //熄屏且进入休眠
            reset_sleep_delay_all();
            reset_pwroff_delay();
            return true;
        }
    }
    else
    {
        if (sys_cb.guioff_delay == 0 && !sys_cb.gui_sleep_sta)
        {
            gui_sleep();                //仅熄屏
        }
        reset_sleep_delay();
        reset_pwroff_delay();
    }
    return false;
}

////红外关机进入sleep mode
//AT(.text.lowpwr.sleep)
//void func_sleepmode(void)
//{
//
//}

///解决充满关机等待5V掉电，超时1分钟
AT(.text.pwroff.vusb)
u8 sfunc_pwrdown_w4_vusb_offline(void)
{
//    u32 timeout = 10000, off_cnt = 0;
//    while (timeout--) {
//        WDT_CLR();
//        delay_us(330);                          //5ms
//        if (!CHARGE_DC_IN()) {
//            off_cnt++;
//        } else {
//            off_cnt = 0;
//        }
//        if (off_cnt > 5) {
//            return 1;                           //VUSB已掉线，打开VUSB唤醒
//        }
//    }
    return 0;                                   //关VUSB唤醒
}

AT(.text.pwroff.enter)
void sfunc_power_save_enter(void)
{
#if LP_XOSC_CLOCK_EN
    bool lp_xosc_clock_err = lp_xosc_check();
#endif

    sys_clk_set(SYS_24M);
    LOUDSPEAKER_MUTE_DIS();
    WDT_CLR();
#if CHARGE_EN
    if (xcfg_cb.charge_en)
    {
        bsp_charge_off();
    }
#endif
    set_buck_mode(0);                           //关机前切回ldo mode
    vusb_delay_rst_dis();
    RTCCON4 = (RTCCON4 & ~(3 << 5)) | (1 << 5); //RI_VCORE_SVBT = 0, RI_VCORE_SVIO = 1
    USBCON0 = BIT(5);
    USBCON1 = 0;
    GPIOADE = 0;
    GPIOBDE = 0;
    GPIOEDE = 0;
    GPIOFDE = 0;
    GPIOGDE = 0x3F;                             //MCP FLASH
    GPIOHDE = 0;
    RTCCON8 &= ~BIT(15);                        //RI_EN_VUSBDIV = 0

#if !LP_XOSC_CLOCK_EN
    RTCCON15 &= ~(3 << 13);                     //disable LP_XOSC
    if (sys_cb.flag_shipping_mode)
    {
        RTCCON0 &= ~(BIT(0) | BIT(18) | BIT(2));    //RI_EN_SNIFF = 0, RI_EN_RINGOSC = 0, 关掉RTC_RC
    }
#else
    RTCCON0 &= ~(BIT(0) | BIT(18) | BIT(2));    //RI_EN_SNIFF = 0, RI_EN_RINGOSC = 0, 关掉RTC_RC
    if (lp_xosc_clock_err)
    {
        RTCCON15 &= ~(3 << 13);                     //disable LP_XOSC
    }
#endif

    PICCONCLR = BIT(0);                         //Globle IE disable
    CLKCON0 |= BIT(0);                          //enable RC
    CLKCON0 = (CLKCON0 & ~(0x3 << 2));          //system clk select RC

    RSTCON0 &= ~(BIT(4) | BIT(6));              //pllsdm, adda_clk
    CLKGAT0 &= ~(BIT(12) | BIT(14));            //disable dac clk
    PLL0CON &= ~(BIT(6) | BIT(18));             //PLL0 disable
    XOSCCON &= ~((0xf<<24) | (3 << 6) | (7 << 10) | BIT(5));//X24 output, xosc, xosc24m disable
    PLL0CON &= ~(BIT(12) | BIT(0) | BIT(1));    //PLL0 & XOSC关电
    RTCCON9 = 0xfff;                            //Clr pending
}

//硬开关方案，低电时，进入省电状态
AT(.text.pwroff)
void sfunc_lowbat_do(void)
{
    sfunc_power_save_enter();
    WDT_DIS();
    RTCCON3 |= BIT(11);                         //VUSB Wakeup enable
    while(1)
    {
        LPMCON |= BIT(0);                       //Sleep mode enable
        asm("nop");
        asm("nop");
        asm("nop");
        if (RTCCON10 & BIT(3))                  //VUSB wake up pending
        {
            WDT_RST();
        }
    }
}

//软开关方案，POWER键/低电时，进入关机状态
AT(.text.pwroff.pwrdwn)
void sfunc_pwrdown_do(u8 vusb_wakeup_en)
{
#if !LP_XOSC_CLOCK_EN
    if (!sys_cb.flag_shipping_mode)
    {
        rtc_set_alarm_wakeup(900);              //15min
    }
#endif

    sys_set_tmr_enable(0, 0);
    sfunc_power_save_enter();
    if (!vusb_wakeup_en)
    {
        RTCCON8 = (RTCCON8 & ~BIT(6)) | BIT(1); //disable charger function
        vusb_wakeup_en = sfunc_pwrdown_w4_vusb_offline();
    }
    RTCCON1 &= ~(BIT(5) | BIT(7));              //BIT(7): VRTC voltage for ADC, BIT(5):WK pin analog enable, output WKO voltage for ADC
    RTCCON11 = (RTCCON11 & ~0x03) | BIT(2);     //WK PIN filter select 8ms

    uint rtccon3 = RTCCON3 & ~BIT(11);
    uint rtccon1 = RTCCON1 & ~0x1f;
#if CHARGE_EN
    if ((xcfg_cb.charge_en) && (vusb_wakeup_en))
    {
        rtccon3 |= BIT(11);                     //VUSB wakeup enable
    }
#endif
    RTCCON3 = rtccon3 & ~BIT(10);               //关WK PIN，再打开，以清除Pending
    rtccon3 = RTCCON3 & ~0x87;                  //Disable VDDCORE VDDIO VDDBUCK VDDIO_AON
    rtccon3 &= ~BIT(22);                        //RI_EN_VLCORE = 0,
    rtccon3 &= ~(3 << 20);                      //RL2VDD12 = 0, C2VDD12 = 0
    rtccon3 |= BIT(6) | BIT(4) | BIT(19);       //PDCORE, PDCORE2, PDCORE3
    rtccon3 |= BIT(10);                         //WK pin wake up enable
//    rtccon3 |= BIT(16);                         //RTC_WDT wake ep enable
    rtccon1 |= BIT(0) | BIT(4);                 //WK 90K pull up enable
    WDT_DIS();
    RTCCON12 |= (0x3<<6);                       //DIS RTC_WDT
    RTCCON &= ~(3 << 1);                        //避免关机时rtc配置来不及生效问题
    RTCCON |= BIT(5);                           //PowerDown Reset，如果有Pending，则马上Reset
    RTCCON1 = rtccon1;
    RTCCON3 = rtccon3;
    LPMCON |= BIT(0);                           //sleep mode
    LPMCON |= BIT(1);                           //idle mode
    asm("nop");
    asm("nop");
    asm("nop");
    while (1);
}

void sfunc_pwrdown(u8 vusb_wakeup_en)
{
    lock_code_pwrsave();
    sfunc_pwrdown_do(vusb_wakeup_en);
}

void sfunc_lowbat(void)
{
    lock_code_pwrsave();
    sfunc_lowbat_do();
}

AT(.text.lowpwr.pwroff)
void func_pwroff(int pwroff_tone_en)
{
    printf("%s\n", __func__);

#if !LP_XOSC_CLOCK_EN
    if (!sys_cb.flag_shipping_mode)
    {

        sniff_rc_init();
        rtc_rc_sleep_enter();
        rtc_calibration_write(PARAM_RTC_CAL_ADDR);
        cm_write8(PARAM_RTC_CAL_VALID, 1);
        cm_sync();
        rtc_printf();
    }
#endif

#if WARNING_POWER_OFF
    if (SOFT_POWER_ON_OFF)
    {
        mp3_res_play(RES_BUF_POWEROFF_MP3, RES_LEN_POWEROFF_MP3);
    }
#endif // WARNING_POWER_OFF

//    gui_off();

    if (SOFT_POWER_ON_OFF)
    {
        if (!PWRKEY_2_HW_PWRON)
        {
            while (IS_PWRKEY_PRESS())       //等待PWRKWY松开
            {
                delay_5ms(1);
                WDT_CLR();
            }
        }
        dac_power_off();                    //dac power down
        if (CHARGE_DC_IN())
        {
            if (power_off_check())          //充电过程中等待结束再关机
            {
                return;
            }
        }
        bsp_saradc_exit();                  //close saradc及相关通路模拟
        if ((PWRKEY_2_HW_PWRON) && (sys_cb.poweron_flag))
        {
            RTCCON1 |= BIT(6);              //WK PIN High level wakeup
        }
        sfunc_pwrdown(1);
    }
    else
    {
        dac_power_off();                    //dac power down
        bsp_saradc_exit();                  //close saradc及相关通路模拟

        sfunc_lowbat();                     //低电关机进入Sleep Mode
    }
}
