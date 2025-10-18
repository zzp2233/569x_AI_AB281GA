/**
*@file
*@brief     血氧模块
*@details
*@author        zn.zeng
*@date        2021-07-21
*@version       v1.0
*/
#include "ute_module_heart.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_bloodoxygen.h"
#include "ute_module_protocol.h"
#include "ute_module_crc.h"
#include "ute_drv_motor.h"
#include "ute_module_sport.h"
#include "ute_drv_battery_common.h"
#include "ute_module_factoryTest.h"
#include "ute_module_newFactoryTest.h"
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
#include "ute_module_emotionPressure.h"
#endif
#if UTE_MODULE_ONE_CLICK_MEASURE_SUPPORT
#include "ute_module_one_click_measure.h"
#endif
#define BLOOD_OXYGEN_COUNT 288   //144：10分钟自动测量一次; 288 : 5分钟测量一次
#if UTE_MODULE_BLOODOXYGEN_SUPPORT
/*! 血压数据结构zn.zeng, 2021-07-23  */
ute_module_bloodoxygen_data_t uteModuleBloodoxygenData;
#if DUG_VCXX_BLOOD_OXYGEN_SUPPORT
// #undef UTE_MODULE_BLOODOXYGEN_TEST_TIMEOUT_SECOND
// #define UTE_MODULE_BLOODOXYGEN_TEST_TIMEOUT_SECOND 0xFFFE
#endif
/**
*@brief        初始化函数
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, NULL, NULL);
#endif
    memset(&uteModuleBloodoxygenData,0,sizeof(ute_module_bloodoxygen_data_t));
    uteModuleBloodoxygenReadLastTestInfo();
    uteModuleSystemtimeRegisterSecond(uteModuleBloodoxygenEverySecond);
}
/**
*@brief        读取config
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[8];
    readbuff[0] = UTE_MODULE_BLOODOXYGEN_IS_AUTO_OPEN;
    readbuff[1] = UTE_MODULE_BLOODOXYGEN_IS_AUTO_TIME_BUCKET_OPEN;
    readbuff[2] = (UTE_MODULE_BLOODOXYGEN_AUTO_INTERVAL_MIN>>8)&0xff;
    readbuff[3] = (UTE_MODULE_BLOODOXYGEN_AUTO_INTERVAL_MIN)&0xff;
    readbuff[4] = (UTE_MODULE_BLOODOXYGEN_TIME_BUCKET_START>>8)&0xff;
    readbuff[5] = (UTE_MODULE_BLOODOXYGEN_TIME_BUCKET_START)&0xff;
    readbuff[6] = (UTE_MODULE_BLOODOXYGEN_TIME_BUCKET_END>>8)&0xff;
    readbuff[7] = (UTE_MODULE_BLOODOXYGEN_TIME_BUCKET_END)&0xff;
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SET_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleBloodoxygenData.isAutoTesting=readbuff[0];
    uteModuleBloodoxygenData.isAutoTimeBucketTesting=readbuff[1];
    uteModuleBloodoxygenData.intervalMin=readbuff[2]<<8|readbuff[3];
    uteModuleBloodoxygenData.startTimeHourMin=readbuff[4]<<8|readbuff[5];
    uteModuleBloodoxygenData.endTimeHourMin=readbuff[6]<<8|readbuff[7];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isAutoTesting=%d,intervalMin=%d", __func__,uteModuleBloodoxygenData.isAutoTesting,uteModuleBloodoxygenData.intervalMin);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__,uteModuleBloodoxygenData.startTimeHourMin,uteModuleBloodoxygenData.endTimeHourMin);
#if APP_SYNC_RECORD_TIME_FLAG_SUPPORT
    uteModuleBloodoxygenReadSyncTimeInfo();
#endif
}

/**
*@brief        血氧每秒函数
*@details       需要注册到主时间，每秒执行一次
*@author       zn.zeng
*@date       2021-07-23
*/
void uteModuleBloodoxygenEverySecond(void)
{
    if(!uteApplicationCommonIsPowerOn())
    {
        return;
    }

    static bool isNeedAutoTest = false;

    if(uteModuleBloodoxygenData.isSingleTesting)
    {
        uteModuleBloodoxygenData.value = uteModuleHeartGetBloodOxygenValue();
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s,.value=%d",__func__,uteModuleBloodoxygenData.value);
        if(uteModuleBloodoxygenIsWear() && (uteModuleHeartGetWorkMode() == HR_WORK_MODE_SPO2) && bsp_sensor_hr_work_status())
        {
#if !DUG_VCXX_BLOOD_OXYGEN_SUPPORT
            UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s,testingSecond:%d",__func__,uteModuleBloodoxygenData.testingSecond);
            if(uteModuleBloodoxygenData.testingSecond > UTE_MODULE_BLOODOXYGEN_TEST_TIMEOUT_SECOND)
            {
                if(!uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
                {
                    uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                }
                uteModuleBloodoxygenStopSingleTesting();
            }
            else
            {
                if (uteModuleBloodoxygenData.testingSecond > UTE_MODULE_BLOODOXYGEN_TEST_DEFAULT_SECOND &&
                    uteModuleBloodoxygenData.value > 0 && uteModuleBloodoxygenData.value < 0xff)
                {
                    if(!uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
                    {
                        uteDrvMotorStart(UTE_MOTOR_DURATION_TIME,UTE_MOTOR_INTERVAL_TIME,1);
                    }
                    uteModuleBloodoxygenStopSingleTesting();
                }
                uteModuleBloodoxygenData.testingSecond++;
            }
#endif
            uteModuleBloodoxygenData.notWearSecond = 0;
        }
        else
        {
            uteModuleBloodoxygenData.notWearSecond++;
            if (uteModuleBloodoxygenData.notWearSecond >= 2)
            {
                if (uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
                {
                    isNeedAutoTest = false;
                }
                uteModuleBloodoxygenStopSingleTesting();
                uteModuleBloodoxygenData.notWearSecond = 0;
            }
        }
    }

    /*! 自动测试逻辑 zn.zeng, 2021-08-31  */
    if(uteModuleBloodoxygenData.isAutoTesting)
    {
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        uint32_t oneDaySec = time.hour*3600+time.min*60+time.sec;
        if(uteModuleBloodoxygenData.isAutoTimeBucketTesting)
        {
            if(((oneDaySec%(60*uteModuleBloodoxygenData.intervalMin))==0) && !isNeedAutoTest)
            {
                uint16_t currentTime = ((time.hour<<8&0xff00)|time.min);
                if(uteModuleBloodoxygenData.endTimeHourMin>uteModuleBloodoxygenData.startTimeHourMin)
                {
                    if((currentTime>=uteModuleBloodoxygenData.startTimeHourMin)&&(currentTime<=uteModuleBloodoxygenData.endTimeHourMin))
                    {
                        isNeedAutoTest = true;
                    }
                }
                else
                {
                    if((currentTime>=uteModuleBloodoxygenData.startTimeHourMin)||(currentTime<=uteModuleBloodoxygenData.endTimeHourMin))
                    {
                        isNeedAutoTest = true;
                    }
                }
            }
        }
        else
        {
            if((oneDaySec%(60*uteModuleBloodoxygenData.intervalMin))==0 && !isNeedAutoTest)
            {
                isNeedAutoTest = true;
            }
        }
        if (//uteModuleBloodoxygenIsWear() &&
            (uteDrvBatteryCommonGetChargerStatus() == BAT_STATUS_NO_CHARGE) &&
            (!uteModuleSportMoreSportIsRuning()) &&
            (uteApplicationCommonGetSystemPowerOnSecond() > 5) &&
            (uteModuleFactoryTestGetCurrTestItem() == TEST_ITEM_NONE) &&
#if UTE_MODULE_NEW_FACTORY_TEST_SUPPORT
            (uteModuleNewFactoryTestGetMode() == FACTORY_TEST_MODE_NULL) &&
#endif
            // (!uteModuleBreathrateIsTesting()) &&
            // (!uteModuleGetBreathTrainingStatus()) &&
            (!uteModuleHeartIsSingleTesting()) &&
            (!uteModuleHeartIsAutoTestFlag())
#if UTE_MODULE_EMOTION_PRESSURE_SUPPORT
            && (!uteModuleEmotionPressureIsTesting())
#endif
#if UTE_MODULE_ONE_CLICK_MEASURE_SUPPORT
            && (!uteModuleOneClickMeasureIsTesting())
#endif
        )
        {
#if UTE_MODULE_BLOODOXYGEN_RANDOM_SUPPORT
            if (isNeedAutoTest && !uteModuleBloodoxygenData.isSingleTesting)
            {
                if (uteModuleHeartGetHeartValue() > 0 && uteModuleHeartGetHeartValue() < 0xff)
                {
                    uteModuleBloodoxygenData.value = 96 + get_random(4);
                    uteModuleBloodoxygenAutoSaveOxygenData();
                    isNeedAutoTest = false;
                }
                else
                {
                    uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag = true;
                    uteModuleBloodoxygenStartSingleTesting();
                }
            }
            else
            {
                isNeedAutoTest = false;
            }
#else
            if (isNeedAutoTest && !uteModuleBloodoxygenData.isSingleTesting)
            {
                uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag = true;
                uteModuleBloodoxygenStartSingleTesting();
            }
            else
            {
                isNeedAutoTest = false;
            }
#endif
        }
    }
}
/**
*@brief        开始单次测试
*@details
*@author       zn.zeng
*@date       2021-07-21
*/
void uteModuleBloodoxygenStartSingleTesting(void)
{
    if(bsp_sensor_hr_work_status() && uteModuleHeartGetWorkMode() == HR_WORK_MODE_SPO2)
    {
        uteModuleBloodoxygenData.isSingleTesting = true;
        return;
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s",__func__);
    uteModuleBloodoxygenData.isSingleTesting = true;
    uteModuleBloodoxygenData.value = 0;
    uteModuleBloodoxygenData.testingSecond = 0;
    if (!uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
    {
        uint8_t startCmdRsp[2] = {CMD_SPO2_TEST,0x11};
        uteModuleProfileBleSendToPhone(&startCmdRsp[0],2);
    }
    uteModuleHeartStartSingleTesting(TYPE_BLOODOXYGEN);
}
/**
*@brief        结束单次测试
*@details
*@author       zn.zeng
*@date       2021-07-21
*/
void uteModuleBloodoxygenStopSingleTesting(void)
{
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s",__func__);
    if(uteModuleHeartGetWorkMode() != HR_WORK_MODE_SPO2 || !bsp_sensor_hr_work_status())
    {
        uteModuleBloodoxygenData.isSingleTesting = false;
        return;
    }
    uint8_t stopCmd[5];
    stopCmd[0] =CMD_SPO2_TEST;
    stopCmd[1] =0x00;
    uteModuleBloodoxygenData.value = uteModuleHeartGetBloodOxygenValue();
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL,"%s,.value=%d",__func__,uteModuleBloodoxygenData.value);
    if(uteModuleBloodoxygenData.value > 0 && uteModuleBloodoxygenData.value < 0xff)
    {
        stopCmd[2] =0x00;
        stopCmd[3] =uteModuleBloodoxygenData.value ;
#if UTE_MODULE_BLOODGXYGEN_REAL_TIME_MAX_AND_MIN_VAULE_SUPPORT
        uteModuleBloodoxygenData.lastValue = uteModuleBloodoxygenData.value;
        if (uteModuleBloodoxygenData.dayMinValue == 0)
        {
            uteModuleBloodoxygenData.dayMinValue = 0xff;
        }
        if (uteModuleBloodoxygenData.lastValue > uteModuleBloodoxygenData.dayMaxValue)
        {
            uteModuleBloodoxygenData.dayMaxValue = uteModuleBloodoxygenData.lastValue;
        }
        if (uteModuleBloodoxygenData.lastValue < uteModuleBloodoxygenData.dayMinValue)
        {
            uteModuleBloodoxygenData.dayMinValue = uteModuleBloodoxygenData.lastValue;
        }
#endif
        /*! 保存数据zn.zeng, 2021-07-23  */
        uteModuleBloodoxygenAutoSaveOxygenData();
    }
    else
    {
        stopCmd[2] =0xff;
        stopCmd[3] =0xff;
        uteModuleBloodoxygenData.value = 0xff;
    }
    if (!uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag)
    {
        uteModuleProfileBleSendToPhone(&stopCmd[0],4);
    }
    uteModuleBloodoxygenData.isSingleTesting = false;
    uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag = false;
    // uteModuleBloodoxygenData.testingSecond = 0;
    uteModuleHeartStopSingleTesting(TYPE_BLOODOXYGEN);

    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
}
/**
*@brief        是否佩戴
*@details
*@author       zn.zeng
*@date       2021-07-21
*/
bool uteModuleBloodoxygenIsWear(void)
{
    bool isWear = false;
    isWear = uteModuleHeartIsWear();
    return isWear;
}
/**
*@brief        获取血氧值
*@details
*@author       zn.zeng
*@date       2021-08-05
*/
uint8_t uteModuleBloodoxygenGetValue(void)
{
    return uteModuleBloodoxygenData.value;
}

/**
 * @brief        设置血氧值
 * @details
 * @param[in]    value 血氧值
 * @author       Wang.Luo
 * @date         2022-11-29
 */
void uteModuleBloodoxygenSetValue(uint8_t value)
{
    uteModuleBloodoxygenData.value = value;
}

/**
*@brief        获取血氧测试状态
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
bool uteModuleBloodoxygenIsTesting(void)
{
    return uteModuleBloodoxygenData.isSingleTesting;
}
/**
*@brief        保存血氧设置参数
*@details
*@param[in] bool isAutoTesting  是否打开自动测试
*@param[in] uint16_t intervalMin  测试间隔，单位为分钟
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenSaveAutoIntervalParam(bool isAutoTesting,uint16_t intervalMin)
{
    uteModuleBloodoxygenData.isAutoTesting = isAutoTesting;
    uteModuleBloodoxygenData.intervalMin = intervalMin;
    /*! 保存到文件zn.zeng, 2021-08-30*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleBloodoxygenData.isAutoTesting;
    writebuff[1] = uteModuleBloodoxygenData.isAutoTimeBucketTesting;
    writebuff[2] = (uteModuleBloodoxygenData.intervalMin>>8)&0xff;
    writebuff[3] = (uteModuleBloodoxygenData.intervalMin)&0xff;
    writebuff[4] = (uteModuleBloodoxygenData.startTimeHourMin>>8)&0xff;
    writebuff[5] = (uteModuleBloodoxygenData.startTimeHourMin)&0xff;
    writebuff[6] = (uteModuleBloodoxygenData.endTimeHourMin>>8)&0xff;
    writebuff[7] = (uteModuleBloodoxygenData.endTimeHourMin)&0xff;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SET_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,isAutoTesting=%d,intervalMin=%d,isAutoTimeBucketTesting=%d", __func__,uteModuleBloodoxygenData.isAutoTesting,uteModuleBloodoxygenData.intervalMin,uteModuleBloodoxygenData.isAutoTimeBucketTesting);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__,uteModuleBloodoxygenData.startTimeHourMin,uteModuleBloodoxygenData.endTimeHourMin);

}
/**
*@brief        保存血氧设置参数
*@details
*@param[in] bool isAutoTimeBucketTesting  是否打开时间段自动测试
*@param[in] uint16_t startTimeHourMin 开始时间，高8bit为小时，低8bit为分钟
*@param[in] uint16_t endTimeHourMin 结束时间，高8bit为小时，低8bit为分钟
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenSaveAutoTimeBucketParam(bool isAutoTimeBucketTesting,uint16_t startTimeHourMin,uint16_t endTimeHourMin)
{
    uteModuleBloodoxygenData.isAutoTimeBucketTesting = isAutoTimeBucketTesting;
    uteModuleBloodoxygenData.startTimeHourMin = startTimeHourMin;
    uteModuleBloodoxygenData.endTimeHourMin = endTimeHourMin;
    /*! 保存到文件zn.zeng, 2021-08-30*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleBloodoxygenData.isAutoTesting;
    writebuff[1] = uteModuleBloodoxygenData.isAutoTimeBucketTesting;
    writebuff[2] = (uteModuleBloodoxygenData.intervalMin>>8)&0xff;
    writebuff[3] = (uteModuleBloodoxygenData.intervalMin)&0xff;
    writebuff[4] = (uteModuleBloodoxygenData.startTimeHourMin>>8)&0xff;
    writebuff[5] = (uteModuleBloodoxygenData.startTimeHourMin)&0xff;
    writebuff[6] = (uteModuleBloodoxygenData.endTimeHourMin>>8)&0xff;
    writebuff[7] = (uteModuleBloodoxygenData.endTimeHourMin)&0xff;
    if( uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SET_INFO,&file,FS_O_WRONLY|FS_O_CREAT|FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file,&writebuff[0],8);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,isAutoTesting=%d,intervalMin=%d,isAutoTimeBucketTesting=%d", __func__,uteModuleBloodoxygenData.isAutoTesting,uteModuleBloodoxygenData.intervalMin,uteModuleBloodoxygenData.isAutoTimeBucketTesting);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__,uteModuleBloodoxygenData.startTimeHourMin,uteModuleBloodoxygenData.endTimeHourMin);
}
#if APP_SAVE_HR_OXYGEN_EVERY_FIVE_MINUTES_SUPPORT
/**
*@brief        保存血氧数据数据，每5分钟一条数据
*@details      每天保存一个文件，自动保存
*@author       wuhuowang
*@date       2022-11-19
*/
void uteModuleBloodoxygenAutoSaveOxygenData(void)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = 12 * 24 + 4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    char path[50];
    memset(&readBuff[0], 0xff, buffSize);
    memset(&path[0], 0, 50);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0], "%04d%02d%02d", time.year, time.month, time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, dirInfo, NULL);
    if ((dirInfo->filesCnt >= UTE_MODULE_BLOODOXYGEN_SAVE_DATA_MAX_DAYS) && (memcmp(&path[0], &dirInfo->filesName[0][0], 8) != 0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-08-25*/
        memset(&path[0], 0, 50);
        sprintf(&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, &dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile(&path[0]);
    }
    memset(&path[0], 0, 50);
    sprintf(&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-08-30  */
    void *file;
    // read
    if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_RDONLY))
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
    if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_WRONLY | FS_O_CREAT))
    {
        uint16_t cacheOffset = 0;
        cacheOffset = 4 + (time.hour * 60 + time.min) / 5;
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,cacheOffset=%d", __func__,cacheOffset);
        readBuff[cacheOffset] = uteModuleBloodoxygenData.value;
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &readBuff[0], buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(readBuff);
    uteModulePlatformMemoryFree(dirInfo);
    uteModuleBloodoxygenSetLastTestInfo(uteModuleBloodoxygenData.value, time);
}
/**
*@brief        开始发送24小时血氧数据
*@details
*@author       wuhuowang
*@date       2022-11-19
*/
void uteModuleBloodoxygenSendHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[30];
    uint8_t response[20];
    uint8_t sendSize = 0;
    response[0] = CMD_SPO2_TEST;
    response[1] = 0xfa;
    memset(&path[0], 0, 30);
    uint8_t *tempDataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&tempDataBuff[0], 0, sendParam->dataBuffSize);
    memset(&dataBuff[0], 0, sendParam->dataBuffSize);
    if (sendParam->dirInfo.filesCnt > 0)
    {
    SEND_NEXT_DATA:
        sendSize = 20;
        bool isHasData = false;
        void *file;
        sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,read file=%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,currSendFileIndex=%d,currSendMinIndex=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendMinIndex);
        if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
            memcpy(&dataBuff[0], tempDataBuff, 4);      /*!获取年月日*/
            memcpy(&dataBuff[4], &tempDataBuff[5], 287); /*!获取当天00：10~23：55的数据*/
            uteModuleFilesystemCloseFile(file);
        }
        if ((sendParam->currSendFileIndex + 1) < sendParam->dirInfo.filesCnt)
        {
            memset(&tempDataBuff[0], 0, sendParam->dataBuffSize);
            sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex + 1][0]);
            if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
            {
                uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
                memcpy(&dataBuff[sendParam->dataBuffSize - 1], &tempDataBuff[4], 1); /*!获取后一天00：00的数据*/
                uteModuleFilesystemCloseFile(file);
            }
        }
        uint16_t invalidMin = 0;
        for(uint16_t i = sendParam->currSendMinIndex; i < BLOOD_OXYGEN_COUNT;)
        {
            if ((memcmp(&dataBuff[4 + i], "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12) == 0) || (memcmp(&dataBuff[4 + i], "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", 12) == 0))
            {
                invalidMin = invalidMin + 12;
                i = i + 12;
            }
            else
            {
                uint8_t sendHour = i / 12 + 1;
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
                memcpy(&response[2], &dataBuff[0], 4);//year month day
                response[6] = sendHour;
                response[7] = 0;
                memcpy(&response[8], &dataBuff[4 + i], 12);
                invalidMin = invalidMin + 12;
                i = i + 12;
                isHasData = true;
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,invalidMin=%d", __func__,invalidMin);
        sendParam->currSendMinIndex += invalidMin;
        if (sendParam->currSendMinIndex >= BLOOD_OXYGEN_COUNT)
        {
            sendParam->currSendFileIndex ++;
            if (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendMinIndex = 0;
            }
            else
            {
                sendParam->currSendMinIndex = BLOOD_OXYGEN_COUNT;
            }
        }
        if ((!isHasData) && (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt))
        {
            goto SEND_NEXT_DATA;
        }
    }
    else
    {
        sendParam->currSendMinIndex = BLOOD_OXYGEN_COUNT;
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex,sendParam->dirInfo.filesCnt);
    if ((sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt)
        && (sendParam->currSendMinIndex == BLOOD_OXYGEN_COUNT))
    {
        sendSize = 4;
        response[1] = 0xfa;
        response[2] = 0xfd;
        response[3] = sendParam->crc;
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,stop", __func__);
        uteApplicationCommonSyncDataTimerStop();
#if APP_SYNC_RECORD_TIME_FLAG_SUPPORT
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        ute_module_systemtime_measure_time_t setTime;
        memset(&setTime,0,sizeof(ute_module_systemtime_measure_time_t));
        setTime.year = time.year;
        setTime.month = time.month;
        setTime.day = time.day;
        setTime.hour = time.hour;
        setTime.min = time.min;
        setTime.sec = time.sec;
        uteModuleBloodoxygenSetSyncTime(setTime);
#endif
    }
    if (sendSize == 20)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[2], 18);
    }
    uteModulePlatformMemoryFree(tempDataBuff);
    uteModulePlatformMemoryFree(dataBuff);
    uteModuleProfileBleSendToPhone(&response[0], sendSize);
}
/**
*@brief        准备开始发送血氧数据
*@details
*@param[in]  需要同步的数据时间戳
*@author       wuhuowang
*@date       2022-11-19
*/
void uteModuleBloodoxygenStartSendAutoTestHistoryData(ute_module_systemtime_time_t time)
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
    param->dataBuffSize = 12 * 24 + 4;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, &param->dirInfo, NULL);
