/**
 *@file
 *@brief    ����ѹ������ģ��
 *@details
 *@author   xjc
 *@date     2022-02-15
 *@version  v1.0
 */
#include "ute_module_log.h"
#include "ute_module_message.h"
#include "ute_module_emotionPressure.h"
#include "ute_module_protocol.h"
#include "ute_module_profile_ble.h"
#include "ute_module_heart.h"
#include "ute_application_common.h"
#include "ute_drv_motor.h"
#include "ute_drv_battery_common.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_sport.h"
#include "ute_module_crc.h"
#include "ute_module_factoryTest.h"
#include "ute_module_newFactoryTest.h"

#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
ute_module_emotion_pressure_data_t uteModuleEmotionPressureData;

/* ������ zn.zeng 2022-02-14*/
void *uteModuleEmotionPressureMute;

#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT
static uint32_t avgPressureCnt = 0;
#endif
static void uteModuleEmotionPrintfLog(const char *format, ...)
{
#if (UTE_LOG_EMOTION_PRESSURE_LVL && !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT)
    //  uteModuleLogPrintf(UTE_LOG_EMOTION_PRESSURE_LVL,format);
    extern uint8_t uteModuleLogArrayBuff[UTE_MODULE_LOG_MAX_BUFF_SIZE];
    int size = 0;
    va_list argList;
    va_start(argList, format);
    size = snprintf((void *)&uteModuleLogArrayBuff[0], UTE_MODULE_LOG_MAX_BUFF_SIZE, "%s:", &uteModuleLogTagString[UTE_LOG_EMOTION_PRESSURE_LVL][0]);
    size = vsnprintf((void *)&uteModuleLogArrayBuff[size], UTE_MODULE_LOG_MAX_BUFF_SIZE - size, format, argList);
    va_end(argList);
    // DBG_DIRECT("%s",&uteModuleLogArrayBuff[0]);
    // APP_PRINT_INFO1("%s",TRACE_STRING(&uteModuleLogArrayBuff[0]));
#endif
}

#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
void uteDrvHeartVcxxStartPressureSample(void)
{
    StressEst_Init(-1);
    uteModuleEmotionPressureData.isVkPressureTesting = true;
    uteModuleEmotionPressureData.vkPressureValue = 0;
}

void uteDrvHeartVcxxStopPressureSample(void)
{
    uteModuleEmotionPressureData.isVkPressureTesting = false;
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
    uteModuleEmotionPressureData.isVkPressureTesting = false;
    StressOutputData_t vkPressureData;
    StressEst_Output(&vkPressureData);
    if (vkPressureData.strData > 0 && vkPressureData.strData < 100)
    {
        uteModuleEmotionPressureData.vkPressureValue = vkPressureData.strData;
    }
    else
    {
        uteModuleEmotionPressureData.vkPressureValue = 0xFF;
    }
    uint8_t EmotionValue = 2;
    if (uteModuleEmotionPressureData.vkPressureValue == 0xFF)
    {
        EmotionValue = 0xFF;
    }
    else
    {
        if (uteModuleEmotionPressureData.vkPressureValue > 80)
        {
            EmotionValue = 0;
        }
        else if (uteModuleEmotionPressureData.vkPressureValue > 30)
        {
            EmotionValue = 1;
        }
    }
    UTE_MODULE_LOG(1, "%s,.emotionValue=%d,.pressureValue=%d", __func__, EmotionValue, uteModuleEmotionPressureData.vkPressureValue);
    uteModuleEmotionPressureSetEmotionPressureValue(EmotionValue, uteModuleEmotionPressureData.vkPressureValue, 0);
#else
    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,error,vcxx pressure is not define", __func__);
#endif
}

bool uteDrvHeartVcxxIsPressureTesting(void)
{
    return uteModuleEmotionPressureData.isVkPressureTesting;
}

uint8_t uteDrvHeartVcxxGetVkPressureValue(void)
{
    return uteModuleEmotionPressureData.vkPressureValue;
}
#endif

/**
 *@brief     ��ʼ������
 *@details
 *@author    xjc
 *@date      2022-02-15
 */
void uteModuleEmotionPressureInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, NULL, NULL);
#endif
    uteModulePlatformCreateMutex(&uteModuleEmotionPressureMute);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,uteModuleEmotionPressureMute = 0x%d,", __func__, uteModuleEmotionPressureMute);
    memset(&uteModuleEmotionPressureData, 0, sizeof(ute_module_emotion_pressure_data_t));
#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT
    memcpy(uteModuleEmotionPressureData.algoKeyCode, EMOTION_PRESSURE_DEFAULT_KEY_CODE, 88);
    memcpy(uteModuleEmotionPressureData.algoReqCode, EMOTION_PRESSURE_DEFAULT_REQ_CODE, 64);
#endif
    uteModuleEmotionPressureData.emotionValue = 0xFF;
    uteModuleEmotionPressureData.pressureValue = 0xFF;
    uteModuleEmotionPressureData.fatigueValue = 0xFF;
#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT
    uteModuleEmotionPressureData.curDayPressureMaxValue = 0;
    uteModuleEmotionPressureData.curDayPressureMinValue = 0xff;
    uteModuleEmotionPressureData.curDayPressureAvgValue = 0;
    uteModuleEmotionPressureData.lastPressureValue = 0;
#endif
    uteModuleSystemtimeRegisterSecond(uteModuleEmotionPressureEverySecond);

#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά���㷨������
#if UTE_DRV_BLOODPRESSURE_DN02_SUPPORT
    // �㷨�����ڴ�
    int status = init_algorithm(uteModuleEmotionPressureData.ppgData.algorithmBuff, EMOTION_PRESSURE_ALGORITHM_BUFF_MAX_LEN);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,algorithm init status = %d", __func__, status);
#else
    // �㷨��ȡϵͳʱ��
    extern_get_time_cb_reg(uteModuleEmotionPressureAlgoGetSystemTimeHandler);
#endif
    extern_printf_cb_reg(uteModuleEmotionPrintfLog);
#endif
}

/**
 *@brief     ��ȡconfig
 *@details
 *@author    xjc
 *@date      2022-02-15
 */
void uteModuleEmotionPressureReadConfig(void)
{
    void *file = NULL;
    /*! ����ѹ������,xjc 2022-02-15*/
    uint8_t readbuff[8];
    readbuff[0] = UTE_MODULE_EMOTION_PRESSURE_IS_AUTO_OPEN;
    readbuff[1] = UTE_MODULE_EMOTION_PRESSURE_IS_AUTO_TIME_BUCKET_OPEN;
    readbuff[2] = (UTE_MODULE_EMOTION_PRESSURE_AUTO_INTERVAL_MIN >> 8) & 0xff;
    readbuff[3] = (UTE_MODULE_EMOTION_PRESSURE_AUTO_INTERVAL_MIN) & 0xff;
    readbuff[4] = (UTE_MODULE_EMOTION_PRESSURE_TIME_BUCKET_START >> 8) & 0xff;
    readbuff[5] = (UTE_MODULE_EMOTION_PRESSURE_TIME_BUCKET_START) & 0xff;
    readbuff[6] = (UTE_MODULE_EMOTION_PRESSURE_TIME_BUCKET_END >> 8) & 0xff;
    readbuff[7] = (UTE_MODULE_EMOTION_PRESSURE_TIME_BUCKET_END) & 0xff;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_SET_INFO, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 8);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleEmotionPressureData.isAutoTesting = readbuff[0];
    uteModuleEmotionPressureData.isAutoTimeBucketTesting = readbuff[1];
    uteModuleEmotionPressureData.intervalMin = readbuff[2] << 8 | readbuff[3];
    uteModuleEmotionPressureData.startTimeHourMin = readbuff[4] << 8 | readbuff[5];
    uteModuleEmotionPressureData.endTimeHourMin = readbuff[6] << 8 | readbuff[7];
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,isAutoTesting=%d,intervalMin=%d", __func__, uteModuleEmotionPressureData.isAutoTesting, uteModuleEmotionPressureData.intervalMin);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__, uteModuleEmotionPressureData.startTimeHourMin, uteModuleEmotionPressureData.endTimeHourMin);

    /*! ����ѹ��������,xjc 2022-02-15*/
#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // ά���㷨�رռ�����  add  by pcm 2023-08-21
    uint8_t reqCodeBuff[EMOTION_PRESSURE_REQ_CODE_LEN];
    memset(reqCodeBuff, 0xFF, sizeof(reqCodeBuff));
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_REQ_CODE, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &reqCodeBuff[0], EMOTION_PRESSURE_REQ_CODE_LEN);
        uteModuleFilesystemCloseFile(file);
    }
    memcpy(uteModuleEmotionPressureData.algoReqCode, reqCodeBuff, EMOTION_PRESSURE_REQ_CODE_LEN);
    // APP_PRINT_INFO2("%s,.algoReqCode = %s ", __func__, TRACE_BINARY(EMOTION_PRESSURE_REQ_CODE_LEN, &uteModuleEmotionPressureData.algoReqCode[0]));

    /*! ����ѹ��������,xjc 2022-02-15*/
    uint8_t keyCodeBuff[EMOTION_PRESSURE_KEY_CODE_LEN];
    memset(keyCodeBuff, 0xFF, sizeof(keyCodeBuff));
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_KEY_CODE, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &keyCodeBuff[0], EMOTION_PRESSURE_KEY_CODE_LEN);
        uteModuleFilesystemCloseFile(file);
    }
    memcpy(uteModuleEmotionPressureData.algoKeyCode, keyCodeBuff, EMOTION_PRESSURE_KEY_CODE_LEN);
    // APP_PRINT_INFO2("%s,.algoKeyCode = %s ", __func__, TRACE_BINARY(EMOTION_PRESSURE_KEY_CODE_LEN, &uteModuleEmotionPressureData.algoKeyCode[0]));

    /*! ����ѹ���㷨����״̬,xjc 2022-02-15*/
    uteModuleEmotionPressureActiveAlgo(false);
#endif
}

#if (!UTE_DRV_BLOODPRESSURE_DN02_SUPPORT && !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT)
void uteModuleEmotionPressureAlgoGetSystemTimeHandler(lib_time_t *crt_time)
{
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    crt_time->year = time.year;
    crt_time->month = time.month;
    crt_time->day = time.day;
    crt_time->hour = time.hour;
    crt_time->minute = time.min;
    crt_time->second = time.sec;
}
#endif

/**
 *@brief     ����ѹ��ÿ�뺯��
 *@details   ��Ҫע�ᵽ��ʱ�䣬ÿ��ִ��һ��
 *@author    xjc
 *@date      2021-07-23
 */
void uteModuleEmotionPressureEverySecond(void)
{
    if (!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    /*! ���β����߼� xjc, 2022-02-15  */
    if (uteModuleEmotionPressureData.isSingleTesting)
    {
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,.testingSecond=%d", __func__, uteModuleEmotionPressureData.testingSecond);
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά���㷨����ѹ��ֵ��55���ֵ
        if (uteModuleEmotionPressureIsWear())
        {
            if (uteModuleEmotionPressureData.testingSecond > UTE_DRV_EMOTION_PRESSURE_TEST_TIMEOUT_SECOND)
            {
                if (!uteModuleEmotionPressureData.isEmotionPressureAutoTestFlag)
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
                }
                uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_SUCCESS);
            }
            else
            {
                uteModuleEmotionPressureData.testingSecond++;
            }
        }
        else
        {
            uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_UNWEAR);
        }
