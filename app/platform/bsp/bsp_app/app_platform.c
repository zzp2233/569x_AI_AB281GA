#include "include.h"
#include "func_cover.h"
#include "ute_module_systemtime.h"
#define TRACE_EN                0
#if TRACE_EN
#define TRACE(...)              printf(__VA_ARGS__)
#else
#define TRACE(...)
#endif

#if (USE_APP_TYPE == USE_AB_APP)
void app_ab_data_storage_process(void);
void app_ab_data_storage_init(void);
#elif (USE_APP_TYPE == USE_UTE_APP)

///---------��������,��������---------
co_timer_t ute_remind_timer;

//������,һ,��,��,��,��,�� -> BIT(6) BIT(0) BIT(1) BIT(2) BIT(3) BIT(4) BIT(5)
static const uint8_t rtc_weak_mask[7] = {0x40, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20};

static uint8_t wday_rtc_to_proc(uint8_t wday)
{
    uint8_t ret = 0;
    if (wday < 7)
    {
        ret = rtc_weak_mask[wday];
    }
    return ret;
}

static bool ute_alarm_check(tm_t *now_time)
{
    uint8_t week_mask = wday_rtc_to_proc(now_time->weekday);
    ute_module_systemtime_one_alarm_t* alarm_p = ab_zalloc(sizeof(ute_module_systemtime_one_alarm_t));

    if (alarm_p == NULL)
    {
        printf("%s malloc err!!\n", __func__);
        return false;
    }

    if (!week_mask)
    {
        goto __err;
    }

    for (uint8_t i = 0; i < uteModuleSystemtimeGetAlarmMaxCnt(); i++)
    {

        //��ȡ�û�����
        uteModuleSystemtimeGetAlarm(alarm_p, i);

        uint8_t cycle = uteModuleSystemtimeGetAlarmCycle(i);
        if (alarm_p->durationTimeSec == 0)           //��������Ĭ�����ʱ��S
        {
            alarm_p->durationTimeSec = UTE_LOCAL_ALARM_DEFAULT_RING_TIMES;
        }

        if ((alarm_p->repeatRemindHour == 0) || (alarm_p->repeatRemindMin == 0))
        {
            alarm_p->repeatRemindHour = alarm_p->hour;
            alarm_p->repeatRemindMin = alarm_p->min;
        }

        TRACE("i[%d] num[%d] cycle[0x%02x] mask[0x%02x] alarm[%02d:%02d] now[%02d:%02d] repeat[%02d,%02d,%d] \n", \
              i, uteModuleSystemtimeGetAlarmMaxCnt(), cycle, week_mask, alarm_p->hour, alarm_p->min, now_time->hour, now_time->min,
              alarm_p->repeatRemindHour, alarm_p->repeatRemindMin, alarm_p->isRepeatRemindOpen);
        if ((cycle & week_mask) > 0 || (cycle >> 7))  //�������ֻ����һ��
        {
            if (alarm_p->isRepeatRemindOpen)        //̰˯ʱ��
            {
                if ((alarm_p->repeatRemindHour == now_time->hour) && (alarm_p->repeatRemindMin == now_time->min))  //��ʱ��
                {
                    TRACE("repeat alarm[%d] ring, [%02d:%02d]\n", i, alarm_p->repeatRemindHour, alarm_p->repeatRemindMin);
                    uteModuleSystemtimeSetAlarmRingIndex(i);
                    sys_cb.cover_index = REMIND_COVER_ALARM;

                    goto __exit;
                }
            }
            else if (alarm_p->isOpen)     //�û�����
            {
                if ((alarm_p->hour == now_time->hour) && (alarm_p->min == now_time->min))  //��ʱ��
                {
                    TRACE("alarm[%d] ring, [%02d:%02d]\n", i, alarm_p->hour, alarm_p->min);
                    uteModuleSystemtimeSetAlarmRingIndex(i);
                    //alarm_later_check();
                    sys_cb.cover_index = REMIND_COVER_ALARM;
                    if (cycle >> 7)
                    {
                        alarm_p->isOpen = false;
                    }
                    goto __exit;
                }
            }
            else
            {
                //goto __err;
            }
        }

        //goto __err;
    }

    goto __err;

__exit:
    uteModuleSystemtimeSetAlarm(*alarm_p, uteModuleSystemtimeGetAlarmRingIndex());
    ab_free(alarm_p);
    return true;

__err:
    ab_free(alarm_p);
    return false;
}

//1s���Ӽ��ص�����
//AT(.text.lowpwr.sleep)
void ute_remind_check_1s_pro(void)
{
    static u8 rtc_cal_cnt_bkp;
    static u32 rtccnt_tmp;
    static u8 minute_bkp;
    static bool lowpwr_sta_bkp;
    bool lowpwr_sta = bsp_system_is_sleep()/* | sys_cb.idle_sta*/;
    tm_t tm_now;

    if (!lowpwr_sta)
    {
        rtccnt_tmp = RTCCNT;
    }
    else        //ʡ��/����ģʽ��RTC������
    {
        if (lowpwr_sta_bkp == false)      //���ν���
        {
            rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
        }
        if (rtc_cal_cnt_bkp != sys_cb.rtc_cal_cnt)      //RTC��У׼��ͬ��У׼
        {
            rtc_cal_cnt_bkp = sys_cb.rtc_cal_cnt;
            rtccnt_tmp = RTCCNT;
            printf("RTC calibrated!\n");
        }
        else
        {
            rtccnt_tmp++;
        }
    }

    tm_now = time_to_tm(rtccnt_tmp);
    TRACE("tm_now[%02d:%02d:%02d]\n", tm_now.hour, tm_now.min, tm_now.sec);
    if (minute_bkp != tm_now.min && tm_now.sec == 0)   //1min���һ��
    {
        TRACE("date_now[%d.%d.%d week%d]\n", tm_now.year, tm_now.mon, tm_now.day, tm_now.weekday);
        minute_bkp = tm_now.min;
        if (((uteModuleSystemtimeGetAlarmTotalCnt()) && ute_alarm_check(&tm_now)))   //|| interval_remind_check(&tm_now)) {   //�������� //|| ��������
        {
            sys_cb.remind_tag = true;
        }
    }

    lowpwr_sta_bkp = lowpwr_sta;
}

void app_ute_remind_init(void)
{
    // co_timer_set(&ute_remind_timer, 1000, TIMER_REPEAT, LEVEL_LOW_PRI, ute_remind_check_1s_pro, NULL);
    // co_timer_set_sleep(&ute_remind_timer, true);
    uteModuleSystemtimeRegisterSecond(ute_remind_check_1s_pro);
}
#endif

void app_platform_process(void)
{
#if (USE_APP_TYPE == USE_AB_APP)
    app_ab_data_storage_process();
#endif
}

void app_platform_init(void)
{
#if (USE_APP_TYPE == USE_AB_APP)
    app_ab_data_storage_init();
#endif

    //�����������������ʱ��
    // app_ute_remind_init();

}
