
/*********************************************************************************************************
*               Copyright(c) 2018, Vcare Corporation. All rights reserved.
**********************************************************************************************************
* @file     module_heart_vc.c
* @brief
* @details
* @author
* @date
* @version
*********************************************************************************************************
*/
#include "include.h"
#include "ute_module_message.h"
#include "ute_module_factorytest.h"
#include "ute_drv_gsensor_common.h"

#if (SENSOR_HR_SEL == SENSOR_HR_VCLC09A)

#include "algo.h"
#include "vcHr11Hci.h"
#include "ute_module_sport.h"
#include "spo2Algo_16bit.h"
#if UTE_MODULE_BREATHRATE_SUPPORT
#include "RspRateEst.h"
#include "ute_module_breathrate.h"
#endif
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
#include "ute_module_emotionPressure.h"
#endif

/* Include your INT,I2C,Timer header file */
//#include "INT.h"
//#include "I2C.h"
//#include "Timer.h"

#define VCLC09_WRITE_ADDR(ADDR)         ((ADDR) << 1)
#define VCLC09_READ_ADDR_UPDATE(ADDR)   ((ADDR) << 1 | 1)
#define VCLC09_READ_ADDR(ADDR)          ((ADDR) << 1 | 1) << 8 | ((ADDR) << 1)

#define GsensorEn 1

const unsigned char arry10[] = {0, 2, 3, 4, 5, 7, 8, 9, 10, 11, 12};
const unsigned char arry20[] = {0, 2, 3, 5, 6, 8, 9, 11, 12, 13, 15, 16, 18, 19, 21, 22, 24, 25, 27, 28};

/* Heart rate value */
int HeartRateValue = 0;

/* Timer clock frequency
   Be used to adjust INT frequency */
const uint32_t mcuOscData = 31250;

/* MCU I2C clock frequency */
const uint16_t mcuI2cClock = 400;

/* Heart rate sample frequency */
const uint8_t vcHr11SampleRate = 25;

/* Spo2 value */
float vcSpo2Value = 0.0;
int real_spo = 0;

/* Spo2 data */
float vcRedPPG = 0.0;
float vcIrPPG = 0.0;

/* INT Flag */
bool vcHr11IRQFlag = 0;

uint16_t GsensorLength = 0;

/* G-Sensor Data */
int16_t xData[40] = {0};
int16_t yData[40] = {0};
int16_t zData[40] = {0};

/* The algorithm of hert rate data struct */
AlgoInputData_t algoInputData;
AlgoOutputData_t algoOutputData;

/* Heart rate data struct */
vcHr11_t vcHr11;

/* Heart rate mode */
vcHr11Mode_t vcMode = VCWORK_MODE_HRWORK;

void vcHr11IRQHandler();

static bool vclc09_pwr_sta = false;

void vclc09_pwr_en(void)        //PF5
{
    printf("%s\n",__func__);
    // if(vcHr11.oscCheckFinishFlag == 0)
    {
        uteModulePlatformDlpsDisable(UTE_MODULE_PLATFORM_DLPS_BIT_HEART); //禁用睡眠，睡眠下无法测量
    }

    if(!vclc09_pwr_sta)
    {
        uteModulePlatformOutputGpioSet(IO_PF5,true);
        delay_5ms(2);
    }

    bsp_i2c_init();
#if GsensorEn
    uteModuleSprotAlgoTimerStop();
#endif
    bsp_sensor_hr_interrupt_flag_set(false);
    vclc09_pwr_sta = true;
}

void vclc09_pwr_dis(void)       //PF5
{
    printf("%s\n",__func__);
    uteModulePlatformOutputGpioSet(IO_PF5,false);
    i2c_gsensor_init();
    uteModuleSportAlgoTimerStart(UTE_MODULE_ALL_SPORT_STEP_ALGORITHMS_TIMER_DURATION);
#if (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    uteModulePlatformOutputGpioSet(IO_PE4,false);
    uteModulePlatformOutputGpioSet(IO_PE5,false);
#elif (CHIP_PACKAGE_SELECT == CHIP_5691G)
    uteModulePlatformOutputGpioSet(IO_PE1,false);
    uteModulePlatformOutputGpioSet(IO_PE2,false);
#endif
    vclc09_pwr_sta = false;
#if (CHIP_PACKAGE_SELECT == CHIP_5691G)
    extab_user_isr_clr(IO_PG6);
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    extab_user_isr_clr(IO_PE7);
#endif
    uteModulePlatformDlpsEnable(UTE_MODULE_PLATFORM_DLPS_BIT_HEART); //恢复睡眠
}