#else
#if (UTE_DRV_HEART_VC30FX_SUPPORT & UTE_DRV_HEART_VC30S_SUPPORT)
        // uint8_t id = uteDrvheartvc30fxEmotionReadId();
        if (uteModuleEmotionPressureIsWear() && (uteModuleHeartGetWorkMode() == (uteDrvHeartVcxxCommonFunction->id == UTE_DRV_HEART_ID_VC30FX ? VC30FX_WORK_MODE_IRPPG : VCWORK_MODE_IRPPG)))
#else
        if (uteModuleEmotionPressureIsWear() && (uteModuleHeartGetWorkMode() == VCWORK_MODE_IRPPG))
#endif
        {
            if (uteModuleEmotionPressureData.testingSecond > UTE_DRV_EMOTION_PRESSURE_TEST_TIMEOUT_SECOND)
            {
                if (!uteModuleEmotionPressureData.isEmotionPressureAutoTestFlag)
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
                }
                uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_TIMEOUT);
            }
            else
            {
                uteModuleEmotionPressureData.testingSecond++;
            }
        }
        else
        {
            uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_UNWEAR);
        }
#endif
    }

    /*! �Զ������߼� xjc, 2022-02-15  */
    if (uteModuleEmotionPressureData.isAutoTesting)
    {
        bool isNeedAutoTest = false;
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        uint32_t oneDaySec = time.hour * 3600 + time.min * 60 + time.sec;
        if (uteModuleEmotionPressureData.isAutoTimeBucketTesting)
        {
            uint16_t currentTime = ((time.hour << 8 & 0xff00) | time.min);
            if (uteModuleEmotionPressureData.endTimeHourMin > uteModuleEmotionPressureData.startTimeHourMin)
            {
                if ((currentTime >= uteModuleEmotionPressureData.startTimeHourMin) && (currentTime <= uteModuleEmotionPressureData.endTimeHourMin))
                {
                    isNeedAutoTest = true;
                }
            }
            else
            {
                if ((currentTime >= uteModuleEmotionPressureData.startTimeHourMin) || (currentTime <= uteModuleEmotionPressureData.endTimeHourMin))
                {
                    isNeedAutoTest = true;
                }
            }
            if (((oneDaySec % (60 * (uteModuleEmotionPressureData.intervalMin))) == 60) && isNeedAutoTest) // ��һ�����ٲ��ԣ���ֹ��������Ҫ����Sensor�Ķ�ʱ���Գ�ͻ
            {
                isNeedAutoTest = true;
            }
            else
            {
                isNeedAutoTest = false;
            }
        }
        else
        {
            if ((oneDaySec % (60 * (uteModuleEmotionPressureData.intervalMin))) == 60) // ��1�����ٲ�������ֹ��Ѫ���Զ����Գ�ͻ
            {
                isNeedAutoTest = true;
            }
        }
        if ((uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_NO_CHARGE) &&
            (!uteModuleSportMoreSportIsRuning()) &&
            (uteApplicationCommonGetSystemPowerOnSecond() > 5) &&
            (uteModuleFactoryTestGetCurrTestItem() == TEST_ITEM_NONE) &&
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
            (uteModuleNewFactoryTestGetMode() == FACTORY_TEST_MODE_NULL) &&
#endif
            // (!uteModuleBreathrateIsTesting()) &&
            // (!uteModuleGetBreathTrainingStatus()) &&
            (!uteModuleHeartIsSingleTesting())
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
            && (!uteModuleBloodoxygenIsTesting())
#endif
           )
        {
            if (isNeedAutoTest && (!uteModuleEmotionPressureData.isSingleTesting))
            {
                uteModuleEmotionPressureData.isEmotionPressureAutoTestFlag = true;
                uteModuleEmotionPressureStartSingleTesting(false);
            }
            else
            {
                isNeedAutoTest = false;
            }
        }
    }
#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,curDayPressureAvgValue=%d,curDayPressureMinValue=%d", __func__, uteModuleEmotionPressureData.curDayPressureAvgValue, uteModuleEmotionPressureData.curDayPressureMinValue);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,curDayPressureMaxValue=%d,lastPressureValue=%d", __func__, uteModuleEmotionPressureData.curDayPressureMaxValue, uteModuleEmotionPressureData.lastPressureValue);
#endif
}

/**
 *@brief        ��ʼ���β���
 *@details
 *@param[in] bool isAppStart �Ƿ�APP��������
 *@author       xjc
 *@date       2021-07-21
 */
void uteModuleEmotionPressureStartSingleTesting(bool isAppStart)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    if (isAppStart)
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_EMOTION_PRESSURE_START_SINGLE_TESTING, 1);
    }
    else
    {
        uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_EMOTION_PRESSURE_START_SINGLE_TESTING, 0);
    }
}

/**
 *@brief        ��ʼ���β���
 *@details
 *@param[in] bool isAppStart �Ƿ�APP��������
 *@author       xjc
 *@date       2021-07-21
 */
void uteModuleEmotionPressureStartSingleTestingMsgHandler(uint32_t param)
{
    bool isAppStart = false;
    if (param == 1)
    {
        isAppStart = true;
    }
#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά���㷨�رռ�������
#if (UTE_DRV_HEART_VC30FX_SUPPORT & UTE_DRV_HEART_VC30S_SUPPORT)
    // uint8_t id = uteDrvheartvc30fxEmotionReadId();
    if (uteModuleEmotionPressureIsWear() && (uteModuleHeartGetWorkMode() == (uteDrvHeartVcxxCommonFunction->id == UTE_DRV_HEART_ID_VC30FX ? VC30FX_WORK_MODE_IRPPG : VCWORK_MODE_IRPPG)))
#else
    if (uteModuleHeartGetWorkMode() == VCWORK_MODE_IRPPG)
#endif
    {
        uteModuleEmotionPressureData.isSingleTesting = true;
        return;
    }
    if (!uteModuleEmotionPressureData.isAlgoActive) // δ����
    {
#if UTE_MODULE_EMOTION_SAVE_REQCODE_PRESSURE_SUPPORT
        uteModuleEmotionPressureActiveAlgo(true);
#endif
        return;
    }
#endif

    uteModuleEmotionPressureData.isSingleTesting = true;
    uteModuleEmotionPressureData.isAppStartTesting = isAppStart;
    uteModuleEmotionPressureData.emotionValue = 0xFF;
    uteModuleEmotionPressureData.pressureValue = 0xFF;
    uteModuleEmotionPressureData.fatigueValue = 0xFF;
    uteModuleEmotionPressureData.testingSecond = 0;
#if 0 // UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT
    uteModuleEmotionPressureData.curDayPressureMaxValue = 0;
    uteModuleEmotionPressureData.curDayPressureMinValue = 0xff;
    uteModuleEmotionPressureData.curDayPressureAvgValue = 0;
    uteModuleEmotionPressureData.lastPressureValue = 0;
#endif
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά�Ϳ�ʼ�����㷨����
    uteModuleHeartStartSingleTesting(TYPE_HEART);
    uteDrvHeartVcxxStartPressureSample();
#else
    uteModuleEmotionPressureData.stopSendMidValue = false;
    memset(&uteModuleEmotionPressureData.ppgData, 0, sizeof(emotion_pressure_ppg_data_t));
    uteModuleHeartStartSingleTestingMsgHandler((uint32_t)TYPE_EMOTION_PRESSURE);
#endif
    uint8_t startCmdRsp[2] = {CMD_EMOTION_PRESSURE_TEST, 0x11};
    uteModuleProfileBleSendToPhone(&startCmdRsp[0], 2);
}

/**
 *@brief        �������β���
 *@details
 *@param[in] uint8_t stopReasion ����ԭ�� EMOTION_PRESSURE_STOP_REASION_T
 *@author       xjc
 *@date       2021-07-21
 */
void uteModuleEmotionPressureStopSingleTesting(uint8_t stopReasion)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_EMOTION_PRESSURE_STOP_SINGLE_TESTING, (uint32_t)stopReasion);
}

/**
 *@brief        �������β���
 *@details
 *@param[in] uint8_t stopReasion ����ԭ�� EMOTION_PRESSURE_STOP_REASION_T
 *@author       xjc
 *@date       2021-07-21
 */
void uteModuleEmotionPressureStopSingleTestingMsgHandler(uint32_t param)
{
    uint8_t stopReasion = (uint32_t)param;
#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά���㷨�����ã�����Ҫ�ж�ģʽ
#if (UTE_DRV_HEART_VC30FX_SUPPORT & UTE_DRV_HEART_VC30S_SUPPORT)
    // uint8_t id = uteDrvheartvc30fxEmotionReadId();
    if (uteModuleHeartGetWorkMode() != (uteDrvHeartVcxxCommonFunction->id == UTE_DRV_HEART_ID_VC30FX ? VC30FX_WORK_MODE_IRPPG : VCWORK_MODE_IRPPG))
#else
    if (uteModuleHeartGetWorkMode() != VCWORK_MODE_IRPPG)
#endif
    {
        uteModuleEmotionPressureData.isSingleTesting = false;
        return;
    }
#endif

    if (!uteModuleEmotionPressureData.isSingleTesting)
        return;

#if 0 // UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT&&!UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
    uteModuleEmotionPressureData.lastPressureValue = uteModuleEmotionPressureData.pressureValue;
    uteModuleCalculateMaxMinAvgPressValue();
#endif
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά���㷨ֹͣ���ԺͲ���
    uteModuleHeartStopSingleTesting(TYPE_HEART);
    uteDrvHeartVcxxStopPressureSample();

    if ((uteDrvHeartVcxxGetVkPressureValue() == 0) || (uteDrvHeartVcxxGetVkPressureValue() == 0xff))
    {
        stopReasion = EP_STOP_REASION_TIMEOUT;
    }
#endif

    if ((!uteModuleEmotionPressureData.isEmotionPressureAutoTestFlag) && (stopReasion == EP_STOP_REASION_SUCCESS))
    {
        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME, UTE_MOTOR_INTERVAL_TIME, 1);
    }

    uint8_t stopCmd[6];
    stopCmd[0] = CMD_EMOTION_PRESSURE_TEST;
    stopCmd[1] = 0x00;
    stopCmd[2] = uteModuleEmotionPressureData.emotionValue;
    stopCmd[3] = uteModuleEmotionPressureData.pressureValue;
    stopCmd[4] = uteModuleEmotionPressureData.fatigueValue;
    stopCmd[5] = stopReasion;
    uteModuleProfileBleSendToPhone(&stopCmd[0], 6);
#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 APP��������̲���Ҫ�������ѹ����ֵ���ڷ�Χ�ڣ����Ƿ��ش���״̬
    if (uteModuleEmotionPressureData.isAppStartTesting)
    {
        if (stopReasion != EP_STOP_REASION_SUCCESS)
        {
            uteModuleEmotionPressureData.emotionValue = 0xFF;
            uteModuleEmotionPressureData.pressureValue = 0xFF;
            uteModuleEmotionPressureData.fatigueValue = 0xFF;
#if 0 // UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT&&!UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
            uteModuleEmotionPressureData.curDayPressureMaxValue = 0;
            uteModuleEmotionPressureData.curDayPressureMinValue = 0xff;
            uteModuleEmotionPressureData.curDayPressureAvgValue = 0;
            uteModuleEmotionPressureData.lastPressureValue = 0;
#endif
        }
    }
