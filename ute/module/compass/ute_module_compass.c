/**
*@file
*@brief        指南针模块
*@details
*@author        pcm
*@date        2024-11-25
*@version      v1.0
*/

#include "ute_module_compass.h"
#include "ute_module_log.h"
#include "ute_module_filesystem.h"
#if UTE_MODULE_MAGNETIC_SUPPORT
#include "ute_application_common.h"
#include "ute_drv_magnetic_common.h"
#include "ute_drv_gsensor_common.h"
#include "ute_module_message.h"
#include "MemsicAlgo.h"
#include "MemsicCompass.h"
#include "ute_drv_led.h"
// #include "ute_module_system_sleep.h"
// acc 左侧垂直是X的最大值   屏幕正对人垂直是Y轴最大   平放桌面是Z最大
#ifndef A_LAYOUT
#define A_LAYOUT    0
#define M_LAYOUT    3
#endif
#if UTE_MODULE_GPS_SUPPORT
#define UTE_DRV_COMPASS_SAMPLING_INTERVAL  100  //开启数据采样和处理定时器，时间间隔为100ms
#else
#define UTE_DRV_COMPASS_SAMPLING_INTERVAL  20  //开启数据采样和处理定时器，时间间隔为20ms
#endif

static bool isPowerUpInitAlgo = false;
void *uteModuleCompassCheckTimerPointer = NULL;

ute_module_compass_data_t uteModuleCompassData;

static float acc_raw_data[3] = {0.0f}; // accelerometer field vector, unit is g

/**
*@brief        延时初始化回调函数
*@details
*@author        pcm
*@date        2024-11-27
*/
void uteModuleCompassCheckTimerCallback(void *pxTimer)
{
    uteModulePlatformSendMsgToUteApplicationTask(MSG_TYPE_COMPASS_TESTING,0);
}
/**
*@brief        延时初始化定时器创建函数
*@details
*@author        pcm
*@date        2024-11-27
*/
void uteModuleCompassCheckTimerCreate(void)
{
    if(uteModuleCompassCheckTimerPointer == NULL)
    {
        uteModulePlatformCreateTimer(&uteModuleCompassCheckTimerPointer, "CompassCheckTimer",1, 1000, true, uteModuleCompassCheckTimerCallback);
    }
}
/**
*@brief   启动初始化定时器创建函数
*@details
*@author        pcm
*@date        2024-11-27
*/
void uteModuleCompassCheckTimerStart(uint16_t ms)
{
    UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL, "%s,%d ms", __func__,ms);
    uteModuleCompassCheckTimerCreate();
    uteModulePlatformRestartTimer(&uteModuleCompassCheckTimerPointer, ms);
}
/**
*@brief   停止初始化定时器创建函数
*@details
*@author        pcm
*@date        2024-11-27
*/
void uteModuleCompassCheckTimerStop(void)
{
    UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL, "%s", __func__);
    uteModulePlatformStopTimer(&uteModuleCompassCheckTimerPointer);
    uteModulePlatformDeleteTimer(&uteModuleCompassCheckTimerPointer);
    uteModuleCompassCheckTimerPointer = NULL;
}
/**
*@brief  读取校准参数
*@details
*@return bool true为成功，false为没保存有数据
*@author        pcm
*@date        2024-11-25
*/
bool uteModuleCompassReadCalibration(float *data)
{
    void *file;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_COMPASS_CALIBRATION_FILE,&file,FS_O_RDONLY))
    {
        int32_t size = uteModuleFilesystemGetFileSize(file);
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,(uint8_t*)&data[0],size);
        uteModuleFilesystemCloseFile(file);
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief  写入校准参数
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassWriteCalibration(float *data)
{
    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_COMPASS_CALIBRATION_FILE, &file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, (uint8_t *)&data[0], sizeof(float)*4);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  初始化
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_COMPASS_DATA_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_COMPASS_DATA_DIR, NULL, NULL);
#endif
    memset(&uteModuleCompassData,0,sizeof(ute_module_compass_data_t));
}
/**
*@brief  算法初始化
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassAlgoInit(void)
{
    if(isPowerUpInitAlgo == true) return;
    /* magnetic sensor soft iron calibration matrix, default is unit matrix */
    float mag_smm[9] = {1.0231f, 0.0142f, -0.0052f,
                        0.0142f, 0.9868f, 0.0034f,
                        -0.0052f, 0.0034f, 0.9907f
                       };

    /* default hard iron offset and magnetic field strength*/
    float mag_hmm[4] = {0.0f, 0.0f, 0.0f, 0.5f};
    /* Read saved calibration parameters. These para is saved in somewhere that can not disappear when power-off
     * ReadPara function should be create by customer.If customer can not implement this function, should comment it out
     * If there is no parameter saved, ReadPara() should return -1, else return 1;
     * If there is no para saved, pass the default value to the algorithm library
     */
    // uteModuleCompassReadCalibration(mag_hmm);
    /* Initial the acc, mag, and calibrated parameters
     * if already saved the calibrated offset and radius last time, read it out and init the magOffset and magRadius
     */
    InitialAlgorithm(mag_smm, mag_hmm);
    isPowerUpInitAlgo = true;
}
/* Convert the sensor coordinate to right-front-up coordinate system;
 */