bool vclc09_pwr_sta_get(void)
{
    return vclc09_pwr_sta;
}

/*
 * @brief
 *
 *
 **/
void vcHr11Init(vcHr11_t *pVcHr11,vcHr11Mode_t vcHr11WorkMode)
{
    vcHr11SoftReset(pVcHr11);
    pVcHr11->vcSampleRate = vcHr11SampleRate;
    pVcHr11->mcuOscValue = mcuOscData;
    pVcHr11->mcuSclRate = mcuI2cClock;
    pVcHr11->workMode = vcHr11WorkMode;

    if(pVcHr11->workMode == VCWORK_MODE_HRWORK)
    {
        vcHr11StartSample(pVcHr11);
        Algo_Init();
#if UTE_MODULE_BREATHRATE_SUPPORT
        RespRate_Init();
#endif
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
        if(uteDrvHeartVcxxIsPressureTesting())
        {
            StressEst_Init(-1);
        }
#endif
    }
    else if (pVcHr11->workMode == VCWORK_MODE_CROSSTALKTEST)
    {
        vcHr11StartSample(pVcHr11);
    }
    else if (vcHr11WorkMode == VCWORK_MODE_SPO2WORK)
    {
        vcHr11StopSample(pVcHr11);

        vcHr11StartSample(pVcHr11);
        // vcSpo2AlgoInit();
    }

    cc_time_init();
#if (CHIP_PACKAGE_SELECT == CHIP_5691G)
    extab_user_isr_set(IO_PG6, RISE_EDGE, IOUD_SEL_PD, vcHr11IRQHandler);
    extab_user_isr_mode_set(IO_PG6, MODE_BOTH_AWAKEN_SLEEP);
#elif (CHIP_PACKAGE_SELECT == CHIP_5691C_F)
    extab_user_isr_set(IO_PE7, RISE_EDGE, IOUD_SEL_PD, vcHr11IRQHandler);
    extab_user_isr_mode_set(IO_PE7, MODE_BOTH_AWAKEN_SLEEP);
#endif

}


/*
 * @brief
 *
 *
 **/