#if APP_SYNC_RECORD_TIME_FLAG_SUPPORT
    int diffday = 0;
    ute_module_systemtime_time_t curryTime;
    uteModuleSystemtimeGetTime(&curryTime);
    if(uteModuleBloodoxygenData.syncTime.year == 0)
    {
        diffday = 0;
    }
    else
    {
        diffday = uteModuleSystemtimeGetDiffDay(uteModuleBloodoxygenData.syncTime.year,uteModuleBloodoxygenData.syncTime.month,uteModuleBloodoxygenData.syncTime.day,curryTime.year,curryTime.month,curryTime.day);
    }
    if((diffday >= 0)&&(diffday <= UTE_MODULE_BLOODOXYGEN_SAVE_DATA_MAX_DAYS))
    {
        if((param->dirInfo.filesCnt - diffday) > 0)
        {
            param->currSendFileIndex = param->dirInfo.filesCnt - diffday - 1;
        }
    }
#endif
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleBloodoxygenSendHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s", __func__);
}
#else
/**
*@brief        保存血氧数据数据，每10分钟一条数据
*@details      每天保存一个文件，自动保存
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenAutoSaveOxygenData(void)
{
    /*! 没有连接过APP不保存数据 xjc, 2022-05-06  */
    if(!uteApplicationCommonIsHasConnectOurApp())
    {
        return;
    }
    uint16_t buffSize = 6*24+4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    uint8_t path[42];
    memset(&readBuff[0],0xff,buffSize);
    memset(&path[0],0,42);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0],"%04d%02d%02d",time.year,time.month,time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, dirInfo, NULL);
    if((dirInfo->filesCnt>=UTE_MODULE_BLOODOXYGEN_SAVE_DATA_MAX_DAYS)&&(memcmp(&path[0],&dirInfo->filesName[0][0],8)!=0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-08-25*/
        memset(&path[0],0,42);
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,&dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    memset(&path[0],0,42);
    sprintf((char *)&path[0],"%s/%04d%02d%02d",UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,time.year,time.month,time.day);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,dirInfo->filesCnt=%d", __func__,dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,save file=%s", __func__,&path[0]);
    /*! 保存当前数据zn.zeng, 2021-08-30  */
    void *file;
    // read
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    else
    {
        readBuff[0] = time.year>>8&0xff;
        readBuff[1] = time.year&0xff;
        readBuff[2] = time.month;
        readBuff[3] = time.day;
    }
    if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_WRONLY|FS_O_CREAT))
    {
        uint16_t cacheOffset = 0;
        cacheOffset = 4+(time.hour*60+time.min)/10;
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,cacheOffset=%d , uteModuleBloodoxygenData.value = %d", __func__,cacheOffset,uteModuleBloodoxygenData.value);
        if(uteModuleBloodoxygenData.value!=0xFF && uteModuleBloodoxygenData.value!=0x00)
        {
            readBuff[cacheOffset] = uteModuleBloodoxygenData.value;
        }
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&readBuff[0],buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(readBuff);
    uteModulePlatformMemoryFree(dirInfo);
    uteModuleBloodoxygenSetLastTestInfo(uteModuleBloodoxygenData.value, time);
}
/**
*@brief        开始发送24小时血氧数据
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenSendHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    uint8_t path[42];
    uint8_t response[20];
    uint8_t sendSize = 0;
    response[0] = CMD_SPO2_TEST;
    response[1] = 0xfa;
    memset(&path[0],0,42);
    uint8_t *tempDataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&tempDataBuff[0],0,sendParam->dataBuffSize);
    memset(&dataBuff[0],0,sendParam->dataBuffSize);
    if(sendParam->dirInfo.filesCnt>0)
    {
    SEND_NEXT_DATA:
        sendSize = 20;
        bool isHasData = false;
        void *file;
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,&sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,read file=%s", __func__,&path[0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,currSendFileIndex=%d,currSendMinIndex=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendMinIndex);
        if(uteModuleFilesystemOpenFile((char *)&path[0],&file,FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
            memcpy(&dataBuff[0], tempDataBuff, 4);      /*!获取年月日 , xjc 2022-03-03*/
            memcpy(&dataBuff[4], &tempDataBuff[5], 143); /*!获取当天00：10~23：50的数据, xjc 2022-03-03*/
            uteModuleFilesystemCloseFile(file);
        }
        if ((sendParam->currSendFileIndex + 1) < sendParam->dirInfo.filesCnt)
        {
            memset(&tempDataBuff[0],0,sendParam->dataBuffSize);
            sprintf((char *)&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex + 1][0]);
            if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
            {
                uteModuleFilesystemReadData(file, &tempDataBuff[0], sendParam->dataBuffSize);
                memcpy(&dataBuff[sendParam->dataBuffSize - 1], &tempDataBuff[4], 1); /*!获取后一天00：00的数据, xjc 2022-03-03*/
                uteModuleFilesystemCloseFile(file);
            }
        }
        uint16_t invalidMin = 0;
        for(uint16_t i=sendParam->currSendMinIndex; i<144;)
        {
            if((memcmp(&dataBuff[4+i],"\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00",12)==0)||(memcmp(&dataBuff[4+i],"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff",12)==0))
            {
                invalidMin=invalidMin+12;
                i = i+12;
            }
            else
            {
                uint8_t sendHour = i/6+2;
                if(sendHour==24)
                {
                    ute_module_systemtime_time_t timeAdd;
                    timeAdd.year = dataBuff[0]<<8|dataBuff[1];
                    timeAdd.month = dataBuff[2];
                    timeAdd.day = dataBuff[3];
                    uteModuleSystemtimeInputDateCalDay(&timeAdd,1);
                    dataBuff[0] = (timeAdd.year>>8)&0xff;
                    dataBuff[1] = (timeAdd.year)&0xff;
                    dataBuff[2] = timeAdd.month;
                    dataBuff[3] = timeAdd.day;
                    sendHour = 0;
                }
                memcpy(&response[2],&dataBuff[0],4);//year month day
                response[6] = sendHour;
                response[7] = 0;
                memcpy(&response[8],&dataBuff[4+i],12);
                invalidMin=invalidMin+12;
                i = i+12;
                isHasData = true;
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,invalidMin=%d", __func__,invalidMin);
        sendParam->currSendMinIndex+=invalidMin;
        if(sendParam->currSendMinIndex>=144)
        {
            sendParam->currSendFileIndex++;
            if(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendMinIndex = 0;
            }
            else
            {
                sendParam->currSendMinIndex=144;
            }
        }
        if((!isHasData)&&(sendParam->currSendFileIndex<sendParam->dirInfo.filesCnt))
        {
            goto SEND_NEXT_DATA;
        }
    }
    else
    {
        sendParam->currSendMinIndex=144;
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__,sendParam->currSendFileIndex,sendParam->currSendHourIndex,sendParam->dirInfo.filesCnt);
    if((sendParam->currSendFileIndex==sendParam->dirInfo.filesCnt)
       &&(sendParam->currSendMinIndex==144))
    {
        sendSize = 4;
        response[1] = 0xfa;
        response[2] = 0xfd;
        response[3] = sendParam->crc;
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,stop", __func__);
        uteApplicationCommonSyncDataTimerStop();
    }
    if(sendSize==20)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[2],18);
    }
    uteModulePlatformMemoryFree(tempDataBuff);
    uteModulePlatformMemoryFree(dataBuff);
    uteModuleProfileBleSendToPhone(&response[0],sendSize);
}
/**
*@brief        准备开始发送血氧数据
*@details
*@param[in]  需要同步的数据时间戳
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenStartSendAutoTestHistoryData(ute_module_systemtime_time_t time)
{
    ute_application_sync_data_param_t *param;
    uteApplicationCommonGetSyncDataParam(&param);
    param->crc = 0;
    param->time = time;
    if(param->time.year==0)
    {
        param->isAllData = true;
    }
    param->currSendFileIndex = 0;
    param->currSendHourIndex = 0;
    param->currSendMinIndex = 0;
    param->dataBuffSize = 6*24+4;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleBloodoxygenSendHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s", __func__);

}
#endif
/**
*@brief        删除血氧数据
*@details
*@author       zn.zeng
*@date       2021-08-31
*/
void uteModuleBloodoxygenDelHistoryData(void)
{
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s", __func__);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,dirInfo, NULL);
    for(uint8_t i=0; i<dirInfo->filesCnt; i++)
    {
        uint8_t path[40];
        sprintf((char *)&path[0],"%s/%s",UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR,&dirInfo->filesName[i][0]);
        UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,del file=%s", __func__,&path[0]);
        uteModuleFilesystemDelFile((char *)&path[0]);
    }
    uteModulePlatformMemoryFree(dirInfo);
}

