#include "include.h"
#include "ute_module_sport.h"
#include "ute_module_newFactoryTest.h"
#include "ute_module_bloodoxygen.h"

sensor_sta_t bsensor;
//获得外设初始化状态
u8 bsp_sensor_init_sta_get(sensor_init_type type)
{
    return (bsensor.init & type);
}

//设置外设初始化状态
void bsp_sensor_init_sta_set(sensor_init_type type)
{
    bsensor.init |= type;
}

//清除外设初始化状态
void bsp_sensor_init_sta_clr(sensor_init_type type)
{
    bsensor.init &= ~type;
}

//开 外设共用电源控制脚 PE2
void bsp_sensor_pe2_pwr_pg_on(void)
{
    GPIOFDE |= BIT(2);
    GPIOFDIR &= ~BIT(2);
    GPIOFCLR |= BIT(2);
}

//关 外设共用电源控制脚 PE2
void bsp_sensor_pe2_pwr_pg_off(void)
{
    GPIOFDE &= ~BIT(2);
    GPIOFDIR |= BIT(2);
    GPIOFSET |= BIT(2);
}

//心率开启
u8 bsp_sensor_hr_init(u8 mode)
{
    // if (bsp_sensor_init_sta_get(SENSOR_INIT_HR)) return true;

    bool init = false;
#if (SENSOR_HR_SEL == SENSOR_HR_TYHX_HX3605)
    init = sensor_hx3605_init(mode);
#elif(SENSOR_HR_SEL == SENSOR_HR_TYHX_HRS3300)
    init = sensor_hrs3300_init(mode);
#elif(SENSOR_HR_SEL == SENSOR_HR_VC30FX)
    vc30fx_pwr_en();
    vc30fx_data.work_mode = mode;
    init = vc30fx_usr_device_init(&vc30fx_data);
#elif (SENSOR_HR_SEL == SENSOR_HR_VCLC09A)
    extern void vclc09_pwr_en(void);
    vclc09_pwr_en();
    vcHr11Mode_t vcMode = 0;
    if (mode == HR_WORK_MODE_SPO2)
    {
        vcMode = VCWORK_MODE_SPO2WORK;
    }
    else if (mode == HR_WORK_MODE_FACTORY0)
    {
        vcMode = VCWORK_MODE_CROSSTALKTEST;
    }
    else
    {
        vcMode = VCWORK_MODE_HRWORK;
    }
    extern vcHr11_t vcHr11;
    vcHr11Init(&vcHr11, vcMode);
#endif

    if (init) bsp_sensor_init_sta_set(SENSOR_INIT_HR);

    return init;
}

//心率关闭
u8 bsp_sensor_hr_stop(void)
{
    // if (!bsp_sensor_init_sta_get(SENSOR_INIT_HR)) return false;
    bool stop = true;
#if (SENSOR_HR_SEL == SENSOR_HR_TYHX_HX3605)
    stop = sensor_hx3605_stop();
#elif(SENSOR_HR_SEL == SENSOR_HR_TYHX_HRS3300)
    stop = sensor_hrs3300_stop();
#elif(SENSOR_HR_SEL == SENSOR_HR_VC30FX)
#if UTE_DRV_HEART_VCXX_REMAIN_POWER_SUPPORT
    vc30fx_data.work_mode = WORK_MODE_WEAR;
    vc30fx_usr_device_init(&vc30fx_data);
#elif UTE_DRV_HEART_VCXX_NIGHT_OPTIMIZE_SLEEP_SUPPORT
    if(uteModuleSystemtimeIsNight() && uteModuleSportGetStepType() == STEP_TYPE_SLEEP)
    {
        vc30fx_data.work_mode = WORK_MODE_WEAR;
        vc30fx_usr_device_init(&vc30fx_data);
    }
    else
#endif
    {
        stop = vc30fx_usr_stop_work();
        vc30fx_pwr_dis();
    }
#elif (SENSOR_HR_SEL == SENSOR_HR_VCLC09A)
    extern vcHr11_t vcHr11;
    stop = !vcHr11StopSample(&vcHr11);
    extern void vclc09_pwr_dis();
    vclc09_pwr_dis();
#endif

    return stop;
}

