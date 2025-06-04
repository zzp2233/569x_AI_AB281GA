/**
 *@file
 *@brief     呼吸率模块
 *@details
 *@author        zn.zeng
 *@date        2021-08-16
 *@version       v1.0
 */
#include "ute_module_heart.h"
#include "ute_module_systemtime.h"
#include "ute_module_log.h"
#include "ute_application_common.h"
#include "ute_module_profile_ble.h"
#include "ute_module_breathrate.h"
#include "stdlib.h"
#include "ute_module_protocol.h"
#include "ute_module_crc.h"

#if UTE_MODULE_BREATHRATE_SUPPORT
/*! 呼吸率数据结构zn.zeng, 2021-08-16  */
ute_module_breathrate_data_t uteModuleBreathrateData;
/**
 *@brief        初始化函数
 *@details
 *@author       zn.zeng
 *@date       2021-08-31
 */
void uteModuleBreathrateInit(void)
{
    uteModuleFilesystemCreateDirectory(UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR);
#if UTE_MODULE_LOG_SUPPORT
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, NULL, NULL);
#endif
    memset(&uteModuleBreathrateData, 0, sizeof(ute_module_breathrate_data_t));
    uteModuleSystemtimeRegisterSecond(uteModuleBreathrateEverySecond);
}
/**
 *@brief        读取config
 *@details
 *@author       zn.zeng
 *@date       2021-08-31
 */
void uteModuleBreathrateReadConfig(void)
{
    void *file = NULL;
    uint8_t readbuff[8];
    readbuff[0] = UTE_MODULE_BREATHRATE_IS_AUTO_OPEN;
    readbuff[1] = UTE_MODULE_BREATHRATE_IS_AUTO_TIME_BUCKET_OPEN;
    readbuff[2] = (UTE_MODULE_BREATHRATE_AUTO_INTERVAL_MIN >> 8) & 0xff;
    readbuff[3] = (UTE_MODULE_BREATHRATE_AUTO_INTERVAL_MIN) & 0xff;
    readbuff[4] = (UTE_MODULE_BREATHRATE_TIME_BUCKET_START >> 8) & 0xff;
    readbuff[5] = (UTE_MODULE_BREATHRATE_TIME_BUCKET_START) & 0xff;
    readbuff[6] = (UTE_MODULE_BREATHRATE_TIME_BUCKET_END >> 8) & 0xff;
    readbuff[7] = (UTE_MODULE_BREATHRATE_TIME_BUCKET_END) & 0xff;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BREATHRATE_SET_INFO, &file, FS_O_RDONLY))
    {
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemReadData(file, &readbuff[0], 8);
        uteModuleFilesystemCloseFile(file);
    }
    uteModuleBreathrateData.isAutoTesting = readbuff[0];
    uteModuleBreathrateData.isAutoTimeBucketTesting = readbuff[1];
    uteModuleBreathrateData.intervalMin = readbuff[2] << 8 | readbuff[3];
    uteModuleBreathrateData.startTimeHourMin = readbuff[4] << 8 | readbuff[5];
    uteModuleBreathrateData.endTimeHourMin = readbuff[6] << 8 | readbuff[7];
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,isAutoTesting=%d,intervalMin=%d", __func__, uteModuleBreathrateData.isAutoTesting, uteModuleBreathrateData.intervalMin);
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__, uteModuleBreathrateData.startTimeHourMin, uteModuleBreathrateData.endTimeHourMin);
}
/**
 *@brief        呼吸率每秒函数
 *@details       需要注册到主时间，每秒执行一次
 *@author       zn.zeng
 *@date       2021-07-23
 */