/**
*@brief        是否正在测量血氧
*@details
*@author       xjc
*@date       2022-01-18
*/
AT(.com_text.ute_spo2)
bool uteModuleBloodoxygenIsBloodOxygenAutoTesting(void)
{
    return uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag;
}

void uteModuleBloodoxygenSetBloodOxygenAutoTesting(bool isAutoTest)
{
    uteModuleBloodoxygenData.isBloodOxygenAutoTestFlag = isAutoTest;
}

/**
*@brief        是否正在测量血氧
*@details
*@author       xjc
*@date       2022-01-18
*/
uint8_t uteModuleBloodoxygenGetTestingSecond(void)
{
    return uteModuleBloodoxygenData.testingSecond;
}

/**
 * @brief        保存最后一次血氧测量信息
 * @details
 * @param[in]    bloodoxygenValue 血氧值
 * @param[in]    time 时间
 * @author       Wang.Luo
 * @date         2025-09-26
 */
uint8_t uteModuleBloodoxygenSetLastTestInfo(uint8_t bloodoxygenValue, ute_module_systemtime_time_t time)
{
    uteModuleBloodoxygenData.lastTestInfo.lastValue = bloodoxygenValue;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.year = time.year;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.month = time.month;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.day = time.day;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.hour = time.hour;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.min = time.min;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.sec = time.sec;

    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_LAST_INFO, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file, &uteModuleBloodoxygenData.lastTestInfo, sizeof(ute_module_bloodoxygen_test_last_info_t));
        uteModuleFilesystemCloseFile(file);
    }

    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,lastValue=%d,year=%d,month=%d,day=%d,hour=%d,min=%d,sec=%d", __func__, bloodoxygenValue, time.year, time.month, time.day, time.hour, time.min, time.sec);
}