void uteModuleCompassAccCoordinateConvert(int layout, float *in, float *out)
{
    switch (layout)
    {
        case 0:
            out[0] =  in[0];
            out[1] =  in[1];
            out[2] =  in[2];
            break;
        case 1:
            out[0] = -in[1];
            out[1] =  in[0];
            out[2] =  in[2];
            break;
        case 2:
            out[0] = -in[0];
            out[1] = -in[1];
            out[2] =  in[2];
            break;
        case 3:
            out[0] =  in[1];
            out[1] = -in[0];
            out[2] =  in[2];
            break;
        case 4:
            out[0] =  in[1];
            out[1] =  in[0];
            out[2] = -in[2];
            break;
        case 5:
            out[0] = -in[0];
            out[1] =  in[1];
            out[2] = -in[2];
            break;
        case 6:
            out[0] = -in[1];
            out[1] = -in[0];
            out[2] = -in[2];
            break;
        case 7:
            out[0] =  in[0];
            out[1] = -in[1];
            out[2] = -in[2];
            break;
        case 8:
            out[0] =  in[1];
            out[1] = -in[0];
            out[2] = -in[2];
            break;
        default:
            out[0] =  in[0];
            out[1] =  in[1];
            out[2] =  in[2];
            break;
    }
}
/* Convert the sensor(MMC3630KJ) coordinate to right-front-up coordinate system;
 */
void uteModuleCompassMagCoordinateConvert(int layout, float *in, float *out)
{
    switch (layout)
    {
        case 0:
            out[0] = in[0];
            out[1] = in[1];
            out[2] = in[2];
            break;
        case 1:
            out[0] = -in[1];
            out[1] = in[0];
            out[2] = in[2];
            break;
        case 2:
            out[0] = -in[0];
            out[1] = -in[1];
            out[2] = in[2];
            break;
        case 3:
            out[0] = in[1];
            out[1] = -in[0];
            out[2] = in[2];
            break;
        case 4:
            out[0] = in[1];
            out[1] = in[0];
            out[2] = -in[2];
            break;
        case 5:
            out[0] = -in[0];
            out[1] = in[1];
            out[2] = -in[2];
            break;
        case 6:
            out[0] = -in[1];
            out[1] = -in[0];
            out[2] = -in[2];
            break;
        case 7:
            out[0] = in[0];
            out[1] = -in[1];
            out[2] = -in[2];
            break;
        case 8:
            out[0] = -in[1];
            out[1] = -in[0];
            out[2] = in[2];
            break;
        default:
            out[0] = in[0];
            out[1] = in[1];
            out[2] = -in[2];
            break;
    }
}
/**
*@brief  mag acc算法处理函数
*@details  计算出方向角度，地磁质量
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassAlgohandler(float *rawMag,float *rawAcc)
{
    /* This variable is used to store the accelerometer and magnetometer raw data.
     * please NOTE that the data is going to store here MUST has been transmitted
     * to match the Right-Handed coordinate sytem already.
     */
    float acc_real_data[3] = {0.0f};

    float mag_raw_data[3] = {0.0f}; // magnetic field vector, unit is gauss
    float mag_real_data[3] = {0.0f};
    /* Get the acc raw data, unit is g*/
    // acc_raw_data[0] = rawAcc[0]/9.80665f;
    // acc_raw_data[1] = rawAcc[1]/9.80665f;
    // acc_raw_data[2] = rawAcc[2]/9.80665f;
    if(uteDrvGsensorCommonGetAccRange() ==4)//+-4G
    {
        acc_raw_data[0] = rawAcc[0]/128.0f;
        acc_raw_data[1] = rawAcc[1]/128.0f;
        acc_raw_data[2] = rawAcc[2]/128.0f;
    }
    else // +-8G
    {
        acc_raw_data[0] = rawAcc[0]/64.0f;
        acc_raw_data[1] = rawAcc[1]/64.0f;
        acc_raw_data[2] = rawAcc[2]/64.0f;
    }

    // UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s,rawAcc:%f,%f,%f",__func__,rawAcc[0],rawAcc[1],rawAcc[2]);
    // UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s,acc_raw_data:%f,%f,%f",__func__,acc_raw_data[0],acc_raw_data[1],acc_raw_data[2]);
    /* convert the coordinate system */
    uteModuleCompassAccCoordinateConvert(A_LAYOUT, acc_raw_data, acc_real_data);

    /* Get the mag raw data, unit is gauss */
    mag_raw_data[0] = rawMag[0];
    mag_raw_data[1] = rawMag[1];
    mag_raw_data[2] = rawMag[2];
    /* Convert the coordinate system */
    uteModuleCompassMagCoordinateConvert(M_LAYOUT, mag_raw_data, mag_real_data);

    UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s,acc:%f,%f,%f",__func__,acc_real_data[0],acc_real_data[1],acc_real_data[2]);
    UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s,mag:%f,%f,%f",__func__,mag_real_data[0],mag_real_data[1],mag_real_data[2]);