#endif
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,.emotionValue=%d,.pressureValue=%d,.fatigueValue=%d", __func__, uteModuleEmotionPressureData.emotionValue, uteModuleEmotionPressureData.pressureValue, uteModuleEmotionPressureData.fatigueValue);
    /*! ��������xjc, 2022-02-15  */
    if ((uteModuleEmotionPressureData.emotionValue != 0xFF) &&
        (uteModuleEmotionPressureData.pressureValue != 0) && (uteModuleEmotionPressureData.pressureValue != 0xFF))
    {
        uteModuleEmotionPressureAutoSaveData();
#if UTE_MODULE_PRESSURE_PERCENT_SUPPORT
        uteModuleEmotionPressureCalculatePressurePercent();
#endif
    }
    uteModuleEmotionPressureData.isSingleTesting = false;
    uteModuleEmotionPressureData.isAppStartTesting = false;
    uteModuleEmotionPressureData.isEmotionPressureAutoTestFlag = false;
    uteModuleEmotionPressureData.testingSecond = 0;
#if UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά���㷨��������������
    uteModuleHeartStopSingleTesting(TYPE_HEART);
    uteDrvHeartVcxxStopPressureSample();
#else
    uteModuleEmotionPressureData.stopSendMidValue = true;
    uteModuleHeartStopSingleTestingMsgHandler((uint32_t)TYPE_EMOTION_PRESSURE);
#endif
}
/**
 *@brief        �Ƿ����
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
bool uteModuleEmotionPressureIsWear(void)
{
    bool isWear = false;
    isWear = uteModuleHeartIsWear();
    return isWear;
}

#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά���㷨������������
/**
 *@brief        ��������ѹ��ֵ������
 *@details      APP�·�����Ȩ������
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureSetKeyCode(uint8_t *keyCode)
{
    memcpy(uteModuleEmotionPressureData.algoKeyCode, keyCode, EMOTION_PRESSURE_KEY_CODE_LEN);
    // APP_PRINT_INFO2("%s,SetKeyCode = %s", __func__, uteModuleEmotionPressureData.algoKeyCode);
}
/**
 *@brief        ��ȡ����ѹ��ֵ������
 *@details      APP�·�����Ȩ������
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureGetKeyCode(uint8_t *keyCode)
{
    memcpy(keyCode, uteModuleEmotionPressureData.algoKeyCode, EMOTION_PRESSURE_KEY_CODE_LEN);
}
/**
 *@brief        ��ȡ����ѹ��ֵ������
 *@details      APP�·�����Ȩ������
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureGetReqCode(uint8_t *reqCode)
{
    memcpy(reqCode, uteModuleEmotionPressureData.algoReqCode, EMOTION_PRESSURE_REQ_CODE_LEN);
}
#endif

/**
 *@brief        ��������ѹ��ֵ
 *@details      APP��������ʱʹ�ã�APP�·�����ѹ��ֵ
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureSetEmotionPressureValue(uint8_t emotionValue, uint8_t pressureValue, uint8_t fatigueValue)
{
    uteModuleEmotionPressureData.emotionValue = emotionValue;
    uteModuleEmotionPressureData.pressureValue = pressureValue;
    uteModuleEmotionPressureData.fatigueValue = fatigueValue;
}

/**
 *@brief        ��ȡ����ֵ
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
uint8_t uteModuleEmotionPressureGetEmotionValue(void)
{
    return uteModuleEmotionPressureData.emotionValue;
}

/**
 *@brief        ��ȡѹ��ֵ
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
uint8_t uteModuleEmotionPressureGetPressureValue(void)
{
    return uteModuleEmotionPressureData.pressureValue;
}

/**
 *@brief        ��ȡƣ�Ͷ�ֵ
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
uint8_t uteModuleEmotionPressureGetFatigueValue(void)
{
    return uteModuleEmotionPressureData.fatigueValue;
}

/**
 *@brief        ��ȡ����ѹ������״̬
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
bool uteModuleEmotionPressureIsTesting(void)
{
    return uteModuleEmotionPressureData.isSingleTesting;
}

/**
 *@brief        ��������ѹ�����ò���
 *@details
 *@param[in] bool isAutoTesting  �Ƿ���Զ�����
 *@param[in] uint16_t intervalMin  ���Լ������λΪ����
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureSaveAutoIntervalParam(bool isAutoTesting, uint16_t intervalMin)
{
    uteModuleEmotionPressureData.isAutoTesting = isAutoTesting;
    uteModuleEmotionPressureData.intervalMin = intervalMin;
    /*! ���浽�ļ�xjc, 2021-08-30*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleEmotionPressureData.isAutoTesting;
    writebuff[1] = uteModuleEmotionPressureData.isAutoTimeBucketTesting;
    writebuff[2] = (uteModuleEmotionPressureData.intervalMin >> 8) & 0xff;
    writebuff[3] = (uteModuleEmotionPressureData.intervalMin) & 0xff;
    writebuff[4] = (uteModuleEmotionPressureData.startTimeHourMin >> 8) & 0xff;
    writebuff[5] = (uteModuleEmotionPressureData.startTimeHourMin) & 0xff;
    writebuff[6] = (uteModuleEmotionPressureData.endTimeHourMin >> 8) & 0xff;
    writebuff[7] = (uteModuleEmotionPressureData.endTimeHourMin) & 0xff;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_SET_INFO, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file, &writebuff[0], 8);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,isAutoTesting=%d,intervalMin=%d,isAutoTimeBucketTesting=%d", __func__, uteModuleEmotionPressureData.isAutoTesting, uteModuleEmotionPressureData.intervalMin, uteModuleEmotionPressureData.isAutoTimeBucketTesting);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__, uteModuleEmotionPressureData.startTimeHourMin, uteModuleEmotionPressureData.endTimeHourMin);
}

/**
 *@brief        ��������ѹ�����ò���
 *@details
 *@param[in] bool isAutoTimeBucketTesting  �Ƿ��ʱ����Զ�����
 *@param[in] uint16_t startTimeHourMin ��ʼʱ�䣬��8bitΪСʱ����8bitΪ����
 *@param[in] uint16_t endTimeHourMin ����ʱ�䣬��8bitΪСʱ����8bitΪ����
 *@author       xjc
 *@date         2022-02-15
 */
void uteModuleEmotionPressureSaveAutoTimeBucketParam(bool isAutoTimeBucketTesting, uint16_t startTimeHourMin, uint16_t endTimeHourMin)
{
    uteModuleEmotionPressureData.isAutoTimeBucketTesting = isAutoTimeBucketTesting;
    uteModuleEmotionPressureData.startTimeHourMin = startTimeHourMin;
    uteModuleEmotionPressureData.endTimeHourMin = endTimeHourMin;
    /*! ���浽�ļ�xjc, 2022-02-15*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleEmotionPressureData.isAutoTesting;
    writebuff[1] = uteModuleEmotionPressureData.isAutoTimeBucketTesting;
    writebuff[2] = (uteModuleEmotionPressureData.intervalMin >> 8) & 0xff;
    writebuff[3] = (uteModuleEmotionPressureData.intervalMin) & 0xff;
    writebuff[4] = (uteModuleEmotionPressureData.startTimeHourMin >> 8) & 0xff;
    writebuff[5] = (uteModuleEmotionPressureData.startTimeHourMin) & 0xff;
    writebuff[6] = (uteModuleEmotionPressureData.endTimeHourMin >> 8) & 0xff;
    writebuff[7] = (uteModuleEmotionPressureData.endTimeHourMin) & 0xff;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_SET_INFO, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file, &writebuff[0], 8);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,isAutoTesting=%d,intervalMin=%d,isAutoTimeBucketTesting=%d", __func__, uteModuleEmotionPressureData.isAutoTesting, uteModuleEmotionPressureData.intervalMin, uteModuleEmotionPressureData.isAutoTimeBucketTesting);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__, uteModuleEmotionPressureData.startTimeHourMin, uteModuleEmotionPressureData.endTimeHourMin);
}

#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // add by pcm 2023-07-28 ά���㷨������������
/**
 *@brief        ��������ѹ��������
 *@details
 *@author       xjc
 *@date         2022-02-15
 */
void uteModuleEmotionPressureSaveKeyCode(void)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    /*! ���浽�ļ�xjc, 2022-02-15*/
    void *file;
    uint8_t writebuff[EMOTION_PRESSURE_KEY_CODE_LEN];
    memcpy(writebuff, uteModuleEmotionPressureData.algoKeyCode, EMOTION_PRESSURE_KEY_CODE_LEN);
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_KEY_CODE, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,Save data", __func__);
        uteModuleFilesystemWriteData(file, &writebuff[0], EMOTION_PRESSURE_KEY_CODE_LEN);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
 *@brief        ��������ѹ��������
 *@details
 *@author       xjc
 *@date         2022-02-15
 */
void uteModuleEmotionPressureSaveReqCode(void)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    /*! ���浽�ļ�xjc, 2022-02-15*/
    void *file;
    uint8_t writebuff[EMOTION_PRESSURE_REQ_CODE_LEN];
    memcpy(writebuff, uteModuleEmotionPressureData.algoReqCode, EMOTION_PRESSURE_REQ_CODE_LEN);
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_REQ_CODE, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,Save data", __func__);
        uteModuleFilesystemWriteData(file, &writebuff[0], EMOTION_PRESSURE_REQ_CODE_LEN);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
 *@brief        ��������ѹ���㷨����״̬
 *@details
 *@author       xjc
 *@date         2022-02-15
 */
void uteModuleEmotionPressureSaveAlgoActiveStatus(void)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    /*! ���浽�ļ�xjc, 2022-02-15*/
    // void *file;
    // uint8_t writebuff[2];
    // writebuff[0] = uteModuleEmotionPressureData.isAlgoActive;
    // if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_EMOTION_PRESSURE_ALGO_ACTIVE, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    // {
    //     uteModuleFilesystemWriteData(file, &writebuff[0], 1);
    //     uteModuleFilesystemCloseFile(file);
    // }
}
#endif