// AT(.com_text.vc30fx)
void vcHr11_process(sport_mode_type vcSportMode)
{
    uint8_t algoCallNum = 0;
    uint8_t ppgLength = 25;
    uint8_t vcSportFlag = 0;
    // if (vcHr11IRQFlag)
    {
        // vcHr11IRQFlag = false;

        if (vcHr11.workMode == VCWORK_MODE_HRWORK)
        {
            if(VCHR11RET_UNWEARTOISWEAR == vcHr11GetSampleValues(&vcHr11,&ppgLength))
            {
                Algo_Init();
#if UTE_MODULE_BREATHRATE_SUPPORT
                RespRate_Init();
#endif
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
                if(uteDrvHeartVcxxIsPressureTesting())
                {
                    StressEst_Init(-1);
                }
#endif
            }

            if(vcHr11.vcFifoReadFlag || vcHr11.vcPsFlag)
            {
#if GsensorEn
                /* Gsensor精度要求 */
                /* ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) */

                static int16_t xx[UTE_DRV_GSENSOR_AXIS_DATA_MAX], yy[UTE_DRV_GSENSOR_AXIS_DATA_MAX], zz[UTE_DRV_GSENSOR_AXIS_DATA_MAX];
                memset(xx, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
                memset(yy, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));
                memset(zz, 0, UTE_DRV_GSENSOR_AXIS_DATA_MAX * sizeof(int16_t));

                ute_drv_gsensor_common_axis_data_t *data = NULL;
                uteDrvGsensorCommonReadFifo(&data);
                GsensorLength = data->frameCnt;

                ute_drv_gsensor_common_axis_bit_change_t axisBitChange;
                axisBitChange.inputXaxis = &data->accXaxis[0];
                axisBitChange.inputYaxis = &data->accYaxis[0];
                axisBitChange.inputZaxis = &data->accZaxis[0];
                axisBitChange.outputXaxis = &xx[0];
                axisBitChange.outputYaxis = &yy[0];
                axisBitChange.outputZaxis = &zz[0];
                for (uint8_t i = data->frameCnt; i < UTE_DRV_GSENSOR_AXIS_DATA_MAX; i++)
                {
                    data->accXaxis[i] = data->accXaxis[data->frameCnt - 1];
                    data->accYaxis[i] = data->accYaxis[data->frameCnt - 1];
                    data->accZaxis[i] = data->accZaxis[data->frameCnt - 1];
                }
                // change g-sensor bit
                uteDrvGsensorCommonXYZaxisDataBitChange(&axisBitChange, 50, GSENSOR_DATA_BIT_VCXX);

                //========================================
                //       GsensorEn里的内容均为示例
                //
                //========================================
                if(GsensorLength>=29)
                {
                    //GsensorLength为GsensorFIFO数据长度，当Gsensor采样率为36HZ，800mS中断时长度众数是29个，大于29个数据舍弃；
                    GsensorLength = 29;
                }
                else if(GsensorLength>=1)
                {
                    //小于29个数据补足29个；
                    for(uint8_t i=GsensorLength; i<29; i++)
                    {
                        xData[i]=xData[GsensorLength-1];
                        yData[i]=yData[GsensorLength-1];
                        zData[i]=zData[GsensorLength-1];
                    }
                }

                for(uint8_t i=0; i<20; i++)
                {

                    //cash_num[20]数组为抽样数组，抽样数组由我们提供此处将29个数据抽成20个，匹配800mS中断的20个PPG
                    xData[i]=yData[arry20[i]]>>5;
                    yData[i]=xData[arry20[i]]>>5;
                    zData[i]=zData[arry20[i]]>>5;
                }
#endif
                if(vcHr11.vcFifoReadFlag)
                {
                    vcHr11.vcFifoReadFlag = false;
                    if(vcHr11.wearStatus == VCHR11WEARST_ISWEAR)
                    {
#if GsensorEn
                        if(GsensorLength < ppgLength)//此处为了防止丢中断，保证每个PPG数据参与算法运算，丢中断的情况下，Gsensor数据个数补足到ppgLength；
                        {
                            for(uint8_t i = GsensorLength; i < ppgLength; i++)
                            {
                                xData[i] = xData[GsensorLength - 1];
                                yData[i] = yData[GsensorLength - 1];
                                zData[i] = zData[GsensorLength - 1];
                            }
                        }
#endif

                        algoInputData.envSample = vcHr11.sampleData.envValue[0];
                        for(algoCallNum= 0; algoCallNum < ppgLength; algoCallNum++)
                        {
                            algoInputData.ppgSample = vcHr11.sampleData.ppgValue[algoCallNum];
                            algoInputData.axes.x = xData[algoCallNum];//The direction vertical with ARM.
                            algoInputData.axes.y = yData[algoCallNum];//The direction parallel with ARM.
                            algoInputData.axes.z = zData[algoCallNum];//The direction upside.
                            Algo_Input(&algoInputData, 1000/vcHr11SampleRate, vcSportMode, 0, 0);
#if UTE_MODULE_BREATHRATE_SUPPORT
                            RespInputData_t respInputData;
                            respInputData.ppgSample = algoInputData.ppgSample;
                            respInputData.envSample = algoInputData.envSample;
                            respInputData.axes.x  =algoInputData.axes.x;
                            respInputData.axes.y  =algoInputData.axes.y;
                            respInputData.axes.z  =algoInputData.axes.z;
                            RespRate_Input(&respInputData);
#endif
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
                            if(uteDrvHeartVcxxIsPressureTesting())
                            {
                                StressEst_Input(algoOutputData.hrData,algoOutputData.reliability,xData[0], algoInputData.ppgSample);
                            }
#endif
                        }

                        Algo_Output(&algoOutputData);
                        HeartRateValue = algoOutputData.hrData;

                        if(HeartRateValue >= 0)
                        {
                            bsp_sensor_hrs_data_save(HeartRateValue);
                        }

#if UTE_MODULE_BREATHRATE_SUPPORT
                        {
                            RespOutputData_t pOutputData;
                            RespRate_Output(&pOutputData);
                            if(pOutputData.resData > 0 && pOutputData.resData < UTE_MODULE_BREATHRATE_MAX_VALUE)
                            {
                                uteModuleBreathrateSetValue(pOutputData.resData);
                            }
                            else
                            {
                                uteModuleBreathrateSetValue(0);
                            }
                            //  UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,pOutputData.resData = %d,errType=%d", __func__,pOutputData.resData,pOutputData.errType);
                        }
#endif
                        if(HeartRateValue == -1)
                        {
                            Algo_Init();
#if UTE_MODULE_BREATHRATE_SUPPORT
                            RespRate_Init();
#endif
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
                            if(uteDrvHeartVcxxIsPressureTesting())
                            {
                                StressEst_Init(-1);
                            }
#endif
                        }
#if GsensorEn
                        uteModuleSportInputDataBeforeAlgo();
#endif
                    }
                }
                else
                {
#if SportMotionEn
                    /* 此处的Gsensor的数据直接写0 */
                    green_led_off_state_gsensor_abs_sum_diff_func(xData[0],yData[0],zData[0]);
#endif
                    HeartRateValue = -2;
                }
            }
        }
        else if (vcHr11.workMode == VCWORK_MODE_CROSSTALKTEST)
        {
            vcHr11GetSampleValues(&vcHr11,&ppgLength);
            /* If Pass: */
            // if ((vcHr11.sampleData.maxLedCur >= 100) && (vcHr11.sampleData.preValue[0] <= 2))
            // {
            //PASS：
            //Display the value of vcHr11.sampleData.maxLedCur and vcHr11.sampleData.preValue[0]
            // }
            uteModuleFactoryTestSetVkData(vcHr11.sampleData.preValue[0], vcHr11.sampleData.maxLedCur, vcHr11.sampleData.psValue);
        }
        else if (vcHr11.workMode == VCWORK_MODE_SPO2WORK)
        {
            if(VCHR11RET_UNWEARTOISWEAR == vcHr11GetSampleValues(&vcHr11,&ppgLength))
            {
#if (SENSOR_HR_SEL != SENSOR_HR_VCLC09A)
                vcSpo2AlgoInit();
                vcSportMotionAlgoInit();
#endif
            }
            if(vcHr11.vcFifoReadFlag || vcHr11.vcPsFlag)
            {
#if GsensorEn
                /* ReadGsensorFIFO(+-4G11bit or +-8G12bit 256/g) */
                if(GsensorLength>=29)
                {
                    GsensorLength = 29;
                }
                else if(GsensorLength>=1)
                {
                    for(uint8_t i=GsensorLength; i<29; i++)
                    {
                        xData[i]=xData[GsensorLength-1];
                        yData[i]=yData[GsensorLength-1];
                        zData[i]=zData[GsensorLength-1];
                    }
                }

                for(uint8_t i=0; i<20; i++)
                {
                    xData[i]=yData[arry20[i]]>>5;
                    yData[i]=xData[arry20[i]]>>5;
                    zData[i]=zData[arry20[i]]>>5;
                }
#endif
                if(vcHr11.vcFifoReadFlag)
                {
                    vcHr11.vcFifoReadFlag = 0;
                    if(ppgLength<=50)
                    {
                        for(algoCallNum= 0; algoCallNum < ppgLength/2; algoCallNum++)
                        {
                            vcIrPPG = vcHr11.sampleData.ppgValue[algoCallNum*2];
                            vcRedPPG = vcHr11.sampleData.ppgValue[algoCallNum*2+1];
#if (SENSOR_HR_SEL != SENSOR_HR_VCLC09A)
                            vcSpo2Value = vcSpo2Calculate(vcRedPPG,vcIrPPG);
#if GsensorEn
                            vcSportFlag = vcSportMotionCalculate(xData[algoCallNum], yData[algoCallNum],zData[algoCallNum]);
#endif
#endif
                            if((!vcSportFlag) && (vcSpo2Value > 0))
                            {
                                real_spo = vcSpo2Value;
                                bsp_sensor_spo2_data_save(real_spo);
                            }
                        }
                    }
#if GsensorEn
                    uteModuleSportInputDataBeforeAlgo();
#endif
                }
                else
                {
#if SportMotionEn
                    green_led_off_state_gsensor_abs_sum_diff_func(xData[0],yData[0],zData[0]);
#endif
                }
            }
        }
    }
}