//心率工作状态
AT(.com_text.vc30fx)
bool bsp_sensor_hr_work_status(void)
{
    bool sta = true;
#if (SENSOR_HR_SEL == SENSOR_HR_VC30FX)
    sta = vc30fx_usr_get_work_status();
#elif (SENSOR_HR_SEL == SENSOR_HR_VCLC09A)
    extern bool vclc09_pwr_sta_get(void);
    sta = vclc09_pwr_sta_get();
#endif
    return sta;
}

//心率工作模式
AT(.com_text.vc30fx)
uint8_t bsp_sensor_hr_get_work_mode(void)
{
    uint8_t mode = 0;
#if (SENSOR_HR_SEL == SENSOR_HR_VC30FX)
    mode = vc30fx_usr_get_work_mode();
#elif (SENSOR_HR_SEL == SENSOR_HR_VCLC09A)
    if(vcHr11.workMode == VCWORK_MODE_SPO2WORK)
    {
        mode = HR_WORK_MODE_SPO2;
    }
    else if (vcHr11.workMode == VCWORK_MODE_CROSSTALKTEST)
    {
        mode = HR_WORK_MODE_FACTORY0;
    }
    else
    {
        mode = HR_WORK_MODE_HR;
    }
#endif
    if(!bsp_sensor_hr_work_status())
    {
        mode = HR_WORK_MODE_UNKNOWN;
    }
    return mode;
}

// AT(.com_text.vc30fx)
void bsp_sensor_hr_algo_input_data_handle(void)
{
    uint8_t sportType = uteModuleSportMoreSportGetType();
    if (sportType == SPORT_TYPE_NONE)
    {
        sportType = SPORT_TYPE_PERIODIC_MONITORING;
    }
#if UTE_MODULE_SPORT_HUNDRED_SUPPORT /*! 运动模式适配心率, xjc 2022-08-31*/
    if (sportType > SPORT_TYPE_FREE_TRAINING)
    {
        if ((sportType == SPORT_TYPE_INDOOR_WALK) ||
            (sportType == SPORT_TYPE_STEP_TRAINING) ||
            (sportType == SPORT_TYPE_OUTDOOR_WALK) ||
            (sportType == SPORT_TYPE_HIKING)) // 室内走路/踏步/户外健走/徒步
        {
            sportType = SPORT_TYPE_WALKING;
        }
        else if ((sportType == SPORT_TYPE_INDOOR_RUN) ||
                 (sportType == SPORT_TYPE_PARKOUR) ||
                 (sportType == SPORT_TYPE_MARATHON)) // 室内跑步/跑酷/马拉松
        {
            sportType = SPORT_TYPE_RUNNING;
        }
        else
        {
            sportType = SPORT_TYPE_FREE_TRAINING;
        }
    }
    if (!uteModuleSportMoreSportIsRuning())
    {
        sportType = SPORT_TYPE_INDOOR_WALK; // SPORT_TYPE_NONE;
    }
#endif

#if (SENSOR_HR_SEL == SENSOR_HR_VC30FX)
// get_vc30fx_device();
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
    if(uteModuleNewFactoryTestGetMode()!= FACTORY_TEST_MODE_NULL)
    {
        // 工厂模式下，必须用1模式的测试血氧
        vc30fx_usr_device_handler(sportType,1);
    }
    else
#endif
    {
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
        if (uteModuleBloodoxygenIsBloodOxygenAutoTesting())
        {
            //定时测试时候，使用模式2测试血氧，即切换动态测试
            vc30fx_usr_device_handler(sportType,2);
        }
        else
#endif
        {
            //手动测试时候，使用模式1测试血氧，即切换静态测试
            vc30fx_usr_device_handler(sportType,1);
        }
    }
#elif (SENSOR_HR_SEL == SENSOR_HR_VCLC09A)
    extern void vcHr11_process(sport_mode_type vcSportMode) ;
    vcHr11_process(sportType);
#endif

}