/**
 *@brief      ��������ѹ���������ݣ�ÿ10����һ������
 *@details    ÿ�챣��һ���ļ����Զ�����
 *@author     xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureAutoSaveData(void)
{
    /*! û�����ӹ�APP���������� xjc, 2022-05-06  */
    if (!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = (6 * 3) * 24 + 4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    uint8_t path[40];
    memset(&readBuff[0], 0xff, buffSize);
    memset(&path[0], 0, 40);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0], "%04d%02d%02d", time.year, time.month, time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, dirInfo, NULL);
    if ((dirInfo->filesCnt >= UTE_MODULE_EMOTION_PRESSURE_SAVE_DATA_MAX_DAYS) && (memcmp(&path[0], &dirInfo->filesName[0][0], 8) != 0))
    {
        /*! ɾ�����һ�������xjc, 2022-02-15*/
        memset(&path[0], 0, 40);
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, &dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile(&path[0]);
    }
    memset(&path[0], 0, 40);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,dirInfo->filesCnt=%d", __func__, dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,save file=%s", __func__, &path[0]);
    /*! ���浱ǰ����xjc, 2022-02-15  */
    void *file;
    // read
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readBuff[0], buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    else
    {
        readBuff[0] = time.year >> 8 & 0xff;
        readBuff[1] = time.year & 0xff;
        readBuff[2] = time.month;
        readBuff[3] = time.day;
    }
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uint16_t cacheOffset = 0;
        cacheOffset = 4 + (((time.hour * 60 + time.min) / 10) * 3);
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,cacheOffset=%d", __func__, cacheOffset);
        readBuff[cacheOffset] = uteModuleEmotionPressureData.emotionValue;
        readBuff[cacheOffset + 1] = uteModuleEmotionPressureData.pressureValue;
        readBuff[cacheOffset + 2] = uteModuleEmotionPressureData.fatigueValue;
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &readBuff[0], buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(readBuff);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
 *@brief        ��ʼ����24Сʱ����ѹ������
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureSendHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[40];
    uint8_t response[44];
    uint8_t sendSize = 0;
    memset(response, 0, sizeof(response));
    response[0] = CMD_EMOTION_PRESSURE_TEST;
    response[1] = 0xfa;
    memset(&path[0], 0, 40);
    uint8_t *tempDataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&tempDataBuff[0], 0, sendParam->dataBuffSize);
    memset(&dataBuff[0], 0, sendParam->dataBuffSize);
    if (sendParam->dirInfo.filesCnt > 0)
    {
    SEND_NEXT_DATA:
        sendSize = 2 + 6 + (12 * 3); // cmd = head+date+data(������ѹ����ƣ�Ͷ�)
        bool isHasData = false;
        void *file;
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,read file=%s", __func__, &path[0]);
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,currSendFileIndex=%d,currSendMinIndex=%d", __func__, sendParam->currSendFileIndex, sendParam->currSendMinIndex);
        if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
            memcpy(&dataBuff[0], tempDataBuff, 4);               /*!��ȡ������ , xjc 2022-03-03*/
            memcpy(&dataBuff[4], &tempDataBuff[4 + 3], 143 * 3); /*!��ȡ����00��10~23��50������, xjc 2022-03-03*/
            uteModuleFilesystemCloseFile(file);
        }
        if ((sendParam->currSendFileIndex + 1) < sendParam->dirInfo.filesCnt)
        {
            memset(&tempDataBuff[0], 0, sendParam->dataBuffSize);
            sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex + 1][0]);
            if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
            {
                uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
                memcpy(&dataBuff[sendParam->dataBuffSize - 3], &tempDataBuff[4], 3); /*!��ȡ��һ��00��00������, xjc 2022-03-03*/
                uteModuleFilesystemCloseFile(file);
            }
        }
        uint16_t invalidMin = 0;
        for (uint16_t i = sendParam->currSendMinIndex; i < (144 * 3);)
        {
            if ((memcmp(&dataBuff[4 + i], "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", (12 * 3)) == 0) ||
                (memcmp(&dataBuff[4 + i], "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", (12 * 3)) == 0))
            {
                invalidMin = invalidMin + (12 * 3);
                i = i + (12 * 3);
            }
            else
            {
                uint8_t sendHour = i / (6 * 3) + 2;
                if (sendHour == 24)
                {
                    ute_module_systemtime_time_t timeAdd;
                    timeAdd.year = dataBuff[0] << 8 | dataBuff[1];
                    timeAdd.month = dataBuff[2];
                    timeAdd.day = dataBuff[3];
                    uteModuleSystemtimeInputDateCalDay(&timeAdd, 1);
                    dataBuff[0] = (timeAdd.year >> 8) & 0xff;
                    dataBuff[1] = (timeAdd.year) & 0xff;
                    dataBuff[2] = timeAdd.month;
                    dataBuff[3] = timeAdd.day;
                    sendHour = 0;
                }
                memcpy(&response[2], &dataBuff[0], 4); // year month day
                response[6] = sendHour;
                response[7] = 0;
                memcpy(&response[8], &dataBuff[4 + i], (12 * 3));
                invalidMin = invalidMin + (12 * 3);
                i = i + (12 * 3);
                isHasData = true;
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,invalidMin=%d", __func__, invalidMin);
        sendParam->currSendMinIndex += invalidMin;
        if (sendParam->currSendMinIndex >= (144 * 3))
        {
            sendParam->currSendFileIndex++;
            if (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendMinIndex = 0;
            }
            else
            {
                sendParam->currSendMinIndex = (144 * 3);
            }
        }
        if ((!isHasData) && (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt))
        {
            goto SEND_NEXT_DATA;
        }
    }
    else
    {
        sendParam->currSendMinIndex = (144 * 3);
    }
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__, sendParam->currSendFileIndex, sendParam->currSendHourIndex, sendParam->dirInfo.filesCnt);
    if ((sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt) && (sendParam->currSendMinIndex == (144 * 3)))
    {
        sendSize = 4;
        response[1] = 0xfa;
        response[2] = 0xfd;
        response[3] = sendParam->crc;
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,stop", __func__);
        uteApplicationCommonSyncDataTimerStop();
    }
    if (sendSize == 44)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[2], (44 - 2));
    }
    uteModulePlatformMemoryFree(tempDataBuff);
    uteModulePlatformMemoryFree(dataBuff);
    uteModuleProfileBle50SendToPhone(&response[0], sendSize);
}
/**
 *@brief         ׼����ʼ��������ѹ������
 *@details
 *@param[in]     ��Ҫͬ��������ʱ���
 *@author        xjc
 *@date          2022-02-15
 */
void uteModuleEmotionPressureStartSendAutoTestHistoryData(ute_module_systemtime_time_t time)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->time = time;
    if (param->time.year == 0)
    {
        param->isAllData = true;
    }
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    param->currSendMinIndex = 0;
    param->dataBuffSize = (3 * 6) * 24 + 4; // ������ѹ����ƣ�Ͷ� 3bytes
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleEmotionPressureSendHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,", __func__);
}
/**
 *@brief        ɾ������ѹ����ʷ����
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureDelHistoryData(void)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, dirInfo, NULL);
    for (uint8_t i = 0; i < dirInfo->filesCnt; i++)
    {
        uint8_t path[40];
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, &dirInfo->filesName[i][0]);
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    uteModulePlatformMemoryFree(dirInfo);
}

/**
 *@brief        �Ƿ����ڲ�������ѹ��
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
bool uteModuleEmotionPressureIsEmotionPressureAutoTesting(void)
{
    return uteModuleEmotionPressureData.isEmotionPressureAutoTestFlag;
}

#if !UTE_MODULE_VK_EMOTION_PRESSURE_SUPPORT // ά���㷨������������
/**
 *@brief        ��������ѹ���㷨
 *@details
 *@param[in] bool isAppActiveAlgo �Ƿ�APP��������
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureActiveAlgo(bool isAppActiveAlgo)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,isAppActiveAlgo = %d", __func__, isAppActiveAlgo);
    bool hasReqCode = false;
    bool hasKeyCode = false;
    uint8_t reqCode[EMOTION_PRESSURE_REQ_CODE_LEN + 1];
    uint8_t keyCode[EMOTION_PRESSURE_KEY_CODE_LEN + 1];
    uteModuleEmotionPressureGetReqCode(reqCode);
    uteModuleEmotionPressureGetKeyCode(keyCode);
    for (uint8_t i = 0; i < EMOTION_PRESSURE_REQ_CODE_LEN; i++)
    {
        if (reqCode[i] != 0xFF)
        {
            hasReqCode = true;
            break;
        }
    }
    for (uint8_t i = 0; i < EMOTION_PRESSURE_KEY_CODE_LEN; i++)
    {
        if (keyCode[i] != 0xFF)
        {
            hasKeyCode = true;
            break;
        }
    }
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,hasKeyCode = %d,hasReqCode=%d", __func__, hasKeyCode, hasReqCode);
    if (hasReqCode && hasKeyCode)
    {
        /*! ��ʼ�����㷨,xjc 2022-02-15*/
        bool isActive = activate_algorithm(reqCode, EMOTION_PRESSURE_REQ_CODE_LEN, keyCode, EMOTION_PRESSURE_KEY_CODE_LEN);
        uteModuleEmotionPressureSetAlgoActiveStatus(isActive);
    }
    /*! ���漤����,xjc 2022-02-15*/
    if (isAppActiveAlgo && uteModuleEmotionPressureGetAlgoActiveStatus())
    {
        uteModuleEmotionPressureSaveKeyCode();
    }
}
/**
 *@brief        ��������ѹ���㷨����״̬
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureSetAlgoActiveStatus(bool isAlgoActive)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,isAlgoActive = %d", __func__, isAlgoActive);
    uteModuleEmotionPressureData.isAlgoActive = isAlgoActive;
    // uteModuleEmotionPressureSaveAlgoActiveStatus();
}
/**
 *@brief        ��ѯ����ѹ���㷨�Ƿ��Ѽ���
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
bool uteModuleEmotionPressureGetAlgoActiveStatus(void)
{
    return uteModuleEmotionPressureData.isAlgoActive;
}
/**
 *@brief        ��ȡ�豸mac��ַ
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureGetMacAddress(uint8_t *mac)
{
    uint8_t macAddr[6];
    uteModulePlatformGetBleMacAddress(macAddr);
    for (int i = 0; i < 6; i++)
    {
        mac[2 * i] = macAddr[5 - i] >> 4 & 0x0f;
        mac[2 * i + 1] = macAddr[5 - i] & 0x0f;
    }
    // APP_PRINT_INFO2("%s,mac = %s", __func__, TRACE_BINARY(12, &mac[0]));
}
/**
 *@brief       ������Ȩ��
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureSendDeviceInfomationToService(void)
{
    uint8_t response[68];
    uint32_t algoReqCodeLen = 0;
    memset(response, 0xff, sizeof(response));

    uint8_t fact[EMOTION_PRESSURE_FACT_LEN];
    uint8_t factLen = EMOTION_PRESSURE_FACT_LEN;
    memcpy(fact, EMOTION_PRESSURE_DEFAULT_FACT, sizeof(fact));

    uint8_t macAddr[12]; // ����һ��12��������ܺ���ַ���Ϊ64
    uint8_t macAddrLen = sizeof(macAddr);
    memset(macAddr, 0, macAddrLen);

    // APP_PRINT_INFO2("%s,.algoReqCode = %s ", __func__, TRACE_BINARY(EMOTION_PRESSURE_REQ_CODE_LEN, &uteModuleEmotionPressureData.algoReqCode[0]));
    // APP_PRINT_INFO2("%s,.algoKeyCode = %s ", __func__, TRACE_BINARY(EMOTION_PRESSURE_KEY_CODE_LEN, &uteModuleEmotionPressureData.algoKeyCode[0]));

    response[0] = CMD_EMOTION_PRESSURE_TEST;
    response[1] = 0x0E;
    response[2] = EMOTION_PRESSURE_REQ_CODE_LEN;
    /*! ֮ǰû�������ָ��������û����һ�������룬����֮�󱣴浽����,xjc 2022-02-15*/
    bool hasReqCode = false;
    for (uint8_t i = 0; i < EMOTION_PRESSURE_REQ_CODE_LEN; i++)
    {
        if (uteModuleEmotionPressureData.algoReqCode[i] != 0xFF)
        {
            hasReqCode = true;
            break;
        }
    }
    if (!hasReqCode)
    {
        uteModuleEmotionPressureGetMacAddress(&macAddr[0]);
        /*! �㷨��ȡ������,xjc 2022-02-15*/
        request_validation(fact, factLen, macAddr, macAddrLen, uteModuleEmotionPressureData.algoReqCode, &algoReqCodeLen);
#if UTE_LOG_EMOTION_PRESSURE_LVL
        // APP_PRINT_INFO3("%s,request algoReqCodelen = %d,data=%s", __func__, algoReqCodeLen, TRACE_BINARY(64, uteModuleEmotionPressureData.algoReqCode));
#endif
        /*! ����������,xjc 2022-02-15*/
        uteModuleEmotionPressureSaveReqCode();
    }
    /*! ����������,xjc 2022-02-15*/
    memcpy(&response[3], uteModuleEmotionPressureData.algoReqCode, EMOTION_PRESSURE_REQ_CODE_LEN);
    uteModuleProfileBle50SendToPhone(&response[0], EMOTION_PRESSURE_REQ_CODE_LEN + 3);
}
/**
@para key = false С�˴洢  true ��˴洢
            index �ӵڼ����ֽڿ�ʼ
*/
/**
 *@brief        float2u8Arry
 *@details
 *@param[in] bool bigEndian  false С�˴洢  true ��˴洢
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureFloat2U8Arry(uint8_t *u8Arry, uint32_t index, float *floatData, bool bigEndian)
{
    uint8_t farray[4];
    *(float *)farray = *floatData;
    if (bigEndian)
    {
        u8Arry[index + 3] = farray[0];
        u8Arry[index + 2] = farray[1];
        u8Arry[index + 1] = farray[2];
        u8Arry[index + 0] = farray[3];
    }
    else
    {
        u8Arry[index + 0] = farray[0];
        u8Arry[index + 1] = farray[1];
        u8Arry[index + 2] = farray[2];
        u8Arry[index + 3] = farray[3];
    }
}
/**
 *@brief        ��ʼ�����м�ֵ����
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureSendMidData(void)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    if (!uteModuleEmotionPressureData.isSingleTesting)
    {
        uteApplicationCommonSyncDataTimerStop();
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,stop 1", __func__);
        return;
    }
    static uint8_t negNumCnt = 0;         // �����м�ֵ���и���������
    static int emotionMidValueBagCnt = 0; // ���Ͱ�������
    uint8_t sendMidValue[4 + 32] = {CMD_EMOTION_PRESSURE_TEST, 0x11, 0x00, 0x00};
    uint8_t index = 4;

    // ���ͽ���֡
    if (uteModuleEmotionPressureData.stopSendMidValue)
    {
        negNumCnt = 0;
        emotionMidValueBagCnt = 0;
        uteModuleEmotionPressureData.ppgData.index = 0;
        sendMidValue[0] = CMD_EMOTION_PRESSURE_TEST;
        sendMidValue[1] = 0x11;
        sendMidValue[2] = 0xFD;
        uteModuleProfileBle50SendToPhone(&sendMidValue[0], 3);
        uteModuleEmotionPressureData.ppgData.algoCaculatePerSampleTimeMidValueCnt = 0;
        memset(uteModuleEmotionPressureData.ppgData.emotionOutPutFMvaule, 0, sizeof(uteModuleEmotionPressureData.ppgData.emotionOutPutFMvaule)); // ��������
        // ֹͣ��ʱ��
        uteApplicationCommonSyncDataTimerStop();
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,stop 3", __func__);
        return;
    }
    // ת��
    for (uint8_t i = 0; i < 8; i++)
    {
        uteModuleEmotionPressureFloat2U8Arry(sendMidValue, index, &uteModuleEmotionPressureData.ppgData.emotionOutPutFMvaule[emotionMidValueBagCnt][i], false);
        index += 4;
    }
    for (uint8_t i = 0; i < 8; i++)
    {
        if (uteModuleEmotionPressureData.ppgData.emotionOutPutFMvaule[emotionMidValueBagCnt][i] < 0)
        {
            negNumCnt++;
            UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,negNumCnt = %d", __func__, negNumCnt);
            break;
        }
    }
    // �����м�ֵ
    emotionMidValueBagCnt++;
    uteModuleEmotionPressureData.ppgData.appTestStartToEndEPMidValueTotalCnt++;
    sendMidValue[3] = uteModuleEmotionPressureData.ppgData.appTestStartToEndEPMidValueTotalCnt & 0xff; // ���Ͱ�������
    uteModuleProfileBle50SendToPhone(&sendMidValue[0], 36);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "emotionMidValueBagCnt = %d,algoCaculatePerSampleTimeMidValueCnt=%d", emotionMidValueBagCnt, uteModuleEmotionPressureData.ppgData.appTestStartToEndEPMidValueTotalCnt);
    if (emotionMidValueBagCnt >= uteModuleEmotionPressureData.ppgData.algoCaculatePerSampleTimeMidValueCnt)
    {
        emotionMidValueBagCnt = 0;
        uteModuleEmotionPressureData.ppgData.algoCaculatePerSampleTimeMidValueCnt = 0;
        memset(uteModuleEmotionPressureData.ppgData.emotionOutPutFMvaule, 0, sizeof(uteModuleEmotionPressureData.ppgData.emotionOutPutFMvaule)); // ��������
        // ֹͣ��ʱ��
        uteApplicationCommonSyncDataTimerStop();
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,stop 2", __func__);
    }
}
/**
 *@brief         ׼����ʼ�����м�ֵ����
 *@details
 *@param[in]     ��Ҫͬ��������ʱ���
 *@author        xjc
 *@date          2022-02-15
 */