/**
 * @brief        读取最后一次血氧测量信息
 * @details
 * @author       Wang.Luo
 * @date         2025-09-26
 */
void uteModuleBloodoxygenReadLastTestInfo(void)
{
    uteModuleBloodoxygenData.lastTestInfo.lastValue = 0;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.year = 0;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.month = 0;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.day = 0;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.hour = 0;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.min = 0;
    uteModuleBloodoxygenData.lastTestInfo.lastTestTime.sec = 0;

    void *file;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_LAST_INFO, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &uteModuleBloodoxygenData.lastTestInfo, sizeof(ute_module_bloodoxygen_test_last_info_t));
        uteModuleFilesystemCloseFile(file);
    }

    UTE_MODULE_LOG(UTE_LOG_HEART_LVL, "%s,lastValue=%d,year=%d,month=%d,day=%d,hour=%d,min=%d,sec=%d", __func__, uteModuleBloodoxygenData.lastTestInfo.lastValue, uteModuleBloodoxygenData.lastTestInfo.lastTestTime.year, uteModuleBloodoxygenData.lastTestInfo.lastTestTime.month, uteModuleBloodoxygenData.lastTestInfo.lastTestTime.day, uteModuleBloodoxygenData.lastTestInfo.lastTestTime.hour, uteModuleBloodoxygenData.lastTestInfo.lastTestTime.min, uteModuleBloodoxygenData.lastTestInfo.lastTestTime.sec);
}