void uteModuleBreathrateEverySecond(void)
{
    if (!uteApplicationCommonIsPowerOn())
    {
        return;
    }
    /*! 自动测试逻辑 zn.zeng, 2021-08-31  */
    if (uteModuleBreathrateData.isAutoTesting)
    {
        bool isNeedAutoTest = false;
        ute_module_systemtime_time_t time;
        uteModuleSystemtimeGetTime(&time);
        uint32_t oneDaySec = time.hour * 3600 + time.min * 60 + time.sec;
        if (uteModuleBreathrateData.isAutoTimeBucketTesting)
        {
            uint16_t currentTime = ((time.hour << 8 & 0xff00) | time.min);
            if (uteModuleBreathrateData.endTimeHourMin > uteModuleBreathrateData.startTimeHourMin)
            {
                if ((currentTime >= uteModuleBreathrateData.startTimeHourMin) && (currentTime <= uteModuleBreathrateData.endTimeHourMin))
                {
                    isNeedAutoTest = true;
                }
            }
            else
            {
                if ((currentTime >= uteModuleBreathrateData.startTimeHourMin) || (currentTime <= uteModuleBreathrateData.endTimeHourMin))
                {
                    isNeedAutoTest = true;
                }
            }
            if (((oneDaySec % (60 * uteModuleBreathrateData.intervalMin)) == 0) && isNeedAutoTest)
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
            if ((oneDaySec % (60 * uteModuleBreathrateData.intervalMin)) == 0)
            {
                isNeedAutoTest = true;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,isNeedAutoTest=%d", __func__, isNeedAutoTest);
        if (isNeedAutoTest)
        {
            /*!保存数据 zn.zeng, 2021-08-31  */
            uteModuleBreathrateData.breathrateValue = uteModuleBreathrateGetValue();
            uteModuleBreathrateAutoSaveData();
        }
    }
}

/**
 *@brief        开始单次测试
 *@details
 *@author       zn.zeng
 *@date       2021-08-16
 */
void uteModuleBreathrateStartSingleTesting(void)
{
    if (uteModuleBreathrateData.isSingleTesting)
    {
        return;
    }
    uteModuleBreathrateData.isSingleTesting = true;
    uteModuleBreathrateData.breathrateValue = 0;
    uint8_t startBRCmdRsp[2] = {CMD_BREATH_RATE_TEST, 0x11};
    uteModuleProfileBleSendToPhone(&startBRCmdRsp[0], 2);
    uteModuleHeartStartSingleTesting(TYPE_BREATHRATE);
}
/**
 *@brief        结束单次测试
 *@details
 *@author       zn.zeng
 *@date       2021-08-16
 */
void uteModuleBreathrateStopSingleTesting(void)
{
    if (!uteModuleBreathrateData.isSingleTesting)
    {
        return;
    }
    uint8_t stopCmd[5];
    stopCmd[0] = CMD_BREATH_RATE_TEST;
    stopCmd[1] = 0x00;
    bool isWear = uteModuleBreathrateIsWear();
    uteModuleBreathrateData.isSingleTesting = false;
    if (isWear)
    {
        stopCmd[2] = 0x00;
        stopCmd[3] = uteModuleBreathrateGetValue();
        // 保存数据
        uteModuleBreathrateAutoSaveData();
    }
    else
    {
        stopCmd[2] = 0xff;
        stopCmd[3] = 0xff;
    }
    uteModuleProfileBleSendToPhone(&stopCmd[0], 4);
    uteModuleHeartStopSingleTesting(TYPE_BREATHRATE);
}
/**
 *@brief        是否佩戴
 *@details
 *@author       zn.zeng
 *@date       2021-08-16
 */
bool uteModuleBreathrateIsWear(void)
{
    bool isWear = uteModuleHeartIsWear();
    return isWear;
}
/**
 *@brief        呼吸率数值
 *@details
 *@author       zn.zeng
 *@date       2021-08-16
 */
uint8_t uteModuleBreathrateGetValue(void)
{
#if UTE_DRV_BREATHRATE_VCXX_SUPPORT
    if (uteModuleBreathrateData.isSingleTesting)
    {
        uteModuleBreathrateData.breathrateValue = uteModuleHeartGetBreathrateValue();
    }
#endif
    return uteModuleBreathrateData.breathrateValue;
}

/**
 * @brief        设置呼吸率数值
 * @details
 * @param[in]    value 呼吸率值
 * @return       void*
 * @author       Wang.Luo
 * @date         2025-04-26
 */
void uteModuleBreathrateSetValue(uint8_t value)
{
    uteModuleBreathrateData.breathrateValue = value;
    UTE_MODULE_LOG(UTE_LOG_SYSTEM_LVL,"%s,value=%d",__func__,value);
}

/**
 *@brief        获取呼吸率测试状态
 *@details
 *@author       zn.zeng
 *@date       2021-08-31
 */
bool uteModuleBreathrateIsTesting(void)
{
    return uteModuleBreathrateData.isSingleTesting;
}
/**
 *@brief        保存呼吸率设置参数
 *@details
 *@param[in] bool isAutoTesting  是否打开自动测试
 *@param[in] uint16_t intervalMin  测试间隔，单位为分钟
 *@author       zn.zeng
 *@date       2021-08-31
 */
void uteModuleBreathrateSaveAutoIntervalParam(bool isAutoTesting, uint16_t intervalMin)
{
    uteModuleBreathrateData.isAutoTesting = isAutoTesting;
    uteModuleBreathrateData.intervalMin = intervalMin;
    /*! 保存到文件zn.zeng, 2021-08-30*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleBreathrateData.isAutoTesting;
    writebuff[1] = uteModuleBreathrateData.isAutoTimeBucketTesting;
    writebuff[2] = (uteModuleBreathrateData.intervalMin >> 8) & 0xff;
    writebuff[3] = (uteModuleBreathrateData.intervalMin) & 0xff;
    writebuff[4] = (uteModuleBreathrateData.startTimeHourMin >> 8) & 0xff;
    writebuff[5] = (uteModuleBreathrateData.startTimeHourMin) & 0xff;
    writebuff[6] = (uteModuleBreathrateData.endTimeHourMin >> 8) & 0xff;
    writebuff[7] = (uteModuleBreathrateData.endTimeHourMin) & 0xff;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BREATHRATE_SET_INFO, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file, &writebuff[0], 8);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,isAutoTesting=%d,intervalMin=%d,isAutoTimeBucketTesting=%d", __func__, uteModuleBreathrateData.isAutoTesting, uteModuleBreathrateData.intervalMin, uteModuleBreathrateData.isAutoTimeBucketTesting);
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__, uteModuleBreathrateData.startTimeHourMin, uteModuleBreathrateData.endTimeHourMin);
}
/**
 *@brief        保存呼吸率设置参数
 *@details
 *@param[in] bool isAutoTimeBucketTesting  是否打开时间段自动测试
 *@param[in] uint16_t startTimeHourMin 开始时间，高8bit为小时，低8bit为分钟
 *@param[in] uint16_t endTimeHourMin 结束时间，高8bit为小时，低8bit为分钟
 *@author       zn.zeng
 *@date       2021-08-31
 */
void uteModuleBreathrateSaveAutoTimeBucketParam(bool isAutoTimeBucketTesting, uint16_t startTimeHourMin, uint16_t endTimeHourMin)
{
    uteModuleBreathrateData.isAutoTimeBucketTesting = isAutoTimeBucketTesting;
    uteModuleBreathrateData.startTimeHourMin = startTimeHourMin;
    uteModuleBreathrateData.endTimeHourMin = endTimeHourMin;
    /*! 保存到文件zn.zeng, 2021-08-30*/
    void *file;
    uint8_t writebuff[8];
    writebuff[0] = uteModuleBreathrateData.isAutoTesting;
    writebuff[1] = uteModuleBreathrateData.isAutoTimeBucketTesting;
    writebuff[2] = (uteModuleBreathrateData.intervalMin >> 8) & 0xff;
    writebuff[3] = (uteModuleBreathrateData.intervalMin) & 0xff;
    writebuff[4] = (uteModuleBreathrateData.startTimeHourMin >> 8) & 0xff;
    writebuff[5] = (uteModuleBreathrateData.startTimeHourMin) & 0xff;
    writebuff[6] = (uteModuleBreathrateData.endTimeHourMin >> 8) & 0xff;
    writebuff[7] = (uteModuleBreathrateData.endTimeHourMin) & 0xff;
    if (uteModuleFilesystemOpenFile(UTE_MODULE_FILESYSTEM_SYSTEMPARM_BREATHRATE_SET_INFO, &file, FS_O_WRONLY | FS_O_CREAT | FS_O_TRUNC))
    {
        uteModuleFilesystemWriteData(file, &writebuff[0], 8);
        uteModuleFilesystemCloseFile(file);
    }
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,isAutoTesting=%d,intervalMin=%d,isAutoTimeBucketTesting=%d", __func__, uteModuleBreathrateData.isAutoTesting, uteModuleBreathrateData.intervalMin, uteModuleBreathrateData.isAutoTimeBucketTesting);
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,startTimeHourMin=0x%02x,endTimeHourMin=0x%02x", __func__, uteModuleBreathrateData.startTimeHourMin, uteModuleBreathrateData.endTimeHourMin);
}
/**
 *@brief        保存呼吸率数据数据，每10分钟一条数据
 *@details      每天保存一个文件，自动保存
 *@author       zn.zeng
 *@date       2021-08-31
 */
void uteModuleBreathrateAutoSaveData(void)
{
    uint16_t buffSize = 6 * 24 + 4;
    uint8_t *readBuff = uteModulePlatformMemoryAlloc(buffSize);
    char path[40];
    memset(&readBuff[0], 0xff, buffSize);
    memset(&path[0], 0, sizeof(path));
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    ute_module_systemtime_time_t time;
    uteModuleSystemtimeGetTime(&time);
    sprintf(&path[0], "%04d%02d%02d", time.year, time.month, time.day);
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, dirInfo, NULL);
    if ((dirInfo->filesCnt >= UTE_MODULE_BREATHRATE_SAVE_DATA_MAX_DAYS) && (memcmp(&path[0], &dirInfo->filesName[0][0], 8) != 0))
    {
        /*! 删除最旧一天的数据zn.zeng, 2021-08-25*/
        memset(&path[0], 0, sizeof(path));
        sprintf(&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, &dirInfo->filesName[0][0]);
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile(&path[0]);
    }
    memset(&path[0], 0, sizeof(path));
    sprintf(&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,dirInfo->filesCnt=%d", __func__, dirInfo->filesCnt);
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,save file=%s", __func__, &path[0]);
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
        cacheOffset = 4 + (time.hour * 60 + time.min) / 10;
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,cacheOffset=%d,.breathrateValue=%d", __func__, cacheOffset, uteModuleBreathrateData.breathrateValue);
        readBuff[cacheOffset] = uteModuleBreathrateData.breathrateValue;
        uteModuleFilesystemSeek(file, 0, FS_SEEK_SET);
        uteModuleFilesystemWriteData(file, &readBuff[0], buffSize);
        uteModuleFilesystemCloseFile(file);
    }
    uteModulePlatformMemoryFree(readBuff);
    uteModulePlatformMemoryFree(dirInfo);
}
/**
 *@brief        开始发送24小时呼吸率测试数据
 *@details
 *@author       zn.zeng
 *@date       2021-08-31
 */
void uteModuleBreathrateSendHistoryData(void)
{
    ute_application_sync_data_param_t *sendParam;
    uteApplicationCommonGetSyncDataParam(&sendParam);
    char path[40];
    uint8_t response[20];
    uint8_t sendSize = 0;
    response[0] = CMD_BREATH_RATE_TEST;
    response[1] = 0xfa;
    uint8_t *dataBuff = (uint8_t *)uteModulePlatformMemoryAlloc(sendParam->dataBuffSize);
    memset(&dataBuff[0], 0, sendParam->dataBuffSize);
    memset(&path[0], 0, 40);
    if (sendParam->dirInfo.filesCnt > 0)
    {
    SEND_NEXT_DATA:
        sendSize = 19;
        bool isHasData = false;
        void *file;
        sprintf(&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, &sendParam->dirInfo.filesName[sendParam->currSendFileIndex][0]);
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,read file=%s", __func__, &path[0]);
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,currSendFileIndex=%d,currSendMinIndex=%d", __func__, sendParam->currSendFileIndex, sendParam->currSendMinIndex);
        if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_RDONLY))
        {
            uteModuleFilesystemReadData(file, &dataBuff[0], sendParam->dataBuffSize);
            uteModuleFilesystemCloseFile(file);
        }
        uint16_t invalidMin = 0;
        for (uint16_t i = sendParam->currSendMinIndex; i < 144;)
        {
            if ((memcmp(&dataBuff[4 + i], "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00", 12) == 0) || (memcmp(&dataBuff[4 + i], "\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", 12) == 0))
            {
                invalidMin = invalidMin + 12;
                i = i + 12;
            }
            else
            {
                uint8_t sendHour = i + 2;
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
                memcpy(&response[7], &dataBuff[4 + i], 12);
                invalidMin = invalidMin + 12;
                i = i + 12;
                isHasData = true;
                break;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,invalidMin=%d", __func__, invalidMin);
        sendParam->currSendMinIndex += invalidMin;
        if (sendParam->currSendMinIndex >= 144)
        {
            sendParam->currSendFileIndex++;
            if (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt)
            {
                sendParam->currSendMinIndex = 0;
            }
            else
            {
                sendParam->currSendMinIndex = 144;
            }
        }
        if ((!isHasData) && (sendParam->currSendFileIndex < sendParam->dirInfo.filesCnt))
        {
            goto SEND_NEXT_DATA;
        }
    }
    else
    {
        sendParam->currSendMinIndex = 144;
    }
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,currSendFileIndex=%d,currSendHourIndex=%d,dirInfo.filesCnt=%d", __func__, sendParam->currSendFileIndex, sendParam->currSendHourIndex, sendParam->dirInfo.filesCnt);
    if ((sendParam->currSendFileIndex == sendParam->dirInfo.filesCnt) && (sendParam->currSendMinIndex == 144))
    {
        sendSize = 3;
        response[1] = 0xfa;
        response[2] = sendParam->crc;
        uteApplicationCommonSyncDataTimerStop();
    }
    if (sendSize == 19)
    {
        uteModuleCrc8Bit(&sendParam->crc, &response[1], 18);
    }
    uteModulePlatformMemoryFree(dataBuff);
    uteModuleProfileBleSendToPhone(&response[0], sendSize);
}
/**
 *@brief        准备开始发送呼吸率测试数据
 *@details
 *@param[in]  需要同步的数据时间戳
 *@author       zn.zeng
 *@date       2021-08-31
 */
void uteModuleBreathrateStartSendAutoTestHistoryData(ute_module_systemtime_time_t time)
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
    param->dataBuffSize = 6 * 24 + 4;
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, &param->dirInfo, NULL);
    uteApplicationCommonRegisterSyncDataTimerFunction(uteModuleBreathrateSendHistoryData);
    uteApplicationCommonSyncDataTimerStart();
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,", __func__);
}
/**
 *@brief        删除呼吸率测试数据
 *@details
 *@author       zn.zeng
 *@date       2021-08-31
 */