void uteModuleEmotionPressureStartSendMidData(void)
{
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleEmotionPressureSendMidData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,", __func__);
}
/**
 *@brief      uteArrayEnemLeftMove
 *@details    ������������������������� ��:���� :[0,1,2,3] ��� :[1,2,3,0];;;���� :[1,2,3,0] ��� :[2,3,0,1];...
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureArrayEnemLeftMove(uint8_t *arr, uint8_t len, bool isInit)
{
    if (len <= 1)
    {
        return;
    }
    if (isInit)
    {
        arr[0] = len - 1;
        for (uint8_t i = 1; i < len; i++)
        {
            arr[i] = i - 1;
        }
        // APP_PRINT_INFO1("arr[] = %s is init ok!", TRACE_BINARY(len, &arr[0]));
        return;
    }
    uint8_t temp = arr[0];
    for (uint8_t i = 0; i < len - 1; i++)
    {
        arr[i] = arr[i + 1];
    }
    arr[len - 1] = temp;
    // APP_PRINT_INFO1("arr[] = %s", TRACE_BINARY(len, &arr[0]));
}
/**
 *@brief      ppg����
 *@details
 *@author       xjc
 *@date       2022-02-15
 */
void uteModuleEmotionPressureAlgoInputData(vcxxCommonSampleData_t *sampleData, uint8_t size)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,.ppgDataSize = %d", __func__, size);
    uteModuleEmotionPressureData.ppgData.ppgMinValue = 0xfffff0;
    /*! �ռ�1�������,xjc 2022-02-16*/
    if ((uteModuleEmotionPressureData.ppgData.oneSencondDataCnt < VCXX_SAMPLERATE + 20) && size >= 20)
    {
        for (uint8_t i = 0; i < size; i++)
        {
            uteModuleEmotionPressureData.ppgData.oneSecondData[uteModuleEmotionPressureData.ppgData.oneSencondDataCnt + i] = (sampleData->ppgValue[i]);
#if DUG_EMOTION_PRESSURE_SUPPORT
            uteModuleEmotionPressureData.vcxxSendPPGBuff[2 + (i * 2)] = sampleData->ppgValue[i] >> 8 & 0xFF;
            uteModuleEmotionPressureData.vcxxSendPPGBuff[2 + (i * 2 + 1)] = sampleData->ppgValue[i] & 0xFF;
#endif
        }
#if DUG_EMOTION_PRESSURE_SUPPORT
        uteModuleEmotionPressureData.vcxxSendPPGBuff[0] = 0x44;
        uteModuleEmotionPressureData.vcxxSendPPGBuff[1] = 0xFE;
        uteModuleProfileBle50SendToPhone(&uteModuleEmotionPressureData.vcxxSendPPGBuff[0], 42);
#endif
        uteModuleEmotionPressureData.ppgData.oneSencondDataCnt = uteModuleEmotionPressureData.ppgData.oneSencondDataCnt + 20;
    }
    //////////////////////////////////////////////////////////////////
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "oneSencondDataCnt = %d", uteModuleEmotionPressureData.ppgData.oneSencondDataCnt);
    if (uteModuleEmotionPressureData.ppgData.oneSencondDataCnt == VCXX_SAMPLERATE) // ����һ���PPG����
    {
        /***����Ԥ����**/
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,.bufferIsFull = %d", __func__, uteModuleEmotionPressureData.ppgData.bufferIsFull);
        if (uteModuleEmotionPressureData.ppgData.bufferIsFull) // ����������ִ���㷨�߼�
        {
            // �������,����������
            for (uint8_t i = 0; i < VCXX_SAMPLERATE; i++)
            {
                uteModuleEmotionPressureData.ppgData.buffer[uteModuleEmotionPressureData.ppgData.index][i] = uteModuleEmotionPressureData.ppgData.oneSecondData[i];
            }
            uteModuleEmotionPressureData.ppgData.index++;
            UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,ppgData.index = %d", __func__, uteModuleEmotionPressureData.ppgData.index);
            if (uteModuleEmotionPressureData.ppgData.index == APP_TEST_SAMPLETIME_BUFFER)
            {
                uteModuleEmotionPressureData.ppgData.index = 0;
            }
            // ����Ԥ����
            uteModuleEmotionPressureArrayEnemLeftMove(uteModuleEmotionPressureData.ppgData.ppgDataBufferIndex, APP_TEST_SAMPLETIME_BUFFER, false);
            for (uint8_t i = 0; i < APP_TEST_SAMPLETIME_BUFFER; i++)
            {
                uint16_t ppgIndex = uteModuleEmotionPressureData.ppgData.ppgDataBufferIndex[i];
                UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,ppgIndex = %d ", __func__, ppgIndex);
                for (uint8_t j = 0; j < VCXX_SAMPLERATE; j++)
                {
                    uteModuleEmotionPressureData.ppgData.emotionData[i * VCXX_SAMPLERATE + j] = uteModuleEmotionPressureData.ppgData.buffer[ppgIndex][j];
                }
            }
            for (uint16_t index = 0; index < VCXX_SAMPLERATE * APP_TEST_SAMPLETIME_BUFFER; index++)
            {
                if (uteModuleEmotionPressureData.ppgData.ppgMinValue > uteModuleEmotionPressureData.ppgData.emotionData[index])
                {
                    uteModuleEmotionPressureData.ppgData.ppgMinValue = uteModuleEmotionPressureData.ppgData.emotionData[index];
                }
            }
            for (uint16_t index = 0; index < VCXX_SAMPLERATE * APP_TEST_SAMPLETIME_BUFFER; index++)
            {
                uteModuleEmotionPressureData.ppgData.emotionData[index] = (uteModuleEmotionPressureData.ppgData.emotionData[index] - uteModuleEmotionPressureData.ppgData.ppgMinValue) * 100;
            }
            UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,%d seconds ppg data pretreatment completed!", __func__, APP_TEST_SAMPLETIME_BUFFER);
            /***����Ԥ����**/
            // �����㷨������Ĭ�ϼ���,�����㷨��
            //  uint8_t req_code[] = UTE_EMOTION_PRESSURE_DEFAULT_REQUSTCODE;
            //  uint8_t key_code[] = UTE_EMOTION_PRESSURE_DEFAULT_KEYCODE;
            //  uteModuleEmotionPressureData.isAlgoActive = activate_algorithm(req_code, EMOTION_PRESSURE_REQ_CODE_LEN, key_code, EMOTION_PRESSURE_KEY_CODE_LEN); //isAlgoActive=1��ʾ�㷨����  ���յ�APP�˷��صļ�����֮����ô˺�����
            int fmlen = 0;
            UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,isAlgoActive = %d", __func__, uteModuleEmotionPressureData.isAlgoActive);
            if (uteModuleEmotionPressureData.isAlgoActive) // ����ɹ�
            {
                uteModuleEmotionPressureData.ppgData.algoExeCnt++;
#if UTE_DRV_BLOODPRESSURE_DN02_SUPPORT
                int status = ppg_midvalue(uteModuleEmotionPressureData.ppgData.emotionData, VCXX_SAMPLERATE * APP_TEST_SAMPLETIME_BUFFER, VCXX_SAMPLERATE, uteModuleEmotionPressureData.ppgData.emotionOutPutFMvaule, &fmlen);
                UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,status=%d", __func__, status);
#else
                ppg_correction(uteModuleEmotionPressureData.ppgData.emotionData, VCXX_SAMPLERATE * APP_TEST_SAMPLETIME_BUFFER); // �ź�������ÿ�����źŴ���ʱ��Ҫ����
                fmlen = ppg_midvalue(uteModuleEmotionPressureData.ppgData.emotionData, VCXX_SAMPLERATE * APP_TEST_SAMPLETIME_BUFFER, VCXX_SAMPLERATE, uteModuleEmotionPressureData.ppgData.emotionOutPutFMvaule);
#endif
                uteModuleEmotionPressureData.ppgData.algoCaculatePerSampleTimeMidValueCnt = fmlen;
                UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,.algoCaculatePerSampleTimeMidValueCnt = %d", __func__, uteModuleEmotionPressureData.ppgData.algoCaculatePerSampleTimeMidValueCnt);
                UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,.isAppStartTesting = %d", __func__, uteModuleEmotionPressureData.isAppStartTesting);
                UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,.isEmotionPressureAutoTestFlag = %d", __func__, uteModuleEmotionPressureData.isEmotionPressureAutoTestFlag);
                if (uteModuleEmotionPressureData.isAppStartTesting)
                {
                    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "start send mid value to app , epAlgoExeCnt = %d", uteModuleEmotionPressureData.ppgData.algoExeCnt);
                    uteModuleEmotionPressureData.ppgData.algoExeCnt = 0;
                    // APP����;���������ӶϿ�,��������
                    ute_ble_connect_state_t connectStatus;
                    uteApplicationCommonGetBleConnectionState(&connectStatus);
                    if (!connectStatus.isConnected)
                    {
                        uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_TIMEOUT);
                    }
                    // �ɼ�����Ч���м�֮��ŷ���APP
                    if (fmlen > 0)
                    {
                        if (!uteModuleEmotionPressureData.stopSendMidValue)
                        {
                            uteModuleEmotionPressureStartSendMidData();
                            if (uteModuleEmotionPressureData.testingSecond >= (60 * APP_START_TEST_OVER_TIME))
                            {
                                uteModuleEmotionPressureData.stopSendMidValue = true;
                            }
                        }
                    }
                    else
                    {
                        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,wait server send data...", __func__);
                    }
                }
                else
                {
                    int emo = 0;
                    int pi = 0;
                    // ���ؼ���
#if UTE_LOG_EMOTION_PRESSURE_LVL
                    uint32_t diffTime = uteModulePlatformGetSystemTick();
#endif
                    get_emotion_pi(uteModuleEmotionPressureData.ppgData.emotionData, VCXX_SAMPLERATE * APP_TEST_SAMPLETIME_BUFFER, VCXX_SAMPLERATE, &emo, &pi);
//                  tempPiValue = get_pi(uteModuleEmotionPressureData.ppgData.emotionData, VCXX_SAMPLERATE * APP_TEST_SAMPLETIME_BUFFER, VCXX_SAMPLERATE);       // ���õ�EMOTIONTIME/SINGLETIME=20��ʱ��ѹ����������򷵻�0
#if UTE_LOG_EMOTION_PRESSURE_LVL
                    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%d,diffTime1=%d ms", __LINE__, (uteModulePlatformGetSystemTick() - diffTime) / 32);
#endif
                    //                  tempEmotionValue = get_emotion(uteModuleEmotionPressureData.ppgData.emotionData, VCXX_SAMPLERATE * APP_TEST_SAMPLETIME_BUFFER, VCXX_SAMPLERATE); // ���õ�EMOTIONTIME/SINGLETIME=20��ʱ��������������򷵻�-4
                    //                  #if UTE_LOG_GUI_LVL
                    //                  UTE_MODULE_LOG(UTE_LOG_GUI_LVL, "%d,diffTime2=%d ms", __LINE__,(uteModulePlatformGetSystemTick()-diffTime)/32);
                    //                  #endif
                    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,pi = %d, emo = %d", __func__, pi, emo);
                    if (pi > 0 && emo >= 0)
                    {
                        // pi �� emo��Ч���ɷ��������������ط�ʹ��
                        uteModuleEmotionPressureData.emotionValue = emo & 0xff;
                        uteModuleEmotionPressureData.pressureValue = pi & 0xff;
                        //                        uteModuleEmotionPressureData.fatigueValue = 0xFF;//ƣ�Ͷ�Ĭ�ϲ���ֵ�������㷨û����ʵ�ĳ�ֵ xjc 2022-04-13*/
                        uteModuleEmotionPressureData.fatigueValue = 10 + rand() % 10; // ƣ�Ͷȱ��ؼ�����ʱ�����ֵ
                        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,emotionValue = %d,pressureValue = %d,fatigueValue = %d", __func__, uteModuleEmotionPressureData.emotionValue, uteModuleEmotionPressureData.pressureValue, uteModuleEmotionPressureData.fatigueValue);
                        if (uteModuleEmotionPressureData.ppgData.algoExeCnt >= LOCAL_TEST_OUTPUT_VALUE_TIME) // �ӳ����ز��Գ�ֵʱ�䣬���Ӵ����������ز��Գ�ֵ�Ͽ�
                        {
                            uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_SUCCESS);
                        }
                    }
                    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,int counter of emotion algorithm execution = %d ", __func__, uteModuleEmotionPressureData.ppgData.algoExeCnt);
                }
            }
            else
            {
                uteModuleEmotionPressureStopSingleTesting(EP_STOP_REASION_TIMEOUT);
            }
        }
        else // ����δ��������һ���������뻺��
        {
            APP_PRINT_ERROR0("3 seconds ppg data isn't enough !");
            for (uint8_t i = 0; i < VCXX_SAMPLERATE; i++)
            {
                uteModuleEmotionPressureData.ppgData.buffer[uteModuleEmotionPressureData.ppgData.index][i] = uteModuleEmotionPressureData.ppgData.oneSecondData[i];
            }
            uteModuleEmotionPressureData.ppgData.index++;
            UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,ppgData.index = %d", __func__, uteModuleEmotionPressureData.ppgData.index);
            if (uteModuleEmotionPressureData.ppgData.index == APP_TEST_SAMPLETIME_BUFFER - 1)
            {
                uteModuleEmotionPressureData.ppgData.bufferIsFull = true;
                uteModuleEmotionPressureArrayEnemLeftMove(uteModuleEmotionPressureData.ppgData.ppgDataBufferIndex, APP_TEST_SAMPLETIME_BUFFER, true); // ��ʼ��
            }
        }
        uteModuleEmotionPressureData.ppgData.oneSencondDataCnt = 0;
    }
}
#endif