/**
* @brief        获取最后一次血氧测量信息
* @details
* @param info   存储最后一次血氧测量信息
* @author       Wang.Luo
* @date         2025-09-26
*/
void uteModuleBloodoxygenGetLastTestInfo(ute_module_bloodoxygen_test_last_info_t *info)
{
    memcpy(info, &uteModuleBloodoxygenData.lastTestInfo, sizeof(ute_module_bloodoxygen_test_last_info_t));
}

#if UTE_DISPLAY_BLOODGXYGEN_HISTORY_HISTOGRAM_SUPPORT

/*! 血氧画图的最小值，例如：要画的图表范围从85~100，则MINIMUM=85,wang.luo 2023-03-15 */
#ifndef UTE_DISPLAY_BLOODGXYGEN_MINIMUM
#define UTE_DISPLAY_BLOODGXYGEN_MINIMUM 0
#endif

/**
*@brief 设置血氧柱状图具体的坐标
*@details
*@author       hcj
*@date       2022-04-19
*/
bool uteModuleBloodoxygenSetBloodoxygenHistoryGraph(uint8_t *Bloodoxygen, uint8_t BloodoxygenCount, uint8_t *BloodoxygenHistoryData, int BloodoxygenHistoryDataLen)
{
    if (BloodoxygenCount > BloodoxygenHistoryDataLen || BloodoxygenCount == 0)
    {
        return false;
    }

#if UTE_LOG_GUI_LVL // test
    for (uint8_t i = 0; i < BLOOD_OXYGEN_COUNT; i++)
    {
        BloodoxygenHistoryData[i] = 80 + get_random(20);
    }
#endif

    uint8_t groupSize = 144 / BloodoxygenCount;
#if APP_SAVE_HR_OXYGEN_EVERY_FIVE_MINUTES_SUPPORT
    groupSize = BLOOD_OXYGEN_COUNT / BloodoxygenCount;
#endif

    uint8_t dayMaxValue = 0;
    uint8_t dayMinValue = 0xff;

    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
#if APP_SAVE_HR_OXYGEN_EVERY_FIVE_MINUTES_SUPPORT
    uint16_t currentTimePoint = (time.hour * 60 + time.min) / 5;
#else
    uint16_t currentTimePoint = (time.hour * 60 + time.min) / 10;
#endif

    // 首先遍历整个BloodoxygenHistoryData数组，查找所有有效数据的最大最小值
    for (int i = 0; i < BloodoxygenHistoryDataLen && i <= currentTimePoint; i++)
    {
        uint8_t oxygenValue = BloodoxygenHistoryData[i];
        if (oxygenValue >= 60 && oxygenValue <= 100)
        {
            if (oxygenValue > dayMaxValue)
            {
                dayMaxValue = oxygenValue;
            }
            if (oxygenValue < dayMinValue)
            {
                dayMinValue = oxygenValue;
            }
        }
    }

    // 处理每组数据，计算平均值
    for (uint8_t i = 0; i < BloodoxygenCount; i++)
    {
        uint16_t oxygenValidValue = 0;
        uint8_t validCount = 0;

        // 计算当前组的平均有效血氧值
        for (uint8_t j = 0; j < groupSize; j++)
        {
            // 确保索引不超出范围
            int index = i * groupSize + j;
            if (index < BloodoxygenHistoryDataLen)
            {
                uint8_t oxygenValue = BloodoxygenHistoryData[index];
                if (oxygenValue >= 60 && oxygenValue <= 100)
                {
                    oxygenValidValue += oxygenValue;
                    validCount++;
                }
            }
        }
        uint8_t averageOxygen = (validCount > 0) ? (oxygenValidValue / validCount) : 0;
        if (averageOxygen == 0xFF)
        {
            averageOxygen = 0;
        }
        // 调整显示范围并存储结果
        Bloodoxygen[i] = (averageOxygen > UTE_DISPLAY_BLOODGXYGEN_MINIMUM) ? (averageOxygen - UTE_DISPLAY_BLOODGXYGEN_MINIMUM) : 0;
    }
#if UTE_MODULE_BLOODGXYGEN_REAL_TIME_MAX_AND_MIN_VAULE_SUPPORT
    if (uteModuleBloodoxygenData.dayMaxValue == 0 && dayMaxValue != 0)
#endif
    {
        if (dayMinValue == 0xff)
        {
            dayMinValue = 0;
        }
        uteModuleBloodoxygenData.dayMinValue = dayMinValue;
        uteModuleBloodoxygenData.dayMaxValue = dayMaxValue;
    }
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,dayMaxValue = %d ,dayMinValue = %d", __func__, uteModuleBloodoxygenData.dayMaxValue, uteModuleBloodoxygenData.dayMinValue);
    return true;
}
/**
*@brief        获取当天最大最小血氧值
*@details
*@author       zn.zeng
*@date       2022-09-03
*/
void uteModuleBloodoxygenGetMinMaxValue(uint8_t *min,uint8_t *max)
{
    *min = uteModuleBloodoxygenData.dayMinValue;
    *max = uteModuleBloodoxygenData.dayMaxValue;
}
/**
*@brief      获取当天血氧历史数据，用于血氧柱状图显示
*@details    当天有数据返回true,没有则返回false
*@author     xjc
*@date       2021-12-22
*/
bool uteModuleBloodoxygenGetTodayHistoryData(uint8_t *Bloodoxygen, uint8_t BloodoxygenCount)
{
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s", __func__);
    void *file;
    uint8_t path[40];
    uint8_t BloodoxygenHistoryData[BLOOD_OXYGEN_COUNT]; //一天24h的数据，十分钟一条，一共144条
    uint8_t tempBloodoxygenHistoryData[BLOOD_OXYGEN_COUNT + 4];
    uint16_t buffSize = BLOOD_OXYGEN_COUNT + 4;
#if !UTE_MODULE_BLOODGXYGEN_REAL_TIME_MAX_AND_MIN_VAULE_SUPPORT
    uteModuleBloodoxygenData.dayMaxValue = 0;
    uteModuleBloodoxygenData.dayMinValue = 0xff;
#endif
    ute_module_systemtime_time_t time;
    memset(&Bloodoxygen[0], 0, BloodoxygenCount);
    memset(BloodoxygenHistoryData, 0, BLOOD_OXYGEN_COUNT);
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_BLOODOXYGEN_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_OXYGEN_LVL, "%s,read file=%s", __func__, &path[0]);
    if (uteModuleFilesystemOpenFile((char *)&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file, tempBloodoxygenHistoryData, buffSize);
        uteModuleFilesystemCloseFile(file);
        memcpy(BloodoxygenHistoryData, &tempBloodoxygenHistoryData[4], BLOOD_OXYGEN_COUNT);
        uteModuleBloodoxygenSetBloodoxygenHistoryGraph(Bloodoxygen, BloodoxygenCount, BloodoxygenHistoryData, BLOOD_OXYGEN_COUNT);
        return true;
    }
    else
    {
#if UTE_LOG_GUI_LVL // test
        uteModuleBloodoxygenSetBloodoxygenHistoryGraph(Bloodoxygen, BloodoxygenCount, BloodoxygenHistoryData, 144);
#endif
        return false;
    }
}
#endif

