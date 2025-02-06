#include "include.h"
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
const char vol_hot_str[] = "%d %d %d\n";
AT(.com_rodata.str)
const char short_circuit_str[] = "short circuit! %d sta [%d]\n";
AT(.com_rodata.str)
const char info_8s[] = "8S warning\n";

#endif

AT(.com_text.isr)
void timer_hot_dual_isr(void)//
{
    if (ecig.mic_sta && (ecig.hot_time_cnt < ecig.cfg->heat_time_max)  && (ecig.smoke_sta != LOW_POWER)
        && (ecig.short_circuit_flag != SHORT_CIRCUIT) &&  (ecig.short_circuit_flag != OPEN_CIRCUIT))
    {
        if (ecig.mic_sta != ecig.mic_start)
        {
            ecig.timer_cycle_cnt++;
            ecig.hot_time_flag++;
            switch(ecig.timer_switch_acc_cnt)
            {
                case 0://采集阻值
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                    if (saradc_is_finish() && ecig.timer_switch_adc_flag)
                    {
                        ecig.timer_switch_acc_cnt ++;

                        ecig.timer_switch_adc_flag = false;
                        ecig.adc1 = saradc_get_value10(ecig.cfg->adc1_ch);
                        ecig.adc2 = saradc_get_value10(ecig.cfg->adc2_ch);
                        caculate_res();
                    }
                    else
                    {
                        ECIG_PWM2_ON();
                        ECIG_PWM1_OFF();
                        ecig_adgnd_io_init();
                        if(!ecig.timer_switch_adc_flag)
                        {
                            if (!saradc_adc15_is_bg())
                            {
                                saradc_adc15_analog_select(ADCCH15_ANA_BG);
                            }
                            saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ecig.cfg->adc2_ch) | SADCCH, 0, 0);

                            ecig.timer_switch_adc_flag = true;
                        }
                    }
                    break;
                case 6:
                {
                    ECIG_PWM2_OFF();
                    ecig.timer_cycle_cnt = 0;
                    ecig.timer_switch_acc_cnt = 0;
                    if (!ecig.hot_res)
                    {
                        ECIG_PWM1_OFF();
                        ecig.power_on_flag = 0;
                        ecig.short_circuit_flag = ecig.smoke_sta;//短路开路
                        printf(short_circuit_str,-1,ecig.short_circuit_flag);
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = ecig.smoke_sta;
                        //break;
                    }
                    else
                    {
                        ecig.smoke_sta = SMOKING;
                        ecig.power_on_flag = 1;
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = SMOKING;
                    }
                    printf(hot_str,6,ecig.hot_res,0xff);
                    ecig.mic_start = 1;
                }
                break;
            }
        }
        else
        {
            switch(ecig.timer_switch_acc_cnt)
            {
                case 0:////获取电量
                case 1:
                case 2:
                {
                    if (saradc_is_finish() && ecig.timer_switch_adc_flag)
                    {
                        ecig.timer_switch_acc_cnt ++;
                        ecig.timer_switch_adc_flag = false;

                        ecig.AD_BAT_voltage_mv = ecig_vbat_get();
                        if(ecig.AD_BAT_voltage_mv < 3300)                   //欠压保护
                        {
                            ECIG_PWM1_OFF();
                            ecig.smoke_sta = LOW_POWER;
                            ecig.power_on_flag = 0;
                            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                            sys_cb.smoke_index = LOW_POWER;
                            printf(hot_str,13,ecig.AD_BAT_voltage_mv,0);
                        }
                        else
                        {
                            //ECIG_PWM1_ON();
                            ecig.smoke_sta = SMOKING;
                            ecig.power_on_flag = 1;
                            //printf(hot_str,14,ecig.AD_BAT_voltage_mv,0);
                        }
                        //printf(hot_str,55,ecig.AD_BAT_voltage_mv,ecig.timer_cycle_cnt);
                    }
                    else
                    {
                        if(!ecig.timer_switch_adc_flag)
                        {
                            ecig.timer_switch_adc_flag = true;
                            ECIG_PWM1_OFF();
                            if (!saradc_adc15_is_bg())
                            {
                                saradc_adc15_analog_select(ADCCH15_ANA_BG);
                            }
                            saradc_kick_start_do(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                        }

                        //printf(hot_str,66,ecig.AD_BAT_voltage_mv,ecig.timer_cycle_cnt);
                    }
                }
                break;
                case 3:////等一段时间电压稳定，做VDDIO校准
                case 4:
                case 5:
                {
                    if (saradc_is_finish() && ecig.timer_switch_adc_flag)
                    {
                        ecig.timer_switch_adc_flag = false;
                        u32 adc_vbg = saradc_get_value10(ADCCH_BGOP);
                        u32 hot_voltage = saradc_get_value10(ecig.cfg->adc1_ch);
                        ecig.AD_hot_voltage_mv = (hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg) * 4 / 3 / ECIG_VBG_VOLTAGE_MULTIPLE;
                        ecig.AD_hot_voltage = (ecig.AD_hot_voltage_mv << 13) / 1000;
                        //TRACE(hot_str, ecig.timer_switch_acc_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                        ecig.timer_switch_acc_cnt ++;
                    }
                    else
                    {
                        ecig.timer_switch_adc_flag = true;
                        ECIG_PWM1_ON();
                        saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                    }
                    ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                    ecig.p_prev = ecig.p_current;
                }
                break;
                case 6:
                {
                    if (saradc_is_finish() && ecig.timer_switch_adc_flag && ecig.power_on_flag)
                    {
                        ecig.timer_switch_acc_cnt  = 6;
                        ecig.timer_switch_adc_flag = false;
                        u32 adc_vbg = saradc_get_value10(ADCCH_BGOP);
                        u32 hot_voltage = saradc_get_value10(ecig.cfg->adc1_ch);
                        ecig.AD_hot_voltage_mv = (hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg) * 4 / 3 / ECIG_VBG_VOLTAGE_MULTIPLE;
                        ecig.AD_hot_voltage = (ecig.AD_hot_voltage_mv << 13) / 1000;
                        //TRACE(hot_str, 77, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                        if (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) <= ecig.cfg->short_res_prop)     //short circuit
                        {
                            ECIG_PWM1_OFF();
                            ecig.power_on_flag = 0;/*  */
                            ecig.smoke_sta = SHORT_CIRCUIT;
                            ecig.short_circuit_flag = ecig.smoke_sta;
                            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                            sys_cb.smoke_index = SHORT_CIRCUIT;
                            TRACE(hot_str, 7, ecig.AD_hot_voltage_mv,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                            TRACE(hot_str, 8, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                            TRACE(hot_str, ecig.AD_hot_voltage_mv,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv),ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                        }
                        else if (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) >= ecig.cfg->open_res_prop)       //检阻部分已有开路保护，可以屏蔽
                        {
                            ECIG_PWM1_OFF();
                            ecig.power_on_flag = 0;/*  */
                            ecig.smoke_sta = OPEN_CIRCUIT;
                            ecig.short_circuit_flag = ecig.smoke_sta;
                            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                            sys_cb.smoke_index = OPEN_CIRCUIT;
                            TRACE(hot_str, 8, ecig.AD_hot_voltage_mv,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                            TRACE(hot_str, 9, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                            TRACE(hot_str, ecig.AD_hot_voltage_mv,(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv),ecig.AD_hot_voltage_mv * 10 / (ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv));
                        }
                        else
                        {
                            ecig.power_on_flag = 1;
                            //TRACE(hot_str, ecig.timer_cycle_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                        }
                    }
                    else
                    {
                        if(ecig.power_on_flag)
                        {
                            ECIG_PWM1_ON();
                            if(!ecig.timer_switch_adc_flag)
                            {
                                ecig.timer_switch_adc_flag = true;
                                saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                                //TRACE(hot_str, ecig.timer_cycle_cnt, ecig.AD_hot_voltage_mv,ecig.AD_BAT_voltage_mv );
                            }
                        }

                    }

                    if (ecig.power_on_flag)
                    {
                        ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                        //TRACE(hot_str, ecig.p_current, (calculate_power(ecig.AD_hot_voltage) >> 13),ecig.p_prev );
                        if (ecig.p_current >= ecig.cfg->aim_power * ecig.timer_big_cycles_10ms)
                        {
                            if (ecig.power_on_flag)
                            {
                                //TRACE(hot_str, ecig.timer_cycle_cnt,ecig.cfg->aim_power, ecig.p_current);
                                //                                TRACE(hot_str, 12, ecig.p_prev);
                            }
                            ECIG_PWM1_OFF();
                            ecig.power_on_flag = 0;
                        }
                        else
                        {
                            ecig.p_prev = ecig.p_current;
                        }
                    }
                }
                break;

            }
            if (ecig.timer_cycle_cnt == ecig.timer_big_cycles_10ms)
            {
                ecig.timer_cycle_cnt = 0;
                ecig.timer_switch_acc_cnt = 0;
                ECIG_PWM1_OFF();
                //ecig_vbat_proc();//获取电量
                ecig.power_on_flag = 1;
                ecig.p_current = 0;
                ecig.p_prev = 0;
                return;
            }
            ecig.timer_cycle_cnt++;
        }
    }
    else
    {
        if (!ecig.mic_sta)
        {
            ecig.mic_start = 0;
            ecig.hot_time_cnt = 0;
            ecig.hot_time_flag = 0;
            ecig.cnt_1s = 0;
            ecig.smoke_sta = IDLE;

            ecig.timer_cycle_cnt = 0;
            ecig.timer_switch_acc_cnt = 0;
            ecig.timer_switch_adc_flag = false;
            if(ecig.short_circuit_flag != SHORT_CIRCUIT)
            {
                ecig.short_circuit_flag = IDLE;
            }
//                ecig.short_circuit_flag = 0;
            //printf(ecig_end);
        }
        if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)
        {
            if (ecig.cnt_1s == 1)
            {
                printf(info_8s);
                if(ecig.smoke_sta != SMOKE_TIMEOUT)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SMOKE_TIMEOUT;
                }
            }
        }
        if (ecig.short_circuit_flag == SHORT_CIRCUIT)
        {
            if (ecig.cnt_1s == 1)
            {
                printf(short_circuit_str,0,ecig.short_circuit_flag);
                if(ecig.smoke_sta != SHORT_CIRCUIT)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SHORT_CIRCUIT;
                }
            }
        }
        ecig_adgnd_io_deinit();
        ECIG_PWM1_OFF();
        ECIG_PWM2_OFF();
    }
}