#if 1
/**
*@brief      ������ص���ѹ����ʷ���ݣ�������״ͼ��ʾ
*@details    ���������ݷ���true,û���򷵻�false
*@author     xjc
*@date       2022-02-18
*/
bool uteModuleEmotionPressureLoadTodayPressureHistoryData(uint8_t *pressureHistoryGraph, uint8_t pressureHistoryGraphCount, uint8_t *pressureHistoryData, uint8_t pressureHistoryDataLen)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
#if UTE_LOG_GUI_LVL // test
    for (uint8_t i = 0; i < 144; i++)
    {
        pressureHistoryData[i] = 50 + rand()%15;
    }
#endif
    /*! ѹ����ʷ����ÿ��10������ʾ xjc, 2022-05-27  */
#if UTE_EMOTION_PRESSURE_HISTOTY_DISPLAY_EACH_20_MIN_SUPPORT
    uint8_t tempPressureHistoryData[72];
    memset(tempPressureHistoryData,0,sizeof(tempPressureHistoryData));
    for (uint8_t i = 0; i < 72; i++)
    {
        tempPressureHistoryData[i] = pressureHistoryData[i*2];
    }
    memcpy(pressureHistoryData,tempPressureHistoryData,72);
#else
    uint8_t tempPressureHistoryData[144];
    uint8_t m = 144/pressureHistoryGraphCount;
    memset(tempPressureHistoryData, 0, sizeof(tempPressureHistoryData));
#if UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
    uteModuleEmotionPressureData.curDayPressureMaxValue = 0;
    uteModuleEmotionPressureData.curDayPressureMinValue = 255;
#endif
    for (uint8_t i = 0; i < pressureHistoryGraphCount; i++)
    {
        uint8_t pressureValue = 0;
        uint16_t pressureValidValue = 0; //������ֵ
        uint8_t averagePressureValueCnt = 0;
        for (int j = m; j > 0; j--)
        {
            pressureValue = pressureHistoryData[i * m + (j - 1)];
            if ( pressureValue > 0 && pressureValue < 0xff)
            {
                pressureValidValue += pressureValue;
                averagePressureValueCnt++;
            }
        }
        tempPressureHistoryData[i] = averagePressureValueCnt > 0 ? (pressureValidValue/averagePressureValueCnt) : 0;

#if UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
        if(tempPressureHistoryData[i]!=0&&tempPressureHistoryData[i]!=0xFF)
        {
//            uteModuleEmotionPressureData.curDayPressureMaxValue = 0;
//            uteModuleEmotionPressureData.curDayPressureMinValue = 255;
            if(tempPressureHistoryData[i]>uteModuleEmotionPressureData.curDayPressureMaxValue)
            {
                uteModuleEmotionPressureData.curDayPressureMaxValue = tempPressureHistoryData[i];
            }
            if(tempPressureHistoryData[i]<uteModuleEmotionPressureData.curDayPressureMinValue)
            {
                uteModuleEmotionPressureData.curDayPressureMinValue = tempPressureHistoryData[i];
            }
        }
#endif

    }
    memcpy(pressureHistoryGraph, tempPressureHistoryData, pressureHistoryGraphCount);
#endif
    return false;
}

/**
*@brief      ��ȡ����ѹ����ʷ���ݣ�������״ͼ��ʾ
*@details    ���������ݷ���true,û���򷵻�false
*@author     xjc
*@date       2022-02-18
*/
bool uteModuleEmotionPressureGetTodayPressureHistoryData(uint8_t *pressureHistoryGraph, uint8_t pressureHistoryGraphCount)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    void *file;
    bool isHasData = false;
    uint8_t path[40];
    uint8_t pressureHistoryData[144]; // һ��24h�����ݣ�ʮ����һ����һ��144��
    uint16_t readPressureHistoryDataSize = (144 * 3) + 4;
    uint8_t *readPressureHistoryData = uteModulePlatformMemoryAlloc(readPressureHistoryDataSize);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);

    memset(pressureHistoryData, 0, 144);
    memset(pressureHistoryGraph, 0, sizeof(uint8_t) * pressureHistoryGraphCount);

    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,read file=%s", __func__, &path[0]);

#if UTE_LOG_GUI_LVL // test
    uteModuleEmotionPressureLoadTodayPressureHistoryData(pressureHistoryGraph, pressureHistoryGraphCount, pressureHistoryData, 144);
    isHasData = true;
#endif
    if (!isHasData)
    {
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, readPressureHistoryData, readPressureHistoryDataSize);
            uteModuleFilesystemCloseFile(file);

#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT && !UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
            uint8_t maxValue = 0;
            uint8_t minValue = 255;