/*
 * @brief
 *
 *
 **/
#if 0
void vcxx_process(void)
{
    /*-----------INSERT BOARD HARDWARE AND SOFTWARE INITIAL FUNCTION------------*/
//    accInit();            //Accelerometer initial +-8G 12bit
    vcHr11PhyInit();        //vcHr11 board initial
    Algo_Init();            //vcHr11 algorithm initial
    vcHr11Init(&vcHr11,vcMode);
    while(1)
    {
        vcHr11_process(vcSportMode);
    }
}
#endif

#if LOG_LVL
AT(.com_text.vc30fx_dbg)
char vc_hr11_irq_str[]="vcHr11IRQHandler,oscCheckFinishFlag=%d,sys_is_sleep=%d\n";
#endif

/*
 * @brief
 *
 *
 **/
AT(.com_text.vc30fx)
void vcHr11IRQHandler()
{
#if LOG_LVL
    printf(vc_hr11_irq_str,vcHr11.oscCheckFinishFlag,sleep_cb.sys_is_sleep);
#endif
    if (vcHr11.oscCheckFinishFlag == 0 || !sleep_cb.sys_is_sleep)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_HEART_ALGO_HANDLER,0);
    }

    if (vcHr11.oscCheckFinishFlag == 1 && sleep_cb.sys_is_sleep)
    {
        bsp_sensor_hr_interrupt_flag_set(true);
    }
}