AT(.com_text.isr) WEAK
void timer_hot_single_isr(void)//
{
    if (ecig.mic_sta && (ecig.hot_time_cnt < ecig.cfg->heat_time_max) && (ecig.short_circuit_cnt < 10) && (ecig.smoke_sta != LOW_POWER)
        && (ecig.short_circuit_flag != SHORT_CIRCUIT) &&  (ecig.short_circuit_flag != OPEN_CIRCUIT))
    {
        ecig.hot_time_flag = 1;
        ecig.timer_cycle_cnt++;
        ecig_adgnd_io_init();
        switch(ecig.timer_switch_acc_cnt)
        {
            case 0: //电池电压采集
            case 1:
            case 2:
                if (saradc_is_finish() && ecig.timer_switch_adc_flag)
                {
                    //TRACE(not_finish_str, 3, SADCCH, BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH);
                    ecig.timer_switch_adc_flag = false;
                    ecig.AD_BAT_voltage_mv = ecig_vbat_get();
                    //TRACE(hot_str, ecig.timer_switch_acc_cnt,ecig.timer_switch_acc_cnt, ecig.AD_BAT_voltage_mv);
                    ecig.timer_switch_acc_cnt ++;
                }
                else
                {
                    ECIG_PWM1_OFF();
                    if(!ecig.timer_switch_adc_flag)
                    {

                        if (!saradc_adc15_is_bg())
                        {
                            saradc_adc15_analog_select(ADCCH15_ANA_BG);
                        }
                        saradc_kick_start_do(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                        ecig.timer_switch_adc_flag = true;
                    }
                }
                break;
            case 3:
                if (saradc_is_finish() && ecig.timer_switch_adc_flag)
                {
                    //TRACE(not_finish_str, 3, SADCCH, BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH);
                    ecig.timer_switch_adc_flag = false;
                    ecig.AD_BAT_voltage_mv = ecig_vbat_get();
                    if (ecig.AD_BAT_voltage_mv < 3000)                    //欠压保护
                    {
                        ECIG_PWM1_OFF();
                        ecig.power_on_flag = 0;
                        ecig.smoke_sta = LOW_POWER;
                        TRACE(hot_str, 11, ecig.AD_BAT_voltage_mv,0);
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = LOW_POWER;
                    }
                    else
                    {
                        if(ecig.smoke_sta != SMOKING)
                        {
                            msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                            sys_cb.smoke_index = SMOKING;
                        }
                        ecig.power_on_flag = 1;
                        ECIG_PWM1_ON();
                        if(!ecig.timer_switch_adc_flag)
                        {
                            ecig.timer_switch_adc_flag = true;
                            saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                        }
                    }
                    //TRACE(hot_str, ecig.timer_switch_acc_cnt,ecig.timer_switch_acc_cnt, ecig.AD_BAT_voltage_mv);
                    ecig.timer_switch_acc_cnt ++;
                }
                else
                {
                    ECIG_PWM1_OFF();
                    if(!ecig.timer_switch_adc_flag)
                    {

                        if (!saradc_adc15_is_bg())
                        {
                            saradc_adc15_analog_select(ADCCH15_ANA_BG);
                        }
                        saradc_kick_start_do(BIT(ADCCH_VBAT) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                        ecig.timer_switch_adc_flag = true;
                    }
                }
                break;
            case 4:
            case 5:
            case 6:
            case 7:
            {
                //加热电压采集
                if (saradc_is_finish() && ecig.timer_switch_adc_flag)
                {
                    //TRACE(not_finish_str, 1, SADCCH, BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH);

                    ecig.timer_switch_adc_flag = false;

                    u32 adc_vbg = saradc_get_value10(ADCCH_BGOP);
                    u32 hot_voltage = saradc_get_value10(ecig.cfg->adc1_ch);
                    ecig.AD_hot_voltage_mv = (hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg) * 4 / 3 / ECIG_VBG_VOLTAGE_MULTIPLE;
                    ecig.AD_hot_voltage = (ecig.AD_hot_voltage_mv << 13) / 1000;
                    //TRACE(vol_hot_str, ecig.timer_cycle_cnt, ecig.timer_switch_acc_cnt,ecig.AD_hot_voltage_mv);
                    ecig.timer_switch_acc_cnt ++;
                }
                else
                {
                    ECIG_PWM1_ON();
                    if(!ecig.timer_switch_adc_flag)
                    {
                        ecig.timer_switch_adc_flag = true;
                        saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                    }
                }
                ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                ecig.p_prev = ecig.p_current;
            }
            break;
            case 8:  //加热电压采集
            {
                if (saradc_is_finish() && ecig.timer_switch_adc_flag && ecig.power_on_flag)
                {
                    //TRACE(not_finish_str, 1, SADCCH, BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH);
                    ecig.timer_switch_adc_flag = false;
                    u32 adc_vbg = saradc_get_value10(ADCCH_BGOP);
                    u32 hot_voltage = saradc_get_value10(ecig.cfg->adc1_ch);
                    ecig.AD_hot_voltage_mv = (hot_voltage * ECIG_VBG_VOLTAGE / adc_vbg) * 4 / 3 / ECIG_VBG_VOLTAGE_MULTIPLE;
                    ecig.AD_hot_voltage = (ecig.AD_hot_voltage_mv << 13) / 1000;

                    ecig.timer_switch_acc_cnt = 8;
                    if (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) <= ecig.cfg->short_res_prop)           //短路保护
                    {
                        ecig.short_circuit_cnt++;
                        ECIG_PWM1_OFF();
                        ecig.power_on_flag = 0;
                        ecig.smoke_sta = SHORT_CIRCUIT;
                        ecig.short_circuit_flag = ecig.smoke_sta;
                        TRACE(hot_str, 12, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = SHORT_CIRCUIT;
                    }
                    else if (ecig.AD_hot_voltage_mv * 10 / abs(ecig.AD_BAT_voltage_mv - ecig.AD_hot_voltage_mv) >= ecig.cfg->open_res_prop)       //开路保护
                    {
                        ECIG_PWM1_OFF();
                        ecig.power_on_flag = 0;
                        ecig.smoke_sta = OPEN_CIRCUIT;
                        ecig.short_circuit_flag = ecig.smoke_sta;
                        TRACE(hot_str, 13, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                        msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                        sys_cb.smoke_index = OPEN_CIRCUIT;
                    }
                    else
                    {
                        ecig.power_on_flag = 1;
                        ecig.smoke_sta = SMOKING;
                        //TRACE(hot_str, 15, ecig.AD_BAT_voltage_mv,ecig.AD_hot_voltage_mv);
                    }
                }
                else
                {
                    ECIG_PWM1_ON();
                    if(!ecig.timer_switch_adc_flag)
                    {
                        ecig.timer_switch_adc_flag = true;
                        saradc_kick_start_do(BIT(ecig.cfg->adc1_ch) | BIT(ADCCH_BGOP) | SADCCH, 0, 0);
                    }
                }
                if(ecig.power_on_flag)
                {
                    ecig.p_current = (u16)((calculate_power(ecig.AD_hot_voltage) >> 13) + ecig.p_prev);
                    if (ecig.p_current >= ecig.cfg->aim_power * ecig.timer_big_cycles_10ms)
                    {
                        ECIG_PWM1_OFF();
                        ecig.power_on_flag = 0;
                        //TRACE(vol_hot_str, ecig.timer_cycle_cnt, ecig.hot_time_cnt,ecig.AD_hot_voltage_mv);
                    }
                    else
                    {
                        ecig.p_prev = ecig.p_current;
                    }
                }
                //TRACE(vol_hot_str, ecig.timer_cycle_cnt, ecig.timer_switch_acc_cnt,ecig.AD_hot_voltage_mv);
            }
            break;
            default :
                break;
        }
        if (ecig.timer_cycle_cnt == (ecig.timer_big_cycles_10ms + 1))     //多加1个t，清零
        {
            ecig.timer_cycle_cnt = 0;
            ecig.timer_switch_acc_cnt = 0;
            ECIG_PWM1_OFF();
            ecig.timer_switch_adc_flag = false;
            //TRACE(hot_str, 32, ecig.cfg->aim_power,ecig.p_current);
            //ecig_vbat_proc();
            ecig.power_on_flag = 1;
            ecig.p_current = 0;
            ecig.p_prev = 0;
            ecig.short_circuit_cnt = 0;
        }
    }
    else
    {
        ecig_adgnd_io_deinit();
        ECIG_PWM1_OFF();
        if (!ecig.mic_sta)
        {
            ecig.hot_time_cnt = 0;
            ecig.hot_time_flag = 0;
            ecig.cnt_1s = 0;
            ecig.timer_cycle_cnt = 0;
            ecig.timer_switch_acc_cnt = 0;
            ecig.timer_switch_adc_flag = false;
            ecig.smoke_sta = IDLE;
            if(ecig.short_circuit_flag != SHORT_CIRCUIT)
            {
                ecig.short_circuit_flag = IDLE;
            }
        }
        if (ecig.short_circuit_cnt >= 10 || ecig.short_circuit_flag == SHORT_CIRCUIT)            //累计判断短路，短路后不再开启
        {
            ecig.power_on_flag = 0;
            //ecig.short_circuit_flag = 1;
            ecig.hot_time_cnt = 0;
            ecig.hot_time_flag = 0;
            if (ecig.cnt_1s == 1)
            {
                printf(short_circuit_str,1,ecig.short_circuit_flag);
                if(ecig.smoke_sta != SHORT_CIRCUIT)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SHORT_CIRCUIT;
                }
            }
        }
        if(ecig.hot_time_cnt >= ecig.cfg->heat_time_max)
        {
            if (ecig.cnt_1s == 1)
            {
                if(ecig.smoke_sta != SMOKE_TIMEOUT)
                {
                    msg_enqueue(EVT_ECIG_SMOKE_REMINDER);
                    sys_cb.smoke_index = SMOKE_TIMEOUT;
                }
                printf(info_8s);
            }
        }
    }
}

#endif

#endif