#endif

            for (uint16_t i = 0; i < 144; i++)
            {
                pressureHistoryData[i] = readPressureHistoryData[4 + (i * 3) + 1];

#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT && !UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
                if (pressureHistoryData[i] > 0 && pressureHistoryData[i] < 255)
                {
                    if (pressureHistoryData[i] > maxValue)
                    {
                        maxValue = pressureHistoryData[i];
                    }
                    if (pressureHistoryData[i] < minValue)
                    {
                        minValue = pressureHistoryData[i];
                    }
                }
#endif
            }
            uteModuleEmotionPressureLoadTodayPressureHistoryData(pressureHistoryGraph, pressureHistoryGraphCount, pressureHistoryData, 144);
            isHasData = true;

#if (UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT && !UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT)
            if (uteModuleEmotionPressureData.curDayPressureMaxValue == 0 && maxValue > 0)
            {
                uteModuleEmotionPressureData.curDayPressureMaxValue = maxValue;
            }
            if (uteModuleEmotionPressureData.curDayPressureMinValue == 0xff && minValue < 0xff)
            {
                uteModuleEmotionPressureData.curDayPressureMinValue = minValue;
            }
#endif
        }
    }
    uteModulePlatformMemoryFree(readPressureHistoryData);
    return isHasData;
}

/**
*@brief      ������ص���ѹ����ʷ���ݣ�������״ͼ��ʾ
*@details    ���������ݷ���true,û���򷵻�false
*@author     xjc
*@date       2022-02-18
*/
bool uteModuleEmotionPressureLoadTodayEmotionHistoryData(uint8_t *emotionHistoryGraph, uint8_t emotionHistoryGraphCount, uint8_t *emotionHistoryData, uint8_t emotionHistoryDataLen)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
#if UTE_LOG_GUI_LVL // test
    for (uint8_t i = 0; i < 144; i++)
    {
        emotionHistoryData[i] = rand() % 3;
    }
#endif
    uint8_t tempEmotionHistoryData[144];
    uint8_t m = 144 / emotionHistoryGraphCount;
    memset(tempEmotionHistoryData, 0xff, sizeof(tempEmotionHistoryData));
    for (uint8_t i = 0; i < emotionHistoryGraphCount; i++)
    {
        for (uint8_t j = 0; j < m; j++)
        {
            uint8_t lastEmotion = 0xff;
            for (int j = m; j > 0; j--)
            {
                if (emotionHistoryData[i * m + (j - 1)] != 0xff)
                {
                    lastEmotion = emotionHistoryData[i * m + (j - 1)];
                }
            }
            if (lastEmotion != 0xff) // ��Чֵ
            {
                tempEmotionHistoryData[i] = lastEmotion;
            }
        }
    }
    memcpy(emotionHistoryData, tempEmotionHistoryData, emotionHistoryGraphCount);
    for (uint8_t i = 0; i < emotionHistoryGraphCount; i++)
    {
        uint8_t emotionData = emotionHistoryData[i];
        if (emotionData > 2)
        {
            emotionData = 2;
        }
        emotionHistoryGraph[i] = emotionData;
    }
    return false;
}

/**
*@brief      ��ȡ����������ʷ���ݣ�������״ͼ��ʾ
*@details    ���������ݷ���true,û���򷵻�false
*@author     zn.zeng
*@date       2022-09-04
*/
bool uteModuleEmotionPressureGetTodayEmotionHistoryData(uint8_t *emotionHistoryGraph, uint8_t emotionHistoryGraphCount)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    void *file;
    bool isHasData = false;
    uint8_t path[40];
    uint8_t emotionHistoryData[144]; // һ��24h�����ݣ�ʮ����һ����һ��144��
    uint16_t readEmotionHistoryDataSize = (144 * 3) + 4;
    uint8_t *readEmotionHistoryData = uteModulePlatformMemoryAlloc(readEmotionHistoryDataSize);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);

    memset(emotionHistoryData, 0, 144);
    memset(emotionHistoryGraph, 0, sizeof(uint8_t) * emotionHistoryGraphCount);

    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,read file=%s", __func__, &path[0]);

#if UTE_LOG_GUI_LVL // test
    uteModuleEmotionPressureLoadTodayEmotionHistoryData(emotionHistoryGraph, emotionHistoryGraphCount, emotionHistoryData, 144);
    isHasData = true;
#endif
    if (!isHasData)
    {
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, readEmotionHistoryData, readEmotionHistoryDataSize);
            uteModuleFilesystemCloseFile(file);
            for (uint16_t i = 0; i < 144; i++)
            {
                if (readEmotionHistoryData[4 + (i * 3)] != 0xFF)
                {
                    emotionHistoryData[i] = readEmotionHistoryData[4 + (i * 3)];
                }
                else
                {
                    emotionHistoryData[i] = 0xff;
                }
            }
            uteModuleEmotionPressureLoadTodayEmotionHistoryData(emotionHistoryGraph, emotionHistoryGraphCount, emotionHistoryData, 144);
            isHasData = true;
        }
    }
    uteModulePlatformMemoryFree(readEmotionHistoryData);
    return isHasData;
}

/**
*@brief      ������ص���ƣ�Ͷ���ʷ���ݣ�������״ͼ��ʾ
*@details    ���������ݷ���true,û���򷵻�false
*@author     xjc
*@date       2022-02-18
*/
bool uteModuleEmotionPressureLoadTodayFatigueHistoryData(uint8_t *fatigueHistoryGraph, uint8_t fatigueHistoryGraphCount, uint8_t *fatigueHistoryData, uint8_t fatigueHistoryDataLen)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
#if UTE_LOG_GUI_LVL // test
    for (uint8_t i = 0; i < 144; i++)
    {
        fatigueHistoryData[i] = 20 + rand()%10;
    }
#endif
    uint8_t tempFatigueHistoryData[144];
    /*! ѹ����ʷ����ÿ��10������ʾ xjc, 2022-05-27  */
#if UTE_EMOTION_PRESSURE_HISTOTY_DISPLAY_EACH_20_MIN_SUPPORT
    memset(tempFatigueHistoryData,0,sizeof(tempFatigueHistoryData));
    for (uint8_t i = 0; i < 72; i++)
    {
        tempFatigueHistoryData[i] = fatigueHistoryData[i*2];
    }
    memcpy(fatigueHistoryData,tempFatigueHistoryData,72);
#else
    uint8_t m = 144/fatigueHistoryGraphCount;
    memset(tempFatigueHistoryData,0,sizeof(tempFatigueHistoryData));
    for (uint8_t i = 0; i < fatigueHistoryGraphCount; i++)
    {
        uint8_t fatigueValue = 0;
        uint16_t fatigueValidValue = 0; //������ֵ
        uint8_t averageFatigueValueCnt = 0;
        for (int j = m; j > 0; j--)
        {
            fatigueValue = fatigueHistoryData[i * m + (j - 1)];
            if (fatigueValue > 0 && fatigueValue < 0xff)
            {
                fatigueValidValue += fatigueValue;
                averageFatigueValueCnt++;
            }
        }
        tempFatigueHistoryData[i] = averageFatigueValueCnt > 0 ? (fatigueValidValue/averageFatigueValueCnt) : 0;

#if UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
        if(tempFatigueHistoryData[i] != 0 && tempFatigueHistoryData[i] != 0xFF)
        {
            if(tempFatigueHistoryData[i]>uteModuleEmotionPressureData.curDayFatigueMaxValue)
            {
                uteModuleEmotionPressureData.curDayFatigueMaxValue = tempFatigueHistoryData[i];
            }
            if(tempFatigueHistoryData[i]<uteModuleEmotionPressureData.curDayFatigueMinValue)
            {
                uteModuleEmotionPressureData.curDayFatigueMinValue = tempFatigueHistoryData[i];
            }
        }
#endif
    }
    memcpy(fatigueHistoryGraph,tempFatigueHistoryData,fatigueHistoryGraphCount);
#endif
    return false;
}

/**
*@brief      ��ȡ����ƣ�Ͷ���ʷ���ݣ�������״ͼ��ʾ
*@details    ���������ݷ���true,û���򷵻�false
*@author     xjc
*@date       2022-02-18
*/
bool uteModuleEmotionPressureGetTodayFatigueHistoryData(uint8_t *fatigueHistoryGraph, uint8_t fatigueHistoryGraphCount)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    void *file;
    bool isHasData = false;
    uint8_t path[40];
    uint8_t fatigueHistoryData[144]; //һ��24h�����ݣ�ʮ����һ����һ��144��
    uint16_t readFatigueHistoryDataSize = (144 * 3) + 4;
    uint8_t *readFatigueHistoryData = uteModulePlatformMemoryAlloc(readFatigueHistoryDataSize);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);

    memset(fatigueHistoryData, 0, 144);
    memset(fatigueHistoryGraph, 0, sizeof(uint8_t) * fatigueHistoryGraphCount);

    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,read file=%s", __func__, &path[0]);

#if UTE_LOG_GUI_LVL // test
    uteModuleEmotionPressureLoadTodayFatigueHistoryData(fatigueHistoryGraph, fatigueHistoryGraphCount, fatigueHistoryData, 144, x, y, drawWidth, intervalWidth, hightRange);
    isHasData = true;
#endif
    if (!isHasData)
    {
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, readFatigueHistoryData, readFatigueHistoryDataSize);
            uteModuleFilesystemCloseFile(file);
#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT&&!UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
            uint8_t maxValue = 0;
            uint8_t minValue = 255;
#endif
            for (uint16_t i = 0; i < 144; i++)
            {
                fatigueHistoryData[i] = readFatigueHistoryData[4 + (i * 3) + 2];
#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT&&!UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
                if (fatigueHistoryData[i]  > 0 && fatigueHistoryData[i] < 255)
                {
                    if (fatigueHistoryData[i] > maxValue)
                    {
                        maxValue = fatigueHistoryData[i];
                    }
                    if (fatigueHistoryData[i] < minValue)
                    {
                        minValue = fatigueHistoryData[i];
                    }
                }
#endif
            }
#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT&&!UTE_MODULE_PRESSURE_MAX_MIN_AVG_FOLLOW_HISTOGRAM_SUPPORT
            if(uteModuleEmotionPressureData.curDayFatigueMaxValue == 0 && maxValue > 0)
            {
                uteModuleEmotionPressureData.curDayFatigueMaxValue = maxValue;
            }
            if(uteModuleEmotionPressureData.curDayFatigueMinValue == 0xff && minValue < 0xff)
            {
                uteModuleEmotionPressureData.curDayFatigueMinValue = minValue;
            }
#endif
            uteModuleEmotionPressureLoadTodayFatigueHistoryData(fatigueHistoryGraph, fatigueHistoryGraphCount, fatigueHistoryData, 144);
            isHasData = true;
        }
    }
    uteModulePlatformMemoryFree(readFatigueHistoryData);
    return isHasData;
}
#endif

#if UTE_MODULE_PRESSURE_MAX_AND_MIN_VAULE_SUPPORT
/**
 *@brief        ����ѹ�������Сֵ
 *@details      APP��������ʱʹ�ã�APP�·�����ѹ��ֵ
 *@author     txj
 *@date       2022-07-21
 */
void uteModuleEmotionPressureSetEmotionPressureMaxAndMinValue(uint8_t lastPressureValue)
{
    if (uteModuleEmotionPressureData.lastPressureValue < uteModuleEmotionPressureData.curDayPressureMinValue)
    {
        uteModuleEmotionPressureData.curDayPressureMinValue = lastPressureValue;
    }
    if (uteModuleEmotionPressureData.lastPressureValue > uteModuleEmotionPressureData.curDayPressureMaxValue)
    {
        uteModuleEmotionPressureData.curDayPressureMaxValue = lastPressureValue;
    }
    if (avgPressureCnt != 0)
    {
        uteModuleEmotionPressureData.curDayPressureAvgValue = lastPressureValue;
    }
}
/**
 *@brief        ��ȡѹ�����ֵ
 *@details      APP��������ʱʹ�ã�APP�·�����ѹ��ֵ
 *@author     txj
 *@date       2022-07-21
 */