/*
 * @brief
 *
 *
 **/
void vcHr11PhyInit(void)
{
    /***************************************************************************//**
    * If Reset Pin of vcHr11 is connected to MCU, Initial it;
    * If I2C and SPI is selected by software (Mode Pin is connected), Initial it
    *       according to which interface is used (0 for SPI and 1 for I2C);
    *       otherwise, ignore that;
    * If I2C is selected, and I2C address is controlled by MCU, Initial these two
    *       pins; otherwise, ignore that;
    * If interrupt is enabled, Initial interrupt pin, configurate interrupt pin,
            interrupt edge;
    * If interrupt is enabled, clear IRQ, set priority and enable interrupt;
    * Initial communication interface selected.
    ******************************************************************************/

    /*------------------INSERT YOUR CODE HERE-----------------*/

}


/*
 * @brief
 *
 *
 **/
vcHr11Ret_t vcHr11WriteRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)
{
    /*****************************************************************************
    * When SPI is used, write WRITE command to vcHr11 FIRSTLY,
    * then write len byte data to register(s) start from startAddress
    * to register(s) pointed by pRegisters.
    *
    * When I2C is used, write Slave Address and write flag to vcHr11 firstly,
    * then write len byte data to register(s) start from startAddress
    * to register(s) pointed by pRegisters.
    ******************************************************************************/


    /*------------------INSERT YOUR CODE HERE-----------------*/
    if(!vclc09_pwr_sta)
    {
        return VCHR11RET_ISERR;
    }
    if(sys_cb.gsensor_iic_en)
    {
        bsp_i2c_init();
    }
    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | WDATA;
    bsp_hw_i2c_tx_buf(i2c_cfg, VCLC09_WRITE_ADDR(0x33), startAddress, pRegisters, len);

    return VCHR11RET_ISOK;
}


/*
 * @brief
 *
 *
 **/
vcHr11Ret_t vcHr11ReadRegisters(uint8_t startAddress, uint8_t *pRegisters, uint8_t len)
{
    /*****************************************************************************
    * When SPI is used, write READ command to vcHr11 FIRSTLY,
    * then read len byte data from register(s) start from startAddress
    * to register(s) pointed by pRegisters.
    *
    * When I2C is used, write Slave Address and read flag to vcHr11 firstly,
    * then read len byte data from register(s) start from startAddress
    * to register(s) pointed by pRegisters.
    ******************************************************************************/


    /*------------------INSERT YOUR CODE HERE-----------------*/
    if(!vclc09_pwr_sta)
    {
        return VCHR11RET_ISERR;
    }
    if(sys_cb.gsensor_iic_en)
    {
        bsp_i2c_init();
    }
    uint32_t i2c_cfg = START_FLAG0 | DEV_ADDR0 | REG_ADDR_0 | START_FLAG1 | DEV_ADDR1;
    bsp_hw_i2c_rx_buf(i2c_cfg, VCLC09_READ_ADDR(0x33), startAddress, pRegisters, len);

    return VCHR11RET_ISOK;
}


/*
 * @brief
 *
 *
 **/
uint32_t vcHr11GetRtcCountFromMCU(void)
{
    /*****************************************************************************
    * Get the time difference between two interrupts.
    ******************************************************************************/

    /*------------------INSERT YOUR CODE HERE-----------------*/

    /*------------------THE CODE FOR -----------------*/
    /*
        return RTC_GetCounter();
    */
    return cc_time_count() / 32 + 1;
}
#endif