#if APP_SYNC_RECORD_TIME_FLAG_SUPPORT
/**
*@brief     读取血氧同步时间
*@details
*@author        dengli.lu
*@date        2022-03-08
*/
void uteModuleBloodoxygenReadSyncTimeInfo(void)
{
    /*! 读取数据*/
    void *file;
    uint8_t readbuff[7];
    memset(&readbuff[0],0,sizeof(readbuff));
    if(uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SYNC_TIME_INFO,&file,FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemReadData(file,&readbuff[0],7);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleBloodoxygenData.syncTime.year = (readbuff[0] << 8) | readbuff[1];
    uteModuleBloodoxygenData.syncTime.month = readbuff[2];
    uteModuleBloodoxygenData.syncTime.day = readbuff[3];
    uteModuleBloodoxygenData.syncTime.hour = readbuff[4];
    uteModuleBloodoxygenData.syncTime.min = readbuff[5];
    uteModuleBloodoxygenData.syncTime.sec = readbuff[6];
}
/**
*@brief  保存血氧同步时间
*@details
*@author        dengli.lu
*@date        2022-03-08
*/
void uteModuleBloodoxygenSaveSyncTimeInfo(void)
{
    /*! 保存到文件zn.zeng, 2021-08-20*/
    void *file;
    uint8_t writeBuff[7];
    memset(&writeBuff[0],0,sizeof(writeBuff));
    writeBuff[0] = uteModuleBloodoxygenData.syncTime.year >> 8 & 0xff;
    writeBuff[1] = uteModuleBloodoxygenData.syncTime.year & 0xff;
    writeBuff[2] = uteModuleBloodoxygenData.syncTime.month;
    writeBuff[3] = uteModuleBloodoxygenData.syncTime.day;
    writeBuff[4] = uteModuleBloodoxygenData.syncTime.hour;
    writeBuff[5] = uteModuleBloodoxygenData.syncTime.min;
    writeBuff[6] = uteModuleBloodoxygenData.syncTime.sec;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BLOODOXYGEN_SYNC_TIME_INFO, &file, FS_O_RDONLY|FS_O_WRONLY|FS_O_CREAT))
    {
        uteModuleFilesystemSeek(file,0,FS_SEEK_SET);
        uteModuleFilesystemWriteData(file,&writeBuff[0],7);
        uteModuleFilesystemCloseFile(file);
    }
}
/**
*@brief  设置血氧同步时间
*@details
*@author        dengli.lu
*@date        2022-03-08
*/
void uteModuleBloodoxygenSetSyncTime(ute_module_systemtime_measure_time_t setTime)
{
    uteModuleBloodoxygenData.syncTime.year = setTime.year;
    uteModuleBloodoxygenData.syncTime.month = setTime.month;
    uteModuleBloodoxygenData.syncTime.day = setTime.day;
    uteModuleBloodoxygenData.syncTime.hour = setTime.hour;
    uteModuleBloodoxygenData.syncTime.min = setTime.min;
    uteModuleBloodoxygenData.syncTime.sec = setTime.sec;
    uteModuleBloodoxygenSaveSyncTimeInfo();
}
/**
*@brief  获取血氧同步时间
*@details
*@param[out] ute_module_systemtime_measure_time_t *time  时间变量指针
*@author        dengli.lu
*@date        2022-10-24
*/
void uteModuleBloodoxygenGetSyncTime(ute_module_systemtime_measure_time_t *time)
{
    memcpy(time, &uteModuleBloodoxygenData.syncTime, sizeof(ute_module_systemtime_measure_time_t));
}
#endif

#endif //UTE_MODULE_BLOODOXYGEN_SUPPORT