uint8_t uteModuleEmotionPressureGetEmotionPressureMaxValue(void)
{
    return (uteModuleEmotionPressureData.curDayPressureMaxValue == 0xFF) ? 0 : uteModuleEmotionPressureData.curDayPressureMaxValue;
}
/**
 *@brief        ��ȡѹ����Сֵ
 *@details      APP��������ʱʹ�ã�APP�·�����ѹ��ֵ
 *@author     txj
 *@date       2022-07-21
 */
uint8_t uteModuleEmotionPressureGetEmotionPressureMinValue(void)
{
    return (uteModuleEmotionPressureData.curDayPressureMinValue == 0xFF) ? 0 : uteModuleEmotionPressureData.curDayPressureMinValue;
}
/**
 *@brief        ��ȡѹ��ƽ��ֵ
 *@details      APP��������ʱʹ�ã�APP�·�����ѹ��ֵ
 *@author     txj
 *@date       2022-07-21
 */
uint8_t uteModuleEmotionPressureGetEmotionPressureAvgValue(void)
{
    return (uteModuleEmotionPressureData.curDayPressureAvgValue == 0xFF) ? 0 : uteModuleEmotionPressureData.curDayPressureAvgValue;
}
#endif

#if UTE_MODULE_PRESSURE_PERCENT_SUPPORT
/**
 *@brief        ����ѹ��ÿ���ȼ��İٷֱ�  High Medium Normal Relax
 *@details
 *@author     xjc
 *@date       2022-09-17
 */
bool uteModuleEmotionPressureCalculatePressurePercent(void)
{
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s", __func__);
    void *file;
    bool isHasData = false;
    uint8_t path[30];
    uint8_t pressureHistoryData[144]; // һ��24h�����ݣ�ʮ����һ����һ��144��
    uint16_t readPressureHistoryDataSize = (144 * 3) + 4;
    uint8_t *readPressureHistoryData = uteModulePlatformMemoryAlloc(readPressureHistoryDataSize);
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    uteModulePlatformTakeMutex(uteModuleEmotionPressureMute);
    memset(pressureHistoryData, 0, 144);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_EMOTION_PRESSURE_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,read file=%s", __func__, &path[0]);

#if UTE_LOG_GUI_LVL // test
    for (uint16_t i = 0; i < 144; i++)
    {
        pressureHistoryData[i] = 20 + rand() % 70;
    }
    isHasData = true;
#endif
    if (!isHasData)
    {
        if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, readPressureHistoryData, readPressureHistoryDataSize);
            uteModuleFilesystemCloseFile(file);
            for (uint16_t i = 0; i < 144; i++)
            {
                pressureHistoryData[i] = readPressureHistoryData[4 + (i * 3) + 1];
            }
            isHasData = true;
        }
    }
    memset(&uteModuleEmotionPressureData.result, 0, sizeof(emotion_pressure_result_data_t));
    for (uint16_t i = 0; i < 144; i++)
    {
        if (pressureHistoryData[i] != 0 && pressureHistoryData[i] != 0xFF)
        {
            if (pressureHistoryData[i] >= 80)
            {
                uteModuleEmotionPressureData.result.HighCnt++;
            }
            else if (pressureHistoryData[i] >= 60)
            {
                uteModuleEmotionPressureData.result.MediumCnt++;
            }
            else if (pressureHistoryData[i] >= 30)
            {
                uteModuleEmotionPressureData.result.NormalCnt++;
            }
            else
            {
                uteModuleEmotionPressureData.result.RelaxCnt++;
            }
            uteModuleEmotionPressureData.result.TotalValidCnt++;
        }
    }
    // no data
    if (uteModuleEmotionPressureData.result.TotalValidCnt == 0)
    {
        UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,TotalValidCnt = %d return", __func__, uteModuleEmotionPressureData.result.TotalValidCnt);
        uteModulePlatformMemoryFree(readPressureHistoryData);
        uteModulePlatformGiveMutex(uteModuleEmotionPressureMute);
        return isHasData;
    }
    uteModuleEmotionPressureData.result.HighPercent = uteModuleEmotionPressureData.result.HighCnt * 1000 / uteModuleEmotionPressureData.result.TotalValidCnt;
    uteModuleEmotionPressureData.result.MediumPercent = uteModuleEmotionPressureData.result.MediumCnt * 1000 / uteModuleEmotionPressureData.result.TotalValidCnt;
    uteModuleEmotionPressureData.result.NormalPercent = uteModuleEmotionPressureData.result.NormalCnt * 1000 / uteModuleEmotionPressureData.result.TotalValidCnt;
    uteModuleEmotionPressureData.result.RelaxPercent = uteModuleEmotionPressureData.result.RelaxCnt * 1000 / uteModuleEmotionPressureData.result.TotalValidCnt;
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,HighPercent = %d,.TotalValidCnt = %d,.HighCnt = %d", __func__, uteModuleEmotionPressureData.result.HighPercent, uteModuleEmotionPressureData.result.TotalValidCnt, uteModuleEmotionPressureData.result.HighCnt);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,MediumPercent = %d,.MediumCnt = %d", __func__, uteModuleEmotionPressureData.result.MediumPercent, uteModuleEmotionPressureData.result.MediumCnt);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,NormalPercent = %d,.NormalCnt = %d", __func__, uteModuleEmotionPressureData.result.NormalPercent, uteModuleEmotionPressureData.result.NormalCnt);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,RelaxPercent = %d,.RelaxCnt = %d", __func__, uteModuleEmotionPressureData.result.RelaxPercent, uteModuleEmotionPressureData.result.RelaxCnt);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,total = %d", __func__, uteModuleEmotionPressureData.result.RelaxPercent + uteModuleEmotionPressureData.result.NormalPercent + uteModuleEmotionPressureData.result.MediumPercent + uteModuleEmotionPressureData.result.HighPercent);
    while ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) < 100)
    {
        bool endWhile = false;
        if (uteModuleEmotionPressureData.result.RelaxPercent / 10 > 0)
        {
            uteModuleEmotionPressureData.result.RelaxPercent += 10;
        }
        if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) >= 100)
        {
            endWhile = true;
            goto END_WHILE_1;
        }
        if (uteModuleEmotionPressureData.result.NormalPercent / 10 > 0)
        {
            uteModuleEmotionPressureData.result.NormalPercent += 10;
        }
        if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) >= 100)
        {
            endWhile = true;
            goto END_WHILE_1;
        }
        if (uteModuleEmotionPressureData.result.MediumPercent / 10 > 0)
        {
            uteModuleEmotionPressureData.result.MediumPercent += 10;
        }
        if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) >= 100)
        {
            endWhile = true;
            goto END_WHILE_1;
        }
        if (uteModuleEmotionPressureData.result.HighPercent / 10 > 0)
        {
            uteModuleEmotionPressureData.result.HighPercent += 10;
        }
        if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) >= 100)
        {
            endWhile = true;
            goto END_WHILE_1;
        }
    END_WHILE_1:
        if (endWhile)
        {
            uteModuleEmotionPressureData.result.HighPercent = uteModuleEmotionPressureData.result.HighPercent / 10;
            uteModuleEmotionPressureData.result.MediumPercent = uteModuleEmotionPressureData.result.MediumPercent / 10;
            uteModuleEmotionPressureData.result.NormalPercent = uteModuleEmotionPressureData.result.NormalPercent / 10;
            uteModuleEmotionPressureData.result.RelaxPercent = uteModuleEmotionPressureData.result.RelaxPercent / 10;
            break;
        }
    }
    while ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) > 100)
    {
        bool endWhile = false;
        if (uteModuleEmotionPressureData.result.HighPercent / 10 > 0)
        {
            uteModuleEmotionPressureData.result.HighPercent -= 10;
        }
        if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) <= 100)
        {
            endWhile = true;
            goto END_WHILE_2;
        }
        if (uteModuleEmotionPressureData.result.MediumPercent / 10 > 0)
        {
            uteModuleEmotionPressureData.result.MediumPercent -= 10;
        }
        if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) <= 100)
        {
            endWhile = true;
            goto END_WHILE_2;
        }
        if (uteModuleEmotionPressureData.result.NormalPercent / 10 > 0)
        {
            uteModuleEmotionPressureData.result.NormalPercent -= 10;
        }
        if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) <= 100)
        {
            endWhile = true;
            goto END_WHILE_2;
        }
        if (uteModuleEmotionPressureData.result.RelaxPercent / 10 > 0)
        {
            uteModuleEmotionPressureData.result.RelaxPercent -= 10;
        }
        if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) <= 100)
        {
            endWhile = true;
            goto END_WHILE_2;
        }
    END_WHILE_2:
        if (endWhile)
        {
            uteModuleEmotionPressureData.result.HighPercent = uteModuleEmotionPressureData.result.HighPercent / 10;
            uteModuleEmotionPressureData.result.MediumPercent = uteModuleEmotionPressureData.result.MediumPercent / 10;
            uteModuleEmotionPressureData.result.NormalPercent = uteModuleEmotionPressureData.result.NormalPercent / 10;
            uteModuleEmotionPressureData.result.RelaxPercent = uteModuleEmotionPressureData.result.RelaxPercent / 10;
            break;
        }
    }
    if ((uteModuleEmotionPressureData.result.HighPercent / 10 + uteModuleEmotionPressureData.result.MediumPercent / 10 + uteModuleEmotionPressureData.result.NormalPercent / 10 + uteModuleEmotionPressureData.result.RelaxPercent / 10) == 100)
    {
        uteModuleEmotionPressureData.result.HighPercent = uteModuleEmotionPressureData.result.HighPercent / 10;
        uteModuleEmotionPressureData.result.MediumPercent = uteModuleEmotionPressureData.result.MediumPercent / 10;
        uteModuleEmotionPressureData.result.NormalPercent = uteModuleEmotionPressureData.result.NormalPercent / 10;
        uteModuleEmotionPressureData.result.RelaxPercent = uteModuleEmotionPressureData.result.RelaxPercent / 10;
    }
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,2HighPercent = %d,", __func__, uteModuleEmotionPressureData.result.HighPercent);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,2MediumPercent = %d", __func__, uteModuleEmotionPressureData.result.MediumPercent);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,2NormalPercent = %d", __func__, uteModuleEmotionPressureData.result.NormalPercent);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,2RelaxPercent = %d", __func__, uteModuleEmotionPressureData.result.RelaxPercent);
    UTE_MODULE_LOG(UTE_LOG_EMOTION_PRESSURE_LVL, "%s,2total = %d", __func__, uteModuleEmotionPressureData.result.RelaxPercent + uteModuleEmotionPressureData.result.NormalPercent + uteModuleEmotionPressureData.result.MediumPercent + uteModuleEmotionPressureData.result.HighPercent);

    uteModulePlatformMemoryFree(readPressureHistoryData);
    uteModulePlatformGiveMutex(uteModuleEmotionPressureMute);
    return isHasData;
}

/**
 *@brief        ��ȡѹ���ٷֱȲ���
 *@details
 *@author        xjc
 *@date        2022-09-18
 */
void uteModuleEmotionPressureCalculateGetPressurePercent(emotion_pressure_result_data_t *param)
{
    memcpy(param, &uteModuleEmotionPressureData.result, sizeof(emotion_pressure_result_data_t));
}
#endif

#endif