void uteModuleBreathrateDelHistoryData(void)
{
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s", __func__);
    ute_module_filesystem_dir_t *dirInfo = (ute_module_filesystem_dir_t *)uteModulePlatformMemoryAlloc(sizeof(ute_module_filesystem_dir_t));
    uteModuleFilesystemLs(UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, dirInfo, NULL);
    for (uint8_t i = 0; i < dirInfo->filesCnt; i++)
    {
        char path[40];
        sprintf(&path[0], "%s/%s", UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, &dirInfo->filesName[i][0]);
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,del file=%s", __func__, &path[0]);
        uteModuleFilesystemDelFile(&path[0]);
    }
    uteModulePlatformMemoryFree(dirInfo);
}
#if UTE_DISPLAY_BREATH_RATE_HISTORY_HISTOGRAM_SUPPORT
/**
 *@brief 设置呼吸率柱状图具体的坐标
 *@details
 *@author       黄鹤
 *@date       2022-11-21
 */
bool uteModuleBreathrateSetHistoryGraph(UT_GraphsParam *BreathRateHistoryGraph, uint8_t BreathRateHistoryGraphCount, uint8_t *BreathRateHistoryData, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange)
{
#if UTE_LOG_GUI_LVL // test
    for (uint8_t i = 0; i < 144; i++)
    {
        BreathRateHistoryData[i] = 5 + rand() % 30;
    }
#endif
    uint8_t averageBalueBuff[144];
    uint8_t m = 144 / BreathRateHistoryGraphCount;
    memset(&averageBalueBuff[0], 0, 144);
    for (uint8_t i = 0; i < BreathRateHistoryGraphCount; i++)
    {
        uint8_t readValue = 0;
        uint16_t sumValidValue = 0; // 总呼吸率值
        uint8_t validValueCnt = 0;
        for (int j = m; j > 0; j--)
        {
            readValue = BreathRateHistoryData[i * m + (j - 1)];
            UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,i=%d,j=%d,readValue=%d", __func__, i, j, readValue);
            if ((readValue != 255) && (readValue != 0))
            {
                sumValidValue += readValue;
                validValueCnt++;
            }
        }
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,sumValidValue=%d,validValueCnt=%d", __func__, sumValidValue, validValueCnt);
        if (validValueCnt > 0)
        {
            averageBalueBuff[i] = sumValidValue / validValueCnt;
        }
    }
    for (uint8_t i = 0; i < BreathRateHistoryGraphCount; i++)
    {
        if (averageBalueBuff[i] == 0)
        {
            continue;
        }
        if (averageBalueBuff[i] < 12)
        {
            BreathRateHistoryGraph[i].colorData = BREATH_RATE_HIGHT_TOO_FAST_HISTORGRAM_24COLOR;
        }
        else if ((averageBalueBuff[i] < 24) && (averageBalueBuff[i] >= 12))
        {
            BreathRateHistoryGraph[i].colorData = BREATH_RATE_HIGHT_NORMAL_HISTORGRAM_24COLOR;
        }
        else
        {
            BreathRateHistoryGraph[i].colorData = BREATH_RATE_HIGHT_TOO_SLOW_HISTORGRAM_24COLOR;
        }
        // 宽高通过图片的对应比例算出.比如下面高度一共hightRange个像素点，心率范围值取0~200
        BreathRateHistoryGraph[i].hight = averageBalueBuff[i] * hightRange / UTE_MODULE_BREATHRATE_MAX_VALUE;
        BreathRateHistoryGraph[i].width = drawWidth;
        BreathRateHistoryGraph[i].x = x + i * drawWidth + i * intervalWidth;
        BreathRateHistoryGraph[i].y = y - BreathRateHistoryGraph[i].hight;
        UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,BreathRateHistoryGraph[i].hight=%d", __func__, BreathRateHistoryGraph[i].hight);
    }
    return true;
}
/**
 *@brief      获取当天呼吸率历史数据，用于呼吸率柱状图显示
 *@details    当天有数据返回true,没有则返回false
 *@author       黄鹤
 *@date       2022-11-26
 */