#if UTE_MODULE_RUNING_LOG_SUPPORT
    static char log_buf[200] = {0}; // 日志缓冲区
    char tmp[50];
    // 计算当前 mag 数据并格式化为字符串
    snprintf(tmp, sizeof(tmp), "mag:%d,%d,%d\n",
             (int)(mag_raw_data[0] * 10000.f + 0.5),
             (int)(mag_raw_data[1] * 10000.f + 0.5),
             (int)(mag_raw_data[2] * 10000.f + 0.5));

    // 检查是否还有足够空间容纳新内容
    if (strlen(log_buf) + strlen(tmp) < sizeof(log_buf) - 1)
    {
        strcat(log_buf, tmp); // 空间足够，直接拼接
    }
    else
    {
        // 空间不足，先输出当前缓冲区内容
        if (log_buf[0] != '\0')
        {
            UTE_MODULE_LOG_BLE(UTE_LOG_COMPASS_LVL, "%s", log_buf);
        }
        // 重置缓冲区并写入新内容
        memset(log_buf, 0, sizeof(log_buf));
        strcpy(log_buf, tmp);
    }
#endif

    /* Below functions are algorithm interface.
     * input acc, mag data into algorithm
     * make sure that acc and mag XYZ data meet the right-hand(right-front-up) coordinate system
     */
    MainAlgorithmProcess(acc_real_data, mag_real_data, 1);
    /* This variable is used to save the calibrated mag sensor XYZ output. */
    float caliMag[3] = {0.0f};

    /* This variable is used to save the calibrated orientation data. */
    float caliOri[3] = {0.0f};

    /* Get calibrated mag data */
    GetCalMag(caliMag);
    /* Get orientation vector */
    GetCalOri(acc_real_data, caliMag, caliOri);
    /* Get the fAzimuth Pitch Roll for the eCompass */
    uteModuleCompassData.fAzimuth = caliOri[0];
    uteModuleCompassData.fPitch = caliOri[1];
    uteModuleCompassData.fRoll = caliOri[2];

    /* Get the accuracy of the algorithm */
    uteModuleCompassData.iAccuracy = GetMagAccuracy();
    /* Get the SET Flag from algorithm */
    if (GetMagSaturation())
    {
        uteDrvMagneticCommonSensorSet(); //Do SET action
    }
    /* Get corrected mag data */
    /* This variable is used to save calibrated mag sensor parameters */
    float caliMagPara[4] = {0.0f};
    int saveFlag = GetCalPara(caliMagPara);
    if (saveFlag == 1)
    {
        /* Save the calpara buffer into the system file */
        /* This function should be create by customer.If customer can not implement this function, should comment it out */
        uteModuleCompassWriteCalibration(caliMagPara);
    }
    UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s,fAzimuth=%f,fPitch=%f,fRoll=%f,iAccuracy=%d,saveFlag=%d",__func__,uteModuleCompassData.fAzimuth,uteModuleCompassData.fPitch,uteModuleCompassData.fRoll,uteModuleCompassData.iAccuracy,saveFlag);
}
/**
 * @brief 获取罗盘开关状态
 * @details 该函数用于获取罗盘的开关状态。
 * @author pcm
 * @date 2024-11-25
 * @return bool 返回罗盘的开关状态，true表示开启，false表示关闭。
 */