static bool bsp_sensor_hr_interrupt_flag = false;

AT(.com_text.vc30fx)
bool bsp_sensor_hr_interrupt_flag_get(void)
{
    return bsp_sensor_hr_interrupt_flag;
}

AT(.com_text.vc30fx)
void bsp_sensor_hr_interrupt_flag_set(bool flag)
{
    bsp_sensor_hr_interrupt_flag = flag;
}

//保存心率值
AT(.com_text.vc30fx)
void bsp_sensor_hrs_data_save(u8 bpm)
{
    if (bsensor.hr.back_bpm != bpm)
    {
        bsensor.hr.back_bpm = bpm;
        printf("%s: %d\n", __func__, bpm);
    }
}

//保存血氧值
void bsp_sensor_spo2_data_save(u8 spo2)
{
    if (bsensor.hr.spo2_value != spo2)
    {
        bsensor.hr.spo2_value = spo2;
        printf("%s: %d\n", __func__, spo2);
    }
}

//保存血压值
void bsp_sensor_bp_data_save(u8 sbp, u8 dbp)
{
    if (bsensor.hr.sbp != sbp || bsensor.hr.dbp != dbp)
    {
        bsensor.hr.sbp = sbp;
        bsensor.hr.dbp = dbp;
        printf("%s: sbp->%d, dbp->%d\n", __func__, sbp, dbp);
    }
}

//获取心率值
u8 bsp_sensor_hrs_data_get(void)
{
    return bsensor.hr.back_bpm;
}

//获取血氧值
u8 bsp_sensor_spo2_data_get(void)
{
    return bsensor.hr.spo2_value;
}

//获取血压值
void bsp_sensor_bp_data_get(u8 *sbp, u8 *dbp)
{
    if (NULL == sbp || NULL == dbp)
    {
        return ;
    }

    *sbp = bsensor.hr.sbp;
    *dbp = bsensor.hr.dbp;
}

//获取佩戴状态
bool bsp_sensor_hr_wear_sta_get(void)
{
    bool sta = false;

#if (SENSOR_HR_SEL == SENSOR_HR_TYHX_HX3605)
    sta = sensor_hx3605_wear_sta_get();
#elif(SENSOR_HR_SEL == SENSOR_HR_TYHX_HRS3300)
    sta = sensor_hx3300_wear_sta_get();
#elif(SENSOR_HR_SEL == SENSOR_HR_VC30FX)
    sta = vc30fx_usr_get_wear_status();
#elif(SENSOR_HR_SEL == SENSOR_HR_VCLC09A)
    sta = vcHr11.wearStatus == 1 ? true : false;
#endif

    return sta;
}

//空函数
void sensor_step_init(void) {}
void sensor_step_stop(void) {}
AT(.text.lowpwr.sleep)
void sensor_step_lowpwr_pro(void) {}

AT(.text.lowpwr.sleep)
void sensor_step_pro_isr(void) {}

void sensor_step_wrist_set(bool x) {}
u8   sensor_step_wrist_get(void)
{
    return 0;
}
s8   sensor_step_wkup_get(void)
{
    return 0;
}
void sensor_step_sway_set(bool x) {}
bool sensor_step_sway_get(void)
{
    return 0;
}
bool sensor_step_sway_status_get(void)
{
    return 0;
}
int  sensor_step_cur_steps_get(void)
{
    return 0;
}
int  sensor_step_cur_kcal_get(void)
{
    return 0;
}
int  sensor_step_cur_distance_get(void)
{
    return 0;
}