bool uteModuleBreathrateGetTodayHistoryData(UT_GraphsParam *BreathRateHistoryGraph, uint8_t BreathRateHistoryGraphCount, int16_t x, int16_t y, uint8_t drawWidth, uint8_t intervalWidth, uint16_t hightRange)
{
    void *file;
    uint8_t path[25];
    uint16_t buffSize = 6 * 24 + 4;
    uint8_t readBuff[148];
    ute_module_systemtime_time_t time;
    memset(readBuff, 0, buffSize);
    memset(BreathRateHistoryGraph, 0, sizeof(UT_GraphsParam) * BreathRateHistoryGraphCount);
    uteModuleSystemtimeGetTime(&time);
    sprintf((char *)&path[0], "%s/%04d%02d%02d", UTE_MODULE_FILESYSTEM_BREATHRATE_AUTO_DATA_DIR, time.year, time.month, time.day);
    UTE_MODULE_LOG(UTE_LOG_BREATHRATE_LVL, "%s,read file=%s", __func__, &path[0]);
    if (uteModuleFilesystemOpenFile(&path[0], &file, FS_O_RDONLY))
    {
        uteModuleFilesystemReadData(file, &readBuff[0], buffSize);
        uteModuleFilesystemCloseFile(file);
        uteModuleBreathrateSetHistoryGraph(BreathRateHistoryGraph, BreathRateHistoryGraphCount, &readBuff[4], x, y, drawWidth, intervalWidth, hightRange);
        return true;
    }
    else
    {
#if UTE_LOG_GUI_LVL // test
        uteModuleBreathrateSetHistoryGraph(BreathRateHistoryGraph, BreathRateHistoryGraphCount, &readBuff[4], x, y, drawWidth, intervalWidth, hightRange);
#endif
        return false;
    }
}
#endif

#endif // UTE_MODULE_BREATHRATE_SUPPORT