bool uteModulecompassOnoff(void)
{
    // 返回罗盘的开关状态
    return uteModuleCompassData.compassOnoff;
}
/**

/**
*@brief  开始测量
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassStart(void)
{
#if UTE_DRV_REUSE_HEART_POWER_SUPPORT
    if (!bsp_sensor_hr_work_status()
#if UTE_DRV_REUSE_HEART_POWER_SUPPORT && UTE_DRV_LED_SUPPORT
        && (!uteDrvLedIsOpen())
#endif
       )
#endif
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_LED_POWER_GPIO_PIN, true);
        uteModulePlatformDelayMs(10);
    }
    UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s compassOnoff = %d",__func__,uteModuleCompassData.compassOnoff);
    if(uteModuleCompassData.compassOnoff==false)
    {
        uteModuleCompassData.compassOnoff = true;
        uteDrvMagneticCommonInit();
        uteModuleCompassCheckTimerStart(UTE_DRV_COMPASS_SAMPLING_INTERVAL);  //开启数据采样和处理定时器，时间间隔为20ms
        // uteModuleSystemSleepExit(UTE_MODULE_SYSTEM_MAGNETIC_BIT);
        // sensor_manager_enable(RAW_MAG, 0);
    }
}
/**
*@brief  结束测量
*@details
*@author        pcm
*@date        2024-11-25
*/
void uteModuleCompassStop(void)
{
    UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s compassOnoff = %d",__func__,uteModuleCompassData.compassOnoff);
    if(uteModuleCompassData.compassOnoff==true)
    {
        uteModuleCompassData.compassOnoff = false;
        uteModuleCompassCheckTimerStop();
        uteDrvMagneticCommonDeepSleep();

    }
#if UTE_DRV_REUSE_HEART_POWER_SUPPORT
    if (!bsp_sensor_hr_work_status()
#if UTE_DRV_REUSE_HEART_POWER_SUPPORT && UTE_DRV_LED_SUPPORT
        && (!uteDrvLedIsOpen())
#endif
       )
#endif
    {
        uteModulePlatformOutputGpioSet(UTE_DRV_LED_POWER_GPIO_PIN, false);
    }
}
/**
*@brief  获取角度值，正北方向为0度
*@details  0~359度
*@author        pcm
*@date        2024-11-25
*/
int uteModuleCompassGetAzimuth(void)
{
    // UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s fAzimuth = %f",__func__,uteModuleCompassData.fAzimuth);
    return (int)(uteModuleCompassData.fAzimuth);
}
/**
*@brief  获取水平角度值，水平面为0度
*@details  绕着X轴旋转
*@author        pcm
*@date        2024-11-25
*/
float uteModuleCompassGetPitch(void)
{
    return uteModuleCompassData.fPitch;
}
/**
*@brief
*@details  绕着Z轴旋转
*@author        pcm
*@date        2024-11-25
*/
float uteModuleCompassGetRoll(void)
{
    return uteModuleCompassData.fRoll;
}
/**
*@brief  获取地磁质量值
*@details
*@author        pcm
*@date        2024-11-25
*/
int uteModuleCompassGetAccuracy(void)
{
    return uteModuleCompassData.iAccuracy;
}
/**
*@brief  是否校准完成
*@details
*@author        pcm
*@date        2024-11-25
*/
bool uteModuleCompassIsCalibrationFinish(void)
{
    if (uteModuleCompassData.iAccuracy >= 3)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
*@brief  获取原始三轴数据
*@details
*@author        pcm
*@date        2024-11-25
*/
float* uteModuleCompassAccrawData(void)
{
    return &acc_raw_data[3];
}
/**
*@brief  开始定时器中断处理，处理数据发送给算法
*@details
*@author        pcm
*@date        2024-11-27
*/
void uteModuleCompassDataputHandler(void)
{
    int16_t x, y, z;
    int16_t xOut, yOut, zOut;
    ute_drv_gsensor_common_axis_bit_change_t bitChange;
    uteDrvGsensorCommonGetAccXyz(&x, &y, &z);
    bitChange.inputXaxis = &x;
    bitChange.inputYaxis = &y;
    bitChange.inputZaxis = &z;
    bitChange.outputXaxis = &xOut;
    bitChange.outputYaxis = &yOut;
    bitChange.outputZaxis = &zOut;
    uteDrvGsensorCommonXYZaxisDataBitChange(&bitChange, 1, GSENSOR_DATA_BIT_STEP);

    float magoutData[3];    //读取返回的高斯值
    float radData[6]= {0};  //写入数据不需要赋值，里面有处理
    uteDrvMagneticCommonConvertData(magoutData,radData);

    float accRaw[3];
    accRaw[0] = xOut;
    accRaw[1] = yOut;
    accRaw[2] = zOut;
    // UTE_MODULE_LOG(UTE_LOG_COMPASS_LVL,"%s,accRaw:%f,%f,%f,xOut=%d,%d,%d",__func__,accRaw[0],accRaw[1],accRaw[2],xOut,yOut,zOut);
    uteModuleCompassAlgohandler(magoutData,accRaw);
};
#endif


